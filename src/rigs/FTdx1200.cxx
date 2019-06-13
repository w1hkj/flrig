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

#include "FTdx1200.h"
#include "debug.h"
#include "support.h"

enum mFTdx1200 {
  mLSB, mUSB, mCW, mFM, mAM, mRTTY_L, mCW_R, mDATA_L, mRTTY_U, mFM_N, mDATA_U };
//  0,    1,    2,   3,   4,   5,       6,     7,      8,       9,       10   	// mode index

static const char FTdx1200name_[] = "FTdx1200";

#undef  NUM_MODES
#define NUM_MODES  11

static int mode_bwA[NUM_MODES] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
static int mode_bwB[NUM_MODES] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

static const char *FTdx1200modes_[] = {
"LSB", "USB", "CW", "FM", "AM", "RTTY-L", "CW-R", "DATA-L", "RTTY-U", "FM-N", "DATA-U", NULL};

static const char FTdx1200_mode_chr[] =  { '1', '2', '3', '4', '5', '6', '7', '8', '9', 'B', 'C' };
static const char FTdx1200_mode_type[] = { 'L', 'U', 'L', 'U', 'U', 'L', 'U', 'L', 'U', 'U', 'U' };
//                                           lsb   usb  cw   fm   am  ttyL cwR  datL ttyU fmN  datU

static const int FTdx1200_def_bw[] = {
    18,   18,   10,   0,   0,   6,       10,     15,     6,        0,     15    };
// mLSB, mUSB, mCW, mFM, mAM, mRTTY_L, mCW_R, mDATA_L, mRTTY_U, mFM_N, mDATA_U

static const char *FTdx1200_widths_SSB[] = {
 "200",  "400",  "600",  "850", "1100",
"1350", "1500", "1650", "1800", "1950",
"2100", "2250", "2400", "2500", "2600",
"2700", "2800", "2900", "3000", "3200",
"3400", "3800", "4000", NULL }; // def_bw = 18

static int FTdx1200_wvals_SSB[] = {
1,2,3,4,5,6,7,8,9,10,11,12,13,15,16,17,18,19,20,21,22,23, WVALS_LIMIT};

static const char *FTdx1200_widths_SSBD[] = {
  "50",  "150",  "100",  "150",  "200",
 "250",  "300",  "350",  "400",  "450",
 "500",  "800", "1200", "1400", "1700",
"2000", "2400", NULL }; // def_bw = 16 for DATA, 6 for RTTY

static int FTdx1200_wvals_SSBD[] = {
1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, WVALS_LIMIT};

static const char *FTdx1200_widths_CW[] = {
  "50",  "150",  "100",  "150",  "200",
 "250",  "300",  "350",  "400",  "450",
 "500",  "800", "1200", "1400", "1700",
"2000", "2400", NULL }; // def_bw = 10

static int FTdx1200_wvals_CW[] = {
1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, WVALS_LIMIT};

// Single bandwidth modes
static const char *FTdx1200_widths_FMnar[] = { "NARR", NULL };
static const char *FTdx1200_widths_AMFM[]  = { "NORM", NULL };

static const int FTdx1200_wvals_AMFM[] = { 0, WVALS_LIMIT };

// US 60M 5-USB, 5-CW
static const char *US_60m_chan[]  = {"000","125","126","127","128","130","141","142","143","144","146",NULL};
static const char *US_60m_label[] = {"VFO","U51","U52","U53","U54","U55","U56","U57","U58","U59","U50",NULL};

// UK 60m channel numbers by Brian, G8SEZ
static const char *UK_60m_chan[]  = {"000","118","120","121","127","128","129","130",NULL};
static const char *UK_60m_label[] = {"VFO","U51","U52","U53","U54","U55","U56","U57",NULL};

static const char **Channels_60m = US_60m_chan;
static const char **label_60m    = US_60m_label;

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

RIG_FTdx1200::RIG_FTdx1200() {
// base class values
	IDstr = "ID";
	name_ = FTdx1200name_;
	modes_ = FTdx1200modes_;
	bandwidths_ = FTdx1200_widths_SSB;
	bw_vals_ = FTdx1200_wvals_SSB;

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
	A.iBW = B.iBW = bwA = bwB = def_bw = 12;
	A.freq = B.freq = freqA = freqB = def_freq = 14070000;

	has_compON =
	has_compression =
	has_a2b =
	has_xcvr_auto_on_off =
	has_split =
	has_split_AB =
	has_band_selection =
	has_noise_reduction =
	has_noise_reduction_control =
	has_extras =
	has_vox_onoff =
	has_vox_gain =
	has_vox_anti =
	has_vox_hang =
	has_vox_on_dataport =

	has_vfo_adj =

	has_cw_wpm =
	has_cw_keyer =
//	has_cw_vol =
	has_cw_spot =
	has_cw_spot_tone =
	has_cw_qsk =
	has_cw_weight =

	can_change_alt_vfo =
	has_smeter =
	has_alc_control =
	has_swr_control =
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
	atten_level = 3;
	preamp_level = 2;
	notch_on = false;
	m_60m_indx = 0;

	precision = 1;
	ndigits = 8;

}

void RIG_FTdx1200::initialize()
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
	rig_widgets[9].W = btnNR;
	rig_widgets[10].W = sldrNR;

// set progStatus defaults
	if (progStatus.notch_val < 10) progStatus.notch_val = 1500;
	if (progStatus.noise_reduction_val < 1) progStatus.noise_reduction_val = 1;
	if (progStatus.power_level < 5) progStatus.power_level = 5;
// first-time-thru, or reset
	if (progStatus.cw_qsk < 15) {
		progStatus.cw_qsk = 15;
		progStatus.cw_spot_tone = 700;
		progStatus.cw_weight = 3.0;
		progStatus.cw_wpm = 18;
		progStatus.vox_on_dataport = false;
		progStatus.vox_gain = 50;
		progStatus.vox_anti = 50;
		progStatus.vox_hang = 500;
	}
// Disable Auto Information mode
	sendCommand("AI0;");

// "MRnnn;" if valid, returns last channel used, "mrlll...;", along with channel nnn info.
	cmd = "MR118;";
	wait_char(';', 27, 100, "Read UK 60m Channel Mem", ASC);
	size_t p = replystr.rfind("MR");
	if (p == string::npos) {
		Channels_60m = US_60m_chan;
		label_60m    = US_60m_label;
		op_yaesu_select60->clear();
		char **p = (char **)US_60m_label;
		while (*p) op_yaesu_select60->add(*p++);
	}
	else {
		Channels_60m = UK_60m_chan;
		label_60m    = UK_60m_label;
		op_yaesu_select60->clear();
		char **p = (char **)UK_60m_label;
		while (*p) op_yaesu_select60->add(*p++);
	}
	op_yaesu_select60->index(m_60m_indx);

//	cmd = "EX035;";
//	wait_char(';', 11,100,"Read Vfo Adjust", ASC);
//	size_t p = replystr.rfind("EX035");

}

void RIG_FTdx1200::post_initialize()
{
	enable_yaesu_bandselect(12, false);
	enable_yaesu_bandselect(13, true);
}

void RIG_FTdx1200::shutdown()
{
}

bool RIG_FTdx1200::check ()
{
	cmd = "FA;";
	rsp = "FA";
	int ret = wait_char(';', 11, 100, "check", ASC);

	get_trace(3, "check()", cmd.c_str(), replystr.c_str());

	if (ret >= 11) return true;
	return false;
}

long RIG_FTdx1200::get_vfoA ()
{
	cmd = "FA;";
	rsp = "FA";
	wait_char(';', 11, 100, "get vfo A", ASC);

	get_trace(3, "get_vfoA()", cmd.c_str(), replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return freqA;
	int f = 0;
	for (size_t n = 2; n < 10; n++)
		f = f*10 + replystr[p+n] - '0';
	freqA = f;
	return freqA;
}

void RIG_FTdx1200::set_vfoA (long freq)
{
	freqA = freq;
	cmd = "FA00000000;";
	for (int i = 9; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vfo A", cmd, replystr);
	set_trace(3,"set_vfoA", cmd.c_str(), replystr.c_str());
}

long RIG_FTdx1200::get_vfoB ()
{
	cmd = "FB;";
	rsp = "FB";
	wait_char(';', 11, 100, "get vfo B", ASC);

	get_trace(3, "get_vfoB()", cmd.c_str(), replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return freqB;
	int f = 0;
	for (size_t n = 2; n < 10; n++)
		f = f*10 + replystr[p+n] - '0';
	freqB = f;
	return freqB;
}


void RIG_FTdx1200::set_vfoB (long freq)
{
	freqB = freq;
	cmd = "FB00000000;";
	for (int i = 9; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vfo B", cmd, replystr);
	set_trace(3,"set_vfoB", cmd.c_str(), replystr.c_str());
}

void RIG_FTdx1200::setVfoAdj(double v)
{
	char cmdstr[20];
	snprintf(cmdstr, sizeof(cmdstr), "EX035%+03d;", (int)v);
	cmd = cmdstr;
	sendCommand(cmd);
	set_trace(3,"setVfoAdj", cmd.c_str(), replystr.c_str());
}

double RIG_FTdx1200::getVfoAdj()
{
	cmd = rsp = "EX035";
	sendCommand(cmd.append(";"));
	wait_char(';', 9, 100, "get Vfo Adjust", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	return (double)(atoi(&replystr[p+5]));
}

void RIG_FTdx1200::get_vfoadj_min_max_step(int &min, int &max, int &step)
{
	min = -25;
	max = 25;
	step = 1;
}

bool RIG_FTdx1200::twovfos()
{
	return true;
}

void RIG_FTdx1200::selectA()
{
	cmd = "FR0;FT2;";
	sendCommand(cmd);
	showresp(WARN, ASC, "select A", cmd, replystr);
	set_trace(3,"selectA", cmd.c_str(), replystr.c_str());
}

void RIG_FTdx1200::selectB()
{
	cmd = "FR4;FT3;";
	sendCommand(cmd);
	showresp(WARN, ASC, "select B", cmd, replystr);
	set_trace(3,"selectB", cmd.c_str(), replystr.c_str());
}

void RIG_FTdx1200::A2B()
{
	cmd = "AB;";
	sendCommand(cmd);
	showresp(WARN, ASC, "vfo A --> B", cmd, replystr);
	set_trace(3,"A2B", cmd.c_str(), replystr.c_str());
}

bool RIG_FTdx1200::can_split()
{
	return true;
}

void RIG_FTdx1200::set_split(bool val)
{
	split = val;
	if (useB) {
		if (val) {
			cmd = "FR4;FT2;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on B, Tx on A", cmd, replystr);
			set_trace(3,"set_split", cmd.c_str(), replystr.c_str());
		} else {
			cmd = "FR4;FT3;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on B, Tx on B", cmd, replystr);
			set_trace(3,"set_split", cmd.c_str(), replystr.c_str());
		}
	} else {
		if (val) {
			cmd = "FR0;FT3;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on A, Tx on B", cmd, replystr);
			set_trace(3,"set_split", cmd.c_str(), replystr.c_str());
		} else {
			cmd = "FR0;FT2;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on A, Tx on A", cmd, replystr);
			set_trace(3,"set_split", cmd.c_str(), replystr.c_str());
		}
	}
	Fl::awake(highlight_vfo, (void *)0);
}

int RIG_FTdx1200::get_split()
{
	size_t p;
	int split = 0;
	char rx, tx;
// tx vfo
	cmd = rsp = "FT";
	cmd.append(";");
	wait_char(';', 4, 100, "get split tx vfo", ASC);
	p = replystr.rfind(rsp);
	if (p == string::npos) return false;
	tx = replystr[p+2] - '0';

// rx vfo
	cmd = rsp = "FR";
	cmd.append(";");
	wait_char(';', 4, 100, "get split rx vfo", ASC);

	p = replystr.rfind(rsp);
	if (p == string::npos) return false;
	rx = replystr[p+2] - '0';

	split = (tx == 1 ? 2 : 0) + (rx >= 4 ? 1 : 0);

	return split;
}


int RIG_FTdx1200::get_smeter()
{
	cmd = rsp = "SM0";
	cmd += ';';
	wait_char(';', 7, 100, "get smeter", ASC);

	get_trace(3, "get_smeter()", cmd.c_str(), replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	if (p + 6 >= replystr.length()) return 0;
	int mtr = atoi(&replystr[p+3]);
	mtr = mtr * 100.0 / 256.0;
	return mtr;
}

int RIG_FTdx1200::get_swr()
{
	cmd = rsp = "RM6";
	cmd += ';';
	wait_char(';', 7, 100, "get swr", ASC);

	get_trace(3, "get_swr()", cmd.c_str(), replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	if (p + 6 >= replystr.length()) return 0;
	int mtr = atoi(&replystr[p+3]);
	return (int)ceil(mtr / 2.56);
}

int RIG_FTdx1200::get_alc()
{
	cmd = rsp = "RM4";
	cmd += ';';
	wait_char(';', 7, 100, "get alc", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	if (p + 6 >= replystr.length()) return 0;
	int mtr = atoi(&replystr[p+3]);
	return (int)ceil(mtr / 2.56);
}

int RIG_FTdx1200::get_power_out()
{
	cmd = rsp = "RM5";
	sendCommand(cmd.append(";"));
	wait_char(';', 7, 100, "get pout", ASC);

	get_trace(3, "get_power_out()", cmd.c_str(), replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	if (p + 6 >= replystr.length()) return 0;
	double mtr = (double)(atoi(&replystr[p+3]));

// following conversion iaw data measured by Terry, KJ4EED
	mtr = (.06 * mtr) + (.002 * mtr * mtr);

	return (int)ceil(mtr);
}

// Transceiver power level
int RIG_FTdx1200::get_power_control()
{
	cmd = rsp = "PC";
	cmd += ';';
	wait_char(';', 6, 100, "get power", ASC);

	get_trace(3, "get_power_control()", cmd.c_str(), replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.power_level;
	if (p + 5 >= replystr.length()) return progStatus.power_level;

	int mtr = atoi(&replystr[p+2]);
	return mtr;
}

void RIG_FTdx1200::set_power_control(double val)
{
	int ival = (int)val;
	cmd = "PC000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ival % 10;
		ival /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET power", cmd, replystr);
	set_trace(3,"set_power_control", cmd.c_str(), replystr.c_str());
}

// Volume control return 0 ... 100
int RIG_FTdx1200::get_volume_control()
{
	cmd = rsp = "AG0";
	cmd += ';';
	wait_char(';', 7, 100, "get vol", ASC);

	get_trace(3, "get_volume_control()", cmd.c_str(), replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.volume;
	if (p + 6 >= replystr.length()) return progStatus.volume;
	int val = round(atoi(&replystr[p+3]) / 2.55);
	if (val > 100) val = 100;
	return ceil(val);
}

void RIG_FTdx1200::set_volume_control(int val)
{
	int ivol = (int)(val * 2.55);
	cmd = "AG0000;";
	for (int i = 5; i > 2; i--) {
		cmd[i] += ivol % 10;
		ivol /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vol", cmd, replystr);
	set_trace(3,"set_volume_control", cmd.c_str(), replystr.c_str());
}

// Tranceiver PTT on/off
void RIG_FTdx1200::set_PTT_control(int val)
{
	cmd = val ? "TX1;" : "TX0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "SET PTT", cmd, replystr);
	ptt_ = val;
	set_trace(3,"set_PTT_control", cmd.c_str(), replystr.c_str());
}

int RIG_FTdx1200::get_PTT()
{
	cmd = "TX;";
	rsp = "TX";
	wait_char(';', 4, 100, "get PTT", ASC);

	get_trace(3, "get_PTT()", cmd.c_str(), replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return ptt_;
	ptt_ =  (replystr[p+2] != '0' ? 1 : 0);
	return ptt_;
}


void RIG_FTdx1200::tune_rig(int val)
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

int RIG_FTdx1200::get_tune()
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

/*
// internal or external tune mode
void RIG_FTdx1200::tune_rig(int)
{
	static bool extun_on = false;
	static int rmd = modeA;
	static int rbw = bwA;
	static int rpwr = 100;
	static long rfreq = freqA;	// fix for menu 047 OFSt default
	int use_int_tuner = true;

//  On-The-Fly bandstack by bandstack int/ext tuner
//  if rig "Tuner" light is on internal else external
	cmd = rsp = "AC";
	cmd.append(";");
	wait_char(';', 6, 100, "is Int. Tuner Enabled", ASC);
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return;
	if ((p + 5) >= replystr.length()) return;
	if (replystr[p+4] == '0') {
		use_int_tuner = false;
	}

	if (use_int_tuner) {
		cmd = "AC002;";
		sendCommand(cmd);
		showresp(WARN, ASC, "tune rig", cmd, replystr);
	} else {
		if (extun_on == false) {
			if (btnPTT->value() == true) return;
			if (get_split() == 1 || get_split() == 2) return;	// no split extune
			useB ? rmd = modeB : rmd = modeA;
			useB ? rbw = bwB : rbw = bwA;
			useB ? rfreq = freqB : rfreq = freqA;
			useB ? set_modeB(mAM) : set_modeA(mAM);
			useB ? set_vfoB(rfreq) : set_vfoA(rfreq);
			rpwr = get_power_control();
			set_power_control(10);
			sendCommand("EX0360000;");	// AM mic off
//			sendCommand("EX0560000;");	// FM mic off
			extun_on = true;
			btnPTT->value(true);
			sendCommand("MX1;");
		} else {
			extun_on = false;
			btnPTT->value(false);
			sendCommand("MX0;");
			sendCommand("EX0361000;");	// AM mic default
//			sendCommand("EX0560050;");	// FM mic default
			set_power_control(rpwr);
			useB ? set_modeB(rmd) : set_modeA(rmd);
			useB ? set_bwB(rbw) : set_bwA(rbw);
			useB ? set_vfoB(rfreq) : set_vfoA(rfreq);
		}
	}
}
*/

int  RIG_FTdx1200::next_attenuator()
{
	switch (atten_level) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 3;
		case 3: return 0;
	}
	return 0;
}

void RIG_FTdx1200::set_attenuator(int val)
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
	set_trace(3,"set_attenuator", cmd.c_str(), replystr.c_str());
}

int RIG_FTdx1200::get_attenuator()
{
	cmd = rsp = "RA0";
	cmd += ';';
	wait_char(';', 5, 100, "get att", ASC);

	get_trace(3, "get_attenuator()", cmd.c_str(), replystr.c_str());

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

int RIG_FTdx1200::next_preamp()
{
	switch (preamp_level) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 0;
	}
	return 0;
}

void RIG_FTdx1200::set_preamp(int val)
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
	set_trace(3,"set_preamp", cmd.c_str(), replystr.c_str());
}

int RIG_FTdx1200::get_preamp()
{
	cmd = rsp = "PA0";
	cmd += ';';
	wait_char(';', 5, 100, "get pre", ASC);

	get_trace(3, "get_preamp()", cmd.c_str(), replystr.c_str());

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

int RIG_FTdx1200::adjust_bandwidth(int val)
{
	switch (val) {
		case mCW     :
		case mCW_R   :
			bandwidths_ = FTdx1200_widths_CW;
			bw_vals_ = FTdx1200_wvals_CW;
			break;
		case mFM     :
		case mAM     :
			bandwidths_ = FTdx1200_widths_AMFM;
			bw_vals_    = FTdx1200_wvals_AMFM;
			break;
		case mFM_N   :
			bandwidths_ = FTdx1200_widths_FMnar;
			bw_vals_    = FTdx1200_wvals_AMFM;
			break;
		case mDATA_L :
		case mDATA_U :
		case mRTTY_L :
		case mRTTY_U :
			bandwidths_ = FTdx1200_widths_SSBD;
			bw_vals_ = FTdx1200_wvals_SSBD;
			break;
		default:
			bandwidths_ = FTdx1200_widths_SSB;
			bw_vals_ = FTdx1200_wvals_SSB;
	}
	return FTdx1200_def_bw[val];
}

int RIG_FTdx1200::def_bandwidth(int m)
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

const char ** RIG_FTdx1200::bwtable(int n)
{
	switch (n) {
		case mFM     : return FTdx1200_widths_AMFM;
		case mAM     : return FTdx1200_widths_AMFM;
		case mFM_N   : return FTdx1200_widths_FMnar;
		case mCW     : return FTdx1200_widths_CW;
		case mCW_R   : return FTdx1200_widths_CW;
		case mRTTY_L : return FTdx1200_widths_SSBD;
		case mRTTY_U : return FTdx1200_widths_SSBD;
		case mDATA_L : return FTdx1200_widths_SSBD;
		case mDATA_U : return FTdx1200_widths_SSBD;
	}
	return FTdx1200_widths_SSB;
}

void RIG_FTdx1200::set_modeA(int val)
{
	modeA = val;
	cmd = "MD0";
	cmd += FTdx1200_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mode A", cmd, replystr);
	set_trace(3,"set_modeA", cmd.c_str(), replystr.c_str());
	adjust_bandwidth(modeA);
	if (val != mCW && val != mCW_R) return;
	if (progStatus.spot_onoff) {
		progStatus.spot_onoff = false;
		set_spot_onoff();
		cmd = "CS0;";
		sendCommand(cmd);
		showresp(WARN, ASC, "SET spot off", cmd, replystr);
		btnSpot->value(0);
	}
}

int RIG_FTdx1200::get_modeA()
{
	cmd = rsp = "MD0";
	cmd += ';';
	wait_char(';', 5, 100, "get mode A", ASC);

	get_trace(3, "get_modeA()", cmd.c_str(), replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p != string::npos) {
		if (p + 3 < replystr.length()) {
			int md = replystr[p+3];
			if (md <= '9') md -= '1';
			else md = md - 'B' + 9;
			modeA = md;
		}
	}
	adjust_bandwidth(modeA);
	return modeA;
}

void RIG_FTdx1200::set_modeB(int val)
{
	modeB = val;
	cmd = "MD0";
	cmd += FTdx1200_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mode B", cmd, replystr);
	set_trace(3,"set_modeB", cmd.c_str(), replystr.c_str());
	adjust_bandwidth(modeB);
	if (val != mCW && val != mCW_R) return;
	if (progStatus.spot_onoff) {
		progStatus.spot_onoff = false;
		set_spot_onoff();
		cmd = "CS0;";
		sendCommand(cmd);
		showresp(WARN, ASC, "SET spot off", cmd, replystr);
		btnSpot->value(0);
	}
}

int RIG_FTdx1200::get_modeB()
{
	cmd = rsp = "MD0";
	cmd += ';';
	wait_char(';', 5, 100, "get mode B", ASC);

	get_trace(3, "get_modeB()", cmd.c_str(), replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p != string::npos) {
		if (p + 3 < replystr.length()) {
			int md = replystr[p+3];
			if (md <= '9') md -= '1';
			else md = md - 'B' + 9;
			modeB = md;
		}
	}
	adjust_bandwidth(modeB);
	return modeB;
}

void RIG_FTdx1200::set_bwA(int val)
{
	int bw_indx = bw_vals_[val];
	bwA = val;

	int md = modeA;
	if (md == mFM || md == mAM || md == mFM_N ) return;
	cmd = "NA00;";
	switch (md) {
		case mLSB : case mUSB :
			if (val < 8 ) ++cmd[3];
			break;
		case mCW : case mCW_R :
			if (val < 9) ++cmd[3];
			break;
		case mRTTY_L : case mRTTY_U :
		case mDATA_L : case mDATA_U :
			if (val < 9) ++cmd[3];
			break;
	}

	cmd.append("SH0");
	cmd += '0' + bw_indx / 10;
	cmd += '0' + bw_indx % 10;
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET bw A", cmd, replystr);
	mode_bwA[modeA] = val;
	set_trace(3,"set_bwA", cmd.c_str(), replystr.c_str());
}

int RIG_FTdx1200::get_bwA()
{
	size_t p;
	if (modeA == mFM || modeA == mAM || modeA == mFM_N) {
		bwA = 0;
		mode_bwA[modeA] = bwA;
		return bwA;
	}
	cmd = rsp = "SH0";
	cmd += ';';
	wait_char(';', 6, 100, "get bw A", ASC);

	get_trace(3, "get_bwA()", cmd.c_str(), replystr.c_str());

	p = replystr.rfind(rsp);
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

void RIG_FTdx1200::set_bwB(int val)
{
	int bw_indx = bw_vals_[val];
	bwB = val;

	int md = modeB;
	if (md == mFM || md == mAM || md == mFM_N ) return;
	cmd = "NA00;";
	switch (md) {
		case mLSB : case mUSB :
			if (val < 8 ) ++cmd[3];
			break;
		case mCW : case mCW_R :
			if (val < 9) ++cmd[3];
			break;
		case mRTTY_L : case mRTTY_U :
		case mDATA_L : case mDATA_U :
			if (val < 9) ++cmd[3];
			break;
	}

	cmd.append("SH0");
	cmd += '0' + bw_indx / 10;
	cmd += '0' + bw_indx % 10;
	cmd += ';';
	sendCommand(cmd);
	mode_bwB[modeB] = bwB;
	showresp(WARN, ASC, "SET bw B", cmd, replystr);
	set_trace(3,"set_bwB", cmd.c_str(), replystr.c_str());
}

int RIG_FTdx1200::get_bwB()
{
	size_t p;
	if (modeB == mFM || modeB == mAM || modeB == mFM_N) {
		bwB = 0;
		mode_bwB[modeB] = bwB;
		return bwB;
	}
	cmd = rsp = "SH0";
	cmd += ';';
	wait_char(';', 6, 100, "get bw B", ASC);

	get_trace(3, "get_bwB()", cmd.c_str(), replystr.c_str());

	p = replystr.rfind(rsp);
	if (p == string::npos) return bwB;
	if (p + 5 >= replystr.length()) return bwB;

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
	bwB = i;
	mode_bwB[modeB] = bwB;
	return bwB;
}

std::string RIG_FTdx1200::get_BANDWIDTHS()
{
	stringstream s;
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_bwA[i] << " ";
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_bwB[i] << " ";
	return s.str();
}

void RIG_FTdx1200::set_BANDWIDTHS(std::string s)
{
	stringstream strm;
	strm << s;
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_bwA[i];
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_bwB[i];
}


int RIG_FTdx1200::get_modetype(int n)
{
	return FTdx1200_mode_type[n];
}

void RIG_FTdx1200::set_if_shift(int val)
{
	cmd = "IS0+0000;";
	if (val < 0) cmd[3] = '-';
	val = abs(val);
	for (int i = 4; i > 0; i--) {
		cmd[3+i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET if shift", cmd, replystr);
	set_trace(3,"set_if_shift", cmd.c_str(), replystr.c_str());
}

bool RIG_FTdx1200::get_if_shift(int &val)
{
	cmd = rsp = "IS0";
	cmd += ';';
	wait_char(';', 9, 100, "get if shift", ASC);

	get_trace(3, "get_if_shift()", cmd.c_str(), replystr.c_str());

	size_t p = replystr.rfind(rsp);
	val = progStatus.shift_val;
	if (p == string::npos) return progStatus.shift;
	val = atoi(&replystr[p+4]);
	if (replystr[p+3] == '-') val = -val;
	return (val != 0);
}

void RIG_FTdx1200::get_if_min_max_step(int &min, int &max, int &step)
{
	if_shift_min = min = -1000;
	if_shift_max = max = 1000;
	if_shift_step = step = 20;
	if_shift_mid = 0;
}

void RIG_FTdx1200::set_notch(bool on, int val)
{
// set notch frequency
	if (on) {
		cmd = "BP00001;";
		sendCommand(cmd);
		showresp(WARN, ASC, "SET notch on", cmd, replystr);

		set_trace(3,"set_notch on", cmd.c_str(), replystr.c_str());

		cmd = "BP01000;";
		if (val % 10 >= 5) val += 10;
		val /= 10;
		for (int i = 3; i > 0; i--) {
			cmd[3 + i] += val % 10;
			val /=10;
		}
		sendCommand(cmd);
		showresp(WARN, ASC, "SET notch val", cmd, replystr);

		set_trace(3,"set_notch val", cmd.c_str(), replystr.c_str());

		return;
	}

// set notch off
	cmd = "BP00000;";
	sendCommand(cmd);
	set_trace(3,"set_notch off", cmd.c_str(), replystr.c_str());
	showresp(WARN, ASC, "SET notch off", cmd, replystr);
}

bool  RIG_FTdx1200::get_notch(int &val)
{
	bool ison = false;
	cmd = rsp = "BP00";
	cmd += ';';
	wait_char(';', 8, 100, "get notch on/off", ASC);

	get_trace(3, "get_notch()", cmd.c_str(), replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return ison;

	if (replystr[p+6] == '1') { // manual notch enabled
		ison = true;
		val = progStatus.notch_val;
		cmd = rsp = "BP01";
		cmd += ';';
		wait_char(';', 8, 100, "get notch val", ASC);

		get_trace(3, "get_notch_val()", cmd.c_str(), replystr.c_str());
		p = replystr.rfind(rsp);
		if (p == string::npos)
			val = 10;
		else
			val = fm_decimal(replystr.substr(p+4), 3) * 10;
	}
	return ison;
}

void RIG_FTdx1200::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = 10;
	max = 3000;
	step = 10;
}

void RIG_FTdx1200::set_auto_notch(int v)
{
	cmd.assign("BC0").append(v ? "1" : "0" ).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET DNF Auto Notch Filter", cmd, replystr);
	set_trace(3,"set_auto_notch", cmd.c_str(), replystr.c_str());
}

int  RIG_FTdx1200::get_auto_notch()
{
	cmd = "BC0;";
	wait_char(';', 5, 100, "get auto notch", ASC);

	get_trace(3, "get_auto_notch()", cmd.c_str(), replystr.c_str());

	size_t p = replystr.rfind("BC0");
	if (p == string::npos) return 0;
	if (replystr[p+3] == '1') return 1;
	return 0;
}

int FTdx1200_blanker_level = 2;

void RIG_FTdx1200::set_noise(bool b)
{
	cmd = "NB00;";
	if (FTdx1200_blanker_level == 0) {
		FTdx1200_blanker_level = 1;
		nb_label("NB 1", true);
	} else if (FTdx1200_blanker_level == 1) {
		FTdx1200_blanker_level = 2;
		nb_label("NB 2", true);
	} else if (FTdx1200_blanker_level == 2) {
		FTdx1200_blanker_level = 0;
		nb_label("NB", false);
	}
	cmd[3] = '0' + FTdx1200_blanker_level;
	sendCommand (cmd);
	showresp(WARN, ASC, "SET NB", cmd, replystr);
	set_trace(3,"set_noise", cmd.c_str(), replystr.c_str());
}

int RIG_FTdx1200::get_noise()
{
	cmd = rsp = "NB0";
	cmd += ';';
	wait_char(';', 5, 100, "get NB", ASC);

	get_trace(3, "get_noise()", cmd.c_str(), replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return FTdx1200_blanker_level;

	FTdx1200_blanker_level = replystr[p+3] - '0';
	if (FTdx1200_blanker_level == 1) {
		nb_label("NB 1", true);
	} else if (FTdx1200_blanker_level == 2) {
		nb_label("NB 2", true);
	} else {
		nb_label("NB", false);
		FTdx1200_blanker_level = 0;
	}
	return FTdx1200_blanker_level;
}

// val 0 .. 100
void RIG_FTdx1200::set_mic_gain(int val)
{
	cmd = "MG000;";
	val = (int)(val * 2.50);
	for (int i = 3; i > 0; i--) {
		cmd[1+i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mic", cmd, replystr);
	set_trace(3,"set_mic_gain", cmd.c_str(), replystr.c_str());
}

int RIG_FTdx1200::get_mic_gain()
{
	cmd = rsp = "MG";
	cmd += ';';
	wait_char(';', 6, 100, "get mic", ASC);

	get_trace(3, "get_mic_gain()", cmd.c_str(), replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.mic_gain;
	int val = atoi(&replystr[p+2]);
	val = (int)(val / 2.50);
	if (val > 100) val = 100;
	return ceil(val);
}

void RIG_FTdx1200::get_mic_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_FTdx1200::set_rf_gain(int val)
{
	cmd = "RG0000;";
	int rfval = (int)(val * 2.50);
	for (int i = 5; i > 2; i--) {
		cmd[i] = rfval % 10 + '0';
		rfval /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET rfgain", cmd, replystr);
	set_trace(3,"set_rf_gain", cmd.c_str(), replystr.c_str());
}

int  RIG_FTdx1200::get_rf_gain()
{
	int rfval = 0;
	cmd = rsp = "RG0";
	cmd += ';';
	wait_char(';', 7, 100, "get rfgain", ASC);

	get_trace(3, "get_rf_gain()", cmd.c_str(), replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.rfgain;
	for (int i = 3; i < 6; i++) {
		rfval *= 10;
		rfval += replystr[p+i] - '0';
	}
	rfval = (int)(rfval / 2.50);
	if (rfval > 100) rfval = 100;
	return ceil(rfval);
}

void RIG_FTdx1200::get_rf_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_FTdx1200::set_vox_onoff()
{
	cmd = "VX0;";
	if (progStatus.vox_onoff) cmd[2] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox", cmd, replystr);
	set_trace(3,"set_vox_onoff", cmd.c_str(), replystr.c_str());
}

void RIG_FTdx1200::set_vox_gain()
{
	cmd = "VG";
	cmd.append(to_decimal(progStatus.vox_gain, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox gain", cmd, replystr);
	set_trace(3,"set_vox_gain", cmd.c_str(), replystr.c_str());
}

void RIG_FTdx1200::set_vox_anti()
{
	cmd = "EX117";
	cmd.append(to_decimal(progStatus.vox_anti, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET anti-vox", cmd, replystr);
	set_trace(3,"set_vox_anti", cmd.c_str(), replystr.c_str());
}

void RIG_FTdx1200::set_vox_hang()
{
	cmd = "VD";
	cmd.append(to_decimal(progStatus.vox_hang, 4)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox delay", cmd, replystr);
	set_trace(3,"set_vox_hang", cmd.c_str(), replystr.c_str());
}

void RIG_FTdx1200::set_vox_on_dataport()
{
	cmd = "EX1140;";
	if (progStatus.vox_on_dataport) cmd[5] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox on data port", cmd, replystr);
	set_trace(3,"set_vox_on_dataport", cmd.c_str(), replystr.c_str());
}

void RIG_FTdx1200::set_cw_wpm()
{
	cmd = "KS";
	if (progStatus.cw_wpm > 60) progStatus.cw_wpm = 60;
	if (progStatus.cw_wpm < 4) progStatus.cw_wpm = 4;
	cmd.append(to_decimal(progStatus.cw_wpm, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw wpm", cmd, replystr);
	set_trace(3,"set_cw_wpm", cmd.c_str(), replystr.c_str());
}


void RIG_FTdx1200::enable_keyer()
{
	cmd = "KR0;";
	if (progStatus.enable_keyer) cmd[2] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET keyer on/off", cmd, replystr);
	set_trace(3,"enable_keyer", cmd.c_str(), replystr.c_str());
}

bool RIG_FTdx1200::set_cw_spot()
{
	if (vfo->imode == mCW || vfo->imode == mCW_R) {
		cmd = "CS0;";
		if (progStatus.spot_onoff) cmd[2] = '1';
		sendCommand(cmd);
		showresp(WARN, ASC, "SET spot on/off", cmd, replystr);
		set_trace(3,"set_cw_spot", cmd.c_str(), replystr.c_str());
		return true;
	} else
		return false;
}

void RIG_FTdx1200::set_cw_weight()
{
	int n = round(progStatus.cw_weight * 10);
	cmd.assign("EX046").append(to_decimal(n, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw weight", cmd, replystr);
	set_trace(3,"set_cw_weight", cmd.c_str(), replystr.c_str());
}

void RIG_FTdx1200::set_cw_qsk()
{
	int n = progStatus.cw_qsk / 5 - 3;
	cmd.assign("EX049").append(to_decimal(n, 1)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw qsk", cmd, replystr);
	set_trace(3,"set_cw_qsk", cmd.c_str(), replystr.c_str());
}

void RIG_FTdx1200::set_cw_spot_tone()
{
	int n = (progStatus.cw_spot_tone - 300) / 50;
	cmd.assign("EX045").append(to_decimal(n, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw tone", cmd, replystr);
	set_trace(3,"set_cw_spot_tone", cmd.c_str(), replystr.c_str());
}

/*
void RIG_FTdx1200::set_cw_vol()
{
}
*/

void RIG_FTdx1200::get_band_selection(int v)
{
	int chan_mem_on = false;
	cmd = "IF;";
	wait_char(';', 27, 100, "get vfo mode in get_band_selection", ASC);

	get_trace(3, "get set_band vfo_mode()", cmd.c_str(), replystr.c_str());

	size_t p = replystr.rfind("IF");
	if (p == string::npos) return;
	if ((p + 26) >= replystr.length()) return;
	if (replystr[p+21] != '0') {	// vfo 60M memory mode
		chan_mem_on = true;
	}

	if (v == 13) {
		m_60m_indx = op_yaesu_select60->index();
		if (m_60m_indx)
			cmd.assign("MC").append(Channels_60m[m_60m_indx]).append(";");
		else if (chan_mem_on)
			cmd = "VM;";
	} else {		// v == 1..11 band selection OR return to vfo mode == 0
		if (chan_mem_on) {
			cmd = "VM;";
			op_yaesu_select60->index(m_60m_indx = 0);
		} else {
			if (v < 3)
				v = v - 1;
			cmd.assign("BS").append(to_decimal(v, 2)).append(";");
			get_trace(3, "get band", cmd.c_str(), replystr.c_str());
		}
	}

	sendCommand(cmd);
	showresp(WARN, ASC, "Select Band Stacks", cmd, replystr);
}

// DNR
void RIG_FTdx1200::set_noise_reduction_val(int val)
{
	cmd.assign("RL0").append(to_decimal(val, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET_noise_reduction_val", cmd, replystr);
	set_trace(3,"set_noise_reduction_val", cmd.c_str(), replystr.c_str());
}

int  RIG_FTdx1200::get_noise_reduction_val()
{
	int val = 1;
	cmd = rsp = "RL0";
	cmd.append(";");
	wait_char(';', 6, 100, "GET noise reduction val", ASC);
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return val;
	val = atoi(&replystr[p+3]);
	return val;
}

// DNR
void RIG_FTdx1200::set_noise_reduction(int val)
{
	cmd.assign("NR0").append(val ? "1" : "0" ).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET noise reduction", cmd, replystr);
	set_trace(3,"set_noise_reduction", cmd.c_str(), replystr.c_str());
}

int  RIG_FTdx1200::get_noise_reduction()
{
	int val;
	cmd = rsp = "NR0";
	cmd.append(";");
	wait_char(';', 5, 100, "GET noise reduction", ASC);
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	val = replystr[p+3] - '0';
	return val;
}

void RIG_FTdx1200::set_xcvr_auto_on()
{
	if (!progStatus.xcvr_auto_on) return;

	cmd = rsp = "PS";
	cmd.append(";");
	wait_char(';', 4, 100, "Test: Is Rig ON", ASC);
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) {	// rig is off, power on
		cmd = "PS1;";
		sendCommand(cmd);
		MilliSleep(1500);	// 1.0 < T < 2.0 seconds
		sendCommand(cmd);
		MilliSleep(3000);	// Wait for rig startup?  Maybe not needed.
	}
}

void RIG_FTdx1200::set_xcvr_auto_off()
{
	if (!progStatus.xcvr_auto_off) return;

	cmd = "PS0;";
	sendCommand(cmd);
}

void RIG_FTdx1200::set_compression(int on, int val)
{
	if (on) {
		if (val == 0) {
			cmd.assign("PR2;");	// mic eq on
			sendCommand(cmd);
			showresp(WARN, ASC, "set Comp EQ on", cmd, replystr);
			set_trace(3,"set_compression EQ on", cmd.c_str(), replystr.c_str());
		} else {
			cmd.assign("PR1;PL").append(to_decimal(val, 3)).append(";");
			sendCommand(cmd);
			showresp(WARN, ASC, "set Comp on", cmd, replystr);
			set_trace(3,"set_compression on", cmd.c_str(), replystr.c_str());
		}
	} else{
		cmd.assign("PR0;");
		sendCommand(cmd);
		showresp(WARN, ASC, "set Comp off", cmd, replystr);
		set_trace(3,"set_compression off", cmd.c_str(), replystr.c_str());
	}
}

/*
// Audio Peak Filter, like set_cw_spot
bool RIG_FTdx1200::set_cw_APF()
{
	if (vfo->imode == mCW || vfo->imode == mCW_R) {
		cmd = "CO0000;";
		if (progStatus.apf_onoff) cmd[5] = '2';
		sendCommand(cmd);
		showresp(WARN, ASC, "SET APF on/off", cmd, replystr);
		return true;
	} else
		return false;
}
*/
