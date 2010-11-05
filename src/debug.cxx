// ----------------------------------------------------------------------------
//      debug.cxx
//
// Copyright (C) 2008
//              Stelios Bounanos, M0GLD
//
// This file is part of fldigi.
//
// fldigi is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// fldigi is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <string>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Menu_Button.H>

#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>

#include "debug.h"
//#include "icons.h"
#include "gettext.h"
#include "rig.h"

using namespace std;

#define MAX_LINES 65536

static FILE* wfile;
static FILE* rfile;
//static size_t nlines = 0;
static int rfd;
static bool tty;
//static bool want_popup = false;

static Fl_Double_Window* window;
static Fl_Text_Display* text;
static Fl_Text_Buffer* buffer;

debug* debug::inst = 0;
debug::level_e debug::level = debug::WARN_LEVEL;
uint32_t debug::mask = ~0u;

const char* prefix[] = { _("Quiet"), _("Error"), _("Warning"), _("Info"), _("Debug") };

static void slider_cb(Fl_Widget* w, void*);
static void src_menu_cb(Fl_Widget* w, void*);
static void clear_cb(Fl_Widget *w, void*);
static void save_cb(Fl_Widget *w, void*);

//static void popup_message(void*);

Fl_Menu_Item src_menu[] = {
	{ _("Rig control"), 0, 0, 0, FL_MENU_TOGGLE | FL_MENU_VALUE },
	{ _("RPC"), 0, 0, 0, FL_MENU_TOGGLE | FL_MENU_VALUE },
	{ _("Other"), 0, 0, 0, FL_MENU_TOGGLE | FL_MENU_VALUE },
	{ 0 }
};

void debug::start(const char* filename)
{
	if (debug::inst)
		return;
	inst = new debug(filename);

	window = new Fl_Double_Window(500, 256, _("Event log"));

	int pad = 2;
	Fl_Menu_Button* button = new Fl_Menu_Button(pad, pad, 128, 20, _("Log sources"));
	button->menu(src_menu);
	button->callback(src_menu_cb);

	Fl_Slider* slider = new Fl_Slider(button->x() + button->w() + pad, pad, 128, 20, prefix[level]);
	slider->tooltip(_("Change log level"));
	slider->align(FL_ALIGN_RIGHT);
	slider->type(FL_HOR_NICE_SLIDER);
	slider->range(0.0, LOG_NLEVELS - 1);
	slider->step(1.0);
	slider->value(level);
	slider->callback(slider_cb);
	
	Fl_Button* savebtn  = new Fl_Button(window->w() - 124, pad, 60, 20, "save");
	savebtn->callback(save_cb);
	
	Fl_Button* clearbtn = new Fl_Button(window->w() - 60, pad, 60, 20, "clear");
	clearbtn->callback(clear_cb);

	text = new Fl_Text_Display(pad, slider->h()+pad, window->w()-2*pad, window->h()-slider->h()-2*pad, 0);
	text->textfont(FL_COURIER);
	text->textsize(FL_NORMAL_SIZE);
    text->wrap_mode(true, 60);
	window->resizable(text);
	
	buffer = new Fl_Text_Buffer();
	text->buffer(buffer);
	
	window->end();
}

void debug::stop(void)
{
	delete inst;
	inst = 0;
	delete window;
}

static char fmt[1024];
static char sztemp[1024];
static string estr = "";
bool   debug_in_use = false;

void debug::log(level_e level, const char* func, const char* srcf, int line, const char* format, ...)
{
	if (!inst)
		return;

	snprintf(fmt, sizeof(fmt), "%c: %s: %s\n", *prefix[level], func, format);

    while(debug_in_use) MilliSleep(1);
    
	va_list args;
	va_start(args, format);

//	intptr_t nw = vfprintf(wfile, fmt, args);

	vsnprintf(sztemp, sizeof(sztemp), fmt, args);
	estr.append(sztemp);

	va_end(args);
/*
#ifndef __WIN32__
	if (tty) {
		if (level <= DEBUG_LEVEL && level > QUIET_LEVEL) {
			va_start(args, format);
			vfprintf(stderr, fmt, args);
			va_end(args);
		}
	}
#else
*/
	fflush(wfile);
//#endif

    Fl::awake(sync_text, 0);

//	Fl::add_timeout(0.0, sync_text, (void*)nw);
}

void debug::elog(const char* func, const char* srcf, int line, const char* text)
{
	log(ERROR_LEVEL, func, srcf, line, "%s: %s", text, strerror(errno));
}

void debug::show(void)
{
	window->show();
}

//static char buf[BUFSIZ+1];

void debug::sync_text(void* arg)
{
/*
	intptr_t toread = (intptr_t)arg;
	size_t block = MIN((size_t)toread, sizeof(buf) - 1);
	ssize_t n;
    string tempbuf;

	while (toread > 0) {
		if ((n = read(rfd, buf, block)) <= 0)
			break;
		buf[n] = '\0';
		tempbuf.append(buf);
		toread -= n;
	}
	text->insert(tempbuf.c_str());
*/
    debug_in_use = true;
    text->insert(estr.c_str());
    estr = "";
    debug_in_use = false;
}

debug::debug(const char* filename)
{
	if ((wfile = fopen(filename, "w")) == NULL)
		throw strerror(errno);
	setvbuf(wfile, (char*)NULL, _IOLBF, 0);

	if ((rfile = fopen(filename, "r")) == NULL)
		throw strerror(errno);
	rfd = fileno(rfile);
#ifndef __WIN32__
	int f;
	if ((f = fcntl(rfd, F_GETFL)) == -1)
		throw strerror(errno);
	if (fcntl(rfd, F_SETFL, f | O_NONBLOCK) == -1)
		throw strerror(errno);
#endif
	tty = isatty(fileno(stderr));
}

debug::~debug()
{
	fclose(wfile);
	fclose(rfile);
}

static void slider_cb(Fl_Widget* w, void*)
{
	debug::level = (debug::level_e)((Fl_Slider*)w)->value();
	w->label(prefix[debug::level]);
	w->parent()->redraw();
}

static void src_menu_cb(Fl_Widget* w, void*)
{
	debug::mask ^= 1 << ((Fl_Menu_*)w)->value();
}

static void clear_cb(Fl_Widget* w, void*)
{
	buffer->text("");
}

static void save_cb(Fl_Widget* w, void*)
{
	string filename = RigHomeDir;
	filename.append("debug_log.txt");
	buffer->savefile(filename.c_str());
}
