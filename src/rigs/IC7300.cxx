// ----------------------------------------------------------------------------
// Copyright (C) 2014
//              David Freese, W1HKJ
// Modified: January 2017
//              Andy Stewart, KB1OIQ
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

#include "IC7300.h"
#include "support.h"

#define EMULATE 0 // emulate 7200 operations

//=============================================================================
// IC-7300

const char IC7300name_[] = "IC-7300";

const char *IC7300modes_[] = {
	"LSB", "USB", "AM", "FM", "CW", "CW-R", "RTTY", "RTTY-R",
	"LSB-D", "USB-D", "AM-D", "FM-D", NULL};

const char IC7300_mode_type[] = {
	'L', 'U', 'U', 'U', 'L', 'U', 'L', 'U',
	'L', 'U', 'U', 'U' };

const char IC7300_mode_nbr[] = {
	0x00, 0x01, 0x02, 0x05, 0x03, 0x07, 0x04, 0x08,
	0x00, 0x01, 0x02, 0x05 };

const char *IC7300_ssb_bws[] = {
"50",    "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700", "2800", "2900", "3000", "3100", "3200", "3300", "3400", "3500",
"3600", NULL };
static int IC7300_bw_vals_SSB[] = {
 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,
30,31,32,33,34,35,36,37,38,39,
40, WVALS_LIMIT};

const char *IC7300_rtty_bws[] = {
"50",    "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700", NULL };
static int IC7300_bw_vals_RTTY[] = {
 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,
30,31, WVALS_LIMIT};

const char *IC7300_am_bws[] = {
"200",   "400",  "600",  "800", "1000", "1200", "1400", "1600", "1800", "2000",
"2200", "2400", "2600", "2800", "3000", "3200", "3400", "3600", "3800", "4000",
"4200", "4400", "4600", "4800", "5000", "5200", "5400", "5600", "5800", "6000",
"6200", "6400", "6600", "6800", "7000", "7300", "7400", "7300", "7800", "8000",
"8200", "8400", "8600", "8800", "9000", "9200", "9400", "9600", "9800", "10000", NULL };
static int IC7300_bw_vals_AM[] = {
 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,
30,31,32,33,34,35,36,37,38,39,
40,41,42,43,44,45,46,47,48,49
WVALS_LIMIT};

const char *IC7300_fm_bws[] = { "FIXED", NULL };
static int IC7300_bw_vals_FM[] = { 1, WVALS_LIMIT};

static GUI IC7300_widgets[]= {
	{ (Fl_Widget *)btnVol, 2, 125,  50 },			// 0
	{ (Fl_Widget *)sldrVOLUME, 54, 125, 156 },		// 1
	{ (Fl_Widget *)btnAGC, 2, 145, 50 },			// 2
	{ (Fl_Widget *)sldrRFGAIN, 54, 145, 156 },		// 3
	{ (Fl_Widget *)sldrSQUELCH, 54, 165, 156 },		// 4
	{ (Fl_Widget *)btnNR, 2, 185,  50 },			// 5
	{ (Fl_Widget *)sldrNR, 54, 185, 156 },			// 6

	{ (Fl_Widget *)btnIFsh, 214, 125,  50 },		// 7
	{ (Fl_Widget *)sldrIFSHIFT, 266, 125, 156 },	// 8
	{ (Fl_Widget *)btnNotch, 214, 145,  50 },		// 9
	{ (Fl_Widget *)sldrNOTCH, 266, 145, 156 },		// 10
	{ (Fl_Widget *)sldrMICGAIN, 266, 165, 156 },	// 11
	{ (Fl_Widget *)sldrPOWER, 266, 185, 156 },		// 12

	{ (Fl_Widget *)NULL, 0, 0, 0 }
};

RIG_IC7300::RIG_IC7300() {
	defaultCIV = 0x94;
	adjustCIV(defaultCIV);

	name_ = IC7300name_;
	modes_ = IC7300modes_;
	bandwidths_ = IC7300_ssb_bws;
	bw_vals_ = IC7300_bw_vals_SSB;

	_mode_type = IC7300_mode_type;

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

	widgets = IC7300_widgets;

	A.freq = 14070000;
	A.imode = 9;
	A.iBW = 34;
	B.freq = 7070000;
	B.imode = 9;
	B.iBW = 34;

	has_extras = true;

	has_cw_wpm = true;
	has_cw_spot_tone = true;
	has_cw_qsk = true;
	has_cw_break_in = true;

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

	has_power_out = true;
	has_swr_control = true;
	has_alc_control = true;
	has_sql_control = true;
	has_agc_control = true;

	has_power_control = true;
	has_volume_control = true;
	has_mode_control = true;

	has_attenuator_control = true;
	has_preamp_control = true;

	has_noise_control = true;
	has_nb_level = true;

	has_noise_reduction = true;
	has_noise_reduction_control = true;

	has_auto_notch = true;
	has_notch_control = true;
	has_ifshift_control = true;

	has_rf_control = true;

	has_ptt_control = true;
	has_tune_control = true;

	precision = 1;
	ndigits = 8;

//	has_vfo_adj = true;

#if !EMULATE
	can_change_alt_vfo = true;
#endif
};

void RIG_IC7300::initialize()
{
	IC7300_widgets[0].W = btnVol;
	IC7300_widgets[1].W = sldrVOLUME;
	IC7300_widgets[2].W = btnAGC;
	IC7300_widgets[3].W = sldrRFGAIN;
	IC7300_widgets[4].W = sldrSQUELCH;
	IC7300_widgets[5].W = btnNR;
	IC7300_widgets[6].W = sldrNR;
	IC7300_widgets[7].W = btnIFsh;
	IC7300_widgets[8].W = sldrIFSHIFT;
	IC7300_widgets[9].W = btnNotch;
	IC7300_widgets[10].W = sldrNOTCH;
	IC7300_widgets[11].W = sldrMICGAIN;
	IC7300_widgets[12].W = sldrPOWER;

	selectA(); // for A to be active vfo
}

static inline void minmax(int min, int max, int &val)
{
	if (val > max) val = max;
	if (val < min) val = min;
}

void RIG_IC7300::selectA()
{
	cmd.assign(pre_to).append("\x07");
	cmd += '\x00';
	cmd.append(post);
	waitFB("select A");

	trace(4, "selectA():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC7300::selectB()
{
	cmd.assign(pre_to).append("\x07");
	cmd += '\x01';
	cmd.append(post);
	waitFB("select B");

	trace(4, "selectB():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));
}

////////////////////////////////////////////////////////////////////////
#if EMULATE // change to zero for alternate vfo usage
////////////////////////////////////////////////////////////////////////
long RIG_IC7300::get_vfoA ()
{
	if (useB) return A.freq;
	string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );

	if (waitFOR(11, "get vfo A")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			A.freq = fm_bcd_be(replystr.substr(p+5), 10);
	}

	trace(4, "get_vfoA():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));

	return A.freq;
}

void RIG_IC7300::set_vfoA (long freq)
{
	A.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );

	waitFB("set vfo A");

	trace(4, "set_vfoA():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));
}

long RIG_IC7300::get_vfoB ()
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

	trace(4, "get_vfoB():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));

	return B.freq;
}

void RIG_IC7300::set_vfoB (long freq)
{
	B.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );

	waitFB("set vfo B");

	trace(4, "set_vfoA():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC7300::get_modeA()
{
	int md = A.imode;
	string resp = pre_fm;
	resp += '\x04';
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	if (waitFOR(8, "get mode A")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			md = replystr[p + 5];
			if (md > 6) md -= 2;
		}
	}

	trace(4, "get_modeA():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));

	cmd = pre_to;
	cmd += "\x1A\x04";
	cmd.append(post);
	resp = pre_fm;
	resp += "\x1A\x04";
	if (waitFOR(9, "data mode?")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			if ((replystr[p+6] & 0x01) == 0x01 && A.imode < 4) {
				A.imode += 8;
			if (A.imode > 11)
				A.imode = 1;
			}
		}
	}

	trace(4, "get_data_mode_A():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));

	return (A.imode = md);
}

void RIG_IC7300::set_modeA(int val)
{
	A.imode = val;
	if (val > 7) val -= 8;
	cmd = pre_to;
	cmd += '\x06';
	cmd += val > 4 ? val + 2 : val;
	cmd.append( post );
	waitFB("set mode A");

	trace(4, "set_modeA():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));

	cmd = pre_to;
	cmd += '\x1A'; cmd += '\x04';
	if (A.imode > 7) cmd += '\x01';
	else cmd += '\x00';
	cmd.append( post);
	waitFB("data mode");

	trace(4, "set_data_mode_A():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));

}

int RIG_IC7300::get_modeB()
{
	int md = B.imode;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	string resp = pre_fm;
	resp += '\x04';
	if (waitFOR(8, "get mode B")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			md = replystr[p+5];
			if (md > 6) md -= 2;
		}
	}

	trace(4, "get_modeB():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));

	cmd = pre_to;
	cmd += "\x1A\x04";
	cmd.append(post);
	resp = pre_fm;
	resp += "\x1A\x04";
	if (waitFOR(9, "get data B")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			if ((replystr[p+6] & 0x01) == 0x01 && B.imode < 4) {
				B.imode += 8;
			if (B.imode > 11)
				B.imode = 1;
			}
		}
	}

	trace(4, "get_data_mode_b():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));

	return (B.imode = md);
}

void RIG_IC7300::set_modeB(int val)
{
	B.imode = val;
	if (val > 7) val -= 8;
	cmd = pre_to;
	cmd += '\x06';
	cmd += val > 4 ? val + 2 : val;
	cmd.append( post );
	waitFB("set mode B");

	trace(4, "set_modeB():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));

	cmd = pre_to;
	cmd += '\x1A'; cmd += '\x04';
	if (B.imode > 7) cmd += '\x01';
	else cmd += '\x00';
	cmd.append( post);
	waitFB("data mode");

	trace(4, "set_data_mode_B():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));

}

////////////////////////////////////////////////////////////////////////
#else
////////////////////////////////////////////////////////////////////////

//======================================================================
// IC7300 unique commands
//======================================================================

static int FIL_A = 1;
static int FIL_B = 2;

long RIG_IC7300::get_vfoA ()
{
	string resp;

	cmd.assign(pre_to).append("\x25");
	resp.assign(pre_fm).append("\x25");

	if (useB) {
		cmd  += '\x01';
		resp += '\x01';
	} else {
		cmd  += '\x00';
		resp += '\x00';
	}

	cmd.append(post);
	if (waitFOR(12, "get vfo A")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			A.freq = fm_bcd_be(replystr.substr(p+6), 10);
	}

	trace(4, "get_vfoA():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));

	return A.freq;
}

void RIG_IC7300::set_vfoA (long freq)
{
	A.freq = freq;

	cmd.assign(pre_to).append("\x25");
	if (useB) cmd += '\x01';
	else      cmd += '\x00';

	cmd.append( to_bcd_be( freq, 10) );
	cmd.append( post );
	waitFB("set vfo A");

	trace(4, "set_vfoA():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));

}

long RIG_IC7300::get_vfoB ()
{
	string resp;

	cmd.assign(pre_to).append("\x25");
	resp.assign(pre_fm).append("\x25");

	if (useB) {
		cmd  += '\x00';
		resp += '\x00';
	} else {
		cmd  += '\x01';
		resp += '\x01';
	}

	cmd.append(post);
	if (waitFOR(12, "get vfo B")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			B.freq = fm_bcd_be(replystr.substr(p+6), 10);
	}

	trace(4, "get_vfoB():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));

	return B.freq;
}

void RIG_IC7300::set_vfoB (long freq)
{
	B.freq = freq;

	cmd.assign(pre_to).append("\x25");
	if (useB) cmd += '\x00';
	else      cmd += '\x01';

	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	waitFB("set vfo B");

	trace(4, "set_vfoB():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));
}

// expecting
//  0  1  2  3  4  5  6  7  8  9
// FE FE E0 94 26 NN NN NN NN FD
//                |  |  |  |
//                |  |  |  |__filter setting, 01, 02, 03
//                |  |  |_____data mode, 00 - off, 01 - on
//                |  |________Mode 00 - LSB
//                |                01 - USB
//                |                02 - AM
//                |                03 - CW
//                |                04 - RTTY
//                |                05 - FM
//                |                07 - CW-R
//                |                08 - RTTY-R
//                |___________selected vfo, 00 - active, 01 - inactive

int RIG_IC7300::get_modeA()
{
	int md = 0;
	size_t p;

	string resp;
	cmd.assign(pre_to).append("\x26");
	resp.assign(pre_fm).append("\x26");

	if (useB) {
		cmd += '\x01';
		resp += '\x01';
	} else {
		cmd += '\x00';
		resp += '\x00';
	}
	cmd.append(post);

	if (waitFOR(10, "get mode A")) {
		p = replystr.rfind(resp);
		if (p == string::npos)
			return A.imode;

		for (md = 0; md < 8; md++) {
			if (replystr[p+6] == IC7300_mode_nbr[md]) {
				A.imode = md;
				if (replystr[p+7] == 0x01 && A.imode < 4)
					A.imode += 8;
				if (A.imode > 11)
					A.imode = 1;
				break;
			}
		}
		FIL_A = replystr[p+8];
	}

	trace(4, "get_modeA():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));

	return A.imode;
}

// LSB  USB  AM   CW  RTTY  FM  CW-R  RTTY-R  LSB-D  USB-D
//  0    1    2    3   4     5    6     7      8      9

void RIG_IC7300::set_modeA(int val)
{
	A.imode = val;
	cmd.assign(pre_to);
	cmd += '\x26';
	if (useB) cmd += '\x01';  // inactive vfo
	else      cmd += '\x00';  // active vfo

	cmd += IC7300_mode_nbr[val];
	if (val > 7) cmd += '\x01';
	else cmd += '\x00';
	cmd += FIL_A;
	cmd.append( post );
	waitFB("set modeA");

	trace(4, "set_modeA():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));

	if (!useB) adjust_bandwidth(val);
}

int RIG_IC7300::get_modeB()
{
	int md = 0;
	size_t p;

	string resp;
	cmd.assign(pre_to).append("\x26");
	resp.assign(pre_fm).append("\x26");

	if (useB) {
		cmd += '\x00';   // active vfo
		resp += '\x00';
	} else {
		cmd += '\x01';   // inactive vfo
		resp += '\x01';
	}
	cmd.append(post);

	if (waitFOR(10, "get mode B")) {
		p = replystr.rfind(resp);
		if (p == string::npos)
			return B.imode;

		for (md = 0; md < 8; md++) {
			if (replystr[p+6] == IC7300_mode_nbr[md]) {
				B.imode = md;
				if (replystr[p+7] == 0x01 && B.imode < 4)
					B.imode += 8;
				if (B.imode > 11)
					B.imode = 1;
				break;
			}
		}
		FIL_B = replystr[p+8];
	}

	trace(4, "get_modeB():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));

	return B.imode;
}

void RIG_IC7300::set_modeB(int val)
{
	B.imode = val;
	cmd.assign(pre_to);
	cmd += '\x26';
	if (useB) cmd += '\x00';  // active vfo
	else      cmd += '\x01';  // inactive vfo

	cmd += IC7300_mode_nbr[val];
	if (val > 7) cmd += '\x01';
	else cmd += '\x00';
	cmd += FIL_B;
	cmd.append( post );
	waitFB("set modeB");

	trace(4, "set_modeB():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));

	if (useB) adjust_bandwidth(val);
}

////////////////////////////////////////////////////////////////////////
#endif // ifdef 1
////////////////////////////////////////////////////////////////////////

bool RIG_IC7300::can_split()
{
	return true;
}

void RIG_IC7300::set_split(bool val)
{
	split = val;
	cmd = pre_to;
	cmd += 0x0F;
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	waitFB(val ? "set split ON" : "set split OFF");

	trace(4, "set_split():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC7300::get_split()
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

	trace(4, "set_split():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));

	return split;
}

int RIG_IC7300::get_bwA()
{
	if (A.imode == 3 || A.imode == 11) return 0; // FM, FM-D

	if (useB) selectA();

	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(post);
	int bwval = A.iBW;
	if (waitFOR(8, "get_bwA")) {
		string resp = pre_fm;
		resp.append("\x1A\x03");
		size_t p = replystr.find(resp);
		if (p != string::npos)
			bwval = fm_bcd(replystr.substr(p+6), 2);
	}
	if (bwval != A.iBW) {
		A.iBW = bwval;
	}

	if (useB) selectB();

	trace(4, "get_bwA():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));

	return A.iBW;
}

void RIG_IC7300::set_bwA(int val)
{

	if (A.imode == 3 || A.imode == 11) return; // FM, FM-D

	A.iBW = val;
	if (useB) selectA();

	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(to_bcd(A.iBW, 2));
	cmd.append(post);
	waitFB("set bwA");

	trace(4, "set_bwA():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));

	if (useB) selectB();
}

int RIG_IC7300::get_bwB()
{
	if (B.imode == 3 || B.imode == 11) return 0; // FM, FM-D

	if (!useB) selectB();

	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(post);
	int bwval = B.iBW;
	if (waitFOR(8, "get_bwB")) {
		string resp = pre_fm;
		resp.append("\x1A\x03");
		size_t p = replystr.find(resp);
		if (p != string::npos)
			bwval = fm_bcd(replystr.substr(p+6), 2);
	}
	if (bwval != B.iBW) {
		B.iBW = bwval;
	}

	if (!useB) selectA();

	trace(4, "get_bwB():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));

	return B.iBW;
}

void RIG_IC7300::set_bwB(int val)
{
	if (B.imode == 3 || B.imode == 11) return; // FM, FM-D
	B.iBW = val;

	if (!useB) selectB();

	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(to_bcd(B.iBW, 2));
	cmd.append(post);
	waitFB("set bwB");

	trace(4, "set_bwB():\n", 
		str2hex(cmd.c_str(), cmd.length()),
		"\n",
		str2hex(replystr.c_str(), replystr.length()));

	if (!useB) selectA();
}

// LSB  USB  AM   FM   CW  CW-R  RTTY  RTTY-R  LSB-D  USB-D  AM-D  FM-D
//  0    1    2    3   4     5    6     7      8      9       10    11

int RIG_IC7300::adjust_bandwidth(int m)
{
	int bw = 0;
	switch (m) {
		case 2: case 10: // AM, AM-D
			bandwidths_ = IC7300_am_bws;
			bw_vals_ = IC7300_bw_vals_AM;
			bw = 19;
			break;
		case 3: case 11: // FM, FM-D
			bandwidths_ = IC7300_fm_bws;
			bw_vals_ = IC7300_bw_vals_FM;
			bw = 0;
			break;
		case 6: case 7: // RTTY, RTTY-R
			bandwidths_ = IC7300_rtty_bws;
			bw_vals_ = IC7300_bw_vals_RTTY;
			bw = 12;
			break;
		case 4: case 5: // CW, CW -R
			bandwidths_ = IC7300_ssb_bws;
			bw_vals_ = IC7300_bw_vals_SSB;
			bw = 12;
			break;
		case 0: case 1: // LSB, USB
		case 8: case 9: // LSB-D, USB-D
		default:
			bandwidths_ = IC7300_ssb_bws;
			bw_vals_ = IC7300_bw_vals_SSB;
			bw = 34;
	}
	return bw;
}

const char ** RIG_IC7300::bwtable(int m)
{
	const char **table;
	switch (m) {
		case 2: case 10: // AM, AM-D
			table = IC7300_am_bws;
			break;
		case 3: case 11: // FM, FM-D
			table = IC7300_fm_bws;
			break;
		case 6: case 7: // RTTY, RTTY-R
			table = IC7300_rtty_bws;
			break;
		case 4: case 5: // CW, CW -R
		case 0: case 1: // LSB, USB
		case 8: case 9: // LSB-D, USB-D
		default:
			table = IC7300_ssb_bws;
	}
	return table;
}

int RIG_IC7300::def_bandwidth(int m)
{
	int bw = 0;
	switch (m) {
		case 2: case 10: // AM
			bw = 19;
			break;
		case 3: case 11: // FM
			bw = 0;
			break;
		case 6: case 7: // RTTY
			bw = 12;
			break;
		case 4: case 5: // CW
			bw = 12;
			break;
		case 8: case 9: // DATA
		case 0: case 1: // SSB
		default:
			bw = 34;
	}
	return bw;
}

int RIG_IC7300::get_mic_gain()
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
			val = num100(replystr.substr(p + 6));
	}
	return val;
}

void RIG_IC7300::set_mic_gain(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x0B");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set mic gain");
}

void RIG_IC7300::get_mic_gain_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

static int comp_level[] = {11,34,58,81,104,128,151,174,197,221,244};
void RIG_IC7300::set_compression(int on, int val)
{
	cmd = pre_to;
	cmd.append("\x16\x44");
	if (on) cmd += '\x01';
	else cmd += '\x00';
	cmd.append(post);
	waitFB("set Comp ON/OFF");

	if (val < 0) return;
	if (val > 10) return;

	cmd.assign(pre_to).append("\x14\x0E");
	cmd.append(to_bcd(comp_level[val], 3));
	cmd.append( post );
	waitFB("set comp");
}

void RIG_IC7300::get_compression(int &on, int &val)
{
	std::string resp;

	cmd.assign(pre_to).append("\x16\x44").append(post);

	resp.assign(pre_fm).append("\x16\x44");

	if (waitFOR(8, "get comp on/off")) {
		size_t p = replystr.find(resp);
		if (p != string::npos)
			on = (replystr[p+6] == 0x01);
	}

	cmd.assign(pre_to).append("\x14\x0E").append(post);
	resp.assign(pre_fm).append("\x14\x0E");

	if (waitFOR(9, "get comp level")) {
		size_t p = replystr.find(resp);
		int level = 0;
		if (p != string::npos) {
			level = fm_bcd(replystr.substr(p+6), 3);
			for (val = 0; val < 11; val++)
				if (level <= comp_level[val]) break;
		}
	}
}

void RIG_IC7300::set_vox_onoff()
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

// Xcvr values range 0...255 step 1
void RIG_IC7300::get_vox_gain_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

void RIG_IC7300::set_vox_gain()
{
	int vox_gain = round((progStatus.vox_gain * 255 / 100 + 0.5));
	minmax(0, 255, vox_gain);
	cmd.assign(pre_to).append("\x14\x16");
	cmd.append(to_bcd(vox_gain, 3));
	cmd.append( post );
	waitFB("SET vox gain");
}

// Xcvr values range 0...255 step 1
void RIG_IC7300::get_vox_anti_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

void RIG_IC7300::set_vox_anti()
{
	int vox_anti = round((progStatus.vox_anti * 255 / 100 + 0.5));
	minmax(0, 255, vox_anti);
	cmd.assign(pre_to).append("\x14\x17");
	cmd.append(to_bcd(vox_anti, 3));
	cmd.append( post );
	waitFB("SET anti-vox");
}

// VOX hang 0.0 - 2.0, step 0.1
// Xcvr values 0..20 step 1
void RIG_IC7300::get_vox_hang_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 20; step = 1;
}

void RIG_IC7300::set_vox_hang()
{
	cmd.assign(pre_to).append("\x1A\x05\x01\x91");
	cmd.append(to_bcd(progStatus.vox_hang, 2));
	cmd.append( post );
	waitFB("SET vox hang");
}

//----------------------------------------------------------------------
// CW controls

void RIG_IC7300::get_cw_wpm_min_max(int &min, int &max)
{
	min = 6; max = 48;
}

void RIG_IC7300::set_cw_wpm()
{
	int iwpm = round((progStatus.cw_wpm - 6) * 255 / 42 + 0.5);
	minmax(0, 255, iwpm);

	cmd.assign(pre_to).append("\x14\x0C");
	cmd.append(to_bcd(iwpm, 3));
	cmd.append( post );
	waitFB("SET cw wpm");
}

void RIG_IC7300::enable_break_in()
{
// 16 47 00 break-in off
// 16 47 01 break-in semi
// 16 47 02 break-in full

	cmd.assign(pre_to).append("\x16\x47");

	switch (progStatus.break_in) {
		case 2: cmd += '\x02'; break_in_label("FULL"); break;
		case 1: cmd += '\x01'; break_in_label("SEMI");  break;
		case 0:
		default: cmd += '\x00'; break_in_label("BK-IN");
	}
	cmd.append(post);
	waitFB("SET break-in");
}

void RIG_IC7300::get_cw_qsk_min_max_step(double &min, double &max, double &step)
{
	min = 2.0; max = 13.0; step = 0.1;
}

void RIG_IC7300::set_cw_qsk()
{
	int qsk = round ((progStatus.cw_qsk - 2.0) * 255.0 / 11.0 + 0.5);
	minmax(0, 255, qsk);

	cmd.assign(pre_to).append("\x14\x0F");
	cmd.append(to_bcd(qsk, 3));
	cmd.append(post);
	waitFB("Set cw qsk delay");
}

void RIG_IC7300::get_cw_spot_tone_min_max_step(int &min, int &max, int &step)
{
	min = 300; max = 900; step = 5;
}

void RIG_IC7300::set_cw_spot_tone()
{
	cmd.assign(pre_to).append("\x14\x09"); // values 0=300Hz 255=900Hz
	int n = round((progStatus.cw_spot_tone - 300) * 255.0 / 600.0 + 0.5);
	minmax(0, 255, n);

	cmd.append(to_bcd(n, 3));
	cmd.append( post );
	waitFB("SET cw spot tone");
}

void RIG_IC7300::set_cw_vol()
{
	cmd.assign(pre_to);
	cmd.append("\x1A\x05");
	cmd += '\x00';
	cmd += '\x24';
	cmd.append(to_bcd((int)(progStatus.cw_vol * 2.55), 3));
	cmd.append( post );
	waitFB("SET cw sidetone volume");
}

// Tranceiver PTT on/off
void RIG_IC7300::set_PTT_control(int val)
{
	cmd = pre_to;
	cmd += '\x1c';
	cmd += '\x00';
	cmd += (unsigned char) val;
	cmd.append( post );
	waitFB("set ptt");
	ptt_ = val;
}

int RIG_IC7300::get_PTT()
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

// Volume control val 0 ... 100
void RIG_IC7300::set_volume_control(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append(bcd255(val));
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
int RIG_IC7300::get_volume_control()
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
			val = num100(replystr.substr(p + 6));
	}
	return (val);
}

void RIG_IC7300::get_vol_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

void RIG_IC7300::set_power_control(double val)
{
	cmd = pre_to;
	cmd.append("\x14\x0A");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set power");
}

int RIG_IC7300::get_power_control()
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
			val = num100(replystr.substr(p+6));
	}
	return val;
}

void RIG_IC7300::get_pc_min_max_step(double &min, double &max, double &step)
{
	min = 2; max = 100; step = 1;
}

int RIG_IC7300::get_smeter()
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

struct pwrpair {int mtr; float pwr;};

static pwrpair pwrtbl[] = { 
	{0, 0.0}, 
	{49, 12.5},
	{74, 20.0}, 
	{89, 25.0}, 
	{103, 30.0}, 
	{124, 40.0}, 
	{143, 50.0}, 
	{183, 75.0},
	{213, 100.0},
	{255, 120.0 } };

int RIG_IC7300::get_power_out(void)
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
			mtr = fm_bcd(replystr.substr(p+6), 3);
			size_t i = 0;
			for (i = 0; i < sizeof(pwrtbl) / sizeof(pwrpair) - 1; i++)
				if (mtr >= pwrtbl[i].mtr && mtr < pwrtbl[i+1].mtr)
					break;
			if (mtr < 0) mtr = 0;
			if (mtr > 255) mtr = 255;
			mtr = (int)ceil(pwrtbl[i].pwr + 
				(pwrtbl[i+1].pwr - pwrtbl[i].pwr)*(mtr - pwrtbl[i].mtr)/(pwrtbl[i+1].mtr - pwrtbl[i].mtr));
			
			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

int RIG_IC7300::get_swr(void)
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

int RIG_IC7300::get_alc(void)
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

void RIG_IC7300::set_rf_gain(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x02");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set RF");
}

int RIG_IC7300::get_rf_gain()
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
	return val;
}

void RIG_IC7300::get_rf_min_max_step(double &min, double &max, double &step)
{
	min = 0; max = 100; step = 1;
}

void RIG_IC7300::tune_rig()
{
	cmd = pre_to;
	cmd.append("\x1c\x01\x02");
	cmd.append( post );
	waitFB("tune rig");
}

int RIG_IC7300::next_preamp()
{
	if (atten_level == 1)
		return preamp_level;
	switch (preamp_level) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 0;
	}
	return 0;
}

void RIG_IC7300::set_preamp(int val)
{
	if (val) {
		atten_level = 0;
		atten_label("ATT", false);
	}

	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';

	preamp_level = val;
	switch (val) {
		case 1: 
			preamp_label("Amp 1", true);
			break;
		case 2:
			preamp_label("Amp 2", true);
			break;
		case 0:
		default:
			preamp_label("PRE", false);
	}

	cmd += (unsigned char)preamp_level;
	cmd.append( post );
	waitFB(	(preamp_level == 0) ? "set Preamp OFF" :
			(preamp_level == 1) ? "set Preamp Level 1" :
			"set Preamp Level 2");
}

int RIG_IC7300::get_preamp()
{
	string cstr = "\x16\x02";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get Preamp Level")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			preamp_level = replystr[p+6];
			if (preamp_level == 1) {
				preamp_label("Amp 1", true);
			} else if (preamp_level == 2) {
				preamp_label("Amp 2", true);
			} else {
				preamp_label("PRE", false);
				preamp_level = 0;
			}
	}
	return preamp_level;
}

void RIG_IC7300::set_attenuator(int val)
{
	if (val) {
		atten_label("20 dB", true);
		atten_level = 1;
		preamp_label("PRE", false);
	} else {
		atten_level = 0;
		atten_label("ATT", false);
	}

	cmd = pre_to;
	cmd += '\x11';
	cmd += atten_level ? '\x20' : '\x00';
	cmd.append( post );
	waitFB("set att");
}

int RIG_IC7300::next_attenuator()
{
	if (atten_level) return 0;
	return 1;
}

int RIG_IC7300::get_attenuator()
{
	cmd = pre_to;
	cmd += '\x11';
	cmd.append( post );
	string resp = pre_fm;
	resp += '\x11';
	if (waitFOR(7, "get ATT")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			if (replystr[p+5] == 0x20) {
				atten_label("20 dB", true);
				atten_level = 1;
				return 1;
			} else {
				atten_label("ATT", false);
				atten_level = 0;
				return 0;
			}
		}
	}
	return 0;
}

void RIG_IC7300::set_noise(bool val)
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set noise");
}

int RIG_IC7300::get_noise()
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
		if (p != string::npos) {
			val = replystr[p+6];
		}
	}
	return val;
}

void RIG_IC7300::set_nb_level(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x12");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set NB level");
}

int  RIG_IC7300::get_nb_level()
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
	return val;
}

void RIG_IC7300::set_noise_reduction(int val)
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set NR");
}

int RIG_IC7300::get_noise_reduction()
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

void RIG_IC7300::set_noise_reduction_val(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x06");
	val *= 16;
	val += 8;
	cmd.append(to_bcd(val, 3));
	cmd.append(post);
	waitFB("set NRval");
}

int RIG_IC7300::get_noise_reduction_val()
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
		if (p != string::npos) {
			val = fm_bcd(replystr.substr(p+6),3);
			val -= 8;
			val /= 16;
		}
	}
	return val;
}

void RIG_IC7300::set_squelch(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x03");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set Sqlch");
}

int  RIG_IC7300::get_squelch()
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
			val = num100(replystr.substr(p+6));
	}
	return val;
}

void RIG_IC7300::set_auto_notch(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x41';
	cmd += (unsigned char)val;
	cmd.append( post );
	waitFB("set AN");
}

int RIG_IC7300::get_auto_notch()
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

static bool IC7300_notchon = false;

void RIG_IC7300::set_notch(bool on, int val)
{
	int notch = val / 20 + 53;
	minmax(0, 255, notch);
	if (on != IC7300_notchon) {
		cmd = pre_to;
		cmd.append("\x16\x48");
		cmd += on ? '\x01' : '\x00';
		cmd.append(post);
		waitFB("set notch");
		IC7300_notchon = on;
	}
	cmd = pre_to;
	cmd.append("\x14\x0D");
	cmd.append(to_bcd(notch,3));
	cmd.append(post);
	waitFB("set notch val");
}

bool RIG_IC7300::get_notch(int &val)
{
	bool on = false;

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
	return (IC7300_notchon = on);
}

void RIG_IC7300::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 4040;
	step = 20;
}

static int agcval = 3;
int  RIG_IC7300::get_agc()
{
	cmd = pre_to;
	cmd.append("\x16\x12");
	cmd.append(post);
	if (waitFOR(8, "get AGC")) {
		size_t p = replystr.find(pre_fm);
		if (p != string::npos)
			agcval = replystr[p+6]; // 1 == off, 2 = FAST, 3 = MED, 4 = SLOW
	}
	return agcval;
}

int RIG_IC7300::incr_agc()
{
	agcval++;
	if (agcval == 4) agcval = 1;
	cmd = pre_to;
	cmd.append("\x16\x12");
	cmd += agcval;
	cmd.append(post);
	waitFB("set AGC");
	return agcval;
}


static const char *agcstrs[] = {"AGC", "FST", "MED", "SLO"};
const char *RIG_IC7300::agc_label()
{
	return agcstrs[agcval];
}

int  RIG_IC7300::agc_val()
{
	return (agcval);
}

void RIG_IC7300::set_if_shift(int val)
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

bool RIG_IC7300::get_if_shift(int &val) {
	val = sh_;
	return sh_on_;
}

void RIG_IC7300::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -50;
	max = +50;
	step = 1;
}

void RIG_IC7300::setVfoAdj(double v)
{
	vfo_ = v;
	cmd.assign(pre_to);
	cmd.append("\x1A\x05");
	cmd += '\x00';
	cmd += '\x58';
	cmd.append(bcd255(int(v)));
	cmd.append(post);
	waitFB("SET vfo adjust");
}

double RIG_IC7300::getVfoAdj()
{
	cmd.assign(pre_to);
	cmd.append("\x1A\x05");
	cmd += '\x00';
	cmd += '\x58';
	cmd.append(post);

	if (waitFOR(11, "get vfo adj")) {
		size_t p = replystr.find(pre_fm);
		if (p != string::npos) {
			vfo_ = num100(replystr.substr(p+8));
		}
	}
	return vfo_;
}
