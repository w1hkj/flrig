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

Fl_Check_Button *btn_wide_ui=(Fl_Check_Button *)0;
Fl_Box *lblTest=(Fl_Box *)0;
Fl_Button *prefFont=(Fl_Button *)0;
Fl_Button *prefForeground=(Fl_Button *)0;
Fl_Button *btnBacklight=(Fl_Button *)0;
Fl_Group *grpMeterColor=(Fl_Group *)0;
Fl_Box *scaleSmeterColor=(Fl_Box *)0;
Fl_SigBar *sldrRcvSignalColor=(Fl_SigBar *)0;
Fl_SigBar *sldrSWRcolor=(Fl_SigBar *)0;
Fl_Box *scaleSWRcolor=(Fl_Box *)0;
Fl_SigBar *sldrPWRcolor=(Fl_SigBar *)0;
Fl_Box *scalePWRcolor=(Fl_Box *)0;
Fl_Button *btMeterColor=(Fl_Button *)0;
Fl_Button *btnSWRColor=(Fl_Button *)0;
Fl_Button *btnPwrColor=(Fl_Button *)0;
Fl_Button *btnPeakColor=(Fl_Button *)0;
Fl_Choice *mnuScheme=(Fl_Choice *)0;
Fl_Button *pref_sys_foreground=(Fl_Button *)0;
Fl_Button *pref_sys_background=(Fl_Button *)0;
Fl_Button *prefsys_background2=(Fl_Button *)0;
Fl_Button *prefsys_defaults=(Fl_Button *)0;
Fl_Wheel_Value_Slider *sldrColors=(Fl_Wheel_Value_Slider *)0;
Fl_Button *pref_slider_background=(Fl_Button *)0;
Fl_Button *pref_slider_select=(Fl_Button *)0;
Fl_Button *prefslider_defaults=(Fl_Button *)0;
Fl_Button *btnReset=(Fl_Button *)0;
Fl_Button *btnCancel=(Fl_Button *)0;
Fl_Return_Button *btnOkDisplayDialog=(Fl_Return_Button *)0;
Fl_Light_Button *btn_lighted=(Fl_Light_Button *)0;
Fl_Button *btn_lighted_default=(Fl_Button *)0;

static void cb_btn_wide_ui(Fl_Check_Button* o, void*) {
	progStatus.UIchanged = true;
}

static void cb_prefFont(Fl_Button*, void*) {
	cbPrefFont();
}

static void cb_prefForeground(Fl_Button*, void*) {
	cbPrefForeground();
}

static void cb_btnBacklight(Fl_Button*, void*) {
	cbBacklightColor();
}

static void cb_btMeterColor(Fl_Button*, void*) {
	cbSMeterColor();
}

static void cb_btnSWRColor(Fl_Button*, void*) {
	cbSWRMeterColor();
}

static void cb_btnPwrColor(Fl_Button*, void*) {
	cbPwrMeterColor();
}

static void cb_btnPeakColor(Fl_Button*, void*) {
	cbPeakMeterColor();
}

static void cb_mnuScheme(Fl_Choice* o, void*) {
	progStatus.ui_scheme = o->text();
	Fl::scheme(progStatus.ui_scheme.c_str());
}

static void cb_pref_sys_foreground(Fl_Button*, void*) {
	cb_sys_foreground();
}

static void cb_pref_sys_background(Fl_Button*, void*) {
	cb_sys_background();
}

static void cb_prefsys_background2(Fl_Button*, void*) {
	cb_sys_background2();
}

static void cb_prefsys_defaults(Fl_Button*, void*) {
	cb_sys_defaults();
}

static void cb_pref_slider_background(Fl_Button*, void*) {
	cb_slider_background();
}

static void cb_pref_slider_select(Fl_Button*, void*) {
	cb_slider_select();
}

static void cb_prefslider_defaults(Fl_Button*, void*) {
	cb_slider_defaults();
}

static void cb_btnReset(Fl_Button*, void*) {
	cb_reset_display_dialog();
}

static void cb_btnCancel(Fl_Button*, void*) {
	cbCancelDisplayDialog();
}

static void cb_btnOkDisplayDialog(Fl_Return_Button*, void*) {
	cbOkDisplayDialog();
}

static void cb_btn_lighted(Fl_Light_Button*, void*) {
	cb_lighted_button();
}

static void cb_btn_lighted_default(Fl_Button*, void*) {
	cb_lighted_default();
}

Fl_Double_Window* DisplayDialog() {
	Fl_Double_Window* w = new Fl_Double_Window(424, 250, _("Flrig User Interface"));

	Fl_Group* su_grp1 = new Fl_Group(1, 1, 421, 39, _("Main Dialog Aspect Ratio (change requires restart)"));
		su_grp1->box(FL_ENGRAVED_FRAME);
		su_grp1->align(Fl_Align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE));

		btn_wide_ui = new Fl_Check_Button(158, 20, 102, 15, _("Wide UI"));
		btn_wide_ui->down_box(FL_DOWN_BOX);
		btn_wide_ui->callback((Fl_Callback*)cb_btn_wide_ui);
		btn_wide_ui->value(progStatus.UIsize);
	su_grp1->end();

	Fl_Group* su_grp2 = new Fl_Group(2, 41, 218, 70);
		su_grp2->box(FL_ENGRAVED_FRAME);

		Fl_Group* su_grp2a = new Fl_Group(5, 45, 210, 35);
			su_grp2a->box(FL_DOWN_BOX);

			lblTest = new Fl_Box(7, 47, 206, 31, _("14070.000"));
			lblTest->box(FL_FLAT_BOX);
			lblTest->labelfont(4);
			lblTest->labelsize(32);
		su_grp2a->end();

		prefFont = new Fl_Button(10, 85, 60, 22, _("Font"));
		prefFont->callback((Fl_Callback*)cb_prefFont);

		prefForeground = new Fl_Button(78, 85, 60, 22, _("Color"));
		prefForeground->callback((Fl_Callback*)cb_prefForeground);

		btnBacklight = new Fl_Button(146, 85, 60, 22, _("Back"));
		btnBacklight->callback((Fl_Callback*)cb_btnBacklight);

	su_grp2->end();

	Fl_Group* su_grp3 = new Fl_Group(2, 113, 284, 107);

		Fl_Group* su_grp3a = new Fl_Group(5, 116, 210, 100);
			su_grp3a->box(FL_DOWN_BOX);

			grpMeterColor = new Fl_Group(7, 118, 206, 96);
				grpMeterColor->box(FL_FLAT_BOX);

				scaleSmeterColor = new Fl_Box(8, 118, 205, 20);
				scaleSmeterColor->box(FL_FLAT_BOX);
				scaleSmeterColor->image(image_S60);
				scaleSmeterColor->color(scaleSmeter->color());

				sldrRcvSignalColor = new Fl_SigBar(10, 138, 200, 6);
				sldrRcvSignalColor->box(FL_FLAT_BOX);
				sldrRcvSignalColor->color(FL_BACKGROUND_COLOR);
				sldrRcvSignalColor->selection_color(FL_BACKGROUND_COLOR);
				sldrRcvSignalColor->labeltype(FL_NORMAL_LABEL);
				sldrRcvSignalColor->labelfont(0);
				sldrRcvSignalColor->labelsize(14);
				sldrRcvSignalColor->labelcolor(FL_FOREGROUND_COLOR);
				sldrRcvSignalColor->align(Fl_Align(FL_ALIGN_CENTER));
				sldrRcvSignalColor->when(FL_WHEN_RELEASE);

				sldrSWRcolor = new Fl_SigBar(10, 169, 200, 6);
				sldrSWRcolor->box(FL_FLAT_BOX);
				sldrSWRcolor->color(FL_BACKGROUND_COLOR);
				sldrSWRcolor->selection_color(FL_BACKGROUND_COLOR);
				sldrSWRcolor->labeltype(FL_NORMAL_LABEL);
				sldrSWRcolor->labelfont(0);
				sldrSWRcolor->labelsize(14);
				sldrSWRcolor->labelcolor(FL_FOREGROUND_COLOR);
				sldrSWRcolor->align(Fl_Align(FL_ALIGN_CENTER));
				sldrSWRcolor->when(FL_WHEN_RELEASE);
				sldrSWRcolor->minimum(0);
				sldrSWRcolor->maximum(100);

				scaleSWRcolor = new Fl_Box(8, 149, 205, 20);
				scaleSWRcolor->box(FL_FLAT_BOX);
				scaleSWRcolor->image(image_SWR);

				sldrPWRcolor = new Fl_SigBar(8, 186, 200, 6);
				sldrPWRcolor->box(FL_FLAT_BOX);
				sldrPWRcolor->color(FL_BACKGROUND_COLOR);
				sldrPWRcolor->selection_color(FL_BACKGROUND_COLOR);
				sldrPWRcolor->labeltype(FL_NORMAL_LABEL);
				sldrPWRcolor->labelfont(0);
				sldrPWRcolor->labelsize(14);
				sldrPWRcolor->labelcolor(FL_FOREGROUND_COLOR);
				sldrPWRcolor->align(Fl_Align(FL_ALIGN_CENTER));
				sldrPWRcolor->when(FL_WHEN_RELEASE);
				sldrPWRcolor->minimum(0);
				sldrPWRcolor->maximum(100);

				scalePWRcolor = new Fl_Box(8, 192, 205, 20);
				scalePWRcolor->box(FL_FLAT_BOX);
				scalePWRcolor->image(image_P100);

			grpMeterColor->end();

		su_grp3a->end();

		btMeterColor = new Fl_Button(221, 117, 60, 22, _("Smeter"));
		btMeterColor->callback((Fl_Callback*)cb_btMeterColor);

		btnSWRColor = new Fl_Button(221, 142, 60, 22, _("SWR"));
		btnSWRColor->callback((Fl_Callback*)cb_btnSWRColor);

		btnPwrColor = new Fl_Button(221, 167, 60, 22, _("Pwr"));
		btnPwrColor->callback((Fl_Callback*)cb_btnPwrColor);

		btnPeakColor = new Fl_Button(221, 193, 60, 22, _("Peak"));
		btnPeakColor->callback((Fl_Callback*)cb_btnPeakColor);

	su_grp3->end();

	Fl_Group* su_grp4 = new Fl_Group(220, 41, 202, 70, _("System"));
		su_grp4->box(FL_ENGRAVED_FRAME);
		su_grp4->align(Fl_Align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE));

		mnuScheme = new Fl_Choice(227, 61, 80, 22, _("UI"));
		mnuScheme->tooltip(_("Change application look and feel"));
		mnuScheme->down_box(FL_BORDER_BOX);
		mnuScheme->callback((Fl_Callback*)cb_mnuScheme);
		mnuScheme->align(Fl_Align(FL_ALIGN_RIGHT));
		mnuScheme->add("base");
		mnuScheme->add("gtk+");
		mnuScheme->add("plastic");
		mnuScheme->value(mnuScheme->find_item(progStatus.ui_scheme.c_str()));

		pref_sys_foreground = new Fl_Button(227, 85, 60, 22, _("Fgnd"));
		pref_sys_foreground->tooltip(_("Label color"));
		pref_sys_foreground->callback((Fl_Callback*)cb_pref_sys_foreground);

		pref_sys_background = new Fl_Button(291, 85, 60, 22, _("Bngd"));
		pref_sys_background->tooltip(_("Background - normal"));
		pref_sys_background->callback((Fl_Callback*)cb_pref_sys_background);

		prefsys_background2 = new Fl_Button(356, 85, 60, 22, _("Bgnd2"));
		prefsys_background2->tooltip(_("Background - selected"));
		prefsys_background2->callback((Fl_Callback*)cb_prefsys_background2);

		prefsys_defaults = new Fl_Button(356, 60, 60, 22, _("Default"));
		prefsys_defaults->tooltip(_("Background - selected"));
		prefsys_defaults->callback((Fl_Callback*)cb_prefsys_defaults);

	su_grp4->end();

	Fl_Group* su_grp5 = new Fl_Group(287, 161, 134, 88);
		su_grp5->box(FL_ENGRAVED_FRAME);

		sldrColors = new Fl_Wheel_Value_Slider(291, 167, 125, 20);
		sldrColors->tooltip(_("Adjust power level"));
		sldrColors->type(5);
		sldrColors->box(FL_THIN_DOWN_BOX);
		sldrColors->color(FL_BACKGROUND_COLOR);
		sldrColors->selection_color(FL_BACKGROUND_COLOR);
		sldrColors->labeltype(FL_NORMAL_LABEL);
		sldrColors->labelfont(0);
		sldrColors->labelsize(14);
		sldrColors->labelcolor(FL_FOREGROUND_COLOR);
		sldrColors->maximum(100);
		sldrColors->step(1);
		sldrColors->value(15);
		sldrColors->textsize(12);
		sldrColors->align(Fl_Align(FL_ALIGN_CENTER));
		sldrColors->when(FL_WHEN_CHANGED);
        sldrColors->reverse(true);

		pref_slider_background = new Fl_Button(291, 193, 60, 22, _("Bngd"));
		pref_slider_background->tooltip(_("Background - normal"));
		pref_slider_background->callback((Fl_Callback*)cb_pref_slider_background);

		pref_slider_select = new Fl_Button(356, 193, 60, 22, _("Button"));
		pref_slider_select->tooltip(_("Background - normal"));
		pref_slider_select->callback((Fl_Callback*)cb_pref_slider_select);

		prefslider_defaults = new Fl_Button(324, 222, 60, 22, _("Default"));
		prefslider_defaults->tooltip(_("Background - selected"));
		prefslider_defaults->callback((Fl_Callback*)cb_prefslider_defaults);

	su_grp5->end();

	btnReset = new Fl_Button(25, 223, 60, 22, _("Reset"));
	btnReset->tooltip(_("Restore all flrig defaults"));
	btnReset->callback((Fl_Callback*)cb_btnReset);

	btnCancel = new Fl_Button(113, 223, 60, 22, _("Cancel"));
	btnCancel->tooltip(_("Discard current changes"));
	btnCancel->callback((Fl_Callback*)cb_btnCancel);

	btnOkDisplayDialog = new Fl_Return_Button(201, 223, 60, 22, _("OK"));
	btnOkDisplayDialog->tooltip(_("Save Current Changes"));
	btnOkDisplayDialog->callback((Fl_Callback*)cb_btnOkDisplayDialog);

	Fl_Group* su_grp6 = new Fl_Group(287, 113, 134, 45);
		su_grp6->box(FL_ENGRAVED_FRAME);

		btn_lighted = new Fl_Light_Button(291, 124, 60, 22, _("Lt Btn"));
		btn_lighted->callback((Fl_Callback*)cb_btn_lighted);

		btn_lighted_default = new Fl_Button(356, 124, 60, 22, _("Default"));
		btn_lighted_default->tooltip(_("Background - selected"));
		btn_lighted_default->callback((Fl_Callback*)cb_btn_lighted_default);

	su_grp6->end();

	w->end();

	return w;
}

