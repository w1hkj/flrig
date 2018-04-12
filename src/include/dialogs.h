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

#ifndef __DIALOGS_H__
#define __DIALOGS_H__

#include <string>

#include "font_browser.h"
#include "support.h"

extern Fl_Double_Window *dlgDisplayConfig;
extern Fl_Double_Window *dlgXcvrConfig;
extern Fl_Double_Window *dlgMemoryDialog;
extern Fl_Double_Window *dlgControls;

extern Font_Browser     *fntbrowser;

extern void init_port_combos();

#endif // __DIALOGS_H__
