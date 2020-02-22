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
#include <iostream>
#include "gettext.h"
#include "rigpanel.h"
#include "images.h"
#include "rig.h"
#include "rigs.h"
#include "status.h"
#include "support.h"
#include "K3_ui.h"
#include "KX3_ui.h"
#include "socket_io.h"
#include "hspinner.h"
#include "ui.h"

Fl_Light_Button *btnPOWER = (Fl_Light_Button *)0;

// controls for touch screen interface
Hspinner *spnrPOWER = (Hspinner *)0;
Hspinner *spnrVOLUME = (Hspinner *)0;
Hspinner *spnrRFGAIN = (Hspinner *)0;
Hspinner *spnrSQUELCH = (Hspinner *)0;
Hspinner *spnrNR = (Hspinner *)0;
Hspinner *spnrIFSHIFT = (Hspinner *)0;
Hspinner *spnrINNER = (Hspinner *)0;
Hspinner *spnrOUTER = (Hspinner *)0;
Hspinner *spnrNOTCH = (Hspinner *)0;
Hspinner *spnrMICGAIN = (Hspinner *)0;

static const int freq_sel_widths[]={110, 70, 70, 0};
static Fl_Double_Window *meter_scale_dialog = (Fl_Double_Window *)0;
static Fl_Double_Window *meter_filters = (Fl_Double_Window *)0;

Fl_Group* grp_menu = (Fl_Group *)0;

Fl_Menu_Bar *small_menu=(Fl_Menu_Bar *)0;
Fl_Menu_Bar *wide_menu=(Fl_Menu_Bar *)0;

Fl_Menu_Item *mnuExit = (Fl_Menu_Item *)0;
Fl_Menu_Item *mnuConfig = (Fl_Menu_Item *)0;
Fl_Menu_Item *mnuTooltips = (Fl_Menu_Item *)0;
Fl_Menu_Item *mnuSchema = (Fl_Menu_Item *)0;
Fl_Menu_Item *mnuColorConfig = (Fl_Menu_Item *)0;
Fl_Menu_Item *mnu_meter_filtering = (Fl_Menu_Item *)0;
Fl_Menu_Item *mnuConfigXcvr = (Fl_Menu_Item *)0;
Fl_Menu_Item *save_me = (Fl_Menu_Item *)0;
Fl_Menu_Item *mnuHelp = (Fl_Menu_Item *)0;
Fl_Menu_Item *mnuOnLineHelp = (Fl_Menu_Item *)0;
Fl_Menu_Item *mnuAbout = (Fl_Menu_Item *)0;
Fl_Box *tcpip_menu_box = (Fl_Box *)0;
Fl_Group *tcpip_box = (Fl_Group *)0;

Fl_Box *labelMEMORY = (Fl_Box *)0;

Fl_Output *txt_encA=(Fl_Output *)0;

Fl_Group *grpMeters = (Fl_Group *)0;
Fl_Box *scaleSmeter=(Fl_Box *)0;

Fl_Light_Button *btnPreamp = (Fl_Light_Button *)0;
Fl_Light_Button *btnNOISE = (Fl_Light_Button *)0;
Fl_Light_Button *btnAutoNotch = (Fl_Light_Button *)0;
Fl_Group *grp_row1b2 = (Fl_Group *)0;
Fl_Button *btnAswapB = (Fl_Button *)0;
Fl_Button *btn_K3_swapAB = (Fl_Button *)0;
Fl_Button *btn_KX3_swapAB = (Fl_Button *)0;
Fl_Light_Button *btnB = (Fl_Light_Button *)0;
Fl_Light_Button *btnSplit = (Fl_Light_Button *)0;
Fl_Light_Button *btnPTT = (Fl_Light_Button *)0;
Fl_Box *bx_row1_expander2=(Fl_Box *)0;
Fl_Group *grp_row2 = (Fl_Group *)0;
Fl_Group *grp_row2a = (Fl_Group *)0;
Fl_Wheel_Value_Slider *sldrSQUELCH = (Fl_Wheel_Value_Slider *)0;
Fl_Wheel_Value_Slider *sldrMICGAIN = (Fl_Wheel_Value_Slider *)0;
Fl_Box *bx_sep2a=(Fl_Box *)0;
Fl_Group *grp_row2b = (Fl_Group *)0;
Fl_Light_Button *btnIFsh = (Fl_Light_Button *)0;
Fl_Wheel_Value_Slider *sldrIFSHIFT = (Fl_Wheel_Value_Slider *)0;

Fl_Light_Button *btnLOCK = (Fl_Light_Button *)0;
Fl_Button *btnCLRPBT = (Fl_Light_Button *)0;
Fl_Wheel_Value_Slider *sldrINNER = (Fl_Wheel_Value_Slider *)0;
Fl_Wheel_Value_Slider *sldrOUTER = (Fl_Wheel_Value_Slider *)0;

Fl_Light_Button *btnNR = (Fl_Light_Button *)0;
Fl_Wheel_Value_Slider *sldrNR = (Fl_Wheel_Value_Slider *)0;
Fl_Box *bx_sep2b=(Fl_Box *)0;
Fl_Group *grp_row2c = (Fl_Group *)0;
Fl_Light_Button *btnNotch = (Fl_Light_Button *)0;
Fl_Wheel_Value_Slider *sldrNOTCH = (Fl_Wheel_Value_Slider *)0;
Fl_Light_Button *btnAGC = (Fl_Light_Button *)0;
Fl_Wheel_Value_Slider *sldrRFGAIN = (Fl_Wheel_Value_Slider *)0;
Fl_Box *bx_sep2c=(Fl_Box *)0;
Fl_Group *grp_row2d = (Fl_Group *)0;
Fl_Wheel_Value_Slider *sldrPOWER = (Fl_Wheel_Value_Slider *)0;
Fl_Light_Button *btnVol = (Fl_Light_Button *)0;
Fl_Wheel_Value_Slider *sldrVOLUME = (Fl_Wheel_Value_Slider *)0;

Fl_Group *grpTABS = (Fl_Group *)0;

Fl_Group *hidden_tabs = (Fl_Group *)0;

Fl_Tabs *tabsGeneric=(Fl_Tabs *)0;

Fl_Group *tab_yaesu_bands = (Fl_Group *)0;
Fl_Button *btn_yaesu_select_1 = (Fl_Button *)0;
Fl_Button *btn_yaesu_select_2 = (Fl_Button *)0;
Fl_Button *btn_yaesu_select_3 = (Fl_Button *)0;
Fl_Button *btn_yaesu_select_4 = (Fl_Button *)0;
Fl_Button *btn_yaesu_select_5 = (Fl_Button *)0;
Fl_Button *btn_yaesu_select_6 = (Fl_Button *)0;
Fl_Button *btn_yaesu_select_7 = (Fl_Button *)0;
Fl_Button *btn_yaesu_select_8 = (Fl_Button *)0;
Fl_Button *btn_yaesu_select_9 = (Fl_Button *)0;
Fl_Button *btn_yaesu_select_10 = (Fl_Button *)0;
Fl_Button *btn_yaesu_select_11 = (Fl_Button *)0;
Fl_ComboBox *op_yaesu_select60 = (Fl_ComboBox *)0;

Fl_Group *tab_FT8n_bands = (Fl_Group *)0;
Fl_Button *btn_FT8n_select_1 = (Fl_Button *)0;
Fl_Button *btn_FT8n_select_2 = (Fl_Button *)0;
Fl_Button *btn_FT8n_select_3 = (Fl_Button *)0;
Fl_Button *btn_FT8n_select_4 = (Fl_Button *)0;
Fl_Button *btn_FT8n_select_5 = (Fl_Button *)0;
Fl_Button *btn_FT8n_select_6 = (Fl_Button *)0;
Fl_Button *btn_FT8n_select_7 = (Fl_Button *)0;
Fl_Button *btn_FT8n_select_8 = (Fl_Button *)0;
Fl_Button *btn_FT8n_select_9 = (Fl_Button *)0;
Fl_Button *btn_FT8n_select_10 = (Fl_Button *)0;
Fl_Button *btn_FT8n_select_11 = (Fl_Button *)0;
Fl_Button *btn_FT8n_select_12 = (Fl_Button *)0;
Fl_Button *btn_FT8n_select_13 = (Fl_Button *)0;

Fl_Group *tab_FT8n_CTCSS = (Fl_Group *)0;
Fl_PL_tone *choice_FT8n_tTONE = (Fl_PL_tone *)0;
Fl_PL_tone *choice_FT8n_rTONE = (Fl_PL_tone *)0;
Fl_Button *setTONES = (Fl_Button *)0;
Fl_ComboBox *FMoffset = (Fl_ComboBox *)0;
Fl_Button *setOFFSET = (Fl_Button *)0;
cFreqControl *FMoff_freq = (cFreqControl *)0;

Fl_Group *tab_icom_bands = (Fl_Group *)0;
Fl_Button *btn_icom_select_1 = (Fl_Button *)0;
Fl_Button *btn_icom_select_2 = (Fl_Button *)0;
Fl_Button *btn_icom_select_3 = (Fl_Button *)0;
Fl_Button *btn_icom_select_4 = (Fl_Button *)0;
Fl_Button *btn_icom_select_5 = (Fl_Button *)0;
Fl_Button *btn_icom_select_6 = (Fl_Button *)0;
Fl_Button *btn_icom_select_7 = (Fl_Button *)0;
Fl_Button *btn_icom_select_8 = (Fl_Button *)0;
Fl_Button *btn_icom_select_9 = (Fl_Button *)0;
Fl_Button *btn_icom_select_10 = (Fl_Button *)0;
Fl_Button *btn_icom_select_11 = (Fl_Button *)0;
Fl_Button *btn_icom_select_12 = (Fl_Button *)0;
Fl_Button *btn_icom_select_13 = (Fl_Button *)0;

Fl_PL_tone *choice_tTONE = (Fl_PL_tone *)0;
Fl_PL_tone *choice_rTONE = (Fl_PL_tone *)0;

Fl_Group *genericCW = (Fl_Group *)0;
Fl_Light_Button *btnSpot = (Fl_Light_Button *)0;
Hspinner *spnr_cw_spot_tone = (Hspinner *)0;
Hspinner *spnr_cw_qsk = (Hspinner *)0;
Hspinner *spnr_cw_weight = (Hspinner *)0;
Hspinner *spnr_cw_wpm = (Hspinner *)0;
Fl_Check_Button *btn_enable_keyer = (Fl_Check_Button *)0;

Fl_Group *genericQSK = (Fl_Group *)0;
Fl_Button *btnBreakIn = (Fl_Button *)0;
Hspinner *spnr_cw_delay = (Hspinner *)0;

Fl_Group *genericVOX = (Fl_Group *)0;
Hspinner *spnr_vox_gain = (Hspinner *)0;
Hspinner *spnr_anti_vox = (Hspinner *)0;
Hspinner *spnr_vox_hang = (Hspinner *)0;
Fl_Light_Button *btn_vox = (Fl_Light_Button *)0;
Fl_Check_Button *btn_vox_on_dataport = (Fl_Check_Button *)0;

Fl_Group *genericSpeech = (Fl_Group *)0;
Fl_Light_Button *btnCompON = (Fl_Light_Button *)0;
Hspinner *spnr_compression = (Hspinner *)0;

Fl_Group *genericRx = (Fl_Group *)0;
Fl_Wheel_Value_Slider *sldr_nb_level = (Fl_Wheel_Value_Slider *)0;
//Fl_ComboBox *cbo_agc_level = (Fl_ComboBox *)0;
Hspinner *spnr_bpf_center = (Hspinner *)0;

Fl_Button *btnALC_SWR = (Fl_Button *)0;
Fl_SigBar *sldrRcvSignal=(Fl_SigBar *)0;
Fl_SigBar *sldrALC=(Fl_SigBar *)0;
Fl_SigBar *sldrSWR=(Fl_SigBar *)0;
Fl_SigBar *sldrFwdPwr=(Fl_SigBar *)0;
Fl_Button *scalePower = (Fl_Button *)0;
Fl_Box *meter_fill_box=(Fl_Box *)0;
cFreqControl *FreqDispA=(cFreqControl *)0;
cFreqControl *FreqDispB=(cFreqControl *)0;
Fl_Group *grp_row0a = (Fl_Group *)0;
Fl_Light_Button *btnA = (Fl_Light_Button *)0;
Fl_Button *btnTune = (Fl_Button *)0;
Fl_Light_Button *btn_tune_on_off = (Fl_Light_Button *)0;
Fl_Box *bx_row0_expander=(Fl_Box *)0;
Fl_Button *btn_show_controls = (Fl_Button *)0;
Fl_Group *grp_row1 = (Fl_Group *)0;
Fl_Group *grp_row1a = (Fl_Group *)0;
Fl_ComboBox *opBW = (Fl_ComboBox *)0;
Fl_ComboBox *opDSP_lo = (Fl_ComboBox *)0;
Fl_Button *btnDSP = (Fl_Button *)0;
Fl_ComboBox *opDSP_hi = (Fl_ComboBox *)0;
Fl_Button *btnFILT= (Fl_Button*)0;
Fl_ComboBox *opMODE = (Fl_ComboBox *)0;
Fl_Group *grp_row1b = (Fl_Group *)0;
Fl_Group *grp_row1b1 = (Fl_Group *)0;
Fl_Light_Button *btnAttenuator = (Fl_Light_Button *)0;

Fl_Check_Button *btn_use_bpf_center = (Fl_Check_Button *)0;
Fl_Group *genericMisc = (Fl_Group *)0;
Hspinner *spnr_vfo_adj = (Hspinner *)0;
Hspinner *spnr_line_out = (Hspinner *)0;
Fl_Light_Button *btnSpecial = (Fl_Light_Button *)0;
Fl_Check_Button *btn_ext_tuner = (Fl_Check_Button *)0;
Fl_Check_Button *btn_xcvr_auto_on = (Fl_Check_Button *)0;
Fl_Check_Button *btn_xcvr_auto_off = (Fl_Check_Button *)0;

Fl_Group  *genericUser_1 = (Fl_Group *)0;
Fl_Button *btnUser1 = (Fl_Button*)0;
Fl_Button *btnUser2 = (Fl_Button*)0;
Fl_Button *btnUser3 = (Fl_Button*)0;
Fl_Button *btnUser4 = (Fl_Button*)0;
Fl_Button *btnUser5 = (Fl_Button*)0;
Fl_Button *btnUser6 = (Fl_Button*)0;
Fl_Button *btnUser7 = (Fl_Button*)0;
Fl_Button *btnUser8 = (Fl_Button*)0;

Fl_Group  *genericUser_2 = (Fl_Group *)0;
Fl_Button *btnUser9 = (Fl_Button*)0;
Fl_Button *btnUser10 = (Fl_Button*)0;
Fl_Button *btnUser11 = (Fl_Button*)0;
Fl_Button *btnUser12 = (Fl_Button*)0;
Fl_Button *btnUser13 = (Fl_Button*)0;
Fl_Button *btnUser14 = (Fl_Button*)0;
Fl_Button *btnUser15 = (Fl_Button*)0;
Fl_Button *btnUser16 = (Fl_Button*)0;

Fl_Group *genericRXB = (Fl_Group *)0;
Hspinner *cntRIT = (Hspinner *)0;
Hspinner *cntXIT = (Hspinner *)0;
Hspinner *cntBFO = (Hspinner *)0;
Fl_Group *genericAux = (Fl_Group *)0;
Fl_Box *boxControl=(Fl_Box *)0;
Fl_Light_Button *btnAuxRTS = (Fl_Light_Button *)0;

Fl_Group *tab7610 = (Fl_Group *)0;
Fl_ComboBox *ic7610att = (Fl_ComboBox *)0;
Fl_Light_Button *ic7610dual_watch = (Fl_Light_Button *)0;
Fl_Light_Button *ic7610digi_sel_on_off = (Fl_Light_Button *)0;
Fl_Wheel_Value_Slider *ic7610_digi_sel_val = (Fl_Wheel_Value_Slider *)0;

Fl_Light_Button *btnAuxDTR = (Fl_Light_Button *)0;
Fl_Light_Button *btnDataPort = (Fl_Light_Button *)0;

Fl_Tabs *tabs550=(Fl_Tabs *)0;
Fl_Group *tt550_CW = (Fl_Group *)0;
Hspinner *spnr_tt550_cw_wpm = (Hspinner *)0;
Hspinner *spnr_tt550_cw_weight = (Hspinner *)0;
Hspinner *spnr_tt550_cw_qsk = (Hspinner *)0;
Hspinner *spnr_tt550_cw_vol = (Hspinner *)0;
Hspinner *spnr_tt550_cw_spot = (Hspinner *)0;
Fl_Check_Button *btn_tt550_enable_keyer = (Fl_Check_Button *)0;
Fl_Group *tt550_VOX = (Fl_Group *)0;
Hspinner *spnr_tt550_vox_gain = (Hspinner *)0;
Hspinner *spnr_tt550_anti_vox = (Hspinner *)0;
Hspinner *spnr_tt550_vox_hang = (Hspinner *)0;
Fl_Light_Button *btn_tt550_vox = (Fl_Light_Button *)0;
Fl_Group *tt550_Speech = (Fl_Group *)0;
Fl_Light_Button *btn_tt550_CompON = (Fl_Light_Button *)0;
Hspinner *spnr_tt550_compression = (Hspinner *)0;
Hspinner *spnr_tt550_mon_vol = (Hspinner *)0;
Fl_Group *tt550_Audio = (Fl_Group *)0;
Hspinner *spnr_tt550_line_out = (Hspinner *)0;
Fl_Check_Button *btnAccInp = (Fl_Check_Button *)0;
Fl_Group *tt550_Rx = (Fl_Group *)0;
Fl_ComboBox *cbo_tt550_nb_level = (Fl_ComboBox *)0;
Fl_ComboBox *cbo_tt550_agc_level = (Fl_ComboBox *)0;
Hspinner *spnr_tt550_vfo_adj = (Hspinner *)0;
Fl_Group *tt550_Tx = (Fl_Group *)0;
Fl_Check_Button *btn_tt550_enable_xmtr = (Fl_Check_Button *)0;
Fl_Check_Button *btn_tt550_enable_tloop = (Fl_Check_Button *)0;
Fl_Check_Button *btn_tt550_tuner_bypass = (Fl_Check_Button *)0;
Fl_ComboBox *op_tt550_XmtBW = (Fl_ComboBox *)0;
Fl_Check_Button *btn_tt550_use_xmt_bw = (Fl_Check_Button *)0;
Fl_Group *tt550_302A = (Fl_Group *)0;
Hspinner *spnr_tt550_keypad_time_out = (Hspinner *)0;
Hspinner *spnr_tt550_encoder_sensitivity = (Hspinner *)0;
Fl_Choice *sel_tt550_encoder_step = (Fl_Choice *)0;
Fl_Group *tt550_302B = (Fl_Group *)0;
Fl_Choice *sel_tt550_F1_func = (Fl_Choice *)0;
Fl_Choice *sel_tt550_F2_func = (Fl_Choice *)0;
Fl_Choice *sel_tt550_F3_func = (Fl_Choice *)0;

Fl_Group *tt550_AT = (Fl_Group *)0;
Fl_Light_Button *tt550_AT_inline = (Fl_Light_Button *)0;
Fl_Light_Button *tt550_AT_Z = (Fl_Light_Button *)0;
Fl_Button *tt550_AT_capUP = (Fl_Button *)0;
Fl_Button *tt550_AT_capDN = (Fl_Button *)0;
Fl_Button *tt550_AT_indUP = (Fl_Button *)0;
Fl_Button *tt550_AT_indDN = (Fl_Button *)0;
Fl_Simple_Counter *tt550_Nsamples = (Fl_Simple_Counter *)0;

Fl_Group *main_group = (Fl_Group *)0;

Fl_Group *grpInitializing = (Fl_Group *)0;

Fl_Progress *progress=(Fl_Progress *)0;

#define RIGHT_OF(widget) (widget->x() + widget->w())

#include "ui_bitmaps.cxx"

static const char *mtr_scales[] = {
"5 watt",
"25 watt",
"50 watt",
"100 watt",
"200 watt",
"Auto scaled"};

void select_power_meter_scales() {
	if (!meter_scale_dialog)
		meter_scale_dialog = power_meter_scale_select();
	meter_scale_dialog->show();
}

static void cb_mnuExit(Fl_Menu_*, void*) {
	cbExit();
}

static void cb_mnuTooltips(Fl_Menu_*, void*) {
	progStatus.tooltips = ! progStatus.tooltips;
}

static void cb_mnuSchema(Fl_Menu_*, void*) {
	progStatus.schema = !progStatus.schema;
	adjust_control_positions();
}

static void cb_mnuColorConfig(Fl_Menu_*, void*) {
	setDisplayColors();
}

static void cb_mnu_meter_filtering(Fl_Menu_*, void*) {
	if (!meter_filters)
		meter_filters = MetersDialog();
	meter_filters->show();
}

static void cb_mnuConfigXcvr(Fl_Menu_*, void*) {
	configXcvr();
}

static void cb_Memory(Fl_Menu_*, void*) {
	openMemoryDialog();
}

static void cb_save_me(Fl_Menu_*, void*) {
	addFreq();
}

static void cb_Events(Fl_Menu_*, void*) {
	cbEventLog();
}

static void cb_Polling(Fl_Menu_*, void*) {
	open_poll_tab();
}

static void cb_mnuTrace(Fl_Menu_*, void*) {
	open_trace_tab();
}

static void cb_mnuCommands(Fl_Menu_ *, void *) {
	open_commands_tab();
}

static void cb_Restore(Fl_Menu_*, void*) {
	open_restore_tab();
}

static void cb_Send(Fl_Menu_*, void*) {
	open_send_command_tab();
}

static void cb_mnuTCPIP(Fl_Menu_*, void*) {
	open_tcpip_tab();
}

//static void cb_mnuXMLRPC(Fl_Menu_*, void*) {
//	open_xmlrpc_tab();
//}

static void cb_mnuPTT(Fl_Menu_*, void*) {
	open_ptt_tab();
}

static void cb_mnuAUX(Fl_Menu_*, void *) {
	open_aux_tab();
}

static void cb_mnuOnLineHelp(Fl_Menu_*, void*) {
	visit_URL((void *)("http://www.w1hkj.com/flrig-help/index.html"));
}

static void cb_mnuAbout(Fl_Menu_*, void*) {
	about();
}

static void cb_btnALC_SWR(Fl_Button*, void*) {
	cbALC_SWR();
}

static void cb_btnAGC(Fl_Button*, void*) {
	cbAGC();
}

static void cb_scalePower(Fl_Button*, void*) {
	if (Fl::event_button() == FL_RIGHT_MOUSE)
		select_power_meter_scales();
}

extern bool useB;

static void cb_btnA(Fl_Light_Button*, void*) {
	if (Fl::event_button() == FL_RIGHT_MOUSE) {
		if (!useB) {
			addFreq();
			btnA->value(1);
			btnB->value(0);
		} else {
			btnA->value(0);
			btnB->value(1);
		}
	} else {
		if (useB) {
			cb_selectA();
		} else {
			if (FreqDispA->is_reversed_colors()) {
				FreqDispA->restore_colors();
				Fl::focus(btnA);
			} else {
				FreqDispA->reverse_colors();
				Fl::focus(FreqDispA);
			}
		}

		btnA->value(1);
		btnB->value(0);
		if (progStatus.UIsize == touch_ui) {
			FreqDispB->hide();
			FreqDispA->show();
		}
	}
}

static void cb_btn_K3_swapAB(Fl_Button*, void*) {
	cb_K3_swapAB();
}

static void cb_btn_KX3_swapAB(Fl_Button*, void*) {
	cb_KX3_swapAB();
}

static void cb_btnB(Fl_Light_Button*, void*) {
	if (Fl::event_button() == FL_RIGHT_MOUSE) {
		if (useB) {
			addFreq();
			btnB->value(1);
			btnA->value(0);
		} else {
			btnB->value(0);
			btnA->value(1);
		}
	} else {
		if (!useB) {
			cb_selectB();
		} else { 
			if (FreqDispB->is_reversed_colors()) {
				FreqDispB->restore_colors();
				Fl::focus(btnB);
			} else {
				FreqDispB->reverse_colors();
				Fl::focus(FreqDispB);
			}
		}
		btnB->value(1);
		btnA->value(0);
		if (progStatus.UIsize == touch_ui) {
			FreqDispA->hide();
			FreqDispB->show();
		}
	}
}

static void cb_btnTune(Fl_Button*, void*) {
	cbTune();
}

static void cb_btn_tune_on_off(Fl_Light_Button *, void *) {
	cb_tune_on_off();
}

static void cb_btn_show_controls(Fl_Button*, void*) {
	show_controls();
}

static void cb_btnFILT(Fl_Button *, void *) {
	selectFILT();
}

static void cb_opBW(Fl_ComboBox*, void*) {
	setBW();
}

static void cb_btnDSP(Fl_Button*, void*) {
	selectDSP();
}

static void cb_opDSP_lo(Fl_ComboBox*, void*) {
	setDSP();
}

static void cb_opDSP_hi(Fl_ComboBox*, void*) {
	setDSP();
}

static void cb_opMODE(Fl_ComboBox*, void*) {
	setMode();
}

static void cb_btnAttenuator(Fl_Light_Button*, void*) {
	cbAttenuator();
}

static void cb_btnPreamp(Fl_Light_Button*, void*) {
	cbPreamp();
}

static void cb_btnNOISE(Fl_Light_Button*, void*) {
	cbNoise();
}

static void cb_btnAutoNotch(Fl_Light_Button*, void*) {
	cbAN();
}

static void cb_btnAswapB(Fl_Button*, void*) {
	cbAswapB();
}

static void cb_btnSplit(Fl_Light_Button* o, void*) {
	cb_set_split(o->value());
}

static void cb_btnPTT(Fl_Light_Button*, void*) {
	cbPTT();
}

static void cb_sldrSQUELCH(Fl_Wheel_Value_Slider*, void*) {
	setSQUELCH();
}

static void cb_sldrMICGAIN(Fl_Wheel_Value_Slider*, void*) {
	setMicGain();
}

static void cb_btnIFsh(Fl_Light_Button*, void*) {
	cbIFsh();
}

static void cb_sldrIFSHIFT(Fl_Wheel_Value_Slider*, void*) {
	setIFshift();
}

static void cb_btnLOCK(Fl_Button*, void*) {
	setLOCK();
}

static void cb_sldrINNER(Fl_Wheel_Value_Slider*, void*) {
	setINNER();
}

static void cb_btnCLRPBT(Fl_Light_Button*, void*) {
	setCLRPBT();
}

static void cb_sldrOUTER(Fl_Wheel_Value_Slider*, void*) {
	setOUTER();
}

static void cb_btnNR(Fl_Light_Button*, void*) {
	cbNR();
}

static void cb_sldrNR(Fl_Wheel_Value_Slider*, void*) {
	setNR();
}

static void cb_btnNotch(Fl_Light_Button*, void*) {
	cbbtnNotch();
}

static void cb_sldrNOTCH(Fl_Wheel_Value_Slider*, void*) {
	setNotch();
}

static void cb_sldrRFGAIN(Fl_Wheel_Value_Slider*, void*) {
	if ( Fl::event_button() == FL_RIGHT_MOUSE )
		progStatus.ft950_rg_reverse = !progStatus.ft950_rg_reverse;
	setRFGAIN();
}

static void cb_btnPOWER(Fl_Light_Button *btn, void*) {
	if (btn->value()) sldrPOWER->activate();
	else sldrPOWER->deactivate();
}

static void cb_sldrPOWER(Fl_Wheel_Value_Slider*, void*) {
	setPower();
}

static void cb_btnVol(Fl_Light_Button*, void*) {
	cbMute();
}

static void cb_sldrVOLUME(Fl_Wheel_Value_Slider*, void*) {
	setVolume();
}

static void cb_btn_band_select(Fl_Button *, void *d) {
	long lband = reinterpret_cast<long>(d);
	int band = lband;
	cbBandSelect(band);
}

static void cb_btnSpot(Fl_Light_Button* o, void*) {
	progStatus.spot_onoff=o->value();
	cb_cw_spot();
}

static void cb_spnr_cw_spot_tone(Hspinner* o, void*) {
	progStatus.cw_spot_tone=(int)o->value();
	cb_cw_spot_tone();
}

static void cb_spnr_cw_qsk(Hspinner* o, void*) {
	progStatus.cw_qsk = o->value();
	cb_cw_qsk();
}

static void cb_spnr_cw_weight(Hspinner* o, void*) {
	progStatus.cw_weight = o->value();
	cb_cw_weight();
}

static void cb_spnr_cw_wpm(Hspinner* o, void*) {
	progStatus.cw_wpm = o->value();
	cb_cw_wpm();
}

static void cb_spnr_cw_delay(Hspinner* o, void*) {
	progStatus.cw_delay = o->value();
	cb_cw_delay();
}

static void cb_btn_enable_keyer(Fl_Check_Button* o, void*) {
	progStatus.enable_keyer=o->value();
	cb_enable_keyer();
}

static void cb_btnBreakIn(Fl_Check_Button* o, void*) {
	if (xcvr_name == rig_IC7300.name_) {
		progStatus.break_in++;
		if (progStatus.break_in == 3)
			progStatus.break_in = 0;
	}
	else {
		progStatus.break_in = ! progStatus.break_in;
	}
	cb_enable_break_in();
}

static void cb_spnr_vox_gain(Hspinner* o, void*) {
	progStatus.vox_gain=(int)o->value();
	cb_vox_gain();
}

static void cb_spnr_anti_vox(Hspinner* o, void*) {
	progStatus.vox_anti=(int)o->value();
	cb_vox_anti();
}

static void cb_spnr_vox_hang(Hspinner* o, void*) {
	progStatus.vox_hang = (int)o->value();
	cb_vox_hang();
}

static void cb_btn_vox(Fl_Light_Button* o, void*) {
	progStatus.vox_onoff=o->value();
cb_vox_onoff();
}

static void cb_btn_vox_on_dataport(Fl_Check_Button* o, void*) {
	progStatus.vox_on_dataport=o->value();
	cb_vox_on_dataport();
}

static void cb_btnCompON(Fl_Light_Button* o, void*) {
	progStatus.compON=o->value();
	cb_compression();
}

static void cb_spnr_compression(Hspinner* o, void*) {
	progStatus.compression=(int)o->value();
	cb_compression();
}

static void cb_sldr_nb_level(Fl_Wheel_Value_Slider* o, void*) {
	progStatus.nb_level = o->value();
	cb_nb_level();
}

//static void cb_cbo_agc_level(Fl_ComboBox* o, void*) {
//	progStatus.agc_level = o->index();
//	cb_agc_level();
//}

static void cb_spnr_bpf_center(Hspinner* o, void*) {
	progStatus.bpf_center=o->value();
	cb_bpf_center();
}

static void cb_btn_use_bpf_center(Fl_Check_Button* o, void*) {
	progStatus.use_bpf_center = o->value();
	cb_bpf_center();
}

static void cb_spnr_vfo_adj(Hspinner* o, void*) {
	progStatus.vfo_adj=o->value();
	cb_vfo_adj();
}

static void cb_spnr_line_out(Hspinner* o, void*) {
	progStatus.line_out = o->value();
	cb_line_out();
}

static void cb_btnSpecial(Fl_Light_Button*, void*) {
	cb_special();
}

static void cb_btn_ext_tuner(Fl_Check_Button* o, void*) {
	progStatus.external_tuner = o->value();
}

static void cb_btn_xcvr_auto_on(Fl_Check_Button* o, void*) {
	progStatus.xcvr_auto_on = o->value();
}

static void cb_btn_xcvr_auto_off(Fl_Check_Button* o, void*) {
	progStatus.xcvr_auto_off = o->value();
}

static void cb_btnUser( Fl_Button * o, void *d) {
	long val = reinterpret_cast<long>(d);
	if (((Fl::event_state() & FL_SHIFT) == FL_SHIFT) ||
		(Fl::event_button() == FL_RIGHT_MOUSE)) {
		switch (val) {
			case 1: cb_send_command(progStatus.shftcmd1, cmdResponse); break;
			case 2: cb_send_command(progStatus.shftcmd2, cmdResponse); break;
			case 3: cb_send_command(progStatus.shftcmd3, cmdResponse); break;
			case 4: cb_send_command(progStatus.shftcmd4, cmdResponse); break;
			case 5: cb_send_command(progStatus.shftcmd5, cmdResponse); break;
			case 6: cb_send_command(progStatus.shftcmd6, cmdResponse); break;
			case 7: cb_send_command(progStatus.shftcmd7, cmdResponse); break;
			case 8: cb_send_command(progStatus.shftcmd8, cmdResponse); break;
			case 9: cb_send_command(progStatus.shftcmd9, cmdResponse); break;
			case 10: cb_send_command(progStatus.shftcmd10, cmdResponse); break;
			case 11: cb_send_command(progStatus.shftcmd11, cmdResponse); break;
			case 12: cb_send_command(progStatus.shftcmd12, cmdResponse); break;
			case 13: cb_send_command(progStatus.shftcmd13, cmdResponse); break;
			case 14: cb_send_command(progStatus.shftcmd14, cmdResponse); break;
			case 15: cb_send_command(progStatus.shftcmd15, cmdResponse); break;
			case 16: cb_send_command(progStatus.shftcmd16, cmdResponse); break;
		}
	} else {
		switch (val) {
			case 1: cb_send_command(progStatus.command1, cmdResponse); break;
			case 2: cb_send_command(progStatus.command2, cmdResponse); break;
			case 3: cb_send_command(progStatus.command3, cmdResponse); break;
			case 4: cb_send_command(progStatus.command4, cmdResponse); break;
			case 5: cb_send_command(progStatus.command5, cmdResponse); break;
			case 6: cb_send_command(progStatus.command6, cmdResponse); break;
			case 7: cb_send_command(progStatus.command7, cmdResponse); break;
			case 8: cb_send_command(progStatus.command8, cmdResponse); break;
			case 9: cb_send_command(progStatus.command9, cmdResponse); break;
			case 10: cb_send_command(progStatus.command10, cmdResponse); break;
			case 11: cb_send_command(progStatus.command11, cmdResponse); break;
			case 12: cb_send_command(progStatus.command12, cmdResponse); break;
			case 13: cb_send_command(progStatus.command13, cmdResponse); break;
			case 14: cb_send_command(progStatus.command14, cmdResponse); break;
			case 15: cb_send_command(progStatus.command15, cmdResponse); break;
			case 16: cb_send_command(progStatus.command16, cmdResponse); break;
		}
	}
}

static void cb_digi_sel_on_off(Fl_Light_Button *o, void *d) {
	progStatus.digi_sel_on_off = o->value();
	digi_sel_on_off();
}

static void cb_digi_sel_val(Fl_Wheel_Value_Slider *o, void *d) {
	progStatus.digi_sel_val = o->value();
	digi_sel_val();
}

static void cb_dual_watch(Fl_Light_Button *o, void *d) {
	progStatus.dual_watch = o->value();
	dual_watch();
}

static void cb_ic7610_att(Fl_ComboBox *o, void *d) {
	progStatus.index_ic7610att = o->index();
	index_att();
}

static void cb_cntRIT(Hspinner*, void*) {
	cbRIT();
}

static void cb_cntXIT(Hspinner*, void*) {
	cbXIT();
}

static void cb_cntBFO(Hspinner*, void*) {
	cbBFO();
}

static void cb_btnAuxRTS(Fl_Light_Button* o, void*) {
	progStatus.aux_rts = o->value();
	cbAuxPort();
}

static void cb_btnAuxDTR(Fl_Light_Button* o, void*) {
	progStatus.aux_dtr = o->value();
	cbAuxPort();
}

static void cb_btnDataPort(Fl_Light_Button* o, void*) {
	progStatus.data_port = o->value();
	if (o->value()) btnDataPort->label("Data");
	else btnDataPort->label("Mic");
	btnDataPort->redraw_label();
	selrig->set_data_port();
}

static void cb_spnr_tt550_cw_wpm(Hspinner* o, void*) {
	progStatus.tt550_cw_wpm = (int)o->value();
	cb_tt550_cw_wpm();
}

static void cb_spnr_tt550_cw_weight(Hspinner* o, void*) {
	progStatus.tt550_cw_weight = o->value();
	cb_tt550_cw_weight();
}

static void cb_spnr_tt550_cw_qsk(Hspinner* o, void*) {
	progStatus.tt550_cw_qsk = (int)o->value();
	cb_tt550_cw_qsk();
}

static void cb_spnr_tt550_cw_vol(Hspinner* o, void*) {
	progStatus.tt550_cw_vol=(int)o->value();
	cb_tt550_cw_vol();
}

static void cb_spnr_tt550_cw_spot(Hspinner* o, void*) {
	progStatus.tt550_cw_spot=(int)o->value();
	cb_tt550_cw_spot();
}

static void cb_btn_tt550_enable_keyer(Fl_Check_Button* o, void*) {
	progStatus.tt550_enable_keyer=o->value();
	cb_tt550_enable_keyer();
}

static void cb_spnr_tt550_vox_gain(Hspinner* o, void*) {
	progStatus.tt550_vox_gain=(int)o->value();
	cb_tt550_vox_gain();
}

static void cb_spnr_tt550_anti_vox(Hspinner* o, void*) {
	progStatus.tt550_vox_anti=(int)o->value();
	cb_tt550_vox_anti();
}

static void cb_spnr_tt550_vox_hang(Hspinner* o, void*) {
	progStatus.tt550_vox_hang = (int)o->value();
	cb_tt550_vox_hang();
}

static void cb_btn_tt550_vox(Fl_Light_Button* o, void*) {
	progStatus.tt550_vox_onoff=o->value();
	cb_tt550_vox_onoff();
}

static void cb_btn_tt550_CompON(Fl_Light_Button* o, void*) {
	progStatus.tt550_compON=o->value();
	cb_tt550_compression();
}

static void cb_spnr_tt550_compression(Hspinner* o, void*) {
	progStatus.tt550_compression=(int)o->value();
	cb_tt550_compression();
}

static void cb_spnr_tt550_mon_vol(Hspinner* o, void*) {
	progStatus.tt550_mon_vol=(int)o->value();
	cb_tt550_mon_vol();
}

static void cb_spnr_tt550_line_out(Hspinner* o, void*) {
	progStatus.tt550_line_out = (int)o->value();
	cb_tt550_line_out();
}

static void cb_btnAccInp(Fl_Check_Button* o, void*) {
	progStatus.tt550_use_line_in=o->value();
	cb_tt550_use_line_in();
}

static void cb_cbo_tt550_nb_level(Fl_ComboBox* o, void*) {
	progStatus.tt550_nb_level = o->index();
	cb_tt550_nb_level();
}

static void cb_cbo_tt550_agc_level(Fl_ComboBox* o, void*) {
	progStatus.tt550_agc_level = o->index();
	cb_tt550_agc_level();
}

static void cb_spnr_tt550_vfo_adj(Hspinner* o, void*) {
	progStatus.vfo_adj=o->value();
	cb_vfo_adj();
}

static void cb_btn_tt550_enable_xmtr(Fl_Check_Button* o, void*) {
	progStatus.tt550_enable_xmtr=o->value();
	cb_tt550_enable_xmtr();
}

static void cb_btn_tt550_enable_tloop(Fl_Check_Button* o, void*) {
	progStatus.tt550_enable_tloop=o->value();
	cb_tt550_enable_tloop();
}

static void cb_btn_tt550_tuner_bypass(Fl_Check_Button* o, void*) {
	progStatus.tt550_tuner_bypass=o->value();
	cb_tt550_tuner_bypass();
}

static void cb_op_tt550_XmtBW(Fl_ComboBox* o, void*) {
	progStatus.tt550_xmt_bw = o->index();
	cb_tt550_setXmtBW();
}

static void cb_btn_tt550_use_xmt_bw(Fl_Check_Button* o, void*) {
	progStatus.tt550_use_xmt_bw = o->value();
	cb_tt550_setXmtBW();
}

static void cb_spnr_tt550_keypad_time_out(Hspinner* o, void*) {
	progStatus.tt550_keypad_timeout=(int)(o->value()*1000.0);
}

static void cb_spnr_tt550_encoder_sensitivity(Hspinner* o, void*) {
	progStatus.tt550_encoder_sensitivity = o->value();
}

static void cb_sel_tt550_encoder_step(Fl_Choice* o, void*) {
	progStatus.tt550_encoder_step = o->value();
}

static void cb_sel_tt550_F1_func(Fl_Choice* o, void*) {
	progStatus.tt550_F1_func = o->value();
}

static void cb_sel_tt550_F2_func(Fl_Choice* o, void*) {
	progStatus.tt550_F2_func = o->value();
}

static void cb_sel_tt550_F3_func(Fl_Choice* o, void*) {
	progStatus.tt550_F3_func = o->value();
}

static void cb_AT_bypass(Fl_Light_Button *btn, void*) {
	if (!btn->value()) {
		progStatus.tt550_at11_inline = false;
		tt550_AT_inline->label("Bypassed");
		tt550_AT_inline->redraw_label();
		selrig->at11_bypass();
	} else {
		progStatus.tt550_at11_inline = true;
		tt550_AT_inline->label("Inline");
		tt550_AT_inline->redraw_label();
		selrig->at11_autotune();
	}
}

static void cb_AT_Z(Fl_Light_Button *btn, void*) {
	if (btn->value())
		selrig->at11_hiZ();
	else
		selrig->at11_loZ();
}

static void cb_AT_capUP(Fl_Button*, void*) {
	selrig->at11_cap_up();
}

static void cb_AT_capDN(Fl_Button*, void*) {
	selrig->at11_cap_dn();
}

static void cb_AT_indUP(Fl_Button*, void*) {
	selrig->at11_ind_up();
}

static void cb_AT_indDN(Fl_Button*, void*) {
	selrig->at11_ind_dn();
}

static void cb_tt550_Nsamples(Fl_Simple_Counter *, void *) {
	progStatus.tt550_Nsamples =  tt550_Nsamples->value();
}

Fl_Menu_Item *getMenuItem(const char *caption, Fl_Menu_Item* menu = 0)
{
	if (menu == 0) {
		return (Fl_Menu_Item*)0;
	}

	int size = menu->size() - 1;
	Fl_Menu_Item *item = 0;
	const char* label;
	for (int i = 0; i < size; i++) {
		label = menu[i].text;
		if (label && !strcmp(label, caption)) {
			item = menu + i;
			break;
		}
	}
	return item;
}

int PL_tones[] = {
 670,  693,  719,  744,  770,  797,  825,  854,  885,  915,
 948,  974, 1000, 1035, 1072, 1109, 1148, 1188, 1230, 1273,
1318, 1365, 1413, 1462, 1514, 1567, 1598, 1622, 1655, 1679,
1713, 1738, 1773, 1799, 1835, 1862, 1899, 1928, 1966, 1995,
2035, 2065, 2107, 2181, 2257, 2291, 2336, 2418, 2503, 2541 };

void cb_tones(Fl_Button *b, void *)
{
	int tTONE = PL_tones[choice_FT8n_tTONE->value()];
	int rTONE = PL_tones[choice_FT8n_rTONE->value()];
	guard_lock serial(&mutex_serial);
	selrig->set_tones(tTONE, rTONE);
}

void cb_offset(Fl_Button *b, void*)
{
	guard_lock serial(&mutex_serial);
	selrig->set_offset(FMoffset->index(), FMoff_freq->value());
}

#include "ui_small.cxx"

#include "ui_wide.cxx"

#include "ui_touch.cxx"

#include "ui_setup.cxx"

#include "ui_memory.cxx"

#include "xcvr_setup.cxx"

#include "meters_setup.cxx"

#include "power_meter_setup.cxx"
