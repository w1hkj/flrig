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

#include "debug.h"
#include "support.h"

//=============================================================================
// 756
//=============================================================================
#include "IC756.h"

const char IC756name_[] = "IC-756";

const char *IC756modes_[] = {
		"LSB", "USB", "AM", "CW", "RTTY", "FM", "CW-R", "RTTY-R", NULL};
// mode values are 0, 1, 2, 3, 4, 5, 7, 8
const char IC756_mode_type[] =
	{ 'L', 'U', 'U', 'U', 'L', 'U', 'L', 'U'};

const char *IC756_widths[] = { "NORM", "NARR", NULL};
static int IC756_bw_vals[] = {1, 2, WVALS_LIMIT};

static GUI ic756_widgets[]= {
	{ (Fl_Widget *)btnVol, 2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME, 54, 125, 156 },
	{ (Fl_Widget *)sldrRFGAIN, 54, 145, 156 },
	{ (Fl_Widget *)sldrSQUELCH, 54, 165, 156 },
	{ (Fl_Widget *)btnNR, 2, 185,  50 },
	{ (Fl_Widget *)sldrNR, 54, 185, 156 },
	{ (Fl_Widget *)btnIFsh, 214, 125,  50 },
	{ (Fl_Widget *)sldrIFSHIFT, 266, 125, 156 },
	{ (Fl_Widget *)btnNotch, 214, 145,  50 },
	{ (Fl_Widget *)sldrNOTCH, 266, 145, 156 },
	{ (Fl_Widget *)sldrMICGAIN, 266, 165, 156 },
	{ (Fl_Widget *)sldrPOWER, 266, 185, 156 },
	{ (Fl_Widget *)NULL, 0, 0, 0 }
};

RIG_IC756::RIG_IC756() {
	defaultCIV = 0x50;
	name_ = IC756name_;
	modes_ = IC756modes_;
	bandwidths_ = IC756_widths;
	bw_vals_ = IC756_bw_vals;
	_mode_type = IC756_mode_type;

	widgets = ic756_widgets;

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
	def_mode = modeB = modeA = B.imode = A.imode = 1;
	def_bw = bwB = bwA = B.iBW = A.iBW = 0;
	filter_nbr = 0;

	ICvol = 0;

	has_bandwidth_control =
	has_smeter =
	has_power_control =
	has_volume_control =
	has_mode_control =
	has_micgain_control =
	has_notch_control =
	has_attenuator_control =
	has_preamp_control =
	has_ifshift_control =
	has_ptt_control =
	has_tune_control =
	has_noise_control =
	has_noise_reduction =
	has_noise_reduction_control =
	has_rf_control =
	has_sql_control =
	has_split =
	restore_mbw = true;

	precision = 1;
	ndigits = 9;

	adjustCIV(defaultCIV);
};

void RIG_IC756::initialize()
{
	ic756_widgets[0].W = btnVol;
	ic756_widgets[1].W = sldrVOLUME;
	ic756_widgets[2].W = sldrRFGAIN;
	ic756_widgets[3].W = sldrSQUELCH;
	ic756_widgets[4].W = btnNR;
	ic756_widgets[5].W = sldrNR;
	ic756_widgets[6].W = btnIFsh;
	ic756_widgets[7].W = sldrIFSHIFT;
	ic756_widgets[8].W = btnNotch;
	ic756_widgets[9].W = sldrNOTCH;
	ic756_widgets[10].W = sldrMICGAIN;
	ic756_widgets[11].W = sldrPOWER;
}

//=============================================================================
// 756PRO
//=============================================================================
#include "IC756.h"

const char IC756PROname_[] = "IC-756PRO";

//=============================================================================
const char *IC756PROmodes_[] = {
		"LSB", "USB", "AM", "CW", "RTTY", "FM", "CW-R", "RTTY-R", NULL};

const char IC756PRO_mode_type[] =
	{ 'L', 'U', 'U', 'U', 'L', 'U', 'L', 'U' };

const char *IC756PRO_widths[] = { "FIXED", NULL};
static int IC756PRO_bw_vals[] = { 1, WVALS_LIMIT};

static GUI ic756pro_widgets[]= {
	{ (Fl_Widget *)btnVol, 2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME, 54, 125, 156 },
	{ (Fl_Widget *)sldrRFGAIN, 54, 145, 156 },
	{ (Fl_Widget *)sldrMICGAIN, 266, 125, 156 },
	{ (Fl_Widget *)sldrPOWER, 266, 145, 156 },
	{ (Fl_Widget *)NULL, 0, 0, 0 }
};

RIG_IC756PRO::RIG_IC756PRO() {
	defaultCIV = 0x5C;
	name_ = IC756PROname_;
	modes_ = IC756PROmodes_;
	bandwidths_ = IC756PRO_widths;
	bw_vals_ = IC756PRO_bw_vals;
	_mode_type = IC756PRO_mode_type;

	widgets = ic756pro_widgets;

	def_freq = freqA = freqB = A.freq = 14070000;
	def_mode = modeA = modeB = B.imode = 1;
	def_bw = bwA = bwB = A.iBW = B.iBW = 0;
	atten_level = 3;
	preamp_level = 2;
	adjustCIV(defaultCIV);

	has_smeter =
	has_power_control =
	has_volume_control =
	has_mode_control =
	has_micgain_control =
	has_auto_notch =
	has_attenuator_control =
	has_preamp_control =
	has_ptt_control =
	has_noise_control =
	has_rf_control = true;

	precision = 1;
	ndigits = 9;

};

void RIG_IC756PRO::initialize()
{
	ic756pro_widgets[0].W = btnVol;
	ic756pro_widgets[1].W = sldrVOLUME;
	ic756pro_widgets[2].W = sldrRFGAIN;
	ic756pro_widgets[3].W = sldrMICGAIN;
	ic756pro_widgets[4].W = sldrPOWER;
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

long RIG_IC756PRO::get_vfoA ()
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
	if (waitFOR(9, "get noise")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (replystr[p+6] ? 1 : 0);
	}
	return 0;
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

int IC756PROrfg = 0;
void RIG_IC756PRO::set_rf_gain(int val)
{
	IC756PROrfg = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x02");
	cmd.append(to_bcd(IC756PROrfg, 3));
	cmd.append( post );
	waitFB("set rf gain");
}

void RIG_IC756PRO::set_power_control(double val)
{
	cmd = pre_to;
	cmd.append("\x14\x0A");
	cmd.append(to_bcd((int)(val * 255 / 100), 3));
	cmd.append( post );
	waitFB("set power");
}

void RIG_IC756PRO::set_split(bool val)
{
	cmd = pre_to;
	cmd += 0x0F;
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	waitFB("set split");
}

//======================================================================
// IC756PRO unique commands
//======================================================================

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
	cmd.append( post );
	waitFB("set mode A");
}

int RIG_IC756PRO::get_modeA()
{
	int md;
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
		}
	}
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
	cmd.append( post );
	waitFB("set mode B");
}

int RIG_IC756PRO::get_modeB()
{
	int md;
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
		}
	}
	return B.imode;
}

int RIG_IC756PRO::adjust_bandwidth(int m)
{
	return 0;
}

int RIG_IC756PRO::def_bandwidth(int m)
{
	return (0);
}

const char **RIG_IC756PRO::bwtable(int m)
{
	return IC756PRO_widths;
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


