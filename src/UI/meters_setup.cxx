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

Fl_Wheel_Value_Slider *sldr_smeter_avg=(Fl_Wheel_Value_Slider *)0;
Fl_Wheel_Value_Slider *sldr_smeter_peak=(Fl_Wheel_Value_Slider *)0;
Fl_Wheel_Value_Slider *sldr_pout_avg=(Fl_Wheel_Value_Slider *)0;
Fl_Wheel_Value_Slider *sldr_pout_peak=(Fl_Wheel_Value_Slider *)0;

static void cb_sldr_smeter_avg(Fl_Wheel_Value_Slider* o, void*) {
	progStatus.rx_avg=o->value();
	sldrRcvSignal->avg(o->value());
}

static void cb_sldr_smeter_peak(Fl_Wheel_Value_Slider* o, void*) {
	progStatus.rx_peak=o->value();
	sldrRcvSignal->aging(o->value());
}

static void cb_sldr_pout_avg(Fl_Wheel_Value_Slider* o, void*) {
	progStatus.pwr_avg=o->value();
	sldrFwdPwr->avg(o->value());
}

static void cb_sldr_pout_peak(Fl_Wheel_Value_Slider* o, void*) {
	progStatus.pwr_peak=o->value();
	sldrFwdPwr->aging(o->value());
}

Fl_Double_Window* MetersDialog() {
	Fl_Double_Window* w = new Fl_Double_Window(410, 192, _("Meter Filters"));

	sldr_smeter_avg = new Fl_Wheel_Value_Slider(17, 31, 375, 18, _("S meter averaging"));
	sldr_smeter_avg->tooltip(_("Meter averaged over # samples"));
	sldr_smeter_avg->type(5);
	sldr_smeter_avg->box(FL_THIN_DOWN_BOX);
	sldr_smeter_avg->color(FL_LIGHT3);
	sldr_smeter_avg->selection_color((Fl_Color)4);
	sldr_smeter_avg->labeltype(FL_NORMAL_LABEL);
	sldr_smeter_avg->labelfont(0);
	sldr_smeter_avg->labelsize(14);
	sldr_smeter_avg->labelcolor(FL_FOREGROUND_COLOR);
	sldr_smeter_avg->minimum(1);
	sldr_smeter_avg->maximum(10);
	sldr_smeter_avg->step(1);
	sldr_smeter_avg->value(5);
	sldr_smeter_avg->textsize(12);
	sldr_smeter_avg->callback((Fl_Callback*)cb_sldr_smeter_avg);
	sldr_smeter_avg->align(Fl_Align(FL_ALIGN_TOP_LEFT));
	sldr_smeter_avg->when(FL_WHEN_CHANGED);
	sldr_smeter_avg->reverse(true);
	sldr_smeter_avg->value(progStatus.rx_avg);

	sldr_smeter_peak = new Fl_Wheel_Value_Slider(17, 70, 375, 18, _("S meter peak "));
	sldr_smeter_peak->tooltip(_("Max peak reading over last # averaged samples"));
	sldr_smeter_peak->type(5);
	sldr_smeter_peak->box(FL_THIN_DOWN_BOX);
	sldr_smeter_peak->color(FL_LIGHT3);
	sldr_smeter_peak->selection_color((Fl_Color)4);
	sldr_smeter_peak->labeltype(FL_NORMAL_LABEL);
	sldr_smeter_peak->labelfont(0);
	sldr_smeter_peak->labelsize(14);
	sldr_smeter_peak->labelcolor(FL_FOREGROUND_COLOR);
	sldr_smeter_peak->minimum(1);
	sldr_smeter_peak->maximum(10);
	sldr_smeter_peak->step(1);
	sldr_smeter_peak->value(5);
	sldr_smeter_peak->textsize(12);
	sldr_smeter_peak->callback((Fl_Callback*)cb_sldr_smeter_peak);
	sldr_smeter_peak->align(Fl_Align(FL_ALIGN_TOP_LEFT));
	sldr_smeter_peak->when(FL_WHEN_CHANGED);
	sldr_smeter_peak->reverse(true);
	sldr_smeter_peak->value(progStatus.rx_peak);

	sldr_pout_avg = new Fl_Wheel_Value_Slider(17, 109, 375, 18, _("Pwr out averaging"));
	sldr_pout_avg->tooltip(_("Meter averaged over # samples"));
	sldr_pout_avg->type(5);
	sldr_pout_avg->box(FL_THIN_DOWN_BOX);
	sldr_pout_avg->color(FL_LIGHT3);
	sldr_pout_avg->selection_color((Fl_Color)4);
	sldr_pout_avg->labeltype(FL_NORMAL_LABEL);
	sldr_pout_avg->labelfont(0);
	sldr_pout_avg->labelsize(14);
	sldr_pout_avg->labelcolor(FL_FOREGROUND_COLOR);
	sldr_pout_avg->minimum(1);
	sldr_pout_avg->maximum(10);
	sldr_pout_avg->step(1);
	sldr_pout_avg->value(5);
	sldr_pout_avg->textsize(12);
	sldr_pout_avg->callback((Fl_Callback*)cb_sldr_pout_avg);
	sldr_pout_avg->align(Fl_Align(FL_ALIGN_TOP_LEFT));
	sldr_pout_avg->when(FL_WHEN_CHANGED);
	sldr_pout_avg->reverse(true);
	sldr_pout_avg->value(progStatus.pwr_avg);

	sldr_pout_peak = new Fl_Wheel_Value_Slider(17, 149, 375, 18, _("Pwr out peak"));
	sldr_pout_peak->tooltip(_("Max peak reading over last # averaged samples"));
	sldr_pout_peak->type(5);
	sldr_pout_peak->box(FL_THIN_DOWN_BOX);
	sldr_pout_peak->color(FL_LIGHT3);
	sldr_pout_peak->selection_color((Fl_Color)4);
	sldr_pout_peak->labeltype(FL_NORMAL_LABEL);
	sldr_pout_peak->labelfont(0);
	sldr_pout_peak->labelsize(14);
	sldr_pout_peak->labelcolor(FL_FOREGROUND_COLOR);
	sldr_pout_peak->minimum(1);
	sldr_pout_peak->maximum(10);
	sldr_pout_peak->step(1);
	sldr_pout_peak->value(5);
	sldr_pout_peak->textsize(12);
	sldr_pout_peak->callback((Fl_Callback*)cb_sldr_pout_peak);
	sldr_pout_peak->align(Fl_Align(FL_ALIGN_TOP_LEFT));
	sldr_pout_peak->when(FL_WHEN_CHANGED);
	sldr_pout_peak->reverse(true);
	sldr_pout_peak->value(progStatus.pwr_peak);

	w->end();

	return w;
}

