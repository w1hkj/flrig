// ----------------------------------------------------------------------------
// Copyright (C) 2014
//              David Freese, W1HKJ
//
// This file is part of flrig.
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

#ifndef _TRACE_H
#define _TRACE_H

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>

extern Fl_Double_Window*	tracewindow;

extern void trace(int n, ...); // all args of type const char *
extern void xml_trace(int n, ...); // all args of type const char *
extern void rig_trace(int n, ...); // trace transceiver class methods
extern void get_trace(int n, ...); // trace get methods
extern void set_trace(int n, ...); // trace set methods
extern void ser_trace(int n, ...); // trace serial methods
extern void rpc_trace(int n, ...); // trace transceiver class methods

extern void make_trace_window();

#define getr(s)  get_trace(1, s);
#define setr(s)  set_trace(1, s);

#define gett(str) get_trace(3, str, cmd.c_str(), replystr.c_str())
#define sett(str) set_trace(3, str, cmd.c_str(), replystr.c_str())

#define getthex(str) { \
	std::string hex1 = str2hex(cmd.c_str(), cmd.length()); \
	std::string hex2 = str2hex(replystr.c_str(), replystr.length()); \
	get_trace(3, str, hex1.c_str(), hex2.c_str()); \
}

#define setthex(str) { \
	std::string hex1 = str2hex(cmd.c_str(), cmd.length()); \
	std::string hex2 = str2hex(replystr.c_str(), replystr.length()); \
	set_trace(3, str, hex1.c_str(), hex2.c_str()); \
}

#define seth() { \
	std::string hex1 = str2hex(cmd.c_str(), cmd.length()); \
	std::string hex2 = str2hex(replystr.c_str(), replystr.length()); \
	set_trace(3, hex1.c_str(), " : ", hex2.c_str()); \
}

#define geth() { \
	std::string hex1 = str2hex(cmd.c_str(), cmd.length()); \
	std::string hex2 = str2hex(replystr.c_str(), replystr.length()); \
	set_trace(3, hex1.c_str(), " : ", hex2.c_str()); \
}

#endif

