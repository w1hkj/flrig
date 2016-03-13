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

RIG_IC7600::RIG_IC7600() {
	defaultCIV = 0x7A;
	adjustCIV(defaultCIV);

	name_ = IC7600name_;
	modes_ = IC7600modes_;
	bandwidths_ = IC7600_ssb_bws;
	bw_vals_ = IC7600_bw_vals_SSB;

	_mode_type = IC7600_mode_type;

	widgets = IC7600_widgets;

	A.freq = 14070000;
	A.imode = 13;
	A.iBW = 34;
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

	has_rf_control = true;

	has_ptt_control = true;
	has_tune_control = true;

	precision = 1;
	ndigits = 8;

};

//======================================================================
// IC7600 unique commands
//======================================================================

void RIG_IC7600::initialize()
{
	IC7600_widgets[0].W = btnVol;
	IC7600_widgets[1].W = sldrVOLUME;
	IC7600_widgets[2].W = sldrRFGAIN;
	IC7600_widgets[3].W = sldrSQUELCH;
	IC7600_widgets[4].W = btnNR;
	IC7600_widgets[5].W = sldrNR;
	IC7600_widgets[6].W = btnIFsh;
	IC7600_widgets[7].W = sldrIFSHIFT;
	IC7600_widgets[8].W = btnNotch;
	IC7600_widgets[9].W = sldrNOTCH;
	IC7600_widgets[10].W = sldrMICGAIN;
	IC7600_widgets[11].W = sldrPOWER;
}

static inline void minmax(int min, int max, int &val)
{
	if (val > max) val = max;
	if (val < min) val = min;
}

void RIG_IC7600::selectA()
{
	cmd.assign(pre_to).append("\x07\xD0").append(post);
	waitFB("select A");
}

void RIG_IC7600::selectB()
{
	cmd.assign(pre_to).append("\x07\xD1").append(post);
	waitFB("select B");
}

long RIG_IC7600::get_vfoA ()
{
	string resp;
	cmd.assign(pre_to).append("\x03").append( post );
	if (waitFOR(11, "get vfo A")) {
		resp.assign(pre_fm).append("\x03");
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			A.freq = fm_bcd_be(&replystr[p+5], 10);
	}
	return A.freq;
}

void RIG_IC7600::set_vfoA (long freq)
{
	A.freq = freq;
	cmd.assign(pre_to).append("\x05");
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	waitFB("set vfo A");
}

long RIG_IC7600::get_vfoB ()
{
	string resp = pre_fm;
	cmd.assign(pre_to).append("\x03").append(post);
	if (waitFOR(11, "get vfo B")) {
		resp.assign(pre_fm).append("\x03");
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			B.freq = fm_bcd_be(&replystr[p+5], 10);
	}
	return B.freq;
}

void RIG_IC7600::set_vfoB (long freq)
{
	B.freq = freq;
	cmd.assign(pre_to).append("\x05");
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	waitFB("set vfo B");
}

bool RIG_IC7600::can_split()
{
	return true;
}

void RIG_IC7600::set_split(bool val)
{
	split = val;
	if (val) {
		cmd.assign(pre_to).append("\x0F\x01").append( post );
		waitFB("Split ON");
	} else {
		cmd.assign(pre_to).append("\x0F");
		cmd += '\x00';
		cmd.append( post );
		waitFB("Split OFF");
	}
}

int RIG_IC7600::get_split()
{
	int split = 0;
	cmd.assign(pre_to);
	cmd.append("\x0F");
	cmd.append( post );
	if (waitFOR(7, "get split")) {
		string resp = pre_fm;
		resp.append("\x0F");
		size_t p = replystr.find(resp);
		if (p != string::npos)
			split = replystr[p+5];
	}
	return split;
}

void RIG_IC7600::swapvfos()
{
	cmd = pre_to;
	cmd += 0x07; cmd += 0xB0;
	cmd.append(post);
	if (IC7600_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	waitFB("swap vfos");
}

void RIG_IC7600::set_modeA(int val)
{
	A.imode = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += IC7600_mode_nbr[val];
	cmd.append( post );
	waitFB("set modeA");

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
	}
}

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
		if (A.imode < 2) {
			cmd.assign(pre_to).append("\x1A\x06").append(post);
			if (waitFOR(9, "data mode?")) {
				resp.assign(pre_fm).append("\x1A\x06");
				p = replystr.rfind(resp);
				if (p == string::npos) return A.imode;
				int dmode = replystr[p+6];
				if (A.imode == 0) A.imode = 9 + dmode;
				else if (A.imode == 1) A.imode = 12 + dmode;
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
	cmd.append( post );
	waitFB("set modeB");

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

		if (B.imode < 2) {
			cmd.assign(pre_to).append("\x1A\x06").append(post);
			if (waitFOR(9, "data mode?")) {
				resp.assign(pre_fm).append("\x1A\x06");
				p = replystr.rfind(resp);
				if (p == string::npos) return B.imode;
				int dmode = replystr[p+6];
				if (B.imode == 0) B.imode = 9 + dmode;
				else if (B.imode == 1) B.imode = 12 + dmode;
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
			A.iBW = fm_bcd(&replystr[p+6], 2);
	}
	return A.iBW;
}

void RIG_IC7600::set_bwA(int val)
{
	A.iBW = val;
	if (A.imode == 5) return;
	if (A.imode > bw_size_) A.imode = bw_size_;
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
			B.iBW = fm_bcd(&replystr[p+6], 2);
	}
	return B.iBW;
}

void RIG_IC7600::set_bwB(int val)
{
	B.iBW = val;
	if (B.imode == 5) return;
	if (B.iBW > bw_size_) B.iBW = bw_size_;
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
			bw_size_ = sizeof(IC7600_bw_vals_AM);
			bw = 19;
			break;
		case 5: // FM
			bandwidths_ = IC7600_fm_bws;
			bw_vals_ = IC7600_bw_vals_FM;
			bw_size_ = sizeof(IC7600_bw_vals_FM);
			bw = 0;
			break;
		case 4: case 7: // RTTY
			bandwidths_ = IC7600_rtty_bws;
			bw_vals_ = IC7600_bw_vals_RTTY;
			bw_size_ = sizeof(IC7600_bw_vals_RTTY);
			bw = 12;
			break;
		case 3: case 6: // CW
			bandwidths_ = IC7600_ssb_bws;
			bw_vals_ = IC7600_bw_vals_SSB;
			bw_size_ = sizeof(IC7600_bw_vals_SSB);
			bw = 12;
			break;
		case 8: case 9: // PKT
			bandwidths_ = IC7600_ssb_bws;
			bw_vals_ = IC7600_bw_vals_SSB;
			bw_size_ = sizeof(IC7600_bw_vals_SSB);
			bw = 34;
			break;
		case 0: case 1: // SSB
		case 10: case 11 : case 12 :
		case 13: case 14 : case 15 :
		default:
			bandwidths_ = IC7600_ssb_bws;
			bw_vals_ = IC7600_bw_vals_SSB;
			bw_size_ = sizeof(IC7600_bw_vals_SSB);
			bw = 34;
	}
	return bw;
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
			val = (int)ceil(fm_bcd(&replystr[p+6],3) / 2.55);
	}
	minmax(0,100,val);
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
}

void RIG_IC7600::get_mic_gain_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}


// alh added ++++++++++++++++++++++++++++

void RIG_IC7600::set_compression()
{
	if (progStatus.compON) {
		cmd.assign(pre_to).append("\x14\x0E");
		cmd.append(to_bcd(progStatus.compression * 255 / 100, 3));
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
			val = (int)ceil(fm_bcd(&replystr[p + 6],3) * 100 / 255);
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
			val = (int)ceil(fm_bcd(&replystr[p + 6],3) * 100 / 255);
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
			mtr = fm_bcd(&replystr[p+6], 3);
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
			mtr = fm_bcd(&replystr[p+6], 3);
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
			mtr = fm_bcd(&replystr[p+6], 3);
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
			mtr = fm_bcd(&replystr[p+6], 3);
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
			val = (int)(fm_bcd(&replystr[p + 6],3) * 100 / 255);
	}
	minmax(0, 100, val);
	progStatus.rfgain = val;
	return (progStatus.rfgain);
}

void RIG_IC7600::get_rf_min_max_step(double &min, double &max, double &step)
{
	min = 0; max = 100; step = 1;
}

void RIG_IC7600::tune_rig()
{
	cmd = pre_to;
	cmd.append("\x1c\x01\x02");
	cmd.append( post );
	waitFB("tune rig");
}

void RIG_IC7600::set_preamp(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';

	if (preamp_level == 0) {
		preamp_level = 1;
		preamp_label("Amp 1", true);
	} else if (preamp_level == 1) {
		preamp_level = 2;
		preamp_label("Amp 2", true);
	} else if (preamp_level == 2) {
		preamp_level = 0;
		preamp_label("OFF", false);
	}

	cmd += (unsigned char)preamp_level;
	cmd.append( post );
	waitFB("set Pre");
}

int RIG_IC7600::get_preamp()
{
	string cstr = "\x16\x02";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get Pre")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
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
	return progStatus.preamp = preamp_level;
}

void RIG_IC7600::set_attenuator(int val)
{
	unsigned char level = 0x00;
	if (atten_level == 0x00) {
		atten_level = 0x06;
		atten_label("6 dB", true);
	} else if (atten_level == 0x06) {
		atten_level = 0x12;
		atten_label("12 dB", true);
	} else if (atten_level == 0x12) {
		atten_level = 0x18;
		atten_label("18 dB", true);
	} else if (atten_level == 0x18) {
		atten_level = 0x00;
		atten_label("ATT", false);
	}
	cmd = pre_to;
	cmd += '\x11';
	cmd += atten_level;
	cmd.append( post );
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
			atten_level = 0;
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
			val = (int)ceil(fm_bcd(&replystr[p+6],3) * 100 / 255);
	}
	minmax(0, 100, val);
	progStatus.noise_reduction_val = val;
	return progStatus.noise_reduction_val;
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
			val = (int)ceil(fm_bcd(&replystr[p+6], 3) * 100 / 255);
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
				val = fm_bcd(&replystr[p+6],3);
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

