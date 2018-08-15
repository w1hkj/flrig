// ----------------------------------------------------------------------------
// Copyright (C) 2018
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


/** \file
   The SHOW_PL_TONES() function hides the implementation classes used
   to provide the popup window and tone selection mechanism.
*/

#ifndef PL_TONES_H
#define PL_TONES_H

#include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl.H>

class TonePicker : public Fl_Window {
	int initial;
	int  which, previous;
	int done;
	void drawbox(int);
	void draw();
	int handle(int);
	Fl_Color clr_bkgnd;
	Fl_Color clr_select;
public:
	TonePicker(int oldtone, Fl_Color c1, Fl_Color c2);
	int run();
};

class Fl_PL_tone : public Fl_Group {
friend
	void btn_cb(Fl_Button *v, void *d);
private:
	Fl_Button	*btn;
	Fl_Box		*valbox;
	int val;
	Fl_Color clr_bkgnd;
	Fl_Color clr_select;
public:
	Fl_PL_tone(int X, int Y, int W, int H, const char *label);
	~Fl_PL_tone();
	int value();
	void value(int val_);
	void background(Fl_Color clr);
	void color_select(Fl_Color clr);
};



#endif
