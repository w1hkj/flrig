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

#include <sstream>

#include "IC9100.h"

//=============================================================================
// IC-9100

const char IC9100name_[] = "IC-9100";

static int nummodes = 9;

const char *IC9100modes_[] = {
"LSB", "USB", "AM", "FM", "DV", 
"CW", "CW-R", "RTTY", "RTTY-R",
"LSB-D", "USB-D", "AM-D", "FM-D", "DV-R", NULL};

enum {
	LSB9100, USB9100, AM9100, FM9100, DV9100,
	CW9100, CW9100R, RTTY9100, RTTY9100R,
	LSB9100D, USB9100D, AM9100D, FM9100D, DVR9100 };

const char IC9100_mode_type[] = {
	'L', 'U', 'U', 'U', 'U',
	'L', 'U', 'L', 'U',
	'L', 'U', 'U', 'U' };

const char IC9100_mode_nbr[] = {
	0x00, 0x01, 0x02, 0x05, 0x17,
	0x03, 0x07, 0x04, 0x08,
	0x00, 0x01, 0x02, 0x05, 0x17 };

const char *IC9100_ssb_bws[] = {
"50",    "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700", "2800", "2900", "3000", "3100", "3200", "3300", "3400", "3500",
"3600",
NULL };
static int IC9100_bw_vals_SSB[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,
41, WVALS_LIMIT};

const char *IC9100_am_bws[] = {
"200",   "400",  "600",  "800", "1000", "1200", "1400", "1600", "1800", "2000",
"2200", "2400", "2600", "2800", "3000", "3200", "3400", "3600", "3800", "4000",
"4200", "4400", "4600", "4800", "5000", "5200", "5400", "5600", "5800", "6000",
"6200", "6400", "6600", "6800", "7000", "7200", "7400", "9100", "7800", "8000",
"8200", "8400", "8600", "8800", "9000", "9200", "9400", "9600", "9800", "10000", 
NULL };
static int IC9100_bw_vals_AM[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,
41,42,43,44,45,46,47,48,49,50,
 WVALS_LIMIT};

const char *IC9100_rtty_bws[] = {
"50",    "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700",
NULL };
static int IC9100_bw_vals_RTTY[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32, WVALS_LIMIT};

const char *IC9100_fixed_bws[] = { "FIXED", NULL };
static int IC9100_bw_vals_fixed[] = { 1, WVALS_LIMIT};

static GUI IC9100_widgets[]= {
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

void RIG_IC9100::initialize()
{
	IC9100_widgets[0].W = btnVol;
	IC9100_widgets[1].W = sldrVOLUME;
	IC9100_widgets[2].W = btnAGC;
	IC9100_widgets[3].W = sldrRFGAIN;
	IC9100_widgets[4].W = sldrSQUELCH;
	IC9100_widgets[5].W = btnNR;
	IC9100_widgets[6].W = sldrNR;
	IC9100_widgets[7].W = btnLOCK;
	IC9100_widgets[8].W = sldrINNER;
	IC9100_widgets[9].W = btnCLRPBT;
	IC9100_widgets[10].W = sldrOUTER;
	IC9100_widgets[11].W = btnNotch;
	IC9100_widgets[12].W = sldrNOTCH;
	IC9100_widgets[13].W = sldrMICGAIN;
	IC9100_widgets[14].W = sldrPOWER;

	btn_icom_select_11->activate();
	btn_icom_select_12->activate();
	btn_icom_select_13->activate();

	choice_rTONE->activate();
	choice_tTONE->activate();
}

RIG_IC9100::RIG_IC9100() {
	defaultCIV = 0x7C;
	adjustCIV(defaultCIV);

	name_ = IC9100name_;
	modes_ = IC9100modes_;

	bandwidths_ = IC9100_ssb_bws;
	bw_vals_ = IC9100_bw_vals_SSB;

	_mode_type = IC9100_mode_type;

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

	A.freq = 14070000;
	A.imode = USB9100D;
	A.iBW = 34;
	B.freq = 7070000;
	B.imode = USB9100D;
	B.iBW = 34;

	def_freq = 14070000L;
	def_mode = USB9100D;
	def_bw = 34;

	widgets = IC9100_widgets;

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

	has_rf_control = true;

	has_ptt_control = true;
	has_tune_control = true;

	ICOMmainsub = true;

	has_band_selection = true;

	precision = 1;
	ndigits = 10;

};

//======================================================================
// IC9100 unique commands
//======================================================================

void RIG_IC9100::selectA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\xD0';
	cmd.append(post);

	waitFB("select A");
	set_trace(2, "selectA()", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC9100::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\xD1';
	cmd.append(post);
	waitFB("select B");
	set_trace(2, "selectB()", str2hex(replystr.c_str(), replystr.length()));
}

bool RIG_IC9100::check ()
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

long RIG_IC9100::get_vfoA ()
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

void RIG_IC9100::set_vfoA (long freq)
{
	A.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	waitFB("set vfo A");
	set_trace(2, "set_vfoA()", str2hex(replystr.c_str(), replystr.length()));
}

long RIG_IC9100::get_vfoB ()
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

void RIG_IC9100::set_vfoB (long freq)
{
	B.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	waitFB("set vfo B");
	set_trace(2, "set_vfoB()", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC9100::set_modeA(int val)
{
	A.imode = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += IC9100_mode_nbr[val];
	cmd.append( post );
	waitFB("set mode A");
	stringstream ss;
	ss << "set_modeA(" << val << ") [" << IC9100modes_[val] << "] " <<
		str2hex(replystr.c_str(), replystr.length());
	set_trace(1, ss.str().c_str());

	cmd = pre_to;
	cmd += '\x1A'; cmd += '\x06';
	if (A.imode > RTTY9100R) cmd += '\x01';
	else cmd += '\x00';
	cmd.append( post);
	waitFB("data mode");
	set_trace(2, "set_data_modeA()", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC9100::get_modeA()
{
	int md = A.imode;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	string resp = pre_fm;
	resp += '\x04';
	int ret = waitFOR(8, "get mode/bw A");

	stringstream ss;
	ss << "get_modeA() " << str2hex(replystr.c_str(), replystr.length());
	get_trace(2, "get_modeA()", ss.str().c_str());

	if (ret) {
		size_t p = replystr.find(resp);
		if (p != std::string::npos) {
			for (md = 0; md < nummodes; md++)
				if (replystr[p+5] == IC9100_mode_nbr[md])
					break;
			if (md == nummodes) {
				checkresponse();
				return A.imode;
			}
		}
	} else {
		checkresponse();
		return A.imode;
	}

	if (md < CW9100) { // check for DATA  / DVR
		cmd = pre_to;
		cmd += "\x1A\x06";
		cmd.append(post);
		resp = pre_fm;
		resp += "\x1A\x06";
		if (waitFOR(9, "data mode?")) {
			size_t p = replystr.rfind(resp);
			if (p != string::npos) {
				if ((replystr[p+6] & 0x01) == 0x01) {
					if (md == LSB9100) md = LSB9100D;
					if (md == USB9100) md = USB9100D;
					if (md == AM9100) md = AM9100D;
					if (md == FM9100) md = FM9100D;
					if (md == DV9100) md = DVR9100;
				}
			}
		}
		get_trace(2,"get_data_modeA", str2hex(replystr.c_str(), replystr.length()));
	}

	A.imode = md;

	return A.imode;
}

void RIG_IC9100::set_modeB(int val)
{
	B.imode = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += IC9100_mode_nbr[val];
	cmd.append( post );
	waitFB("set mode B");
	stringstream ss;
	ss << "set_modeB(" << val << ") [" << IC9100modes_[val] << "] " <<
		str2hex(replystr.c_str(), replystr.length());
	set_trace(1, ss.str().c_str());

	cmd = pre_to;
	cmd += '\x1A'; cmd += '\x06';
	if (B.imode > RTTY9100R) cmd += '\x01';
	else cmd += '\x00';
	cmd.append( post);
	waitFB("data mode");
	set_trace(2, "set_data_modeB()", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC9100::get_modeB()
{
	int md = B.imode;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	string resp = pre_fm;
	resp += '\x04';
	int ret = waitFOR(8, "get mode/bw B");

	stringstream ss;
	ss << "get_modeA() " << str2hex(replystr.c_str(), replystr.length());
	get_trace(1, ss.str().c_str());

	if (ret) {
		size_t p = replystr.find(resp);
		if (p != std::string::npos) {
			for (md = 0; md < nummodes; md++)
				if (replystr[p+5] == IC9100_mode_nbr[md])
					break;
			if (md == nummodes) {
				checkresponse();
				return B.imode;
			}
		}
	} else {
		checkresponse();
		return B.imode;
	}

	if (md < CW9100) { // check for DATA  / DVR
		cmd = pre_to;
		cmd += "\x1A\x06";
		cmd.append(post);
		resp = pre_fm;
		resp += "\x1A\x06";
		if (waitFOR(9, "data mode?")) {
			size_t p = replystr.rfind(resp);
			if (p != string::npos) {
				if ((replystr[p+6] & 0x01) == 0x01) {
					if (md == LSB9100) md = LSB9100D;
					if (md == USB9100) md = USB9100D;
					if (md == AM9100) md = AM9100D;
					if (md == FM9100) md = FM9100D;
					if (md == DV9100) md = DVR9100;
				}
			}
		}
		get_trace(2,"get_data_modeB", str2hex(replystr.c_str(), replystr.length()));
	}

	B.imode = md;

	return B.imode;
}

int RIG_IC9100::get_bwA()
{
	if (A.imode == DV9100 || A.imode == DVR9100 ||
		A.imode == FM9100 || A.imode == FM9100D) return 0;

	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(post);
	string resp = pre_fm;
	resp.append("\x1a\x03");
	int ret = waitFOR(8, "get_bw A");

	get_trace(2, "get_bwA()", str2hex(replystr.c_str(), replystr.length()));

	if (ret) {
		size_t p = replystr.rfind(resp);
		A.iBW = fm_bcd(replystr.substr(p+6), 2);
	}
	return A.iBW;
}

void RIG_IC9100::set_bwA(int val)
{
	A.iBW = val;
	if (A.imode == DV9100 || A.imode == DVR9100 ||
		A.imode == FM9100 || A.imode == FM9100D) return;

	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(to_bcd(A.iBW, 2));
	cmd.append(post);

	waitFB("set bw A");

	stringstream ss;
	const char *bwstr = IC9100_ssb_bws[val];
	if ((A.imode == AM9100) || (A.imode == AM9100D)) bwstr = IC9100_am_bws[val];
	if ((A.imode == RTTY9100) || (A.imode == RTTY9100R)) bwstr = IC9100_rtty_bws[val];
	 ss << "set_bwA(" << val << ") [" << bwstr << "] " <<
		str2hex(replystr.data(), replystr.length());
	set_trace(1, ss.str().c_str());

}

int RIG_IC9100::get_bwB()
{
	if (B.imode == DV9100 || B.imode == DVR9100 ||
		B.imode == FM9100 || B.imode == FM9100D) return 0;

	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(post);
	string resp = pre_fm;
	resp.append("\x1a\x03");
	int ret = waitFOR(8, "get_bw B");

	get_trace(2, "get_bwB()", str2hex(replystr.c_str(), replystr.length()));

	if (ret) {
		size_t p = replystr.rfind(resp);
		B.iBW = fm_bcd(replystr.substr(p+6), 2);
	}
	return B.iBW;
}

void RIG_IC9100::set_bwB(int val)
{
	B.iBW = val;
	if (B.imode == DV9100 || B.imode == DVR9100 ||
		B.imode == FM9100 || B.imode == FM9100D) return;

	if (B.imode == 5) return;
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(to_bcd(B.iBW, 2));
	cmd.append(post);
	waitFB("set bw B");

	stringstream ss;
	const char *bwstr = IC9100_ssb_bws[val];
	if ((B.imode == AM9100) || (B.imode == AM9100D)) bwstr = IC9100_am_bws[val];
	if ((B.imode == RTTY9100) || (B.imode == RTTY9100R)) bwstr = IC9100_rtty_bws[val];
	ss << "set_bwB(" << val << ") [" << bwstr << "] " <<
		str2hex(replystr.data(), replystr.length());
	set_trace(1, ss.str().c_str());

}

int RIG_IC9100::adjust_bandwidth(int m)
{
	int bw = 0;
	switch (m) {
		case AM9100: case AM9100D:
			bandwidths_ = IC9100_am_bws;
			bw_vals_ = IC9100_bw_vals_AM;
			bw = 19;
			break;
		case CW9100: case CW9100R:
			bandwidths_ = IC9100_ssb_bws;
			bw_vals_ = IC9100_bw_vals_SSB;
			bw = 12;
			break;
		case RTTY9100: case RTTY9100R:
			bandwidths_ = IC9100_ssb_bws;
			bw_vals_ = IC9100_bw_vals_RTTY;
			bw = 12;
			break;
		case FM9100: case FM9100D :
		case DV9100: case DVR9100 :
			bandwidths_ = IC9100_fixed_bws;
			bw_vals_ = IC9100_bw_vals_fixed;
			bw = 0;
			break;
		case LSB9100: case USB9100:
		case LSB9100D: case USB9100D:
		default:
			bandwidths_ = IC9100_ssb_bws;
			bw_vals_ = IC9100_bw_vals_SSB;
			bw = 34;
	}
	return bw;
}

const char ** RIG_IC9100::bwtable(int m)
{
	const char ** table;
	switch (m) {
		case AM9100: case AM9100D:
			table = IC9100_am_bws;
			break;
		case FM9100: case FM9100D :
		case DV9100: case DVR9100 :
			table = IC9100_fixed_bws;
			break;
		case CW9100: case CW9100R:
		case RTTY9100: case RTTY9100R:
		case LSB9100: case USB9100:
		case LSB9100D: case USB9100D:
		default:
			table = IC9100_ssb_bws;
	}
	return table;
}

int RIG_IC9100::def_bandwidth(int m)
{
	int bw = 0;
	switch (m) {
		case AM9100: case AM9100D:
			bw = 19;
			break;
		case DV9100: case DVR9100:
		case FM9100: case FM9100D:
			bw = 0;
			break;
		case RTTY9100: case RTTY9100R:
			bw = 12;
			break;
		case CW9100: case CW9100R:
			bw = 12;
			break;
		case LSB9100: case USB9100:
		case LSB9100D: case USB9100D:
		default:
			bw = 34;
	}
	return bw;
}

void RIG_IC9100::set_mic_gain(int val)
{
	if (!progStatus.USBaudio) {
		cmd = pre_to;
		cmd.append("\x14\x0B");
		cmd.append(bcd255(val));
		cmd.append( post );
	} else {
		cmd = pre_to;
		cmd += '\x1A'; cmd += '\x05';
		cmd += '\x00'; cmd += '\x29';
		cmd.append(bcd255(val));
		cmd.append( post );
	}
	waitFB("set mic gain");
	set_trace(2, "set_mic_gain()", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC9100::set_compression(int on, int val)
{
	if (on) {
		cmd.assign(pre_to).append("\x14\x0E");
		cmd.append(bcd255(val));
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
	set_trace(2, "set_compression()", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC9100::set_vox_onoff()
{
	cmd.assign(pre_to).append("\x16\x46");
	if (progStatus.vox_onoff) {
		cmd += '\x01';
		cmd.append( post );
		waitFB("set vox ON");
	} else {
		cmd += '\x00';
		cmd.append( post );
		waitFB("set vox OFF");
	}
	set_trace(2, "set_vox_onoff()", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC9100::set_vox_gain()
{
	cmd.assign(pre_to).append("\x1A\x05");
	cmd +='\x01';
	cmd +='\x25';
	cmd.append(bcd255(progStatus.vox_gain));
	cmd.append( post );
	waitFB("SET vox gain");
	set_trace(2, "set_vox_gain()", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC9100::set_vox_anti()
{
	cmd.assign(pre_to).append("\x1A\x05");
	cmd +='\x01';
	cmd +='\x26';
	cmd.append(bcd255(progStatus.vox_anti));
	cmd.append( post );
	waitFB("SET anti-vox");
	set_trace(2, "set_vox_anti()", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC9100::set_vox_hang()
{
	cmd.assign(pre_to).append("\x1A\x05");	//values 00-20 = 0.0 - 2.0 sec
	cmd +='\x01';
	cmd +='\x27';
	cmd.append(to_bcd((int)(progStatus.vox_hang / 10 ), 2));
	cmd.append( post );
	waitFB("SET vox hang");
	set_trace(2, "set_vox_hang()", str2hex(replystr.c_str(), replystr.length()));
}

// these need to be written and tested
void  RIG_IC9100::get_vox_onoff()
{
}

void  RIG_IC9100::get_vox_gain()
{
}

void  RIG_IC9100::get_vox_anti()
{
}

void  RIG_IC9100::get_vox_hang()
{
}

// CW controls

void RIG_IC9100::set_cw_wpm()
{
	cmd.assign(pre_to).append("\x14\x0C"); // values 0-255
	cmd.append(to_bcd(round((progStatus.cw_wpm - 6) * 255 / (60 - 6)), 3));
	cmd.append( post );
	waitFB("SET cw wpm");
	set_trace(2, "set_cw_wpm()", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC9100::set_cw_qsk()
{
	int n = round(progStatus.cw_qsk * 10); // values 0-255
	cmd.assign(pre_to).append("\x14\x0F");
	cmd.append(to_bcd(n, 3));
	cmd.append(post);
	waitFB("Set cw qsk delay");
	set_trace(2, "set_cw_qsk()", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC9100::set_cw_spot_tone()
{
	cmd.assign(pre_to).append("\x14\x09"); // values 0=300Hz 255=900Hz
	int n = round((progStatus.cw_spot_tone - 300) * 255.0 / 600.0);
	if (n > 255) n = 255;
	if (n < 0) n = 0;
	cmd.append(to_bcd(n, 3));
	cmd.append( post );
	waitFB("SET cw spot tone");
	set_trace(2, "set_cw_spot_tone()", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC9100::set_cw_vol()
{
	cmd.assign(pre_to);
	cmd.append("\x1A\x05");
	cmd += '\x00';
	cmd += '\x24';
	cmd.append(to_bcd((int)(progStatus.cw_vol * 2.55), 3));
	cmd.append( post );
	waitFB("SET cw sidetone volume");
	set_trace(2, "set_cw_vol()", str2hex(replystr.c_str(), replystr.length()));
}

// Volume control val 0 ... 100
void RIG_IC9100::set_volume_control(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set vol");
	set_trace(2, "set_volume_control()", str2hex(replystr.c_str(), replystr.length()));
}

/*

I:12:20:22: get vol ans in 0 ms, OK
cmd FE FE 7A E0 14 01 FD
ans FE FE 7A E0 14 01 FD
FE FE E0 7A 14 01 00 65 FD
 0  1  2  3  4  5  6  7  8
*/
int RIG_IC9100::get_volume_control()
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
	get_trace(2, "get_volume_control()", str2hex(replystr.c_str(), replystr.length()));
	return (val);
}

void RIG_IC9100::get_vol_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

void RIG_IC9100::set_power_control(double val)
{
	cmd = pre_to;
	cmd.append("\x14\x0A");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set power");
	set_trace(2, "set_power_control()", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC9100::get_power_control()
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
	get_trace(2, "get_power_control()", str2hex(replystr.c_str(), replystr.length()));
	return val;
}

void RIG_IC9100::get_pc_min_max_step(double &min, double &max, double &step)
{
	min = 2; max = 100; step = 1;
}

void RIG_IC9100::set_rf_gain(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x02");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set RF");
	set_trace(2, "set_rf_gain()", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC9100::get_rf_gain()
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

void RIG_IC9100::get_rf_min_max_step(double &min, double &max, double &step)
{
	min = 0; max = 100; step = 1;
}

int RIG_IC9100::get_smeter()
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
			mtr = (int)ceil(mtr /2.40);
			if (mtr > 100) mtr = 100;
		}
	}
	get_trace(2, "get_smeter()", str2hex(replystr.c_str(), replystr.length()));
	return mtr;
}

int RIG_IC9100::get_power_out()
{
	string cstr = "\x15\x11";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;
	if (waitFOR(9, "get pout")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			mtr = (int)ceil(mtr /2.15);
			if (mtr > 100) mtr = 100;
		}
	}
	get_trace(2, "get_power_out()", str2hex(replystr.c_str(), replystr.length()));
	return mtr;
}

int RIG_IC9100::get_swr()
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
			mtr = fm_bcd(replystr.substr(p+6), 3);
			mtr = (int)ceil(mtr /2.40);
			if (mtr > 100) mtr = 100;
		}
	}
	get_trace(2, "get_swr()", str2hex(replystr.c_str(), replystr.length()));
	return mtr;
}

int RIG_IC9100::get_alc()
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
			mtr = (int)ceil(mtr /2.55);
			if (mtr > 100) mtr = 100;
		}
	}
	get_trace(2, "get_alc()", str2hex(replystr.c_str(), replystr.length()));
	return mtr;
}

void RIG_IC9100::set_notch(bool on, int val)
{
	int notch = (int)(val * 256.0 / 3000.0);

	cmd = pre_to;
	cmd.append("\x16\x48");
	cmd += on ? '\x01' : '\x00';
	cmd.append(post);
	waitFB("set notch");

	cmd = pre_to;
	cmd.append("\x14\x0D");
	cmd.append(to_bcd(notch,3));
	cmd.append(post);
	waitFB("set notch val");

	stringstream ss;
	ss << "set_notch(" << on << ", " << val << ") " << str2hex(replystr.data(), replystr.length());
	set_trace(2, "set_notch()", ss.str().c_str());

}

bool RIG_IC9100::get_notch(int &val)
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
			get_trace(2, "get_notch_val()", str2hex(replystr.c_str(), replystr.length()));
			size_t p = replystr.rfind(resp);
			if (p != string::npos)
				val = (int)ceil(fm_bcd(replystr.substr(p+6),3) * 3000.0 / 255.0);
		}
	}
	return on;
}

void RIG_IC9100::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 3000;
	step = 20;
}

void RIG_IC9100::set_auto_notch(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x41';
	cmd += val ? 0x01 : 0x00;
	cmd.append( post );
	waitFB("set AN");
	set_trace(2, "set_auto_notch()", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC9100::get_auto_notch()
{
	string cstr = "\x16\x41";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get AN")) {
		get_trace(2, "get_auto_notch()", str2hex(replystr.c_str(), replystr.length()));
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

void RIG_IC9100::set_split(bool val)
{
	split = val;
	cmd = pre_to;
	cmd += 0x0F;
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	waitFB(val ? "set split ON" : "set split OFF");
	set_trace(2, "set_split()", str2hex(replystr.c_str(), replystr.length()));
}

int  RIG_IC9100::get_split()
{
	return split;
}

static int agcval = 1;
int  RIG_IC9100::get_agc()
{
	cmd = pre_to;
	cmd.append("\x16\x12");
	cmd.append(post);
	if (waitFOR(8, "get AGC")) {
		size_t p = replystr.find(pre_fm);
		if (p == string::npos) return agcval;
		return (agcval = replystr[p+6]); // 1 = FAST, 2 = MID, 3 = SLOW
	}
	get_trace(2, "get_agc()", str2hex(replystr.c_str(), replystr.length()));
	return agcval;
}

int RIG_IC9100::incr_agc()
{
	agcval++;
	if (agcval == 4) agcval = 1;
	cmd = pre_to;
	cmd.append("\x16\x12");
	cmd += agcval;
	cmd.append(post);
	waitFB("set AGC");
	set_trace(2, "incr_agc()", str2hex(replystr.c_str(), replystr.length()));
	return agcval;
}


static const char *agcstrs[] = {"FST", "MID", "SLO"};
const char *RIG_IC9100::agc_label()
{
	return agcstrs[agcval - 1];
}

int  RIG_IC9100::agc_val()
{
	return (agcval);
}

void RIG_IC9100::set_attenuator(int val)
{
	if (val) {
		atten_label("20 dB", true);
		atten_level = 1;
		set_preamp(0);
	} else {
		atten_label("ATT", false);
		atten_level = 0;
	}

	cmd = pre_to;
	cmd += '\x11';
	cmd += atten_level ? '\x20' : '\x00';
	cmd.append( post );
	waitFB("set att");
	stringstream ss;
	ss << "set_attenuator(" << val << ") " << str2hex(replystr.data(), replystr.length());
	set_trace(1, ss.str().c_str());

}

int RIG_IC9100::next_attenuator()
{
	if (atten_level) return 0;
	return 1;
}

int RIG_IC9100::get_attenuator()
{
	cmd = pre_to;
	cmd += '\x11';
	cmd.append( post );
	string resp = pre_fm;
	resp += '\x11';
	if (waitFOR(7, "get ATT")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			if (!replystr[p+5]) {
				atten_label("ATT", false);
				atten_level = 0;
				return 0;
			} else {
				atten_label("20 dB", true);
				atten_level = 1;
				return 1;
			}
		}
	}
	get_trace(2, "get_attenuator()", str2hex(replystr.data(), replystr.length()));
	return 0;
}

int RIG_IC9100::next_preamp()
{
	switch (preamp_level) {
		case 0: return 1;
		case 1: return 2;
		default:
		case 2: return 0;
	}
	return 0;
}

void RIG_IC9100::set_preamp(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';

	if (val == 0) {
		preamp_label("Pre", false);
		preamp_level = 0;
	} else if (val == 1) {
		preamp_label("Pre 1", true);
		preamp_level = 1;
	} else {
		preamp_label("Pre 2", true);
		preamp_level = 2;
	}
	cmd += preamp_level;

	cmd.append( post );
	waitFB("set Pre");

	stringstream ss;
	ss << "set_preamp(" << val << ") " << str2hex(replystr.data(), replystr.length());
	set_trace(1, ss.str().c_str());

	if (val)
		set_attenuator(0);

}

int RIG_IC9100::get_preamp()
{
	string cstr = "\x16\x02";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get Pre")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			if (replystr[p+6] == 0x01) {
				preamp_label("Pre 1", true);
				preamp_level = 1;
			} else if (replystr[p+6] == 0x02) {
				preamp_label("Pre 2", true);
				preamp_level = 2;
			} else {
				preamp_label("Pre", false);
				preamp_level = 0;
			}
		}
	}
	get_trace(2, "get_preamp()", str2hex(replystr.c_str(), replystr.length()));

	return preamp_level;
}

// Tranceiver PTT on/off
void RIG_IC9100::set_PTT_control(int val)
{
	cmd = pre_to;
	cmd += '\x1c';
	cmd += '\x00';
	cmd += (unsigned char) val;
	cmd.append( post );
	waitFB("set ptt");

	stringstream ss;
	ss << "set_PTT_control(" << val << ") " << str2hex(replystr.data(), replystr.length());
	set_trace(1, ss.str().c_str());

	ptt_ = val;
}

int RIG_IC9100::get_PTT()
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
	get_trace(2, "get_PTT()", str2hex(replystr.data(), replystr.length()));
	return ptt_;
}

void RIG_IC9100::set_noise(bool val)
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set noise");
	set_trace(2, "set_noise()", str2hex(replystr.data(), replystr.length()));
}

int RIG_IC9100::get_noise()
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
	get_trace(2, "get_noise()", str2hex(replystr.data(), replystr.length()));
	return val;
}

void RIG_IC9100::set_nb_level(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x12");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set NB level");
	set_trace(2, "set_nb_level()", str2hex(replystr.data(), replystr.length()));
}

int  RIG_IC9100::get_nb_level()
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
	get_trace(2, "get_nb_level()", str2hex(replystr.data(), replystr.length()));
	return val;
}

void RIG_IC9100::set_noise_reduction(int val)
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set NR");
	get_trace(2, "get_noise_reduction()", str2hex(replystr.data(), replystr.length()));
}

int RIG_IC9100::get_noise_reduction()
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
	get_trace(2, "get_noise_reduction()", str2hex(replystr.data(), replystr.length()));
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

void RIG_IC9100::set_noise_reduction_val(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x06");
	val *= 16;
	val += 8;
	cmd.append(to_bcd(val, 3));
	cmd.append(post);
	waitFB("set NRval");
	set_trace(2, "set_noise_reduction_val()", str2hex(replystr.data(), replystr.length()));
}

int RIG_IC9100::get_noise_reduction_val()
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
	get_trace(2, "get_noise_reduction_val()", str2hex(replystr.data(), replystr.length()));
	return val;
}

void RIG_IC9100::set_squelch(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x03");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set Sqlch");
	set_trace(2, "set_squelch()", str2hex(replystr.data(), replystr.length()));
}

int  RIG_IC9100::get_squelch()
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
	get_trace(2, "get_squelch()", str2hex(replystr.data(), replystr.length()));
	return val;
}

void RIG_IC9100::set_if_shift(int val)
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
	set_trace(2, "set_if_shift()", str2hex(replystr.data(), replystr.length()));

	cmd = pre_to;
	cmd.append("\x14\x08");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set IF val");
	set_trace(2, "set_if_shift_val()", str2hex(replystr.data(), replystr.length()));
}

bool RIG_IC9100::get_if_shift(int &val) {
	val = sh_;
	return sh_on_;
}

void RIG_IC9100::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -50;
	max = +50;
	step = 1;
}

void RIG_IC9100::set_pbt_inner(int val)
{
	int shift = 128 + val * 128 / 50;
	if (shift < 0) shift = 0;
	if (shift > 255) shift = 255;

	cmd = pre_to;
	cmd.append("\x14\x07");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set PBT inner");
	set_trace(2, "set_pbt_inner()", str2hex(replystr.c_str(), replystr.length()));
}

void RIG_IC9100::set_pbt_outer(int val)
{
	int shift = 128 + val * 128 / 50;
	if (shift < 0) shift = 0;
	if (shift > 255) shift = 255;

	cmd = pre_to;
	cmd.append("\x14\x08");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set PBT outer");
	set_trace(2, "set_pbt_outer()", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_IC9100::get_pbt_inner()
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

int RIG_IC9100::get_pbt_outer()
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
	get_trace(2, "get_pbt_outer()", str2hex(replystr.c_str(), replystr.length()));
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

void RIG_IC9100::get_band_selection(int v)
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
			if ((bandmode == 0) && banddata) bandmode = 9;
			if ((bandmode == 1) && banddata) bandmode = 10;
			if ((bandmode == 2) && banddata) bandmode = 11;
			if ((bandmode == 3) && banddata) bandmode = 12;
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

void RIG_IC9100::set_band_selection(int v)
{
	long freq = (useB ? B.freq : A.freq);
	int mode = (useB ? B.imode : A.imode);

	cmd.assign(pre_to);
	cmd.append("\x1A\x01");
	cmd += to_bcd_be( v, 2 );
	cmd += '\x01';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd += mode;
	cmd += '\x01';
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
