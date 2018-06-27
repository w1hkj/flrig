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

#include "TS570.h"
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

	cmd = "FR0;"; sendCommand(cmd);
	showresp(WARN, ASC, "Rx on A", cmd, "");
	cmd = "AC001;"; sendCommand(cmd);
	showresp(WARN, ASC, "Thru - tune ON", cmd, "");
	get_preamp();
	get_attenuator();
	is_TS570S = get_ts570id();
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

bool RIG_TS570::get_ts570id()
{
	cmd = "ID;";
	if (wait_char(';', 6, 100, "get ID", ASC) < 6) return false;

	size_t p = replystr.rfind("ID");
	if (p == string::npos) return false;
	if (replystr[p + 3] == '1' && 
		replystr[p + 4] == '8')  return true;
	return false;
}

void RIG_TS570::selectA()
{
	cmd = "FR0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "select A", cmd, "");
}

void RIG_TS570::selectB()
{
	cmd = "FR1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "select B", cmd, "");
}

void RIG_TS570::set_split(bool val) 
{
	split = val;
	if (useB) {
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
	if (wait_char(';', 38, 100, "get IF", ASC) < 38) return split;

	size_t p = replystr.rfind("IF");
	if (p == string::npos) return split;
	split = replystr[p+32] ? true : false;
	return split;
}

bool RIG_TS570::check ()
{
	cmd = "FA;";
	int ret = wait_char(';', 14, 100, "check", ASC);
	if (ret < 14) return false;
	return true;
}

long RIG_TS570::get_vfoA ()
{
	cmd = "FA;";
	if (wait_char(';', 14, 100, "get vfoA", ASC) < 14) return A.freq;

	size_t p = replystr.rfind("FA");
	if (p == string::npos) return A.freq;
	
	int f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replystr[p + n] - '0';
	A.freq = f;
	return A.freq;
}

void RIG_TS570::set_vfoA (long freq)
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

long RIG_TS570::get_vfoB ()
{
	cmd = "FB;";
	if (wait_char(';', 14, 100, "get vfoB", ASC) < 14) return freqB;

	size_t p = replystr.rfind("FB");
	if (p == string::npos) return freqB;
	
	int f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replystr[p + n] - '0';
	freqB = f;
	return freqB;
}

void RIG_TS570::set_vfoB (long freq)
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
int RIG_TS570::get_smeter()
{
	cmd = "SM;";
	if (wait_char(';', 7, 100, "get smeter", ASC) < 7) return 0;

	size_t p = replystr.rfind("SM");
	if (p == string::npos) return -1;

	replystr[p + 6] = 0;
	int mtr = atoi(&replystr[p + 2]);
	mtr = (mtr * 100) / 15;
	return mtr;
}

// RM cmd 0 ... 100 (rig values 0 ... 8)
int RIG_TS570::get_swr()
{
	sendCommand("RM1;"); // select measurement #1
	cmd = "RM;"; // select measurement '1' (swr) and read meter
	if (wait_char(';', 8, 100, "get swr", ASC) < 8) return 0;

	size_t p = replystr.rfind("RM1");
	if (p == string::npos) return 0;
	
	replystr[p + 7] = 0;
	int mtr = atoi(&replystr[p + 3]);
	mtr = (mtr * 100) / 15;
	return mtr;
}

// power output measurement 0 ... 15
int RIG_TS570::get_power_out()
{
	cmd = "SM;";
	if (wait_char(';', 7, 100, "get pwr out", ASC) < 7) return 0;

	size_t p = replystr.rfind("SM");
	if (p == string::npos) return 0;
	
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

int RIG_TS570::get_power_control()
{
	cmd = "PC;";
	if (wait_char(';', 6, 100, "get pwr ctl", ASC) < 6) return 0;

	size_t p = replystr.rfind("PC");
	if (p == string::npos) return 0;
	
	replystr[p + 5] = 0;
	int mtr = atoi(&replystr[p + 2]);
	return mtr;
}

// Volume control return 0 ... 100  (rig values 0 ... 255)
int RIG_TS570::get_volume_control()
{
	cmd = "AG;";
	if (wait_char(';', 6, 100, "get vol", ASC) < 6) return 0;

	size_t p = replystr.rfind("AG");
	if (p == string::npos) return 0;
	
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
	if (wait_char(';', 5, 100, "get att", ASC) < 5) return att_on;

	size_t p = replystr.rfind("RA");
	if (p == string::npos) return att_on;

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
	if (wait_char(';', 4, 100, "get pre", ASC) < 4 ) return preamp_on;

	size_t p = replystr.rfind("PA");
	if (p == string::npos) return preamp_on;

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
	if (wait_char(';', 4, 100, "get modeA", ASC) < 4) return A.imode;

	size_t p = replystr.rfind("MD");
	if (p == string::npos) return A.imode;

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
	if (wait_char(';', 4, 100, "get modeB", ASC) < 4) return B.imode;

	size_t p = replystr.rfind("MD");
	if (p == string::npos) return B.imode;

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
	if (wait_char(';', 7, 100, "get bwA", ASC) < 7) return A.iBW;

	size_t p = replystr.rfind("FW");
	if (p == string::npos) return A.iBW;

	string test = replystr.substr(p, 7);

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
	if (wait_char(';', 7, 100, "get bwB", ASC) < 7) return B.iBW;

	size_t p = replystr.rfind("FW");
	if (p == string::npos) return B.iBW;

	string test = replystr.substr(p,7);

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
	if (wait_char(';', 6, 100, "get mic", ASC) < 6) return 0;

	size_t p = replystr.rfind("MG");
	if (p == string::npos) return 0;

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
	if (wait_char(';', 4, 100, "get NB", ASC) < 4) return 0;

	size_t p = replystr.rfind("NB");
	if (p == string::npos) return 0;

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
	size_t p = 0;
	cmd = "IS;";
	if (wait_char(';', 8, 100, "get IFsh", ASC) >= 8) {
		p = replystr.rfind("IS");
		if (p == string::npos) return false;
		replystr[p + 7] = 0;
		val = atoi(&replystr[p + 3]);
		if (replystr[p+2] == '-') val = -val;
		return true;
	}
	val = 0;
	return false;
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
	if (wait_char(';', 6, 100, "get rf gain", ASC) < 6) return val;

	size_t p = replystr.rfind("RG");
	if (p != string::npos)
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
	if (wait_char(';', 6, 100, "get squelch", ASC) < 6) return 0;

	size_t p = replystr.rfind("SQ");

	if (p == string::npos) return 0;

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
	int ret = wait_char(';', 38, 100, "get VFO", ASC);
	if (ret < 38) return ptt_;
	ptt_ = (replybuff[28] == '1');
	return ptt_;
}
