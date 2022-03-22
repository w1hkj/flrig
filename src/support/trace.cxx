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

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>

#include <FL/Fl_Button.H>
#include <FL/Fl_Light_Button.H>

#include "icons.h"
#include "support.h"
#include "debug.h"
#include "gettext.h"
#include "rig_io.h"
#include "dialogs.h"
#include "rigbase.h"
#include "ptt.h"
#include "socket_io.h"
#include "ui.h"
#include "tod_clock.h"
#include "trace.h"

#include "rig.h"
#include "rigs.h"
#include "K3_ui.h"
#include "KX3_ui.h"

#include "rigpanel.h"

#include "tod_clock.h"

Fl_Double_Window*	tracewindow = (Fl_Double_Window *)0;
Fl_Text_Display*	tracedisplay = (Fl_Text_Display *)0;
Fl_Text_Buffer*		tracebuffer = (Fl_Text_Buffer*)0;
Fl_Button*			btn_cleartrace = (Fl_Button*)0;
Fl_Light_Button*	btn_pausetrace = (Fl_Light_Button*)0;

std::string tracestring;

bool stdout_trace = false;

bool pausetrace = false;

static void cb_pausetrace(Fl_Light_Button *o, void *)
{
	pausetrace = o->value();
}

static void cb_cleartrace(Fl_Button *, void *)
{
	guard_lock tt(&mutex_trace);
	tracedisplay->buffer()->text("");
	tracestring.clear();
}

void make_trace_window() {
	tracewindow = new Fl_Double_Window(600, 300, _("Trace log"));
	tracedisplay = new Fl_Text_Display(0, 0, 600, 270);
	tracebuffer = new Fl_Text_Buffer;
	tracedisplay->buffer(tracebuffer);
	tracedisplay->textfont(FL_SCREEN);
	tracedisplay->wrap_mode(Fl_Text_Display::WRAP_NONE, 100);
	btn_pausetrace = new Fl_Light_Button(430, 275, 80, 20, _("Pause"));
	btn_pausetrace->callback((Fl_Callback *)cb_pausetrace);
	btn_cleartrace = new Fl_Button(515, 275, 80, 20, _("Clear"));
	btn_cleartrace->callback((Fl_Callback *)cb_cleartrace);
	tracewindow->resizable(tracedisplay);
}

static void write_trace_file(std::string s)
{
	std::string trace_fname = RigHomeDir;
	trace_fname.append("trace.txt");
	std::fstream tfile(trace_fname.c_str(), std::ios::app);
	if (tfile)
		tfile << s;
	tfile.close();
}

static void update_tracetext(void *)
{
	guard_lock tt(&mutex_trace);

	if (tracewindow->visible()) {
		if (!pausetrace) {
			if (tracedisplay->buffer()->length() > 100000)
				tracedisplay->buffer()->text("");
			tracedisplay->insert(tracestring.c_str());
		}
	}
    tracestring.clear();
}

void trace(int n, ...) // all args of type const char *
{
	if (!progStatus.trace) return;

	if (!tracewindow) make_trace_window();

	if (!n) return;

	std::stringstream s;
	va_list vl;
	va_start(vl, n);
	s << ztime() << " : " << va_arg(vl, const char *);
	for (int i = 1; i < n; i++)
		s << " " << va_arg(vl, const char *);
	va_end(vl);
	s << "\n";

	if (stdout_trace) {
		std::cout << s.str();
		return;
	}

	write_trace_file(s.str());

	guard_lock tt(&mutex_trace);
	tracestring.append(s.str());
	Fl::awake(update_tracetext);
}

#include "XmlRpc.h"

void xml_trace(int n, ...) // all args of type const char *
{
	if (!progStatus.xmltrace) return;

	if (!tracewindow) make_trace_window();

	if (!n) return;

	std::stringstream s;
	va_list vl;
	va_start(vl, n);
	s << ztime();
#ifdef HAS_XMLRPC_CLIENT_ID
	s << " [" << XmlRpc::client_id << "]";
#endif
	s << " : " << va_arg(vl, const char *);
	for (int i = 1; i < n; i++)
		s << " " << va_arg(vl, const char *);
	va_end(vl);
	s << "\n";

	if (stdout_trace) {
		std::cout << s.str();
		return;
	}

	write_trace_file(s.str());

	guard_lock tt(&mutex_trace);
	tracestring.append(s.str());
	Fl::awake(update_tracetext);
}

void rig_trace(int n, ...) // all args of type const char *
{
	std::stringstream s;
	va_list vl;
	va_start(vl, n);
	s << ztime() << " : " << va_arg(vl, const char *);
	for (int i = 1; i < n; i++)
		s << " " << va_arg(vl, const char *);
	va_end(vl);
	s << "\n";

	if (stdout_trace) {
		std::cout << s.str();
		return;
	}

	if (!progStatus.rigtrace) return;
	if (!tracewindow) make_trace_window();
	if (!n) return;

	write_trace_file(s.str());

	guard_lock tt(&mutex_trace);
	tracestring.append(s.str());
	Fl::awake(update_tracetext);
}

void set_trace(int n, ...) // all args of type const char *
{
	std::stringstream s;
	va_list vl;
	va_start(vl, n);
	s << ztime() << " : " << va_arg(vl, const char *);
	for (int i = 1; i < n; i++)
		s << " " << va_arg(vl, const char *);
	va_end(vl);
	s << "\n";

	if (stdout_trace) {
		std::cout << s.str();
		return;
	}

	if (!progStatus.settrace) return;
	if (!tracewindow) make_trace_window();
	if (!n) return;

	write_trace_file(s.str());

	guard_lock tt(&mutex_trace);
	tracestring.append(s.str());
	Fl::awake(update_tracetext);
}

void get_trace(int n, ...) // all args of type const char *
{
	std::stringstream s;
	va_list vl;
	va_start(vl, n);
	s << ztime() << " : " << va_arg(vl, const char *);
	for (int i = 1; i < n; i++)
		s << " " << va_arg(vl, const char *);
	va_end(vl);
	s << "\n";

	if (stdout_trace) {
		std::cout << s.str();
		return;
	}

	if (!progStatus.gettrace) return;
	if (!tracewindow) make_trace_window();
	if (!n) return;

	write_trace_file(s.str());

	guard_lock tt(&mutex_trace);
	tracestring.append(s.str());
	Fl::awake(update_tracetext);
}

void rpc_trace(int n, ...) // all args of type const char *
{
	std::stringstream s;
	va_list vl;
	va_start(vl, n);
	s << ztime() << " : " << va_arg(vl, const char *);
	for (int i = 1; i < n; i++)
		s << " " << va_arg(vl, const char *);
	va_end(vl);
	std::string str = s.str();
	size_t p = str.find("\r\n");
	while (p != std::string::npos) {
		str.replace(p, 2, "\n");
		p = str.find("\r\n");
	}
	p = str.find("\r");
	while (p != std::string::npos) {
		str.replace(p, 1, "\n");
		p = str.find("\r");
	}
	p = str.find("\t");
	while (p != std::string::npos) {
		str.erase(p, 1);
		p = str.find("\t");
	}

	int indent = 0;
	p = str.find("<");
	while (p != std::string::npos) {
		if (str[p+1] != '/') {
			str.insert(p, "\n");
			str.insert(p+1, indent, ' ');
			indent += 2;
		} else {
			str.insert(p, "\n");
			indent -= 2;
			str.insert(p+1, indent, ' ');
		}
		if (indent < 0) indent = 0;
		p = str.find(">", p);
		p = str.find("<", p);
	}

	p = str.find("\n\n");
	while (p != std::string::npos) {
		str.erase(p,1);
		p = str.find("\n\n");
	}

	if (str[str.length()-1] != '\n') str += '\n';

	if (stdout_trace) {
		std::cout << s.str();
		return;
	}

	if (!tracewindow) make_trace_window();
	if (!progStatus.rpctrace) return;
	if (!n) return;

	write_trace_file(s.str());

	guard_lock tt(&mutex_trace);
	tracestring.append(s.str());
	Fl::awake(update_tracetext);
}

void ser_trace(int n, ...) // all args of type const char *
{
	std::stringstream s;
	va_list vl;
	va_start(vl, n);
	s << ztime() << " : " << va_arg(vl, const char *);
	for (int i = 1; i < n; i++)
		s << " " << va_arg(vl, const char *);
	va_end(vl);
	s << "\n";

	if (stdout_trace) {
		std::cout << s.str();
		return;
	}

	if (!progStatus.serialtrace) return;
	if (!tracewindow) make_trace_window();
	if (!n) return;

	write_trace_file(s.str());

	guard_lock tt(&mutex_trace);
	tracestring.append(s.str());
	Fl::awake(update_tracetext);
}
