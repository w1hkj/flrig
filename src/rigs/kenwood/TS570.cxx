// ----------------------------------------------------------------------------
// Copyright (C) 2021
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
// aunsigned long int with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include "kenwood/TS570.h"
#include "support.h"

static const char TS570name_[] = "TS-570";

static const char *TS570modes_[] = {
		"LSB", "USB", "CW", "FM", "AM", "FSK", "CW-R", "FSK-R", NULL};
static const char TS570_mode_chr[] =  { '1', '2', '3', '4', '5', '6', '7', '9' };
static const char TS570_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'L', 'L', 'U' };

static const char *TS570_SSBwidths[] = { // same for AM and FM
"NARR", "WIDE", NULL};
static int TS570_SSB_bw_vals[] = { 1,2, WVALS_LIMIT};

static const char *TS570_SSBbw[] = {
"FW0000;", "FW0001;", NULL};

static const char *TS570_CWwidths[] = {
"50", "100", "200", "300", "400", "600", "1000", "2000", NULL};
static int TS570_CW_bw_vals[] = { 1,2,3,4,5,6,7,8, WVALS_LIMIT};

static const char *TS570_CWbw[] = {
"FW0050;", "FW0100;", "FW0200;", "FW0300;",
"FW0400;", "FW0600;", "FW1000;", "FW2000;", NULL};

static const char *TS570_FSKwidths[] = {
"250", "500", "1000", "1500", NULL};
static int TS570_FSK_bw_vals[] = { 1,2,3,4, WVALS_LIMIT};

static const char *TS570_FSKbw[] = {
  "FW0250;", "FW0500;", "FW1000;", "FW1500;", NULL};

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol, 2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME, 54, 125, 156 },
	{ (Fl_Widget *)sldrRFGAIN, 266, 145, 156 },
	{ (Fl_Widget *)sldrPOWER, 54, 165, 368 },
	{ (Fl_Widget *)btnIFsh, 214, 105,  50 },
	{ (Fl_Widget *)sldrIFSHIFT, 266, 105, 156 },
	{ (Fl_Widget *)sldrSQUELCH, 266, 125, 156 },
	{ (Fl_Widget *)sldrMICGAIN, 54, 145, 156 },
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

void RIG_TS570::initialize()
{
	rig_widgets[0].W = btnVol;
	rig_widgets[1].W = sldrVOLUME;
	rig_widgets[2].W = sldrRFGAIN;
	rig_widgets[3].W = sldrPOWER;
	rig_widgets[4].W = btnIFsh;
	rig_widgets[5].W = sldrIFSHIFT;
	rig_widgets[6].W = sldrSQUELCH;
	rig_widgets[7].W = sldrMICGAIN;

//	progStatus.gettrace = progStatus.settrace = true;

	is_TS570 = get_ts570id();

	if (is_TS570) {
		cmd = "FR0;"; sendCommand(cmd);
		showresp(WARN, ASC, "Rx on A", cmd, "");
		cmd = "AC001;"; sendCommand(cmd);
		showresp(WARN, ASC, "Thru - tune ON", cmd, "");
		get_preamp();
		get_attenuator();
	}
}

RIG_TS570::RIG_TS570() {
// base class values
	name_ = TS570name_;
	modes_ = TS570modes_;
	bandwidths_ = TS570_SSBwidths;
	bw_vals_ = TS570_SSB_bw_vals;

	widgets = rig_widgets;

	comm_baudrate = BR4800;
	stopbits = 2;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_rtscts = true;
	comm_rtsplus = false;
	comm_dtrplus = false;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;

	A.freq = 14070000;
	A.imode = 1;
	A.iBW = 1;
	B.freq = 7035000;
	B.imode = 1;
	B.iBW = 1;
	can_change_alt_vfo = true;

	has_notch_control = false;

	has_smeter =
	has_power_out =
	has_split = has_split_AB =
	has_swr_control =
	has_ifshift_control =
	has_noise_control =
	has_micgain_control =
	has_volume_control =
	has_power_control =
	has_tune_control =
	has_attenuator_control =
	has_preamp_control =
	has_mode_control =
	has_bandwidth_control =
	has_rf_control =
	has_sql_control =
	has_ptt_control = true;

	precision = 1;
	ndigits = 8;

}

// Test for connection to a TS590nn transceiver
// repeat test 20 times to provide autobaud detection
// in the event an adapter is expecting to detect the pc baud rate
//
// TS590 identifier std::strings:
// TS590D: ID017;
// TS590S: ID018;

bool RIG_TS570::get_ts570id()
{
	int ret;
	size_t p;
	cmd = "ID;";
	for (int i = 0; i < 20; i++) {
		get_trace(1, "get_ts570id()");
		ret = wait_char(';', 6, 100, "get_ID()", ASC);
		gett("");
		if (ret < 6) {
			MilliSleep(10);
			Fl::awake();
			continue;
		}
		p = replystr.rfind("ID");  // expect either ID017; or ID018;
		if (replystr.substr(p, 6) == "ID017;" ||
			replystr.substr(p, 6) == "ID018;" )
			return true;
	}
	LOG_ERROR("get_ts570id() failed");
	return false;
}

void RIG_TS570::selectA()
{
	cmd = "FR0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "select A", cmd, "");
	inuse = onA;
}

void RIG_TS570::selectB()
{
	cmd = "FR1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "select B", cmd, "");
	inuse = onB;
}

void RIG_TS570::set_split(bool val)
{
	split = val;
	if (inuse == onB) {
		if (val) {
			cmd = "FR1;FT0;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on B, Tx on A", cmd, "");
		} else {
			cmd = "FR1;FT1;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on B, Tx on B", cmd, "");
		}
	} else {
		if (val) {
			cmd = "FR0;FT1;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on A, Tx on B", cmd, "");
		} else {
			cmd = "FR0;FT0;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on A, Tx on A", cmd, "");
		}
	}
	Fl::awake(highlight_vfo, (void *)0);
}

int RIG_TS570::get_split()
{
	cmd = "IF;";
	get_trace(1, "get_split()");
	int n = wait_char(';', 38, 100, "get IF", ASC);
	gett("");

	if (n < 38) return split;
	size_t p = replystr.rfind("IF");
	if (p == std::string::npos) return split;
	split = replystr[p+32] ? true : false;
	return split;
}

bool RIG_TS570::check ()
{
	return is_TS570;
}

unsigned long int RIG_TS570::get_vfoA ()
{
	cmd = "FA;";
	get_trace(1, "get_vfoA()");
	int n = wait_char(';', 14, 100, "get vfoA", ASC);
	gett("");

	if (n < 14) return A.freq;
	size_t p = replystr.rfind("FA");
	if (p == std::string::npos) return A.freq;

	int f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replystr[p + n] - '0';
	A.freq = f;
	return A.freq;
}

void RIG_TS570::set_vfoA (unsigned long int freq)
{
	A.freq = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set vfoA", cmd, "");
}

unsigned long int RIG_TS570::get_vfoB ()
{
	cmd = "FB;";

	get_trace(1, "get_vfoB()");
	int n = wait_char(';', 14, 100, "get vfoB", ASC);
	gett("");

	if (n < 14) return freqB;
	size_t p = replystr.rfind("FB");
	if (p == std::string::npos) return freqB;

	int f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replystr[p + n] - '0';
	freqB = f;
	return freqB;
}

void RIG_TS570::set_vfoB (unsigned long int freq)
{
	freqB = freq;
	cmd = "FB00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set vfoB", cmd, "");
}

// SM cmd 0 ... 100 (rig values 0 ... 15)
//S meter   Response       
//  0       SM0000;
//  3       SM0003;
//  5       SM0005;
//  7       SM0007;
//  9       SM0009;
//+20       SM0011;
//+40       SM0013;

int RIG_TS570::get_smeter()
{
	cmd = "SM;";

	get_trace(1, "get_smeter()");
	int n = wait_char(';', 6, 100, "get smeter", ASC);
	gett("");

	if (n < 6) return 0;
	size_t p = replystr.rfind("SM");
	if (p == std::string::npos) return -1;

	size_t len = replystr.length();
	replystr[len - 1] = 0;

	int mtr = atoi(&replystr[p + 2]);
	if (mtr <= 9)
		mtr = mtr * 50 / 9;
	else
		mtr = 50 + (mtr - 9) * 50 / 6;
	if (mtr > 100) mtr = 100;

	return mtr;
}

// RM cmd 0 ... 100 (rig values 0 ... 8)
int RIG_TS570::get_swr()
{
	sendCommand("RM1;"); // select measurement #1
	cmd = "RM;"; // select measurement '1' (swr) and read meter

	get_trace(1, "get_swr()");
	int n = wait_char(';', 8, 100, "get swr", ASC);
	gett("");

	if (n < 8) return 0;
	size_t p = replystr.rfind("RM1");
	if (p == std::string::npos) return 0;

	replystr[p + 7] = 0;
	int mtr = atoi(&replystr[p + 3]);
	mtr = (mtr * 100) / 15;
	return mtr;
}

// power output measurement 0 ... 15
int RIG_TS570::get_power_out()
{
	cmd = "SM;";

	get_trace(1, "get_power_out()");
	int n = wait_char(';', 7, 100, "get pwr out", ASC);
	gett("");

	if (n < 7) return 0;
	size_t p = replystr.rfind("SM");
	if (p == std::string::npos) return 0;

	replystr[p + 6] = 0;
	int mtr = atoi(&replystr[p + 2]);
	mtr = (int)(0.34 + (((0.035*mtr - 0.407)*mtr + 5.074)*mtr));
	if (mtr < 1) mtr = 0;
	if (mtr > 100) mtr = 100;
	return mtr;
}

// (xcvr power level is in 5W increments)
void RIG_TS570::set_power_control(double val)
{
	int ival = (int)val;
	cmd = "PC000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ival % 10;
		ival /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set pwr", cmd, "");
}

double RIG_TS570::get_power_control()
{
	cmd = "PC;";

	get_trace(1, "get_power_control()");
	int n = wait_char(';', 6, 100, "get pwr ctl", ASC);
	gett("");

	if (n < 6) return 0;
	size_t p = replystr.rfind("PC");
	if (p == std::string::npos) return 0;

	replystr[p + 5] = 0;
	int mtr = atoi(&replystr[p + 2]);
	return mtr;
}

// Volume control return 0 ... 100  (rig values 0 ... 255)
int RIG_TS570::get_volume_control()
{
	cmd = "AG;";

	get_trace(1, "get_volume_control()");
	int n = wait_char(';', 6, 100, "get vol", ASC);
	gett("");

	if (n < 6) return 0;
	size_t p = replystr.rfind("AG");
	if (p == std::string::npos) return 0;

	replystr[p + 5] = 0;
	int val = atoi(&replystr[p + 2]);
	return (int)(val / 2.55);
}

void RIG_TS570::set_volume_control(int val)
{
	int ivol = (int)(val * 2.55);
	showresp(WARN, ASC, "set vol", cmd, "");
	cmd = "AG000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ivol % 10;
		ivol /= 10;
	}
	sendCommand(cmd);
}

void RIG_TS570::tune_rig()
{
	cmd = "AC 11;";
	sendCommand(cmd);
	showresp(WARN, ASC, "TUNE", cmd, "");
}

void RIG_TS570::set_attenuator(int val)
{
	att_on = val;
	if (val) cmd = "RA01;";
	else	 cmd = "RA00;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set Att", cmd, "");
}

int RIG_TS570::get_attenuator()
{
	cmd = "RA;";

	get_trace(1, "get_attenuator()");
	int n = wait_char(';', 5, 100, "get att", ASC);
	gett("");

	if (n < 5) return att_on;
	size_t p = replystr.rfind("RA");
	if (p == std::string::npos) return att_on;

	if (replystr[p + 2] == '0' &&
		replystr[p + 3] == '0')
		att_on = 0;
	else
		att_on = 1;
	return att_on;
}

void RIG_TS570::set_preamp(int val)
{
	preamp_on = val;
	if (val) cmd = "PA1;";
	else	 cmd = "PA0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set pre", cmd, "");
}

int RIG_TS570::get_preamp()
{
	cmd = "PA;";

	get_trace(1, "get_preamp()");
	int n = wait_char(';', 4, 100, "get pre", ASC);
	gett("");

	if (n < 4 ) return preamp_on;
	size_t p = replystr.rfind("PA");
	if (p == std::string::npos) return preamp_on;

	if (replystr[p + 2] == '1')
		preamp_on = 1;
	else
		preamp_on = 0;
	return preamp_on;
}

void RIG_TS570::set_widths()
{
	switch (A.imode) {
	case 0:
	case 1:
	case 3:
	case 4:
	bandwidths_ = TS570_SSBwidths;
	bw_vals_ = TS570_SSB_bw_vals;
	A.iBW = 1;
	break;
	case 2:
	case 6:
	bandwidths_ = TS570_CWwidths;
	bw_vals_ = TS570_CW_bw_vals;
	A.iBW = 5;
	break;
	case 5:
	case 7:
	bandwidths_ = TS570_FSKwidths;
	bw_vals_ = TS570_FSK_bw_vals;
	A.iBW = 2;
	break;
	default:
	break;
	}
}

const char **RIG_TS570::bwtable(int m)
{
	switch (m) {
		case 0:
		case 1:
		case 3:
		case 4:
			return TS570_SSBwidths;
			break;
		case 2:
		case 6:
			return TS570_CWwidths;
			break;
		case 5:
		case 7:
			return TS570_FSKwidths;
			break;
	}
	return TS570_SSBwidths;
}

void RIG_TS570::set_modeA(int val)
{
	A.imode = val;
	cmd = "MD";
	cmd += TS570_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "set modeA", cmd, "");
	set_widths();
}

int RIG_TS570::get_modeA()
{
	cmd = "MD;";

	get_trace(1, "get_modeA()");
	int n = wait_char(';', 4, 100, "get modeA", ASC);
	gett("");

	if (n < 4) return A.imode;
	size_t p = replystr.rfind("MD");
	if (p == std::string::npos) return A.imode;

	int md = replystr[p + 2];
	md = md - '1';
	if (md == 8) md = 7;
	A.imode = md;
	set_widths();
	return A.imode;
}

void RIG_TS570::set_modeB(int val)
{
	B.imode = val;
	cmd = "MD";
	cmd += TS570_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "set modeB", cmd, "");
	set_widths();
}

int RIG_TS570::get_modeB()
{
	cmd = "MD;";

	get_trace(1, "get_modeB()");
	int n = wait_char(';', 4, 100, "get modeB", ASC);
	gett("");

	if (n < 4) return B.imode;
	size_t p = replystr.rfind("MD");
	if (p == std::string::npos) return B.imode;

	int md = replystr[p + 2];
	md = md - '1';
	if (md == 8) md = 7;
	B.imode = md;
	set_widths();
	return B.imode;
}

int RIG_TS570::adjust_bandwidth(int val)
{
	switch (val) {
	case 0:
	case 1:
	case 3:
	case 4:
		bandwidths_ = TS570_SSBwidths;
		bw_vals_ = TS570_SSB_bw_vals;
		return 1;
	case 2:
	case 6:
		bandwidths_ = TS570_CWwidths;
		bw_vals_ = TS570_CW_bw_vals;
		return 5;
	case 5:
	case 7:
		bandwidths_ = TS570_FSKwidths;
		bw_vals_ = TS570_FSK_bw_vals;
		return 2;
	}
	return 1;
}

int RIG_TS570::def_bandwidth(int val)
{
	switch (val) {
	case 0:
	case 1:
	case 3:
	case 4:
		return 1;
	case 2:
	case 6:
		return 5;
	case 5:
	case 7:
		return 2;
	}
	return 2;
}

void RIG_TS570::set_bwA(int val)
{
	A.iBW = val;

	switch (A.imode) {
	case 0:
	case 1:
	case 3:
	case 4:
		cmd = TS570_SSBbw[A.iBW];
		sendCommand(cmd, 0);
		break;
	case 2:
	case 6:
		cmd = TS570_CWbw[A.iBW];
		sendCommand(cmd, 0);
		break;
	case 5:
	case 7:
		cmd = TS570_FSKbw[A.iBW];
		sendCommand(cmd, 0);
		break;
	default:
		break;
	}
	showresp(WARN, ASC, "set bwA", cmd, "");
}

int RIG_TS570::get_bwA()
{
	int i;

	cmd = "FW;";

	get_trace(1, "get_bwA()");
	int n = wait_char(';', 7, 100, "get bwA", ASC);
	gett("");

	if (n < 7) return A.iBW;
	size_t p = replystr.rfind("FW");
	if (p == std::string::npos) return A.iBW;

	std::string test = replystr.substr(p, 7);

	switch (A.imode) {
	case 0:
	case 1:
	case 3:
	case 4:
		for (i = 0; TS570_SSBbw[i] != NULL; i++)
			if (test == TS570_SSBbw[i])  break;
		if (TS570_SSBbw[i] != NULL) A.iBW = i;
		else A.iBW = 1;
		break;
	case 2:
	case 6:
		for (i = 0; TS570_CWbw[i] != NULL; i++)
			if (test == TS570_CWbw[i])  break;
		if (TS570_CWbw[i] != NULL) A.iBW = i;
		else A.iBW = 1;
		break;
	case 5:
	case 7:
		for (i = 0; TS570_FSKbw[i] != NULL; i++)
			if (test == TS570_FSKbw[i])  break;
		if (TS570_FSKbw[i] != NULL) A.iBW = i;
		else A.iBW = 1;
		break;
	default:
	break;
	}

	return A.iBW;
}

void RIG_TS570::set_bwB(int val)
{
	B.iBW = val;

	switch (B.imode) {
	case 0:
	case 1:
	case 3:
	case 4:
		cmd = TS570_SSBbw[B.iBW];
		sendCommand(cmd, 0);
		break;
	case 2:
	case 6:
		cmd = TS570_CWbw[B.iBW];
		sendCommand(cmd, 0);
		break;
	case 5:
	case 7:
		cmd = TS570_FSKbw[B.iBW];
		sendCommand(cmd, 0);
		break;
	default:
		break;
	}
	showresp(WARN, ASC, "set bwB", cmd, "");
}

int RIG_TS570::get_bwB()
{
	int i;

	cmd = "FW;";

	get_trace(1, "get_bwB()");
	int n = wait_char(';', 7, 100, "get bwB", ASC);
	gett("");

	if (n < 7) return B.iBW;
	size_t p = replystr.rfind("FW");
	if (p == std::string::npos) return B.iBW;

	std::string test = replystr.substr(p,7);

	switch (B.imode) {
	case 0:
	case 1:
	case 3:
	case 4:
		for (i = 0; TS570_SSBbw[i] != NULL; i++)
			if (test == TS570_SSBbw[i])  break;
		if (TS570_SSBbw[i] != NULL) B.iBW = i;
		else B.iBW = 1;
		break;
	case 2:
	case 6:
		for (i = 0; TS570_CWbw[i] != NULL; i++)
			if (test == TS570_CWbw[i])  break;
		if (TS570_CWbw[i] != NULL) B.iBW = i;
		else B.iBW = 1;
		break;
	case 5:
	case 7:
		for (i = 0; TS570_FSKbw[i] != NULL; i++)
			if (test == TS570_FSKbw[i])  break;
		if (TS570_FSKbw[i] != NULL) B.iBW = i;
		else B.iBW = 1;
		break;
	default:
	break;
	}

	return B.iBW;
}

int RIG_TS570::get_modetype(int n)
{
	return TS570_mode_type[n];
}

// val 0 .. 100
void RIG_TS570::set_mic_gain(int val)
{
	cmd = "MG000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set mic", cmd, "");
}

int RIG_TS570::get_mic_gain()
{
	cmd = "MG;";

	get_trace(1, "get_mic_gain()");
	int n = wait_char(';', 6, 100, "get mic", ASC);
	gett("");

	if (n < 6) return 0;
	size_t p = replystr.rfind("MG");
	if (p == std::string::npos) return 0;

	replystr[p + 5] = 0;
	int val = atoi(&replystr[p + 2]);
	return val;
}

void RIG_TS570::get_mic_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_TS570::set_noise(bool b)
{
	if (b)
		cmd = "NB1;";
	else
		cmd = "NB0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set NB", cmd, "");
}

int  RIG_TS570::get_noise()
{
	cmd = "NB;";

	get_trace(1, "get_noise()");
	int n = wait_char(';', 4, 100, "get NB", ASC);
	gett("");

	if (n < 4) return 0;
	size_t p = replystr.rfind("NB");
	if (p == std::string::npos) return 0;

	return (replystr[p + 2] == '1');
}

//======================================================================
// IF shift command
// step size is 100 Hz
//======================================================================
void RIG_TS570::set_if_shift(int val)
{
	cmd = "IS+0000;";
	if (val < 0) cmd[2] = '-';
	val = abs(val);
	for (int i = 6; i > 2; i--) {
		cmd[i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd, 0);
	showresp(WARN, ASC, "set IF shift", cmd, "");
}

bool RIG_TS570::get_if_shift(int &val)
{
	cmd = "IS;";

	get_trace(1, "get_if_shift()");
	int n = wait_char(';', 8, 100, "get IFsh", ASC);
	gett("");

	val = 0;
	if (n < 8) {
		return false;
	}
	size_t p = replystr.rfind("IS");
	if (p == std::string::npos) {
		return false;
	}
	replystr[p + 7] = 0;
	val = atoi(&replystr[p + 3]);
	if (replystr[p+2] == '-') val = -val;
	return true;
}

void RIG_TS570::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -1000;
	max = 1000;
	step = 100;
}

void RIG_TS570::set_rf_gain(int val)
{
	cmd = "RG";
	cmd.append(to_decimal(val,3)).append(";");
	sendCommand(cmd,0);
	showresp(WARN, ASC, "set rf gain", cmd, "");
}

int  RIG_TS570::get_rf_gain()
{
	int val = progStatus.rfgain;
	cmd = "RG;";

	get_trace(1, "get_rf_gain()");
	int n = wait_char(';', 6, 100, "get rf gain", ASC);
	gett("");

	if (n < 6) return val;
	size_t p = replystr.rfind("RG");
	if (p != std::string::npos)
		val = fm_decimal(replystr.substr(p+2), 3);
	return val;
}

void RIG_TS570::get_rf_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 255; step = 1;
}

void RIG_TS570::set_squelch(int val)
{
	cmd = "SQ";
	cmd.append(to_decimal(abs(val),3)).append(";");
	sendCommand(cmd,0);
	showresp(WARN, ASC, "set squelch", cmd, "");
}

int  RIG_TS570::get_squelch()
{
	cmd = "SQ;";

	get_trace(1, "get_squelch()");
	int n = wait_char(';', 6, 100, "get squelch", ASC);
	gett("");

	if (n < 6) return 0;
	size_t p = replystr.rfind("SQ");

	if (p == std::string::npos) return 0;

	replystr[p + 5] = 0;
	return atoi(&replystr[p + 2]);
}

void RIG_TS570::get_squelch_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 255; step = 1;
}

// Tranceiver PTT on/off
void RIG_TS570::set_PTT_control(int val)
{
	if (val) cmd = "TX;";
	else	 cmd = "RX;";
	sendCommand(cmd);
	showresp(WARN, ASC, "PTT", cmd, "");
}

/*
========================================================================
	frequency & mode data are contained in the IF; response
		IFaaaaaaaaaaaXXXXXbbbbbcdXeefghjklmmX;
		12345678901234567890123456789012345678
		01234567890123456789012345678901234567 byte #
		          1         2         3
		                            ^ position 28
		where:
			aaaaaaaaaaa => decimal value of vfo frequency
			bbbbb => rit/xit frequency
			c => rit off/on
			d => xit off/on
			e => memory channel
			f => tx/rx
			g => mode
			h => function
			j => scan off/on
			k => split off /on
			l => tone off /on
			m => tone number
			X => unused characters

========================================================================
*/

int RIG_TS570::get_PTT()
{
	cmd = "IF;";
	get_trace(1, "get_split()");
	int n = wait_char(';', 38, 100, "get IF", ASC);
	gett("");

	if (n < 38) return ptt_;
	ptt_ = (replystr[28] == '1');
	return ptt_;
}
