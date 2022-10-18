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
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

// comment out for distribution
//#define TESTING 1

#include <iostream>
#include <sstream>

#include "yaesu/FTdx10.h"
#include "debug.h"
#include "support.h"

enum mFTdx10 {
   mLSB, mUSB, mCW_U, mFM, mAM, mRTTY_L, mCW_L, mDATA_L, mRTTY_U, mDATA_FM, mFM_N, mDATA_U, mAM_N, mPSK, mDATA_FMN };
//  0,    1,    2,    3,    4,    5,       6,     7,      8,       9,        10,    11,      12,    13,      14		// mode index

static const char FTdx10name_[] = "FTdx10";

#undef  NUM_MODES
#define NUM_MODES  15

static int defBW_narrow[NUM_MODES] = {
//  mLSB, mUSB, mCW_U, mFM, mAM, mRTTY_L, mCW_L, mDATA_L, mRTTY_U, mDATA_FM, mFM_N, mDATA_U, mAM_N, mPSK, mDATA_FMN };
//  0,    1,    2,    3,    4,    5,       6,     7,      8,       9,        10,    11,      12,    13,      14		// mode index
	6,    6,    9,    0,    0,   10,       9,     6,     10,       0,         0,     6,       0,     5,       0 
};
static int defBW_wide[NUM_MODES] = {
//  mLSB, mUSB, mCW_U, mFM, mAM, mRTTY_L, mCW_L, mDATA_L, mRTTY_U, mDATA_FM, mFM_N, mDATA_U, mAM_N, mPSK, mDATA_FMN };
//  0,    1,    2,    3,    4,    5,       6,     7,      8,       9,        10,    11,      12,    13,      14		// mode index
	13,  13,   16,    0,    0,   10,      16,    17,     10,       0,         0,    17,       0,     9,       0 
};

static int mode_bwA[NUM_MODES] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
static int mode_bwB[NUM_MODES] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

static const char *FTdx10modes_[] = {
"LSB", "USB", "CW-U", "FM", "AM", 
"RTTY-L", "CW-L", "DATA-L", "RTTY-U", "DATA-FM",
"FM-N", "DATA-U", "AM-N", "PSK", "DATA-FMN", NULL};

static const char FTdx10_mode_chr[] =  { '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
static const char FTdx10_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'L', 'L', 'L', 'U', 'U', 'U', 'U', 'U', 'U', 'U' };

static const char *FTdx10_widths_SSB[] = {
"Default",										// default width for mode
 "300",  "400",  "600",  "850", "1100", 	// 1 ... 5
"1200", "1500", "1650", "1800", "1950",		// 6 ... 10
"2100", "2250", "2400", "2450", "2500",		// 7 ... 15
"2600", "2700", "2800", "2900", "3000",		// 16 ... 20
"3200", "3500", "4000", NULL };				// 21 ... 23

static int FTdx10_wvals_SSB[] = {
0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23, WVALS_LIMIT};

static const char *FTdx10_widths_CW[] = {
"Default",										// default for mode
  "50",  "100",  "150",  "200",  "250",		// 1 ... 5
 "300",  "350",  "400",  "450",  "500",		// 6 ... 10
 "600",  "800", "1200", "1400", "1700",		// 11 ... 15
"2000", "2400", "3000", NULL };				// 16 ... 18

static int FTdx10_wvals_CW[] = {
0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18, WVALS_LIMIT };

static const char *FTdx10_widths_RTTY[] = {
"Default",										// default for mode
  "50",  "100",  "150",  "200",  "250",		// 1 ... 5
 "300",  "350",  "400",  "450",  "500",		// 6 ... 10
 "600",  "800", "1200", "1400", "1700",		// 11 ... 15
"2000", "2400", "3000", NULL };				// 16 ... 18

static int FTdx10_wvals_RTTY[] = {
0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18, WVALS_LIMIT };

static const char *FTdx10_widths_DATA[] = {
"Default",										// default for mode
  "50",  "100",  "150",  "200",  "250",		// 1 ... 5
 "300",  "350",  "400",  "450",  "500",		// 6 ... 10
 "600",  "800", "1200", "1400", "1700",		// 11 ... 15
"2000", "2400", "3000", NULL };				// 16 ... 18

static int FTdx10_wvals_PSK[] = {
0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18, WVALS_LIMIT };

static const int FTdx10_wvals_AMFM[] = { 0, WVALS_LIMIT };

static const char *FTdx10_widths_AMwide[] = { "9000", NULL };
static const char *FTdx10_widths_AMnar[]  = { "6000", NULL };
static const char *FTdx10_widths_FMnar[]  = { "9000", NULL };
static const char *FTdx10_widths_FMwide[] = { "16000", NULL };
static const char *FTdx10_widths_DATA_FM[]  = { "16000", NULL };
static const char *FTdx10_widths_DATA_FMN[] = { "9000", NULL };

static const char *FTdx10_US_60m[] = {NULL, "126", "127", "128", "130", NULL};
// US has 5 60M presets. Using dummy numbers for all.
// First NULL means skip 60m sets in get_band_selection().
// Maybe someone can do a cat command MC; on all 5 presets and add returned numbers above.
// To send cat commands in flrig goto menu Config->Xcvr select->Send Cmd.
//
// UK has 7 60M presets. Using dummy numbers for all.  If you want support,
// Maybe someone can do a cat command MC; on all 7 presets and add returned numbers below.
// static const char *FTdx10_UK_60m[] = {NULL, "126", "127", "128", "130", "131", "132", NULL};

static const char **Channels_60m = FTdx10_US_60m;

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

void RIG_FTdx10::initialize()
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
return;
	cmd = "AI0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Auto Info OFF", cmd, replystr);
	sett("Auto Info OFF");

	set_cw_spot();

	get_vfoAorB();
}

RIG_FTdx10::RIG_FTdx10() {
// base class values
	IDstr = "ID";
	name_ = FTdx10name_;
	modes_ = FTdx10modes_;
	bandwidths_ = FTdx10_widths_SSB;
	bw_vals_ = FTdx10_wvals_SSB;

	widgets = rig_widgets;

	serial_baudrate = BR38400;
	stopbits = 1;
	serial_retries = 2;

	serial_write_delay = 0;
	serial_post_write_delay = 0;

	serial_timeout = 50;
	serial_rtscts = true;
	serial_rtsplus = false;
	serial_dtrplus = false;
	serial_catptt = true;
	serial_rtsptt = false;
	serial_dtrptt = false;

	A.imode = B.imode = modeB = modeA = def_mode = 1;
	A.iBW = B.iBW = bwA = bwB = def_bw = 0;
	A.freq = B.freq = freqA = freqB = def_freq = 14070000ULL;

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
	has_cw_break_in =
	has_split =
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
	has_noise_reduction =
	has_noise_reduction_control =
	has_bandwidth_control =
	has_notch_control =
	has_auto_notch =
	has_attenuator_control =
	has_preamp_control =
	has_ifshift_control =
	has_ptt_control =
	has_tune_control = 
	has_xcvr_auto_on_off = true;

// derived specific
	atten_level = 0;
	preamp_level = 0;
	notch_on = false;
	m_60m_indx = 0;

	inuse = onA;

	can_synch_clock = true;

	precision = 1;
	ndigits = 8;

}

void RIG_FTdx10::set_xcvr_auto_on()
{
	cmd = "ID;";
	wait_char(';', 7 , 100, "check", ASC);
//std::cout << "check: " << replystr << std::endl;
	if (replystr.find("ID") != std::string::npos)
		return;

//std::cout << "Xcvr not ON ... power ON cycle" << std::endl;

// wait 1.2 seconds
	for (int i = 0; i < 12; i++) {
		MilliSleep(100);
		update_progress(i * 10);
		Fl::awake();
	}

	cmd = "PS1;";
//	std::cout << "power ON" << std::endl;
	RigSerial->WriteBuffer(cmd.c_str(), cmd.length());

	update_progress(0);

// wait 7 seconds
//	std::cout << "wait 10 seconds" << std::endl;
	for (int i = 0; i < 140; i++) {
		MilliSleep(50);
		update_progress(i);
		Fl::awake();
	}
	update_progress(0);

//	std::cout << "restart serial port" << std::endl;

	RigSerial->OpenPort();
	cmd = "PS;";
	wait_char(';', 4, 100, "closed/reopened port", ASC);
	if (replystr.find("PS1;") == std::string::npos) {
//		std::cout << "Reply to reopen port: " << replystr << std::endl;
		exit(3);
	}

	return;

}

void RIG_FTdx10::set_xcvr_auto_off()
{
	cmd = "PS0;";
	sendCommand(cmd);
	sett("set_xcvr_auto_off");
}

void RIG_FTdx10::get_band_selection(int v)
{
	int inc_60m = false;
	cmd = "IF;";
	wait_char(';', 28, 100, "get band", ASC);

	sett("get band");

	size_t p = replystr.rfind("IF");
	if (p == std::string::npos) return;
	if (replystr[p+22 ] != '0') {	// vfo 60M memory mode
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

//static std::string Avfo = "FA014070000;";
//static std::string Bvfo = "FB007070000;";

bool RIG_FTdx10::check ()
{
#ifdef TESTING
return true;
#endif
	cmd = "ID;";
	wait_char(';', 7 , 500, "check", ASC);
//std::cout << "check: " << replystr << std::endl;

	if (replystr.find("ID") == std::string::npos)
		return false;
	return true;
}

unsigned long long RIG_FTdx10::get_vfoA ()
{
	cmd = rsp = "FA";
	cmd += ';';
	wait_char(';', 12, 100, "get vfo A", ASC);

	gett("get_vfoA()");
//replystr = Avfo;
	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return freqA;
	unsigned long long f = 0;
	for (size_t n = 2; n < 11; n++)
		f = f*10 + replystr[p+n] - '0';
	freqA = f;
	return freqA;
}

void RIG_FTdx10::set_vfoA (unsigned long long freq)
{
	freqA = freq;
	cmd = "FA000000000;";
	for (int i = 10; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vfo A", cmd, replystr);
	sett("SET vfo A");
}

unsigned long long RIG_FTdx10::get_vfoB ()
{
	cmd = rsp = "FB";
	cmd += ';';
	wait_char(';', 12, 100, "get vfo B", ASC);

	gett("get_vfoB()");
//replystr = Bvfo;
	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return freqB;
	unsigned long long f = 0;
	for (size_t n = 2; n < 11; n++)
		f = f*10 + replystr[p+n] - '0';
	freqB = f;
	return freqB;
}


void RIG_FTdx10::set_vfoB (unsigned long long freq)
{
	freqB = freq;
	cmd = "FB000000000;";
	for (int i = 10; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vfo B", cmd, replystr);
	sett("SET vfo B");
}


bool RIG_FTdx10::twovfos()
{
	return true;
}

int RIG_FTdx10::get_vfoAorB()
{
	cmd = "VS;";
	rsp = "VS";
	wait_char(';', 4, 100, "get vfoAorB()", ASC);
	gett("get vfoAorB()");
	size_t p = replystr.rfind(rsp);
	inuse = onA;
	if (p != std::string::npos)
		inuse = (replystr[p + 2] == '1') ? onB : onA;
	return inuse;
}


void RIG_FTdx10::selectA()
{
	cmd = "VS0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "select A", cmd, replystr);
	sett("selectA()");
	inuse = onA;
}

void RIG_FTdx10::selectB()
{
	cmd = "VS1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "select B", cmd, replystr);
	sett("selectB()");
	inuse = onB;
}

void RIG_FTdx10::A2B()
{
	cmd = "AB;";
	sendCommand(cmd);
	showresp(WARN, ASC, "vfo A --> B", cmd, replystr);
	sett("A2B()");
}

bool RIG_FTdx10::can_split()
{
	return true;
}

void RIG_FTdx10::set_split(bool val)
{
	split = val;
	if (val) {
		cmd = "ST1;";
		sendCommand(cmd);
		sett("Split ON");
	} else {
		cmd = "ST0;";
		sendCommand(cmd);
		sett("Split OFF");
	}
}

int RIG_FTdx10::get_split()
{
	cmd = rsp = "FT";
	cmd += ";";
	wait_char(';', 4, 100, "Get split", ASC);
	gett("get split()");
	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return 0;
	int split = replystr[p+2] - '0';

	return (split > 0);
}

void RIG_FTdx10::swapAB()
{
	cmd = "SV;";
	sendCommand(cmd);
	sett("swapAB()");
}


int RIG_FTdx10::get_smeter()
{
	cmd = rsp = "SM0";
	cmd += ';';
	wait_char(';', 7, 100, "get smeter", ASC);

	gett("get_smeter()");

	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return 0;
	if (p + 6 >= replystr.length()) return 0;
	int mtr = atoi(&replystr[p+3]);
	mtr = mtr * 100.0 / 256.0;
	return mtr;
}

int RIG_FTdx10::get_swr()
{
	cmd = rsp = "RM6";
	cmd += ';';
	wait_char(';', 10, 100, "get swr", ASC);

	gett("get_swr()");

	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return 0;
	if (p + 6 >= replystr.length()) return 0;
	replystr[p+6] = 0;
	int mtr = atoi(&replystr[p+3]);
	return mtr / 2.56;
}

struct pwrpair {int mtr; float pwr;};

int RIG_FTdx10::get_power_out()
{
	static pwrpair pwrtbl[] = { 
		{ 35,  5.0 },
		{ 94, 25.0 },
		{147, 50.0 },
		{176, 75.0 },
		{205,100.0 }
	};

	cmd = rsp = "RM5";
	sendCommand(cmd.append(";"));
	wait_char(';', 7, 100, "get pout", ASC);
	gett("get_power_out()");

	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return 0;
 	replystr.erase(p, p + rsp.length());
	replystr.erase(3);
	int mtr = atoi(replystr.c_str());
	size_t i = 0;
	for (i = 0; i < sizeof(pwrtbl) / sizeof(pwrpair) - 1; i++)
		if (mtr >= pwrtbl[i].mtr && mtr < pwrtbl[i+1].mtr)
			break;
	if (mtr < 0) mtr = 0;
	if (mtr > 205) mtr = 205;
	double pwr = (int)ceil(pwrtbl[i].pwr + 
			  (pwrtbl[i+1].pwr - pwrtbl[i].pwr)*(mtr - pwrtbl[i].mtr) / (pwrtbl[i+1].mtr - pwrtbl[i].mtr));

	if (pwr > 100) pwr = 100;

	return pwr;
}

int RIG_FTdx10::get_alc()
{
	cmd = rsp = "RM4";
	cmd += ';';
	wait_char(';',10, 100, "get alc", ASC);
	gett("get_alc");

	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return 0;
	if (p + 9 >= replystr.length()) return 0;
	replystr[6] = '\x00';
	int mtr = atoi(&replystr[p+3]);
	return (int)ceil(mtr / 2.56);
}

// Transceiver power level
double RIG_FTdx10::get_power_control()
{
	cmd = rsp = "PC";
	cmd += ';';
	wait_char(';', 6, 100, "get power", ASC);

	gett("get_power_control()");

	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return progStatus.power_level;
	if (p + 5 >= replystr.length()) return progStatus.power_level;

	int mtr = atoi(&replystr[p+2]);
	return mtr;
}

void RIG_FTdx10::set_power_control(double val)
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
int RIG_FTdx10::get_volume_control()
{
	cmd = rsp = "AG0";
	cmd += ';';
	wait_char(';', 7, 100, "get vol", ASC);

	gett("get_volume_control()");

	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return progStatus.volume;
	if (p + 6 >= replystr.length()) return progStatus.volume;
	int val = atoi(&replystr[p+3]) * 100 / 250;
	if (val > 100) val = 100;
	return val;
}

void RIG_FTdx10::set_volume_control(int val) 
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
void RIG_FTdx10::set_PTT_control(int val)
{
	cmd = val ? "TX1;" : "TX0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "SET PTT", cmd, replystr);
	ptt_ = val;
}

int RIG_FTdx10::get_PTT()
{
	cmd = "TX;";
	rsp = "TX";
	wait_char(';', 4, 100, "get PTT", ASC);

	gett("get_PTT()");

	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return ptt_;
	ptt_ =  (replystr[p+2] != '0' ? 1 : 0);
	return ptt_;
}


void RIG_FTdx10::tune_rig(int val)
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

int RIG_FTdx10::get_tune()
{
	cmd = rsp = "AC";
	cmd += ';';
	wait_char(';', 5, 100, "get tune", ASC);

	rig_trace(2, "get_tuner status()", replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return 0;
	if (replystr[p+4] == '0') return 0;
	return 1;
}


int  RIG_FTdx10::next_attenuator()
{
	switch (atten_level) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 3;
		case 3: return 0;
	}
	return 0;
}

void RIG_FTdx10::set_attenuator(int val)
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

int RIG_FTdx10::get_attenuator()
{
	cmd = rsp = "RA0";
	cmd += ';';
	wait_char(';', 5, 100, "get att", ASC);

	gett("get_attenuator()");

	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return progStatus.attenuator;
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

int  RIG_FTdx10::next_preamp()
{
	switch (preamp_level) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 0;
	}
	return 0;
}

void RIG_FTdx10::set_preamp(int val)
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

int RIG_FTdx10::get_preamp()
{
	cmd = rsp = "PA0";
	cmd += ';';
	wait_char(';', 5, 100, "get pre", ASC);

	gett("get_preamp()");

	size_t p = replystr.rfind(rsp);
	if (p != std::string::npos)
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

static bool narrow = 0; // 0 - wide, 1 - narrow

int RIG_FTdx10::adjust_bandwidth(int val)
{
	int bw = 0;
	if (val == mCW_U || val == mCW_L) {
		bandwidths_ = FTdx10_widths_CW;
		bw_vals_ = FTdx10_wvals_CW;
	} else if (val == mFM || val == mAM || val == mFM_N || val == mDATA_FM || val == mAM_N) {
		if (val == mFM) bandwidths_ = FTdx10_widths_FMwide;
		else if (val ==  mAM) bandwidths_ = FTdx10_widths_AMwide;
		else if (val == mAM_N) bandwidths_ = FTdx10_widths_AMnar;
		else if (val == mFM_N) bandwidths_ = FTdx10_widths_FMnar;
		else if (val == mDATA_FM) bandwidths_ = FTdx10_widths_DATA_FM;
		else if (val == mDATA_FMN) bandwidths_ = FTdx10_widths_DATA_FMN;
		bw_vals_ = FTdx10_wvals_AMFM;
	} else if (val == mRTTY_L || val == mRTTY_U) { // RTTY
		bandwidths_ = FTdx10_widths_RTTY;
		bw_vals_ = FTdx10_wvals_RTTY;
	} else if (val == mDATA_L || val == mDATA_U) { // PSK
		bandwidths_ = FTdx10_widths_DATA;
		bw_vals_ = FTdx10_wvals_PSK;
	} else {
		bandwidths_ = FTdx10_widths_SSB;
		bw_vals_ = FTdx10_wvals_SSB;
	}

	if (narrow)
		bw = defBW_narrow[val];
	else
		bw = defBW_wide[val];

	return bw;
}

int RIG_FTdx10::def_bandwidth(int m)
{
	int bw = adjust_bandwidth(m);
	if (inuse == onB) {
		if (mode_bwB[m] == -1)
			mode_bwB[m] = bw;
		return mode_bwB[m];
	}
	if (mode_bwA[m] == -1)
		mode_bwA[m] = bw;
	return mode_bwA[m];
}

const char ** RIG_FTdx10::bwtable(int n)
{
	switch (n) {
		case mCW_U: case mCW_L:
			return FTdx10_widths_CW;
		case mFM: 
			return FTdx10_widths_FMwide;
		case mAM: 
			return FTdx10_widths_AMwide;
		case mAM_N : 
			return FTdx10_widths_AMnar;
		case mRTTY_L: case mRTTY_U: 
			return FTdx10_widths_RTTY;
		case mDATA_L: case mDATA_U: 
			return FTdx10_widths_DATA;
		case mFM_N: 
			return FTdx10_widths_DATA_FMN;
		case mDATA_FM: 
			return FTdx10_widths_DATA_FM;
		default: ;
	}
	return FTdx10_widths_SSB;
}

void RIG_FTdx10::set_modeA(int val)
{
	modeA = val;
	if (inuse == onB)
		cmd = rsp = "MD1";
	else
		cmd = rsp = "MD0";
	cmd += FTdx10_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mode A", cmd, replystr);
	adjust_bandwidth(modeA);
}

int RIG_FTdx10::get_modeA()
{
	if (inuse == onB)
		cmd = rsp = "MD1";
	else
		cmd = rsp = "MD0";
	cmd += ';';
	wait_char(';', 5, 100, "get mode A", ASC);

	gett("get_modeA()");

	size_t p = replystr.rfind(rsp);
	if (p != std::string::npos) {
		if (p + 3 < replystr.length()) {
			int md = replystr[p+3];
			int n = 0;
			for (n = 0; n < NUM_MODES; n++)
				if (md == FTdx10_mode_chr[n])
					break;
			modeA = n;
		}
	}
	adjust_bandwidth(modeA);
	return modeA;
}

void RIG_FTdx10::set_modeB(int val)
{
	modeB = val;
	if (inuse == onA)
		cmd = rsp = "MD1";
	else
		cmd = rsp = "MD0";
	cmd += FTdx10_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mode B", cmd, replystr);
	adjust_bandwidth(modeA);
}

int RIG_FTdx10::get_modeB()
{
	if (inuse == onA)
		cmd = rsp = "MD1";
	else
		cmd = rsp = "MD0";
	cmd += ';';
	wait_char(';', 5, 100, "get mode B", ASC);

	gett("get_modeB()");

	size_t p = replystr.rfind(rsp);
	if (p != std::string::npos) {
		if (p + 3 < replystr.length()) {
			int md = replystr[p+3];
			int n = 0;
			for (n = 0; n < NUM_MODES; n++)
				if (md == FTdx10_mode_chr[n])
					break;
			modeB = n;
		}
	}
	adjust_bandwidth(modeB);
	return modeB;
}

void RIG_FTdx10::set_bwA(int val)
{
	int bw_indx = bw_vals_[val];
	bwA = val;

	if (modeA == mFM || modeA == mAM || modeA == mFM_N || modeA == mDATA_FM ) {
		return;
	}
	cmd.clear();
	cmd.append("SH00");
	cmd += '0' + bw_indx / 10;
	cmd += '0' + bw_indx % 10;
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET bw A", cmd, replystr);
	sett("SET bwA");
	mode_bwA[modeA] = val;
}

int RIG_FTdx10::get_bwA()
{
	if (modeA == mFM || modeA == mAM || modeA == mFM_N || modeA == mDATA_FM) {
		bwA = 0;
		mode_bwA[modeA] = bwA;
		return bwA;	
	} 
	cmd = rsp = "SH0";
	cmd += ';';
	wait_char(';', 7, 100, "get bw A", ASC);

	gett("get_bwA()");

	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return bwA;

	replystr[p+6] = 0;
	int bw_idx = fm_decimal(replystr.substr(p+4), 2);

	const int *idx = bw_vals_;
	int i = 0;
	while (*idx != WVALS_LIMIT) {
		if (*idx == bw_idx) break;
		idx++;
		i++;
	}
	if (*idx == WVALS_LIMIT) i = 0;
	bwA = i;
	mode_bwA[modeA] = bwA;
	return bwA;
}

void RIG_FTdx10::set_bwB(int val)
{
	int bw_indx = bw_vals_[val];
	bwB = val;

	if (modeB == mFM || modeB == mAM || modeB == mFM_N || modeB == mDATA_FM) {
		mode_bwB[modeB] = 0;
		return;
	}
	cmd.clear();
	cmd.append("SH00");
	cmd += '0' + bw_indx / 10;
	cmd += '0' + bw_indx % 10;
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET bw B", cmd, replystr);
	sett("SET bwB");
	mode_bwB[modeB] = bwB;
}

int RIG_FTdx10::get_bwB()
{
	if (modeB == mFM || modeB == mAM || modeB == mFM_N || modeB == mDATA_FM) {
		bwB = 0;
		mode_bwB[modeB] = bwB;
		return bwB;
	} 
	cmd = rsp = "SH0";
	cmd += ';';
	wait_char(';', 7, 100, "get bw B", ASC);

	gett("get_bwB()");

	size_t p = replystr.rfind(rsp);
	p = replystr.find(rsp);
	if (p == std::string::npos) return bwB;

	replystr[p+6] = 0;
	int bw_idx = fm_decimal(replystr.substr(p+4),2);

	const int *idx = bw_vals_;
	int i = 0;
	while (*idx != WVALS_LIMIT) {
		if (*idx == bw_idx) break;
		idx++;
		i++;
	}
	if (*idx == WVALS_LIMIT) i = 0;
	bwB = i;
	mode_bwB[modeB] = bwB;
	return bwB;
}

std::string RIG_FTdx10::get_BANDWIDTHS()
{
	std::stringstream s;
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_bwA[i] << " ";
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_bwB[i] << " ";
	return s.str();
}

void RIG_FTdx10::set_BANDWIDTHS(std::string s)
{
	std::stringstream strm;
	strm << s;
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_bwA[i];
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_bwB[i];
}

int RIG_FTdx10::get_modetype(int n)
{
	return FTdx10_mode_type[n];
}

void RIG_FTdx10::set_if_shift(int val)
{
	if (inuse == onB)
		cmd = "IS10+0000;";
	else
		cmd = "IS00+0000;";
	if (val != 0) progStatus.shift = true;
	else progStatus.shift = false;
	if (val < 0) cmd[4] = '-';
	val = abs(val);
	for (int i = 8; i > 4; i--) {
		cmd[i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET if shift", cmd, replystr);
}

bool RIG_FTdx10::get_if_shift(int &val)
{
	cmd = rsp = "IS0";
	cmd += ';';
	wait_char(';', 10, 100, "get if shift", ASC);

	gett("get_if_shift()");

	size_t p = replystr.rfind(rsp);
	val = progStatus.shift_val;
	if (p == std::string::npos) return progStatus.shift;
	val = atoi(&replystr[p+5]);
	if (replystr[p+4] == '-') val = -val;
	return (val != 0);
}

void RIG_FTdx10::get_if_min_max_step(int &min, int &max, int &step)
{
	if_shift_min = min = -1200;
	if_shift_max = max = 1200;
	if_shift_step = step = 20;
	if_shift_mid = 0;
}

/*
BPabcde;
a: Fixed, '0'

b: Manual NOTCH ON/OFF, 1/0

cde: 001 - 320, (NOTCH Frequency : x 10 Hz )
*/
static std::string notch_str_on  = "BP00001;";
static std::string notch_str_off = "BP00000;";
static std::string notch_str_val = "BP01000;";
static int notch_val = 1500;

void RIG_FTdx10::set_notch(bool on, int val)
{
	if (notch_val != val) {
		cmd = notch_str_on;
		sendCommand(cmd);
		showresp(WARN, ASC, "SET notch ON", cmd, replystr);
		set_trace(3,"set_notch ON", cmd.c_str(), replystr.c_str());
// set notch frequency
		notch_val = val;
		val /= 10;
		for (int i = 0; i < 3; i++) {
			notch_str_val[6 - i] = '0' + (val % 10);
			val /= 10;
		}
		cmd = notch_str_val;
// set notch ON
		sendCommand(cmd);
		showresp(WARN, ASC, "SET notch val", cmd, replystr);
		set_trace(3,"set_notch val", cmd.c_str(), replystr.c_str());
	}
	if (on)
		cmd = notch_str_on;
	else
		cmd = notch_str_off;
	sendCommand(cmd);
	set_trace(3,"set_notch OFF", cmd.c_str(), replystr.c_str());
	showresp(WARN, ASC, "SET notch OFF", cmd, replystr);

}

bool  RIG_FTdx10::get_notch(int &val)
{
	bool ison = false;

	cmd = "BP00;";
	rsp = "BP";
	wait_char(';', 8, 100, "get notch on/off", ASC);
	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return ison;

	gett("get_notch()");

	if (replystr[p+6] == '1') // manual notch enabled
		ison = true;

	val = progStatus.notch_val;
	cmd = "BP01;";
	rsp = "BP";
	wait_char(';', 8, 100, "get notch val", ASC);

	gett("get_notch_val()");

	p = replystr.rfind(rsp);
	if (p == std::string::npos)
		val = 10;
	else
		val = fm_decimal(replystr.substr(p+4), 3) * 10;

	return (notch_on = ison);
}

void RIG_FTdx10::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = 10;
	max = 3200;
	step = 10;
}

void RIG_FTdx10::set_auto_notch(int v)
{
	if (inuse == onB)
		cmd = "BC10;";
	else
		cmd = "BC00;";
	if (v) cmd[3] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET auto notch", cmd, replystr);
}

int  RIG_FTdx10::get_auto_notch()
{
	cmd = "BC0;";
	wait_char(';', 5, 100, "get auto notch", ASC);

	gett("get_auto_notch()");

	size_t p = replystr.rfind("BC");
	if (p == std::string::npos) return 0;
	if (replystr[p+3] == '1') return 1;
	return 0;
}

int FTdx10_blanker_level = 0;

void RIG_FTdx10::set_noise(bool b)
{
	if (inuse == onB)
		cmd = "NB10;";
	else
		cmd = "NB00;";

	if (b) {
		cmd[3] = '1';
		nb_label("NB on", true);
	} else
		nb_label("NB", false);

	sendCommand (cmd);
	showresp(WARN, ASC, "SET NB", cmd, replystr);
}

int RIG_FTdx10::get_noise()
{
	cmd = rsp = "NB0";
	cmd += ';';
	wait_char(';', 5, 100, "get NB", ASC);

	gett("get_noise()");

	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return FTdx10_blanker_level;

	FTdx10_blanker_level = replystr[p+3] - '0';

	if (FTdx10_blanker_level) {
		nb_label("NB on", true);
	} else
		nb_label("NB", false);

	return FTdx10_blanker_level;
}

// val 0 .. 100
void RIG_FTdx10::set_mic_gain(int val)
{
	cmd = "MG000;";
	for (int i = 3; i > 0; i--) {
		cmd[1+i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mic", cmd, replystr);
}

int RIG_FTdx10::get_mic_gain()
{
	cmd = rsp = "MG";
	cmd += ';';
	wait_char(';', 6, 100, "get mic", ASC);

	gett("get_mic_gain()");

	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return progStatus.mic_gain;
	int val = atoi(&replystr[p+2]);
	return val;
}

void RIG_FTdx10::get_mic_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_FTdx10::set_rf_gain(int val)
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

int  RIG_FTdx10::get_rf_gain()
{
	int rfval = 0;
	cmd = rsp = "RG0";
	cmd += ';';
	wait_char(';', 7, 100, "get rfgain", ASC);

	gett("get_rf_gain()");

	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return progStatus.rfgain;
	for (int i = 3; i < 6; i++) {
		rfval *= 10;
		rfval += replystr[p+i] - '0';
	}
	rfval = rfval * 100 / 250;
	if (rfval > 100) rfval = 100;
	return rfval;
}

void RIG_FTdx10::get_rf_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_FTdx10::set_vox_onoff()
{
	cmd = "VX0;";
	if (progStatus.vox_onoff) cmd[2] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox", cmd, replystr);
}

void RIG_FTdx10::set_vox_gain()
{
	cmd = "VG";
	cmd.append(to_decimal(progStatus.vox_gain, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox gain", cmd, replystr);
}

void RIG_FTdx10::set_vox_anti()
{
}

void RIG_FTdx10::set_vox_hang()
{
	cmd = "VD";
	cmd.append(to_decimal(progStatus.vox_hang, 4)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox delay", cmd, replystr);
}

void RIG_FTdx10::set_vox_on_dataport()
{
	cmd = "EX0304050;";
	if (progStatus.vox_on_dataport) cmd[8] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vox on data port", cmd, replystr);
}

void RIG_FTdx10::set_cw_wpm()
{
	cmd = "KS";
	if (progStatus.cw_wpm > 60) progStatus.cw_wpm = 60;
	if (progStatus.cw_wpm < 4) progStatus.cw_wpm = 4;
	cmd.append(to_decimal(progStatus.cw_wpm, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw wpm", cmd, replystr);
}


void RIG_FTdx10::enable_keyer()
{
	cmd = "KR0;";
	if (progStatus.enable_keyer) cmd[2] = '1';
	sendCommand(cmd);
	showresp(WARN, ASC, "SET keyer on/off", cmd, replystr);
}

bool RIG_FTdx10::set_cw_spot()
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

void RIG_FTdx10::set_cw_weight()
{
	int n = round(progStatus.cw_weight * 10);
	cmd.assign("EX020205").append(to_decimal(n, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw weight", cmd, replystr);
}

void RIG_FTdx10::set_cw_qsk()
{
	int n = progStatus.cw_qsk / 5 - 3;
	cmd.assign("EX0202116").append(to_decimal(n, 1)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET cw qsk", cmd, replystr);
}

void RIG_FTdx10::set_break_in()
{
	if (progStatus.break_in) {
		cmd = "BI1;";
		break_in_label("BK-IN");
	} else {
		cmd = "BI0;";
		break_in_label("QSK ?");
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET break in on/off", cmd, replystr);
	sett("set_break_in");
}

int RIG_FTdx10::get_break_in()
{
	cmd = "BI;";
	wait_char(';', 4, 100, "get break in", ASC);
	progStatus.break_in = (replystr[2] == '1');
	if (progStatus.break_in) {
		break_in_label("BK-IN");
		progStatus.cw_delay = 0;
	} else {
		break_in_label("QSK ?");
//		get_qsk_delay();
	}
	return progStatus.break_in;
}

// DNR
void RIG_FTdx10::set_noise_reduction_val(int val)
{
	cmd.assign("RL0").append(to_decimal(val, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET_noise_reduction_val", cmd, replystr);
	sett("set_noise_reduction_val");
}

int  RIG_FTdx10::get_noise_reduction_val()
{
	int val = 1;
	cmd = rsp = "RL0";
	cmd.append(";");
	wait_char(';',6, 100, "GET noise reduction val", ASC);
	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return val;
	val = atoi(&replystr[p+3]);
	return val;
}

// DNR
void RIG_FTdx10::set_noise_reduction(int val)
{
	cmd.assign("NR0").append(val ? "1" : "0" ).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET noise reduction", cmd, replystr);
	sett("set_noise_reduction_on/off");
}

int  RIG_FTdx10::get_noise_reduction()
{
	int val;
	cmd = rsp = "NR0";
	cmd.append(";");
	wait_char(';',5, 100, "GET noise reduction", ASC);
	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return 0;
	val = replystr[p+3] - '0';
	return val;
}

// ---------------------------------------------------------------------
// set date and time
// ---------------------------------------------------------------------
// dt formated as YYYYMMDD
// ---------------------------------------------------------------------
void RIG_FTdx10::sync_date(char *dt)
{
	cmd.assign("DT0");
	cmd.append(dt);
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "sync_date", cmd, replystr);
	sett("sync_date");
}

// ---------------------------------------------------------------------
// tm formated as HH:MM:SS
// ---------------------------------------------------------------------
void RIG_FTdx10::sync_clock(char *tm)
{
	cmd.assign("DT1");
	cmd += tm[0]; cmd += tm[1];
	cmd += tm[3]; cmd += tm[4];
	cmd += tm[6]; cmd += tm[7];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "sync_time", cmd, replystr);
	sett("sync_time");
}

