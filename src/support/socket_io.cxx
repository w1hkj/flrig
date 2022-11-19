// socket_io.cxx
//
// Author: Dave Freese, W1HKJ
//         Stelios Bounanos, M0GLD
//
// ----------------------------------------------------------------------------
// Copyright (C) 2014
//              David Freese, W1HKJ
//
// This file is part of flrig
//
// flrig is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// flrig is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include "config.h"
#include "compat.h" // Must precede all FL includes

#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <math.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>

#include <FL/Fl.H>
#include <FL/Enumerations.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Sys_Menu_Bar.H>
#include <FL/x.H>
#include <FL/Fl_Help_Dialog.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_File_Icon.H>
#include <FL/x.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Image.H>

#include "status.h"
#include "debug.h"
#include "util.h"
#include "gettext.h"
#include "rigpanel.h"

#include "socket_io.h"
#include "socket.h"
#include "support.h"

#ifdef WIN32
#  include <winsock2.h>
#else
#  include <arpa/inet.h>
#endif

Socket *tcpip = (Socket *)0;
Address *remote_addr = (Address *)0;

static bool exit_socket_loop = false;

static std::string rxbuffer;
pthread_t *rcv_socket_thread = 0;
pthread_mutex_t mutex_rcv_socket = PTHREAD_MUTEX_INITIALIZER;

void *rcv_socket_loop(void *)
{
	for (;;) {
		MilliSleep(500);//5);
		{
			guard_lock socket_lock(&mutex_rcv_socket);
			if (exit_socket_loop) break;
			if (tcpip && tcpip->fd() != -1) { 
				try {
					tcpip->recv(rxbuffer);
					box_tcpip_connect->color(FL_GREEN);
					box_tcpip_connect->redraw();
					box_xcvr_connect->color(FL_GREEN);
					box_xcvr_connect->redraw();
					tcpip_menu_box->color(FL_GREEN);
					tcpip_menu_box->redraw();
				} catch (const SocketException& e) {
					LOG_ERROR("Error %d, %s", e.error(), e.what());
					box_tcpip_connect->color(FL_YELLOW);
					box_tcpip_connect->redraw();
					box_xcvr_connect->color(FL_YELLOW);
					box_xcvr_connect->redraw();
					tcpip_menu_box->color(FL_YELLOW);
					tcpip_menu_box->redraw();
				}
			}
		} // end guard_lock
	}
	exit_socket_loop = false;
	return NULL;
}

void connect_to_remote()
{
	try {
		if (remote_addr) delete remote_addr;

		remote_addr = new Address(progStatus.tcpip_addr.c_str(), progStatus.tcpip_port.c_str());
		LOG_QUIET("Created new remote_addr @ %p", remote_addr);

		if (!tcpip) {
			guard_lock socket_lock(&mutex_rcv_socket);
			tcpip = new Socket(*remote_addr);
			LOG_QUIET("Created new socket @ %p", tcpip);

			tcpip->set_timeout(0.001);
			tcpip->connect();
			tcpip->set_nonblocking(true);
			LOG_QUIET("Connected to %d", tcpip->fd());
			tcpip_box->show();
			box_tcpip_connect->color(FL_GREEN);
			box_tcpip_connect->redraw();
			box_xcvr_connect->color(FL_GREEN);
			box_xcvr_connect->redraw();
			tcpip_menu_box->color(FL_GREEN);
			tcpip_menu_box->redraw();
		}
		if (tcpip->fd() == -1) {

			try {
				tcpip->connect(*remote_addr);
				tcpip->set_nonblocking(true);
				LOG_QUIET("Connected to %d", tcpip->fd());

				tcpip_box->show();
				box_tcpip_connect->color(FL_GREEN);
				box_tcpip_connect->redraw();
				box_xcvr_connect->color(FL_GREEN);
				box_xcvr_connect->redraw();
				tcpip_menu_box->color(FL_GREEN);
				tcpip_menu_box->redraw();
			} catch (const SocketException & e) {
				LOG_ERROR("Error: %d, %s", e.error(), e.what());

				delete remote_addr;
				remote_addr = 0;
				delete tcpip;
				tcpip = 0;
				box_tcpip_connect->color(FL_LIGHT1);
				box_tcpip_connect->redraw();
				box_xcvr_connect->color(FL_LIGHT1);
				box_xcvr_connect->redraw();
				tcpip_menu_box->color(FL_LIGHT1);
				tcpip_menu_box->redraw();
				throw e;
			}
		}
		if (!rcv_socket_thread) {
			rcv_socket_thread = new pthread_t;
			if (pthread_create(rcv_socket_thread, NULL, rcv_socket_loop, NULL)) {
				perror("pthread_create");
				exit(EXIT_FAILURE);
			}
			LOG_QUIET("%s", "Socket receive thread started");
		}
	}
	catch (const SocketException& e) {
		LOG_ERROR("Error: %d, %s", e.error(), e.what());

		delete remote_addr;
		remote_addr = 0;
		LOG_ERROR("Deleted remote address");

		delete tcpip;
		tcpip = 0;
		LOG_ERROR("Deleted tcpip socket");

		throw e;
	}
	return;
}

void disconnect_from_remote()
{
	if (!tcpip || tcpip->fd() == -1) return;

	tcpip->close();
	delete tcpip;
	tcpip = 0;
	LOG_QUIET("%s", "Deleted tcpip socket instance");
	delete remote_addr;
	remote_addr = 0;
	LOG_QUIET("%s", "Deleted socket address instance");
	exit_socket_loop = true;

	pthread_join(*rcv_socket_thread, NULL);
	rcv_socket_thread = NULL;
	LOG_QUIET("%s", "Exited from socket read thread");

	box_tcpip_connect->color(FL_LIGHT1);
	box_tcpip_connect->redraw();
	box_xcvr_connect->color(FL_LIGHT1);
	box_xcvr_connect->redraw();
	tcpip_menu_box->color(FL_LIGHT1);
	tcpip_menu_box->redraw();
}

int retry_after = 0;
int drop_count = 0;

void send_to_remote(std::string cmd_string)
{
	if (retry_after > 0) {
		retry_after -= progStatus.serloop_timing;
		if (retry_after < 0) retry_after = 0;
		return;
	}

	if (!tcpip || tcpip->fd() == -1) {
		try {
			connect_to_remote();
		} catch (...) {
			LOG_QUIET("Retry connect in %d seconds", progStatus.tcpip_reconnect_after);
			retry_after = 1000 * progStatus.tcpip_reconnect_after;
			return;
		}
	}

	try {
		tcpip->send(cmd_string);

		LOG_WARN("send to remote: %s", cmd_string.c_str());

		drop_count = 0;
	} catch (const SocketException& e) {
		LOG_ERROR("Error: %d, %s", e.error(), e.what());
		drop_count++;
		if (drop_count == progStatus.tcpip_drops_allowed) {
			disconnect_from_remote();
			drop_count = 0;
		}
	}
	return;
}

int read_from_remote(std::string &str)
{
	if (!tcpip || tcpip->fd() == -1) return 0;

	{	guard_lock socket_lock(&mutex_rcv_socket);
		str = rxbuffer;
		rxbuffer.clear();
	}
	char szc[200];
	snprintf(szc, sizeof(szc), "read_from_remote() : %s", str.c_str());

	LOG_WARN("%s", szc);

	return (int)str.length();
}
