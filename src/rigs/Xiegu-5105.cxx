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
// aunsigned long int with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include <iostream>

#include "Xiegu-5105.h"
#include "trace.h"

//=============================================================================
// Xiegu 5105
//
const char XI5105name_[] = "Xiegu-5105";
const char *XI5105modes_[] = { "LSB", "USB", "AM", "CW", NULL};
//"RTTY", "CW-R", "RTTY-R", NULL};
const char XI5105_mode_type[] = { 'L', 'U', 'U', 'L'};
//, 'L', 'U', 'U'};
const char *XI5105_widths[] = { "Wide", "Med", "Narr", NULL};
static int XI5105_bw_vals[] = {1,2,3,WVALS_LIMIT};

/*
static GUI XI5105_widgets[]= {
	{ (Fl_Widget *)btnVol,       2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME,  54, 125, 156 },
	{ (Fl_Widget *)sldrRFGAIN,  54, 145, 156 },
	{ (Fl_Widget *)btnNR,      214, 105,  50 },
	{ (Fl_Widget *)sldrNR,     266, 105, 156 },
	{ (Fl_Widget *)sldrMICGAIN,266, 125, 156 },
	{ (Fl_Widget *)sldrPOWER,  266, 145, 156 },
	{ (Fl_Widget *)NULL, 0, 0, 0 }
};
*/

RIG_XI5105::RIG_XI5105() {
	name_ = XI5105name_;
	modes_ = XI5105modes_;
	_mode_type = XI5105_mode_type;
	bandwidths_ = XI5105_widths;
	bw_vals_ = XI5105_bw_vals;
//	widgets = XI5105_widgets;

	comm_baudrate = BR19200;
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
	modeA = 1;
	bwA = 0;
	filter_nbr = 1;

	has_ptt_control = 
	has_smeter =
//	has_power_control =
//	has_volume_control =
	has_mode_control =
	has_bandwidth_control =
//	has_micgain_control =
//	has_attenuator_control =
//	has_preamp_control =
//	has_noise_control =
//	has_noise_reduction =
//	has_noise_reduction_control =
//	has_auto_notch =
//	has_rf_control =
//	has_split =
//	has_compON =
//	has_extras =
//	has_vox_onoff =
	true;

	defaultCIV = 0x70;
	adjustCIV(defaultCIV);

	precision = 1;
	ndigits = 9;

};

//=============================================================================

void RIG_XI5105::initialize()
{
/*
	XI5105_widgets[0].W = btnVol;
	XI5105_widgets[1].W = sldrVOLUME;
	XI5105_widgets[2].W = sldrRFGAIN;
	XI5105_widgets[3].W = btnNR;
	XI5105_widgets[4].W = sldrNR;
	XI5105_widgets[5].W = sldrMICGAIN;
	XI5105_widgets[6].W = sldrPOWER;
*/
}

//int  RIG_XI5105::adjust_bandwidth(int m)
//{
//	return 0;
//}

void RIG_XI5105::selectA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x00';
	cmd.append(post);
	waitFB("sel A");
set_trace(2, "selectA()", str2hex(cmd.c_str(), cmd.length()));
}

void RIG_XI5105::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x01';
	cmd.append(post);
set_trace(2, "selectB()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("sel B");
}

bool RIG_XI5105::check ()
{
// send a series of select A commands to allow xcvr to recognize baud rate
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x00';
	for (int n = 0; n < 5; n++) {
		cmd.append(post);
		MilliSleep(50);
	}
set_trace(2, "check()", str2hex(replystr.c_str(), replystr.length()));
	return true;
}

unsigned long int RIG_XI5105::get_vfoA ()
{
	if (useB) return A.freq;
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	string resp = pre_fm;
	resp += '\x03';
	if (waitFOR(11, "get vfo A")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			A.freq = fm_bcd_be(replystr.substr(p+5), 10);
	}
	return A.freq;
}

void RIG_XI5105::set_vfoA (unsigned long int freq)
{
	A.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	waitFB("set vfo A");
set_trace(2, "set_vfoA()", str2hex(cmd.c_str(), cmd.length()));
}

unsigned long int RIG_XI5105::get_vfoB ()
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
	return B.freq;
}

void RIG_XI5105::set_vfoB (unsigned long int freq)
{
	B.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	waitFB("set vfo B");
set_trace(2, "set_vfoB()", str2hex(cmd.c_str(), cmd.length()));
}

// Tranceiver PTT on/off
void RIG_XI5105::set_PTT_control(int val)
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

/*
// Volume control val 0 ... 100
void RIG_XI5105::set_volume_control(int val)
{
	int ICvol = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append(to_bcd(ICvol, 3));
	cmd.append( post );
	waitFB("set vol");
}

int RIG_XI5105::get_volume_control()
{
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append( post );
	string resp = pre_fm;
	resp.append("\x14\x01");
	if (waitFOR(9, "get vol")) {
		size_t p = replystr.rfind(resp);
		if ( p != string::npos)
			return (int)ceil(fm_bcd(replystr.substr(p+6),3) * 100 / 255);
	}
	return 0;
}

void RIG_XI5105::get_vol_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}
*/
int RIG_XI5105::get_smeter()
{
	cmd = pre_to;
	cmd.append("\x15\x02");
	cmd.append( post );
	string resp = pre_fm;
	resp.append("\x15\x02");
	int mtr = 0;
	if (waitFOR(9, "get smeter")) {
get_trace(2, "get_smeter()", str2hex(replystr.c_str(), replystr.length()));
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			mtr = replystr[p+7] * 100 + replystr[p+6];
			mtr = (int)ceil(mtr /2.55);
			if (mtr < 0) mtr = 0;
			if (mtr > 100) mtr = 100;
			return mtr;
		}
	}
	return -1;
}

/*
void RIG_XI5105::set_attenuator(int val)
{
	cmd = pre_to;
	cmd += '\x11';
	cmd += val ? '\x20' : '\x00';
	cmd.append( post );
	waitFB("set att");
}

void RIG_XI5105::set_noise(bool val)
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set noise");
}

int RIG_XI5105::get_noise()
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd.append(post);
	string resp = pre_fm;
	resp.append("\x16\x22");
	if (waitFOR(8, "get noise")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (replystr[p+6] ? 1 : 0);
	}
	return 0;
}

void RIG_XI5105::set_nb_level(int val)
{
}

int  RIG_XI5105::get_nb_level()
{
	return 0;
}

void RIG_XI5105::set_noise_reduction(int val)
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set nr");
}

int RIG_XI5105::get_noise_reduction()
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd.append(post);
	string resp = pre_fm;
	resp.append("\x16\x40");
	if (waitFOR(8, "get nr")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (replystr[p+6] ? 1 : 0);
	}
	return 0;
}

void RIG_XI5105::set_noise_reduction_val(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x06");
	val = val * 16 + 8;
	cmd.append(to_bcd(val, 3));
	cmd.append(post);
	waitFB("set NRval");
}

int RIG_XI5105::get_noise_reduction_val()
{
	int val = 0;
	string cstr = "\x14\x06";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get NRval")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			val = fm_bcd(replystr.substr(p+6), 3);
		val = (val - 8) / 16;
	}
	return val;
}

void RIG_XI5105::set_preamp(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd += (unsigned char)val;
	cmd.append( post );
	waitFB("set pre");
}

int RIG_XI5105::get_preamp()
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd.append( post );
	string resp = pre_fm;
	resp += '\x16';
	resp += '\x02';
	if (waitFOR(8, "get pre")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			if (replystr[p+6] == 0x01) {
				preamp_label("Pre", true);
				return 1;
			} else {
				preamp_label("Pre", false);
				return 0;
			}
		}
	}
	return 0;
}

void RIG_XI5105::set_rf_gain(int val)
{
	int ICrfg = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x02");
	cmd.append(to_bcd(ICrfg, 3));
	cmd.append( post );
	waitFB("set rf gain");
}

int RIG_XI5105::get_rf_gain()
{
	cmd = pre_to;
	cmd.append("\x14\x02").append(post);
	string resp = pre_fm;
	cmd.append("\x14\x02");
	if (waitFOR(9, "get rfg")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (int)ceil(fm_bcd(replystr.substr(p+6),3) * 100 / 255);
	}
	return 0;
}

void RIG_XI5105::set_squelch(int val)
{
	int ICsql = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x03");
	cmd.append(to_bcd(ICsql, 3));
	cmd.append( post );
	waitFB("set sql");
}

void RIG_XI5105::set_power_control(double val)
{
	cmd = pre_to;
	cmd.append("\x14\x0A");
	cmd.append(to_bcd((int)(val * 255 / 100), 3));
	cmd.append( post );
	waitFB("set power");
}

int RIG_XI5105::get_power_control()
{
	string cstr = "\x14\x0A";
	string resp = pre_fm;
	cmd = pre_to;
	cmd.append(cstr).append(post);
	resp.append(cstr);
	if (waitFOR(9, "get power")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (int)ceil(fm_bcd(replystr.substr(p + 6),3) * 100 / 255);
	}
	return progStatus.power_level;
}

void RIG_XI5105::set_mic_gain(int val)
{
	val = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x0B");
	cmd.append(to_bcd(val,3));
	cmd.append(post);
	waitFB("set mic");
}

int RIG_XI5105::get_mic_gain()
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

void RIG_XI5105::get_mic_gain_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}
*/

void RIG_XI5105::set_modeA(int val)
{
	modeA = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += val > 4 ? val + 2 : val;
	cmd += filter_nbr;
	cmd.append( post );
	waitFB("set mode A");
}

int RIG_XI5105::get_modeA()
{
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	string resp = pre_fm;
	resp += '\x04';
	if (waitFOR(8, "get modeA")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			modeA = replystr[p+5];
			if (modeA > 6) modeA -= 2;
			filter_nbr = replystr[p+6];
		}
	}
	return modeA;
}

void RIG_XI5105::set_modeB(int val)
{
	modeB = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += val > 4 ? val + 2 : val;
	cmd += filter_nbr;
	cmd.append( post );
	waitFB("set mode A");
}

int RIG_XI5105::get_modeB()
{
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	string resp = pre_fm;
	resp += '\x04';
	if (waitFOR(8, "get modeB")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			modeB = replystr[p+5];
			if (modeB > 6) modeB -= 2;
			filter_nbr = replystr[p+6];
		}
	}
	return modeB;
}

int RIG_XI5105::get_modetype(int n)
{
	return _mode_type[n];
}

void RIG_XI5105::set_bwA(int val)
{
	filter_nbr = val + 1;
	set_modeA(modeA);
}

int RIG_XI5105::get_bwA()
{
	return filter_nbr - 1;
}

void RIG_XI5105::set_bwB(int val)
{
	filter_nbr = val + 1;
	set_modeB(modeB);
}

int RIG_XI5105::get_bwB()
{
	return filter_nbr - 1;
}

/*
void RIG_XI5105::set_auto_notch(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x41';
	cmd += (unsigned char)val;
	cmd.append( post );
	waitFB("set AN");
}

int RIG_XI5105::get_auto_notch()
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x41';
	cmd.append( post );
	string resp = pre_fm;
	resp += '\x16';
	resp += '\x41';
	if (waitFOR(8, "get AN")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			if (replystr[6] == 0x01) {
				auto_notch_label("AN", true);
				return 1;
			} else {
				auto_notch_label("AN", false);
				return 0;
			}
		}
	}
	return 0;
}

void RIG_XI5105::set_compression(int on, int val)
{
	if (on) {
		cmd = pre_to;
		cmd.append("\x16\x44");
		cmd += '\x01';
		cmd.append(post);
		waitFB("set compON");
	} else {
		cmd = pre_to;
		cmd.append("\x16\x44");
		cmd += '\x00';
		cmd.append(post);
		waitFB("set compOFF");
	}
}

void RIG_XI5105::set_vox_onoff()
{
	if (progStatus.vox_onoff) {
		cmd = pre_to;
		cmd.append("\x16\x46");
		cmd += '\x01';
		cmd.append(post);
		waitFB("set voxON");
	} else {
		cmd = pre_to;
		cmd.append("\x16\x46");
		cmd += '\x00';
		cmd.append(post);
		waitFB("set voxOFF");
	}
}

void RIG_XI5105::set_split(bool val)
{
	split = val;
	cmd = pre_to;
	cmd += 0x0F;
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	waitFB(val ? "set split ON" : "set split OFF");
}

int RIG_XI5105::get_split()
{
	int read_split = 0;
	cmd.assign(pre_to);
	cmd.append("\x0F");
	cmd.append( post );
	if (waitFOR(7, "get split")) {
		string resp = pre_fm;
		resp.append("\x0F");
		size_t p = replystr.find(resp);
		if (p != string::npos)
			read_split = replystr[p+5];
		if (read_split != 0xFA) // fail byte
			split = read_split;
	}
	return split;
}
*/
