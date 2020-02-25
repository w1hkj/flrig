// ----------------------------------------------------------------------------
// Copyright (C) 2017
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
#include "FT891.h"
#include "debug.h"
#include "support.h"
#include "trace.h"

#define FL891_WAIT_TIME 200

enum mFT891 {
   mLSB, mUSB, mCW, mFM,  mAM, mTTYL, mCWR, mDATAL, mTTYU, mFMN, mDATAU, mAMN };
//   0    1,    2,   3,    4,    5,     6,    7,     8,     9,    10,   11    // mode index

static const char FT891name_[] = "FT-891";

static const char *FT891modes_[] = {
"LSB", "USB", "CW-U", "FM", "AM", "RTTY-L", "CW-L", "DATA-L", "RTTY-U", "FM-N", "DATA-U", "AM-N", NULL};

static const char FT891_mode_chr[] =  {
 '1', '2', '3', '4', '5', '6', '7', '8', '9', 'B', 'C', 'D' };

static const char FT891_mode_type[] = {
 'L', 'U', 'U', 'U', 'U', 'L', 'L', 'L', 'U', 'U', 'U', 'U' };

static const int FT891_def_bw[] = {
    17,   17,   5,   0,   0,   10,       5,     16,     10,     0,     16,     0 };
// mLSB, mUSB, mCW, mFM, mAM, mTTYL, mCWR, mDATAL, mTTYU, mFMN, mDATAU, mAMN

static const char *FT891_widths_SSB[] = {
"200",   "400",  "600",  "850", "1100", "1350", "1500", "1650", "1800", "1950",
"2100", "2200", "2300", "2400", "2500", "2600", "2700", "2800", "2900", "3000",
"3200", NULL };

static int FT891_wvals_SSB[] = {
 1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
21, WVALS_LIMIT};

static const char *FT891_widths_SSBD[] = {
   "50",  "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
  "800", "1200", "1400", "1700", "2000", "2400", "3000", NULL };

static int FT891_wvals_SSBD[] = {
 1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
11, 12, 13, 14, 15, 16, 17, WVALS_LIMIT};

static const char *FT891_widths_CW[] = {
   "50",  "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
  "800", "1200", "1400", "1700", "2000", "2400", "3000", NULL };

static int FT891_wvals_CW[] = {
 1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
11, 12, 13, 14, 15, 16, 17, WVALS_LIMIT};

// Single bandwidth modes
static const char *FT891_widths_AMFMnar[]  = { "NARR", NULL };
static const char *FT891_widths_AMFMnorm[] = { "NORM", NULL };

static const int FT891_wvals_AMFM[] = { 0, WVALS_LIMIT };

static const int FT891_wvals_NN[] = {0, 1, WVALS_LIMIT};

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

RIG_FT891::RIG_FT891() {
// base class values
	IDstr = "ID";
	name_ = FT891name_;
	modes_ = FT891modes_;
	bandwidths_ = FT891_widths_SSB;
	bw_vals_ = FT891_wvals_SSB;

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

	has_compression =
	has_compON =
	has_a2b =
	has_ext_tuner =
	has_xcvr_auto_on_off =
//	has_split =
//	has_split_AB =
	has_noise_reduction =
	has_noise_reduction_control =
	has_extras =
	has_vox_onoff =
	has_vox_gain =
	has_vox_anti =
	has_vox_hang =
	has_vox_on_dataport =

//	has_vfo_adj =

	has_cw_wpm =
	has_cw_keyer =
//	has_cw_vol =
	has_cw_spot =
	has_cw_spot_tone =
	has_cw_qsk =
	has_cw_weight =

	has_band_selection =

	can_change_alt_vfo =
	has_smeter =
	has_alc_control =
	has_swr_control =
	has_power_out =
	has_power_control =
	has_volume_control =
	has_rf_control =
	has_sql_control =
	has_micgain_control =
	has_mode_control =
	has_nb_level =
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
	preamp_level = 1;
	notch_on = false;
	m_60m_indx = 0;

	precision = 1;
	ndigits = 9;

}

void RIG_FT891::initialize()
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

	op_yaesu_select60->deactivate();

}

void RIG_FT891::post_initialize()
{
}

bool RIG_FT891::check ()
{
	cmd = rsp = "FA";
	cmd += ';';
	int ret = wait_char(';',12, FL891_WAIT_TIME, "check", ASC);
	if (ret >= 12) return true;
	return false;
}

long RIG_FT891::get_vfoA ()
{
	// When VFOA is 'selected', radio has it actively loaded in FA, otherwise
	// it is in FB
	if (rigbase::isOnA()) {
		cmd = rsp = "FA";
	} else 	{
		cmd = rsp = "FB";
	}
	cmd += ';';

	wait_char(';',12, FL891_WAIT_TIME, "get vfo A", ASC);

	get_trace(4, "get_vfoA():\n", cmd.c_str(), "\n", replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return freqA;
	p += 2;
	int f = 0;
	for (int n = 0; n < ndigits; n++)
		f = f * 10 + replystr[p + n] - '0';
	freqA = f;
	return freqA;
}

void RIG_FT891::set_vfoA (long freq)
{
	freqA = freq;
	
	// When VFOA is 'selected', radio has it actively loaded in FA, otherwise
	// it is in FB
	if (rigbase::isOnA()) {
		cmd = "FA000000000;";
	} else 	{
		cmd = "FB000000000;";
	}
	
	for (int i = 0; i < ndigits; i++) {
		cmd[ndigits + 1 - i] += freq % 10;
		freq /= 10;
	}

	sendCommand(cmd);
	showresp(WARN, ASC, "SET vfo A", cmd, replystr);

	set_trace(4, "set_vfoA():\n", cmd.c_str(), "\n", replystr.c_str());

}

long RIG_FT891::get_vfoB ()
{
	// When VFOB is 'selected', radio has it actively loaded in FA, otherwise
	// it is in FB
	if (rigbase::isOnB()) {
		cmd = rsp = "FA";
	} else {
		cmd = rsp = "FB";
	}
	cmd += ';';
	wait_char(';',12, FL891_WAIT_TIME, "get vfo B", ASC);

	get_trace(4, "get_vfoB():\n", cmd.c_str(), "\n", replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return freqB;
	p += 2;
	int f = 0;
	for (int n = 0; n < ndigits; n++)
		f = f * 10 + replystr[p + n] - '0';
	freqB = f;
	return freqB;
}


void RIG_FT891::set_vfoB (long freq)
{
	freqB = freq;
	
	// When VFOB is 'selected', radio has it actively loaded in FA, otherwise
	// it is in FB
	if (rigbase::isOnB()) {
		cmd = "FA000000000;";
	} else {
		cmd = "FB000000000;";
	}
	
	for (int i = 0; i < ndigits; i++) {
		cmd[ndigits + 1 - i] += freq % 10;
		freq /= 10;
	}

	sendCommand(cmd);
	showresp(WARN, ASC, "SET vfo B", cmd, replystr);

	set_trace(4, "set_vfoB():\n", cmd.c_str(), "\n", replystr.c_str());
}

void RIG_FT891::selectA()
{
	if (rigbase::isOnA()) return;
	rigbase::selectA();
	
	cmd = "SV;";

	sendCommand(cmd);
	showresp(WARN, ASC, "select A", cmd, replystr);

	get_trace(4, "selectA():\n", cmd.c_str(), "\n", replystr.c_str());
}

void RIG_FT891::selectB()
{
	if (rigbase::isOnB()) return;
	rigbase::selectB();

	cmd = "SV;";

	sendCommand(cmd);
	showresp(WARN, ASC, "select B", cmd, replystr);

	set_trace(4, "selectB():\n", cmd.c_str(), "\n", replystr.c_str());
}


void RIG_FT891::A2B()
{
	cmd = "AB;";

	sendCommand(cmd);
	showresp(WARN, ASC, "vfo A->B", cmd, replystr);

	set_trace(4, "A2B():\n", cmd.c_str(), "\n", replystr.c_str());
}

void RIG_FT891::B2A()
{
	cmd = "BA;";

	sendCommand(cmd);
	showresp(WARN, ASC, "vfo B->A", cmd, replystr);

	set_trace(4, "B2A():\n", cmd.c_str(), "\n", replystr.c_str());
}

void RIG_FT891::swapAB()
{
	rigbase::swapAB();

	cmd = "SV;";

	sendCommand(cmd);
	showresp(WARN, ASC, "vfo A<>B", cmd, replystr);

	set_trace(4, "swapAB():\n", cmd.c_str(), "\n", replystr.c_str());
}

int RIG_FT891::get_smeter()
{
	cmd = rsp = "SM0";
	cmd += ';';
	wait_char(';',7, FL891_WAIT_TIME, "get smeter", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	if (p + 6 >= replystr.length()) return 0;
	int mtr = atoi(&replystr[p+3]);
	mtr = mtr / 2.56;
	return mtr;
}

int RIG_FT891::get_swr()
{
	cmd = rsp = "RM6";
	cmd += ';';
	wait_char(';',7, FL891_WAIT_TIME, "get swr", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	if (p + 6 >= replystr.length()) return 0;
	int mtr = atoi(&replystr[p+3]);
	return (int)ceil(mtr / 2.56);
}

int RIG_FT891::get_alc()
{
	cmd = rsp = "RM4";
	cmd += ';';
	wait_char(';',7, FL891_WAIT_TIME, "get alc", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	if (p + 6 >= replystr.length()) return 0;
	int mtr = atoi(&replystr[p+3]);
	return (int)ceil(mtr / 2.56);
}

int RIG_FT891::get_power_out()
{
	cmd = rsp = "RM5";
	sendCommand(cmd.append(";"));
	wait_char(';',7, FL891_WAIT_TIME, "get pout", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	if (p + 6 >= replystr.length()) return 0;

// this needs to be measured and adjusted
	int mtr = atoi(&replystr[p+3]);
	return (int)ceil(mtr / 2.56);
}

// Transceiver power level
int RIG_FT891::get_power_control()
{
	cmd = rsp = "PC";
	cmd += ';';
	wait_char(';',6, FL891_WAIT_TIME, "get power", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.power_level;
	if (p + 5 >= replystr.length()) return progStatus.power_level;

	int mtr = atoi(&replystr[p+2]);
	return mtr;
}

void RIG_FT891::set_power_control(double val)
{
	int ival = (int)val;
	cmd = "PC000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ival % 10;
		ival /= 10;
	}

	sendCommand(cmd);
	showresp(WARN, ASC, "SET power", cmd, replystr);

	set_trace(4, "set_power_control():\n", cmd.c_str(), "\n", replystr.c_str());

}

// Volume control return 0 ... 100
int RIG_FT891::get_volume_control()
{
	cmd = rsp = "AG0";
	cmd += ';';
	wait_char(';',7, FL891_WAIT_TIME, "get vol", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.volume;
	if (p + 6 >= replystr.length()) return progStatus.volume;
	int val = round(atoi(&replystr[p+3]) / 2.55);
	if (val > 100) val = 100;
	return ceil(val);
}

void RIG_FT891::set_volume_control(int val)
{
	int ivol = (int)(val * 2.55);
	cmd = "AG0000;";
	for (int i = 5; i > 2; i--) {
		cmd[i] += ivol % 10;
		ivol /= 10;
	}

	sendCommand(cmd);
	showresp(WARN, ASC, "SET vol", cmd, replystr);

	set_trace(4, "set_volume_control():\n", cmd.c_str(), "\n", replystr.c_str());
}

// Tranceiver PTT on/off
void RIG_FT891::set_PTT_control(int val)
{
	cmd = val ? "TX1;" : "TX0;";

	sendCommand(cmd);
	showresp(WARN, ASC, "SET PTT", cmd, replystr);
	ptt_ = val;

	set_trace(4, "set_ptt_control():\n", cmd.c_str(), "\n", replystr.c_str());
}

int RIG_FT891::get_PTT()
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
void RIG_FT891::tune_rig(int)
{
	cmd = "AC012;";
	sendCommand(cmd);
	showresp(WARN, ASC, "tune rig", cmd, replystr);
	set_trace(4, "tune_rig():\n", cmd.c_str(), "\n", replystr.c_str());
}

int RIG_FT891::get_tune()
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

void RIG_FT891::set_attenuator(int val)
{
	if (val) cmd = "RA01;";
	else     cmd = "RA00;";
	sendCommand(cmd);
	showresp(WARN, ASC, "SET att", cmd, replystr);
}

int RIG_FT891::get_attenuator()
{
	cmd = rsp = "RA0";
	cmd += ';';
	wait_char(';',5, FL891_WAIT_TIME, "get att", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.attenuator;
	if (p + 3 >= replystr.length()) return progStatus.attenuator;
	atten_level = replystr[p+3] - '0';

	return atten_level;
}

void RIG_FT891::set_preamp(int val)
{
	if (val) cmd = "PA01;";
	else     cmd = "PA00;";
	if (val) {
		preamp_label("AMP", true);
	} else {
		preamp_label("IPO", false);
	}
	preamp_level = val;

	sendCommand (cmd);
	showresp(WARN, ASC, "SET preamp", cmd, replystr);
}

int RIG_FT891::get_preamp()
{
	cmd = rsp = "PA0";
	cmd += ';';
	wait_char(';',5, FL891_WAIT_TIME, "get pre", ASC);

	size_t p = replystr.rfind(rsp);
	if (p != string::npos)
		preamp_level = replystr[p+3] - '0';

	if (preamp_level == 1) {
		preamp_label("Amp", true);
	} else {
		preamp_label("IPO", false);
	}

	return preamp_level;
}

int RIG_FT891::adjust_bandwidth(int val)
{
	switch (val) {
		case mCW     :
		case mCWR   :
		case mTTYL :
		case mTTYU :
			bandwidths_ = FT891_widths_CW;
			bw_vals_ = FT891_wvals_CW;
			break;
		case mFM     :
		case mAM     :
			bandwidths_ = FT891_widths_AMFMnorm;
			bw_vals_    = FT891_wvals_AMFM;
			break;
		case mFMN   :
		case mAMN   :
			bandwidths_ = FT891_widths_AMFMnar;
			bw_vals_    = FT891_wvals_AMFM;
			break;
		case mDATAL :
		case mDATAU :
			bandwidths_ = FT891_widths_SSBD;
			bw_vals_ = FT891_wvals_SSBD;
			break;
		default:
			bandwidths_ = FT891_widths_SSB;
			bw_vals_ = FT891_wvals_SSB;
	}
	return FT891_def_bw[val];
}

int RIG_FT891::def_bandwidth(int val)
{
	return FT891_def_bw[val];
}

const char ** RIG_FT891::bwtable(int n)
{
	switch (n) {
		case mFM     :
		case mAM     : return FT891_widths_AMFMnorm;
		case mFMN   :
		case mAMN   : return FT891_widths_AMFMnar;
		case mCW     :
		case mCWR   :
		case mTTYL :
		case mTTYU : return FT891_widths_CW;
		case mDATAL  :
		case mDATAU  : return FT891_widths_SSBD;
		default      : break;
	}
	return FT891_widths_SSB;
}

void RIG_FT891::set_sideband(int md)
{
	switch (md) {
		case mLSB: case mUSB:
			cmd = "EX1107";
			cmd += (md == mLSB ? '1' : '0');
			cmd += ';';
			sendCommand(cmd);
			showresp(WARN, ASC, "SET SSB sideband", cmd, replystr);
			set_trace(4, "SET SSB sideband:\n", cmd.c_str(), "\n", replystr.c_str());
			break;
		case mCW: case mCWR:
			cmd = "EX0707";
			cmd += (md == mCWR ? '1' : '0');
			cmd += ';';
			sendCommand(cmd);
			showresp(WARN, ASC, "SET CW sideband", cmd, replystr);
			set_trace(4, "SET CW sideband:\n", cmd.c_str(), "\n", replystr.c_str());
			break;
		case mTTYL: case mTTYU:
			cmd = "EX1011";
			cmd += (md == mTTYL ? '1' : '0');
			cmd += ';';
			sendCommand(cmd);
			showresp(WARN, ASC, "SET TTY sideband", cmd, replystr);
			set_trace(4, "SET TTY sideband:\n", cmd.c_str(), "\n", replystr.c_str());
			break;
		case mDATAL: case mDATAU:
			cmd = "EX0812";
			cmd += (md == mDATAL ? '1' : '0');
			cmd += ';';
			sendCommand(cmd);
			showresp(WARN, ASC, "SET DATA sideband", cmd, replystr);
			set_trace(4, "SET DATA sideband:\n", cmd.c_str(), "\n", replystr.c_str());
			break;
		default: return;
	}
	return;
}

int RIG_FT891::get_sideband(int md)
{
	size_t p;
	switch (md) {
		case mLSB: case mUSB:
			cmd = "EX1107;";
			wait_char(';', 8, FL891_WAIT_TIME, "GET SSB sideband", ASC);
			p = replystr.find("EX");
			if (p != string::npos)
				return replystr[p+6] - '0';
			break;
		case mCW: case mCWR:
			cmd = "EX0707;";
			wait_char(';', 8, FL891_WAIT_TIME, "GET CW sideband", ASC);
			p = replystr.find("EX");
			if (p != string::npos)
				return replystr[p+6] - '0';
			break;
		case mTTYL: case mTTYU:
			cmd = "EX1011;";
			wait_char(';', 8, FL891_WAIT_TIME, "GET TTY sideband", ASC);
			p = replystr.find("EX");
			if (p != string::npos)
				return replystr[p+6] - '0';
			break;
		case mDATAL: case mDATAU:
			cmd = "EX0812;";
			wait_char(';', 8, FL891_WAIT_TIME, "GET DATA sideband", ASC);
			p = replystr.find("EX");
			if (p != string::npos)
				return replystr[p+6] - '0';
			break;
		default: return 1;
	}
	return 1;
}

void RIG_FT891::set_modeA(int val)
{
	modeA = val;

	if (!rigbase::isOnA()) {
		LOG_WARN("set_modeA, but on B.  Call selectA() first.");
		return;
	}

	adjust_bandwidth(modeA);

	cmd = "MD0";
	cmd += FT891_mode_chr[val];
	cmd += ';';

	sendCommand(cmd);
	showresp(WARN, ASC, "SET mode A", cmd, replystr);

	set_trace(4, "set_modeA():\n", cmd.c_str(), "\n", replystr.c_str());

	set_sideband(val);
}

int RIG_FT891::get_modeA()
{
	if (!rigbase::isOnA()) {
		//LOG_WARN("get_modeA, but on B.  Call selectA() first.");
		return modeA;
	}

	cmd = rsp = "MD0";
	cmd += ';';
	wait_char(';',5, FL891_WAIT_TIME, "get mode A", ASC);

	size_t p = replystr.rfind(rsp);
	if (p != string::npos) {
		if (p + 3 < replystr.length()) {
			int md = 0;
			switch (replystr[p+3]) {
				case '1': case '2': md = (get_sideband(mLSB) ? mLSB : mUSB); break;
				case '3': case '7': md = (get_sideband(mCW) ? mCWR : mCW); break;
				case '6': case '9': md = (get_sideband(mTTYU) ? mTTYL : mTTYU); break;
				case '8': case 'C': md = (get_sideband(mDATAU) ? mDATAL : mDATAU); break;
				case '4': md = mFM; break;
				case '5': md = mAM; break;
				case 'B': md = mFMN; break;
				case 'D': md = mAMN; break;
			}
			modeA = md;
		}
	}

	adjust_bandwidth(modeA);

	get_trace(4, "get_modeA():\n", cmd.c_str(), "\n", replystr.c_str());

	return modeA;
}

void RIG_FT891::set_modeB(int val)
{
	modeB = val;

	if (!rigbase::isOnB()) {
		LOG_WARN("set_modeB, but on A.  Call selectB() first.");
		return;
	}

	adjust_bandwidth(modeB);

	cmd = "MD0";
	cmd += FT891_mode_chr[val];
	cmd += ';';

	sendCommand(cmd);
	showresp(WARN, ASC, "SET mode B", cmd, replystr);

	set_trace(4, "set_modeB():\n", cmd.c_str(), "\n", replystr.c_str());

	set_sideband(val);
}

int RIG_FT891::get_modeB()
{
	if (!rigbase::isOnB()) {
		//LOG_WARN("set_modeB, but on A.  Call selectB() first.");
		return modeB;
	}

	cmd = rsp = "MD0";
	cmd += ';';
	wait_char(';',5, FL891_WAIT_TIME, "get mode B", ASC);

	size_t p = replystr.rfind(rsp);
	if (p != string::npos) {
		if (p + 3 < replystr.length()) {
			int md = 0;
			switch (replystr[p+3]) {
				case '1': case '2': md = (get_sideband(mLSB) ? mLSB : mUSB); break;
				case '3': case '7': md = (get_sideband(mCW) ? mCWR : mCW); break;
				case '6': case '9': md = (get_sideband(mTTYU) ? mTTYL : mTTYU); break;
				case '8': case 'C': md = (get_sideband(mDATAU) ? mDATAL : mDATAU); break;
				case '4': md = mFM; break;
				case '5': md = mAM; break;
				case 'B': md = mFMN; break;
				case 'D': md = mAMN; break;
			}
			modeB = md;
		}
	}

	adjust_bandwidth(modeB);

	get_trace(4, "get_modeB():\n", cmd.c_str(), "\n", replystr.c_str());

	return modeB;
}

void RIG_FT891::set_bwA(int val)
{
	bwA = val;

	if (!rigbase::isOnA()) {
		LOG_WARN("set_bwA, but on B.  Call selectA() first.");
		return;
	}

	int bw_indx = bw_vals_[val];

	if (modeA == mFM || modeA == mAM || modeA == mFMN || modeA == mAMN) return;
	cmd = "NA00;";
	if ((((modeA == mLSB || modeA == mUSB) && val < 8)) ||
		((modeA == mCW || modeA == mCWR ||
		  modeA == mTTYL || modeA == mTTYU ||
		  modeA == mDATAL || modeA == mDATAU) && val < 9) )
		cmd = "NA01;";

	cmd.append("SH01");
	cmd += '0' + bw_indx / 10;
	cmd += '0' + bw_indx % 10;
	cmd += ';';

	sendCommand(cmd);
	showresp(WARN, ASC, "SET bw A", cmd, replystr);

	set_trace(4, "set_bwA():\n", cmd.c_str(), "\n", replystr.c_str());
}

int RIG_FT891::get_bwA()
{
	size_t p;
	if (!rigbase::isOnA()) {
		//LOG_WARN("get_bwA, but on B.  Call selectA() first.");
		return bwA;
	}

	if (modeA == mFM || modeA == mAM || modeA == mFMN || modeA == mAMN) {
		bwA = 0;
		return bwA;
	}

	cmd = rsp = "SH0";
	cmd += ';';

	wait_char(';',7, FL891_WAIT_TIME, "get bw A", ASC);

	get_trace(4, "get_bwA():\n", cmd.c_str(), "\n", replystr.c_str());

	p = replystr.rfind(rsp);
	if (p == string::npos) return bwA;
	if (p + 6 >= replystr.length()) return bwA;

	replystr[p+6] = 0;
	int bw_idx = fm_decimal(replystr.substr(p+4), 2);
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

void RIG_FT891::set_bwB(int val)
{
	bwB = val;

	if (!rigbase::isOnB()) {
		LOG_WARN("set_bwB, but on A.  Call selectB() first.");
		return;
	}

	int bw_indx = bw_vals_[val];

	if (modeB == mFM || modeB == mAM || modeB == mFMN || modeB == mAMN) return;
	cmd = "NA00;";
	if ((((modeB == mLSB || modeB == mUSB) && val < 8)) ||
		((modeB == mCW || modeB == mCWR ||
		  modeB == mTTYL || modeB == mTTYU ||
		  modeB == mDATAL || modeB == mDATAU) && val < 9) )
		cmd = "NA01;";

	cmd.append("SH01");
	cmd += '0' + bw_indx / 10;
	cmd += '0' + bw_indx % 10;
	cmd += ';';

	sendCommand(cmd);
	showresp(WARN, ASC, "SET bw B", cmd, replystr);

	set_trace(4, "set_bwB():\n", cmd.c_str(), "\n", replystr.c_str());

}

int RIG_FT891::get_bwB()
{
	size_t p;
	if (!rigbase::isOnB()) {
		//LOG_WARN("get_bwB, but on A.  Call selectB() first.");
		return bwB;
	}

	if (modeB == mFM || modeB == mAM || modeB == mFMN || modeB == mAMN) {
		bwB = 0;
		return bwB;
	}
	cmd = rsp = "SH0";
	cmd += ';';
	wait_char(';',7, FL891_WAIT_TIME, "get bw B", ASC);

	get_trace(4, "get_bwB():\n", cmd.c_str(), "\n", replystr.c_str());

	p = replystr.rfind(rsp);
	if (p == string::npos) return bwB;
	if (p + 6 >= replystr.length()) return bwB;

	replystr[p+6] = 0;
	int bw_idx = fm_decimal(replystr.substr(p+4), 2);
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

int RIG_FT891::get_modetype(int n)
{
	return FT891_mode_type[n];
}

void RIG_FT891::set_if_shift(int val)
{
	cmd = "IS01+0000;";
	if (val == 0) cmd[3] = '0';
	if (val < 0) cmd[4] = '-';
	val = abs(val);
	for (int i = 4; i > 0; i--) {
		cmd[4+i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET if shift", cmd, replystr);
}

bool RIG_FT891::get_if_shift(int &val)
{
	cmd = rsp = "IS0";
	cmd += ';';
	wait_char(';',10, FL891_WAIT_TIME, "get if shift", ASC);

	size_t p = replystr.rfind(rsp);
	val = progStatus.shift_val;
	if (p == string::npos) return progStatus.shift;
	val = atoi(&replystr[p+5]);
	if (replystr[p+4] == '-') val = -val;
	return (replystr[3] == '1');
}

// P1	0: (Fixed)
// P2   0: Manual NOTCH “ON/OFF”
//      1: Manual NOTCH LEVEL
// P3  if P2=0
//       000: “OFF”
//       001: “ON”
//     if P2=1
//        001 - 320 (NOTCH Frequency : x 10 Hz )

void RIG_FT891::set_notch(bool on, int val)
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

bool  RIG_FT891::get_notch(int &val)
{
	bool ison = false;
	cmd = rsp = "BP00";
	cmd += ';';
	wait_char(';',8, FL891_WAIT_TIME, "get notch on/off", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return ison;

	if (replystr[p+6] == '1') { // manual notch enabled
		ison = true;
		val = progStatus.notch_val;
		cmd = rsp = "BP01";
		cmd += ';';
		wait_char(';',8, FL891_WAIT_TIME, "get notch val", ASC);
		p = replystr.rfind(rsp);
		if (p == string::npos)
			val = 10;
		else
			val = fm_decimal(replystr.substr(p+4), 3) * 10;
	}
	return ison;
}

void RIG_FT891::set_auto_notch(int v)
{
	cmd.assign("BC0").append(v ? "1" : "0" ).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET DNF Auto Notch Filter", cmd, replystr);
}

int  RIG_FT891::get_auto_notch()
{
	cmd = "BC0;";
	wait_char(';',5, FL891_WAIT_TIME, "get auto notch", ASC);
	size_t p = replystr.rfind("BC0");
	if (p == string::npos) return 0;
	if (replystr[p+3] == '1') return 1;
	return 0;
}

void RIG_FT891::set_noise(bool b)
{
	if (b) cmd = "NR01;";
	else   cmd = "NR00;";
	sendCommand (cmd);
	showresp(WARN, ASC, "SET NR", cmd, replystr);
}

int RIG_FT891::get_noise()
{
	cmd = "NR0;";
	wait_char(';',5, FL891_WAIT_TIME, "get NR", ASC);

	size_t p = replystr.rfind("NR0");
	if (p == string::npos) return 0;
	return replystr[p+3] - '0';
}

void RIG_FT891::set_nb_level(int val) 
{
	cmd = "RL000;";
	for (int i = 4; i > 2; i--) {
		cmd[i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set RL level", cmd, replystr);
}

int RIG_FT891::get_nb_level() 
{ 
	cmd = "RL0;";
	wait_char(';', 7, FL891_WAIT_TIME, "get RL level", ASC);
	size_t p = replystr.rfind("RL0");
	if (p == string::npos) return 0;

	int val = atoi(&replystr[p+3]);
	return val;
}

void RIG_FT891::set_noise_reduction(int val)
{
	if (val) cmd = "NB01;";
	else     cmd = "NB00;";
	sendCommand (cmd);
	showresp(WARN, ASC, "Set NB on/off", cmd, replystr);
}

int  RIG_FT891::get_noise_reduction()
{
	cmd = "NB0;";
	wait_char(';', 5, FL891_WAIT_TIME, "get NB", ASC);

	size_t p = replystr.rfind("NB0");
	if (p == string::npos) return 0;
	return replystr[p+3] - '0';
}

void RIG_FT891::set_noise_reduction_val(int val)
{
	cmd = "NL0000;";
	for (int i = 5; i > 2; i--) {
		cmd[i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set NB level", cmd, replystr);
}

int  RIG_FT891::get_noise_reduction_val()
{
	cmd = "NL0;";
	wait_char(';', 7, FL891_WAIT_TIME, "get NB level", ASC);
	size_t p = replystr.rfind("NL0");
	if (p == string::npos) return 0;

	int val = atoi(&replystr[p+3]);
	return val;
}

// val 0 .. 100
void RIG_FT891::set_mic_gain(int val)
{
	cmd = "MG000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] = val % 10 + '0';
		val /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mic", cmd, replystr);
}

int RIG_FT891::get_mic_gain()
{
	cmd = rsp = "MG";
	cmd += ';';
	wait_char(';',6, FL891_WAIT_TIME, "get mic", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.mic_gain;
	int val = atoi(&replystr[p+2]);
	if (val > 100) val = 100;
	return ceil(val);
}

void RIG_FT891::set_rf_gain(int val)
{
	cmd = "RG0000;";
	for (int i = 5; i > 2; i--) {
		cmd[i] = val % 10 + '0';
		val /= 10;
	}

	sendCommand(cmd);
	showresp(WARN, ASC, "SET rfgain", cmd, replystr);

	set_trace(4, "set_rf_gain():\n", cmd.c_str(), "\n", replystr.c_str());

}

int  RIG_FT891::get_rf_gain()
{
	int rfval = 0;
	cmd = rsp = "RG0";
	cmd += ';';
	wait_char(';',7, FL891_WAIT_TIME, "get rfgain", ASC);

	get_trace(4, "get_rf_gain():\n", cmd.c_str(), "\n", replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.rfgain;
	for (int i = 3; i < 6; i++) {
		rfval *= 10;
		rfval += replystr[p+i] - '0';
	}
	return ceil(rfval);
}

void RIG_FT891::set_squelch(int val)
{
	cmd = "SQ0000;";
	for (int i = 5; i > 2; i--) {
		cmd[i] = val % 10 + '0';
		val /= 10;
	}

	sendCommand(cmd);
	showresp(WARN, ASC, "SET squelch", cmd, replystr);

	set_trace(4, "set_squelch():\n", cmd.c_str(), "\n", replystr.c_str());

}

int  RIG_FT891::get_squelch()
{
	int rfval = 0;
	cmd = rsp = "SQ0";
	cmd += ';';
	wait_char(';',7, FL891_WAIT_TIME, "get squelch", ASC);

	get_trace(4, "get_squelch():\n", cmd.c_str(), "\n", replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.rfgain;
	for (int i = 3; i < 6; i++) {
		rfval *= 10;
		rfval += replystr[p+i] - '0';
	}
	return ceil(rfval);
}


// NEED
// bool RIG_FT891::get_vox_onoff()

// EX1616 VOX SELECT      0: MIC 1: DATA
// EX1617 VOX GAIN        0 - 100 (P2= 000 - 100)
// VG     VOX GAIN        0 - 100 (P2= 000 - 100)
// EX1618 VOX DELAY       30 - 3000 msec (P2= 0030 - 3000) (10 msec/step)
// EX1619 ANTI VOX GAIN   0 - 100 (P2= 000 - 100)

// EX1620 DATA VOX GAIN   0 - 100 (P2= 000 - 100)
// EX1621 DATA VOX DELAY  30 - 3000 msec (P2= 0030 - 3000)
// EX1622 ANTI DVOX GAIN  0 - 100 (P2= 000 - 100)

void RIG_FT891::set_vox_onoff()
{
	cmd = "VX0;";
	if (progStatus.vox_onoff) cmd[2] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox", cmd, replystr);
}

void RIG_FT891::set_vox_gain()
{
	if (progStatus.vox_on_dataport)
		cmd = "EX1620";
	else
		cmd = "VG";
	cmd.append(to_decimal(progStatus.vox_gain, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox gain", cmd, replystr);
}

void RIG_FT891::set_vox_anti()
{
	if (progStatus.vox_on_dataport)
		cmd = "EX1622";
	else
		cmd = "EX1619";
	cmd.append(to_decimal(progStatus.vox_anti, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET anti-vox", cmd, replystr);
}

void RIG_FT891::set_vox_hang()
{
	if (progStatus.vox_on_dataport)
		cmd = "EX1621";
	else
		cmd = "VD";
	cmd.append(to_decimal(progStatus.vox_hang, 4)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox delay", cmd, replystr);
}

void RIG_FT891::set_vox_on_dataport()
{
	cmd = "EX16160;";
	if (progStatus.vox_on_dataport) cmd[6] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox on data port", cmd, replystr);
}

void RIG_FT891::set_cw_wpm()
{
	cmd = "KS";
	if (progStatus.cw_wpm > 60) progStatus.cw_wpm = 60;
	if (progStatus.cw_wpm < 4) progStatus.cw_wpm = 4;
	cmd.append(to_decimal(progStatus.cw_wpm, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw wpm", cmd, replystr);
}

void RIG_FT891::enable_keyer()
{
	if (progStatus.enable_keyer)
		cmd = "KR1;";
	else
		cmd = "KR0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "SET keyer on/off", cmd, replystr);
}

bool RIG_FT891::set_cw_spot()
{
	if (vfo->imode == mCW || vfo->imode == mCWR) {
		cmd = "CS0;";
		if (progStatus.spot_onoff) cmd[2] = '1';
		sendCommand(cmd);
		showresp(WARN, ASC, "SET spot on/off", cmd, replystr);
		return true;
	} else
		return false;
}

void RIG_FT891::set_cw_weight()
{
	int n = round(progStatus.cw_weight * 10);
	cmd.assign("EX0403").append(to_decimal(n, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw weight", cmd, replystr);
}

void RIG_FT891::set_cw_qsk()
{
	int n = progStatus.cw_qsk / 5 - 3;
	cmd.assign("EX0713").append(to_decimal(n, 1)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw qsk", cmd, replystr);
}

// 00: 300 Hz to 75: 1050 Hz (10Hz steps)
void RIG_FT891::set_cw_spot_tone()
{
	int n = progStatus.cw_spot_tone / 10 - 30;
	if (n < 0) n = 0;
	if (n > 75) n = 75;
	cmd.assign("KP").append(to_decimal(n, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw tone", cmd, replystr);
}

void RIG_FT891::set_xcvr_auto_on()
{
	if (!progStatus.xcvr_auto_on) return;

	cmd = rsp = "PS";
	cmd.append(";");
	wait_char(';',4, FL891_WAIT_TIME, "Test: Is Rig ON", ASC);
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) {	// rig is off, power on
		cmd = "PS1;";
		sendCommand(cmd);
		MilliSleep(1500);	// 1.0 < T < 2.0 seconds
		sendCommand(cmd);
		MilliSleep(3000);	// Wait for rig startup?  Maybe not needed.
	}
}

void RIG_FT891::set_xcvr_auto_off()
{
	if (!progStatus.xcvr_auto_off) return;

	cmd = "PS0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "SET xcvr auto on/off", cmd, replystr);
}

void RIG_FT891::set_compression(int on, int val)
{
	cmd = "PL";
	cmd.append(to_decimal(val, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set Comp PL", cmd, replystr);

	// Can only send PR command in SSB mode.  Other modes will cause 891 to
	// return ?; in response to sending this
	int curMode = rigbase::isOnA() ? modeA : modeB;
	if ( curMode == mLSB || curMode == mUSB ) {
		if (on)
			cmd = "PR01;";
		else
			cmd = "PR00;";
		set_trace(2, "set Comp", cmd.c_str());
		sendCommand(cmd);
		showresp(WARN, ASC, "set Comp", cmd, replystr);
	}
}

void RIG_FT891::get_compression(int &on, int &val)
{ 
	on = 0; val = 0;

	cmd = rsp = "PL";
	cmd += ';';
	wait_char(';',6, FL891_WAIT_TIME, "get Comp PL", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return;
	val = atoi(&replystr[p+2]);
	if (val > 100) val = 100;
	val = ceil(val);

	// Can only send PR command in SSB mode.  Other modes will cause 891 to
	// return ?; in response to sending this
	int curMode = rigbase::isOnA() ? modeA : modeB;
	if ( curMode == mLSB || curMode == mUSB ) {
		cmd = "PR0;";
		wait_char(';', 7, FL891_WAIT_TIME, "get PR level", ASC);
		size_t p = replystr.rfind("PR0");
		if (p == string::npos) return;

		on = replystr[p+3] - '0';
	}
	
	stringstream s;
	s << "get_compression: " << (on ? "ON" : "OFF") << "(" << on << "), comp PL=" << val;
	get_trace(1, s.str().c_str());
}


void RIG_FT891::get_band_selection(int v)
{
	if (v < 3) v = v - 1;
	cmd.assign("BS").append(to_decimal(v, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "Select Band Stacks", cmd, replystr);
	set_trace(2, "get band", cmd.c_str());
}

