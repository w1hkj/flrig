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

//#define IC9100_DEBUG

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
	{ (Fl_Widget *)btnVol, 2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME, 54, 125, 156 },
	{ (Fl_Widget *)btnAGC, 2, 145, 50 },
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
	has_ifshift_control = true;

	has_rf_control = true;

	has_ptt_control = true;
	has_tune_control = true;

	precision = 1;
	ndigits = 10;

};

//======================================================================
// IC9100 unique commands
//======================================================================

void RIG_IC9100::initialize()
{
	IC9100_widgets[0].W = btnVol;
	IC9100_widgets[1].W = sldrVOLUME;
	IC9100_widgets[2].W = btnAGC;
	IC9100_widgets[3].W = sldrRFGAIN;
	IC9100_widgets[4].W = sldrSQUELCH;
	IC9100_widgets[5].W = btnNR;
	IC9100_widgets[6].W = sldrNR;
	IC9100_widgets[7].W = btnIFsh;
	IC9100_widgets[8].W = sldrIFSHIFT;
	IC9100_widgets[9].W = btnNotch;
	IC9100_widgets[10].W = sldrNOTCH;
	IC9100_widgets[11].W = sldrMICGAIN;
	IC9100_widgets[12].W = sldrPOWER;

//	cmd = pre_to;
//	cmd += '\x1A'; cmd += '\x05';
//	cmd += '\x00'; cmd += '\x58'; cmd += '\x00';
//	cmd.append(post);
//	waitFB("CI-V transceive OFF");
//	checkresponse();
}

void RIG_IC9100::selectA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\xD0';
	cmd.append(post);
#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " () " <<  str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
	waitFB("select A");
}

void RIG_IC9100::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\xD1';
	cmd.append(post);
#ifdef IC9100_DEBUG
		stringstream ss;
	 ss << " () " << str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
	waitFB("select B");
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
	return A.freq;
}

void RIG_IC9100::set_vfoA (long freq)
{
	A.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " (" << freq << ") " << 
		str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
	waitFB("set vfo A");
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
	return B.freq;
}

void RIG_IC9100::set_vfoB (long freq)
{
	B.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " (" << freq << ") " << 
		str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
	waitFB("set vfo B");
}

void RIG_IC9100::set_modeA(int val)
{
	A.imode = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += IC9100_mode_nbr[val];
	cmd.append( post );
#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " (" << val << ") [" << IC9100modes_[val] << "] " <<
		str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
	waitFB("set mode A");

	cmd = pre_to;
	cmd += '\x1A'; cmd += '\x06';
	if (A.imode > RTTY9100R) cmd += '\x01';
	else cmd += '\x00';
	cmd.append( post);
	waitFB("data mode");
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
#ifdef IC9100_DEBUG
	stringstream ss;
	ss << "get_modeA() " << str2hex(replystr.c_str(), replystr.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
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
#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " (" << val << ") [" << IC9100modes_[val] << "] " <<
		str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
	waitFB("set mode B");

	cmd = pre_to;
	cmd += '\x1A'; cmd += '\x06';
	if (B.imode > RTTY9100R) cmd += '\x01';
	else cmd += '\x00';
	cmd.append( post);
	waitFB("data mode");
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
#ifdef IC9100_DEBUG
	stringstream ss;
	ss << "get_modeB() " << str2hex(replystr.c_str(), replystr.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
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
#ifdef IC9100_DEBUG
	stringstream ss;
	ss << "get_bwA() " << str2hex(replystr.c_str(), replystr.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
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
#ifdef IC9100_DEBUG
	stringstream ss;
	const char *bwstr = IC9100_ssb_bws[val];
	if ((A.imode == AM9100) || (A.imode == AM9100D)) bwstr = IC9100_am_bws[val];
	if ((A.imode == RTTY9100) || (A.imode == RTTY9100R)) bwstr = IC9100_rtty_bws[val];
	 ss << " (" << val << ") [" << bwstr << "] " <<
		str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
	waitFB("set bw A");
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
#ifdef IC9100_DEBUG
	stringstream ss;
	ss << "get_bwB() " << str2hex(replystr.c_str(), replystr.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
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
#ifdef IC9100_DEBUG
	stringstream ss;
	const char *bwstr = IC9100_ssb_bws[val];
	if ((A.imode == AM9100) || (A.imode == AM9100D)) bwstr = IC9100_am_bws[val];
	if ((A.imode == RTTY9100) || (A.imode == RTTY9100R)) bwstr = IC9100_rtty_bws[val];
	 ss << " (" << val << ") [" << bwstr << "] " <<
		str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
	waitFB("set bw B");
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
#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " (" << val << ") " << str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
	waitFB("set mic gain");
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
#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " (" << on << ", " << val << ") " << 
		str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
}

void RIG_IC9100::set_vox_onoff()
{
	cmd.assign(pre_to).append("\x16\x46");
	if (progStatus.vox_onoff) {
		cmd += '\x01';
		cmd.append( post );
#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " () " << str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
		waitFB("set vox ON");
	} else {
		cmd += '\x00';
		cmd.append( post );
#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " () " << str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
		waitFB("set vox OFF");
	}
}

void RIG_IC9100::set_vox_gain()
{
	cmd.assign(pre_to).append("\x1A\x05");
	cmd +='\x01';
	cmd +='\x25';
	cmd.append(bcd255(progStatus.vox_gain));
	cmd.append( post );
#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " [" << progStatus.vox_gain << "] " <<
		str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
	waitFB("SET vox gain");
}

void RIG_IC9100::set_vox_anti()
{
	cmd.assign(pre_to).append("\x1A\x05");
	cmd +='\x01';
	cmd +='\x26';
	cmd.append(bcd255(progStatus.vox_anti));
	cmd.append( post );
#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " [" << progStatus.vox_anti << "] " <<
		str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
	waitFB("SET anti-vox");
}

void RIG_IC9100::set_vox_hang()
{
	cmd.assign(pre_to).append("\x1A\x05");	//values 00-20 = 0.0 - 2.0 sec
	cmd +='\x01';
	cmd +='\x27';
	cmd.append(to_bcd((int)(progStatus.vox_hang / 10 ), 2));
	cmd.append( post );
#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " [" << progStatus.vox_hang << "] " <<
		str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
	waitFB("SET vox hang");
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
#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " [" << progStatus.cw_wpm << "] " <<
		str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
	waitFB("SET cw wpm");
}

void RIG_IC9100::set_cw_qsk()
{
	int n = round(progStatus.cw_qsk * 10); // values 0-255
	cmd.assign(pre_to).append("\x14\x0F");
	cmd.append(to_bcd(n, 3));
	cmd.append(post);
#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " [" << progStatus.cw_qsk << "] " <<
		str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
	waitFB("Set cw qsk delay");
}

void RIG_IC9100::set_cw_spot_tone()
{
	cmd.assign(pre_to).append("\x14\x09"); // values 0=300Hz 255=900Hz
	int n = round((progStatus.cw_spot_tone - 300) * 255.0 / 600.0);
	if (n > 255) n = 255;
	if (n < 0) n = 0;
	cmd.append(to_bcd(n, 3));
	cmd.append( post );
#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " [" << progStatus.cw_spot_tone << "[ " <<
		str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
	waitFB("SET cw spot tone");
}

void RIG_IC9100::set_cw_vol()
{
	cmd.assign(pre_to);
	cmd.append("\x1A\x05");
	cmd += '\x00';
	cmd += '\x24';
	cmd.append(to_bcd((int)(progStatus.cw_vol * 2.55), 3));
	cmd.append( post );
#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " [" << progStatus.cw_vol << "[ " << 
		str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
	waitFB("SET cw sidetone volume");
}

// Volume control val 0 ... 100
void RIG_IC9100::set_volume_control(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append(bcd255(val));
	cmd.append( post );
#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " [" << val << "[ " << 
		str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
	waitFB("set vol");
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
#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " [" << val << "[ " << 
		str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
	waitFB("set power");
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
#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " [" << val << "[ " << 
		str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
	waitFB("set RF");
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

#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " (" << on << ", " << val << ")" << str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif

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
#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " (" <<  val << ")" << str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
	waitFB("set AN");
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
#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " (" << val << ")" << str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
	waitFB(val ? "set split ON" : "set split OFF");
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
#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " (" << agcval << ")" << str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif

	waitFB("set AGC");
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
#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " (" << val << ")" << str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif

	waitFB("set att");
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

#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " (" << val << ")" << str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif

	waitFB("set Pre");

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
#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " (" << val << ")" << str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif

	waitFB("set ptt");
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
	return ptt_;
}

void RIG_IC9100::set_noise(bool val)
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set noise");
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
	return val;
}

void RIG_IC9100::set_nb_level(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x12");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set NB level");
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
	return val;
}

void RIG_IC9100::set_noise_reduction(int val)
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set NR");
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
	return val;
}

void RIG_IC9100::set_squelch(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x03");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set Sqlch");
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

	cmd = pre_to;
	cmd.append("\x14\x08");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set IF val");
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

void RIG_IC9100::tune_rig()
{
	cmd = pre_to;
	cmd.append("\x1c\x01\x02");
	cmd.append( post );
#ifdef IC9100_DEBUG
	stringstream ss;
	 ss << " ()" << str2hex(cmd.data(), cmd.length());
	LOG_INFO("%s", ss.str().c_str());
#endif
	waitFB("tune rig");
}

