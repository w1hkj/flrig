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

#include <stdio.h>
#include <iostream>

#include "xiegu/X6100.h"
#include "debug.h"

#include "support.h"

//=============================================================================
// X6100

static int ret = 0;  // used by get_trace

enum {
	LSB_6100, USB_6100, AM_6100, CW_6100, RTTY_6100,
	FM_6100, WFM_6100, CWR_6100, RTTYR_6100 };

const char X6100name_[] = "Xiegu X6100";

const char *X6100modes_[] = {
		"LSB", "USB", "AM", "CW", "RTTY", "FM", "CW-R", "RTTY-R", NULL};

const char X6100_mode_type[] =
	{ 'L', 'U', 'U', 'U', 'L', 'U', 'L', 'U' };

const char *X6100_SSB_CWwidths[] = {
  "50",  "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
 "600",  "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700", "2800", "2900", "3000", "3100", "3200", "3300", "3400", "3500",
"3600",
NULL};
static int X6100_bw_vals_SSB[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,
41, WVALS_LIMIT};

const char *X6100_RTTYwidths[] = {
  "50",  "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
 "600",  "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700",
NULL};
static int X6100_bw_vals_RTTY[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32, WVALS_LIMIT};

const char *X6100_AMwidths[] = {
 "200",  "400",  "600",  "800", "1000", "1200", "1400", "1600", "1800", "2000",
"2200", "2400", "2600", "2800", "3000", "3200", "3400", "3600", "3800", "4000",
"4200", "4400", "4600", "4800", "5000", "5200", "5400", "5600", "5800", "6000",
"6200", "6400", "6600", "6800", "7000", "7200", "7400", "7600", "7800", "8000",
"8200", "8400", "8600", "8800", "9000", "9200", "9400", "9600", "9800", "10000",
NULL};
static int X6100_bw_vals_AM[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,
41,42,43,44,45,46,47,48,49,50,
WVALS_LIMIT};

const char *X6100_FMwidths[] = { "FIXED", NULL };
static int X6100_bw_vals_FM[] = {
1, WVALS_LIMIT};

static GUI X6100_widgets[]= {
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

void RIG_X6100::initialize()
{
	X6100_widgets[0].W = btnVol;
	X6100_widgets[1].W = sldrVOLUME;
	X6100_widgets[2].W = btnAGC;
	X6100_widgets[3].W = sldrRFGAIN;
	X6100_widgets[4].W = sldrSQUELCH;
	X6100_widgets[5].W = btnNR;
	X6100_widgets[6].W = sldrNR;
	X6100_widgets[7].W = btnLOCK;
	X6100_widgets[8].W = sldrINNER;
	X6100_widgets[9].W = btnCLRPBT;
	X6100_widgets[10].W = sldrOUTER;
	X6100_widgets[11].W = btnNotch;
	X6100_widgets[12].W = sldrNOTCH;
	X6100_widgets[13].W = sldrMICGAIN;
	X6100_widgets[14].W = sldrPOWER;

	btn_icom_select_10->deactivate();
	btn_icom_select_11->deactivate();
	btn_icom_select_12->deactivate();

	btn_icom_select_13->copy_label(_("GEN"));
	btn_icom_select_13->tooltip(_("General RX"));

	choice_rTONE->deactivate();
	choice_tTONE->deactivate();

}

RIG_X6100::RIG_X6100() {

	defaultCIV = 0x70;
	adjustCIV(defaultCIV);

	name_ = X6100name_;
	modes_ = X6100modes_;
	bandwidths_ = X6100_SSB_CWwidths;
	bw_vals_ = X6100_bw_vals_SSB;

	_mode_type = X6100_mode_type;
	widgets = X6100_widgets;

	def_freq = A.freq = 432399230;
	def_mode = A.imode = 1;
	def_bw = A.iBW = 28;
	B.freq = 432399230;
	B.imode = 1;
	B.iBW = 28;

	preamp_level = 0;

	restore_mbw = false;

	has_ifshift_control = false;

	has_extras =
	has_compON =
	has_compression =
	has_smeter =
	has_power_out =
	has_alc_control =
	has_swr_control =
	has_split =
	has_auto_notch =
	has_power_control =
	has_volume_control =
	has_mode_control =
	has_bandwidth_control =
	has_micgain_control =
	has_notch_control =
	has_noise_control =
	has_noise_reduction_control =
	has_noise_reduction =
	has_attenuator_control =
	has_preamp_control =
	has_ptt_control =
	has_tune_control =
	has_rf_control =
	has_sql_control = true;

	has_band_selection = true;

	precision = 1;
	ndigits = 9;

};

//======================================================================
// X6100 unique commands
//======================================================================

#include "debug.h"

void RIG_X6100::selectA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x00';
	cmd.append(post);
	waitFB("sel A");
	inuse = onA;
}

void RIG_X6100::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x01';
	cmd.append(post);
	waitFB("sel B");
	inuse = onB;
}

bool RIG_X6100::check ()
{
	std::string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	get_trace(1, "check");
	bool ok = waitFOR(11, "check vfo");
	igett("check()");
	return ok;
}

unsigned long int RIG_X6100::get_vfoA ()
{
	if (inuse == onB) return A.freq;
	std::string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	get_trace(1, "get vfo A");
	ret = waitFOR(11, "get vfo A");
	igett("");
	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			if (replystr[p+5] == -1)
				A.freq = 0;
			else
				A.freq = fm_bcd_be(replystr.substr(p+5), 10);
		}
	}
	return A.freq;
}

void RIG_X6100::set_vfoA (unsigned long int freq)
{
	A.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append(post);
	waitFB("set vfo A");
	isett("set_vfoA()");
}

unsigned long int RIG_X6100::get_vfoB ()
{
	if (inuse == onA) return B.freq;
	std::string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	get_trace(1, "get vfo B");
	ret = waitFOR(11, "get vfo B");
	igett("");
	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			if (replystr[p+5] == -1)
				A.freq = 0;
			else
				B.freq = fm_bcd_be(replystr.substr(p+5), 10);
		}
	}
	return B.freq;
}

void RIG_X6100::set_vfoB (unsigned long int freq)
{
	B.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	waitFB("set vfo B");
	isett("set_vfoB()");
}

void RIG_X6100::set_modeA(int val)
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
	isett("set_modeA()");
}

int RIG_X6100::get_modeA()
{
	int md;
	std::string resp = pre_fm;
	resp += '\x04';
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	get_trace(1,"get mode A");
	ret = waitFOR(8, "get mode A");
	igett("");
	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			md = replystr[p+5];
			if (md == -1) { md = filA = 0; }
			else {
				if (md > 6) md--;
				A.imode = md;
			}
		}
	}
	return A.imode;
}

void RIG_X6100::set_modeB(int val)
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
	isett("set_modeB()");
}

int RIG_X6100::get_modeB()
{
	int md;
	std::string resp = pre_fm;
	resp += '\x04';
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	get_trace(1, "get mode B");
	ret = waitFOR(8, "get mode B");
	igett("");
	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			md = replystr[p+5];
			if (md == -1) { md = filB = 0; }
			else {
				if (md > 6) md--;
				B.imode = md;
			}
		}
	}

	return B.imode;
}

int  RIG_X6100::get_bwA()
{
	std::string cstr = "\x1A\x03";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	get_trace(1, "get bw A");
	ret = waitFOR(8, "get bw A");
	igett("");
	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			A.iBW = (fm_bcd(replystr.substr(p+6),2));
	}
	return A.iBW;
}

int  RIG_X6100::get_bwB()
{
	std::string cstr = "\x1A\x03";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get bw B")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			B.iBW = (fm_bcd(replystr.substr(p+6),2));
	}
	return B.iBW;
}

void RIG_X6100::set_bwA(int val)
{
	A.iBW = val;
	cmd = pre_to;
	cmd.append("\x1A\x03");
	cmd.append(to_bcd(val, 2));
	cmd.append( post );
	waitFB("set bw A");
	isett("set_bwA()");
}

void RIG_X6100::set_bwB(int val)
{
	B.iBW = val;
	cmd = pre_to;
	cmd.append("\x1A\x03");
	cmd.append(to_bcd(val, 2));
	cmd.append( post );
	waitFB("set bw B");
	isett("set_bwB()");
}

int RIG_X6100::adjust_bandwidth(int m)
{
	if (m == 3 || m == 6) { //CW
		bandwidths_ = X6100_SSB_CWwidths;
		bw_vals_ = X6100_bw_vals_SSB;
		return 12;
	}
	if (m == 4 || m == 7) { //RTTY
		bandwidths_ = X6100_RTTYwidths;
		bw_vals_ = X6100_bw_vals_RTTY;
		return 12;
	}
	if (m == 2) { // AM
		bandwidths_ = X6100_AMwidths;
		bw_vals_ = X6100_bw_vals_AM;
		return 28;
	}
	if (m == 5) { // FM
		bandwidths_ = X6100_FMwidths;
		bw_vals_ = X6100_bw_vals_FM;
		return 0;
	}
	bandwidths_ = X6100_SSB_CWwidths;
	bw_vals_ = X6100_bw_vals_SSB;
	return 32;
}

int RIG_X6100::def_bandwidth(int m)
{
	return adjust_bandwidth(m);
}


int RIG_X6100::get_attenuator()
{
	std::string cstr = "\x11";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	get_trace(1, "get att");
	ret = waitFOR(7, "get att");
	igett("");
	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			return (replystr[p+5] ? 1 : 0);
	}
	return progStatus.attenuator;
}

void RIG_X6100::set_attenuator(int val)
{
	cmd = pre_to;
	cmd += '\x11';
	cmd += val ? '\x12' : '\x00';
	cmd.append( post );
	waitFB("set att");
	isett("set_attenuator()");
}

void RIG_X6100::set_preamp(int val)
{
	preamp_level = val;
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd += val ? '\x01' : '\x00';
	cmd.append( post );
	set_trace(1, "set preamp");
	waitFB("set preamp");
	isett("");
}

int RIG_X6100::get_preamp()
{
	std::string cstr = "\x16\x02";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	get_trace(1, "get preamp");
	ret = waitFOR(8, "get preamp");
	igett("");
	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			preamp_level = replystr[p+6] ? 1 : 0;
	}
	return preamp_level;
}

void RIG_X6100::set_auto_notch(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x41';
	cmd += val ? 0x01 : 0x00;
	cmd.append( post );
	waitFB("set AN");
	isett("set_auto_notch()");
}

int RIG_X6100::get_auto_notch()
{
	std::string cstr = "\x16\x41";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	get_trace(1, "get autonotch");
	ret = waitFOR(8, "get AN");
	igett("");
	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
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

void RIG_X6100::set_split(bool val)
{
	split = val;
	cmd = pre_to;
	cmd += 0x0F;
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	waitFB(val ? "set split ON" : "set split OFF");
	isett("set_split()");
}

int  RIG_X6100::get_split()
{
	int read_split = 0;
	cmd.assign(pre_to);
	cmd.append("\x0F");
	cmd.append( post );
	get_trace(1, "get split");
	ret = waitFOR(7, "get split");
	igett("");
	if (ret) {
		std::string resp = pre_fm;
		resp.append("\x0F");
		size_t p = replystr.find(resp);
		if (p != std::string::npos)
			read_split = replystr[p+5];
		if (read_split != 0xFA) // fail byte
			split = read_split;
	}
	return split;
}

// Volume control val 0 ... 100
void RIG_X6100::set_volume_control(int val)
{
	int vol = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append(to_bcd(vol, 3));
	cmd.append( post );
	waitFB("set vol");
	isett("set_volume_control()");
}

int RIG_X6100::get_volume_control()
{
	std::string cstr = "\x14\x01";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	get_trace(1, "get vol");
	ret = waitFOR(9, "get vol");
	igett("");
	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			return (int)ceil(fm_bcd(replystr.substr(p + 6),3) * 100 / 255);
	}
	return progStatus.volume;
}

void RIG_X6100::get_vol_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

void RIG_X6100::set_rf_gain(int val)
{
	int rfg = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x02");
	cmd.append(to_bcd(rfg, 3));
	cmd.append( post );
	waitFB("set RF");
	isett("set_rf_gain()");
}

int RIG_X6100::get_rf_gain()
{
	std::string cstr = "\x14\x02";
	std::string resp = pre_fm;
	cmd = pre_to;
	cmd.append(cstr).append(post);
	resp.append(cstr);
	get_trace(1, "get rfgain");
	ret = waitFOR(9, "get RF");
	igett("");
	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			return (int)ceil(fm_bcd(replystr.substr(p + 6),3) * 100 / 255);
	}
	return progStatus.rfgain;
}

void RIG_X6100::set_squelch(int val)
{
	int sql = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x03");
	cmd.append(to_bcd(sql, 3));
	cmd.append( post );
	waitFB("set sql");
	isett("set_squelch()");
}

int  RIG_X6100::get_squelch()
{
	std::string cstr = "\x14\x03";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	get_trace(1, "get squelch");
	ret = waitFOR(9, "get sql");
	igett("");
	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			return (int)ceil(fm_bcd(replystr.substr(p+6), 3) * 100 / 255);
	}
	return progStatus.squelch;
}

void RIG_X6100::set_power_control(double val)
{
	cmd = pre_to;
	cmd.append("\x14\x0A");
	cmd.append(to_bcd((int)(val * 255 / 100), 3));
	cmd.append( post );
	waitFB("set power");
	isett("set_power_control()");
}

double RIG_X6100::get_power_control()
{
	std::string cstr = "\x14\x0A";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr).append(post);
	get_trace(1, "get power");
	ret = waitFOR(9, "get power");
	igett("");
	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			return (int)ceil(fm_bcd(replystr.substr(p + 6),3) * 100 / 255);
	}
	return progStatus.power_level;
}

void RIG_X6100::get_pc_min_max_step(double &min, double &max, double &step)
{
	min = 1; max = 10.0; step = 0.1;
}

int RIG_X6100::get_smeter()
{
	std::string cstr = "\x15\x02";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;
	if (waitFOR(9, "get smeter")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			mtr = (int)ceil(mtr /2.55);
			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

int RIG_X6100::get_power_out()
{
	std::string cstr = "\x15\x11";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;
	if (waitFOR(9, "get pout")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			mtr = (int)ceil(mtr /2.55);
			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

int RIG_X6100::get_alc()
{
	std::string cstr = "\x15\x13";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;
	if (waitFOR(9, "get alc")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			mtr = (int)ceil(mtr /2.55);
			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

int RIG_X6100::get_mic_gain()
{
	std::string cstr = "\x14\x0B";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	get_trace(1, "get micgain");
	ret = waitFOR(9, "get mic");
	igett("");
	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			return (int)ceil(fm_bcd(replystr.substr(p+6),3) / 2.55);
	}
	return 0;
}

void RIG_X6100::set_mic_gain(int val)
{
	val = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x0B");
	cmd.append(to_bcd(val,3));
	cmd.append(post);
	waitFB("set mic");
	isett("set_mic_gain()");
}

void RIG_X6100::get_mic_gain_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_X6100::set_notch(bool on, int val)
{
	int notch = (int)(val * 256.0 / 3000.0);

	cmd = pre_to;
	cmd.append("\x16\x48");
	cmd += on ? '\x01' : '\x00';
	cmd.append(post);
	waitFB("set notch");
	isett("set_notc()");

	cmd = pre_to;
	cmd.append("\x14\x0D");
	cmd.append(to_bcd(notch,3));
	cmd.append(post);
	waitFB("set notch val");
	isett("set_notch_val()");

}

bool RIG_X6100::get_notch(int &val)
{
	bool on = false;
	val = 0;

	std::string cstr = "\x16\x48";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	get_trace(1, "get notch");
	ret = waitFOR(8, "get notch");
	igett("");
	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			on = replystr[p + 6];
		cmd = pre_to;
		resp = pre_fm;
		cstr = "\x14\x0D";
		cmd.append(cstr);
		resp.append(cstr);
		cmd.append(post);
		get_trace(1, "get notch value");
		ret = waitFOR(9, "notch val");
		igett("");
		if (ret) {
			size_t p = replystr.rfind(resp);
			if (p != std::string::npos)
				val = (int)ceil(fm_bcd(replystr.substr(p+6),3) * 3000.0 / 255.0);
		}
	}
	return on;
}

void RIG_X6100::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 3000;
	step = 20;
}

void RIG_X6100::set_noise(bool val)
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set noise");
	isett("set_noise()");
}

int RIG_X6100::get_noise()
{
	std::string cstr = "\x16\x22";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(8, "get noise")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			return (replystr[p+6] ? 1 : 0);
	}
	return 0;
}

void RIG_X6100::set_noise_reduction(int val)
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	waitFB("set NR");
	isett("set_noise_reduction()");
}

int RIG_X6100::get_noise_reduction()
{
	std::string cstr = "\x16\x40";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	get_trace(1, "get noise reduction");
	ret = waitFOR(8, "get NR");
	igett("");
	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			return (replystr[p+6] == 0x01 ? 1 : 0);
	}
	return 0;
}

// 0 < val < 100
void RIG_X6100::set_noise_reduction_val(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x06");
	cmd.append(to_bcd(val * 255 / 100, 3));
	cmd.append(post);
	waitFB("set NR val");
	isett("set_noise_reduction_val()");
}

int RIG_X6100::get_noise_reduction_val()
{
	std::string cstr = "\x14\x06";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	get_trace(1, "get noise reduction value");
	ret = waitFOR(9, "get NR val");
	igett("");
	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			return (int)ceil(fm_bcd(replystr.substr(p+6),3) / 2.55);
	}
	return 0;
}

void RIG_X6100::set_compression(int on, int val)
{
	if (on) {
		cmd = pre_to;
		cmd.append("\x16\x44");
		cmd += '\x01';
		cmd.append(post);
		waitFB("set Comp ON");
		isett("set compression ON");

		cmd.assign(pre_to).append("\x14\x0E");
		cmd.append(to_bcd(val * 255 / 10, 3));	// 0 - 10
		cmd.append( post );
		waitFB("set comp level");
		isett("set compression level");

	} else{
		cmd.assign(pre_to).append("\x16\x44");
		cmd += '\x00';
		cmd.append(post);
		waitFB("set Comp OFF");
		isett("set_commpression OFF");
	}
}

// Tranceiver PTT on/off
void RIG_X6100::set_PTT_control(int val)
{
	cmd = pre_to;
	cmd += '\x1c';
	cmd += '\x00';
	cmd += (unsigned char) val;
	cmd.append( post );
	waitFB("set ptt");
	isett("set PTT");
	ptt_ = val;
}

int RIG_X6100::get_PTT()
{
	cmd = pre_to;
	cmd += '\x1c'; cmd += '\x00';
	std::string resp = pre_fm;
	resp += '\x1c'; resp += '\x00';
	cmd.append(post);
	get_trace(1, "get PTT");
	ret = waitFOR(8, "get PTT");
	igett("");
	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			ptt_ = replystr[p + 6];
	}
	return ptt_;
}

void RIG_X6100::set_pbt_inner(int val)
{
	int shift = 128 + val * 128 / 50;
	if (shift < 0) shift = 0;
	if (shift > 255) shift = 255;

	cmd = pre_to;
	cmd.append("\x14\x07");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set PBT inner");
	isett("set PBT inner");
}

void RIG_X6100::set_pbt_outer(int val)
{
	int shift = 128 + val * 128 / 50;
	if (shift < 0) shift = 0;
	if (shift > 255) shift = 255;

	cmd = pre_to;
	cmd.append("\x14\x08");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set PBT outer");
	isett("set PBT outer");
}

int RIG_X6100::get_pbt_inner()
{
	int val = 0;
	std::string cstr = "\x14\x07";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	get_trace(1, "get pbt inner");
	ret = waitFOR(9, "get pbt inner");
	igett("");
	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			val = num100(replystr.substr(p+6));
			val -= 50;
		}
	}
	return val;
}

int RIG_X6100::get_pbt_outer()
{
	int val = 0;
	std::string cstr = "\x14\x08";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	get_trace(1, "get pbt outer");
	ret = waitFOR(9, "get pbt outer");
	igett("");
	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			val = num100(replystr.substr(p+6));
			val -= 50;
		}
	}
	return val;
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

void RIG_X6100::get_band_selection(int v)
{
	cmd.assign(pre_to);
	cmd.append("\x1A\x01");
	cmd += to_bcd_be( v, 2 );
	cmd += '\x01';
	cmd.append( post );
	get_trace(1, "get band stack");
	ret = waitFOR(23, "get band stack");
	igett("");
	if (ret) {
		size_t p = replystr.rfind(pre_fm);
		if (p != std::string::npos) {
			unsigned long int bandfreq = fm_bcd_be(replystr.substr(p+8, 5), 10);
			int bandmode = replystr[p+13];
			int bandfilter = replystr[p+14];
			if (inuse == onB) {
				set_vfoB(bandfreq);
				set_modeB(bandmode);
				set_FILT(bandfilter);
			} else {
				set_vfoA(bandfreq);
				set_modeA(bandmode);
				set_FILT(bandfilter);
			}
		}
	}
}

void RIG_X6100::set_band_selection(int v)
{
	unsigned long int freq = (inuse == onB ? B.freq : A.freq);
	int fil = (inuse == onB ? filB : filA);
	int mode = (inuse == onB ? B.imode : A.imode);

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
	isett("set band selection");

	set_trace(2, "set_band_selection()", str2hex(replystr.c_str(), replystr.length()));

	cmd.assign(pre_to);
	cmd.append("\x1A\x01");
	cmd += to_bcd_be( v, 2 );
	cmd += '\x01';
	cmd.append( post );

	get_trace(1, "get band stack");
	ret = waitFOR(23, "get band stack");
	igett("");
}

const char ** RIG_X6100::bwtable(int m)
{
	const char **table;
	switch (m) {
		case AM_6100:
			table = X6100_AMwidths;
			break;
		case FM_6100: case WFM_6100:
			table = X6100_SSB_CWwidths;
			break;
		case RTTY_6100: case RTTYR_6100:
			table = X6100_RTTYwidths;
			break;
		case CW_6100: case CWR_6100:
		case USB_6100: case LSB_6100:
			table = X6100_SSB_CWwidths;
			break;
		default:
			table = X6100_SSB_CWwidths;
			break;
	}
	return table;
}
