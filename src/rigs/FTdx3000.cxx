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

#include "FTdx3000.h"
#include "debug.h"
#include "support.h"

enum mFTdx3000 {
  mLSB, mUSB, mCW, mFM, mAM, mRTTY_L, mCW_R, mPKT_L, mRTTY_U, mPKT_FM, mFM_N, mPKT_U, mAM_N };
// mLSB, mUSB, mCW, mFM, mAM, mRTTY_L, mCW_R, mPKT_L, mRTTY_U, mPKT_FM, mFM_N, mPKT_U, mAM_N
//  0,    1,    2,   3,   4,   5,       6,     7,      8,       9,       10,    11,    12	// mode index
// 19,   19,    9,   0,   0,  10,       9,    15,     10,       0,        0,    15,     0	// FTdx3000_def_bw

static const char FTdx3000name_[] = "FTdx3000";

#undef  NUM_MODES
#define NUM_MODES  13

static int mode_bwA[NUM_MODES] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
static int mode_bwB[NUM_MODES] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

static const char *FTdx3000modes_[] = {
"LSB", "USB", "CW", "FM", "AM", 
"RTTY-L", "CW-R", "PSK-L", "RTTY-U", "PKT-FM",
"FM-N", "PSK-U", "AM-N",  NULL};

static const int FTdx3000_def_bw[] = {
20, 20, 9, 0, 0, 
10, 9, 15, 10, 0,
0, 15, 0 };

static const char FTdx3000_mode_chr[] =  { '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D' };
static const char FTdx3000_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'L', 'L', 'L', 'U', 'U', 'U', 'U', 'U' };

static const char *FTdx3000_widths_SSB[] = {
 "200",  "400",  "600",  "850", "1100", 
"1350", "1500", "1650", "1800", "1950",
"2100", "2250", "2400", "2500", "2600",
"2700", "2800", "2900", "3000", "3200", 
"3400", "3600", "3800", "4000", NULL };

static int FTdx3000_wvals_SSB[] = {
1,2,3,4,5,6,7,  8,9,10,11,12,13,  15,16,17,18,19,20,  21,22,23,24,25, WVALS_LIMIT};

static const char *FTdx3000_widths_CW[] = {
  "50",  "100",  "150",  "200",  "250",
 "300",  "350",  "400",  "450",  "500", 
 "800", "1200", "1400", "1700", "2000",
"2400", NULL };

static int FTdx3000_wvals_CW[] = {
1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, WVALS_LIMIT };

static const char *FTdx3000_widths_RTTY[] = {
  "50",  "100",  "150",  "200",  "250",
 "300",  "350",  "400",  "450",  "500",
 "800", "1200", "1400", "1700", "2000", 
"2400", NULL };

static int FTdx3000_wvals_RTTY[] = {
1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, WVALS_LIMIT };

static const char *FTdx3000_widths_PSK[] = {
 "50",  "100",  "150",  "200",  "250", 
"300",  "350",  "400",  "450",  "500",
"800", "1200", "1400", "1700", "2000",
"2400", NULL };

static int FTdx3000_wvals_PSK[] = {
1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, WVALS_LIMIT };

static const int FTdx3000_wvals_AMFM[] = { 0, WVALS_LIMIT };

static const char *FTdx3000_widths_AMwide[] = { "AM-bw", NULL };
static const char *FTdx3000_widths_AMnar[]  = { "AM-nar", NULL };
static const char *FTdx3000_widths_FMnar[]  = { "FM-nar", NULL };
static const char *FTdx3000_widths_FMwide[] = { "FM-wid", NULL };
static const char *FTdx3000_widths_FMpkt[]  = { "FM-pkt", NULL };

static const char *FTdx3000_US_60m[] = {NULL, "126", "127", "128", "130", NULL};
// US has 5 60M presets. Using dummy numbers for all.
// First NULL means skip 60m sets in get_band_selection().
// Maybe someone can do a cat command MC; on all 5 presets and add returned numbers above.
// To send cat commands in flrig goto menu Config->Xcvr select->Send Cmd.
//
// UK has 7 60M presets. Using dummy numbers for all.  If you want support,
// Maybe someone can do a cat command MC; on all 7 presets and add returned numbers below.
// static const char *FTdx3000_UK_60m[] = {NULL, "126", "127", "128", "130", "131", "132", NULL};

static const char **Channels_60m = FTdx3000_US_60m;

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 },
	{ (Fl_Widget *)sldrRFGAIN,   54, 145, 156 },
	{ (Fl_Widget *)btnIFsh,     214, 105,  50 },
	{ (Fl_Widget *)sldrIFSHIFT, 266, 105, 156 },
	{ (Fl_Widget *)btnNotch,    214, 125,  50 },
	{ (Fl_Widget *)sldrNOTCH,   266, 125, 156 },
	{ (Fl_Widget *)sldrMICGAIN, 266, 145, 156 },
	{ (Fl_Widget *)sldrPOWER,    54, 165, 368 },
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

void RIG_FTdx3000::initialize()
{
	rig_widgets[0].W = btnVol;
	rig_widgets[1].W = sldrVOLUME;
	rig_widgets[2].W = sldrRFGAIN;
	rig_widgets[3].W = btnIFsh;
	rig_widgets[4].W = sldrIFSHIFT;
	rig_widgets[5].W = btnNotch;
	rig_widgets[6].W = sldrNOTCH;
	rig_widgets[7].W = sldrMICGAIN;
	rig_widgets[8].W = sldrPOWER;

	cmd = "AI0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Auto Info OFF", cmd, replystr);
}

RIG_FTdx3000::RIG_FTdx3000() {
// base class values
	IDstr = "ID";
	name_ = FTdx3000name_;
	modes_ = FTdx3000modes_;
	bandwidths_ = FTdx3000_widths_SSB;
	bw_vals_ = FTdx3000_wvals_SSB;

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

	A.imode = B.imode = modeB = modeA = def_mode = 1;
	A.iBW = B.iBW = bwA = bwB = def_bw = 19;
	A.freq = B.freq = freqA = freqB = def_freq = 14070000;

	notch_on = false;

	has_band_selection =
	has_extras =
	has_vox_onoff =
	has_vox_gain =
	has_vox_anti =
	has_vox_hang =
	has_vox_on_dataport =

	has_cw_wpm =
	has_cw_keyer =
//	has_cw_vol =
	has_cw_spot =
//	has_cw_spot_tone = // does not exist???
	has_cw_qsk =
	has_cw_weight =
	has_split_AB =

	can_change_alt_vfo =
	has_smeter =
	has_swr_control =
	has_alc_control =
	has_power_out =
	has_power_control =
	has_volume_control =
	has_rf_control =
	has_micgain_control =
	has_mode_control =
	has_noise_control =
	has_bandwidth_control =
	has_notch_control =
	has_auto_notch =
	has_attenuator_control =
	has_preamp_control =
	has_ifshift_control =
	has_ptt_control =
	has_tune_control = true;

// derived specific
	atten_level = 0;
	preamp_level = 0;
	notch_on = false;
	m_60m_indx = 0;

	precision = 1;
	ndigits = 8;

}

void RIG_FTdx3000::get_band_selection(int v)
{
	int inc_60m = false;
	cmd = "IF;";
	wait_char(';', 27, 100, "get band", ASC);

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

bool RIG_FTdx3000::check ()
{
	cmd = rsp = "FA";
	cmd += ';';
	int ret = wait_char(';', 11, 100, "check", ASC);

	gett("check()");

	if (ret >= 11) return true;
	return false;
}

long RIG_FTdx3000::get_vfoA ()
{
	cmd = rsp = "FA";
	cmd += ';';
	wait_char(';', 11, 100, "get vfo A", ASC);

	gett("get_vfoA()");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return freqA;
	int f = 0;
	for (size_t n = 2; n < 10; n++)
		f = f*10 + replystr[p+n] - '0';
	freqA = f;
	return freqA;
}

void RIG_FTdx3000::set_vfoA (long freq)
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

long RIG_FTdx3000::get_vfoB ()
{
	cmd = rsp = "FB";
	cmd += ';';
	wait_char(';', 11, 100, "get vfo B", ASC);

	gett("get_vfoB()");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return freqB;
	int f = 0;
	for (size_t n = 2; n < 10; n++)
		f = f*10 + replystr[p+n] - '0';
	freqB = f;
	return freqB;
}


void RIG_FTdx3000::set_vfoB (long freq)
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


bool RIG_FTdx3000::twovfos()
{
	return true;
}

void RIG_FTdx3000::selectA()
{
	cmd = "FR0;FT2;";
	sendCommand(cmd);
	showresp(WARN, ASC, "select A", cmd, replystr);
}

void RIG_FTdx3000::selectB()
{
	cmd = "FR4;FT3;";
	sendCommand(cmd);
	showresp(WARN, ASC, "select B", cmd, replystr);
}

void RIG_FTdx3000::A2B()
{
	cmd = "AB;";
	sendCommand(cmd);
	showresp(WARN, ASC, "vfo A --> B", cmd, replystr);
}

bool RIG_FTdx3000::can_split()
{
	return true;
}

void RIG_FTdx3000::set_split(bool val)
{
	split = val;
	if (val) {
		useB = false;
		cmd = "FR0;";
		sendCommand(cmd);
		showresp(WARN, ASC, "Rx on A", cmd, replystr);
		cmd = "FT3;";
		sendCommand(cmd);
		showresp(WARN, ASC, "Tx on B", cmd, replystr);
	} else {
		cmd = "FR0;";
		sendCommand(cmd);
		showresp(WARN, ASC, "Rx on A", cmd, replystr);
		cmd = "FT2;";
		sendCommand(cmd);
		showresp(WARN, ASC, "Tx on A", cmd, replystr);
	}
	Fl::awake(highlight_vfo, (void *)0);
}

int RIG_FTdx3000::get_split()
{
	return split;
}


int RIG_FTdx3000::get_smeter()
{
	cmd = rsp = "SM0";
	cmd += ';';
	wait_char(';', 7, 100, "get smeter", ASC);

	gett("get_smeter()");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	if (p + 6 >= replystr.length()) return 0;
	int mtr = atoi(&replystr[p+3]);
	mtr = mtr * 100.0 / 256.0;
	return mtr;
}

int RIG_FTdx3000::get_swr()
{
	cmd = rsp = "RM6";
	cmd += ';';
	wait_char(';', 7, 100, "get swr", ASC);

	gett("get_swr()");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	if (p + 6 >= replystr.length()) return 0;
	int mtr = atoi(&replystr[p+3]);
	return mtr / 2.56;
}

struct pwrpair {int mtr; float pwr;};

static pwrpair pwrtbl[] = { 
{ 32,  5.0 },
{ 53, 10.0 },
{ 80, 20.0 },
{ 97, 30.0 },
{119, 40.0 },
{137, 50.0 },
{154, 60.0 },
{167, 70.0 },
{177, 80.0 },
{188, 90.0 },
{197,100.0 },
};

int RIG_FTdx3000::get_power_out()
{
	cmd = rsp = "RM5";
	sendCommand(cmd.append(";"));
	wait_char(';', 7, 100, "get pout", ASC);

	gett("get_power_out()");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	if (p + 6 >= replystr.length()) return 0;
	int mtr = atoi(&replystr[p+3]);
	size_t i = 0;
	for (i = 0; i < sizeof(pwrtbl) / sizeof(pwrpair) - 1; i++)
		if (mtr >= pwrtbl[i].mtr && mtr < pwrtbl[i+1].mtr)
			break;
	if (mtr < 0) mtr = 0;
	if (mtr > 197) mtr = 197;
	int pwr = (int)ceil(pwrtbl[i].pwr + 
			  (pwrtbl[i+1].pwr - pwrtbl[i].pwr)*(mtr - pwrtbl[i].mtr) / (pwrtbl[i+1].mtr - pwrtbl[i].mtr));

	if (pwr > 100) pwr = 100;

	return (int)pwr;
}

int RIG_FTdx3000::get_alc()
{
	cmd = rsp = "RM4";
	cmd += ';';
	wait_char(';',7, 100, "get alc", ASC);
	gett("get_alc");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	if (p + 6 >= replystr.length()) return 0;
	int mtr = atoi(&replystr[p+3]);
	return (int)ceil(mtr / 2.56);
}

// Transceiver power level
int RIG_FTdx3000::get_power_control()
{
	cmd = rsp = "PC";
	cmd += ';';
	wait_char(';', 6, 100, "get power", ASC);

	gett("get_power_control()");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.power_level;
	if (p + 5 >= replystr.length()) return progStatus.power_level;

	int mtr = atoi(&replystr[p+2]);
	return mtr;
}

void RIG_FTdx3000::set_power_control(double val)
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
int RIG_FTdx3000::get_volume_control()
{
	cmd = rsp = "AG0";
	cmd += ';';
	wait_char(';', 7, 100, "get vol", ASC);

	gett("get_volume_control()");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.volume;
	if (p + 6 >= replystr.length()) return progStatus.volume;
	int val = atoi(&replystr[p+3]) * 100 / 250;
	if (val > 100) val = 100;
	return val;
}

void RIG_FTdx3000::set_volume_control(int val) 
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

// Tranceiver PTT on/off
void RIG_FTdx3000::set_PTT_control(int val)
{
	cmd = val ? "TX1;" : "TX0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "SET PTT", cmd, replystr);
	ptt_ = val;
}

int RIG_FTdx3000::get_PTT()
{
	cmd = "TX;";
	rsp = "TX";
	wait_char(';', 4, 100, "get PTT", ASC);

	gett("get_PTT()");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return ptt_;
	ptt_ =  (replystr[p+2] != '0' ? 1 : 0);
	return ptt_;
}


void RIG_FTdx3000::tune_rig(int val)
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

int RIG_FTdx3000::get_tune()
{
	cmd = rsp = "AC";
	cmd += ';';
	waitN(5, 100, "get tune", ASC);

	rig_trace(2, "get_tuner status()", replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	int val = replystr[p+4] - '0';
	return !(val < 2);
}


int  RIG_FTdx3000::next_attenuator()
{
	switch (atten_level) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 3;
		case 3: return 0;
	}
	return 0;
}

void RIG_FTdx3000::set_attenuator(int val)
{
	atten_level = val;
	if (atten_level == 1)
		atten_label("6 dB", true);
	else if (atten_level == 2)
		atten_label("12 dB", true);
	else if (atten_level == 3)
		atten_label("18 dB", true);
	else if (atten_level == 0)
		atten_label("Att", false);

	cmd = "RA00;";
	cmd[3] += atten_level;
	sendCommand(cmd);
	showresp(WARN, ASC, "SET att", cmd, replystr);
}

int RIG_FTdx3000::get_attenuator()
{
	cmd = rsp = "RA0";
	cmd += ';';
	wait_char(';', 5, 100, "get att", ASC);

	gett("get_attenuator()");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.attenuator;
	if (p + 3 >= replystr.length()) return progStatus.attenuator;
	atten_level = replystr[p+3] - '0';
	if (atten_level == 1) {
		atten_label("6 dB", true);
	} else if (atten_level == 2) {
		atten_label("12 dB", true);
	} else if (atten_level == 3) {
		atten_label("18 dB", true);
	} else {
		atten_level = 0;
		atten_label("Att", false);
	}
	return atten_level;
}

int  RIG_FTdx3000::next_preamp()
{
	switch (preamp_level) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 0;
	}
	return 0;
}

void RIG_FTdx3000::set_preamp(int val)
{
	preamp_level = val;
	cmd = "PA00;";

	if (preamp_level == 1)
		preamp_label("Amp 1", true);
	else if (preamp_level == 2)
		preamp_label("Amp 2", true);
	else if (preamp_level == 0)
		preamp_label("IPO", false);

	cmd[3] = '0' + preamp_level;
	sendCommand (cmd);
	showresp(WARN, ASC, "SET preamp", cmd, replystr);
}

int RIG_FTdx3000::get_preamp()
{
	cmd = rsp = "PA0";
	cmd += ';';
	wait_char(';', 5, 100, "get pre", ASC);

	gett("get_preamp()");

	size_t p = replystr.rfind(rsp);
	if (p != string::npos)
		preamp_level = replystr[p+3] - '0';
	if (preamp_level == 1) {
		preamp_label("Amp 1", true);
	} else if (preamp_level == 2) {
		preamp_label("Amp 2", true);
	} else {
		preamp_label("IPO", false);
		preamp_level = 0;
	}
	return preamp_level;
}

int RIG_FTdx3000::adjust_bandwidth(int val)
{
	int bw = 0;
	if (val == mCW || val == mCW_R) {
		bandwidths_ = FTdx3000_widths_CW;
		bw_vals_ = FTdx3000_wvals_CW;
	} else if (val == mFM || val == mAM || val == mFM_N || val == mPKT_FM || val == mAM_N) {
		if (val == mFM) bandwidths_ = FTdx3000_widths_FMwide;
		else if (val ==  mAM) bandwidths_ = FTdx3000_widths_AMwide;
		else if (val == mAM_N) bandwidths_ = FTdx3000_widths_AMnar;
		else if (val == mFM_N) bandwidths_ = FTdx3000_widths_FMnar;
		else if (val == mPKT_FM) bandwidths_ = FTdx3000_widths_FMpkt;
		bw_vals_ = FTdx3000_wvals_AMFM;
	} else if (val == mRTTY_L || val == mRTTY_U) { // RTTY
		bandwidths_ = FTdx3000_widths_RTTY;
		bw_vals_ = FTdx3000_wvals_RTTY;
	} else if (val == mPKT_L || val == mPKT_U) { // PSK
		bandwidths_ = FTdx3000_widths_PSK;
		bw_vals_ = FTdx3000_wvals_PSK;
	} else {
		bandwidths_ = FTdx3000_widths_SSB;
		bw_vals_ = FTdx3000_wvals_SSB;
	}
	bw = FTdx3000_def_bw[val];
	return bw;
}

int RIG_FTdx3000::def_bandwidth(int m)
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

const char ** RIG_FTdx3000::bwtable(int n)
{
	switch (n) {
		case mCW: case mCW_R: return FTdx3000_widths_CW;
		case mFM: return FTdx3000_widths_FMwide;
		case mAM: return FTdx3000_widths_AMwide;
		case mAM_N : return FTdx3000_widths_AMnar;
		case mRTTY_L: case mRTTY_U: return FTdx3000_widths_RTTY;
		case mPKT_L: case mPKT_U: return FTdx3000_widths_PSK;
		case mFM_N: return FTdx3000_widths_FMnar;
		case mPKT_FM: return FTdx3000_widths_FMpkt;
		default: ;
	}
	return FTdx3000_widths_SSB;
}

void RIG_FTdx3000::set_modeA(int val)
{
	modeA = val;
	cmd = "MD0";
	cmd += FTdx3000_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mode A", cmd, replystr);
	adjust_bandwidth(modeA);
	if (val == mCW || val == mCW_R) return;
	if (progStatus.spot_onoff) {
		progStatus.spot_onoff = false;
		set_spot_onoff();
		cmd = "CS0;";
		sendCommand(cmd);
		showresp(WARN, ASC, "SET spot off", cmd, replystr);
		btnSpot->value(0);
	}
}

int RIG_FTdx3000::get_modeA()
{
	cmd = rsp = "MD0";
	cmd += ';';
	wait_char(';', 5, 100, "get mode A", ASC);

	gett("get_modeA()");

	size_t p = replystr.rfind(rsp);
	if (p != string::npos) {
		if (p + 3 < replystr.length()) {
			int md = replystr[p+3];
			if (md <= '9') md = md - '1';
			else md = 9 + md - 'A';
			modeA = md;
		}
	}
	adjust_bandwidth(modeA);
	return modeA;
}

void RIG_FTdx3000::set_modeB(int val)
{
	modeB = val;
	cmd = "MD0";
	cmd += FTdx3000_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mode B", cmd, replystr);
	adjust_bandwidth(modeA);
	if (val == mCW || val == mCW_R) return;
	if (progStatus.spot_onoff) {
		progStatus.spot_onoff = false;
		set_spot_onoff();
		cmd = "CS0;";
		sendCommand(cmd);
		showresp(WARN, ASC, "SET spot off", cmd, replystr);
		btnSpot->value(0);
	}
}

int RIG_FTdx3000::get_modeB()
{
	cmd = rsp = "MD0";
	cmd += ';';
	wait_char(';', 5, 100, "get mode B", ASC);

	gett("get_modeB()");

	size_t p = replystr.rfind(rsp);
	if (p != string::npos) {
		if (p + 3 < replystr.length()) {
			int md = replystr[p+3];
			if (md <= '9') md = md - '1';
			else md = 9 + md - 'A';
			modeB = md;
		}
	}
	adjust_bandwidth(modeB);
	return modeB;
}

void RIG_FTdx3000::set_bwA(int val)
{
	int bw_indx = bw_vals_[val];
	bwA = val;

	if (modeA == mFM || modeA == mAM || modeA == mFM_N || modeA == mPKT_FM ) {
		return;
	}
	if ((((modeA == mLSB || modeA == mUSB)  && val < 8)) ||
		 ((modeA == mCW  || modeA == mCW_R) && val < 11) ||
		 ((modeA == mRTTY_L || modeA == mRTTY_U) && val < 11) ||
		 ((modeA == mPKT_L  || modeA == mPKT_U)  && val < 11))  cmd = "NA01;";
	else cmd = "NA00;";

	cmd.append("SH0");
	cmd += '0' + bw_indx / 10;
	cmd += '0' + bw_indx % 10;
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET bw A", cmd, replystr);
	mode_bwA[modeA] = val;
}

int RIG_FTdx3000::get_bwA()
{
	if (modeA == mFM || modeA == mAM || modeA == mFM_N || modeA == mPKT_FM) {
		bwA = 0;
		mode_bwA[modeA] = bwA;
		return bwA;	
	} 
	cmd = rsp = "SH0";
	cmd += ';';
	wait_char(';', 6, 100, "get bw A", ASC);

	gett("get_bwA()");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return bwA;
	if (p + 5 >= replystr.length()) return bwA;

	replystr[p+5] = 0;
	int bw_idx = fm_decimal(replystr.substr(p+3), 2);
	const int *idx = bw_vals_;
	int i = 0;
	while (*idx != WVALS_LIMIT) {
		if (*idx == bw_idx) break;
		idx++;
		i++;
	}
	if (*idx == WVALS_LIMIT) i--;
	bwA = i;
	mode_bwA[modeA] = bwA;
	return bwA;
}

void RIG_FTdx3000::set_bwB(int val)
{
	int bw_indx = bw_vals_[val];
	bwB = val;

	if (modeB == mFM || modeB == mAM || modeB == mFM_N || modeB == mPKT_FM) {
		mode_bwB[modeB] = 0;
		return;
	}
	if ((((modeB == mLSB || modeB == mUSB)  && val < 8)) ||
		 ((modeB == mCW  || modeB == mCW_R) && val < 11) ||
		 ((modeB == mRTTY_L || modeB == mRTTY_U) && val < 11) ||
		 ((modeB == mPKT_L  || modeB == mPKT_U)  && val < 11))  cmd = "NA01;";
	else cmd = "NA00;";

	cmd.append("SH0");
	cmd += '0' + bw_indx / 10;
	cmd += '0' + bw_indx % 10;
	cmd += ';';
	sendCommand(cmd);
	mode_bwB[modeB] = bwB;
	showresp(WARN, ASC, "SET bw B", cmd, replystr);
}

int RIG_FTdx3000::get_bwB()
{
	if (modeB == mFM || modeB == mAM || modeB == mFM_N || modeB == mPKT_FM) {
		bwB = 0;
		mode_bwB[modeB] = bwB;
		return bwB;
	} 
	cmd = rsp = "SH0";
	cmd += ';';
	wait_char(';', 6, 100, "get bw B", ASC);

	gett("get_bwB()");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return bwB;
	if (p + 5 >= replystr.length()) return bwB;
	
	replystr[p+5] = 0;
	int bw_idx = fm_decimal(replystr.substr(p+3),2);
	const int *idx = bw_vals_;
	int i = 0;
	while (*idx != WVALS_LIMIT) {
		if (*idx == bw_idx) break;
		idx++;
		i++;
	}
	if (*idx == WVALS_LIMIT) i--;
	bwB = i;
	mode_bwB[modeB] = bwB;
	return bwB;
}

std::string RIG_FTdx3000::get_BANDWIDTHS()
{
	stringstream s;
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_bwA[i] << " ";
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_bwB[i] << " ";
	return s.str();
}

void RIG_FTdx3000::set_BANDWIDTHS(std::string s)
{
	stringstream strm;
	strm << s;
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_bwA[i];
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_bwB[i];
}

int RIG_FTdx3000::get_modetype(int n)
{
	return FTdx3000_mode_type[n];
}

void RIG_FTdx3000::set_if_shift(int val)
{
	cmd = "IS0+0000;";
	if (val != 0) progStatus.shift = true;
	else progStatus.shift = false;
	if (val < 0) cmd[3] = '-';
	val = abs(val);
	for (int i = 4; i > 0; i--) {
		cmd[3+i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET if shift", cmd, replystr);
}

bool RIG_FTdx3000::get_if_shift(int &val)
{
	cmd = rsp = "IS0";
	cmd += ';';
	wait_char(';', 9, 100, "get if shift", ASC);

	gett("get_if_shift()");

	size_t p = replystr.rfind(rsp);
	val = progStatus.shift_val;
	if (p == string::npos) return progStatus.shift;
	val = atoi(&replystr[p+4]);
	if (replystr[p+3] == '-') val = -val;
	return (val != 0);
}

void RIG_FTdx3000::get_if_min_max_step(int &min, int &max, int &step)
{
	if_shift_min = min = -1000;
	if_shift_max = max = 1000;
	if_shift_step = step = 50;
	if_shift_mid = 0;
}

void RIG_FTdx3000::set_notch(bool on, int val)
{
	if (on && !notch_on) {
		notch_on = true;
		cmd = "BP00001;";
		sendCommand(cmd);
		showresp(WARN, ASC, "SET notch on", cmd, replystr);
	} else if (!on && notch_on) {
		notch_on = false;
		cmd = "BP00000;";
		sendCommand(cmd);
		showresp(WARN, ASC, "SET notch off", cmd, replystr);
		return;
	}

	cmd = "BP01";
	val /= 10;
	cmd.append(to_decimal(val,3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET notch val", cmd, replystr);
}

bool  RIG_FTdx3000::get_notch(int &val)
{
	bool ison = false;
	cmd = rsp = "BP00";
	cmd += ';';
	wait_char(';', 8, 100, "get notch on/off", ASC);
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return ison;

	gett("get_notch()");

	if (replystr[p+6] == '1') // manual notch enabled
		ison = true;

	val = progStatus.notch_val;
	cmd = rsp = "BP01";
	cmd += ';';
	wait_char(';', 8, 100, "get notch val", ASC);

	gett("get_notch_val()");

	p = replystr.rfind(rsp);
	if (p == string::npos)
		val = 10;
	else
		val = fm_decimal(replystr.substr(p+4), 3) * 10;

	return (notch_on = ison);
}

void RIG_FTdx3000::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = 10;
	max = 3000;
	step = 10;
}

void RIG_FTdx3000::set_auto_notch(int v)
{
	cmd = "BC00;";
	if (v) cmd[3] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET auto notch", cmd, replystr);
}

int  RIG_FTdx3000::get_auto_notch()
{
	cmd = "BC0;";
	wait_char(';', 5, 100, "get auto notch", ASC);

	gett("get_auto_notch()");

	size_t p = replystr.rfind("BC0");
	if (p == string::npos) return 0;
	if (replystr[p+3] == '1') return 1;
	return 0;
}

int FTdx3000_blanker_level = 0;

void RIG_FTdx3000::set_noise(bool b)
{
	cmd = "NB00;";
	if (FTdx3000_blanker_level == 0) {
		FTdx3000_blanker_level = 1;
		nb_label("NB 1", true);
	} else if (FTdx3000_blanker_level == 1) {
		FTdx3000_blanker_level = 2;
		nb_label("NB 2", true);
	} else if (FTdx3000_blanker_level == 2) {
		FTdx3000_blanker_level = 0;
		nb_label("NB", false);
	}
	cmd[3] = '0' + FTdx3000_blanker_level;
	sendCommand (cmd);
	showresp(WARN, ASC, "SET NB", cmd, replystr);
}

int RIG_FTdx3000::get_noise()
{
	cmd = rsp = "NB0";
	cmd += ';';
	wait_char(';', 5, 100, "get NB", ASC);

	gett("get_noise()");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return FTdx3000_blanker_level;

	FTdx3000_blanker_level = replystr[p+3] - '0';
	if (FTdx3000_blanker_level == 1) {
		nb_label("NB 1", true);
	} else if (FTdx3000_blanker_level == 2) {
		nb_label("NB 2", true);
	} else {
		nb_label("NB", false);
		FTdx3000_blanker_level = 0;
	}
	return FTdx3000_blanker_level;
}

// val 0 .. 100
void RIG_FTdx3000::set_mic_gain(int val)
{
	cmd = "MG000;";
	val = (int)(val * 255 / 100); // convert to 0 .. 255
	for (int i = 3; i > 0; i--) {
		cmd[1+i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mic", cmd, replystr);
}

int RIG_FTdx3000::get_mic_gain()
{
	cmd = rsp = "MG";
	cmd += ';';
	wait_char(';', 6, 100, "get mic", ASC);

	gett("get_mic_gain()");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.mic_gain;
	int val = atoi(&replystr[p+2]);
	return val * 100 / 255;
}

void RIG_FTdx3000::get_mic_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_FTdx3000::set_rf_gain(int val)
{
	cmd = "RG0000;";
	int rfval = val * 250 / 100;
	for (int i = 5; i > 2; i--) {
		cmd[i] = rfval % 10 + '0';
		rfval /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET rfgain", cmd, replystr);
}

int  RIG_FTdx3000::get_rf_gain()
{
	int rfval = 0;
	cmd = rsp = "RG0";
	cmd += ';';
	wait_char(';', 7, 100, "get rfgain", ASC);

	gett("get_rf_gain()");

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.rfgain;
	for (int i = 3; i < 6; i++) {
		rfval *= 10;
		rfval += replystr[p+i] - '0';
	}
	rfval = rfval * 100 / 250;
	if (rfval > 100) rfval = 100;
	return rfval;
}

void RIG_FTdx3000::get_rf_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_FTdx3000::set_vox_onoff()
{
	cmd = "VX0;";
	if (progStatus.vox_onoff) cmd[2] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox", cmd, replystr);
}

void RIG_FTdx3000::set_vox_gain()
{
	cmd = "VG";
	cmd.append(to_decimal(progStatus.vox_gain, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox gain", cmd, replystr);
}

void RIG_FTdx3000::set_vox_anti()
{
	cmd = "EX175";
	cmd.append(to_decimal(progStatus.vox_anti, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET anti-vox", cmd, replystr);
}

void RIG_FTdx3000::set_vox_hang()
{
	cmd = "VD";
	cmd.append(to_decimal(progStatus.vox_hang, 4)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox delay", cmd, replystr);
}

void RIG_FTdx3000::set_vox_on_dataport()
{
	cmd = "EX1140;";
	if (progStatus.vox_on_dataport) cmd[5] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox on data port", cmd, replystr);
}

void RIG_FTdx3000::set_cw_wpm()
{
	cmd = "KS";
	if (progStatus.cw_wpm > 60) progStatus.cw_wpm = 60;
	if (progStatus.cw_wpm < 4) progStatus.cw_wpm = 4;
	cmd.append(to_decimal(progStatus.cw_wpm, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw wpm", cmd, replystr);
}


void RIG_FTdx3000::enable_keyer()
{
	cmd = "KR0;";
	if (progStatus.enable_keyer) cmd[2] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET keyer on/off", cmd, replystr);
}

bool RIG_FTdx3000::set_cw_spot()
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

void RIG_FTdx3000::set_cw_weight()
{
	int n = round(progStatus.cw_weight * 10);
	cmd.assign("EX065").append(to_decimal(n, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw weight", cmd, replystr);
}

void RIG_FTdx3000::set_cw_qsk()
{
	int n = progStatus.cw_qsk / 5 - 3;
	cmd.assign("EX068").append(to_decimal(n, 1)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw qsk", cmd, replystr);
}

/*
void RIG_FTdx3000::set_cw_spot_tone()
{
	int n = (progStatus.cw_spot_tone - 300) / 50;
	cmd = "EX045";
	cmd.append(to_decimal(n, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw tone", cmd, replystr);
}

void RIG_FTdx3000::set_cw_vol()
{
}
*/
