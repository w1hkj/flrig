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

#ifndef _Rig_H
#define _Rig_H

#include <string>
#include <sys/types.h>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Enumerations.H>

#ifndef WIN32
#include <unistd.h>
#include <pwd.h>
#endif

#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/fl_draw.H>

#include "config.h"

#include "threads.h"

#include "FreqControl.h"
#include "rigpanel.h"
#include "rig_io.h"
#include "serial.h"
#include "support.h"
//#include "xml_io.h"

extern Fl_Double_Window *mainwindow;
extern Fl_Double_Window *tabs_dialog;
extern string HomeDir;
extern string RigHomeDir;
extern string TempDir;
extern string rigfile;
extern string pttfile;

extern int    xmlport;

extern string defFileName;
extern string title;

extern pthread_t *serial_thread;
extern pthread_t *digi_thread;
extern pthread_mutex_t mutex_serial;
extern pthread_mutex_t mutex_xmlrpc;
extern pthread_mutex_t mutex_vfoque;
extern pthread_mutex_t mutex_ptt;
extern pthread_mutex_t mutex_replystr;
extern pthread_mutex_t mutex_srvc_reqs;
extern pthread_mutex_t mutex_trace;

extern bool EXPAND_CONTROLS;

extern void cb_xml_help(Fl_Menu_*, void*);

enum UISIZE {small_ui, wide_ui, touch_ui};

#endif
