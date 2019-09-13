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

#include <FL/Fl.H>
#include <FL/Fl_Single_Window.H>
#include <FL/fl_draw.H>
#include "pl_tones.h"

#define BOXWIDTH 50
#define BOXHEIGHT 20
#define BORDER 2

/** 
 This widget creates a modal window for selecting a tone from a CTCSS map.
*/
static const char *szTONES[] = {
 "67.0",  "69.3",  "71.9",  "74.4",  "77.0",
 "79.7",  "82.5",  "85.4",  "88.5",  "91.5", 
 "94.8",  "97.4", "100.0", "103.5", "107.2",
"110.9", "114.8", "118.8", "123.0", "127.3",
"131.8", "136.5", "141.3", "146.2", "151.4",
"156.7", "159.8", "162.2", "165.5", "167.9",
"171.3", "173.8", "177.3", "179.9", "183.5",
"186.2", "189.9", "192.8", "196.6", "199.5",
"203.5", "206.5", "210.7", "218.1", "225.7",
"229.1", "233.6", "241.8", "250.3", "254.1" };

TonePicker::TonePicker(int oldtone, Fl_Color clr1, Fl_Color clr2) :
	Fl_Window(BOXWIDTH*5 + 1 + 2*BORDER, BOXHEIGHT*10 + 1 + 2*BORDER),
	clr_bkgnd(clr1),
	clr_select(clr2)
{
	clear_border();
	set_modal();
	initial = which = oldtone;
}

void TonePicker::drawbox(int c) {
	int X = (c % 5) * BOXWIDTH + BORDER;
	int Y = (c / 5) * BOXHEIGHT + BORDER;
	fl_draw_box(c == which ? FL_DOWN_BOX : FL_BORDER_BOX,
		X, Y, BOXWIDTH + 1, BOXHEIGHT + 1, 
		c == which ? clr_select : clr_bkgnd);
	fl_color(c == which ? FL_WHITE : FL_FOREGROUND_COLOR);
	fl_draw(szTONES[c], 
			X, Y, BOXWIDTH + 1, BOXHEIGHT + 1, 
			(Fl_Align)(FL_ALIGN_CENTER | FL_ALIGN_INSIDE) );
}

void TonePicker::draw() {
	if (damage() != FL_DAMAGE_CHILD) {
		fl_draw_box(FL_UP_BOX,0,0,w(),h(),color());
		for (int c = 0; c < 50; c++) drawbox(c);
	} else {
		drawbox(previous);
		drawbox(which);
	}
	previous = which;
}

int TonePicker::handle(int e) {
	int c = which;
	switch (e) {
	case FL_PUSH:
	case FL_DRAG: {
		int X = (Fl::event_x_root() - x() - BORDER);
		if (X >= 0) X = X / BOXWIDTH;
		int Y = (Fl::event_y_root() - y() - BORDER);
		if (Y >= 0) Y = Y / BOXHEIGHT;
		if (X >= 0 && X < 5 && Y >= 0 && Y < 10)
			c = 5*Y + X;
		else
			c = initial;
		} break;
	case FL_RELEASE:
		done = 1;
		return 1;
	case FL_KEYBOARD:
		switch (Fl::event_key()) {
		case FL_Up: if (c > 5) c -= 5; break;
		case FL_Down: if (c < 45) c += 5; break;
		case FL_Left: if (c > 0) c--; break;
		case FL_Right: if (c < 50) c++; break;
		case FL_Escape: which = initial; done = 1; return 1;
		case FL_KP_Enter:
		case FL_Enter: done = 1; return 1;
		default: return 0;
		}
		break;
	default:
		return 0;
	}
	if (c != which) {
		which = c; damage(FL_DAMAGE_CHILD);
		int bx = (c % 5) * BOXWIDTH + BORDER;
		int by = (c / 5) * BOXHEIGHT + BORDER;
		int px = x();
		int py = y();
		int scr_x, scr_y, scr_w, scr_h;
		Fl::screen_xywh(scr_x, scr_y, scr_w, scr_h);
		if (px < scr_x) px = scr_x;
		if (px + bx + BOXWIDTH + BORDER >= scr_x+scr_w)
			px = scr_x + scr_w - bx - BOXWIDTH-BORDER;
		if (py < scr_y) py = scr_y;
		if (py + by + BOXHEIGHT + BORDER >= scr_y+scr_h)
			py = scr_y + scr_h - by - BOXHEIGHT - BORDER;
		if (px + bx < BORDER) px = BORDER - bx;
		if (py + by < BORDER) py = BORDER - by;
		position(px,py);
	}
	return 1;
}

int  TonePicker::run() {
	if (which > 50) {
		position(
			Fl::event_x_root() - w()/2, Fl::event_y_root() - y()/2);
	} else {
		position(
			Fl::event_x_root() - (initial % 5)*BOXWIDTH - BOXWIDTH/2 - BORDER,
			Fl::event_y_root() - (initial / 10)*BOXHEIGHT - BOXHEIGHT/2 - BORDER);
	}
	show();
	Fl::grab(*this);
	done = 0;
	while (!done) Fl::wait();
	Fl::grab(0);
	return which;
}

void btn_cb (Fl_Button *v, void *d)
{
	Fl_PL_tone *p = (Fl_PL_tone *)(v->parent());
	TonePicker m(p->val, p->clr_bkgnd, p->clr_select);
	int val = m.run();
	p->val = val;
	p->valbox->label(szTONES[val]);
	p->valbox->redraw();
}

Fl_PL_tone::Fl_PL_tone(int X, int Y, int W, int H, const char *label)
		: Fl_Group (X, Y, W, H, label)
{
	valbox = new Fl_Box (FL_DOWN_BOX, X, Y, W-H, H, "");
	valbox->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
	valbox->color(FL_BACKGROUND2_COLOR);

	btn = new Fl_Button (X + W - H + 1, Y, H - 1, H, "@2>");
	btn->callback ((Fl_Callback *)btn_cb, 0);

	clr_bkgnd = FL_BACKGROUND_COLOR;
	clr_select = FL_SELECTION_COLOR;

	val = 0;
}

Fl_PL_tone::~Fl_PL_tone()
{
	delete valbox;
	delete btn;
}

int Fl_PL_tone::value()
{
	return val;
}

void Fl_PL_tone::value(int val_)
{
	val = val_;
	valbox->label(szTONES[val]);
}

void Fl_PL_tone::background(Fl_Color clr)
{
	clr_bkgnd = clr;
}

void Fl_PL_tone::color_select(Fl_Color clr)
{
	clr_select = clr;
}
