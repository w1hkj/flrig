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

#include "IC756PRO2.h"
#include "debug.h"
#include "support.h"

const char IC756PRO2name_[] = "IC-756PRO-II";

//=============================================================================
const char *IC756PRO2modes_[] = {
		"LSB", "USB", "AM", "CW", "RTTY", "FM", "CW-R", "RTTY-R",
		"D-LSB", "D-USB", "D-FM", NULL};

const char IC756PRO2_mode_type[] =
	{ 'L', 'U', 'U', 'U', 'L', 'U', 'L', 'U',
	  'L', 'U', 'U' };

const char *IC756PRO2_SSBwidths[] = {
  "50",  "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700", "2800", "2900", "3000", "3100", "3200", "3300", "3400", "3500",
"3600",
NULL};
static int IC756PRO2_bw_vals_SSB[] = {
 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,
30,31,32,33,34,35,36,37,38,39,
40, WVALS_LIMIT};

const char *IC756PRO2_RTTYwidths[] = {
  "50",  "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
 "600",  "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700",
NULL};
static int IC756PRO2_bw_vals_RTTY[] = {
 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,
30,31, WVALS_LIMIT};

const char *IC756PRO2_AMFMwidths[] = { "FILT-1", "FILT-2", "FILT-3", NULL };
static int IC756PRO2_bw_vals_AMFM[] = { 0, 1, 2, WVALS_LIMIT};

static GUI IC756PRO2_widgets[]= {
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

void RIG_IC756PRO2::initialize()
{
	IC756PRO2_widgets[0].W = btnVol;
	IC756PRO2_widgets[1].W = sldrVOLUME;
	IC756PRO2_widgets[2].W = btnAGC;
	IC756PRO2_widgets[3].W = sldrRFGAIN;
	IC756PRO2_widgets[4].W = sldrSQUELCH;
	IC756PRO2_widgets[5].W = btnNR;
	IC756PRO2_widgets[6].W = sldrNR;
	IC756PRO2_widgets[7].W = btnLOCK;
	IC756PRO2_widgets[8].W = sldrINNER;
	IC756PRO2_widgets[9].W = btnCLRPBT;
	IC756PRO2_widgets[10].W = sldrOUTER;
	IC756PRO2_widgets[11].W = btnNotch;
	IC756PRO2_widgets[12].W = sldrNOTCH;
	IC756PRO2_widgets[13].W = sldrMICGAIN;
	IC756PRO2_widgets[14].W = sldrPOWER;

	btn_icom_select_11->deactivate();
	btn_icom_select_12->deactivate();
	btn_icom_select_13->deactivate();

	choice_rTONE->activate();
	choice_tTONE->activate();
}

RIG_IC756PRO2::RIG_IC756PRO2() {
	defaultCIV = 0x64;
	name_ = IC756PRO2name_;
	modes_ = IC756PRO2modes_;
	bandwidths_ = IC756PRO2_SSBwidths;
	bw_vals_ = IC756PRO2_bw_vals_SSB;

	_mode_type = IC756PRO2_mode_type;

	widgets = IC756PRO2_widgets;

	def_freq = freqA = freqB = A.freq = 14070000;
	def_mode = modeA = modeB = B.imode = 1;
	def_bw = bwA = bwB = A.iBW = B.iBW = 32;

	atten_level = 3;
	preamp_level = 2;

	adjustCIV(defaultCIV);

	has_extras = true;
	has_bandwidth_control = true;
	has_pbt_controls = true;
	has_FILTER = true;
	has_tune_control = true;
	has_swr_control = true;
	has_alc_control =  true;
	has_smeter = true;
	has_power_out = true;
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

	ICOMmainsub = true;

	has_band_selection = true;

	precision = 1;
	ndigits = 9;
	filA = filB = 1;

};

void RIG_IC756PRO2::swapAB()
{
	cmd = pre_to;
	cmd += 0x07; cmd += 0xB0;
	cmd.append(post);
	waitFB("Exchange vfos");
	set_trace(2, "swapAB()", str2hex(cmd.c_str(), cmd.length()));
}

void RIG_IC756PRO2::A2B()
{
	cmd = pre_to;
	cmd += 0x07; cmd += 0xB1;
	cmd.append(post);
	waitFB("Equalize vfos");
	set_trace(2, "A2B()", str2hex(cmd.c_str(), cmd.length()));
}

void RIG_IC756PRO2::selectA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\xD0';
	cmd.append(post);
	waitFB("sel A");
	set_trace(2, "selectA()", str2hex(cmd.c_str(), cmd.length()));
}

void RIG_IC756PRO2::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\xD1';
	cmd.append(post);
	waitFB("sel B");
	set_trace(2, "selectB()", str2hex(cmd.c_str(), cmd.length()));
}

bool RIG_IC756PRO2::check ()
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

long RIG_IC756PRO2::get_vfoA ()
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
	get_trace(2, "get_vfoA()", str2hex(replystr.c_str(), replystr.length()));
	return A.freq;
}

void RIG_IC756PRO2::set_vfoA (long freq)
{
	A.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	waitFB("set vfo A");
	set_trace(2, "set_vfoA()", str2hex(cmd.c_str(), cmd.length()));
}

long RIG_IC756PRO2::get_vfoB ()
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
	get_trace(2, "get_vfoB()", str2hex(replystr.c_str(), replystr.length()));
	return B.freq;
}

void RIG_IC756PRO2::set_vfoB (long freq)
{
	B.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	waitFB("set vfo B");
	set_trace(2, "set_vfoB()", str2hex(cmd.c_str(), cmd.length()));
}

int RIG_IC756PRO2::get_smeter()
{
	string cstr = "\x15\x02";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	int mtr = -1;
	if (waitFOR(9, "get smeter")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			mtr = (int)ceil(fm_bcd(replystr.substr(p+6), 3) / 2.55);
	}
	get_trace(2, "get_smeter()", str2hex(replystr.c_str(), replystr.length()));
	return mtr;
}

// Volume control val 0 ... 100

void RIG_IC756PRO2::set_volume_control(int val)
{
	ICvol = (int)(val);
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append(to_bcd(ICvol, 3));
	cmd.append( post );
	waitFB("set vol");
	set_trace(2, "set_volume_control()", str2hex(cmd.c_str(), cmd.length()));
}

int RIG_IC756PRO2::get_volume_control()
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
			return ((int)(fm_bcd(replystr.substr(p+6),3)));
	}
	get_trace(2, "get_volume_control()", str2hex(replystr.c_str(), replystr.length()));
	return 0;
}

void RIG_IC756PRO2::get_vol_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 255; step = 1;
}

// Tranceiver PTT on/off
void RIG_IC756PRO2::set_PTT_control(int val)
{
	cmd = pre_to;
	cmd += '\x1c';
	cmd += '\x00';
	cmd += (unsigned char) val;
	cmd.append( post );
	waitFB("set ptt");
	ptt_ = val;
	set_trace(2, "set_PTT()", str2hex(cmd.c_str(), cmd.length()));
}

int RIG_IC756PRO2::get_PTT()
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

// changed noise blanker to noise reduction
void RIG_IC756PRO2::set_noise(bool val)
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set noise");
	set_trace(2, "set_noise()", str2hex(cmd.c_str(), cmd.length()));
}

int RIG_IC756PRO2::get_noise()
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
		if (p != string::npos)
			val = (replystr[p+6] ? 1 : 0);
	}
	get_trace(2, "get_noise()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

void RIG_IC756PRO2::set_noise_reduction(int val)
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set NR");
}

int RIG_IC756PRO2::get_noise_reduction()
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
	return 0;
}

// 0 < val < 100
void RIG_IC756PRO2::set_noise_reduction_val(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x06");
	cmd.append(to_bcd(val * 255 / 100, 3));
	cmd.append(post);
	waitFB("set NR val");
}

int RIG_IC756PRO2::get_noise_reduction_val()
{
	string cstr = "\x14\x06";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get NR val")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (int)ceil(fm_bcd(replystr.substr(p+6),3) / 2.55);
	}
	return 0;
}


int RIG_IC756PRO2::get_modetype(int n)
{
	return _mode_type[n];
}

void RIG_IC756PRO2::set_mic_gain(int val)
{
	val = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x0B");
	cmd.append(to_bcd(val,3));
	cmd.append(post);
	waitFB("set mic");
}

void RIG_IC756PRO2::get_mic_gain_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_IC756PRO2::set_if_shift(int val)
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

void RIG_IC756PRO2::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -50;
	max = +50;
	step = 1;
}

void RIG_IC756PRO2::set_pbt_inner(int val)
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

void RIG_IC756PRO2::set_pbt_outer(int val)
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

int RIG_IC756PRO2::get_pbt_inner()
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

int RIG_IC756PRO2::get_pbt_outer()
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

int IC756PRO2sql = 0;
void RIG_IC756PRO2::set_squelch(int val)
{
	IC756PRO2sql = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x03");
	cmd.append(to_bcd(IC756PRO2sql, 3));
	cmd.append( post );
	waitFB("set sql");
}

int IC756PRO2rfg = 0;
void RIG_IC756PRO2::set_rf_gain(int val)
{
	IC756PRO2rfg = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x02");
	cmd.append(to_bcd(IC756PRO2rfg, 3));
	cmd.append( post );
	waitFB("set rf gain");
	set_trace(2, "set_rf_gain()", str2hex(cmd.c_str(), cmd.length()));
}

void RIG_IC756PRO2::set_power_control(double val)
{
	cmd = pre_to;
	cmd.append("\x14\x0A");
	cmd.append(to_bcd((int)(val * 255 / 100), 3));
	cmd.append( post );
	waitFB("set power");
	set_trace(2, "set_power_control()", str2hex(cmd.c_str(), cmd.length()));
}

void RIG_IC756PRO2::set_split(bool val)
{
	split = val;
	cmd = pre_to;
	cmd += 0x0F;
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	waitFB("set split");
	set_trace(2, "set_split()", str2hex(cmd.c_str(), cmd.length()));
}

int  RIG_IC756PRO2::get_split()
{
	return split;
}

//======================================================================
// IC756PRO2 unique commands
//======================================================================

void RIG_IC756PRO2::set_modeA(int val)
{
	A.imode = val;
	bool datamode = false;
	switch (val) {
		case 10 : val = 5; datamode = true; break;
		case 9  : val = 1; datamode = true; break;
		case 8  : val = 0; datamode = true; break;
		case 7  : val = 8; break;
		case 6  : val = 7; break;
		default: break;
	}
	cmd = pre_to;
	cmd += '\x06';
	cmd += val;
	cmd += filA;
	cmd.append( post );
	waitFB("set mode A");

	set_trace(4, "set mode A[", IC756PRO2modes_[A.imode], "] ", str2hex(replystr.c_str(), replystr.length()));

	if (datamode) { // LSB / USB ==> use DATA mode
		cmd = pre_to;
		cmd.append("\x1A\x06\x01");
		cmd.append(post);
		waitFB("data mode");
		set_trace(2, "set_digital()", str2hex(cmd.c_str(), cmd.length()));
	}
}

static const char *szfilter[] = {"1", "2", "3"};

int RIG_IC756PRO2::get_modeA()
{
	int md;
	string cstr = "\x04";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
//	replystr = "\xFE\xFE\x64\xE0\x04\xFD\xFE\xFE\xE0\x64\x04\x05\x01\xFD"; {
	if (waitFOR(8, "get mode A")) {
		get_trace(2, "get_modeA()", str2hex(replystr.c_str(), replystr.length()));
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			md = replystr[p+5];
			if (md > 6) md--;
			filA = replystr[p+6];
			cstr = "\x1A\x06";
			resp = pre_fm;
			resp.append(cstr);
			cmd = pre_to;
			cmd.append(cstr);
			cmd.append(post);
//			replystr = "\xFE\xFE\x64\xE0\x1A\x06\xFD\xFE\xFE\xE0\x64\x1A\x06\x00\xFD"; {
			if (waitFOR(8, "data ?")) {
				get_trace(2, "get_data_modeA()", str2hex(replystr.c_str(), replystr.length()));
				p = replystr.rfind(resp);
				if (p != string::npos) {
					if (replystr[p+6]) {
						switch (md) {
							case 0 : md = 8; break;
							case 1 : md = 9; break;
							case 5 : md = 10; break;
							default : break;
						}
					}
				}
			}
			A.imode = md;
		}
	}
	return A.imode;
}

void RIG_IC756PRO2::set_modeB(int val)
{
	B.imode = val;
	bool datamode = false;
	switch (val) {
		case 10 : val = 5; datamode = true; break;
		case 9  : val = 1; datamode = true; break;
		case 8  : val = 0; datamode = true; break;
		case 7  : val = 8; break;
		case 6  : val = 7; break;
		default: break;
	}
	cmd = pre_to;
	cmd += '\x06';
	cmd += val;
	cmd += filB;
	cmd.append( post );
	waitFB("set mode B");

	set_trace(4, "set mode B[", IC756PRO2modes_[A.imode], "] ", str2hex(replystr.c_str(), replystr.length()));

	if (datamode) { // LSB / USB ==> use DATA mode
		cmd = pre_to;
		cmd.append("\x1A\x06\x01");
		cmd.append(post);
		waitFB("data mode");
		set_trace(2, "set_digital()", str2hex(cmd.c_str(), cmd.length()));
	}
}

int RIG_IC756PRO2::get_modeB()
{
	int md;
	string cstr = "\x04";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(8, "get mode B")) {
		get_trace(2, "get_modeB()", str2hex(replystr.c_str(), replystr.length()));
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			md = replystr[p+5];
			if (md > 6) md--;
			filB = replystr[p+6];
			cstr = "\x1A\x06";
			resp = pre_fm;
			resp.append(cstr);
			cmd = pre_to;
			cmd.append(cstr);
			cmd.append(post);
			if (waitFOR(8, "data ?")) {
				get_trace(2, "get_data_modeB()", str2hex(replystr.c_str(), replystr.length()));
				p = replystr.rfind(resp);
				if (p != string::npos) {
					if (replystr[p+6]) {
						switch (md) {
							case 0 : md = 8; break;
							case 1 : md = 9; break;
							case 5 : md = 10; break;
							default : break;
						}
					}
				}
			}
			B.imode = md;
		}
	}
	return B.imode;
}

int RIG_IC756PRO2::adjust_bandwidth(int m)
{
	if (m == 0 || m == 1 || m == 8 || m == 9) { //SSB
		bandwidths_ = IC756PRO2_SSBwidths;
		bw_vals_ = IC756PRO2_bw_vals_SSB;
		return (32);
	}
	if (m == 3 || m == 6) { //CW
		bandwidths_ = IC756PRO2_SSBwidths;
		bw_vals_ = IC756PRO2_bw_vals_SSB;
		return (14);
	}
	if (m == 4 || m == 7) { //RTTY
		bandwidths_ = IC756PRO2_RTTYwidths;
		bw_vals_ = IC756PRO2_bw_vals_RTTY;
		return (28);
	}
	bandwidths_ = IC756PRO2_AMFMwidths;
		bw_vals_ = IC756PRO2_bw_vals_AMFM;
	return (0);
}

int RIG_IC756PRO2::def_bandwidth(int m)
{
	if (m == 0 || m == 1 || m == 8 || m == 9) { //SSB
		return (32);
	}
	if (m == 3 || m == 6) { //CW
		return (14);
	}
	if (m == 4 || m == 7) { //RTTY
		return (28);
	}
	bandwidths_ = IC756PRO2_AMFMwidths;
	return (0);
}

const char **RIG_IC756PRO2::bwtable(int m)
{
	if (m == 0 || m == 1 || m == 8 || m == 9) //SSB
		return IC756PRO2_SSBwidths;
	if (m == 3 || m == 6) //CW
		return IC756PRO2_SSBwidths;
	if (m == 4 || m == 7) //RTTY
		return IC756PRO2_RTTYwidths;
	return IC756PRO2_AMFMwidths;
}

int RIG_IC756PRO2::get_swr()
{
	string cstr = "\x15\x12";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr = -1;
	if (waitFOR(9, "get swr")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			mtr = (int)ceil(fm_bcd(replystr.substr(p + 6),3) / 2.55 );
	}
	get_trace(2, "get_swr()", str2hex(replystr.c_str(), replystr.length()));
	return mtr;
}

int RIG_IC756PRO2::get_alc()
{
	string cstr = "\x15\x13";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr = -1;
	if (waitFOR(9, "get alc")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			mtr = (int)ceil(fm_bcd(replystr.substr(p + 6),3) / 2.55 );
	}
	get_trace(2, "get_alc()", str2hex(replystr.c_str(), replystr.length()));
	return mtr;
}

// Transceiver power level return power in watts
int RIG_IC756PRO2::get_power_out()
{
	string cstr = "\x15\x11";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr = -1;
	if (waitFOR(9, "get power")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			mtr = (int)ceil(fm_bcd(replystr.substr(p + 6),3) / 2.55 );
	}
	get_trace(2, "get_power_out()", str2hex(replystr.c_str(), replystr.length()));
	return mtr;
}

void RIG_IC756PRO2::set_bwA(int val)
{
	if (bandwidths_ == IC756PRO2_AMFMwidths) {
		A.iBW = val;
		set_modeA(A.imode);
		return;
	}

	A.iBW = val;
	cmd = pre_to;
	cmd.append("\x1A\x03");
	cmd.append(to_bcd(A.iBW,2));
	cmd.append( post );
	waitFB("set bw A");
	set_trace(4, "set_bwA() ", bwtable(A.imode)[val], ": ", str2hex(cmd.c_str(), cmd.length()));
}

int  RIG_IC756PRO2::get_bwA()
{
	string cstr = "\x1A\x03";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get bw A")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			A.iBW = (int)(fm_bcd(replystr.substr(p + 6), 2));
	}
	get_trace(2, "get_bwA()", str2hex(replystr.c_str(), replystr.length()));
	if (bandwidths_ == IC756PRO2_AMFMwidths) {
		if (A.iBW < 0) A.iBW = 0;
		if (A.iBW > 2) A.iBW = 2;
	}
	return A.iBW;
}

void RIG_IC756PRO2::set_bwB(int val)
{
	if (bandwidths_ == IC756PRO2_AMFMwidths) {
		B.iBW = val;
		set_modeB(B.imode);
		return;
	}

	B.iBW = val;
	cmd = pre_to;
	cmd.append("\x1A\x03");
	cmd.append(to_bcd(B.iBW,2));
	cmd.append( post );
	waitFB("set bw B");
	set_trace(4, "set_bwB() ", bwtable(B.imode)[val], ": ", str2hex(cmd.c_str(), cmd.length()));
}

int  RIG_IC756PRO2::get_bwB()
{
	string cstr = "\x1A\x03";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get bw B")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			B.iBW = (int)(fm_bcd(replystr.substr(p + 6), 2));
	}
	get_trace(2, "get_bwB()", str2hex(replystr.c_str(), replystr.length()));
	if (bandwidths_ == IC756PRO2_AMFMwidths) {
		if (B.iBW < 0) B.iBW = 0;
		if (B.iBW > 2) B.iBW = 2;
	}
	return B.iBW;
}

bool IC756PRO2_notchon = false;

void RIG_IC756PRO2::set_notch(bool on, int val)
{
	int notch = (int)(val/20.0 + 128);
	if (notch > 256) notch = 255;
	if (on != IC756PRO2_notchon) {
		cmd = pre_to;
		cmd.append("\x16\x48");
		cmd += on ? '\x01' : '\x00';
		cmd.append(post);
		waitFB("set notch");
		IC756PRO2_notchon = on;
	}

	if (on) {
		cmd = pre_to;
		cmd.append("\x14\x0D");
		cmd.append(to_bcd(notch,3));
		cmd.append(post);
		waitFB("set notch val");
	}
}

bool RIG_IC756PRO2::get_notch(int &val)
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
			if (p != string::npos)
				val = 20*ceil(fm_bcd(replystr.substr(p + 6),3) - 128);
		}
	}
	return on;
}

void RIG_IC756PRO2::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = -1280;
	max = 1280;
	step = 20;
}

int  RIG_IC756PRO2::next_attenuator()
{
	switch (atten_level) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 3;
		case 3: return 0;
	}
	return 0;
}

void RIG_IC756PRO2::set_attenuator(int val)
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

int RIG_IC756PRO2::get_attenuator()
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
	get_trace(2, "get_ATT()", str2hex(replystr.c_str(), replystr.length()));
	return atten_level;
}

int  RIG_IC756PRO2::next_preamp()
{
	switch (preamp_level) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 0;
	}
	return 0;
}

void RIG_IC756PRO2::set_preamp(int val)
{
	if (preamp_level == 1) {
		preamp_label("Pre 1", true);
	} else if (preamp_level == 2) {
		preamp_label("Pre 2", true);
	} else if (preamp_level == 0) {
		preamp_label("Pre", false);
	}
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd += (unsigned char) preamp_level;
	cmd.append( post );
	waitFB("set preamp");
	set_trace(2, "set_preamp()", str2hex(cmd.c_str(), cmd.length()));
}

int RIG_IC756PRO2::get_preamp()
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
	get_trace(2, "get_preamp()", str2hex(replystr.c_str(), replystr.length()));
	return preamp_level;
}

const char *RIG_IC756PRO2::FILT(int &val)
{
	if (useB) {
		val = filB;
		return(szfilter[filB - 1]);
	}
	else {
		val = filA;
		return (szfilter[filA - 1]);
	}
}

const char *RIG_IC756PRO2::nextFILT()
{
	if (useB) {
		filB++;
		if (filB > 3) filB = 1;
		set_modeB(B.imode);
		return(szfilter[filB - 1]);
	} else {
		filA++;
		if (filA > 3) filA = 1;
		set_modeA(A.imode);
		return(szfilter[filA - 1]);
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

void RIG_IC756PRO2::get_band_selection(int v)
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
			int banddata = replystr[p+15] & 0x10;
			if ((bandmode == 0 || bandmode == 1) && banddata) bandmode += 7;
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

void RIG_IC756PRO2::set_band_selection(int v)
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
	if (mode >= 7)
		cmd += '\x10';
	else
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


