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

#include "IC910.h"

const char IC910Hname_[] = "IC-910H";
const char *IC910Hmodes_[] = {
		"LSB", "USB", "AM", "CW", "FM", NULL};
// mode values are 0, 1, 2, 3, 4, 5, 7, 8
const char IC910H_mode_type[] =
	{ 'L', 'U', 'U', 'U', 'L', 'U'};

const char *IC910H_widths[] = {"none", NULL};
static int IC910H_bw_val[] = {1, WVALS_LIMIT};

static GUI IC910_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 },	//0
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 },	//1
	{ (Fl_Widget *)btnAGC,        2, 145,  50 },	//2
	{ (Fl_Widget *)sldrRFGAIN,   54, 145, 156 },	//3
	{ (Fl_Widget *)sldrSQUELCH,  54, 165, 156 },	//4
	{ (Fl_Widget *)btnNR,         2, 185,  50 },	//5
	{ (Fl_Widget *)sldrNR,       54, 185, 156 },	//6
	{ (Fl_Widget *)btnLOCK,     214, 125,  50 },	//7
	{ (Fl_Widget *)sldrINNER,   266, 125, 156 },	//8
	{ (Fl_Widget *)btnCLRPBT,   214, 145,  50 },	//9
	{ (Fl_Widget *)sldrOUTER,   266, 145, 156 },	//10
	{ (Fl_Widget *)sldrMICGAIN, 266, 165, 156 },	//11
	{ (Fl_Widget *)sldrPOWER,   266, 185, 156 },	//12
	{ (Fl_Widget *)NULL, 0, 0, 0 }
};

void RIG_IC910H::initialize()
{
	IC910_widgets[0].W = btnVol;
	IC910_widgets[1].W = sldrVOLUME;
	IC910_widgets[2].W = btnAGC;
	IC910_widgets[3].W = sldrRFGAIN;
	IC910_widgets[4].W = sldrSQUELCH;
	IC910_widgets[5].W = btnNR;
	IC910_widgets[6].W = sldrNR;
	IC910_widgets[7].W = btnLOCK;
	IC910_widgets[8].W = sldrINNER;
	IC910_widgets[9].W = btnCLRPBT;
	IC910_widgets[10].W = sldrOUTER;
	IC910_widgets[11].W = sldrMICGAIN;
	IC910_widgets[12].W = sldrPOWER;
}

RIG_IC910H::RIG_IC910H() {
	defaultCIV = 0x60;
	name_ = IC910Hname_;
	modes_ = IC910Hmodes_;
	_mode_type = IC910H_mode_type;
	bandwidths_ = IC910H_widths;
	bw_vals_ = IC910H_bw_val;

	widgets = IC910_widgets;

	def_freq = 1296070000L;
	def_mode = 1;

	has_notch_control =
	has_tune_control =
	has_alc_control =
	has_bandwidth_control = false;

	has_smeter =
	has_extras =
	has_vox_onoff =
	has_vox_gain =
	has_vox_anti =
	has_vox_hang =
	has_compON =
	has_compression =
	has_ptt_control =
	has_power_control =
	has_volume_control =
	has_mode_control =
	has_micgain_control =
	has_attenuator_control =
	has_preamp_control =
	has_ifshift_control =
	has_pbt_controls =
	has_swr_control =
	has_noise_control =
	has_noise_reduction =
	has_noise_reduction_control =
	has_rf_control =
	has_sql_control =
	restore_mbw = true;

	comp_is_on = !progStatus.compON;
	adjustCIV(defaultCIV);

	precision = 10;
	ndigits = 9;

};

// this looks like trouble
void RIG_IC910H::set_vfoA (long freq)
{
	long nufreq;
	if (freq > 1300000000L) nufreq = 1300000000L;
	else if (freq > 450000000L && freq < 1240000000L && freqA <= 450000000L)
		nufreq = 1240000000L;
	else if (freq > 450000000L && freq < 1240000000L && freqA >= 1240000000L)
		nufreq = 450000000L;
	else if (freq > 148000000L && freq < 430000000L && freqA <= 148000000L)
		nufreq = 430000000L;
	else if (freq > 148000000L && freq < 430000000L && freqA >= 430000000L)
		nufreq = 148000000L;
	else if (freq < 144000000L) nufreq = 144000000L;
	else nufreq = freq;
	freqA = nufreq;

	if (freqA != freq) {
		vfoA.freq = freqA;
		setFreqDispA((void*)0);
	}
	A.freq = freqA;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freqA, 10 ) );
	cmd.append( post );
	waitFB("set vfo A");}


void RIG_IC910H::set_compression(int on, int val)
{
	if (on) {
		cmd = pre_to;
		cmd.append("\x14\x0E");
		cmd.append(to_bcd(val * 255 / 100, 3));
		cmd.append( post );
		waitFB("set comp");
		if (comp_is_on != on) {
			comp_is_on = on;
			cmd = pre_to;
			cmd.append("\x16\01");
			cmd.append( post );
			waitFB("set comp val");
		}
	} else if (comp_is_on != on) {
		comp_is_on = on;
		cmd = pre_to;
		cmd.append("\x16\00");
		cmd.append( post );
		waitFB("set comp");
	}
}

void RIG_IC910H::set_vox_onoff()
{
	cmd = pre_to;
	cmd.append("\x16\x46");
	cmd += progStatus.vox_onoff ? 1 : 0;
	cmd.append(post);
	waitFB("set vox");
}

void RIG_IC910H::set_vox_gain()
{
	cmd = pre_to;
	cmd.append("\x1A\x02");
	cmd.append(to_bcd(progStatus.vox_gain * 255 / 100, 3));
	cmd.append(post);
	waitFB("set vox gain");
}

void RIG_IC910H::set_vox_anti()
{
	cmd = pre_to;
	cmd.append("\x1A\x04");
	cmd.append(to_bcd(progStatus.vox_anti * 255 / 100, 3));
	cmd.append(post);
	waitFB("set antivox");
}

void RIG_IC910H::set_vox_hang()
{
	cmd = pre_to;
	cmd.append("\x1A\x03");
	cmd.append(to_bcd(progStatus.vox_hang * 255 / 100, 3));
	cmd.append(post);
	waitFB("set vox hang");
}

int RIG_IC910H::get_smeter()
{
	string cstr = "\x16\x02";
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
			mtr = (int)(mtr /2.55);
			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

void RIG_IC910H::set_pbt_inner(int val)
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

void RIG_IC910H::set_pbt_outer(int val)
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

int RIG_IC910H::get_pbt_inner()
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

int RIG_IC910H::get_pbt_outer()
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

