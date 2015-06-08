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
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,
41, WVALS_LIMIT};

const char *IC7600_rtty_bws[] = {
"50",    "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700", NULL };
static int IC7600_bw_vals_RTTY[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32, WVALS_LIMIT};

const char *IC7600_am_bws[] = {
"200",   "400",  "600",  "800", "1000", "1200", "1400", "1600", "1800", "2000",
"2200", "2400", "2600", "2800", "3000", "3200", "3400", "3600", "3800", "4000",
"4200", "4400", "4600", "4800", "5000", "5200", "5400", "5600", "5800", "6000",
"6200", "6400", "6600", "6800", "7000", "7200", "7400", "7600", "7800", "8000",
"8200", "8400", "8600", "8800", "9000", "9200", "9400", "9600", "9800", "10000", NULL };
static int IC7600_bw_vals_AM[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,
41,42,43,44,45,46,47,48,49,50,
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
	name_ = IC7600name_;
	modes_ = IC7600modes_;
	bandwidths_ = IC7600_ssb_bws;
	bw_vals_ = IC7600_bw_vals_SSB;

	_mode_type = IC7600_mode_type;
	adjustCIV(defaultCIV);

	widgets = IC7600_widgets;

	has_extras =

	has_cw_wpm =
	has_cw_spot_tone =
	has_cw_qsk =

	has_vox_onoff =
	has_vox_gain =
	has_vox_anti =
	has_vox_hang =

	has_compON =
	has_compression =

	has_split =
	has_split_AB =

	has_micgain_control =
	has_bandwidth_control = true;

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

void RIG_IC7600::selectA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\xD0';
	cmd.append(post);
	sendICcommand(cmd, 6);
	checkresponse();
}

void RIG_IC7600::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\xD1';
	cmd.append(post);
	sendICcommand(cmd, 6);
	checkresponse();
}

bool RIG_IC7600::can_split()
{
	return true;
}

void RIG_IC7600::set_split(bool val)
{
	split = val;
	if (val) {
		cmd.assign(pre_to);
		cmd.append("\x0F");
		cmd += '\x01';
		cmd.append( post );
		waitFB("Split ON");
	} else {
		cmd.assign(pre_to);
		cmd.append("\x0F");
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
	if (sendICcommand (cmd, 7))
		split = replystr[5];
	return split;
}

void RIG_IC7600::swapvfos()
{
	cmd = pre_to;
	cmd += 0x07; cmd += 0xB0;
	cmd.append(post);
	if (IC7600_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	sendICcommand (cmd, 6);
	checkresponse();
}

void RIG_IC7600::set_modeA(int val)
{
	A.imode = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += IC7600_mode_nbr[val];
	cmd.append( post );
	if (IC7600_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	sendICcommand (cmd, 6);
	checkresponse();
// digital set / clear
	if (A.imode >= 10) {
		cmd = pre_to;
		cmd += '\x1A'; cmd += '\x06';
		switch (A.imode) {
			case 10 : case 13 : cmd += '\x01'; cmd += '\x01';break;
			case 11 : case 14 : cmd += '\x02'; cmd += '\x01';break;
			case 12 : case 15 : cmd += '\x03'; cmd += '\x01';break;
			default :
				cmd += '\x00'; cmd += '\x00';
		}
		cmd.append( post);
		if (IC7600_DEBUG)
			LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
		sendICcommand (cmd, 6);
		checkresponse();
	}
}

int RIG_IC7600::get_modeA()
{
	int md = 0;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	if (sendICcommand (cmd, 8 )) {
		for (md = 0; md < 10; md++) if (replystr[5] == IC7600_mode_nbr[md]) break;
		if (md == 10) md = 0;
		A.imode = md;
	} else
		checkresponse();
	if (md == 0 || md == 1) {
		cmd = pre_to;
		cmd += '\x1A'; cmd += '\x06';
		cmd.append(post);
		if (sendICcommand(cmd, 9)) {
			if (replystr[6] > 0 && A.imode < 2) {
				if (replystr[6] < 4)
					A.imode = 9 + A.imode * 3 + replystr[6];
			}
		}
	}
	return A.imode;
}

void RIG_IC7600::set_modeB(int val)
{
	B.imode = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += IC7600_mode_nbr[val];
	cmd.append( post );
	if (IC7600_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	sendICcommand (cmd, 6);
	checkresponse();
// digital set / clear
	if (B.imode >= 10) {
		cmd = pre_to;
		cmd += '\x1A'; cmd += '\x06';
		switch (B.imode) {
			case 10 : case 13 : cmd += '\x01'; cmd += '\x01';break;
			case 11 : case 14 : cmd += '\x02'; cmd += '\x01';break;
			case 12 : case 15 : cmd += '\x03'; cmd += '\x01';break;
			default :
				cmd += '\x00'; cmd += '\x00';
		}
		cmd.append( post);
		if (IC7600_DEBUG)
			LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
		sendICcommand (cmd, 6);
		checkresponse();
	}
}

int RIG_IC7600::get_modeB()
{
	int md = 0;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	if (sendICcommand (cmd, 8 )) {
		for (md = 0; md < 10; md++) if (replystr[5] == IC7600_mode_nbr[md]) break;
		if (md == 10) md = 0;
		B.imode = md;
	} else
		checkresponse();
	if (md == 0 || md == 1) {
		cmd = pre_to;
		cmd += '\x1A'; cmd += '\x06';
		cmd.append(post);
		if (sendICcommand(cmd, 9)) {
			if (replystr[6] > 0 && B.imode < 2) {
				if (replystr[6] < 4)
					B.imode = 9 + B.imode * 3 + replystr[6];
			}
		}
	}
	return B.imode;
}

int RIG_IC7600::get_bwA()
{
	if (A.imode == 5) return 0;
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(post);
	if (sendICcommand (cmd, 8)) {
		A.iBW = fm_bcd(&replystr[6], 2);
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
	if (IC7600_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	sendICcommand (cmd, 6);
	checkresponse();
}

int RIG_IC7600::get_bwB()
{
	if (B.imode == 5) return 0;
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(post);
	if (sendICcommand (cmd, 8)) {
		B.iBW = fm_bcd(&replystr[6], 2);
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
	if (IC7600_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	sendICcommand (cmd, 6);
	checkresponse();
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

void RIG_IC7600::set_mic_gain(int v)
{
	ICvol = (int)(v * 255 / 100);
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
	sendICcommand (cmd, 6);
	checkresponse();
	if (RIG_DEBUG)
		LOG_WARN("%s", str2hex(cmd.data(), cmd.length()));
}

// alh added ++++++++++++++++++++++++++++

void RIG_IC7600::set_attenuator(int val)
{
	int cmdval = 0;
	if (atten_level == 0) {
		atten_level = 1;
		atten_label("6 dB", true);
		cmdval = 0x06;
	} else if (atten_level == 1) {
		atten_level = 2;
		atten_label("12 dB", true);
		cmdval = 0x12;
	} else if (atten_level == 2) {
		atten_level = 3;
		atten_label("18 dB", true);
		cmdval = 0x18;
	} else if (atten_level == 3) {
		atten_level = 0;
		atten_label("Att", false);
		cmdval = 0x00;
	}
	cmd = pre_to;
	cmd += '\x11';
	cmd += cmdval;
	cmd.append( post );
	sendICcommand(cmd,6);
	checkresponse();
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

int RIG_IC7600::get_attenuator()
{
	cmd = pre_to;
	cmd += '\x11';
	cmd.append( post );
	if (sendICcommand(cmd,7)) {
		if (replystr[5] == 0x06) {
			atten_level = 1;
			atten_label("6 dB", true);
		} else if (replystr[6] == 0x12) {
			atten_level = 2;
			atten_label("12 dB", true);
		} else if (replystr[6] == 0x18) {
			atten_level = 3;
			atten_label("18 dB", true);
		} else if (replystr[6] == 0x00) {
			atten_level = 0;
			atten_label("Att", false);
		}
	}
	return atten_level;
}

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

