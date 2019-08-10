// ---------------------------------------------------------------------
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
// ---------------------------------------------------------------------

#include <string>
#include <sstream>

#include "debug.h"
#include "support.h"

//======================================================================
static inline void minmax(int min, int max, int &val)
{
	if (val > max) val = max;
	if (val < min) val = min;
}
//======================================================================
// 756
//======================================================================
#include "IC756.h"

const char IC756name_[] = "IC-756";

const char *IC756modes_[] = {
		"LSB", "USB", "AM", "CW", "RTTY", "FM", "CW-R", "RTTY-R", NULL};
// mode values are 0, 1, 2, 3, 4, 5, 7, 8
const char IC756_mode_type[] =
	{ 'L', 'U', 'U', 'U', 'L', 'U', 'L', 'U'};

const char *IC756_widths[] = { "NORM", "NARR", NULL};
static int IC756_bw_vals[] = {1, 2, WVALS_LIMIT};

static GUI IC756_widgets[]= {
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

void RIG_IC756::initialize()
{
	IC756_widgets[0].W = btnVol;
	IC756_widgets[1].W = sldrVOLUME;
	IC756_widgets[2].W = btnAGC;
	IC756_widgets[3].W = sldrRFGAIN;
	IC756_widgets[4].W = sldrSQUELCH;
	IC756_widgets[5].W = btnNR;
	IC756_widgets[6].W = sldrNR;
	IC756_widgets[7].W = btnLOCK;
	IC756_widgets[8].W = sldrINNER;
	IC756_widgets[9].W = btnCLRPBT;
	IC756_widgets[10].W = sldrOUTER;
	IC756_widgets[11].W = btnNotch;
	IC756_widgets[12].W = sldrNOTCH;
	IC756_widgets[13].W = sldrMICGAIN;
	IC756_widgets[14].W = sldrPOWER;
}

RIG_IC756::RIG_IC756() {
	defaultCIV = 0x50;
	name_ = IC756name_;
	modes_ = IC756modes_;
	bandwidths_ = IC756_widths;
	bw_vals_ = IC756_bw_vals;
	_mode_type = IC756_mode_type;

	widgets = IC756_widgets;

	comm_baudrate = BR19200;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_echo = true;
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = false;
	comm_rtsptt = true;
	comm_dtrptt = false;

	def_freq = freqB = freqA = B.freq = A.freq = 14070000L;
	def_mode = B.imode = A.imode = 1;
	def_bw = B.iBW = A.iBW = 0;
	filter_nbr = 0;

	ICvol = 0;

	has_bandwidth_control =
	has_smeter =
	has_power_control =
	has_volume_control =
	has_mode_control =
	has_micgain_control =
	has_auto_notch =
	has_notch_control =
	has_attenuator_control =
	has_preamp_control =
	has_pbt_controls =
	has_ptt_control =
	has_noise_control =
	has_noise_reduction =
	has_noise_reduction_control =
	has_rf_control =
	has_sql_control =
	has_split =
	restore_mbw = true;

	precision = 1;
	ndigits = 9;

	ICOMmainsub = true;

	adjustCIV(defaultCIV);
};

void RIG_IC756::set_pbt_inner(int val)
{
	int shift = 128 + val * 128 / 50;
	if (shift < 0) shift = 0;
	if (shift > 255) shift = 255;

	cmd = pre_to;
	cmd.append("\x14\x07");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set PBT inner");
}

void RIG_IC756::set_pbt_outer(int val)
{
	int shift = 128 + val * 128 / 50;
	if (shift < 0) shift = 0;
	if (shift > 255) shift = 255;

	cmd = pre_to;
	cmd.append("\x14\x08");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set PBT outer");
}

int RIG_IC756::get_pbt_inner()
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
	rig_trace(2, "get_pbt_inner()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

int RIG_IC756::get_pbt_outer()
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
	rig_trace(2, "get_pbt_outer()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

//=============================================================================
// 756PRO
//=============================================================================
#include "IC756.h"

const char IC756PROname_[] = "IC-756PRO";

//=============================================================================
enum { PRO_LSB, PRO_USB, PRO_AM, PRO_CW, PRO_RTTY, PRO_FM_, PRO_CWR, PRO_RTTYR };

#define NUM_FILTERS 3
#define NUM_MODES  9

static int mode_filterA[NUM_MODES] = {1,1,1,1,1,1,1,1,1};
static int mode_filterB[NUM_MODES] = {1,1,1,1,1,1,1,1,1};

static const char *szfilter[NUM_FILTERS] = {"1", "2", "3"};

const char *IC756PROmodes_[] = {
	"LSB", "USB", "AM", "CW", "RTTY", "FM", "CW-R", "RTTY-R", NULL};

const char IC756PRO_mode_type[] =
	{ 'L', 'U', 'U', 'U', 'L', 'U', 'L', 'U' };

const char *IC756PRO_bw_vals[] = { "FIXED", NULL};

// IC756_widgets[] declared in RIG_IC756

static GUI IC756PRO_widgets[]= {
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

RIG_IC756PRO::RIG_IC756PRO() {
	defaultCIV = 0x5C;
	name_ = IC756PROname_;
	modes_ = IC756PROmodes_;
	bandwidths_ = IC756PRO_bw_vals;
	_mode_type = IC756PRO_mode_type;

	widgets = IC756PRO_widgets;

	def_freq = freqA = freqB = A.freq = 14070000;
	def_mode = modeA = modeB = B.imode = 1;
	def_bw = A.iBW = B.iBW = 0;
	atten_level = 3;
	preamp_level = 2;
	adjustCIV(defaultCIV);

	has_extras = true;
	has_bandwidth_control = true;
	has_pbt_controls = true;
	has_tune_control = true;
	has_swr_control = true;
	has_alc_control =  true;
	has_smeter = true;
	has_power_control = true;
	has_volume_control = true;
	has_mode_control = true;
	has_micgain_control = true;
	has_auto_notch = true;
	has_notch_control = true;
	has_attenuator_control = true;
	has_preamp_control = true;
	has_ptt_control = true;
	has_noise_reduction = true;
	has_noise_reduction_control = true;
	has_noise_control = true;
	has_rf_control = true;
	has_a2b = true;
	has_FILTER = true;

	has_compON = true;
	has_compression = true;
	has_vox_onoff = true;

	has_cw_wpm = true;
	has_cw_spot_tone = true;
	has_cw_qsk = true;
	has_cw_break_in = true;

	ICOMmainsub = true;

	has_band_selection = true;

	precision = 1;
	ndigits = 9;

};

void RIG_IC756PRO::initialize()
{
	IC756PRO_widgets[0].W = btnVol;
	IC756PRO_widgets[1].W = sldrVOLUME;
	IC756PRO_widgets[2].W = btnAGC;
	IC756PRO_widgets[3].W = sldrRFGAIN;
	IC756PRO_widgets[4].W = sldrSQUELCH;
	IC756PRO_widgets[5].W = btnNR;
	IC756PRO_widgets[6].W = sldrNR;
	IC756PRO_widgets[7].W = btnLOCK;
	IC756PRO_widgets[8].W = sldrINNER;
	IC756PRO_widgets[9].W = btnCLRPBT;
	IC756PRO_widgets[10].W = sldrOUTER;
	IC756PRO_widgets[11].W = btnNotch;
	IC756PRO_widgets[12].W = sldrNOTCH;
	IC756PRO_widgets[13].W = sldrMICGAIN;
	IC756PRO_widgets[14].W = sldrPOWER;

	btn_icom_select_11->deactivate();
	btn_icom_select_12->deactivate();
	btn_icom_select_13->deactivate();

	choice_rTONE->activate();
	choice_tTONE->activate();
}

void RIG_IC756PRO::swapAB()
{
	cmd = pre_to;
	cmd += 0x07; cmd += 0xB0;
	cmd.append(post);
	waitFB("Exchange main/sub");

	selectA();
	get_modeA();
	get_bwA();
	selectB();
	get_modeB();
	get_bwB();
	if (!useB) selectA();
}

void RIG_IC756PRO::A2B()
{
	cmd = pre_to;
	cmd += 0x07; cmd += 0xB1;
	cmd.append(post);
	waitFB("Equalize main/sub");

	selectB();
	get_modeB();
	get_bwB();
	if (!useB) selectA();
}

void RIG_IC756PRO::selectA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\xD0';
	cmd.append(post);
	waitFB("sel A");
}

void RIG_IC756PRO::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\xD1';
	cmd.append(post);
	waitFB("sel B");
}

bool RIG_IC756PRO::check ()
{
	string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	bool ok = waitFOR(11, "check vfo");
	rig_trace(2, "check()", str2hex(replystr.c_str(), replystr.length()));
	return ok;
}

long RIG_IC756PRO::get_vfoA ()
{
	if (useB) return A.freq;
	string cstr = "\x03";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(11, "get vfo A")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			A.freq = fm_bcd_be(replystr.substr(p+5), 10);
	}
	return A.freq;
}

void RIG_IC756PRO::set_vfoA (long freq)
{
	A.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	waitFB("set vfo A");
}

long RIG_IC756PRO::get_vfoB ()
{
	if (!useB) return B.freq;
	string cstr = "\x03";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(11, "get vfo B")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			B.freq = fm_bcd_be(replystr.substr(p+5), 10);
	}
	return B.freq;
}

void RIG_IC756PRO::set_vfoB (long freq)
{
	B.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	waitFB("set vfo B");
}

int RIG_IC756PRO::get_smeter()
{
	string cstr = "\x15\x02";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get smeter")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (int)ceil(fm_bcd(replystr.substr(p+6), 3) / 2.55);
	}
	return 0;
}

// Volume control val 0 ... 100

void RIG_IC756PRO::set_volume_control(int val)
{
	ICvol = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append(to_bcd(ICvol, 3));
	cmd.append( post );
	waitFB("set vol");
}

int RIG_IC756PRO::get_volume_control()
{
	string cstr = "\x14\x01";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get vol")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return ((int)ceil(fm_bcd(replystr.substr(p+6),3) * 100 / 255));
	}
	return progStatus.volume;
}

void RIG_IC756PRO::get_vol_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

// Tranceiver PTT on/off
void RIG_IC756PRO::set_PTT_control(int val)
{
	if (val) trace(1, "set_PTT_control(1)");
	else trace(1, "set_PTT_control(0)");
	cmd = pre_to;
	cmd += '\x1c';
	cmd += '\x00';
	cmd += (unsigned char) val;
	cmd.append( post );
	waitFB("set ptt");
	ptt_ = val;
}

int RIG_IC756PRO::get_PTT()
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
	return ptt_;
}

// changed noise blanker to noise reduction
void RIG_IC756PRO::set_noise(bool val)
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set noise");
}

int RIG_IC756PRO::get_noise()
{
	string cstr = "\x16\x22";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(8, "get noise")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (replystr[p+6] ? 1 : 0);
	}
	return 0;
}

void RIG_IC756PRO::set_noise_reduction(int val)
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set NR");
}

int RIG_IC756PRO::get_noise_reduction()
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
	return progStatus.noise_reduction;
}

// 0 < val < 15
void RIG_IC756PRO::set_noise_reduction_val(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x06");
	if (val == 16) val = 255;
	else val = val * 16;
	cmd.append(to_bcd(val, 3));
	cmd.append(post);
	waitFB("set NRval");
}

int RIG_IC756PRO::get_noise_reduction_val()
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
			if (val == 255) val = 16;
			else val = val/ 16;
		}
	}
	return val;
}

int RIG_IC756PRO::get_modetype(int n)
{
	return _mode_type[n];
}

void RIG_IC756PRO::set_mic_gain(int val)
{
	val = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x0B");
	cmd.append(to_bcd(val,3));
	cmd.append(post);
	waitFB("set mic");
}

void RIG_IC756PRO::get_mic_gain_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_IC756PRO::set_if_shift(int val)
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

	cmd = pre_to;
	cmd.append("\x14\x08");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set IF val");
}

void RIG_IC756PRO::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -50;
	max = +50;
	step = 1;
}

void RIG_IC756PRO::set_pbt_inner(int val)
{
	int shift = 128 + val * 128 / 50;
	if (shift < 0) shift = 0;
	if (shift > 255) shift = 255;

	cmd = pre_to;
	cmd.append("\x14\x07");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set PBT inner");
}

void RIG_IC756PRO::set_pbt_outer(int val)
{
	int shift = 128 + val * 128 / 50;
	if (shift < 0) shift = 0;
	if (shift > 255) shift = 255;

	cmd = pre_to;
	cmd.append("\x14\x08");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set PBT outer");
}

int RIG_IC756PRO::get_pbt_inner()
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
	rig_trace(2, "get_pbt_inner()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

int RIG_IC756PRO::get_pbt_outer()
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
	rig_trace(2, "get_pbt_outer()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

int IC756PROsql = 0;
void RIG_IC756PRO::set_squelch(int val)
{
	IC756PROsql = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x03");
	cmd.append(to_bcd(IC756PROsql, 3));
	cmd.append( post );
	waitFB("set sql");
}

void RIG_IC756PRO::set_power_control(double val)
{
	cmd = pre_to;
	cmd.append("\x14\x0A");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set power");
}

int RIG_IC756PRO::get_power_control()
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
	return val;
}

void RIG_IC756PRO::set_rf_gain(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x02");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set RF");
}

int RIG_IC756PRO::get_rf_gain()
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
	return val;
}

void RIG_IC756PRO::set_split(bool val)
{
	split = val;
	cmd = pre_to;
	cmd += 0x0F;
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	waitFB("set split");
}

int  RIG_IC756PRO::get_split()
{
	return split;
}

void RIG_IC756PRO::set_modeA(int val)
{
	A.imode = val;
	switch (val) {
		case 7  : val = 8; break;
		case 6  : val = 7; break;
		default: break;
	}
	cmd = pre_to;
	cmd += '\x06';
	cmd += val;
	cmd += mode_filterA[A.imode];
	cmd.append( post );
}

int RIG_IC756PRO::get_modeA()
{
	int md = A.imode;
	int mf = mode_filterA[A.imode];

	string cstr = "\x04";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(8, "get mode A")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			md = replystr[p+5];
			if (md > 6) md--;
			A.imode = md;
			mf = replystr[p+6];
		}
	}
	if (mf > 0 && mf < 4)
		mode_filterA[A.imode] = A.filter = mf;
	return A.imode;
}

void RIG_IC756PRO::set_modeB(int val)
{
	B.imode = val;
	switch (val) {
		case 7  : val = 8; break;
		case 6  : val = 7; break;
		default: break;
	}
	cmd = pre_to;
	cmd += '\x06';
	cmd += val;
	cmd += mode_filterB[B.imode];
	cmd.append( post );
}

int RIG_IC756PRO::get_modeB()
{
	int md = B.imode;
	int mf = mode_filterB[B.imode];

	string cstr = "\x04";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(8, "get mode B")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			md = replystr[p+5];
			if (md > 6) md--;
			B.imode = md;
			mf = replystr[p+6];
		}
	}
	if (mf > 0 && mf < 3)
		mode_filterB[B.imode] = B.filter = mf;

	return B.imode;
}

int RIG_IC756PRO::adjust_bandwidth(int m)
{
	if (useB) return B.iBW;
	return A.iBW;
}

int RIG_IC756PRO::def_bandwidth(int m)
{
	if (useB) return B.iBW;
	return A.iBW;
}

const char **RIG_IC756PRO::bwtable(int m)
{
	return IC756PRO_bw_vals;
}

void RIG_IC756PRO::set_bwA(int val)
{
	A.iBW = val;
	int md = A.imode;
	switch (md) {
		case 7  : md = 8; break;
		case 6  : md = 7; break;
		default: break;
	}
	cmd = pre_to;
	cmd += '\x06';
	cmd += md;
	cmd += A.iBW + 1;
	cmd.append( post );
	waitFB("set mode/filter A");
}

int RIG_IC756PRO::get_bwA()
{
	return A.iBW;
}

void RIG_IC756PRO::set_bwB(int val)
{
	B.iBW = val;
	int md = B.imode;
	switch (md) {
		case 7  : md = 8; break;
		case 6  : md = 7; break;
		default: break;
	}
	cmd = pre_to;
	cmd += '\x06';
	cmd += md;
	cmd += B.iBW + 1;
	cmd.append( post );
	waitFB("set mode/filter B");
}

int RIG_IC756PRO::get_bwB()
{
	return B.iBW;
}

bool IC756PRO_notchon = false;

void RIG_IC756PRO::set_notch(bool on, int val)
{
	int notch = val / 20 + 53;
	if (notch > 255) notch = 255;
	if (on != IC756PRO_notchon) {
		cmd = pre_to;
		cmd.append("\x16\x48");
		cmd += on ? '\x01' : '\x00';
		cmd.append(post);
		waitFB("set notch");
		IC756PRO_notchon = on;
	}

	if (on) {
		cmd = pre_to;
		cmd.append("\x14\x0D");
		cmd.append(to_bcd(notch,3));
		cmd.append(post);
		waitFB("set notch val");
	}
}

bool RIG_IC756PRO::get_notch(int &val)
{
	bool on = false;
	val = 0;

	string cstr = "\x16\x48";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get notch")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			on = replystr[p + 6] ? 1 : 0;
		cmd = pre_to;
		resp = pre_fm;
		cstr = "\x14\x0D";
		cmd.append(cstr);
		resp.append(cstr);
		cmd.append(post);
		if (waitFOR(9, "get notch val")) {
			size_t p = replystr.rfind(resp);
			if (p != string::npos) {
				val = fm_bcd(replystr.substr(p+6),3);
				val = (val - 53) * 20;
				if (val < 0) val = 0;
				if (val > 4040) val = 4040;
			}
		}
	}
	return on;
}

void RIG_IC756PRO::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = 20;
	max = 4040;
	step = 20;
}

void RIG_IC756PRO::set_auto_notch(int v)
{
	progStatus.auto_notch = v;
	cmd = pre_to;
	cmd.append("\x16\x48");
	cmd += v ? '\x01' : '\x00';
	cmd.append(post);
	waitFB("set auto notch");
}

int  RIG_IC756PRO::get_auto_notch()
{
	return progStatus.auto_notch;
}

int  RIG_IC756PRO::next_attenuator()
{
	switch(atten_level) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 3;
		case 3: return 0;
	}
	return 0;
}

void RIG_IC756PRO::set_attenuator(int val)
{
	atten_level = val;
	int cmdval = 0;
	if (atten_level == 1) {
		atten_label("6 dB", true);
		cmdval = 0x06;
	} else if (atten_level == 2) {
		atten_label("12 dB", true);
		cmdval = 0x12;
	} else if (atten_level == 3) {
		atten_label("18 dB", true);
		cmdval = 0x18;
	} else if (atten_level == 0) {
		atten_label("Att", false);
		cmdval = 0x00;
	}
	cmd = pre_to;
	cmd += '\x11';
	cmd += cmdval;
	cmd.append( post );
	waitFB("set att");
}

int RIG_IC756PRO::get_attenuator()
{
	string cstr = "\x11";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(7, "get att")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			if (replystr[p+5] == 0x06) {
				atten_level = 1;
				atten_label("6 dB", true);
			} else if (replystr[p+5] == 0x12) {
				atten_level = 2;
				atten_label("12 dB", true);
			} else if (replystr[p+5] == 0x18) {
				atten_level = 3;
				atten_label("18 dB", true);
			} else if (replystr[p+5] == 0x00) {
				atten_level = 0;
				atten_label("Att", false);
			}
		}
	}
	return atten_level;
}

int  RIG_IC756PRO::next_preamp()
{
	switch (preamp_level) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 0;
	}
	return 0;
}

void RIG_IC756PRO::set_preamp(int val)
{
	preamp_level = val;
	if (preamp_level == 1)
		preamp_label("Pre 1", true);
	else if (preamp_level == 2)
		preamp_label("Pre 2", true);
	else if (preamp_level == 0)
		preamp_label("Pre", false);

	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd += (unsigned char) preamp_level;
	cmd.append( post );
	waitFB("set preamp");
}

int RIG_IC756PRO::get_preamp()
{
	string cstr = "\x16\x02";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get preamp")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			if (replystr[p+6] == 0x01) {
				preamp_label("Pre 1", true);
				preamp_level = 1;
			} else if (replystr[p+6] == 0x02) {
				preamp_label("Pre 2", true);
				preamp_level = 2;
			} else {
				preamp_label("Pre", false);
				preamp_level = 0;
			}
		}
	}
	return preamp_level;
}

const char *RIG_IC756PRO::FILT(int val)
{
	if (val < 1) val = 1;
	if (val > 3) val = 3;
	return(szfilter[val - 1]);
}

int RIG_IC756PRO::get_FILT(int mode)
{
	if (useB) return mode_filterB[mode];
	return mode_filterA[mode];
}

void RIG_IC756PRO::set_FILT(int filter)
{
	if (filter < 1 || filter > 3)
		return;

	if (useB) {
		B.filter = filter;
		int val = B.imode;
		mode_filterB[val - 1] = filter;
		switch (val) {
			case 7  : val = 8; break;
			case 6  : val = 7; break;
			default: break;
		}
		cmd = pre_to;
		cmd += '\x06';
		cmd += val;
		cmd += filter;
		cmd.append( post );
		waitFB("set mode/filter B");

	} else {
		A.filter = filter;
		int val = A.imode;
		mode_filterA[val - 1] = filter;
		switch (val) {
			case 7  : val = 8; break;
			case 6  : val = 7; break;
			default: break;
		}
		cmd = pre_to;
		cmd += '\x06';
		cmd += val;
		cmd += filter;
		cmd.append( post );
		waitFB("set filter A");
	}
}

const char *RIG_IC756PRO::nextFILT()
{
	if (useB) {
		B.filter++;
		if (B.filter > 3) B.filter = 1;

		set_FILT(B.filter);

		return(szfilter[B.filter - 1]);
	} else {
		A.filter++;
		if (A.filter > 3) A.filter = 1;

		set_FILT(A.filter);

		return(szfilter[A.filter - 1]);
	}
}

void RIG_IC756PRO::set_FILTERS(std::string s)
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

std::string RIG_IC756PRO::get_FILTERS()
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

// CW methods

void RIG_IC756PRO::get_cw_wpm_min_max(int &min, int &max)
{
	min = 6; max = 48;
}

void RIG_IC756PRO::set_cw_wpm()
{
	int iwpm = round((progStatus.cw_wpm - 6) * 255 / 42 + 0.5);
	minmax(0, 255, iwpm);

	cmd.assign(pre_to).append("\x14\x0C");
	cmd.append(to_bcd(iwpm, 3));
	cmd.append( post );
	waitFB("SET cw wpm");
}

void RIG_IC756PRO::enable_break_in()
{
	cmd.assign(pre_to).append("\x16\x47");

	switch (progStatus.break_in) {
		case 1: cmd += '\x01'; break_in_label("SEMI");  break;
		case 0:
		default: cmd += '\x00'; break_in_label("BK-IN");
	}
	cmd.append(post);
	waitFB("SET break-in");
}

void RIG_IC756PRO::get_cw_qsk_min_max_step(double &min, double &max, double &step)
{
	min = 2.0; max = 13.0; step = 0.1;
}

void RIG_IC756PRO::set_cw_qsk()
{
	int qsk = round ((progStatus.cw_qsk - 2.0) * 255.0 / 11.0 + 0.5);
	minmax(0, 255, qsk);

	cmd.assign(pre_to).append("\x14\x0F");
	cmd.append(to_bcd(qsk, 3));
	cmd.append(post);
	waitFB("Set cw qsk delay");
}

void RIG_IC756PRO::get_cw_spot_tone_min_max_step(int &min, int &max, int &step)
{
	min = 300; max = 900; step = 5;
}

void RIG_IC756PRO::set_cw_spot_tone()
{
	cmd.assign(pre_to).append("\x14\x09"); // values 0=300Hz 255=900Hz
	int n = round((progStatus.cw_spot_tone - 300) * 255.0 / 600.0 + 0.5);
	minmax(0, 255, n);

	cmd.append(to_bcd(n, 3));
	cmd.append( post );
	waitFB("SET cw spot tone");
}

static int comp_level[] = {11,34,58,81,104,128,151,174,197,221,244};
void RIG_IC756PRO::set_compression(int on, int val)
{
	cmd = pre_to;
	cmd.append("\x16\x44");
	if (on) cmd += '\x01';
	else cmd += '\x00';
	cmd.append(post);
	waitFB("set Comp ON/OFF");

	if (val < 0) return;
	if (val > 10) return;

	cmd.assign(pre_to).append("\x14\x0E");
	cmd.append(to_bcd(comp_level[val], 3));
	cmd.append( post );
	waitFB("set comp");
}

void RIG_IC756PRO::get_compression(int &on, int &val)
{
	std::string resp;

	cmd.assign(pre_to).append("\x16\x44").append(post);

	resp.assign(pre_fm).append("\x16\x44");

	if (waitFOR(8, "get comp on/off")) {
		size_t p = replystr.find(resp);
		if (p != string::npos)
			on = (replystr[p+6] == 0x01);
	}

	cmd.assign(pre_to).append("\x14\x0E").append(post);
	resp.assign(pre_fm).append("\x14\x0E");

	if (waitFOR(9, "get comp level")) {
		size_t p = replystr.find(resp);
		int level = 0;
		if (p != string::npos) {
			level = fm_bcd(replystr.substr(p+6), 3);
			for (val = 0; val < 11; val++)
				if (level <= comp_level[val]) break;
		}
	}
}

void RIG_IC756PRO::set_vox_onoff()
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
}

// Read/Write band stack registers
//
// Read 23 bytes
//
//  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22
// FE FE nn E0 1A 01 bd rn f5 f4 f3 f2 f1 mo fi fg t1 t2 t3 r1 r2 r3 FD
// Write 23 bytes
//
// FE FE E0 nn 1A 01 bd rn f5 f4 f3 f2 f1 mo fi fg t1 t2 t3 r1 r2 r3 FD
//
// nn - CI-V address
// bd - band selection 1/2/3
// rn - register number 1/2/3
// f5..f1 - frequency BCD reverse
// mo - mode
// fi - filter #
// fg flags: x01 use Tx tone, x02 use Rx tone, x10 data mode
// t1..t3 - tx tone BCD fwd
// r1..r3 - rx tone BCD fwd
//
// FE FE E0 94 1A 01 06 01 70 99 08 18 00 01 03 10 00 08 85 00 08 85 FD
//
// band 6; freq 0018,089,970; USB; data mode; t 88.5; r 88.5

void RIG_IC756PRO::get_band_selection(int v)
{
	cmd.assign(pre_to);
	cmd.append("\x1A\x01");
	cmd += to_bcd_be( v, 2 );
	cmd += '\x01';
	cmd.append( post );

	if (waitFOR(23, "get band stack")) {
		set_trace(2, "get band stack", str2hex(replystr.c_str(), replystr.length()));
		size_t p = replystr.rfind(pre_fm);
		if (p != string::npos) {
			long int bandfreq = fm_bcd_be(replystr.substr(p+8, 5), 10);
			int bandmode = replystr[p+13];
			int bandfilter = replystr[p+14];
			int tone = fm_bcd(replystr.substr(p+16, 3), 6);
			size_t index = 0;
			for (index = 0; index < sizeof(PL_tones) / sizeof(*PL_tones); index++)
				if (tone == PL_tones[index]) break;
			tTONE = index;
			tone = fm_bcd(replystr.substr(p+19, 3), 6);
			for (index = 0; index < sizeof(PL_tones) / sizeof(*PL_tones); index++)
				if (tone == PL_tones[index]) break;
			rTONE = index;
			if (useB) {
				set_vfoB(bandfreq);
				set_modeB(bandmode);
				set_FILT(bandfilter);
			} else {
				set_vfoA(bandfreq);
				set_modeA(bandmode);
				set_FILT(bandfilter);
			}
		}
	} else
		set_trace(2, "get band stack", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC756PRO::set_band_selection(int v)
{
	long freq = (useB ? B.freq : A.freq);
	int fil = (useB ? filB : filA);
	int mode = (useB ? B.imode : A.imode);

	cmd.assign(pre_to);
	cmd.append("\x1A\x01");
	cmd += to_bcd_be( v, 2 );
	cmd += '\x01';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd += mode;
	cmd += fil;
	cmd += '\x00';
	cmd.append(to_bcd(PL_tones[tTONE], 6));
	cmd.append(to_bcd(PL_tones[rTONE], 6));
	cmd.append(post);
	waitFB("set_band_selection");
	set_trace(2, "set_band_selection()", str2hex(replystr.c_str(), replystr.length()));

	cmd.assign(pre_to);
	cmd.append("\x1A\x01");
	cmd += to_bcd_be( v, 2 );
	cmd += '\x01';
	cmd.append( post );

	waitFOR(23, "get band stack");
}

// these are only defined in this file
#undef NUM_FILTERS
#undef NUM_MODES

