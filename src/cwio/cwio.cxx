// ---------------------------------------------------------------------
// cwio.cxx  --  morse code modem
//
// Copyright (C) 2020
//		Dave Freese, W1HKJ
//
// This file is part of flrig
//
// flrig is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// flrig is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with fldigi.  If not, see <http://www.gnu.org/licenses/>.
// ---------------------------------------------------------------------


#include <config.h>

#include <cstring>
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstdlib>

#include <time.h>
#include <sys/time.h>

#include <FL/Fl.H>

#include "status.h"
#include "util.h"
#include "threads.h"
#include "debug.h"
#include "tod_clock.h"
#include "cwio.h"
#include "cwioUI.h"

using namespace std;

Cserial *cwio_serial = 0;
Cmorse  *morse = 0;

static pthread_t       cwio_pthread;
static pthread_cond_t  cwio_cond;
static pthread_mutex_t cwio_mutex = PTHREAD_MUTEX_INITIALIZER;
//static pthread_mutex_t cwio_text_mutex = PTHREAD_MUTEX_INITIALIZER;

enum { NONE, START, ADD, SEND, END, TERMINATE, CALIBRATE };
int cwio_process = NONE;
bool cwio_thread_running = false;

void send_cwkey(char c)
{
	Cserial *port = cwio_serial;
	if (progStatus.cwioSHARED == 1) {
		port = RigSerial; // sharing CAT port
		bypass_serial_thread_loop = true;
	}
	if (progStatus.cwioSHARED == 2) port = AuxSerial; // sharing AUX port
	if (progStatus.cwioSHARED == 3) port = SepSerial; // sharing SEP port
	if (!port) return;
	if (!port->IsOpen()) return;

	double tc = 1200.0 / progStatus.cwioWPM;
	double tch = 3 * tc;
	double twd = 4 * tc;

	if ((progStatus.cwio_comp > 0) && (progStatus.cwio_comp < tc)) {
		tc = round (tc - progStatus.cwio_comp);
		tch = round (tch - 3 * progStatus.cwio_comp);
		twd = round (twd - 4 * progStatus.cwio_comp);
	}

	if (c == ' ' || c == 0x0a) {
		MilliSleep(twd);
		return;
	}

	string code = morse->tx_lookup(c);
	for (size_t n = 0; n < code.length(); n++) {
		if (cwio_process == END) return;
		if (progStatus.cwioKEYLINE == 2) {
			port->setDTR(1);
		} else if (progStatus.cwioKEYLINE == 1) {
			port->setRTS(1);
		}
		if (code[n] == '.')
			MilliSleep(tc);
		else
			MilliSleep(tch);

		if (progStatus.cwioKEYLINE == 2) {
			port->setDTR(0);
		} else if (progStatus.cwioKEYLINE == 1) {
			port->setRTS(0);
		}
		if (n == code.length() -1)
			MilliSleep(tch);
		else
			MilliSleep(tc);
	}

	if (progStatus.cwioSHARED == 1) {
		bypass_serial_thread_loop = false;
	}
	return;
}

int open_cwkey()
{
	if (progStatus.cwioSHARED)
		return 1;
	if (!cwio_serial)
		cwio_serial = new Cserial;

	cwio_serial->Device(progStatus.cwioPORT);

	if (cwio_serial->OpenPort() == false) {
		LOG_ERROR("Cannot open serial port %s", cwio_serial->Device().c_str());
		cwio_serial = 0;
		return 0;
	}
	LOG_INFO("Opened %s for CW keyline control", cwio_serial->Device().c_str());

	cwio_serial->RTS(false);
	cwio_serial->DTR(false);

	return 1;
}

void close_cwkey()
{
	if (cwio_serial) {
		cwio_serial->ClosePort();
	}
}

static string snd;
void update_txt_to_send(void *)
{
	txt_to_send->value(snd.c_str());
	txt_to_send->redraw();
}

void sending_text()
{
	char c;
	if (progStatus.cwioPTT) {
		setPTT((void *)1);
		MilliSleep(progStatus.cwioPTT);
	}
	while (cwio_process == SEND) {
		snd = txt_to_send->value();
		if (snd.empty()) MilliSleep(10);
		else {
			c = snd[0];
			snd.erase(0,1);
			Fl::awake(update_txt_to_send);
			send_cwkey(c);
		}
	}
	if (progStatus.cwioPTT) {
		setPTT((void *)0);
		MilliSleep(progStatus.cwioPTT);
	}
}

void update_comp_value(void *)
{
	cnt_cwio_comp->value(progStatus.cwio_comp);
	cnt_cwio_comp->redraw();
	btn_cw_dtr_calibrate->value(0);
	btn_cw_dtr_calibrate->redraw();
}

void do_calibration()
{
	std::string paris = "PARIS ";
	std::string teststr;
	unsigned long start_time = 0;
	unsigned long end_time = 0;

	progStatus.cwio_comp = 0;

	for (int i = 0; i < progStatus.cwioWPM; i++)
		teststr.append(paris);

	txt_to_send->value();

	start_time = zmsec();
	for (size_t n = 0; n < teststr.length(); n++) {
		send_cwkey(teststr[n]);
	}

	end_time = zmsec();

	progStatus.cwio_comp = 50 * (1.0 - 60000.0 / (end_time - start_time));

//std::cout << "start_time: " << start_time << ", end_time: " << end_time << std::endl;
//std::cout << "cwio_comp: " << progStatus.cwio_comp << std::endl;

	Fl::awake(update_comp_value);
}

//----------------------------------------------------------------------
// cwio thread
//----------------------------------------------------------------------

void *cwio_loop(void *)
{
	cwio_thread_running = true;
	cwio_process = NONE;
	while (1) {
		pthread_mutex_lock(&cwio_mutex);
		pthread_cond_wait(&cwio_cond, &cwio_mutex);
		pthread_mutex_unlock(&cwio_mutex);

		if (cwio_process == TERMINATE)
			return NULL;

		if (cwio_process == SEND)
			sending_text();

		if (cwio_process == CALIBRATE)
			do_calibration();
	}
	return NULL;
}

int start_cwio_thread()
{
	if(cwio_thread_running) return 0;

	if (!morse)
		morse = new Cmorse;

	memset((void *) &cwio_pthread, 0, sizeof(cwio_pthread));
	memset((void *) &cwio_mutex,   0, sizeof(cwio_mutex));
	memset((void *) &cwio_cond,    0, sizeof(cwio_cond));

	if(pthread_cond_init(&cwio_cond, NULL)) {
		LOG_ERROR("cwio thread create fail (pthread_cond_init)");
		return 1;
	}

	if(pthread_mutex_init(&cwio_mutex, NULL)) {
		LOG_ERROR("cwio thread create fail (pthread_mutex_init)");
		return 1;
	}

	if (pthread_create(&cwio_pthread, NULL, cwio_loop, NULL) < 0) {
		pthread_mutex_destroy(&cwio_mutex);
		LOG_ERROR("cwio thread create fail (pthread_create)");
		return 1;
	}

	LOG_INFO("started cwio thread");

	MilliSleep(10); // Give the CPU time to set 'cwio_thread_running'
	return 0;
}

void stop_cwio_thread()
{
	if(!cwio_thread_running) return;
//std::cout << "stopping cwio thread" << std::endl;

	cwio_process = END;
	btn_cwioSEND->value(0);

	MilliSleep(4 * 1200 / progStatus.cwioWPM);

	cwio_process = TERMINATE;
	pthread_cond_signal(&cwio_cond);
	MilliSleep(50);

//std::cout << "joining cwio_pthread" << std::endl;

	pthread_join(cwio_pthread, NULL);
//std::cout << "cwio thread - stopped" << std::endl;

	LOG_INFO("%s", "cwio thread - stopped");

	pthread_mutex_destroy(&cwio_mutex);
	pthread_cond_destroy(&cwio_cond);

	memset((void *) &cwio_pthread, 0, sizeof(cwio_pthread));
	memset((void *) &cwio_mutex,   0, sizeof(cwio_mutex));

	cwio_thread_running = false;
	cwio_process = NONE;

	close_cwkey();

	if (cwio_serial) {
		delete cwio_serial;
		cwio_serial = 0;
	}
	if (morse) {
		delete morse;
		morse = 0;
	}

}

static string new_text;

void add_cwio(string txt)
{
	if (!cwio_thread_running) return;

	new_text = txt_to_send->value();
	new_text.append(txt);
	txt_to_send->value(new_text.c_str());
	txt_to_send->redraw();
}

void send_text(bool state)
{
	if (!cwio_thread_running) return;

	if (state && cwio_process != SEND) {
		cwio_process = SEND;
		pthread_cond_signal(&cwio_cond);
	} else {
		cwio_process = NONE;
	}
}

void cwio_clear_text()
{
	txt_to_send->value("");
}

void msg_cb(int n)
{
}

void label_cb(int n)
{
}

void exec_msg(int n)
{
	if ((Fl::event_state() & FL_CTRL) == FL_CTRL) {
		for (int n = 0; n < 12; n++) {
			edit_label[n]->value(progStatus.cwio_labels[n].c_str());
			edit_msg[n]->value(progStatus.cwio_msgs[n].c_str());
		}
		cwio_editor->show();
		return;
	}
	add_cwio(progStatus.cwio_msgs[n]);
}

void cancel_edit()
{
	cwio_editor->hide();
}

void apply_edit()
{
	for (int n = 0; n < 12; n++) {
		progStatus.cwio_labels[n] = edit_label[n]->value();
		progStatus.cwio_msgs[n] = edit_msg[n]->value();
		btn_msg[n]->label(progStatus.cwio_labels[n].c_str());
		btn_msg[n]->redraw_label();
	}
}

void done_edit()
{
	cwio_editor->hide();
}

// Alt-P pause transmit
// Alt-S start sending text
// F1 - F12 same as function-button mouse press

void control_function_keys()
{
	int key = Fl::event_key();
	int state = Fl::event_state();

	if (state & FL_ALT) {
		if (key == 'p') {
			btn_cwioSEND->value(0);
			btn_cwioSEND->redraw();
			send_text(false);
			return;
		}
		if (key == 's') {
			btn_cwioSEND->value(1);
			btn_cwioSEND->redraw();
			send_text(true);
			return;
		}
		if (key == 'c') {
			txt_to_send->value("");
			return;
		}
	}
	if ((key >= FL_F) && (key <= FL_F_Last)) {
		exec_msg( key - FL_F - 1);
	}
}

void calibrate_cwio()
{
	txt_to_send->value("");
	btn_cwioSEND->value(0);

	cwio_process = END;
	MilliSleep(50);

	cwio_process = CALIBRATE;
	pthread_cond_signal(&cwio_cond);
}


void open_cwio_config()
{
	cwio_configure->show();
}
