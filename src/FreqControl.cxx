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
	double v = val;
	v += mult[nbr] * precision;
	if (v <= maxVal) {
		val += mult[nbr] * precision;
		updatevalue();
	}
	do_callback();
}

void cFreqControl::DecFreq (int nbr) {
	long v = 1;
	v = val - mult[nbr] * precision;
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
	fc->redraw();
}

cFreqControl::cFreqControl(int x, int y, int w, int h, const char *lbl):
			  Fl_Group(x,y,w,h,"") {
	font_number = FL_COURIER;
	ONCOLOR = FL_YELLOW;
	OFFCOLOR = FL_BLACK;
	SELCOLOR = fl_rgb_color(100, 100, 100);
	ILLUMCOLOR = FL_GREEN;
	oldval = val = 0;
	precision = 1;
	dpoint = 3;

	W = w;
	nD = atoi(lbl);
	if (nD > MAX_DIGITS) nD = MAX_DIGITS;
	if (nD < MIN_DIGITS) nD = MIN_DIGITS;

	bdr = 2;
	fcHeight = h - 2 * bdr;

	int fw, fh, ht = fcHeight;

	fl_font(font_number, ++ht);
	fh = fl_height() + 1;
	while (fh > fcHeight) {
		ht--;
		fl_font(font_number, ht);
		fh = fl_height();
	}

	fl_font(font_number, ht);
	fw = fl_width("0") + 4;
	pw = fw / 2;
	while( (nD * fw + pw) >= (W - 2*bdr)) {
		ht--;
		fl_font(font_number, ht);
		fw = fl_width("0") + 4;
		pw = fw / 2;
	}
	fh = fl_height();

	wfill = (w - nD * fw - pw - 2*bdr) / 2;
	int wf2 = w - nD * fw - pw - 2*bdr - wfill;

	fcWidth = fw;

	fcTop = y + bdr;
	int xpos;

	box(FL_DOWN_BOX);
	color(OFFCOLOR);

	minVal = 0;
	maxVal = (long int)(pow(10, nD) - 1) * precision;
	double fmaxval = maxVal / 1000.0;

	static char tt[100];
	snprintf(tt, sizeof(tt), "Enter frequency (max %.3f) or\nLeft/Right/Up/Down/Pg_Up/Pg_Down", fmaxval);
	tooltip(tt);

	hfill2 = new Fl_Box(x + w - bdr - wf2, fcTop, wf2, fcHeight, "");
	hfill2->box(FL_FLAT_BOX);
	hfill2->labelcolor(ONCOLOR);
	hfill2->color(OFFCOLOR);

	xpos = x + w - bdr - wfill;
	for (int n = 0; n < 3; n++) {
		xpos -= fcWidth;
		Digit[n] = new Fl_Repeat_Button (
			xpos,
			fcTop,
			fcWidth,
			fcHeight,
			" ");
		Digit[n]->box(FL_FLAT_BOX); 
		Digit[n]->labelfont(font_number);
		Digit[n]->labelcolor(ONCOLOR);
		Digit[n]->color(OFFCOLOR, SELCOLOR);
		Digit[n]->labelsize(fh);
		Digit[n]->callback(cbSelectDigit, (void *) n);
		if (n == 0) mult[n] = 1;
		else mult[n] = 10 * mult[n-1];
	}

	xpos -= pw;
	decbx = new Fl_Box(xpos, fcTop, pw, fcHeight,".");
	decbx->box(FL_FLAT_BOX);
	decbx->labelfont(font_number);
	decbx->labelcolor(ONCOLOR);
	decbx->color(OFFCOLOR);
	decbx->labelsize(fh);

	for (int n = 3; n < nD; n++) {
		xpos -= fcWidth;
		Digit[n] = new Fl_Repeat_Button (
			xpos,
			fcTop,
			fcWidth,
			fcHeight,
			" ");
		Digit[n]->box(FL_FLAT_BOX); 
		Digit[n]->labelfont(font_number);
		Digit[n]->labelcolor(ONCOLOR);
		Digit[n]->color(OFFCOLOR, SELCOLOR);
		Digit[n]->labelsize(fh);
		Digit[n]->callback(cbSelectDigit, (void *) n);
		if (n == 0) mult[n] = 1;
		else mult[n] = 10 * mult[n-1];
	}

	hfill1 = new Fl_Box(x + bdr, fcTop, Digit[nD-1]->x() - x - bdr, fcHeight, "");
	hfill1->box(FL_FLAT_BOX);
	hfill1->labelcolor(ONCOLOR);
	hfill1->color(OFFCOLOR);

	cbFunc = NULL;
	end();

	finp = new Fl_Float_Input(0, 0, 24,24);//1, 1);
	finp->callback(freq_input_cb, this);
	finp->when(FL_WHEN_CHANGED);

	parent()->remove(finp);

	precision = 1;

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
	long v = val / precision;
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
	set_ndigits(nD);
	updatevalue();
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
	hfill1->labelcolor(ONCOLOR);
	hfill1->color(OFFCOLOR);
	hfill1->redraw();
	hfill1->redraw_label();
	hfill2->labelcolor(ONCOLOR);
	hfill2->color(OFFCOLOR);
	hfill2->redraw();
	hfill2->redraw_label();
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
	decbx->redraw();
	decbx->redraw_label();
	hfill1->color(OFFCOLOR);
	hfill1->labelcolor(ONCOLOR);
	hfill1->redraw();
	hfill1->redraw_label();
	hfill2->labelcolor(ONCOLOR);
	hfill2->color(OFFCOLOR);
	hfill2->redraw();
	hfill2->redraw_label();
	color(OFFCOLOR);
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
	decbx->redraw();
	decbx->redraw_label();
	hfill1->color(OFFCOLOR);
	hfill1->labelcolor(ONCOLOR);
	hfill1->redraw();
	hfill1->redraw_label();
	hfill2->labelcolor(ONCOLOR);
	hfill2->color(OFFCOLOR);
	hfill2->redraw();
	hfill2->redraw_label();
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
			break;
		case FL_Down:
			DecFreq(1);
			return 1;
			break;
		case FL_Right:
			IncFreq(0);
			return 1;
			break;
		case FL_Up:
			IncFreq(1);
			return 1;
			break;
		case FL_Page_Up:
			IncFreq(2);
			return 1;
			break;
		case FL_Page_Down:
			DecFreq(2);
			return 1;
			break;
		default:
			if (Fl::event_ctrl()) {
				if (Fl::event_key() == 'v') {
					finp->handle(event);
					Fl::remove_timeout((Fl_Timeout_Handler)blink_point, decbx);
					return 1;
				}
			}
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
		return Fl_Group::handle(event); // in turn calls the digit[] callback

	}

	return 1;
}

void cFreqControl::freq_input_cb(Fl_Widget*, void* arg)
{
	cFreqControl* fc = reinterpret_cast<cFreqControl*>(arg);
	double val = strtod(fc->finp->value(), NULL);
	long lval;
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

void cFreqControl::set_ndigits(int nbr)
{
	delete decbx;
	for (int n = 0; n < nD; n++) {
		this->remove(Digit[n]);
		delete Digit[n];
	}

	nD = nbr;
	if (nD > MAX_DIGITS) nD = MAX_DIGITS;
	if (nD < MIN_DIGITS) nD = MIN_DIGITS;

	int fw, fh, ht = h() - 2*bdr;

	fl_font(font_number, ht);
	fh = fl_height() + 1;
	while (fh > fcHeight) {
		ht--;
		fl_font(font_number, ht);
		fh = fl_height();
	}

	fl_font(font_number, ht);
	fw = fl_width("0") + 4;
	pw = fw / 2;
	while( (nD * fw + pw) >= (W - 2*bdr)) {
		ht--;
		fl_font(font_number, ht);
		fw = fl_width("0") + 4;
		pw = fw / 2;
	}
	fh = fl_height();
	fcWidth = fw;

	int wf1 = (w() - nD * fcWidth - pw - 2*bdr)/2;
	int wf2 = w() - nD * fcWidth - pw - 2*bdr - wf1;

	fcTop = y() + bdr;
	int xpos;

	minVal = 0;
	maxVal = (long int)(pow(10, nD) - 1) * precision;
	double fmaxval = maxVal / 1000.0;

	static char tt[100];
	snprintf(tt, sizeof(tt), "Enter frequency (max %.3f) or\nLeft/Right/Up/Down/Pg_Up/Pg_Down", fmaxval);
	tooltip(tt);

	hfill2->resize(x() + w() - bdr - wf2, fcTop, wf2, fcHeight);

	xpos = x() + w() - bdr - wf2;
	for (int n = 0; n < dpoint; n++) {
		xpos -= fcWidth;
		Digit[n] = new Fl_Repeat_Button (
			xpos,
			fcTop,
			fcWidth,
			fcHeight,
			" ");
		Digit[n]->box(FL_FLAT_BOX); 
		Digit[n]->labelfont(font_number);
		Digit[n]->labelcolor(ONCOLOR);
		Digit[n]->color(OFFCOLOR, SELCOLOR);
		Digit[n]->labelsize(fh);
		Digit[n]->callback(cbSelectDigit, (void *) n);
		if (n == 0) mult[n] = 1;
		else mult[n] = 10 * mult[n-1];
		this->add(Digit[n]);
	}

	xpos -= pw;
	decbx = new Fl_Box(xpos, fcTop, pw, fcHeight,".");
	decbx->box(FL_FLAT_BOX);
	decbx->labelfont(font_number);
	decbx->labelcolor(ONCOLOR);
	decbx->color(OFFCOLOR);
	decbx->labelsize(fh);
	this->add(decbx);

	for (int n = dpoint; n < nD; n++) {
		xpos -= fcWidth;
		Digit[n] = new Fl_Repeat_Button (
			xpos,
			fcTop,
			fcWidth,
			fcHeight,
			" ");
		Digit[n]->box(FL_FLAT_BOX); 
		Digit[n]->labelfont(font_number);
		Digit[n]->labelcolor(ONCOLOR);
		Digit[n]->color(OFFCOLOR, SELCOLOR);
		Digit[n]->labelsize(fh);
		Digit[n]->callback(cbSelectDigit, (void *) n);
		if (n == 0) mult[n] = 1;
		else mult[n] = 10 * mult[n-1];
		this->add(Digit[n]);
	}

	hfill1->resize(x() + bdr, fcTop, wf1, fcHeight);

	redraw();
}

void cFreqControl::resize(int x, int y, int w, int h)
{
	Fl_Group::resize(x,y,w,h);

	int wf1 = (w - nD * fcWidth - pw - 2*bdr) / 2;
	int wf2 = w - nD * fcWidth - pw - 2*bdr - wf1;

	hfill2->resize(x + w - bdr - wf2, y + bdr, wf2, h - 2*bdr);

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

	hfill1->resize(x + bdr, y + bdr, wf1, h - 2*bdr);

	Fl_Group::redraw();
}
