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
#include <iostream>
#include <sstream>

#include "ICF8101.h"
#include "support.h"
#include "trace.h"

//=============================================================================
// IC-F8101

const char ICF8101name_[] = "IC-F8101";

#define NUM_MODES  11

enum F81_modes {
	F81_LSB, F81_USB, F81_AM, F81_CW, F81_RTTY,
	F81_LSBD1, F81_USBD1,
	F81_LSBD2, F81_USBD2,
	F81_LSBD3, F81_USBD3 };

const char *ICF8101modes_[NUM_MODES + 1] = { 
"LSB", "USB", "AM", "CW", "RTTY", "LSB-D1", "USB-D1", "LSB-D2", "USB-D2", "LSB-D3", "USB-D3", NULL};

const char mdval[NUM_MODES] = { 0X00, 0X01, 0X02, 0X03, 0X04, 0x18, 0x19, 0x20, 0x21, 0x22, 0x23};

const char ICF8101_mode_type[NUM_MODES] = { 
'L', 'U', 'U', 'L', 'L', 'L', 'U', 'L', 'U', 'L', 'U' };

static int mode_bwA[NUM_MODES] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
static int mode_bwB[NUM_MODES] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

static const char *ICF8101_CW_SSB_widths[] = {
"100",   "200",  "300",  "400",  "500",  "600",  "700",  "800",  "900", "1000",
"1100", "1200", "1300", "1400", "1500", "1600", "1700", "1800", "1900", "2000", 
"2100", "2200", "2300", "2400", "2500", "2600", "2700", "2800", "2900", "3000",
NULL};

static const char ICF8101_CW_SSB_width_vals[] = {
'\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\x10',
'\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x20',
'\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27', '\x28', '\x29', '\x30',
};
#define NUM_CW_SSB_WIDTHS 30

static const char *ICF8101_RTTY_widths[] = {
  "NONE", NULL};

static const char *ICF8101_AM_widths[] = {
  "200",  "400",  "600",  "800", "1000", "1200", "1400", "1600", "1800", "2000",
 "2200", "2400", "2600", "2800", "3000", "3200", "3400", "3600", "3800", "4000",
 "4200", "4400", "4600", "4800", "5000", "5200", "5400", "5600", "5800", "5000",
 "6200", "6400", "6600", "6800", "7000", "7200", "7400", "7600", "7800", "8000",
 "8200", "8400", "8600", "8800", "9000", "9200", "9400", "9600", "9800", "10000",
NULL};

static const char ICF8101_bw_vals_AM[] = {
'\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\x10',
'\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x20',
'\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27', '\x28', '\x29', '\x30',
'\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37', '\x38', '\x39', '\x40',
'\x41', '\x42', '\x43', '\x44', '\x45', '\x46', '\x47', '\x48', '\x49', '\x50',
};
#define NUM_AM_WIDTHS 50

//======================================================================
// ICF8101 unique commands
//======================================================================

static GUI ICF8101_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 },	//0
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 },	//1
	{ (Fl_Widget *)btnAGC,      214, 125,  50 },	//2
	{ (Fl_Widget *)sldrPOWER,    54, 145, 156 },	//3
	{ (Fl_Widget *)sldrMICGAIN, 266, 145, 156 },	//4
	{ (Fl_Widget *)NULL, 0, 0, 0 }
};
/*
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
};
*/

void RIG_ICF8101::initialize()
{
	ICF8101_widgets[0].W = btnVol;
	ICF8101_widgets[1].W = sldrVOLUME;
	ICF8101_widgets[2].W = btnAGC;
	ICF8101_widgets[3].W = sldrPOWER;
	ICF8101_widgets[4].W = sldrMICGAIN;
/*
	ICF8101_widgets[3].W = sldrRFGAIN;
	ICF8101_widgets[4].W = sldrSQUELCH;
	ICF8101_widgets[5].W = btnNR;
	ICF8101_widgets[6].W = sldrNR;
	ICF8101_widgets[7].W = btnLOCK;
	ICF8101_widgets[8].W = sldrINNER;
	ICF8101_widgets[9].W = btnCLRPBT;
	ICF8101_widgets[10].W = sldrOUTER;
	ICF8101_widgets[11].W = btnNotch;
	ICF8101_widgets[12].W = sldrNOTCH;

	btn_icom_select_11->deactivate();
	btn_icom_select_12->deactivate();
	btn_icom_select_13->deactivate();

	choice_rTONE->deactivate();
	choice_tTONE->deactivate();
*/
}

void RIG_ICF8101::shutdown()
{
	cmd = pre_to;
	cmd += '\x08';
	cmd.append(post);
	set_trace(2, "set memory mode", str2hex(cmd.c_str(), cmd.length()));
	waitFB("set memory mode");

	cmd = pre_to;
	cmd.append("\x0E\x01");
	cmd.append(post);
	set_trace(2, "set scan mode", str2hex(cmd.c_str(), cmd.length()));
	waitFB("set scan mode");
}

RIG_ICF8101::RIG_ICF8101() {
	name_ = ICF8101name_;
	modes_ = ICF8101modes_;
	_mode_type = ICF8101_mode_type;
//	bandwidths_ = ICF8101_CW_SSB_widths;
//	bw_vals_ = ICF8101_bw_vals_SSB;
	widgets = ICF8101_widgets;

	comm_baudrate = BR9600;
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

	def_freq = A.freq = 14070000;
	def_mode = A.imode = 1;
//	def_bw = A.iBW = 29;

	B.freq = 7070000;
	B.imode = 1;
//	B.iBW = 29;

	has_smeter = true;

	has_mode_control = true;
	has_volume_control = true;
	has_rf_control = true;
	has_micgain_control = true;
	has_power_control = true;

	has_ptt_control = true;
	has_tune_control = true;
	has_agc_control = true;

	has_split_AB = true;

	has_noise_control = true;
	has_nb_level = true;
	has_preamp_control = true;

	has_compON = true;
	has_compression = true;

/*
	has_extras = true;
	has_power_out = true;
	has_swr_control = true;
	has_alc_control = true;
	has_sql_control = true;
	has_attenuator_control = true;
	has_noise_reduction = true;
	has_noise_reduction_control = true;
	has_auto_notch = true;
	has_notch_control = true;
	has_pbt_controls = true;
	has_FILTER = true;
	has_vox_onoff = true;
	has_vfo_adj = true;
	has_a2b = true;

	has_band_selection = true;

	has_cw_wpm = true;
	has_cw_spot_tone = true;
	has_cw_qsk = true;
	has_cw_break_in = true;
*/

	defaultCIV = 0x8A;
	adjustCIV(defaultCIV);

	precision = 10;
	ndigits = 8;

};

static inline void minmax(int min, int max, int &val)
{
	if (val > max) val = max;
	if (val < min) val = min;
}

//======================================================================

void RIG_ICF8101::selectA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x00';
	cmd.append(post);
	set_trace(2, "selectA()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("select A");
	inuse = onA;
}

void RIG_ICF8101::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x01';
	cmd.append(post);
	set_trace(2, "selectB()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("select B");
	inuse = onB;
}

bool RIG_ICF8101::check ()
{
	string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	bool ok = waitFOR(11, "check vfo");
	get_trace(2, "check()", str2hex(replystr.c_str(), replystr.length()));
	return ok;
}

long RIG_ICF8101::get_vfoA ()
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
	get_trace(2, "get_vfoA()", str2hex(replystr.c_str(), replystr.length()));
	return A.freq;
}

void RIG_ICF8101::set_vfoA (long freq)
{
	A.freq = freq;
	cmd = pre_to;
	cmd += '\x1A';
	cmd += '\x35';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	set_trace(2, "set_vfoA()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("set vfo A");
}

long RIG_ICF8101::get_vfoB ()
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
	get_trace(2, "get_vfoB()", str2hex(replystr.c_str(), replystr.length()));
	return B.freq;
}

void RIG_ICF8101::set_vfoB (long freq)
{
	B.freq = freq;
	cmd = pre_to;
	cmd += '\x1A';
	cmd += '\x35';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	set_trace(2, "set_vfoB()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("set vfo B");
}

// Tranceiver PTT on/off
// ON  1A 37 00 02
// OFF 1A 37 00 00
void RIG_ICF8101::set_PTT_control(int val)
{
	cmd = pre_to;
	cmd.append("\x1A\x37");
	cmd += '\x00';
	cmd += (val ? '\x02' : '\x00');
	cmd.append( post );
	if (val) {
		waitFB("set ptt ON");
		set_trace(2, "set_PTT(ON)", str2hex(cmd.c_str(), cmd.length()));
	} else {
		waitFB("set ptt OFF");
		set_trace(2, "set_PTT(OFF)", str2hex(cmd.c_str(), cmd.length()));
	}
	ptt_ = val;
}

int RIG_ICF8101::get_PTT()
{
	cmd = pre_to;
	cmd.append("\x1A\x37");
	string resp = pre_fm;
	resp.append("\x1A\x37");
	cmd.append(post);
	if (waitFOR(9, "get PTT")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			ptt_ = (replystr[p + 7] == 0x02);
	}
	return ptt_;
}

// Volume control val 0 ... 100
struct VOL_TBL {
	string catval;
	int vol;
} vol_tbl[] = {
{"\x00\x00", 0},
{"\x01\x39", 1},  {"\x01\x43", 2},  {"\x01\x47", 3},  {"\x01\x51", 4},  {"\x01\x55", 5},
{"\x01\x59", 6},  {"\x01\x63", 7},  {"\x01\x67", 8},  {"\x01\x71", 9},  {"\x01\x75", 10},
{"\x01\x77", 11}, {"\x01\x79", 12}, {"\x01\x81", 13}, {"\x01\x83", 14}, {"\x01\x85", 15},
{"\x01\x87", 16}, {"\x01\x89", 17}, {"\x01\x91", 18}, {"\x01\x93", 19}, {"\x01\x95", 20},
{"\x01\x97", 21}, {"\x01\x99", 22}, {"\x02\x01", 23}, {"\x02\x03", 24}, {"\x02\x05", 25},
{"\x02\x07", 26}, {"\x02\x09", 27}, {"\x02\x11", 28}, {"\x02\x13", 29}, {"\x02\x15", 30},
{"\x02\x17", 31}, {"\x02\x19", 32}, {"\x02\x21", 33}, {"\x02\x23", 34}, {"\x02\x25", 35},
{"\x02\x27", 36}, {"\x02\x29", 37}, {"\x02\x31", 38}, {"\x02\x33", 39}, {"\x02\x35", 40},
{"\x02\x37", 41}, {"\x02\x39", 42}, {"\x02\x41", 43}, {"\x02\x43", 44}, {"\x02\x45", 45},
{"\x02\x47", 46}, {"\x02\x49", 47}, {"\x02\x51", 48}, {"\x02\x53", 49}, {"\x02\x55", 50} };

void RIG_ICF8101::set_volume_control(int val)
{
	if (val < 0) val = 0;
	if (val > 50) val = 50;
	cmd = pre_to;
	cmd.append("\x14\x01");
	if (val == 0) {
		cmd += '\x00'; cmd += '\x00';
	} else
		cmd.append(vol_tbl[val].catval);
	cmd.append( post );
	waitFB("set vol");
	set_trace(2, "set_volume_control()", str2hex(cmd.c_str(), cmd.length()));
}

int RIG_ICF8101::get_volume_control()
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
			resp = resp.substr(6, 2);
			if (resp[0] == '\x00' && resp[1] == '\x00') {
				val = 0;
			} else { 
				for (int i = 1; i <= 50; i++) {
					if (resp == vol_tbl[i].catval) {
						val = vol_tbl[i].vol;
						break;
					}
				}
			}
		}
	}
	get_trace(2, "get_volume_control()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

void RIG_ICF8101::get_vol_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 50; step = 1;
}

struct smtr_map {float meter; int sig[3];}; // sig[PRE/OFF/ATT]

static smtr_map sm_map[] = {
{  0.0, { 56,  12,   0}}, // S0
{ 11.0, { 60,  25,   0}}, // S2
{ 17.0, { 62,  37,   0}}, // S3
{ 22.0, { 72,  49,  15}}, // S4
{ 28.0, { 83,  61,  25}}, // S5
{ 33.0, { 95,  73,  40}}, // S6
{ 39.0, {108,  85,  55}}, // S7
{ 45.0, {115,  97,  71}}, // S8
{ 50.0, {166, 110,  85}}, // S9
{ 58.0, {231, 167, 107}}, // +10
{ 67.0, {273, 223, 141}}, // +20
{ 74.0, {315, 268, 212}}, // +30
{ 84.0, {356, 310, 256}}, // +40
{100.0, {400, 356, 300}}  // +60
};

int RIG_ICF8101::get_smeter()
{
	string cstr = "\x1A\x08";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr = 0;
	int n = preamp_level;
	if (n == 0) n = 1;
	else if (n == 1) n = 0;
	if (waitFOR(9, "get smeter")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			int val = fm_bcd(replystr.substr(p+6), 3);
			if (val <= sm_map[0].sig[n]) mtr = 0;
			else {
				int i = 0;
				int i_max = sizeof(sm_map) / sizeof(smtr_map) - 1;
				if (val > sm_map[i_max].sig[n]) val = sm_map[i_max].sig[n];
				for (i = 0; i < i_max; i++)
					if (val >= sm_map[i].sig[n] && val < sm_map[i+1].sig[n])
						break;
				mtr = (int)ceil(sm_map[i].meter + 
					(sm_map[i+1].meter - sm_map[i].meter)*(val - sm_map[i].sig[n]) /
					(sm_map[i+1].sig[n] - sm_map[i].sig[n]) );
			}
		}
	}
	return mtr;
}

struct mtr_map {int mtr; float pwr;};

static mtr_map pwr_map[] = { 
{0, 0.0},
{51, 10.0},
{102, 20.0},
{153, 30.0},
{204, 40.0},
{255, 50.0}
};

int RIG_ICF8101::get_power_out(void)
{
	string cstr = "\x15\x11";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr = 0;
	if (waitFOR(9, "get power out")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			int val = fm_bcd(replystr.substr(p+6), 3);
			size_t i = 0;
			if (val < 0) val = 0;
			if (val > 255) val = 255;
			for (i = 0; i < sizeof(pwr_map) / sizeof(mtr_map) - 1; i++)
				if (val >= pwr_map[i].mtr && val < pwr_map[i+1].mtr)
					break;
			mtr = (int)ceil(pwr_map[i].pwr + 
				(pwr_map[i+1].pwr - pwr_map[i].pwr)*(val - pwr_map[i].mtr)/(pwr_map[i+1].mtr - pwr_map[i].mtr));
		}
	}
	return mtr;
}

static int agcval = 0;
int  RIG_ICF8101::get_agc()
{
	cmd = pre_to;
	cmd.append("\x1A\x05\x03\x06");
	cmd.append(post);
	if (waitFOR(11, "get AGC")) {
		size_t p = replystr.find(pre_fm);
		if (p == string::npos) return agcval;
		return (agcval = replystr[p+9]); // 0 = off, 1 = FAST, 2 = SLOW, 3 = AUTO
	}
	return agcval;
}

int RIG_ICF8101::incr_agc()
{
	agcval++;
	if (agcval == 4) agcval = 0;
	cmd = pre_to;
	cmd.append("\x1A\x05\x03\x06");
	cmd += '\x00';
	cmd += agcval;
	cmd.append(post);
	waitFB("set AGC");
	return agcval;
}


static const char *agcstrs[] = {"AGC", "FST", "SLO", "AUT"};
const char *RIG_ICF8101::agc_label()
{
	return agcstrs[agcval];
}

int  RIG_ICF8101::agc_val()
{
	return (agcval);
}


// FE FE 8A E0 1A 05 03 07 00 00/01/02 FD (00=LOW, 01=MID, 02=HIGH)

void RIG_ICF8101::set_power_control(double val)
{
	cmd = pre_to;
	cmd.append("\x1A\x05\x03\x07");
	cmd += '\x00';
	cmd += val;
	cmd.append( post );
	waitFB("set power");
	set_trace(2, "set_power()", str2hex(cmd.c_str(), cmd.length()));
}

// reply FE FE E0 8A 1A 05 03 07 00 XX FD

int RIG_ICF8101::get_power_control()
{
	string cstr = "\x1A\x05\x03\x07";
	string resp = pre_fm;
	cmd = pre_to;
	cmd.append(cstr).append(post);
	resp.append(cstr);
	int val = progStatus.power_level;
	if (waitFOR(11, "get power")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			val = replystr[p+9];
	}
	return (progStatus.power_level = val);
}

int RIG_ICF8101::get_mic_gain()
{
	string cstr = "\x1A\x05\x03\x11";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	int val = progStatus.mic_gain;
	if (waitFOR(11, "get mic")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			val = replystr[p+9];
			if (val == '\x0A') val = 10;
		}
	}
	return val;
}

void RIG_ICF8101::set_mic_gain(int val)
{
	cmd = pre_to;
	cmd.append("\x1A\x05\x03\x11");
	cmd += '\x00';
	if (val < 10) cmd += val;
	else cmd += '\x10';
	cmd.append(post);
	waitFB("set mic");
	set_trace(2, "set_mic_gain()", str2hex(cmd.c_str(), cmd.length()));
}

int RIG_ICF8101::get_modeA()
{
	int md = A.imode;
	int val = 0;

	string resp = pre_fm;
	resp.append("\x1A\x34");
	cmd = pre_to;
	cmd.append("\x1A\x34");
	cmd.append(post);

	if (waitFOR(9, "get mode A")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			val = replystr[p + 7];
			for (int i = 0; i < NUM_MODES; i++)
				if (val == mdval[i]) {
					md = i;
					break;
				}
		}
	}
	get_trace(2, "get_modeA()", str2hex(replystr.c_str(), replystr.length()));
	return (A.imode = md);
}

void RIG_ICF8101::set_modeA(int val)
{
	A.imode = val;

	cmd = pre_to;
	cmd.append("\x1A\x36");
	cmd += '\x00';
	cmd += mdval[A.imode];
	cmd.append( post );
	waitFB("set mode A");
	set_trace(2, "set_modeA()", str2hex(cmd.c_str(), cmd.length()));
}

int RIG_ICF8101::get_modeB()
{
	int md = B.imode;
	int val = 0;

	string resp = pre_fm;
	resp.append("\x1A\x34");
	cmd = pre_to;
	cmd.append("\x1A\x34");
	cmd.append(post);

	if (waitFOR(9, "get mode B")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			val = replystr[p + 7];
			for (int i = 0; i < NUM_MODES; i++)
				if (val == mdval[i]) {
					md = i;
					break;
				}
		}
	}
	get_trace(2, "get_modeB()", str2hex(replystr.c_str(), replystr.length()));
	return (B.imode = md);
}

void RIG_ICF8101::set_modeB(int val)
{
	B.imode = val;

	cmd = pre_to;
	cmd.append("\x1A\x36");
	cmd += '\x00';
	cmd += mdval[B.imode];
	cmd.append( post );
	waitFB("set mode B");
	set_trace(2, "set_modeB()", str2hex(cmd.c_str(), cmd.length()));
}

int RIG_ICF8101::get_modetype(int n)
{
	return _mode_type[n];
}

bool RIG_ICF8101::can_split()
{
	return true;
}

// 1A 05 03 17 00 [01|00]
void RIG_ICF8101::set_split(bool val)
{
	split = val;
	cmd = pre_to;
	cmd.append("\x1A\x05\x03\x17");
	cmd += '\x00';
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	waitFB(val ? "set split ON" : "set split OFF");
	set_trace(2, (val ? "set split ON" : "set split OFF"), str2hex(cmd.c_str(), cmd.length()));
}

// F8101 does not respond to get split CAT command
int RIG_ICF8101::get_split()
{
	string cstr = "\x1A\x05\x03\x17";
	cstr += '\x00';
	string resp = pre_fm;
	resp.append(cstr);

	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );

	int mtr= -1;
	if (waitFOR(11, "get split")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			mtr = replystr[p+9];
			if (mtr == 0x01) split = 1;
		}
	}
	get_trace(2, "get_split()", str2hex(replystr.c_str(), replystr.length()));
	return split;
}

void RIG_ICF8101::set_noise(bool val)
{
	cmd = pre_to;
	cmd.append("\x1A\x05\x03\x01");
	cmd += '\x00';
	cmd += val ? '\x01' : '\x00';
	cmd.append(post);
	waitFB("set noise blanker");
	set_trace(2, "set_noise_blanker() ", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_ICF8101::get_noise()
{
	int val = progStatus.noise;
	string cstr = "\x1A\x05\x03\x01";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(11, "get noise blanker")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			val = replystr[p+9];
		}
	}
	get_trace(2, "get_noise_blanker()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

void RIG_ICF8101::set_nb_level(int val)
{
	cmd = pre_to;
	cmd.append("\x1A\x05\x03\x02");
	cmd += '\x00';
	cmd += bcdval[val];
	cmd.append(post);
	waitFB("set NB level");
	set_trace(2, "set_nb_level() ", str2hex(replystr.c_str(), replystr.length()));
}

int  RIG_ICF8101::get_nb_level()
{
	int val = progStatus.nb_level;
	string cstr = "\x1A\x05\x03\x02";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(11, "get NB level")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			for (int i = 0; i < 16; i++) {
				if (replystr[p+9] == bcdval[i]) {
					val = i;
					break;
				}
			}
		}
	}
	progStatus.nb_level = val;
	get_trace(2, "get_nb_level()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

int RIG_ICF8101::next_preamp()
{
	preamp_level++;
	if (preamp_level == 3) preamp_level = 0;
	return preamp_level;
}

void RIG_ICF8101::set_preamp(int val)
{
	preamp_level = val;
	cmd = pre_to;
	cmd.append("\x1A\x05\x03\x05");
	cmd += '\x00';
	switch (val) {
		case 0:
			preamp_label("OFF", false);
			cmd += '\x01';
			break;
		case 1:
			preamp_label("PRE", true);
			cmd += '\x00';
			break;
		case 2:
			preamp_label("ATT", true);
			cmd += '\x02';
	}
	cmd.append( post );
	waitFB("set Pre/Att");
	set_trace(2, "set_preamp_att() ", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_ICF8101::get_preamp()
{
	string cstr = "\x1A\x05\x03\x05";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(11, "get Pre")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			switch (replystr[p+9]) {
			case 0:
				preamp_label("PRE", false);
				preamp_level = 1;
				break;
			case 1:
				preamp_label("OFF", true);
				preamp_level = 0;
				break;
			case 2:
				preamp_label("ATT", true);
				preamp_level = 2;
			}
		}
	}
	get_trace(2, "get_preamp_attenuator()", str2hex(replystr.c_str(), replystr.length()));
	return preamp_level;
}

void RIG_ICF8101::set_compression(int on, int val)
{
	std::string onoff = "\x1A\x05\x03\x09";
	std::string level = "\x1A\x05\x03\x10";
	cmd = pre_to;
	cmd.append(onoff);
	cmd += '\x00';
	if (on) cmd += '\x01';
	else cmd += '\x00';
	cmd.append(post);
	waitFB("set Comp ON/OFF");
	set_trace(2, "set_speech_proc() ", str2hex(replystr.c_str(), replystr.length()));

	if (val < 0) val = 0;
	if (val > 10) val = 10;

	cmd.assign(pre_to).append(level);
	cmd += '\x00';
	cmd += bcdval[val];
	cmd.append(post);
	waitFB("set comp");
	set_trace(2, "set_speech_proc_level() ", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_ICF8101::get_compression(int &on, int &val)
{
	std::string onoff = "\x1A\x05\x03\x09";
	std::string level = "\x1A\x05\x03\x10";
	std::string resp = pre_fm;
	resp.append(onoff);
	cmd.assign(pre_to).append(onoff);
	cmd.append(post);
	if (waitFOR(11, "get comp on/off")) {
		size_t p = replystr.find(resp);
		if (p != string::npos)
			on = (replystr[p+9] == 0x01);
	}
	get_trace(2, "get_speech_comp_on_off()", str2hex(replystr.c_str(), replystr.length()));

	cmd.assign(pre_to).append(level).append(post);
	resp.assign(pre_fm).append(level);

	if (waitFOR(11, "get compression level")) {
		size_t p = replystr.find(resp);
		if (p != string::npos) {
			for (int i = 0; i < 11; i++) {
				if (replystr[p+9] == bcdval[i]) {
					val = i;
					break;
				}
			}
			get_trace(2, "get_comp_level()", str2hex(replystr.c_str(), replystr.length()));
		}
	}
}

// LSB:
//  offset: 1A 05 07 01 [00 00]...[15 00]
//  width:  1A 05 07 02 [00 01]...[00 30], 100 TO 3000 Hz in 100 Hz steps
// USB:
//  offset: 1A 05 08 01 [00 00]...[15 00]
//  width:  1A 05 08 02 [00 01]...[00 30], 100 TO 3000 Hz in 100 Hz steps
// CW:
//  offset: 1A 05 09 01 [00 00]...[08 00]
//  width:  1A 05 09 02 [00 01]...[00 30], 100 TO 3000 Hz in 100 Hz steps
// AM:
//  offset: 1A 05 10 01 [00 00]...[00 00]
//  width:  1A 05 10 02 [00 01]...[00 50], 200 TO 10000 Hz in 200 Hz steps
// RTTY:
//  offset: 1A 05 11 01 [00 00] = 1200.0 Hz (mark frequency)
//                      [00 01] = 1275.0 Hz
//                      [00 02] = 1487.5 Hz
//                      [00 03] = 1615.0 Hz
//                      [00 04] = 1700.0 Hz
//                      [00 05] = 2100.0 Hz
//                      [00 05] = 2125.0 Hz
//  shift:  1A 05 11 02 [00 00] = 170 Hz
//                      [00 01] = 200 Hz
//                      [00 01] = 425 Hz
//                      [00 01] = 850 Hz
//  polarity: 1A 05 11 02 [00 00] = NORMAL
//                        [00 01] = REVERSE
// LSBD1
//  offset: 1A 05 12 01 [00 00] = 1500 Hz
//                      [00 01] = 1650 Hz
//                      [00 02] = 1800 Hz
//  width:  1A 05 12 02 [00 01]...[00 30], 100 TO 3000 Hz in 100 Hz steps
// USBD1
//  offset: 1A 05 13 01 [00 00] = 1500 Hz
//                      [00 01] = 1650 Hz
//                      [00 02] = 1800 Hz
//  width:  1A 05 13 02 [00 01]...[00 30], 100 TO 3000 Hz in 100 Hz steps
// LSBD2
//  offset: 1A 05 14 01 [00 00] = 1500 Hz
//                      [00 01] = 1650 Hz
//                      [00 02] = 1800 Hz
//  width:  1A 05 14 02 [00 01]...[00 30], 100 TO 3000 Hz in 100 Hz steps
// USBD2
//  offset: 1A 05 15 01 [00 00] = 1500 Hz
//                      [00 01] = 1650 Hz
//                      [00 02] = 1800 Hz
//  width:  1A 05 15 02 [00 01]...[00 30], 100 TO 3000 Hz in 100 Hz steps
// LSBD3
//  offset: 1A 05 16 01 [00 00] = 1500 Hz
//                      [00 01] = 1650 Hz
//                      [00 02] = 1800 Hz
//  width:  1A 05 16 02 [00 01]...[00 30], 100 TO 3000 Hz in 100 Hz steps
// USBD3
//  offset: 1A 05 17 01 [00 00] = 1500 Hz
//                      [00 01] = 1650 Hz
//                      [00 02] = 1800 Hz
//  width:  1A 05 17 02 [00 01]...[00 30], 100 TO 3000 Hz in 100 Hz steps

std::string RIG_ICF8101::get_BANDWIDTHS()
{
	stringstream s;
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_bwA[i] << " ";
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_bwB[i] << " ";
	return s.str();
}

void RIG_ICF8101::set_BANDWIDTHS(std::string s)
{
	stringstream strm;
	strm << s;
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_bwA[i];
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_bwB[i];
}

int RIG_ICF8101::adjust_bandwidth(int m)
{
	int iBW;
	switch (m) {
		case F81_AM:
			bandwidths_ = ICF8101_AM_widths;
			iBW = 29;
			break;
		case F81_CW:
			bandwidths_ = ICF8101_CW_SSB_widths;
			iBW = 9;
			break;
		case F81_RTTY:
			bandwidths_ = ICF8101_RTTY_widths;
			iBW = 0;
			break;
		case F81_LSB: case F81_USB:
		case F81_LSBD1: case F81_USBD1:
		case F81_LSBD2: case F81_USBD2:
		case F81_LSBD3: case F81_USBD3:
		default:
			bandwidths_ = ICF8101_CW_SSB_widths;
			iBW = 29;
	}
	return iBW;
}

int RIG_ICF8101::def_bandwidth(int m)
{
	int bw = adjust_bandwidth(m);
	if (useB) {
		if (mode_bwB[m] == -1)
			mode_bwB[m] = bw;
		return mode_bwB[m];
	}
	if (mode_bwA[m] == -1)
		mode_bwA[m] = bw;
	return mode_bwA[m];
}

const char ** RIG_ICF8101::bwtable(int m)
{
	switch (m) {
		case F81_AM:
			return ICF8101_AM_widths;
			break;
		case F81_RTTY:
			return ICF8101_RTTY_widths;
			break;
		case F81_CW:
		case F81_LSB: case F81_USB:
		case F81_LSBD1: case F81_USBD1:
		case F81_LSBD2: case F81_USBD2:
		case F81_LSBD3: case F81_USBD3:
		default:
			return ICF8101_CW_SSB_widths;
	}
	return ICF8101_CW_SSB_widths;
}

void RIG_ICF8101::set_BW(int m)
{
	cmd = pre_to;
	switch (m) {
		case F81_AM:
			cmd.append("\x1A\x05\x10\x02");
			cmd += '\x00';
			cmd += ICF8101_bw_vals_AM[m];
			break;
		case F81_RTTY:
			return;
			break;
		case F81_CW:
			cmd.append("\x1A\x05\x09\x02");
			cmd += '\x00';
			cmd += ICF8101_CW_SSB_width_vals[m];
			break;
		case F81_LSB: 
			cmd.append("\x1A\x05\x07\x02");
			cmd += '\x00';
			cmd += ICF8101_CW_SSB_width_vals[m];
			break;
		case F81_USB:
			cmd.append("\x1A\x05\x08\x02");
			cmd += '\x00';
			cmd += ICF8101_CW_SSB_width_vals[m];
			break;
		case F81_LSBD1:
			cmd.append("\x1A\x05\x12\x02");
			cmd += '\x00';
			cmd += ICF8101_CW_SSB_width_vals[m];
			break;
		case F81_USBD1:
			cmd.append("\x1A\x05\x13\x02");
			cmd += '\x00';
			cmd += ICF8101_CW_SSB_width_vals[m];
			break;
		case F81_LSBD2:
			cmd.append("\x1A\x05\x14\x02");
			cmd += '\x00';
			cmd += ICF8101_CW_SSB_width_vals[m];
			break;
		case F81_USBD2:
			cmd.append("\x1A\x05\x15\x02");
			cmd += '\x00';
			cmd += ICF8101_CW_SSB_width_vals[m];
			break;
		case F81_LSBD3:
			cmd.append("\x1A\x05\x15\x02");
			cmd += '\x00';
			cmd += ICF8101_CW_SSB_width_vals[m];
			break;
		case F81_USBD3:
			cmd.append("\x1A\x05\x16\x02");
			cmd += '\x00';
			cmd += ICF8101_CW_SSB_width_vals[m];
			break;
		default:
			cmd.append("\x1A\x05\x08\x02");
			cmd += '\x00';
			cmd += ICF8101_CW_SSB_width_vals[m];
			break;
	}
	cmd.append( post );
}

void RIG_ICF8101::set_bwA(int val)
{
	A.iBW = val;
	set_BW(val);
	waitFB("set BW A");
	mode_bwA[A.imode] = val;
	set_trace(4, "set_bwA() ", bwtable(A.imode)[val], ": ", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_ICF8101::set_bwB(int val)
{
	B.iBW = val;
	set_BW(val);
	waitFB("set BW B");
	mode_bwB[B.imode] = val;
	set_trace(4, "set_bwB() ", bwtable(B.imode)[val], ": ", str2hex(replystr.c_str(), replystr.length()));
}

int  RIG_ICF8101::get_BW(int m)
{
	cmd = pre_to;
	string resp = pre_fm;
	switch (m) {
		case F81_AM:
			cmd.append("\x1A\x05\x10\x02");
			resp.append("\x1A\x05\x10\x02");
			break;
		case F81_RTTY:
			return 0;
		case F81_CW:
			cmd.append("\x1A\x05\x09\x02");
			resp.append("\x1A\x05\x09\x02");
			break;
		case F81_LSB: 
			cmd.append("\x1A\x05\x07\x02");
			resp.append("\x1A\x05\x07\x02");
			break;
		case F81_USB:
			cmd.append("\x1A\x05\x08\x02");
			resp.append("\x1A\x05\x08\x02");
			break;
		case F81_LSBD1:
			cmd.append("\x1A\x05\x12\x02");
			resp.append("\x1A\x05\x12\x02");
			break;
		case F81_USBD1:
			cmd.append("\x1A\x05\x13\x02");
			resp.append("\x1A\x05\x13\x02");
			break;
		case F81_LSBD2:
			cmd.append("\x1A\x05\x14\x02");
			resp.append("\x1A\x05\x14\x02");
			break;
		case F81_USBD2:
			cmd.append("\x1A\x05\x15\x02");
			resp.append("\x1A\x05\x15\x02");
			break;
		case F81_LSBD3:
			cmd.append("\x1A\x05\x15\x02");
			resp.append("\x1A\x05\x15\x02");
			break;
		case F81_USBD3:
			cmd.append("\x1A\x05\x16\x02");
			resp.append("\x1A\x05\x16\x02");
			break;
		default:
			cmd.append("\x1A\x05\x08\x02");
			resp.append("\x1A\x05\x08\x02");
	}
	int bwval = 0;
	if (waitFOR(11, "get BW")) {
		get_trace(2, "get_bwA()", str2hex(replystr.c_str(), replystr.length()));
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			bwval = replystr[p+9];
			const char *vals = ICF8101_CW_SSB_width_vals;
			int n = NUM_CW_SSB_WIDTHS;
			if (m == F81_AM) {
				vals = ICF8101_bw_vals_AM;
				n = NUM_AM_WIDTHS;
			}
			for (int i = 0; i < n; n++) {
				if (bwval == vals[i]) {
					return i;
				}
			}
		}
	}
	get_trace(2, "get_bwA()", str2hex(replystr.c_str(), replystr.length()));
	return 0;
}

int  RIG_ICF8101::get_bwA()
{
	int bw = get_BW(A.imode);
	if (bw) {
		A.iBW = bw;
		mode_bwA[A.imode] = A.iBW;
	}
	return A.iBW;
}

int  RIG_ICF8101::get_bwB()
{
	int bw = get_BW(B.imode);
	if (bw) {
		B.iBW = bw;
		mode_bwB[B.imode] = B.iBW;
	}
	return B.iBW;
}
