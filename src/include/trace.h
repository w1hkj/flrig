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
extern void rpc_trace(int n, ...); // trace transceiver class methods

#define gett(str) get_trace(3, str, cmd.c_str(), replystr.c_str())
#define sett(str) set_trace(3, str, cmd.c_str(), replystr.c_str())

#define getthex(str) get_trace(3, \
	str, \
	(str2hex(cmd.c_str(), cmd.length())), \
	(str2hex(replystr.c_str(), replystr.length())) )

#define setthex(str) set_trace(3, \
	str, \
	(str2hex(cmd.c_str(), cmd.length())), \
	(str2hex(replystr.c_str(), replystr.length())) )

#endif
