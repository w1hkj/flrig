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
#include <sstream>

#include "FT450.h"
#include "rig.h"

static const char FT450name_[] = "FT-450";

#undef  NUM_MODES
#define NUM_MODES  11

enum mFT450 {
  mLSB, mUSB, mCW, mFM, mAM, mRTTY_L, mCW_R, mDATA_L, mRTTY_U, mFM_N, mDATA_U };
//  0,    1,   2,   3,   4,    5,       6,     7,       8,        9,    10		mode index

static int mode_bwA[NUM_MODES] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
static int mode_bwB[NUM_MODES] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

static const char *FT450modes_[] = {
		"LSB", "USB", "CW", "FM", "AM", "RTTY-L", "CW-R", "USER-L", "RTTY-U", "FM-N", "USER-U", NULL};
static const char mode_chr[] =  { '1', '2', '3', '4', '5', '6', '7', '8', '9', 'B', 'C' };
static const char mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'L', 'L', 'L', 'U', 'U', 'U' };

static const char *FT450_ssb_widths[]  = {"1800", "2400", "3000", NULL};
static int FT450_wvals_ssb_widths[] = {1, 2, 3, WVALS_LIMIT};

static const char *FT450_cw_widths[]   = {"300", "500", "2400", NULL};
static int FT450_wvals_cw_widths[] = {1, 2, 3, WVALS_LIMIT};

static const char *FT450_data_widths[] = {"300", "2400", "3000", NULL};
static int FT450_wvals_data_widths[] = {1, 2, 3, WVALS_LIMIT};

static const char *FT450_am_widths[]   = {"3000", "6000", "9000", NULL};
static int FT450_wvals_am_widths[] = {1, 2, 3, WVALS_LIMIT};

static const char *FT450_fm_widths[]   = {"2500", "5000", NULL};
static int FT450_wvals_fm_widths[] = {1, 2, WVALS_LIMIT};

static const int FT450_def_bw[] = {
  2, 2, 0, 1, 1, 0, 0, 1, 0, 1, 1 };

/*
static const char *FT450_US_60m[] = {NULL, "126", "127", "128", "130", NULL};
// US has 5 60M presets. Using dummy numbers for all.
// First NULL means skip 60m sets in get_band_selection().
// Maybe someone can do a cat command MC; on all 5 presets and add returned numbers above.
// To send cat commands in flrig goto menu Config->Xcvr select->Send Cmd.

static const char **Channels_60m = FT450_US_60m;
*/

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 },
	{ (Fl_Widget *)btnIFsh,     214, 105,  50 },
	{ (Fl_Widget *)sldrIFSHIFT, 266, 105, 156 },
	{ (Fl_Widget *)btnNotch,    214, 125,  50 },
	{ (Fl_Widget *)sldrNOTCH,   266, 125, 156 },
	{ (Fl_Widget *)sldrMICGAIN,  54, 145, 156 },
	{ (Fl_Widget *)sldrPOWER,    54, 165, 368 },
	{ (Fl_Widget *)btnNR,       214, 145,  50 },
	{ (Fl_Widget *)sldrNR,      266, 145, 156 },
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

RIG_FT450::RIG_FT450() {
// base class values
	IDstr = "ID";
	name_ = FT450name_;
	modes_ = FT450modes_;
	bandwidths_ = FT450_ssb_widths;
	bw_vals_ = FT450_wvals_ssb_widths;

	widgets = rig_widgets;

	comm_baudrate = BR38400;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_rtscts = true;
	comm_rtsplus = false;
	comm_dtrplus = false;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	modeA = 1;
	bwA = 2;
	def_mode = 10;
	def_bw = 2;
	def_freq = 14070000;

	precision = 10;
	ndigits = 8;

	has_a2b =
	can_change_alt_vfo =
	has_xcvr_auto_on_off =
	has_noise_reduction =
	has_noise_reduction_control =
	has_extras =
	has_vox_onoff =
	has_vox_gain =
	has_vox_hang =

	has_cw_wpm =
	has_cw_keyer =
//	has_cw_vol =
	has_cw_spot =
	has_cw_spot_tone =
	has_cw_qsk =
	has_cw_weight =

	has_split =
	has_smeter =
	has_swr_control =
	has_power_out =
	has_power_control =
	has_volume_control =
	has_mode_control =
	has_noise_control =
	has_band_selection =
	has_bandwidth_control =
	has_micgain_control =
	has_notch_control =
	has_attenuator_control =
	has_preamp_control =
	has_ifshift_control =
	has_ptt_control =
	has_tune_control = true;

// derived specific
	notch_on = false;
	m_60m_indx = 0;

	precision = 1;
	ndigits = 8;
}

void RIG_FT450::initialize()
{
	rig_widgets[0].W = btnVol;
	rig_widgets[1].W = sldrVOLUME;
	rig_widgets[2].W = btnIFsh;
	rig_widgets[3].W = sldrIFSHIFT;
	rig_widgets[4].W = btnNotch;
	rig_widgets[5].W = sldrNOTCH;
	rig_widgets[6].W = sldrMICGAIN;
	rig_widgets[7].W = sldrPOWER;
	rig_widgets[8].W = btnNR;
	rig_widgets[9].W = sldrNR;

// set progStatus defaults
	if (progStatus.noise_reduction_val < 1) progStatus.noise_reduction_val = 1;
// first-time-thru, or reset
	if (progStatus.cw_qsk < 15) {
		progStatus.cw_qsk = 15;
		progStatus.cw_spot_tone = 700;
		progStatus.cw_weight = 3.0;
		progStatus.cw_wpm = 18;
		progStatus.vox_gain = 50;
		progStatus.vox_hang = 500;
	}

	op_yaesu_select60->deactivate();

// turn off auto information mode
	sendCommand("AI0;");

	selectA();
}

void RIG_FT450::selectA()
{
	cmd = "VS0;";
	sendCommand(cmd);
}

void RIG_FT450::selectB()
{
	cmd = "VS1;";
	sendCommand(cmd);
}

void RIG_FT450::A2B()
{
	cmd = "VV;";
	sendCommand(cmd);
	showresp(WARN, ASC, "vfo A --> B", cmd, replystr);
}

bool RIG_FT450::check()
{
	cmd = rsp = "FA";
	cmd += ';';
	int ret = wait_char(';',11, 100, "check", ASC);

	rig_trace(2, "check()", replystr.c_str());

	if (ret >= 11) return true;
	return false;
}

long RIG_FT450::get_vfoA ()
{
	cmd = rsp = "FA";
	cmd += ';';
	waitN(11, 100, "get vfo A", ASC);

	rig_trace(2, "get_vfoA()", replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return freqA;
	int f = 0;
	for (size_t n = 2; n < 10; n++)
		f = f*10 + replystr[p+n] - '0';
	freqA = f;
	return freqA;
}

void RIG_FT450::set_vfoA (long freq)
{
	freqA = freq;
	cmd = "FA00000000;";
	for (int i = 9; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vfo A", cmd, replystr);
}

long RIG_FT450::get_vfoB ()
{
	cmd = rsp = "FB";
	cmd += ';';
	waitN(11, 100, "get vfo B", ASC);

	rig_trace(2, "get_vfoB()", replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return freqB;
	int f = 0;
	for (size_t n = 2; n < 10; n++)
		f = f*10 + replystr[p+n] - '0';
	freqB = f;
	return freqB;
}

void RIG_FT450::set_vfoB (long freq)
{
	freqB = freq;
	cmd = "FB00000000;";
	for (int i = 9; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vfo B", cmd, replystr);
}

void RIG_FT450::set_split(bool on)
{
	if (on) cmd = "FT1;";
	else cmd = "FT0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "SET split", cmd, replystr);
}

int RIG_FT450::get_split()
{
	size_t p;
	int split = 0;
	char tx;
// tx vfo
	cmd = rsp = "FT";
	cmd.append(";");
	wait_char(';',4, 100, "get split tx vfo", ASC);

	p = replystr.rfind(rsp);
	if (p == string::npos) return false;
	tx = replystr[p+2] - '0';

	split = (tx == 1 ? 2 : 0);

	return split;
}

int RIG_FT450::get_smeter()
{
	cmd = rsp = "SM0";
	cmd += ';';
	waitN(7, 100, "get smeter", ASC);

	rig_trace(2, "get_smeter()", replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	replystr[p+6] = 0;
	int mtr = atoi(&replystr[p+3]);
	mtr = mtr * 100.0 / 256.0;
	return mtr;
}

// measured by W3NR
//  SWR..... mtr ... display
//  6:1..... 255 ... 100
//  3:1..... 132 ...  50
//  2:1..... 066 ...  26
//  2.5:1... 099 ...  39
//  1.5:1... 033 ...  13
//  1.1:1... 008 ...   3

int RIG_FT450::get_swr()
{
	cmd = rsp = "RM6";
	cmd += ';';
	waitN(7, 100, "get swr", ASC);

	rig_trace(2, "get_swr()", replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	replystr[p+6] = 0;
	int mtr = atoi(&replystr[p+3]);
	return mtr / 2.55;
}


int RIG_FT450::get_power_out()
{
	cmd = rsp = "RM5";
	sendCommand(cmd.append(";"));
	waitN(7, 100, "get pout", ASC);

	rig_trace(2, "get_power_out()", replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	replystr[p+6] = 0;
	double mtr = (double)(atoi(&replystr[p+3]));
	mtr = -6.6263535 + .11813178 * mtr + .0013607405 * mtr * mtr;
	return (int)mtr;
}

int RIG_FT450::get_power_control()
{
	cmd = rsp = "PC";
	cmd += ';';
	waitN(6, 100, "get power", ASC);

	rig_trace(2, "get_power_control()", replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	replystr[p+5] = 0;
	int mtr = atoi(&replystr[p+2]);
	return mtr;
}

void RIG_FT450::set_power_control(double val)
{
	int ival = (int)val;
	cmd = "PC000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ival % 10;
		ival /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET power", cmd, replystr);
}

// Volume control return 0 ... 100
int RIG_FT450::get_volume_control()
{
	cmd = rsp = "AG0";
	cmd += ';';
	waitN(7, 100, "get vol", ASC);

	rig_trace(2, "get_volume_control()", replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.volume;
	if (p + 6 >= replystr.length()) return progStatus.volume;
	int val = atoi(&replystr[p+3]) * 100 / 250;
	if (val > 100) val = 100;
	return val;
}

void RIG_FT450::set_volume_control(int val) 
{
	int ivol = (int)(val * 250 / 100);
	cmd = "AG0000;";
	for (int i = 5; i > 2; i--) {
		cmd[i] += ivol % 10;
		ivol /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vol", cmd, replystr);
}

void RIG_FT450::get_vol_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 255; step = 1;
}

// Tranceiver PTT on/off
void RIG_FT450::set_PTT_control(int val)
{
	cmd = val ? "TX1;" : "TX0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "SET PTT", cmd, replystr);
	ptt_ = val;
}

int RIG_FT450::get_PTT()
{
	cmd = "TX;";
	rsp = "TX";
	waitN(4, 100, "get PTT", ASC);

	rig_trace(2, "get_PTT()", replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return ptt_;
	ptt_ =  (replystr[p+2] != '0' ? 1 : 0);
	return ptt_;
}

void RIG_FT450::tune_rig(int val)
{
	switch (val) {
		case 0:
			cmd = "AC000;";
			break;
		case 1:
			cmd = "AC001;";
			break;
		case 2:
		default:
			cmd = "AC002;";
			break;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "tune rig", cmd, replystr);
	sett("tune_rig");
}

int RIG_FT450::get_tune()
{
	cmd = rsp = "AC";
	cmd += ';';
	waitN(5, 100, "get tune", ASC);

	rig_trace(2, "get_tuner status()", replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	int val = replystr[p+4] - '0';
	return (val > 0);
}

void RIG_FT450::set_attenuator(int val)
{
	if (val) cmd = "RA01;";
	else	 cmd = "RA00;";
	sendCommand(cmd);
	showresp(WARN, ASC, "get att", cmd, replystr);
}

int RIG_FT450::get_attenuator()
{
	cmd = rsp = "RA0";
	cmd += ';';
	waitN(5, 100, "get att", ASC);

	rig_trace(2, "get_attenuator()", replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	return (replystr[p+3] == '3' ? 1 : 0);
}

void RIG_FT450::set_preamp(int val)
{
	if (val) cmd = "PA00;";
	else	 cmd = "PA01;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set preamp", cmd, replystr);
}

int RIG_FT450::get_preamp()
{
	cmd = rsp = "PA0";
	cmd += ';';
	waitN(5, 100, "get pre", ASC);

	rig_trace(2, "get_preamp()", replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	return (replystr[p+3] == '1' ? 1 : 0);
}

const char ** RIG_FT450::bwtable(int n)
{
	switch (n) {
		case mCW     :
		case mCW_R   :
			return FT450_cw_widths;
		case mFM     :
		case mFM_N   :
			return FT450_fm_widths;
		case mAM :
			return FT450_am_widths;
		case mRTTY_L :
		case mRTTY_U :
		case mDATA_L :
		case mDATA_U :
			return FT450_data_widths;
	}
	return FT450_ssb_widths;
}

void RIG_FT450::set_modeA(int val)
{
	modeA = val;
	cmd = "MD0";
	cmd += mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mode A", cmd, replystr);
	if (val == 2 || val == 6) return;
	if (progStatus.spot_onoff) {
		progStatus.spot_onoff = false;
		set_spot_onoff();
		cmd = "CS0;";
		sendCommand(cmd);
		showresp(WARN, ASC, "SET spot off", cmd, replystr);
		btnSpot->value(0);
	}
}

int RIG_FT450::get_modeA()
{
	cmd = rsp = "MD0";
	cmd += ';';
	waitN(5, 100, "get mode A", ASC);

	rig_trace(2, "get_modeA()", replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return modeA;
	int md = replystr[p+3];
	if (md <= '9') md = md - '1';
	else md = 9 + md - 'B';
	modeA = md;
	return modeA;
}

int RIG_FT450::adjust_bandwidth(int val)
{
	switch (val) {
		case mCW     :
		case mCW_R   :
			bandwidths_ = FT450_cw_widths;
			bw_vals_ = FT450_wvals_cw_widths;
			break;
		case mFM     :
		case mFM_N   :
			bandwidths_ = FT450_fm_widths;
			bw_vals_ = FT450_wvals_fm_widths;
			break;
		case mAM :
			bandwidths_ = FT450_am_widths;
			bw_vals_ = FT450_wvals_am_widths;
			break;
		case mRTTY_L :
		case mRTTY_U :
		case mDATA_L :
		case mDATA_U :
			bandwidths_ = FT450_data_widths;
			bw_vals_ = FT450_wvals_data_widths;
			break;
		default:
			bandwidths_ = FT450_ssb_widths;
			bw_vals_ = FT450_wvals_ssb_widths;
			break;
	}
	return FT450_def_bw[val];
}

int RIG_FT450::def_bandwidth(int m)
{
	int bw = adjust_bandwidth(m);
	if (useB) {
		if (mode_bwB[m] == -1)
			mode_bwB[m] = bw;
		return mode_bwB[m];
	}
	if (mode_bwA[m] == -1)
		mode_bwA[m] = bw;
	return mode_bwA[m];
}

void RIG_FT450::set_bwA(int val)
{
	bwA = val;
	switch (val) {
		case 0 : cmd = "SH000;"; break;
		case 1 : cmd = "SH016;"; break;
		case 2 : cmd = "SH031;"; break;
		default: cmd = "SH031;";
	}
	sendCommand(cmd);
	mode_bwA[modeA] = bwA;
	showresp(WARN, ASC, "SET bwA", cmd, replystr);
}

int RIG_FT450::get_bwA()
{
	cmd = rsp = "SH0";
	cmd += ';';
	waitN(6, 100, "get bw A", ASC);

	rig_trace(2, "get_bwA()", replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return bwA;
	string bws = replystr.substr(p+3,2);
	if (bws == "00") bwA = 0;
	else if (bws == "16") bwA = 1;
	else if (bws == "31") bwA = 2;
	mode_bwA[modeA] = bwA;
	return bwA;
}

void RIG_FT450::set_modeB(int val)
{
	modeB = val;
	cmd = "MD0";
	cmd += mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mode B", cmd, replystr);
	if (val == 2 || val == 6) return;
	if (progStatus.spot_onoff) {
		progStatus.spot_onoff = false;
		set_spot_onoff();
		cmd = "CS0;";
		sendCommand(cmd);
		showresp(WARN, ASC, "SET spot off", cmd, replystr);
		btnSpot->value(0);
	}
}

int RIG_FT450::get_modeB()
{
	cmd = rsp = "MD0";
	cmd += ';';
	waitN(5, 100, "get mode B", ASC);

	rig_trace(2, "get_modeB()", replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return modeB;
	int md = replystr[p+3];
	if (md <= '9') md = md - '1';
	else md = 9 + md - 'B';
	modeB = md;
	return modeB;
}

void RIG_FT450::set_bwB(int val)
{
	bwB = val;
	switch (val) {
		case 0 : cmd = "SH000;"; break;
		case 1 : cmd = "SH016;"; break;
		case 2 : cmd = "SH031;"; break;
		default: cmd = "SH031;";
	}
	sendCommand(cmd);
	mode_bwB[modeB] = bwB;
	showresp(WARN, ASC, "SET bwB", cmd, replystr);
}

int RIG_FT450::get_bwB()
{
	cmd = rsp = "SH0";
	cmd += ';';
	waitN(6, 100, "get bw B", ASC);

	rig_trace(2, "get_bwB()", replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return bwB;
	string bws = replystr.substr(p+3,2);
	if (bws == "00") bwB = 0;
	else if (bws == "16") bwB = 1;
	else if (bws == "31") bwB = 2;
	mode_bwB[modeB] = bwB;
	return bwB;
}

std::string RIG_FT450::get_BANDWIDTHS()
{
	stringstream s;
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_bwA[i] << " ";
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_bwB[i] << " ";
	return s.str();
}

void RIG_FT450::set_BANDWIDTHS(std::string s)
{
	stringstream strm;
	strm << s;
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_bwA[i];
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_bwB[i];
}

int RIG_FT450::get_modetype(int n)
{
	return mode_type[n];
}

void RIG_FT450::set_if_shift(int val)
{
	cmd = "IS0+0000;";
	if (val < 0) cmd[3] = '-';
	val = abs(val);
	for (int i = 4; i > 0; i--) {
		cmd[3+i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET ifshift", cmd, replystr);
}

bool RIG_FT450::get_if_shift(int &val)
{
	cmd = rsp = "IS0";
	cmd += ';';
	waitN(9, 100, "get if shift", ASC);

	rig_trace(2, "get_if_shift()", replystr.c_str());

	size_t p = replystr.rfind(rsp);
	val = progStatus.shift_val;
	if (p == string::npos) return progStatus.shift;
	val = atoi(&replystr[p+4]);
	if (replystr[p+3] == '-') val = -val;
	return (val != 0);
}

void RIG_FT450::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -1000;
	max = 1000;
	step = 100;
}

void RIG_FT450::set_notch(bool on, int val)
{
	cmd = "BP00000;";
	if (on == false) {
		sendCommand(cmd);
		showresp(WARN, ASC, "SET notch off", cmd, replystr);
		notch_on = false;
		return;
	}
	cmd[6] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET notch on", cmd, replystr);
	notch_on = true;

	cmd[3] = '1'; // manual NOTCH position
	cmd[6] = '0';
	val = val / 10 + 200;
	for (int i = 3; i > 0; i--) {
		cmd[3 + i] += val % 10;
		val /=10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET notch val", cmd, replystr);
}

bool  RIG_FT450::get_notch(int &val)
{
	bool ison = false;
	cmd = rsp = "BP00";
	cmd += ';';
	waitN(8, 100, "get notch on/off", ASC);

	rig_trace(2, "get_notch()", replystr.c_str());

	size_t p = replystr.rfind(rsp);
	val = progStatus.notch_val = 0; // disabled default slider position
	if (p == string::npos) return ison;

	if (replystr[p+6] == '1') { // manual notch enabled
		ison = true;
		val = progStatus.notch_val;
		cmd = rsp = "BP01";
		cmd += ';';
		waitN(8, 100, "get notch val", ASC);

		rig_trace(2, "get_notch_val()", replystr.c_str());

		p = replystr.rfind(rsp);
		if (p == string::npos) return ison;
		val = atoi(&replystr[p+4]);
		val = (val - 200) * 10;
	}
	return ison;
}

void RIG_FT450::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = -1990;
	max = +2000;
	step = 100;
}

void RIG_FT450::set_noise(bool b)
{
	if (b)
		cmd = "NB01;";
	else
		cmd = "NB00;";
	sendCommand (cmd);
	showresp(WARN, ASC, "SET NB", cmd, replystr);
}

void RIG_FT450::set_xcvr_auto_on()
{
// send dummy data request for ID (see pg 12 CAT reference book)
	cmd = "ID;";
	sendCommand(cmd);
// wait 1 to 2 seconds
	MilliSleep(1500);
	cmd = "PS1;";
	sendCommand(cmd);
// wait for power on status
	cmd = "PS;";
	waitN(4, 500, "Xcvr ON?", ASC);
}

void RIG_FT450::set_xcvr_auto_off()
{
	cmd = "PS0;";
	sendCommand(cmd);
}

// val 0 .. 100
void RIG_FT450::set_mic_gain(int val)
{
	cmd = "MG000;";
	val = (int)(val * 2.55); // convert to 0 .. 255
	for (int i = 3; i > 0; i--) {
		cmd[1+i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mic", cmd, replystr);
}

int RIG_FT450::get_mic_gain()
{
	cmd = rsp = "MG";
	cmd += ';';
	waitN(6, 100, "get mic", ASC);

	rig_trace(2, "get_mic_gain()", replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	replystr[p+5] = 0;
	return atoi(&replystr[p+2]);;
}

void RIG_FT450::get_mic_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_FT450::set_special(int v)
{
	if (v) cmd = "VR1;";
	else   cmd = "VR0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Set special", cmd, replystr);
}

void RIG_FT450::set_vox_onoff()
{
	cmd = "VX0;";
	if (progStatus.vox_onoff) cmd[2] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox on/off", cmd, replystr);
}

void RIG_FT450::set_vox_gain()
{
	cmd = "VG";
	cmd.append(to_decimal(progStatus.vox_gain, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox gain", cmd, replystr);
}

void RIG_FT450::set_vox_hang()
{
	cmd = "VD";
	cmd.append(to_decimal(progStatus.vox_hang, 4)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox delay", cmd, replystr);
}

void RIG_FT450::set_cw_wpm()
{
	cmd = "KS";
	if (progStatus.cw_wpm > 60) progStatus.cw_wpm = 60;
	if (progStatus.cw_wpm < 4) progStatus.cw_wpm = 4;
	cmd.append(to_decimal(progStatus.cw_wpm, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw wpm", cmd, replystr);
}


void RIG_FT450::enable_keyer()
{
	cmd = "KR0;";
	if (progStatus.enable_keyer) cmd[2] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET keyer on/off", cmd, replystr);
}

bool RIG_FT450::set_cw_spot()
{
	if (vfo->imode == 2 || vfo->imode == 6) {
		cmd = "CS0;";
		if (progStatus.spot_onoff) cmd[2] = '1';
		sendCommand(cmd);
		showresp(WARN, ASC, "SET spot on/off", cmd, replystr);
		return true;
	} else
		return false;
}

void RIG_FT450::set_cw_weight()
{
	int n = round(progStatus.cw_weight * 10);
	cmd.assign("EX024").append(to_decimal(n, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw weight", cmd, replystr);
}

void RIG_FT450::set_cw_qsk()
{
	int n = progStatus.cw_qsk / 5 - 3;
	cmd.assign("EX018").append(to_decimal(n, 1)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw qsk", cmd, replystr);
}

void RIG_FT450::set_cw_spot_tone()
{
	int n = (progStatus.cw_spot_tone - 400) / 50 + 1;
	cmd = "EX020";
	cmd.append(to_decimal(n, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw tone", cmd, replystr);
}

// DNR
void RIG_FT450::set_noise_reduction_val(int val)
{
	cmd.assign("RL0").append(to_decimal(val, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET_noise_reduction_val", cmd, replystr);
}

int  RIG_FT450::get_noise_reduction_val()
{
	int val = 1;
	cmd = rsp = "RL0";
	cmd.append(";");
	waitN(6, 100, "GET noise reduction val", ASC);

	rig_trace(2, "get_noise_reduction_val()", replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return val;
	val = atoi(&replystr[p+3]);
	return val;
}

// DNR
void RIG_FT450::set_noise_reduction(int val)
{
	cmd.assign("NR0").append(val ? "1" : "0" ).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET noise reduction", cmd, replystr);
}

int  RIG_FT450::get_noise_reduction()
{
	int val;
	cmd = rsp = "NR0";
	cmd.append(";");
	waitN(5, 100, "GET noise reduction", ASC);
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	val = replystr[p+3] - '0';
	return val;
}

void RIG_FT450::get_band_selection(int v)
{
	if (v < 3) v = v - 1;
	cmd.assign("BS").append(to_decimal(v, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "Select Band Stacks", cmd, replystr);
	set_trace(2, "get band", cmd.c_str());
}

/*
void RIG_FT450::get_band_selection(int v)
{
	int inc_60m = false;
	cmd = "IF;";
	waitN(27, 100, "get band", ASC);

	set_trace(2, "get band", replystr.c_str());

	size_t p = replystr.rfind("IF");
	if (p == string::npos) return;
	if (replystr[p+21] != '0') {	// vfo 60M memory mode
		inc_60m = true;
	}

	if (v == 12) {	// 5MHz 60m presets
		if (Channels_60m[0] == NULL) return;	// no 60m Channels so skip
		if (inc_60m) {
			if (Channels_60m[++m_60m_indx] == NULL)
				m_60m_indx = 0;
		}
		cmd.assign("MC").append(Channels_60m[m_60m_indx]).append(";");
	} else {		// v == 1..11 band selection OR return to vfo mode == 0
		if (inc_60m)
			cmd = "VM;";
		else {
			if (v < 3)
				v = v - 1;
			cmd.assign("BS").append(to_decimal(v, 2)).append(";");
		}
	}

	sendCommand(cmd);
	showresp(WARN, ASC, "Select Band Stacks", cmd, replystr);
}
*/
