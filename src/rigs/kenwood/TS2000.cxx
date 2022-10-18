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

#include "kenwood/TS2000.h"
#include "support.h"

static const char TS2000name_[] = "TS-2000";

static const char *TS2000modes_[] = {
	"LSB", "USB", "CW", "FM", "AM", "FSK", "CW-R", "FSK-R", NULL};

static const char TS2000_mode_chr[] =  { '1', '2', '3', '4', '5', '6', '7', '9' };
static const char TS2000_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'L', 'L', 'U' };

static const char *TS2000_empty[] = { "N/A", NULL };
//------------------------------------------------------------------------------
static const char *TS2000_SL[] = {
 "0",   "50", "100", "200", "300",
"400",  "500", "600", "700", "800",
"900", "1000", NULL };
static const char *TS2000_CAT_SL[] = {
"SL00;", "SL01;", "SL02;", "SL03;", "SL04;",
"SL05;", "SL06;", "SL07;", "SL08;", "SL09;",
"SL10;", "SL11;" };
static const char *TS2000_SL_tooltip = "lo cut";
static const char *TS2000_SSB_btn_SL_label = "L";
//------------------------------------------------------------------------------
static const char *TS2000_SH[] = {
"1400", "1600", "1800", "2000", "2200",
"2400", "2600", "2800", "3000", "3400",
"4000", "5000", NULL };
static const char *TS2000_CAT_SH[] = {
"SH00;", "SH01;", "SH02;", "SH03;", "SH04;",
"SH05;", "SH06;", "SH07;", "SH08;", "SH09;",
"SH10;", "SH11;" };
static const char *TS2000_SH_tooltip = "hi cut";
static const char *TS2000_SSB_btn_SH_label = "H";
//------------------------------------------------------------------------------
static const char *TS2000_AM_SL[] = {
"0", "100", "200", "500", NULL };
static const char *TS2000_AM_SH[] = {
"2500", "3000", "4000", "5000" };
//------------------------------------------------------------------------------
static const char *TS2000_CWwidths[] = {
"50", "80", "100", "150", "200",
"300", "400", "500", "600", "1000",
"2000", NULL};
static const char *TS2000_CWbw[] = {
"FW0050;", "FW0080;", "FW0100;", "FW0150;", "FW0200;",
"FW0300;", "FW0400;", "FW0500;", "FW0600;", "FW1000;",
"FW2000;" };
//------------------------------------------------------------------------------
static const char *TS2000_FSKwidths[] = {
"250", "500", "1000", "1500", NULL};
static const char *TS2000_FSKbw[] = {
"FW0250;", "FW0500;", "FW1000;", "FW1500;" };
//------------------------------------------------------------------------------

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 }, // 0
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 }, // 1
	{ (Fl_Widget *)sldrRFGAIN,   54, 145, 156 }, // 2
	{ (Fl_Widget *)btnIFsh,     214, 105,  50 }, // 3
	{ (Fl_Widget *)sldrIFSHIFT, 266, 105, 156 }, // 4
	{ (Fl_Widget *)btnNotch,    214, 125,  50 }, // 5
	{ (Fl_Widget *)sldrNOTCH,   266, 125, 156 }, // 6
	{ (Fl_Widget *)sldrSQUELCH, 266, 145, 156 }, // 7
	{ (Fl_Widget *)sldrMICGAIN, 266, 165, 156 }, // 8
	{ (Fl_Widget *)sldrPOWER,    54, 165, 368 }, // 9
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

// mid range on loudness
static std::string menu012 = "EX01200004";

void RIG_TS2000::initialize()
{
	rig_widgets[0].W = btnVol;
	rig_widgets[1].W = sldrVOLUME;
	rig_widgets[2].W = sldrRFGAIN;
	rig_widgets[3].W = btnIFsh;
	rig_widgets[4].W = sldrIFSHIFT;
	rig_widgets[5].W = btnNotch;
	rig_widgets[6].W = sldrNOTCH;
	rig_widgets[7].W = sldrSQUELCH;
	rig_widgets[8].W = sldrMICGAIN;
	rig_widgets[9].W = sldrPOWER;

	menu012.clear();
	cmd = "EX0120000;"; // read menu 012 state
//might return something like EX01200004;

	if (wait_char(';', 11, 100, "read ex 012", ASC) == 11)
		menu012 = replystr;

// disable beeps before resetting front panel display to SWR
	cmd = "EX01200000;";
	sendCommand(cmd);
	sett("No beeps");
	select_swr();

// restore state of xcvr beeps
//	cmd = menu012;
//	sendCommand(cmd);

// get current noise reduction values for NR1 and NR2
	std::string current_nr;
	cmd = "NR;";
	if (wait_char(';', 4, 100, "read current NR", ASC) == 4)
		current_nr = replystr;
	if (current_nr == "?;") return;

	cmd = "NR1;";
	sendCommand(cmd);
	gett("get NR");
	cmd = "RL;";
	if (wait_char(';', 5, 100, "GET noise reduction val", ASC) == 5) {
		size_t p = replystr.rfind("RL");
		if (p != std::string::npos)
			_nrval1 = atoi(&replystr[p+2]);
	}
	cmd = "NR2;";
	sendCommand(cmd);
	gett("get NR value");

	cmd = "RL;";
	if (wait_char(';', 5, 100, "GET noise reduction val", ASC) == 5) {
		size_t p = replystr.rfind("RL");
		if (p != std::string::npos)
			_nrval2 = atoi(&replystr[p+2]);
	}

// restore xcvr setting for NR
	cmd = current_nr;
	sendCommand(cmd);
}

void RIG_TS2000::shutdown()
{
// restore state of xcvr beeps
	if (menu012.empty()) return;
	cmd = menu012;
	sendCommand(cmd);
	sett("restore beeps");
}

static bool is_tuning = false;
static int  skip_get = 2;

RIG_TS2000::RIG_TS2000() {
// base class values
	name_ = TS2000name_;
	modes_ = TS2000modes_;
	bandwidths_ = TS2000_empty;

	dsp_SL     = TS2000_SL;
	SL_tooltip = TS2000_SL_tooltip;
	SL_label   = TS2000_SSB_btn_SL_label;

	dsp_SH     = TS2000_SH;
	SH_tooltip = TS2000_SH_tooltip;
	SH_label   = TS2000_SSB_btn_SH_label;

	widgets = rig_widgets;

	serial_baudrate = BR4800;
	stopbits = 2;
	serial_retries = 2;

//	serial_write_delay = 0;
//	serial_post_write_delay = 0;

	serial_timeout = 50;
	serial_rtscts = true;
	serial_rtsplus = false;
	serial_dtrplus = false;
	serial_catptt = true;
	serial_rtsptt = false;
	serial_dtrptt = false;
	B.imode = A.imode = 1;
	B.iBW = A.iBW = 0x8803;
	B.freq = A.freq = 14070000ULL;
	can_change_alt_vfo = true;

	has_power_out =
	has_swr_control =
	has_alc_control =
	has_split =
	has_split_AB =
	has_dsp_controls =
	has_rf_control =
	has_notch_control =
	has_auto_notch =
	has_ifshift_control =
	has_smeter =
	has_noise_reduction =
	has_noise_reduction_control =
	has_noise_control =
	has_micgain_control =
	has_volume_control =
	has_power_control =
	has_tune_control =
	has_attenuator_control =
	has_preamp_control =
	has_mode_control =
	has_bandwidth_control =
	has_sql_control =
	has_ptt_control = 
	has_extras = true;

	rxona = true;

	precision = 1;
	ndigits = 9;

	att_level = 0;
	preamp_level = 0;
	_noise_reduction_level = 0;
	_nrval1 = 2;
	_nrval2 = 4;

	is_tuning = false;
}

static int ret = 0;

const char * RIG_TS2000::get_bwname_(int n, int md)
{
	static char bwname[20];
	if (n > 256) {
		int hi = (n >> 8) & 0x7F;
		int lo = n & 0xFF;
		snprintf(bwname, sizeof(bwname), "%s/%s",
			(md == LSB || md == USB || md == FM) ? TS2000_SL[lo] : TS2000_AM_SL[lo],
			(md == LSB || md == USB || md == FM) ? TS2000_SH[hi] : TS2000_AM_SH[hi] );
	} else {
		snprintf(bwname, sizeof(bwname), "%s",
			(md == CW || md == CWR) ? TS2000_CWwidths[n] : TS2000_FSKwidths[n]);
	}
	return bwname;
}

int RIG_TS2000::get_smeter()
{
	int smtr = 0;
	if (rxona)
		cmd = "SM0;";
	else
		cmd = "SM1;";
	get_trace(1, "get_smeter");
	ret = wait_char(';', 8, 100, "get smeter", ASC);
	gett("");
	if (ret == 8) {
		size_t p = replystr.rfind("SM");
		if (p != std::string::npos) {
			smtr = fm_decimal(replystr.substr(p+3),4);
			if (rxona)
				smtr = (smtr * 100) / 30;
			else
				smtr = (smtr * 100) / 15;
		}
	}
	return smtr;
}

// Transceiver power level
void RIG_TS2000::set_power_control(double val)
{
	int ival = (int)val;
	cmd = "PC";
	cmd.append(to_decimal(ival, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set pwr ctrl", cmd, "");
	sett("pwr control");
}

int RIG_TS2000::get_power_out()
{
	int poutmtr = 0;
	cmd = "SM0;";
	get_trace(1, "get_power_out");
	ret = wait_char(';', 8, 100, "get power out", ASC);
	gett("");
	if (ret == 8) {
		size_t p = replystr.rfind("SM0");
		if (p != std::string::npos) {
			poutmtr = fm_decimal(replystr.substr(p+3),4);
			if (poutmtr <= 6) poutmtr = poutmtr * 2;
			else if (poutmtr <= 11) poutmtr = 11 + (poutmtr - 6)*(26 - 11)/(11 - 6);
			else if (poutmtr <= 18) poutmtr = 26 + (poutmtr - 11)*(50 - 26)/(18 - 11);
			else poutmtr = 50 + (poutmtr - 18)*(100 - 50)/(27 - 18);
			if (poutmtr > 100) poutmtr = 100;
		}
	}
	return poutmtr;
}

double RIG_TS2000::get_power_control()
{
	int pctrl = 0;
	cmd = "PC;";
	get_trace(1, "get_power_contro");
	ret = wait_char(';', 6, 100, "get pout", ASC);
	gett("");
	if (ret == 6) {
		size_t p = replystr.rfind("PC");
		if (p != std::string::npos) {
			pctrl = fm_decimal(replystr.substr(p+2), 3);
		}
	}
	return pctrl;
}

void RIG_TS2000::set_attenuator(int val)
{
	att_level = val;
	if (val) cmd = "RA01;";
	else     cmd = "RA00;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set ATT", cmd, "");
	sett("attenuator");
}

int RIG_TS2000::get_attenuator()
{
	cmd = "RA;";
	get_trace(1, "get_attenuator");
	ret = wait_char(';', 7, 100, "get ATT", ASC);
	gett("");
	if (ret == 7) {
		size_t p = replystr.rfind("RA");
		if (p != std::string::npos && (p+3 < replystr.length())) {
			if (replystr[p+2] == '0' && replystr[p+3] == '0')
				att_level = 0;
			else
				att_level = 1;
		}
	}
	return att_level;
}

void RIG_TS2000::set_preamp(int val)
{
	preamp_level = val;
	if (val) cmd = "PA1;";
	else     cmd = "PA0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set PRE", cmd, "");
	sett("preamp");
}

int RIG_TS2000::get_preamp()
{
	cmd = "PA;";
	get_trace(1, "get_preamp");
	ret = wait_char(';', 5, 100, "get PRE", ASC);
	gett("");
	if (ret == 5) {
		size_t p = replystr.rfind("PA");
		if (p != std::string::npos && (p+2 < replystr.length())) {
			if (replystr[p+2] == '1')
				preamp_level = 1;
			else
				preamp_level = 0;
		}
	}
	return preamp_level;
}

int RIG_TS2000::set_widths(int val)
{
	int bw;
	switch (val) {
	case LSB: case USB: case FM:
		bandwidths_ = TS2000_SH;
		dsp_SL = TS2000_SL;
		SL_tooltip = TS2000_SL_tooltip;
		SL_label   = TS2000_SSB_btn_SL_label;
		dsp_SH = TS2000_SH;
		SH_tooltip = TS2000_SH_tooltip;
		SH_label   = TS2000_SSB_btn_SH_label;
		if (val == FM) bw = 0x8A03; // 200 ... 4000 Hz
		else bw = 0x8803; // 200 ... 3000 Hz
		break;
	case CW: case CWR:
		bandwidths_ = TS2000_CWwidths;
		dsp_SL = TS2000_empty;
		dsp_SH = TS2000_empty;
		bw = 7;
		break;
	case FSK: case FSKR:
		bandwidths_ = TS2000_FSKwidths;
		dsp_SL = TS2000_empty;
		dsp_SH = TS2000_empty;
		bw = 1;
		break;
	case AM: default:
		bandwidths_ = TS2000_AM_SH;
		dsp_SL = TS2000_AM_SL;
		dsp_SH = TS2000_AM_SH;
		bw = 0x8201;
		break;
	}
	return bw;
}

const char **RIG_TS2000::bwtable(int val)
{
	if (val == LSB || val == USB || val == FM)
		return TS2000_SH;
	else if (val == CW || val == CWR)
		return TS2000_CWwidths;
	else if (val == FSK || val == FSKR)
		return TS2000_FSKwidths;
//else AM m == 4
	return TS2000_AM_SH;
}

const char **RIG_TS2000::lotable(int val)
{
	if (val == LSB || val == USB || val == FM)
		return TS2000_SL;
	if (val == AM)
		return TS2000_AM_SL;
	return NULL;
}

const char **RIG_TS2000::hitable(int val)
{
	if (val == LSB || val == USB || val == FM)
		return TS2000_SH;
	if (val == AM)
		return TS2000_AM_SH;
	return NULL;
}

void RIG_TS2000::set_modeA(int val)
{
	if (val >= (int)(sizeof(TS2000_mode_chr)/sizeof(*TS2000_mode_chr))) return;
	_currmode = A.imode = val;
	cmd = "MD";
	cmd += TS2000_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "set mode", cmd, "");
	sett("modeA");
	A.iBW = set_widths(val);
}

int RIG_TS2000::get_modeA()
{
	if (tuning()) return A.imode;
	if (skip_get) {
		skip_get--;
		return A.imode;
	}
	cmd = "MD;";
	get_trace(1, "get_modeA");
	ret = wait_char(';', 4, 100, "get mode A", ASC);
	gett("");
	if (ret == 4) {
		size_t p = replystr.rfind("MD");
		if (p != std::string::npos) {
			int md = replystr[p+2];
			md = md - '1';
			if (md == 8) md = 7;
			A.imode = md;
			A.iBW = set_widths(A.imode);
		}
	}
	_currmode = A.imode;
	return A.imode;
}

void RIG_TS2000::set_modeB(int val)
{
	if (val >= (int)(sizeof(TS2000_mode_chr)/sizeof(*TS2000_mode_chr))) return;
	_currmode = B.imode = val;
	cmd = "MD";
	cmd += TS2000_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "set mode B", cmd, "");
	sett("modeB");
	B.iBW = set_widths(val);
}

int RIG_TS2000::get_modeB()
{
	if (tuning()) return B.imode;
	if (skip_get) return B.imode;
	cmd = "MD;";
	get_trace(1, "get_modeB");
	ret = wait_char(';', 4, 100, "get mode B", ASC);
	gett("");
	if (ret == 4) {
		size_t p = replystr.rfind("MD");
		if (p != std::string::npos) {
			int md = replystr[p+2];
			md = md - '1';
			if (md == 8) md = 7;
			B.imode = md;
			B.iBW = set_widths(B.imode);
		}
	}
	_currmode = B.imode;
	return B.imode;
}

int RIG_TS2000::adjust_bandwidth(int val)
{
	int bw = 0;
	if (val == LSB || val == USB)
		bw = 0x8803;
	else if (val == FM)
		bw = 0x8A03;
	else if (val == AM)
		bw = 0x8301;
	else if (val == CW || val == CWR)
		bw = 7;
	else if (val == FSK || val == FSKR)
		bw = 1;
	return bw;
}

int RIG_TS2000::def_bandwidth(int val)
{
	return adjust_bandwidth(val);
}

void RIG_TS2000::set_bwA(int val)
{
	if (A.imode == LSB || A.imode == USB || A.imode == FM || A.imode == AM) {
		if (val < 256) return;
		A.iBW = val;
		cmd = "SL";
		int index = A.iBW & 0x7F;
		if (index >= (int)(sizeof(TS2000_CAT_SL)/sizeof(*TS2000_CAT_SL))) return;
		cmd = TS2000_CAT_SL[index];
		sendCommand(cmd);
		showresp(WARN, ASC, "set lower", cmd, "");
		sett("bwA lower");
		cmd = "SH";
		index = (A.iBW >> 8) & 0x7F;
		if (index >= (int)(sizeof(TS2000_CAT_SH)/sizeof(*TS2000_CAT_SH))) return;
		cmd = TS2000_CAT_SH[index];
		sendCommand(cmd);
		showresp(WARN, ASC, "set upper", cmd, "");
		sett("bwA upper");
	}
	if (val > 256) return;
	else if (A.imode == CW || A.imode == CWR) {
		A.iBW = val;
		int index = A.iBW & 0x7F;
		if (index >= (int)(sizeof(TS2000_CWbw)/sizeof(*TS2000_CWbw))) return;
		cmd = TS2000_CWbw[index];
		sendCommand(cmd);
		showresp(WARN, ASC, "set CW bw", cmd, "");
		sett("CW bw");
	}else if (A.imode == FSK || A.imode == FSKR) {
		A.iBW = val;
		int index = A.iBW & 0x7F;
		if (index >= (int)(sizeof(TS2000_FSKbw)/sizeof(*TS2000_FSKbw))) return;
		cmd = TS2000_FSKbw[index];
		sendCommand(cmd);
		showresp(WARN, ASC, "set FSK bw", cmd, "");
		sett("FSK bw");
	}
}

int RIG_TS2000::get_bwA()
{
	if (tuning()) return A.iBW;
	if (skip_get) return A.iBW;
	size_t i = 0;
	size_t p;
	if (A.imode == LSB || A.imode == USB || A.imode == FM || A.imode == AM) {
		int lo = A.iBW & 0xFF, hi = (A.iBW >> 8) & 0x7F;
		cmd = "SL;";
		get_trace(1, "get SL");
		ret = wait_char(';', 5, 100, "get SL", ASC);
		gett("");
		if (ret == 5) {
			p = replystr.rfind("SL");
			if (p != std::string::npos)
				lo = fm_decimal(replystr.substr(2), 2);
		}
		cmd = "SH;";
		get_trace(1, "get SH");
		ret = wait_char(';', 5, 100, "get SH", ASC);
		gett("");
		if (ret == 5) {
			p = replystr.rfind("SH");
			if (p != std::string::npos)
				hi = fm_decimal(replystr.substr(2), 2);
			A.iBW = ((hi << 8) | (lo & 0xFF)) | 0x8000;
		}
	} else if (A.imode == CW || A.imode == CWR) { // CW
		cmd = "FW;";
		get_trace(1, "get FW");
		ret = wait_char(';', 7, 100, "get FW", ASC);
		gett("");
		if (ret == 7) {
			p = replystr.rfind("FW");
			if (p != std::string::npos) {
				for (i = 0; i < sizeof(TS2000_CWbw)/sizeof(*TS2000_CWbw); i++)
					if (replystr.find(TS2000_CWbw[i]) == p)
						break;
				A.iBW = i;
			}
		}
	} else if (A.imode == FSK || A.imode == FSKR) {
		cmd = "FW;";
		get_trace(1, "get FW");
		ret = wait_char(';', 7, 100, "get FW", ASC);
		gett("");
		if (ret == 7) {
			p = replystr.rfind("FW");
			if (p != std::string::npos) {
				for (i = 0; i < sizeof(TS2000_FSKbw)/sizeof(*TS2000_FSKbw); i++)
					if (replystr.find(TS2000_FSKbw[i]) == p)
						break;
				A.iBW = i;
			}
		}
	}
	return A.iBW;
}

void RIG_TS2000::set_bwB(int val)
{
	if (B.imode == LSB || B.imode == USB || B.imode == FM || B.imode == AM) {
		if (val < 256) return;
		B.iBW = val;
		cmd = "SL";
		int index = B.iBW & 0x7F;
		if (index >= (int)(sizeof(TS2000_CAT_SL)/sizeof(*TS2000_CAT_SL))) return;
		cmd = TS2000_CAT_SL[index];
		sendCommand(cmd);
		showresp(WARN, ASC, "set lower", cmd, "");
		sett("bwB lower");
		cmd = "SH";
		index = (B.iBW >> 8) & 0x7F;
		if (index >= (int)(sizeof(TS2000_CAT_SH)/sizeof(*TS2000_CAT_SH))) return;
		cmd = TS2000_CAT_SH[index];
		sendCommand(cmd);
		showresp(WARN, ASC, "set upper", cmd, "");
		sett("bwB upper");
	}
	if (val > 256) return;
	else if (B.imode == CW || B.imode == CWR) {
		B.iBW = val;
		int index = B.iBW & 0x7F;
		if (index >= (int)(sizeof(TS2000_CWbw)/sizeof(*TS2000_CWbw))) return;
		cmd = TS2000_CWbw[index];
		sendCommand(cmd);
		showresp(WARN, ASC, "set CW bw", cmd, "");
		sett("bwB CW");
	}else if (B.imode == FSK || B.imode == FSKR) {
		B.iBW = val;
		int index = B.iBW & 0x7F;
		if (index >= (int)(sizeof(TS2000_FSKbw)/sizeof(*TS2000_FSKbw))) return;
		cmd = TS2000_FSKbw[index];
		sendCommand(cmd);
		showresp(WARN, ASC, "set FSK bw", cmd, "");
		sett("bwB FSK");
	}
}

int RIG_TS2000::get_bwB()
{
	if (tuning()) return B.iBW;
	if (skip_get) return B.iBW;
	size_t i = 0;
	size_t p;
	if (B.imode == LSB || B.imode == USB || B.imode == FM || B.imode == AM) {
		int lo = B.iBW & 0xFF, hi = (B.iBW >> 8) & 0x7F;
		cmd = "SL;";
		get_trace(1, "get SL");
		ret = wait_char(';', 5, 100, "get SL", ASC);
		gett("");
		if (ret == 5) {
			p = replystr.rfind("SL");
			if (p != std::string::npos)
				lo = fm_decimal(replystr.substr(2), 2);
		}
		cmd = "SH;";
		get_trace(1, "get SH");
		ret = wait_char(';', 5, 100, "get SH", ASC);
		gett("");
		if (ret == 5) {
			p = replystr.rfind("SH");
			if (p != std::string::npos)
				hi = fm_decimal(replystr.substr(2), 2);
			B.iBW = ((hi << 8) | (lo & 0xFF)) | 0x8000;
		}
	} else if (B.imode == CW || B.imode == CWR) {
		cmd = "FW;";
		get_trace(1, "get FW");
		ret = wait_char(';', 7, 100, "get FW", ASC);
		gett("");
		if (ret == 7) {
			p = replystr.rfind("FW");
			if (p != std::string::npos) {
				for (i = 0; i < sizeof(TS2000_CWbw)/sizeof(*TS2000_CWbw); i++)
					if (replystr.find(TS2000_CWbw[i]) == p)
						break;
				B.iBW = i;
			}
		}
	} else if (B.imode == FSK || B.imode == FSKR) {
		cmd = "FW;";
		get_trace(1, "get FW");
		ret = wait_char(';', 7, 100, "get FW", ASC);
		gett("");
		if (ret == 7) {
			p = replystr.rfind("FW");
			if (p != std::string::npos) {
				for (i = 0; i < sizeof(TS2000_FSKbw)/sizeof(*TS2000_FSKbw); i++)
					if (replystr.find(TS2000_FSKbw[i]) == p)
						break;
				B.iBW = i;
			}
		}
	}
	return B.iBW;
}

int RIG_TS2000::get_modetype(int n)
{
	if (n >= (int)(sizeof(TS2000_mode_type)/sizeof(*TS2000_mode_type))) return 0;
	return TS2000_mode_type[n];
}

void RIG_TS2000::get_if_min_max_step(int &min, int &max, int &step)
{
	if_shift_min = min = 400;
	if_shift_max = max = 1000;
	if_shift_step = step = 50;
	if_shift_mid = 700;
}

void RIG_TS2000::set_notch(bool on, int val)
{
	if (on) {
		cmd = "BC2;"; // set manual notch
		sendCommand(cmd);
		showresp(WARN, ASC, "set notch on", cmd, "");
		sett("notch ON");
		cmd = "BP";
//		val = round((val - 220) / 50);
		val = round((val - 200) / 50);
		cmd.append(to_decimal(val, 3)).append(";");
		sendCommand(cmd);
		showresp(WARN, ASC, "set notch val", cmd, "");
		sett("notch val");
	} else {
		cmd = "BC0;"; // no notch action
		sendCommand(cmd);
		showresp(WARN, ASC, "set notch off", cmd, "");
		sett("notch OFF");
	}
}

bool  RIG_TS2000::get_notch(int &val)
{
	bool ison = false;
	cmd = "BC;";

	get_trace(1, "get_notch_on_off");
	ret = wait_char(';', 4, 100, "get notch on/off", ASC);
	gett("");

	if (ret == 4) {
		size_t p = replystr.rfind("BC");
		if (p != std::string::npos) {
			if (replystr[p+2] == '2') {
				ison = true;
				cmd = "BP;";

				get_trace(1, "get_notch_val");
				ret = wait_char(';', 6, 100, "get notch val", ASC);
				gett("");

				if (ret == 6) {
					gett("notch val");
					p = replystr.rfind("BP");
					if (p != std::string::npos)
						val = 200 + 50 * fm_decimal(replystr.substr(p+2),3);
				}
			}
		}
	}
	return (ison);
}

void RIG_TS2000::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = 200;
	max = 3350;
	step = 50;
}

void RIG_TS2000::set_auto_notch(int v)
{
	cmd = v ? "NT1;" : "NT0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set auto notch", cmd, "");
	sett("auto notch");
}

int  RIG_TS2000::get_auto_notch()
{
	int anotch = 0;
	cmd = "NT;";

	get_trace(1, "get_auto_notch");
	ret = wait_char(';', 4, 100, "get auto notch", ASC);
	gett("");

	if (ret == 4) {
		size_t p = replystr.rfind("NT");
		if (p != std::string::npos) {
			anotch = (replystr[p+2] == '1');
		}
	}
	return anotch;
}

void RIG_TS2000::set_noise_reduction(int val)
{
	if (val == -1) {
		return;
	}
	_noise_reduction_level = val;
	if (_noise_reduction_level == 0) {
		nr_label("NR", false);
	} else if (_noise_reduction_level == 1) {
		nr_label("NR1", true);
	} else if (_noise_reduction_level == 2) {
		nr_label("NR2", true);
	}
	cmd.assign("NR");
	cmd += '0' + _noise_reduction_level;
	cmd += ';';
	sendCommand (cmd);
	showresp(WARN, ASC, "SET noise reduction", cmd, "");
	sett("noise reduction");
}

int  RIG_TS2000::get_noise_reduction()
{
	cmd = rsp = "NR";
	cmd.append(";");

	get_trace(1, "get_noise_reduction");
	ret = wait_char(';', 4, 100, "GET noise reduction", ASC);
	gett("");

	if (ret == 4) {
		size_t p = replystr.rfind(rsp);
		if (p == std::string::npos) return _noise_reduction_level;
		_noise_reduction_level = replystr[p+2] - '0';
	}
	if (replystr == "?;") {
		_noise_reduction_level = 0;
		return 0;
	}

	if (_noise_reduction_level == 1) {
		nr_label("NR1", true);
	} else if (_noise_reduction_level == 2) {
		nr_label("NR2", true);
	} else {
		nr_label("NR", false);
	}
	return _noise_reduction_level;
}

void RIG_TS2000::set_noise_reduction_val(int val)
{
	if (_noise_reduction_level == 0) return;
	if (_noise_reduction_level == 1) _nrval1 = val;
	else _nrval2 = val;

	cmd.assign("RL").append(to_decimal(val, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET_noise_reduction_val", cmd, "");
	sett("noise reduction val");
}

int  RIG_TS2000::get_noise_reduction_val()
{
	int nrval = 0;
	if (_noise_reduction_level == 0) return 0;
	int val = progStatus.noise_reduction_val;
	cmd = rsp = "RL";
	cmd.append(";");

	get_trace(1, "get_noise_reduction_val");
	ret = wait_char(';', 5, 100, "GET noise reduction val", ASC);
	gett("");

	if (ret == 5) {
		size_t p = replystr.rfind(rsp);
		if (p == std::string::npos) {
			nrval = (_noise_reduction_level == 1 ? _nrval1 : _nrval2);
			return nrval;
		}
		val = atoi(&replystr[p+2]);
	}

	if (_noise_reduction_level == 1) _nrval1 = val;
	else _nrval2 = val;

	return val;
}


//====================== vfo controls

void RIG_TS2000::selectA()
{
	cmd = "FR0;FT0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Rx on A, Tx on A", cmd, "");
	inuse = onA;
}

void RIG_TS2000::selectB()
{
	cmd = "FR1;FT1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Rx on B, Tx on B", cmd, "");
	inuse = onB;
}

unsigned long long RIG_TS2000::get_vfoA ()
{
	cmd = "FA;";
	if (wait_char(';', 14, 100, "get vfo A", ASC) < 14) return A.freq;

	size_t p = replystr.rfind("FA");
	if (p != std::string::npos && (p + 12 < replystr.length())) {
		unsigned long long f = 0;
		for (size_t n = 2; n < 13; n++)
			f = f*10 + replystr[p+n] - '0';
		A.freq = f;
	}
	return A.freq;
}

void RIG_TS2000::set_vfoA (unsigned long long freq)
{
	A.freq = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set vfo A", cmd, "");
}

unsigned long long RIG_TS2000::get_vfoB ()
{
	cmd = "FB;";
	if (wait_char(';', 14, 100, "get vfo B", ASC) < 14) return B.freq;

	size_t p = replystr.rfind("FB");
	if (p != std::string::npos && (p + 12 < replystr.length())) {
		unsigned long long f = 0;
		for (size_t n = 2; n < 13; n++)
			f = f*10 + replystr[p+n] - '0';
		B.freq = f;
	}
	return B.freq;
}

void RIG_TS2000::set_vfoB (unsigned long long freq)
{
	B.freq = freq;
	cmd = "FB00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set vfo B", cmd, "");
}

void RIG_TS2000::set_split(bool val) 
{
	split = val;
	if (inuse == onB) {
		if (val) {
			cmd = "FR1;FT0;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on B, Tx on A", cmd, "");
		} else {
			cmd = "FR1;FT1;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on B, Tx on B", cmd, "");
		}
	} else {
		if (val) {
			cmd = "FR0;FT1;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on A, Tx on B", cmd, "");
		} else {
			cmd = "FR0;FT0;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on A, Tx on A", cmd, "");
		}
	}
}

int RIG_TS2000::get_split()
{
	size_t p;
	int split = 0;
	char rx = 0, tx = 0;
// tx vfo
	cmd = rsp = "FT";
	cmd.append(";");
	if (wait_char(';', 4, 100, "get split tx vfo", ASC) == 4) {
		p = replystr.rfind(rsp);
		if (p == std::string::npos) return split;
		tx = replystr[p+2];
	}
// rx vfo
	cmd = rsp = "FR";
	cmd.append(";");
	if (wait_char(';', 4, 100, "get split rx vfo", ASC) == 4) {
		p = replystr.rfind(rsp);
		if (p == std::string::npos) return split;
		rx = replystr[p+2];
	}

	if (tx == '0' && rx == '0') split = 0;
	else if (tx == '1' && rx == '0') split = 1;
	else if (tx == '0' && rx == '1') split = 2;
	else if (tx == '1' && rx == '1') split = 3;

	return split;
}

