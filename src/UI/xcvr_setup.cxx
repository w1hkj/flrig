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

#include <sstream>
#include <string>

#include "dialogs.h"
#include "cmedia.h"
#include "tmate2.h"
#include "rigs.h"
#include "trace.h"
#include "xml_server.h"

#include "XmlRpc.h"

Fl_Group *tabXCVR = (Fl_Group *)0;
	Fl_ComboBox *selectRig = (Fl_ComboBox *)0;
	Fl_Counter *cntRigCatRetries = (Fl_Counter *)0;
	Fl_Counter *cntRigCatTimeout = (Fl_Counter *)0;
	Fl_Counter *cntRigCatWait = (Fl_Counter *)0;
	Fl_Counter *query_interval = (Fl_Counter *)0;
	Fl_Counter *byte_interval = (Fl_Counter *)0;
	Fl_ComboBox *selectCommPort = (Fl_ComboBox *)0;
	Fl_ComboBox *mnuBaudrate = (Fl_ComboBox *)0;
	Fl_Check_Button *btnTwoStopBit = (Fl_Check_Button *)0;
	Fl_Check_Button *btnOneStopBit = (Fl_Check_Button *)0;
	Fl_Check_Button *btnRigCatEcho = (Fl_Check_Button *)0;

	Fl_ListBox *lbox_catptt = (Fl_ListBox *)0;
	Fl_ListBox *lbox_rtsptt = (Fl_ListBox *)0;
	Fl_ListBox *lbox_dtrptt = (Fl_ListBox *)0;

	Fl_Check_Button *chkrtscts = (Fl_Check_Button *)0;
	Fl_Check_Button *btnrtsplus1 = (Fl_Check_Button *)0;
	Fl_Check_Button *btndtrplus1 = (Fl_Check_Button *)0;
	Fl_Check_Button *btnrtsplus2 = (Fl_Check_Button *)0;
	Fl_Check_Button *btndtrplus2 = (Fl_Check_Button *)0;
	//Fl_Check_Button *btn_notxqsy = (Fl_Check_Button *)0;
	Fl_Int_Input *txtCIV = (Fl_Int_Input *)0;
	Fl_Button *btnCIVdefault = (Fl_Button *)0;
	Fl_Check_Button *btnUSBaudio = (Fl_Check_Button *)0;

Fl_Group *tabTCPIP = (Fl_Group *)0;
	Fl_Input2 *inp_tcpip_addr = (Fl_Input2 *)0;
	Fl_Input2 *inp_tcpip_port = (Fl_Input2 *)0;
	Fl_Check_Button *chk_use_tcpip = (Fl_Check_Button *)0;
	Fl_Box *box_tcpip_connect = (Fl_Box *)0;
	Fl_Counter *inp_tcpip_ping_delay = (Fl_Counter *)0;
	Fl_Counter *cntRetryAfter = (Fl_Counter *)0;
	Fl_Counter *cntDropsAllowed = (Fl_Counter *)0;

Fl_Group *tabPTTGEN = (Fl_Group *)0;
	Fl_ComboBox *selectSepPTTPort = (Fl_ComboBox *)0;
	Fl_Check_Button *btnSep_SCU_17 = (Fl_Check_Button *)0;

	Fl_ListBox *lbox_sep_rtsptt = (Fl_ListBox *)0;
	Fl_Check_Button *btnSepRTSplus = (Fl_Check_Button *)0;

	Fl_ListBox *lbox_sep_dtrptt = (Fl_ListBox *)0;
	Fl_Check_Button *btnSepDTRplus = (Fl_Check_Button *)0;

Fl_Group *tabGPIO = (Fl_Group *)0;
	Fl_Check_Button *btn_enable_gpio[17];
	Fl_Check_Button *btn_gpio_on[17];
	Fl_Counter *cnt_gpio_pulse_width;

Fl_Group *tabAUX = (Fl_Group *)0;
	Fl_ComboBox *selectAuxPort = (Fl_ComboBox *)0;
	Fl_Check_Button *btnAux_SCU_17 = (Fl_Check_Button *)0;

Fl_Group *tabSERVER = (Fl_Group *)0;
	Fl_Box *server_text = (Fl_Box *)0;
	Fl_Input2 *inp_serverport = (Fl_Input2 *)0;
	Fl_Box *box_fldigi_connect = (Fl_Box *)0;

Fl_Group *tabPOLLING = (Fl_Group *)0;
	Fl_Check_Button *poll_smeter = (Fl_Check_Button *)0;
	Fl_Check_Button *poll_pout = (Fl_Check_Button *)0;
	Fl_Check_Button *poll_swr = (Fl_Check_Button *)0;
	Fl_Check_Button *poll_alc = (Fl_Check_Button *)0;
	Fl_Check_Button *poll_frequency = (Fl_Check_Button *)0;
	Fl_Check_Button *poll_mode = (Fl_Check_Button *)0;
	Fl_Check_Button *poll_bandwidth = (Fl_Check_Button *)0;
	Fl_Check_Button *poll_volume = (Fl_Check_Button *)0;
	Fl_Check_Button *poll_micgain = (Fl_Check_Button *)0;
	Fl_Check_Button *poll_rfgain = (Fl_Check_Button *)0;
	Fl_Check_Button *poll_power_control = (Fl_Check_Button *)0;
	Fl_Check_Button *poll_ifshift = (Fl_Check_Button *)0;
	Fl_Check_Button *poll_notch = (Fl_Check_Button *)0;
	Fl_Check_Button *poll_auto_notch = (Fl_Check_Button *)0;
	Fl_Check_Button *poll_pre_att = (Fl_Check_Button *)0;
	Fl_Check_Button *poll_squelch = (Fl_Check_Button *)0;
	Fl_Check_Button *poll_split = (Fl_Check_Button *)0;
	Fl_Check_Button *poll_noise = (Fl_Check_Button *)0;
	Fl_Check_Button *poll_nr = (Fl_Check_Button *)0;
	Fl_Check_Button *poll_compression = (Fl_Check_Button *)0;
	Fl_Check_Button *poll_tuner = (Fl_Check_Button *)0;
	Fl_Check_Button *poll_ptt = (Fl_Check_Button *)0;
	Fl_Check_Button *poll_break_in = (Fl_Check_Button *)0;
	Fl_Button *btnClearAddControls = (Fl_Button *)0;

	Fl_Button *btnSetMeters = (Fl_Button *)0;
	Fl_Button *btnSetOps = (Fl_Button *)0;
	Fl_Button *btnSetAdd = (Fl_Button *)0;

	Fl_Check_Button *poll_meters = (Fl_Check_Button *)0;
	Fl_Check_Button *poll_ops = (Fl_Check_Button *)0;
	Fl_Check_Button *poll_all = (Fl_Check_Button *)0;

	Fl_Check_Button *disable_polling = (Fl_Check_Button *)0;
	Fl_Check_Button *disable_xmlrpc = (Fl_Check_Button *)0;

Fl_Group *tabSNDCMD = (Fl_Group *)0;
	Fl_Input2 *txt_command = (Fl_Input2 *)0;
	Fl_Button *btn_icom_pre = (Fl_Button *)0;
	Fl_Button *btn_icom_post = (Fl_Button *)0;
	Fl_Button *btn_send_command = (Fl_Button *)0;
	Fl_Output *txt_response = (Fl_Output *)0;
	Fl_Box *box_xcvr_connect = (Fl_Box *)0;

Fl_Button *btn_init_ser_port = (Fl_Button *)0;
Fl_Button *btn2_init_ser_port = (Fl_Button *)0;

Fl_Button *btnCloseCommConfig = (Fl_Button *)0;
Fl_Button *btnOkSepSerial = (Fl_Button *)0;
Fl_Button *btnOkAuxSerial = (Fl_Button *)0;

Fl_Group *tabTRACE = (Fl_Group *)0;
	Fl_Check_Button *btn_trace = (Fl_Check_Button *)0;
	Fl_Check_Button *btn_xmltrace = (Fl_Check_Button *)0;
	Fl_Check_Button *btn_rigtrace = (Fl_Check_Button *)0;
	Fl_Check_Button *btn_gettrace = (Fl_Check_Button *)0;
	Fl_Check_Button *btn_settrace = (Fl_Check_Button *)0;
	Fl_Check_Button *btn_debugtrace = (Fl_Check_Button *)0;
	Fl_Check_Button *btn_rpctrace = (Fl_Check_Button *)0;
	Fl_Check_Button *btn_serialtrace = (Fl_Check_Button *)0;
	Fl_Check_Button *btn_start_stop_trace = (Fl_Check_Button *)0;
	Fl_ComboBox *selectlevel = (Fl_ComboBox *)0;
	Fl_Button *btn_viewtrace = (Fl_Button *)0;

Fl_Group *tabCOMMANDS = (Fl_Group *)0;
	Fl_Tabs  *tabCmds = (Fl_Tabs *)0;
	Fl_Group *tabCmds1 = (Fl_Group *)0;
	Fl_Group *tabCmds2 = (Fl_Group *)0;
	Fl_Group *tabCmds3 = (Fl_Group *)0;
	Fl_Group *tabCmds4 = (Fl_Group *)0;
	Fl_Group *tabCmds5 = (Fl_Group *)0;
	Fl_Group *tabCmds6 = (Fl_Group *)0;
	Fl_Group *tabCmds_on_start_exit = (Fl_Group *)0;

	Fl_Box *bx1a = (Fl_Box *)0;
	Fl_Box *bx1b = (Fl_Box *)0;
	Fl_Box *bx2a = (Fl_Box *)0;
	Fl_Box *bx2b = (Fl_Box *)0;
	Fl_Box *bx3a = (Fl_Box *)0;
	Fl_Box *bx3b = (Fl_Box *)0;
	Fl_Box *bx4a = (Fl_Box *)0;
	Fl_Box *bx4b = (Fl_Box *)0;
	Fl_Box *bx5a = (Fl_Box *)0;
	Fl_Box *bx5b = (Fl_Box *)0;
	Fl_Box *bx6a = (Fl_Box *)0;
	Fl_Box *bx6b = (Fl_Box *)0;

	Fl_Input2 * cmdlbl1 = (Fl_Input2 *)0;
	Fl_Input2 * cmdlbl2 = (Fl_Input2 *)0;
	Fl_Input2 * cmdlbl3 = (Fl_Input2 *)0;
	Fl_Input2 * cmdlbl4 = (Fl_Input2 *)0;
	Fl_Input2 * cmdlbl5 = (Fl_Input2 *)0;
	Fl_Input2 * cmdlbl6 = (Fl_Input2 *)0;
	Fl_Input2 * cmdlbl7 = (Fl_Input2 *)0;
	Fl_Input2 * cmdlbl8 = (Fl_Input2 *)0;
	Fl_Input2 * cmdlbl9 = (Fl_Input2 *)0;
	Fl_Input2 * cmdlbl10 = (Fl_Input2 *)0;
	Fl_Input2 * cmdlbl11 = (Fl_Input2 *)0;
	Fl_Input2 * cmdlbl12 = (Fl_Input2 *)0;
	Fl_Input2 * cmdlbl13 = (Fl_Input2 *)0;
	Fl_Input2 * cmdlbl14 = (Fl_Input2 *)0;
	Fl_Input2 * cmdlbl15 = (Fl_Input2 *)0;
	Fl_Input2 * cmdlbl16 = (Fl_Input2 *)0;
	Fl_Input2 * cmdlbl17 = (Fl_Input2 *)0;
	Fl_Input2 * cmdlbl18 = (Fl_Input2 *)0;
	Fl_Input2 * cmdlbl19 = (Fl_Input2 *)0;
	Fl_Input2 * cmdlbl20 = (Fl_Input2 *)0;
	Fl_Input2 * cmdlbl21 = (Fl_Input2 *)0;
	Fl_Input2 * cmdlbl22 = (Fl_Input2 *)0;
	Fl_Input2 * cmdlbl23 = (Fl_Input2 *)0;
	Fl_Input2 * cmdlbl24 = (Fl_Input2 *)0;

	Fl_Input2 * cmdtext1 = (Fl_Input2 *)0;
	Fl_Input2 * cmdtext2 = (Fl_Input2 *)0;
	Fl_Input2 * cmdtext3 = (Fl_Input2 *)0;
	Fl_Input2 * cmdtext4 = (Fl_Input2 *)0;
	Fl_Input2 * cmdtext5 = (Fl_Input2 *)0;
	Fl_Input2 * cmdtext6 = (Fl_Input2 *)0;
	Fl_Input2 * cmdtext7 = (Fl_Input2 *)0;
	Fl_Input2 * cmdtext8 = (Fl_Input2 *)0;
	Fl_Input2 * cmdtext9 = (Fl_Input2 *)0;
	Fl_Input2 * cmdtext10 = (Fl_Input2 *)0;
	Fl_Input2 * cmdtext11 = (Fl_Input2 *)0;
	Fl_Input2 * cmdtext12 = (Fl_Input2 *)0;
	Fl_Input2 * cmdtext13 = (Fl_Input2 *)0;
	Fl_Input2 * cmdtext14 = (Fl_Input2 *)0;
	Fl_Input2 * cmdtext15 = (Fl_Input2 *)0;
	Fl_Input2 * cmdtext16 = (Fl_Input2 *)0;
	Fl_Input2 * cmdtext17 = (Fl_Input2 *)0;
	Fl_Input2 * cmdtext18 = (Fl_Input2 *)0;
	Fl_Input2 * cmdtext19 = (Fl_Input2 *)0;
	Fl_Input2 * cmdtext20 = (Fl_Input2 *)0;
	Fl_Input2 * cmdtext21 = (Fl_Input2 *)0;
	Fl_Input2 * cmdtext22 = (Fl_Input2 *)0;
	Fl_Input2 * cmdtext23 = (Fl_Input2 *)0;
	Fl_Input2 * cmdtext24 = (Fl_Input2 *)0;

	Fl_Input2 * shftcmdtext1 = (Fl_Input2 *)0;
	Fl_Input2 * shftcmdtext2 = (Fl_Input2 *)0;
	Fl_Input2 * shftcmdtext3 = (Fl_Input2 *)0;
	Fl_Input2 * shftcmdtext4 = (Fl_Input2 *)0;
	Fl_Input2 * shftcmdtext5 = (Fl_Input2 *)0;
	Fl_Input2 * shftcmdtext6 = (Fl_Input2 *)0;
	Fl_Input2 * shftcmdtext7 = (Fl_Input2 *)0;
	Fl_Input2 * shftcmdtext8 = (Fl_Input2 *)0;
	Fl_Input2 * shftcmdtext9 = (Fl_Input2 *)0;
	Fl_Input2 * shftcmdtext10 = (Fl_Input2 *)0;
	Fl_Input2 * shftcmdtext11 = (Fl_Input2 *)0;
	Fl_Input2 * shftcmdtext12 = (Fl_Input2 *)0;
	Fl_Input2 * shftcmdtext13 = (Fl_Input2 *)0;
	Fl_Input2 * shftcmdtext14 = (Fl_Input2 *)0;
	Fl_Input2 * shftcmdtext15 = (Fl_Input2 *)0;
	Fl_Input2 * shftcmdtext16 = (Fl_Input2 *)0;
	Fl_Input2 * shftcmdtext17= (Fl_Input2 *)0;
	Fl_Input2 * shftcmdtext18 = (Fl_Input2 *)0;
	Fl_Input2 * shftcmdtext19 = (Fl_Input2 *)0;
	Fl_Input2 * shftcmdtext20 = (Fl_Input2 *)0;
	Fl_Input2 * shftcmdtext21 = (Fl_Input2 *)0;
	Fl_Input2 * shftcmdtext22 = (Fl_Input2 *)0;
	Fl_Input2 * shftcmdtext23 = (Fl_Input2 *)0;
	Fl_Input2 * shftcmdtext24 = (Fl_Input2 *)0;

	Fl_Input2 * start_exit_label1 = (Fl_Input2 *)0;
	Fl_Input2 * start_exit_text1  = (Fl_Input2 *)0;
	Fl_Input2 * start_exit_label2 = (Fl_Input2 *)0;
	Fl_Input2 * start_exit_text2  = (Fl_Input2 *)0;
	Fl_Input2 * start_exit_label3 = (Fl_Input2 *)0;
	Fl_Input2 * start_exit_text3  = (Fl_Input2 *)0;
	Fl_Input2 * start_exit_label4 = (Fl_Input2 *)0;
	Fl_Input2 * start_exit_text4  = (Fl_Input2 *)0;
	Fl_Input2 * start_exit_label5 = (Fl_Input2 *)0;
	Fl_Input2 * start_exit_text5  = (Fl_Input2 *)0;
	Fl_Input2 * start_exit_label6 = (Fl_Input2 *)0;
	Fl_Input2 * start_exit_text6  = (Fl_Input2 *)0;
	Fl_Input2 * start_exit_label7 = (Fl_Input2 *)0;
	Fl_Input2 * start_exit_text7  = (Fl_Input2 *)0;
	Fl_Input2 * start_exit_label8 = (Fl_Input2 *)0;
	Fl_Input2 * start_exit_text8  = (Fl_Input2 *)0;

	Fl_Output * cmdResponse = (Fl_Output *)0;

Fl_Group *tabRESTORE = (Fl_Group *)0;
	Fl_Check_Button *btnRestoreFrequency	= (Fl_Check_Button *)0;
	Fl_Check_Button *btnRestoreMode			= (Fl_Check_Button *)0;
	Fl_Check_Button *btnRestoreBandwidth	= (Fl_Check_Button *)0;
	Fl_Check_Button *btnRestoreVolume		= (Fl_Check_Button *)0;
	Fl_Check_Button *btnRestoreMicGain		= (Fl_Check_Button *)0;
	Fl_Check_Button *btnRestoreRfGain		= (Fl_Check_Button *)0;
	Fl_Check_Button *btnRestorePowerControl	= (Fl_Check_Button *)0;
	Fl_Check_Button *btnRestoreIFshift		= (Fl_Check_Button *)0;
	Fl_Check_Button *btnRestoreNotch		= (Fl_Check_Button *)0;
	Fl_Check_Button *btnRestoreAutoNotch	= (Fl_Check_Button *)0;
	Fl_Check_Button *btnRestoreSquelch		= (Fl_Check_Button *)0;
	Fl_Check_Button *btnRestoreSplit		= (Fl_Check_Button *)0;
	Fl_Check_Button *btnRestorePreAtt		= (Fl_Check_Button *)0;
	Fl_Check_Button *btnRestoreNoise		= (Fl_Check_Button *)0;
	Fl_Check_Button *btnRestoreNR			= (Fl_Check_Button *)0;
	Fl_Check_Button *btnRestoreCompOnOff	= (Fl_Check_Button *)0;
	Fl_Check_Button *btnRestoreCompLevel	= (Fl_Check_Button *)0;
	Fl_Check_Button *btnUseRigData			= (Fl_Check_Button *)0;

static void cb_btn_trace(Fl_Check_Button *, void *) {
	progStatus.trace = btn_trace->value();
}

static void cb_btn_rigtrace(Fl_Check_Button *, void *) {
	progStatus.rigtrace = btn_rigtrace->value();
}

static void cb_btn_gettrace(Fl_Check_Button *, void *) {
	progStatus.gettrace = btn_gettrace->value();
}

static void cb_btn_settrace(Fl_Check_Button *, void *) {
	progStatus.settrace = btn_settrace->value();
}

static void cb_btn_xmltrace(Fl_Check_Button *, void *) {
	progStatus.xmltrace = btn_xmltrace->value();
}

static void cb_btn_debugtrace(Fl_Check_Button *, void *) {
	progStatus.debugtrace = btn_debugtrace->value();
}

static void cb_btn_rpctrace(Fl_Check_Button *, void *) {
	progStatus.rpctrace = btn_rpctrace->value();
}

static void cb_btn_serialtrace(Fl_Check_Button *, void *) {
	progStatus.serialtrace = btn_serialtrace->value();
}

static void cb_btn_start_stop_trace(Fl_Check_Button *, void *) {
	progStatus.start_stop_trace = btn_start_stop_trace->value();
}

static void cb_selectlevel(Fl_ComboBox *, void *) {
	progStatus.rpc_level = selectlevel->index();
	XmlRpc::setVerbosity(progStatus.rpc_level);
}

static void cb_btn_viewtrace(Fl_Button *, void *) {
	if (!tracewindow) make_trace_window();
	tracewindow->show();
}

static void cb_selectRig(Fl_ComboBox*, void*) {
	btn_init_ser_port->labelcolor(FL_RED);
	btn_init_ser_port->redraw_label();

	btn2_init_ser_port->labelcolor(FL_RED);
	btn2_init_ser_port->redraw_label();

	initConfigDialog();
}

static void cb_cntRigCatRetries(Fl_Counter* o, void*) {
	progStatus.comm_retries = (int)o->value();
}

static void cb_cntRigCatTimeout(Fl_Counter* o, void*) {
	progStatus.comm_timeout = (int)o->value();
}

static void cb_cntRigCatWait(Fl_Counter* o, void*) {
	progStatus.comm_wait = (int)o->value();
}

static void cb_query_interval(Fl_Counter* o, void*) {
	progStatus.serloop_timing = (int)o->value();
}

static void cb_byte_interval(Fl_Counter* o, void*) {
	progStatus.byte_interval = (int)o->value();
}

static void cb_selectCommPort(Fl_ComboBox*, void*) {
	btn_init_ser_port->labelcolor(FL_RED);
	btn_init_ser_port->redraw_label();

	btn2_init_ser_port->labelcolor(FL_RED);
	btn2_init_ser_port->redraw_label();
}

static void cb_mnuBaudrate(Fl_ComboBox*, void*) {
	btn_init_ser_port->labelcolor(FL_RED);
	btn_init_ser_port->redraw_label();

	btn2_init_ser_port->labelcolor(FL_RED);
	btn2_init_ser_port->redraw_label();
}

static void cb_btnTwoStopBit(Fl_Check_Button* o, void*) {
	if (o->value() == true) {
		btnOneStopBit->value(false);
		progStatus.stopbits = 2;
	} else
		o->value(true);
	btn_init_ser_port->labelcolor(FL_RED);
	btn_init_ser_port->redraw_label();

	btn2_init_ser_port->labelcolor(FL_RED);
	btn2_init_ser_port->redraw_label();
}

static void cb_btnOneStopBit(Fl_Check_Button* o, void*) {
	if (o->value() == true) {
		btnTwoStopBit->value(false);
		progStatus.stopbits = 1;
	} else
		o->value(true);
	btn_init_ser_port->labelcolor(FL_RED);
	btn_init_ser_port->redraw_label();

	btn2_init_ser_port->labelcolor(FL_RED);
	btn2_init_ser_port->redraw_label();
}

static void cb_btnRigCatEcho(Fl_Check_Button*, void*) {
	btn_init_ser_port->labelcolor(FL_RED);
	btn_init_ser_port->redraw_label();

	btn2_init_ser_port->labelcolor(FL_RED);
	btn2_init_ser_port->redraw_label();
}

static void cb_tcpip_addr(Fl_Input2* o, void*) {
	progStatus.tcpip_addr = o->value();
}

static void cb_tcpip_port(Fl_Input2* o, void*) {
	progStatus.tcpip_port = o->value();
}

static void cb_tcpip_ping_delay(Fl_Counter* o, void*) {
	progStatus.tcpip_ping_delay = o->value();
}

static void cb_use_tcpip(Fl_Check_Button* o, void*) {
	progStatus.use_tcpip = o->value();
	if (!progStatus.use_tcpip)
		disconnect_from_remote();
	else
		try {
			connect_to_remote();
			initRig();
		} catch (...) {
			progStatus.use_tcpip = 0;
			o->value(0);
			return;
		}
}

static void cb_cntRetryAfter(Fl_Counter* o, void *) {
	progStatus.tcpip_reconnect_after = o->value();
}

static void cb_cntDropsAllowed(Fl_Counter* o, void *) {
	progStatus.tcpip_drops_allowed = o->value();
}

static void cb_lbox_catptt(Fl_ListBox* o, void*) {
	progStatus.comm_catptt = o->index();

	btn_init_ser_port->labelcolor(FL_RED);
	btn_init_ser_port->redraw_label();

	btn2_init_ser_port->labelcolor(FL_RED);
	btn2_init_ser_port->redraw_label();
}

static void cb_lbox_rtsptt(Fl_ListBox* o, void*) {
	progStatus.comm_rtsptt = o->index();

	btn_init_ser_port->labelcolor(FL_RED);
	btn_init_ser_port->redraw_label();

	btn2_init_ser_port->labelcolor(FL_RED);
	btn2_init_ser_port->redraw_label();
}

static void cb_lbox_dtrptt(Fl_ListBox* o, void*) {
	progStatus.comm_dtrptt = o->index();

	btn_init_ser_port->labelcolor(FL_RED);
	btn_init_ser_port->redraw_label();

	btn2_init_ser_port->labelcolor(FL_RED);
	btn2_init_ser_port->redraw_label();
}

static void cb_chkrtscts(Fl_Check_Button*, void*) {
	btn_init_ser_port->labelcolor(FL_RED);
	btn_init_ser_port->redraw_label();

	btn2_init_ser_port->labelcolor(FL_RED);
	btn2_init_ser_port->redraw_label();
}

static void cb_btnrtsplus(Fl_Check_Button *b, void*) {
	int val = b->value();
	btn_init_ser_port->labelcolor(FL_RED);
	btn_init_ser_port->redraw_label();

	btn2_init_ser_port->labelcolor(FL_RED);
	btn2_init_ser_port->redraw_label();

	btnrtsplus1->value(val);
	btnrtsplus2->value(val);
}

static void cb_btndtrplus(Fl_Check_Button *b, void*) {
	int val = b->value();
	btn_init_ser_port->labelcolor(FL_RED);
	btn_init_ser_port->redraw_label();

	btn2_init_ser_port->labelcolor(FL_RED);
	btn2_init_ser_port->redraw_label();

	btndtrplus1->value(val);
	btndtrplus2->value(val);
}

static void cb_txtCIV(Fl_Int_Input* o, void*) {
	cbCIV();
}

static void cb_btnCIVdefault(Fl_Button*, void*) {
	cbCIVdefault();
}

static void cb_btnUSBaudio(Fl_Check_Button*, void*) {
	cbUSBaudio();
}

static void cb_selectSepPTTPort(Fl_ComboBox*, void*) {
	btnOkSepSerial->labelcolor(FL_RED);
	btnOkSepSerial->redraw_label();
}

static void cb_lbox_sep_rtsptt(Fl_ListBox* o, void*) {
	progStatus.sep_rtsptt = o->index();
	btnOkSepSerial->labelcolor(FL_RED);
	btnOkSepSerial->redraw_label();
}

static void cb_btnSepRTSplus(Fl_Check_Button*, void*) {
	btnOkSepSerial->labelcolor(FL_RED);
	btnOkSepSerial->redraw_label();
}

static void cb_lbox_sep_dtrptt(Fl_ListBox* o, void*) {
	progStatus.sep_dtrptt = o->index();
	btnOkSepSerial->labelcolor(FL_RED);
	btnOkSepSerial->redraw_label();
}

static void cb_btnSepDTRplus(Fl_Check_Button*, void*) {
	btnOkSepSerial->labelcolor(FL_RED);
	btnOkSepSerial->redraw_label();
}

static void cb_btnSep_SCU_17(Fl_Check_Button*, void*) {
	btnOkSepSerial->labelcolor(FL_RED);
	btnOkSepSerial->redraw_label();
}

static void cb_selectAuxPort(Fl_ComboBox*, void*) {
	btnOkAuxSerial->labelcolor(FL_RED);
	btnOkAuxSerial->redraw_label();
}

static void cb_btnAux_SCU_17(Fl_Check_Button*, void*) {
	btnOkAuxSerial->labelcolor(FL_RED);
	btnOkAuxSerial->redraw_label();
}

static void cb_disable_CW_ptt(Fl_Check_Button *btn, void*) {
	progStatus.disable_CW_ptt = btn->value();
}

static void cb_server_port(Fl_Input2* o, void*) {
	progStatus.xmlport = o->value();
	::xmlport = atoi(progStatus.xmlport.c_str());
	set_server_port(::xmlport);
}

static void cb_poll_smeter(Fl_Check_Button* o, void*) {
	progStatus.poll_smeter = o->value();
}

static void cb_poll_pout(Fl_Check_Button* o, void*) {
	progStatus.poll_pout = o->value();
}

static void cb_poll_swr(Fl_Check_Button* o, void*) {
	progStatus.poll_swr = o->value();
}

static void cb_poll_alc(Fl_Check_Button* o, void*) {
	progStatus.poll_alc = o->value();
}

static void cb_poll_frequency(Fl_Check_Button* o, void*) {
	progStatus.poll_frequency = o->value();
}

static void cb_poll_mode(Fl_Check_Button* o, void*) {
	progStatus.poll_mode = o->value();
}

static void cb_poll_bandwidth(Fl_Check_Button* o, void*) {
	progStatus.poll_bandwidth = o->value();
}

static void cb_poll_volume(Fl_Check_Button* o, void*) {
	progStatus.poll_volume = o->value();
}

static void cb_poll_micgain(Fl_Check_Button* o, void*) {
	progStatus.poll_micgain = o->value();
}

static void cb_poll_rfgain(Fl_Check_Button* o, void*) {
	progStatus.poll_rfgain = o->value();
}

static void cb_poll_power_control(Fl_Check_Button* o, void*) {
	progStatus.poll_power_control = o->value();
}

static void cb_poll_ifshift(Fl_Check_Button* o, void*) {
	progStatus.poll_ifshift = o->value();
}

static void cb_poll_notch(Fl_Check_Button* o, void*) {
	progStatus.poll_notch = o->value();
}

static void cb_poll_auto_notch(Fl_Check_Button* o, void*) {
	progStatus.poll_auto_notch = o->value();
}

static void cb_poll_pre_att(Fl_Check_Button* o, void*) {
	progStatus.poll_pre_att = o->value();
}

static void cb_poll_squelch(Fl_Check_Button* o, void*) {
	progStatus.poll_squelch = o->value();
}

static void cb_poll_split(Fl_Check_Button* o, void*) {
	progStatus.poll_split = o->value();
}

static void cb_poll_noise(Fl_Check_Button* o, void*) {
	progStatus.poll_noise = o->value();
}

static void cb_poll_nr(Fl_Check_Button* o, void*) {
	progStatus.poll_nr = o->value();
}

static void cb_poll_compression(Fl_Check_Button* o, void *) {
	progStatus.poll_compression = o->value();
}

static void cb_poll_tuner(Fl_Check_Button* o, void *) {
	progStatus.poll_tuner = o->value();
}

static void cb_poll_meters(Fl_Check_Button* o, void*) {
	progStatus.poll_meters = o->value();
}

static void cb_poll_ptt(Fl_Check_Button* o, void*) {
	progStatus.poll_ptt = o->value();
}

static void cb_poll_break_in(Fl_Check_Button* o, void*) {
	progStatus.poll_break_in = o->value();
}

static void cb_btnSetMeters(Fl_Button*, void*) {
	poll_smeter->value(progStatus.poll_meters);
	poll_pout->value(progStatus.poll_meters);
	poll_swr->value(progStatus.poll_meters);
	poll_alc->value(progStatus.poll_meters);

	progStatus.poll_smeter = progStatus.poll_meters;
	progStatus.poll_pout = progStatus.poll_meters;
	progStatus.poll_swr = progStatus.poll_meters;
	progStatus.poll_alc = progStatus.poll_meters;
}

static void cb_poll_ops(Fl_Check_Button* o, void*) {
	progStatus.poll_ops = o->value();
}

static void cb_btnSetOps(Fl_Button*, void*) {
	poll_frequency->value(progStatus.poll_ops);
	poll_mode->value(progStatus.poll_ops);
	poll_bandwidth->value(progStatus.poll_ops);

	progStatus.poll_frequency = progStatus.poll_ops;
	progStatus.poll_mode = progStatus.poll_ops;
	progStatus.poll_bandwidth = progStatus.poll_ops;
}

static void cb_poll_all(Fl_Check_Button* o, void*) {
	progStatus.poll_all = o->value();
}

static void cb_btnSetAdd(Fl_Button*, void*) {
	poll_volume->value(progStatus.poll_all);
	poll_micgain->value(progStatus.poll_all);
	poll_rfgain->value(progStatus.poll_all);
	poll_power_control->value(progStatus.poll_all);
	poll_ifshift->value(progStatus.poll_all);
	poll_notch->value(progStatus.poll_all);
	poll_auto_notch->value(progStatus.poll_all);
	poll_pre_att->value(progStatus.poll_all);
	poll_squelch->value(progStatus.poll_all);
	poll_split->value(progStatus.poll_all);
	poll_noise->value(progStatus.poll_all);
	poll_nr->value(progStatus.poll_all);
	poll_compression->value(progStatus.poll_all);
	poll_tuner->value(progStatus.poll_all);
	poll_ptt->value(progStatus.poll_all);
	poll_break_in->value(progStatus.poll_all);

	progStatus.poll_volume = 
	progStatus.poll_micgain = 
	progStatus.poll_rfgain = 
	progStatus.poll_power_control = 
	progStatus.poll_ifshift = 
	progStatus.poll_notch = 
	progStatus.poll_auto_notch = 
	progStatus.poll_pre_att = 
	progStatus.poll_squelch = 
	progStatus.poll_split = 
	progStatus.poll_noise = 
	progStatus.poll_nr = 
	progStatus.poll_compression = 
	progStatus.poll_tuner = 
	progStatus.poll_ptt = 
	progStatus.poll_break_in = 
		progStatus.poll_all;
}

static void cb_btn_send_command(Fl_Button *o, void*) {
	cb_send_command(txt_command->value(), txt_response);
}

static const char hexsym[] = "0123456789ABCDEF";
static std::string str2hex(std::string str)
{
	static std::string hexbuf;

	size_t len = str.length();

	if (!len) return "";
 
	hexbuf.clear();
	for (size_t i = 0; i < len; i++) {
		hexbuf += 'x';
		hexbuf += hexsym[(str[i] & 0xFF) >> 4];
		hexbuf += hexsym[str[i] & 0xF];
		hexbuf += ' ';
	}
	return hexbuf;
}

static void cb_btn_icom_pre(Fl_Button *o, void*) {
	if (!selrig->ICOMrig) return;
	txt_command->value(str2hex(selrig->pre_to).c_str());
}

static void cb_btn_icom_post(Fl_Button *o, void*) {
	if (!selrig->ICOMrig) return;
	std::string s = txt_command->value();
	s.append(str2hex(selrig->post));
	txt_command->value(s.c_str());
}

static void cb_cmdlbl(Fl_Input2 *o, void *d) {
	size_t val = reinterpret_cast<size_t>(d);
	struct BPAIR { std::string *plbl; Fl_Button *b; } bpairs[] =
	{ { &progStatus.label1, btnUser1 },
	  { &progStatus.label2, btnUser2 },
	  { &progStatus.label3, btnUser3 },
	  { &progStatus.label4, btnUser4 },
	  { &progStatus.label5, btnUser5 },
	  { &progStatus.label6, btnUser6 },
	  { &progStatus.label7, btnUser7 },
	  { &progStatus.label8, btnUser8 },
	  { &progStatus.label9, btnUser9 },
	  { &progStatus.label10, btnUser10 },
	  { &progStatus.label11, btnUser11 },
	  { &progStatus.label12, btnUser12 },
	  { &progStatus.label13, btnUser13 },
	  { &progStatus.label14, btnUser14 },
	  { &progStatus.label15, btnUser15 },
	  { &progStatus.label16, btnUser16 },
	  { &progStatus.label17, btnUser17 },
	  { &progStatus.label18, btnUser18 },
	  { &progStatus.label19, btnUser19 },
	  { &progStatus.label20, btnUser20 },
	  { &progStatus.label21, btnUser21 },
	  { &progStatus.label22, btnUser22 },
	  { &progStatus.label23, btnUser23 },
	  { &progStatus.label24, btnUser24 } };

	if (val < 0) val = 0;
	if (val > 23) val = 23;

	*bpairs[val].plbl = o->value();
	bpairs[val].b->label(o->value());
	bpairs[val].b->redraw_label();
}

static void cb_cmdtext(Fl_Input2 *o, void *d) {
	size_t val = reinterpret_cast<size_t>(d);
	std::string *cmd[] =
	{ &progStatus.command1, &progStatus.command2,
	  &progStatus.command3, &progStatus.command4,
	  &progStatus.command5, &progStatus.command6,
	  &progStatus.command7, &progStatus.command8,
	  &progStatus.command9, &progStatus.command10,
	  &progStatus.command11, &progStatus.command12,
	  &progStatus.command13, &progStatus.command14,
	  &progStatus.command15, &progStatus.command16,
	  &progStatus.command17, &progStatus.command18,
	  &progStatus.command19, &progStatus.command20,
	  &progStatus.command21, &progStatus.command22,
	  &progStatus.command23, &progStatus.command24 };
	if (val < 0) val = 0;
	if (val > 23) val = 23;

	*cmd[val] = o->value();
}

static void cb_shftcmdtext(Fl_Input2 *o, void *d) {
	size_t val = reinterpret_cast<size_t>(d);
	std::string *cmd[] =
	{ &progStatus.shftcmd1, &progStatus.shftcmd2,
	  &progStatus.shftcmd3, &progStatus.shftcmd4,
	  &progStatus.shftcmd5, &progStatus.shftcmd6,
	  &progStatus.shftcmd7, &progStatus.shftcmd8,
	  &progStatus.shftcmd9, &progStatus.shftcmd10,
	  &progStatus.shftcmd11, &progStatus.shftcmd12,
	  &progStatus.shftcmd13, &progStatus.shftcmd14,
	  &progStatus.shftcmd15, &progStatus.shftcmd16,
	  &progStatus.shftcmd17, &progStatus.shftcmd18,
	  &progStatus.shftcmd19, &progStatus.shftcmd20,
	  &progStatus.shftcmd21, &progStatus.shftcmd22,
	  &progStatus.shftcmd23, &progStatus.shftcmd24 };
	if (val < 0) val = 0;
	if (val > 23) val = 23;

	*cmd[val] = o->value();
}

static void cb_se_label(Fl_Input2 *o, void *d) {
	size_t val = reinterpret_cast<size_t>(d);
	std::string *labels[] =
	{ &progStatus.label_on_start1, &progStatus.label_on_start2,
	  &progStatus.label_on_start3, &progStatus.label_on_start4,
	  &progStatus.label_on_exit1,  &progStatus.label_on_exit2,
	  &progStatus.label_on_exit3,  &progStatus.label_on_exit4,
	};
	if (val < 0) val = 0;
	if (val > 8) val = 8;
	*labels[val] = o->value();
}

static void cb_se_text(Fl_Input2 *o, void *d) {
	size_t val = reinterpret_cast<size_t>(d);
	std::string *cmd[] =
	{ &progStatus.cmd_on_start1, &progStatus.cmd_on_start2,
	  &progStatus.cmd_on_start3, &progStatus.cmd_on_start4,
	  &progStatus.cmd_on_exit1,  &progStatus.cmd_on_exit2,
	  &progStatus.cmd_on_exit3,  &progStatus.cmd_on_exit4,
	};
	if (val < 0) val = 0;
	if (val > 8) val = 8;

	*cmd[val] = o->value();
}

static void cb_init_ser_port(Fl_Return_Button*, void*) {
	std::string p1 = selectCommPort->value();
	std::string p2 = selectAuxPort->value();
	std::string p3 = selectSepPTTPort->value();

	if ( (p1.compare("NONE") != 0) && (p1 == p2 || p1 == p3) ) {
		fl_message("Select separate ports");
		return;
	}

	if (progStatus.UIsize == wide_ui) {
		btn_show_controls->label("@-22->");
		btn_show_controls->redraw_label();
		mainwindow->size(mainwindow->w(), WIDE_MAINH);
		mainwindow->size_range(WIDE_MAINW, WIDE_MAINH, 0, WIDE_MAINH);
	}

	// close the current rig control
	closeRig();               // local serial comm connection

	{ guard_lock gl_serial(&mutex_serial);
		bypass_serial_thread_loop = true;
trace(1, "close serial port");
		RigSerial->ClosePort();
	}
trace(1, "clear frequency list");
	clearList();
	saveFreqList();
	selrig = rigs[selectRig->index()];
	xcvr_name = selrig->name_;

	progStatus.xcvr_serial_port = selectCommPort->value();

	progStatus.comm_baudrate = mnuBaudrate->index();
	progStatus.stopbits = btnOneStopBit->value() ? 1 : 2;
	progStatus.comm_retries = (int)cntRigCatRetries->value();
	progStatus.comm_timeout = (int)cntRigCatTimeout->value();
	progStatus.comm_wait = (int)cntRigCatWait->value();
	progStatus.comm_echo = btnRigCatEcho->value();

	progStatus.comm_rtsptt = lbox_rtsptt->index();
	progStatus.comm_catptt = lbox_catptt->index();
	progStatus.comm_dtrptt = lbox_dtrptt->index();

	progStatus.comm_rtscts = chkrtscts->value();
	progStatus.comm_rtsplus = btnrtsplus1->value();
	progStatus.comm_dtrplus = btndtrplus1->value();

	progStatus.imode_B  = progStatus.imode_A  = selrig->def_mode;
	progStatus.iBW_B    = progStatus.iBW_A    = selrig->def_bw;
	progStatus.freq_B   = progStatus.freq_A   = selrig->def_freq;
trace(1, "initialize title bar");
	init_title();
trace(1, "start tranceiver serial port");
	if (!startXcvrSerial()) {
trace(1, "FAILED");
		if (progStatus.xcvr_serial_port.compare("NONE") == 0) {
			LOG_WARN("No comm port ... test mode");
		} else {
//			progStatus.xcvr_serial_port = "NONE";
//			selectCommPort->value(progStatus.xcvr_serial_port.c_str());
		}
	}
trace(1, "initialize transceiver");
	initRig();

	btn_init_ser_port->labelcolor(FL_BLACK);
	btn_init_ser_port->redraw_label();

	btn2_init_ser_port->labelcolor(FL_BLACK);
	btn2_init_ser_port->redraw_label();
}

static void cb_btnOkSepSerial(Fl_Button*, void*) {
	std::string p2 = selectAuxPort->value();
	std::string p3 = selectSepPTTPort->value();

	if ((p3.compare("NONE") != 0) && p2 == p3) {
		fl_message("Port used for Aux i/o");
		return;
	}

	progStatus.sep_serial_port = selectSepPTTPort->value();
	progStatus.sep_SCU_17 = btnSep_SCU_17->value();

	progStatus.sep_dtrplus = btnSepDTRplus->value();
	progStatus.sep_dtrptt = lbox_sep_dtrptt->index();

	progStatus.sep_rtsplus = btnSepRTSplus->value();
	progStatus.sep_rtsptt = lbox_sep_rtsptt->index();

	if (!startSepSerial()) {
		if (progStatus.sep_serial_port.compare("NONE") != 0) {
			progStatus.sep_serial_port = "NONE";
			selectSepPTTPort->value(progStatus.sep_serial_port.c_str());
		}
	}

	btnOkSepSerial->labelcolor(FL_BLACK);
	btnOkSepSerial->redraw_label();
}

static void cb_btnOkAuxSerial(Fl_Button*, void*) {
	AuxSerial->ClosePort();

	std::string p2 = selectAuxPort->value();
	std::string p3 = selectSepPTTPort->value();

	if ((p2.compare("NONE") != 0) && p2 == p3) {
		fl_message("Port used for Sep serial");
		return;
	}

	progStatus.aux_serial_port = selectAuxPort->value();
	progStatus.aux_SCU_17 = btnAux_SCU_17->value();

	if (!startAuxSerial()) {
		if (progStatus.aux_serial_port.compare("NONE") != 0) {
			progStatus.aux_serial_port = "NONE";
			selectAuxPort->value(progStatus.aux_serial_port.c_str());
		}
	}

	btnOkAuxSerial->labelcolor(FL_BLACK);
	btnOkAuxSerial->redraw_label();
}

static void cb_restore(Fl_Check_Button *btn, void*)
{
	progStatus.restore_frequency = btnRestoreFrequency->value();
	progStatus.restore_mode = btnRestoreMode->value();
	progStatus.restore_bandwidth =  btnRestoreBandwidth->value();
	progStatus.restore_volume = btnRestoreVolume->value();
	progStatus.restore_mic_gain = btnRestoreMicGain->value();
	progStatus.restore_rf_gain = btnRestoreRfGain->value();
	progStatus.restore_power_control = btnRestorePowerControl->value();
	progStatus.restore_if_shift = btnRestoreIFshift->value();
	progStatus.restore_notch = btnRestoreNotch->value();
	progStatus.restore_auto_notch = btnRestoreAutoNotch->value();
	progStatus.restore_noise = btnRestoreNoise->value();
	progStatus.restore_squelch = btnRestoreSquelch->value();
	progStatus.restore_split = btnRestoreSplit->value();
	progStatus.restore_pre_att = btnRestorePreAtt->value();
	progStatus.restore_nr = btnRestoreNR->value();
	progStatus.restore_comp_on_off = btnRestoreCompOnOff->value();
	progStatus.restore_comp_level = btnRestoreCompLevel->value();
	progStatus.use_rig_data = btnUseRigData->value();
}

void cb_comports(Fl_Button *b, void* d)
{
	init_port_combos();
}

Fl_Group *createXCVR(int X, int Y, int W, int H, const char *label)
{
	Fl_Group *tabXCVR = new Fl_Group(X, Y, W, H, label);

	Fl_Group* xcvr_grp1 = new Fl_Group(X + 2, Y + 2, W - 4, 200);
		xcvr_grp1->box(FL_ENGRAVED_FRAME);

		selectRig = new Fl_ComboBox(
			xcvr_grp1->x() + 60, xcvr_grp1->y() + 4,
			xcvr_grp1->w() - 64, 22, _("Rig:"));
		selectRig->tooltip(_("Select Transceiver"));
		selectRig->box(FL_DOWN_BOX);
		selectRig->color(FL_BACKGROUND2_COLOR);
		selectRig->selection_color(FL_BACKGROUND_COLOR);
		selectRig->labeltype(FL_NORMAL_LABEL);
		selectRig->labelfont(0);
		selectRig->labelsize(14);
		selectRig->labelcolor(FL_FOREGROUND_COLOR);
		selectRig->callback((Fl_Callback*)cb_selectRig);
		selectRig->align(Fl_Align(FL_ALIGN_LEFT));
		selectRig->readonly();
		selectRig->when(FL_WHEN_RELEASE);
		selectRig->end();

		Fl_Button *comports = new Fl_Button(
			xcvr_grp1->x() + 4, selectRig->y() + 26,
			52, 22, _("Update"));
		comports->box(FL_THIN_UP_BOX);
		comports->tooltip(_("Update serial port combo"));
		comports->callback((Fl_Callback*)cb_comports);
		comports->when(FL_WHEN_RELEASE);

		selectCommPort = new Fl_ComboBox(
			selectRig->x(), comports->y(),
			selectRig->w(), 22, "");
		selectCommPort->tooltip(_("Xcvr serial port"));
		selectCommPort->box(FL_DOWN_BOX);
		selectCommPort->color(FL_BACKGROUND2_COLOR);
		selectCommPort->selection_color(FL_BACKGROUND_COLOR);
		selectCommPort->labeltype(FL_NORMAL_LABEL);
		selectCommPort->labelfont(0);
		selectCommPort->labelsize(14);
		selectCommPort->labelcolor(FL_FOREGROUND_COLOR);
		selectCommPort->callback((Fl_Callback*)cb_selectCommPort);
		selectCommPort->align(Fl_Align(FL_ALIGN_CENTER));
		selectCommPort->when(FL_WHEN_RELEASE);
		selectCommPort->end();

		mnuBaudrate = new Fl_ComboBox(
			selectCommPort->x(), selectCommPort->y() + 26,
			160, 22, _("Baud:"));
		mnuBaudrate->tooltip(_("Xcvr baudrate"));
		mnuBaudrate->box(FL_DOWN_BOX);
		mnuBaudrate->color(FL_BACKGROUND2_COLOR);
		mnuBaudrate->selection_color(FL_BACKGROUND_COLOR);
		mnuBaudrate->labeltype(FL_NORMAL_LABEL);
		mnuBaudrate->labelfont(0);
		mnuBaudrate->labelsize(14);
		mnuBaudrate->labelcolor(FL_FOREGROUND_COLOR);
		mnuBaudrate->callback((Fl_Callback*)cb_mnuBaudrate);
		mnuBaudrate->align(Fl_Align(FL_ALIGN_LEFT));
		mnuBaudrate->readonly();
		mnuBaudrate->when(FL_WHEN_RELEASE);
		mnuBaudrate->end();

		btnOneStopBit = new Fl_Check_Button(
			mnuBaudrate->x(), mnuBaudrate->y() + 26,
			22, 22, _("1"));
		btnOneStopBit->tooltip(_("One Stop Bit"));
		btnOneStopBit->down_box(FL_DOWN_BOX);
		btnOneStopBit->callback((Fl_Callback*)cb_btnOneStopBit);
		btnOneStopBit->align(Fl_Align(FL_ALIGN_RIGHT));
		btnOneStopBit->value(progStatus.stopbits == 1);

		btnTwoStopBit = new Fl_Check_Button(
			btnOneStopBit->x() + 120, btnOneStopBit->y(),
			22, 22, _("2 -StopBits"));
		btnTwoStopBit->down_box(FL_DOWN_BOX);
		btnTwoStopBit->callback((Fl_Callback*)cb_btnTwoStopBit);
		btnTwoStopBit->align(Fl_Align(FL_ALIGN_RIGHT));
		btnTwoStopBit->value(progStatus.stopbits == 2);

		btnRigCatEcho = new Fl_Check_Button(
			btnOneStopBit->x(), btnOneStopBit->y() + 25,
			22, 22, _("Echo "));
		btnRigCatEcho->down_box(FL_DOWN_BOX);
		btnRigCatEcho->callback((Fl_Callback*)cb_btnRigCatEcho);
		btnRigCatEcho->align(Fl_Align(FL_ALIGN_RIGHT));
		btnRigCatEcho->value(progStatus.comm_echo);

		chkrtscts = new Fl_Check_Button(
			btnTwoStopBit->x(), btnRigCatEcho->y(),
			22, 22, _("RTS/CTS"));
		chkrtscts->tooltip(_("Xcvr uses RTS/CTS handshake"));
		chkrtscts->down_box(FL_DOWN_BOX);
		chkrtscts->callback((Fl_Callback*)cb_chkrtscts);
		chkrtscts->value(progStatus.comm_rtscts);

		btnrtsplus1 = new Fl_Check_Button(
			btnRigCatEcho->x(), btnRigCatEcho->y() + 25, 22, 22, _("RTS +12 v"));
		btnrtsplus1->tooltip(_("Initial state of RTS"));
		btnrtsplus1->callback((Fl_Callback*)cb_btnrtsplus);
		btnrtsplus1->value(progStatus.comm_rtsplus);

		btndtrplus1 = new Fl_Check_Button(
			chkrtscts->x(), btnrtsplus1->y(), 22, 22, _("DTR +12 v"));
		btndtrplus1->tooltip(_("Initial state of DTR"));
		btndtrplus1->callback((Fl_Callback*)cb_btndtrplus);
		btndtrplus1->value(progStatus.comm_dtrplus);

		cntRigCatRetries = new Fl_Counter(
			xcvr_grp1->x() + xcvr_grp1->w() - 110 - 4, mnuBaudrate->y(),
			110, 22, _("Retries"));
		cntRigCatRetries->tooltip(_("Number of  times to resend\ncommand before giving up"));
		cntRigCatRetries->minimum(1);
		cntRigCatRetries->maximum(10);
		cntRigCatRetries->step(1);
		cntRigCatRetries->value(5);
		cntRigCatRetries->callback((Fl_Callback*)cb_cntRigCatRetries);
		cntRigCatRetries->align(Fl_Align(FL_ALIGN_LEFT));
		cntRigCatRetries->value(progStatus.comm_retries);
		cntRigCatRetries->lstep(10);

		cntRigCatTimeout = new Fl_Counter(
			cntRigCatRetries->x(), btnOneStopBit->y(),
			110, 22, _("Timeout"));
		cntRigCatTimeout->tooltip(_("Serial port select timeout"));
		cntRigCatTimeout->minimum(0);
		cntRigCatTimeout->maximum(100);
		cntRigCatTimeout->step(1);
		cntRigCatTimeout->value(10);
		cntRigCatTimeout->callback((Fl_Callback*)cb_cntRigCatTimeout);
		cntRigCatTimeout->align(Fl_Align(FL_ALIGN_LEFT));
		cntRigCatTimeout->value(progStatus.comm_timeout);
		cntRigCatTimeout->lstep(10);

		cntRigCatWait = new Fl_Counter(
			cntRigCatRetries->x(), btnRigCatEcho->y(),
			110, 22, _("Cmds"));
		cntRigCatWait->tooltip(_("Wait millseconds between set/get commands"));
		cntRigCatWait->minimum(0);
		cntRigCatWait->maximum(100);
		cntRigCatWait->step(1);
		cntRigCatWait->value(5);
		cntRigCatWait->callback((Fl_Callback*)cb_cntRigCatWait);
		cntRigCatWait->align(Fl_Align(FL_ALIGN_LEFT));
		cntRigCatWait->value(progStatus.comm_wait);
		cntRigCatWait->lstep(10);

		query_interval = new Fl_Counter(
			cntRigCatRetries->x(), cntRigCatWait->y() + 26,
			110, 22, _("Poll intvl"));
		query_interval->tooltip(_("Polling interval in msec"));
		query_interval->minimum(10);
		query_interval->maximum(5000);
		query_interval->step(1);
		query_interval->value(50);
		query_interval->callback((Fl_Callback*)cb_query_interval);
		query_interval->align(Fl_Align(FL_ALIGN_LEFT));
		query_interval->value(progStatus.serloop_timing);
		query_interval->lstep(10);

		byte_interval = new Fl_Counter(
			cntRigCatRetries->x(), query_interval->y() + 26,
			110, 22, _("Byte intvl"));
		byte_interval->tooltip(_("Inter-byte interval (msec)"));
		byte_interval->minimum(0);
		byte_interval->maximum(200);
		byte_interval->step(1);
		byte_interval->value(0);
		byte_interval->callback((Fl_Callback*)cb_byte_interval);
		byte_interval->align(Fl_Align(FL_ALIGN_LEFT));
		byte_interval->value(progStatus.byte_interval);
		byte_interval->lstep(10);

		Fl_Group* xcvr_grp4 = new Fl_Group(
			xcvr_grp1->x() + 2, xcvr_grp1->y() + xcvr_grp1->h() - 38, 
			150, 36);
		xcvr_grp4->box(FL_ENGRAVED_FRAME);
		xcvr_grp4->align(Fl_Align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE));

			txtCIV = new Fl_Int_Input(
				xcvr_grp4->x() + 4, xcvr_grp4->y() + 8,
				58, 22, _("CI-V adr"));
			txtCIV->tooltip(_("Enter hex value, ie: 0x5F"));
			txtCIV->type(2);
			txtCIV->callback((Fl_Callback*)cb_txtCIV);
			txtCIV->align(Fl_Align(FL_ALIGN_RIGHT));

			btnCIVdefault = new Fl_Button(
				txtCIV->x() + txtCIV->w() + 4, txtCIV->y(),
				70, 22, _("Default"));
			btnCIVdefault->callback((Fl_Callback*)cb_btnCIVdefault);

		xcvr_grp4->end();

	xcvr_grp1->end();

	Fl_Group* xcvr_grp5 = new Fl_Group(
		xcvr_grp1->x(), xcvr_grp1->y() + xcvr_grp1->h() + 2,
		152, H - xcvr_grp1->y() - xcvr_grp1->h() - 4);
	xcvr_grp5->box(FL_ENGRAVED_FRAME);

		btnUSBaudio = new Fl_Check_Button(
			xcvr_grp5->x() + 4, xcvr_grp5->y() + xcvr_grp5->h()  / 2 - 9,
			100, 18, _("USB audio"));
		btnUSBaudio->down_box(FL_DOWN_BOX);
		btnUSBaudio->callback((Fl_Callback*)cb_btnUSBaudio);

	xcvr_grp5->end();

	box_xcvr_connect = new Fl_Box(
		X + W - 90, Y + H - 27,
		18, 18, _("Connected"));
	box_xcvr_connect->tooltip(_("Lit when connected"));
	box_xcvr_connect->box(FL_DIAMOND_DOWN_BOX);
	box_xcvr_connect->color(FL_LIGHT1);
	box_xcvr_connect->align(Fl_Align(FL_ALIGN_LEFT));

	btn_init_ser_port = new Fl_Button(
		X + W - 60, Y + H - 30,
		50, 24, _("Init"));
	btn_init_ser_port->callback((Fl_Callback*)cb_init_ser_port);

	tabXCVR->end();

	return tabXCVR;
}

Fl_Group *createTRACE(int X, int Y, int W, int H, const char *label)
{
	Fl_Group *tabTRACE = new Fl_Group(X, Y, W, H, label);

	tabTRACE->hide();
	btn_trace = new Fl_Check_Button(X + 10, Y + 20, 80, 20, _("Trace support code"));
	btn_trace->value(progStatus.trace);
	btn_trace->callback((Fl_Callback*)cb_btn_trace);
	btn_trace->tooltip(_("Enable trace support"));

	btn_debugtrace = new Fl_Check_Button(X + 10, Y + 50, 80, 20, _("Trace debug code"));
	btn_debugtrace->value(progStatus.debugtrace);
	btn_debugtrace->callback((Fl_Callback*)cb_btn_debugtrace);
	btn_debugtrace->tooltip(_("Display debug output on trace view"));

	btn_rigtrace = new Fl_Check_Button(X + 10, Y + 80, 80, 20, _("Trace rig class code"));
	btn_rigtrace->value(progStatus.rigtrace);
	btn_rigtrace->callback((Fl_Callback*)cb_btn_rigtrace);
	btn_rigtrace->tooltip(_("Enable trace of rig methods"));

	btn_gettrace = new Fl_Check_Button(X + 10, Y + 110, 80, 20, _("Trace rig class get code"));
	btn_gettrace->value(progStatus.gettrace);
	btn_gettrace->callback((Fl_Callback*)cb_btn_gettrace);
	btn_gettrace->tooltip(_("Enable trace of rig get methods"));

	btn_settrace = new Fl_Check_Button(X + 10, Y + 140, 80, 20, _("Trace rig class set code"));
	btn_settrace->value(progStatus.settrace);
	btn_settrace->callback((Fl_Callback*)cb_btn_settrace);
	btn_settrace->tooltip(_("Enable trace of rig set methods"));

	btn_xmltrace = new Fl_Check_Button(X + 240, Y + 20, 80, 20, _("Trace xml_server code"));
	btn_xmltrace->value(progStatus.xmltrace);
	btn_xmltrace->callback((Fl_Callback*)cb_btn_xmltrace);
	btn_xmltrace->tooltip(_("Enable trace of xmlrpc functions"));

	btn_rpctrace = new Fl_Check_Button(X + 240, Y + 50, 80, 20, _("Trace xmlrpcpp code"));
	btn_rpctrace->value(progStatus.rpctrace);
	btn_rpctrace->callback((Fl_Callback*)cb_btn_rpctrace);
	btn_rpctrace->tooltip(_("Enable trace of XmlRpc methods"));

	btn_serialtrace = new Fl_Check_Button(X + 240, Y + 80, 80, 20, _("Trace serial code"));
	btn_serialtrace->value(progStatus.serialtrace);
	btn_serialtrace->callback((Fl_Callback*)cb_btn_serialtrace);
	btn_serialtrace->tooltip(_("Enable trace of serial i/o"));

	btn_start_stop_trace = new Fl_Check_Button(X + 240, Y + 110, 80, 20, _("Trace start/stop code"));
	btn_start_stop_trace->value(progStatus.start_stop_trace);
	btn_start_stop_trace->callback((Fl_Callback*)cb_btn_start_stop_trace);
	btn_start_stop_trace->tooltip(_("Enable trace of start/stop operations"));

	selectlevel = new Fl_ComboBox(X + 240, Y + 140, 80, 20, _("XmlRpc trace level"));
	selectlevel->add("0|1|2|3|4");
	selectlevel->align(FL_ALIGN_RIGHT);
	selectlevel->index(progStatus.rpc_level);
	selectlevel->tooltip(_("0 = off ... 4 maximum depth"));
	selectlevel->readonly();
	selectlevel->callback((Fl_Callback*)cb_selectlevel);

	btn_viewtrace = new Fl_Button(X + W - 90, Y + H - 30, 85, 24, _("View Trace"));
	btn_viewtrace->callback((Fl_Callback*)cb_btn_viewtrace);

	tabTRACE->end();

	return tabTRACE;
}

Fl_Group *createTCPIP(int X, int Y, int W, int H, const char *label)
{
	Fl_Group * tabTCPIP = new Fl_Group(X, Y, W, H, label);
		tabTCPIP->hide();

		inp_tcpip_addr = new Fl_Input2(X + 120, Y + 20, 300, 22, _("TCPIP address:"));
		inp_tcpip_addr->tooltip(_("remote tcpip server address"));
		inp_tcpip_addr->callback((Fl_Callback*)cb_tcpip_addr);
		inp_tcpip_addr->value(progStatus.tcpip_addr.c_str());

		inp_tcpip_port = new Fl_Input2(X + 120, Y + 44, 100, 22, _("TCPIP port:"));
		inp_tcpip_port->tooltip(_("remote tcpip server port"));
		inp_tcpip_port->type(2);
		inp_tcpip_port->callback((Fl_Callback*)cb_tcpip_port);
		inp_tcpip_port->value(progStatus.tcpip_port.c_str());

		inp_tcpip_ping_delay = new Fl_Counter(X + 120, Y + 70, 100, 22, _("Ping delay"));
		inp_tcpip_ping_delay->tooltip(_("enter round trip ping delay"));
		inp_tcpip_ping_delay->callback((Fl_Callback*)cb_tcpip_ping_delay);
		inp_tcpip_ping_delay->minimum(0);
		inp_tcpip_ping_delay->maximum(500);
		inp_tcpip_ping_delay->step(5);
		inp_tcpip_ping_delay->lstep(20);
		inp_tcpip_ping_delay->value(progStatus.tcpip_ping_delay);
		inp_tcpip_ping_delay->align(Fl_Align(FL_ALIGN_LEFT));

		chk_use_tcpip = new Fl_Check_Button(X + 120, Y + 95, 18, 18, _("Use tcpip"));
		chk_use_tcpip->tooltip(_("Rig control via tcpip"));
		chk_use_tcpip->callback((Fl_Callback*)cb_use_tcpip);
		chk_use_tcpip->value(progStatus.use_tcpip);
		chk_use_tcpip->align(Fl_Align(FL_ALIGN_LEFT));

		box_tcpip_connect = new Fl_Box(X + 120, Y + 120, 18, 18, _("Connected"));
		box_tcpip_connect->tooltip(_("Lit when connected to remote tcpip"));
		box_tcpip_connect->box(FL_DIAMOND_DOWN_BOX);
		box_tcpip_connect->color(FL_LIGHT1);
		box_tcpip_connect->align(Fl_Align(FL_ALIGN_RIGHT));

		cntRetryAfter = new Fl_Counter(X + 120, Y + 145, 100, 20, _("Retry (secs)"));
		cntRetryAfter->tooltip(_("Retry connection if lost"));
		cntRetryAfter->minimum(1);
		cntRetryAfter->maximum(120);
		cntRetryAfter->step(1);
		cntRetryAfter->lstep(10);
		cntRetryAfter->callback((Fl_Callback*)cb_cntRetryAfter);
		cntRetryAfter->align(Fl_Align(FL_ALIGN_LEFT));
		cntRetryAfter->value(progStatus.tcpip_reconnect_after);

		cntDropsAllowed = new Fl_Counter(X + 120, Y + 170, 100, 20, _("Allowed drops"));
		cntDropsAllowed->tooltip(_("# tcpip drop-outs before connection declared down"));
		cntDropsAllowed->minimum(1);
		cntDropsAllowed->maximum(25);
		cntDropsAllowed->step(1);
		cntDropsAllowed->lstep(5);
		cntDropsAllowed->callback((Fl_Callback*)cb_cntDropsAllowed);
		cntDropsAllowed->align(Fl_Align(FL_ALIGN_LEFT));
		cntDropsAllowed->value(progStatus.tcpip_drops_allowed);

	tabTCPIP->end();

	return tabTCPIP;
}

Fl_Group *createPTT(int X, int Y, int W, int H, const char *label)
{
	Fl_Group *tab = new Fl_Group(X, Y, W, H, label);
	tab->hide();

	Fl_Group *grp_CW_ptt = new Fl_Group(X + 2, Y + 20, W - 4, 30, 
		_("CW mode PTT"));
		grp_CW_ptt->box(FL_ENGRAVED_BOX);
		grp_CW_ptt->align(Fl_Align(FL_ALIGN_TOP_LEFT));

		Fl_Check_Button *btn_disable_CW_ptt = new Fl_Check_Button(
			grp_CW_ptt->x() + 10, grp_CW_ptt->y() + 4, 200, 22,
			_("disable PTT in CW mode"));
			btn_disable_CW_ptt->value(progStatus.disable_CW_ptt);
			btn_disable_CW_ptt->callback((Fl_Callback*)cb_disable_CW_ptt); 

	grp_CW_ptt->end();

	Fl_Group *grp_catptt = new Fl_Group (
		grp_CW_ptt->x(), grp_CW_ptt->y() + grp_CW_ptt->h() + 20,
		grp_CW_ptt->w(), 60, _("PTT on CAT Serial Port"));

		grp_catptt->box(FL_ENGRAVED_BOX);
		grp_catptt->align(Fl_Align(FL_ALIGN_TOP_LEFT));

		int incr = (grp_CW_ptt->w() - 10) / 3.5;

		lbox_catptt = new Fl_ListBox(
			grp_catptt->x() + 5, grp_catptt->y() + 8, 90, 22, _("CAT"));
		lbox_catptt->tooltip(_("PTT is a CAT command (not hardware)"));
		lbox_catptt->callback((Fl_Callback*)cb_lbox_catptt);
		lbox_catptt->add("OFF|BOTH|SET|GET");
		lbox_catptt->index(progStatus.comm_catptt);
		lbox_catptt->align(FL_ALIGN_RIGHT);

		lbox_rtsptt = new Fl_ListBox(
			lbox_catptt->x() + incr, grp_catptt->y() + 8, 90, 22, _("RTS"));
		lbox_rtsptt->tooltip(_("RTS is ptt line"));
		lbox_rtsptt->callback((Fl_Callback*)cb_lbox_rtsptt);
		lbox_rtsptt->add("OFF|BOTH|SET|GET");
		lbox_rtsptt->index(progStatus.comm_rtsptt);
		lbox_rtsptt->align(FL_ALIGN_RIGHT);

		lbox_dtrptt = new Fl_ListBox(
			lbox_rtsptt->x() + incr, grp_catptt->y() + 8, 90, 22, _("DTR"));
		lbox_dtrptt->tooltip(_("DTR is ptt line"));
		lbox_dtrptt->callback((Fl_Callback*)cb_lbox_dtrptt);
		lbox_dtrptt->add("OFF|BOTH|SET|GET");
		lbox_dtrptt->index(progStatus.comm_dtrptt);
		lbox_dtrptt->align(FL_ALIGN_RIGHT);

		btn2_init_ser_port = new Fl_Button(
			lbox_dtrptt->x() + incr, lbox_dtrptt->y(), 50, 24, _("Init"));
		btn2_init_ser_port->callback((Fl_Callback*)cb_init_ser_port);

		btnrtsplus2 = new Fl_Check_Button(
			lbox_rtsptt->x(), lbox_rtsptt->y() + 24, 100, 21, _("RTS +12 v"));
		btnrtsplus2->tooltip(_("Initial state of RTS"));
		btnrtsplus2->callback((Fl_Callback*)cb_btnrtsplus);
		btnrtsplus2->value(progStatus.comm_rtsplus);

		btndtrplus2 = new Fl_Check_Button(
			lbox_dtrptt->x(), lbox_dtrptt->y() + 24, 100, 21, _("DTR +12 v"));
		btndtrplus2->tooltip(_("Initial state of DTR"));
		btndtrplus2->callback((Fl_Callback*)cb_btndtrplus);
		btndtrplus2->value(progStatus.comm_dtrplus);

	grp_catptt->end();

	Fl_Group *grp_ptt = new Fl_Group(
		grp_catptt->x(), grp_catptt->y() + grp_catptt->h() + 20,
		grp_catptt->w(), H - (grp_catptt->y() + grp_catptt->h() + 20),
_("PTT control on Separate Serial Port"));
		grp_ptt->box(FL_ENGRAVED_BOX);
		grp_ptt->align(Fl_Align(FL_ALIGN_TOP_LEFT));

		selectSepPTTPort = new Fl_ComboBox(
			grp_catptt->x() + 50, grp_ptt->y() + 10,
			grp_ptt->w() - 54, 22, _("Port"));
		selectSepPTTPort->tooltip(_("PTT on Separate Serial Port"));
		selectSepPTTPort->box(FL_DOWN_BOX);
		selectSepPTTPort->color(FL_BACKGROUND2_COLOR);
		selectSepPTTPort->selection_color(FL_BACKGROUND_COLOR);
		selectSepPTTPort->labeltype(FL_NORMAL_LABEL);
		selectSepPTTPort->labelfont(0);
		selectSepPTTPort->labelsize(14);
		selectSepPTTPort->labelcolor(FL_FOREGROUND_COLOR);
		selectSepPTTPort->callback((Fl_Callback*)cb_selectSepPTTPort);
		selectSepPTTPort->align(Fl_Align(FL_ALIGN_LEFT));
		selectSepPTTPort->when(FL_WHEN_RELEASE);
		selectSepPTTPort->end();

		btnSep_SCU_17 = new Fl_Check_Button(
			selectSepPTTPort->x(), selectSepPTTPort->y() + 28,
			128, 22, _("SCU-17"));
		btnSep_SCU_17->tooltip(_("Port is SCU-17 auxilliary\nSet stop bits to ZERO"));
		btnSep_SCU_17->callback((Fl_Callback*)cb_btnSep_SCU_17);
		btnSep_SCU_17->value(progStatus.sep_SCU_17);

		lbox_sep_rtsptt = new Fl_ListBox(
			lbox_rtsptt->x(), selectSepPTTPort->y() + 30, 90, 22, _("RTS"));
		lbox_sep_rtsptt->tooltip(_("RTS is ptt line"));
		lbox_sep_rtsptt->callback((Fl_Callback*)cb_lbox_sep_rtsptt);
		lbox_sep_rtsptt->add("OFF|BOTH|SET|GET");
		lbox_sep_rtsptt->index(progStatus.sep_rtsptt);
		lbox_sep_rtsptt->align(FL_ALIGN_RIGHT);

		lbox_sep_dtrptt = new Fl_ListBox(
			lbox_dtrptt->x(), lbox_sep_rtsptt->y(), 90, 22, _("DTR"));
		lbox_sep_dtrptt->tooltip(_("DTR is ptt line"));
		lbox_sep_dtrptt->callback((Fl_Callback*)cb_lbox_sep_dtrptt);
		lbox_sep_dtrptt->add("OFF|BOTH|SET|GET");
		lbox_sep_dtrptt->index(progStatus.sep_dtrptt);
		lbox_sep_dtrptt->align(FL_ALIGN_RIGHT);

		btnOkSepSerial = new Fl_Button(
			lbox_sep_dtrptt->x() + incr, lbox_sep_dtrptt->y(),
			50, 24, _("Init"));
		btnOkSepSerial->callback((Fl_Callback*)cb_btnOkSepSerial);

		btnSepRTSplus = new Fl_Check_Button(
			lbox_rtsptt->x(), lbox_sep_rtsptt->y() + 24, 125, 22, _("RTS +12 v"));
		btnSepRTSplus->tooltip(_("Initial state of RTS"));
		btnSepRTSplus->callback((Fl_Callback*)cb_btnSepRTSplus);
		btnSepRTSplus->value(progStatus.sep_rtsplus);

		btnSepDTRplus = new Fl_Check_Button(
			lbox_dtrptt->x(), btnSepRTSplus->y(), 125, 22, _("DTR +12 v"));
		btnSepDTRplus->tooltip(_("Initial state of DTR"));
		btnSepDTRplus->callback((Fl_Callback*)cb_btnSepDTRplus);
		btnSepDTRplus->value(progStatus.sep_dtrplus);

	grp_ptt->end();

	tab->end();

	return tab;
}

Fl_Group *createAUX(int X, int Y, int W, int H, const char *label)
{
	Fl_Group * tabAUX = new Fl_Group(X, Y, W, H, label);
	tabAUX->hide();

	selectAuxPort = new Fl_ComboBox(X + 130, Y + 60, 240, 22, _("Aux"));
	selectAuxPort->tooltip(_("Aux control port"));
	selectAuxPort->box(FL_DOWN_BOX);
	selectAuxPort->color(FL_BACKGROUND2_COLOR);
	selectAuxPort->selection_color(FL_BACKGROUND_COLOR);
	selectAuxPort->labeltype(FL_NORMAL_LABEL);
	selectAuxPort->labelfont(0);
	selectAuxPort->labelsize(14);
	selectAuxPort->labelcolor(FL_FOREGROUND_COLOR);
	selectAuxPort->callback((Fl_Callback*)cb_selectAuxPort);
	selectAuxPort->align(Fl_Align(FL_ALIGN_LEFT));
	selectAuxPort->when(FL_WHEN_RELEASE);
	selectAuxPort->end();

	btnAux_SCU_17 = new Fl_Check_Button(X + 130, Y + 100, 128, 22, _("  SCU-17 auxiliary\n  Yaesu 2nd USB port"));
	btnAux_SCU_17->tooltip(_("Set stop bits to ZERO"));
	btnAux_SCU_17->callback((Fl_Callback*)cb_btnAux_SCU_17);
	btnAux_SCU_17->value(progStatus.aux_SCU_17);

	Fl_Box *bxsep = new Fl_Box(X + 55, Y + 10, 400, 40,
_("Use only if your setup requires a separate\nSerial Port for a special Control Signals"));
	bxsep->box(FL_FLAT_BOX);

	btnOkAuxSerial = new Fl_Button(X + W - 60, Y + H - 30, 50, 24, _("Init"));
	btnOkAuxSerial->callback((Fl_Callback*)cb_btnOkAuxSerial);

	tabAUX->end();

	return tabAUX;
}

Fl_Group *createSERVER(int X, int Y, int W, int H, const char *label)
{
	Fl_Group * tabSERVER = new Fl_Group(X, Y, W, H, label);
	tabSERVER->hide();

	server_text = new Fl_Box(X + 30, Y + 10, W - 60, 60,
_("\
Change port if multiple instances of flrig / fldigi will\n\
execute concurrently.  Be sure to change the respective\n\
fldigi configuration item for xmlrpc server port."));
	server_text->box(FL_FLAT_BOX);
	server_text->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

	inp_serverport = new Fl_Input2(X + 120, Y + 80, 100, 22, _("Xmlrpc port:"));
	inp_serverport->tooltip(_("Socket port for xmlrpc server"));
	inp_serverport->type(2);
	inp_serverport->callback((Fl_Callback*)cb_server_port);
	inp_serverport->value(progStatus.xmlport.c_str());
	inp_serverport->when(FL_ENTER);

	box_fldigi_connect = new Fl_Box(X + 25, Y + 145, 18, 18, _("Xmlrpc Client Connected"));
	box_fldigi_connect->tooltip(_("Lit when connected to external xmlrpc client"));
	box_fldigi_connect->box(FL_DIAMOND_DOWN_BOX);
	box_fldigi_connect->color(FL_LIGHT1);
	box_fldigi_connect->align(Fl_Align(FL_ALIGN_RIGHT));

	tabSERVER->end();

	return tabSERVER;
}

Fl_Group *createPOLLING(int X, int Y, int W, int H, const char *label)
{
	tabPOLLING = new Fl_Group(X, Y, W, H, label);
	tabPOLLING->hide();

	Fl_Group* xcvr_grp7 = new Fl_Group(X + 5, Y + 5, W - 10, 45, _("Meters"));
		xcvr_grp7->box(FL_ENGRAVED_BOX);
		xcvr_grp7->align(Fl_Align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE));

		poll_smeter = new Fl_Check_Button(X + 10, Y + 25, 30, 20, _("S-mtr"));
		poll_smeter->tooltip(_("Poll every Nth interval"));
		poll_smeter->value(1);
		poll_smeter->callback((Fl_Callback*)cb_poll_smeter);
		poll_smeter->align(Fl_Align(FL_ALIGN_RIGHT));
		poll_smeter->value(progStatus.poll_smeter);

		poll_pout = new Fl_Check_Button(X + 100, Y + 25, 30, 20, _("Pwr out"));
		poll_pout->tooltip(_("Poll every Nth interval"));
		poll_pout->value(1);
		poll_pout->callback((Fl_Callback*)cb_poll_pout);
		poll_pout->align(Fl_Align(FL_ALIGN_RIGHT));
		poll_pout->value(progStatus.poll_pout);

		poll_swr = new Fl_Check_Button(X + 190, Y + 25, 30, 20, _("SWR"));
		poll_swr->tooltip(_("Poll every Nth interval"));
		poll_swr->value(1);
		poll_swr->callback((Fl_Callback*)cb_poll_swr);
		poll_swr->align(Fl_Align(FL_ALIGN_RIGHT));
		poll_swr->value(progStatus.poll_swr);

		poll_alc = new Fl_Check_Button(X + 280, Y + 25, 30, 20, _("ALC"));
		poll_alc->tooltip(_("Poll every Nth interval"));
		poll_alc->value(1);
		poll_alc->callback((Fl_Callback*)cb_poll_alc);
		poll_alc->align(Fl_Align(FL_ALIGN_RIGHT));
		poll_alc->value(progStatus.poll_alc);

		btnSetMeters = new Fl_Button(X + 370, Y + 25, 60, 20, _("Set all"));
		btnSetMeters->tooltip("Set all meter polls");
		btnSetMeters->callback((Fl_Callback*)cb_btnSetMeters);

		poll_meters = new Fl_Check_Button(X + 435, Y + 25, 30, 20);
		poll_meters->tooltip(_("Poll all"));
		poll_meters->value(progStatus.poll_meters);
		poll_meters->callback((Fl_Callback*)cb_poll_meters);
		poll_meters->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
		poll_meters->value(progStatus.poll_meters);

	xcvr_grp7->end();
	Fl_Group* xcvr_grp8 = new Fl_Group(X + 5, Y + 50, W - 10, 45, _("Operating Controls"));
		xcvr_grp8->box(FL_ENGRAVED_BOX);
		xcvr_grp8->align(Fl_Align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE));

		poll_frequency = new Fl_Check_Button(X + 10, Y + 70, 30, 20, _("Freq"));
		poll_frequency->tooltip(_("Poll xcvr frequency"));
		poll_frequency->value(1);
		poll_frequency->callback((Fl_Callback*)cb_poll_frequency);
		poll_frequency->align(Fl_Align(FL_ALIGN_RIGHT));
		poll_frequency->value(progStatus.poll_frequency);

		poll_mode = new Fl_Check_Button(X + 100, Y + 70, 30, 20, _("Mode"));
		poll_mode->tooltip(_("Poll xcvr mode"));
		poll_mode->value(1);
		poll_mode->callback((Fl_Callback*)cb_poll_mode);
		poll_mode->align(Fl_Align(FL_ALIGN_RIGHT));
		poll_mode->value(progStatus.poll_mode);

		poll_bandwidth = new Fl_Check_Button(X + 190, Y + 70, 30, 20, _("BW"));
		poll_bandwidth->tooltip(_("Poll xcvr bandwidth"));
		poll_bandwidth->value(1);
		poll_bandwidth->callback((Fl_Callback*)cb_poll_bandwidth);
		poll_bandwidth->align(Fl_Align(FL_ALIGN_RIGHT));
		poll_bandwidth->value(progStatus.poll_bandwidth);

		btnSetOps = new Fl_Button(X + 370, Y + 70, 60, 20, _("Set all"));
		btnSetOps->tooltip("Poll all operating values");
		btnSetOps->callback((Fl_Callback*)cb_btnSetOps);

		poll_ops = new Fl_Check_Button(X + 435, Y + 70, 30, 20);
		poll_ops->tooltip(_("Poll all"));
		poll_ops->value(progStatus.poll_ops);
		poll_ops->callback((Fl_Callback*)cb_poll_ops);
		poll_ops->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
		poll_ops->value(progStatus.poll_ops);

		xcvr_grp8->end();

	Fl_Group* xcvr_grp9 = new Fl_Group(X + 5, Y + 95, W - 10, 130, _("Additional Controls"));
		xcvr_grp9->box(FL_ENGRAVED_FRAME);
		xcvr_grp9->align(Fl_Align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE));

		poll_volume = new Fl_Check_Button(X + 10, Y + 115, 30, 20, _("Volume"));
		poll_volume->tooltip(_("Volume control"));
		poll_volume->callback((Fl_Callback*)cb_poll_volume);
		poll_volume->align(Fl_Align(FL_ALIGN_RIGHT));
		poll_volume->value(progStatus.poll_volume);

		poll_micgain = new Fl_Check_Button(X + 100, Y + 115, 30, 20, _("Mic"));
		poll_micgain->tooltip(_("Microphone gain"));
		poll_micgain->callback((Fl_Callback*)cb_poll_micgain);
		poll_micgain->align(Fl_Align(FL_ALIGN_RIGHT));
		poll_micgain->value(progStatus.poll_micgain);

		poll_rfgain = new Fl_Check_Button(X + 190, Y + 115, 30, 20, _("RF"));
		poll_rfgain->tooltip(_("RF gain"));
		poll_rfgain->callback((Fl_Callback*)cb_poll_rfgain);
		poll_rfgain->align(Fl_Align(FL_ALIGN_RIGHT));
		poll_rfgain->value(progStatus.poll_rfgain);

		poll_power_control = new Fl_Check_Button(X + 280, Y + 115, 30, 20, _("Power"));
		poll_power_control->tooltip(_("Power output"));
		poll_power_control->callback((Fl_Callback*)cb_poll_power_control);
		poll_power_control->align(Fl_Align(FL_ALIGN_RIGHT));
		poll_power_control->value(progStatus.poll_power_control);

		poll_ptt = new Fl_Check_Button(X + 370, Y + 115, 30, 20, _("PTT"));
		poll_ptt->tooltip(_("Push to talk"));
		poll_ptt->callback((Fl_Callback*)cb_poll_ptt);
		poll_ptt->align(Fl_Align(FL_ALIGN_RIGHT));
		poll_ptt->value(progStatus.poll_ptt);

		poll_ifshift = new Fl_Check_Button(X + 10, Y + 140, 30, 20, _("IF"));
		poll_ifshift->tooltip(_("IF shift"));
		poll_ifshift->callback((Fl_Callback*)cb_poll_ifshift);
		poll_ifshift->align(Fl_Align(FL_ALIGN_RIGHT));
		poll_ifshift->value(progStatus.poll_ifshift);

		poll_notch = new Fl_Check_Button(X + 100, Y + 140, 30, 20, _("Notch"));
		poll_notch->tooltip(_("Manual notch"));
		poll_notch->callback((Fl_Callback*)cb_poll_notch);
		poll_notch->align(Fl_Align(FL_ALIGN_RIGHT));
		poll_notch->value(progStatus.poll_notch);

		poll_auto_notch = new Fl_Check_Button(X + 190, Y + 140, 30, 20, _("Auto"));
		poll_auto_notch->tooltip(_("Auto notch"));
		poll_auto_notch->callback((Fl_Callback*)cb_poll_auto_notch);
		poll_auto_notch->align(Fl_Align(FL_ALIGN_RIGHT));
		poll_auto_notch->value(progStatus.poll_auto_notch);

		poll_tuner = new Fl_Check_Button(X + 280, Y + 140, 30, 20, _("Tuner"));
		poll_tuner->tooltip(_("Auto Tuner"));
		poll_tuner->callback((Fl_Callback*)cb_poll_tuner);
		poll_tuner->align(Fl_Align(FL_ALIGN_RIGHT));
		poll_tuner->value(progStatus.poll_tuner);

		poll_pre_att = new Fl_Check_Button(X + 10, Y + 165, 30, 20, _("Pre/Att"));
		poll_pre_att->tooltip(_("Preamp / Attenuator"));
		poll_pre_att->callback((Fl_Callback*)cb_poll_pre_att);
		poll_pre_att->align(Fl_Align(FL_ALIGN_RIGHT));
		poll_pre_att->value(progStatus.poll_pre_att);

		poll_squelch = new Fl_Check_Button(X + 100, Y + 165, 30, 20, _("Squelch"));
		poll_squelch->tooltip(_("Squelch"));
		poll_squelch->callback((Fl_Callback*)cb_poll_squelch);
		poll_squelch->align(Fl_Align(FL_ALIGN_RIGHT));
		poll_squelch->value(progStatus.poll_squelch);

		poll_split = new Fl_Check_Button(X + 190, Y + 165, 30, 20, _("Split"));
		poll_split->tooltip(_("Split vfo operation"));
		poll_split->callback((Fl_Callback*)cb_poll_split);
		poll_split->align(Fl_Align(FL_ALIGN_RIGHT));
		poll_split->value(progStatus.poll_split);

		poll_noise = new Fl_Check_Button(X + 10, Y + 190, 30, 20, _("Blanker"));
		poll_noise->tooltip(_("Noise blanker"));
		poll_noise->callback((Fl_Callback*)cb_poll_noise);
		poll_noise->align(Fl_Align(FL_ALIGN_RIGHT));
		poll_noise->value(progStatus.poll_noise);

		poll_nr = new Fl_Check_Button(X + 100, Y + 190, 30, 20, _("Noise red"));
		poll_nr->tooltip(_("Noise reduction"));
		poll_nr->callback((Fl_Callback*)cb_poll_nr);
		poll_nr->align(Fl_Align(FL_ALIGN_RIGHT));
		poll_nr->value(progStatus.poll_nr);

		poll_compression = new Fl_Check_Button(X + 190, Y + 190, 30, 20, _("Comp"));
		poll_compression->tooltip(_("Compression"));
		poll_compression->callback((Fl_Callback*)cb_poll_compression);
		poll_compression->align(Fl_Align(FL_ALIGN_RIGHT));
		poll_compression->value(progStatus.poll_compression);

		poll_break_in = new Fl_Check_Button(X + 280, Y + 165, 30, 20, _("QSK"));
		poll_break_in->tooltip(_("Break In"));
		poll_break_in->callback((Fl_Callback*)cb_poll_break_in);
		poll_break_in->align(Fl_Align(FL_ALIGN_RIGHT));
		poll_break_in->value(progStatus.poll_break_in);

		btnSetAdd = new Fl_Button(X + 370, Y + 190, 60, 20, _("Set all"));
		btnSetAdd->callback((Fl_Callback*)cb_btnSetAdd);

		poll_all = new Fl_Check_Button(X + 435, Y + 190, 30, 20);
		poll_all->tooltip(_("Poll all"));
		poll_all->value(1);
		poll_all->callback((Fl_Callback*)cb_poll_all);
		poll_all->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
		poll_all->value(progStatus.poll_all);

	xcvr_grp9->end();

	disable_xmlrpc = new Fl_Check_Button(X + 10, Y + 230, 18, 18, _("Disable XmlRpc Server"));
	disable_xmlrpc->tooltip(_("Use for testing ONLY!"));
	disable_xmlrpc->value(0);

	disable_polling = new Fl_Check_Button(X + 280, Y + 230, 18, 18, _("Disable Polling"));
	disable_polling->tooltip(_("Use for testing ONLY!"));
	disable_polling->value(0);

	tabPOLLING->end();

	return tabPOLLING;
}

Fl_Group *createSNDCMD(int X, int Y, int W, int H, const char *label)
{
	Fl_Group *tabSNDCMD = new Fl_Group(X, Y, W, H, label);
	tabSNDCMD->hide();

	txt_command = new Fl_Input2(X + 25, Y + 20, 435, 24,
_("Enter text as ASCII std::string\nOr sequence of hex values, x80 etc separated by spaces"));
	txt_command->box(FL_DOWN_BOX);
	txt_command->color(FL_BACKGROUND2_COLOR);
	txt_command->selection_color(FL_SELECTION_COLOR);
	txt_command->labeltype(FL_NORMAL_LABEL);
	txt_command->labelfont(0);
	txt_command->labelsize(14);
	txt_command->labelcolor(FL_FOREGROUND_COLOR);
	txt_command->align(Fl_Align(FL_ALIGN_BOTTOM_LEFT));
	txt_command->when(FL_WHEN_RELEASE);

	txt_response = new Fl_Output(X + 25, Y + 80, 435, 24, _("Response to the SEND button"));
	txt_response->align(Fl_Align(FL_ALIGN_BOTTOM_LEFT));

	btn_icom_pre = new Fl_Button(X + 380, Y + 115, 80, 20, _("ICOM pre"));
	btn_icom_pre->callback((Fl_Callback*)cb_btn_icom_pre);

	btn_icom_post = new Fl_Button(X + 380, Y + 145, 80, 20, _("ICOM post"));
	btn_icom_post->callback((Fl_Callback*)cb_btn_icom_post);

	btn_send_command = new Fl_Button(X + 380, Y + 180, 80, 20, _("SEND"));
	btn_send_command->callback((Fl_Callback*)cb_btn_send_command);

	tabSNDCMD->end();

	return tabSNDCMD;
}

Fl_Group *createCOMMANDS(int X, int Y, int W, int H, const char *label)
{
	Fl_Group *tabCOMMANDS = new Fl_Group(X, Y, W, H, label);

	Fl_Tabs *tabCmds = new Fl_Tabs(X, Y, W, H - 25);

	std::string *cmdlbls[] = {
		&progStatus.label1,  &progStatus.label2,  &progStatus.label3,  &progStatus.label4,
		&progStatus.label5,  &progStatus.label6,  &progStatus.label7,  &progStatus.label8,
		&progStatus.label9,  &progStatus.label10, &progStatus.label11, &progStatus.label12,
		&progStatus.label13, &progStatus.label14, &progStatus.label15, &progStatus.label16,
		&progStatus.label17, &progStatus.label18, &progStatus.label19, &progStatus.label20,
		&progStatus.label21, &progStatus.label22, &progStatus.label23, &progStatus.label24
		};

	Fl_Input2 *cmdbtns[] = {
		cmdlbl1,  cmdlbl2,  cmdlbl3,  cmdlbl4,
		cmdlbl5,  cmdlbl6,  cmdlbl7,  cmdlbl8,
		cmdlbl9,  cmdlbl10, cmdlbl11, cmdlbl12,
		cmdlbl13, cmdlbl14, cmdlbl15, cmdlbl16,
		cmdlbl17, cmdlbl18, cmdlbl19, cmdlbl20,
		cmdlbl21, cmdlbl22, cmdlbl23, cmdlbl24
		};
 
	Fl_Input2 *cmdtexts[] = {
		cmdtext1,  cmdtext2,  cmdtext3,  cmdtext4,
		cmdtext5,  cmdtext6,  cmdtext7,  cmdtext8,
		cmdtext9,  cmdtext10, cmdtext11, cmdtext12,
		cmdtext13, cmdtext14, cmdtext15, cmdtext16,
		cmdtext17, cmdtext18, cmdtext19, cmdtext20,
		cmdtext21, cmdtext22, cmdtext23, cmdtext24
		};
 
	Fl_Input2 *shftcmdtexts[] = {
		shftcmdtext1,  shftcmdtext2,  shftcmdtext3,  shftcmdtext4,
		shftcmdtext5,  shftcmdtext6,  shftcmdtext7,  shftcmdtext8,
		shftcmdtext9,  shftcmdtext10, shftcmdtext11, shftcmdtext12,
		shftcmdtext13, shftcmdtext14, shftcmdtext15, shftcmdtext16,
		shftcmdtext17, shftcmdtext18, shftcmdtext19, shftcmdtext20,
		shftcmdtext21, shftcmdtext22, shftcmdtext23, shftcmdtext24
		};
 
	std::string *cmd[] = { 
		&progStatus.command1,  &progStatus.command2,  &progStatus.command3,  &progStatus.command4,
		&progStatus.command5,  &progStatus.command6,  &progStatus.command7,  &progStatus.command8,
		&progStatus.command9,  &progStatus.command10, &progStatus.command11, &progStatus.command12,
		&progStatus.command13, &progStatus.command14, &progStatus.command15, &progStatus.command16,
		&progStatus.command17, &progStatus.command18, &progStatus.command19, &progStatus.command20,
		&progStatus.command21, &progStatus.command22, &progStatus.command23, &progStatus.command24
		};

	std::string *shftcmd[] = { 
		&progStatus.shftcmd1,  &progStatus.shftcmd2,  &progStatus.shftcmd3,  &progStatus.shftcmd4,
		&progStatus.shftcmd5,  &progStatus.shftcmd6,  &progStatus.shftcmd7,  &progStatus.shftcmd8,
		&progStatus.shftcmd9,  &progStatus.shftcmd10, &progStatus.shftcmd11, &progStatus.shftcmd12,
		&progStatus.shftcmd13, &progStatus.shftcmd14, &progStatus.shftcmd15, &progStatus.shftcmd16,
		&progStatus.shftcmd17, &progStatus.shftcmd18, &progStatus.shftcmd19, &progStatus.shftcmd20,
		&progStatus.shftcmd21, &progStatus.shftcmd22, &progStatus.shftcmd23, &progStatus.shftcmd24
		};

	Fl_Input2 *start_exit_label[] = {
		start_exit_label1,  start_exit_label2,  start_exit_label3,  start_exit_label4,
		start_exit_label5,  start_exit_label6,  start_exit_label7,  start_exit_label8
		};

	Fl_Input2 *start_exit_text[] = {
		start_exit_text1,  start_exit_text2,  start_exit_text3,  start_exit_text4,
		start_exit_text5,  start_exit_text6,  start_exit_text7,  start_exit_text8,
		};

	std::string *se_label[] = {
		&progStatus.label_on_start1,  &progStatus.label_on_start2,
		&progStatus.label_on_start3,  &progStatus.label_on_start4,
		&progStatus.label_on_exit1,   &progStatus.label_on_exit2,
		&progStatus.label_on_exit3,   &progStatus.label_on_exit4
	};

	std::string *se_cmd[] = {
		&progStatus.cmd_on_start1,  &progStatus.cmd_on_start2,
		&progStatus.cmd_on_start3,  &progStatus.cmd_on_start4,
		&progStatus.cmd_on_exit1,   &progStatus.cmd_on_exit2,
		&progStatus.cmd_on_exit3,   &progStatus.cmd_on_exit4
	};

	static const char *lbl[24] = {
		"1", "2", "3", "4", "5", "6", "7", "8",
		"9", "10", "11", "12", "13", "14", "15", "16",
		"17", "18", "19", "20", "21", "22", "23", "24"
		};

	tabCmds1 = new Fl_Group(X, Y + 20, W, H - 50, _("1-4"));

		bx1a = new Fl_Box(X + 23, Y + 26, 75, 16, "Label");
		bx1a->box(FL_FLAT_BOX);
		bx1a->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);

		bx1b = new Fl_Box(X + 80, Y + 26, W - 80 - 2, 16, "Command");
		bx1b->box(FL_FLAT_BOX);
		bx1b->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);

		for (int n = 0; n < 4; n++) {
			cmdbtns[n] = new Fl_Input2(X + 27, Y + 46 + (n % 4) * 40, 80, 20, lbl[n]);
			cmdbtns[n]->align(FL_ALIGN_LEFT);
			cmdbtns[n]->value(cmdlbls[n]->c_str());
			cmdbtns[n]->callback((Fl_Callback*)cb_cmdlbl, reinterpret_cast<void *>(n));

			cmdtexts[n] = new Fl_Input2(X + 110, Y + 46 + (n % 4) * 40, W - 110 - 2, 20, "");
			cmdtexts[n]->align(FL_ALIGN_LEFT);
			cmdtexts[n]->value(cmd[n]->c_str());
			cmdtexts[n]->callback((Fl_Callback*)cb_cmdtext, reinterpret_cast<void *>(n));

			shftcmdtexts[n] = new Fl_Input2(X + 110, Y + 66 + (n % 4) * 40, W - 110 - 2, 20, "SHIFT");
			shftcmdtexts[n]->align(FL_ALIGN_LEFT);
			shftcmdtexts[n]->value(shftcmd[n]->c_str());
			shftcmdtexts[n]->callback((Fl_Callback*)cb_shftcmdtext, reinterpret_cast<void *>(n));
		}

	tabCmds1->end();

	tabCmds2 = new Fl_Group(X, Y + 20, W, H - 50, _("5-8"));

		bx2a = new Fl_Box(X + 23, Y + 26, 75, 16, "Label");
		bx2a->box(FL_FLAT_BOX);
		bx2a->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);

		bx2b = new Fl_Box(X + 80, Y + 26, W - 80 - 2, 16, "Command");
		bx2b->box(FL_FLAT_BOX);
		bx2b->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);

		for (int n = 4; n < 8; n++) {
			cmdbtns[n] = new Fl_Input2(X + 27, Y + 46 + (n % 4) * 40, 80, 20, lbl[n]);
			cmdbtns[n]->align(FL_ALIGN_LEFT);
			cmdbtns[n]->value(cmdlbls[n]->c_str());
			cmdbtns[n]->callback((Fl_Callback*)cb_cmdlbl, reinterpret_cast<void *>(n));

			cmdtexts[n] = new Fl_Input2(X + 110, Y + 46 + (n % 4) * 40, W - 110 - 2, 20, "");
			cmdtexts[n]->align(FL_ALIGN_LEFT);
			cmdtexts[n]->value(cmd[n]->c_str());
			cmdtexts[n]->callback((Fl_Callback*)cb_cmdtext, reinterpret_cast<void *>(n));

			shftcmdtexts[n] = new Fl_Input2(X + 110, Y + 66 + (n % 4) * 40, W - 110 - 2, 20, "SHIFT");
			shftcmdtexts[n]->align(FL_ALIGN_LEFT);
			shftcmdtexts[n]->value(shftcmd[n]->c_str());
			shftcmdtexts[n]->callback((Fl_Callback*)cb_shftcmdtext, reinterpret_cast<void *>(n));
		}

	tabCmds2->end();

	tabCmds3 = new Fl_Group(X, Y + 20, W, H - 50, _("9-12"));
		tabCmds2->hide();

		bx3a = new Fl_Box(X + 23, Y + 26, 75, 16, "Label");
		bx3a->box(FL_FLAT_BOX);
		bx3a->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);

		bx3b = new Fl_Box(X + 80, Y + 26, W - 80 - 2, 16, "Command");
		bx3b->box(FL_FLAT_BOX);
		bx3b->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);

		for (int n = 8; n < 12; n++) {
			cmdbtns[n] = new Fl_Input2(X + 27, Y + 46 + (n % 4) * 40, 80, 20, lbl[n]);
			cmdbtns[n]->align(FL_ALIGN_LEFT);
			cmdbtns[n]->value(cmdlbls[n]->c_str());
			cmdbtns[n]->callback((Fl_Callback*)cb_cmdlbl, reinterpret_cast<void *>(n));

			cmdtexts[n] = new Fl_Input2(X + 110, Y + 46 + (n % 4) * 40, W - 110 - 2, 20, "");
			cmdtexts[n]->align(FL_ALIGN_LEFT);
			cmdtexts[n]->value(cmd[n]->c_str());
			cmdtexts[n]->callback((Fl_Callback*)cb_cmdtext, reinterpret_cast<void *>(n));

			shftcmdtexts[n] = new Fl_Input2(X + 110, Y + 66 + (n % 4) * 40, W - 110 - 2, 20, "SHIFT");
			shftcmdtexts[n]->align(FL_ALIGN_LEFT);
			shftcmdtexts[n]->value(shftcmd[n]->c_str());
			shftcmdtexts[n]->callback((Fl_Callback*)cb_shftcmdtext, reinterpret_cast<void *>(n));
		}

	tabCmds3->end();

	tabCmds4 = new Fl_Group(X, Y + 20, W, H - 50, _("13-16"));

		bx4a = new Fl_Box(X + 23, Y + 26, 75, 16, "Label");
		bx4a->box(FL_FLAT_BOX);
		bx4a->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);

		bx4b = new Fl_Box(X + 80, Y + 26, W - 80 - 2, 16, "Command");
		bx4b->box(FL_FLAT_BOX);
		bx4b->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);

		for (int n = 12; n < 16; n++) {
			cmdbtns[n] = new Fl_Input2(X + 27, Y + 46 + (n % 4) * 40, 80, 20, lbl[n]);
			cmdbtns[n]->align(FL_ALIGN_LEFT);
			cmdbtns[n]->value(cmdlbls[n]->c_str());
			cmdbtns[n]->callback((Fl_Callback*)cb_cmdlbl, reinterpret_cast<void *>(n));

			cmdtexts[n] = new Fl_Input2(X + 110, Y + 46 + (n % 4) * 40, W - 110 - 2, 20, "");
			cmdtexts[n]->align(FL_ALIGN_LEFT);
			cmdtexts[n]->value(cmd[n]->c_str());
			cmdtexts[n]->callback((Fl_Callback*)cb_cmdtext, reinterpret_cast<void *>(n));

			shftcmdtexts[n] = new Fl_Input2(X + 110, Y + 66 + (n % 4) * 40, W - 110 - 2, 20, "SHIFT");
			shftcmdtexts[n]->align(FL_ALIGN_LEFT);
			shftcmdtexts[n]->value(shftcmd[n]->c_str());
			shftcmdtexts[n]->callback((Fl_Callback*)cb_shftcmdtext, reinterpret_cast<void *>(n));
		}

	tabCmds4->end();

	tabCmds5 = new Fl_Group(X, Y + 20, W, H - 50, _("17-20"));

		bx5a = new Fl_Box(X + 23, Y + 26, 75, 16, "Label");
		bx5a->box(FL_FLAT_BOX);
		bx5a->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);

		bx5b = new Fl_Box(X + 80, Y + 26, W - 80 - 2, 16, "Command");
		bx5b->box(FL_FLAT_BOX);
		bx5b->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);

		for (int n = 16; n < 20; n++) {
			cmdbtns[n] = new Fl_Input2(X + 27, Y + 46 + (n % 4) * 40, 80, 20, lbl[n]);
			cmdbtns[n]->align(FL_ALIGN_LEFT);
			cmdbtns[n]->value(cmdlbls[n]->c_str());
			cmdbtns[n]->callback((Fl_Callback*)cb_cmdlbl, reinterpret_cast<void *>(n));

			cmdtexts[n] = new Fl_Input2(X + 110, Y + 46 + (n % 4) * 40, W - 110 - 2, 20, "");
			cmdtexts[n]->align(FL_ALIGN_LEFT);
			cmdtexts[n]->value(cmd[n]->c_str());
			cmdtexts[n]->callback((Fl_Callback*)cb_cmdtext, reinterpret_cast<void *>(n));

			shftcmdtexts[n] = new Fl_Input2(X + 110, Y + 66 + (n % 4) * 40, W - 110 - 2, 20, "SHIFT");
			shftcmdtexts[n]->align(FL_ALIGN_LEFT);
			shftcmdtexts[n]->value(shftcmd[n]->c_str());
			shftcmdtexts[n]->callback((Fl_Callback*)cb_shftcmdtext, reinterpret_cast<void *>(n));
		}

	tabCmds5->end();

	tabCmds6 = new Fl_Group(X, Y + 20, W, H - 50, _("21-24"));

		bx6a = new Fl_Box(X + 23, Y + 26, 75, 16, "Label");
		bx6a->box(FL_FLAT_BOX);
		bx6a->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);

		bx6b = new Fl_Box(X + 80, Y + 26, W - 80 - 2, 16, "Command");
		bx6b->box(FL_FLAT_BOX);
		bx6b->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);

		for (int n = 20; n < 24; n++) {
			cmdbtns[n] = new Fl_Input2(X + 27, Y + 46 + (n % 4) * 40, 80, 20, lbl[n]);
			cmdbtns[n]->align(FL_ALIGN_LEFT);
			cmdbtns[n]->value(cmdlbls[n]->c_str());
			cmdbtns[n]->callback((Fl_Callback*)cb_cmdlbl, reinterpret_cast<void *>(n));

			cmdtexts[n] = new Fl_Input2(X + 110, Y + 46 + (n % 4) * 40, W - 110 - 2, 20, "");
			cmdtexts[n]->align(FL_ALIGN_LEFT);
			cmdtexts[n]->value(cmd[n]->c_str());
			cmdtexts[n]->callback((Fl_Callback*)cb_cmdtext, reinterpret_cast<void *>(n));

			shftcmdtexts[n] = new Fl_Input2(X + 110, Y + 66 + (n % 4) * 40, W - 110 - 2, 20, "SHIFT");
			shftcmdtexts[n]->align(FL_ALIGN_LEFT);
			shftcmdtexts[n]->value(shftcmd[n]->c_str());
			shftcmdtexts[n]->callback((Fl_Callback*)cb_shftcmdtext, reinterpret_cast<void *>(n));
		}

	tabCmds6->end();

	tabCmds_on_start_exit = new Fl_Group(X, Y + 20, W, H - 50, _("Start/Exit"));

		Fl_Box *start_labels = new Fl_Box(X + 23, Y + 26, 75, 16, "Label");
		start_labels->box(FL_FLAT_BOX);
		start_labels->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);

		Fl_Box *start_commands = new Fl_Box(X + 80, Y + 26, W - 80 - 2, 16, "Command");
		start_commands->box(FL_FLAT_BOX);
		start_commands->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);

		for (int n = 0; n < 8; n++) {
			start_exit_label[n] = new Fl_Input2(
				X + 27, Y + 46 + n * 20, 
				80, 20,
				n < 4 ? "St" : "Ex");
			start_exit_label[n]->align(FL_ALIGN_LEFT);
			start_exit_label[n]->value(se_label[n]->c_str());
			start_exit_label[n]->callback((Fl_Callback*)cb_se_label, reinterpret_cast<void *>(n));

			start_exit_text[n] = new Fl_Input2(
					X + 110, start_exit_label[n]->y(),
					W - 110 - 2, 20, "");
			start_exit_text[n]->align(FL_ALIGN_LEFT);
			start_exit_text[n]->value(se_cmd[n]->c_str());
			start_exit_text[n]->callback((Fl_Callback*)cb_se_text, reinterpret_cast<void *>(n));
		}

	tabCmds_on_start_exit->end();

	tabCmds->end();

	cmdResponse = new Fl_Output(cmdtexts[0]->x(), Y + H - 20, cmdtexts[0]->w(), 20, "Response:");
	cmdResponse->align(FL_ALIGN_LEFT);

	tabCOMMANDS->end();

	return tabCOMMANDS;
}

Fl_Group *createRestore(int X, int Y, int W, int H, const char *label)
{
	Fl_Group *tabRESTORE = new Fl_Group(X, Y, W, H, label);

	tabRESTORE->hide();

	Fl_Box *restore_box = new Fl_Box(X + 8, Y + 5, 455, 30, 
		_("Read / Restore these parameters"));
	restore_box->box(FL_ENGRAVED_FRAME);

	btnRestoreFrequency = new Fl_Check_Button(X + 8, Y + 40, 20, 20, _("Freq"));
	btnRestoreFrequency->tooltip(_("Restore frequency"));
	btnRestoreFrequency->callback((Fl_Callback*)cb_restore);
	btnRestoreFrequency->align(Fl_Align(FL_ALIGN_RIGHT));
	btnRestoreFrequency->value(progStatus.restore_frequency == 1);

	btnRestoreMode = new Fl_Check_Button(X + 8, Y + 65, 20, 20, _("Mode"));
	btnRestoreMode->tooltip(_("Restore Mode"));
	btnRestoreMode->callback((Fl_Callback*)cb_restore);
	btnRestoreMode->align(Fl_Align(FL_ALIGN_RIGHT));
	btnRestoreMode->value(progStatus.restore_mode == 1);

	btnRestoreBandwidth = new Fl_Check_Button(X + 8, Y + 90, 20, 20, _("Bandwidth"));
	btnRestoreBandwidth->tooltip(_("Restore bandwidth"));
	btnRestoreBandwidth->callback((Fl_Callback*)cb_restore);
	btnRestoreBandwidth->align(Fl_Align(FL_ALIGN_RIGHT));
	btnRestoreBandwidth->value(progStatus.restore_bandwidth == 1);

	btnRestoreVolume = new Fl_Check_Button(X + 8, Y + 115, 20, 20, _("Volume"));
	btnRestoreVolume->tooltip(_("Restore volume control"));
	btnRestoreVolume->callback((Fl_Callback*)cb_restore);
	btnRestoreVolume->align(Fl_Align(FL_ALIGN_RIGHT));
	btnRestoreVolume->value(progStatus.restore_volume == 1);

	btnRestoreMicGain = new Fl_Check_Button(X + 8, Y + 140, 20, 20, _("Mic gain"));
	btnRestoreMicGain->tooltip(_("Restore mic gain"));
	btnRestoreMicGain->callback((Fl_Callback*)cb_restore);
	btnRestoreMicGain->align(Fl_Align(FL_ALIGN_RIGHT));
	btnRestoreMicGain->value(progStatus.restore_mic_gain == 1);

	btnRestoreRfGain = new Fl_Check_Button(X + 8, Y + 165, 20, 20, _("RF gain"));
	btnRestoreRfGain->tooltip(_("Restore RF gain"));
	btnRestoreRfGain->callback((Fl_Callback*)cb_restore);
	btnRestoreRfGain->align(Fl_Align(FL_ALIGN_RIGHT));
	btnRestoreRfGain->value(progStatus.restore_rf_gain == 1);

	btnRestorePowerControl = new Fl_Check_Button(X + 8, Y + 190, 20, 20, _("Pwr level"));
	btnRestorePowerControl->tooltip(_("Restore power control"));
	btnRestorePowerControl->callback((Fl_Callback*)cb_restore);
	btnRestorePowerControl->align(Fl_Align(FL_ALIGN_RIGHT));
	btnRestorePowerControl->value(progStatus.restore_power_control == 1);

	btnRestoreIFshift = new Fl_Check_Button(X + 128, Y + 40, 20, 20, _("IFshift"));
	btnRestoreIFshift->tooltip(_("Restore IF shift control"));
	btnRestoreIFshift->callback((Fl_Callback*)cb_restore);
	btnRestoreIFshift->align(Fl_Align(FL_ALIGN_RIGHT));
	btnRestoreIFshift->value(progStatus.restore_if_shift == 1);

	btnRestoreNotch = new Fl_Check_Button(X + 128, Y + 65, 20, 20, _("Notch"));
	btnRestoreNotch->tooltip(_("Restore notch control"));
	btnRestoreNotch->callback((Fl_Callback*)cb_restore);
	btnRestoreNotch->align(Fl_Align(FL_ALIGN_RIGHT));
	btnRestoreNotch->value(progStatus.restore_notch == 1);

	btnRestoreAutoNotch = new Fl_Check_Button(X + 128, Y + 90, 20, 20, _("Auto Ntch"));
	btnRestoreAutoNotch->tooltip(_("Restore auto notch setting"));
	btnRestoreAutoNotch->callback((Fl_Callback*)cb_restore);
	btnRestoreAutoNotch->align(Fl_Align(FL_ALIGN_RIGHT));
	btnRestoreAutoNotch->value(progStatus.restore_auto_notch == 1);

	btnRestoreSquelch = new Fl_Check_Button(X + 128, Y + 115, 20, 20, _("Squelch"));
	btnRestoreSquelch->tooltip(_("Restore squelch"));
	btnRestoreSquelch->callback((Fl_Callback*)cb_restore);
	btnRestoreSquelch->align(Fl_Align(FL_ALIGN_RIGHT));
	btnRestoreSquelch->value(progStatus.restore_squelch == 1);

	btnRestoreSplit = new Fl_Check_Button(X + 128, Y + 140, 20, 20, _("Split"));
	btnRestoreSplit->tooltip(_("Restore split"));
	btnRestoreSplit->callback((Fl_Callback*)cb_restore);
	btnRestoreSplit->align(Fl_Align(FL_ALIGN_RIGHT));
	btnRestoreSplit->value(progStatus.restore_split == 1);

	btnRestorePreAtt = new Fl_Check_Button(X + 128, Y + 165, 20, 20, _("Pre/Att"));
	btnRestorePreAtt->tooltip(_("Restore Pre/Att"));
	btnRestorePreAtt->callback((Fl_Callback*)cb_restore);
	btnRestorePreAtt->align(Fl_Align(FL_ALIGN_RIGHT));
	btnRestorePreAtt->value(progStatus.restore_pre_att == 1);

	btnRestoreNoise = new Fl_Check_Button(X + 128, Y + 190, 20, 20, _("Blanker"));
	btnRestoreNoise->tooltip(_("Restore noise blanker control"));
	btnRestoreNoise->callback((Fl_Callback*)cb_restore);
	btnRestoreNoise->align(Fl_Align(FL_ALIGN_RIGHT));
	btnRestoreNoise->value(progStatus.restore_noise == 1);

	btnRestoreNR = new Fl_Check_Button(X + 248, Y + 40, 20, 20, _("Noise Red'"));
	btnRestoreNR->tooltip(_("Restore noise reduction"));
	btnRestoreNR->callback((Fl_Callback*)cb_restore);
	btnRestoreNR->align(Fl_Align(FL_ALIGN_RIGHT));
	btnRestoreNR->value(progStatus.restore_nr == 1);

	btnRestoreCompOnOff = new Fl_Check_Button(X + 248, Y + 65, 20, 20, _("Comp On/Off"));
	btnRestoreCompOnOff->tooltip(_("Restore Comp On/Off"));
	btnRestoreCompOnOff->callback((Fl_Callback*)cb_restore);
	btnRestoreCompOnOff->align(Fl_Align(FL_ALIGN_RIGHT));
	btnRestoreCompOnOff->value(progStatus.restore_comp_on_off == 1);

	btnRestoreCompLevel = new Fl_Check_Button(X + 248, Y + 90, 20, 20, _("Comp Level"));
	btnRestoreCompLevel->tooltip(_("Restore comp level"));
	btnRestoreCompLevel->callback((Fl_Callback*)cb_restore);
	btnRestoreCompLevel->align(Fl_Align(FL_ALIGN_RIGHT));
	btnRestoreCompLevel->value(progStatus.restore_comp_level == 1);

	btnUseRigData = new Fl_Check_Button(X + 248, Y + 190, 20, 20, _("Use xcvr data"));
	btnUseRigData->tooltip(_("Set flrig to xcvr values at startup"));
	btnUseRigData->callback((Fl_Callback*)cb_restore);
	btnUseRigData->align(Fl_Align(FL_ALIGN_RIGHT));
	btnUseRigData->value(progStatus.use_rig_data == 1);

	tabRESTORE->end();

	init_hids();
	tmate2_init_hids();

	return tabRESTORE;
}

//----------------------------------------------------------------------

Fl_Group *tabCMEDIA=(Fl_Group *)0;

	Fl_Round_Button *btn_use_cmedia_PTT=(Fl_Round_Button *)0;

	Fl_ComboBox *inp_cmedia_dev=(Fl_ComboBox *)0;
	Fl_ComboBox *inp_cmedia_GPIO_line=(Fl_ComboBox *)0;
	Fl_Button *btn_refresh_cmedia=(Fl_Button *)0;
	Fl_Button *btn_init_cmedia_PTT=(Fl_Button *)0;
	Fl_Button *btn_test_cmedia=(Fl_Button *)0;

static void cb_btn_use_cmedia(Fl_Round_Button* o, void*) {
	if (o->value()) {
		progStatus.cmedia_ptt = PTT_BOTH;

		btn_init_cmedia_PTT->labelcolor(FL_RED);
		btn_init_cmedia_PTT->redraw();

		lbox_catptt->index(PTT_NONE);
		lbox_rtsptt->index(PTT_NONE);
		lbox_dtrptt->index(PTT_NONE);

		lbox_sep_rtsptt->index(PTT_NONE);
		lbox_sep_dtrptt->index(PTT_NONE);

		progStatus.comm_catptt = PTT_NONE;
		progStatus.comm_rtsptt = PTT_NONE;
		progStatus.comm_dtrptt = PTT_NONE;

		progStatus.sep_dtrptt = PTT_NONE;
		progStatus.sep_rtsptt = PTT_NONE;

	} else {
		progStatus.cmedia_ptt = PTT_NONE;
		close_cmedia();
	}
}

static void cb_inp_cmedia_dev(Fl_ComboBox* o, void*) {
	close_cmedia();
	progStatus.cmedia_device = o->value();
	btn_init_cmedia_PTT->labelcolor(FL_RED);
	btn_init_cmedia_PTT->redraw();
}

static void cb_inp_cmedia_GPIO_line(Fl_ComboBox* o, void*) {
  progStatus.cmedia_gpio_line = o->value();
}

static void cb_btn_init_cmedia_PTT(Fl_Button* o, void*) {
	open_cmedia(progStatus.cmedia_device);
	o->labelcolor(FL_FOREGROUND_COLOR);
}

static void cb_btn_refresh_cmedia(Fl_Button* o, void*) {
	init_hids();
}

static void cb_btn_test_cmedia(Fl_Button*, void*) {
  test_hid_ptt();
}

Fl_Group *createCmediaTab(int X, int Y, int W, int H, const char *label)
{
	Fl_Group *tab = new Fl_Group(X, Y, W, H, label);

	tab->hide();

	Fl_Box *bx  = new Fl_Box(X + 2, Y + 2, W - 4, H / 2 - 2, _(
"C-Media audio codecs used in DRA Series have 8 user controllable GPIO pins. \
GPIO signal line 3 (pin 13) is used for PTT control.\n\
On Linux: add a file named cmedia.rules to /etc/udev/rules.d/ containing:\n\
\n\
KERNEL==\"hidraw*\", \\\n\
        SUBSYSTEM==\"hidraw\", MODE=\"0664\", GROUP=\"plugdev\""));
		bx->box(FL_FLAT_BOX);
		bx->align(Fl_Align(132|FL_ALIGN_INSIDE));
		bx->labelfont(0);
		bx->labelsize(13);

	btn_use_cmedia_PTT = new Fl_Round_Button(X + 10, Y + H/2 + 5, 100, 20, _("Use C-Media PTT"));
		btn_use_cmedia_PTT->down_box(FL_DOWN_BOX);
		btn_use_cmedia_PTT->selection_color((Fl_Color)1);
		btn_use_cmedia_PTT->value(progStatus.cmedia_ptt);
		btn_use_cmedia_PTT->callback((Fl_Callback*)cb_btn_use_cmedia);

	inp_cmedia_dev = new Fl_ComboBox(X + 10, Y + H/2 + 50, 350, 20, _("C-Media device"));
		inp_cmedia_dev->box(FL_DOWN_BOX);
		inp_cmedia_dev->color(FL_BACKGROUND2_COLOR);
		inp_cmedia_dev->selection_color(FL_BACKGROUND_COLOR);
		inp_cmedia_dev->labeltype(FL_NORMAL_LABEL);
		inp_cmedia_dev->labelfont(0);
		inp_cmedia_dev->labelsize(13);
		inp_cmedia_dev->labelcolor(FL_FOREGROUND_COLOR);
		inp_cmedia_dev->callback((Fl_Callback*)cb_inp_cmedia_dev);
		inp_cmedia_dev->align(Fl_Align(FL_ALIGN_TOP_LEFT));
		inp_cmedia_dev->when(FL_WHEN_RELEASE);
		inp_cmedia_dev->labelsize(FL_NORMAL_SIZE);
		inp_cmedia_dev->value(progStatus.cmedia_device.c_str());
		inp_cmedia_dev->end();

	inp_cmedia_GPIO_line = new Fl_ComboBox(X + 10, Y + H/2 + 95, 350, 20, _("GPIO line"));
		inp_cmedia_GPIO_line->box(FL_DOWN_BOX);
		inp_cmedia_GPIO_line->color(FL_BACKGROUND2_COLOR);
		inp_cmedia_GPIO_line->selection_color(FL_BACKGROUND_COLOR);
		inp_cmedia_GPIO_line->labeltype(FL_NORMAL_LABEL);
		inp_cmedia_GPIO_line->labelfont(0);
		inp_cmedia_GPIO_line->labelsize(13);
		inp_cmedia_GPIO_line->labelcolor(FL_FOREGROUND_COLOR);
		inp_cmedia_GPIO_line->callback((Fl_Callback*)cb_inp_cmedia_GPIO_line);
		inp_cmedia_GPIO_line->align(Fl_Align(FL_ALIGN_TOP_LEFT));
		inp_cmedia_GPIO_line->when(FL_WHEN_RELEASE);
		inp_cmedia_GPIO_line->labelsize(FL_NORMAL_SIZE);
		inp_cmedia_GPIO_line->value(progStatus.cmedia_gpio_line.c_str());
		inp_cmedia_GPIO_line->add("GPIO-1|GPIO-2|GPIO-3|GPIO-4");
		inp_cmedia_GPIO_line->end();

	btn_refresh_cmedia = new Fl_Button(X + W - 80, Y + H/2 + 5, 70, 20, _("Refresh"));
		btn_refresh_cmedia->tooltip(_("Refresh the list of Cmedia devices"));
		btn_refresh_cmedia->callback((Fl_Callback*)cb_btn_refresh_cmedia);

	btn_init_cmedia_PTT = new Fl_Button(X + W - 80, Y + H/2 + 50, 70, 20, _("Select"));
		btn_init_cmedia_PTT->tooltip(_("Select device & Initialize the H/W PTT interface"));
		btn_init_cmedia_PTT->callback((Fl_Callback*)cb_btn_init_cmedia_PTT);

	btn_test_cmedia = new Fl_Button(X + W - 80, Y + H/2 + 95, 70, 20, _("TEST"));
		btn_test_cmedia->tooltip(_("Toggles PTT line 20x; check DRA ptt LED"));
		btn_test_cmedia->callback((Fl_Callback*)cb_btn_test_cmedia);

	tab->end();

	return tab;
}

//----------------------------------------------------------------------

Fl_Group *tabTMATE2 = (Fl_Group *)0;
	Fl_ComboBox *cbo_tmate2_dev = (Fl_ComboBox *)0;
	Fl_ComboBox *cbo_tmate2_freq_step = (Fl_ComboBox *)0;
	Fl_Button *btn_refresh_tmate2 = (Fl_Button *)0;
	Fl_Button *btn_init_tmate2 = (Fl_Button *)0;

static void cb_cbo_tmate2_dev(Fl_ComboBox* o, void*) {
	tmate2_close();
	progStatus.tmate2_device = o->value();
	btn_init_tmate2->labelcolor(FL_RED);
	btn_init_tmate2->redraw();
}

static void cb_cbo_tmate2_freq_step(Fl_ComboBox* o, void*) {
  progStatus.tmate2_freq_step = o->value();
}

static void cb_btn_refresh_tmate2(Fl_Button* o, void*) {
	tmate2_init_hids();
}

static void cb_btn_init_tmate2(Fl_Button* o, void*) {
	tmate2_open();
	o->labelcolor(FL_FOREGROUND_COLOR);
}

Fl_Group *createTMATE2Tab(int X, int Y, int W, int H, const char *label)
{
	Fl_Group *tab = new Fl_Group(X, Y, W, H, label);

	tab->hide();

	Fl_Box *bx  = new Fl_Box(X + 2, Y + 2, W - 4, H / 2 - 2, _(
"TMATE-2 interface units use hid i/o.\n\
Gn Linux: add a file named tmate2.rules to /etc/udev/rules.d/ containing:\n\
\n\
KERNEL==\"hidraw*\", \\\n\
SUBSYSTEM==\"hidraw\", MODE=\"0664\", GROUP=\"plugdev\""));
		bx->box(FL_FLAT_BOX);
		bx->align(Fl_Align(132|FL_ALIGN_INSIDE));
		bx->labelfont(0);
		bx->labelsize(13);

	cbo_tmate2_dev = new Fl_ComboBox(X + 10, Y + H/2 + 50, 350, 20, _("Elad TMATE-2"));
		cbo_tmate2_dev->box(FL_DOWN_BOX);
		cbo_tmate2_dev->color(FL_BACKGROUND2_COLOR);
		cbo_tmate2_dev->selection_color(FL_BACKGROUND_COLOR);
		cbo_tmate2_dev->labeltype(FL_NORMAL_LABEL);
		cbo_tmate2_dev->labelfont(0);
		cbo_tmate2_dev->labelsize(13);
		cbo_tmate2_dev->labelcolor(FL_FOREGROUND_COLOR);
		cbo_tmate2_dev->callback((Fl_Callback*)cb_cbo_tmate2_dev);
		cbo_tmate2_dev->align(Fl_Align(FL_ALIGN_TOP_LEFT));
		cbo_tmate2_dev->when(FL_WHEN_RELEASE);
		cbo_tmate2_dev->labelsize(FL_NORMAL_SIZE);
		cbo_tmate2_dev->value(progStatus.tmate2_device.c_str());
		cbo_tmate2_dev->end();

	cbo_tmate2_freq_step = new Fl_ComboBox(X + 10, Y + H/2 + 95, 350, 20, _("Transceiver frequency tune step in Hz"));
		cbo_tmate2_freq_step->box(FL_DOWN_BOX);
		cbo_tmate2_freq_step->color(FL_BACKGROUND2_COLOR);
		cbo_tmate2_freq_step->selection_color(FL_BACKGROUND_COLOR);
		cbo_tmate2_freq_step->labeltype(FL_NORMAL_LABEL);
		cbo_tmate2_freq_step->labelfont(0);
		cbo_tmate2_freq_step->labelsize(13);
		cbo_tmate2_freq_step->labelcolor(FL_FOREGROUND_COLOR);
		cbo_tmate2_freq_step->callback((Fl_Callback*)cb_cbo_tmate2_freq_step);
		cbo_tmate2_freq_step->align(Fl_Align(FL_ALIGN_TOP_LEFT));
		cbo_tmate2_freq_step->when(FL_WHEN_RELEASE);
		cbo_tmate2_freq_step->labelsize(FL_NORMAL_SIZE);
		cbo_tmate2_freq_step->add("1|100|500|1000|2500|5000|6250|10000|12500|15000|20000|25000|30000|50000|100000|125000|200000");
		cbo_tmate2_freq_step->value(progStatus.tmate2_freq_step.c_str());
		cbo_tmate2_freq_step->end();

	btn_refresh_tmate2 = new Fl_Button(X + W - 80, Y + H/2 + 5, 70, 20, _("Refresh"));
		btn_refresh_tmate2->tooltip(_("Refresh the list of hid devices"));
		btn_refresh_tmate2->callback((Fl_Callback*)cb_btn_refresh_tmate2);

	btn_init_tmate2 = new Fl_Button(X + W - 80, Y + H/2 + 50, 70, 20, _("Select"));
		btn_init_tmate2->tooltip(_("Select device & Initialize the interface"));
		btn_init_tmate2->callback((Fl_Callback*)cb_btn_init_tmate2);

	tab->end();

	return tab;
}

//----------------------------------------------------------------------

#include "gpio.cxx"

#include <vector>

std::vector< Fl_Group* > config_pages; 
static Fl_Group *current = 0; 
Fl_Tree *tab_tree;

void add_tree_item(Fl_Group *g) {
  config_pages.push_back(g);
  tab_tree->add(g->label());
  g->hide();
}

void cleartabs()
{
	tabXCVR->hide();
	tabTRACE->hide();
	tabTCPIP->hide();
	tabPTTGEN->hide();
	tabAUX->hide();
	tabGPIO->hide();
	tabCMEDIA->hide();
	tabPOLLING->hide();
	tabSNDCMD->hide();
	tabCOMMANDS->hide();
	tabRESTORE->hide();
	Fl_Tree_Item *item = tab_tree->last_selected_item();
	if (item) tab_tree->deselect(item,0);
}

void select_tab(const char *label)
{
	cleartabs();
	tab_tree->select(label);
	dlgXcvrConfig->show();
}

void SelectItem_CB(Fl_Widget *w) {
	cleartabs();
	Fl_Tree *tree = (Fl_Tree*)w;
	Fl_Tree_Item *item = tree->callback_item();
	tree->select_only(item, 0);

	if (tree->callback_reason() == FL_TREE_REASON_SELECTED) {
		std::string pname;
		char pn[200];
		tree->item_pathname(pn, 200, item);
		pname = pn;
		size_t pos = std::string::npos;
		for (size_t i = 0; i < config_pages.size(); i++) {
			if ((pos = pname.find(config_pages[i]->label())) != std::string::npos) {
				if (pname.substr(pos) == config_pages[i]->label()) {
					if (current) current->hide();
					current = config_pages[i];//->grp;
					current->show();
					return;
				}
			}
		}
	}
}

extern Fl_Double_Window *dlgXcvrConfig;

Fl_Double_Window* XcvrDialog() {

	int W = 675, H = 260;

	Fl_Double_Window* w = new Fl_Double_Window(W, H, _("Configuration"));
	w->size_range(W, H, W, H);

	int xtree = 2;
	int ytree = 2;
	int htree = H - 2*ytree;
	int wtree = 180;
	int wtabs = W - wtree - 2 * xtree;
	int xtabs = xtree + wtree;

	tab_tree = new Fl_Tree(xtree, ytree, wtree, htree);
	tab_tree->callback((Fl_Callback*)SelectItem_CB);
	Fl_Group::current()->resizable(tab_tree);
	tab_tree->root_label(_("Configure"));
	tab_tree->selectmode(FL_TREE_SELECT_SINGLE);
	tab_tree->connectorstyle(FL_TREE_CONNECTOR_DOTTED);
	tab_tree->connectorwidth(14); // default is 17

	tabXCVR     = createXCVR(xtabs, ytree, wtabs, htree, _("Xcvr"));
	tabCMEDIA   = createCmediaTab(xtabs, ytree, wtabs, htree, ("PTT-Cmedia"));
	tabTMATE2   = createTMATE2Tab(xtabs, ytree, wtabs, htree, ("TMATE-2"));
	tabGPIO     = createGPIO(xtabs, ytree, wtabs, htree, _("PTT-GPIO"));
	tabPTTGEN   = createPTT(xtabs, ytree, wtabs, htree, _("PTT-Generic"));
	tabTCPIP    = createTCPIP(xtabs, ytree, wtabs, htree, _("TCPIP"));
	tabAUX      = createAUX(xtabs, ytree, wtabs, htree, _("Auxiliary"));
	tabSERVER   = createSERVER(xtabs, ytree, wtabs, htree, _("Server"));
	tabPOLLING  = createPOLLING(xtabs, ytree, wtabs, htree, _("Poll"));
	tabRESTORE  = createRestore(xtabs, ytree, wtabs, htree, _("Restore"));
	tabCOMMANDS = createCOMMANDS(xtabs, ytree, wtabs, htree, _("Commands"));
	tabSNDCMD   = createSNDCMD(xtabs, ytree, wtabs, htree, _("Send"));
	tabTRACE    = createTRACE(xtabs, ytree, wtabs, htree, _("Trace"));

	tab_tree->begin();

	add_tree_item(tabXCVR);

	add_tree_item(tabPTTGEN);
	add_tree_item(tabCMEDIA);
	add_tree_item(tabTMATE2);

	add_tree_item(tabTCPIP);
	add_tree_item(tabAUX);
	add_tree_item(tabSERVER);
	add_tree_item(tabPOLLING);
	add_tree_item(tabRESTORE);
	add_tree_item(tabCOMMANDS);
	add_tree_item(tabSNDCMD);
	add_tree_item(tabTRACE);

	tab_tree->end();

w->end();

return w;

}

