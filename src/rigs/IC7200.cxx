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

#include "IC7200.h"
#include "support.h"
#include "trace.h"

//=============================================================================
// IC-7200

const char IC7200name_[] = "IC-7200";

// these are only defined in this file
// undef'd at end of file
#define NUM_FILTERS 3
#define NUM_MODES  9

enum {
	LSB7200, USB7200, AM7200,
	CW7200, RTTY7200,
	CWR7200, RTTYR7200,
	LSBD7200, USBD7200 };

static int mode_filterA[NUM_MODES] = {1,1,1,1,1,1,1,1,1};
static int mode_filterB[NUM_MODES] = {1,1,1,1,1,1,1,1,1};

static int mode_bwA[NUM_MODES] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,};
static int mode_bwB[NUM_MODES] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,};

static const char *szfilter[NUM_FILTERS] = {"W", "M", "N"};

const char *IC7200modes_[] = { 
"LSB", "USB", "AM", "CW", "RTTY", "CW-R", "RTTY-R", 
"LSB-D", "USB-D", NULL};

const char mdval[] = { 0, 1, 2, 3, 4, 7, 8, 0, 1};

static char IC7200_mode_type[] = { 
'L', 'U', 'U', 'L', 'L', 'U', 'U',
'L', 'U' };

const char *IC7200_SSBwidths[] = {
  "50",  "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700", "2800", "2900", "3000", "3100", "3200", "3300", "3400", "3500",
"3600",
NULL};
static int IC7200_bw_vals_SSB[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,
41, WVALS_LIMIT};

const char *IC7200_RTTYwidths[] = {
  "50",  "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
 "600",  "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700",
NULL};
static int IC7200_bw_vals_RTTY[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32, WVALS_LIMIT};

const char *IC7200_AMwidths[] = {
  "200",  "400",  "600",  "800", "1000", "1200", "1400", "1600", "1800", "2000",
 "2200", "2400", "2600", "2800", "3000", "3200", "3400", "3600", "3800", "4000",
 "4200", "4400", "4600", "4800", "5000", "5200", "5400", "5600", "5800", "5000",
 "6200", "6400", "6600", "6800", "7000", "7200", "7400", "7600", "7800", "8000",
NULL};
static int IC7200_bw_vals_AM[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,
WVALS_LIMIT};

//======================================================================
// IC7200 unique commands
//======================================================================

static GUI IC7200_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 },	//0
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 },	//1
	{ (Fl_Widget *)btnAGC,        2, 145,  50 },	//2
	{ (Fl_Widget *)sldrRFGAIN,   54, 145, 156 },	//3
	{ (Fl_Widget *)sldrSQUELCH,  54, 165, 156 },	//4
	{ (Fl_Widget *)btnNR,         2, 185,  50 },	//5
	{ (Fl_Widget *)sldrNR,       54, 185, 156 },	//6
	{ (Fl_Widget *)btnLOCK,     214, 105,  50 },	//7
	{ (Fl_Widget *)sldrINNER,   266, 105, 156 },	//8
	{ (Fl_Widget *)btnCLRPBT,   214, 125,  50 },	//9
	{ (Fl_Widget *)sldrOUTER,   266, 125, 156 },	//10
	{ (Fl_Widget *)btnNotch,    214, 145,  50 },	//11
	{ (Fl_Widget *)sldrNOTCH,   266, 145, 156 },	//12
	{ (Fl_Widget *)sldrMICGAIN, 266, 165, 156 },	//13
	{ (Fl_Widget *)sldrPOWER,   266, 185, 156 },	//14
	{ (Fl_Widget *)NULL, 0, 0, 0 }
};

void RIG_IC7200::initialize()
{
	IC7200_widgets[0].W = btnVol;
	IC7200_widgets[1].W = sldrVOLUME;
	IC7200_widgets[2].W = btnAGC;
	IC7200_widgets[3].W = sldrRFGAIN;
	IC7200_widgets[4].W = sldrSQUELCH;
	IC7200_widgets[5].W = btnNR;
	IC7200_widgets[6].W = sldrNR;
	IC7200_widgets[7].W = btnLOCK;
	IC7200_widgets[8].W = sldrINNER;
	IC7200_widgets[9].W = btnCLRPBT;
	IC7200_widgets[10].W = sldrOUTER;
	IC7200_widgets[11].W = btnNotch;
	IC7200_widgets[12].W = sldrNOTCH;
	IC7200_widgets[13].W = sldrMICGAIN;
	IC7200_widgets[14].W = sldrPOWER;

	btn_icom_select_11->deactivate();
	btn_icom_select_12->deactivate();
	btn_icom_select_13->deactivate();

	choice_rTONE->deactivate();
	choice_tTONE->deactivate();
}

RIG_IC7200::RIG_IC7200() {
	name_ = IC7200name_;
	modes_ = IC7200modes_;
	_mode_type = IC7200_mode_type;
	bandwidths_ = IC7200_SSBwidths;
	bw_vals_ = IC7200_bw_vals_SSB;
	widgets = IC7200_widgets;

	comm_baudrate = BR9600;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_echo = true;
	comm_rtscts = false;
	comm_rtsplus = true;
	comm_dtrplus = true;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;

	def_freq = A.freq = 14070000;
	def_mode = A.imode = 1;
	def_bw = A.iBW = 32;

	B.freq = 7070000;
	B.imode = 1;
	B.iBW = 32;

	has_extras = true;
	has_smeter = true;
	has_power_out = true;
	has_swr_control = true;
	has_alc_control = true;
	has_agc_control = true;
	has_sql_control = true;
	has_power_control = true;
	has_volume_control = true;
	has_mode_control = true;
	has_bandwidth_control = true;
	has_micgain_control = true;
	has_attenuator_control = true;
	has_preamp_control = true;
	has_noise_control = true;
	has_nb_level = true;
	has_noise_reduction = true;
	has_noise_reduction_control = true;
	has_auto_notch = true;
	has_notch_control = true;
	has_pbt_controls = true;
	has_FILTER = true;
	has_rf_control = true;
	has_compON = true;
	has_compression = true;
	has_vox_onoff = true;
	has_ptt_control = true;
	has_tune_control = true;
	has_split_AB = true;
	has_vfo_adj = true;
	has_a2b = true;

	has_band_selection = true;

	has_cw_wpm = true;
	has_cw_spot_tone = true;
	has_cw_qsk = true;
	has_cw_break_in = true;

	defaultCIV = 0x76;
	adjustCIV(defaultCIV);

	precision = 1;
	ndigits = 8;

	CW_sense = 0; // normal
};

static inline void minmax(int min, int max, int &val)
{
	if (val > max) val = max;
	if (val < min) val = min;
}

//======================================================================

void RIG_IC7200::selectA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x00';
	cmd.append(post);
	set_trace(2, "selectA()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("select A");
	inuse = onA;
}

void RIG_IC7200::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x01';
	cmd.append(post);
	set_trace(2, "selectB()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("select B");
	inuse = onB;
}

bool RIG_IC7200::check ()
{
	string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	bool ok = waitFOR(11, "check vfo");
	get_trace(2, "check()", str2hex(replystr.c_str(), replystr.length()));
	return ok;
}

long RIG_IC7200::get_vfoA ()
{
	if (useB) return A.freq;
	string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	if (waitFOR(11, "get vfo A")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			A.freq = fm_bcd_be(replystr.substr(p+5), 10);
	}
	get_trace(2, "get_vfoA()", str2hex(replystr.c_str(), replystr.length()));
	return A.freq;
}

void RIG_IC7200::set_vfoA (long freq)
{
	A.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	set_trace(2, "set_vfoA()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("set vfo A");
}

long RIG_IC7200::get_vfoB ()
{
	if (!useB) return B.freq;
	string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	if (waitFOR(11, "get vfo B")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			B.freq = fm_bcd_be(replystr.substr(p+5), 10);
	}
	get_trace(2, "get_vfoB()", str2hex(replystr.c_str(), replystr.length()));
	return B.freq;
}

void RIG_IC7200::set_vfoB (long freq)
{
	B.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	set_trace(2, "set_vfoB()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("set vfo B");
}

bool RIG_IC7200::can_split()
{
	return true;
}

void RIG_IC7200::set_split(bool val)
{
	split = val;
	cmd = pre_to;
	cmd += 0x0F;
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	waitFB(val ? "set split ON" : "set split OFF");
	set_trace(2, "set_split()", str2hex(cmd.c_str(), cmd.length()));
}

// 7200 does not respond to get split CAT command
int RIG_IC7200::get_split()
{
	return split;
}

// Tranceiver PTT on/off
void RIG_IC7200::set_PTT_control(int val)
{
	cmd = pre_to;
	cmd += '\x1c';
	cmd += '\x00';
	cmd += (val ? '\x01' : '\x00');
	cmd.append( post );
	if (val)
		waitFB("set ptt ON");
	else
		waitFB("set ptt OFF");
	ptt_ = val;
	set_trace(2, "set_PTT()", str2hex(cmd.c_str(), cmd.length()));
}

int RIG_IC7200::get_PTT()
{
	cmd = pre_to;
	cmd += '\x1c'; cmd += '\x00';
	string resp = pre_fm;
	resp += '\x1c'; resp += '\x00';
	cmd.append(post);
	if (waitFOR(8, "get PTT")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			ptt_ = replystr[p + 6];
	}
	get_trace(2, "get_PTT()", str2hex(replystr.c_str(), replystr.length()));
	return ptt_;
}

// Volume control val 0 ... 100
void RIG_IC7200::set_volume_control(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set vol");
	set_trace(2, "set_volume_control()", str2hex(cmd.c_str(), cmd.length()));
}

int RIG_IC7200::get_volume_control()
{
	int val = progStatus.volume;
	string cstr = "\x14\x01";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(9, "get vol")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			val = num100(replystr.substr(p+6));
	}
	get_trace(2, "get_volume_control()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

void RIG_IC7200::get_vol_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

int RIG_IC7200::get_smeter()
{
	string cstr = "\x15\x02";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;
	if (waitFOR(9, "get smeter")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			mtr = (int)ceil(mtr /2.55);
			if (mtr > 100) mtr = 100;
		}
	}
	get_trace(2, "get_smeter()", str2hex(replystr.c_str(), replystr.length()));
	return mtr;
}

struct pwrpair {int mtr; float pwr;};

static pwrpair pwrtbl[] = { 
{0, 0.0},
{40, 10.0},
{76, 20.0},
{92, 25.0},
{103, 30.0},
{124, 40.0},
{143, 50.0},
{183, 75.0},
{213, 100.0},
{255, 140.0} };

int RIG_IC7200::get_power_out(void)
{
	string cstr = "\x15\x11";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= 0;
	if (waitFOR(9, "get power out")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			size_t i = 0;
			for (i = 0; i < sizeof(pwrtbl) / sizeof(pwrpair) - 1; i++)
				if (mtr >= pwrtbl[i].mtr && mtr < pwrtbl[i+1].mtr)
					break;
			if (mtr < 0) mtr = 0;
			if (mtr > 255) mtr = 255;
			mtr = (int)ceil(pwrtbl[i].pwr + 
				(pwrtbl[i+1].pwr - pwrtbl[i].pwr)*(mtr - pwrtbl[i].mtr)/(pwrtbl[i+1].mtr - pwrtbl[i].mtr));
			
			if (mtr > 100) mtr = 100;
		}
	}
	get_trace(2, "get_power_out()", str2hex(replystr.c_str(), replystr.length()));
	return mtr;
}

int RIG_IC7200::get_swr(void) 
{
	string cstr = "\x15\x12";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;
	if (waitFOR(9, "get swr")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			mtr = (int)ceil(mtr /2.55);
			if (mtr > 100) mtr = 100;
		}
	}
	get_trace(2, "get_swr()", str2hex(replystr.c_str(), replystr.length()));
	return mtr;
}

int RIG_IC7200::get_alc(void)
{
	string cstr = "\x15\x13";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;
	if (waitFOR(9, "get alc")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			mtr = (int)ceil(mtr /2.55);
			if (mtr > 100) mtr = 100;
		}
	}
	get_trace(2, "get_alc()", str2hex(replystr.c_str(), replystr.length()));
	return mtr;
}

void RIG_IC7200::set_noise(bool val)
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set noise");
	set_trace(2, "set_noise()", str2hex(cmd.c_str(), cmd.length()));
}

int RIG_IC7200::get_noise()
{
	int val = progStatus.noise;
	string cstr = "\x16\x22";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(8, "get noise")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			val = replystr[p+6];
		}
	}
	get_trace(2, "get_noise()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

void RIG_IC7200::set_nb_level(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x12");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set NB level");
	set_trace(2, "set_nb_level()", str2hex(cmd.c_str(), cmd.length()));
}

int  RIG_IC7200::get_nb_level()
{
	int val = progStatus.nb_level;
	string cstr = "\x14\x12";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get NB level")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			val = num100(replystr.substr(p+6));
	}
	get_trace(2, "get_nb_level()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

void RIG_IC7200::set_noise_reduction(int val)
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set NR");
	set_trace(2, "set_noise_reduction()", str2hex(cmd.c_str(), cmd.length()));
}

int RIG_IC7200::get_noise_reduction()
{
	string cstr = "\x16\x40";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(8, "get NR")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (replystr[p+6] ? 1 : 0);
	}
	get_trace(2, "get_noise_reduction()", str2hex(replystr.c_str(), replystr.length()));
	return progStatus.noise_reduction;
}

// 0 < val < 100
void RIG_IC7200::set_noise_reduction_val(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x06");
	val = val * 16 + 8;
	cmd.append(to_bcd(val, 3));
	cmd.append(post);
	waitFB("set NRval");
	set_trace(2, "set_noise_reduction_val()", str2hex(cmd.c_str(), cmd.length()));
}

int RIG_IC7200::get_noise_reduction_val()
{
	int val = progStatus.noise_reduction_val;
	string cstr = "\x14\x06";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get NRval")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			val = fm_bcd(replystr.substr(p+6), 3);
			val = (val - 8) / 16;
		}
	}
	get_trace(2, "get_nr_val()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

void RIG_IC7200::set_attenuator(int val)
{
	if (val) {
		atten_label("20 dB", true);
		atten_level = 1;
		set_preamp(0);
	} else {
		atten_label("ATT", false);
		atten_level = 0;
	}

	cmd = pre_to;
	cmd += '\x11';
	cmd += atten_level ? '\x20' : '\x00';
	cmd.append( post );
	waitFB("set att");
	set_trace(2, "set_attenuator()", str2hex(cmd.c_str(), cmd.length()));
}

int RIG_IC7200::next_attenuator()
{
	if (atten_level) return 0;
	return 1;
}

int RIG_IC7200::get_attenuator()
{
	cmd = pre_to;
	cmd += '\x11';
	cmd.append( post );
	string resp = pre_fm;
	resp += '\x11';
	if (waitFOR(7, "get ATT")) {
		get_trace(2, "get_ATT()", str2hex(replystr.c_str(), replystr.length()));
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			if (!replystr[p+5]) {
				atten_label("ATT", false);
				atten_level = 0;
				return 0;
			} else {
				atten_label("20 dB", true);
				atten_level = 1;
				return 1;
			}
		}
	}
	return 0;
}

int RIG_IC7200::next_preamp()
{
	if (preamp_level)
		return 0;
	return 1;
}

void RIG_IC7200::set_preamp(int val)
{
	if (val) {
		preamp_label("Pre ON", true);
		preamp_level = 1;
		if (atten_level == 1) {
			atten_label("ATT", false);
			atten_level = 0;
		}
	} else {
		preamp_label("Pre", false);
		preamp_level = 0;
	}

	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd += preamp_level ? 0x01 : 0x00;
	cmd.append( post );
	waitFB("set Pre");
	set_trace(2, "set_preamp()", str2hex(cmd.c_str(), cmd.length()));
}

int RIG_IC7200::get_preamp()
{
	string cstr = "\x16\x02";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get Pre")) {
		get_trace(2, "get_preamp()", str2hex(replystr.c_str(), replystr.length()));
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			if (replystr[p+6] == 0x01) {
				preamp_label("Pre ON", true);
				preamp_level = 1;
//				progStatus.preamp = true;
			} else {
				preamp_label("Pre", false);
				preamp_level = 0;
//				progStatus.preamp = false;
			}
		}
	}
	return preamp_level; //progStatus.preamp;
}

void RIG_IC7200::set_rf_gain(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x02");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set RF");
	set_trace(2, "set_rf_gain()", str2hex(cmd.c_str(), cmd.length()));
}

int RIG_IC7200::get_rf_gain()
{
	int val = progStatus.rfgain;
	string cstr = "\x14\x02";
	string resp = pre_fm;
	cmd = pre_to;
	cmd.append(cstr).append(post);
	resp.append(cstr);
	if (waitFOR(9, "get RF")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			val = num100(replystr.substr(p + 6));
	}
	get_trace(2, "get_rf_gain()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

void RIG_IC7200::set_squelch(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x03");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set Sqlch");
	set_trace(2, "set_squelch()", str2hex(cmd.c_str(), cmd.length()));
}

int  RIG_IC7200::get_squelch()
{
	int val = progStatus.squelch;
	string cstr = "\x14\x03";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get squelch")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			val = num100(replystr.substr(p+6));
	}
	get_trace(2, "get_squelch()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

void RIG_IC7200::set_power_control(double val)
{
	cmd = pre_to;
	cmd.append("\x14\x0A");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set power");
	set_trace(2, "set_power_control()", str2hex(cmd.c_str(), cmd.length()));
}

int RIG_IC7200::get_power_control()
{
	string cstr = "\x14\x0A";
	string resp = pre_fm;
	cmd = pre_to;
	cmd.append(cstr).append(post);
	resp.append(cstr);
	int val = progStatus.power_level;
	string retstr = "ret str";
	if (waitFOR(9, "get power")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			val = num100(replystr.substr(p+6));
			retstr = str2hex(replystr.substr(p).c_str(), 9);
		}
	}
	get_trace(2, "get_power_control()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

void RIG_IC7200::get_mic_gain_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

int RIG_IC7200::get_mic_gain()
{
	string cstr = "\x14\x0B";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get mic")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			return num100(replystr.substr(p + 6));
		}
	}
	get_trace(2, "get_mic_gain()", str2hex(replystr.c_str(), replystr.length()));
	return 0;
}

void RIG_IC7200::set_mic_gain(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x0B");
	cmd.append(bcd255(val));
	cmd.append(post);
	waitFB("set mic");
	set_trace(2, "set_mic_gain()", str2hex(cmd.c_str(), cmd.length()));
}

int RIG_IC7200::get_modeA()
{
	int md = A.imode;
	size_t p = 0;

	string resp = pre_fm;
	resp += '\x04';
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);

	if (waitFOR(8, "get mode A")) {
		p = replystr.rfind(resp);
		if (p != string::npos) {
			md = replystr[p + 5];
			if (md > 6) md -= 2;
			A.filter = replystr[p+6];
		}
	}
	get_trace(2, "get_modeA()", str2hex(replystr.c_str(), replystr.length()));

	cmd = pre_to;
	cmd += "\x1A\x04";
	cmd.append(post);
	resp = pre_fm;
	resp += "\x1A\x04";
	if (waitFOR(9, "data mode?")) {
		p = replystr.rfind(resp);
		if (p != string::npos) {
			if ((replystr[p+6] & 0x01) == 0x01) {
				if (md == 0) md = 7;
				if (md == 1) md = 8;
			}
		}
	}
	if (A.filter > 0 && A.filter < 4)
		mode_filterA[A.imode] = A.filter;

	get_trace(2, "get_data_modeA()", str2hex(replystr.c_str(), replystr.length()));
	get_trace(4, "mode_filterA[", IC7200modes_[md], "] = ", szfilter[A.filter-1]);

	if (A.imode == CW7200 || A.imode == CWR7200) {
		cmd.assign(pre_to).append("\x1A\x03\x37");
		resp.assign(pre_fm).append("\x1A\x03\x37");
		if (waitFOR(9, "get CW sideband")) {
			p = replystr.rfind(resp);
			CW_sense = replystr[p + 7];
			if (CW_sense) IC7200_mode_type[A.imode] = 'U';
			else IC7200_mode_type[A.imode] = 'L';
		}
	}

	return (A.imode = md);
}

void RIG_IC7200::set_modeA(int val)
{
	A.imode = val;

	cmd = pre_to;
	cmd += '\x06';
	cmd += mdval[A.imode];
	cmd += mode_filterA[A.imode];
	cmd.append( post );

	waitFB("set mode A, default filter");

	set_trace(4, "set mode A[", IC7200modes_[A.imode], "] ", str2hex(replystr.c_str(), replystr.length()));

	if (val < 7) return;

	cmd = pre_to;
	cmd += '\x1A'; cmd += '\x04';
	cmd += '\x01';
	cmd += mode_filterA[A.imode];
	cmd.append( post);
	waitFB("set data mode A");

	set_trace(2, "set_data_modeA()", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC7200::get_modeB()
{
	int md = B.imode;
	size_t p = 0;

	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	string resp = pre_fm;
	resp += '\x04';
	if (waitFOR(8, "get mode B")) {
		p = replystr.rfind(resp);
		if (p != string::npos) {
			md = replystr[p+5];
			if (md > 6) md -= 2;
			B.filter = replystr[p+6];
		}
	}
	get_trace(2, "get_modeB()", str2hex(replystr.c_str(), replystr.length()));

	cmd = pre_to;
	cmd += "\x1A\x04";
	cmd.append(post);
	resp = pre_fm;
	resp += "\x1A\x04";
	if (waitFOR(9, "get data B")) {
		p = replystr.rfind(resp);
		if (p != string::npos) {
			if ((replystr[p+6] & 0x01) == 0x01) {
				if (md == 0) md = 7;
				if (md == 1) md = 8;
			}
		}
	}
	if (B.filter > 0 && B.filter < 4)
		mode_filterB[B.imode] = B.filter;

	get_trace(2, "get_data_modeB()", str2hex(replystr.c_str(), replystr.length()));
	get_trace(4, "mode_filterB[", IC7200modes_[md], "] = ", szfilter[B.filter-1]);

	if (B.imode == CW7200 || B.imode == CWR7200) {
		cmd.assign(pre_to).append("\x1A\x03\x37");
		resp.assign(pre_fm).append("\x1A\x03\x37");
		if (waitFOR(9, "get CW sideband")) {
			p = replystr.rfind(resp);
			CW_sense = replystr[p + 7];
			if (CW_sense) IC7200_mode_type[B.imode] = 'U';
			else IC7200_mode_type[B.imode] = 'L';
		}
	}

	return (B.imode = md);
}

void RIG_IC7200::set_modeB(int val)
{
	B.imode = val;

	cmd = pre_to;
	cmd += '\x06';
	cmd += mdval[B.imode];
	cmd += mode_filterB[B.imode];
	cmd.append( post );
	waitFB("set mode B, default filter");

	set_trace(4, "set mode B[", IC7200modes_[B.imode], "] ", str2hex(replystr.c_str(), replystr.length()));

	if (val < 7) return;

	cmd = pre_to;
	cmd += '\x1A'; cmd += '\x04';
	cmd += '\x01';
	cmd += mode_filterB[B.imode];
	cmd.append( post);
	waitFB("set data mode B");

	set_trace(2, "set data mode B ",  str2hex(replystr.c_str(), replystr.length()));

}

int RIG_IC7200::get_modetype(int n)
{
	return _mode_type[n];
}

int RIG_IC7200::get_FILT(int mode)
{
	if (useB) return mode_filterB[mode];
	return mode_filterA[mode];
}

void RIG_IC7200::set_FILT(int filter)
{
	if (filter < 1 || filter > 3)
		return;

	if (useB) {
		B.filter = filter;
		mode_filterB[B.imode] = filter;

		cmd = pre_to;
		cmd += '\x06';
		cmd += mdval[B.imode];
		cmd += filter;
		cmd.append( post );

		waitFB("set mode/filter B");
	set_trace(2, "set mode/filter B", str2hex(replystr.c_str(), replystr.length()));

		if (B.imode < 7) return;

		cmd = pre_to;
		cmd += '\x1A'; cmd += '\x04';
		cmd += '\x01';
		cmd += filter;
		cmd.append( post);
		waitFB("set data mode/filter B");
	set_trace(2, "set data mode/filter B", str2hex(replystr.c_str(), replystr.length()));

	} else {
		A.filter = filter;
		mode_filterA[A.imode] = filter;

		cmd = pre_to;
		cmd += '\x06';
		cmd += mdval[A.imode];
		cmd += filter;
		cmd.append( post );
		waitFB("set filter A ");
	set_trace(2, "set filter A", str2hex(replystr.c_str(), replystr.length()));

		if (A.imode < 7) return;

		cmd = pre_to;
		cmd += '\x1A'; cmd += '\x04';
		cmd += '\x01';
		cmd += mode_filterA[A.imode];
		cmd.append( post);
		waitFB("set data filter A");
	set_trace(2, "set data filter A", str2hex(replystr.c_str(), replystr.length()));
	}

}

const char *RIG_IC7200::FILT(int val)
{
	if (val < 1) val = 1;
	if (val > 3) val = 3;
	return(szfilter[val - 1]);
}

const char *RIG_IC7200::nextFILT()
{
	int val = A.filter;
	if (useB) val = B.filter;
	val++;
	if (val > 3) val = 1;
	set_FILT(val);
	return szfilter[val - 1];
}

void RIG_IC7200::set_FILTERS(std::string s)
{
	stringstream strm;
	strm << s;
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_filterA[i];
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_filterB[i];
	for (int i = 0; i < NUM_MODES; i++) {
		if (mode_filterA[i] < 1) mode_filterA[i] = 1;
		if (mode_filterA[i] > 3) mode_filterA[i] = 3;
		if (mode_filterB[i] < 1) mode_filterB[i] = 1;
		if (mode_filterB[i] > 3) mode_filterB[i] = 3;
	}
}

std::string RIG_IC7200::get_FILTERS()
{
	for (int i = 0; i < NUM_MODES; i++) {
		if (mode_filterA[i] < 1) mode_filterA[i] = 1;
		if (mode_filterA[i] > 3) mode_filterA[i] = 3;
		if (mode_filterB[i] < 1) mode_filterB[i] = 1;
		if (mode_filterB[i] > 3) mode_filterB[i] = 3;
	}
	stringstream s;
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_filterA[i] << " ";
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_filterB[i] << " ";
	return s.str();
}

std::string RIG_IC7200::get_BANDWIDTHS()
{
	stringstream s;
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_bwA[i] << " ";
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_bwB[i] << " ";
	return s.str();
}

void RIG_IC7200::set_BANDWIDTHS(std::string s)
{
	stringstream strm;
	strm << s;
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_bwA[i];
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_bwB[i];
}

int RIG_IC7200::adjust_bandwidth(int m)
{
	switch (m) {
		case 2: // AM
			bandwidths_ = IC7200_AMwidths;
			bw_vals_ = IC7200_bw_vals_AM;
			bwA = 30;
			break;
		case 3:
		case 5: // CW
			bandwidths_ = IC7200_SSBwidths;
			bw_vals_ = IC7200_bw_vals_SSB;
			bwA = 14;
			break;
		case 4:
		case 6: // RTTY
			bandwidths_ = IC7200_RTTYwidths;
			bw_vals_ = IC7200_bw_vals_RTTY;
			bwA = 28;
			break;
		case 0:
		case 1:
		case 7:
		case 8: 
		default: // SSB
			bandwidths_ = IC7200_SSBwidths;
			bw_vals_ = IC7200_bw_vals_SSB;
			bwA = 32;
	}
	return bwA;
}

int RIG_IC7200::def_bandwidth(int m)
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

const char ** RIG_IC7200::bwtable(int m)
{
	switch (m) {
		case 2: // AM
			return IC7200_AMwidths;
			break;
		case 3:
		case 5: // CW
			return IC7200_SSBwidths;
			break;
		case 4:
		case 6: // RTTY
			return IC7200_RTTYwidths;
			break;
		case 0:
		case 1:
		case 7:
		case 8: 
		default: // SSB
			return IC7200_SSBwidths;
	}
	return IC7200_SSBwidths;
}

void RIG_IC7200::set_bwA(int val)
{
	A.iBW = val;
	cmd = pre_to;
	cmd.append("\x1A\x02");
	cmd.append(to_bcd(val, 2));
	cmd.append( post );
	waitFB("set BW A");
	mode_bwA[A.imode] = val;
	set_trace(4, "set_bwA() ", bwtable(A.imode)[val], ": ", str2hex(replystr.c_str(), replystr.length()));
}

int  RIG_IC7200::get_bwA()
{
	cmd = pre_to;
	cmd += "\x1A\x02";
	cmd.append( post );
	string resp = pre_fm;
	resp += "\x1A\x02";
	int bwval = A.iBW;
	if (waitFOR(8, "get BW A")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			bwval = (fm_bcd(replystr.substr(p+6),2));
	}
	if (bwval != A.iBW) {
		A.iBW = bwval;
	}
	get_trace(2, "get_bwA()", str2hex(replystr.c_str(), replystr.length()));

	mode_bwA[A.imode] = A.iBW;

	return A.iBW;
}

void RIG_IC7200::set_bwB(int val)
{
	B.iBW = val;
	cmd = pre_to;
	cmd.append("\x1A\x02");
	cmd.append(to_bcd(val, 2));
	cmd.append( post );
	waitFB("set BW B");
	mode_bwB[B.imode] = val;
	set_trace(4, "set_bwB() ", bwtable(B.imode)[val], ": ", str2hex(replystr.c_str(), replystr.length()));
}

int  RIG_IC7200::get_bwB()
{
	cmd = pre_to;
	cmd += "\x1A\x02";
	cmd.append( post );
	string resp = pre_fm;
	resp += "\x1A\x02";
	int bwval = B.iBW;
	if (waitFOR(8, "get BW B")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			bwval = (fm_bcd(replystr.substr(p+6),2));
	}
	if (bwval != B.iBW) {
		B.iBW = bwval;
	}
	get_trace(2, "get_bwB()", str2hex(replystr.c_str(), replystr.length()));
	mode_bwB[B.imode] = B.iBW;
	return B.iBW;
}

void RIG_IC7200::set_auto_notch(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x41';
	cmd += (unsigned char)val;
	cmd.append( post );
	waitFB("set AN");
	set_trace(2, "set_auto_notch()", str2hex(cmd.c_str(), cmd.length()));
}

int RIG_IC7200::get_auto_notch()
{
	string cstr = "\x16\x41";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get AN")) {
		get_trace(2, "get_auto_notch()", str2hex(replystr.c_str(), replystr.length()));
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			if (replystr[p+6] == 0x01) {
				auto_notch_label("AN", true);
				return true;
			} else {
				auto_notch_label("AN", false);
				return false;
			}
		}
	}
	return progStatus.auto_notch;
}

static int comp_level[] = {11,34,58,81,104,128,151,174,197,221,244};
void RIG_IC7200::set_compression(int on, int val)
{
	cmd = pre_to;
	cmd.append("\x16\x44");
	if (on) cmd += '\x01';
	else cmd += '\x00';
	cmd.append(post);
	waitFB("set Comp ON/OFF");
	set_trace(2, "set_compression_on_off()", str2hex(cmd.c_str(), cmd.length()));

	if (val < 0) return;
	if (val > 10) return;

	cmd.assign(pre_to).append("\x14\x0E");
	cmd.append(to_bcd(comp_level[val], 3));
	cmd.append( post );
	waitFB("set comp");
	set_trace(2, "set_compression_level()", str2hex(cmd.c_str(), cmd.length()));
}

void RIG_IC7200::get_compression(int &on, int &val)
{
	std::string resp;

	cmd.assign(pre_to).append("\x16\x44").append(post);

	resp.assign(pre_fm).append("\x16\x44");

	if (waitFOR(8, "get comp on/off")) {
		get_trace(2, "get_comp_on_off()", str2hex(replystr.c_str(), replystr.length()));
		size_t p = replystr.find(resp);
		if (p != string::npos)
			on = (replystr[p+6] == 0x01);
	}

	cmd.assign(pre_to).append("\x14\x0E").append(post);
	resp.assign(pre_fm).append("\x14\x0E");

	if (waitFOR(9, "get comp level")) {
		get_trace(2, "get_comp_level()", str2hex(replystr.c_str(), replystr.length()));
		size_t p = replystr.find(resp);
		int level = 0;
		if (p != string::npos) {
			level = fm_bcd(replystr.substr(p+6), 3);
			for (val = 0; val < 11; val++)
				if (level <= comp_level[val]) break;
		}
	}
}

void RIG_IC7200::set_vox_onoff()
{
	if (progStatus.vox_onoff) {
		cmd = pre_to;
		cmd.append("\x16\x46");
		cmd += '\x01';
		cmd.append(post);
		waitFB("set Vox ON");
	} else {
		cmd = pre_to;
		cmd.append("\x16\x46");
		cmd += '\x00';
		cmd.append(post);
		waitFB("set Vox OFF");
	}
	set_trace(2, "set_vox_on_off()", str2hex(cmd.c_str(), cmd.length()));
}

static bool IC7200_notchon = false;

void RIG_IC7200::set_notch(bool on, int freq)
{
	int hexval;
	switch (vfo->imode) {
		default: case USB7200: case USBD7200: case RTTYR7200:
			hexval = freq - 1500;
			break;
		case LSB7200: case LSBD7200: case RTTY7200:
			hexval = 1500 - freq;
			break;
		case CW7200:
			if (CW_sense)
				hexval = freq - progStatus.cw_spot_tone;
			else
				hexval = progStatus.cw_spot_tone - freq;
			break;
		case CWR7200:
			if (CW_sense)
				hexval = progStatus.cw_spot_tone - freq;
			else
				hexval = freq - progStatus.cw_spot_tone;
			break;
	}

	hexval /= 20;
	hexval += 128;
	if (hexval < 0) hexval = 0;
	if (hexval > 255) hexval = 255;

	cmd = pre_to;
	cmd.append("\x16\x48");
	cmd += on ? '\x01' : '\x00';
	cmd.append(post);
	waitFB("set notch");

	cmd = pre_to;
	cmd.append("\x14\x0D");
	cmd.append(to_bcd(hexval,3));
	cmd.append(post);
	waitFB("set notch val");
}

bool RIG_IC7200::get_notch(int &val)
{
	bool on = false;
	val = 1500;

	string cstr = "\x16\x48";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get notch")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			on = replystr[p + 6];
		cmd = pre_to;
		resp = pre_fm;
		cstr = "\x14\x0D";
		cmd.append(cstr);
		resp.append(cstr);
		cmd.append(post);
		if (waitFOR(9, "notch val")) {
			size_t p = replystr.rfind(resp);
			if (p != string::npos) {
				val = (int)ceil(fm_bcd(replystr.substr(p+6),3));
				val -= 128;
				val *= 20;
				switch (vfo->imode) {
					default: case USB7200: case USBD7200: case RTTYR7200:
						val = 1500 + val;
						break;
					case LSB: case LSBD7200: case RTTY7200:
						val = 1500 - val;
						break;
					case CW7200:
						if (CW_sense)
							val = progStatus.cw_spot_tone + val;
						else
							val = progStatus.cw_spot_tone - val;
						break;
					case CWR7200:
						if (CW_sense)
							val = progStatus.cw_spot_tone - val;
						else
							val = progStatus.cw_spot_tone + val;
						break;
				}
			}
		}
	}
	return on;
}

void RIG_IC7200::get_notch_min_max_step(int &min, int &max, int &step)
{
	switch (vfo->imode) {
		default:
		case USB7200: case USBD7200: case RTTYR7200:
		case LSB7200: case LSBD7200: case RTTY7200:
			min = 0; max = 3000; step = 20; break;
		case CW7200: case CWR7200:
			min = progStatus.cw_spot_tone - 500;
			max = progStatus.cw_spot_tone + 500;
			step = 20;
			break;
	}
}

static int agcval = 0;
int  RIG_IC7200::get_agc()
{
	cmd = pre_to;
	cmd.append("\x16\x12");
	cmd.append(post);
	if (waitFOR(8, "get AGC")) {
		size_t p = replystr.find(pre_fm);
		if (p == string::npos) return agcval;
		return (agcval = replystr[p+6]); // 1 = off, 2 = FAST, 3 = SLOW
	}
	get_trace(2, "get_agc()", str2hex(replystr.c_str(), replystr.length()));
	return agcval;
}

int RIG_IC7200::incr_agc()
{
	agcval++;
	if (agcval == 3) agcval = 0;
	cmd = pre_to;
	cmd.append("\x16\x12");
	cmd += agcval;
	cmd.append(post);
	waitFB("set AGC");
	return agcval;
}


static const char *agcstrs[] = {"AGC", "FST", "SLO"};
const char *RIG_IC7200::agc_label()
{
	return agcstrs[agcval];
}

int  RIG_IC7200::agc_val()
{
	return (agcval);
}

void RIG_IC7200::set_if_shift(int val)
{
	int shift;
	sh_ = val;
	if (val == 0) sh_on_ = false;
	else sh_on_ = true;

	shift = 128 + val * 128 / 50;
	if (shift < 0) shift = 0;
	if (shift > 255) shift = 255;

	cmd = pre_to;
	cmd.append("\x14\x07");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set IF on/off");
	set_trace(2, "set_if_shift_on_off()", str2hex(cmd.c_str(), cmd.length()));

	cmd = pre_to;
	cmd.append("\x14\x08");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set IF val");
	set_trace(2, "set_if_shift_val()", str2hex(cmd.c_str(), cmd.length()));
}

bool RIG_IC7200::get_if_shift(int &val) {
	val = sh_;
	return sh_on_;
}

void RIG_IC7200::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -50;
	max = +50;
	step = 1;
}

void RIG_IC7200::set_pbt_inner(int val)
{
	int shift = 128 + val * 128 / 50;
	if (shift < 0) shift = 0;
	if (shift > 255) shift = 255;

	cmd = pre_to;
	cmd.append("\x14\x07");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set PBT inner");
	set_trace(2, "set_PBT_inner()", str2hex(cmd.c_str(), cmd.length()));
}

void RIG_IC7200::set_pbt_outer(int val)
{
	int shift = 128 + val * 128 / 50;
	if (shift < 0) shift = 0;
	if (shift > 255) shift = 255;

	cmd = pre_to;
	cmd.append("\x14\x08");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set PBT outer");
	set_trace(2, "set_PBT_outer()", str2hex(cmd.c_str(), cmd.length()));
}

int RIG_IC7200::get_pbt_inner()
{
	int val = 0;
	string cstr = "\x14\x07";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(9, "get pbt inner")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			val = num100(replystr.substr(p+6));
			val -= 50;
		}
	}
	get_trace(2, "get_pbt_inner()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

int RIG_IC7200::get_pbt_outer()
{
	int val = 0;
	string cstr = "\x14\x08";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(9, "get pbt inner")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			val = num100(replystr.substr(p+6));
			val -= 50;
		}
	}
	get_trace(2, "get_pbt_outer()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

// CW methods

void RIG_IC7200::get_cw_wpm_min_max(int &min, int &max)
{
	min = 6; max = 48;
}

void RIG_IC7200::set_cw_wpm()
{
	int iwpm = round((progStatus.cw_wpm - 6) * 255 / 42 + 0.5);
	minmax(0, 255, iwpm);

	cmd.assign(pre_to).append("\x14\x0C");
	cmd.append(to_bcd(iwpm, 3));
	cmd.append( post );
	waitFB("SET cw wpm");
	set_trace(2, "set_cw_wpm()", str2hex(cmd.c_str(), cmd.length()));
}

void RIG_IC7200::enable_break_in()
{
// 16 47 00 break-in off
// 16 47 01 break-in semi
// 16 47 02 break-in full

	cmd.assign(pre_to).append("\x16\x47");

	switch (progStatus.break_in) {
		case 2: cmd += '\x02'; break_in_label("FULL"); break;
		case 1: cmd += '\x01'; break_in_label("SEMI");  break;
		case 0:
		default: cmd += '\x00'; break_in_label("BK-IN");
	}
	cmd.append(post);
	waitFB("SET break-in");
	set_trace(2, "set_break_in()", str2hex(cmd.c_str(), cmd.length()));
}

void RIG_IC7200::get_cw_qsk_min_max_step(double &min, double &max, double &step)
{
	min = 2.0; max = 13.0; step = 0.1;
}

void RIG_IC7200::set_cw_qsk()
{
	int qsk = round ((progStatus.cw_qsk - 2.0) * 255.0 / 11.0 + 0.5);
	minmax(0, 255, qsk);

	cmd.assign(pre_to).append("\x14\x0F");
	cmd.append(to_bcd(qsk, 3));
	cmd.append(post);
	waitFB("Set cw qsk delay");
	set_trace(2, "set_cw_qsk()", str2hex(cmd.c_str(), cmd.length()));
}

void RIG_IC7200::get_cw_spot_tone_min_max_step(int &min, int &max, int &step)
{
	min = 300; max = 900; step = 5;
}

void RIG_IC7200::set_cw_spot_tone()
{
	cmd.assign(pre_to).append("\x14\x09"); // values 0=300Hz 255=900Hz
	int n = round((progStatus.cw_spot_tone - 300) * 255.0 / 600.0 + 0.5);
	minmax(0, 255, n);

	cmd.append(to_bcd(n, 3));
	cmd.append( post );
	waitFB("SET cw spot tone");
	set_trace(2, "set_cw_spot_tone()", str2hex(cmd.c_str(), cmd.length()));
}

void RIG_IC7200::set_cw_vol()
{
	cmd.assign(pre_to);
	cmd.append("\x1A\x03\x06");
	cmd.append(to_bcd((int)(progStatus.cw_vol * 2.55), 3));
	cmd.append( post );
	waitFB("SET cw sidetone volume");
	set_trace(2, "set_cw_vol()", str2hex(cmd.c_str(), cmd.length()));
}

void RIG_IC7200::setVfoAdj(double v)
{
	vfo_ = v;
	cmd.assign(pre_to);
	cmd.append("\x1A\x03\x49");
	cmd.append(bcd255(int(v)));
	cmd.append(post);
	waitFB("SET vfo adjust");
	set_trace(2, "set_vfo_adj()", str2hex(cmd.c_str(), cmd.length()));
}

double RIG_IC7200::getVfoAdj()
{
	cmd.assign(pre_to);
	cmd.append("\x1A\x03\x49");
	cmd.append(post);

	if (waitFOR(10, "get vfo adj")) {
		size_t p = replystr.find(pre_fm);
		if (p != string::npos) {
			vfo_ = num100(replystr.substr(p+7));
		}
	}
	get_trace(2, "get_vfo_adj()", str2hex(replystr.c_str(), replystr.length()));
	return vfo_;
}

void RIG_IC7200::get_band_selection(int v)
{
	cmd.assign(pre_to);
	cmd.append("\x1A\x01");
	cmd += to_bcd_be( v, 2 );
	cmd += '\x01';
	cmd.append( post );

	if (waitFOR(17, "get band")) {
		std::string ans = replystr;
		size_t p = ans.rfind(pre_fm);
		if (p != string::npos) {
			long int bandfreq = fm_bcd_be(ans.substr(p+8, 5), 10);
			int bandmode = ans[p+13];
			int bandfilter = ans[p+14];
			int banddata = ans[p+15] & 0x10;
			if ((bandmode == 0 || bandmode == 1) && banddata) bandmode += 7;
			if (useB) {
				set_vfoB(bandfreq);
				set_modeB(bandmode);
				set_FILT(bandfilter);
			} else {
				set_vfoA(bandfreq);
				set_modeA(bandmode);
				set_FILT(bandfilter);
			}
			get_trace(2, "get_band", str2hex(ans.substr(p).c_str(), ans.substr(p).length()));
		}
	}
}

void RIG_IC7200::set_band_selection(int v)
{
	long freq = (useB ? B.freq : A.freq);
	int mode = (useB ? B.imode : A.imode);
	int fil = (useB ? B.filter : A.filter);

	cmd.assign(pre_to);
	cmd.append("\x1A\x01");
	cmd += to_bcd_be( v, 2 );
	cmd += '\x01';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd += mdval[mode];
	cmd += fil;
	if (mode >= 7)
		cmd += '\x10';
	else
		cmd += '\x00';
	cmd.append(post);
	waitFB("set band");
	set_trace(2, "SET", str2hex(cmd.c_str(), cmd.length()));

	cmd.assign(pre_to);
	cmd.append("\x1A\x01");
	cmd += to_bcd_be( v, 2 );
	cmd += '\x01';
	cmd.append( post );

	waitFOR(17, "get band stack");
	get_trace(2, "get_band_stack", str2hex(replystr.c_str(), replystr.length()));
}

