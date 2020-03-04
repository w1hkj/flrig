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

#include "IC7600.h"

bool IC7600_DEBUG = true;

//=============================================================================
// IC-7600

const char IC7600name_[] = "IC-7600";

const char *IC7600modes_[] = {
	"LSB", "USB", "AM", "CW", "RTTY", "FM", "CW-R", "RTTY-R", "PSK", "PSK-R", 
	"LSB-D1", "LSB-D2", "LSB-D3",
	"USB-D1", "USB-D2", "USB-D3", NULL};

const char IC7600_mode_type[] = {
	'L', 'U', 'U', 'U', 'L', 'U', 'L', 'U', 'U', 'L', 
	'L', 'L', 'L',
	'U', 'U', 'U' };

const char IC7600_mode_nbr[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x07, 0x08, 0x12, 0x13, 
	0x00, 0x00, 0x00,
	0x01, 0x01, 0x01 };

const char *IC7600_ssb_bws[] = {
"50",    "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700", "2800", "2900", "3000", "3100", "3200", "3300", "3400", "3500",
"3600", NULL };
static int IC7600_bw_vals_SSB[] = {
 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,
30,31,32,33,34,35,36,37,38,39,
40, WVALS_LIMIT};

const char *IC7600_rtty_bws[] = {
"50",    "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700", NULL };
static int IC7600_bw_vals_RTTY[] = {
 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,
30,31, WVALS_LIMIT};

const char *IC7600_am_bws[] = {
"200",   "400",  "600",  "800", "1000", "1200", "1400", "1600", "1800", "2000",
"2200", "2400", "2600", "2800", "3000", "3200", "3400", "3600", "3800", "4000",
"4200", "4400", "4600", "4800", "5000", "5200", "5400", "5600", "5800", "6000",
"6200", "6400", "6600", "6800", "7000", "7600", "7400", "7600", "7800", "8000",
"8200", "8400", "8600", "8800", "9000", "9200", "9400", "9600", "9800", "10000", NULL };
static int IC7600_bw_vals_AM[] = {
 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,
30,31,32,33,34,35,36,37,38,39,
40,41,42,43,44,45,46,47,48,49
WVALS_LIMIT};

const char *IC7600_fm_bws[] = { "FIXED", NULL };
static int IC7600_bw_vals_FM[] = { 1, WVALS_LIMIT};

static GUI IC7600_widgets[]= {
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

void RIG_IC7600::initialize()
{
	IC7600_widgets[0].W = btnVol;
	IC7600_widgets[1].W = sldrVOLUME;
	IC7600_widgets[2].W = btnAGC;
	IC7600_widgets[3].W = sldrRFGAIN;
	IC7600_widgets[4].W = sldrSQUELCH;
	IC7600_widgets[5].W = btnNR;
	IC7600_widgets[6].W = sldrNR;
	IC7600_widgets[7].W = btnLOCK;
	IC7600_widgets[8].W = sldrINNER;
	IC7600_widgets[9].W = btnCLRPBT;
	IC7600_widgets[10].W = sldrOUTER;
	IC7600_widgets[11].W = btnNotch;
	IC7600_widgets[12].W = sldrNOTCH;
	IC7600_widgets[13].W = sldrMICGAIN;
	IC7600_widgets[14].W = sldrPOWER;

	btn_icom_select_11->deactivate();
	btn_icom_select_12->deactivate();
	btn_icom_select_13->deactivate();

	choice_rTONE->activate();
	choice_tTONE->activate();
}

RIG_IC7600::RIG_IC7600() {
	defaultCIV = 0x7A;
	adjustCIV(defaultCIV);

	name_ = IC7600name_;
	modes_ = IC7600modes_;
	bandwidths_ = IC7600_ssb_bws;
	bw_vals_ = IC7600_bw_vals_SSB;

	_mode_type = IC7600_mode_type;

	widgets = IC7600_widgets;

	def_freq = A.freq = 14070000;
	def_mode = A.imode = 13;
	def_bw = A.iBW = 34;

	B.freq = 7070000;
	B.imode = 13;
	B.iBW = 34;

	has_extras = true;

	has_cw_wpm = true;
	has_cw_spot_tone = true;
	has_cw_qsk = true;

	has_vox_onoff = true;
	has_vox_gain = true;
	has_vox_anti = true;
	has_vox_hang = true;

	has_compON = true;
	has_compression = true;

	has_split = true;
	has_split_AB = true;

	has_micgain_control = true;
	has_bandwidth_control = true;

	has_smeter = true;

	has_power_out =
	has_swr_control =
	has_alc_control =
	has_sql_control = true;

	has_power_control = true;
	has_volume_control = true;
	has_mode_control = true;

	has_attenuator_control = true;
	has_preamp_control = true;

	has_noise_control = true;
	has_noise_reduction = true;
	has_noise_reduction_control = true;

	has_auto_notch = true;
	has_notch_control = true;

	has_pbt_controls = true;
	has_FILTER = true;

	has_rf_control = true;

	has_ptt_control = true;
	has_tune_control = true;

	ICOMmainsub = true;

	has_band_selection = true;

	precision = 1;
	ndigits = 8;
	filA = filB = 1;

};

//======================================================================
// IC7600 unique commands
//======================================================================

static inline void minmax(int min, int max, int &val)
{
	if (val > max) val = max;
	if (val < min) val = min;
}

void RIG_IC7600::selectA()
{
	cmd.assign(pre_to).append("\x07\xD0").append(post);
	set_trace(2, "selectA()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("select A");
}

void RIG_IC7600::selectB()
{
	cmd.assign(pre_to).append("\x07\xD1").append(post);
	set_trace(2, "selectB()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("select B");
}

bool RIG_IC7600::check ()
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

long RIG_IC7600::get_vfoA ()
{
	if (useB) return A.freq;
	string resp;
	cmd.assign(pre_to).append("\x03").append( post );
	if (waitFOR(11, "get vfo A")) {
		resp.assign(pre_fm).append("\x03");
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			A.freq = fm_bcd_be(replystr.substr(p+5), 10);
	}
	get_trace(2, "get_vfoA()", str2hex(replystr.c_str(), replystr.length()));
	return A.freq;
}

void RIG_IC7600::set_vfoA (long freq)
{
	A.freq = freq;
	cmd.assign(pre_to).append("\x05");
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	set_trace(2, "set_vfoA()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("set vfo A");
}

long RIG_IC7600::get_vfoB ()
{
	if (!useB) return B.freq;
	string resp = pre_fm;
	cmd.assign(pre_to).append("\x03").append(post);
	if (waitFOR(11, "get vfo B")) {
		resp.assign(pre_fm).append("\x03");
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			B.freq = fm_bcd_be(replystr.substr(p+5), 10);
	}
	get_trace(2, "get_vfoB()", str2hex(replystr.c_str(), replystr.length()));
	return B.freq;
}

void RIG_IC7600::set_vfoB (long freq)
{
	B.freq = freq;
	cmd.assign(pre_to).append("\x05");
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	set_trace(2, "set_vfoB()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("set vfo B");
}

bool RIG_IC7600::can_split()
{
	return true;
}

void RIG_IC7600::set_split(bool val)
{
	split = val;
	cmd = pre_to;
	cmd += 0x0F;
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	set_trace(2, "set_split()", str2hex(cmd.c_str(), cmd.length()));
	waitFB(val ? "set split ON" : "set split OFF");
}

int RIG_IC7600::get_split()
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
	get_trace(2, "get_split()", str2hex(replystr.c_str(), replystr.length()));
	return split;
}

void RIG_IC7600::set_modeA(int val)
{
	A.imode = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += IC7600_mode_nbr[val];
	cmd += filA;
	cmd.append( post );
	waitFB("set modeA");
	set_trace(4, "set mode A[", IC7600modes_[A.imode], "] ", str2hex(replystr.c_str(), replystr.length()));

// digital set / clear
	if (val >= 10) {
		cmd = pre_to;
		cmd.append("\x1A\x06");
		switch (val) {
			case 10 : case 13 : cmd.append("\x01\x01"); break;
			case 11 : case 14 : cmd.append("\x02\x01"); break;
			case 12 : case 15 : cmd.append("\x03\x01"); break;
		}
		cmd.append( post);
		waitFB("set digital mode ON/OFF");
		set_trace(2, "set_data_modeA()", str2hex(replystr.c_str(), replystr.length()));
	}
}

static const char *szfilter[] = {"1", "2", "3"};

int RIG_IC7600::get_modeA()
{
	int md = 0;
	string resp;
	size_t p;
	cmd.assign(pre_to).append("\x04").append(post);
	if (waitFOR(8, "get mode A")) {
		resp.assign(pre_fm).append("\x04");
		p = replystr.rfind(resp);
		if (p == string::npos) return A.imode;
		for (md = 0; md < 10; md++) {
			if (replystr[p+5] == IC7600_mode_nbr[md]) {
				A.imode = md;
			}
		}
		filA = replystr[p+6];
		get_trace(2, "get_modeA()", str2hex(replystr.c_str(), replystr.length()));
		if (A.imode < 2) {
			cmd.assign(pre_to).append("\x1A\x06").append(post);
			if (waitFOR(9, "data mode?")) {
				resp.assign(pre_fm).append("\x1A\x06");
				p = replystr.rfind(resp);
				if (p == string::npos) return A.imode;
				int dmode = replystr[p+6];
				if(dmode != 0) {
					if (A.imode == 0) A.imode = 9 + dmode;
					else if (A.imode == 1) A.imode = 12 + dmode;
				}
				get_trace(2, "get_data_modeA()", str2hex(replystr.c_str(), replystr.length()));
			}
		}
	}
	if (A.imode > 15) A.imode = 0;
	return A.imode;
}

void RIG_IC7600::set_modeB(int val)
{
	B.imode = val;
	cmd.assign(pre_to).append("\x06");
	cmd += IC7600_mode_nbr[val];
	cmd += filB;
	cmd.append( post );
	waitFB("set modeB");
	set_trace(4, "set mode B[", IC7600modes_[A.imode], "] ", str2hex(replystr.c_str(), replystr.length()));

// digital set / clear
	if (val >= 10) {
		cmd = pre_to;
		cmd.append("\x1A\x06");
		switch (val) {
			case 10 : case 13 : cmd.append("\x01\x01"); break;
			case 11 : case 14 : cmd.append("\x02\x01"); break;
			case 12 : case 15 : cmd.append("\x03\x01"); break;
		}
		cmd.append( post);
		waitFB("set digital mode ON/OFF");
		set_trace(2, "set_data_modeB()", str2hex(replystr.c_str(), replystr.length()));
	}
}

int RIG_IC7600::get_modeB()
{
	int md = 0;
	string resp;
	size_t p;
	cmd.assign(pre_to).append("\x04").append(post);
	if (waitFOR(8, "get mode B")) {
		resp.assign(pre_fm).append("\x04");
		p = replystr.rfind(resp);
		if (p == string::npos) return B.imode;
		for (md = 0; md < 10; md++) if (replystr[p+5] == IC7600_mode_nbr[md]) break;
		if (md == 10) md = 0;
		B.imode = md;
		filB = replystr[p+6];
		get_trace(2, "get_modeB()", str2hex(replystr.c_str(), replystr.length()));

		if (B.imode < 2) {
			cmd.assign(pre_to).append("\x1A\x06").append(post);
			if (waitFOR(9, "data mode?")) {
				resp.assign(pre_fm).append("\x1A\x06");
				p = replystr.rfind(resp);
				if (p == string::npos) return B.imode;
				int dmode = replystr[p+6];
				if(dmode != 0) {
					if (B.imode == 0) B.imode = 9 + dmode;
					else if (B.imode == 1) B.imode = 12 + dmode;
				}
				get_trace(2, "get_data_modeB()", str2hex(replystr.c_str(), replystr.length()));
			}
		}
	}
	if (B.imode > 15) B.imode = 0;
	return B.imode;
}

int RIG_IC7600::get_bwA()
{
	if (A.imode == 5) return 0;
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(post);
	if (waitFOR(8, "get_bwA")) {
		string resp = pre_fm;
		resp.append("\x1A\x02");
		size_t p = replystr.find(resp);
		if (p != string::npos)
			A.iBW = fm_bcd(replystr.substr(p+6), 2);
	}
	return A.iBW;
}

void RIG_IC7600::set_bwA(int val)
{
	A.iBW = val;
	if (A.imode == 5) return;
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(to_bcd(A.iBW, 2));
	cmd.append(post);
	waitFB("set bwA");
}

int RIG_IC7600::get_bwB()
{
	if (B.imode == 5) return 0;
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(post);
	if (waitFOR(8, "get_bwB")) {
		string resp = pre_fm;
		resp.append("\x1A\x02");
		size_t p = replystr.find(resp);
		if (p != string::npos)
			B.iBW = fm_bcd(replystr.substr(p+6), 2);
	}
	return B.iBW;
}

void RIG_IC7600::set_bwB(int val)
{
	B.iBW = val;
	if (B.imode == 5) return;
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(to_bcd(A.iBW, 2));
	cmd.append(post);
	waitFB("set bwB");
}

int RIG_IC7600::adjust_bandwidth(int m)
{
	int bw = 0;
	switch (m) {
		case 2: // AM
			bandwidths_ = IC7600_am_bws;
			bw_vals_ = IC7600_bw_vals_AM;
			bw = 19;
			break;
		case 5: // FM
			bandwidths_ = IC7600_fm_bws;
			bw_vals_ = IC7600_bw_vals_FM;
			bw = 0;
			break;
		case 4: case 7: // RTTY
			bandwidths_ = IC7600_rtty_bws;
			bw_vals_ = IC7600_bw_vals_RTTY;
			bw = 12;
			break;
		case 3: case 6: // CW
			bandwidths_ = IC7600_ssb_bws;
			bw_vals_ = IC7600_bw_vals_SSB;
			bw = 12;
			break;
		case 8: case 9: // PKT
			bandwidths_ = IC7600_ssb_bws;
			bw_vals_ = IC7600_bw_vals_SSB;
			bw = 34;
			break;
		case 0: case 1: // SSB
		case 10: case 11 : case 12 :
		case 13: case 14 : case 15 :
		default:
			bandwidths_ = IC7600_ssb_bws;
			bw_vals_ = IC7600_bw_vals_SSB;
			bw = 34;
	}
	return bw;
}

const char ** RIG_IC7600::bwtable(int m)
{
	const char **table;
	switch (m) {
		case 2: // AM
			table = IC7600_am_bws;
			break;
		case 5: // FM
			table = IC7600_fm_bws;
			break;
		case 4: case 7: // RTTY
			table = IC7600_rtty_bws;
			break;
		case 3: case 6: // CW
		case 8: case 9: // PKT
		case 0: case 1: // SSB
		case 10: case 11 : case 12 :
		case 13: case 14 : case 15 :
		default:
			table = IC7600_ssb_bws;
	}
	return table;
}

int RIG_IC7600::def_bandwidth(int m)
{
	int bw = 0;
	switch (m) {
		case 2: // AM
			bw = 19;
			break;
		case 5: // FM
			bw = 0;
			break;
		case 4: case 7: // RTTY
			bw = 12;
			break;
		case 3: case 6: // CW
			bw = 12;
			break;
		case 8: case 9: // PKT
			bw = 34;
			break;
		case 0: case 1: // SSB
		case 10: case 11 : case 12 :
		case 13: case 14 : case 15 :
		default:
			bw = 34;
	}
	return bw;
}

int RIG_IC7600::get_mic_gain()
{
	int val = 0;
	string cstr = "\x14\x0B";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get mic")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			val = (int)ceil(fm_bcd(replystr.substr(p+6),3) / 2.55);
	}
	minmax(0,100,val);
	get_trace(2, "get_mic_gain()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

void RIG_IC7600::set_mic_gain(int v)
{
	int ICvol = (int)(v * 255 / 100);
	minmax(0, 255, ICvol);
	if (!progStatus.USBaudio) {
		cmd = pre_to;
		cmd.append("\x14\x0B");
		cmd.append(to_bcd(ICvol, 3));
		cmd.append( post );
	} else {
		cmd = pre_to;
		cmd += '\x1A'; cmd += '\x05';
		cmd += '\x00'; cmd += '\x29';
		cmd.append(to_bcd(ICvol, 3));
		cmd.append( post );
	}
	waitFB("set mic gain");
	set_trace(2, "set_mic_gain()", str2hex(cmd.c_str(), cmd.length()));
}

void RIG_IC7600::get_mic_gain_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}


// alh added ++++++++++++++++++++++++++++

void RIG_IC7600::set_compression(int on, int val)
{
	if (on) {
		cmd.assign(pre_to).append("\x14\x0E");
		cmd.append(to_bcd(val * 255 / 100, 3));
		cmd.append( post );
		waitFB("set comp");

		cmd = pre_to;
		cmd.append("\x16\x44");
		cmd += '\x01';
		cmd.append(post);
		waitFB("set Comp ON");

	} else{
		cmd.assign(pre_to).append("\x16\x44");
		cmd += '\x00';
		cmd.append(post);
		waitFB("set Comp OFF");
	}
}

void RIG_IC7600::set_vox_onoff()
{
	if (progStatus.vox_onoff) {
		cmd.assign(pre_to).append("\x16\x46\x01");
		cmd.append( post );
		waitFB("set vox ON");
	} else {
		cmd.assign(pre_to).append("\x16\x46");
		cmd += '\x00';		// ALH
		cmd.append( post );
		waitFB("set vox OFF");
	}
}

void RIG_IC7600::set_vox_gain()
{
	cmd.assign(pre_to).append("\x1A\x05"); // ALH values 0-255
	cmd +='\x01';
	cmd +='\x65';
	cmd.append(to_bcd((int)(progStatus.vox_gain * 2.55), 3));
	cmd.append( post );
	waitFB("SET vox gain");
}

void RIG_IC7600::set_vox_anti()
{
	cmd.assign(pre_to).append("\x1A\x05");	//ALH values 0-255
	cmd +='\x01';
	cmd +='\x66';
	cmd.append(to_bcd((int)(progStatus.vox_anti * 2.55), 3));
	cmd.append( post );
	waitFB("SET anti-vox");
}

void RIG_IC7600::set_vox_hang()
{
	cmd.assign(pre_to).append("\x1A\x05");	//ALH values 00-20 = 0.0 - 2.0 sec
	cmd +='\x01';	// ALH
	cmd +='\x67';	// ALH
	cmd.append(to_bcd((int)(progStatus.vox_hang / 10 ), 2));
	cmd.append( post );
	waitFB("SET vox hang");
}

// CW controls

void RIG_IC7600::set_cw_wpm()
{
	cmd.assign(pre_to).append("\x14\x0C"); // values 0-255
	cmd.append(to_bcd(round((progStatus.cw_wpm - 6) * 255 / (60 - 6)), 3));
	cmd.append( post );
	waitFB("SET cw wpm");
}

void RIG_IC7600::set_cw_qsk()
{
	int n = round(progStatus.cw_qsk * 10); // values 0-255
	cmd.assign(pre_to).append("\x14\x0F");
	cmd.append(to_bcd(n, 3));
	cmd.append(post);
	waitFB("Set cw qsk delay");
}

void RIG_IC7600::set_cw_spot_tone()
{
	cmd.assign(pre_to).append("\x14\x09"); // values 0=300Hz 255=900Hz
	int n = round((progStatus.cw_spot_tone - 300) * 255.0 / 600.0);
	if (n > 255) n = 255;
	if (n < 0) n = 0;
	cmd.append(to_bcd(n, 3));
	cmd.append( post );
	waitFB("SET cw spot tone");
}

void RIG_IC7600::set_cw_vol()
{
	cmd.assign(pre_to);
	cmd.append("\x1A\x05");
	cmd += '\x00';
	cmd += '\x24';	// ALH / DF
	cmd.append(to_bcd((int)(progStatus.cw_vol * 2.55), 3));
	cmd.append( post );
	waitFB("SET cw sidetone volume");
}

// Tranceiver PTT on/off
void RIG_IC7600::set_PTT_control(int val)
{
	cmd = pre_to;
	cmd += '\x1c';
	cmd += '\x00';
	cmd += (unsigned char) val;
	cmd.append( post );
	waitFB("set ptt");
	set_trace(2, "set_PTT()", str2hex(cmd.c_str(), cmd.length()));
	ptt_ = val;
}

int RIG_IC7600::get_PTT()
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
void RIG_IC7600::set_volume_control(int val)
{
	int ICvol = (int)(val * 255 / 100);
	minmax(0, 255, ICvol);
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append(to_bcd(ICvol, 3));
	cmd.append( post );
	waitFB("set vol");
}

/*

I:12:20:22: get vol ans in 0 ms, OK  
cmd FE FE 7A E0 14 01 FD
ans FE FE 7A E0 14 01 FD 
FE FE E0 7A 14 01 00 65 FD
 0  1  2  3  4  5  6  7  8
*/
int RIG_IC7600::get_volume_control()
{
	int val = 0;
	string cstr = "\x14\x01";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(9, "get vol")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			val = (int)ceil(fm_bcd(replystr.substr(p + 6),3) * 100 / 255);
	}
	minmax(0, 100, val);
	progStatus.volume = val;
	return (progStatus.volume);
}

void RIG_IC7600::get_vol_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

void RIG_IC7600::set_power_control(double value)
{
	int val = (int)(value * 255 / 100);
	minmax(0, 255, val);
	cmd = pre_to;
	cmd.append("\x14\x0A");
	cmd.append(to_bcd(val, 3));
	cmd.append( post );
	waitFB("set power");
}

int RIG_IC7600::get_power_control()
{
	int val = progStatus.power_level;
	string cstr = "\x14\x0A";
	string resp = pre_fm;
	cmd = pre_to;
	cmd.append(cstr).append(post);
	resp.append(cstr);
	if (waitFOR(9, "get power")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			val = (int)ceil(fm_bcd(replystr.substr(p + 6),3) * 100 / 255);
	}
	minmax(0, 100, val);
	progStatus.power_level = val;
	return (progStatus.power_level);
}

void RIG_IC7600::get_pc_min_max_step(double &min, double &max, double &step)
{
	min = 2; max = 100; step = 1;
}

int RIG_IC7600::get_smeter()
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
			mtr = (int)ceil(mtr /2.41);
			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

int RIG_IC7600::get_power_out(void) 
{
	string cstr = "\x15\x11";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;
	if (waitFOR(9, "get power out")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			mtr = (int)ceil(mtr /2.13);
			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

int RIG_IC7600::get_swr(void) 
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
	return mtr;
}

int RIG_IC7600::get_alc(void)
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
	return mtr;
}

void RIG_IC7600::set_rf_gain(int val)
{
	int ICrfg = (int)(val * 255 / 100);
	minmax(0, 255, ICrfg);
	cmd = pre_to;
	cmd.append("\x14\x02");
	cmd.append(to_bcd(ICrfg, 3));
	cmd.append( post );
	waitFB("set RF");
}

int RIG_IC7600::get_rf_gain()
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
			val = (int)(fm_bcd(replystr.substr(p + 6),3) * 100 / 255);
	}
	minmax(0, 100, val);
	progStatus.rfgain = val;
	return (progStatus.rfgain);
}

void RIG_IC7600::get_rf_min_max_step(double &min, double &max, double &step)
{
	min = 0; max = 100; step = 1;
}

int RIG_IC7600::next_preamp()
{
	switch (preamp_level) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 0;
	}
	return 0;
}

void RIG_IC7600::set_preamp(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';

	if (preamp_level == 1) {
		preamp_label("Amp 1", true);
		cmd += '\x01';
	} else if (preamp_level == 2) {
		preamp_label("Amp 2", true);
		cmd += '\x02';
	} else if (preamp_level == 0) {
		preamp_label("OFF", false);
		cmd += '\x00';
	}

	cmd.append( post );
	set_trace(2, "set_preamp()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("set Pre");
}

int RIG_IC7600::get_preamp()
{
	preamp_level = progStatus.preamp;
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
			preamp_level = replystr[p+6];
			if (preamp_level == 1) {
				preamp_label("Amp 1", true);
			} else if (preamp_level == 2) {
				preamp_label("Amp 2", true);
			} else {
				preamp_label("OFF", false);
				preamp_level = 0;
			}
		}
	}
	return preamp_level;
}

int  RIG_IC7600::next_attenuator()
{
	switch (atten_level) {
		case 0x00: return 0x06;
		case 0x06: return 0x12;
		case 0x12: return 0x18;
		case 0x18: return 0x00;
	}
	return 0;
}

void RIG_IC7600::set_attenuator(int val)
{
	atten_level = val;
	if (atten_level == 0x06) {
		atten_label("6 dB", true);
	} else if (atten_level == 0x12) {
		atten_label("12 dB", true);
	} else if (atten_level == 0x18) {
		atten_label("18 dB", true);
	} else if (atten_level == 0x00) {
		atten_label("ATT", false);
	}
	cmd = pre_to;
	cmd += '\x11';
	cmd += atten_level;
	cmd.append( post );
	set_trace(2, "set_attenuator()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("set att");
}

int RIG_IC7600::get_attenuator()
{
	cmd = pre_to;
	string resp = pre_fm;
	cmd += '\x11';
	resp += '\x11';
	cmd.append( post );

	if (waitFOR(7, "get ATT")) {
		get_trace(2, "get_ATT()", str2hex(replystr.c_str(), replystr.length()));
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			atten_level = replystr[p+5];
		if (atten_level == 0x06) {
			atten_label("6 dB", true);
		} else if (atten_level == 0x12) {
			atten_label("12 dB", true);
		} else if (atten_level == 0x18) {
			atten_label("18 dB", true);
		} else {
			atten_level = 0x00;
			atten_label("ATT", false);
		}
	}
	return atten_level;
}


void RIG_IC7600::set_noise(bool val)
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set noise");
}


/*

I:12:06:50: get noise ans in 0 ms, OK  
cmd FE FE 7A E0 16 22 FD
ans FE FE 7A E0 16 22 FD FE FE E0 7A 16 22 00 FD

*/

int RIG_IC7600::get_noise()
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

void RIG_IC7600::set_noise_reduction(int val)
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set NR");
}

int RIG_IC7600::get_noise_reduction()
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

/*

I:12:06:50: get NR ans in 0 ms, OK  
cmd FE FE 7A E0 16 40 FD
ans FE FE 7A E0 16 40 FD
FE FE E0 7A 16 40 01 FD
 0  1  2  3  4  5  6  7

I:12:06:50: get NRval ans in 0 ms, OK  
cmd FE FE 7A E0 14 06 FD
ans FE FE 7A E0 14 06 FD
FE FE E0 7A 14 06 00 24 FD
 0  1  2  3  4  5  6  7  8

*/

void RIG_IC7600::set_noise_reduction_val(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x06");
	cmd.append(to_bcd(val * 255 / 100, 3));
	cmd.append(post);
	waitFB("set NRval");
}

int RIG_IC7600::get_noise_reduction_val()
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
		if (p != string::npos)
			val = (int)ceil(fm_bcd(replystr.substr(p+6),3) * 100 / 255);
	}
	minmax(0, 100, val);
	return val;
}

void RIG_IC7600::set_squelch(int val)
{
	int ICsql = (int)(val * 255 / 100);
	minmax(0, 255, ICsql);
	cmd = pre_to;
	cmd.append("\x14\x03");
	cmd.append(to_bcd(ICsql, 3));
	cmd.append( post );
	waitFB("set Sqlch");
}

int  RIG_IC7600::get_squelch()
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
			val = (int)ceil(fm_bcd(replystr.substr(p+6), 3) * 100 / 255);
	}
	minmax(0, 100, val);
	progStatus.squelch = val;
	return (progStatus.squelch);
}

void RIG_IC7600::set_auto_notch(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x41';
	cmd += (unsigned char)val;
	cmd.append( post );
	waitFB("set AN");
}

int RIG_IC7600::get_auto_notch()
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

static bool IC7600_notchon = false;

void RIG_IC7600::set_notch(bool on, int val)
{
	int notch = val / 20 + 53;
	minmax(0, 255, notch);
	if (on != IC7600_notchon) {
		cmd = pre_to;
		cmd.append("\x16\x48");
		cmd += on ? '\x01' : '\x00';
		cmd.append(post);
		waitFB("set notch");
		IC7600_notchon = on;
	}

	if (on) {
		cmd = pre_to;
		cmd.append("\x14\x0D");
		cmd.append(to_bcd(notch,3));
		cmd.append(post);
		waitFB("set notch val");
	}
}

bool RIG_IC7600::get_notch(int &val)
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
	return (IC7600_notchon = on);
}

void RIG_IC7600::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 4040;
	step = 20;
}

void RIG_IC7600::set_pbt_inner(int val)
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

void RIG_IC7600::set_pbt_outer(int val)
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

int RIG_IC7600::get_pbt_inner()
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

int RIG_IC7600::get_pbt_outer()
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

const char *RIG_IC7600::FILT(int &val)
{
	if (useB) {
		if (filB < 0) filB = 0;
		if (filB > 3) filB = 3;
		val = filB;
		return(szfilter[filB - 1]);
	}
	else {
		if (filA < 0) filA = 0;
		if (filA > 3) filA = 3;
		val = filA;
		return (szfilter[filA - 1]);
	}
}

const char *RIG_IC7600::nextFILT()
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

void RIG_IC7600::get_band_selection(int v)
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
			int tone = fm_bcd(replystr.substr(p+16, 3), 6);
			size_t index = 0;
			for (index = 0; index < sizeof(PL_tones) / sizeof(*PL_tones); index++)
				if (tone == PL_tones[index]) break;
			tTONE = index;
			tone = fm_bcd(replystr.substr(p+19, 3), 6);
			for (index = 0; index < sizeof(PL_tones) / sizeof(*PL_tones); index++)
				if (tone == PL_tones[index]) break;
			rTONE = index;
			if ((bandmode == 0) && banddata) 
				bandmode = ((banddata == 0x10) ? 10 : 
							(banddata == 0x20) ? 11 :
							(banddata == 0x30) ? 12 : 0);
			if ((bandmode == 1) && banddata)
				bandmode = ((banddata == 0x10) ? 13 : 
							(banddata == 0x20) ? 14 :
							(banddata == 0x30) ? 15 : 0);
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

void RIG_IC7600::set_band_selection(int v)
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
	if (mode >= 9)
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

