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

#include "IC756PRO3.h"
#include "debug.h"
#include "support.h"

//=============================================================================
// 756PRO-III

const char IC756PRO3name_[] = "IC-756PRO-III";

const char *IC756PRO3modes_[] = {
		"LSB", "USB", "AM", "CW", "RTTY", "FM", "CW-R", "RTTY-R",
		"D-LSB", "D-USB", "D-FM", NULL};

const char IC756PRO3_mode_type[] =
	{ 'L', 'U', 'U', 'U', 'L', 'U', 'L', 'U',
	  'L', 'U', 'U' };

const char *IC756PRO3_SSBwidths[] = {
  "50",  "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700", "2800", "2900", "3000", "3100", "3200", "3300", "3400", "3500",
"3600",
NULL};
static int IC756PRO3_bw_vals_SSB[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,
41, WVALS_LIMIT};

const char *IC756PRO3_RTTYwidths[] = {
  "50",  "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
 "600",  "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700",
NULL};
static int IC756PRO3_bw_vals_RTTY[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32, WVALS_LIMIT};

const char *IC756PRO3_AMFMwidths[] = { "FILT-1", "FILT-2", "FILT-3", NULL };
static int IC756PRO3_bw_vals_AMFM[] = { 1,2,3, WVALS_LIMIT};

static GUI ic756pro3_widgets[]= {
	{ (Fl_Widget *)btnVol, 2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME, 54, 125, 156 },
	{ (Fl_Widget *)sldrRFGAIN, 54, 145, 156 },
	{ (Fl_Widget *)sldrSQUELCH, 54, 165, 156 },
	{ (Fl_Widget *)btnNR, 214, 125,  50 },
	{ (Fl_Widget *)sldrNR, 266, 125, 156 },
	{ (Fl_Widget *)btnIFsh, 214, 105,  50 },
	{ (Fl_Widget *)sldrIFSHIFT, 266, 105, 156 },
	{ (Fl_Widget *)sldrMICGAIN, 266, 145, 156 },
	{ (Fl_Widget *)sldrPOWER, 266, 165, 156 },
	{ (Fl_Widget *)NULL, 0, 0, 0 }
};

RIG_IC756PRO3::RIG_IC756PRO3() {
	defaultCIV = 0x6E;
	name_ = IC756PRO3name_;
	modes_ = IC756PRO3modes_;
	bandwidths_ = IC756PRO3_SSBwidths;
	bw_vals_ = IC756PRO3_bw_vals_SSB;

	_mode_type = IC756PRO3_mode_type;
	atten_level = 3; // will force initializing to 0 dB
	preamp_level = 2; // will force initializaing to 0 dB

	widgets = ic756pro3_widgets;

	def_freq = freqA = freqB = A.freq = 14070000;
	def_mode = modeA = modeB = B.imode = 1;
	def_bw = bwA = bwB = A.iBW = B.iBW = 32;

	has_a2b =
	has_split =
	has_split_AB =
	has_bandwidth_control =
	has_ifshift_control =
	has_tune_control =

	has_swr_control =
	has_alc_control = 
	has_smeter =
	has_power_out =

	has_power_control =
	has_volume_control =
	has_mode_control =
	has_micgain_control =
	has_auto_notch =
	has_attenuator_control =
	has_preamp_control =
	has_ptt_control =
	has_noise_reduction =
	has_noise_reduction_control =
	has_noise_control =
	has_sql_control =
	has_rf_control = true;

	adjustCIV(defaultCIV);

	precision = 1;
	ndigits = 9;

};

void RIG_IC756PRO3::initialize()
{
	ic756pro3_widgets[0].W = btnVol;
	ic756pro3_widgets[1].W = sldrVOLUME;
	ic756pro3_widgets[2].W = sldrRFGAIN;
	ic756pro3_widgets[3].W = sldrSQUELCH;
	ic756pro3_widgets[4].W = btnNR;
	ic756pro3_widgets[5].W = sldrNR;
	ic756pro3_widgets[6].W = btnIFsh;
	ic756pro3_widgets[7].W = sldrIFSHIFT;
	ic756pro3_widgets[8].W = sldrMICGAIN;
	ic756pro3_widgets[9].W = sldrPOWER;
}

void RIG_IC756PRO3::selectA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\xB0';
	cmd.append(post);
	waitFB("sel A");
}

void RIG_IC756PRO3::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\xB1';
	cmd.append(post);
	waitFB("sel B");
}

void RIG_IC756PRO3::A2B()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\xB1';
	cmd.append(post);
	waitFB("A->B");
}

long RIG_IC756PRO3::get_vfoA ()
{
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

void RIG_IC756PRO3::set_vfoA (long freq)
{
	A.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	waitFB("set vfo A");
}

long RIG_IC756PRO3::get_vfoB ()
{
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

void RIG_IC756PRO3::set_vfoB (long freq)
{
	B.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	waitFB("set vfo B");
}

int RIG_IC756PRO3::get_smeter()
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

int RIG_IC756PRO3::get_power_out()
{
	string cstr = "\x15\x11";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get pout")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (int)ceil(fm_bcd(replystr.substr(p+6), 3) / 2.55);
	}
	return 0;
}

int RIG_IC756PRO3::get_alc()
{
	string cstr = "\x15\x13";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get alc")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (int)ceil(fm_bcd(replystr.substr(p+6), 3) / 2.55);
	}
	return 0;
}

int RIG_IC756PRO3::get_swr()
{
	string cstr = "\x15\x12";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get swr")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (int)ceil(fm_bcd(replystr.substr(p+6), 3) / 2.55);
	}
	return 0;
}

// Volume control val 0 ... 100

void RIG_IC756PRO3::set_volume_control(int val)
{
	ICvol = (int)(val * 2.55);
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append(to_bcd(ICvol, 3));
	cmd.append( post );
	waitFB("set vol");
}

int RIG_IC756PRO3::get_volume_control()
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
			return (int)ceil(fm_bcd(replystr.substr(p+6),3) / 2.55);
	}
	return 0;
}

void RIG_IC756PRO3::get_vol_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

// Tranceiver PTT on/off
void RIG_IC756PRO3::set_PTT_control(int val)
{
	cmd = pre_to;
	cmd += '\x1c';
	cmd += '\x00';
	cmd += (unsigned char) val;
	cmd.append( post );
	waitFB("set PTT");
}

// changed noise blanker to noise reduction
void RIG_IC756PRO3::set_noise(bool val)
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set noise");
}

int RIG_IC756PRO3::get_noise()
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
	return progStatus.noise;
}

void RIG_IC756PRO3::set_noise_reduction(int val)
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set NR");
}

int RIG_IC756PRO3::get_noise_reduction()
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

// 0 < val < 100
void RIG_IC756PRO3::set_noise_reduction_val(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x06");
	cmd.append(to_bcd(val * 2.55, 3));
	cmd.append(post);
	waitFB("set NR val");
}

int RIG_IC756PRO3::get_noise_reduction_val()
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
	return progStatus.noise_reduction_val;
}

int RIG_IC756PRO3::get_modetype(int n)
{
	return _mode_type[n];
}

int RIG_IC756PRO3::get_mic_gain()
{
	string cstr = "\x14\x0B";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get mic")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (int)ceil(fm_bcd(replystr.substr(p+6),3) / 2.55);
	}
	return 0;
}

void RIG_IC756PRO3::set_mic_gain(int val)
{
	val = (int)(val * 2.55);
	cmd = pre_to;
	cmd.append("\x14\x0B");
	cmd.append(to_bcd(val,3));
	cmd.append(post);
	waitFB("set mic");
}

void RIG_IC756PRO3::get_mic_gain_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_IC756PRO3::set_if_shift(int val)
{
	int shift = (int)(val * 128.0 / 825.0 + 128);
	cmd = pre_to;
	cmd.append("\x14\x07");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set if-shift");
}

bool  RIG_IC756PRO3::get_if_shift(int &val)
{
	string cstr = "\x14\x07";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	val = progStatus.shift_val;
	if (waitFOR(9, "get if-shift")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			val = (int)((fm_bcd(replystr.substr(p+6), 3) - 128) * 825.0 / 128.0);
	}
	return (progStatus.shift = (val != 0));
}

void RIG_IC756PRO3::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -825;
	max = +825;
	step = 25;
}

void RIG_IC756PRO3::set_squelch(int val)
{
	int IC756PRO3sql = (int)(val * 2.55);
	cmd = pre_to;
	cmd.append("\x14\x03");
	cmd.append(to_bcd(IC756PRO3sql, 3));
	cmd.append( post );
	waitFB("set sql");
}

int  RIG_IC756PRO3::get_squelch()
{
	string cstr = "\x14\x03";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get sql")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (int)ceil(fm_bcd(replystr.substr(p+6), 3) / 2.55);
	}
	return progStatus.squelch;
}

void RIG_IC756PRO3::set_rf_gain(int val)
{
	int IC756PRO3rfg = (int)(val * 2.55);
	cmd = pre_to;
	cmd.append("\x14\x02");
	cmd.append(to_bcd(IC756PRO3rfg, 3));
	cmd.append( post );
	waitFB("set rf gain");
}

int RIG_IC756PRO3::get_rf_gain()
{
	string cstr = "\x14\x02";
	string resp = pre_fm;
	cmd = pre_to;
	cmd.append(cstr).append(post);
	resp.append(cstr);
	if (waitFOR(9, "get rfgain")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (int)ceil(fm_bcd(replystr.substr(p + 6),3) / 2.55);
	}
	return progStatus.rfgain;
}

void RIG_IC756PRO3::set_power_control(double val)
{
	cmd = pre_to;
	cmd.append("\x14\x0A");
	cmd.append(to_bcd((int)(val * 2.55), 3));
	cmd.append( post );
	waitFB("set power");
}

int RIG_IC756PRO3::get_power_control()
{
	string cstr = "\x14\x0A";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr).append(post);
	if (waitFOR(9, "get power")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (int)ceil(fm_bcd(replystr.substr(p + 6),3) / 2.55);
	}
	return progStatus.power_level;
}

void RIG_IC756PRO3::set_split(bool val)
{
	cmd = pre_to;
	cmd += 0x0F;
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	waitFB("set split");
}

int RIG_IC756PRO3::get_split()
{
	LOG_WARN("%s", "get split - not implemented");
	return progStatus.split;
}

int RIG_IC756PRO3::adjust_bandwidth(int m)
{
	if (m == 0 || m == 1 || m == 8 || m == 9) { //SSB
		bandwidths_ = IC756PRO3_SSBwidths;
		bw_vals_ = IC756PRO3_bw_vals_SSB;
		return (32);
	}
	if (m == 3 || m == 6) { //CW
		bandwidths_ = IC756PRO3_SSBwidths;
		bw_vals_ = IC756PRO3_bw_vals_SSB;
		return (14);
	}
	if (m == 4 || m == 7) { //RTTY
		bandwidths_ = IC756PRO3_RTTYwidths;
		bw_vals_ = IC756PRO3_bw_vals_RTTY;
		return (28);
	}
	bandwidths_ = IC756PRO3_AMFMwidths;
	bw_vals_ = IC756PRO3_bw_vals_AMFM;
	return (0);
}

int RIG_IC756PRO3::def_bandwidth(int m)
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
	bandwidths_ = IC756PRO3_AMFMwidths;
	return (0);
}

const char **RIG_IC756PRO3::bwtable(int m)
{
	if (m == 0 || m == 1 || m == 8 || m == 9) //SSB
		return IC756PRO3_SSBwidths;
	if (m == 3 || m == 6) //CW
		return IC756PRO3_SSBwidths;
	if (m == 4 || m == 7) //RTTY
		return IC756PRO3_RTTYwidths;
	return IC756PRO3_AMFMwidths;
}

void RIG_IC756PRO3::tune_rig()
{
	cmd = pre_to;
	cmd.append("\x1c\x01\x02");
	cmd.append( post );
	waitFB("tune");
}

void RIG_IC756PRO3::set_bwA(int val)
{
	if (bandwidths_ == IC756PRO3_AMFMwidths) {
		A.iBW = val + 1;
		set_modeA(A.imode);
		return;
	}

	A.iBW = val;
	cmd = pre_to;
	cmd.append("\x1A\x03");
	cmd.append(to_bcd(A.iBW,2));
	cmd.append( post );
	waitFB("set bw A");
}

int  RIG_IC756PRO3::get_bwA()
{
	if (bandwidths_ == IC756PRO3_AMFMwidths) {
		return A.iBW - 1;
	}
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
	return A.iBW;
}

void RIG_IC756PRO3::set_bwB(int val)
{
	if (bandwidths_ == IC756PRO3_AMFMwidths) {
		B.iBW = val + 1;
		set_modeB(B.imode);
		return;
	}

	B.iBW = val;
	cmd = pre_to;
	cmd.append("\x1A\x03");
	cmd.append(to_bcd(B.iBW,2));
	cmd.append( post );
	waitFB("set bw B");
}

int  RIG_IC756PRO3::get_bwB()
{
	if (bandwidths_ == IC756PRO3_AMFMwidths) {
		return B.iBW - 1;
	}
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
	return B.iBW;
}

bool IC756PRO3_notchon = false;
void RIG_IC756PRO3::set_notch(bool on, int val)
{
	int notch = (int)(val/20.0 + 128);
	if (notch > 256) notch = 255;
	if (on != IC756PRO3_notchon) {
		cmd = pre_to;
		cmd.append("\x16\x48");
		cmd += on ? '\x01' : '\x00';
		cmd.append(post);
		waitFB("set notch");
		IC756PRO3_notchon = on;
	}

	if (on) {
		cmd = pre_to;
		cmd.append("\x14\x0D");
		cmd.append(to_bcd(notch,3));
		cmd.append(post);
		waitFB("set notch val");
	}
}

bool RIG_IC756PRO3::get_notch(int &val)
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

void RIG_IC756PRO3::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = -1280;
	max = 1280;
	step = 20;
}

int  RIG_IC756PRO3::next_attenuator()
{
	switch (atten_level) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 3;
		case 3: return 0;
	}
	return 0;
}

void RIG_IC756PRO3::set_attenuator(int val)
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

int RIG_IC756PRO3::get_attenuator()
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

int  RIG_IC756PRO3::next_preamp()
{
	switch (preamp_level) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 0;
	}
	return 0;
}

void RIG_IC756PRO3::set_preamp(int val)
{
	preamp_level = val;
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
}

int RIG_IC756PRO3::get_preamp()
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


void RIG_IC756PRO3::set_modeA(int val)
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
	cmd.append( post );
	waitFB("set mode A");
	if (datamode) { // LSB / USB ==> use DATA mode
		cmd = pre_to;
		cmd.append("\x1A\x06\x01");
		cmd.append(post);
		waitFB("data mode");
	}
}

int RIG_IC756PRO3::get_modeA()
{
	int md = 0;
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
			A.iBW = replystr[p+6];
			cstr = "\x1A\x06";
			resp = pre_fm;
			resp.append(cstr);
			cmd = pre_to;
			cmd.append(cstr);
			cmd.append(post);
			if (waitFOR(9, "data mode?")) {
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

void RIG_IC756PRO3::set_modeB(int val)
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
	cmd.append( post );
	waitFB("set mode B");
	if (datamode) { // LSB / USB ==> use DATA mode
		cmd = pre_to;
		cmd.append("\x1A\x06\x01");
		cmd.append(post);
		waitFB("data mode");
	}
}

int RIG_IC756PRO3::get_modeB()
{
	int md = 0;
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
			B.iBW = replystr[p+6];
			cstr = "\x1A\x06";
			resp = pre_fm;
			resp.append(cstr);
			cmd = pre_to;
			cmd.append(cstr);
			cmd.append(post);
			if (waitFOR(9, "data mode?")) {
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

void RIG_IC756PRO3::set_auto_notch(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x41';
	cmd += val ? 0x01 : 0x00;
	cmd.append( post );
	waitFB("set AN");
}

int RIG_IC756PRO3::get_auto_notch()
{
	string cstr = "\x16\x41";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get AN")) {
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

