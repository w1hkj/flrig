// ----------------------------------------------------------------------------
// Copyright (C) 2018
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

#include <string>
#include <sstream>

#include "IC7610.h"
#include "support.h"
#include "trace.h"

//=============================================================================
// IC-7610

const char IC7610name_[] = "IC-7610";

// these are only defined in this file
// undef'd at end of file
#define NUM_FILTERS 3
#define NUM_MODES  22

static int mode_filterA[NUM_MODES] = {
	1,1,1,1,1,
	1,1,1,1,1,
	1,1,1,1,
	1,1,1,1,
	1,1,1,1};
static int mode_filterB[NUM_MODES] = {
	1,1,1,1,1,
	1,1,1,1,1,
	1,1,1,1,
	1,1,1,1,
	1,1,1,1};

static int mode_bwA[NUM_MODES] = {
	-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,
	-1,-1,-1,-1,
	-1,-1,-1,-1,
	-1,-1,-1,-1};
static int mode_bwB[NUM_MODES] = {
	-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,
	-1,-1,-1,-1,
	-1,-1,-1,-1,
	-1,-1,-1,-1};

static const char *szfilter[NUM_FILTERS] = {"1", "2", "3"};

enum { 
m7610LSB, m7610USB, m7610AM, m7610FM, m7610CW,
m7610CWR, m7610RTTY, m7610RTTYR, m7610PSK, m7610PSKR,
m7610LSBD1, m7610USBD1, m7610AMD1, m7610FMD1,
m7610LSBD2, m7610USBD2, m7610AMD2, m7610FMD2,
m7610LSBD3, m7610USBD3, m7610AMD3, m7610FM3D
};

const char *IC7610modes_[] = {
	"LSB", "USB", "AM", "FM", "CW", 
	"CW-R", "RTTY", "RTTY-R", "PSK", "PSK-R",
	"LSB-D1", "USB-D1", "AM-D1", "FM-D1",
	"LSB-D2", "USB-D2", "AM-D2", "FM-D2",
	"LSB-D3", "USB-D3", "AM-D3", "FM-D3",
	NULL};

const char IC7610_mode_type[] = {
	'L', 'U', 'U', 'U', 'L', 'U', 'L', 'U',
	'L', 'U', 'U', 'U' };

const char IC7610_mode_nbr[] = {
	0x00, 0x01, 0x02, 0x05, 0x03, 0x07, 0x04, 0x08, 0x12, 0x13,
	0x00, 0x01, 0x02, 0x05 };

const char *IC7610_ssb_bws[] = {
"50",    "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700", "2800", "2900", "3000", "3100", "3200", "3300", "3400", "3500",
"3600", NULL };
static int IC7610_bw_vals_SSB[] = {
 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,
30,31,32,33,34,35,36,37,38,39,
40, WVALS_LIMIT};

const char *IC7610_rtty_bws[] = {
"50",    "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700", NULL };
static int IC7610_bw_vals_RTTY[] = {
 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,
30,31, WVALS_LIMIT};

const char *IC7610_am_bws[] = {
"200",   "400",  "600",  "800", "1000", "1200", "1400", "1600", "1800", "2000",
"2200", "2400", "2600", "2800", "3000", "3200", "3400", "3600", "3800", "4000",
"4200", "4400", "4600", "4800", "5000", "5200", "5400", "5600", "5800", "6000",
"6200", "6400", "6600", "6800", "7000", "7610", "7400", "7610", "7800", "8000",
"8200", "8400", "8600", "8800", "9000", "9200", "9400", "9600", "9800", "10000", NULL };
static int IC7610_bw_vals_AM[] = {
 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,
30,31,32,33,34,35,36,37,38,39,
40,41,42,43,44,45,46,47,48,49
WVALS_LIMIT};

const char *IC7610_fm_bws[] = { "FIXED", NULL };
static int IC7610_bw_vals_FM[] = { 1, WVALS_LIMIT};

static GUI IC7610_widgets[]= {
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

void RIG_IC7610::initialize()
{
	IC7610_widgets[0].W = btnVol;
	IC7610_widgets[1].W = sldrVOLUME;
	IC7610_widgets[2].W = btnAGC;
	IC7610_widgets[3].W = sldrRFGAIN;
	IC7610_widgets[4].W = sldrSQUELCH;
	IC7610_widgets[5].W = btnNR;
	IC7610_widgets[6].W = sldrNR;
	IC7610_widgets[7].W = btnLOCK;
	IC7610_widgets[8].W = sldrINNER;
	IC7610_widgets[9].W = btnCLRPBT;
	IC7610_widgets[10].W = sldrOUTER;
	IC7610_widgets[11].W = btnNotch;
	IC7610_widgets[12].W = sldrNOTCH;
	IC7610_widgets[13].W = sldrMICGAIN;
	IC7610_widgets[14].W = sldrPOWER;

	btn_icom_select_11->deactivate();
	btn_icom_select_12->deactivate();
	btn_icom_select_13->deactivate();

	choice_rTONE->activate();
	choice_tTONE->activate();
}

RIG_IC7610::RIG_IC7610() {
	defaultCIV = 0x98;
	adjustCIV(defaultCIV);

	name_ = IC7610name_;
	modes_ = IC7610modes_;
	bandwidths_ = IC7610_ssb_bws;
	bw_vals_ = IC7610_bw_vals_SSB;

	_mode_type = IC7610_mode_type;

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

	widgets = IC7610_widgets;

	def_freq = A.freq = 14070000;
	def_mode = A.imode = 9;
	def_bw = A.iBW = 34;

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

	has_pbt_controls = true;
	has_FILTER = true;

	has_rf_control = true;

	has_ptt_control = true;
	has_tune_control = true;

	precision = 1;
	ndigits = 8;

	has_vfo_adj = true;

	has_xcvr_auto_on_off = true;

	has_band_selection = true;

	can_change_alt_vfo = true;
	has_a2b = true;

};

static inline void minmax(int min, int max, int &val)
{
	if (val > max) val = max;
	if (val < min) val = min;
}

void RIG_IC7610::selectA()
{
	cmd.assign(pre_to).append("\x07");
	cmd += '\xD0';
	cmd.append(post);
	waitFB("select A");

	set_trace(2, "selectA() ", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC7610::selectB()
{
	cmd.assign(pre_to).append("\x07");
	cmd += '\xD1';
	cmd.append(post);
	waitFB("select B");

	set_trace(2, "selectB() ",str2hex(replystr.c_str(), replystr.length()));
}

//======================================================================
// IC7610 unique commands
//======================================================================

void RIG_IC7610::swapAB()
{
	cmd = pre_to;
	cmd += 0x07; cmd += 0xB0;
	cmd.append(post);
	waitFB("Exchange vfos");
	get_modeA(); // get mode to update the filter A / B usage
	get_modeB();
}

void RIG_IC7610::set_xcvr_auto_on()
{
	cmd.clear();
	int fes[] = { 2, 2, 2, 3, 7, 13, 25, 50, 75, 150, 150, 150 };
	if (progStatus.comm_baudrate >= 0 && progStatus.comm_baudrate <= 11) {
		cmd.append( fes[progStatus.comm_baudrate], '\xFE');
	}
	cmd.append(pre_to);
	cmd += '\x18'; cmd += '\x01';
	cmd.append(post);
	waitFB("Power ON", 200);

	cmd = pre_to;
	cmd += '\x19'; cmd += '\x00';
	cmd.append(post);
	waitFOR(8, "get ID", 10000);
}

void RIG_IC7610::set_xcvr_auto_off()
{
	cmd.clear();
	cmd.append(pre_to);
	cmd += '\x18'; cmd += '\x00';
	cmd.append(post);
	waitFB("Power OFF", 200);
}

bool RIG_IC7610::check ()
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

// FE FE 98 E0 25 00 XX XX XX XX 00 FD
// FE FE 98 E0 25 01 XX XX XX XX 00 FD
//                   || || || ||________ 1   MHz digit
//                   || || || |_________ 10  MHz digit
//                   || || ||___________ 10  kHz digit
//                   || || |____________ 100 kHz digit
//                   || ||______________ 100 Hz  digit
//                   || |_______________ 1   kHz digit
//                   ||_________________ 1   Hz  digit
//                   |__________________ 10  Hz  digit

long RIG_IC7610::get_vfoA ()
{
	string resp;

	resp.assign(pre_fm).append("\x25");
	resp += '\x00';

	cmd.assign(pre_to).append("\x25");
	cmd  += '\x00';
	cmd.append(post);
	if (waitFOR(12, "get vfo A")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			A.freq = fm_bcd_be(replystr.substr(p+6), 8);
	}

	get_trace(2, "get_vfoA() ", str2hex(replystr.c_str(), replystr.length()));

	return A.freq;
}

void RIG_IC7610::set_vfoA (long freq)
{
	A.freq = freq;

	cmd.assign(pre_to).append("\x25");
	cmd += '\x00';
	cmd.append( to_bcd_be( freq, 8) );
	cmd += '\x00';
	cmd.append( post );
	waitFB("set vfo A");

	set_trace(2, "set_vfoA() ", str2hex(replystr.c_str(), replystr.length()));

}

long RIG_IC7610::get_vfoB ()
{
	string resp;

	resp.assign(pre_fm).append("\x25");
	resp += '\x01';

	cmd.assign(pre_to).append("\x25");
	cmd  += '\x01';
	cmd.append(post);
	if (waitFOR(12, "get vfo B")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			B.freq = fm_bcd_be(replystr.substr(p+6), 8);
	}

	get_trace(2, "get_vfoB() ", str2hex(replystr.c_str(), replystr.length()));

	return B.freq;
}

void RIG_IC7610::set_vfoB (long freq)
{
	B.freq = freq;

	cmd.assign(pre_to).append("\x25");
	cmd += '\x01';
	cmd.append( to_bcd_be( freq, 8 ) );
	cmd += '\x00';
	cmd.append( post );
	waitFB("set vfo B");

	set_trace(2, "set_vfoB() ", str2hex(replystr.c_str(), replystr.length()));
}

// expecting
//  0  1  2  3  4  5  6  7  8  9
// FE FE E0 94 26 NN NN NN NN FD
//                |  |  |  |
//                |  |  |  |__filter setting, 01, 02, 03
//                |  |  |_____data mode, 00 - off, 01 - data mode 1
//                |  |  |_____02 - data mode 2, 03 - data mode 3
//                |  |________Mode 00 - LSB
//                |                01 - USB
//                |                02 - AM
//                |                03 - CW
//                |                04 - RTTY
//                |                05 - FM
//                |                07 - CW-R
//                |                08 - RTTY-R
//                |___________selected vfo, 00 - active, 01 - inactive

int RIG_IC7610::get_modeA()
{
	int md = 0;
	size_t p;

	string resp;
	resp.assign(pre_fm).append("\x26");

	cmd.assign(pre_to).append("\x26");
	cmd += '\x00';
	cmd.append(post);

	if (waitFOR(10, "get mode A")) {
		p = replystr.rfind(resp);
		if (p == string::npos)
			goto end_wait_modeA;

		for (md = 0; md < m7610LSBD1; md++) {
			if (replystr[p+6] == IC7610_mode_nbr[md]) {
				A.imode = md;
				if (replystr[p+7] == 0x01 && A.imode < 4)
					A.imode += 10;
				if (replystr[p+7] == 0x02 && A.imode < 4)
					A.imode += 14;
				if (replystr[p+7] == 0x03 && A.imode < 4)
					A.imode += 18;
				if (A.imode > 21)
					A.imode = 1;
				break;
			}
		}
		A.filter = replystr[p+8];
	}

end_wait_modeA:
	get_trace(4, 
		"get mode A[",
		IC7610modes_[A.imode], 
		"] ", 
		str2hex(replystr.c_str(), replystr.length()));

	mode_filterA[A.imode] = A.filter;

	return A.imode;
}

// LSB  USB  AM   CW  RTTY  FM  CW-R  RTTY-R  LSB-D  USB-D
//  0    1    2    3   4     5    6     7      8      9

void RIG_IC7610::set_modeA(int val)
{
	A.imode = val;
	cmd.assign(pre_to);
	cmd += '\x26';
	cmd += '\x00';
	cmd += IC7610_mode_nbr[A.imode];	// operating mode
	if (A.imode >= m7610LSBD3)
		cmd += '\x03';					// data mode D1
	else if (A.imode >= m7610LSBD2)
		cmd += '\x02';					// data mode D2
	else if (A.imode >= m7610LSBD1)
		cmd += '\x01';					// data mode D1
	else
		cmd += '\x00';
	cmd += mode_filterA[A.imode];		// filter
	cmd.append( post );
	waitFB("set mode A");

	set_trace(4, 
		"set mode A[",
		IC7610modes_[A.imode], 
		"] ", 
		str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC7610::get_modeB()
{
	int md = 0;
	size_t p;

	string resp;
	resp.assign(pre_fm).append("\x26");

	cmd.assign(pre_to).append("\x26");
	cmd += '\x01';
	cmd.append(post);

	if (waitFOR(10, "get mode B")) {
		p = replystr.rfind(resp);
		if (p == string::npos)
			goto end_wait_modeB;

		for (md = 0; md < m7610LSBD1; md++) {
			if (replystr[p+6] == IC7610_mode_nbr[md]) {
				B.imode = md;
				if (replystr[p+7] == 0x01 && B.imode < 4)
					B.imode += 10;
				if (replystr[p+7] == 0x02 && B.imode < 4)
					B.imode += 14;
				if (replystr[p+7] == 0x03 && B.imode < 4)
					B.imode += 18;
				if (B.imode > 21)
					B.imode = 1;
				break;
			}
		}
		B.filter = replystr[p+8];
	}

end_wait_modeB:
	get_trace(4, 
		"get mode B[",
		IC7610modes_[B.imode], 
		"] ", 
		str2hex(replystr.c_str(), replystr.length()));

	mode_filterB[B.imode] = B.filter;

	return B.imode;
}

void RIG_IC7610::set_modeB(int val)
{
	B.imode = val;
	cmd.assign(pre_to);
	cmd += '\x26';
	cmd += '\x01';
	cmd += IC7610_mode_nbr[B.imode];	// operating mode
	if (B.imode >= m7610LSBD3)
		cmd += '\x03';					// data mode D1
	else if (B.imode >= m7610LSBD2)
		cmd += '\x02';					// data mode D2
	else if (B.imode >= m7610LSBD1)
		cmd += '\x01';					// data mode D1
	else
		cmd += '\x00';
	cmd += mode_filterB[B.imode];		// filter
	cmd.append( post );
	waitFB("set mode B");

	set_trace(4, 
		"set mode B[",
		IC7610modes_[B.imode], 
		"] ", 
		str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC7610::get_FILT(int mode)
{
	if (useB) return mode_filterB[mode];
	return mode_filterA[mode];
}

void RIG_IC7610::set_FILT(int filter)
{
	if (useB) {
		B.filter = filter;
		mode_filterB[B.imode] = filter;
		cmd.assign(pre_to);
		cmd += '\x26';
		cmd += '\x01';
		cmd += IC7610_mode_nbr[B.imode];	// operating mode
		if (B.imode >= m7610LSBD3)
			cmd += '\x03';					// data mode D1
		else if (B.imode >= m7610LSBD2)
			cmd += '\x02';					// data mode D2
		else if (B.imode >= m7610LSBD1)
			cmd += '\x01';					// data mode D1
		else
			cmd += '\x00';
		cmd += filter;						// filter
		cmd.append( post );
		waitFB("set mode/filter B");

		set_trace(4, 
			"set mode/filter B[",
			IC7610modes_[B.imode], 
			"] ", 
			str2hex(replystr.c_str(), replystr.length()));
	} else {
		A.filter = filter;
		mode_filterA[A.imode] = filter;
		cmd.assign(pre_to);
		cmd += '\x26';
		cmd += '\x00';						// selected vfo
		cmd += IC7610_mode_nbr[A.imode];	// operating mode
		if (A.imode >= m7610LSBD3)
			cmd += '\x03';					// data mode D1
		else if (A.imode >= m7610LSBD2)
			cmd += '\x02';					// data mode D2
		else if (A.imode >= m7610LSBD1)
			cmd += '\x01';					// data mode D1
		else
			cmd += '\x00';
		cmd += filter;						// filter
		cmd.append( post );
		waitFB("set mode/filter A");

		set_trace(4, "set mode/filter A[",
			IC7610modes_[A.imode], 
			"] ", 
			str2hex(replystr.c_str(), replystr.length()));
	}
}

const char *RIG_IC7610::FILT(int val)
{
	if (val < 1) val = 1;
	if (val > 3) val = 3;
	return(szfilter[val - 1]);
}

const char * RIG_IC7610::nextFILT()
{
	int val = A.filter;
	if (useB) val = B.filter;
	val++;
	if (val > 3) val = 1;
	set_FILT(val);
	return szfilter[val - 1];
}

void RIG_IC7610::set_FILTERS(std::string s)
{
	stringstream strm;
	strm << s;
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_filterA[i];
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_filterB[i];
}

std::string RIG_IC7610::get_FILTERS()
{
	stringstream s;
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_filterA[i] << " ";
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_filterB[i] << " ";
	return s.str();
}

std::string RIG_IC7610::get_BANDWIDTHS()
{
	stringstream s;
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_bwA[i] << " ";
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_bwB[i] << " ";
	return s.str();
}

void RIG_IC7610::set_BANDWIDTHS(std::string s)
{
	stringstream strm;
	strm << s;
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_bwA[i];
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_bwB[i];
}

bool RIG_IC7610::can_split()
{
	return true;
}

void RIG_IC7610::set_split(bool val)
{
	split = val;
	cmd = pre_to;
	cmd += 0x0F;
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	waitFB(val ? "set split ON" : "set split OFF");

	set_trace(2, "set_split() ", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC7610::get_split()
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

	get_trace(2, "get_split() ", str2hex(replystr.c_str(), replystr.length()));

	return split;
}

int RIG_IC7610::get_bwA()
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
		mode_bwA[A.imode] = bwval;
	}

	if (useB) selectB();

	get_trace(2, "get_bwA() ", str2hex(replystr.c_str(), replystr.length()));

	return A.iBW;
}

void RIG_IC7610::set_bwA(int val)
{

	if (A.imode == 3 || A.imode == 11) return; // FM, FM-D

	A.iBW = val;
	if (useB) selectA();

	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(to_bcd(A.iBW, 2));
	cmd.append(post);
	waitFB("set bwA");

	mode_bwA[A.imode] = val;
	set_trace(2, "set_bwA() ", str2hex(replystr.c_str(), replystr.length()));

	if (useB) selectB();
}

int RIG_IC7610::get_bwB()
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
		mode_bwB[B.imode] = bwval;
	}

	if (!useB) selectA();

	get_trace(2, "get_bwB() ", str2hex(replystr.c_str(), replystr.length()));

	return B.iBW;
}

void RIG_IC7610::set_bwB(int val)
{
	if (B.imode == 3 || B.imode == 11) return; // FM, FM-D
	B.iBW = val;

	if (!useB) selectB();

	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(to_bcd(B.iBW, 2));
	cmd.append(post);
	waitFB("set bwB");

	mode_bwB[B.imode] = val;
	set_trace(2, "set_bwB() ", str2hex(replystr.c_str(), replystr.length()));

	if (!useB) selectA();
}

// LSB  USB  AM   FM   CW  CW-R  RTTY  RTTY-R  PSK   PSK-R
//  0    1    2    3   4     5    6     7      8      9
// 10   11   12   13   // DATA-1
// 14   15   16   17   // DATA-2
// 18   19   20   21   // DATA-3

int RIG_IC7610::adjust_bandwidth(int m)
{
	int bw = 0;
	switch (m) {
		case 2: case 12: case 16: case 20: // AM, AM-D
			bandwidths_ = IC7610_am_bws;
			bw_vals_ = IC7610_bw_vals_AM;
			bw = 19;
			break;
		case 3: case 13: case 17: case 21: // FM, FM-D
			bandwidths_ = IC7610_fm_bws;
			bw_vals_ = IC7610_bw_vals_FM;
			bw = 0;
			break;
		case 4: case 5: // CW, CW -R
			bandwidths_ = IC7610_ssb_bws;
			bw_vals_ = IC7610_bw_vals_SSB;
			bw = 12;
			break;
		case 6: case 7: // RTTY, RTTY-R
			bandwidths_ = IC7610_rtty_bws;
			bw_vals_ = IC7610_bw_vals_RTTY;
			bw = 12;
			break;
		case 0: case 1: case 8: case 9: // LSB, USB, PSK, PSK-R
		case 10: case 11: // DATA-1
		case 14: case 15: // DATA-1
		case 18: case 19: // DATA-1
		default:
			bandwidths_ = IC7610_ssb_bws;
			bw_vals_ = IC7610_bw_vals_SSB;
			bw = 34;
	}
	return bw;
}

const char ** RIG_IC7610::bwtable(int m)
{
	const char **table;
	switch (m) {
		case 2: case 12: case 16: case 20: // AM, AM-D
			table = IC7610_am_bws;
			break;
		case 3: case 13: case 17: case 21: // FM, FM-D
			table = IC7610_fm_bws;
			break;
		case 6: case 7: // RTTY, RTTY-R
			table = IC7610_rtty_bws;
			break;
		case 4: case 5: // CW, CW -R
		case 0: case 1: case 8: case 9: // LSB, USB, PSK, PSK-R
		case 10: case 11: // DATA-1
		case 14: case 15: // DATA-1
		case 18: case 19: // DATA-1
		default:
			table = IC7610_ssb_bws;
	}
	return table;
}

int RIG_IC7610::def_bandwidth(int m)
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

int RIG_IC7610::get_mic_gain()
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
	get_trace(2, "get_mic_gain() ", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

void RIG_IC7610::set_mic_gain(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x0B");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set mic gain");
	set_trace(2, "set_mic_gain() ", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC7610::get_mic_gain_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

static int comp_level[] = {11,34,58,81,104,128,151,174,197,221,244};
void RIG_IC7610::set_compression(int on, int val)
{
	cmd = pre_to;
	cmd.append("\x16\x44");
	if (on) cmd += '\x01';
	else cmd += '\x00';
	cmd.append(post);
	waitFB("set Comp ON/OFF");
	set_trace(2, "set_compression on/off ", str2hex(replystr.c_str(), replystr.length()));

	if (val < 0) return;
	if (val > 10) return;

	cmd.assign(pre_to).append("\x14\x0E");
	cmd.append(to_bcd(comp_level[val], 3));
	cmd.append( post );
	waitFB("set comp");
	set_trace(2, "set_compression_level ", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC7610::get_compression(int &on, int &val)
{
	std::string resp;

	cmd.assign(pre_to).append("\x16\x44").append(post);

	resp.assign(pre_fm).append("\x16\x44");

	if (waitFOR(8, "get comp on/off")) {
		size_t p = replystr.find(resp);
		if (p != string::npos)
			on = (replystr[p+6] == 0x01);
	}
	get_trace(2, "get_comp on/off() ", str2hex(replystr.c_str(), replystr.length()));

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
	get_trace(2, "get_comp_level() ", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC7610::set_vox_onoff()
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
	set_trace(2, "set_vox_on/off ", str2hex(replystr.c_str(), replystr.length()));
}

// Xcvr values range 0...255 step 1
void RIG_IC7610::get_vox_gain_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

void RIG_IC7610::set_vox_gain()
{
	int vox_gain = round((progStatus.vox_gain * 255 / 100 + 0.5));
	minmax(0, 255, vox_gain);
	cmd.assign(pre_to).append("\x14\x16");
	cmd.append(to_bcd(vox_gain, 3));
	cmd.append( post );
	waitFB("SET vox gain");
	set_trace(2, "set_vox_gain() ", str2hex(replystr.c_str(), replystr.length()));
}

// Xcvr values range 0...255 step 1
void RIG_IC7610::get_vox_anti_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

void RIG_IC7610::set_vox_anti()
{
	int vox_anti = round((progStatus.vox_anti * 255 / 100 + 0.5));
	minmax(0, 255, vox_anti);
	cmd.assign(pre_to).append("\x14\x17");
	cmd.append(to_bcd(vox_anti, 3));
	cmd.append( post );
	waitFB("SET anti-vox");
	set_trace(2, "set_vox_anti() ", str2hex(replystr.c_str(), replystr.length()));
}

// VOX hang 0.0 - 2.0, step 0.1
// Xcvr values 0..20 step 1
void RIG_IC7610::get_vox_hang_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 20; step = 1;
}

void RIG_IC7610::set_vox_hang()
{
	cmd.assign(pre_to).append("\x1A\x05\x02\x92");
	cmd.append(to_bcd(progStatus.vox_hang, 2));
	cmd.append( post );
	waitFB("SET vox hang");
	set_trace(2, "set_vox_hang() ", str2hex(replystr.c_str(), replystr.length()));
}

//----------------------------------------------------------------------
// CW controls

void RIG_IC7610::get_cw_wpm_min_max(int &min, int &max)
{
	min = 6; max = 48;
}

void RIG_IC7610::set_cw_wpm()
{
	int iwpm = round((progStatus.cw_wpm - 6) * 255 / 42 + 0.5);
	minmax(0, 255, iwpm);

	cmd.assign(pre_to).append("\x14\x0C");
	cmd.append(to_bcd(iwpm, 3));
	cmd.append( post );
	waitFB("SET cw wpm");
	set_trace(2, "set_cw_wpm() ", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC7610::enable_break_in()
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
	set_trace(2, "set_break_in() ", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC7610::get_cw_qsk_min_max_step(double &min, double &max, double &step)
{
	min = 2.0; max = 13.0; step = 0.1;
}

void RIG_IC7610::set_cw_qsk()
{
	int qsk = round ((progStatus.cw_qsk - 2.0) * 255.0 / 11.0 + 0.5);
	minmax(0, 255, qsk);

	cmd.assign(pre_to).append("\x14\x0F");
	cmd.append(to_bcd(qsk, 3));
	cmd.append(post);
	waitFB("Set cw qsk delay");
	set_trace(2, "set_cw_qsk() ", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC7610::get_cw_spot_tone_min_max_step(int &min, int &max, int &step)
{
	min = 300; max = 900; step = 5;
}

void RIG_IC7610::set_cw_spot_tone()
{
	cmd.assign(pre_to).append("\x14\x09"); // values 0=300Hz 255=900Hz
	int n = round((progStatus.cw_spot_tone - 300) * 255.0 / 600.0 + 0.5);
	minmax(0, 255, n);

	cmd.append(to_bcd(n, 3));
	cmd.append( post );
	waitFB("SET cw spot tone");
	set_trace(2, "set_cw_spot_tone() ", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC7610::set_cw_vol()
{
	cmd.assign(pre_to);
	cmd.append("\x1A\x05");
	cmd += '\x02';
	cmd += '\x25';
	cmd.append(to_bcd((int)(progStatus.cw_vol * 2.55), 3));
	cmd.append( post );
	waitFB("SET cw sidetone volume");
	set_trace(2, "set_cw_vol() ", str2hex(replystr.c_str(), replystr.length()));
}

// Tranceiver PTT on/off
void RIG_IC7610::set_PTT_control(int val)
{
	cmd = pre_to;
	cmd += '\x1c';
	cmd += '\x00';
	cmd += (unsigned char) val;
	cmd.append( post );
	waitFB("set ptt");
	ptt_ = val;
	set_trace(2, "set_PTT_control() ", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC7610::get_PTT()
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
	get_trace(2, "get_PTT() ", str2hex(replystr.c_str(), replystr.length()));
	return ptt_;
}

// Volume control val 0 ... 100
void RIG_IC7610::set_volume_control(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set vol");
	set_trace(2, "set_volume_control() ", str2hex(replystr.c_str(), replystr.length()));
}

/*

I:12:20:22: get vol ans in 0 ms, OK
cmd FE FE 7A E0 14 01 FD
ans FE FE 7A E0 14 01 FD
FE FE E0 7A 14 01 00 65 FD
 0  1  2  3  4  5  6  7  8
*/
int RIG_IC7610::get_volume_control()
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
	get_trace(2, "get_volume_control() ", str2hex(replystr.c_str(), replystr.length()));
	return (val);
}

void RIG_IC7610::get_vol_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

void RIG_IC7610::set_power_control(double val)
{
	cmd = pre_to;
	cmd.append("\x14\x0A");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set power");
	set_trace(2, "set_power_control() ", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC7610::get_power_control()
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
	get_trace(2, "get_power_control() ", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

void RIG_IC7610::get_pc_min_max_step(double &min, double &max, double &step)
{
	min = 2; max = 100; step = 1;
}

int RIG_IC7610::get_smeter()
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
	{21, 5.0},
	{43,10.0}, 
	{65, 15.0},
	{83, 20.0}, 
	{95, 25.0}, 
	{105, 30.0},
	{114, 35.0}, 
	{124, 40.0}, 
	{143, 50.0}, 
	{183, 75.0},
	{212, 100.0},
	{255, 120.0 } };

int RIG_IC7610::get_power_out(void)
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

struct swrpair {int mtr; float swr;};

// Table entries below correspond to SWR readings of 1.1, 1.5, 2.0, 2.5, 3.0 and infinity.
// Values are also tweaked to fit the display of the SWR meter.


static swrpair swrtbl[] = { 
	{0, 0.0},
	{48, 10.5},
	{80, 23.0}, 
	{103, 35.0},
	{120, 48.0},
	{255, 100.0 } };



int RIG_IC7610::get_swr(void)
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
			size_t i = 0;
			for (i = 0; i < sizeof(swrtbl) / sizeof(swrpair) - 1; i++)
				if (mtr >= swrtbl[i].mtr && mtr < swrtbl[i+1].mtr)
					break;
			if (mtr < 0) mtr = 0;
			if (mtr > 255) mtr = 255;
			mtr = (int)ceil(swrtbl[i].swr + 
				(swrtbl[i+1].swr - swrtbl[i].swr)*(mtr - swrtbl[i].mtr)/(swrtbl[i+1].mtr - swrtbl[i].mtr));

			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

int RIG_IC7610::get_alc(void)
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

void RIG_IC7610::set_rf_gain(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x02");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set RF");
	set_trace(2, "set_rf_gain() ", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC7610::get_rf_gain()
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
	get_trace(2, "get_rf_gain() ", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

void RIG_IC7610::get_rf_min_max_step(double &min, double &max, double &step)
{
	min = 0; max = 100; step = 1;
}

int RIG_IC7610::next_preamp()
{
	switch (preamp_level) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 0;
	}
	return 0;
}

void RIG_IC7610::set_preamp(int val)
{
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
			preamp_label("Pre", false);
	}

	cmd += (unsigned char)preamp_level;
	cmd.append( post );
	waitFB(	(preamp_level == 0) ? "set Preamp OFF" :
			(preamp_level == 1) ? "set Preamp Level 1" :
			"set Preamp Level 2");
	set_trace(2, "set_preamp() ", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC7610::get_preamp()
{
	string cstr = "\x16\x02";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get Preamp Level")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			preamp_level = replystr[p+6];
			if (preamp_level == 1) {
				preamp_label("Amp 1", true);
			} else if (preamp_level == 2) {
				preamp_label("Amp 2", true);
			} else {
				preamp_label("Pre", false);
				preamp_level = 0;
			}
		}
	}
	get_trace(2, "get_preamp() ", str2hex(replystr.c_str(), replystr.length()));
	return preamp_level;
}

void RIG_IC7610::set_noise(bool val)
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set noise");
	set_trace(2, "set_noise() ", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC7610::get_noise()
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
	get_trace(2, "get_noise() ", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

void RIG_IC7610::set_nb_level(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x12");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set NB level");
	set_trace(2, "set_nb_level() ", str2hex(replystr.c_str(), replystr.length()));
}

int  RIG_IC7610::get_nb_level()
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
	get_trace(2, "get_nb_level() ", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

void RIG_IC7610::set_noise_reduction(int val)
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set NR");
	set_trace(2, "set_noise_reduction() ", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC7610::get_noise_reduction()
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
	get_trace(2, "get_noise_reduction() ", str2hex(replystr.c_str(), replystr.length()));
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

void RIG_IC7610::set_noise_reduction_val(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x06");
	val *= 16;
	val += 8;
	cmd.append(to_bcd(val, 3));
	cmd.append(post);
	waitFB("set NRval");
	set_trace(2, "set_noise_reduction_val() ", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC7610::get_noise_reduction_val()
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
	get_trace(2, "get_noise_reduction_val() ", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

void RIG_IC7610::set_squelch(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x03");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set Sqlch");
	set_trace(2, "set_squelch() ", str2hex(replystr.c_str(), replystr.length()));
}

int  RIG_IC7610::get_squelch()
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
	get_trace(2, "get_squelch() ", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

void RIG_IC7610::set_auto_notch(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x41';
	cmd += (unsigned char)val;
	cmd.append( post );
	waitFB("set AN");
	set_trace(2, "set_auto_notch() ", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC7610::get_auto_notch()
{
	string cstr = "\x16\x41";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get AN")) {
		get_trace(2, "get_auto_notch() ", str2hex(replystr.c_str(), replystr.length()));
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

static bool IC7610_notchon = false;

void RIG_IC7610::set_notch(bool on, int val)
{
	int notch = val / 20 + 53;
	minmax(0, 255, notch);
	if (on != IC7610_notchon) {
		cmd = pre_to;
		cmd.append("\x16\x48");
		cmd += on ? '\x01' : '\x00';
		cmd.append(post);
		waitFB("set notch");
		IC7610_notchon = on;
		set_trace(2, "set_notch_on/off() ", str2hex(replystr.c_str(), replystr.length()));
	}
	cmd = pre_to;
	cmd.append("\x14\x0D");
	cmd.append(to_bcd(notch,3));
	cmd.append(post);
	waitFB("set notch val");
	set_trace(2, "set_notch_val() ", str2hex(replystr.c_str(), replystr.length()));
}

bool RIG_IC7610::get_notch(int &val)
{
	bool on = false;

	string cstr = "\x16\x48";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get notch")) {
		get_trace(2, "get_notch() ", str2hex(replystr.c_str(), replystr.length()));
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
			get_trace(2, "get_notch_val() ", str2hex(replystr.c_str(), replystr.length()));
			size_t p = replystr.rfind(resp);
			if (p != string::npos) {
				val = fm_bcd(replystr.substr(p+6),3);
				val = (val - 53) * 20;
				if (val < 0) val = 0;
				if (val > 4040) val = 4040;
			}
		}
	}
	return (IC7610_notchon = on);
}

void RIG_IC7610::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 4040;
	step = 20;
}

static int agcval = 3;
int  RIG_IC7610::get_agc()
{
	cmd = pre_to;
	cmd.append("\x16\x12");
	cmd.append(post);
	if (waitFOR(8, "get AGC")) {
		size_t p = replystr.find(pre_fm);
		if (p != string::npos)
			agcval = replystr[p+6]; // 1 == off, 2 = FAST, 3 = MED, 4 = SLOW
	}
	get_trace(2, "get_agc() ", str2hex(replystr.c_str(), replystr.length()));
	return agcval;
}

int RIG_IC7610::incr_agc()
{
	agcval++;
	if (agcval == 4) agcval = 1;
	cmd = pre_to;
	cmd.append("\x16\x12");
	cmd += agcval;
	cmd.append(post);
	waitFB("set AGC");
	set_trace(2, "set_agc() ", str2hex(replystr.c_str(), replystr.length()));
	return agcval;
}


static const char *agcstrs[] = {"AGC", "FST", "MED", "SLO"};
const char *RIG_IC7610::agc_label()
{
	return agcstrs[agcval];
}

int  RIG_IC7610::agc_val()
{
	return (agcval);
}

void RIG_IC7610::set_if_shift(int val)
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
	set_trace(2, "set_IF_shift_on/off() ", str2hex(replystr.c_str(), replystr.length()));

	cmd = pre_to;
	cmd.append("\x14\x08");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set IF val");
	set_trace(2, "set_IF_shift_val() ", str2hex(replystr.c_str(), replystr.length()));
}

bool RIG_IC7610::get_if_shift(int &val) {
	val = sh_;
	return sh_on_;
}

void RIG_IC7610::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -50;
	max = +50;
	step = 1;
}

void RIG_IC7610::set_pbt_inner(int val)
{
	int shift = 128 + val * 128 / 50;
	if (shift < 0) shift = 0;
	if (shift > 255) shift = 255;

	cmd = pre_to;
	cmd.append("\x14\x07");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set PBT inner");
	set_trace(4, "set_pbt_inner(", val, ") ", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC7610::set_pbt_outer(int val)
{
	int shift = 128 + val * 128 / 50;
	if (shift < 0) shift = 0;
	if (shift > 255) shift = 255;

	cmd = pre_to;
	cmd.append("\x14\x08");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set PBT outer");
	set_trace(4, "set_pbt_outer(", val, ") ", str2hex(cmd.c_str(), cmd.length()));
}

int RIG_IC7610::get_pbt_inner()
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
	get_trace(2, "get_pbt_inner()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

int RIG_IC7610::get_pbt_outer()
{
	get_dual_watch();
	get_digi_sel();

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
	get_trace(2, "get_pbt_outer()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

void RIG_IC7610::setVfoAdj(double v)
{
	vfo_ = v;
	cmd.assign(pre_to);
	cmd.append("\x1A\x05");
	cmd += '\x00';
	cmd += '\x70';
	cmd.append(to_bcd(v, 4));
	cmd.append(post);
	waitFB("SET vfo adjust");
	set_trace(2, "set_vfo_adjust()", str2hex(replystr.c_str(), replystr.length()));
}

double RIG_IC7610::getVfoAdj()
{
	cmd.assign(pre_to);
	cmd.append("\x1A\x05");
	cmd += '\x00';
	cmd += '\70';
	cmd.append(post);

	if (waitFOR(11, "get vfo adj")) {
		size_t p = replystr.find(pre_fm);
		if (p != string::npos) {
			vfo_ = fm_bcd(replystr.substr(p+8), 4);
		}
	}
	return vfo_;
}

void RIG_IC7610::set_digi_sel(bool b)
{
	cmd.assign(pre_to).append("\x16\x4E");
	if (b) cmd += '\x01';
	else   cmd += '\x00';
	cmd.append(post);
	waitFB("set_digi_sel");
	set_trace(2, "set_digi_sel()", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC7610::get_digi_sel()
{
	cmd = pre_to;
	cmd.append("\x16\x4E");
	cmd.append(post);
	if (waitFOR(8, "get AGC")) {
		size_t p = replystr.find(pre_fm);
		if (p != string::npos)
			progStatus.digi_sel_on_off = replystr[p+6];
	}
	get_trace(2, "get_digi_sel() ", str2hex(replystr.c_str(), replystr.length()));
	return progStatus.digi_sel_on_off;
}

void RIG_IC7610::set_digi_val(int v)
{
	cmd = pre_to;
	cmd.append("\x14\x13");
	cmd.append(to_bcd(v, 3));
	cmd.append(post);
	waitFB("set_digi_val");
	set_trace(2, "set_digi_val()", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC7610::get_digi_val()
{
	cmd = pre_to;
	cmd.append("\x14\x13");
	cmd.append( post );
	if (waitFOR(9, "get digi val")) {
		size_t p = replystr.rfind(pre_fm);
		if (p != string::npos) {
			progStatus.digi_sel_val = num100(replystr.substr(p+6));
			Fl::awake(set_ic7610_digi_sel_val);
		}
	}
	get_trace(2, "get_digi_val()", str2hex(replystr.c_str(), replystr.length()));
	return progStatus.digi_sel_val;
}

void RIG_IC7610::set_dual_watch(bool b)
{
	cmd.assign(pre_to);
	cmd += '\x07';
	cmd += '\xC2';
	if (b) cmd += '\x01';
	else   cmd += '\x00';
	cmd.append(post);
	waitFB("set_dual_watch");
	set_trace(2, "set_dual_watch()", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC7610::get_dual_watch()
{
	cmd.assign(pre_to);
	cmd += '\x07';
	cmd += '\xC2';
	cmd.append(post);
	string resp = pre_fm;
	cmd += '\x07'; cmd += '\xC2';
	if (waitFOR(8, "get dual watch")) {
		size_t p = replystr.rfind(resp);
		progStatus.dual_watch = replystr[p+7];
	}
	get_trace(2, "get_dual_watch()", str2hex(replystr.c_str(), replystr.length()));
	Fl::awake(set_ic7610_dual_watch);
	return progStatus.dual_watch;
}

static char attval[] = {
'\x00', '\x03', '\x06', '\x09', '\x12',
'\x15', '\x18', '\x21', '\x24', '\x27',
'\x30', '\x33', '\x36', '\x39', '\x42',
'\x45' };

void RIG_IC7610::set_index_att(int v)
{
	if (v < 0) return;
	if (v >= (int)sizeof(attval)) return;

	cmd.assign(pre_to);
	cmd += '\x11';
	cmd += attval[v];
	cmd.append(post);
	waitFB("SET attenuator");
	set_trace(2, "set_index_att()", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC7610::get_attenuator()
{
	cmd = pre_to;
	cmd += '\x11';
	cmd.append( post );
	string resp = pre_fm;
	resp += '\x11';
	if (waitFOR(7, "get ATT")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			atten_level = replystr[p+5];
			size_t i = 0;
			for (i = 0; i < sizeof(attval); i++) {
				if (attval[i] == atten_level) {
					progStatus.index_ic7610att = i;
					Fl::awake(set_ic7610_index_att);
					break;
				}
			}
		}
	}
	get_trace(2, "get_attenuator() ", str2hex(replystr.c_str(), replystr.length()));
	return progStatus.index_ic7610att;
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

void RIG_IC7610::get_band_selection(int v)
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
							(banddata == 0x20) ? 14 :
							(banddata == 0x30) ? 18 : 0);
			if ((bandmode == 1) && banddata)
				bandmode = ((banddata == 0x10) ? 11 : 
							(banddata == 0x20) ? 15 :
							(banddata == 0x30) ? 19 : 1);
			if ((bandmode == 2) && banddata)
				bandmode = ((banddata == 0x10) ? 12 : 
							(banddata == 0x20) ? 16 :
							(banddata == 0x30) ? 20 : 2);
			if ((bandmode == 3) && banddata)
				bandmode = ((banddata == 0x10) ? 13 : 
							(banddata == 0x20) ? 17 :
							(banddata == 0x30) ? 21 : 3);
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

void RIG_IC7610::set_band_selection(int v)
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
	if (mode >= m7610LSBD1 && mode <= m7610FM3D )
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

