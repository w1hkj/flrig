// ----------------------------------------------------------------------------
// Copyright (C) 2020
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
#include "gpio.h"
#include "gpio_ptt.h"

static const char *gpio_label[] = {
" 17   00   11",
" 18   01   12",
" 27   02   13",
" 22   03   15",
" 23   04   16",
" 24   05   18",
" 25   06   22",
"  4   07    7",
"  5   21   29",
"  6   22   31",
" 13   23   33",
" 19   24   35",
" 26   25   37",
" 12   26   32",
" 16   27   36",
" 20   28   38",
" 21   29   40"
};

static void show_pins()
{
	printf("%05x | %05x\n", progStatus.enable_gpio, progStatus.gpio_on);
}

void cb_btn_enable_gpio(Fl_Check_Button* btn, void *val)
{
	size_t n = (size_t)(val);
	if (btn->value()) {
		progStatus.enable_gpio |= (1 << n);
	} else {
		progStatus.enable_gpio &= ~(1 << n);
	}
	show_pins();
}

static void cb_btn_gpio_on(Fl_Check_Button* btn, void *val)
{
	size_t n = (size_t)(val);
	if (btn->value()) {
		progStatus.gpio_on |= (1 << n);
	} else {
		progStatus.gpio_on &= ~(1 << n);
	}
	show_pins();
}

static void cb_cnt_gpio_pulse_width(Fl_Counter* o, void*)
{
	progStatus.gpio_pulse_width = (int)o->value();
}

static void cb_btn_use_gpio(Fl_Check_Button* btn, void *)
{
	if (btn->value()) {
		progStatus.gpio_ptt = true;
		open_gpio();
	} else {
		progStatus.gpio_ptt = false;
		close_gpio();
	}
}

Fl_Group *createGPIO(int X, int Y, int W, int H, const char *label)
{
	Fl_Group *tab = new Fl_Group(X, Y, W, H, label);

	size_t w = (W - 20)/4;
	size_t h = 18;
	size_t hd = (H - 4)/ 11;
	size_t y = Y + 2 + hd;
	size_t col1 = X + 20;
	size_t col2 = col1 + w;
	size_t col3 = col2 + w;
	size_t col4 = col3 + w;

	Fl_Check_Button * btn_use_gpio = new Fl_Check_Button (col2 + w/2, Y + 2, w, h, "Use GPIO PTT");
	btn_use_gpio->down_box(FL_DOWN_BOX);
	btn_use_gpio->labelfont(FL_COURIER);
	btn_use_gpio->labelsize(12);
	btn_use_gpio->value(progStatus.gpio_ptt);
	btn_use_gpio->callback((Fl_Callback*)cb_btn_use_gpio);

	Fl_Box* bx1 = new Fl_Box(col1, y, w, h, "  BCM  GPIO Pin ON");
	bx1->labelfont(FL_COURIER);
	bx1->labelsize(12);
	bx1->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));

	Fl_Box* bx3 = new Fl_Box(col3, y, w, h, "  BCM  GPIO Pin ON");
	bx3->labelfont(FL_COURIER);
	bx3->labelsize(12);
	bx3->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));

	for (size_t n = 0; n < 8; n++) {
		btn_enable_gpio[n] = new Fl_Check_Button(col1, y + hd * (n + 1), w, h, gpio_label[n]);
		btn_enable_gpio[n]->tooltip(_("Select pin number"));
		btn_enable_gpio[n]->down_box(FL_DOWN_BOX);
		btn_enable_gpio[n]->labelfont(FL_COURIER);
		btn_enable_gpio[n]->labelsize(12);
		btn_enable_gpio[n]->callback((Fl_Callback*)cb_btn_enable_gpio, (void *)n);
		btn_enable_gpio[n]->value((progStatus.enable_gpio >> n)& 0x01);
	}

	for (size_t n = 8; n < 17; n++) {
		btn_enable_gpio[n] = new Fl_Check_Button(col3, y + hd * (n - 7), w, h, gpio_label[n]);
		btn_enable_gpio[n]->tooltip(_("Select pin number"));
		btn_enable_gpio[n]->down_box(FL_DOWN_BOX);
		btn_enable_gpio[n]->labelfont(FL_COURIER);
		btn_enable_gpio[n]->labelsize(12);
		btn_enable_gpio[n]->callback((Fl_Callback*)cb_btn_enable_gpio, (void *)n);
		btn_enable_gpio[n]->value((progStatus.enable_gpio >> n)& 0x01);
	}

	for (size_t n = 0; n < 8; n++) {
		btn_gpio_on[n] = new Fl_Check_Button(col2, y + hd * (n + 1), w, h, "");
		btn_gpio_on[n]->tooltip(_("Select PTT on state"));
		btn_gpio_on[n]->down_box(FL_DOWN_BOX);
		btn_gpio_on[n]->labelfont(FL_COURIER);
		btn_gpio_on[n]->labelsize(12);
		btn_gpio_on[n]->callback((Fl_Callback*)cb_btn_gpio_on, (void *)n);
		btn_gpio_on[n]->value((progStatus.gpio_on >> n) & 0x01);
	}

	for (size_t n = 8; n < 17; n++) {
		btn_gpio_on[n] = new Fl_Check_Button(col4, y + hd * (n - 7), w, h, "");
		btn_gpio_on[n]->tooltip(_("Select PTT on state"));
		btn_gpio_on[n]->down_box(FL_DOWN_BOX);
		btn_gpio_on[n]->labelfont(FL_COURIER);
		btn_gpio_on[n]->labelsize(12);
		btn_gpio_on[n]->callback((Fl_Callback*)cb_btn_gpio_on, (void *)n);
		btn_gpio_on[n]->value((progStatus.gpio_on >> n) & 0x01);
	}

	cnt_gpio_pulse_width = new Fl_Counter(col1, y + hd * 9, 80, 20, "Pulse width (msec)");
	cnt_gpio_pulse_width->tooltip(_("Set >0 if pulsed PTT used"));
	cnt_gpio_pulse_width->type(1);
	cnt_gpio_pulse_width->labelfont(FL_COURIER);
	cnt_gpio_pulse_width->labelsize(12);
	cnt_gpio_pulse_width->minimum(0);
	cnt_gpio_pulse_width->maximum(50);
	cnt_gpio_pulse_width->step(1);
	cnt_gpio_pulse_width->callback((Fl_Callback*)cb_cnt_gpio_pulse_width);
	cnt_gpio_pulse_width->align(Fl_Align(FL_ALIGN_RIGHT));
	cnt_gpio_pulse_width->value(progStatus.gpio_pulse_width);

	tab->end();

  return tab;
}
