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
#include "ICbase.h"

Fl_Tabs *tabsConfig = (Fl_Tabs *)0;
Fl_Group *tabPrimary = (Fl_Group *)0;
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
Fl_Round_Button *btncatptt = (Fl_Round_Button *)0;
Fl_Round_Button *btnrtsptt = (Fl_Round_Button *)0;
Fl_Round_Button *btndtrptt = (Fl_Round_Button *)0;
Fl_Check_Button *chkrtscts = (Fl_Check_Button *)0;
Fl_Check_Button *btnrtsplus = (Fl_Check_Button *)0;
Fl_Check_Button *btndtrplus = (Fl_Check_Button *)0;
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

Fl_Group *tabPTT = (Fl_Group *)0;
Fl_ComboBox *selectSepPTTPort = (Fl_ComboBox *)0;
Fl_Check_Button *btnSep_SCU_17 = (Fl_Check_Button *)0;
Fl_Check_Button *btnSepRTSptt = (Fl_Check_Button *)0;
Fl_Check_Button *btnSepRTSplus = (Fl_Check_Button *)0;
Fl_Check_Button *btnSepDTRptt = (Fl_Check_Button *)0;
Fl_Check_Button *btnSepDTRplus = (Fl_Check_Button *)0;

Fl_Group *tabAux = (Fl_Group *)0;
Fl_ComboBox *selectAuxPort = (Fl_ComboBox *)0;
Fl_Check_Button *btnAux_SCU_17 = (Fl_Check_Button *)0;

Fl_Group *tabPolling = (Fl_Group *)0;
Fl_Value_Input *poll_smeter = (Fl_Value_Input *)0;
Fl_Value_Input *poll_pout = (Fl_Value_Input *)0;
Fl_Value_Input *poll_swr = (Fl_Value_Input *)0;
Fl_Value_Input *poll_alc = (Fl_Value_Input *)0;
Fl_Value_Input *poll_frequency = (Fl_Value_Input *)0;
Fl_Value_Input *poll_mode = (Fl_Value_Input *)0;
Fl_Value_Input *poll_bandwidth = (Fl_Value_Input *)0;
Fl_Value_Input *poll_volume = (Fl_Value_Input *)0;
Fl_Value_Input *poll_micgain = (Fl_Value_Input *)0;
Fl_Value_Input *poll_rfgain = (Fl_Value_Input *)0;
Fl_Value_Input *poll_power_control = (Fl_Value_Input *)0;
Fl_Value_Input *poll_ifshift = (Fl_Value_Input *)0;
Fl_Value_Input *poll_notch = (Fl_Value_Input *)0;
Fl_Value_Input *poll_auto_notch = (Fl_Value_Input *)0;
Fl_Value_Input *poll_pre_att = (Fl_Value_Input *)0;
Fl_Value_Input *poll_squelch = (Fl_Value_Input *)0;
Fl_Value_Input *poll_split = (Fl_Value_Input *)0;
Fl_Value_Input *poll_noise = (Fl_Value_Input *)0;
Fl_Value_Input *poll_nr = (Fl_Value_Input *)0;
Fl_Value_Input *poll_compression = (Fl_Value_Input *)0;
Fl_Button *btnClearAddControls = (Fl_Button *)0;

Fl_Value_Input *poll_meters = (Fl_Value_Input *)0;
Fl_Button *btnSetMeters = (Fl_Button *)0;
Fl_Value_Input *poll_ops = (Fl_Value_Input *)0;
Fl_Button *btnSetOps = (Fl_Button *)0;
Fl_Value_Input *poll_all = (Fl_Value_Input *)0;
Fl_Button *btnSetAdd = (Fl_Button *)0;

Fl_Group *tabSndCmd = (Fl_Group *)0;
Fl_Input2 *txt_command = (Fl_Input2 *)0;
Fl_Button *btn_icom_pre = (Fl_Button *)0;
Fl_Button *btn_icom_post = (Fl_Button *)0;
Fl_Button *btn_send_command = (Fl_Button *)0;
Fl_Output *txt_response = (Fl_Output *)0;
Fl_Box *box_xcvr_connect = (Fl_Box *)0;
Fl_Box *box_fldigi_connect = (Fl_Box *)0;

Fl_Button *btnOkXcvrDialog = (Fl_Button *)0;
Fl_Button *btnCloseCommConfig = (Fl_Button *)0;
Fl_Button *btnOkSepSerial = (Fl_Button *)0;
Fl_Button *btnOkAuxSerial = (Fl_Button *)0;

//Fl_Group *tabXMLRPC = (Fl_Group *)0;
//Fl_Input *server_addr = (Fl_Input *)0;
//Fl_Int_Input *server_port = (Fl_Int_Input *)0;
//Fl_Check_Button *btn_xmlrpc_server = (Fl_Check_Button *)0;

Fl_Group *tabCmds = (Fl_Group *)0;
Fl_Box *bx1 = (Fl_Box *)0;
Fl_Box *bx2 = (Fl_Box *)0;
Fl_Input2 * cmdlbl1 = (Fl_Input2 *)0;
Fl_Input2 * cmdlbl2 = (Fl_Input2 *)0;
Fl_Input2 * cmdlbl3 = (Fl_Input2 *)0;
Fl_Input2 * cmdlbl4 = (Fl_Input2 *)0;
Fl_Input2 * cmdlbl5 = (Fl_Input2 *)0;
Fl_Input2 * cmdlbl6 = (Fl_Input2 *)0;
Fl_Input2 * cmdlbl7 = (Fl_Input2 *)0;
Fl_Input2 * cmdlbl8 = (Fl_Input2 *)0;
Fl_Input2 * cmdtext1 = (Fl_Input2 *)0;
Fl_Input2 * cmdtext2 = (Fl_Input2 *)0;
Fl_Input2 * cmdtext3 = (Fl_Input2 *)0;
Fl_Input2 * cmdtext4 = (Fl_Input2 *)0;
Fl_Input2 * cmdtext5 = (Fl_Input2 *)0;
Fl_Input2 * cmdtext6 = (Fl_Input2 *)0;
Fl_Input2 * cmdtext7 = (Fl_Input2 *)0;
Fl_Input2 * cmdtext8 = (Fl_Input2 *)0;
Fl_Output * cmdResponse = (Fl_Output *)0;

Fl_Group *tabRestore = (Fl_Group *)0;
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


//static void cb_xmlrpc_server(Fl_Check_Button *w, void *)
//{
//	progStatus.fldigi_is_server = btn_xmlrpc_server->value();
//}

static void cb_selectRig(Fl_ComboBox*, void*) {
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw_label();
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
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw_label();
}

static void cb_mnuBaudrate(Fl_ComboBox*, void*) {
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw_label();
}

static void cb_btnTwoStopBit(Fl_Check_Button* o, void*) {
	if (o->value() == true) {
		btnOneStopBit->value(false);
		progStatus.stopbits = 2;
	} else
		o->value(true);
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw_label();
}

static void cb_btnOneStopBit(Fl_Check_Button* o, void*) {
	if (o->value() == true) {
		btnTwoStopBit->value(false);
		progStatus.stopbits = 1;
	} else
		o->value(true);
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw_label();
}

static void cb_btnRigCatEcho(Fl_Check_Button*, void*) {
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw_label();
}

//static void cb_server_addr(Fl_Input* o, void*) {
//	progStatus.server_addr = o->value();
//}

//static void cb_server_port(Fl_Int_Input* o, void*) {
//	progStatus.server_port = o->value();
//}

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

static void cb_btncatptt(Fl_Round_Button* o, void*) {
	if (o->value()== 1) {
		btnrtsptt->value(0);
		btndtrptt->value(0);
	}
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw_label();
}

static void cb_btnrtsptt(Fl_Round_Button* o, void*) {
	if (o->value() == 1)
		btncatptt->value(0);
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw_label();
}

static void cb_btndtrptt(Fl_Round_Button* o, void*) {
	if (o->value() == 1)
		btncatptt->value(0);
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw_label();
}

static void cb_chkrtscts(Fl_Check_Button*, void*) {
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw_label();
}

static void cb_btnrtsplus(Fl_Check_Button*, void*) {
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw_label();
}

static void cb_btndtrplus(Fl_Check_Button*, void*) {
	btnOkXcvrDialog->labelcolor(FL_RED);
	btnOkXcvrDialog->redraw_label();
}

//static void cb_btn_notxqsy(Fl_Check_Button*, void*) {
//	progStatus.no_txqsy = btn_notxqsy->value();
//}

static void cb_txtCIV(Fl_Int_Input* o, void*) {
	progStatus.CIV = atol(o->value());
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

static void cb_btnSepRTSptt(Fl_Check_Button* o, void*) {
	btnOkSepSerial->labelcolor(FL_RED);
	btnOkSepSerial->redraw_label();
}

static void cb_btnSepRTSplus(Fl_Check_Button*, void*) {
	btnOkSepSerial->labelcolor(FL_RED);
	btnOkSepSerial->redraw_label();
}

static void cb_btnSepDTRptt(Fl_Check_Button* o, void*) {
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

static void cb_poll_smeter(Fl_Value_Input* o, void*) {
	progStatus.poll_smeter = o->value();
}

static void cb_poll_pout(Fl_Value_Input* o, void*) {
	progStatus.poll_pout = o->value();
}

static void cb_poll_swr(Fl_Value_Input* o, void*) {
	progStatus.poll_swr = o->value();
}

static void cb_poll_alc(Fl_Value_Input* o, void*) {
	progStatus.poll_alc = o->value();
}

static void cb_poll_frequency(Fl_Value_Input* o, void*) {
	progStatus.poll_frequency = o->value();
}

static void cb_poll_mode(Fl_Value_Input* o, void*) {
	progStatus.poll_mode = o->value();
}

static void cb_poll_bandwidth(Fl_Value_Input* o, void*) {
	progStatus.poll_bandwidth = o->value();
}

static void cb_poll_volume(Fl_Value_Input* o, void*) {
	progStatus.poll_volume = o->value();
}

static void cb_poll_micgain(Fl_Value_Input* o, void*) {
	progStatus.poll_micgain = o->value();
}

static void cb_poll_rfgain(Fl_Value_Input* o, void*) {
	progStatus.poll_rfgain = o->value();
}

static void cb_poll_power_control(Fl_Value_Input* o, void*) {
	progStatus.poll_power_control = o->value();
}

static void cb_poll_ifshift(Fl_Value_Input* o, void*) {
	progStatus.poll_ifshift = o->value();
}

static void cb_poll_notch(Fl_Value_Input* o, void*) {
	progStatus.poll_notch = o->value();
}

static void cb_poll_auto_notch(Fl_Value_Input* o, void*) {
	progStatus.poll_auto_notch = o->value();
}

static void cb_poll_pre_att(Fl_Value_Input* o, void*) {
	progStatus.poll_pre_att = o->value();
}

static void cb_poll_squelch(Fl_Value_Input* o, void*) {
	progStatus.poll_squelch = o->value();
}

static void cb_poll_split(Fl_Value_Input* o, void*) {
	progStatus.poll_split = o->value();
}

static void cb_poll_noise(Fl_Value_Input* o, void*) {
	progStatus.poll_noise = o->value();
}

static void cb_poll_nr(Fl_Value_Input* o, void*) {
	progStatus.poll_noise = o->value();
}

static void cb_poll_compression(Fl_Value_Input* o, void *) {
	progStatus.poll_compression = o->value();
}

static void cb_poll_meters(Fl_Value_Input* o, void*) {
	progStatus.poll_meters = o->value();
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

static void cb_poll_ops(Fl_Value_Input* o, void*) {
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

static void cb_poll_all(Fl_Value_Input* o, void*) {
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

	progStatus.poll_volume = progStatus.poll_all;
	progStatus.poll_micgain = progStatus.poll_all;
	progStatus.poll_rfgain = progStatus.poll_all;
	progStatus.poll_power_control = progStatus.poll_all;
	progStatus.poll_ifshift = progStatus.poll_all;
	progStatus.poll_notch = progStatus.poll_all;
	progStatus.poll_auto_notch = progStatus.poll_all;
	progStatus.poll_pre_att = progStatus.poll_all;
	progStatus.poll_squelch = progStatus.poll_all;
	progStatus.poll_split = progStatus.poll_all;
	progStatus.poll_noise = progStatus.poll_all;
	progStatus.poll_nr = progStatus.poll_all;
	progStatus.poll_compression = progStatus.poll_all;
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
	long val = reinterpret_cast<long>(d);
	switch (val) {
		case 1: 
			progStatus.label1 = o->value();
			btnUser1->label(o->value()); 
			btnUser1->redraw_label();
			break;
		case 2: 
			progStatus.label2 = o->value();
			btnUser2->label(o->value()); 
			btnUser2->redraw_label();
			break;
		case 3: 
			progStatus.label3 = o->value();
			btnUser3->label(o->value()); 
			btnUser3->redraw_label();
			break;
		case 4: 
			progStatus.label4 = o->value();
			btnUser4->label(o->value()); 
			btnUser4->redraw_label();
			break;
		case 5: 
			progStatus.label5 = o->value();
			btnUser5->label(o->value()); 
			btnUser5->redraw_label();
			break;
		case 6: 
			progStatus.label6 = o->value();
			btnUser6->label(o->value()); 
			btnUser6->redraw_label();
			break;
		case 7: 
			progStatus.label7 = o->value();
			btnUser7->label(o->value()); 
			btnUser7->redraw_label();
			break;
		case 8: 
			progStatus.label8 = o->value();
			btnUser8->label(o->value()); 
			btnUser8->redraw_label();
			break;
	}
}

static void cb_cmdtext(Fl_Input2 *o, void *d) {
	long val = reinterpret_cast<long>(d);
	switch (val) {
		case 1: 
			progStatus.command1 = o->value();
			break;
		case 2: 
			progStatus.command2 = o->value();
			break;
		case 3: 
			progStatus.command3 = o->value();
			break;
		case 4: 
			progStatus.command4 = o->value();
			break;
		case 5: 
			progStatus.command5 = o->value();
			break;
		case 6: 
			progStatus.command6 = o->value();
			break;
		case 7: 
			progStatus.command7 = o->value();
			break;
		case 8: 
			progStatus.command8 = o->value();
			break;
	}
}

static void cb_btnOkXcvrDialog(Fl_Return_Button*, void*) {
	string p1 = selectCommPort->value();
	string p2 = selectAuxPort->value();
	string p3 = selectSepPTTPort->value();

	if ( (p1.compare("NONE") != 0) && (p1 == p2 || p1 == p3) ) {
		fl_message("Select separate ports");
		return;
	}

	if (progStatus.UIsize == wide_ui) {
		btn_show_controls->label("@-22->");
		btn_show_controls->redraw_label();
		grpTABS->hide();
		mainwindow->resizable(grpTABS);
		mainwindow->size(mainwindow->w(), 150);
		mainwindow->size_range(735, 150, 0, 150);
	}

	// close the current rig control
	closeRig();               // local serial comm connection

	{ guard_lock gl_serial(&mutex_serial, 200);
		RigSerial->ClosePort();
		bypass_serial_thread_loop = true;
	}

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
	progStatus.comm_rtsptt = btnrtsptt->value();
	progStatus.comm_catptt = btncatptt->value();
	progStatus.comm_dtrptt = btndtrptt->value();
	progStatus.comm_rtscts = chkrtscts->value();
	progStatus.comm_rtsplus = btnrtsplus->value();
	progStatus.comm_dtrplus = btndtrplus->value();

	progStatus.imode_B  = progStatus.imode_A  = selrig->def_mode;
	progStatus.iBW_B    = progStatus.iBW_A    = selrig->def_bw;
	progStatus.freq_B   = progStatus.freq_A   = selrig->def_freq;

	init_title();

	if (!startXcvrSerial()) {
		if (progStatus.xcvr_serial_port.compare("NONE") == 0) {
			LOG_WARN("No comm port ... test mode");
		} else {
			progStatus.xcvr_serial_port = "NONE";
			selectCommPort->value(progStatus.xcvr_serial_port.c_str());
		}
	}

	initRig();

	btnOkXcvrDialog->labelcolor(FL_BLACK);
	btnOkXcvrDialog->redraw_label();
}

static void cb_btnOkSepSerial(Fl_Button*, void*) {
	string p2 = selectAuxPort->value();
	string p3 = selectSepPTTPort->value();

	if ((p3.compare("NONE") != 0) && p2 == p3) {
		fl_message("Port used for Aux i/o");
		return;
	}

	progStatus.sep_serial_port = selectSepPTTPort->value();
	progStatus.sep_SCU_17 = btnSep_SCU_17->value();

	progStatus.sep_dtrplus = btnSepDTRplus->value();
	progStatus.sep_dtrptt = btnSepDTRptt->value();
	progStatus.sep_rtsplus = btnSepRTSplus->value();
	progStatus.sep_rtsptt = btnSepRTSptt->value();

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

	string p2 = selectAuxPort->value();
	string p3 = selectSepPTTPort->value();

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

extern Fl_Double_Window *dlgXcvrConfig;

static void cbCloseXcvrDialog(Fl_Button*, void*)
{
	btnOkXcvrDialog->labelcolor(FL_BLACK);
	btnOkXcvrDialog->redraw_label();
	btnOkAuxSerial->labelcolor(FL_BLACK);
	btnOkAuxSerial->redraw_label();
	btnOkSepSerial->labelcolor(FL_BLACK);
	btnOkSepSerial->redraw_label();

	dlgXcvrConfig->hide();
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

Fl_Double_Window* XcvrDialog() {

Fl_Double_Window* w = new Fl_Double_Window(480, 255, _("I/O Ports"));

	tabsConfig = new Fl_Tabs(0, 8, 482, 246);

	tabPrimary = new Fl_Group(2, 30, 475, 222, _("Xcvr"));

		Fl_Group* xcr_grp1 = new Fl_Group(5, 34, 465, 140);
			xcr_grp1->box(FL_ENGRAVED_FRAME);

			selectRig = new Fl_ComboBox(80, 40, 190, 22, _("Rig:"));
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
			selectRig->when(FL_WHEN_RELEASE);
			selectRig->end();

			Fl_Button *comports = new Fl_Button(xcr_grp1->x()+4, 65,
							80 - xcr_grp1->x() - 8, 22, _("Ser Port"));
			comports->box(FL_THIN_UP_BOX);
			comports->tooltip(_("Update serial port combo"));
			comports->callback((Fl_Callback*)cb_comports);
			comports->when(FL_WHEN_RELEASE);

			selectCommPort = new Fl_ComboBox(80, 65, 190, 22, "");
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

			mnuBaudrate = new Fl_ComboBox(80, 90, 190, 22, _("Baud:"));
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
			mnuBaudrate->when(FL_WHEN_RELEASE);
			mnuBaudrate->end();

			btnOneStopBit = new Fl_Check_Button(20, 120, 22, 15, _("1"));
			btnOneStopBit->tooltip(_("One Stop Bit"));
			btnOneStopBit->down_box(FL_DOWN_BOX);
			btnOneStopBit->callback((Fl_Callback*)cb_btnOneStopBit);
			btnOneStopBit->align(Fl_Align(FL_ALIGN_RIGHT));
			btnOneStopBit->value(progStatus.stopbits == 1);

			btnTwoStopBit = new Fl_Check_Button(120, 120, 22, 15, _("2 -StopBits"));
			btnTwoStopBit->down_box(FL_DOWN_BOX);
			btnTwoStopBit->callback((Fl_Callback*)cb_btnTwoStopBit);
			btnTwoStopBit->align(Fl_Align(FL_ALIGN_RIGHT));
			btnTwoStopBit->value(progStatus.stopbits == 2);

			btnRigCatEcho = new Fl_Check_Button(20, 145, 22, 15, _("Echo "));
			btnRigCatEcho->down_box(FL_DOWN_BOX);
			btnRigCatEcho->callback((Fl_Callback*)cb_btnRigCatEcho);
			btnRigCatEcho->align(Fl_Align(FL_ALIGN_RIGHT));
			btnRigCatEcho->value(progStatus.comm_echo);

//			btn_notxqsy = new Fl_Check_Button(120, 145, 22, 15, _("Disable tx_qsy"));
//			btn_notxqsy->down_box(FL_DOWN_BOX);
//			btn_notxqsy->callback((Fl_Callback*)cb_btn_notxqsy);
//			btn_notxqsy->align(Fl_Align(FL_ALIGN_RIGHT));
//			btn_notxqsy->value(progStatus.no_txqsy);
//			btn_notxqsy->tooltip(_("check if transceiver cannot change freq\nwhen PTT enabled"));

			cntRigCatRetries = new Fl_Counter(350, 40, 100, 20, _("Retries"));
			cntRigCatRetries->tooltip(_("Number of  times to resend\ncommand before giving up"));
			cntRigCatRetries->minimum(1);
			cntRigCatRetries->maximum(10);
			cntRigCatRetries->step(1);
			cntRigCatRetries->value(5);
			cntRigCatRetries->callback((Fl_Callback*)cb_cntRigCatRetries);
			cntRigCatRetries->align(Fl_Align(FL_ALIGN_LEFT));
			cntRigCatRetries->value(progStatus.comm_retries);
			cntRigCatRetries->lstep(10);

			cntRigCatTimeout = new Fl_Counter(350, 65, 100, 20, _("Retry intvl"));
			cntRigCatTimeout->tooltip(_("Time between retries is msec"));
			cntRigCatTimeout->minimum(2);
			cntRigCatTimeout->maximum(200);
			cntRigCatTimeout->step(1);
			cntRigCatTimeout->value(10);
			cntRigCatTimeout->callback((Fl_Callback*)cb_cntRigCatTimeout);
			cntRigCatTimeout->align(Fl_Align(FL_ALIGN_LEFT));
			cntRigCatTimeout->value(progStatus.comm_timeout);
			cntRigCatTimeout->lstep(10);

			cntRigCatWait = new Fl_Counter(350, 90, 100, 20, _("Cmds"));
			cntRigCatWait->tooltip(_("Wait millseconds between sequential commands"));
			cntRigCatWait->minimum(0);
			cntRigCatWait->maximum(100);
			cntRigCatWait->step(1);
			cntRigCatWait->value(5);
			cntRigCatWait->callback((Fl_Callback*)cb_cntRigCatWait);
			cntRigCatWait->align(Fl_Align(FL_ALIGN_LEFT));
			cntRigCatWait->value(progStatus.comm_wait);
			cntRigCatWait->lstep(10);

			query_interval = new Fl_Counter(350, 115, 100, 22, _("Poll intvl"));
			query_interval->tooltip(_("Polling interval in msec"));
			query_interval->minimum(10);
			query_interval->maximum(5000);
			query_interval->step(1);
			query_interval->value(50);
			query_interval->callback((Fl_Callback*)cb_query_interval);
			query_interval->align(Fl_Align(FL_ALIGN_LEFT));
			query_interval->value(progStatus.serloop_timing);
			query_interval->lstep(10);

			byte_interval = new Fl_Counter(350, 140, 100, 22, _("Byte intvl"));
			byte_interval->tooltip(_("Inter-byte interval (msec)"));
			byte_interval->minimum(0);
			byte_interval->maximum(200);
			byte_interval->step(1);
			byte_interval->value(0);
			byte_interval->callback((Fl_Callback*)cb_byte_interval);
			byte_interval->align(Fl_Align(FL_ALIGN_LEFT));
			byte_interval->value(progStatus.byte_interval);
			byte_interval->lstep(10);

		xcr_grp1->end();

		Fl_Group* xcr_grp4 = new Fl_Group(4, 175, 243, 73);
			xcr_grp4->box(FL_ENGRAVED_FRAME);

			btncatptt = new Fl_Round_Button(15, 180, 149, 22, _("PTT via CAT"));
			btncatptt->tooltip(_("PTT is a CAT command (not hardware)"));
			btncatptt->down_box(FL_ROUND_DOWN_BOX);
			btncatptt->callback((Fl_Callback*)cb_btncatptt);
			btncatptt->value(progStatus.comm_catptt);

			btnrtsptt = new Fl_Round_Button(15, 203, 149, 22, _("PTT via RTS"));
			btnrtsptt->tooltip(_("RTS is ptt line"));
			btnrtsptt->down_box(FL_ROUND_DOWN_BOX);
			btnrtsptt->callback((Fl_Callback*)cb_btnrtsptt);
			btnrtsptt->value(progStatus.comm_rtsptt);

			btndtrptt = new Fl_Round_Button(15, 226, 149, 22, _("PTT via DTR"));
			btndtrptt->tooltip(_("DTR is ptt line"));
			btndtrptt->down_box(FL_ROUND_DOWN_BOX);
			btndtrptt->callback((Fl_Callback*)cb_btndtrptt);
			btndtrptt->value(progStatus.comm_dtrptt);

			chkrtscts = new Fl_Check_Button(129, 178, 98, 21, _("RTS/CTS"));
			chkrtscts->tooltip(_("Xcvr uses RTS/CTS handshake"));
			chkrtscts->down_box(FL_DOWN_BOX);
			chkrtscts->callback((Fl_Callback*)cb_chkrtscts);
			chkrtscts->value(progStatus.comm_rtscts);

			btnrtsplus = new Fl_Check_Button(129, 201, 102, 21, _("RTS +12 v"));
			btnrtsplus->tooltip(_("Initial state of RTS"));
			btnrtsplus->down_box(FL_DOWN_BOX);
			btnrtsplus->callback((Fl_Callback*)cb_btnrtsplus);
			btnrtsplus->value(progStatus.comm_rtsplus);

			btndtrplus = new Fl_Check_Button(129, 224, 100, 21, _("DTR +12 v"));
			btndtrplus->tooltip(_("Initial state of DTR"));
			btndtrplus->down_box(FL_DOWN_BOX);
			btndtrplus->callback((Fl_Callback*)cb_btndtrplus);
			btndtrplus->value(progStatus.comm_dtrplus);

		xcr_grp4->end();

		Fl_Group* xcr_grp5 = new Fl_Group(247, 175, 225, 36);
			xcr_grp5->box(FL_ENGRAVED_FRAME);
			xcr_grp5->align(Fl_Align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE));

			txtCIV = new Fl_Int_Input(261, 183, 58, 22, _("CI-V adr"));
			txtCIV->tooltip(_("Enter hex value, ie: 0x5F"));
			txtCIV->type(2);
			txtCIV->callback((Fl_Callback*)cb_txtCIV);
			txtCIV->align(Fl_Align(FL_ALIGN_RIGHT));

			btnCIVdefault = new Fl_Button(387, 183, 69, 22, _("Default"));
			btnCIVdefault->callback((Fl_Callback*)cb_btnCIVdefault);

		xcr_grp5->end();

		Fl_Group* xcr_grp6 = new Fl_Group(247, 212, 103, 36);
			xcr_grp6->box(FL_ENGRAVED_FRAME);

			btnUSBaudio = new Fl_Check_Button(250, 222, 100, 15, _("USB audio"));
			btnUSBaudio->down_box(FL_DOWN_BOX);
			btnUSBaudio->callback((Fl_Callback*)cb_btnUSBaudio);

		xcr_grp6->end();

		btnOkXcvrDialog = new Fl_Button(w->w() - 60, 214, 50, 24, _("Init"));
		btnOkXcvrDialog->callback((Fl_Callback*)cb_btnOkXcvrDialog);

	tabPrimary->end();
/*
	tabXMLRPC = new Fl_Group(2, 30, 475, 222, _("XML"));
		tabXMLRPC->hide();

		Fl_Group* grp_fldigi_server = new Fl_Group(80, 40, 300, 80);
		grp_fldigi_server->box(FL_ENGRAVED_FRAME);

		btn_xmlrpc_server = new Fl_Check_Button(200, 44, 80, 20, _("Use fldigi server"));
		btn_xmlrpc_server->tooltip(_("Change requires restart!\nAre you sure?"));
		btn_xmlrpc_server->value(progStatus.fldigi_is_server);
		btn_xmlrpc_server->callback((Fl_Callback*)cb_xmlrpc_server);

		server_addr = new Fl_Input(200, 68, 140, 22, _("Fldigi address:"));
		server_addr->tooltip(_("xmlrpc server address (7362)\nchange requires restart!\nAre you sure?"));
		server_addr->callback((Fl_Callback*)cb_server_addr);
		server_addr->value(progStatus.server_addr.c_str());

		server_port = new Fl_Int_Input(200, 92, 100, 22, _("Fldigi port:"));
		server_port->tooltip(_("xmlrpc server address (7362)\nchange requires restart!\nAre you sure?"));
		server_port->type(2);
		server_port->callback((Fl_Callback*)cb_server_port);
		server_port->value(progStatus.server_port.c_str());

		grp_fldigi_server->end();

		Fl_Box *bx_xml_info = new Fl_Box(20, 140, 450, 100,
_("\
Recommended setup: flrig as server, fldigi as client\n\n\
Changes to requires an flrig restart before they are effective.\n\
"));
		bx_xml_info->box(FL_ENGRAVED_FRAME);

	tabXMLRPC->end();
*/
	tabTCPIP = new Fl_Group(2, 30, 475, 222, _("TCPIP"));
		tabTCPIP->hide();

		inp_tcpip_addr = new Fl_Input2(120, 50, 300, 22, _("TCPIP address:"));
		inp_tcpip_addr->tooltip(_("remote tcpip server address"));
		inp_tcpip_addr->callback((Fl_Callback*)cb_tcpip_addr);
		inp_tcpip_addr->value(progStatus.tcpip_addr.c_str());

		inp_tcpip_port = new Fl_Input2(120, 74, 100, 22, _("TCPIP port:"));
		inp_tcpip_port->tooltip(_("remote tcpip server port"));
		inp_tcpip_port->type(2);
		inp_tcpip_port->callback((Fl_Callback*)cb_tcpip_port);
		inp_tcpip_port->value(progStatus.tcpip_port.c_str());

		inp_tcpip_ping_delay = new Fl_Counter(120, 100, 100, 22, _("Ping delay"));
		inp_tcpip_ping_delay->tooltip(_("enter round trip ping delay"));
		inp_tcpip_ping_delay->callback((Fl_Callback*)cb_tcpip_ping_delay);
		inp_tcpip_ping_delay->minimum(0);
		inp_tcpip_ping_delay->maximum(500);
		inp_tcpip_ping_delay->step(5);
		inp_tcpip_ping_delay->lstep(20);
		inp_tcpip_ping_delay->value(progStatus.tcpip_ping_delay);
		inp_tcpip_ping_delay->align(Fl_Align(FL_ALIGN_LEFT));

		chk_use_tcpip = new Fl_Check_Button(120, 128, 18, 18, _("Use tcpip"));
		chk_use_tcpip->tooltip(_("Rig control via tcpip"));
		chk_use_tcpip->down_box(FL_DOWN_BOX);
		chk_use_tcpip->callback((Fl_Callback*)cb_use_tcpip);
		chk_use_tcpip->value(progStatus.use_tcpip);
		chk_use_tcpip->align(Fl_Align(FL_ALIGN_LEFT));

		box_tcpip_connect = new Fl_Box(120, 150, 18, 18, _("Connected"));
		box_tcpip_connect->tooltip(_("Lit when connected to remote tcpip"));
		box_tcpip_connect->box(FL_DIAMOND_DOWN_BOX);
		box_tcpip_connect->color(FL_LIGHT1);
		box_tcpip_connect->align(Fl_Align(FL_ALIGN_RIGHT));

		cntRetryAfter = new Fl_Counter(120, 172, 100, 20, _("Retry (secs)"));
		cntRetryAfter->tooltip(_("Retry connection if lost"));
		cntRetryAfter->minimum(1);
		cntRetryAfter->maximum(120);
		cntRetryAfter->step(1);
		cntRetryAfter->lstep(10);
		cntRetryAfter->callback((Fl_Callback*)cb_cntRetryAfter);
		cntRetryAfter->align(Fl_Align(FL_ALIGN_LEFT));
		cntRetryAfter->value(progStatus.tcpip_reconnect_after);

		cntDropsAllowed = new Fl_Counter(120, 196, 100, 20, _("Allowed drops"));
		cntDropsAllowed->tooltip(_("# tcpip drop-outs before connection declared down"));
		cntDropsAllowed->minimum(1);
		cntDropsAllowed->maximum(25);
		cntDropsAllowed->step(1);
		cntDropsAllowed->lstep(5);
		cntDropsAllowed->callback((Fl_Callback*)cb_cntDropsAllowed);
		cntDropsAllowed->align(Fl_Align(FL_ALIGN_LEFT));
		cntDropsAllowed->value(progStatus.tcpip_drops_allowed);

	tabTCPIP->end();

	tabPTT = new Fl_Group(2, 30, 475, 222, _("PTT"));
		tabPTT->hide();

		Fl_Box *bxptt = new Fl_Box(53, 73, 399, 37,
_("Use only if your setup requires a separate\nSerial Port for a PTT control line"));
		bxptt->box(FL_FLAT_BOX);

		selectSepPTTPort = new Fl_ComboBox(128, 121, 192, 22, _("PTT Port"));
		selectSepPTTPort->tooltip(_("Aux control port"));
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

		btnSepRTSptt = new Fl_Check_Button(111, 150, 128, 22, _("PTT via RTS"));
		btnSepRTSptt->tooltip(_("RTS is ptt line"));
		btnSepRTSptt->callback((Fl_Callback*)cb_btnSepRTSptt);
		btnSepRTSptt->value(progStatus.sep_rtsptt);

		btnSepRTSplus = new Fl_Check_Button(274, 150, 128, 22, _("RTS +12 v"));
		btnSepRTSplus->tooltip(_("Initial state of RTS"));
		btnSepRTSplus->down_box(FL_DOWN_BOX);
		btnSepRTSplus->callback((Fl_Callback*)cb_btnSepRTSplus);
		btnSepRTSplus->value(progStatus.sep_rtsplus);

		btnSepDTRptt = new Fl_Check_Button(111, 176, 128, 22, _("PTT via DTR"));
		btnSepDTRptt->tooltip(_("DTR is ptt line"));
		btnSepDTRptt->callback((Fl_Callback*)cb_btnSepDTRptt);
		btnSepDTRptt->value(progStatus.sep_dtrptt);

		btnSepDTRplus = new Fl_Check_Button(274, 176, 128, 22, _("DTR +12 v"));
		btnSepDTRplus->tooltip(_("Initial state of DTR"));
		btnSepDTRplus->down_box(FL_DOWN_BOX);
		btnSepDTRplus->callback((Fl_Callback*)cb_btnSepDTRplus);
		btnSepDTRplus->value(progStatus.sep_dtrplus);

		btnSep_SCU_17 = new Fl_Check_Button(111, 200, 128, 22, _("Serial Port is SCU-17 auxiliary"));
		btnSep_SCU_17->tooltip(_("Set stop bits to ZERO"));
		btnSep_SCU_17->callback((Fl_Callback*)cb_btnSep_SCU_17);
		btnSep_SCU_17->value(progStatus.sep_SCU_17);

		btnOkSepSerial = new Fl_Button(w->w() - 60, 214, 50, 24, _("Init"));
		btnOkSepSerial->callback((Fl_Callback*)cb_btnOkSepSerial);

	tabPTT->end();


	tabAux = new Fl_Group(2, 30, 475, 222, _("Aux"));
		tabAux->hide();

		selectAuxPort = new Fl_ComboBox(131, 132, 192, 22, _("Aux"));
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

		btnAux_SCU_17 = new Fl_Check_Button(131, 170, 128, 22, _("Serial Port is SCU-17 auxiliary"));
		btnAux_SCU_17->tooltip(_("Set stop bits to ZERO"));
		btnAux_SCU_17->callback((Fl_Callback*)cb_btnAux_SCU_17);
		btnAux_SCU_17->value(progStatus.aux_SCU_17);

		Fl_Box *bxsep = new Fl_Box(56, 84, 399, 38,
_("Use only if your setup requires a separate\nSerial Port for a special Control Signals"));
		bxsep->box(FL_FLAT_BOX);

		btnOkAuxSerial = new Fl_Button(w->w() - 60, 214, 50, 24, _("Init"));
		btnOkAuxSerial->callback((Fl_Callback*)cb_btnOkAuxSerial);

	tabAux->end();


	tabPolling = new Fl_Group(2, 30, 476, 222, _("Poll"));
		tabPolling->hide();

		Fl_Group* xcr_grp7 = new Fl_Group(4, 34, 474, 48, _("Meters"));
			xcr_grp7->box(FL_ENGRAVED_BOX);
			xcr_grp7->align(Fl_Align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE));

			poll_smeter = new Fl_Value_Input(10, 55, 30, 20, _("S-mtr"));
			poll_smeter->tooltip(_("Poll every Nth interval"));
			poll_smeter->maximum(10);
			poll_smeter->step(1);
			poll_smeter->value(1);
			poll_smeter->callback((Fl_Callback*)cb_poll_smeter);
			poll_smeter->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_smeter->value(progStatus.poll_smeter);

			poll_pout = new Fl_Value_Input(105, 55, 30, 20, _("Pwr out"));
			poll_pout->tooltip(_("Poll every Nth interval"));
			poll_pout->maximum(10);
			poll_pout->step(1);
			poll_pout->value(1);
			poll_pout->callback((Fl_Callback*)cb_poll_pout);
			poll_pout->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_pout->value(progStatus.poll_pout);

			poll_swr = new Fl_Value_Input(200, 55, 30, 20, _("SWR"));
			poll_swr->tooltip(_("Poll every Nth interval"));
			poll_swr->maximum(10);
			poll_swr->step(1);
			poll_swr->value(1);
			poll_swr->callback((Fl_Callback*)cb_poll_swr);
			poll_swr->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_swr->value(progStatus.poll_swr);

			poll_alc = new Fl_Value_Input(295, 55, 30, 20, _("ALC"));
			poll_alc->tooltip(_("Poll every Nth interval"));
			poll_alc->maximum(10);
			poll_alc->step(1);
			poll_alc->value(1);
			poll_alc->callback((Fl_Callback*)cb_poll_alc);
			poll_alc->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_alc->value(progStatus.poll_alc);

			btnSetMeters = new Fl_Button(370, 55, 60, 20, _("Set all"));
			btnSetMeters->tooltip("Set all meter polls");
			btnSetMeters->callback((Fl_Callback*)cb_btnSetMeters);

			poll_meters = new Fl_Value_Input(435, 55, 30, 20);
			poll_meters->tooltip(_("Poll every Nth interval"));
			poll_meters->maximum(10);
			poll_meters->step(1);
			poll_meters->value(progStatus.poll_meters);
			poll_meters->callback((Fl_Callback*)cb_poll_meters);
			poll_meters->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
			poll_meters->value(progStatus.poll_meters);

		xcr_grp7->end();


		Fl_Group* xcr_grp8 = new Fl_Group(4, 82, 474, 48, _("Operating Controls"));
			xcr_grp8->box(FL_ENGRAVED_BOX);
			xcr_grp8->align(Fl_Align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE));

			poll_frequency = new Fl_Value_Input(10, 103, 30, 20, _("Freq"));
			poll_frequency->tooltip(_("Poll xcvr frequency"));
			poll_frequency->maximum(10);
			poll_frequency->step(1);
			poll_frequency->value(1);
			poll_frequency->callback((Fl_Callback*)cb_poll_frequency);
			poll_frequency->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_frequency->value(progStatus.poll_frequency);

			poll_mode = new Fl_Value_Input(105, 103, 30, 20, _("Mode"));
			poll_mode->tooltip(_("Poll xcvr mode"));
			poll_mode->maximum(10);
			poll_mode->step(1);
			poll_mode->value(1);
			poll_mode->callback((Fl_Callback*)cb_poll_mode);
			poll_mode->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_mode->value(progStatus.poll_mode);

			poll_bandwidth = new Fl_Value_Input(200, 103, 30, 20, _("BW"));
			poll_bandwidth->tooltip(_("Poll xcvr bandwidth"));
			poll_bandwidth->maximum(10);
			poll_bandwidth->step(1);
			poll_bandwidth->value(1);
			poll_bandwidth->callback((Fl_Callback*)cb_poll_bandwidth);
			poll_bandwidth->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_bandwidth->value(progStatus.poll_bandwidth);

			btnSetOps = new Fl_Button(370, 103, 60, 20, _("Set all"));
			btnSetOps->tooltip("Poll all operating values");
			btnSetOps->callback((Fl_Callback*)cb_btnSetOps);

			poll_ops = new Fl_Value_Input(435, 103, 30, 20);
			poll_ops->tooltip(_("Poll every Nth interval"));
			poll_ops->maximum(10);
			poll_ops->step(1);
			poll_ops->value(progStatus.poll_ops);
			poll_ops->callback((Fl_Callback*)cb_poll_ops);
			poll_ops->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
			poll_ops->value(progStatus.poll_ops);

		xcr_grp8->end();

		Fl_Group* xcr_grp9 = new Fl_Group(4, 130, 474, 122, _("Additional Controls"));
			xcr_grp9->box(FL_ENGRAVED_FRAME);
			xcr_grp9->align(Fl_Align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE));

			poll_volume = new Fl_Value_Input(10, 150, 30, 20, _("Volume"));
			poll_volume->tooltip(_("Volume control"));
			poll_volume->maximum(10);
			poll_volume->step(1);
			poll_volume->callback((Fl_Callback*)cb_poll_volume);
			poll_volume->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_volume->value(progStatus.poll_volume);

			poll_micgain = new Fl_Value_Input(105, 150, 30, 20, _("Mic"));
			poll_micgain->tooltip(_("Microphone gain"));
			poll_micgain->maximum(10);
			poll_micgain->step(1);
			poll_micgain->callback((Fl_Callback*)cb_poll_micgain);
			poll_micgain->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_micgain->value(progStatus.poll_micgain);

			poll_rfgain = new Fl_Value_Input(200, 150, 30, 20, _("RF"));
			poll_rfgain->tooltip(_("RF gain"));
			poll_rfgain->maximum(10);
			poll_rfgain->step(1);
			poll_rfgain->callback((Fl_Callback*)cb_poll_rfgain);
			poll_rfgain->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_rfgain->value(progStatus.poll_rfgain);

			poll_power_control = new Fl_Value_Input(295, 150, 30, 20, _("Power"));
			poll_power_control->tooltip(_("Power output"));
			poll_power_control->maximum(10);
			poll_power_control->step(1);
			poll_power_control->callback((Fl_Callback*)cb_poll_power_control);
			poll_power_control->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_power_control->value(progStatus.poll_power_control);

			poll_ifshift = new Fl_Value_Input(10, 175, 30, 20, _("IF"));
			poll_ifshift->tooltip(_("IF shift"));
			poll_ifshift->maximum(10);
			poll_ifshift->step(1);
			poll_ifshift->callback((Fl_Callback*)cb_poll_ifshift);
			poll_ifshift->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_ifshift->value(progStatus.poll_ifshift);

			poll_notch = new Fl_Value_Input(105, 175, 30, 20, _("Notch"));
			poll_notch->tooltip(_("Manual notch"));
			poll_notch->maximum(10);
			poll_notch->step(1);
			poll_notch->callback((Fl_Callback*)cb_poll_notch);
			poll_notch->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_notch->value(progStatus.poll_notch);

			poll_auto_notch = new Fl_Value_Input(200, 175, 30, 20, _("Auto"));
			poll_auto_notch->tooltip(_("Auto notch"));
			poll_auto_notch->maximum(10);
			poll_auto_notch->step(1);
			poll_auto_notch->callback((Fl_Callback*)cb_poll_auto_notch);
			poll_auto_notch->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_auto_notch->value(progStatus.poll_auto_notch);

			poll_pre_att = new Fl_Value_Input(10, 200, 30, 20, _("Pre/Att"));
			poll_pre_att->tooltip(_("Preamp / Attenuator"));
			poll_pre_att->maximum(10);
			poll_pre_att->step(1);
			poll_pre_att->callback((Fl_Callback*)cb_poll_pre_att);
			poll_pre_att->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_pre_att->value(progStatus.poll_pre_att);

			poll_squelch = new Fl_Value_Input(105, 200, 30, 20, _("Squelch"));
			poll_squelch->tooltip(_("Squelch"));
			poll_squelch->maximum(10);
			poll_squelch->step(1);
			poll_squelch->callback((Fl_Callback*)cb_poll_squelch);
			poll_squelch->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_squelch->value(progStatus.poll_squelch);

			poll_split = new Fl_Value_Input(200, 200, 30, 20, _("Split"));
			poll_split->tooltip(_("Split vfo operation"));
			poll_split->maximum(10);
			poll_split->step(1);
			poll_split->callback((Fl_Callback*)cb_poll_split);
			poll_split->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_split->value(progStatus.poll_split);

			poll_noise = new Fl_Value_Input(10, 225, 30, 20, _("Blanker"));
			poll_noise->tooltip(_("Noise blanker"));
			poll_noise->maximum(10);
			poll_noise->step(1);
			poll_noise->callback((Fl_Callback*)cb_poll_noise);
			poll_noise->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_noise->value(progStatus.poll_noise);

			poll_nr = new Fl_Value_Input(105, 225, 30, 20, _("Noise red"));
			poll_nr->tooltip(_("Noise reduction"));
			poll_nr->maximum(10);
			poll_nr->step(1);
			poll_nr->callback((Fl_Callback*)cb_poll_nr);
			poll_nr->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_nr->value(progStatus.poll_noise);

			poll_compression = new Fl_Value_Input(295, 175, 30, 20, _("Comp'"));
			poll_compression->tooltip(_("Compression"));
			poll_compression->maximum(10);
			poll_compression->step(1);
			poll_compression->callback((Fl_Callback*)cb_poll_compression);
			poll_compression->align(Fl_Align(FL_ALIGN_RIGHT));
			poll_compression->value(progStatus.poll_compression);

			btnSetAdd = new Fl_Button(370, 225, 60, 20, _("Set all"));
			btnSetAdd->callback((Fl_Callback*)cb_btnSetAdd);

			poll_all = new Fl_Value_Input(435, 225, 30, 20);
			poll_all->tooltip(_("Poll every Nth interval"));
			poll_all->maximum(10);
			poll_all->step(1);
			poll_all->value(4);
			poll_all->callback((Fl_Callback*)cb_poll_all);
			poll_all->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
			poll_all->value(progStatus.poll_all);

		xcr_grp9->end();

	tabPolling->end();

	tabSndCmd = new Fl_Group(2, 30, 475, 222, _("Send"));
		tabSndCmd->hide();

		txt_command = new Fl_Input2(30, 53, 435, 24,
_("Enter text as ASCII string\nOr sequence of hex values, x80 etc separated by spaces"));
		txt_command->box(FL_DOWN_BOX);
		txt_command->color(FL_BACKGROUND2_COLOR);
		txt_command->selection_color(FL_SELECTION_COLOR);
		txt_command->labeltype(FL_NORMAL_LABEL);
		txt_command->labelfont(0);
		txt_command->labelsize(14);
		txt_command->labelcolor(FL_FOREGROUND_COLOR);
		txt_command->align(Fl_Align(FL_ALIGN_BOTTOM_LEFT));
		txt_command->when(FL_WHEN_RELEASE);

		txt_response = new Fl_Output(30, 115, 435, 24, _("Response to the SEND button"));
		txt_response->align(Fl_Align(FL_ALIGN_BOTTOM_LEFT));

		btn_icom_pre = new Fl_Button(385, 145, 80, 20, _("ICOM pre"));
		btn_icom_pre->callback((Fl_Callback*)cb_btn_icom_pre);

		btn_icom_post = new Fl_Button(385, 173, 80, 20, _("ICOM post"));
		btn_icom_post->callback((Fl_Callback*)cb_btn_icom_post);

		btn_send_command = new Fl_Button(385, 201, 80, 20, _("SEND"));
		btn_send_command->callback((Fl_Callback*)cb_btn_send_command);

		box_xcvr_connect = new Fl_Box(29, 180, 18, 18, _("Connected to transceiver"));
		box_xcvr_connect->tooltip(_("Lit when connected"));
		box_xcvr_connect->box(FL_DIAMOND_DOWN_BOX);
		box_xcvr_connect->color(FL_LIGHT1);
		box_xcvr_connect->align(Fl_Align(FL_ALIGN_RIGHT));

		box_fldigi_connect = new Fl_Box(29, 204, 18, 18, _("Connected to fldigi"));
		box_fldigi_connect->tooltip(_("Lit when connected"));
		box_fldigi_connect->box(FL_DIAMOND_DOWN_BOX);
		box_fldigi_connect->color(FL_LIGHT1);
		box_fldigi_connect->align(Fl_Align(FL_ALIGN_RIGHT));

	tabSndCmd->end();

	tabCmds = new Fl_Group(2, 30, 475, 222, _("Cmds"));
		tabCmds->hide();
		bx1 = new Fl_Box(15, 32, 80, 22, "Label");
		bx1->box(FL_FLAT_BOX);
		bx1->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
		bx2 = new Fl_Box(100, 32, 370, 22, "Command");
		bx2->box(FL_FLAT_BOX);
		bx2->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);

		cmdlbl1 = new Fl_Input2(15, 54, 80, 20, "1");
		cmdlbl1->align(FL_ALIGN_LEFT);
		cmdlbl1->value(progStatus.label1.c_str());
		cmdlbl1->callback((Fl_Callback*)cb_cmdlbl, (void *)1);

		cmdtext1 = new Fl_Input2(100, 54, 370, 20, "");
		cmdtext1->align(FL_ALIGN_LEFT);
		cmdtext1->value(progStatus.command1.c_str());
		cmdtext1->callback((Fl_Callback*)cb_cmdtext, (void *)1);

		cmdlbl2 = new Fl_Input2(15, 74, 80, 20, "2");
		cmdlbl2->align(FL_ALIGN_LEFT);
		cmdlbl2->value(progStatus.label2.c_str());
		cmdlbl2->callback((Fl_Callback*)cb_cmdlbl, (void *)2);

		cmdtext2 = new Fl_Input2(100, 74, 370, 20, "");
		cmdtext2->align(FL_ALIGN_LEFT);
		cmdtext2->value(progStatus.command2.c_str());
		cmdtext2->callback((Fl_Callback*)cb_cmdtext, (void *)2);

		cmdlbl3 = new Fl_Input2(15, 94, 80, 20, "3");
		cmdlbl3->align(FL_ALIGN_LEFT);
		cmdlbl3->value(progStatus.label3.c_str());
		cmdlbl3->callback((Fl_Callback*)cb_cmdlbl, (void *)3);

		cmdtext3 = new Fl_Input2(100, 94, 370, 20, "");
		cmdtext3->align(FL_ALIGN_LEFT);
		cmdtext3->value(progStatus.command3.c_str());
		cmdtext3->callback((Fl_Callback*)cb_cmdtext, (void *)3);

		cmdlbl4 = new Fl_Input2(15, 114, 80, 20, "4");
		cmdlbl4->align(FL_ALIGN_LEFT);
		cmdlbl4->value(progStatus.label4.c_str());
		cmdlbl4->callback((Fl_Callback*)cb_cmdlbl, (void *)4);

		cmdtext4 = new Fl_Input2(100, 114, 370, 20, "");
		cmdtext4->align(FL_ALIGN_LEFT);
		cmdtext4->value(progStatus.command4.c_str());
		cmdtext4->callback((Fl_Callback*)cb_cmdtext, (void *)4);

		cmdlbl5 = new Fl_Input2(15, 134, 80, 20, "5");
		cmdlbl5->align(FL_ALIGN_LEFT);
		cmdlbl5->value(progStatus.label5.c_str());
		cmdlbl5->callback((Fl_Callback*)cb_cmdlbl, (void *)5);

		cmdtext5 = new Fl_Input2(100, 134, 370, 20, "");
		cmdtext5->align(FL_ALIGN_LEFT);
		cmdtext5->value(progStatus.command5.c_str());
		cmdtext5->callback((Fl_Callback*)cb_cmdtext, (void *)5);

		cmdlbl6 = new Fl_Input2(15, 154, 80, 20, "6");
		cmdlbl6->align(FL_ALIGN_LEFT);
		cmdlbl6->value(progStatus.label6.c_str());
		cmdlbl6->callback((Fl_Callback*)cb_cmdlbl, (void *)6);

		cmdtext6 = new Fl_Input2(100, 154, 370, 20, "");
		cmdtext6->align(FL_ALIGN_LEFT);
		cmdtext6->value(progStatus.command6.c_str());
		cmdtext6->callback((Fl_Callback*)cb_cmdtext, (void *)6);

		cmdlbl7 = new Fl_Input2(15, 174, 80, 20, "7");
		cmdlbl7->align(FL_ALIGN_LEFT);
		cmdlbl7->value(progStatus.label7.c_str());
		cmdlbl7->callback((Fl_Callback*)cb_cmdlbl, (void *)7);

		cmdtext7 = new Fl_Input2(100, 174, 370, 20, "");
		cmdtext7->align(FL_ALIGN_LEFT);
		cmdtext7->value(progStatus.command7.c_str());
		cmdtext7->callback((Fl_Callback*)cb_cmdtext, (void *)7);

		cmdlbl8 = new Fl_Input2(15, 194, 80, 20, "8");
		cmdlbl8->align(FL_ALIGN_LEFT);
		cmdlbl8->value(progStatus.label8.c_str());
		cmdlbl8->callback((Fl_Callback*)cb_cmdlbl, (void *)8);

		cmdtext8 = new Fl_Input2(100, 194, 370, 20, "");
		cmdtext8->align(FL_ALIGN_LEFT);
		cmdtext8->value(progStatus.command8.c_str());
		cmdtext8->callback((Fl_Callback*)cb_cmdtext, (void *)8);

		cmdResponse = new Fl_Output(100, 218, 370, 20, "Response:");
		cmdResponse->align(FL_ALIGN_LEFT);

	tabCmds->end();

	tabRestore = new Fl_Group(2, 30, 475, 222, _("Restore"));  // 470 width 180 height
		tabRestore->hide();

		Fl_Box *restore_box = new Fl_Box(10, 35, 455, 30, 
			_("Read / Restore these parameters"));
		restore_box->box(FL_ENGRAVED_FRAME);

		btnRestoreFrequency = new Fl_Check_Button(10, 70, 20, 20, _("Freq"));
		btnRestoreFrequency->tooltip(_("Restore frequency"));
		btnRestoreFrequency->down_box(FL_DOWN_BOX);
		btnRestoreFrequency->callback((Fl_Callback*)cb_restore);
		btnRestoreFrequency->align(Fl_Align(FL_ALIGN_RIGHT));
		btnRestoreFrequency->value(progStatus.restore_frequency == 1);

		btnRestoreMode = new Fl_Check_Button(10, 95, 20, 20, _("Mode"));
		btnRestoreMode->tooltip(_("Restore Mode"));
		btnRestoreMode->down_box(FL_DOWN_BOX);
		btnRestoreMode->callback((Fl_Callback*)cb_restore);
		btnRestoreMode->align(Fl_Align(FL_ALIGN_RIGHT));
		btnRestoreMode->value(progStatus.restore_mode == 1);

		btnRestoreBandwidth = new Fl_Check_Button(10, 120, 20, 20, _("Bandwidth"));
		btnRestoreBandwidth->tooltip(_("Restore bandwidth"));
		btnRestoreBandwidth->down_box(FL_DOWN_BOX);
		btnRestoreBandwidth->callback((Fl_Callback*)cb_restore);
		btnRestoreBandwidth->align(Fl_Align(FL_ALIGN_RIGHT));
		btnRestoreBandwidth->value(progStatus.restore_bandwidth == 1);

		btnRestoreVolume = new Fl_Check_Button(10, 145, 20, 20, _("Volume"));
		btnRestoreVolume->tooltip(_("Restore volume control"));
		btnRestoreVolume->down_box(FL_DOWN_BOX);
		btnRestoreVolume->callback((Fl_Callback*)cb_restore);
		btnRestoreVolume->align(Fl_Align(FL_ALIGN_RIGHT));
		btnRestoreVolume->value(progStatus.restore_volume == 1);

		btnRestoreMicGain = new Fl_Check_Button(10, 170, 20, 20, _("Mic gain"));
		btnRestoreMicGain->tooltip(_("Restore mic gain"));
		btnRestoreMicGain->down_box(FL_DOWN_BOX);
		btnRestoreMicGain->callback((Fl_Callback*)cb_restore);
		btnRestoreMicGain->align(Fl_Align(FL_ALIGN_RIGHT));
		btnRestoreMicGain->value(progStatus.restore_mic_gain == 1);

		btnRestoreRfGain = new Fl_Check_Button(10, 195, 20, 20, _("RF gain"));
		btnRestoreRfGain->tooltip(_("Restore RF gain"));
		btnRestoreRfGain->down_box(FL_DOWN_BOX);
		btnRestoreRfGain->callback((Fl_Callback*)cb_restore);
		btnRestoreRfGain->align(Fl_Align(FL_ALIGN_RIGHT));
		btnRestoreRfGain->value(progStatus.restore_rf_gain == 1);

		btnRestorePowerControl = new Fl_Check_Button(10, 220, 20, 20, _("Pwr level"));
		btnRestorePowerControl->tooltip(_("Restore power control"));
		btnRestorePowerControl->down_box(FL_DOWN_BOX);
		btnRestorePowerControl->callback((Fl_Callback*)cb_restore);
		btnRestorePowerControl->align(Fl_Align(FL_ALIGN_RIGHT));
		btnRestorePowerControl->value(progStatus.restore_power_control == 1);

		btnRestoreIFshift = new Fl_Check_Button(130, 70, 20, 20, _("IFshift"));
		btnRestoreIFshift->tooltip(_("Restore IF shift control"));
		btnRestoreIFshift->down_box(FL_DOWN_BOX);
		btnRestoreIFshift->callback((Fl_Callback*)cb_restore);
		btnRestoreIFshift->align(Fl_Align(FL_ALIGN_RIGHT));
		btnRestoreIFshift->value(progStatus.restore_if_shift == 1);

		btnRestoreNotch = new Fl_Check_Button(130, 95, 20, 20, _("Notch"));
		btnRestoreNotch->tooltip(_("Restore notch control"));
		btnRestoreNotch->down_box(FL_DOWN_BOX);
		btnRestoreNotch->callback((Fl_Callback*)cb_restore);
		btnRestoreNotch->align(Fl_Align(FL_ALIGN_RIGHT));
		btnRestoreNotch->value(progStatus.restore_notch == 1);

		btnRestoreAutoNotch = new Fl_Check_Button(130, 120, 20, 20, _("Auto Ntch"));
		btnRestoreAutoNotch->tooltip(_("Restore auto notch setting"));
		btnRestoreAutoNotch->down_box(FL_DOWN_BOX);
		btnRestoreAutoNotch->callback((Fl_Callback*)cb_restore);
		btnRestoreAutoNotch->align(Fl_Align(FL_ALIGN_RIGHT));
		btnRestoreAutoNotch->value(progStatus.restore_auto_notch == 1);

		btnRestoreSquelch = new Fl_Check_Button(130, 145, 20, 20, _("Squelch"));
		btnRestoreSquelch->tooltip(_("Restore squelch"));
		btnRestoreSquelch->down_box(FL_DOWN_BOX);
		btnRestoreSquelch->callback((Fl_Callback*)cb_restore);
		btnRestoreSquelch->align(Fl_Align(FL_ALIGN_RIGHT));
		btnRestoreSquelch->value(progStatus.restore_squelch == 1);

		btnRestoreSplit = new Fl_Check_Button(130, 170, 20, 20, _("Split"));
		btnRestoreSplit->tooltip(_("Restore split"));
		btnRestoreSplit->down_box(FL_DOWN_BOX);
		btnRestoreSplit->callback((Fl_Callback*)cb_restore);
		btnRestoreSplit->align(Fl_Align(FL_ALIGN_RIGHT));
		btnRestoreSplit->value(progStatus.restore_split == 1);

		btnRestorePreAtt = new Fl_Check_Button(130, 195, 20, 20, _("Pre/Att"));
		btnRestorePreAtt->tooltip(_("Restore Pre/Att"));
		btnRestorePreAtt->down_box(FL_DOWN_BOX);
		btnRestorePreAtt->callback((Fl_Callback*)cb_restore);
		btnRestorePreAtt->align(Fl_Align(FL_ALIGN_RIGHT));
		btnRestorePreAtt->value(progStatus.restore_pre_att == 1);

		btnRestoreNoise = new Fl_Check_Button(130, 220, 20, 20, _("Blanker"));
		btnRestoreNoise->tooltip(_("Restore noise blanker control"));
		btnRestoreNoise->down_box(FL_DOWN_BOX);
		btnRestoreNoise->callback((Fl_Callback*)cb_restore);
		btnRestoreNoise->align(Fl_Align(FL_ALIGN_RIGHT));
		btnRestoreNoise->value(progStatus.restore_noise == 1);

		btnRestoreNR = new Fl_Check_Button(250, 70, 20, 20, _("Noise Red'"));
		btnRestoreNR->tooltip(_("Restore noise reduction"));
		btnRestoreNR->down_box(FL_DOWN_BOX);
		btnRestoreNR->callback((Fl_Callback*)cb_restore);
		btnRestoreNR->align(Fl_Align(FL_ALIGN_RIGHT));
		btnRestoreNR->value(progStatus.restore_nr == 1);

		btnRestoreCompOnOff = new Fl_Check_Button(250, 95, 20, 20, _("Comp On/Off"));
		btnRestoreCompOnOff->tooltip(_("Restore Comp On/Off"));
		btnRestoreCompOnOff->down_box(FL_DOWN_BOX);
		btnRestoreCompOnOff->callback((Fl_Callback*)cb_restore);
		btnRestoreCompOnOff->align(Fl_Align(FL_ALIGN_RIGHT));
		btnRestoreCompOnOff->value(progStatus.restore_comp_on_off == 1);

		btnRestoreCompLevel = new Fl_Check_Button(250, 120, 20, 20, _("Comp Level"));
		btnRestoreCompLevel->tooltip(_("Restore comp level"));
		btnRestoreCompLevel->down_box(FL_DOWN_BOX);
		btnRestoreCompLevel->callback((Fl_Callback*)cb_restore);
		btnRestoreCompLevel->align(Fl_Align(FL_ALIGN_RIGHT));
		btnRestoreCompLevel->value(progStatus.restore_comp_level == 1);

		btnUseRigData = new Fl_Check_Button(250, 220, 20, 20, _("Use xcvr data"));
		btnUseRigData->tooltip(_("Set flrig to xcvr values at startup"));
		btnUseRigData->down_box(FL_DOWN_BOX);
		btnUseRigData->callback((Fl_Callback*)cb_restore);
		btnUseRigData->align(Fl_Align(FL_ALIGN_RIGHT));
		btnUseRigData->value(progStatus.use_rig_data == 1);

	tabRestore->end();

	tabsConfig->end();

	btnCloseCommConfig = new Fl_Button(w->w() - 52, 2, 50, 25, _("Close"));
	btnCloseCommConfig->callback((Fl_Callback*)cbCloseXcvrDialog);

w->end();

return w;

}

