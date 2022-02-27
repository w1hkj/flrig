// ----------------------------------------------------------------------------
// Copyright (C) 2020
//              David Freese, W1HKJ
//              Mark Gregory, G4LCH
//              Morgan Jones <me@numin.it>
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

#include "xiegu/Xiegu-G90.h"

#include <iostream>
#include <sstream>

#include "gettext.h"

#include "support.h"

//=============================================================================
// IC-_g90
/*

*/

const char Xiegu_G90name_[] = "Xiegu-G90";

// these are only defined in this file
// undef'd at end of file

#define NUM_MODES  5

//static int mode_bwA[NUM_MODES] = {-1,-1,-1,-1,-1};
//static int mode_bwB[NUM_MODES] = {-1,-1,-1,-1,-1};

enum {
	LSB_g90, USB_g90, AM_g90, CW_g90, CWR_g90 };

const char *Xiegu_G90modes_[] = {
"LSB", "USB", "AM", "CW", "CW-R", NULL};

static char Xiegu_G90_mode_type[] = {
	'L', 'U', 'U', 'L', 'U', 'L' };

const char Xiegu_G90_mode_nbr[] = {
	0x00, // Select the LSB mode
	0x01, // Select the USB mode
	0x02, // Select the AM mode
	0x03, // Select the CW mode
	0x07, // Select the CW-R mode
};

const char *Xiegu_G90_ssb_bws[] = { "N/A", NULL };
static int Xiegu_G90_vals_ssb_bws[] = { 1, WVALS_LIMIT };

struct bpair {Fl_Widget *widget; std::string lbl;};
static bpair bpcomp;

static void btn_label(void *p)
{
	bpair *bp = (bpair *)p;
	Fl_Button *b = (Fl_Button *)bp->widget;
	const char *lbl = bp->lbl.c_str();
	if (!bp) return;
	if (!b) return;
	if (!lbl) return;
	b->label(lbl);
	b->redraw();
}

static inline void minmax(int min, int max, int &val)
{
	if (val > max) val = max;
	if (val < min) val = min;
}

static GUI Xiegu_G90_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 },	//0
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 },	//1
	{ (Fl_Widget *)btnAGC,        2, 145,  50 },	//2
	{ (Fl_Widget *)sldrSQUELCH, 266, 125, 156 },	//3
	{ (Fl_Widget *)sldrPOWER,   266, 145, 156 },	//4
	{ (Fl_Widget *)NULL, 0, 0, 0 }
};

void RIG_Xiegu_G90::initialize()
{
	Xiegu_G90_widgets[0].W = btnVol;
	Xiegu_G90_widgets[1].W = sldrVOLUME;
	Xiegu_G90_widgets[2].W = btnAGC;
	Xiegu_G90_widgets[3].W = sldrSQUELCH;
	Xiegu_G90_widgets[4].W = sldrPOWER;

	btn_icom_select_10->deactivate();
	btn_icom_select_11->deactivate();
	btn_icom_select_12->deactivate();

	btn_icom_select_13->copy_label(_("GEN"));
	btn_icom_select_13->tooltip(_("General RX"));

	choice_rTONE->deactivate();
	choice_tTONE->deactivate();

}

RIG_Xiegu_G90::RIG_Xiegu_G90() {
	defaultCIV = 0x88;
	name_ = Xiegu_G90name_;
	modes_ = Xiegu_G90modes_;
	bandwidths_ = Xiegu_G90_ssb_bws;
	bw_vals_ = Xiegu_G90_vals_ssb_bws;
	_mode_type = Xiegu_G90_mode_type;
	adjustCIV(defaultCIV);

	comm_retries = 2;
	comm_wait = 20;
	comm_timeout = 50;
	comm_echo = false;
	comm_rtscts = false;
	comm_rtsplus = true;
	comm_dtrplus = true;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;

	widgets = Xiegu_G90_widgets;

	precision = 1;
	ndigits = 9;
	A.filter = B.filter = 1;

	def_freq = A.freq = 14070000;
	def_mode = A.imode = 1;
	def_bw = A.iBW = 0;

	B.freq = 7070000;
	B.imode = 1;
	B.iBW = 0;

//true
	has_smeter = 
	has_power_out = 
	has_swr_control = 
	has_split = 
	has_split_AB = 
	has_mode_control = 
	has_bandwidth_control = 
	has_power_control = 
	has_volume_control = 
	has_sql_control = 
	has_noise_control = 
	has_attenuator_control = 
	has_preamp_control = 
	has_ptt_control = 
	has_agc_control = 
	has_extras = 
	has_cw_wpm = 
	has_cw_vol = 
	has_cw_spot_tone = 
	has_band_selection = 
	has_compON = 
	has_tune_control = 
	has_nb_level = true;
// false
	CW_sense =
	can_change_alt_vfo =
	has_alc_control =
	has_micgain_control =
	has_ifshift_control = 
	has_pbt_controls = 
	has_rf_control = 
	has_noise_reduction = 
	has_noise_reduction_control = 
	has_auto_notch = 
	has_notch_control = 
	has_cw_qsk = 
	has_vox_onoff = 
	has_vox_gain = 
	has_vox_anti = 
	has_vox_hang = 
	has_compression = 
	has_vfo_adj = 
	restore_mbw = 
	has_xcvr_auto_on_off = false;

};

//======================================================================
// Xiegu_G90 unique commands
//======================================================================

bool RIG_Xiegu_G90::check ()
{
	bool ok = false;
	std::string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	get_trace(1, "check()");
	ok = waitFOR(11, "check vfo");
	igett("");
	return ok;
}

void RIG_Xiegu_G90::selectA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x00';
	cmd.append(post);
	set_trace(1, "selectA()");
	waitFB("select A");
	isett("");
	inuse = onA;
}

void RIG_Xiegu_G90::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x01';
	cmd.append(post);
	set_trace(1, "selectB()");
	waitFB("select B");
	isett("");
	inuse = onB;
}

unsigned long int RIG_Xiegu_G90::get_vfoA ()
{
	if (inuse == onB) return A.freq;
	std::string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	get_trace(1, "get_vfoA()");
	if (waitFOR(11, "get vfo A")) {
		igett("");
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

void RIG_Xiegu_G90::set_vfoA (unsigned long int freq)
{
	A.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	set_trace(1, "set vfoA");
	waitFB("set vfo A");
	isett("");
}

unsigned long int RIG_Xiegu_G90::get_vfoB ()
{
	if (inuse == onA) return B.freq;
	std::string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	get_trace(1, "get vfoB");
	if (waitFOR(11, "get vfo B")) {
		igett("");
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

void RIG_Xiegu_G90::set_vfoB (unsigned long int freq)
{
	B.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	set_trace(1, "set vfoB");
	waitFB("set vfo B");
	isett("");
}

bool RIG_Xiegu_G90::can_split()
{
	return true;
}

void RIG_Xiegu_G90::set_split(bool val)
{
	split = val;
	cmd = pre_to;
	cmd += 0x0F;
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	set_trace(1, "set split");
	waitFB(val ? "set split ON" : "set split OFF");
	isett("");
}

int RIG_Xiegu_G90::get_split()
{
	int read_split = 0;
	cmd.assign(pre_to);
	cmd.append("\x0F");
	cmd.append( post );
	get_trace(1, "get split");
	if (waitFOR(7, "get split")) {
		igett("");
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

// Tranceiver PTT on/off
void RIG_Xiegu_G90::set_PTT_control(int val)
{
	cmd = pre_to;
	cmd += '\x1c';
	cmd += '\x00';
	cmd += (unsigned char) val;
	cmd.append( post );
	set_trace(1, "set PTT");
	waitFB("set ptt");
	isett("");
	ptt_ = val;
}

void RIG_Xiegu_G90::set_modeA(int val)
{
	A.imode = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += Xiegu_G90_mode_nbr[val];
	cmd += A.filter;
	cmd.append( post );
	set_trace(1, "set mode A");
	waitFB("set mode A");
	isett("");
}

int RIG_Xiegu_G90::get_modeA()
{
	int md = 0;
	size_t p = 0;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	std::string resp = pre_fm;
	resp += '\x04';

	get_trace(1, "get modeA");
	if (waitFOR(8, "get mode A")) {
		igett("");
		p = replystr.rfind(resp);
		if (p != std::string::npos) {
			if (replystr[p+5] == -1) { md = filA = 0; }
			else {
				for (md = 0; md < NUM_MODES; md++)
					if (replystr[p+5] == Xiegu_G90_mode_nbr[md]) 
						break;
				A.filter = replystr[p+6];
				if (md == NUM_MODES) {
					checkresponse();
					return A.imode;
				}
			}
		}
	} else {
		checkresponse();
		return A.imode;
	}

	A.imode = md;

	return A.imode;
}

void RIG_Xiegu_G90::set_modeB(int val)
{
	B.imode = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += Xiegu_G90_mode_nbr[val];
	cmd += B.filter;
	cmd.append( post );
	set_trace(1, "set mode B");
	waitFB("set mode B");
	isett("");
}

int RIG_Xiegu_G90::get_modeB()
{
	int md = 0;
	size_t p = 0;

	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	std::string resp = pre_fm;
	resp += '\x04';

	get_trace(1, "get modeB");
	if (waitFOR(8, "get mode B")) {
		igett("");
		p = replystr.rfind(resp);
		if (p != std::string::npos) {
			if (replystr[p+5] == -1) { md = filB = 0; }
			else {
				for (md = 0; md < NUM_MODES; md++)
					if (replystr[p+5] == Xiegu_G90_mode_nbr[md]) 
						break;
				if (md > 6) md -= 2;
				B.filter = replystr[p+6];
				if (md == NUM_MODES) {
					checkresponse();
					return A.imode;
				}
			}
		}
	} else {
		checkresponse();
		return B.imode;
	}

	return B.imode;
}

void RIG_Xiegu_G90::set_attenuator(int val)
{
   if (val) {
		atten_level = 1;
		atten_label("ATT", true);
		preamp_label("PRE", false);
	} else {
		atten_level = 0;
		atten_label("ATT", false);
	}

	cmd = pre_to;
	cmd += '\x11';
	cmd += atten_level ? '\x01' : '\x00';
	cmd.append( post );
	set_trace(1, "set_attenuator()");
	waitFB("set att");
	isett("");
}

int RIG_Xiegu_G90::get_attenuator()
{
	cmd = pre_to;
	cmd += '\x11';
	cmd.append( post );
	std::string resp = pre_fm;
	resp += '\x11';
	get_trace(1, "get ATT");
	if (waitFOR(7, "get ATT")) {
		igett("");
		size_t p = replystr.rfind(resp);
		if (replystr[p+5] == 0x00) {
			atten_level = 0;
			atten_label("ATT", false);
		} else {
			atten_level = 1;
			atten_label("ATT", true);
			preamp_label("PRE", false);
		}
	}
	return atten_level;
}

void RIG_Xiegu_G90::set_preamp(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';

	if (val) {
		preamp_label("PRE", true);
		atten_label("ATT", false);
		cmd += '\x01';
	} else {
		preamp_label("PRE", false);
		cmd += '\x00';
	}

	cmd.append( post );
	set_trace(1, "set preamp");
	waitFB(	(val == 0) ? "set Preamp OFF" : "set Preamp ON");
	isett("");
}


int RIG_Xiegu_G90::get_preamp()
{
	std::string cstr = "\x16\x02";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	get_trace(1, "get preamp");
	if (waitFOR(8, "get Preamp Level")) {
		igett("");
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			preamp_level = replystr[p+6];
		if (preamp_level == 1) {
			preamp_label("PRE", true);
			atten_label("ATT", false);
		} else
			preamp_label("PRE", false);
	}
	return preamp_level;
}

static int compon = 0;
static int compval = 0;

void RIG_Xiegu_G90::get_compression(int &on, int &val)
{
	on = compon;
	val = compval;
}

void RIG_Xiegu_G90::set_compression(int on, int val)
{
	compon = on;
	if (on) {
		cmd = pre_to;
		cmd.append("\x16\x44");
		cmd += '\x01';
		cmd.append(post);
		set_trace(1, "set comp ON");
		waitFB("set Comp ON");
		isett("");
		bpcomp.lbl = "COMP on";
	} else{
		cmd.assign(pre_to).append("\x16\x44");
		cmd += '\x00';
		cmd.append(post);
		set_trace(1, "set comp OFF");
		waitFB("set Comp OFF");
		isett("");
		bpcomp.lbl = "COMP off";
	}
	bpcomp.widget = btnCompON;
	Fl::awake(btn_label, &bpcomp);
}


// CW controls

void RIG_Xiegu_G90::set_cw_wpm()
{
	cmd.assign(pre_to).append("\x14\x0C"); // values 0-255 = 5 to 50 WPM
	cmd.append(to_bcd(round((progStatus.cw_wpm - 6) * 255 / (50 - 5)), 3));
	cmd.append( post );
	set_trace(1, "set CW WPM");
	waitFB("SET cw wpm");
	isett("");
}

static char hexchr(int val)
{
	static char c;
	c = ((val / 10) << 4) + (val % 10);
	return c;
}

struct meterpair {int mtr; float val;};

static meterpair pwrtbl[] = { 
{ 0, 1 },
{ 13, 2 },
{ 26, 3 },
{ 40, 4 },
{ 53, 5 },
{ 67, 6 },
{ 80, 7 },
{ 93, 8 },
{ 107, 9 },
{ 121, 10 },
{ 134, 11 },
{ 147, 12 },
{ 161, 13 },
{ 174, 14 },
{ 187, 15 },
{ 201, 16 },
{ 214, 17 },
{ 228, 18 },
{ 241, 19 },
{ 255, 20 }
};

void RIG_Xiegu_G90::set_power_control(double val)
{
	cmd = pre_to;
	cmd.append("\x14\x0A");
	int p = 0;
	for (int n = 0; n < 20; n++){
		if (val <= pwrtbl[n].val) {
			p = pwrtbl[n].mtr;
			break;
		}
	}
	cmd += hexchr (p / 100);
	cmd += hexchr (p % 100);
	cmd.append( post );
	set_trace(1, "set_power_control");
	waitFB("set power");
	isett("");
}

double RIG_Xiegu_G90::get_power_control()
{
	int val = pwrtbl[int(progStatus.power_level) - 1].mtr;

	std::string cstr = "\x14\x0A";
	std::string resp = pre_fm;
	cmd = pre_to;
	cmd.append(cstr).append(post);
	resp.append(cstr);
	get_trace(1, "get power control");
	if (waitFOR(9, "get power")) {
		igett("");
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			if (replystr[p + 7] == '\xFD') {
				val = hexval(replystr[p + 6]);
			} else {
				val = 100 * hexval(replystr[p + 6]) + hexval(replystr[p + 7]);
			}
		}
	}
	int pwr = 0;
	for (int n = 0; n < 20; n++) {
		if (val <= pwrtbl[n].mtr) {
			pwr = pwrtbl[n].val;
			break;
		}
	}

	return pwr;
}

void RIG_Xiegu_G90::get_pc_min_max_step(double &min, double &max, double &step)
{
	min = 1; max = 20; step = 1;
}

// Get Power-Meter (0000=0%, 0143=50%, 213=100%), 100% = 20 W

int RIG_Xiegu_G90::get_power_out(void)
{
	std::string cstr = "\x15\x11";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr = 0;
	get_trace(1, "get power out");
	if (waitFOR(9, "get power out")) {
		igett("");
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			// Power is 10 * output in watts, as BCD
			if (replystr[p + 7] == '\xFD') {
				mtr = hexval(replystr[p + 6]);
			} else {
				mtr = 100 * hexval(replystr[p + 7]) + hexval(replystr[p + 6]);
			}
//int p6 = hexval(replystr[p+6]);
//int p7 = hexval(replystr[p+7]);
//std::cout << "power out: " << str2hex(replystr.c_str(), replystr.length()) << std::endl;
//std::cout << "pout vals: " << p6 << ", " << p7 << ", " << 100*p7+p6 << std::endl;
			if (mtr < 0) mtr = 0;
			if (mtr > 213) mtr = 213;
		}
	}
	return round(mtr * 20.0 / 213.0);
}

int RIG_Xiegu_G90::power_scale()
{
	return 1;
}

// Volume control val 0 ... 100
void RIG_Xiegu_G90::set_volume_control(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x01");
	val *= 255; val /= 100;
	cmd += hexchr(val / 100);
	cmd += hexchr(val % 100);
	cmd.append( post );
	set_trace(1, "set_volume_control");
	waitFB("set vol");
	isett("");
}

int RIG_Xiegu_G90::get_volume_control()
{
	int val = progStatus.volume;
	std::string cstr = "\x14\x01";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	get_trace(1, "get volume control");
	if (waitFOR(9, "get vol")) {
		igett("");
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			if (replystr[p + 7] == '\xFD') {
				val = hexval(replystr[p + 6]);
			} else {
				val = 100 * hexval(replystr[p + 6]) + hexval(replystr[p + 7]);
			}
		}
		val *= 100;
		val /= 255;
	}
	return val;
}

void RIG_Xiegu_G90::set_squelch(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x03");
	cmd.append(bcd255(val));
	cmd.append( post );
	set_trace(1, "set_squelch");
	waitFB("set Sqlch");
	isett("");
}

/*
OFF FE FE 88 E0 14 03 FD FE FE E0 88 14 03 00 FD 
S1  FE FE 88 E0 14 03 FD FE FE E0 88 14 03 28 FD 
S2  FE FE 88 E0 14 03 FD FE FE E0 88 14 03 56 FD 
S3  FE FE 88 E0 14 03 FD FE FE E0 88 14 03 85 FD 
S4  FE FE 88 E0 14 03 FD FE FE E0 88 14 03 01 13 FD 
S5  FE FE 88 E0 14 03 FD FE FE E0 88 14 03 01 41 FD 
S6  FE FE 88 E0 14 03 FD FE FE E0 88 14 03 01 70 FD 
S7  FE FE 88 E0 14 03 FD FE FE E0 88 14 03 01 98 FD 
S8  FE FE 88 E0 14 03 FD FE FE E0 88 14 03 02 26 FD 
S9  FE FE 88 E0 14 03 FD FE FE E0 88 14 03 02 55 FD 
*/

int  RIG_Xiegu_G90::get_squelch()
{
	int val = progStatus.squelch;
	std::string cstr = "\x14\x03";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	get_trace(1, "get squelch");
	if (waitFOR(9, "get squelch")) {
		igett("");
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			if (replystr[p + 7] == '\xFD') {
				val = hexval(replystr[p + 6]);
			} else {
				val = 100 * hexval(replystr[p + 6]) + hexval(replystr[p + 7]);
			}
		}
		val *= 100;
		val /= 255;
	}
	return val;
}

void RIG_Xiegu_G90::get_vol_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

static meterpair smtrtbl[] = {
	{ 0,   0  }, // s0 -128dbm
	{ 10,  5  }, // s1 -121dbm
	{ 22,  11 }, // s2 -115dbm
	{ 35,  16 }, // s3 -109dbm	
	{ 56,  21 }, // s4 -102dbm
	{ 71,  26 }, // s5 -95dbm
	{ 89,  31 }, // s6 -88dbm
	{ 109, 37 }, // s7 -80dbm
	{ 132, 43 }, // s8 -71dbm
	{ 147, 50 }, // s9 -65dbm
	{ 168, 58 }, // s9+10 -57dbm
	{ 173, 66 }, // s9+20 -55dbm
	{ 183, 74 }, // s9+30 -51dbm
	{ 193, 82 }, // s9+40 -47dbm
	{ 201, 90 }, // s9+50 -45dbm
	{ 209, 100 } // s9+60 -42dbm
};

// Get S-Meter (0000=S0, 0120=S9, 0241=S9+60)

int RIG_Xiegu_G90::get_smeter()
{
	std::string cstr = "\x15\x02";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= 0;
	get_trace(1, "get smeter");
	if (waitFOR(8, "get smeter")) {
		igett("");
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			if (replystr[p + 7] == '\xFD') {
				mtr = hexval(replystr[p + 6]);
			} else {
				mtr = hexval(replystr[p + 6]);
				mtr += 100 * hexval(replystr[p + 7]);
			}
			if (mtr < 0) mtr = 0;
			if (mtr > 241) mtr = 241;
			size_t i = 0;
			for (i = 0; i < sizeof(smtrtbl) / sizeof(meterpair) - 1; i++)
				if (mtr >= smtrtbl[i].mtr && mtr < smtrtbl[i+1].mtr)
					break;
			mtr = (int)ceil(smtrtbl[i].val + 
				(smtrtbl[i+1].val - smtrtbl[i].val)*(mtr - smtrtbl[i].mtr)/(smtrtbl[i+1].mtr - smtrtbl[i].mtr));
			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}


static meterpair swrtbl[] = {
	{0, 0},
	{48, 12},
	{80, 25},
	{120, 50},
	{255, 100}
};

//Get SWR-Meter (0000=SWR1.0, 0048=SWR1.5, 0080=SWR2.0, 0120=SWR3.0)

int RIG_Xiegu_G90::get_swr()
{
	std::string cstr = "\x15\x12";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;
	get_trace(1, "get swr");
	if (waitFOR(9, "get SWR")) {
		igett("");
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			if (replystr[p + 7] == '\xFD') {
				mtr = hexval(replystr[p + 6]);
			} else {
				mtr = 100 * hexval(replystr[p + 7]) + hexval(replystr[p + 6]);
			}
std::cout << "SWR: " << str2hex(replystr.c_str(), replystr.length()) << std::endl;
std::cout << "meter: " << mtr << " --> ";
			size_t i = 0;
			for (i = 0; i < sizeof(swrtbl) / sizeof(meterpair) - 1; i++)
				if (mtr >= swrtbl[i].mtr && mtr < swrtbl[i+1].mtr)
					break;
			if (mtr < 0) mtr = 0;
			if (mtr > 255) mtr = 255;
			mtr = (int)ceil(smtrtbl[i].val + 
				(smtrtbl[i+1].val - smtrtbl[i].val)*(mtr - smtrtbl[i].mtr)/(smtrtbl[i+1].mtr - smtrtbl[i].mtr));
			if (mtr > 100) mtr = 100;
std::cout << mtr << std::endl;
		}
	}
	return mtr;
}

void RIG_Xiegu_G90::set_noise(bool val)
{  
   cmd = pre_to;
	cmd.append("\x16\x22");
	cmd += val ? 1 : 0;
	cmd.append(post);
	set_trace(1, "set_noise");
	waitFB("set noise");
	isett("");
}

int RIG_Xiegu_G90::get_noise()
{
	std::string cstr = "\x16\x22";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	get_trace(1, "get noise");
	if (waitFOR(8, "get noise")) {
		igett("");
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			return (replystr[p+6] ? 1 : 0);
	}
	return progStatus.noise;
}

void RIG_Xiegu_G90::get_band_selection(int v)
{
	int bandfreq, bandmode;
	switch (v) {
		default:
		case 1:  bandfreq = progStatus.f160; bandmode = progStatus.m160; break;
		case 2:  bandfreq = progStatus.f80;  bandmode = progStatus.m80;  break;
		case 3:  bandfreq = progStatus.f40;  bandmode = progStatus.m40;  break;
		case 4:  bandfreq = progStatus.f30;  bandmode = progStatus.m30;  break;
		case 5:  bandfreq = progStatus.f20;  bandmode = progStatus.m20;  break;
		case 6:  bandfreq = progStatus.f17;  bandmode = progStatus.m17;  break;
		case 7:  bandfreq = progStatus.f15;  bandmode = progStatus.m15;  break;
		case 8:  bandfreq = progStatus.f12;  bandmode = progStatus.m12;  break;
		case 9:  bandfreq = progStatus.f10;  bandmode = progStatus.m10;  break;
		case 10: bandfreq = progStatus.f6;   bandmode = progStatus.m6;   break;
		case 13: bandfreq = progStatus.fgen; bandmode = progStatus.mgen; break;
	}
	if (bandmode < 0) bandmode = 0;
	if (bandmode > 4) bandmode = 0;
	if (inuse == onB) {
		set_vfoB(bandfreq);
		set_modeB(bandmode);
	} else {
		set_vfoA(bandfreq);
		set_modeA(bandmode);
		}
	set_trace(1, "get band selection");
}

void RIG_Xiegu_G90::set_band_selection(int v)
{
	switch (v) {
		default:
		case 1:  progStatus.f160 = vfo->freq; progStatus.m160 = vfo->imode; break;
		case 2:  progStatus.f80 = vfo->freq;  progStatus.m80 = vfo->imode;  break;
		case 3:  progStatus.f40 = vfo->freq;  progStatus.m40 = vfo->imode;  break;
		case 4:  progStatus.f30 = vfo->freq;  progStatus.m30 = vfo->imode;  break;
		case 5:  progStatus.f20 = vfo->freq;  progStatus.m20 = vfo->imode;  break;
		case 6:  progStatus.f17 = vfo->freq;  progStatus.m17 = vfo->imode;  break;
		case 7:  progStatus.f15 = vfo->freq;  progStatus.m15 = vfo->imode;  break;
		case 8:  progStatus.f12 = vfo->freq;  progStatus.m12 = vfo->imode;  break;
		case 9:  progStatus.f10 = vfo->freq;  progStatus.m10 = vfo->imode;  break;
		case 10: progStatus.f6 = vfo->freq;   progStatus.m6 = vfo->imode;   break;
		case 13: progStatus.fgen = vfo->freq; progStatus.mgen = vfo->imode; break;
	}
	set_trace(1, "set_band_selection()");
}

static int agcval = 0;
int  RIG_Xiegu_G90::get_agc()
{
	cmd = pre_to;
	cmd.append("\x16\x12");
	cmd.append(post);
	get_trace(1, "get agc");
	if (waitFOR(8, "get AGC")) {
		igett("");
		size_t p = replystr.find(pre_fm);
		if (p == std::string::npos) return agcval;
		return (agcval = replystr[p + 6]); // 1 = off, 2 = FAST, 3 = SLOW 4 = AUTO
	}
	return agcval;
}

int RIG_Xiegu_G90::incr_agc()
{
	agcval++;
	if (agcval == 4) agcval = 0;
	cmd = pre_to;
	cmd.append("\x16\x12");
	cmd += agcval;
	cmd.append(post);
	set_trace(1, "incr agc");
	waitFB("set AGC");
	isett("");
	return agcval;
}

static const char *agcstrs[] = {"AGC-O", "AGC-F", "AGC-M","AGC-S"};
const char *RIG_Xiegu_G90::agc_label()
{
	return agcstrs[agcval];
}

int RIG_Xiegu_G90::agc_val()
{
	return (agcval);
}

void RIG_Xiegu_G90::set_cw_spot_tone()
{
	cmd.assign(pre_to).append("\x14\x09"); // values 0=300Hz 255=900Hz
	int n = round((progStatus.cw_spot_tone - 300) * 255.0 / 600.0 + 0.5);
	minmax(0, 255, n);

	cmd.append(to_bcd(n, 3));
	cmd.append( post );
	set_trace(1, "set cw spot tone");
	waitFB("SET cw spot tone");
	seth();
}

void RIG_Xiegu_G90::tune_rig(int how)
{
	cmd = pre_to;
	cmd.append("\x1c\x01");
	switch (how) {
		default:
		case 0:
			cmd += '\x00'; // off
			break;
		case 1:
			cmd += '\x01'; // ON
			break;
		case 2:
			cmd += '\x02'; // start tuning
			break;
	}
	cmd.append( post );
	waitFB("tune rig");
	ICtrace("tune rig", replystr);
}

int RIG_Xiegu_G90::get_tune()
{
	std::string resp;
	std::string cstr = "\x1C\x01";
	cmd.assign(pre_to).append(cstr).append(post);
	resp.assign(pre_fm).append(cstr);
	int val = tune_;
	if (waitFOR(8, "get TUNE")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			val = replystr[p + 6];
	}
	return (tune_ = val);
}

