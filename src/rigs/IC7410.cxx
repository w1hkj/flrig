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

#include "IC7410.h"

bool IC7410_DEBUG = true;

//=============================================================================
// IC-7410
/*

*/

const char IC7410name_[] = "IC-7410";

const char *IC7410modes_[] = {
	"LSB", "USB", "AM", "CW", "RTTY", "FM",  "CW-R", "RTTY-R", 
	"LSB-D", "USB-D", "FM-D", NULL};

enum {
	LSB7410, USB7410, AM7410, CW7410, RTTY7410, FM7410,  CWR7410, RTTYR7410,
	LSBD7410, USBD7410, FMD7410 };

const char IC7410_mode_type[] = {
	'L', 'U', 'U', 'L', 'L', 'U', 'L', 'U', 'L', 'U', 'U' };

const char IC7410_mode_nbr[] = {
	0x00, // Select the LSB mode
	0x01, // Select the USB mode
	0x02, // Select the AM mode
	0x03, // Select the CW mode
	0x04, // Select the RTTY mode
	0x05, // Select the FM mode
	0x07, // Select the CW-R mode
	0x08, // Select the RTTY-R mode
	0x00, // select lsb-data mode
	0x01, // select usb-data mode
	0x05, // select fm-data mode
};

const char *IC7410_ssb_bws[] = {
"50",    "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700", "2800", "2900", "3000", "3100", "3200", "3300", "3400", "3500",
"3600",
NULL };

const char *IC7410_rtty_bws[] = {
"50",    "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700",
NULL };

const char *IC7410_am_bws[] = {
"200",   "400",  "600",  "800", "1000", "1200", "1400", "1600", "1800", "2000",
"2200", "2400", "2600", "2800", "3000", "3200", "3400", "3600", "3800", "4000",
"4200", "4400", "4600", "4800", "5000", "5200", "5400", "5600", "5800", "6000",
"6200", "6400", "6600", "6800", "7000", "7200", "7400", "7410", "7800", "8000",
"8200", "8400", "8600", "8800", "9000", "9200", "9400", "9600", "9800", "10000", 
NULL };

const char *IC7410_fm_bws[] = {
"FIXED", NULL };

static GUI IC7410_widgets[]= {
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

RIG_IC7410::RIG_IC7410() {
	defaultCIV = 0x80;
	name_ = IC7410name_;
	modes_ = IC7410modes_;
	bandwidths_ = IC7410_ssb_bws;
	_mode_type = IC7410_mode_type;
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

	widgets = IC7410_widgets;

	has_extras =

	has_cw_wpm =
	has_cw_spot_tone =
	has_cw_qsk =
	has_cw_vol =

	has_vox_onoff =
	has_vox_gain =
	has_vox_anti =
	has_vox_hang =

	has_compON =
	has_compression =

	has_micgain_control =
	has_bandwidth_control = true;

	precision = 1;
	ndigits = 9;

};

//======================================================================
// IC7410 unique commands
//======================================================================

void RIG_IC7410::initialize()
{
	IC7410_widgets[0].W = btnVol;
	IC7410_widgets[1].W = sldrVOLUME;
	IC7410_widgets[2].W = sldrRFGAIN;
	IC7410_widgets[3].W = sldrSQUELCH;
	IC7410_widgets[4].W = btnNR;
	IC7410_widgets[5].W = sldrNR;
	IC7410_widgets[6].W = btnIFsh;
	IC7410_widgets[7].W = sldrIFSHIFT;
	IC7410_widgets[8].W = btnNotch;
	IC7410_widgets[9].W = sldrNOTCH;
	IC7410_widgets[10].W = sldrMICGAIN;
	IC7410_widgets[11].W = sldrPOWER;
}

void RIG_IC7410::selectA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x00';
	cmd.append(post);
	waitFB("select A");
}

void RIG_IC7410::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x01';
	cmd.append(post);
	waitFB("select B");
}

void RIG_IC7410::set_modeA(int val)
{
	A.imode = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += IC7410_mode_nbr[val];
	cmd.append( post );
	waitFB("set mode A");
// digital set / clear
	if (val == LSBD7410 || val == USBD7410 || val == FMD7410) {
		cmd = pre_to;
		cmd += '\x1A'; cmd += '\x06';
		cmd += '\x01'; cmd += '\x01';
		cmd.append( post);
		waitFB("set digital");
	}
}

int RIG_IC7410::get_modeA()
{
	int md = 0;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	string resp = pre_fm;
	resp += '\x04';
	if (waitFOR(8, "get mode A")) {
		size_t p = replystr.rfind(resp);
		for (md = LSB7410; md <= RTTYR7410; md++)
			if (replystr[p+6] == IC7410_mode_nbr[md]) break;
		if (md == LSB7410 || md == USB7410 || md == FM7410) {
			cmd = pre_to;
			cmd.append("\x1a\x06");
			cmd.append(post);
			resp = pre_fm;
			resp.append("\x1a\x06");
			if (waitFOR(9, "get digital setting")) {
				size_t p = replystr.rfind(resp);
				if (replystr[p+6] == 0x01) {
					if (md == LSB7410) md = LSBD7410;
					else if (md == USB7410) md = USBD7410;
					else if (md == FM7410) md = FMD7410;
				}
			}
		}
		A.imode = md;
	}
	return A.imode;
}

void RIG_IC7410::set_modeB(int val)
{
	B.imode = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += IC7410_mode_nbr[val];
	cmd.append( post );
	waitFB("set mode B");
	if (val == LSBD7410 || val == USBD7410 || val == FMD7410) {
		cmd = pre_to;
		cmd += '\x1A'; cmd += '\x06';
		cmd += '\x01'; cmd += '\x01';
		cmd.append( post);
		waitFB("set digital");
	}
}

int RIG_IC7410::get_modeB()
{
	int md = 0;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	string resp = pre_fm;
	resp += '\x04';
	if (waitFOR(8, "get mode B")) {
		size_t p = replystr.rfind(resp);
		for (md = LSB7410; md <= RTTYR7410; md++)
			if (replystr[p+6] == IC7410_mode_nbr[md]) break;
		if (md == USB7410 || md == AM7410 || md == FM7410) {
			cmd = pre_to;
			cmd.append("\x1a\x06");
			cmd.append(post);
			resp = pre_fm;
			resp.append("\x1a\x06");
			if (waitFOR(9, "get digital")) {
				size_t p = replystr.rfind(resp);
				if (replystr[p+6] == 0x01) {
					if (md == LSB7410) md = LSBD7410;
					else if (md == USB7410) md = USBD7410;
					else if (md == FM7410) md = FMD7410;
				}
			}
		}
		B.imode = md;
	}
	return B.imode;
}

int RIG_IC7410::get_bwA()
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

void RIG_IC7410::set_bwA(int val)
{
	if (A.imode == FM7410 || A.imode == FMD7410) return;
	A.iBW = val;
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(to_bcd(A.iBW, 2));
	cmd.append(post);
	waitFB("set bw A");
}

int RIG_IC7410::get_bwB()
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

void RIG_IC7410::set_bwB(int val)
{
	if (B.imode == FM7410 || B.imode == FMD7410) return;
	B.iBW = val;
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(to_bcd(B.iBW, 2));
	cmd.append(post);
	waitFB("set bw A");
}

int RIG_IC7410::adjust_bandwidth(int m)
{
	switch (m) {
		case USB7410: case LSB7410: case LSBD7410 : case USBD7410:
			bandwidths_ = IC7410_ssb_bws;
			return 34;
		case AM7410:
			bandwidths_ = IC7410_am_bws;
			return 29;
			break;
		case FM7410: case FMD7410:
			bandwidths_ = IC7410_fm_bws;
			return 0;
			break;
		case RTTY7410: case RTTYR7410:
			bandwidths_ = IC7410_rtty_bws;
			return 12;
			break;
		case CW7410: case CWR7410:
			bandwidths_ = IC7410_ssb_bws;
			return 10;
			break;
		default:
			bandwidths_ = IC7410_ssb_bws;
			return 34;
	}
	return 0;
}

int RIG_IC7410::def_bandwidth(int m)
{
	return adjust_bandwidth(m);
}

void RIG_IC7410::set_mic_gain(int v)
{
	ICvol = (int)(v * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x0B");
	cmd.append(to_bcd(ICvol, 3));
	cmd.append( post );
	waitFB("set mic gain");
}

void RIG_IC7410::set_attenuator(int val)
{
	int cmdval = 0;
	if (atten_level == 0) {
		atten_level = 1;
		atten_label("20 dB", true);
		cmdval = 0x20;
	} else if (atten_level == 1) {
		atten_level = 0;
		atten_label("Att", false);
		cmdval = 0x00;
	}
	cmd = pre_to;
	cmd += '\x11';
	cmd += cmdval;
	cmd.append( post );
	waitFB("set attenuator");
}

int RIG_IC7410::get_attenuator()
{
	cmd = pre_to;
	cmd += '\x11';
	cmd.append( post );
	string resp = pre_fm;
	resp += '\x11';
	if (waitFOR(7, "get attenuator")) {
		size_t p = replystr.rfind(resp);
		if (replystr[p+6] == 0x20) {
			atten_level = 1;
			atten_label("20 dB", true);
		} else {
			atten_level = 0;
			atten_label("Att", false);
		}
	}
	return atten_level;
}

void RIG_IC7410::set_compression(int on, int val)
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

void RIG_IC7410::set_vox_onoff()
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

void RIG_IC7410::set_vox_gain()
{
	cmd.assign(pre_to).append("\x1A\x05"); // values 0-255 = 0 - 100%
	cmd +='\x00';
	cmd +='\x87';
	cmd.append(to_bcd((int)(progStatus.vox_gain * 2.55), 3));
	cmd.append( post );
	waitFB("SET vox gain");
}

void RIG_IC7410::set_vox_anti()
{
	cmd.assign(pre_to).append("\x1A\x05");	// values 0-255 = 0 - 100%
	cmd +='\x00';
	cmd +='\x88';
	cmd.append(to_bcd((int)(progStatus.vox_anti * 2.55), 3));
	cmd.append( post );
	waitFB("SET anti-vox");
}

void RIG_IC7410::set_vox_hang()
{
	cmd.assign(pre_to).append("\x1A\x05");	// values 00-20 = 0.0 - 2.0 sec
	cmd +='\x00';
	cmd +='\x89';
	cmd.append(to_bcd((int)(progStatus.vox_hang / 10 ), 2));
	cmd.append( post );
	waitFB("SET vox hang");
}

// CW controls

void RIG_IC7410::set_cw_wpm()
{
	cmd.assign(pre_to).append("\x14\x0C"); // values 0-255 = 6 to 48 WPM
	cmd.append(to_bcd(round((progStatus.cw_wpm - 6) * 255 / (48 - 6)), 3));
	cmd.append( post );
	waitFB("SET cw wpm");
}

void RIG_IC7410::set_cw_qsk()
{
	int n = round(progStatus.cw_qsk * 10); // values 0-255
	cmd.assign(pre_to).append("\x14\x0F");
	cmd.append(to_bcd(n, 3));
	cmd.append(post);
	waitFB("Set cw qsk delay");
}

void RIG_IC7410::set_cw_spot_tone()
{
	cmd.assign(pre_to).append("\x14\x09"); // values 0=300Hz 255=900Hz
	int n = round((progStatus.cw_spot_tone - 300) * 255.0 / 600.0);
	if (n > 255) n = 255;
	if (n < 0) n = 0;
	cmd.append(to_bcd(n, 3));
	cmd.append( post );
	waitFB("SET cw spot tone");
}

void RIG_IC7410::set_cw_vol()
{
	cmd.assign(pre_to);
	cmd.append("\x14\x15");
	cmd.append(to_bcd((int)(progStatus.cw_vol * 2.55), 3));
	cmd.append( post );
	waitFB("SET cw sidetone volume");
}
