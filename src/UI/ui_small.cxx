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

Fl_Group *gsmall = (Fl_Group *)0;

Fl_Menu_Item menu_small_menu[] = {
 {_("&File"), 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {_("E&xit"), 0,  (Fl_Callback*)cb_mnuExit, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {_("&Config"), 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Setup"), 0, 0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Transceiver"), 0,  (Fl_Callback*)cb_mnuConfigXcvr, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("tcpip"), 0, (Fl_Callback*)cb_mnuTCPIP, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("PTT"), 0, (Fl_Callback*)cb_mnuPTT, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("AUX"), 0, (Fl_Callback*)cb_mnuAUX, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Polling"), 0, (Fl_Callback*)cb_Polling, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Trace"), 0, (Fl_Callback*)cb_mnuTrace, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Commands"), 0, (Fl_Callback*)cb_mnuCommands, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Send command"), 0, (Fl_Callback*)cb_Send, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Restore"), 0, (Fl_Callback*)cb_Restore, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {_("UI"), 0, 0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Meter filtering"), 0,  (Fl_Callback*)cb_mnu_meter_filtering, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Small sliders"), 0,  (Fl_Callback*)cb_mnuSchema, 0, 130, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Tooltips"), 0,  (Fl_Callback*)cb_mnuTooltips, 0, 130, FL_NORMAL_LABEL, 0, 14, 0},
 {_("User Interface"), 0,  (Fl_Callback*)cb_mnuColorConfig, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0},
 {_("&Memory"), 0, 0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Save"), 0,  (Fl_Callback*)cb_save_me, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Manage"), 0,  (Fl_Callback*)cb_Memory, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {_("&Help"), 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {_("On Line Help"), 0,  (Fl_Callback*)cb_mnuOnLineHelp, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("&About"), 0,  (Fl_Callback*)cb_mnuAbout, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
 {_("&Events"), 0,  (Fl_Callback*)cb_Events, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("&xml-help"), 0,  (Fl_Callback*)cb_xml_help, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0}
};

int small_menuH = SMALL_MENUH;
int small_mainW = SMALL_MAINW;
int small_mainH = SMALL_MAINH;

Fl_Group *small_main_group(int X, int Y, int W, int H)
{
	Fl_Group *g = new Fl_Group(X,Y,W,H);
	g->box(FL_FLAT_BOX);

	FreqDispA = new cFreqControl(1, 24, 210, 35, _("10"));
	FreqDispA->box(FL_DOWN_BOX);
	FreqDispA->color(FL_BACKGROUND_COLOR);
	FreqDispA->selection_color(FL_BACKGROUND_COLOR);
	FreqDispA->labeltype(FL_NORMAL_LABEL);
	FreqDispA->labelfont(0);
	FreqDispA->labelsize(14);
	FreqDispA->labelcolor(FL_FOREGROUND_COLOR);
	FreqDispA->align(Fl_Align(FL_ALIGN_CENTER));
	FreqDispA->when(FL_WHEN_CHANGED);
	FreqDispA->SetONOFFCOLOR (FL_YELLOW, FL_BLACK);
	FreqDispA->setCallBack(movFreqA);

	FreqDispB = new cFreqControl(213, 24, 210, 35, _("10"));
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

	btnVol = new Fl_Light_Button(2, 125, 50, 18, _("Vol"));
	btnVol->tooltip(_("Speaker on/off"));
	btnVol->down_box(FL_THIN_DOWN_BOX);
	btnVol->value(1);
	btnVol->labelsize(12);
	btnVol->callback((Fl_Callback*)cb_btnVol);

	sldrVOLUME = new Fl_Wheel_Value_Slider(54, 125, 156, 18);
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
	sldrVOLUME->reverse(true);

	btnAGC = new Fl_Light_Button(2, 145, 50, 18, _("AGC"));
	btnAGC->tooltip(_("Automatic Gain Control"));
	btnAGC->down_box(FL_THIN_DOWN_BOX);
	btnAGC->value(0);
	btnAGC->labelsize(12);
	btnAGC->callback((Fl_Callback*)cb_btnAGC);

	sldrRFGAIN = new Fl_Wheel_Value_Slider(54, 145, 156, 18);
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
	sldrRFGAIN->reverse(true);

	sldrSQUELCH = new Fl_Wheel_Value_Slider(54, 165, 156, 18, _("SQL"));
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
	sldrSQUELCH->reverse(true);

	btnNR = new Fl_Light_Button(2, 185, 50, 18, _("NR"));
	btnNR->tooltip(_("Noise Reduction On/Off"));
	btnNR->down_box(FL_THIN_DOWN_BOX);
	btnNR->labelsize(12);
	btnNR->callback((Fl_Callback*)cb_btnNR);

	sldrNR = new Fl_Wheel_Value_Slider(54, 185, 156, 18);
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
	sldrNR->reverse(true);

	btnNotch = new Fl_Light_Button(2, 205, 50, 18, _("Nch"));
	btnNotch->tooltip(_("Notch - Manual (off) Auto (on)"));
	btnNotch->down_box(FL_THIN_DOWN_BOX);
	btnNotch->labelsize(12);
	btnNotch->callback((Fl_Callback*)cb_btnNotch);

	sldrNOTCH = new Fl_Wheel_Value_Slider(54, 205, 156, 18);
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

	btnLOCK = new Fl_Light_Button(2, 225, 50, 18, _("Lock"));
	btnLOCK->tooltip(_("Lock PBT Inner/Outer sliders"));
	btnLOCK->down_box(FL_THIN_DOWN_BOX);
	btnLOCK->labelsize(12);
	btnLOCK->callback((Fl_Callback*)cb_btnLOCK);

	sldrINNER = new Fl_Wheel_Value_Slider(54, 225, 156, 18, "");
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
	sldrINNER->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
	sldrINNER->when(FL_WHEN_CHANGED);
	sldrINNER->reverse(true);

	btnCLRPBT = new Fl_Button(
		2, 245,
		50, 18, _("ClrPBT"));
	btnCLRPBT->tooltip(_("Zero PBT Inner/Outer sliders"));
	btnCLRPBT->down_box(FL_THIN_DOWN_BOX);
	btnCLRPBT->labelsize(12);
	btnCLRPBT->callback((Fl_Callback*)cb_btnCLRPBT);

	sldrOUTER = new Fl_Wheel_Value_Slider(54, 245, 156, 18, "");
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
	sldrOUTER->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
	sldrOUTER->when(FL_WHEN_CHANGED);
	sldrOUTER->reverse(true);

	btnIFsh = new Fl_Light_Button(2, 265, 50, 18, _("IFsh"));
	btnIFsh->tooltip(_("IF Shift On/Off"));
	btnIFsh->down_box(FL_THIN_DOWN_BOX);
	btnIFsh->labelsize(12);
	btnIFsh->callback((Fl_Callback*)cb_btnIFsh);

	sldrIFSHIFT = new Fl_Wheel_Value_Slider(54, 265, 156, 18);
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

	btnDataPort = new Fl_Light_Button(2, 285, 50, 18, _("Data"));
	btnDataPort->tooltip(_("Input on Data Port"));
	btnDataPort->down_box(FL_THIN_DOWN_BOX);
	btnDataPort->labelsize(12);
	btnDataPort->callback((Fl_Callback*)cb_btnDataPort);
	btnDataPort->hide();
	btnDataPort->value(progStatus.data_port);

	sldrMICGAIN = new Fl_Wheel_Value_Slider(54, 285, 156, 18, _("Mic"));
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

	btnPOWER = new Fl_Light_Button(2, 305, 50, 18, _("Pwr"));
	btnPOWER->tooltip(_("Enable/Disable Power adjuster"));
	btnPOWER->value(1);
	btnPOWER-> callback((Fl_Callback*) cb_btnPOWER);

	sldrPOWER = new Fl_Wheel_Value_Slider(54, 305, 156, 18, "");
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

	btnA = new Fl_Light_Button(212, 62, 52, 20, _("vfoA"));
	btnA->tooltip(_("Rx-A / Tx-A"));
	btnA->down_box(FL_THIN_DOWN_BOX);
	btnA->labelsize(12);
	btnA->callback((Fl_Callback*)cb_btnA);
	btnA->value(!useB);

	btnB = new Fl_Light_Button(265, 62, 52, 20, _("vfoB"));
	btnB->tooltip(_("Rx-B / Tx-B"));
	btnB->down_box(FL_THIN_DOWN_BOX);
	btnB->labelsize(12);
	btnB->callback((Fl_Callback*)cb_btnB);
	btnB->value(useB);

	labelMEMORY = new Fl_Box(212, 62, 105, 20, _("MEMORY"));
	labelMEMORY->box(FL_DOWN_BOX);
	labelMEMORY->color(FL_YELLOW);
	labelMEMORY->align(FL_ALIGN_CENTER);
	labelMEMORY->hide();

	btn_K3_swapAB = new Fl_Button(265, 62, 52, 20, _("A/B"));
	btn_K3_swapAB->tooltip(_("Swap A/B"));
	btn_K3_swapAB->down_box(FL_DOWN_BOX);
	btn_K3_swapAB->labelsize(12);
	btn_K3_swapAB->callback((Fl_Callback*)cb_btn_K3_swapAB);
	btn_K3_swapAB->hide();

	btn_KX3_swapAB = new Fl_Button(265, 62, 52, 20, _("A/B"));
	btn_KX3_swapAB->tooltip(_("Swap A/B"));
	btn_KX3_swapAB->down_box(FL_DOWN_BOX);
	btn_KX3_swapAB->labelsize(12);
	btn_KX3_swapAB->callback((Fl_Callback*)cb_btn_KX3_swapAB);
	btn_KX3_swapAB->hide();

	btnAswapB = new Fl_Button(318, 62, 52, 20, _("A<->B"));
	btnAswapB->tooltip(_("Left click:Swap A/B\nRight click:Copy A to B"));
	btnAswapB->down_box(FL_DOWN_BOX);
	btnAswapB->labelsize(12);
	btnAswapB->callback((Fl_Callback*)cb_btnAswapB);

	btnSplit = new Fl_Light_Button(371, 62, 52, 20, _("Split"));
	btnSplit->tooltip(_("Rx-A / Tx-B"));
	btnSplit->down_box(FL_THIN_DOWN_BOX);
	btnSplit->labelsize(12);
	btnSplit->callback((Fl_Callback*)cb_btnSplit);
	btnSplit->value(progStatus.split);

	opBW = new Fl_ComboBox(212, 84, 105, 18, _("Bandwidth"));
	opBW->tooltip(_("Select Transceiver Bandwidth"));
	opBW->box(FL_NO_BOX);
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

	btnDSP = new Fl_Button(212, 84, 18, 18, _("L"));
	btnDSP->callback((Fl_Callback*)cb_btnDSP);
	btnDSP->hide();

	btnFILT = new Fl_Button(btnDSP->x(), btnDSP->y(), 18, 18, _("1"));
	btnFILT->callback((Fl_Callback*)cb_btnFILT);
	btnFILT->tooltip(_("Select filter"));
	btnFILT->hide();

	opDSP_lo = new Fl_ComboBox(231, 84, 86, 18, _("Lo Cut"));
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
	opDSP_lo->readonly();
	opDSP_lo->hide();
	opDSP_lo->end();

	opDSP_hi = new Fl_ComboBox(231, 84, 86, 18, _("Hi Cut"));
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
	opDSP_hi->readonly();
	opDSP_hi->hide();
	opDSP_hi->end();

	opMODE = new Fl_ComboBox(318, 84, 105, 18, _("Mode"));
	opMODE->tooltip(_("Select transceiver operating mode"));
	opMODE->box(FL_NO_BOX);
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

	btn_show_controls = new Fl_Button(3, 325, 18, 18, _("@-22->"));
	btn_show_controls->tooltip(_("Show/Hide controls"));
	btn_show_controls->callback((Fl_Callback*)cb_btn_show_controls);

	btnAttenuator = new Fl_Light_Button(26, 325, 60, 18, _("Att"));
	btnAttenuator->tooltip(_("Attenuator On/Off"));
	btnAttenuator->down_box(FL_THIN_DOWN_BOX);
	btnAttenuator->labelsize(12);
	btnAttenuator->callback((Fl_Callback*)cb_btnAttenuator);

	btnPreamp = new Fl_Light_Button(93, 325, 60, 18, _("Pre"));
	btnPreamp->tooltip(_("Preamp On/Off"));
	btnPreamp->down_box(FL_THIN_DOWN_BOX);
	btnPreamp->labelsize(12);
	btnPreamp->callback((Fl_Callback*)cb_btnPreamp);

	btnNOISE = new Fl_Light_Button(160, 325, 60, 18, _("NB"));
	btnNOISE->tooltip(_("Noise Blanker On/Off"));
	btnNOISE->down_box(FL_THIN_DOWN_BOX);
	btnNOISE->labelsize(12);
	btnNOISE->callback((Fl_Callback*)cb_btnNOISE);

	btnAutoNotch = new Fl_Light_Button(227, 325, 60, 18, _("AN"));
	btnAutoNotch->tooltip(_("Auto Notch On/Off"));
	btnAutoNotch->down_box(FL_THIN_DOWN_BOX);
	btnAutoNotch->labelsize(12);
	btnAutoNotch->callback((Fl_Callback*)cb_btnAutoNotch);

	btn_tune_on_off = new Fl_Light_Button(294, 325, 20, 18, "");
	btn_tune_on_off->tooltip("Tuner On/Off");
	btn_tune_on_off->callback((Fl_Callback*)cb_btn_tune_on_off);

	btnTune = new Fl_Button(
		btn_tune_on_off->x() + btn_tune_on_off->w(),
		btn_tune_on_off->y(), 40, 18, _("Tune"));
	btnTune->tooltip(_("Manual Tune"));
	btnTune->callback((Fl_Callback*)cb_btnTune);

	btnPTT = new Fl_Light_Button(362, 325, 60, 18, _("PTT"));
	btnPTT->tooltip(_("Xmt On/Off"));
	btnPTT->down_box(FL_THIN_DOWN_BOX);
	btnPTT->labelsize(12);
	btnPTT->callback((Fl_Callback*)cb_btnPTT);

	Fl_Group* sm_grp1 = new Fl_Group(1, 62, 210, 60);
		sm_grp1->box(FL_DOWN_BOX);

		grpMeters = new Fl_Group(3, 63, 206, 57);
		grpMeters->box(FL_FLAT_BOX);

			scaleSmeter = new Fl_Box(3, 63, 206, 20);
			scaleSmeter->box(FL_FLAT_BOX);
			scaleSmeter->image(image_S60);
			scaleSmeter->tooltip(_("S units meter"));

			btnALC_SWR = new Fl_Button(3, 63, 206, 20);
			btnALC_SWR->tooltip(_("Press SWR / ALC"));
			btnALC_SWR->box(FL_FLAT_BOX);
			btnALC_SWR->down_box(FL_FLAT_BOX);
			btnALC_SWR->image(image_SWR);
			btnALC_SWR->callback((Fl_Callback*)cb_btnALC_SWR);
			btnALC_SWR->hide();

			sldrRcvSignal = new Fl_SigBar(5, 85, 200, 6);
			sldrRcvSignal->box(FL_FLAT_BOX);
			sldrRcvSignal->color(FL_BACKGROUND_COLOR);
			sldrRcvSignal->selection_color(FL_BACKGROUND_COLOR);
			sldrRcvSignal->labeltype(FL_NORMAL_LABEL);
			sldrRcvSignal->labelfont(0);
			sldrRcvSignal->labelsize(14);
			sldrRcvSignal->labelcolor(FL_FOREGROUND_COLOR);
			sldrRcvSignal->align(Fl_Align(FL_ALIGN_CENTER));
			sldrRcvSignal->when(FL_WHEN_CHANGED);
			sldrRcvSignal->hide();
			sldrRcvSignal->minimum(0);
			sldrRcvSignal->maximum(100);

			sldrALC = new Fl_SigBar(5, 85, 200, 6);
			sldrALC->box(FL_FLAT_BOX);
			sldrALC->color(FL_BACKGROUND_COLOR);
			sldrALC->selection_color(FL_BACKGROUND_COLOR);
			sldrALC->labeltype(FL_NORMAL_LABEL);
			sldrALC->labelfont(0);
			sldrALC->labelsize(14);
			sldrALC->labelcolor(FL_FOREGROUND_COLOR);
			sldrALC->align(Fl_Align(FL_ALIGN_CENTER));
			sldrALC->when(FL_WHEN_CHANGED);
			sldrALC->hide();
			sldrALC->minimum(0);
			sldrALC->maximum(100);

			sldrSWR = new Fl_SigBar(5, 85, 200, 6);
			sldrSWR->box(FL_FLAT_BOX);
			sldrSWR->color(FL_BACKGROUND_COLOR);
			sldrSWR->selection_color(FL_BACKGROUND_COLOR);
			sldrSWR->labeltype(FL_NORMAL_LABEL);
			sldrSWR->labelfont(0);
			sldrSWR->labelsize(14);
			sldrSWR->labelcolor(FL_FOREGROUND_COLOR);
			sldrSWR->align(Fl_Align(FL_ALIGN_CENTER));
			sldrSWR->when(FL_WHEN_CHANGED);
			sldrSWR->hide();
			sldrSWR->minimum(0);
			sldrSWR->maximum(100);

			sldrFwdPwr = new Fl_SigBar(5, 92, 200, 6);
			sldrFwdPwr->box(FL_FLAT_BOX);
			sldrFwdPwr->color(FL_BACKGROUND_COLOR);
			sldrFwdPwr->selection_color(FL_BACKGROUND_COLOR);
			sldrFwdPwr->labeltype(FL_NORMAL_LABEL);
			sldrFwdPwr->labelfont(0);
			sldrFwdPwr->labelsize(14);
			sldrFwdPwr->labelcolor(FL_FOREGROUND_COLOR);
			sldrFwdPwr->align(Fl_Align(FL_ALIGN_CENTER));
			sldrFwdPwr->when(FL_WHEN_CHANGED);
			sldrFwdPwr->hide();
			sldrFwdPwr->minimum(0);
			sldrFwdPwr->maximum(100);

			scalePower = new Fl_Button(3, 99, 206, 20);
			scalePower->tooltip(_("right-click to select power scale"));
			scalePower->box(FL_FLAT_BOX);
			scalePower->down_box(FL_FLAT_BOX);
			scalePower->image(image_P100);
			scalePower->callback((Fl_Callback*)cb_scalePower);

			meter_fill_box = new Fl_Box(207, 63, 0, 57);
			meter_fill_box->box(FL_FLAT_BOX);
			meter_fill_box->color((Fl_Color)246);

		grpMeters->end();

	sm_grp1->end();

	boxControl = new Fl_Box(3, 231, 130, 17, _("Auxilliary Controls:"));
	boxControl->box(FL_FLAT_BOX);
	boxControl->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));

	btnAuxRTS = new Fl_Light_Button(218, 230, 60, 18, _("RTS"));
	btnAuxRTS->tooltip(_("RTS control pin"));
	btnAuxRTS->down_box(FL_THIN_DOWN_BOX);
	btnAuxRTS->labelsize(12);
	btnAuxRTS->callback((Fl_Callback*)cb_btnAuxRTS);
	btnAuxRTS->value(progStatus.aux_rts);

	btnAuxDTR = new Fl_Light_Button(146, 230, 60, 18, _("DTR"));
	btnAuxDTR->tooltip(_("DTR control pin"));
	btnAuxDTR->down_box(FL_THIN_DOWN_BOX);
	btnAuxDTR->labelsize(12);
	btnAuxDTR->callback((Fl_Callback*)cb_btnAuxDTR);
	btnAuxDTR->value(progStatus.aux_dtr);

	cntRIT = new Hspinner(213, 104, 54, 18, _("R"));
	cntRIT->tooltip(_("RIT"));
	cntRIT->type(FL_INT_INPUT);
	cntRIT->labelsize(12);
	cntRIT->minimum(-1000);
	cntRIT->maximum(1000);
	cntRIT->step(10);
	cntRIT->callback((Fl_Callback*)cb_cntRIT);
	cntRIT->align(Fl_Align(FL_ALIGN_RIGHT));

	cntXIT = new Hspinner(283, 104, 54, 18, _("X"));
	cntXIT->tooltip(_("XIT"));
	cntXIT->type(FL_INT_INPUT);
	cntXIT->labelsize(12);
	cntXIT->minimum(-1000);
	cntXIT->maximum(1000);
	cntXIT->step(10);
	cntXIT->callback((Fl_Callback*)cb_cntXIT);
	cntXIT->align(Fl_Align(FL_ALIGN_RIGHT));

	cntBFO = new Hspinner(355, 104, 54, 18, _("B"));
	cntBFO->tooltip(_("BFO"));
	cntBFO->type(FL_INT_INPUT);
	cntBFO->labelsize(12);
	cntBFO->minimum(0);
	cntBFO->maximum(1000);
	cntBFO->step(10);
	cntBFO->value(600);
	cntBFO->callback((Fl_Callback*)cb_cntBFO);
	cntBFO->align(Fl_Align(FL_ALIGN_RIGHT));

	g->end();

	return g;
}

static Fl_Box *filler = (Fl_Box *)0;

Fl_Double_Window *tabs_window()
{
	int gph = 70 + 22;

	Fl_Double_Window *w = new Fl_Double_Window(
		0, 0, 
		small_mainW, gph,
		"flrig extra controls");

	grpTABS = new Fl_Group(0, 0, small_mainW, gph);

	tabs550 = new Fl_Tabs(0, 0, small_mainW, gph);
		tabs550->selection_color(FL_LIGHT1);
		tabs550->hide();

		tt550_CW = new Fl_Group(0, 24, small_mainW, gph - 24, _("CW"));
			tt550_CW->color(FL_LIGHT1);
			tt550_CW->selection_color(FL_LIGHT1);

			spnr_tt550_cw_wpm = new Hspinner(
				2, 28,
				70, 22, _("wpm"));
			spnr_tt550_cw_wpm->type(FL_INT_INPUT);
			spnr_tt550_cw_wpm->minimum(1);
			spnr_tt550_cw_wpm->maximum(80);
			spnr_tt550_cw_wpm->step(1);
			spnr_tt550_cw_wpm->value(24);
			spnr_tt550_cw_wpm->callback((Fl_Callback*)cb_spnr_tt550_cw_wpm);
			spnr_tt550_cw_wpm->value(progStatus.tt550_cw_wpm);
			spnr_tt550_cw_wpm->align(FL_ALIGN_BOTTOM | FL_ALIGN_CENTER);

			spnr_tt550_cw_weight = new Hspinner(
				spnr_tt550_cw_wpm->x() +
				spnr_tt550_cw_wpm->w() + 2, 40,
				70, 22, _("Weight"));
			spnr_tt550_cw_weight->type(FL_FLOAT_INPUT);
			spnr_tt550_cw_weight->minimum(0.75);
			spnr_tt550_cw_weight->maximum(1.5);
			spnr_tt550_cw_weight->step(0.05);
			spnr_tt550_cw_weight->format("%.2f");
			spnr_tt550_cw_weight->value(1);
			spnr_tt550_cw_weight->callback((Fl_Callback*)cb_spnr_tt550_cw_weight);
			spnr_tt550_cw_weight->value(progStatus.tt550_cw_weight);
			spnr_tt550_cw_weight->align(FL_ALIGN_BOTTOM | FL_ALIGN_CENTER);

			spnr_tt550_cw_qsk = new Hspinner(
				spnr_tt550_cw_weight->x() +
				spnr_tt550_cw_weight->w() + 2, 40,
				70, 22, _("Delay"));
			spnr_tt550_cw_qsk->tooltip(_("QSK delay (msec)"));
			spnr_tt550_cw_qsk->type(1);
			spnr_tt550_cw_qsk->minimum(0);
			spnr_tt550_cw_qsk->maximum(100);
			spnr_tt550_cw_qsk->step(1);
			spnr_tt550_cw_qsk->value(20);
			spnr_tt550_cw_qsk->callback((Fl_Callback*)cb_spnr_tt550_cw_qsk);
			spnr_tt550_cw_qsk->value(progStatus.tt550_cw_qsk);
			spnr_tt550_cw_qsk->align(FL_ALIGN_BOTTOM | FL_ALIGN_CENTER);

			spnr_tt550_cw_vol = new Hspinner(
				spnr_tt550_cw_qsk->x() +
				spnr_tt550_cw_qsk->w() + 2, 40,
				70, 22, _("S-T vol"));
			spnr_tt550_cw_vol->tooltip(_("Side tone volume"));
			spnr_tt550_cw_vol->type(1);
			spnr_tt550_cw_vol->minimum(5);
			spnr_tt550_cw_vol->maximum(80);
			spnr_tt550_cw_vol->step(1);
			spnr_tt550_cw_vol->value(24);
			spnr_tt550_cw_vol->callback((Fl_Callback*)cb_spnr_tt550_cw_vol);
			spnr_tt550_cw_vol->value(progStatus.tt550_cw_vol);
			spnr_tt550_cw_vol->align(FL_ALIGN_BOTTOM | FL_ALIGN_CENTER);

			spnr_tt550_cw_spot = new Hspinner(
				spnr_tt550_cw_vol->x() +
				spnr_tt550_cw_vol->w() + 2, 40,
				70, 22, _("Spot Vol"));
			spnr_tt550_cw_spot->tooltip(_("Spot volume"));
			spnr_tt550_cw_spot->type(1);
			spnr_tt550_cw_spot->minimum(0);
			spnr_tt550_cw_spot->maximum(100);
			spnr_tt550_cw_spot->step(1);
			spnr_tt550_cw_spot->value(20);
			spnr_tt550_cw_spot->callback((Fl_Callback*)cb_spnr_tt550_cw_spot);
			spnr_tt550_cw_spot->value(progStatus.tt550_cw_spot);
			spnr_tt550_cw_spot->align(FL_ALIGN_BOTTOM | FL_ALIGN_CENTER);

			btn_tt550_enable_keyer = new Fl_Check_Button(
				spnr_tt550_cw_spot->x() +
				spnr_tt550_cw_spot->w() + 2, 40,
				50, 22, _("Keyer"));
			btn_tt550_enable_keyer->tooltip(_("Enable keyer"));
			btn_tt550_enable_keyer->down_box(FL_DOWN_BOX);
			btn_tt550_enable_keyer->callback((Fl_Callback*)cb_btn_tt550_enable_keyer);
			btn_tt550_enable_keyer->value(progStatus.tt550_enable_keyer);
		tt550_CW->end();

		tt550_VOX = new Fl_Group(0, 24, small_mainW, gph - 24, _("Vox"));
			tt550_VOX->hide();

			spnr_tt550_vox_gain = new Hspinner(
				2, 40,
				70, 22, _("gain"));
			spnr_tt550_vox_gain->type(1);
			spnr_tt550_vox_gain->minimum(0);
			spnr_tt550_vox_gain->maximum(100);
			spnr_tt550_vox_gain->step(1);
			spnr_tt550_vox_gain->callback((Fl_Callback*)cb_spnr_tt550_vox_gain);
			spnr_tt550_vox_gain->value(progStatus.tt550_vox_gain);
			spnr_tt550_vox_gain->align(FL_ALIGN_BOTTOM | FL_ALIGN_CENTER);

			spnr_tt550_anti_vox = new Hspinner(
				spnr_tt550_vox_gain->x() +
				spnr_tt550_vox_gain->w() + 4, 40,
				70, 22, _("anti"));
			spnr_tt550_anti_vox->type(1);
			spnr_tt550_anti_vox->minimum(0);
			spnr_tt550_anti_vox->maximum(100);
			spnr_tt550_anti_vox->step(1);
			spnr_tt550_anti_vox->callback((Fl_Callback*)cb_spnr_tt550_anti_vox);
			spnr_tt550_anti_vox->value(progStatus.tt550_vox_anti);
			spnr_tt550_anti_vox->align(FL_ALIGN_BOTTOM | FL_ALIGN_CENTER);

			spnr_tt550_vox_hang = new Hspinner(
				spnr_tt550_anti_vox->x() +
				spnr_tt550_anti_vox->w() + 4, 40,
				70, 22, _("hang"));
			spnr_tt550_vox_hang->type(1);
			spnr_tt550_vox_hang->minimum(0);
			spnr_tt550_vox_hang->maximum(100);
			spnr_tt550_vox_hang->step(1);
			spnr_tt550_vox_hang->callback((Fl_Callback*)cb_spnr_tt550_vox_hang);
			spnr_tt550_vox_hang->value(progStatus.tt550_vox_hang);
			spnr_tt550_vox_hang->align(FL_ALIGN_BOTTOM | FL_ALIGN_CENTER);

			btn_tt550_vox = new Fl_Light_Button(
				spnr_tt550_vox_hang->x() +
				spnr_tt550_vox_hang->w() + 4, 40,
				50, 22, _("VOX"));
			btn_tt550_vox->callback((Fl_Callback*)cb_btn_tt550_vox);
			btn_tt550_vox->value(progStatus.tt550_vox_onoff);
		tt550_VOX->end();

		tt550_Speech = new Fl_Group(0, 24, small_mainW, gph - 24, _("Spch"));
			tt550_Speech->hide();

			btn_tt550_CompON = new Fl_Light_Button(
				4, 40, 60, 22, _("On"));
			btn_tt550_CompON->callback((Fl_Callback*)cb_btn_tt550_CompON);
			btn_tt550_CompON->value(progStatus.tt550_compON);

			spnr_tt550_compression = new Hspinner(
				btn_tt550_CompON->x() +
				btn_tt550_CompON->w() + 4, 40,
				70, 22, _("Comp"));
			spnr_tt550_compression->type(1);
			spnr_tt550_compression->minimum(0);
			spnr_tt550_compression->maximum(100);
			spnr_tt550_compression->step(1);
			spnr_tt550_compression->callback((Fl_Callback*)cb_spnr_tt550_compression);
			spnr_tt550_compression->value(progStatus.tt550_compression);
			spnr_tt550_compression->align(FL_ALIGN_BOTTOM | FL_ALIGN_CENTER);

			spnr_tt550_mon_vol = new Hspinner(
				spnr_tt550_compression->x() +
				spnr_tt550_compression->w() + 4, 40,
				70, 22, _("Mon vol"));
			spnr_tt550_mon_vol->tooltip(_("Side tone volume"));
			spnr_tt550_mon_vol->type(1);
			spnr_tt550_mon_vol->minimum(0);
			spnr_tt550_mon_vol->maximum(100);
			spnr_tt550_mon_vol->step(1);
			spnr_tt550_mon_vol->value(25);
			spnr_tt550_mon_vol->callback((Fl_Callback*)cb_spnr_tt550_mon_vol);
			spnr_tt550_mon_vol->value(progStatus.tt550_mon_vol);
			spnr_tt550_mon_vol->align(FL_ALIGN_BOTTOM | FL_ALIGN_CENTER);
		tt550_Speech->end();

		tt550_Audio = new Fl_Group(0, 24, small_mainW, gph, _("Audio"));
			tt550_Audio->hide();

			spnr_tt550_line_out = new Hspinner(
				4, 40,
				70, 22, _("line out"));
			spnr_tt550_line_out->type(1);
			spnr_tt550_line_out->minimum(0);
			spnr_tt550_line_out->maximum(100);
			spnr_tt550_line_out->step(1);
			spnr_tt550_line_out->value(20);
			spnr_tt550_line_out->callback((Fl_Callback*)cb_spnr_tt550_line_out);
			spnr_tt550_line_out->align(Fl_Align(FL_ALIGN_LEFT));
			spnr_tt550_line_out->value(progStatus.tt550_line_out);
			spnr_tt550_line_out->align(FL_ALIGN_BOTTOM | FL_ALIGN_CENTER);

			btnAccInp = new Fl_Check_Button(
				spnr_tt550_line_out->x() +
				spnr_tt550_line_out->w() + 4, 40,
				80, 22, _("Acc Inp"));
			btnAccInp->tooltip(_("Enable to use accessory audio input"));
			btnAccInp->down_box(FL_DOWN_BOX);
			btnAccInp->callback((Fl_Callback*)cb_btnAccInp);
			btnAccInp->value(progStatus.tt550_use_line_in);
		tt550_Audio->end();

		tt550_Rx = new Fl_Group(0, 24, small_mainW, gph - 24, _("RX"));
			tt550_Rx->hide();

			cbo_tt550_nb_level = new Fl_ComboBox(
				4, 40,
				80, 22, _("NB level"));
				cbo_tt550_nb_level->tooltip(_("Select Noise Blanker level"));
			cbo_tt550_nb_level->box(FL_DOWN_BOX);
			cbo_tt550_nb_level->color(FL_WHITE);
			cbo_tt550_nb_level->selection_color(FL_BACKGROUND_COLOR);
			cbo_tt550_nb_level->labeltype(FL_NORMAL_LABEL);
			cbo_tt550_nb_level->labelfont(0);
			cbo_tt550_nb_level->labelsize(14);
			cbo_tt550_nb_level->labelcolor(FL_FOREGROUND_COLOR);
			cbo_tt550_nb_level->callback((Fl_Callback*)cb_cbo_tt550_nb_level);
			cbo_tt550_nb_level->align(FL_ALIGN_BOTTOM | FL_ALIGN_CENTER);
			cbo_tt550_nb_level->when(FL_WHEN_RELEASE);
			cbo_tt550_nb_level->index(progStatus.tt550_nb_level);
			cbo_tt550_nb_level->readonly();
			cbo_tt550_nb_level->end();

			cbo_tt550_agc_level = new Fl_ComboBox(
				cbo_tt550_nb_level->x() +
				cbo_tt550_nb_level->w() + 5, 40,
				80, 22, _("AGC"));
			cbo_tt550_agc_level->tooltip(_("Select AGC"));
			cbo_tt550_agc_level->box(FL_DOWN_BOX);
			cbo_tt550_agc_level->color(FL_WHITE);
			cbo_tt550_agc_level->selection_color(FL_BACKGROUND_COLOR);
			cbo_tt550_agc_level->labeltype(FL_NORMAL_LABEL);
			cbo_tt550_agc_level->labelfont(0);
			cbo_tt550_agc_level->labelsize(14);
			cbo_tt550_agc_level->labelcolor(FL_FOREGROUND_COLOR);
			cbo_tt550_agc_level->callback((Fl_Callback*)cb_cbo_tt550_agc_level);
			cbo_tt550_agc_level->align(FL_ALIGN_BOTTOM | FL_ALIGN_CENTER);
			cbo_tt550_agc_level->when(FL_WHEN_RELEASE);
			cbo_tt550_agc_level->index(progStatus.tt550_agc_level);
			cbo_tt550_agc_level->readonly();
			cbo_tt550_agc_level->end();

			spnr_tt550_vfo_adj = new Hspinner(
				cbo_tt550_agc_level->x() +
				cbo_tt550_agc_level->w() + 5, 40,
				100, 22, _("Vfo Adj(ppm)"), 16);
			spnr_tt550_vfo_adj->callback((Fl_Callback*)cb_spnr_tt550_vfo_adj);
			spnr_tt550_vfo_adj->align(FL_ALIGN_BOTTOM | FL_ALIGN_CENTER);
			spnr_tt550_vfo_adj->minimum(-100);
			spnr_tt550_vfo_adj->maximum(100);
			spnr_tt550_vfo_adj->type(FL_FLOAT_INPUT);
			spnr_tt550_vfo_adj->step(0.02);
			spnr_tt550_vfo_adj->format("%0.2f");
			spnr_tt550_vfo_adj->value(progStatus.vfo_adj);
		tt550_Rx->end();

		tt550_Tx = new Fl_Group(0, 24, small_mainW, gph - 24, _("TX"));
			tt550_Tx->hide();

			btn_tt550_enable_xmtr = new Fl_Check_Button(10, 40, 70, 22, _("Tx ON"));
			btn_tt550_enable_xmtr->down_box(FL_DOWN_BOX);
			btn_tt550_enable_xmtr->value(1);
			btn_tt550_enable_xmtr->callback((Fl_Callback*)cb_btn_tt550_enable_xmtr);
			btn_tt550_enable_xmtr->value(progStatus.tt550_enable_xmtr);

			btn_tt550_enable_tloop = new Fl_Check_Button(87, 40, 70, 22, _("Tloop"));
			btn_tt550_enable_tloop->down_box(FL_DOWN_BOX);
			btn_tt550_enable_tloop->callback((Fl_Callback*)cb_btn_tt550_enable_tloop);
			btn_tt550_enable_tloop->value(progStatus.tt550_enable_tloop);

			btn_tt550_tuner_bypass = new Fl_Check_Button(164, 40, 70, 22, _("Tuner"));
			btn_tt550_tuner_bypass->tooltip(_("Check to bypass tuner"));
			btn_tt550_tuner_bypass->down_box(FL_DOWN_BOX);
			btn_tt550_tuner_bypass->callback((Fl_Callback*)cb_btn_tt550_tuner_bypass);

			op_tt550_XmtBW = new Fl_ComboBox(241, 40, 100, 22, _("Xmt BW"));
			op_tt550_XmtBW->tooltip(_("Select Transceiver Bandwidth"));
			op_tt550_XmtBW->box(FL_DOWN_BOX);
			op_tt550_XmtBW->color(FL_WHITE);
			op_tt550_XmtBW->selection_color(FL_BACKGROUND_COLOR);
			op_tt550_XmtBW->labeltype(FL_NORMAL_LABEL);
			op_tt550_XmtBW->labelfont(0);
			op_tt550_XmtBW->labelsize(12);
			op_tt550_XmtBW->labelcolor(FL_FOREGROUND_COLOR);
			op_tt550_XmtBW->callback((Fl_Callback*)cb_op_tt550_XmtBW);
			op_tt550_XmtBW->align(Fl_Align(FL_ALIGN_BOTTOM));
			op_tt550_XmtBW->when(FL_WHEN_RELEASE);
			op_tt550_XmtBW->index(progStatus.tt550_xmt_bw);
			op_tt550_XmtBW->readonly();
			op_tt550_XmtBW->end();

			btn_tt550_use_xmt_bw = new Fl_Check_Button(345, 40, 21, 22, _("Enable"));
			btn_tt550_use_xmt_bw->tooltip(_("Use Xmt BW setting"));
			btn_tt550_use_xmt_bw->down_box(FL_DOWN_BOX);
			btn_tt550_use_xmt_bw->callback((Fl_Callback*)cb_btn_tt550_use_xmt_bw);
			btn_tt550_use_xmt_bw->align(Fl_Align(FL_ALIGN_RIGHT));
			btn_tt550_use_xmt_bw->value(progStatus.tt550_use_xmt_bw);
		tt550_Tx->end();

		tt550_302A = new Fl_Group(0, 24, small_mainW, gph - 24, _("302-A"));
			tt550_302A->hide();

			spnr_tt550_keypad_time_out = new Hspinner(30, 40, 120, 22, _("Time out"), 16);
			spnr_tt550_keypad_time_out->tooltip(_("Clear M302 keypad after ## secs"));
			spnr_tt550_keypad_time_out->minimum(0.5);
			spnr_tt550_keypad_time_out->maximum(10);
			spnr_tt550_keypad_time_out->value(2);
			spnr_tt550_keypad_time_out->color(FL_WHITE);
			spnr_tt550_keypad_time_out->callback((Fl_Callback*)cb_spnr_tt550_keypad_time_out);
			spnr_tt550_keypad_time_out->align(Fl_Align(34));
			spnr_tt550_keypad_time_out->value(progStatus.tt550_keypad_timeout/1000.0);
			spnr_tt550_keypad_time_out->step(0.1);
			spnr_tt550_keypad_time_out->align(FL_ALIGN_BOTTOM | FL_ALIGN_CENTER);

			spnr_tt550_encoder_sensitivity = new Hspinner(
				spnr_tt550_keypad_time_out->x() +
				spnr_tt550_keypad_time_out->w() + 10, 40, 80, 22, _("Sensitivity"), 16);
			spnr_tt550_encoder_sensitivity->tooltip(_("1 - most sensitive"));
			spnr_tt550_encoder_sensitivity->type(1);
			spnr_tt550_encoder_sensitivity->minimum(1);
			spnr_tt550_encoder_sensitivity->maximum(25);
			spnr_tt550_encoder_sensitivity->step(1);
			spnr_tt550_encoder_sensitivity->value(10);
			spnr_tt550_encoder_sensitivity->color(FL_WHITE);
			spnr_tt550_encoder_sensitivity->callback((Fl_Callback*)cb_spnr_tt550_encoder_sensitivity);
			spnr_tt550_encoder_sensitivity->value(progStatus.tt550_encoder_sensitivity);
			spnr_tt550_encoder_sensitivity->align(FL_ALIGN_BOTTOM | FL_ALIGN_CENTER);

			sel_tt550_encoder_step = new Fl_Choice(312, 277, 72, 21, _("Step size"));
			sel_tt550_encoder_step->down_box(FL_BORDER_BOX);
			sel_tt550_encoder_step->callback((Fl_Callback*)cb_sel_tt550_encoder_step);
			sel_tt550_encoder_step->align(Fl_Align(FL_ALIGN_BOTTOM));
			sel_tt550_encoder_step->add("1|10|100|1 K|10 K");
			sel_tt550_encoder_step->value(progStatus.tt550_encoder_step);
			sel_tt550_encoder_step->align(FL_ALIGN_BOTTOM | FL_ALIGN_CENTER);
		tt550_302A->end();

		tt550_302B = new Fl_Group(0, 24, small_mainW, gph - 24, _("302-B"));
			tt550_302B->hide();

			sel_tt550_F1_func = new Fl_Choice(34, 40, 99, 22, _("F1"));
			sel_tt550_F1_func->tooltip(_("Assign Func Key"));
			sel_tt550_F1_func->down_box(FL_BORDER_BOX);
			sel_tt550_F1_func->callback((Fl_Callback*)cb_sel_tt550_F1_func);
			sel_tt550_F1_func->add("None|Clear|CW++|CW--|Band++|Band--|Step++|Step--");
			sel_tt550_F1_func->value(progStatus.tt550_F1_func);

			sel_tt550_F2_func = new Fl_Choice(169, 40, 99, 22, _("F2"));
			sel_tt550_F2_func->tooltip(_("Assign Func Key"));
			sel_tt550_F2_func->down_box(FL_BORDER_BOX);
			sel_tt550_F2_func->callback((Fl_Callback*)cb_sel_tt550_F2_func);
			sel_tt550_F2_func->add("None|Clear|CW++|CW--|Band++|Band--|Step++|Step--");
			sel_tt550_F2_func->value(progStatus.tt550_F2_func);

			sel_tt550_F3_func = new Fl_Choice(50, 40, 99, 22, _("F3"));
			sel_tt550_F3_func->tooltip(_("Assign Func Key"));
			sel_tt550_F3_func->down_box(FL_BORDER_BOX);
			sel_tt550_F3_func->callback((Fl_Callback*)cb_sel_tt550_F3_func);
			sel_tt550_F3_func->add("None|Clear|CW++|CW--|Band++|Band--|Step++|Step--");
			sel_tt550_F3_func->value(progStatus.tt550_F3_func);
		tt550_302B->end();

		tt550_AT = new Fl_Group(0, 24, small_mainW, gph - 24, _("AT-11"));
			tt550_AT_inline = new Fl_Light_Button(5, 40, 100, 22, _("Inline"));
			tt550_AT_inline->tooltip(_("Enabled - autotune\nDisabled - bypass"));
			tt550_AT_inline->callback((Fl_Callback*)cb_AT_bypass);

			tt550_AT_Z = new Fl_Light_Button(
				RIGHT_OF(tt550_AT_inline) + 10, 40, 55, 22, _("Z hi"));
			tt550_AT_Z->tooltip(_("Hi / Lo Impedance network"));
			tt550_AT_Z->callback((Fl_Callback*)cb_AT_Z);

			tt550_AT_capUP = new Fl_Button(
				RIGHT_OF(tt550_AT_Z) + 4, 40, 40, 22, _("C >"));
			tt550_AT_capUP->tooltip(_("Increase C"));
			tt550_AT_capUP->callback((Fl_Callback*)cb_AT_capUP);

			tt550_AT_capDN = new Fl_Button(
				RIGHT_OF(tt550_AT_capUP) + 4, 40, 40, 22, _("C <"));
			tt550_AT_capDN->tooltip(_("Decrease C"));
			tt550_AT_capDN->callback((Fl_Callback*)cb_AT_capDN);

			tt550_AT_indUP = new Fl_Button(
				RIGHT_OF(tt550_AT_capDN) + 4, 40, 40, 22, _("L >"));
			tt550_AT_indUP->tooltip(_("Increase L"));
			tt550_AT_indUP->callback((Fl_Callback*)cb_AT_indUP);

			tt550_AT_indDN = new Fl_Button(
				RIGHT_OF(tt550_AT_indUP) + 4, 40, 40, 22, _("L <"));
			tt550_AT_indDN->tooltip(_("Decrease L"));
			tt550_AT_indDN->callback((Fl_Callback*)cb_AT_indDN);

			tt550_Nsamples = new Fl_Simple_Counter(
				RIGHT_OF(tt550_AT_indDN) + 4, 40, 50, 22, _("N"));
			tt550_Nsamples->tooltip(_("Fwd/Rev Pwr detect over # samples"));
			tt550_Nsamples->align(FL_ALIGN_RIGHT);
			tt550_Nsamples->step(1);
			tt550_Nsamples->minimum(1);
			tt550_Nsamples->maximum(FPLEN);
			tt550_Nsamples->value(progStatus.tt550_Nsamples);
			tt550_Nsamples->callback((Fl_Callback*)cb_tt550_Nsamples);
		tt550_AT->end();
	tabs550->end();

	tabsGeneric = new Fl_Tabs(0, 0, small_mainW, gph);
		tabsGeneric->selection_color(FL_LIGHT1);

		tab_yaesu_bands = new Fl_Group(0, 24, small_mainW, gph - 24, _("Bands"));
			btn_yaesu_select_1 = new Fl_Button(4, 40, 60, 18, _("1.8"));
			btn_yaesu_select_1->tooltip(_("160m Band"));
			btn_yaesu_select_1->color((Fl_Color)246);
			btn_yaesu_select_1->callback((Fl_Callback*)cb_btn_band_select, (void *)1);

			btn_yaesu_select_2 = new Fl_Button(74, 40, 60, 18, _("3.5"));
			btn_yaesu_select_2->tooltip(_("80m band"));
			btn_yaesu_select_2->color((Fl_Color)246);
			btn_yaesu_select_2->callback((Fl_Callback*)cb_btn_band_select, (void *)2);

			btn_yaesu_select_3 = new Fl_Button(145, 40, 60, 18, _("7"));
			btn_yaesu_select_3->tooltip(_("40m band"));
			btn_yaesu_select_3->color((Fl_Color)246);
			btn_yaesu_select_3->callback((Fl_Callback*)cb_btn_band_select, (void *)3);
			btn_yaesu_select_3->align(Fl_Align(FL_ALIGN_CLIP));

			btn_yaesu_select_4 = new Fl_Button(216, 40, 60, 18, _("10"));
			btn_yaesu_select_4->tooltip(_("30m band"));
			btn_yaesu_select_4->color((Fl_Color)246);
			btn_yaesu_select_4->callback((Fl_Callback*)cb_btn_band_select, (void *)4);

			btn_yaesu_select_5 = new Fl_Button(287, 40, 60, 18, _("14"));
			btn_yaesu_select_5->tooltip(_("20m band"));
			btn_yaesu_select_5->color((Fl_Color)246);
			btn_yaesu_select_5->callback((Fl_Callback*)cb_btn_band_select, (void *)5);

			btn_yaesu_select_6 = new Fl_Button(358, 40, 60, 18, _("18"));
			btn_yaesu_select_6->tooltip(_("17m band"));
			btn_yaesu_select_6->color((Fl_Color)246);
			btn_yaesu_select_6->callback((Fl_Callback*)cb_btn_band_select, (void *)6);

			btn_yaesu_select_7 = new Fl_Button(4, 60, 60, 18, _("21"));
			btn_yaesu_select_7->tooltip(_("15m band"));
			btn_yaesu_select_7->color((Fl_Color)246);
			btn_yaesu_select_7->callback((Fl_Callback*)cb_btn_band_select, (void *)7);

			btn_yaesu_select_8 = new Fl_Button(74, 60, 60, 18, _("24"));
			btn_yaesu_select_8->tooltip(_("12m band"));
			btn_yaesu_select_8->color((Fl_Color)246);
			btn_yaesu_select_8->callback((Fl_Callback*)cb_btn_band_select, (void *)8);

			btn_yaesu_select_9 = new Fl_Button(145, 60, 60, 18, _("28"));
			btn_yaesu_select_9->tooltip(_("10m band"));
			btn_yaesu_select_9->color((Fl_Color)246);
			btn_yaesu_select_9->callback((Fl_Callback*)cb_btn_band_select, (void *)9);

			btn_yaesu_select_10 = new Fl_Button(216, 60, 60, 18, _("50"));
			btn_yaesu_select_10->tooltip(_("6m band"));
			btn_yaesu_select_10->color((Fl_Color)246);
			btn_yaesu_select_10->callback((Fl_Callback*)cb_btn_band_select, (void *)10);

			btn_yaesu_select_11 = new Fl_Button(287, 60, 60, 18, _("GEN"));
			btn_yaesu_select_11->tooltip(_("General RX"));
			btn_yaesu_select_11->color((Fl_Color)246);
			btn_yaesu_select_11->callback((Fl_Callback*)cb_btn_band_select, (void *)11);

			op_yaesu_select60 = new Fl_ComboBox(358, 60, 60, 18, _("combo"));
			op_yaesu_select60->tooltip(_("vfo / 60 meter channel"));
			op_yaesu_select60->box(FL_NO_BOX);
			op_yaesu_select60->color((Fl_Color)246);
			op_yaesu_select60->selection_color(FL_BACKGROUND_COLOR);
			op_yaesu_select60->labeltype(FL_NORMAL_LABEL);
			op_yaesu_select60->labelfont(0);
			op_yaesu_select60->labelsize(12);
			op_yaesu_select60->labelcolor(FL_FOREGROUND_COLOR);
			op_yaesu_select60->callback((Fl_Callback*)cb_btn_band_select, (void *)13);
			op_yaesu_select60->align(Fl_Align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE));
			op_yaesu_select60->when(FL_WHEN_RELEASE);
			op_yaesu_select60->readonly();
			op_yaesu_select60->end();

		tab_yaesu_bands->end();

		tab_FT8n_bands = new Fl_Group(0, 24, small_mainW, gph - 24, _("Bands"));
		static const char setsave[] = _("Left click - set\nRight click - save");
			btn_FT8n_select_1 = new Fl_Button(4, 30, 60, 18, _("1.8"));
			btn_FT8n_select_1->tooltip(setsave);
			btn_FT8n_select_1->color((Fl_Color)246);
			btn_FT8n_select_1->callback((Fl_Callback*)cb_btn_band_select, (void *)1);

			btn_FT8n_select_2 = new Fl_Button(74, 30, 60, 18, _("3.5"));
			btn_FT8n_select_2->tooltip(setsave);
			btn_FT8n_select_2->color((Fl_Color)246);
			btn_FT8n_select_2->callback((Fl_Callback*)cb_btn_band_select, (void *)2);

			btn_FT8n_select_3 = new Fl_Button(145, 30, 60, 18, _("7"));
			btn_FT8n_select_3->tooltip(setsave);
			btn_FT8n_select_3->color((Fl_Color)246);
			btn_FT8n_select_3->callback((Fl_Callback*)cb_btn_band_select, (void *)3);

			btn_FT8n_select_4 = new Fl_Button(216, 30, 60, 18, _("10"));
			btn_FT8n_select_4->tooltip(setsave);
			btn_FT8n_select_4->color((Fl_Color)246);
			btn_FT8n_select_4->callback((Fl_Callback*)cb_btn_band_select, (void *)4);

			btn_FT8n_select_5 = new Fl_Button(287, 30, 60, 18, _("14"));
			btn_FT8n_select_5->tooltip(setsave);
			btn_FT8n_select_5->color((Fl_Color)246);
			btn_FT8n_select_5->callback((Fl_Callback*)cb_btn_band_select, (void *)5);

			btn_FT8n_select_6 = new Fl_Button(358, 30, 60, 18, _("18"));
			btn_FT8n_select_6->tooltip(setsave);
			btn_FT8n_select_6->color((Fl_Color)246);
			btn_FT8n_select_6->callback((Fl_Callback*)cb_btn_band_select, (void *)6);

			btn_FT8n_select_7 = new Fl_Button(4, 50, 60, 18, _("21"));
			btn_FT8n_select_7->tooltip(setsave);
			btn_FT8n_select_7->color((Fl_Color)246);
			btn_FT8n_select_7->callback((Fl_Callback*)cb_btn_band_select, (void *)7);

			btn_FT8n_select_8 = new Fl_Button(74, 50, 60, 18, _("24"));
			btn_FT8n_select_8->tooltip(setsave);
			btn_FT8n_select_8->color((Fl_Color)246);
			btn_FT8n_select_8->callback((Fl_Callback*)cb_btn_band_select, (void *)8);

			btn_FT8n_select_9 = new Fl_Button(145, 50, 60, 18, _("28"));
			btn_FT8n_select_9->tooltip(setsave);
			btn_FT8n_select_9->color((Fl_Color)246);
			btn_FT8n_select_9->callback((Fl_Callback*)cb_btn_band_select, (void *)9);

			btn_FT8n_select_10 = new Fl_Button(216, 50, 60, 18, _("50"));
			btn_FT8n_select_10->tooltip(setsave);
			btn_FT8n_select_10->color((Fl_Color)246);
			btn_FT8n_select_10->callback((Fl_Callback*)cb_btn_band_select, (void *)10);

			btn_FT8n_select_11 = new Fl_Button(287, 50, 60, 18, _("144"));
			btn_FT8n_select_11->tooltip(setsave);
			btn_FT8n_select_11->color((Fl_Color)246);
			btn_FT8n_select_11->callback((Fl_Callback*)cb_btn_band_select, (void *)11);

			btn_FT8n_select_12 = new Fl_Button(358, 50, 60, 18, _("430"));
			btn_FT8n_select_12->tooltip(setsave);
			btn_FT8n_select_12->color((Fl_Color)246);
			btn_FT8n_select_12->callback((Fl_Callback*)cb_btn_band_select, (void *)12);

		tab_FT8n_bands->end();

		tab_FT8n_CTCSS = new Fl_Group(0, 24, small_mainW, gph - 24, _("CTCSS"));

			choice_FT8n_tTONE = new Fl_PL_tone(70, 34, 75, 18, _("Tone X:"));
			choice_FT8n_tTONE->align((Fl_Align)FL_ALIGN_LEFT);
			choice_FT8n_tTONE->value(8);
			choice_FT8n_tTONE->end();

			choice_FT8n_rTONE = new Fl_PL_tone(70, 66, 75, 18, _("R:"));
			choice_FT8n_rTONE->align((Fl_Align)FL_ALIGN_LEFT);
			choice_FT8n_rTONE->value(8);
			choice_FT8n_rTONE->end();

			setTONES = new Fl_Button(150, 34, 60, 18, _("Set"));
			setTONES->callback((Fl_Callback*)cb_tones, 0);

			FMoff_freq = new cFreqControl(225, 34, 100, 26, "5");
			FMoff_freq->value(600);
			Fl_Box *MHZ = new Fl_Box(FMoff_freq->x() + FMoff_freq->w() + 4, 34, 50, 26, "MHz");
			MHZ->box(FL_FLAT_BOX);
			MHZ->align(FL_ALIGN_CENTER);

			FMoffset = new Fl_ComboBox(225, 66, 100, 18, "");
			FMoffset->tooltip(_("Select FM operation"));
			FMoffset->box(FL_NO_BOX);
			FMoffset->color(FL_BACKGROUND2_COLOR);
			FMoffset->selection_color(FL_BACKGROUND_COLOR);
			FMoffset->add("simplex|minus|plus");
			FMoffset->index(0);

			setOFFSET = new Fl_Button(FMoffset->x() + FMoffset->w() + 5, 66, 60, 18, _("Set"));
			setOFFSET->callback((Fl_Callback*)cb_offset, 0);

		tab_FT8n_CTCSS->end();

		tab_icom_bands = new Fl_Group(0, 24, small_mainW, gph - 24, _("Bands"));
			btn_icom_select_1 = new Fl_Button(4, 30, 60, 18, _("1.8"));
			btn_icom_select_1->tooltip(_("Left click - get\nRight click - set"));
			btn_icom_select_1->color((Fl_Color)246);
			btn_icom_select_1->callback((Fl_Callback*)cb_btn_band_select, (void *)1);

			btn_icom_select_2 = new Fl_Button(74, 30, 60, 18, _("3.5"));
			btn_icom_select_2->tooltip(_("Left click - get\nRight click - set"));
			btn_icom_select_2->color((Fl_Color)246);
			btn_icom_select_2->callback((Fl_Callback*)cb_btn_band_select, (void *)2);

			btn_icom_select_3 = new Fl_Button(145, 30, 60, 18, _("7"));
			btn_icom_select_3->tooltip(_("Left click - get\nRight click - set"));
			btn_icom_select_3->color((Fl_Color)246);
			btn_icom_select_3->callback((Fl_Callback*)cb_btn_band_select, (void *)3);

			btn_icom_select_4 = new Fl_Button(216, 30, 60, 18, _("10"));
			btn_icom_select_4->tooltip(_("Left click - get\nRight click - set"));
			btn_icom_select_4->color((Fl_Color)246);
			btn_icom_select_4->callback((Fl_Callback*)cb_btn_band_select, (void *)4);

			btn_icom_select_5 = new Fl_Button(287, 30, 60, 18, _("14"));
			btn_icom_select_5->tooltip(_("Left click - get\nRight click - set"));
			btn_icom_select_5->color((Fl_Color)246);
			btn_icom_select_5->callback((Fl_Callback*)cb_btn_band_select, (void *)5);

			btn_icom_select_6 = new Fl_Button(358, 30, 60, 18, _("18"));
			btn_icom_select_6->tooltip(_("Left click - get\nRight click - set"));
			btn_icom_select_6->color((Fl_Color)246);
			btn_icom_select_6->callback((Fl_Callback*)cb_btn_band_select, (void *)6);

			btn_icom_select_7 = new Fl_Button(4, 50, 60, 18, _("21"));
			btn_icom_select_7->tooltip(_("Left click - get\nRight click - set"));
			btn_icom_select_7->color((Fl_Color)246);
			btn_icom_select_7->callback((Fl_Callback*)cb_btn_band_select, (void *)7);

			btn_icom_select_8 = new Fl_Button(74, 50, 60, 18, _("24"));
			btn_icom_select_8->tooltip(_("Left click - get\nRight click - set"));
			btn_icom_select_8->color((Fl_Color)246);
			btn_icom_select_8->callback((Fl_Callback*)cb_btn_band_select, (void *)8);

			btn_icom_select_9 = new Fl_Button(145, 50, 60, 18, _("28"));
			btn_icom_select_9->tooltip(_("Left click - get\nRight click - set"));
			btn_icom_select_9->color((Fl_Color)246);
			btn_icom_select_9->callback((Fl_Callback*)cb_btn_band_select, (void *)9);

			btn_icom_select_10 = new Fl_Button(216, 50, 60, 18, _("50"));
			btn_icom_select_10->tooltip(_("Left click - get\nRight click - set"));
			btn_icom_select_10->color((Fl_Color)246);
			btn_icom_select_10->callback((Fl_Callback*)cb_btn_band_select, (void *)10);

			btn_icom_select_11 = new Fl_Button(287, 50, 60, 18, _("144"));
			btn_icom_select_11->tooltip(_("Left click - get\nRight click - set"));
			btn_icom_select_11->color((Fl_Color)246);
			btn_icom_select_11->callback((Fl_Callback*)cb_btn_band_select, (void *)11);

			btn_icom_select_12 = new Fl_Button(358, 50, 60, 18, _("430"));
			btn_icom_select_12->tooltip(_("Left click - get\nRight click - set"));
			btn_icom_select_12->color((Fl_Color)246);
			btn_icom_select_12->callback((Fl_Callback*)cb_btn_band_select, (void *)12);

			btn_icom_select_13 = new Fl_Button(4, 70, 60, 18, _("1.2 G"));
			btn_icom_select_13->tooltip(_("Left click - get\nRight click - set"));
			btn_icom_select_13->color((Fl_Color)246);
			btn_icom_select_13->callback((Fl_Callback*)cb_btn_band_select, (void *)13);

			choice_tTONE = new Fl_PL_tone(145, 70, 75, 18, _("tTONE"));
			choice_tTONE->align((Fl_Align)FL_ALIGN_LEFT);
			choice_tTONE->value(8);
			choice_tTONE->end();

			choice_rTONE = new Fl_PL_tone(287, 70, 75, 18, _("rTONE"));
			choice_rTONE->align((Fl_Align)FL_ALIGN_LEFT);
			choice_rTONE->value(8);
			choice_rTONE->end();

		tab_icom_bands->end();

		genericCW = new Fl_Group(0, 24, small_mainW, gph - 24, _("CW"));
			genericCW->color(FL_LIGHT1);
			genericCW->selection_color(FL_LIGHT1);
			genericCW->hide();

			spnr_cw_wpm = new Hspinner(
				4, 40,
				85, 22, _("wpm"), 16);
			spnr_cw_wpm->type(1);
			spnr_cw_wpm->minimum(5);
			spnr_cw_wpm->maximum(80);
			spnr_cw_wpm->step(1);
			spnr_cw_wpm->callback((Fl_Callback*)cb_spnr_cw_wpm);
			spnr_cw_wpm->value(progStatus.cw_wpm);
			spnr_cw_wpm->labelsize(12);
			spnr_cw_wpm->align(FL_ALIGN_BOTTOM | FL_ALIGN_CENTER);
			spnr_cw_wpm->tooltip(_("CW words per minute"));

			spnr_cw_weight = new Hspinner(
				spnr_cw_wpm->x() + spnr_cw_wpm->w() + 4, 40,
				85, 22, _("Weight"), 16);
			spnr_cw_weight->type(1);
			spnr_cw_weight->minimum(2.5);
			spnr_cw_weight->maximum(4.5);
			spnr_cw_weight->step(0.1);
			spnr_cw_weight->value(3);
			spnr_cw_weight->callback((Fl_Callback*)cb_spnr_cw_weight);
			spnr_cw_weight->value(progStatus.cw_weight);
			spnr_cw_weight->labelsize(12);
			spnr_cw_weight->align(FL_ALIGN_BOTTOM | FL_ALIGN_CENTER);
			spnr_cw_weight->tooltip(_("CW weight"));

			spnr_cw_spot_tone= new Hspinner(
				spnr_cw_weight->w() + spnr_cw_weight->x() + 4, 40,
				85, 22, _("Spot tone"), 16);
			spnr_cw_spot_tone->tooltip(_("Spot volume"));
			spnr_cw_spot_tone->type(1);
			spnr_cw_spot_tone->minimum(50);
			spnr_cw_spot_tone->maximum(1050);
			spnr_cw_spot_tone->step(5);
			spnr_cw_spot_tone->value(700);
			spnr_cw_spot_tone->callback((Fl_Callback*)cb_spnr_cw_spot_tone);
			spnr_cw_spot_tone->value(progStatus.cw_spot);
			spnr_cw_spot_tone->labelsize(12);
			spnr_cw_spot_tone->align(FL_ALIGN_BOTTOM | FL_ALIGN_CENTER);

			btnSpot = new Fl_Light_Button(
				spnr_cw_spot_tone->x() + spnr_cw_spot_tone->w() + 4, 36,
				60, 22, _("Spot"));
			btnSpot->tooltip(_("Spot tone on/off"));
			btnSpot->callback((Fl_Callback*)cb_btnSpot);
			btnSpot->value(progStatus.cw_spot);

			btn_enable_keyer = new Fl_Check_Button(
				btnSpot->x(), 62, 60, 15, _("Keyer"));
			btn_enable_keyer->tooltip(_("Enable internal keyer"));
			btn_enable_keyer->down_box(FL_DOWN_BOX);
			btn_enable_keyer->callback((Fl_Callback*)cb_btn_enable_keyer);
			btn_enable_keyer->value(progStatus.enable_keyer);

		genericCW->end();

		genericQSK = new Fl_Group(0, 24, small_mainW, gph - 24, _("QSK"));
			genericQSK->color(FL_LIGHT1);
			genericQSK->selection_color(FL_LIGHT1);
			genericQSK->hide();

			btnBreakIn = new Fl_Button(
				5, 40, 60, 22, _("QSK"));
			btnBreakIn->tooltip(_("Full break in"));
			btnBreakIn->callback((Fl_Callback*)cb_btnBreakIn);
			btnBreakIn->value(progStatus.break_in);

			spnr_cw_qsk = new Hspinner(
				btnBreakIn->x() + btnBreakIn->w() + 4, 40,
				85, 22, _("Bk-in Dly"), 16);
			spnr_cw_qsk->tooltip(_("msec"));
			spnr_cw_qsk->type(1);
			spnr_cw_qsk->minimum(0);
			spnr_cw_qsk->maximum(100);
			spnr_cw_qsk->step(1);
			spnr_cw_qsk->value(20);
			spnr_cw_qsk->callback((Fl_Callback*)cb_spnr_cw_qsk);
			spnr_cw_qsk->value(progStatus.cw_qsk);
			spnr_cw_qsk->align(FL_ALIGN_BOTTOM | FL_ALIGN_CENTER);

			spnr_cw_delay = new Hspinner(
				spnr_cw_qsk->x() + spnr_cw_qsk->w() + 4, 40,
				120, 22, _("QSK delay"), 16);
			spnr_cw_delay->tooltip(_("msec"));
			spnr_cw_delay->type(0);
			spnr_cw_delay->minimum(30);
			spnr_cw_delay->maximum(500);
			spnr_cw_delay->step(10);
			spnr_cw_delay->callback((Fl_Callback*)cb_spnr_cw_delay);
			spnr_cw_delay->value(progStatus.cw_delay);
			spnr_cw_delay->align(FL_ALIGN_BOTTOM | FL_ALIGN_CENTER);

		genericQSK->end();

		genericVOX = new Fl_Group(0, 24, small_mainW, gph - 24, _("Vox"));
			genericVOX->hide();

			spnr_vox_gain = new Hspinner(
				4, 40,
				80, 22, _("gain"), 16);
			spnr_vox_gain->type(1);
			spnr_vox_gain->minimum(0);
			spnr_vox_gain->maximum(100);
			spnr_vox_gain->step(1);
			spnr_vox_gain->tooltip(_("VOX gain"));
			spnr_vox_gain->callback((Fl_Callback*)cb_spnr_vox_gain);
			spnr_vox_gain->value(progStatus.vox_gain);
			spnr_vox_gain->align(FL_ALIGN_CENTER | FL_ALIGN_BOTTOM);

			spnr_anti_vox = new Hspinner(
				spnr_vox_gain->x() + spnr_vox_gain->w() + 10, 40,
				80, 22, _("anti"), 16);
			spnr_anti_vox->type(1);
			spnr_anti_vox->minimum(0);
			spnr_anti_vox->maximum(100);
			spnr_anti_vox->step(1);
			spnr_anti_vox->tooltip(_("Anti VOX gain"));
			spnr_anti_vox->callback((Fl_Callback*)cb_spnr_anti_vox);
			spnr_anti_vox->value(progStatus.vox_anti);
			spnr_anti_vox->align(FL_ALIGN_CENTER | FL_ALIGN_BOTTOM);

			spnr_vox_hang = new Hspinner(
				spnr_anti_vox->x() + spnr_anti_vox->w() + 10, 40,
				80, 22, _("hang"), 16);
			spnr_vox_hang->type(1);
			spnr_vox_hang->minimum(0);
			spnr_vox_hang->maximum(100);
			spnr_vox_hang->step(1);
			spnr_vox_hang->tooltip(_("VOX hang time"));
			spnr_vox_hang->callback((Fl_Callback*)cb_spnr_vox_hang);
			spnr_vox_hang->value(progStatus.vox_hang);
			spnr_vox_hang->align(FL_ALIGN_CENTER | FL_ALIGN_BOTTOM);

			btn_vox = new Fl_Light_Button(
				spnr_vox_hang->x() + spnr_vox_hang->w() + 10, 40,
				50, 22, _("VOX"));
			btn_vox->callback((Fl_Callback*)cb_btn_vox);
			btn_vox->value(progStatus.vox_onoff);
			btn_vox->tooltip(_("VOX on/off"));

			btn_vox_on_dataport = new Fl_Check_Button(
				btn_vox->x() + btn_vox->w() + 10, 44,
				50, 15, _("Data port"));
			btn_vox_on_dataport->tooltip(_("Data port signal triggers VOX"));
			btn_vox_on_dataport->down_box(FL_DOWN_BOX);
			btn_vox_on_dataport->callback((Fl_Callback*)cb_btn_vox_on_dataport);
			btn_vox_on_dataport->value(progStatus.vox_on_dataport);

		genericVOX->end();

		genericSpeech = new Fl_Group(0, 24, small_mainW, gph - 24, _("Spch"));
			genericSpeech->hide();

			btnCompON = new Fl_Light_Button(215, 40, 70, 22, _("On"));
			btnCompON->callback((Fl_Callback*)cb_btnCompON);
			btnCompON->value(progStatus.compON);
			btnCompON->tooltip(_("Compression on/off"));

			spnr_compression = new Hspinner(138, 40, 70, 22, _("Comp"));
			spnr_compression->type(1);
			spnr_compression->minimum(0);
			spnr_compression->maximum(100);
			spnr_compression->step(1);
			spnr_compression->tooltip(_("Compression level"));
			spnr_compression->callback((Fl_Callback*)cb_spnr_compression);
			spnr_compression->value(progStatus.compression);

		genericSpeech->end();

		genericRx = new Fl_Group(0, 24, small_mainW, gph - 24, _("RX"));
			genericRx->hide();

			sldr_nb_level = new Fl_Wheel_Value_Slider(5, 40, 200, 20, _("NB level"));
			sldr_nb_level->tooltip(_("Noise Blanker level"));
			sldr_nb_level->type(5);
			sldr_nb_level->box(FL_THIN_DOWN_BOX);
			sldr_nb_level->color(FL_BACKGROUND_COLOR);
			sldr_nb_level->selection_color(FL_BACKGROUND_COLOR);
			sldr_nb_level->labeltype(FL_NORMAL_LABEL);
			sldr_nb_level->labelfont(0);
			sldr_nb_level->labelsize(14);
			sldr_nb_level->labelcolor(FL_FOREGROUND_COLOR);
			sldr_nb_level->minimum(0);
			sldr_nb_level->maximum(0);
			sldr_nb_level->step(1);
			sldr_nb_level->callback((Fl_Callback*)cb_sldr_nb_level);
			sldr_nb_level->align(Fl_Align(FL_ALIGN_BOTTOM));
			sldr_nb_level->when(FL_WHEN_CHANGED);

			sldr_nb_level->value(progStatus.nb_level);

//			cbo_agc_level = new Fl_ComboBox(123, 40, 80, 22, _("AGC"));
//			cbo_agc_level->tooltip(_("AGC level"));
//			cbo_agc_level->box(FL_DOWN_BOX);
//			cbo_agc_level->color(FL_BACKGROUND_COLOR);
//			cbo_agc_level->selection_color(FL_BACKGROUND_COLOR);
//			cbo_agc_level->labeltype(FL_NORMAL_LABEL);
//			cbo_agc_level->labelfont(0);
//			cbo_agc_level->labelsize(14);
//			cbo_agc_level->labelcolor(FL_FOREGROUND_COLOR);
//			cbo_agc_level->callback((Fl_Callback*)cb_cbo_agc_level);
//			cbo_agc_level->align(Fl_Align(FL_ALIGN_BOTTOM));
//			cbo_agc_level->when(FL_WHEN_CHANGED);
//			cbo_agc_level->end();

			spnr_bpf_center = new Hspinner(234, 40, 96, 22, _("BPF Center Freq"));
			spnr_bpf_center->tooltip(_("Bandpass Filter Center Freq"));
			spnr_bpf_center->type(1);
			spnr_bpf_center->minimum(600);
			spnr_bpf_center->maximum(2500);
			spnr_bpf_center->step(10);
			spnr_bpf_center->value(1500);
			spnr_bpf_center->callback((Fl_Callback*)cb_spnr_bpf_center);
			spnr_bpf_center->align(Fl_Align(34));
			spnr_bpf_center->value(progStatus.bpf_center);

			btn_use_bpf_center = new Fl_Check_Button(361, 40, 50, 15, _("On"));
			btn_use_bpf_center->tooltip(_("Use Filter Center Freq Adj"));
			btn_use_bpf_center->down_box(FL_DOWN_BOX);
			btn_use_bpf_center->value(1);
			btn_use_bpf_center->callback((Fl_Callback*)cb_btn_use_bpf_center);
			btn_use_bpf_center->value(progStatus.use_bpf_center);

		genericRx->end();

		genericMisc = new Fl_Group(0, 24, small_mainW, gph - 24, _("Misc"));
			genericMisc->hide();

			spnr_vfo_adj = new Hspinner(24, 40, 96, 22, _("Vfo Adj"));
			spnr_vfo_adj->type(1);
			spnr_vfo_adj->callback((Fl_Callback*)cb_spnr_vfo_adj);
			spnr_vfo_adj->align(Fl_Align(34));
			spnr_vfo_adj->step(1);
			spnr_vfo_adj->value(progStatus.vfo_adj);

			spnr_line_out = new Hspinner(146, 40, 70, 22, _("line out"));
			spnr_line_out->type(1);
			spnr_line_out->minimum(0);
			spnr_line_out->maximum(100);
			spnr_line_out->step(1);
			spnr_line_out->value(20);
			spnr_line_out->align(Fl_Align(34));
			spnr_line_out->callback((Fl_Callback*)cb_spnr_line_out);
			spnr_line_out->value(progStatus.line_out);

			btnSpecial = new Fl_Light_Button(242, 30, 74, 20, _("Special"));
			btnSpecial->callback((Fl_Callback*)cb_btnSpecial);

			btn_ext_tuner = new Fl_Check_Button(242, 60, 74, 15, _("Ext tuner"));
			btn_ext_tuner->tooltip(_("use external auto tuner"));
			btn_ext_tuner->down_box(FL_DOWN_BOX);
			btn_ext_tuner->callback((Fl_Callback*)cb_btn_ext_tuner);
			btn_ext_tuner->value(progStatus.external_tuner);

			btn_xcvr_auto_on = new Fl_Check_Button(325, 38, 95, 15, _("Rig autOn"));
			btn_xcvr_auto_on->tooltip(_("Auto Turn Rig On with Flrig startup"));
			btn_xcvr_auto_on->down_box(FL_DOWN_BOX);
			btn_xcvr_auto_on->callback((Fl_Callback*)cb_btn_xcvr_auto_on);
			btn_xcvr_auto_on->value(progStatus.xcvr_auto_on);

			btn_xcvr_auto_off = new Fl_Check_Button(325, 60, 95, 15, _("Rig autOff"));
			btn_xcvr_auto_off->tooltip(_("Auto Turn Rig Off with Flrig exit"));
			btn_xcvr_auto_off->down_box(FL_DOWN_BOX);
			btn_xcvr_auto_off->callback((Fl_Callback*)cb_btn_xcvr_auto_off);
			btn_xcvr_auto_off->value(progStatus.xcvr_auto_off);

		genericMisc->end();

		genericUser_1 = new Fl_Group(0, 24, small_mainW, gph - 24, _("User A"));
			btnUser1 = new Fl_Button(5, genericUser_1->y() + 12, 100, 20, "USER 1");
			btnUser1->callback((Fl_Callback*)cb_btnUser, (void*)1);

			btnUser2 = new Fl_Button(110, genericUser_1->y() + 12, 100, 20, "USER 2");
			btnUser2->callback((Fl_Callback*)cb_btnUser, (void*)2);

			btnUser3 = new Fl_Button(215, genericUser_1->y() + 12, 100, 20, "USER 3");
			btnUser3->callback((Fl_Callback*)cb_btnUser, (void*)3);

			btnUser4 = new Fl_Button(320, genericUser_1->y() + 12, 100, 20, "USER 4");
			btnUser4->callback((Fl_Callback*)cb_btnUser, (void*)4);

			btnUser5 = new Fl_Button(5, genericUser_1->y() + 36, 100, 20, "USER 5");
			btnUser5->callback((Fl_Callback*)cb_btnUser, (void*)5);

			btnUser6 = new Fl_Button(110, genericUser_1->y() + 36, 100, 20, "USER 6");
			btnUser6->callback((Fl_Callback*)cb_btnUser, (void*)6);

			btnUser7 = new Fl_Button(215, genericUser_1->y() + 36, 100, 20, "USER 7");
			btnUser7->callback((Fl_Callback*)cb_btnUser, (void*)7);

			btnUser8 = new Fl_Button(320, genericUser_1->y() + 36, 100, 20, "USER 8");
			btnUser8->callback((Fl_Callback*)cb_btnUser, (void*)8);

		genericUser_1->end();

		genericUser_2 = new Fl_Group(0, 24, small_mainW, gph - 24, _("User B"));
			btnUser9 = new Fl_Button(5, genericUser_2->y() + 12, 100, 20, "USER 9");
			btnUser9->callback((Fl_Callback*)cb_btnUser, (void*)9);

			btnUser10 = new Fl_Button(110, genericUser_2->y() + 12, 100, 20, "USER 10");
			btnUser10->callback((Fl_Callback*)cb_btnUser, (void*)10);

			btnUser11 = new Fl_Button(215, genericUser_2->y() + 12, 100, 20, "USER 11");
			btnUser11->callback((Fl_Callback*)cb_btnUser, (void*)11);

			btnUser12 = new Fl_Button(320, genericUser_2->y() + 12, 100, 20, "USER 12");
			btnUser12->callback((Fl_Callback*)cb_btnUser, (void*)12);

			btnUser13 = new Fl_Button(5, genericUser_2->y() + 36, 100, 20, "USER 13");
			btnUser13->callback((Fl_Callback*)cb_btnUser, (void*)13);

			btnUser14 = new Fl_Button(110, genericUser_2->y() + 36, 100, 20, "USER 14");
			btnUser14->callback((Fl_Callback*)cb_btnUser, (void*)14);

			btnUser15 = new Fl_Button(215, genericUser_2->y() + 36, 100, 20, "USER 15");
			btnUser15->callback((Fl_Callback*)cb_btnUser, (void*)15);

			btnUser16 = new Fl_Button(320, genericUser_2->y() + 36, 100, 20, "USER 16");
			btnUser16->callback((Fl_Callback*)cb_btnUser, (void*)16);

		genericUser_2->end();

		tab7610 = new Fl_Group(0, 24, small_mainW, gph - 24, _("IC7610"));
			ic7610att = new Fl_ComboBox(5, tab7610->y() + 10, 70, 20, "Att level");
			ic7610att->add("OFF|3db|6db|9db|12db|15db|18db|21db|24db|27db|30db|33db|36db|39db|42db|45db|");
			ic7610att->align(FL_ALIGN_RIGHT);
			ic7610att->index(progStatus.index_ic7610att);
			ic7610att->readonly();
			ic7610att->callback((Fl_Callback *)cb_ic7610_att);

			ic7610dual_watch = new Fl_Light_Button(150, tab7610->y() + 10, 120, 20, "Dual Watch");
			ic7610dual_watch->value(progStatus.dual_watch);
			ic7610dual_watch->callback((Fl_Callback *)cb_dual_watch);

			ic7610digi_sel_on_off = new Fl_Light_Button(5, tab7610->y() + 35, 70, 20, "Digi-Sel");
			ic7610digi_sel_on_off->value(progStatus.digi_sel_on_off);
			ic7610digi_sel_on_off->callback((Fl_Callback *)cb_digi_sel_on_off);

			ic7610_digi_sel_val = new Fl_Wheel_Value_Slider(80, tab7610->y() + 35, small_mainW - 85, 20, "");
			ic7610_digi_sel_val->tooltip(_("Digi-Sel value"));
			ic7610_digi_sel_val->type(5);
			ic7610_digi_sel_val->box(FL_THIN_DOWN_BOX);
			ic7610_digi_sel_val->color(FL_BACKGROUND_COLOR);
			ic7610_digi_sel_val->selection_color(FL_BACKGROUND_COLOR);
			ic7610_digi_sel_val->labeltype(FL_NORMAL_LABEL);
			ic7610_digi_sel_val->labelfont(0);
			ic7610_digi_sel_val->labelsize(14);
			ic7610_digi_sel_val->labelcolor(FL_FOREGROUND_COLOR);
			ic7610_digi_sel_val->minimum(0);
			ic7610_digi_sel_val->maximum(255);
			ic7610_digi_sel_val->step(1);
			ic7610_digi_sel_val->callback((Fl_Callback*)cb_digi_sel_val);
			ic7610_digi_sel_val->align(Fl_Align(FL_ALIGN_BOTTOM));
			ic7610_digi_sel_val->when(FL_WHEN_CHANGED);
			ic7610_digi_sel_val->value(progStatus.digi_sel_val);

		tab7610->end();

	tabsGeneric->resizable(genericUser_2);
	tabsGeneric->end();

	grpTABS->end();
	w->end();
	return w;
}

Fl_Double_Window* Small_rig_window() {
	Fl_Double_Window* w = new Fl_Double_Window(small_mainW, small_mainH, _("Flrig"));
	w->align(Fl_Align(FL_ALIGN_CLIP|FL_ALIGN_INSIDE));

	hidden_tabs = new Fl_Group(0,0,small_mainW, small_mainH);
	hidden_tabs->hide();
	hidden_tabs->end();

	grp_menu = new Fl_Group(0,0,small_mainW,small_menuH);

		small_menu = new Fl_Menu_Bar(0, 0, small_mainW - 64, small_menuH);
		small_menu->textsize(12);
		small_menu->menu(menu_small_menu);

		Fl_Menu_Item * mnu = getMenuItem(_("Tooltips"), menu_small_menu);
		if (mnu) {
			progStatus.tooltips ? mnu->set() : mnu->clear();
			mnuTooltips = mnu;
		}

		mnu = getMenuItem(_("Small sliders"), menu_small_menu);
		if (mnu) {
			progStatus.schema ? mnu->set() : mnu->clear();
			mnuSchema = mnu;
		}

		Fl_Group *mnu_box = new Fl_Group(small_mainW-64, 0, 64, small_menuH);
		mnu_box->box(FL_UP_BOX);

			tcpip_box = new Fl_Group(small_mainW-62, 2, 60, 18);
			tcpip_box->box(FL_FLAT_BOX);

				tcpip_menu_box = new Fl_Box(small_mainH - 62, 3, 16, 16);
				tcpip_menu_box->box(FL_DIAMOND_BOX);
				tcpip_menu_box->color(FL_GREEN);
				Fl_Box *tcpip_menu_label = new Fl_Box(small_mainH - 62 + 18, 3, 64 - 22, 16, _("tcpip"));
				tcpip_menu_label->box(FL_FLAT_BOX);
				tcpip_menu_label->align(FL_ALIGN_CENTER);
				tcpip_menu_label->tooltip(_("lit when connected to remote tcpip"));

			tcpip_box->end();
			tcpip_box->hide();

		mnu_box->end();
		grp_menu->resizable(small_menu);

	grp_menu->end();

	txt_encA = new Fl_Output( small_mainW - 90, 1, 90, 20, "");
	txt_encA->box(FL_THIN_DOWN_BOX);
	txt_encA->align(20);
	txt_encA->hide();

	main_group = small_main_group(
		0, small_menuH,
		small_mainW, small_mainH - small_menuH);

	grpInitializing = new Fl_Group(
		0, 0, small_mainW, small_mainH);

		grpInitializing->box(FL_FLAT_BOX);
		grpInitializing->color(FL_LIGHT2);

		progress = new Fl_Progress(
			small_mainW / 4, small_mainH / 2,
			small_mainW / 2, 20, "Initializing");
		progress->maximum(100);
		progress->minimum(0);
		progress->labelcolor(FL_DARK_RED);
		progress->labelsize(14);
		progress->align(Fl_Align(FL_ALIGN_TOP));
		progress->selection_color(FL_GREEN);

		filler = new Fl_Box(
			0, small_mainH - 5,
			small_mainW, 4);
		filler->box(FL_FLAT_BOX);
		filler->color(FL_LIGHT2);

		grpInitializing->end();
	grpInitializing->resizable(filler);

	grpInitializing->show();

	w->end();
	return w;
}
