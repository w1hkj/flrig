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

Fl_Button *sel_5=(Fl_Button *)0;
Fl_Button *sel_25=(Fl_Button *)0;
Fl_Button *sel_100=(Fl_Button *)0;
Fl_Button *sel_auto=(Fl_Button *)0;
Fl_Button *sel_50=(Fl_Button *)0;
Fl_Button *sel_200=(Fl_Button *)0;
Fl_Output *pwr_scale_description=(Fl_Output *)0;

static void cb_sel_5(Fl_Button*, void*) {
	progStatus.pwr_scale = 0;
	pwr_scale_description->value(mtr_scales[progStatus.pwr_scale]);
	set_power_controlImage(0);
}

static void cb_sel_25(Fl_Button*, void*) {
	progStatus.pwr_scale = 1;
	pwr_scale_description->value(mtr_scales[progStatus.pwr_scale]);
	set_power_controlImage(0);
}

static void cb_sel_50(Fl_Button*, void*) {
	progStatus.pwr_scale = 2;
	pwr_scale_description->value(mtr_scales[progStatus.pwr_scale]);
	set_power_controlImage(0);
}

static void cb_sel_100(Fl_Button*, void*) {
	progStatus.pwr_scale = 3;
	pwr_scale_description->value(mtr_scales[progStatus.pwr_scale]);
	set_power_controlImage(0);
}

static void cb_sel_200(Fl_Button*, void*) {
	progStatus.pwr_scale = 4;
	pwr_scale_description->value(mtr_scales[progStatus.pwr_scale]);
	set_power_controlImage(0);
}

static void cb_sel_auto(Fl_Button*, void*) {
	progStatus.pwr_scale = 5;
	pwr_scale_description->value(mtr_scales[progStatus.pwr_scale]);
	if (selrig->has_power_control)
		set_power_controlImage(sldrPOWER->value());
	else
		set_power_controlImage(sldrFwdPwr->peak());
}

Fl_Double_Window* power_meter_scale_select() {
	Fl_Double_Window* w = new Fl_Double_Window(455, 175, _("Select Power Meter Scale"));

	sel_5 = new Fl_Button(8, 7, 218, 40, _("5 watt"));
	sel_5->tooltip(_("Press to select"));
	sel_5->color((Fl_Color)215);
	sel_5->image(image_P5);
	sel_5->labelsize(12);
	sel_5->callback((Fl_Callback*)cb_sel_5);
	sel_5->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));

	sel_25 = new Fl_Button(234, 7, 218, 40, _("25 watt"));
	sel_25->tooltip(_("Press to select"));
	sel_25->color((Fl_Color)215);
	sel_25->image(image_P25);
	sel_25->labelsize(12);
	sel_25->callback((Fl_Callback*)cb_sel_25);
	sel_25->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));

	sel_50 = new Fl_Button(8, 53, 218, 40, _("50 watt"));
	sel_50->tooltip(_("Press to select"));
	sel_50->color((Fl_Color)215);
	sel_50->image(image_P50);
	sel_50->labelsize(12);
	sel_50->callback((Fl_Callback*)cb_sel_50);
	sel_50->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));

	sel_100 = new Fl_Button(234, 53, 218, 40, _("100 watt"));
	sel_100->tooltip(_("Press to select"));
	sel_100->color((Fl_Color)215);
	sel_100->image(image_P100);
	sel_100->labelsize(12);
	sel_100->callback((Fl_Callback*)cb_sel_100);
	sel_100->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));

	sel_200 = new Fl_Button(8, 100, 218, 40, _("200 watt"));
	sel_200->tooltip(_("Press to select"));
	sel_200->color((Fl_Color)215);
	sel_200->image(image_P200);
	sel_200->labelsize(12);
	sel_200->callback((Fl_Callback*)cb_sel_200);
	sel_200->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));

	sel_auto = new Fl_Button(234, 100, 218, 40, _("Auto scaled"));
	sel_auto->tooltip(_("Press to select"));
	sel_auto->color((Fl_Color)215);
	sel_auto->labelsize(12);
	sel_auto->callback((Fl_Callback*)cb_sel_auto);
	sel_auto->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));

	pwr_scale_description = new Fl_Output(234, 145, 218, 25, _("Meter face selected:"));
	pwr_scale_description->align(Fl_Align(FL_ALIGN_LEFT));
	pwr_scale_description->value(mtr_scales[progStatus.pwr_scale]);

	w->end();

	return w;
}
