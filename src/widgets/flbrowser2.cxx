// =====================================================================
//
// flinput2.cxx
//
// Author: Stelios Buononos, M0GLD
// Copyright: 2010
//
// This file is part of flrig.
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  It is
// copyright under the GNU General Public License.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// =====================================================================

#include "config.h"

#include <cctype>

#include <iostream>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Tooltip.H>

#include "icons.h"
#include "flbrowser2.h"
#include "gettext.h"
#include "util.h"

#include <FL/names.h>

Fl_Browser2::Fl_Browser2(int x, int y, int w, int h, const char* l)
: Fl_Browser(x, y, w, h, l)
{
}

int Fl_Browser2::handle(int event)
{
	int key = Fl::event_key();
	if (event == FL_KEYUP && 
		( key == FL_Enter ||
		  key == FL_Left ||
		  key == FL_Right ||
		  key == FL_Delete ) ) {
		do_callback();
		return 1;
	}
	return Fl_Browser::handle(event);
}

