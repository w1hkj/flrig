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

#ifndef _CWIO_H
#define _CWIO_H

#include <string>
#include <FL/Fl_Window.H>

#include "morse.h"
#include "serial.h"

extern Fl_Double_Window *cwio_keyer_dialog;
extern Fl_Double_Window *cwio_editor;
extern Fl_Double_Window *cwio_configure;

extern Cserial *cwio_serial;
extern Cmorse  *morse;

extern void send_cwkey(int c);
extern int  open_cwkey();
extern void close_cwkey();

extern int  start_cwio_thread();
extern void stop_cwio_thread();
extern void add_cwio(std::string);
extern void send_text(bool);
extern void cwio_clear_text();

extern void msg_cb(int n);
extern void label_cb(int n);
extern void exec_msg(int n);

extern void cancel_edit();
extern void apply_edit();
extern void done_edit();

extern void control_function_keys();

extern void calibrate_cwio();
extern void open_cwio_config();

#endif
