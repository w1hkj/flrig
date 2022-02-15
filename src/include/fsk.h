// ----------------------------------------------------------------------------
// fsk.cxx  --  FSK signal generator
//
// Copyright (C) 2021
//		Dave Freese, W1HKJ
//
// This file is part of flrig.
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
// ----------------------------------------------------------------------------

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include <iostream>
#include <string>

#include "serial.h"
#include "threads.h"

#ifdef __WIN32__
#	include "compat.h"
#	include <winbase.h>
#	include <windows.h>
#endif

#ifndef FSK_H
#define FSK_H

enum {FSK_USES_FSK, FSK_USES_CAT, FSK_USES_AUX, FSK_USES_SEP, FSK_USES_CWIO};

class FSK
{
// time (msec) for one symbollen @ 45.45 baud
//#define BITLEN 0.022

#define FSK_UNKNOWN	0x000
#define	FSK_LETTERS	0x100
#define	FSK_FIGURES	0x200

#define LTRS 0x1F
#define FIGS 0x1B

#define FSK_MARK  0
#define FSK_SPACE 1

public:
	FSK();
	~FSK();

	bool open();
	bool close();
	void abort();

	bool sending();

	void shift_on_space(bool b) { _shift_on_space = b; }
	bool shift_on_space() { return _shift_on_space; }

	void dtr(bool b) { _dtr = b; }
	bool dtr() { return _dtr; }

	void rts(bool b) { _dtr = !b; }
	bool rts() { return !_dtr; }

	void reverse(bool b) { _reverse = b; }
	bool reverse() { return _reverse; }

	void   open_port(std::string device_name);

	void device(std::string device_name) {
		serial_device = device_name;
	}

	void fsk_shares_port(Cserial *shared_device);

//	size_t io_timer_id;

private:

	std::string serial_device;

	bool   shared_port; // default is false
	static char letters[];
	static char figures[];
	static const char *ascii[];

	int  shift;
	bool _shift_on_space;
	bool _dtr;
	bool _reverse;
	int  idles;

	int mode;
	int shift_state;
	int start_bits;
	int stop_bits;
	int chr_bits;
	int   chr_out;

	double BITLEN;
	double now();
	int    sleep (double sleep_time);

	void send_baudot(int);
	int baudot_enc(int);
	void fsk_out (bool);

public:

	std::string str_buff;

	int callback_method();

	int init_fsk_thread();
	void exit_fsk_thread();

	bool	fsk_loop_terminate;

	pthread_t fsk_thread;

friend
	void *fsk_loop(void *data);

};

//----------------------------------------------------------------------

#include <FL/Fl_Window.H>
#include <FL/Fl_Double_Window.H>

enum { FSK_NONE, FSK_START, FSK_ADD, FSK_SEND, FSK_END, FSK_TERMINATE };

extern Fl_Double_Window *FSK_keyer_dialog;
extern Fl_Double_Window *FSK_editor;
extern Fl_Double_Window *FSK_configure;

extern pthread_mutex_t FSK_text_mutex;
extern std::string FSK_text;
extern int FSK_process;

extern Cserial *FSK_serial;

extern void FSK_send(int c);
extern int  FSK_open_port();
extern void FSK_close_port();
extern void FSK_reset_port();

extern int  FSK_start_thread();
extern void FSK_stop_thread();
extern void FSK_add(std::string);
extern void FSK_send_text(bool);
extern void FSK_clear_text();

extern void FSK_msg_cb(int n);
extern void FSK_label_cb(int n);
extern void FSK_exec_msg(int n);

extern void FSK_cancel_edit();
extern void FSK_apply_edit();
extern void FSK_done_edit();

extern void FSK_control_function_keys();

extern void FSK_open_config();

#endif
