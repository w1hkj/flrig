// ----------------------------------------------------------------------------
// Copyright (C) 2020
//              David Freese, W1HKJ
//              Mark Gregory, G4LCH
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

#include "Xiegu-G90.h"

#include <iostream>
#include <sstream>

#include "gettext.h"

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
	'L', 'U', 'U', 'L', 'L', 'U' };

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
	if (!bp) { std::cout << "invalid bpair" << std::endl; return; }
	if (!b) { std::cout << "button not valid" << std::endl; return; }
	if (!lbl) { std::cout << "invalid label" << std::endl; return; }
	b->label(lbl);
	b->redraw();
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
	comm_echo = true;
	comm_rtscts = false;
	comm_rtsplus = true;
	comm_dtrplus = true;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;

	widgets = Xiegu_G90_widgets;

	has_smeter = true;
	has_power_out = true;
	has_swr_control = true;

	has_split = true;
	has_split_AB = true;

	has_mode_control = true;
	has_bandwidth_control = true;

	has_power_control = true;
	has_volume_control = true;
	has_sql_control = true;

	has_noise_control = true;

	has_attenuator_control = true;
	has_preamp_control = true;

	has_ptt_control = true;
	has_tune_control = true;

	has_agc_control = true;
	has_extras = true;

	has_cw_wpm = true;
	has_cw_vol = true;

	has_band_selection = true;

	has_compON = true;

	has_nb_level = true;

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
	has_tune_control = 
	has_agc_control = 
	has_extras = 
	has_cw_wpm = 
	has_cw_vol = 
	has_band_selection = 
	has_compON = 
	has_nb_level = true;
// false
	CW_sense =
	has_alc_control =
	has_micgain_control =
	has_ifshift_control = 
	has_pbt_controls = 
	has_rf_control = 
	has_noise_reduction = 
	has_noise_reduction_control = 
	has_auto_notch = 
	has_notch_control = 
	has_cw_spot_tone = 
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
	string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	ok = waitFOR(11, "check vfo");
	igett("check()");
	return ok;
}

static bool Xiegu_G90onA = true;

void RIG_Xiegu_G90::selectA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x00';
	cmd.append(post);
	set_trace(2, "selectA()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("select A");
	igett("select A");
	Xiegu_G90onA = true;
}

void RIG_Xiegu_G90::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x01';
	cmd.append(post);
	isett("selectB()");
	waitFB("select B");
	Xiegu_G90onA = false;
}

unsigned long int RIG_Xiegu_G90::get_vfoA ()
{
	if (useB) return A.freq;
	string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	if (waitFOR(11, "get vfo A")) {
		igett("get_vfoA()");
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
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
	waitFB("set vfo A");
	isett("set_vfoA()");
}

unsigned long int RIG_Xiegu_G90::get_vfoB ()
{
	if (!useB) return B.freq;
	string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	if (waitFOR(11, "get vfo B")) {
		igett("get_vfoB()");
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
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
	waitFB("set vfo B");
	isett("set_vfoB()");
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
	waitFB(val ? "set split ON" : "set split OFF");
	isett("set_split()");
}

int RIG_Xiegu_G90::get_split()
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
	igett("get_split()");
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
	waitFB("set ptt");
	ptt_ = val;
	isett("set_PTT()");
}

void RIG_Xiegu_G90::set_modeA(int val)
{
	A.imode = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += Xiegu_G90_mode_nbr[val];
	cmd += A.filter;
	cmd.append( post );
	waitFB("set mode A");
	isett("set mode A()");
}

int RIG_Xiegu_G90::get_modeA()
{
	int md = 0;
	size_t p = 0;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	string resp = pre_fm;
	resp += '\x04';

	if (waitFOR(8, "get mode A")) {
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
		igett("get_modeA()");
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
	waitFB("set mode B");
	isett("set mode B()");
}

int RIG_Xiegu_G90::get_modeB()
{
	int md = 0;
	size_t p = 0;

	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	string resp = pre_fm;
	resp += '\x04';

	if (waitFOR(8, "get mode B")) {
		igett("get_modeB()");
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
	set_trace(2, "set_attenuator()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("set att");
}

int RIG_Xiegu_G90::get_attenuator()
{
	cmd = pre_to;
	cmd += '\x11';
	cmd.append( post );
	string resp = pre_fm;
	resp += '\x11';
	if (waitFOR(7, "get ATT")) {
		get_trace(2, "get_ATT()", str2hex(replystr.c_str(), replystr.length()));
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
	waitFB(	(val == 0) ? "set Preamp OFF" : "set Preamp ON");

	set_trace(2, "set_preamp()", str2hex(cmd.c_str(), cmd.length()));
}


int RIG_Xiegu_G90::get_preamp()
{
	string cstr = "\x16\x02";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get Preamp Level")) {
		get_trace(2, "get_preamp()", str2hex(replystr.c_str(), replystr.length()));
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
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
		waitFB("set Comp ON");
		bpcomp.lbl = "COMP on";
	} else{
		cmd.assign(pre_to).append("\x16\x44");
		cmd += '\x00';
		cmd.append(post);
		waitFB("set Comp OFF");
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
	waitFB("SET cw wpm");
}

static int hexval(int hex)
{
	int val = 0;
	val += 10 * ((hex >> 4) & 0x0F);
	val += hex & 0x0F;
	return val;
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
	set_trace(2, "set_power_control()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("set power");
}

int RIG_Xiegu_G90::get_power_control()
{
	int val = pwrtbl[int(progStatus.power_level) - 1].mtr;

	string cstr = "\x14\x0A";
	string resp = pre_fm;
	cmd = pre_to;
	cmd.append(cstr).append(post);
	resp.append(cstr);
	if (waitFOR(9, "get power")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
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

	get_trace(2, "get_power_control()", str2hex(replystr.c_str(), replystr.length()));

	return pwr;
}

void RIG_Xiegu_G90::get_pc_min_max_step(double &min, double &max, double &step)
{
	min = 1; max = 20; step = 1;
}

/*
1W = xFE xFE x88 xE0 x14 x0A x00 x00 xFD
2W = xFE xFE x88 xE0 x14 x0A x00 x13 xFD
3W = xFE xFE x88 xE0 x14 x0A x00 x26 xFD
4W = xFE xFE x88 xE0 x14 x0A x00 x40 xFD
5W = xFE xFE x88 xE0 x14 x0A x00 x53 xFD
6W = xFE xFE x88 xE0 x14 x0A x00 x67 xFD
7W = xFE xFE x88 xE0 x14 x0A x00 x80 xFD
8W = xFE xFE x88 xE0 x14 x0A x00 x93 xFD
9W = xFE xFE x88 xE0 x14 x0A x01 x07 xFD
10W = xFE xFE x88 xE0 x14 x0A x01 x20 xFD
11W = xFE xFE x88 xE0 x14 x0A x01 x34 xFD
12W = xFE xFE x88 xE0 x14 x0A x01 x47 xFD
13W = xFE xFE x88 xE0 x14 x0A x01 x61 xFD
14W = xFE xFE x88 xE0 x14 x0A x01 x74 xFD
15W = xFE xFE x88 xE0 x14 x0A x01 x87 xFD
16W = xFE xFE x88 xE0 x14 x0A x02 x01 xFD
17W = xFE xFE x88 xE0 x14 x0A x02 x14 xFD
18W = xFE xFE x88 xE0 x14 x0A x02 x28 xFD
19W = xFE xFE x88 xE0 x14 x0A x02 x41 xFD
20W = xFE xFE x88 xE0 x14 x0A x02 x55 xFD
*/

int RIG_Xiegu_G90::get_power_out(void)
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
			if (replystr[p + 7] == '\xFD') {
				mtr = hexval(replystr[p + 6]);
			} else {
				mtr = 100 * hexval(replystr[p + 6]) + hexval(replystr[p + 7]);
			}
			if (mtr < 0) mtr = 0;
			if (mtr > 255) mtr = 255;
			size_t i = 0;
			for (i = 0; i < sizeof(pwrtbl) / sizeof(meterpair) - 1; i++)
				if (mtr >= pwrtbl[i].mtr && mtr < pwrtbl[i+1].mtr)
					break;
			mtr = (int)ceil(pwrtbl[i].val + 
				(pwrtbl[i+1].val - pwrtbl[i].val)*(mtr - pwrtbl[i].mtr)/(pwrtbl[i+1].mtr - pwrtbl[i].mtr));
			if (mtr > 20) mtr = 20;
		}
	}
	get_trace(2, "get_power_out()", str2hex(replystr.c_str(), replystr.length()));
	return mtr;
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
	set_trace(2, "set_volume_control()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("set vol");
}

int RIG_Xiegu_G90::get_volume_control()
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
		if (p != string::npos) {
			if (replystr[p + 7] == '\xFD') {
				val = hexval(replystr[p + 6]);
			} else {
				val = 100 * hexval(replystr[p + 6]) + hexval(replystr[p + 7]);
			}
		}
		val *= 100;
		val /= 255;
	}
	get_trace(2, "get_volume_control()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

void RIG_Xiegu_G90::set_squelch(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x03");
	cmd.append(bcd255(val));
	cmd.append( post );
	set_trace(2, "set_squelch()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("set Sqlch");
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
	string cstr = "\x14\x03";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get squelch")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			if (replystr[p + 7] == '\xFD') {
				val = hexval(replystr[p + 6]);
			} else {
				val = 100 * hexval(replystr[p + 6]) + hexval(replystr[p + 7]);
			}
		}
		val *= 100;
		val /= 255;
	}
	get_trace(2, "get_squelch()", str2hex(replystr.c_str(), replystr.length()));
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

int RIG_Xiegu_G90::get_smeter()
{
	string cstr = "\x15\x02";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= 0;
	if (waitFOR(8, "get smeter")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
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
	get_trace(2, "get_smeter()", str2hex(replystr.c_str(), replystr.length()));
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
	string cstr = "\x15\x12";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;
	if (waitFOR(9, "get SWR")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			if (replystr[p + 7] == '\xFD') {
				mtr = hexval(replystr[p + 6]);
			} else {
				mtr = 100 * hexval(replystr[p + 6]) + hexval(replystr[p + 7]);
			}
			size_t i = 0;
			for (i = 0; i < sizeof(swrtbl) / sizeof(meterpair) - 1; i++)
				if (mtr >= swrtbl[i].mtr && mtr < swrtbl[i+1].mtr)
					break;
			if (mtr < 0) mtr = 0;
			if (mtr > 255) mtr = 255;
			mtr = (int)ceil(smtrtbl[i].val + 
				(smtrtbl[i+1].val - smtrtbl[i].val)*(mtr - smtrtbl[i].mtr)/(smtrtbl[i+1].mtr - smtrtbl[i].mtr));
			if (mtr > 100) mtr = 100;
		}
	}
	get_trace(2, "get_swr()", str2hex(replystr.c_str(), replystr.length()));
	return mtr;
}

void RIG_Xiegu_G90::set_noise(bool val)
{  
   cmd = pre_to;
	cmd.append("\x16\x22");
	cmd += val ? 1 : 0;
	cmd.append(post);
	set_trace(2, "set_noise()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("set noise");
}

int RIG_Xiegu_G90::get_noise()
{
	string cstr = "\x16\x22";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(8, "get noise")) {
		size_t p = replystr.rfind(resp);
	get_trace(2, "get_noise()", str2hex(replystr.c_str(), replystr.length()));
	if (p != string::npos)
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
//		case 11: bandfreq = progStatus.f2;   bandmode = progStatus.m2;   break;
//		case 12: bandfreq = progStatus.f70;  bandmode = progStatus.m70;  break;
		case 13: bandfreq = progStatus.fgen; bandmode = progStatus.mgen; break;
	}
	if (bandmode < 0) bandmode = 0;
	if (bandmode > 4) bandmode = 0;
	if (useB) {
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
//		case 11: progStatus.f2 = vfo->freq;   progStatus.m2 = vfo->imode;   break;
//		case 12: progStatus.f70 = vfo->freq;  progStatus.m70 = vfo->imode;  break;
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
    if (waitFOR(8, "get AGC")) {
        size_t p = replystr.find(pre_fm);
        if (p == string::npos) return agcval;
        return (agcval = replystr[p + 6]); // 1 = off, 2 = FAST, 3 = SLOW 4 = AUTO
    }
    get_trace(2, "get_agc()", str2hex(replystr.c_str(), replystr.length()));
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
    waitFB("set AGC");
    return agcval;
}

static const char *agcstrs[] = {"AGC", "FST", "SLO","AUT"};
const char *RIG_Xiegu_G90::agc_label()
{
    return agcstrs[agcval];
}

int  RIG_Xiegu_G90::agc_val()
{
    return (agcval);
}


/*
void RIG_Xiegu_G90::set_rf_gain(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x02");
	cmd.append(bcd255(val));
	cmd.append( post );
	set_trace(2, "set_rf_gain()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("set RF");
}

int RIG_Xiegu_G90::get_rf_gain()
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

void RIG_Xiegu_G90::get_rf_min_max_step(double &min, double &max, double &step)
{
	min = 0; max = 100; step = 1;
}

int RIG_Xiegu_G90::get_alc()
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
			mtr = (int)ceil(mtr /1.2);
			if (mtr > 100) mtr = 100;
		}
	}
	get_trace(2, "get_alc()", str2hex(replystr.c_str(), replystr.length()));
	return mtr;
}

void RIG_Xiegu_G90::set_notch(bool on, int freq)
{
	int hexval;
	switch (vfo->imode) {
		default: case USB_g90:
			hexval = freq - 1500;
			break;
		case LSB_g90:
			hexval = 1500 - freq;
			break;
		case CW_g90:
			if (CW_sense)
				hexval = freq - progStatus.cw_spot_tone;
			else
				hexval = progStatus.cw_spot_tone - freq;
			break;
		case CWR_g90:
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
	set_trace(2, "set_notch() ", str2hex(cmd.c_str(), cmd.length()));

	cmd = pre_to;
	cmd.append("\x14\x0D");
	cmd.append(to_bcd(hexval,3));
	cmd.append(post);
	waitFB("set notch val");
	set_trace(2, "set_notch_val() ", str2hex(cmd.c_str(), cmd.length()));
}

bool RIG_Xiegu_G90::get_notch(int &val)
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
		get_trace(2, "get_notch()", str2hex(replystr.c_str(), replystr.length()));
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
			get_trace(2, "get_notch_val() ", str2hex(replystr.c_str(), replystr.length()));
			size_t p = replystr.rfind(resp);
			if (p != string::npos) {
				val = (int)ceil(fm_bcd(replystr.substr(p+6),3));
				val -= 128;
				val *= 20;
				switch (vfo->imode) {
					default: case USB_g90:
						val = 1500 + val;
						break;
					case LSB:
						val = 1500 - val;
						break;
					case CW_g90:
						if (CW_sense)
							val = progStatus.cw_spot_tone + val;
						else
							val = progStatus.cw_spot_tone - val;
						break;
					case CWR_g90:
						if (CW_sense)
							val = progStatus.cw_spot_tone - val;
						else
							val = progStatus.cw_spot_tone + val;
						break;
				}
			}
			get_trace(2, "get_notch_val() ", str2hex(replystr.c_str(), replystr.length()));
		}
	}
	return on;
}

void RIG_Xiegu_G90::get_notch_min_max_step(int &min, int &max, int &step)
{
	switch (vfo->imode) {
		default:
		case USB_g90: case LSB_g90:
			min = 0; max = 3000; step = 20; break;
		case CW_g90: case CWR_g90:
			min = progStatus.cw_spot_tone - 500;
			max = progStatus.cw_spot_tone + 500;
			step = 20;
			break;
	}
}

void RIG_Xiegu_G90::set_auto_notch(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x41';
	cmd += val ? 0x01 : 0x00;
	cmd.append( post );
	waitFB("set AN");
}

int RIG_Xiegu_G90::get_auto_notch()
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

void RIG_Xiegu_G90::set_if_shift(int val)
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
	waitFB("set IF inner");

	cmd = pre_to;
	cmd.append("\x14\x08");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set IF outer");
}

bool RIG_Xiegu_G90::get_if_shift(int &val)
{
	val = sh_;
	return sh_on_;
}

void RIG_Xiegu_G90::set_pbt_inner(int val)
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

void RIG_Xiegu_G90::set_pbt_outer(int val)
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

int RIG_Xiegu_G90::get_pbt_inner()
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

int RIG_Xiegu_G90::get_pbt_outer()
{
	int val = 0;
	string cstr = "\x14\x08";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(9, "get pbt outer")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			val = num100(replystr.substr(p+6));
			val -= 50;
		}
	}
	rig_trace(2, "get_pbt_outer()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

void RIG_Xiegu_G90::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -50;
	max = +50;
	step = 1;
}

void RIG_Xiegu_G90::set_nb_level(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x12");
	cmd.append(bcd255(val));
	cmd.append( post );
	set_trace(2, "set_nb_level()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("set NB level");
}

int  RIG_Xiegu_G90::get_nb_level()
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
*/

/*
void RIG_Xiegu_G90::set_noise_reduction(int val)
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd += val ? 1 : 0;
	cmd.append(post);
	set_trace(2, "set_noise_reduction()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("set NR");
}

int RIG_Xiegu_G90::get_noise_reduction()
{
	string cstr = "\x16\x40";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(8, "get NR")) {
		size_t p = replystr.rfind(resp);
	get_trace(2, "get_noise_reduction()", str2hex(replystr.c_str(), replystr.length()));
	if (p != string::npos)
		return (replystr[p+6] ? 1 : 0);
	}
	return progStatus.noise_reduction;
}

void RIG_Xiegu_G90::set_noise_reduction_val(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x06");
	val *= 16;
	val += 8;
	cmd.append(to_bcd(val, 3));
	cmd.append(post);
	set_trace(2, "set_noise_reduction_val()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("set NRval");
}

int RIG_Xiegu_G90::get_noise_reduction_val()
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
		if (p != string::npos) {
			val = fm_bcd(replystr.substr(p+6),3);
			val -= 8;
			val /= 16;
		}
	}
	get_trace(2, "get_nr_val()", str2hex(replystr.c_str(), replystr.length()));
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

void RIG_Xiegu_G90::setVfoAdj(double v)
{
	vfo_ = v;
	cmd.assign(pre_to);
	cmd.append("\x1A\x05");
	cmd += '\x00';
	cmd += '\x47';
	cmd.append(bcd255(int(v)));
	cmd.append(post);
	waitFB("SET vfo adjust");
	set_trace(2, "set_vfo_adj()", str2hex(cmd.c_str(), cmd.length()));
}

double RIG_Xiegu_G90::getVfoAdj()
{
	cmd.assign(pre_to);
	cmd.append("\x1A\x05");
	cmd += '\x00';
	cmd += '\x47';
	cmd.append(post);

	if (waitFOR(11, "get vfo adj")) {
		size_t p = replystr.find(pre_fm);
		if (p != string::npos) {
			vfo_ = num100(replystr.substr(p+8));
		}
	}
	get_trace(2, "get_vfo_adj()", str2hex(replystr.c_str(), replystr.length()));
	return vfo_;
}
 
void RIG_Xiegu_G90::set_cw_qsk()
{
	int n = round(progStatus.cw_qsk * 10); // values 0-255
	cmd.assign(pre_to).append("\x14\x0F");
	cmd.append(to_bcd(n, 3));
	cmd.append(post);
	waitFB("Set cw qsk delay");
}

void RIG_Xiegu_G90::set_cw_spot_tone()
{
	cmd.assign(pre_to).append("\x14\x09"); // values 0=300Hz 255=900Hz
	int n = round((progStatus.cw_spot_tone - 300) * 255.0 / 600.0);
	if (n > 255) n = 255;
	if (n < 0) n = 0;
	cmd.append(to_bcd(n, 3));
	cmd.append( post );
	waitFB("SET cw spot tone");
}

void RIG_Xiegu_G90::set_cw_vol()
{
	cmd.assign(pre_to);
	cmd.append("\x1A\x05");
	cmd += '\x01';
	cmd += '\x32';
	cmd.append(to_bcd((int)(progStatus.cw_vol * 2.55), 3));
	cmd.append( post );
	set_trace(2, "set_cw_vol()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("SET cw sidetone volume");
}

void RIG_Xiegu_G90::set_xcvr_auto_on()
{
	cmd.clear();
	int nr = progStatus.comm_baudrate == 6 ? 25 :
			 progStatus.comm_baudrate == 5 ? 13 :
			 progStatus.comm_baudrate == 4 ? 7 :
			 progStatus.comm_baudrate == 3 ? 3 : 2;
	cmd.append( nr, '\xFE');
	cmd.append(pre_to);
	cmd += '\x18'; cmd += '\x01';
	cmd.append(post);
	waitFB("Power ON", 2000);
	isett("Power ON");

	cmd = pre_to;
	cmd += '\x19'; cmd += '\x00';
	cmd.append(post);
	waitFOR(8, "get ID", 10000);
	igett("get ID");
}

void RIG_Xiegu_G90::set_xcvr_auto_off()
{
	cmd.clear();
	cmd.append(pre_to);
	cmd += '\x18'; cmd += '\x00';
	cmd.append(post);
	waitFB("Power OFF", 200);
	isett("Power OFF");
}

int RIG_Xiegu_G90::get_PTT()
{
	cmd = pre_to;
	cmd += '\x1c'; cmd += '\x00';
	string resp = pre_fm;
	resp += '\x1c'; resp += '\x00';
	cmd.append(post);
	if (waitFOR(8, "get PTT")) {
		igett("get_PTT()");
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			ptt_ = replystr[p + 6];
	}
	return ptt_;
}

int RIG_Xiegu_G90::get_bwA()
{
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(post);
	string resp = pre_fm;
	resp.append("\x1a\x03");
	if (waitFOR(8, "get bw A")) {
		size_t p = replystr.rfind(resp);
		A.iBW = fm_bcd(replystr.substr(p+6), 2);
	}

	return A.iBW;
}

void RIG_Xiegu_G90::set_bwA(int val)
{
	A.iBW = val;
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(to_bcd(A.iBW, 2));
	cmd.append(post);
	waitFB("set bw A");
}

int RIG_Xiegu_G90::get_bwB()
{
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(post);
	string resp = pre_fm;
	resp.append("\x1a\x03");
	if (waitFOR(8, "get bw B")) {
		size_t p = replystr.rfind(resp);
		B.iBW = fm_bcd(replystr.substr(p+6), 2);
	}

	return B.iBW;
}

void RIG_Xiegu_G90::set_bwB(int val)
{
	B.iBW = val;
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(to_bcd(B.iBW, 2));
	cmd.append(post);
	waitFB("set bw B");
}

std::string RIG_Xiegu_G90::get_BANDWIDTHS()
{
	stringstream s;
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_bwA[i] << " ";
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_bwB[i] << " ";
	return s.str();
}

void RIG_Xiegu_G90::set_BANDWIDTHS(std::string s)
{
	stringstream strm;
	strm << s;
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_bwA[i];
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_bwB[i];
}

int RIG_Xiegu_G90::adjust_bandwidth(int m)
{
	switch (m) {
		case AM_g90:
			bandwidths_ = Xiegu_G90_am_bws;
			bw_vals_ = Xiegu_G90_bw_vals_AM;
			return 29;
			break;
		case USB_g90: case LSB_g90:
			bandwidths_ = Xiegu_G90_ssb_bws;
			bw_vals_ = Xiegu_G90_vals_ssb_bws;
			return 34;
			break;
		case CW_g90: case CWR_g90:
			bandwidths_ = Xiegu_G90_ssb_bws;
			bw_vals_ = Xiegu_G90_vals_ssb_bws;
			return 10;
			break;
		default:
			bandwidths_ = Xiegu_G90_ssb_bws;
			bw_vals_ = Xiegu_G90_vals_ssb_bws;
			return 34;
	}
	return 0;
}

const char ** RIG_Xiegu_G90::bwtable(int m)
{
	const char **table;
	switch (m) {
		case AM_g90:
			table = Xiegu_G90_am_bws;
			break;
		case CW_g90: case CWR_g90:
		case USB_g90: case LSB_g90:
		default:
			table = Xiegu_G90_ssb_bws;
	}
	return table;
}

int RIG_Xiegu_G90::def_bandwidth(int m)
{
	return adjust_bandwidth(m);
}

void RIG_Xiegu_G90::set_mic_gain(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x0B");
	cmd.append(bcd255(val));
	cmd.append( post );
	set_trace(2, "set_mic_gain()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("set mic gain");
}

int RIG_Xiegu_G90::get_mic_gain()
{
	int val = progStatus.volume;
	string cstr = "\x14\x0B";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(9, "get mic gain")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			val = num100(replystr.substr(p+6));
	}
	get_trace(2, "get_mic_gain()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

void RIG_Xiegu_G90::get_mic_gain_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

int RIG_Xiegu_G90::next_preamp()
{
	if (Xiegu_G90onA) {
		if (A.freq > 100000000) {
			if (preamp_level) return 0;
			return 1;
		} else {
			switch (preamp_level) {
			case 0: return 1;
			case 1: return 2;
			case 2: return 0;
			}
		}
	} else {
		if (B.freq > 100000000) {
			if (preamp_level) return 0;
			return 1;
		} else {
			switch (preamp_level) {
			case 0: return 1;
			case 1: return 2;
			case 2: return 0;
			}
		}
	}
	return 0;
}

void RIG_Xiegu_G90::set_vox_onoff()
{
	if (progStatus.vox_onoff) {
		cmd.assign(pre_to).append("\x16\x46\x01");
		cmd.append( post );
		waitFB("set vox ON");
	} else {
		cmd.assign(pre_to).append("\x16\x46");
		cmd += '\x00';
		cmd.append( post );
		waitFB("set vox OFF");
	}
}

void RIG_Xiegu_G90::set_vox_gain()
{
	cmd.assign(pre_to).append("\x1A\x05"); // values 0-255 = 0 - 100%
	cmd +='\x01';
	cmd +='\x63';
	cmd.append(to_bcd((int)(progStatus.vox_gain * 2.55), 3));
	cmd.append( post );
	waitFB("SET vox gain");
}

void RIG_Xiegu_G90::set_vox_anti()
{
	cmd.assign(pre_to).append("\x1A\x05");	// values 0-255 = 0 - 100%
	cmd +='\x01';
	cmd +='\x64';
	cmd.append(to_bcd((int)(progStatus.vox_anti * 2.55), 3));
	cmd.append( post );
	waitFB("SET anti-vox");
}

void RIG_Xiegu_G90::set_vox_hang()
{
	cmd.assign(pre_to).append("\x1A\x05");	// values 00-20 = 0.0 - 2.0 sec
	cmd +='\x01';
	cmd +='\x65';
	cmd.append(to_bcd((int)(progStatus.vox_hang / 10 ), 2));
	cmd.append( post );
	waitFB("SET vox hang");
}
*/

