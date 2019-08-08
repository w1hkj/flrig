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

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>

#include "FT450D.h"
#include "rig.h"

#define FL450D_WAIT_TIME 200

static const char FT450Dname_[] = "FT-450D";

enum mFT450D {
  mLSB, mUSB, mCW, mFM, mAM, mRTTY_L, mCW_R, mDATA_L, mRTTY_U, mFM_N, mDATA_U };
//  0,    1,   2,   3,   4,    5,       6,     7,       8,        9,    10		mode index

#undef  NUM_MODES
#define NUM_MODES  11

static int mode_bwA[NUM_MODES] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
static int mode_bwB[NUM_MODES] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

static const char *FT450Dmodes_[] = {
		"LSB", "USB", "CW", "FM", "AM", "RTTY-L", "CW-R", "DATA-L", "RTTY-U", "FM-N", "DATA-U", NULL};
static const char mode_chr[] =  { '1', '2', '3', '4', '5', '6', '7', '8', '9', 'B', 'C' };
static const char mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'L', 'L', 'L', 'U', 'U', 'U' };

static const char *FT450D_ssb_widths[]  = {"1800", "2400", "3000", NULL};
static int FT450D_wvals_ssb_widths[] = {1, 2, 3, WVALS_LIMIT};

static const char *FT450D_cw_widths[]   = {"300", "500", "2400", NULL};
static int FT450D_wvals_cw_widths[] = {1, 2, 3, WVALS_LIMIT};

static const char *FT450D_data_widths[] = {"300", "2400", "3000", NULL};
static int FT450D_wvals_data_widths[] = {1, 2, 3, WVALS_LIMIT};

static const char *FT450D_am_widths[]   = {"3000", "6000", "9000", NULL};
static int FT450D_wvals_am_widths[] = {1, 2, 3, WVALS_LIMIT};

static const char *FT450D_fm_widths[]   = {"2500", "5000", NULL};
static int FT450D_wvals_fm_widths[] = {1, 2, WVALS_LIMIT};

static const int FT450D_def_bw[] = {
  2, 2, 0, 1, 1, 0, 0, 1, 0, 1, 1 };

/*
static const char *FT450D_US_60m[] = {NULL, "126", "127", "128", "130", NULL};
// US has 5 60M presets. Using dummy numbers for all.
// First NULL means skip 60m sets in get_band_selection().
// Maybe someone can do a cat command MC; on all 5 presets and add returned numbers above.
// To send cat commands in flrig goto menu Config->Xcvr select->Send Cmd.
// US 60M 5-USB, 5-CW

static const char **Channels_60m = FT450D_US_60m;
*/

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 },
	{ (Fl_Widget *)sldrRFGAIN,   54, 145, 156 },
	{ (Fl_Widget *)btnIFsh,     214, 105,  50 },
	{ (Fl_Widget *)sldrIFSHIFT, 266, 105, 156 },
	{ (Fl_Widget *)btnNotch,    214, 125,  50 },
	{ (Fl_Widget *)sldrNOTCH,   266, 125, 156 },
	{ (Fl_Widget *)sldrMICGAIN, 266, 145, 156 },
	{ (Fl_Widget *)sldrPOWER,   266, 165, 156 },
	{ (Fl_Widget *)btnNR,         2, 165,  50 },
	{ (Fl_Widget *)sldrNR,       54, 165, 156 },
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

RIG_FT450D::RIG_FT450D() {
// base class values
	IDstr = "ID";
	name_ = FT450Dname_;
	modes_ = FT450Dmodes_;
	bandwidths_ = FT450D_ssb_widths;
	bw_vals_ = FT450D_wvals_ssb_widths;

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
	has_band_selection =
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
	has_cw_delay =
	has_cw_break_in =

	has_split =
	has_split_AB =
	has_getvfoAorB =
	has_smeter =
	has_swr_control =
	has_alc_control =
	has_power_out =
	has_power_control =
	has_volume_control =
	has_rf_control =
	has_mode_control =
	has_noise_control =
	has_bandwidth_control =
	has_micgain_control =
	has_notch_control =
	has_attenuator_control =
	has_preamp_control =
	has_ifshift_control =
	has_ptt_control =
	has_tune_control = true;

	has_band_selection = true;

// derived specific
	notch_on = false;
	m_60m_indx = 0;

	precision = 1;
	ndigits = 8;
}

void RIG_FT450D::initialize()
{
	LOG_INFO("%s", "Initializing FT-450D");

	rig_widgets[0].W = btnVol;
	rig_widgets[1].W = sldrVOLUME;
	rig_widgets[2].W = sldrRFGAIN;
	rig_widgets[3].W = btnIFsh;
	rig_widgets[4].W = sldrIFSHIFT;
	rig_widgets[5].W = btnNotch;
	rig_widgets[6].W = sldrNOTCH;
	rig_widgets[7].W = sldrMICGAIN;
	rig_widgets[8].W = sldrPOWER;
	rig_widgets[9].W = btnNR;
	rig_widgets[10].W = sldrNR;

	op_yaesu_select60->deactivate();

// set progStatus defaults
	if (progStatus.noise_reduction_val < 1) progStatus.noise_reduction_val = 1;

// turn off auto information mode
	sendCommand("AI0;");

	get_cw_weight();
	get_cw_wpm();
	get_break_in();
	get_qsk();
	get_qsk_delay();
	get_cw_spot_tone();
	get_vox_gain();
	get_vox_hang();

	selectA();
}

void RIG_FT450D::selectA()
{
	cmd = "VS0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "select vfo A", cmd, replystr);
	sett("selectA");
}

void RIG_FT450D::selectB()
{
	cmd = "VS1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "select vfo B", cmd, replystr);
	sett("selectB");
}

void RIG_FT450D::A2B()
{
	cmd = "VV;";
	sendCommand(cmd);
	showresp(WARN, ASC, "vfo A --> B", cmd, replystr);
	sett("A2B");
}

bool RIG_FT450D::check()
{
	cmd = rsp = "FA";
	cmd += ';';
	int ret = wait_char(';',11, FL450D_WAIT_TIME, "check", ASC);

	gett("check");

	if (ret >= 11) return true;
	return false;
}

long RIG_FT450D::get_vfoA ()
{
	cmd = rsp = "FA";
	cmd += ';';
	wait_char(';',11, FL450D_WAIT_TIME, "get vfo A", ASC);

	gett("get_vfoA");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return freqA;
	int f = 0;
	for (size_t n = 2; n < 10; n++)
		f = f*10 + replystr[p+n] - '0';
	freqA = f;
	return freqA;
}

void RIG_FT450D::set_vfoA (long freq)
{
	if (useB) selectA();
	freqA = freq;
	cmd = "FA00000000;";
	for (int i = 9; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vfo A", cmd, replystr);
	sett("set_vfoA");
	if (useB) selectB();
}

long RIG_FT450D::get_vfoB ()
{
	cmd = rsp = "FB";
	cmd += ';';
	wait_char(';',11, FL450D_WAIT_TIME, "get vfo B", ASC);

	gett("get_vfoB");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return freqB;
	int f = 0;
	for (size_t n = 2; n < 10; n++)
		f = f*10 + replystr[p+n] - '0';
	freqB = f;
	return freqB;
}

void RIG_FT450D::set_vfoB (long freq)
{
	if (!useB) selectB();
	freqB = freq;
	cmd = "FB00000000;";
	for (int i = 9; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vfo B", cmd, replystr);
	sett("set_vfoB");
	if (!useB) selectA();
}

int RIG_FT450D::get_vfoAorB()
{
	size_t p;
	char tx;
	cmd = rsp = "VS";
	cmd.append(";");
	wait_char(';', 4, FL450D_WAIT_TIME, "get vfo A/B", ASC);
	gett("get vfo A/B");
	p = replystr.rfind(rsp);
	return (replystr[p+2] == '1');
}

void RIG_FT450D::set_split(bool on)
{
	if (useB) {
		if (on) cmd = "FT0;";
		else cmd = "FT1;";
	} else {
		if (on) cmd = "FT1;";
		else cmd = "FT0;";
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET split", cmd, replystr);
	sett("set_split");
}

int RIG_FT450D::get_split()
{
	size_t p;
	int split = 0;
	char tx;
// tx vfo
	cmd = rsp = "FT";
	cmd.append(";");
	wait_char(';',4, FL450D_WAIT_TIME, "get split tx vfo", ASC);
	gett("get_split");

	p = replystr.rfind(rsp);
	if (p == string::npos) return false;
	tx = replystr[p+2];

	if (useB)
		split = (tx == '0');
	else
		split = (tx == '1');

	return split;
}

struct mtrpair {int val; float mtr;};

static mtrpair sm_tbl[] = { 
{0, 0.0},
{18, 6.0},
{51, 17.0},
{85, 28.0},
{118, 39.0},
{151, 50.0},
{255, 100.0} };

int RIG_FT450D::get_smeter()
{
	cmd = rsp = "SM0";
	cmd += ';';
	wait_char(';',7, FL450D_WAIT_TIME, "get smeter", ASC);

	gett("get_smeter");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	if (p + 6 >= replystr.length()) return 0;
	int val = atoi(&replystr[p+3]);

	size_t i = 0;
	if (val < 0) val = 0;
	if (val > 255) val = 255;
	for (i = 0; i < sizeof(sm_tbl) / sizeof(mtrpair) - 1; i++)
		if (val >= sm_tbl[i].val && val < sm_tbl[i+1].val)
			break;
	int mtr = (int)ceil(sm_tbl[i].mtr + 
				(sm_tbl[i+1].mtr - sm_tbl[i].mtr) * 
				(val - sm_tbl[i].val)/(sm_tbl[i+1].val - sm_tbl[i].val));

	if (mtr > 100) mtr = 100;

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

int RIG_FT450D::get_swr()
{
	cmd = rsp = "RM6";
	cmd += ';';
	wait_char(';',7, FL450D_WAIT_TIME, "get swr", ASC);

	gett("get_swr");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	if (p + 6 >= replystr.length()) return 0;
	int mtr = atoi(&replystr[p+3]);
	return (int)ceil(mtr / 2.56);
}

int RIG_FT450D::get_alc()
{
	cmd = rsp = "RM4";
	cmd += ';';
	wait_char(';',7, FL450D_WAIT_TIME, "get alc", ASC);
	gett("get_alc");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	if (p + 6 >= replystr.length()) return 0;
	int mtr = atoi(&replystr[p+3]);
	return (int)ceil(mtr / 2.56);
}


int RIG_FT450D::get_power_out()
{
	cmd = rsp = "RM5";
	sendCommand(cmd.append(";"));
	wait_char(';',7, FL450D_WAIT_TIME, "get pout", ASC);

	gett("get_power_out");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	if (p + 6 >= replystr.length()) return 0;
	double mtr = (double)(atoi(&replystr[p+3]));

// following conversion iaw data measured by Terry, KJ4EED
	mtr = (.06 * mtr) + (.002 * mtr * mtr);

	return (int)ceil(mtr);
}

int RIG_FT450D::get_power_control()
{
	cmd = rsp = "PC";
	cmd += ';';
	wait_char(';',6, FL450D_WAIT_TIME, "get power", ASC);

	gett("get_power_control");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.power_level;
	if (p + 5 >= replystr.length()) return progStatus.power_level;

	int mtr = atoi(&replystr[p+2]);
	return mtr;
}

void RIG_FT450D::set_power_control(double val)
{
	int ival = (int)val;
	cmd = "PC000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ival % 10;
		ival /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET power", cmd, replystr);
	sett("set_power_control");
}

// Volume control return 0 ... 100
int RIG_FT450D::get_volume_control()
{
	cmd = rsp = "AG0";
	cmd += ';';
	wait_char(';',7, FL450D_WAIT_TIME, "get vol", ASC);

	gett("get_volume_control");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.volume;
	if (p + 6 >= replystr.length()) return progStatus.volume;
	int val = round(atoi(&replystr[p+3]) / 2.55);
	if (val > 100) val = 100;
	return ceil(val);
}

void RIG_FT450D::set_volume_control(int val) 
{
	int ivol = (int)(val * 255 / 100);
	cmd = "AG0000;";
	for (int i = 5; i > 2; i--) {
		cmd[i] += ivol % 10;
		ivol /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vol", cmd, replystr);
	sett("set_volume_control");
}

void RIG_FT450D::get_vol_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

// Tranceiver PTT on/off
void RIG_FT450D::set_PTT_control(int val)
{
	cmd = val ? "TX1;" : "TX0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "SET PTT", cmd, replystr);
	sett("set_PTT_control");
	ptt_ = val;
}

int RIG_FT450D::get_PTT()
{
	cmd = "TX;";
	rsp = "TX";
	waitN(4, 100, "get PTT", ASC);

	gett("get_PTT");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return ptt_;
	ptt_ =  (replystr[p+2] != '0' ? 1 : 0);
	return ptt_;
}


void RIG_FT450D::tune_rig(int val)
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

int RIG_FT450D::get_tune()
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

void RIG_FT450D::set_attenuator(int val)
{
	if (val) cmd = "RA01;";
	else	 cmd = "RA00;";
	sendCommand(cmd);
	showresp(WARN, ASC, "get att", cmd, replystr);
	sett("set_attenuator");
}

int RIG_FT450D::get_attenuator()
{
	cmd = rsp = "RA0";
	cmd += ';';
	wait_char(';', 5, FL450D_WAIT_TIME, "get att", ASC);

	gett("get_attenuator");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	return (replystr[p+3] == '3' ? 1 : 0);
}

void RIG_FT450D::set_preamp(int val)
{
	if (val) cmd = "PA01;";
	else	 cmd = "PA00;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set preamp", cmd, replystr);
	sett("set_pramp");
}

int RIG_FT450D::get_preamp()
{
	cmd = rsp = "PA0";
	cmd += ';';
	wait_char(';', 5, FL450D_WAIT_TIME, "get pre", ASC);

	gett("get_preamp");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	return (replystr[p+3] == '1' ? 1 : 0);
}

int RIG_FT450D::adjust_bandwidth(int val)
{
	switch (val) {
		case mCW     :
		case mCW_R   :
			bandwidths_ = FT450D_cw_widths;
			bw_vals_ = FT450D_wvals_cw_widths;
			break;
		case mFM     :
		case mFM_N   :
			bandwidths_ = FT450D_fm_widths;
			bw_vals_ = FT450D_wvals_fm_widths;
			break;
		case mAM :
			bandwidths_ = FT450D_am_widths;
			bw_vals_ = FT450D_wvals_am_widths;
			break;
		case mRTTY_L :
		case mRTTY_U :
		case mDATA_L :
		case mDATA_U :
			bandwidths_ = FT450D_data_widths;
			bw_vals_ = FT450D_wvals_data_widths;
			break;
		default:
			bandwidths_ = FT450D_ssb_widths;
			bw_vals_ = FT450D_wvals_ssb_widths;
			break;
	}
	return FT450D_def_bw[val];
}

int RIG_FT450D::def_bandwidth(int m)
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

const char ** RIG_FT450D::bwtable(int n)
{
	switch (n) {
		case mCW     :
		case mCW_R   :
			return FT450D_cw_widths;
		case mFM     :
		case mFM_N   :
			return FT450D_fm_widths;
		case mAM :
			return FT450D_am_widths;
		case mRTTY_L :
		case mRTTY_U :
		case mDATA_L :
		case mDATA_U :
			return FT450D_data_widths;
	}
	return FT450D_ssb_widths;
}

void RIG_FT450D::set_modeA(int val)
{
	modeA = val;
	cmd = "MD0";
	cmd += mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mode A", cmd, replystr);
	sett("set_modeA");

	adjust_bandwidth(modeA);
	if (val == 2 || val == 6) return;
	if (progStatus.spot_onoff) {
		progStatus.spot_onoff = false;
		set_spot_onoff();
		cmd = "CS0;";
		sendCommand(cmd);
		showresp(WARN, ASC, "SET spot off", cmd, replystr);
		sett("set_spot_off");
		btnSpot->value(0);
	}
}

int RIG_FT450D::get_modeA()
{
	cmd = rsp = "MD0";
	cmd += ';';
	wait_char(';', 5, FL450D_WAIT_TIME, "get mode A", ASC);

	gett("get_modeA");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return modeA;
	int md = replystr[p+3];
	if (md <= '9') md = md - '1';
	else md = 9 + md - 'B';
	modeA = md;
	adjust_bandwidth(modeA);
	return modeA;
}

void RIG_FT450D::set_bwA(int val)
{
	bwA = val;
	switch (val) {
		case 0 : cmd = "SH000;"; break;
		case 1 : cmd = "SH016;"; break;
		case 2 : cmd = "SH031;"; break;
		default: cmd = "SH016;";
	}
	sendCommand(cmd);
	mode_bwA[modeA] = bwA;
	showresp(WARN, ASC, "SET bwA", cmd, replystr);
	sett("set_bwA");
}

int RIG_FT450D::get_bwA()
{
	cmd = rsp = "SH0";
	cmd += ';';
	wait_char(';', 6, FL450D_WAIT_TIME, "get bw A", ASC);

	gett("get_bwA");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return bwA;
	string bws = replystr.substr(p+3,2);
	if (bws == "00") bwA = 0;
	else if (bws == "16") bwA = 1;
	else if (bws == "31") bwA = 2;
	mode_bwA[modeA] = bwA;
	return bwA;
}

void RIG_FT450D::set_modeB(int val)
{
	modeB = val;
	cmd = "MD0";
	cmd += mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mode B", cmd, replystr);
	sett("set_modeB");

	adjust_bandwidth(modeB);
	if (val == 2 || val == 6) return;
	if (progStatus.spot_onoff) {
		progStatus.spot_onoff = false;
		set_spot_onoff();
		cmd = "CS0;";
		sendCommand(cmd);
		showresp(WARN, ASC, "SET spot off", cmd, replystr);
		sett("set spot off");
		btnSpot->value(0);
	}
}

int RIG_FT450D::get_modeB()
{
	cmd = rsp = "MD0";
	cmd += ';';
	wait_char(';', 5, FL450D_WAIT_TIME, "get mode B", ASC);

	gett("get_modeB");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return modeB;
	int md = replystr[p+3];
	if (md <= '9') md = md - '1';
	else md = 9 + md - 'B';
	modeB = md;
	adjust_bandwidth(modeB);
	return modeB;
}

void RIG_FT450D::set_bwB(int val)
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
	sett("set_bwB");
}

int RIG_FT450D::get_bwB()
{
	cmd = rsp = "SH0";
	cmd += ';';
	wait_char(';', 6, FL450D_WAIT_TIME, "get bw B", ASC);

	gett("get_bwB");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return bwB;
	string bws = replystr.substr(p+3,2);
	if (bws == "00") bwB = 0;
	else if (bws == "16") bwB = 1;
	else if (bws == "31") bwB = 2;
	mode_bwB[modeB] = bwB;
	return bwB;
}

std::string RIG_FT450D::get_BANDWIDTHS()
{
	stringstream s;
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_bwA[i] << " ";
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_bwB[i] << " ";
	return s.str();
}

void RIG_FT450D::set_BANDWIDTHS(std::string s)
{
	stringstream strm;
	strm << s;
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_bwA[i];
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_bwB[i];
}

int RIG_FT450D::get_modetype(int n)
{
	return mode_type[n];
}

void RIG_FT450D::set_if_shift(int val)
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
	sett("set_if_shift");
}

bool RIG_FT450D::get_if_shift(int &val)
{
	cmd = rsp = "IS0";
	cmd += ';';
	wait_char(';', 9, FL450D_WAIT_TIME, "get if shift", ASC);

	gett("get_if_shift");

	size_t p = replystr.rfind(rsp);
	val = progStatus.shift_val;
	if (p == string::npos) return progStatus.shift;
	val = atoi(&replystr[p+4]);
	if (replystr[p+3] == '-') val = -val;
	return (val != 0);
}

void RIG_FT450D::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -1000;
	max = 1000;
	step = 100;
}

void RIG_FT450D::set_notch(bool on, int val)
{
	cmd = "BP00000;";
	if (!on) {
		if (notch_on) {
			sendCommand(cmd);
			showresp(WARN, ASC, "SET notch off", cmd, replystr);
			sett("set notch off");
			notch_on = false;
		}
		return;
	}

	cmd = "BP00001;";
	if (!notch_on) {
		sendCommand(cmd);
		showresp(WARN, ASC, "SET notch on", cmd, replystr);
		sett("set notch on");
		notch_on = true;
	}

	cmd = "BP01nnn;";
	val = val / 10;
	for (int i = 3; i > 0; i--) {
		cmd[3 + i] = val % 10 + '0';
		val /=10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET notch val", cmd, replystr);
	sett("set notch val");
}

bool  RIG_FT450D::get_notch(int &val)
{
	bool ison = false;
	cmd = rsp = "BP00";
	cmd += ';';
	wait_char(';', 8, FL450D_WAIT_TIME, "get notch on/off", ASC);

	gett("get_notch");

	size_t p = replystr.rfind(rsp);
	val = progStatus.notch_val = 0; // disabled default slider position
	if (p == string::npos) return ison;

	if (replystr[p+6] == '1') { // manual notch enabled
		ison = true;
		val = progStatus.notch_val;
		cmd = rsp = "BP01";
		cmd += ';';
		waitN(8, 100, "get notch val", ASC);

		gett("get_notch_val");

		p = replystr.rfind(rsp);
		if (p == string::npos || rsp.length() < 8) return ison;
		val = atoi(&replystr[p+4]);
		val = val * 10;
	}
	notch_on = ison;
	return ison;
}

void RIG_FT450D::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = 10;
	max = 4000;
	step = 10;
}

void RIG_FT450D::set_noise(bool b)
{
	if (b)
		cmd = "NB01;";
	else
		cmd = "NB00;";
	sendCommand (cmd);
	showresp(WARN, ASC, "SET NB", cmd, replystr);
	sett("set_noise");
}

void RIG_FT450D::set_xcvr_auto_on()
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

void RIG_FT450D::set_xcvr_auto_off()
{
	cmd = "PS0;";
	sendCommand(cmd);
	sett("set_xcvr_auto_off");
}

// val 0 .. 100
void RIG_FT450D::set_mic_gain(int val)
{
	cmd = "MG000;";
	val = (int)(val * 2.55); // convert to 0 .. 255
	for (int i = 3; i > 0; i--) {
		cmd[1+i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mic", cmd, replystr);
	sett("set_mic_gain");
}

int RIG_FT450D::get_mic_gain()
{
	cmd = rsp = "MG";
	cmd += ';';
	wait_char(';', 6, FL450D_WAIT_TIME, "get mic", ASC);

	gett("get_mic_gain");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	replystr[p+5] = 0;
	return atoi(&replystr[p+2]);;
}

void RIG_FT450D::get_mic_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_FT450D::set_special(int v)
{
	if (v) cmd = "VR1;";
	else   cmd = "VR0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Set special", cmd, replystr);
	sett("set_special");
}

void RIG_FT450D::set_vox_onoff()
{
	cmd = "VX0;";
	if (progStatus.vox_onoff) cmd[2] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox on/off", cmd, replystr);
	sett("set_vox_onoff");
}

void RIG_FT450D::set_vox_gain()
{
	cmd = "VG";
	cmd.append(to_decimal(progStatus.vox_gain, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox gain", cmd, replystr);
	sett("set_vox_gain");
}

void RIG_FT450D::set_vox_hang()
{
	cmd = "VD";
	cmd.append(to_decimal(progStatus.vox_hang, 4)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox delay", cmd, replystr);
	sett("set_vox_hang");
}

void RIG_FT450D::set_cw_wpm()
{
	cmd = "KS";
	if (progStatus.cw_wpm > 60) progStatus.cw_wpm = 60;
	if (progStatus.cw_wpm < 4) progStatus.cw_wpm = 4;
	cmd.append(to_decimal(progStatus.cw_wpm, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw wpm", cmd, replystr);
	sett("set_cw_wpm");
}


void RIG_FT450D::enable_keyer()
{
	cmd = "KR0;";
	if (progStatus.enable_keyer) cmd[2] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET keyer on/off", cmd, replystr);
	sett("enable_keyer");
}

bool RIG_FT450D::set_cw_spot()
{
	if (vfo->imode == 2 || vfo->imode == 6) {
		cmd = "CS0;";
		if (progStatus.spot_onoff) cmd[2] = '1';
		sendCommand(cmd);
		showresp(WARN, ASC, "SET spot on/off", cmd, replystr);
		sett("set_cw_spot");
		return true;
	} else
		return false;
}

void RIG_FT450D::set_cw_weight()
{
	int n = round(progStatus.cw_weight * 10);
	cmd.assign("EX024").append(to_decimal(n, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw weight", cmd, replystr);
	sett("set_cw_weight");
}

void RIG_FT450D::enable_break_in()
{
	if (progStatus.break_in) {
		cmd = "BI1;";
		break_in_label("FULL");
	} else {
		cmd = "BI0;";
		break_in_label("QSK");
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET break in on/off", cmd, replystr);
	sett("enable_break_in");
}

void RIG_FT450D::set_cw_delay()
{
	if (progStatus.break_in) return;

	int n = progStatus.cw_delay;
	cmd.assign("EX016").append(to_decimal(n,4)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw delay", cmd, replystr);
	sett("set_cw_delay");
}

void RIG_FT450D::set_cw_qsk()
{
	if (progStatus.cw_qsk == 0) {
		cmd.assign("BI1;");
		sendCommand(cmd);
		showresp(WARN, ASC, "SET break-in ON", cmd, replystr);
		sett("set break in ON");
		return;
	}
	cmd.assign("BI0;");
	sendCommand(cmd);
	sett("SET break-in OFF");
	int n = progStatus.cw_qsk / 5 - 3;
	cmd.assign("EX018").append(to_decimal(n, 1)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw qsk", cmd, replystr);
	sett("set cw qsk");
}

void RIG_FT450D::set_cw_spot_tone()
{
	int n = (progStatus.cw_spot_tone - 400) / 50 + 1;
	cmd = "EX020";
	cmd.append(to_decimal(n, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw tone", cmd, replystr);
	sett("set_cw_spot_tone");
}

// DNR
void RIG_FT450D::set_noise_reduction_val(int val)
{
	cmd.assign("RL0").append(to_decimal(val, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET_noise_reduction_level", cmd, replystr);
	sett("set_noise_reduction_val");
}

int  RIG_FT450D::get_noise_reduction_val()
{
	int val = 1;
	cmd = rsp = "RL0";
	cmd.append(";");
	waitN(6, 100, "GET noise reduction level", ASC);

	gett("get_noise_reduction_val");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return val;
	val = atoi(&replystr[p+3]);
	return val;
}

// DNR
void RIG_FT450D::set_noise_reduction(int val)
{
	cmd.assign("NR0").append(val ? "1" : "0" ).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET noise reduction", cmd, replystr);
	sett("set_noise_reduction");
}

int  RIG_FT450D::get_noise_reduction()
{
	int val;
	cmd = rsp = "NR0";
	cmd.append(";");
	wait_char(';', 5, FL450D_WAIT_TIME, "GET noise reduction", ASC);
	gett("get_noise_reduction");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	val = replystr[p+3] - '0';
	return val;
}

void RIG_FT450D::set_rf_gain(int val)
{
	cmd = "RG0000;";
	int rfval = (int)((100 - val) * 2.55);
	for (int i = 5; i > 2; i--) {
		cmd[i] = rfval % 10 + '0';
		rfval /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET rfgain", cmd, replystr);
	sett("set_rf_gain");
}

int  RIG_FT450D::get_rf_gain()
{
	int rfval = 0;
	cmd = rsp = "RG0";
	cmd += ';';
	wait_char(';', 7, FL450D_WAIT_TIME, "get rfgain", ASC);

	gett("get_rf_gain");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.rfgain;
	for (int i = 3; i < 6; i++) {
		rfval *= 10;
		rfval += replystr[p+i] - '0';
	}
	rfval = 100 - (int)(rfval / 2.55);
	if (rfval > 100) rfval = 100;
	return ceil(rfval);
}

void RIG_FT450D::get_rf_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

//----------------------------------------------------------------------
// these are used during initialization

void RIG_FT450D::get_cw_weight()
{
	cmd = "EX024;";
	wait_char(';', 9, FL450D_WAIT_TIME, "get CW weight", ASC);
	gett("get_cw_weight");

	size_t p = replystr.rfind("EX024");
	if (p == string::npos) return;
	replystr[p+8] = 0;
	int val = atoi(&replystr[p+5]);
	progStatus.cw_weight = val / 10.0;
}

void RIG_FT450D::get_cw_wpm()
{
	cmd = rsp = "KS;";
	wait_char(';', 6, FL450D_WAIT_TIME, "get WPM", ASC);
	gett("get_cw_wpm");

	size_t p = replystr.rfind("KS");
	if (p == string::npos) return;

	replystr[p+5] = 0;
	int val = atoi(&replystr[p+2]);
	progStatus.cw_wpm = val;
}

void RIG_FT450D::get_break_in()
{
	cmd = "BI;";
	wait_char(';', 4, FL450D_WAIT_TIME, "get Break In", ASC);
	gett("get_break_in");

	size_t p = replystr.rfind("BI");
	if (p != string::npos)
		progStatus.break_in = (replystr[2] == '1');
	if (progStatus.break_in) {
		break_in_label("FULL");
		progStatus.cw_delay = 0;
	} else {
		break_in_label("QSK");
		get_qsk_delay();
	}
}

void RIG_FT450D::get_qsk()
{
	cmd = "EX018;";
	wait_char(';', 7, FL450D_WAIT_TIME, "get CW qsk", ASC);
	gett("get_qsk");

	size_t p = replystr.rfind("EX018");
	if (p == string::npos) return;

	switch (replystr[p+5]) {
		default :
		case '0' : progStatus.cw_qsk = 15; break;
		case '1' : progStatus.cw_qsk = 20; break;
		case '2' : progStatus.cw_qsk = 25; break;
		case '3' : progStatus.cw_qsk = 30; break;
	}
}

void RIG_FT450D::get_qsk_delay()
{
	cmd = "EX016;";
	wait_char(';', 10, FL450D_WAIT_TIME, "get CW delay", ASC);
	gett("get_qsk_delay");

	size_t p = replystr.rfind("EX016");
	if (p == string::npos) return;
	replystr[p+9] = 0;
	progStatus.cw_delay = atoi(&replystr[p+5]);
}

void RIG_FT450D::get_cw_spot_tone()
{
	cmd = "EX020;";
	wait_char(';', 8, FL450D_WAIT_TIME, "get CW spot tone", ASC);
	gett("get_cw_spot_tone");

	size_t p = replystr.rfind("EX020");
	if (p == string::npos) return;
	replystr[p+7] = 0;
	int n = atoi(&replystr[p+5]);
	switch (n) {
		case 0:
		case 1: case 2: progStatus.cw_spot_tone = 400; break;
		case 3: case 4: progStatus.cw_spot_tone = 500; break;
		case 5: case 6: progStatus.cw_spot_tone = 600; break;
		case 7: case 8: progStatus.cw_spot_tone = 700; break;
		case 9: default: progStatus.cw_spot_tone = 800; break;
	}
}

void RIG_FT450D::get_vox_gain()
{
	cmd = "VG;";
	wait_char(';', 6, FL450D_WAIT_TIME, "get VOX gain", ASC);
	gett("get_vox_gain");

	size_t p = replystr.rfind("VG");
	if (p == string::npos) return;
	replystr[p+5] = 0;
	progStatus.vox_gain = atoi(&replystr[p+2]);
}

void RIG_FT450D::get_vox_hang()
{
	cmd = "VD;";
	wait_char(';', 7, FL450D_WAIT_TIME, "get VOX delay", ASC);
	gett("get_vox_hang");

	size_t p = replystr.rfind("VD");
	if (p == string::npos) return;
	replystr[p+6] = 0;
	progStatus.vox_hang = atoi(&replystr[p+2]);
}

void RIG_FT450D::get_band_selection(int v)
{
	if (v < 3) v = v - 1;
	cmd.assign("BS").append(to_decimal(v, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "Select Band Stacks", cmd, replystr);
	sett("get band");
}

/*
void RIG_FT450D::get_band_selection(int v)
{
	int inc_60m = false;
	cmd = "IF;";
	waitN(27, 100, "get band", ASC);

	sett("get band");

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

//----------------------------------------------------------------------
