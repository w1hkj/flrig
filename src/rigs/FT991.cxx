// ----------------------------------------------------------------------------
// Copyright (C) 2015
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

#include "FT991.h"
#include "debug.h"
#include "support.h"

#define FL991_WAIT_TIME 200

enum mFT991 {
  mLSB, mUSB, mCW, mFM, mAM, mRTTY_L, mCW_R, mPKT_L, mRTTY_U, mPKT_FM, mFM_N, mPKT_U, mAM_N, mC4FM };
//  0,    1,    2,   3,   4,   5,       6,     7,      8,       9,       10,    11,     12	// mode index

static const char FT991name_[] = "FT-991";

static const char *FT991modes_[] = {
"LSB", "USB", "CW-U", "FM", "AM", "RTTY-L",
"CW-L", "DATA-L", "RTTY-U", "DATA-FM",
"FM-N", "DATA-U", "AM-N", "C4FM", NULL};

static const char FT991_mode_chr[] =  { '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E' };
static const char FT991_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'L', 'L', 'L', 'U', 'U', 'U', 'U', 'U', 'U' };

static const int FT991_def_bw[] = {
    17,   17,   5,   0,   0,   10,       5,     16,     10,       0,       0,     16,     0,      0 };
// mLSB, mUSB, mCW, mFM, mAM, mRTTY_L, mCW_R, mPKT_L, mRTTY_U, mPKT_FM, mFM_N, mPKT_U, mAM_N, mC4FM

static const char *FT991_widths_SSB[] = {
"200",   "400",  "600",  "850", "1100", "1350", "1500", "1650", "1800", "1950", 
"2100", "2200", "2300", "2400", "2500", "2600", "2700", "2800", "2900", "3000",
"3200", NULL };

static int FT991_wvals_SSB[] = {
 1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
21, WVALS_LIMIT};

static const char *FT991_widths_SSBD[] = {
   "50",  "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
  "800", "1200", "1400", "1700", "2000", "2400", "3000", NULL };

static int FT991_wvals_SSBD[] = {
 1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
11, 12, 13, 14, 15, 16, 17, WVALS_LIMIT};

static const char *FT991_widths_CW[] = {
   "50",  "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
  "800", "1200", "1400", "1700", "2000", "2400", "3000", NULL };

static int FT991_wvals_CW[] = {
 1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
11, 12, 13, 14, 15, 16, 17, WVALS_LIMIT};

// Single bandwidth modes
static const char *FT991_widths_AMFMnar[]  = { "NARR", NULL };
static const char *FT991_widths_AMFMnorm[] = { "NORM", NULL };

static const int FT991_wvals_AMFM[] = { 0, WVALS_LIMIT };

// mPKT_FM Multi bandwidth mode
static const char *FT991_widths_NN[] = {"NORM", "NARR", NULL };

static const int FT991_wvals_NN[] = {0, 1, WVALS_LIMIT};

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

RIG_FT991::RIG_FT991() {
// base class values
	IDstr = "ID";
	name_ = FT991name_;
	modes_ = FT991modes_;
	bandwidths_ = FT991_widths_SSB;
	bw_vals_ = FT991_wvals_SSB;

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
	has_ext_tuner =
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
	atten_level = 1;
	preamp_level = 2;
	notch_on = false;
	m_60m_indx = 0;

	precision = 1;
	ndigits = 9;

}

void RIG_FT991::initialize()
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
	wait_char(';', 27, FL991_WAIT_TIME, "Read UK 60m Channel Mem", ASC);
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

}

void RIG_FT991::post_initialize()
{
	enable_yaesu_bandselect(12, false);
	enable_yaesu_bandselect(13, true);
}

bool RIG_FT991::check ()
{
	cmd = rsp = "FA";
	cmd += ';';
	int ret = wait_char(';',12, FL991_WAIT_TIME, "check", ASC);
	if (ret >= 12) return true;
	return false;
}

long RIG_FT991::get_vfoA ()
{
	cmd = rsp = "FA";
	cmd += ';';
	wait_char(';',12, FL991_WAIT_TIME, "get vfo A", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return freqA;
	p += 2;
	int f = 0;
	for (int n = 0; n < ndigits; n++)
		f = f * 10 + replystr[p + n] - '0';
	freqA = f;
	return freqA;
}

void RIG_FT991::set_vfoA (long freq)
{
	freqA = freq;
	cmd = "FA000000000;";
	for (int i = 0; i < ndigits; i++) {
		cmd[ndigits + 1 - i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vfo A", cmd, replystr);
}

long RIG_FT991::get_vfoB ()
{
	cmd = rsp = "FB";
	cmd += ';';
	wait_char(';',12, FL991_WAIT_TIME, "get vfo B", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return freqB;
	p += 2;
	int f = 0;
	for (int n = 0; n < ndigits; n++)
		f = f * 10 + replystr[p + n] - '0';
	freqB = f;
	return freqB;
}


void RIG_FT991::set_vfoB (long freq)
{
	freqB = freq;
	cmd = "FB000000000;";
	for (int i = 0; i < ndigits; i++) {
		cmd[ndigits + 1 - i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vfo B", cmd, replystr);
}

void RIG_FT991::selectA()
{
	cmd = "FT2;";
	sendCommand(cmd);
	showresp(WARN, ASC, "select A", cmd, replystr);
}

void RIG_FT991::selectB()
{
	cmd = "FT3;";
	sendCommand(cmd);
	showresp(WARN, ASC, "select B", cmd, replystr);
}

void RIG_FT991::A2B()
{
	cmd = "AB;";
	sendCommand(cmd);
	showresp(WARN, ASC, "vfo A->B", cmd, replystr);
}

void RIG_FT991::B2A()
{
	cmd = "BA;";
	sendCommand(cmd);
	showresp(WARN, ASC, "vfo B->A", cmd, replystr);
}

void RIG_FT991::swapAB()
{
	cmd = "SV;";
	sendCommand(cmd);
	showresp(WARN, ASC, "vfo A<>B", cmd, replystr);
}

bool RIG_FT991::can_split()
{
	return true;
}

void RIG_FT991::set_split(bool val)
{
	split = val;
	if (useB) {
		if (val) {
			cmd = "FT2;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on B, Tx on A", cmd, replystr);
		} else {
			cmd = "FT3;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on B, Tx on B", cmd, replystr);
		}
	} else {
		if (val) {
			cmd = "FT3;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on A, Tx on B", cmd, replystr);
		} else {
			cmd = "FT2;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on A, Tx on A", cmd, replystr);
		}
	}
	Fl::awake(highlight_vfo, (void *)0);
}

int RIG_FT991::get_split()
{
	size_t p;
	int split = 0;
	char rx, tx;
// tx vfo
	cmd = rsp = "FT";
	cmd.append(";");
	wait_char(';',4, FL991_WAIT_TIME, "get split tx vfo", ASC);
	p = replystr.rfind(rsp);
	if (p == string::npos) return false;
	tx = replystr[p+2] - '0';

// rx vfo
	cmd = rsp = "FR";
	cmd.append(";");
	wait_char(';',4, FL991_WAIT_TIME, "get split rx vfo", ASC);

	p = replystr.rfind(rsp);
	if (p == string::npos) return false;
	rx = replystr[p+2] - '0';

	split = (tx == 1 ? 2 : 0) + (rx >= 4 ? 1 : 0);

	return split;
}


int RIG_FT991::get_smeter()
{
	cmd = rsp = "SM0";
	cmd += ';';
	wait_char(';',7, FL991_WAIT_TIME, "get smeter", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	if (p + 6 >= replystr.length()) return 0;
	int mtr = atoi(&replystr[p+3]);
	mtr = mtr * 100.0 / 256.0;
	return mtr;
}

int RIG_FT991::get_swr()
{
	cmd = rsp = "RM6";
	cmd += ';';
	wait_char(';',7, FL991_WAIT_TIME, "get swr", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	if (p + 6 >= replystr.length()) return 0;
	int mtr = atoi(&replystr[p+3]);
	return (int)ceil(mtr / 2.56);
}

int RIG_FT991::get_alc()
{
	cmd = rsp = "RM4";
	cmd += ';';
	wait_char(';',7, FL991_WAIT_TIME, "get alc", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	if (p + 6 >= replystr.length()) return 0;
	int mtr = atoi(&replystr[p+3]);
	return (int)ceil(mtr / 2.56);
}

int RIG_FT991::get_power_out()
{
	cmd = rsp = "RM5";
	sendCommand(cmd.append(";"));
	wait_char(';',7, FL991_WAIT_TIME, "get pout", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	if (p + 6 >= replystr.length()) return 0;
	double mtr = (double)(atoi(&replystr[p+3]));

// following conversion iaw data measured by Terry, KJ4EED
	mtr = (.06 * mtr) + (.002 * mtr * mtr);

	return (int)ceil(mtr);
}

// Transceiver power level
int RIG_FT991::get_power_control()
{
	cmd = rsp = "PC";
	cmd += ';';
	wait_char(';',6, FL991_WAIT_TIME, "get power", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.power_level;
	if (p + 5 >= replystr.length()) return progStatus.power_level;

	int mtr = atoi(&replystr[p+2]);
	return mtr;
}

void RIG_FT991::set_power_control(double val)
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
int RIG_FT991::get_volume_control()
{
	cmd = rsp = "AG0";
	cmd += ';';
	wait_char(';',7, FL991_WAIT_TIME, "get vol", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.volume;
	if (p + 6 >= replystr.length()) return progStatus.volume;
	int val = round(atoi(&replystr[p+3]) / 2.55);
	if (val > 100) val = 100;
	return ceil(val);
}

void RIG_FT991::set_volume_control(int val) 
{
	int ivol = (int)(val * 2.55);
	cmd = "AG0000;";
	for (int i = 5; i > 2; i--) {
		cmd[i] += ivol % 10;
		ivol /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vol", cmd, replystr);
}

// Tranceiver PTT on/off
void RIG_FT991::set_PTT_control(int val)
{
	cmd = val ? "TX1;" : "TX0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "SET PTT", cmd, replystr);
	ptt_ = val;
}

int RIG_FT991::get_PTT()
{
	cmd = "TX;";
	rsp = "TX";
	waitN(4, 100, "get PTT", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return ptt_;
	ptt_ =  (replystr[p+2] != '0' ? 1 : 0);
	return ptt_;
}

// internal or external tune mode
void RIG_FT991::tune_rig(int how)
{
	cmd = "AC000;";
	if (how == 1) cmd[4] = '1';
	else if (how == 2) cmd[4] = '2';
	sendCommand(cmd);
	showresp(WARN, ASC, "tune rig", cmd, replystr);
}

int  RIG_FT991::get_tune()
{
	cmd = "AC;";
	wait_char(';',6, 100, "Tuner Enabled?", ASC);
	size_t p = replystr.rfind("AC");
	if (p == string::npos) return 0;
	if ((p + 5) >= replystr.length()) return 0;
	if (replystr[p+4] == '0') {
		return 0;
	}
	return 1;
}

int  RIG_FT991::next_attenuator()
{
	switch (atten_level) {
		case 0: return 1;
		case 1: return 0;
	}
	return 0;
}

void RIG_FT991::set_attenuator(int val)
{
	atten_level = val;
	if (atten_level == 1) {
		atten_label("12 dB", true);
	} else if (atten_level == 0) {
		atten_label("Att", false);
	}
	cmd = "RA00;";
	cmd[3] += atten_level;
	sendCommand(cmd);
	showresp(WARN, ASC, "SET att", cmd, replystr);
}

int RIG_FT991::get_attenuator()
{
	cmd = rsp = "RA0";
	cmd += ';';
	wait_char(';',5, FL991_WAIT_TIME, "get att", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.attenuator;
	if (p + 3 >= replystr.length()) return progStatus.attenuator;
	atten_level = replystr[p+3] - '0';
	if (atten_level == 1) {
		atten_label("12 dB", true);
	} else {
		atten_level = 0;
		atten_label("Att", false);
	}
	return atten_level;
}

int  RIG_FT991::next_preamp()
{
	switch (preamp_level) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 0;
	}
	return 0;
}

void RIG_FT991::set_preamp(int val)
{
	preamp_level = val;
	cmd = "PA00;";
	if (preamp_level == 1) {
		preamp_label("Amp 1", true);
	} else if (preamp_level == 2) {
		preamp_label("Amp 2", true);
	} else if (preamp_level == 0) {
		preamp_label("IPO", false);
	}
	cmd[3] = '0' + preamp_level;
	sendCommand (cmd);
	showresp(WARN, ASC, "SET preamp", cmd, replystr);
}

int RIG_FT991::get_preamp()
{
	cmd = rsp = "PA0";
	cmd += ';';
	wait_char(';',5, FL991_WAIT_TIME, "get pre", ASC);

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

int RIG_FT991::adjust_bandwidth(int val)
{
	switch (val) {
		case mCW     :
		case mCW_R   :
		case mRTTY_L :
		case mRTTY_U :
			bandwidths_ = FT991_widths_CW;
			bw_vals_ = FT991_wvals_CW;
			break;
		case mFM     :
		case mAM     :
			bandwidths_ = FT991_widths_AMFMnorm;
			bw_vals_    = FT991_wvals_AMFM;
			break;
		case mFM_N   :
		case mAM_N   :
			bandwidths_ = FT991_widths_AMFMnar;
			bw_vals_    = FT991_wvals_AMFM;
			break;
		case mPKT_FM :
			bandwidths_ = FT991_widths_NN;
			bw_vals_ = FT991_wvals_NN;
			break;
		case mPKT_L :
		case mPKT_U :
			bandwidths_ = FT991_widths_SSBD;
			bw_vals_ = FT991_wvals_SSBD;
			break;
		default:
			bandwidths_ = FT991_widths_SSB;
			bw_vals_ = FT991_wvals_SSB;
	}
	return FT991_def_bw[val];
}

int RIG_FT991::def_bandwidth(int val)
{
	return FT991_def_bw[val];
}

const char ** RIG_FT991::bwtable(int n)
{
	switch (n) {
		case mPKT_FM : return FT991_widths_NN;
		case mFM     :
		case mAM     : return FT991_widths_AMFMnorm;
		case mFM_N   :
		case mAM_N   : return FT991_widths_AMFMnar;
		case mCW     :
		case mCW_R   :
		case mRTTY_L :
		case mRTTY_U : return FT991_widths_CW;
		case mPKT_L  :
		case mPKT_U  : return FT991_widths_SSBD;
		default      : break;
	}
	return FT991_widths_SSB;
}

void RIG_FT991::set_modeA(int val)
{
	modeA = val;
	cmd = "MD0";
	cmd += FT991_mode_chr[val];
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

int RIG_FT991::get_modeA()
{
	cmd = rsp = "MD0";
	cmd += ';';
	wait_char(';',5, FL991_WAIT_TIME, "get mode A", ASC);

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

void RIG_FT991::set_modeB(int val)
{
	modeB = val;
	cmd = "MD0";
	cmd += FT991_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mode B", cmd, replystr);
	adjust_bandwidth(modeB);
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

int RIG_FT991::get_modeB()
{
	cmd = rsp = "MD0";
	cmd += ';';
	wait_char(';',5, FL991_WAIT_TIME, "get mode B", ASC);

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

void RIG_FT991::set_bwA(int val)
{
	int bw_indx = bw_vals_[val];
	bwA = val;

	if (modeA == mFM || modeA == mAM || modeA == mFM_N || modeA == mAM_N) return;
	if (modeA == mPKT_FM) {
		if (val == 1) cmd = "NA01;";
		else cmd = "NA00;";
		sendCommand(cmd);
		showresp(WARN, ASC, "SET bw A", cmd, replystr);
		return;
	}
	if ((((modeA == mLSB || modeA == mUSB) && val < 8)) ||
		((modeA == mCW || modeA == mCW_R ||
		  modeA == mRTTY_L || modeA == mRTTY_U ||
		  modeA == mPKT_L || modeA == mPKT_U) && val < 4) ) cmd = "NA01;";
	else cmd = "NA00;";

	cmd.append("SH0");
	cmd += '0' + bw_indx / 10;
	cmd += '0' + bw_indx % 10;
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET bw A", cmd, replystr);
}

int RIG_FT991::get_bwA()
{
	size_t p;
	if (modeA == mFM || modeA == mAM || modeA == mFM_N || modeA == mAM_N) {
		bwA = 0;
		return bwA;	
	}
	if (modeA == mPKT_FM) {
		cmd = rsp = "NA0";
		cmd += ';';
		wait_char(';',5, FL991_WAIT_TIME, "get bw A narrow", ASC);
		p = replystr.rfind(rsp);
		if (p == string::npos) { bwA = 0; return bwA; }
		if (p + 4 >= replystr.length()) { bwA = 0; return bwA; }
		if (replystr[p+3] == '1') bwA = 1;	// narrow on
		else bwA = 0;
		return bwA;
	}
	cmd = rsp = "SH0";
	cmd += ';';
	wait_char(';',6, FL991_WAIT_TIME, "get bw A", ASC);

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
	return bwA;
}

void RIG_FT991::set_bwB(int val)
{
	int bw_indx = bw_vals_[val];
	bwB = val;

	if (modeB == mFM || modeB == mAM || modeB == mFM_N || modeB == mAM_N) return;
	if (modeB == mPKT_FM) {
		if (val == 1) cmd = "NA01;";
		else cmd = "NA00;";
		sendCommand(cmd);
		showresp(WARN, ASC, "SET bw B", cmd, replystr);
		return;
	}
	if ((((modeB == mLSB || modeB == mUSB) && val < 8)) ||
		((modeB == mCW || modeB == mCW_R ||
		  modeB == mRTTY_L || modeB == mRTTY_U ||
		  modeB == mPKT_L || modeB == mPKT_U) && val < 4) ) cmd = "NA01;";
	else cmd = "NA00;";

	cmd.append("SH0");
	cmd += '0' + bw_indx / 10;
	cmd += '0' + bw_indx % 10;
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET bw B", cmd, replystr);
}

int RIG_FT991::get_bwB()
{
	size_t p;
	if (modeB == mFM || modeB == mAM || modeB == mFM_N || modeB == mAM_N) {
		bwB = 0;
		return bwB;
	}
	if (modeB == mPKT_FM) {
		cmd = rsp = "NA0";
		cmd += ';';
		wait_char(';',5, FL991_WAIT_TIME, "get bw B narrow", ASC);
		p = replystr.rfind(rsp);
		if (p == string::npos) { bwB = 0; return bwB; }
		if (p + 4 >= replystr.length()) { bwB = 0; return bwB; }
		if (replystr[p+3] == '1') bwB = 1;	// narrow on
		else bwB = 0;
		return bwB;
	}
	cmd = rsp = "SH0";
	cmd += ';';
	wait_char(';',6, FL991_WAIT_TIME, "get bw B", ASC);

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
	return bwB;
}

int RIG_FT991::get_modetype(int n)
{
	return FT991_mode_type[n];
}

void RIG_FT991::set_if_shift(int val)
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
}

bool RIG_FT991::get_if_shift(int &val)
{
	cmd = rsp = "IS0";
	cmd += ';';
	wait_char(';',9, FL991_WAIT_TIME, "get if shift", ASC);

	size_t p = replystr.rfind(rsp);
	val = progStatus.shift_val;
	if (p == string::npos) return progStatus.shift;
	val = atoi(&replystr[p+4]);
	if (replystr[p+3] == '-') val = -val;
	return (val != 0);
}

void RIG_FT991::get_if_min_max_step(int &min, int &max, int &step)
{
	if_shift_min = min = -1000;
	if_shift_max = max = 1000;
	if_shift_step = step = 20;
	if_shift_mid = 0;
}

void RIG_FT991::set_notch(bool on, int val)
{
// set notch frequency
	if (on) {
		cmd = "BP00001;";
		sendCommand(cmd);
		showresp(WARN, ASC, "SET notch on", cmd, replystr);
		cmd = "BP01000;";
		if (val % 10 >= 5) val += 10;
		val /= 10;
		for (int i = 3; i > 0; i--) {
			cmd[3 + i] += val % 10;
			val /=10;
		}
		sendCommand(cmd);
		showresp(WARN, ASC, "SET notch val", cmd, replystr);
		return;
	}

// set notch off
	cmd = "BP00000;";
	sendCommand(cmd);
	showresp(WARN, ASC, "SET notch off", cmd, replystr);
}

bool  RIG_FT991::get_notch(int &val)
{
	bool ison = false;
	cmd = rsp = "BP00";
	cmd += ';';
	wait_char(';',8, FL991_WAIT_TIME, "get notch on/off", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return ison;

	if (replystr[p+6] == '1') { // manual notch enabled
		ison = true;
		val = progStatus.notch_val;
		cmd = rsp = "BP01";
		cmd += ';';
		wait_char(';',8, FL991_WAIT_TIME, "get notch val", ASC);
		p = replystr.rfind(rsp);
		if (p == string::npos)
			val = 10;
		else
			val = fm_decimal(replystr.substr(p+4), 3) * 10;
	}
	return ison;
}

void RIG_FT991::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = 10;
	max = 3000;
	step = 10;
}

void RIG_FT991::set_auto_notch(int v)
{
	cmd.assign("BC0").append(v ? "1" : "0" ).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET DNF Auto Notch Filter", cmd, replystr);
}

int  RIG_FT991::get_auto_notch()
{
	cmd = "BC0;";
	wait_char(';',5, FL991_WAIT_TIME, "get auto notch", ASC);
	size_t p = replystr.rfind("BC0");
	if (p == string::npos) return 0;
	if (replystr[p+3] == '1') return 1;
	return 0;
}

void RIG_FT991::set_noise(bool b)
{
	if (b) cmd = "NB01;";
	else   cmd = "NB00;";
	sendCommand (cmd);
	showresp(WARN, ASC, "SET noise blanker", cmd, replystr);
}

int RIG_FT991::get_noise()
{
	cmd = "NB0;";
	wait_char(';',5, FL991_WAIT_TIME, "get NB", ASC);

	size_t p = replystr.rfind("NB0");
	if (p == string::npos) return 0;
	return replystr[p+3] - '0';
}

// val 0 .. 100
void RIG_FT991::set_mic_gain(int val)
{
	cmd = "MG000;";
	val = (int)(val * 2.50);
	for (int i = 3; i > 0; i--) {
		cmd[1+i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mic", cmd, replystr);
}

int RIG_FT991::get_mic_gain()
{
	cmd = rsp = "MG";
	cmd += ';';
	wait_char(';',6, FL991_WAIT_TIME, "get mic", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.mic_gain;
	int val = atoi(&replystr[p+2]);
	val = (int)(val / 2.50);
	if (val > 100) val = 100;
	return ceil(val);
}

void RIG_FT991::get_mic_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_FT991::set_rf_gain(int val)
{
	cmd = "RG0000;";
	int rfval = (int)(val * 2.50);
	for (int i = 5; i > 2; i--) {
		cmd[i] = rfval % 10 + '0';
		rfval /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET rfgain", cmd, replystr);
}

int  RIG_FT991::get_rf_gain()
{
	int rfval = 0;
	cmd = rsp = "RG0";
	cmd += ';';
	wait_char(';',7, FL991_WAIT_TIME, "get rfgain", ASC);

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

void RIG_FT991::get_rf_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_FT991::set_vox_onoff()
{
	cmd = "VX0;";
	if (progStatus.vox_onoff) cmd[2] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox", cmd, replystr);
}

void RIG_FT991::set_vox_gain()
{
	cmd = "VG";
	cmd.append(to_decimal(progStatus.vox_gain, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox gain", cmd, replystr);
}

void RIG_FT991::set_vox_anti()
{
	cmd = "EX117";
	cmd.append(to_decimal(progStatus.vox_anti, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET anti-vox", cmd, replystr);
}

void RIG_FT991::set_vox_hang()
{
	cmd = "VD";
	cmd.append(to_decimal(progStatus.vox_hang, 4)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox delay", cmd, replystr);
}

void RIG_FT991::set_vox_on_dataport()
{
	cmd = "EX1140;";
	if (progStatus.vox_on_dataport) cmd[5] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox on data port", cmd, replystr);
}

void RIG_FT991::set_cw_wpm()
{
	cmd = "KS";
	if (progStatus.cw_wpm > 60) progStatus.cw_wpm = 60;
	if (progStatus.cw_wpm < 4) progStatus.cw_wpm = 4;
	cmd.append(to_decimal(progStatus.cw_wpm, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw wpm", cmd, replystr);
}


void RIG_FT991::enable_keyer()
{
	cmd = "KR0;";
	if (progStatus.enable_keyer) cmd[2] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET keyer on/off", cmd, replystr);
}

bool RIG_FT991::set_cw_spot()
{
	if (vfo->imode == mCW || vfo->imode == mCW_R) {
		cmd = "CS0;";
		if (progStatus.spot_onoff) cmd[2] = '1';
		sendCommand(cmd);
		showresp(WARN, ASC, "SET spot on/off", cmd, replystr);
		return true;
	} else
		return false;
}

void RIG_FT991::set_cw_weight()
{
	int n = round(progStatus.cw_weight * 10);
	cmd.assign("EX014").append(to_decimal(n, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw weight", cmd, replystr);
}

void RIG_FT991::set_cw_qsk()
{
	int n = progStatus.cw_qsk / 5 - 3;
	cmd.assign("EX056").append(to_decimal(n, 1)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw qsk", cmd, replystr);
}

void RIG_FT991::set_cw_spot_tone()
{
	int n = (progStatus.cw_spot_tone - 300) / 50;
	cmd.assign("EX059").append(to_decimal(n, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw tone", cmd, replystr);
}

/*
void RIG_FT991::set_cw_vol()
{
}
*/

/*
void RIG_FT991::get_band_selection(int v)
{
	int chan_mem_on = false;
	cmd = "IF;";
	wait_char(';',28, FL991_WAIT_TIME, "get vfo mode in get_band_selection", ASC);
	size_t p = replystr.rfind("IF");
	if (p == string::npos) return;
	if ((p + 27) >= replystr.length()) return;
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
		}
	}

	sendCommand(cmd);
	showresp(WARN, ASC, "Select Band Stacks", cmd, replystr);
}
*/

// DNR
void RIG_FT991::set_noise_reduction_val(int val)
{
	cmd.assign("RL0").append(to_decimal(val, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET_noise_reduction_val", cmd, replystr);
}

int  RIG_FT991::get_noise_reduction_val()
{
	int val = 1;
	cmd = rsp = "RL0";
	cmd.append(";");
	wait_char(';',6, FL991_WAIT_TIME, "GET noise reduction val", ASC);
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return val;
	val = atoi(&replystr[p+3]);
	return val;
}

// DNR
void RIG_FT991::set_noise_reduction(int val)
{
	cmd.assign("NR0").append(val ? "1" : "0" ).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET noise reduction", cmd, replystr);
}

int  RIG_FT991::get_noise_reduction()
{
	int val;
	cmd = rsp = "NR0";
	cmd.append(";");
	wait_char(';',5, FL991_WAIT_TIME, "GET noise reduction", ASC);
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	val = replystr[p+3] - '0';
	return val;
}

void RIG_FT991::set_xcvr_auto_on()
{
	if (!progStatus.xcvr_auto_on) return;

	cmd = rsp = "PS";
	cmd.append(";");
	wait_char(';',4, FL991_WAIT_TIME, "Test: Is Rig ON", ASC);
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) {	// rig is off, power on
		cmd = "PS1;";
		sendCommand(cmd);
		MilliSleep(1100); // 1.0 < T < 2.0 seconds
		sendCommand(cmd);
		sendCommand(cmd);
		MilliSleep(3000);	// Wait for rig startup?  Maybe not needed.
	}
}

void RIG_FT991::set_xcvr_auto_off()
{
	if (!progStatus.xcvr_auto_off) return;

	cmd = "PS0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "SET xcvr auto on/off", cmd, replystr);
}

void RIG_FT991::set_compression(int on, int val)
{
	if (on) {
		if (val == 0) {
			cmd.assign("PR2;");	// mic eq on
			sendCommand(cmd);
			showresp(WARN, ASC, "set Comp EQ on", cmd, replystr);
		} else {
			cmd.assign("PR1;PL").append(to_decimal(val, 3)).append(";");
			sendCommand(cmd);
			showresp(WARN, ASC, "set Comp on", cmd, replystr);
		}
	} else{
		cmd.assign("PR0;");
		sendCommand(cmd);
		showresp(WARN, ASC, "set Comp off", cmd, replystr);
	}
}

void RIG_FT991::setVfoAdj(double v)
{
	char cmdstr[20];
	int iv = fabs(v);
	snprintf(cmdstr, sizeof(cmdstr), "%02d", iv);
	cmd = "EX039";
	if (v < 0) cmd += '-';
	else cmd += '+';
	cmd.append(cmdstr);
	cmd.append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vfo adjust", cmd, replystr);
}

double RIG_FT991::getVfoAdj() 
{
	cmd = rsp = "EX039";
	sendCommand(cmd.append(";"));
	wait_char(';',9, FL991_WAIT_TIME, "get Vfo Adjust", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	return (double)(atoi(&replystr[p+5]));
}

void RIG_FT991::get_vfoadj_min_max_step(int &min, int &max, int &step)
{
	min = -25;
	max = 25;
	step = 1;
}

