// ----------------------------------------------------------------------------
// Frequency Control Widget
//
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

#include <FL/Fl_Float_Input.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>
#include <iostream>

#include <cstdlib>
#include <cmath>
#include <stdio.h>

#include "FreqControl.h"
#include "util.h"
#include "gettext.h"

#include "status.h"

const char *cFreqControl::Label[10] = {
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };

void cFreqControl::IncFreq (size_t nbr) {
	double v = val;
	v += mult[nbr] * precision;
	if (v <= maxVal) {
		val += mult[nbr] * precision;
		updatevalue();
	}
	do_callback();
}

void cFreqControl::DecFreq (size_t nbr) {
	unsigned long int v = 1;
	v = val - mult[nbr] * precision;
	if (v >= minVal)
		val = v;
	updatevalue();
	do_callback();
}

void cbSelectDigit (Fl_Widget *btn, void * nbr)
{

	Fl_Button *b = (Fl_Button *)btn;
	size_t Nbr = reinterpret_cast<size_t> (nbr);

	cFreqControl *fc = (cFreqControl *)b->parent();
	if (fc->hrd_buttons) {
		int yclick = Fl::event_y();
		int fc_yc = fc->y() + fc->h()/2;
		if (yclick <= fc_yc)
			fc->IncFreq(Nbr);
		else
			fc->DecFreq(Nbr);
	} else {
		if (Fl::event_button1())
			fc->IncFreq(Nbr);
		else if (Fl::event_button3())
			fc->DecFreq(Nbr);
	}
	fc->redraw();
}

void cFreqControl::set_ndigits(int nbr)
{
	int H = Digit[0]->h();
	int W = Digit[0]->w();
	int ht = H;

	fl_font(font_number, ht);
	int fh = fl_height();
	int fw = fl_width("0");

	while ( ht && ((fh >= H) || (fw >= W))) {
		if (--ht <= 1) break;
		fl_font(font_number, ht);
		fh = fl_height();
		fw = fl_width("0");
	}

	Fl_Boxtype B = FL_FLAT_BOX;
	int xpos = Digit[0]->x() + W;
	int ypos = Digit[0]->y();
	for (int n = 0; n < dpoint; n++) {
		xpos -= W;
		Digit[n]->box(B);
		Digit[n]->labelfont(font_number);
		Digit[n]->labelsize(fh);
		Digit[n]->position(xpos, ypos);
		Digit[n]->redraw_label();
		Digit[n]->redraw();
		Digit[n]->callback(cbSelectDigit, reinterpret_cast<void*>(n));
	}

	xpos -= W / 2;
	decbx->position(xpos, ypos);
	decbx->labelfont(font_number);
	decbx->labelsize(fh);
	decbx->redraw_label();
	decbx->redraw();

	for (int n = dpoint; n < nD; n++) {
		xpos -= W;
		Digit[n]->box(B);
		Digit[n]->labelfont(font_number);
		Digit[n]->labelsize(fh);
		Digit[n]->position(xpos, ypos);
		Digit[n]->redraw_label();
		Digit[n]->redraw();
		Digit[n]->callback(cbSelectDigit, reinterpret_cast<void*>(n));
	}

	font_size = ht;
}

cFreqControl::cFreqControl(int x, int y, int w, int h, const char *lbl) : Fl_Group(x,y,w,h,"")
{
	font_number = FL_COURIER;

	ONCOLOR = FL_YELLOW;
	OFFCOLOR = FL_BLACK;
	SELCOLOR = fl_rgb_color(100, 100, 100);
	ILLUMCOLOR = FL_GREEN;
	oldval = val = 0;
	precision = 1;
	dpoint = 3;

	nD = atol(lbl);
	if (nD > MAX_DIGITS) nD = MAX_DIGITS;
	if (nD < MIN_DIGITS) nD = MIN_DIGITS;

	minVal = 0;
	maxVal = 
		(nD == 1 ? 9L :
		(nD == 2 ? 99L :
		(nD == 3 ? 999L :
		(nD == 4 ? 9999L :
		(nD == 5 ? 99999L :
		(nD == 6 ? 999999L :
		(nD == 7 ? 9999999L :
		(nD == 8 ? 99999999L :
		(nD == 9 ? 999999999L :
		1410065407L )))))))));

	box(FL_DOWN_BOX);
	bdr = Fl::box_dy(box());

	Fl_Boxtype B = FL_FLAT_BOX;

	int H = h - 2 * bdr;
	int W = w - 2 * bdr;
	int X = x + bdr;
	int Y = y + bdr;
	int wb = trunc(W / (nD + 0.5));
	int fill = W - wb * (nD + 0.5);

	hfill = new Fl_Box(X, Y, fill, H, "");
	hfill->box(FL_FLAT_BOX);
	hfill->labelcolor(ONCOLOR);
	hfill->color(ONCOLOR);

	for (int n = 0; n < 3; n++) {
		Digit[n] = new Fl_Repeat_Button ( X + W - (n + 1) * wb, Y, wb, H, " ");
		Digit[n]->box(B); 
		Digit[n]->labelcolor(ONCOLOR);
		Digit[n]->color(OFFCOLOR, SELCOLOR);
		Digit[n]->align(FL_ALIGN_INSIDE);
		Digit[n]->callback(cbSelectDigit, reinterpret_cast<void*>(n));
	}

	decbx = new Fl_Box(X + W - 3*wb - wb/2, Y, wb / 2, H, ".");
	decbx->box(B);
	decbx->color(ONCOLOR);
	decbx->labelcolor(ONCOLOR);
	decbx->align(FL_ALIGN_INSIDE);

	for (int n = 3; n < nD; n++) {
		Digit[n] = new Fl_Repeat_Button ( X + W - wb/2 - 3 * wb  - (n - 2) * wb, Y, wb, H, " ");
		Digit[n]->box(B); 
		Digit[n]->labelcolor(ONCOLOR);
		Digit[n]->color(OFFCOLOR, SELCOLOR);
		Digit[n]->align(FL_ALIGN_INSIDE);
		Digit[n]->callback(cbSelectDigit, reinterpret_cast<void*>(n));
	}

	mult[0] = 1;
	for (int n = 1; n < nD; n++ )
		mult[n] = 10 * mult[n-1];

	cbFunc = NULL;

//	finp = new Fl_Float_Input(0, 0, 24, 24);
	finp = new Fl_Float_Input(X, Y, W, H);
	finp->callback(freq_input_cb, this);
	finp->when(FL_WHEN_CHANGED);
	finp->textsize(H - 4);
	finp->hide();
//	remove(finp);

	end();

	precision = 1;
	hrd_buttons = true;
	colors_reversed = false;

	minVal = 0;
	maxVal = (unsigned long int)(pow(10.0, nD) - 1) * precision;
	double fmaxval = maxVal / 1000.0;
	static char tt[100];
	snprintf(tt, sizeof(tt), "Enter frequency (max %.3f) or\nLeft/Right/Up/Down/Pg_Up/Pg_Down", fmaxval);
	tooltip(tt);

	set_ndigits(nD);

	active = true;
}

cFreqControl::~cFreqControl()
{
	for (int i = 0; i < nD; i++) {
		delete Digit[i];
	}
	delete finp;
}


void cFreqControl::updatevalue()
{
	unsigned long int v = val / precision;
	int i;
	if (likely(v > 0L)) {
		for (i = 0; i < nD; i++) {
			Digit[i]->label(v == 0 ? "" : Label[v % 10]);
			v /= 10;
		}
	}
	else {
		for (i = 0; i < 4; i++)
			Digit[i]->label("0");
		for (; i < nD; i++)
			Digit[i]->label("");
	}
	decbx->label(".");
	redraw();
}

void cFreqControl::font(Fl_Font fnt)
{
	font_number = fnt;
	set_ndigits(nD);
	updatevalue();
	redraw();
}

void cFreqControl::SetONOFFCOLOR( Fl_Color ONcolor, Fl_Color OFFcolor)
{
	OFFCOLOR = REVONCOLOR = OFFcolor;
	ONCOLOR = REVOFFCOLOR = ONcolor;

	for (int n = 0; n < nD; n++) {
		Digit[n]->labelcolor(ONCOLOR);
		Digit[n]->color(OFFCOLOR);
		Digit[n]->redraw();
		Digit[n]->redraw_label();
	}
	decbx->labelcolor(ONCOLOR);
	decbx->color(OFFCOLOR);
	decbx->redraw();
	decbx->redraw_label();
	hfill->color(OFFCOLOR);
	color(OFFCOLOR);
	redraw();
}

void cFreqControl::SetONCOLOR (uchar r, uchar g, uchar b) 
{
	ONCOLOR = fl_rgb_color (r, g, b);
	REVOFFCOLOR = ONCOLOR;
	for (int n = 0; n < nD; n++) {
		Digit[n]->labelcolor(ONCOLOR);
		Digit[n]->color(OFFCOLOR);
		Digit[n]->redraw();
		Digit[n]->redraw_label();
	}
	decbx->labelcolor(ONCOLOR);
	decbx->color(OFFCOLOR);
	decbx->redraw();
	decbx->redraw_label();
	hfill->color(OFFCOLOR);
	color(OFFCOLOR);
	redraw();
}

void cFreqControl::SetOFFCOLOR (uchar r, uchar g, uchar b) 
{
	OFFCOLOR = fl_rgb_color (r, g, b);
	REVONCOLOR = OFFCOLOR;

	for (int n = 0; n < nD; n++) {
		Digit[n]->labelcolor(ONCOLOR);
		Digit[n]->color(OFFCOLOR);
	}
	decbx->labelcolor(ONCOLOR);
	decbx->color(OFFCOLOR);
	decbx->redraw();
	decbx->redraw_label();
	hfill->color(OFFCOLOR);
	color(OFFCOLOR);
	redraw();
}

static void blink_point(Fl_Widget* w)
{
	w->label(*w->label() ? "" : ".");
	Fl::add_timeout(0.2, (Fl_Timeout_Handler)blink_point, w);
}

void cFreqControl::value(unsigned long int lv)
{
	oldval = val = lv;
	Fl::remove_timeout((Fl_Timeout_Handler)blink_point, decbx);
	updatevalue();
}

void cFreqControl::restore_colors()
{
	colors_reversed = false;
	for (int n = 0; n < nD; n++) {
		Digit[n]->labelcolor(ONCOLOR);
		Digit[n]->color(OFFCOLOR);
		Digit[n]->redraw();
		Digit[n]->redraw_label();
	}
	decbx->labelcolor(ONCOLOR);
	decbx->color(OFFCOLOR);
	decbx->redraw();
	decbx->redraw_label();
	color(OFFCOLOR);
	redraw();
}

void cFreqControl::reverse_colors()
{
	colors_reversed = true;
	for (int n = 0; n < nD; n++) {
		Digit[n]->labelcolor(REVONCOLOR);
		Digit[n]->color(REVOFFCOLOR);
		Digit[n]->redraw();
		Digit[n]->redraw_label();
	}
	decbx->labelcolor(REVONCOLOR);
	decbx->color(REVOFFCOLOR);
	decbx->redraw();
	decbx->redraw_label();
	color(REVOFFCOLOR);
	redraw();
}

int cFreqControl::handle(int event)
{
	if (!active) return 0;

	if (Fl::belowmouse() == this) Fl::focus(this);

	int d;
	switch (event) {
	case FL_KEYBOARD:
		d = Fl::event_key();
		if (finp->visible()) {
			if (d == FL_Escape) {
				fcval = 0;
				finp->hide();
				return 1;
			}
			if (d == FL_Enter || d == FL_KP_Enter) {
				if (fcval) {
					val = fcval;
					updatevalue();
					do_callback();
				}
				finp->hide();
				Fl::focus(this);
				return 1;
			}
			return finp->handle(event);
		}
		switch (d) {
			case FL_Page_Up:
				if (Fl::event_shift()) IncFreq(1);
				else IncFreq(0);
				return 1;
				break;
			case FL_Page_Down:
				if (Fl::event_shift()) DecFreq(1);
				else DecFreq(0);
				return 1;
				break;
			case FL_Right:
				if (Fl::event_shift()) IncFreq(4);
				else IncFreq(2);
				return 1;
				break;
			case FL_Left:
				if (Fl::event_shift()) DecFreq(4);
				else DecFreq(2);
				return 1;
				break;
			case FL_Up:
				if (Fl::event_shift()) IncFreq(5);
				else IncFreq(3);
				return 1;
				break;
			case FL_Down:
				if (Fl::event_shift()) DecFreq(5);
				else DecFreq(3);
				return 1;
				break;
			default:
				{
					int ch = Fl::event_text()[0];
					if ((ch >= '0' && ch <= '9') || ch == '.') {
						finp->value("");
						fcval = 0;
						finp->show();
						return finp->handle(event);
					}
				}
				return 0;
				break;
		}
	case FL_MOUSEWHEEL:
		if ( !((d = Fl::event_dy()) || (d = Fl::event_dx())) )
			return 1;

		for (int i = 0; i < nD; i++) {
			if (Fl::event_inside(Digit[i])) {
				d > 0 ? DecFreq(i) : IncFreq(i);
				return 1;
			}
		}
		break;
	case FL_PUSH:
		return Fl_Group::handle(event); // in turn calls the digit[] callback

	}
	return 0;
}

void cFreqControl::freq_input_cb(Fl_Widget*, void* arg)
{
	cFreqControl* fc = reinterpret_cast<cFreqControl*>(arg);
	float val = strtof(fc->finp->value(), NULL);
	fc->fcval = (unsigned long int)(val * 1000);
}

static void restore_color(void* w)
{
	cFreqControl *fc = (cFreqControl *)w;
	fc->restore_colors();
}

void cFreqControl::visual_beep()
{
	reverse_colors();
	Fl::add_timeout(0.1, restore_color, this);
}

void cFreqControl::resize(int x, int y, int w, int h)
{
	Fl_Group::resize(x,y,w,h);
	set_ndigits(nD);
	updatevalue();
	redraw();
return;
	int wf1 = (w - nD * fcWidth - pw - 2*bdr) / 2;
	int wf2 = w - nD * fcWidth - pw - 2*bdr - wf1;

	int xpos = x + w - bdr - wf2;
	int ypos = y + bdr;
	for (int n = 0; n < dpoint; n++) {
		xpos -= fcWidth;
		Digit[n]->resize(xpos, ypos, fcWidth, fcHeight);
	}

	xpos -= pw;
	decbx->resize(xpos, ypos, pw, fcHeight);

	for (int n = dpoint; n < nD; n++) {
		xpos -= fcWidth;
		Digit[n]->resize(xpos, ypos, fcWidth, fcHeight);
	}

	Fl_Group::redraw();
}

