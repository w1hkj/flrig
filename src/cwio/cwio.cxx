
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

#include <errno.h>
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
#include "support.h"
#include "status.h"

//#define CWIO_DEBUG

Cserial *cwio_serial = 0;
Cmorse  *morse = 0;

static pthread_t       cwio_pthread;
static pthread_cond_t  cwio_cond;
static pthread_mutex_t cwio_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cwio_text_mutex = PTHREAD_MUTEX_INITIALIZER;

int cwio_process = NONE;
bool cwio_thread_running = false;

static std::string new_text;
static std::string cwio_text;


// return current tick time in seconds
double cw_now()
{
	static struct timeval t1;
	gettimeofday(&t1, NULL);
	return t1.tv_sec + t1.tv_usec / 1e6;
}

// sub millisecond accurate sleep function
// sleep_time in seconds
#ifdef CWIO_DEBUG
FILE *fcwio = (FILE *)0;
#endif

int cw_sleep (double sleep_time)
{
	struct timespec tv;
	double start_at = cw_now();
	double end_at = start_at + sleep_time;
	double delay = sleep_time - 0.005;
	tv.tv_sec = (time_t) delay;
	tv.tv_nsec = (long) ((delay - tv.tv_sec) * 1e+9);
	int rval = 0;
#ifdef __WIN32__
	timeBeginPeriod(1);
	end_at -= 0.0005;
#endif
	while (1) {
		rval = nanosleep (&tv, &tv);
		if (rval == 0)
			break;
		else if (errno == EINTR)
			continue;
		else 
			return rval;
	}
	while (cw_now() < end_at);
#ifdef __WIN32__
	timeEndPeriod(1);
#endif

#ifdef CWIO_DEBUG
	if (fcwio)
		fprintf(fcwio, "%f\n", cw_now() - start_at - sleep_time);
#endif
	return 0;
}

void cwio_key(bool state)
{
	Cserial *port = cwio_serial;

	switch (progStatus.cwioSHARED) {
		case 1: port = RigSerial; break;
		case 2: port = AuxSerial; break;
		case 3: port = SepSerial; break;
		default: port = cwio_serial;
	}

	if (!port)
		return;

	if (!port->IsOpen())
		return;

	if (progStatus.cwioPTT)
		doPTT(state);

	if (progStatus.cwioKEYLINE == 2) {
			port->setDTR(progStatus.cwioINVERTED ? !state : state);
	} else if (progStatus.cwioKEYLINE == 1) {
		port->setRTS(progStatus.cwioINVERTED ? !state : state);
	}

	return;
}

void send_cwkey(char c)
{
#ifdef CWIO_DEBUG
	if (!fcwio) fcwio = fopen("cwio_timing.txt", "a");
#endif

	static std::string code = "";
	static double tc = 0;
	static double tch = 0;
	static double twd = 0;
	static double xcvr_corr = 0;
	Cserial *port = cwio_serial;

	switch (progStatus.cwioSHARED) {
		case 1: port = RigSerial; break;
		case 2: port = AuxSerial; break;
		case 3: port = SepSerial; break;
		default: port = cwio_serial;
	}

	if (!port) 
		goto exit_send_cwkey;

	if (!port->IsOpen()) 
		goto exit_send_cwkey;

	tc = 1.2 / progStatus.cwioWPM;
	tch = 3 * tc;
	twd = 4 * tc;

	if ((progStatus.cwio_comp > 0) && (progStatus.cwio_comp < tc)) {
		tc = tc - progStatus.cwio_comp;
		tch = tch - 3 * progStatus.cwio_comp;
		twd = twd - 4 * progStatus.cwio_comp;
	}

	xcvr_corr = progStatus.cwio_keycorr * 1e-3;
	if (xcvr_corr < -tc / 2) xcvr_corr = - tc / 2;
	else if (xcvr_corr > tc / 2) xcvr_corr = tc / 2;

	if (c == ' ' || c == 0x0a) {
		cw_sleep(twd);
		goto exit_send_cwkey;
	}

	code = morse->tx_lookup(c);
	for (size_t n = 0; n < code.length(); n++) {
		if (cwio_process == END) {
			goto exit_send_cwkey;
		}
		if (progStatus.cwioKEYLINE == 2) {
			port->setDTR(progStatus.cwioINVERTED ? 0 : 1);
		} else if (progStatus.cwioKEYLINE == 1) {
			port->setRTS(progStatus.cwioINVERTED ? 0 : 1);
		}
		if (code[n] == '.')
			cw_sleep(tc + xcvr_corr);
		else
			cw_sleep(tch);

		if (progStatus.cwioKEYLINE == 2) {
			port->setDTR(progStatus.cwioINVERTED ? 1 : 0);
		} else if (progStatus.cwioKEYLINE == 1) {
			port->setRTS(progStatus.cwioINVERTED ? 1 : 0);
		}
		if (n == code.length() -1)
			cw_sleep(tch);
		else
			cw_sleep(tc - xcvr_corr);
	}

exit_send_cwkey:
	return;
}

void reset_cwioport()
{
	Cserial *port = cwio_serial;
	switch (progStatus.cwioSHARED) {
		case 1: port = RigSerial; break;
		case 2: port = AuxSerial; break;
		case 3: port = SepSerial; break;
		default: port = cwio_serial;
	}
	if (progStatus.cwioKEYLINE == 2) {
		port->setDTR(progStatus.cwioINVERTED ? 1 : 0);
	} else if (progStatus.cwioKEYLINE == 1) {
		port->setRTS(progStatus.cwioINVERTED ? 1 : 0);
	}
}

int open_cwkey()
{
	if (progStatus.cwioSHARED) {
		reset_cwioport();
		return 1;
	}
	if (!cwio_serial)
		cwio_serial = new Cserial;

	cwio_serial->Device(progStatus.cwioPORT);

	if (cwio_serial->OpenPort() == false) {
		LOG_ERROR("Cannot open serial port %s", cwio_serial->Device().c_str());
		cwio_serial = 0;
		return 0;
	}
	LOG_INFO("Opened %s for CW keyline control", cwio_serial->Device().c_str());

	reset_cwioport();

	return 1;
}

void close_cwkey()
{
	if (cwio_serial) {
		cwio_serial->ClosePort();
	}
}

static std::string snd;

void update_txt_to_send(void *)
{
	txt_to_send->value(snd.c_str());
	txt_to_send->redraw();
}

void terminate_sending(void *)
{
	btn_cwioSEND->value(0);
}

void sending_text()
{
	char c = 0;
	if (progStatus.cwioPTT) {
		doPTT(1);
		cw_sleep(progStatus.cwioPTT / 1000.0);
	}
	while (cwio_process == SEND) {
		c = 0;
		{	guard_lock lck(&cwio_text_mutex);
			if (!cwio_text.empty()) {
				c = cwio_text[0];
				cwio_text.erase(0,1);
			}
		}
		if (!c) {
			snd = txt_to_send->value();
			if (!snd.empty()) {
				c = snd[0];
				snd.erase(0,1);
				Fl::awake(update_txt_to_send);
			}
		}
		if (c == ']') {
			cwio_process = END;
			cw_sleep(0.005);
			Fl::awake(terminate_sending);
			return;
		}
		if (c) send_cwkey(c);
		else cw_sleep(0.005);
	}
	if (progStatus.cwioPTT) {
		doPTT(0);
		cw_sleep(progStatus.cwioPTT / 1000.0);
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

	cw_sleep(0.010); // Give the CPU time to set 'cwio_thread_running'
	return 0;
}

void stop_cwio_thread()
{
	if(!cwio_thread_running) return;

	cwio_process = END;
	btn_cwioSEND->value(0);

	cw_sleep( 4.8 / progStatus.cwioWPM);

	cwio_process = TERMINATE;
	pthread_cond_signal(&cwio_cond);
	cw_sleep(0.050);

	pthread_join(cwio_pthread, NULL);

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

void add_cwio(std::string txt)
{
	if (!cwio_thread_running) return;

	new_text = txt_to_send->value();
	new_text.append(txt);
	txt_to_send->value(new_text.c_str());
	txt_to_send->redraw();
	if (txt[0] == '[') {
		send_text(true);
		btn_cwioSEND->value(1);
	}
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

void cwio_new_text(std::string txt)
{
	cwio_text.append(txt);
	send_text(true);
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
	cw_sleep(0.050);

	cwio_process = CALIBRATE;
	pthread_cond_signal(&cwio_cond);
}


void open_cwio_config()
{
	if (progStatus.cwioSHARED == 1) {
		btn_cwioCAT->value(1); btn_cwioCAT->activate();
		btn_cwioAUX->value(0); btn_cwioAUX->deactivate();
		btn_cwioSEP->value(0); btn_cwioSEP->deactivate();
		select_cwioPORT->value("NONE"); select_cwioPORT->deactivate();
		btn_cwioCONNECT->value(0); btn_cwioCONNECT->deactivate();
	} else {
		btn_cwioCAT->activate();
		btn_cwioAUX->activate();
		btn_cwioSEP->activate();
		select_cwioPORT->activate();
		btn_cwioCONNECT->activate();
	}
	cwio_configure->show();
}
