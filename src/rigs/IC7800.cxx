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

#include "IC7800.h"

bool IC7800_DEBUG = true;

//=============================================================================
// IC-7800

const char IC7800name_[] = "IC-7800";

enum {
	LSB7800, USB7800, AM7800, CW7800, RTTY7800,
	FM7800,  CWR7800, RTTYR7800, PSK7800, PSKR7800,
	LSBD7800, USBD7800, AMD7800 };

const char *IC7800modes_[] = {
	"LSB", "USB", "AM", "CW", "RTTY",
	"FM", "CW-R", "RTTY-R", "PSK", "PSK-R", 
	"LSB-D", "USB-D", "AM-D", NULL};

const char IC7800_mode_type[] = {
	'L', 'U', 'U', 'U', 'L',
	'U', 'L', 'U', 'U', 'L',
	'L', 'U', 'U' };

const char IC7800_mode_nbr[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x07, 0x08, 0x12, 0x13,
	0X00, 0X01, 0X02 };

const char *IC7800_ssb_bws[] = {
"50",    "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700", "2800", "2900", "3000", "3100", "3200", "3300", "3400", "3500",
"3600", NULL };
static int IC7800_bw_vals_SSB[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,
41, WVALS_LIMIT};

const char *IC7800_rtty_bws[] = {
"50",    "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700", NULL };
static int IC7800_bw_vals_RTTY[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32, WVALS_LIMIT};

const char *IC7800_am_bws[] = {
"200",   "400",  "600",  "800", "1000", "1200", "1400", "1600", "1800", "2000",
"2200", "2400", "2600", "2800", "3000", "3200", "3400", "3600", "3800", "4000",
"4200", "4400", "4600", "4800", "5000", "5200", "5400", "5600", "5800", "6000",
"6200", "6400", "6600", "6800", "7000", "7200", "7400", "7800", "7800", "8000",
"8200", "8400", "8600", "8800", "9000", "9200", "9400", "9600", "9800", "10000", NULL };
static int IC7800_bw_vals_AM[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,
41,42,43,44,45,46,47,48,49,50,
WVALS_LIMIT};

const char *IC7800_fm_bws[] = { "FIXED", NULL };
static int IC7800_bw_vals_FM[] = { 1, WVALS_LIMIT};

static GUI IC7800_widgets[]= {
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

void RIG_IC7800::initialize()
{
	IC7800_widgets[0].W = btnVol;
	IC7800_widgets[1].W = sldrVOLUME;
	IC7800_widgets[2].W = btnAGC;
	IC7800_widgets[3].W = sldrRFGAIN;
	IC7800_widgets[4].W = sldrSQUELCH;
	IC7800_widgets[5].W = btnNR;
	IC7800_widgets[6].W = sldrNR;
	IC7800_widgets[7].W = btnLOCK;
	IC7800_widgets[8].W = sldrINNER;
	IC7800_widgets[9].W = btnCLRPBT;
	IC7800_widgets[10].W = sldrOUTER;
	IC7800_widgets[11].W = btnNotch;
	IC7800_widgets[12].W = sldrNOTCH;
	IC7800_widgets[13].W = sldrMICGAIN;
	IC7800_widgets[14].W = sldrPOWER;

	btn_icom_select_11->deactivate();
	btn_icom_select_12->deactivate();
	btn_icom_select_13->deactivate();

	choice_rTONE->activate();
	choice_tTONE->activate();
}

RIG_IC7800::RIG_IC7800() {
	defaultCIV = 0x6A;
	name_ = IC7800name_;
	modes_ = IC7800modes_;
	bandwidths_ = IC7800_ssb_bws;
	bw_vals_ = IC7800_bw_vals_SSB;

	_mode_type = IC7800_mode_type;
	adjustCIV(defaultCIV);

	widgets = IC7800_widgets;

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

	has_micgain_control =
	has_bandwidth_control =
	has_smeter =
	has_power_control =
	has_volume_control =
	has_mode_control =
	has_notch_control =
	has_attenuator_control =
	has_preamp_control =
	has_ifshift_control =
	has_pbt_controls =
	has_FILTER = 
	has_ptt_control =
	has_tune_control =
	has_noise_control =
	has_noise_reduction =
	has_noise_reduction_control =
	has_rf_control =
	has_sql_control =
	has_split_AB =
	has_split = true;

	ICOMmainsub = true;

	has_band_selection = true;

	precision = 1;
	ndigits = 8;
	filA = filB = 1;

	def_freq = A.freq = 14070000;
	def_mode = A.imode = 1;
	def_bw = A.iBW = 34;

	B.freq = 7070000;
	B.imode = 1;
	B.iBW = 34;

};

//======================================================================
// IC7800 unique commands
//======================================================================

void RIG_IC7800::selectA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\xD0';
	cmd.append(post);
	sendICcommand(cmd, 6);
	checkresponse();
}

void RIG_IC7800::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\xD1';
	cmd.append(post);
	sendICcommand(cmd, 6);
	checkresponse();
}

bool RIG_IC7800::can_split()
{
	return true;
}

void RIG_IC7800::set_split(bool val)
{
	split = val;
	cmd = pre_to;
	cmd += 0x0F;
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	waitFB(val ? "set split ON" : "set split OFF");
}

int RIG_IC7800::get_split()
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
	return split;
}

void RIG_IC7800::set_modeA(int val)
{
	A.imode = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += IC7800_mode_nbr[val];
	cmd += filA;
	cmd.append( post );
	if (IC7800_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	sendICcommand (cmd, 6);
	checkresponse();
// digital set / clear
	if (val == LSBD7800 || val == USBD7800 || val == AMD7800) {
		cmd = pre_to;
		cmd += '\x1A'; cmd += '\x06';
		cmd += '\x01'; cmd += '\x01';
		cmd += filA;
		cmd.append( post);
		waitFB("set digital");
	}
}

static const char *szfilter[] = {"1", "2", "3"};

int RIG_IC7800::get_modeA()
{
	int md = 0;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	if (sendICcommand (cmd, 8 )) {
		for (md = 0; md < 10; md++) if (replystr[5] == IC7800_mode_nbr[md]) break;
		if (md == 10) md = 0;
		A.imode = md;
	} else
		checkresponse();
	if (md == LSB7800 || md == USB7800 || md == AM7800) {
		cmd = pre_to;
		cmd += '\x1A'; cmd += '\x06';
		cmd.append(post);
		if (sendICcommand(cmd, 9)) {
			if (replystr[6] == 0x01 && A.imode == 0x01) {
					if (A.imode == LSB7800) A.imode = LSBD7800;
					if (A.imode == USB7800) A.imode = USBD7800;
					if (A.imode == AM7800) A.imode = AMD7800;
			}
		}
	}
	return A.imode;
}

void RIG_IC7800::set_modeB(int val)
{
	B.imode = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += IC7800_mode_nbr[val];
	cmd += filB;
	cmd.append( post );
	if (IC7800_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	sendICcommand (cmd, 6);
	checkresponse();
// digital set / clear
	if (val == LSBD7800 || val == USBD7800 || val == AMD7800) {
		cmd = pre_to;
		cmd += '\x1A'; cmd += '\x06';
		cmd += '\x01'; cmd += '\x01';
		cmd += filB;
		cmd.append( post);
		waitFB("set digital");
	}
}

int RIG_IC7800::get_modeB()
{
	int md = 0;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	if (sendICcommand (cmd, 8 )) {
		for (md = 0; md < 10; md++) if (replystr[5] == IC7800_mode_nbr[md]) break;
		if (md == 10) md = 0;
		B.imode = md;
	} else
		checkresponse();
	if (md == LSB7800 || md == USB7800 || md == AM7800) {
		cmd = pre_to;
		cmd += '\x1A'; cmd += '\x06';
		cmd.append(post);
		if (sendICcommand(cmd, 9)) {
			if (replystr[6] == 0x01 && A.imode == 0x01) {
					if (B.imode == LSB7800) B.imode = LSBD7800;
					if (B.imode == USB7800) B.imode = USBD7800;
					if (B.imode == AM7800) B.imode = AMD7800;
			}
		}
	}
	return B.imode;
}

int RIG_IC7800::get_bwA()
{
	if (A.imode == 5) return 0;
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(post);
	if (sendICcommand (cmd, 8)) {
		A.iBW = fm_bcd(replystr.substr(6), 2);
	}
	return A.iBW;
}

void RIG_IC7800::set_bwA(int val)
{
	A.iBW = val;
	if (A.imode == 5) return;
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(to_bcd(A.iBW, 2));
	cmd.append(post);
	if (IC7800_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	sendICcommand (cmd, 6);
	checkresponse();
}

int RIG_IC7800::get_bwB()
{
	if (B.imode == 5) return 0;
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(post);
	if (sendICcommand (cmd, 8)) {
		B.iBW = fm_bcd(replystr.substr(6), 2);
	}
	return B.iBW;
}

void RIG_IC7800::set_bwB(int val)
{
	B.iBW = val;
	if (B.imode == 5) return;
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(to_bcd(A.iBW, 2));
	cmd.append(post);
	if (IC7800_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	sendICcommand (cmd, 6);
	checkresponse();
}

int RIG_IC7800::adjust_bandwidth(int m)
{
	int bw = 0;
	switch (m) {
		case 2: // AM
			bandwidths_ = IC7800_am_bws;
			bw_vals_ = IC7800_bw_vals_AM;
			bw = 19;
			break;
		case 5: // FM
			bandwidths_ = IC7800_fm_bws;
			bw_vals_ = IC7800_bw_vals_FM;
			bw = 0;
			break;
		case 4: case 8: // RTTY
			bandwidths_ = IC7800_rtty_bws;
			bw_vals_ = IC7800_bw_vals_RTTY;
			bw = 12;
			break;
		case 3: case 7: // CW
			bandwidths_ = IC7800_ssb_bws;
			bw_vals_ = IC7800_bw_vals_SSB;
			bw = 12;
			break;
		case 0: case 1: // SSB
		case 12: case 13: // PKT
		default:
			bandwidths_ = IC7800_ssb_bws;
			bw_vals_ = IC7800_bw_vals_SSB;
			bw = 34;
			break;
	}
	return bw;
}

const char ** RIG_IC7800::bwtable(int m)
{
	const char **table;
	switch (m) {
		case 2: // AM
			table = IC7800_am_bws;
			break;
		case 5: // FM
			table = IC7800_fm_bws;
			break;
		case 4: case 8: // RTTY
			table = IC7800_rtty_bws;
			break;
		case 3: case 7: // CW
		case 0: case 1: // SSB
		case 12: case 13: // PKT
		default:
			table = IC7800_ssb_bws;
			break;
	}
	return table;
}

int RIG_IC7800::def_bandwidth(int m)
{
	int bw = 0;
	switch (m) {
		case 2: // AM
			bw = 19;
			break;
		case 5: // FM
			bw = 0;
			break;
		case 4: case 8: // RTTY
			bw = 12;
			break;
		case 3: case 7: // CW
			bw = 12;
			break;
		case 0: case 1: // SSB
		case 12: case 13: // PKT
		default:
			bw = 34;
	}
	return bw;
}

void RIG_IC7800::set_mic_gain(int v)
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
}

static const char *atten_labels[] = {
"ATT", "3 dB", "6 dB", "9 dB", "12 dB", "15 dB", "18 dB", "21 dB"};

int  RIG_IC7800::next_attenuator()
{
	if (atten_level >= 7) return 0;
	else return (atten_level + 1);
}

void RIG_IC7800::set_attenuator(int val)
{
	atten_level = val;

	int cmdval = atten_level;
	atten_label(atten_labels[atten_level], true);
	cmd = pre_to;
	cmd += '\x11';
	cmd += cmdval;
	cmd.append( post );
	sendICcommand(cmd,6);
	checkresponse();
}

int RIG_IC7800::get_attenuator()
{
	cmd = pre_to;
	cmd += '\x11';
	cmd.append( post );
	if (sendICcommand(cmd,7)) {
		if (replystr[4] == 0x06) {
			atten_level = replystr[5];
			if (atten_level >= 0 && atten_level <= 7)
				atten_label(atten_labels[atten_level], true);
		}
	}
	return atten_level;
}

void RIG_IC7800::set_compression(int on, int val)
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

void RIG_IC7800::set_vox_onoff()
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

void RIG_IC7800::set_vox_gain()
{
	cmd.assign(pre_to).append("\x14\x16"); // ALH values 0-255
	cmd.append(to_bcd((int)(progStatus.vox_gain * 2.55), 3));
	cmd.append( post );
	waitFB("SET vox gain");
}

void RIG_IC7800::set_vox_anti()
{
	cmd.assign(pre_to).append("\x14\x17");	//ALH values 0-255
	cmd.append(to_bcd((int)(progStatus.vox_anti * 2.55), 3));
	cmd.append( post );
	waitFB("SET anti-vox");
}

void RIG_IC7800::set_vox_hang()
{
	cmd.assign(pre_to).append("\x1A\x05\0x01\0x83");	//ALH values 00-20 = 0.0 - 2.0 sec
	cmd.append(to_bcd((int)(progStatus.vox_hang / 10 ), 2));
	cmd.append( post );
	waitFB("SET vox hang");
}

// CW controls

void RIG_IC7800::set_cw_wpm()
{
	cmd.assign(pre_to).append("\x14\x0C"); // values 0-255
	cmd.append(to_bcd(round((progStatus.cw_wpm - 6) * 255 / (60 - 6)), 3));
	cmd.append( post );
	waitFB("SET cw wpm");
}

void RIG_IC7800::set_cw_qsk()
{
	int n = round(progStatus.cw_qsk * 10); // values 0-255
	cmd.assign(pre_to).append("\x14\x0F");
	cmd.append(to_bcd(n, 3));
	cmd.append(post);
	waitFB("Set cw qsk delay");
}

void RIG_IC7800::set_cw_spot_tone()
{
	cmd.assign(pre_to).append("\x14\x09"); // values 0=300Hz 255=900Hz
	int n = round((progStatus.cw_spot_tone - 300) * 255.0 / 600.0);
	if (n > 255) n = 255;
	if (n < 0) n = 0;
	cmd.append(to_bcd(n, 3));
	cmd.append( post );
	waitFB("SET cw spot tone");
}

void RIG_IC7800::set_cw_vol()
{
	cmd.assign(pre_to);
	cmd.append("\x14\x15");
	cmd.append(to_bcd((int)(progStatus.cw_vol * 2.55), 3));
	cmd.append( post );
	waitFB("SET cw sidetone volume");
}

// Tranceiver PTT on/off
void RIG_IC7800::set_PTT_control(int val)
{
	cmd = pre_to;
	cmd += '\x1c';
	cmd += '\x00';
	cmd += (unsigned char) val;
	cmd.append( post );
	waitFB("set ptt");
	ptt_ = val;
}

int RIG_IC7800::get_PTT()
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

void RIG_IC7800::set_pbt_inner(int val)
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

void RIG_IC7800::set_pbt_outer(int val)
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

int RIG_IC7800::get_pbt_inner()
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

int RIG_IC7800::get_pbt_outer()
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

const char *RIG_IC7800::FILT(int &val)
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

const char *RIG_IC7800::nextFILT()
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

void RIG_IC7800::get_band_selection(int v)
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
			if ((bandmode == 0) && banddata) bandmode = 10;
			if ((bandmode == 1) && banddata) bandmode = 11;
			if ((bandmode == 2) && banddata) bandmode = 12;
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

void RIG_IC7800::set_band_selection(int v)
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
	if (mode == LSBD7800 || mode == USBD7800 || mode == AMD7800)
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

void RIG_IC7800::set_notch(bool on, int freq)
{
	int hexval;
	switch (vfo->imode) {
		default: case USB7800: case USBD7800: case RTTYR7800:
			hexval = freq - 1500;
			break;
		case LSB7800: case LSBD7800: case RTTY7800:
			hexval = 1500 - freq;
			break;
		case CW7800:
			hexval = progStatus.cw_spot_tone - freq;
			break;
		case CWR7800:
			hexval = freq - progStatus.cw_spot_tone;
			break;
	}

	hexval /= 20;
	hexval += 128;
	if (hexval < 0) hexval = 0;
	if (hexval > 255) hexval = 255;

	cmd = pre_to;
	cmd.append("\x16\x48");
	cmd += on ? '\x01' : '\x00';
	cmd.append(post);
	waitFB("set notch");

	cmd = pre_to;
	cmd.append("\x14\x0D");
	cmd.append(to_bcd(hexval,3));
	cmd.append(post);
	waitFB("set notch val");
}

bool RIG_IC7800::get_notch(int &val)
{
	bool on = false;
	val = 1500;

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
			if (p != string::npos) {
				val = (int)ceil(fm_bcd(replystr.substr(p+6),3));
				val -= 128;
				val *= 20;
				switch (vfo->imode) {
					default: case USB7800: case USBD7800: case RTTYR7800:
						val = 1500 + val;
						break;
					case LSB: case LSBD7800: case RTTY7800:
						val = 1500 - val;
						break;
					case CW7800:
						val = progStatus.cw_spot_tone - val;
						break;
					case CWR7800:
						val = progStatus.cw_spot_tone + val;
						break;
				}
			}
		}
	}
	return on;
}

void RIG_IC7800::get_notch_min_max_step(int &min, int &max, int &step)
{
	switch (vfo->imode) {
		default:
		case USB7800: case USBD7800: case RTTYR7800:
		case LSB7800: case LSBD7800: case RTTY7800:
			min = 0; max = 3000; step = 20; break;
		case CW7800: case CWR7800:
			min = progStatus.cw_spot_tone - 500;
			max = progStatus.cw_spot_tone + 500;
			step = 20;
			break;
	}
}

