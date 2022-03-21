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

Fl_Group *gwide = (Fl_Group *)0;
Fl_Group *grp_row1b1b = (Fl_Group *)0;

//Fl_Group *tabs = (Fl_Group *)0;

Fl_Menu_Item menu_wide_menu[] = {
 {_("&File"), 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {_("E&xit"), 0,  (Fl_Callback*)cb_mnuExit, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {_("&Config"), 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Setup"), 0, 0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Transceiver"), 0,  (Fl_Callback*)cb_mnuConfigXcvr, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("tcpip"), 0, (Fl_Callback*)cb_mnuTCPIP, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("PTT-CMedia"), 0, (Fl_Callback*)cb_mnuCMEDIA, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("PTT-Generic"), 0, (Fl_Callback*)cb_mnuPTT, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("PTT-GPIO"), 0, (Fl_Callback*)cb_mnuGPIO, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("AUX"), 0, (Fl_Callback*)cb_mnuAUX, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("TMATE-2"), 0, (Fl_Callback*)cb_mnuTMATE2, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Server"), 0, (Fl_Callback*)cb_mnuSERVER, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Polling"), 0, (Fl_Callback*)cb_Polling, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Commands"), 0, (Fl_Callback*)cb_mnuCommands, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Send command"), 0, (Fl_Callback*)cb_Send, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Restore"), 0, (Fl_Callback*)cb_Restore, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Trace"), 0, (Fl_Callback*)cb_mnuTrace, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {_("UI"), 0, 0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Meters dialog"), 0, (Fl_Callback*)cb_mnu_show_meters, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Meter filtering"), 0,  (Fl_Callback*)cb_mnu_meter_filtering, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Power meter scale"), 0, (Fl_Callback*)cb_mnu_power_meter_scale, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Embed tabs"), 0, (Fl_Callback*)cb_mnu_embed_tabs, 0, 130, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Tooltips"), 0,  (Fl_Callback*)cb_mnuTooltips, 0, 130, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Voltmeter"), 0, (Fl_Callback*)cb_mnuVoltmeter, 0, 130, FL_NORMAL_LABEL, 0, 14, 0},
 {_("User Interface"), 0,  (Fl_Callback*)cb_mnuColorConfig, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0},
 {_("&Memory"), 0, 0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Save"), 0,  (Fl_Callback*)cb_save_me, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Manage"), 0,  (Fl_Callback*)cb_Memory, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {_("&Keyer"), 0, 0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {_("&CW Keyer"), 0, (Fl_Callback*)cb_CWkeyer, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("&FSK Keyer"), 0, (Fl_Callback*)cb_FSKkeyer, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {_("&Help"), 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {_("On Line Help"), 0,  (Fl_Callback*)cb_mnuOnLineHelp, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("&About"), 0,  (Fl_Callback*)cb_mnuAbout, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
 {_("&Events"), 0,  (Fl_Callback*)cb_Events, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("&xml-help"), 0,  (Fl_Callback*)cb_xml_help, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0}
};

Fl_Group *wide_main_group(int X, int Y, int W, int H)
{
	Fl_Group *g = new Fl_Group(X, Y, W, H);
	g->box(FL_FLAT_BOX);

	int xpos = X + 2;
	int ypos = Y + 2;

	txt_encA = new Fl_Output( H - 90, Y+1, 90, 20, "");
	txt_encA->box(FL_THIN_DOWN_BOX);
	txt_encA->align(20);
	txt_encA->hide();

	Fl_Group *wd_grp1, *wd_grp1a, *wd_grp1b, *wd_grp1c;

	wd_grp1 = new Fl_Group(xpos, ypos, W - 4, 60, "");
{
// Meters
		wd_grp1a = new Fl_Group(
			wd_grp1->x(), wd_grp1->y(), 208, wd_grp1->h(), "");
{
		grpMeters = new Fl_Group(wd_grp1a->x(), wd_grp1a->y(), wd_grp1a->w(), wd_grp1a->h());
{
		grpMeters->box(FL_DOWN_BOX);
		grpMeters->color((Fl_Color)246);

			scaleSmeter = new Fl_Box(xpos+1, ypos+1, 206, 20);
			scaleSmeter->box(FL_FLAT_BOX);
			scaleSmeter->color((Fl_Color)246);
			scaleSmeter->image(image_S60);
			scaleSmeter->tooltip(_("S units meter"));

			btnALC_SWR = new Fl_Button(xpos+1, ypos+1, 206, 20);
			btnALC_SWR->tooltip(_("Press SWR / ALC"));
			btnALC_SWR->box(FL_FLAT_BOX);
			btnALC_SWR->down_box(FL_FLAT_BOX);
			btnALC_SWR->color((Fl_Color)246);
			btnALC_SWR->image(image_SWR);
			btnALC_SWR->callback((Fl_Callback*)cb_btnALC_SWR);
			btnALC_SWR->hide();

			sldrRcvSignal = new Fl_SigBar(xpos+2, ypos+22, 200, 6);
			sldrRcvSignal->box(FL_FLAT_BOX);
			sldrRcvSignal->color(FL_BACKGROUND_COLOR);
			sldrRcvSignal->selection_color(FL_BACKGROUND_COLOR);
			sldrRcvSignal->labeltype(FL_NORMAL_LABEL);
			sldrRcvSignal->labelfont(0);
			sldrRcvSignal->labelsize(12);
			sldrRcvSignal->labelcolor(FL_FOREGROUND_COLOR);
			sldrRcvSignal->align(Fl_Align(FL_ALIGN_CENTER));
			sldrRcvSignal->when(FL_WHEN_CHANGED);
			sldrRcvSignal->hide();
			sldrRcvSignal->minimum(0);
			sldrRcvSignal->maximum(100);

			sldrALC = new Fl_SigBar(xpos+3, ypos+22, 200, 6);
			sldrALC->box(FL_FLAT_BOX);
			sldrALC->color(FL_BACKGROUND_COLOR);
			sldrALC->selection_color(FL_BACKGROUND_COLOR);
			sldrALC->labeltype(FL_NORMAL_LABEL);
			sldrALC->labelfont(0);
			sldrALC->labelsize(12);
			sldrALC->labelcolor(FL_FOREGROUND_COLOR);
			sldrALC->align(Fl_Align(FL_ALIGN_CENTER));
			sldrALC->when(FL_WHEN_CHANGED);
			sldrALC->hide();
			sldrALC->minimum(0);
			sldrALC->maximum(100);

			sldrSWR = new Fl_SigBar(xpos+2, ypos+22, 200, 6);
			sldrSWR->box(FL_FLAT_BOX);
			sldrSWR->color(FL_BACKGROUND_COLOR);
			sldrSWR->selection_color(FL_BACKGROUND_COLOR);
			sldrSWR->labeltype(FL_NORMAL_LABEL);
			sldrSWR->labelfont(0);
			sldrSWR->labelsize(12);
			sldrSWR->labelcolor(FL_FOREGROUND_COLOR);
			sldrSWR->align(Fl_Align(FL_ALIGN_CENTER));
			sldrSWR->when(FL_WHEN_CHANGED);
			sldrSWR->hide();
			sldrSWR->minimum(0);
			sldrSWR->maximum(100);

			sldrFwdPwr = new Fl_SigBar(xpos+2, ypos+29, 200, 6);
			sldrFwdPwr->box(FL_FLAT_BOX);
			sldrFwdPwr->color(FL_BACKGROUND_COLOR);
			sldrFwdPwr->selection_color(FL_BACKGROUND_COLOR);
			sldrFwdPwr->labeltype(FL_NORMAL_LABEL);
			sldrFwdPwr->labelfont(0);
			sldrFwdPwr->labelsize(12);
			sldrFwdPwr->labelcolor(FL_FOREGROUND_COLOR);
			sldrFwdPwr->align(Fl_Align(FL_ALIGN_CENTER));
			sldrFwdPwr->when(FL_WHEN_CHANGED);
			sldrFwdPwr->hide();
			sldrFwdPwr->minimum(0);
			sldrFwdPwr->maximum(100);

			scalePower = new Fl_Button(xpos+1, ypos+36, 206, 20);
			scalePower->tooltip(_("right-click to select power scale"));
			scalePower->box(FL_FLAT_BOX);
			scalePower->down_box(FL_FLAT_BOX);
			scalePower->color((Fl_Color)246);
			scalePower->image(image_P100);
			scalePower->callback((Fl_Callback*)cb_scalePower);

			sldrVoltage = new Fl_SigBar(xpos+2, ypos+29, 200, 6);
			sldrVoltage->box(FL_FLAT_BOX);
			sldrVoltage->color(FL_BACKGROUND_COLOR);
			sldrVoltage->selection_color(FL_BACKGROUND_COLOR);
			sldrVoltage->labeltype(FL_NORMAL_LABEL);
			sldrVoltage->labelfont(0);
			sldrVoltage->labelsize(12);
			sldrVoltage->labelcolor(FL_FOREGROUND_COLOR);
			sldrVoltage->align(Fl_Align(FL_ALIGN_CENTER));
			sldrVoltage->when(FL_WHEN_CHANGED);
			sldrVoltage->hide();
			sldrVoltage->minimum(0);
			sldrVoltage->maximum(100);

			scaleVoltage = new Fl_Box(xpos+1, ypos+36, 206, 20);
			scaleVoltage->box(FL_FLAT_BOX);
			scaleVoltage->image(image_voltmeter);
			scaleVoltage->tooltip(_("DC power"));

			sldrVoltage->hide();
			scaleVoltage->hide();

			meter_fill_box = new Fl_Box(xpos+207, ypos, 0, 60);
			meter_fill_box->box(FL_FLAT_BOX);
			meter_fill_box->color((Fl_Color)246);
}
		grpMeters->end();
}
		wd_grp1a->end();

// FreqDisp A group
		int x_1b = wd_grp1a->x() + wd_grp1a->w() + 2;
		wd_grp1b = new Fl_Group(
			x_1b, wd_grp1->y(), 290, wd_grp1->h(), "");
{
		FreqDispA = new cFreqControl(
			wd_grp1b->x(), wd_grp1b->y(), wd_grp1b->w(), wd_grp1b->h(), "10");
			FreqDispA->when(FL_WHEN_CHANGED);
			FreqDispA->SetONOFFCOLOR (FL_YELLOW, FL_BLACK);
			FreqDispA->setCallBack(movFreqA);
}
		wd_grp1b->end();

// FreqDisp B & vfo selection controls group
		int x_1c = wd_grp1b->x() + wd_grp1b->w() + 2;
		wd_grp1c = new Fl_Group(
			x_1c, wd_grp1->y(), W - x_1c - 2, wd_grp1->h(),""); 
{
			FreqDispB = new cFreqControl(
				wd_grp1c->x(), wd_grp1->y(), wd_grp1c->w(), 38, "10");
			FreqDispB->when(FL_WHEN_CHANGED);
			FreqDispB->SetONOFFCOLOR (FL_YELLOW, FL_BLACK);
			FreqDispB->setCallBack(movFreqB);

			grp_row0a = new Fl_Group(
				wd_grp1c->x() + 2, wd_grp1c->y() + FreqDispB->h() + 2, 
				FreqDispB->w(), wd_grp1c->h() - FreqDispB->h() - 2);
{
				int halfwidth = (wd_grp1c->w() - 8) / 10;

				btnA = new Fl_Light_Button(
					grp_row0a->x(), grp_row0a->y(), 
					3 * halfwidth, grp_row0a->h(), _("vfoA"));
				btnA->tooltip(_("Rx-A / Tx-A"));
				btnA->down_box(FL_THIN_DOWN_BOX);
				btnA->labelsize(12);
				btnA->callback((Fl_Callback*)cb_btnA);
				btnA->value(1);

				btn_KX3_swapAB = new Fl_Button(
					grp_row0a->x(), grp_row0a->y(), 
					3 * halfwidth, grp_row0a->h(), _("A / B"));
				btn_KX3_swapAB->tooltip(_("Swap A/B"));
				btn_KX3_swapAB->down_box(FL_DOWN_BOX);
				btn_KX3_swapAB->labelsize(12);
				btn_KX3_swapAB->callback((Fl_Callback*)cb_btn_KX3_swapAB);
				btn_KX3_swapAB->hide();

				btnB = new Fl_Light_Button(
					btnA->x() + btnA->w() + 2, btnA->y(),
					btnA->w(), btnA->h(), _("vfoB"));
				btnB->tooltip(_("Rx-B / Tx-B"));
				btnB->down_box(FL_THIN_DOWN_BOX);
				btnB->labelsize(12);
				btnB->callback((Fl_Callback*)cb_btnB);
				btnB->value(0);

				btn_KX3_A2B = new Fl_Button(
					btnA->x() + btnA->w() + 2, btnA->y(),
					btnA->w(), btnA->h(), _("A -> B"));
				btn_KX3_A2B->tooltip(_("Copy A to B"));
				btn_KX3_A2B->down_box(FL_DOWN_BOX);
				btn_KX3_A2B->labelsize(12);
				btn_KX3_A2B->callback((Fl_Callback*)cb_KX3_A2B);
				btn_KX3_A2B->hide();

				btn_K3_A2B = new Fl_Button(
					btn_KX3_A2B->x(), btn_KX3_A2B->y(),
					btn_KX3_A2B->w(), btn_KX3_A2B->h(), _("A -> B"));
				btn_K3_A2B->tooltip(_("Copy A to B"));
				btn_K3_A2B->down_box(FL_DOWN_BOX);
				btn_K3_A2B->labelsize(12);
				btn_K3_A2B->callback((Fl_Callback*)K3_A2B);
				btn_K3_A2B->hide();

				btn_tune_on_off = new Fl_Light_Button(
					btnB->x() + btnB->w() + 2, btnA->y(),
					halfwidth, btnA->h(), "");
				btn_tune_on_off->tooltip("Tuner On/Off");
				btn_tune_on_off->callback((Fl_Callback*)cb_btn_tune_on_off);

				btnTune = new Fl_Button(
					btn_tune_on_off->x() + btn_tune_on_off->w(), btnA->y(),
					2 * halfwidth, btnA->h(), _("Tune"));
				btnTune->tooltip(_("Manual Tune"));
				btnTune->callback((Fl_Callback*)cb_btnTune);

				btn_show_controls = new Fl_Button(
					btnTune->x() + btnTune->w() + 2, btnA->y(),
					halfwidth, btnA->h(), _("@-22->"));
				btn_show_controls->tooltip(_("Show/Hide controls"));
				btn_show_controls->callback((Fl_Callback*)cb_btn_show_controls);
}
			grp_row0a->end();
}
		wd_grp1c->end();
}
	wd_grp1->end();
	wd_grp1->resizable(wd_grp1b);

		ypos += 62;
{ // grp_row1 bandwidth, mode, att, ipo, nb, an a/b split ptt
		grp_row1 = new Fl_Group(xpos, ypos, W - 4, 20);
			grp_row1->box(FL_FLAT_BOX);
{ // grp_row1a
			grp_row1a = new Fl_Group(
				xpos, ypos, 
				grpMeters->w(), 20);

				int w1a = (grpMeters->w() - 2) / 2;

				opBW = new Fl_ComboBox(
					xpos, ypos, w1a, 20, _("Bandwidth"));
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
				opBW->readonly();
				opBW->end();

				btnDSP = new Fl_Button(xpos, ypos, 20, 20, _("L"));
				btnDSP->callback((Fl_Callback*)cb_btnDSP);
				btnDSP->hide();

				opDSP_lo = new Fl_ComboBox(xpos + 20, ypos, w1a - 20, 20, _("Lo Cut"));
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
				opDSP_lo->readonly();
				opDSP_lo->end();

				opDSP_hi = new Fl_ComboBox(xpos + 20, ypos, w1a - 20, 20, _("Lo Cut"));
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
				opDSP_hi->readonly();
				opDSP_hi->end();

				btnFILT = new Fl_Button(btnDSP->x(), btnDSP->y(), 20, 20, _("1"));
				btnFILT->callback((Fl_Callback*)cb_btnFILT);
				btnFILT->tooltip(_("Select filter"));
				btnFILT->hide();

				opMODE = new Fl_ComboBox(
					opBW->x() + opBW->w() + 2, ypos, 
					w1a, 20, _("Mode"));
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
				opMODE->readonly();
				opMODE->end();

			grp_row1a->end();
}
{ // grp_row1b
			int x1b = FreqDispA->x();
			grp_row1b = new Fl_Group(
				x1b, ypos,
				FreqDispA->w(), 20);

				int x2b = FreqDispA->w() / 2;
				grp_row1b1 = new Fl_Group(x1b, ypos, FreqDispA->w(), 20);

					Fl_Group* grp_row1b1a = new Fl_Group(x1b, ypos, x2b, 20);

						Fl_Box* bx_row1b1a = new Fl_Box(x1b, ypos, 0, 20);
						bx_row1b1a->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));

						btnAttenuator = new Fl_Light_Button(
							bx_row1b1a->x() + bx_row1b1a->w(), ypos, 70, 20, _("ATT"));
						btnAttenuator->tooltip(_("Attenuator On/Off"));
						btnAttenuator->down_box(FL_THIN_DOWN_BOX);
						btnAttenuator->labelsize(12);
						btnAttenuator->callback((Fl_Callback*)cb_btnAttenuator);

						btnPreamp = new Fl_Light_Button(
							btnAttenuator->x() + btnAttenuator->w() + 2, ypos, 
							70, 20, _("PRE"));
						btnPreamp->tooltip(_("Preamp On/Off"));
						btnPreamp->down_box(FL_THIN_DOWN_BOX);
						btnPreamp->labelsize(12);
						btnPreamp->callback((Fl_Callback*)cb_btnPreamp);

						grp_row1b1a->resizable(bx_row1b1a);

					grp_row1b1a->end();

					grp_row1b1b = new Fl_Group(
						grp_row1b1a->x() + x2b, ypos, 
						FreqDispA->w() - x2b, 20);

						btnNOISE = new Fl_Light_Button(
							grp_row1b1b->x(), ypos, 70, 20, _("NB"));
						btnNOISE->tooltip(_("Noise Blanker On/Off"));
						btnNOISE->down_box(FL_THIN_DOWN_BOX);
						btnNOISE->labelsize(12);
						btnNOISE->callback((Fl_Callback*)cb_btnNOISE);

						btnAutoNotch = new Fl_Light_Button(
							btnNOISE->x() + btnNOISE->w() + 2, ypos, 
							grp_row1b1b->w() - btnNOISE->w() - 4, 20, _("AN"));
						btnAutoNotch->tooltip(_("Auto Notch On/Off"));
						btnAutoNotch->down_box(FL_THIN_DOWN_BOX);
						btnAutoNotch->labelsize(12);
						btnAutoNotch->callback((Fl_Callback*)cb_btnAutoNotch);

						Fl_Box* bx_row1b1b = new Fl_Box(
							btnAutoNotch->x() + btnAutoNotch->w(), ypos, 2, 20);

					grp_row1b1b->end();
					grp_row1b1b->resizable(bx_row1b1b);

				grp_row1b1->end();

			grp_row1b->end();
}
{ // grp_row1c
			Fl_Group *grp_row1c = new Fl_Group(
				grp_row0a->x(), ypos,
				grp_row0a->w(), 20);

				btnAswapB = new Fl_Button(
					btnA->x(), ypos,
					btnA->w(), 20, _("A / B"));
					btnAswapB->tooltip(_("\
Left click:Swap A/B\n\
Right click:Copy A to B\n\
SHIFT click: FreqA -> FreqB\n\
CTRL  click: FreqB -> FreqA\
"));
				btnAswapB->down_box(FL_DOWN_BOX);
				btnAswapB->labelsize(12);
				btnAswapB->callback((Fl_Callback*)cb_btnAswapB);

				btn_K3_swapAB = new Fl_Button(
					btnA->x(), ypos,
					btnA->w(), btnAswapB->h(), _("A / B"));
				btn_K3_swapAB->tooltip(_("Left click: Swap A/B\nRight click: Copy A to B"));
				btn_K3_swapAB->down_box(FL_DOWN_BOX);
				btn_K3_swapAB->labelsize(12);
				btn_K3_swapAB->callback((Fl_Callback*)cb_btn_K3_swapAB);
				btn_K3_swapAB->hide();

				labelMEMORY = new Fl_Box(
					btnA->x(), ypos,
					btnA->w(), btnAswapB->h(), _("MEMORY"));
				labelMEMORY->box(FL_DOWN_BOX);
				labelMEMORY->color(FL_YELLOW);
				labelMEMORY->align(FL_ALIGN_CENTER);
				labelMEMORY->hide();

				btnSplit = new Fl_Light_Button(
					btnB->x(), ypos,
					btnB->w(), 20, _("Split"));
				btnSplit->tooltip(_("Rx-A / Tx-B"));
				btnSplit->down_box(FL_THIN_DOWN_BOX);
				btnSplit->labelsize(12);
				btnSplit->callback((Fl_Callback*)cb_btnSplit);
				btnSplit->value(progStatus.split);

				btnPTT = new Fl_Light_Button(
					btnTune->x(), ypos,
					btnTune->w(), 20, _("PTT"));
				btnPTT->tooltip(_("Xmt On/Off"));
				btnPTT->down_box(FL_THIN_DOWN_BOX);
				btnPTT->labelsize(12);
				btnPTT->callback((Fl_Callback*)cb_btnPTT);

				bx_row1_expander2 = new Fl_Box(
					btnPTT->x() + btnPTT->w(), ypos,
					1, 20);

			grp_row1c->end();
			grp_row1c->resizable(bx_row1_expander2);
}
		grp_row1->end();
		grp_row1->resizable(grp_row1b);
}

  // grp_row2 sliders
		grp_row2 = new Fl_Group(0, 107, WIDE_MAINW, 62);
{
		int colW = WIDE_MAINW / 3;
			grp_row2a = new Fl_Group(0, 107, colW, 62);
  // sql, mic
				sldrSQUELCH = new Fl_Wheel_Value_Slider(
					grp_row2a->x() + 54, grp_row2a->y(), 
					grp_row2a->w() - 54, 18, _("SQL"));
				sldrSQUELCH->tooltip(_("Adjust Squelch"));
				sldrSQUELCH->type(5);
				sldrSQUELCH->box(FL_THIN_DOWN_BOX);
				sldrSQUELCH->color(FL_BACKGROUND_COLOR);
				sldrSQUELCH->selection_color(FL_BACKGROUND_COLOR);
				sldrSQUELCH->labeltype(FL_NORMAL_LABEL);
				sldrSQUELCH->labelfont(0);
				sldrSQUELCH->labelsize(12);
				sldrSQUELCH->labelcolor(FL_FOREGROUND_COLOR);
				sldrSQUELCH->maximum(100);
				sldrSQUELCH->step(1);
				sldrSQUELCH->value(15);
				sldrSQUELCH->textsize(12);
				sldrSQUELCH->callback((Fl_Callback*)cb_sldrSQUELCH);
				sldrSQUELCH->align(Fl_Align(FL_ALIGN_LEFT));
				sldrSQUELCH->when(FL_WHEN_CHANGED);
				sldrSQUELCH->reverse(true);

				btnDataPort = new Fl_Light_Button(
					grp_row2a->x() + 2, grp_row2a->y() + 22,
					50, 18, _("Data"));
				btnDataPort->tooltip(_("Input on Data Port"));
				btnDataPort->down_box(FL_THIN_DOWN_BOX);
				btnDataPort->labelsize(12);
				btnDataPort->callback((Fl_Callback*)cb_btnDataPort);
				btnDataPort->hide();
				btnDataPort->value(progStatus.data_port);

				sldrMICGAIN = new Fl_Wheel_Value_Slider(
					grp_row2a->x() + 54, grp_row2a->y() + 22,
					grp_row2a->w() - 54, 18, _("Mic"));
				sldrMICGAIN->tooltip(_("Adjust Mic Gain"));
				sldrMICGAIN->type(5);
				sldrMICGAIN->box(FL_THIN_DOWN_BOX);
				sldrMICGAIN->color(FL_BACKGROUND_COLOR);
				sldrMICGAIN->selection_color(FL_BACKGROUND_COLOR);
				sldrMICGAIN->labeltype(FL_NORMAL_LABEL);
				sldrMICGAIN->labelfont(0);
				sldrMICGAIN->labelsize(12);
				sldrMICGAIN->labelcolor(FL_FOREGROUND_COLOR);
				sldrMICGAIN->maximum(100);
				sldrMICGAIN->step(1);
				sldrMICGAIN->textsize(12);
				sldrMICGAIN->callback((Fl_Callback*)cb_sldrMICGAIN);
				sldrMICGAIN->align(Fl_Align(FL_ALIGN_LEFT));
				sldrMICGAIN->when(FL_WHEN_CHANGED);
				sldrMICGAIN->reverse(true);

				btnPOWER = new Fl_Light_Button(
					grp_row2a->x() + 2, grp_row2a->y() + 44,
					50, 18, _("Pwr"));
				btnPOWER->tooltip(_("Enable/Disable Power adjuster"));
				btnPOWER->value(1);
				btnPOWER-> callback((Fl_Callback*) cb_btnPOWER);

				sldrPOWER = new Fl_Wheel_Value_Slider(
					grp_row2a->x() + 54, grp_row2a->y() + 44,
					grp_row2a->w() - 54, 18, "");
				sldrPOWER->tooltip(_("Adjust power level"));
				sldrPOWER->type(5);
				sldrPOWER->box(FL_THIN_DOWN_BOX);
				sldrPOWER->color(FL_BACKGROUND_COLOR);
				sldrPOWER->selection_color(FL_BACKGROUND_COLOR);
				sldrPOWER->labeltype(FL_NORMAL_LABEL);
				sldrPOWER->labelfont(0);
				sldrPOWER->labelsize(12);
				sldrPOWER->labelcolor(FL_FOREGROUND_COLOR);
				sldrPOWER->maximum(100);
				sldrPOWER->step(1);
				sldrPOWER->value(15);
				sldrPOWER->textsize(12);
				sldrPOWER->callback((Fl_Callback*)cb_sldrPOWER);
				sldrPOWER->align(Fl_Align(FL_ALIGN_LEFT));
				// TODO: Needed to fix bug in setPower(), where once slider is used, power levels stop tracking radio
				// TODO: Other sliders might need same change?
				// TODO: See also ui_small, and ui_touch for similar
				sldrPOWER->when(FL_WHEN_CHANGED);
				sldrPOWER->reverse(true);

				Fl_Group::current()->resizable(sldrPOWER);
			grp_row2a->end();

			grp_row2b = new Fl_Group(colW, 107, colW, 62);
 // IFshift, NR
				btnIFsh = new Fl_Light_Button(
					grp_row2b->x() + 2, grp_row2b->y(),
					50, 18, _("IFsh"));
				btnIFsh->tooltip(_("IF Shift On/Off"));
				btnIFsh->down_box(FL_THIN_DOWN_BOX);
				btnIFsh->labelsize(12);
				btnIFsh->callback((Fl_Callback*)cb_btnIFsh);

				btn_KX3_IFsh = new Fl_Button(
					grp_row2b->x() + 2, grp_row2b->y(),
					50, 18, _("Cntr"));
				btn_KX3_IFsh->tooltip(_("Center IF shift"));
				btn_KX3_IFsh->down_box(FL_THIN_DOWN_BOX);
				btn_KX3_IFsh->labelsize(12);
				btn_KX3_IFsh->callback((Fl_Callback*)cb_KX3_IFsh);
				btn_KX3_IFsh->hide();

				sldrIFSHIFT = new Fl_Wheel_Value_Slider(
					grp_row2b->x() + 54, grp_row2b->y(),
					grp_row2b->w() - 54, 18);
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

				btnLOCK = new Fl_Light_Button(
					grp_row2b->x() + 2, grp_row2b->y(),
					50, 18, _("Lock"));
				btnLOCK->tooltip(_("Lock PBT Inner/Outer sliders"));
				btnLOCK->down_box(FL_THIN_DOWN_BOX);
				btnLOCK->labelsize(12);
				btnLOCK->callback((Fl_Callback*)cb_btnLOCK);

				sldrINNER = new Fl_Wheel_Value_Slider(
					grp_row2b->x() + 54, grp_row2b->y(),
					grp_row2b->w() - 54, 18, "");
				sldrINNER->tooltip(_("Adjust Icom Inner PBT"));
				sldrINNER->type(5);
				sldrINNER->box(FL_THIN_DOWN_BOX);
				sldrINNER->color(FL_BACKGROUND_COLOR);
				sldrINNER->selection_color(FL_BACKGROUND_COLOR);
				sldrINNER->labeltype(FL_NORMAL_LABEL);
				sldrINNER->labelfont(0);
				sldrINNER->labelsize(12);
				sldrINNER->labelcolor(FL_FOREGROUND_COLOR);
				sldrINNER->minimum(-50);
				sldrINNER->maximum(50);
				sldrINNER->step(1);
				sldrINNER->textsize(12);
				sldrINNER->callback((Fl_Callback*)cb_sldrINNER);
				sldrINNER->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
				sldrINNER->when(FL_WHEN_CHANGED);
				sldrINNER->reverse(true);

				btnCLRPBT = new Fl_Button(
					grp_row2b->x() + 2, grp_row2b->y() + 22,
					50, 18, _("ClrPBT"));
				btnCLRPBT->tooltip(_("Zero PBT Inner/Outer sliders"));
				btnCLRPBT->down_box(FL_THIN_DOWN_BOX);
				btnCLRPBT->labelsize(12);
				btnCLRPBT->callback((Fl_Callback*)cb_btnCLRPBT);

				sldrOUTER = new Fl_Wheel_Value_Slider(
					grp_row2b->x() + 54, grp_row2b->y() + 22,
					grp_row2b->w() - 54, 18, "");
				sldrOUTER->tooltip(_("Adjust Icom Outer PBT"));
				sldrOUTER->type(5);
				sldrOUTER->box(FL_THIN_DOWN_BOX);
				sldrOUTER->color(FL_BACKGROUND_COLOR);
				sldrOUTER->selection_color(FL_BACKGROUND_COLOR);
				sldrOUTER->labeltype(FL_NORMAL_LABEL);
				sldrOUTER->labelfont(0);
				sldrOUTER->labelsize(12);
				sldrOUTER->labelcolor(FL_FOREGROUND_COLOR);
				sldrOUTER->minimum(-50);
				sldrOUTER->maximum(50);
				sldrOUTER->step(1);
				sldrOUTER->textsize(12);
				sldrOUTER->callback((Fl_Callback*)cb_sldrOUTER);
				sldrOUTER->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
				sldrOUTER->when(FL_WHEN_CHANGED);
				sldrOUTER->reverse(true);

				btnNotch = new Fl_Light_Button(
					grp_row2b->x() + 2, grp_row2b->y() + 44,
					50, 18, _("Nch"));
				btnNotch->tooltip(_("Notch - Manual (off) Auto (on)"));
				btnNotch->down_box(FL_THIN_DOWN_BOX);
				btnNotch->labelsize(12);
				btnNotch->callback((Fl_Callback*)cb_btnNotch);

				sldrNOTCH = new Fl_Wheel_Value_Slider(
					grp_row2b->x() + 54, grp_row2b->y() + 44,
					grp_row2b->w() - 54, 18);
				sldrNOTCH->tooltip(_("Adjust Notch Frequency"));
				sldrNOTCH->type(5);
				sldrNOTCH->box(FL_THIN_DOWN_BOX);
				sldrNOTCH->color(FL_BACKGROUND_COLOR);
				sldrNOTCH->selection_color(FL_BACKGROUND_COLOR);
				sldrNOTCH->labeltype(FL_NORMAL_LABEL);
				sldrNOTCH->labelfont(0);
				sldrNOTCH->labelsize(12);
				sldrNOTCH->labelcolor(FL_FOREGROUND_COLOR);
				sldrNOTCH->minimum(-1000);
				sldrNOTCH->maximum(1000);
				sldrNOTCH->step(10);
				sldrNOTCH->textsize(12);
				sldrNOTCH->callback((Fl_Callback*)cb_sldrNOTCH);
				sldrNOTCH->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
				sldrNOTCH->when(FL_WHEN_CHANGED);
				sldrNOTCH->reverse(true);

				Fl_Group::current()->resizable(sldrINNER);

			grp_row2b->end();

			grp_row2c = new Fl_Group(2*colW, 107, WIDE_MAINW - 2*colW - 2, 62);
 // AGC/RF gain, power, volume

				btnAGC = new Fl_Light_Button(
					grp_row2c->x() + 2, grp_row2c->y(),
					50, 18, _("AGC"));
				btnAGC->tooltip(_("Automatic Gain Control"));
				btnAGC->down_box(FL_THIN_DOWN_BOX);
				btnAGC->value(0);
				btnAGC->labelsize(12);
				btnAGC->callback((Fl_Callback*)cb_btnAGC);

				sldrRFGAIN = new Fl_Wheel_Value_Slider(
					grp_row2c->x() + 54, grp_row2c->y(),
					grp_row2c->w() - 54, 18);
				sldrRFGAIN->tooltip(_("Adjust RF gain"));
				sldrRFGAIN->type(5);
				sldrRFGAIN->box(FL_THIN_DOWN_BOX);
				sldrRFGAIN->color(FL_BACKGROUND_COLOR);
				sldrRFGAIN->selection_color(FL_BACKGROUND_COLOR);
				sldrRFGAIN->labeltype(FL_NORMAL_LABEL);
				sldrRFGAIN->labelfont(0);
				sldrRFGAIN->labelsize(12);
				sldrRFGAIN->labelcolor(FL_FOREGROUND_COLOR);
				sldrRFGAIN->maximum(100);
				sldrRFGAIN->step(1);
				sldrRFGAIN->value(15);
				sldrRFGAIN->textsize(12);
				sldrRFGAIN->callback((Fl_Callback*)cb_sldrRFGAIN);
				sldrRFGAIN->align(Fl_Align(FL_ALIGN_LEFT));
				sldrRFGAIN->when(FL_WHEN_CHANGED);
				sldrRFGAIN->reverse(true);

				btnNR = new Fl_Light_Button(
					grp_row2c->x() + 2, grp_row2c->y() + 22,
					50, 18, _("NR"));
				btnNR->tooltip(_("Noise Reduction On/Off"));
				btnNR->down_box(FL_THIN_DOWN_BOX);
				btnNR->labelsize(12);
				btnNR->callback((Fl_Callback*)cb_btnNR);

				sldrNR = new Fl_Wheel_Value_Slider(
					grp_row2c->x() + 54, grp_row2c->y() + 22,
					grp_row2c->w() - 54, 18);
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
				// TODO: Needed to fix bug in setNR(), where once slider is used, levels stop tracking radio
				// TODO: Other sliders might need same change?
				// TODO: See also ui_small, and ui_touch for similar
				sldrNR->when(FL_WHEN_CHANGED);

				sldrNR->reverse(true);

				btnVol = new Fl_Light_Button(
					grp_row2c->x() + 2, grp_row2c->y() + 44,
					50, 18, _("Vol"));
				btnVol->tooltip(_("Speaker on/off"));
				btnVol->down_box(FL_THIN_DOWN_BOX);
				btnVol->value(1);
				btnVol->labelsize(12);
				btnVol->callback((Fl_Callback*)cb_btnVol);

#ifdef __APPLE__
				sldrVOLUME = new Fl_Wheel_Value_Slider(
					grp_row2c->x() + 54, grp_row2c->y() + 44,
					grp_row2c->w() - 54 - 18, 18);
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
				sldrVOLUME->when(FL_WHEN_CHANGED | FL_MOUSEWHEEL | FL_LEAVE);
				Fl_Group::current()->resizable(sldrVOLUME);
				sldrVOLUME->reverse(true);

// box to keep right side of slider control from lying under the OS X
// dialog resize window decoration
				Fl_Box *volbox = new Fl_Box(
					W-18, grp_row2c->y() + 44,
					18, 18);
				volbox->box(FL_FLAT_BOX);
#else
				sldrVOLUME = new Fl_Wheel_Value_Slider(
					grp_row2c->x() + 54, grp_row2c->y() + 44,
					grp_row2c->w() - 54, 18);
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
				sldrVOLUME->when(FL_WHEN_CHANGED | FL_MOUSEWHEEL | FL_LEAVE);
				Fl_Group::current()->resizable(sldrVOLUME);
				sldrVOLUME->reverse(true);
#endif
				Fl_Group::current()->resizable(sldrVOLUME);
			grp_row2c->end();
}
		grp_row2->end();

	g->end();

	return g;
}

Fl_Double_Window* Wide_rig_window() {
	int mainW = WIDE_MAINW;
	int menuH = WIDE_MENUH;
	int mainH = WIDE_MAINH;
	int tabsH = WIDE_TABSH;

	Fl_Double_Window* w = new Fl_Double_Window(mainW, menuH + mainH + tabsH, _("Flrig"));
	w->align(Fl_Align(FL_ALIGN_CLIP|FL_ALIGN_INSIDE));

	grp_menu = new Fl_Group(0, 0, mainW, menuH);

		wide_menu = new Fl_Menu_Bar(0, 0, mainW - 64, menuH);
		wide_menu->textsize(12);
		wide_menu->menu(menu_wide_menu);

		Fl_Menu_Item * mnu = getMenuItem(_("Tooltips"), menu_wide_menu);
		if (mnu) {
			progStatus.tooltips ? mnu->set() : mnu->clear();
			mnuTooltips = mnu;
		}

		mnu = getMenuItem(_("Voltmeter"), menu_wide_menu);
		if (mnu) {
			progStatus.display_voltmeter ? mnu->set() : mnu->clear();
			mnuVoltmeter = mnu;
		}

		mnu = getMenuItem(_("Embed tabs"), menu_wide_menu);
		if (mnu) {
			progStatus.embed_tabs ? mnu->set() : mnu->clear();
			mnuEmbedTabs = mnu;
		}

		Fl_Group *mnu_box = new Fl_Group(mainW - 64, 0, 64, menuH);
		mnu_box->box(FL_UP_BOX);

			tcpip_box = new Fl_Group(mainW - 62, 2, 60, 18);
			tcpip_box->box(FL_FLAT_BOX);

				tcpip_menu_box = new Fl_Box(mainW - 62, 3, 16, 16);
				tcpip_menu_box->box(FL_DIAMOND_BOX);
				tcpip_menu_box->color(FL_GREEN);
				Fl_Box *tcpip_menu_label = new Fl_Box(mainW - 62 + 18, 3, 64 - 22, 16, _("tcpip"));
				tcpip_menu_label->box(FL_FLAT_BOX);
				tcpip_menu_label->align(FL_ALIGN_CENTER);
				tcpip_menu_label->tooltip(_("lit when connected to remote tcpip"));

			tcpip_box->end();
			tcpip_box->hide();

		mnu_box->end();

		grp_menu->resizable(wide_menu);

	grp_menu->end();

	main_group = wide_main_group(0, menuH, mainW, mainH);
	main_group->end();
	main_group->hide();

	tabs = new Fl_Group(0, menuH + mainH, mainW, tabsH);
	tabs->box(FL_FLAT_BOX);
	tabs->end();

	grpInitializing = new Fl_Group(0, menuH, mainW, mainH + tabsH, "");

		grpInitializing->box(FL_FLAT_BOX);
		grpInitializing->color(FL_LIGHT2);

		progress = new Fl_Progress(
			mainW / 4, mainH / 2,
			mainW / 2, 20, "Initializing");
		progress->maximum(100);
		progress->minimum(0);
		progress->labelcolor(FL_RED);
		progress->labelsize(12);
		progress->align(Fl_Align(FL_ALIGN_TOP));
		progress->selection_color(FL_GREEN);

	grpInitializing->end();

	grpInitializing->show();

	w->resizable(tabs);

	w->end();

	return w;
}
