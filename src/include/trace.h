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

#include <cstdio>
#include <string.h>

#define WITH_TRACED
//usage
//int TRACED(add, int a, int b)
// opening brace is part of the macro
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

#define gett(str) get_trace(5, str, "S: ", cmd.c_str(), " R: ", replystr.c_str())
#define sett(str) get_trace(5, str, "S: ", cmd.c_str(), " R: ", replystr.c_str())

#define getthex(str) { \
	static std::string hex1; \
	hex1 = str2hex(cmd.c_str(), cmd.length()); \
	static std::string hex2; \
	hex2 = str2hex(replystr.c_str(), replystr.length()); \
	get_trace(5, str, "S: ", hex1.c_str(), " R: ", hex2.c_str()); \
}

#define setthex(str) { \
	static std::string hex1; \
	hex1 = str2hex(cmd.c_str(), cmd.length()); \
	static std::string hex2; \
	hex2 = str2hex(replystr.c_str(), replystr.length()); \
	get_trace(5, str, "S: ", hex1.c_str(), " R: ", hex2.c_str()); \
}

#define seth() { \
	static std::string hex1; \
	hex1 = str2hex(cmd.c_str(), cmd.length()); \
	static std::string hex2; \
	hex2 = str2hex(replystr.c_str(), replystr.length()); \
	get_trace(4, "S: ", hex1.c_str(), " R: ", hex2.c_str()); \
}

#define geth() { \
	static std::string hex1; \
	hex1 = str2hex(cmd.c_str(), cmd.length()); \
	static std::string hex2; \
	hex2 = str2hex(replystr.c_str(), replystr.length()); \
	get_trace(4, "S: ", hex1.c_str(), " R: ", hex2.c_str()); \
}

#define getcr(str) { \
	static std::string s1; \
	s1 = cmd; \
	static std::string s2; \
	s2 = replystr; \
	size_t n = 0; \
	while (n < s1.length()) { \
		if (s1[n] == '\r') s1.replace(n, 1, "<cr>"); \
		n++; \
	} \
	n = 0; \
	while (n < s2.length()) { \
		if (s2[n] == '\r') s2.replace(n, 1, "<cr>"); \
		n++; \
	} \
	get_trace(5, str, "  ", s1.c_str(), " / ", s2.c_str()); \
}

#define setcr(str) { \
	static std::string s1; \
	s1 = cmd; \
	size_t n = 0; \
	while (n < s1.length()) { \
		if (s1[n] == '\r') s1.replace(n, 1, "<cr>"); \
		n++; \
	} \
	get_trace(3, str, "  ", s1.c_str()); \
}

#ifdef WITH_TRACED
#define TRACED(name, ...) name(__VA_ARGS__) { \
      static char tmsg[50]; \
      static unsigned trace_calls_##name = 0; \
      ++trace_calls_##name; \
      snprintf(tmsg, sizeof(tmsg), "[%2u] %s", trace_calls_##name, #name ); \
      get_trace(1, tmsg);
#else
#  define TRACED(name, ...) name(__VA_ARGS__) {
#endif

#endif
