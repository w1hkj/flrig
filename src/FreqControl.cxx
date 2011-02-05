// "$Id: FreqControl.cpp,v  2006/02/26"
//
// Frequency Control Widget for the Fast Light Tool Kit (Fltk)
//
// Copyright 2005-2006, Dave Freese W1HKJ
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to "w1hkj@w1hkj.com".
//

#include <FL/Fl_Float_Input.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>

#include <cstdlib>
#include <cmath>
#include <stdio.h>

#include "FreqControl.h"
#include "util.h"
#include "gettext.h"

const char *cFreqControl::Label[10] = {
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
	
void cFreqControl::IncFreq (int nbr) {
	if (nbr < log10(precision)) return;
	double v = val;
	v += mult[nbr];
	if (v <= maxVal) {
		val += mult[nbr];
		updatevalue();
	}
	do_callback();
}

void cFreqControl::DecFreq (int nbr) {
	if (nbr < log10(precision)) return;
	long v = 1;
	v = val - mult[nbr];
	if (v >= minVal)
	  val = v;
	updatevalue();
	do_callback();
}

void cbSelectDigit (Fl_Widget *btn, void * nbr)
{

	Fl_Button *b = (Fl_Button *)btn;
	int Nbr = (int)(reinterpret_cast<long> (nbr));
	
	cFreqControl *fc = (cFreqControl *)b->parent();
	if (Fl::event_button1())
		fc->IncFreq(Nbr);
	else if (Fl::event_button3())
		fc->DecFreq(Nbr);

	fc->redraw();//damage();
}

cFreqControl::cFreqControl(int x, int y, int w, int h, const char *lbl):
			  Fl_Group(x,y,w,h,"") {
	font_number = FL_COURIER;
	ONCOLOR = FL_YELLOW;
	OFFCOLOR = FL_BLACK;
	SELCOLOR = fl_rgb_color(100, 100, 100);
	ILLUMCOLOR = FL_GREEN;
	oldval = val = 0;

// nD <= MAXDIGITS
	nD = atoi(lbl);
	if (nD > MAX_DIGITS) nD = MAX_DIGITS;
	if (nD < MIN_DIGITS) nD = MIN_DIGITS;

	int pw = 6; // decimal width
	int fcWidth = (w - pw - 4)/nD;
	int fcFirst = x;
	int fcTop = y;
	int fcHeight = h;
//	long max;
	int xpos;
	
	box(FL_DOWN_BOX);

	minVal = 0;
	double fmaxval = floor(pow(2, 8 * sizeof(maxVal) - 1) / 1000.0);
	if (nD == MAX_DIGITS) {
		maxVal = (long int)(fmaxval * 1000);
	} else {
		maxVal = (long int)(pow(10, nD) - 1);
		fmaxval = maxVal / 1000.0;
	}
	static char tt[100];
	snprintf(tt, sizeof(tt), "Enter frequency (max %.3f) or\nLeft/Right/Up/Down/Pg_Up/Pg_Down", fmaxval);
	tooltip(tt);

	for (int n = 0; n < nD; n++) {
		xpos = fcFirst + (nD - 1 - n) * fcWidth + 2;
		if (n < 3) xpos += pw;
		Digit[n] = new Fl_Repeat_Button (
			xpos,
			fcTop + 2,
			fcWidth,
			fcHeight-4,
			" ");
		Digit[n]->box(FL_FLAT_BOX); 
		Digit[n]->labelfont(font_number);
		Digit[n]->labelcolor(ONCOLOR);
		Digit[n]->color(OFFCOLOR, SELCOLOR);
		Digit[n]->labelsize(fcHeight);
		Digit[n]->callback(cbSelectDigit, (void *) n);
		if (n == 0) mult[n] = 1;
		else mult[n] = 10 * mult[n-1];
	}

	decbx = new Fl_Box(fcFirst + (nD - 3) * fcWidth + 2, fcTop + 2, pw, fcHeight-4,".");
	decbx->box(FL_FLAT_BOX);
	decbx->labelfont(font_number);
	decbx->labelcolor(ONCOLOR);
	decbx->color(OFFCOLOR);
	decbx->labelsize(fcHeight);

	cbFunc = NULL;
	end();

	finp = new Fl_Float_Input(0, 0, 1, 1);
	finp->callback(freq_input_cb, this);
	finp->when(FL_WHEN_CHANGED);
	finp->hide();
	parent()->remove(finp);

	precision = 1;

//	tooltip(_("Enter frequency or change with\nLeft/Right/Up/Down/Pg_Up/Pg_Down"));
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
	val /= precision;
	val *= precision;
	long v = val;
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
	redraw();//damage();
}

void cFreqControl::font(Fl_Font fnt)
{
	font_number = fnt;
	for (int n = 0; n < nD; n++)
		Digit[n]->labelfont(fnt);
	decbx->labelfont(fnt);
	redraw();
}

void cFreqControl::SetONOFFCOLOR( Fl_Color ONcolor, Fl_Color OFFcolor)
{
	OFFCOLOR = OFFcolor;
	ONCOLOR = ONcolor;

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

void cFreqControl::SetONCOLOR (uchar r, uchar g, uchar b) 
{
	ONCOLOR = fl_rgb_color (r, g, b);
	for (int n = 0; n < nD; n++) {
		Digit[n]->labelcolor(ONCOLOR);
		Digit[n]->color(OFFCOLOR);
		Digit[n]->redraw();
		Digit[n]->redraw_label();
	}
	decbx->labelcolor(ONCOLOR);
	decbx->color(OFFCOLOR);
	color(OFFCOLOR);
	decbx->redraw();
	decbx->redraw_label();
	redraw();
}

void cFreqControl::SetOFFCOLOR (uchar r, uchar g, uchar b) 
{
	OFFCOLOR = fl_rgb_color (r, g, b);
	for (int n = 0; n < nD; n++) {
		Digit[n]->labelcolor(ONCOLOR);
		Digit[n]->color(OFFCOLOR);
	}
	decbx->labelcolor(ONCOLOR);
	decbx->color(OFFCOLOR);
	color(OFFCOLOR);
	redraw();
}

static void blink_point(Fl_Widget* w)
{
	w->label(*w->label() ? "" : ".");
	Fl::add_timeout(0.2, (Fl_Timeout_Handler)blink_point, w);
}

void cFreqControl::value(long lv)
{
	oldval = val = lv;
	Fl::remove_timeout((Fl_Timeout_Handler)blink_point, decbx);
	updatevalue();
}

int cFreqControl::handle(int event)
{
	if (!Fl::event_inside(this))
		return Fl_Group::handle(event);

	int d;
	switch (event) {
	case FL_KEYBOARD:
		switch (d = Fl::event_key()) {
		case FL_Left:
			DecFreq(0);
			return 1;
//			d = -1;
			break;
		case FL_Down:
			DecFreq(1);
			return 1;
//			d = -10;
			break;
		case FL_Right:
			IncFreq(0);
			return 1;
//			d = 1;
			break;
		case FL_Up:
			IncFreq(1);
			return 1;
//			d = 10;
			break;
		case FL_Page_Up:
			IncFreq(2);
			return 1;
//			d = 100;
			break;
		case FL_Page_Down:
			DecFreq(2);
			return 1;
//			d = -100;
			break;
		default:
			if (Fl::has_timeout((Fl_Timeout_Handler)blink_point, decbx)) {
				if (d == FL_Escape) {
					Fl::remove_timeout((Fl_Timeout_Handler)blink_point, decbx);
					val = oldval;
					updatevalue();
					return 1;
				}
				else if (d == FL_Enter || d == FL_KP_Enter) { // append
					finp->position(finp->size());
					finp->replace(finp->position(), finp->mark(), "\n", 1);
				}
			}
			else {
//			  if (d == FL_Escape && window() != Fl::first_window()) {
//				  window()->do_callback();
//				  return 1;
//			  }
				Fl::add_timeout(0.0, (Fl_Timeout_Handler)blink_point, decbx);
				finp->static_value("");
				oldval = val;
			}
			return finp->handle(event);
		}
		val += d;
		updatevalue();
		do_callback();
		break;
	case FL_MOUSEWHEEL:
		if ( !((d = Fl::event_dy()) || (d = Fl::event_dx())) )
			return 1;

		for (int i = 0; i < nD; i++) {
			if (Fl::event_inside(Digit[i])) {
				d > 0 ? DecFreq(i) : IncFreq(i);
				break;
			}
		}
		break;
	case FL_PUSH:
		return Fl_Group::handle(event);
	}

	return 1;
}

void cFreqControl::freq_input_cb(Fl_Widget*, void* arg)
{
	cFreqControl* fc = reinterpret_cast<cFreqControl*>(arg);
	double val = strtod(fc->finp->value(), NULL);
	long lval;
//	if (val >= 0.0 && val < pow(10.0, MAX_DIGITS - 3)) {
	val *= 1e3;
	val += 0.5;
	lval = (long)val;
	if (lval <= fc->maxVal) {
		fc->val = (long)val;
		fc->updatevalue();
		if (fc->finp->index(fc->finp->size() - 1) == '\n' && val > 0.0) {
			Fl::remove_timeout((Fl_Timeout_Handler)blink_point, fc->decbx);
			fc->do_callback();
		}
	}
}

Fl_Color onclr;
Fl_Color offclr;

static void restore_colors(void* w)
{
	cFreqControl *fc = (cFreqControl *)w;
	fc->SetONOFFCOLOR(onclr, offclr);
}

void cFreqControl::visual_beep()
{
	onclr = ONCOLOR; offclr = OFFCOLOR;
	SetONOFFCOLOR(offclr, onclr);
	Fl::add_timeout(0.1, restore_colors, this);
}
