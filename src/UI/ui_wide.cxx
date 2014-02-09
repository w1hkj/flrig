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


//unsigned char menu_wide_menu_i18n_done = 0;

Fl_Menu_Item menu_wide_menu[] = {
 {_("&Files"), 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {_("E&xit"), 0,  (Fl_Callback*)cb_mnuExit, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {_("&Config"), 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Restore Freq/Mode"), 0,  (Fl_Callback*)cb_mnuRestoreData, 0, 6, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Keep Freq/Mode"), 0,  (Fl_Callback*)cb_mnuKeepData, 0, 134, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Tooltips"), 0,  (Fl_Callback*)cb_mnuTooltips, 0, 2, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Small sliders"), 0,  (Fl_Callback*)cb_mnuSchema, 0, 146, FL_NORMAL_LABEL, 0, 14, 0},
 {_("User Interface"), 0,  (Fl_Callback*)cb_mnuColorConfig, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Meter filtering"), 0,  (Fl_Callback*)cb_mnu_meter_filtering, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Xcvr select"), 0,  (Fl_Callback*)cb_mnuConfigXcvr, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {_("&Memory"), 0,  (Fl_Callback*)cb_Memory, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Mem-save"), 0,  (Fl_Callback*)cb_save_me, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("&Debug"), 0,  0, 0, 192, FL_NORMAL_LABEL, 0, 14, 0},
 {_("&Events"), 0,  (Fl_Callback*)cb_Events, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
 {_("&Polling"), 0,  (Fl_Callback*)cb_Polling, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("&Send command"), 0,  (Fl_Callback*)cb_Send, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0}
};

Fl_Menu_Item menu_wide_help[] = {
 {_("&Help"), 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {_("On Line Help"), 0,  (Fl_Callback*)cb_mnuOnLineHelp, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("&About"), 0,  (Fl_Callback*)cb_mnuAbout, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0}
};

Fl_Double_Window* Wide_rig_window() {
	Fl_Double_Window* w = new Fl_Double_Window(735, 218, _("Flrig"));
	w->align(Fl_Align(FL_ALIGN_CLIP|FL_ALIGN_INSIDE));

	Fl_Group* grp_menu = new Fl_Group(0,0,735,22);

		wide_menu = new Fl_Menu_Bar(0, 0, 735-64, 22);
		wide_menu->textsize(12);
		progStatus.tooltips ? (&menu_wide_menu[6])->set() : (&menu_wide_menu[6])->clear();
		wide_menu->menu(menu_wide_menu);

		Fl_Menu_Bar *help_menu = new Fl_Menu_Bar(735-64, 0, 64, 22);
		help_menu->textsize(12);
		help_menu->menu(menu_wide_help);

		grp_menu->resizable(wide_menu);

	grp_menu->end();

	mnuExit = (menu_wide_menu+1);
	mnuConfig = (menu_wide_menu+3);
	mnuRestoreData = (menu_wide_menu+4);
	mnuKeepData = (menu_wide_menu+5);
	mnuTooltips = (menu_wide_menu+6);
	mnuSchema = (menu_wide_menu+7);
	mnuColorConfig = (menu_wide_menu+8);
	mnu_meter_filtering = (menu_wide_menu+9);
	mnuConfigXcvr = (menu_wide_menu+10);
	save_me = (menu_wide_menu+13);
	mnuHelp = (menu_wide_menu+19);
	mnuOnLineHelp =  (menu_wide_menu+20);
	mnuAbout = (menu_wide_menu+21);

	txt_encA = new Fl_Output( 733 - 90, 1, 90, 20, "");
	txt_encA->box(FL_THIN_DOWN_BOX);
	txt_encA->align(20);
	txt_encA->hide();

	Fl_Group* wd_grp1 = new Fl_Group(0, 23, 734, 129);
		Fl_Group* wd_grp2 = new Fl_Group(1, 24, 210, 60);
			wd_grp2->box(FL_DOWN_BOX);

			grpMeters = new Fl_Group(3, 25, 207, 57);
				grpMeters->box(FL_FLAT_BOX);
				grpMeters->color((Fl_Color)246);

				scaleSmeter = new Fl_Box(3, 25, 206, 20);
				scaleSmeter->box(FL_FLAT_BOX);
				scaleSmeter->color((Fl_Color)246);
				scaleSmeter->image(image_S60);

				btnALC_SWR = new Fl_Button(3, 25, 206, 20);
				btnALC_SWR->tooltip(_("Press SWR / ALC"));
				btnALC_SWR->box(FL_FLAT_BOX);
				btnALC_SWR->down_box(FL_FLAT_BOX);
				btnALC_SWR->color((Fl_Color)246);
				btnALC_SWR->image(image_SWR);
				btnALC_SWR->callback((Fl_Callback*)cb_btnALC_SWR);
				btnALC_SWR->hide();

				sldrRcvSignal = new Fl_SigBar(5, 47, 200, 6);
				sldrRcvSignal->box(FL_FLAT_BOX);
				sldrRcvSignal->color(FL_BACKGROUND_COLOR);
				sldrRcvSignal->selection_color(FL_BACKGROUND_COLOR);
				sldrRcvSignal->labeltype(FL_NORMAL_LABEL);
				sldrRcvSignal->labelfont(0);
				sldrRcvSignal->labelsize(14);
				sldrRcvSignal->labelcolor(FL_FOREGROUND_COLOR);
				sldrRcvSignal->align(Fl_Align(FL_ALIGN_CENTER));
				sldrRcvSignal->when(FL_WHEN_RELEASE);
				sldrRcvSignal->hide();
				sldrRcvSignal->minimum(0);
				sldrRcvSignal->maximum(100);

				sldrALC = new Fl_SigBar(5, 47, 200, 6);
				sldrALC->box(FL_FLAT_BOX);
				sldrALC->color(FL_BACKGROUND_COLOR);
				sldrALC->selection_color(FL_BACKGROUND_COLOR);
				sldrALC->labeltype(FL_NORMAL_LABEL);
				sldrALC->labelfont(0);
				sldrALC->labelsize(14);
				sldrALC->labelcolor(FL_FOREGROUND_COLOR);
				sldrALC->align(Fl_Align(FL_ALIGN_CENTER));
				sldrALC->when(FL_WHEN_RELEASE);
				sldrALC->hide();
				sldrALC->minimum(0);
				sldrALC->maximum(100);

				sldrSWR = new Fl_SigBar(5, 47, 200, 6);
				sldrSWR->box(FL_FLAT_BOX);
				sldrSWR->color(FL_BACKGROUND_COLOR);
				sldrSWR->selection_color(FL_BACKGROUND_COLOR);
				sldrSWR->labeltype(FL_NORMAL_LABEL);
				sldrSWR->labelfont(0);
				sldrSWR->labelsize(14);
				sldrSWR->labelcolor(FL_FOREGROUND_COLOR);
				sldrSWR->align(Fl_Align(FL_ALIGN_CENTER));
				sldrSWR->when(FL_WHEN_RELEASE);
				sldrSWR->hide();
				sldrSWR->minimum(0);
				sldrSWR->maximum(100);

				sldrFwdPwr = new Fl_SigBar(5, 54, 200, 6);
				sldrFwdPwr->box(FL_FLAT_BOX);
				sldrFwdPwr->color(FL_BACKGROUND_COLOR);
				sldrFwdPwr->selection_color(FL_BACKGROUND_COLOR);
				sldrFwdPwr->labeltype(FL_NORMAL_LABEL);
				sldrFwdPwr->labelfont(0);
				sldrFwdPwr->labelsize(14);
				sldrFwdPwr->labelcolor(FL_FOREGROUND_COLOR);
				sldrFwdPwr->align(Fl_Align(FL_ALIGN_CENTER));
				sldrFwdPwr->when(FL_WHEN_RELEASE);
				sldrFwdPwr->hide();
				sldrFwdPwr->minimum(0);
				sldrFwdPwr->maximum(100);

				scalePower = new Fl_Button(3, 61, 206, 20);
				scalePower->tooltip(_("right-click to select power scale"));
				scalePower->box(FL_FLAT_BOX);
				scalePower->down_box(FL_FLAT_BOX);
				scalePower->color((Fl_Color)246);
				scalePower->image(image_P100);
				scalePower->callback((Fl_Callback*)cb_scalePower);

				meter_fill_box = new Fl_Box(210, 25, 0, 57);
				meter_fill_box->box(FL_FLAT_BOX);
				meter_fill_box->color((Fl_Color)246);
				Fl_Group::current()->resizable(meter_fill_box);

			grpMeters->end();

		wd_grp2->end();

		Fl_Group* wd_grp3 = new Fl_Group(212, 24, 522, 60);

			FreqDispA = new cFreqControl(212, 24, 290, 60, _("9"));
			FreqDispA->box(FL_DOWN_BOX);
			FreqDispA->color(FL_BACKGROUND_COLOR);
			FreqDispA->selection_color(FL_BACKGROUND_COLOR);
			FreqDispA->labeltype(FL_NORMAL_LABEL);
			FreqDispA->labelfont(0);
			FreqDispA->labelsize(14);
			FreqDispA->labelcolor(FL_FOREGROUND_COLOR);
			FreqDispA->align(Fl_Align(FL_ALIGN_CENTER));
			FreqDispA->when(FL_WHEN_CHANGED);
			Fl_Group::current()->resizable(FreqDispA);
			FreqDispA->SetONOFFCOLOR (FL_YELLOW, FL_BLACK);
			FreqDispA->setCallBack(movFreqA);

			FreqDispB = new cFreqControl(504, 24, 228, 38, _("9"));
			FreqDispB->box(FL_DOWN_BOX);
			FreqDispB->color(FL_BACKGROUND_COLOR);
			FreqDispB->selection_color(FL_BACKGROUND_COLOR);
			FreqDispB->labeltype(FL_NORMAL_LABEL);
			FreqDispB->labelfont(0);
			FreqDispB->labelsize(14);
			FreqDispB->labelcolor(FL_FOREGROUND_COLOR);
			FreqDispB->align(Fl_Align(FL_ALIGN_CENTER));
			FreqDispB->when(FL_WHEN_CHANGED);
			FreqDispB->SetONOFFCOLOR (FL_YELLOW, FL_BLACK);
			FreqDispB->setCallBack(movFreqB);

			grp_row0a = new Fl_Group(505, 63, 229, 20);

				btnA = new Fl_Light_Button(505, 63, 65, 20, _("vfoA"));
				btnA->tooltip(_("Rx-A / Tx-A"));
				btnA->down_box(FL_THIN_DOWN_BOX);
				btnA->labelsize(12);
				btnA->callback((Fl_Callback*)cb_btnA);
				btnA->value(!useB);

				btn_K3_swapAB = new Fl_Button(505, 63, 65, 20, _("A/B"));
				btn_K3_swapAB->tooltip(_("Swap A/B"));
				btn_K3_swapAB->down_box(FL_DOWN_BOX);
				btn_K3_swapAB->labelsize(12);
				btn_K3_swapAB->callback((Fl_Callback*)cb_btn_K3_swapAB);
				btn_K3_swapAB->hide();

				btnB = new Fl_Light_Button(573, 63, 65, 20, _("vfoB"));
				btnB->tooltip(_("Rx-B / Tx-B"));
				btnB->down_box(FL_THIN_DOWN_BOX);
				btnB->labelsize(12);
				btnB->callback((Fl_Callback*)cb_btnB);
				btnB->value(useB);

				btnTune = new Fl_Button(643, 63, 65, 20, _("Tune"));
				btnTune->tooltip(_("Momentary Tune"));
				btnTune->callback((Fl_Callback*)cb_btnTune);

				bx_row0_expander = new Fl_Box(710, 63, 1, 20);
				Fl_Group::current()->resizable(bx_row0_expander);

				btn_show_controls = new Fl_Button(713, 63, 18, 18, _("@-22->"));
				btn_show_controls->tooltip(_("Show/Hide controls"));
				btn_show_controls->callback((Fl_Callback*)cb_btn_show_controls);

			grp_row0a->end();

		wd_grp3->end();
		Fl_Group::current()->resizable(FreqDispA);

		grp_row1 = new Fl_Group(0, 86, 732, 20);
			grp_row1->box(FL_FLAT_BOX);

			grp_row1a = new Fl_Group(0, 86, 211, 20);

				opBW = new Fl_ComboBox(3, 86, 100, 20, _("Bandwidth"));
				opBW->tooltip(_("Select Transceiver Bandwidth"));
				opBW->box(FL_FLAT_BOX);
				opBW->color(FL_BACKGROUND2_COLOR);
				opBW->selection_color(FL_BACKGROUND_COLOR);
				opBW->labeltype(FL_NORMAL_LABEL);
				opBW->labelfont(0);
				opBW->labelsize(12);
				opBW->labelcolor(FL_FOREGROUND_COLOR);
				opBW->callback((Fl_Callback*)cb_opBW);
				opBW->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
				opBW->when(FL_WHEN_RELEASE);
				opBW->end();

				btnDSP = new Fl_Button(3, 86, 18, 20, _("L"));
				btnDSP->callback((Fl_Callback*)cb_btnDSP);
				btnDSP->hide();

				opDSP_lo = new Fl_ComboBox(23, 86, 80, 20, _("Lo Cut"));
				opDSP_lo->tooltip(_("Lo Cut Freq"));
				opDSP_lo->box(FL_FLAT_BOX);
				opDSP_lo->color(FL_BACKGROUND2_COLOR);
				opDSP_lo->selection_color(FL_BACKGROUND_COLOR);
				opDSP_lo->labeltype(FL_NORMAL_LABEL);
				opDSP_lo->labelfont(0);
				opDSP_lo->labelsize(12);
				opDSP_lo->labelcolor(FL_FOREGROUND_COLOR);
				opDSP_lo->callback((Fl_Callback*)cb_opDSP_lo);
				opDSP_lo->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
				opDSP_lo->when(FL_WHEN_RELEASE);
				opDSP_lo->hide();
				opDSP_lo->end();

				opDSP_hi = new Fl_ComboBox(23, 86, 80, 20, _("Hi Cut"));
				opDSP_hi->tooltip(_("Hi Cut Freq"));
				opDSP_hi->box(FL_FLAT_BOX);
				opDSP_hi->color(FL_BACKGROUND2_COLOR);
				opDSP_hi->selection_color(FL_BACKGROUND_COLOR);
				opDSP_hi->labeltype(FL_NORMAL_LABEL);
				opDSP_hi->labelfont(0);
				opDSP_hi->labelsize(12);
				opDSP_hi->labelcolor(FL_FOREGROUND_COLOR);
				opDSP_hi->callback((Fl_Callback*)cb_opDSP_hi);
				opDSP_hi->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
				opDSP_hi->when(FL_WHEN_RELEASE);
				opDSP_hi->hide();
				opDSP_hi->end();

				opMODE = new Fl_ComboBox(109, 86, 100, 20, _("Mode"));
				opMODE->tooltip(_("Select transceiver operating mode"));
				opMODE->box(FL_FLAT_BOX);
				opMODE->color(FL_BACKGROUND2_COLOR);
				opMODE->selection_color(FL_BACKGROUND_COLOR);
				opMODE->labeltype(FL_NORMAL_LABEL);
				opMODE->labelfont(0);
				opMODE->labelsize(12);
				opMODE->labelcolor(FL_FOREGROUND_COLOR);
				opMODE->callback((Fl_Callback*)cb_opMODE);
				opMODE->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
				opMODE->when(FL_WHEN_RELEASE);
				opMODE->end();

			grp_row1a->end();

			grp_row1b = new Fl_Group(212, 86, 519, 20);
				grp_row1b1 = new Fl_Group(212, 86, 291, 20);

					Fl_Group* grp_row1b1a = new Fl_Group(212, 86, 144, 20);

						Fl_Box* bx_row1b1a = new Fl_Box(212, 86, 0, 20);
						bx_row1b1a->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
						Fl_Group::current()->resizable(bx_row1b1a);

						btnAttenuator = new Fl_Light_Button(212, 86, 70, 20, _("Att"));
						btnAttenuator->tooltip(_("Attenuator On/Off"));
						btnAttenuator->down_box(FL_THIN_DOWN_BOX);
						btnAttenuator->labelsize(12);
						btnAttenuator->callback((Fl_Callback*)cb_btnAttenuator);

						btnPreamp = new Fl_Light_Button(284, 86, 70, 20, _("Pre"));
						btnPreamp->tooltip(_("Preamp On/Off"));
						btnPreamp->down_box(FL_THIN_DOWN_BOX);
						btnPreamp->labelsize(12);
						btnPreamp->callback((Fl_Callback*)cb_btnPreamp);
					grp_row1b1a->end();

					Fl_Group* grp_row1b1b = new Fl_Group(357, 86, 146, 20);

						btnNOISE = new Fl_Light_Button(357, 86, 70, 20, _("NB"));
						btnNOISE->tooltip(_("Noise Blanker On/Off"));
						btnNOISE->down_box(FL_THIN_DOWN_BOX);
						btnNOISE->labelsize(12);
						btnNOISE->callback((Fl_Callback*)cb_btnNOISE);

						btnAutoNotch = new Fl_Light_Button(433, 86, 69, 20, _("AN"));
						btnAutoNotch->tooltip(_("Auto Notch On/Off"));
						btnAutoNotch->down_box(FL_THIN_DOWN_BOX);
						btnAutoNotch->labelsize(12);
						btnAutoNotch->callback((Fl_Callback*)cb_btnAutoNotch);

						Fl_Box* bx_row1b1b = new Fl_Box(503, 86, 0, 20);
						bx_row1b1b->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
						Fl_Group::current()->resizable(bx_row1b1b);

					grp_row1b1b->end();

				grp_row1b1->end();
				Fl_Group::current()->resizable(grp_row1b1);
    
				grp_row1b2 = new Fl_Group(504, 86, 227, 20);

					btnAswapB = new Fl_Button(504, 86, 65, 20, _("A / B"));
					btnAswapB->tooltip(_("Swap A B\nRight-click; Copy A to B"));
					btnAswapB->down_box(FL_DOWN_BOX);
					btnAswapB->labelsize(12);
					btnAswapB->callback((Fl_Callback*)cb_btnAswapB);

					btnSplit = new Fl_Light_Button(573, 86, 65, 20, _("Split"));
					btnSplit->tooltip(_("Rx-A / Tx-B"));
					btnSplit->down_box(FL_THIN_DOWN_BOX);
					btnSplit->labelsize(12);
					btnSplit->callback((Fl_Callback*)cb_btnSplit);
					btnSplit->value(progStatus.split);

					btnPTT = new Fl_Light_Button(643, 86, 65, 20, _("PTT"));
					btnPTT->tooltip(_("Xmt On/Off"));
					btnPTT->down_box(FL_THIN_DOWN_BOX);
					btnPTT->labelsize(12);
					btnPTT->callback((Fl_Callback*)cb_btnPTT);

					bx_row1_expander2 = new Fl_Box(710, 86, 1, 20);
					Fl_Group::current()->resizable(bx_row1_expander2);

				grp_row1b2->end();

			grp_row1b->end();

			Fl_Group::current()->resizable(grp_row1b);

		grp_row1->end();

		grp_row2 = new Fl_Group(0, 107, 734, 43);

			grp_row2a = new Fl_Group(0, 107, 183, 42);

				sldrSQUELCH = new Fl_Wheel_Value_Slider(54, 108, 128, 18, _("SQL"));
				sldrSQUELCH->tooltip(_("Adjust Squelch"));
				sldrSQUELCH->type(5);
				sldrSQUELCH->box(FL_THIN_DOWN_BOX);
				sldrSQUELCH->color(FL_BACKGROUND_COLOR);
				sldrSQUELCH->selection_color(FL_BACKGROUND_COLOR);
				sldrSQUELCH->labeltype(FL_NORMAL_LABEL);
				sldrSQUELCH->labelfont(0);
				sldrSQUELCH->labelsize(14);
				sldrSQUELCH->labelcolor(FL_FOREGROUND_COLOR);
				sldrSQUELCH->maximum(100);
				sldrSQUELCH->step(1);
				sldrSQUELCH->value(15);
				sldrSQUELCH->textsize(12);
				sldrSQUELCH->callback((Fl_Callback*)cb_sldrSQUELCH);
				sldrSQUELCH->align(Fl_Align(FL_ALIGN_LEFT));
				sldrSQUELCH->when(FL_WHEN_CHANGED);
				Fl_Group::current()->resizable(sldrSQUELCH);
				sldrSQUELCH->reverse(true);

				sldrMICGAIN = new Fl_Wheel_Value_Slider(54, 128, 128, 18, _("Mic"));
				sldrMICGAIN->tooltip(_("Adjust Mic Gain"));
				sldrMICGAIN->type(5);
				sldrMICGAIN->box(FL_THIN_DOWN_BOX);
				sldrMICGAIN->color(FL_BACKGROUND_COLOR);
				sldrMICGAIN->selection_color(FL_BACKGROUND_COLOR);
				sldrMICGAIN->labeltype(FL_NORMAL_LABEL);
				sldrMICGAIN->labelfont(0);
				sldrMICGAIN->labelsize(14);
				sldrMICGAIN->labelcolor(FL_FOREGROUND_COLOR);
				sldrMICGAIN->maximum(100);
				sldrMICGAIN->step(1);
				sldrMICGAIN->textsize(12);
				sldrMICGAIN->callback((Fl_Callback*)cb_sldrMICGAIN);
				sldrMICGAIN->align(Fl_Align(FL_ALIGN_LEFT));
				sldrMICGAIN->when(FL_WHEN_CHANGED);
				sldrMICGAIN->reverse(true);

			grp_row2a->end();

			bx_sep2a = new Fl_Box(184, 108, 2, 42);

			grp_row2b = new Fl_Group(186, 107, 180, 42);

				btnIFsh = new Fl_Light_Button(186, 108, 50, 18, _("IFsh"));
				btnIFsh->tooltip(_("IF Shift On/Off"));
				btnIFsh->down_box(FL_THIN_DOWN_BOX);
				btnIFsh->labelsize(12);
				btnIFsh->callback((Fl_Callback*)cb_btnIFsh);

				sldrIFSHIFT = new Fl_Wheel_Value_Slider(237, 108, 128, 18);
				sldrIFSHIFT->tooltip(_("Adjust IF Shift"));
				sldrIFSHIFT->type(5);
				sldrIFSHIFT->box(FL_THIN_DOWN_BOX);
				sldrIFSHIFT->color(FL_BACKGROUND_COLOR);
				sldrIFSHIFT->selection_color(FL_BACKGROUND_COLOR);
				sldrIFSHIFT->labeltype(FL_NORMAL_LABEL);
				sldrIFSHIFT->labelfont(0);
				sldrIFSHIFT->labelsize(12);
				sldrIFSHIFT->labelcolor(FL_FOREGROUND_COLOR);
				sldrIFSHIFT->minimum(-1000);
				sldrIFSHIFT->maximum(1000);
				sldrIFSHIFT->step(10);
				sldrIFSHIFT->textsize(12);
				sldrIFSHIFT->callback((Fl_Callback*)cb_sldrIFSHIFT);
				sldrIFSHIFT->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
				sldrIFSHIFT->when(FL_WHEN_CHANGED);
				sldrIFSHIFT->reverse(true);

				btnNR = new Fl_Light_Button(186, 128, 50, 18, _("NR"));
				btnNR->tooltip(_("Noise Reduction On/Off"));
				btnNR->down_box(FL_THIN_DOWN_BOX);
				btnNR->labelsize(12);
				btnNR->callback((Fl_Callback*)cb_btnNR);

				sldrNR = new Fl_Wheel_Value_Slider(237, 128, 128, 18);
				sldrNR->tooltip(_("Adjust noise reduction"));
				sldrNR->type(5);
				sldrNR->box(FL_THIN_DOWN_BOX);
				sldrNR->color(FL_BACKGROUND_COLOR);
				sldrNR->selection_color(FL_BACKGROUND_COLOR);
				sldrNR->labeltype(FL_NORMAL_LABEL);
				sldrNR->labelfont(0);
				sldrNR->labelsize(12);
				sldrNR->labelcolor(FL_FOREGROUND_COLOR);
				sldrNR->maximum(100);
				sldrNR->step(2);
				sldrNR->textsize(12);
				sldrNR->callback((Fl_Callback*)cb_sldrNR);
				sldrNR->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
				sldrNR->when(FL_WHEN_CHANGED);
				Fl_Group::current()->resizable(sldrNR);
				sldrNR->reverse(true);

			grp_row2b->end();

			bx_sep2b = new Fl_Box(367, 107, 2, 42);

			grp_row2c = new Fl_Group(370, 107, 180, 42);

				btnNotch = new Fl_Light_Button(370, 108, 50, 18, _("Nch"));
				btnNotch->tooltip(_("Notch - Manual (off) Auto (on)"));
				btnNotch->down_box(FL_THIN_DOWN_BOX);
				btnNotch->labelsize(12);
				btnNotch->callback((Fl_Callback*)cb_btnNotch);

				sldrNOTCH = new Fl_Wheel_Value_Slider(421, 108, 128, 18);
				sldrNOTCH->tooltip(_("Adjust Notch Frequency"));
				sldrNOTCH->type(5);
				sldrNOTCH->box(FL_THIN_DOWN_BOX);
				sldrNOTCH->color(FL_BACKGROUND_COLOR);
				sldrNOTCH->selection_color(FL_BACKGROUND_COLOR);
				sldrNOTCH->labeltype(FL_NORMAL_LABEL);
				sldrNOTCH->labelfont(0);
				sldrNOTCH->labelsize(14);
				sldrNOTCH->labelcolor(FL_FOREGROUND_COLOR);
				sldrNOTCH->minimum(-1000);
				sldrNOTCH->maximum(1000);
				sldrNOTCH->step(10);
				sldrNOTCH->textsize(12);
				sldrNOTCH->callback((Fl_Callback*)cb_sldrNOTCH);
				sldrNOTCH->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
				sldrNOTCH->when(FL_WHEN_CHANGED);
				sldrNOTCH->reverse(true);

				sldrRFGAIN = new Fl_Wheel_Value_Slider(421, 128, 128, 18, _("RF"));
				sldrRFGAIN->tooltip(_("Adjust RF gain"));
				sldrRFGAIN->type(5);
				sldrRFGAIN->box(FL_THIN_DOWN_BOX);
				sldrRFGAIN->color(FL_BACKGROUND_COLOR);
				sldrRFGAIN->selection_color(FL_BACKGROUND_COLOR);
				sldrRFGAIN->labeltype(FL_NORMAL_LABEL);
				sldrRFGAIN->labelfont(0);
				sldrRFGAIN->labelsize(14);
				sldrRFGAIN->labelcolor(FL_FOREGROUND_COLOR);
				sldrRFGAIN->maximum(100);
				sldrRFGAIN->step(1);
				sldrRFGAIN->value(15);
				sldrRFGAIN->textsize(12);
				sldrRFGAIN->callback((Fl_Callback*)cb_sldrRFGAIN);
				sldrRFGAIN->align(Fl_Align(FL_ALIGN_LEFT));
				sldrRFGAIN->when(FL_WHEN_CHANGED);
				Fl_Group::current()->resizable(sldrRFGAIN);
				sldrRFGAIN->reverse(true);
			grp_row2c->end();

			bx_sep2c = new Fl_Box(551, 107, 2, 42);

			grp_row2d = new Fl_Group(552, 107, 182, 42);

				sldrPOWER = new Fl_Wheel_Value_Slider(604, 108, 128, 18, _("PWR"));
				sldrPOWER->tooltip(_("Adjust power level"));
				sldrPOWER->type(5);
				sldrPOWER->box(FL_THIN_DOWN_BOX);
				sldrPOWER->color(FL_BACKGROUND_COLOR);
				sldrPOWER->selection_color(FL_BACKGROUND_COLOR);
				sldrPOWER->labeltype(FL_NORMAL_LABEL);
				sldrPOWER->labelfont(0);
				sldrPOWER->labelsize(14);
				sldrPOWER->labelcolor(FL_FOREGROUND_COLOR);
				sldrPOWER->maximum(100);
				sldrPOWER->step(1);
				sldrPOWER->value(15);
				sldrPOWER->textsize(12);
				sldrPOWER->callback((Fl_Callback*)cb_sldrPOWER);
				sldrPOWER->align(Fl_Align(FL_ALIGN_LEFT));
				sldrPOWER->when(FL_WHEN_CHANGED);
				sldrPOWER->reverse(true);

				btnVol = new Fl_Light_Button(552, 128, 50, 18, _("Vol"));
				btnVol->tooltip(_("Speaker on/off"));
				btnVol->down_box(FL_THIN_DOWN_BOX);
				btnVol->value(1);
				btnVol->labelsize(12);
				btnVol->callback((Fl_Callback*)cb_btnVol);

#ifdef __APPLE__
				sldrVOLUME = new Fl_Wheel_Value_Slider(604, 128, 110, 18);
				sldrVOLUME->tooltip(_("Adjust Audio Volume"));
				sldrVOLUME->type(5);
				sldrVOLUME->box(FL_THIN_DOWN_BOX);
				sldrVOLUME->color(FL_BACKGROUND_COLOR);
				sldrVOLUME->selection_color(FL_BACKGROUND_COLOR);
				sldrVOLUME->labeltype(FL_NORMAL_LABEL);
				sldrVOLUME->labelfont(0);
				sldrVOLUME->labelsize(12);
				sldrVOLUME->labelcolor(FL_FOREGROUND_COLOR);
				sldrVOLUME->maximum(100);
				sldrVOLUME->step(1);
				sldrVOLUME->textsize(12);
				sldrVOLUME->callback((Fl_Callback*)cb_sldrVOLUME);
				sldrVOLUME->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
				sldrVOLUME->when(FL_WHEN_CHANGED);
				Fl_Group::current()->resizable(sldrVOLUME);
				sldrVOLUME->reverse(true);

// box to keep right side of slider control from lying under the OS X
// dialog resize window decoration
				Fl_Box *volbox = new Fl_Box(714, 128, 18, 18);
				volbox->box(FL_FLAT_BOX);
#else
				sldrVOLUME = new Fl_Wheel_Value_Slider(604, 128, 128, 18);
				sldrVOLUME->tooltip(_("Adjust Audio Volume"));
				sldrVOLUME->type(5);
				sldrVOLUME->box(FL_THIN_DOWN_BOX);
				sldrVOLUME->color(FL_BACKGROUND_COLOR);
				sldrVOLUME->selection_color(FL_BACKGROUND_COLOR);
				sldrVOLUME->labeltype(FL_NORMAL_LABEL);
				sldrVOLUME->labelfont(0);
				sldrVOLUME->labelsize(12);
				sldrVOLUME->labelcolor(FL_FOREGROUND_COLOR);
				sldrVOLUME->maximum(100);
				sldrVOLUME->step(1);
				sldrVOLUME->textsize(12);
				sldrVOLUME->callback((Fl_Callback*)cb_sldrVOLUME);
				sldrVOLUME->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
				sldrVOLUME->when(FL_WHEN_CHANGED);
				Fl_Group::current()->resizable(sldrVOLUME);
				sldrVOLUME->reverse(true);
#endif
			grp_row2d->end();

		grp_row2->end();

	wd_grp1->end();

	grpTABS = new Fl_Group(0, 153, 732, 63);
	grpTABS->box(FL_FLAT_BOX);

		tabsGeneric = new Fl_Tabs(2, 158, 729, 56);
			tabsGeneric->selection_color(FL_LIGHT1);

			genericBands = new Fl_Group(2, 179, 730, 32, _("Bands"));

				btnBandSelect_1 = new Fl_Button(4, 188, 50, 18, _("1.8"));
				btnBandSelect_1->tooltip(_("160m Band"));
				btnBandSelect_1->color((Fl_Color)246);
				btnBandSelect_1->callback((Fl_Callback*)cb_btnBandSelect_1);

				btnBandSelect_2 = new Fl_Button(62, 188, 50, 18, _("3.5"));
				btnBandSelect_2->tooltip(_("80m band"));
				btnBandSelect_2->color((Fl_Color)246);
				btnBandSelect_2->callback((Fl_Callback*)cb_btnBandSelect_2);

				btnBandSelect_3 = new Fl_Button(120, 188, 50, 18, _("7"));
				btnBandSelect_3->tooltip(_("40m band"));
				btnBandSelect_3->color((Fl_Color)246);
				btnBandSelect_3->callback((Fl_Callback*)cb_btnBandSelect_3);
				btnBandSelect_3->align(Fl_Align(FL_ALIGN_CLIP));

				btnBandSelect_4 = new Fl_Button(178, 188, 50, 18, _("10"));
				btnBandSelect_4->tooltip(_("30m band"));
				btnBandSelect_4->color((Fl_Color)246);
				btnBandSelect_4->callback((Fl_Callback*)cb_btnBandSelect_4);

				btnBandSelect_5 = new Fl_Button(237, 188, 50, 18, _("14"));
				btnBandSelect_5->tooltip(_("20m band"));
				btnBandSelect_5->color((Fl_Color)246);
				btnBandSelect_5->callback((Fl_Callback*)cb_btnBandSelect_5);

				btnBandSelect_6 = new Fl_Button(295, 188, 50, 18, _("18"));
				btnBandSelect_6->tooltip(_("17m band"));
				btnBandSelect_6->color((Fl_Color)246);
				btnBandSelect_6->callback((Fl_Callback*)cb_btnBandSelect_6);

				btnBandSelect_7 = new Fl_Button(353, 188, 50, 18, _("21"));
				btnBandSelect_7->tooltip(_("15m band"));
				btnBandSelect_7->color((Fl_Color)246);
				btnBandSelect_7->callback((Fl_Callback*)cb_btnBandSelect_7);

				btnBandSelect_8 = new Fl_Button(412, 188, 50, 18, _("24"));
				btnBandSelect_8->tooltip(_("12m band"));
				btnBandSelect_8->color((Fl_Color)246);
				btnBandSelect_8->callback((Fl_Callback*)cb_btnBandSelect_8);

				btnBandSelect_9 = new Fl_Button(470, 188, 50, 18, _("28"));
				btnBandSelect_9->tooltip(_("10m band"));
				btnBandSelect_9->color((Fl_Color)246);
				btnBandSelect_9->callback((Fl_Callback*)cb_btnBandSelect_9);

				btnBandSelect_10 = new Fl_Button(528, 188, 50, 18, _("50"));
				btnBandSelect_10->tooltip(_("6m band"));
				btnBandSelect_10->color((Fl_Color)246);
				btnBandSelect_10->callback((Fl_Callback*)cb_btnBandSelect_10);

				btnBandSelect_11 = new Fl_Button(587, 188, 50, 18, _("GEN"));
				btnBandSelect_11->tooltip(_("General RX"));
				btnBandSelect_11->color((Fl_Color)246);
				btnBandSelect_11->callback((Fl_Callback*)cb_btnBandSelect_11);

				opSelect60 = new Fl_ComboBox(653, 188, 60, 18, _("combo"));
				opSelect60->tooltip(_("vfo / 60 meter channel"));
				opSelect60->box(FL_NO_BOX);
				opSelect60->color((Fl_Color)246);
				opSelect60->selection_color(FL_BACKGROUND_COLOR);
				opSelect60->labeltype(FL_NORMAL_LABEL);
				opSelect60->labelfont(0);
				opSelect60->labelsize(12);
				opSelect60->labelcolor(FL_FOREGROUND_COLOR);
				opSelect60->callback((Fl_Callback*)cb_opSelect60);
				opSelect60->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
				opSelect60->when(FL_WHEN_RELEASE);
				opSelect60->end();

			genericBands->end();

			genericCW = new Fl_Group(2, 179, 730, 32, _("CW"));
				genericCW->color(FL_LIGHT1);
				genericCW->selection_color(FL_LIGHT1);
				genericCW->hide();

				btnSpot = new Fl_Light_Button(13, 187, 54, 20, _("Spot"));
				btnSpot->tooltip(_("Spot tone on/off"));
				btnSpot->callback((Fl_Callback*)cb_btnSpot);
				btnSpot->value(progStatus.cw_spot);

				cnt_cw_spot_tone = new Fl_Counter(164, 187, 70, 20, _("Spot tone"));
				cnt_cw_spot_tone->tooltip(_("Spot volume"));
				cnt_cw_spot_tone->type(1);
				cnt_cw_spot_tone->minimum(300);
				cnt_cw_spot_tone->maximum(1050);
				cnt_cw_spot_tone->step(50);
				cnt_cw_spot_tone->value(1050);
				cnt_cw_spot_tone->callback((Fl_Callback*)cb_cnt_cw_spot_tone);
				cnt_cw_spot_tone->align(Fl_Align(FL_ALIGN_LEFT));
				cnt_cw_spot_tone->value(progStatus.cw_spot);

				cnt_cw_qsk = new Fl_Counter(299, 187, 70, 20, _("QSK dly"));
				cnt_cw_qsk->tooltip(_("QSK delay (msec)"));
				cnt_cw_qsk->type(1);
				cnt_cw_qsk->minimum(0);
				cnt_cw_qsk->maximum(100);
				cnt_cw_qsk->step(1);
				cnt_cw_qsk->value(20);
				cnt_cw_qsk->callback((Fl_Callback*)cb_cnt_cw_qsk);
				cnt_cw_qsk->align(Fl_Align(FL_ALIGN_LEFT));
				cnt_cw_qsk->value(progStatus.cw_qsk);

				cnt_cw_weight = new Fl_Counter(435, 187, 70, 20, _("Weight"));
				cnt_cw_weight->type(1);
				cnt_cw_weight->minimum(2.5);
				cnt_cw_weight->maximum(4.5);
				cnt_cw_weight->value(3);
				cnt_cw_weight->callback((Fl_Callback*)cb_cnt_cw_weight);
				cnt_cw_weight->align(Fl_Align(FL_ALIGN_LEFT));
				cnt_cw_weight->value(progStatus.cw_weight);

				cnt_cw_wpm = new Fl_Counter(571, 187, 70, 20, _("wpm"));
				cnt_cw_wpm->type(1);
				cnt_cw_wpm->minimum(5);
				cnt_cw_wpm->maximum(80);
				cnt_cw_wpm->step(1);
				cnt_cw_wpm->value(24);
				cnt_cw_wpm->callback((Fl_Callback*)cb_cnt_cw_wpm);
				cnt_cw_wpm->align(Fl_Align(FL_ALIGN_LEFT));
				cnt_cw_wpm->value(progStatus.cw_wpm);

				btn_enable_keyer = new Fl_Check_Button(649, 190, 60, 15, _("Keyer"));
				btn_enable_keyer->tooltip(_("Enable internal keyer"));
				btn_enable_keyer->down_box(FL_DOWN_BOX);
				btn_enable_keyer->callback((Fl_Callback*)cb_btn_enable_keyer);
				btn_enable_keyer->value(progStatus.enable_keyer);

			genericCW->end();

			genericVOX = new Fl_Group(2, 179, 730, 32, _("Vox"));
				genericVOX->hide();

				cnt_vox_gain = new Fl_Counter(129, 187, 70, 20, _("gain"));
				cnt_vox_gain->type(1);
				cnt_vox_gain->minimum(0);
				cnt_vox_gain->maximum(100);
				cnt_vox_gain->step(1);
				cnt_vox_gain->callback((Fl_Callback*)cb_cnt_vox_gain);
				cnt_vox_gain->align(Fl_Align(FL_ALIGN_LEFT));
				cnt_vox_gain->value(progStatus.vox_gain);

				cnt_anti_vox = new Fl_Counter(262, 187, 70, 20, _("anti"));
				cnt_anti_vox->type(1);
				cnt_anti_vox->minimum(0);
				cnt_anti_vox->maximum(100);
				cnt_anti_vox->step(1);
				cnt_anti_vox->callback((Fl_Callback*)cb_cnt_anti_vox);
				cnt_anti_vox->align(Fl_Align(FL_ALIGN_LEFT));
				cnt_anti_vox->value(progStatus.vox_anti);

				cnt_vox_hang = new Fl_Counter(396, 187, 70, 20, _("hang"));
				cnt_vox_hang->type(1);
				cnt_vox_hang->minimum(0);
				cnt_vox_hang->maximum(100);
				cnt_vox_hang->step(1);
				cnt_vox_hang->callback((Fl_Callback*)cb_cnt_vox_hang);
				cnt_vox_hang->align(Fl_Align(FL_ALIGN_LEFT));
				cnt_vox_hang->value(progStatus.vox_hang);

				btn_vox = new Fl_Light_Button(525, 187, 70, 20, _("VOX"));
				btn_vox->callback((Fl_Callback*)cb_btn_vox);
				btn_vox->value(progStatus.vox_onoff);

				btn_vox_on_dataport = new Fl_Check_Button(610, 189, 70, 15, _("Data port"));
				btn_vox_on_dataport->tooltip(_("Data port signal triggers VOX"));
				btn_vox_on_dataport->down_box(FL_DOWN_BOX);
				btn_vox_on_dataport->callback((Fl_Callback*)cb_btn_vox_on_dataport);
				btn_vox_on_dataport->value(progStatus.vox_on_dataport);

			genericVOX->end();


			genericSpeech = new Fl_Group(2, 179, 730, 32, _("Spch"));
				genericSpeech->hide();

				btnCompON = new Fl_Light_Button(230, 187, 70, 20, _("On"));
				btnCompON->callback((Fl_Callback*)cb_btnCompON);
				btnCompON->value(progStatus.compON);

				cnt_compression = new Fl_Counter(138, 187, 70, 20, _("Comp"));
				cnt_compression->type(1);
				cnt_compression->minimum(0);
				cnt_compression->maximum(100);
				cnt_compression->step(1);
				cnt_compression->callback((Fl_Callback*)cb_cnt_compression);
				cnt_compression->align(Fl_Align(FL_ALIGN_LEFT));
				cnt_compression->value(progStatus.compression);

			genericSpeech->end();

			genericRx = new Fl_Group(2, 179, 730, 32, _("RX"));
				genericRx->hide();

				cbo_nb_level = new Fl_ComboBox(90, 187, 80, 20, _("NB level"));
				cbo_nb_level->tooltip(_("Select Transceiver"));
				cbo_nb_level->box(FL_DOWN_BOX);
				cbo_nb_level->color(FL_BACKGROUND_COLOR);
				cbo_nb_level->selection_color(FL_BACKGROUND_COLOR);
				cbo_nb_level->labeltype(FL_NORMAL_LABEL);
				cbo_nb_level->labelfont(0);
				cbo_nb_level->labelsize(14);
				cbo_nb_level->labelcolor(FL_FOREGROUND_COLOR);
				cbo_nb_level->callback((Fl_Callback*)cb_cbo_nb_level);
				cbo_nb_level->align(Fl_Align(FL_ALIGN_LEFT));
				cbo_nb_level->when(FL_WHEN_RELEASE);
				cbo_nb_level->index(progStatus.nb_level);
				cbo_nb_level->end();

				cbo_agc_level = new Fl_ComboBox(244, 187, 80, 20, _("AGC"));
				cbo_agc_level->tooltip(_("Select Transceiver"));
				cbo_agc_level->box(FL_DOWN_BOX);
				cbo_agc_level->color(FL_BACKGROUND_COLOR);
				cbo_agc_level->selection_color(FL_BACKGROUND_COLOR);
				cbo_agc_level->labeltype(FL_NORMAL_LABEL);
				cbo_agc_level->labelfont(0);
				cbo_agc_level->labelsize(14);
				cbo_agc_level->labelcolor(FL_FOREGROUND_COLOR);
				cbo_agc_level->callback((Fl_Callback*)cb_cbo_agc_level);
				cbo_agc_level->align(Fl_Align(FL_ALIGN_LEFT));
				cbo_agc_level->when(FL_WHEN_RELEASE);
				cbo_agc_level->end();

				cnt_bpf_center = new Fl_Counter(474, 187, 96, 20, _("BPF Center Freq"));
				cnt_bpf_center->tooltip(_("Bandpass Filter Center Freq"));
				cnt_bpf_center->type(1);
				cnt_bpf_center->minimum(600);
				cnt_bpf_center->maximum(2500);
				cnt_bpf_center->step(10);
				cnt_bpf_center->value(1500);
				cnt_bpf_center->callback((Fl_Callback*)cb_cnt_bpf_center);
				cnt_bpf_center->align(Fl_Align(36));
				cnt_bpf_center->value(progStatus.bpf_center);

				btn_use_bpf_center = new Fl_Check_Button(627, 189, 50, 15, _("On"));
				btn_use_bpf_center->tooltip(_("Use Filter Center Freq Adj"));
				btn_use_bpf_center->down_box(FL_DOWN_BOX);
				btn_use_bpf_center->value(1);
				btn_use_bpf_center->callback((Fl_Callback*)cb_btn_use_bpf_center);
				btn_use_bpf_center->value(progStatus.use_bpf_center);

			genericRx->end();

			genericMisc = new Fl_Group(2, 179, 730, 32, _("Misc"));
				genericMisc->hide();

				cnt_vfo_adj = new Fl_Counter(104, 187, 96, 20, _("Vfo Adj"));
				cnt_vfo_adj->type(1);
				cnt_vfo_adj->callback((Fl_Callback*)cb_cnt_vfo_adj);
				cnt_vfo_adj->align(Fl_Align(36));
				cnt_vfo_adj->value(progStatus.vfo_adj);

				cnt_line_out = new Fl_Counter(273, 187, 70, 20, _("line out"));
				cnt_line_out->type(1);
				cnt_line_out->minimum(0);
				cnt_line_out->maximum(100);
				cnt_line_out->step(1);
				cnt_line_out->value(20);
				cnt_line_out->callback((Fl_Callback*)cb_cnt_line_out);
				cnt_line_out->align(Fl_Align(FL_ALIGN_LEFT));
				cnt_line_out->value(progStatus.line_out);

				btnSpecial = new Fl_Light_Button(364, 187, 74, 20, _("Special"));
				btnSpecial->callback((Fl_Callback*)cb_btnSpecial);

				btn_xcvr_auto_on = new Fl_Check_Button(470, 189, 95, 15, _("Rig autOn"));
				btn_xcvr_auto_on->tooltip(_("Auto Turn Rig On with Flrig startup"));
				btn_xcvr_auto_on->down_box(FL_DOWN_BOX);
				btn_xcvr_auto_on->callback((Fl_Callback*)cb_btn_xcvr_auto_on);
				btn_xcvr_auto_on->value(progStatus.xcvr_auto_on);

				btn_xcvr_auto_off = new Fl_Check_Button(607, 189, 95, 15, _("Rig autOff"));
				btn_xcvr_auto_off->tooltip(_("Auto Turn Rig Off with Flrig exit"));
				btn_xcvr_auto_off->down_box(FL_DOWN_BOX);
				btn_xcvr_auto_off->callback((Fl_Callback*)cb_btn_xcvr_auto_off);
				btn_xcvr_auto_off->value(progStatus.xcvr_auto_off);

			genericMisc->end();

			genericRXB = new Fl_Group(2, 179, 730, 32, _("RIT/XIT/BFO"));
				genericRXB->hide();

				cntRIT = new Fl_Counter(218, 187, 60, 20, _("Rit"));
				cntRIT->tooltip(_("RIT"));
				cntRIT->type(1);
				cntRIT->labelsize(12);
				cntRIT->minimum(-1000);
				cntRIT->maximum(1000);
				cntRIT->step(10);
				cntRIT->callback((Fl_Callback*)cb_cntRIT);
				cntRIT->align(Fl_Align(FL_ALIGN_RIGHT));

				cntXIT = new Fl_Counter(366, 187, 60, 20, _("Xit"));
				cntXIT->tooltip(_("XIT"));
				cntXIT->type(1);
				cntXIT->labelsize(12);
				cntXIT->minimum(-1000);
				cntXIT->maximum(1000);
				cntXIT->step(10);
				cntXIT->callback((Fl_Callback*)cb_cntXIT);
				cntXIT->align(Fl_Align(FL_ALIGN_RIGHT));

				cntBFO = new Fl_Counter(514, 187, 54, 20, _("Bfo"));
				cntBFO->tooltip(_("BFO"));
				cntBFO->type(1);
				cntBFO->labelsize(12);
				cntBFO->minimum(0);
				cntBFO->maximum(1000);
				cntBFO->step(10);
				cntBFO->value(600);
				cntBFO->callback((Fl_Callback*)cb_cntBFO);
				cntBFO->align(Fl_Align(FL_ALIGN_RIGHT));

			genericRXB->end();

			genericAux = new Fl_Group(2, 179, 730, 32, _("Aux"));
				genericAux->hide();

				boxControl = new Fl_Box(57, 187, 130, 20, _("Auxilliary Controls:"));
				boxControl->box(FL_FLAT_BOX);
				boxControl->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));

				btnAuxRTS = new Fl_Light_Button(272, 187, 60, 20, _("RTS"));
				btnAuxRTS->tooltip(_("RTS control pin"));
				btnAuxRTS->down_box(FL_THIN_DOWN_BOX);
				btnAuxRTS->labelsize(12);
				btnAuxRTS->callback((Fl_Callback*)cb_btnAuxRTS);
				btnAuxRTS->value(progStatus.aux_rts);

				btnAuxDTR = new Fl_Light_Button(200, 187, 60, 20, _("DTR"));
				btnAuxDTR->tooltip(_("DTR control pin"));
				btnAuxDTR->down_box(FL_THIN_DOWN_BOX);
				btnAuxDTR->labelsize(12);
				btnAuxDTR->callback((Fl_Callback*)cb_btnAuxDTR);
				btnAuxDTR->value(progStatus.aux_dtr);

				btnDataPort = new Fl_Light_Button(408, 187, 60, 20, _("Data"));
				btnDataPort->tooltip(_("Input on Data Port"));
				btnDataPort->down_box(FL_THIN_DOWN_BOX);
				btnDataPort->labelsize(12);
				btnDataPort->callback((Fl_Callback*)cb_btnDataPort);
				btnDataPort->value(progStatus.data_port);

			genericAux->end();

		tabsGeneric->end();

		tabs550 = new Fl_Tabs(2, 158, 730, 54);
			tabs550->selection_color(FL_LIGHT1);
			tabs550->hide();

			tt550_CW = new Fl_Group(2, 179, 730, 32, _("CW"));
				tt550_CW->color(FL_LIGHT1);
				tt550_CW->selection_color(FL_LIGHT1);

				cnt_tt550_cw_wpm = new Fl_Counter(55, 187, 67, 20, _("wpm"));
				cnt_tt550_cw_wpm->type(1);
				cnt_tt550_cw_wpm->minimum(5);
				cnt_tt550_cw_wpm->maximum(80);
				cnt_tt550_cw_wpm->step(1);
				cnt_tt550_cw_wpm->value(20);
				cnt_tt550_cw_wpm->callback((Fl_Callback*)cb_cnt_tt550_cw_wpm);
				cnt_tt550_cw_wpm->align(Fl_Align(FL_ALIGN_LEFT));
				cnt_tt550_cw_wpm->value(progStatus.tt550_cw_wpm);

				cnt_tt550_cw_weight = new Fl_Counter(187, 187, 67, 20, _("Weight"));
				cnt_tt550_cw_weight->type(1);
				cnt_tt550_cw_weight->minimum(0.75);
				cnt_tt550_cw_weight->maximum(1.5);
				cnt_tt550_cw_weight->step(0.05);
				cnt_tt550_cw_weight->value(20);
				cnt_tt550_cw_weight->callback((Fl_Callback*)cb_cnt_tt550_cw_weight);
				cnt_tt550_cw_weight->align(Fl_Align(FL_ALIGN_LEFT));
				cnt_tt550_cw_weight->value(progStatus.tt550_cw_weight);

				cnt_tt550_cw_qsk = new Fl_Counter(311, 187, 67, 20, _("Delay"));
				cnt_tt550_cw_qsk->tooltip(_("QSK delay (msec)"));
				cnt_tt550_cw_qsk->type(1);
				cnt_tt550_cw_qsk->minimum(0);
				cnt_tt550_cw_qsk->maximum(100);
				cnt_tt550_cw_qsk->step(1);
				cnt_tt550_cw_qsk->value(20);
				cnt_tt550_cw_qsk->callback((Fl_Callback*)cb_cnt_tt550_cw_qsk);
				cnt_tt550_cw_qsk->align(Fl_Align(FL_ALIGN_LEFT));
				cnt_tt550_cw_qsk->value(progStatus.tt550_cw_qsk);

				cnt_tt550_cw_vol = new Fl_Counter(447, 187, 67, 20, _("S-T vol"));
				cnt_tt550_cw_vol->tooltip(_("Side tone volume"));
				cnt_tt550_cw_vol->type(1);
				cnt_tt550_cw_vol->minimum(5);
				cnt_tt550_cw_vol->maximum(80);
				cnt_tt550_cw_vol->step(1);
				cnt_tt550_cw_vol->value(20);
				cnt_tt550_cw_vol->callback((Fl_Callback*)cb_cnt_tt550_cw_vol);
				cnt_tt550_cw_vol->align(Fl_Align(FL_ALIGN_LEFT));
				cnt_tt550_cw_vol->value(progStatus.tt550_cw_vol);

				cnt_tt550_cw_spot = new Fl_Counter(590, 187, 67, 20, _("Spot Vol"));
				cnt_tt550_cw_spot->tooltip(_("Spot volume"));
				cnt_tt550_cw_spot->type(1);
				cnt_tt550_cw_spot->minimum(0);
				cnt_tt550_cw_spot->maximum(100);
				cnt_tt550_cw_spot->step(1);
				cnt_tt550_cw_spot->value(20);
				cnt_tt550_cw_spot->callback((Fl_Callback*)cb_cnt_tt550_cw_spot);
				cnt_tt550_cw_spot->align(Fl_Align(FL_ALIGN_LEFT));
				cnt_tt550_cw_spot->value(progStatus.tt550_cw_spot);

				btn_tt550_enable_keyer = new Fl_Check_Button(662, 189, 63, 16, _("Keyer"));
				btn_tt550_enable_keyer->tooltip(_("Enable keyer"));
				btn_tt550_enable_keyer->down_box(FL_DOWN_BOX);
				btn_tt550_enable_keyer->callback((Fl_Callback*)cb_btn_tt550_enable_keyer);
				btn_tt550_enable_keyer->value(progStatus.tt550_enable_keyer);

			tt550_CW->end();

			tt550_VOX = new Fl_Group(2, 179, 726, 32, _("Vox"));
				tt550_VOX->hide();

				cnt_tt550_vox_gain = new Fl_Counter(101, 187, 79, 20, _("gain"));
				cnt_tt550_vox_gain->type(1);
				cnt_tt550_vox_gain->minimum(0);
				cnt_tt550_vox_gain->maximum(100);
				cnt_tt550_vox_gain->step(1);
				cnt_tt550_vox_gain->callback((Fl_Callback*)cb_cnt_tt550_vox_gain);
				cnt_tt550_vox_gain->align(Fl_Align(FL_ALIGN_LEFT));
				cnt_tt550_vox_gain->value(progStatus.tt550_vox_gain);

				cnt_tt550_anti_vox = new Fl_Counter(299, 187, 78, 20, _("anti"));
				cnt_tt550_anti_vox->type(1);
				cnt_tt550_anti_vox->minimum(0);
				cnt_tt550_anti_vox->maximum(100);
				cnt_tt550_anti_vox->step(1);
				cnt_tt550_anti_vox->callback((Fl_Callback*)cb_cnt_tt550_anti_vox);
				cnt_tt550_anti_vox->align(Fl_Align(FL_ALIGN_LEFT));
				cnt_tt550_anti_vox->value(progStatus.tt550_vox_anti);

				cnt_tt550_vox_hang = new Fl_Counter(497, 187, 78, 20, _("hang"));
				cnt_tt550_vox_hang->type(1);
				cnt_tt550_vox_hang->minimum(0);
				cnt_tt550_vox_hang->maximum(100);
				cnt_tt550_vox_hang->step(1);
				cnt_tt550_vox_hang->callback((Fl_Callback*)cb_cnt_tt550_vox_hang);
				cnt_tt550_vox_hang->align(Fl_Align(FL_ALIGN_LEFT));
				cnt_tt550_vox_hang->value(progStatus.tt550_vox_hang);

				btn_tt550_vox = new Fl_Light_Button(633, 187, 78, 20, _("VOX"));
				btn_tt550_vox->callback((Fl_Callback*)cb_btn_tt550_vox);
				btn_tt550_vox->value(progStatus.tt550_vox_onoff);

			tt550_VOX->end();

			tt550_Speech = new Fl_Group(2, 179, 730, 32, _("Spch"));
				tt550_Speech->hide();

				btn_tt550_CompON = new Fl_Light_Button(559, 187, 78, 20, _("On"));
				btn_tt550_CompON->callback((Fl_Callback*)cb_btn_tt550_CompON);
				btn_tt550_CompON->value(progStatus.tt550_compON);

				cnt_tt550_compression = new Fl_Counter(422, 187, 78, 20, _("Comp"));
				cnt_tt550_compression->type(1);
				cnt_tt550_compression->minimum(0);
				cnt_tt550_compression->maximum(100);
				cnt_tt550_compression->step(1);
				cnt_tt550_compression->callback((Fl_Callback*)cb_cnt_tt550_compression);
				cnt_tt550_compression->align(Fl_Align(FL_ALIGN_LEFT));
				cnt_tt550_compression->value(progStatus.tt550_compression);

				cnt_tt550_mon_vol = new Fl_Counter(247, 187, 78, 20, _("Mon vol"));
				cnt_tt550_mon_vol->tooltip(_("Side tone volume"));
				cnt_tt550_mon_vol->type(1);
				cnt_tt550_mon_vol->minimum(0);
				cnt_tt550_mon_vol->maximum(100);
				cnt_tt550_mon_vol->step(1);
				cnt_tt550_mon_vol->value(25);
				cnt_tt550_mon_vol->callback((Fl_Callback*)cb_cnt_tt550_mon_vol);
				cnt_tt550_mon_vol->align(Fl_Align(FL_ALIGN_LEFT));
				cnt_tt550_mon_vol->value(progStatus.tt550_mon_vol);

			tt550_Speech->end();

			tt550_Audio = new Fl_Group(2, 179, 730, 32, _("Audio"));
				tt550_Audio->hide();

				cnt_tt550_line_out = new Fl_Counter(296, 187, 77, 20, _("line out"));
				cnt_tt550_line_out->type(1);
				cnt_tt550_line_out->minimum(0);
				cnt_tt550_line_out->maximum(100);
				cnt_tt550_line_out->step(1);
				cnt_tt550_line_out->value(20);
				cnt_tt550_line_out->callback((Fl_Callback*)cb_cnt_tt550_line_out);
				cnt_tt550_line_out->align(Fl_Align(FL_ALIGN_LEFT));
				cnt_tt550_line_out->value(progStatus.tt550_line_out);

				btnAccInp = new Fl_Check_Button(455, 187, 92, 16, _("Acc Inp."));
				btnAccInp->tooltip(_("Enable to use accessory audio input"));
				btnAccInp->down_box(FL_DOWN_BOX);
				btnAccInp->callback((Fl_Callback*)cb_btnAccInp);
				btnAccInp->value(progStatus.tt550_use_line_in);

			tt550_Audio->end();

			tt550_Rx = new Fl_Group(2, 179, 730, 32, _("RX"));
				tt550_Rx->hide();

				cbo_tt550_nb_level = new Fl_ComboBox(179, 187, 89, 20, _("NB level"));
				cbo_tt550_nb_level->tooltip(_("Select Transceiver"));
				cbo_tt550_nb_level->box(FL_DOWN_BOX);
				cbo_tt550_nb_level->color(FL_BACKGROUND_COLOR);
				cbo_tt550_nb_level->selection_color(FL_BACKGROUND_COLOR);
				cbo_tt550_nb_level->labeltype(FL_NORMAL_LABEL);
				cbo_tt550_nb_level->labelfont(0);
				cbo_tt550_nb_level->labelsize(14);
				cbo_tt550_nb_level->labelcolor(FL_FOREGROUND_COLOR);
				cbo_tt550_nb_level->callback((Fl_Callback*)cb_cbo_tt550_nb_level);
				cbo_tt550_nb_level->align(Fl_Align(FL_ALIGN_LEFT));
				cbo_tt550_nb_level->when(FL_WHEN_RELEASE);
				cbo_tt550_nb_level->index(progStatus.tt550_nb_level);
				cbo_tt550_nb_level->end();

				cbo_tt550_agc_level = new Fl_ComboBox(340, 187, 90, 20, _("AGC"));
				cbo_tt550_agc_level->tooltip(_("Select Transceiver"));
				cbo_tt550_agc_level->box(FL_DOWN_BOX);
				cbo_tt550_agc_level->color(FL_BACKGROUND_COLOR);
				cbo_tt550_agc_level->selection_color(FL_BACKGROUND_COLOR);
				cbo_tt550_agc_level->labeltype(FL_NORMAL_LABEL);
				cbo_tt550_agc_level->labelfont(0);
				cbo_tt550_agc_level->labelsize(14);
				cbo_tt550_agc_level->labelcolor(FL_FOREGROUND_COLOR);
				cbo_tt550_agc_level->callback((Fl_Callback*)cb_cbo_tt550_agc_level);
				cbo_tt550_agc_level->align(Fl_Align(FL_ALIGN_LEFT));
				cbo_tt550_agc_level->when(FL_WHEN_RELEASE);
				cbo_tt550_agc_level->index(progStatus.tt550_agc_level);
				cbo_tt550_agc_level->end();

				cnt_tt550_vfo_adj = new Fl_Counter(556, 187, 134, 20, _("Vfo Adj(ppm)"));
				cnt_tt550_vfo_adj->callback((Fl_Callback*)cb_cnt_tt550_vfo_adj);
				cnt_tt550_vfo_adj->align(Fl_Align(36));
				cnt_tt550_vfo_adj->value(progStatus.vfo_adj);
				cnt_tt550_vfo_adj->lstep(1.0);

			tt550_Rx->end();

			tt550_Tx = new Fl_Group(2, 179, 730, 32, _("TX"));
				tt550_Tx->hide();

				btn_tt550_enable_xmtr = new Fl_Check_Button(111, 187, 78, 16, _("Tx ON"));
				btn_tt550_enable_xmtr->down_box(FL_DOWN_BOX);
				btn_tt550_enable_xmtr->value(1);
				btn_tt550_enable_xmtr->callback((Fl_Callback*)cb_btn_tt550_enable_xmtr);
				btn_tt550_enable_xmtr->value(progStatus.tt550_enable_xmtr);

				btn_tt550_enable_tloop = new Fl_Check_Button(197, 187, 78, 16, _("Tloop"));
				btn_tt550_enable_tloop->down_box(FL_DOWN_BOX);
				btn_tt550_enable_tloop->callback((Fl_Callback*)cb_btn_tt550_enable_tloop);
				btn_tt550_enable_tloop->value(progStatus.tt550_enable_tloop);

				btn_tt550_tuner_bypass = new Fl_Check_Button(283, 187, 78, 16, _("Tuner"));
				btn_tt550_tuner_bypass->tooltip(_("Check to bypass tuner"));
				btn_tt550_tuner_bypass->down_box(FL_DOWN_BOX);
				btn_tt550_tuner_bypass->callback((Fl_Callback*)cb_btn_tt550_tuner_bypass);

				op_tt550_XmtBW = new Fl_ComboBox(462, 187, 112, 20, _("Xmt BW"));
				op_tt550_XmtBW->tooltip(_("Select Transceiver Bandwidth"));
				op_tt550_XmtBW->box(FL_DOWN_BOX);
				op_tt550_XmtBW->color(FL_BACKGROUND_COLOR);
				op_tt550_XmtBW->selection_color(FL_BACKGROUND_COLOR);
				op_tt550_XmtBW->labeltype(FL_NORMAL_LABEL);
				op_tt550_XmtBW->labelfont(0);
				op_tt550_XmtBW->labelsize(12);
				op_tt550_XmtBW->labelcolor(FL_FOREGROUND_COLOR);
				op_tt550_XmtBW->callback((Fl_Callback*)cb_op_tt550_XmtBW);
				op_tt550_XmtBW->align(Fl_Align(FL_ALIGN_LEFT));
				op_tt550_XmtBW->when(FL_WHEN_RELEASE);
				op_tt550_XmtBW->index(progStatus.tt550_xmt_bw);
				op_tt550_XmtBW->end();

				btn_tt550_use_xmt_bw = new Fl_Check_Button(613, 187, 23, 16, _("Enable"));
				btn_tt550_use_xmt_bw->tooltip(_("Use Xmt BW setting"));
				btn_tt550_use_xmt_bw->down_box(FL_DOWN_BOX);
				btn_tt550_use_xmt_bw->callback((Fl_Callback*)cb_btn_tt550_use_xmt_bw);
				btn_tt550_use_xmt_bw->align(Fl_Align(FL_ALIGN_RIGHT));
				btn_tt550_use_xmt_bw->value(progStatus.tt550_use_xmt_bw);

			tt550_Tx->end();

			tt550_302A = new Fl_Group(2, 179, 730, 32, _("302-A"));
				tt550_302A->hide();

				cnt_tt550_keypad_time_out = new Fl_Counter(106, 187, 134, 20, _("Time out"));
				cnt_tt550_keypad_time_out->tooltip(_("Clear M302 keypad after ## secs"));
				cnt_tt550_keypad_time_out->minimum(0.5);
				cnt_tt550_keypad_time_out->maximum(10);
				cnt_tt550_keypad_time_out->value(2);
				cnt_tt550_keypad_time_out->callback((Fl_Callback*)cb_cnt_tt550_keypad_time_out);
				cnt_tt550_keypad_time_out->align(Fl_Align(36));
				cnt_tt550_keypad_time_out->value(progStatus.tt550_keypad_timeout/1000.0);
				cnt_tt550_keypad_time_out->step(0.1); 
				cnt_tt550_keypad_time_out->lstep(1.0);

				cnt_tt550_encoder_sensitivity = new Fl_Counter(354, 187, 89, 20, _("Sensitivity"));
				cnt_tt550_encoder_sensitivity->tooltip(_("1 - most sensitive"));
				cnt_tt550_encoder_sensitivity->type(1);
				cnt_tt550_encoder_sensitivity->minimum(1);
				cnt_tt550_encoder_sensitivity->maximum(25);
				cnt_tt550_encoder_sensitivity->step(1);
				cnt_tt550_encoder_sensitivity->value(10);
				cnt_tt550_encoder_sensitivity->callback((Fl_Callback*)cb_cnt_tt550_encoder_sensitivity);
				cnt_tt550_encoder_sensitivity->align(Fl_Align(FL_ALIGN_LEFT));
				cnt_tt550_encoder_sensitivity->value(progStatus.tt550_encoder_sensitivity);

				sel_tt550_encoder_step = new Fl_Choice(575, 187, 80, 20, _("Step size"));
				sel_tt550_encoder_step->down_box(FL_BORDER_BOX);
				sel_tt550_encoder_step->callback((Fl_Callback*)cb_sel_tt550_encoder_step);
				sel_tt550_encoder_step->add("1|10|100|1 K|10 K");
				sel_tt550_encoder_step->value(progStatus.tt550_encoder_step);

			tt550_302A->end();

			tt550_302B = new Fl_Group(2, 179, 730, 32, _("302-B"));
				tt550_302B->hide();

				sel_tt550_F1_func = new Fl_Choice(168, 187, 111, 20, _("F1"));
				sel_tt550_F1_func->tooltip(_("Assign Func Key"));
				sel_tt550_F1_func->down_box(FL_BORDER_BOX);
				sel_tt550_F1_func->callback((Fl_Callback*)cb_sel_tt550_F1_func);
				sel_tt550_F1_func->add("None|Clear|CW++|CW--|Band++|Band--|Step++|Step--");
				sel_tt550_F1_func->value(progStatus.tt550_F1_func);

				sel_tt550_F2_func = new Fl_Choice(319, 187, 111, 20, _("F2"));
				sel_tt550_F2_func->tooltip(_("Assign Func Key"));
				sel_tt550_F2_func->down_box(FL_BORDER_BOX);
				sel_tt550_F2_func->callback((Fl_Callback*)cb_sel_tt550_F2_func);
				sel_tt550_F2_func->add("None|Clear|CW++|CW--|Band++|Band--|Step++|Step--");
				sel_tt550_F2_func->value(progStatus.tt550_F2_func);

				sel_tt550_F3_func = new Fl_Choice(465, 187, 111, 20, _("F3"));
				sel_tt550_F3_func->tooltip(_("Assign Func Key"));
				sel_tt550_F3_func->down_box(FL_BORDER_BOX);
				sel_tt550_F3_func->callback((Fl_Callback*)cb_sel_tt550_F3_func);
				sel_tt550_F3_func->add("None|Clear|CW++|CW--|Band++|Band--|Step++|Step--");
				sel_tt550_F3_func->value(progStatus.tt550_F3_func);

			tt550_302B->end();

		tabs550->end();

	grpTABS->end();
	Fl_Group::current()->resizable(grpTABS);

	btnInitializing = new Fl_Button(0, 23, 734, 194, _("Initializing\n\nPlease wait"));
	btnInitializing->color((Fl_Color)206);
	btnInitializing->labelfont(1);
	btnInitializing->labelsize(24);
	btnInitializing->hide();

	w->size_range(735, 218, 0, 218);
	w->end();

	return w;
}
