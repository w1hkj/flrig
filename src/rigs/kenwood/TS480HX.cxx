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

#include "kenwood/TS480HX.h"
#include "support.h"

static const char TS480HXname_[] = "TS-480HX";

static const char *TS480HXmodes_[] = {
		"LSB", "USB", "CW", "FM", "AM", "FSK", "CW-R", "FSK-R", NULL};
static const char TS480HX_mode_chr[] =  { '1', '2', '3', '4', '5', '6', '7', '9' };
static const char TS480HX_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'L', 'L', 'U' };

static const char *TS480HX_empty[] = { "N/A", NULL };
static int TS480HX_bw_vals[] = {1, WVALS_LIMIT};

// SL command is lo cut when menu 045 OFF
static const char *TS480HX_SL[] = {
  "0",   "50", "100", "200", "300", 
"400",  "500", "600", "700", "800", 
"900", "1000", NULL };
static const char *TS480HX_SL_tooltip = "lo cut";
static const char *TS480HX_btn_SL_label = "L";

// SH command is hi cut when menu 045 OFF
static const char *TS480HX_SH[] = {
"1000", "1200", "1400", "1600", "1800", 
"2000", "2200", "2400", "2600", "2800", 
"3000", "3400", "4000", "5000", NULL };
static int TS480HX_HI_bw_vals[] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,WVALS_LIMIT};

static const char *TS480HX_SH_tooltip = "hi cut";
static const char *TS480HX_btn_SH_label = "H";

// SL command is width when menu 045 ON
static const char *TS480HX_dataW[] = {
"50", "100", "250", "500", "1000", "1500", "2400", NULL };
static int TS480HX_data_bw_vals[] = {1,2,3,4,5,6,7, WVALS_LIMIT};

static const char *TS480HX_dataW_tooltip = "width";
static const char *TS480HX_dataW_label = "W";

// SH command is center when menu 045 ON
static const char *TS480HX_dataC[] = {
"1000", "1500", "2210", NULL };
static const char *TS480HX_dataC_tooltip = "center";
static const char *TS480HX_dataC_label = "C";

static const char *TS480HX_AM_SL[] = {
"10", "100", "200", "500",
NULL };

static const char *TS480HX_AM_SH[] = {
"2500", "3000", "4000", "5000",
NULL };
//static int TS480HX_AM_bw_vals[] = {1,2,3,4,WVALS_LIMIT};

static const char *TS480HX_CWwidths[] = {
"50", "80", "100", "150", "200", 
"300", "400", "500", "600", "1000", 
"2000", NULL};
static int TS480HX_CW_bw_vals[] = {1,2,3,4,5,6,7,8,9,10,11,WVALS_LIMIT};

static const char *TS480HX_CWbw[] = {
"FW0050;", "FW0080;", "FW0100;", "FW0150;", "FW0200;", 
"FW0300;", "FW0400;", "FW0500;", "FW0600;", "FW1000;", 
"FW2000;" };

static const char *TS480HX_FSKwidths[] = {
"250", "500", "1000", "1500", NULL};
static int TS480HX_FSK_bw_vals[] = { 1,2,3,4,WVALS_LIMIT};

static const char *TS480HX_FSKbw[] = {
"FW0250;", "FW0500;", "FW1000;", "FW1500;" };

static int agcval = 1;
static bool fm_mode = false;

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 }, // 0
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 }, // 1
	{ (Fl_Widget *)sldrRFGAIN,   54, 145, 156 }, // 2
	{ (Fl_Widget *)btnIFsh,     214, 105,  50 }, // 3
	{ (Fl_Widget *)sldrIFSHIFT, 266, 105, 156 }, // 4
	{ (Fl_Widget *)btnDataPort, 214, 125,  50 }, // 5
	{ (Fl_Widget *)sldrSQUELCH, 266, 125, 156 }, // 6
	{ (Fl_Widget *)sldrMICGAIN, 266, 145, 156 }, // 7
	{ (Fl_Widget *)sldrPOWER,    54, 165, 368 }, // 8
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

static std::string menu012 = "EX01200004";

void RIG_TS480HX::initialize()
{
	rig_widgets[0].W = btnVol;
	rig_widgets[1].W = sldrVOLUME;
	rig_widgets[2].W = sldrRFGAIN;
	rig_widgets[3].W = btnIFsh;
	rig_widgets[4].W = sldrIFSHIFT;
	rig_widgets[5].W = btnDataPort;
	rig_widgets[6].W = sldrSQUELCH;
	rig_widgets[7].W = sldrMICGAIN;
	rig_widgets[8].W = sldrPOWER;

	check_menu_45();

	menu012.clear();
	cmd = "EX0120000;"; // read menu 012 state
//might return something like EX01200004;

	if (wait_char(';', 11, 100, "read ex 012", ASC) == 11)
		menu012 = replystr;

	cmd = "EX01200000;";
	sendCommand(cmd);
}

RIG_TS480HX::RIG_TS480HX() {
// base class values
	name_ = TS480HXname_;
	modes_ = TS480HXmodes_;
	_mode_type = TS480HX_mode_type;
	bandwidths_ = TS480HX_empty;
	bw_vals_ = TS480HX_bw_vals;

	dsp_SL     = TS480HX_SL;
	SL_tooltip = TS480HX_SL_tooltip;
	SL_label   = TS480HX_btn_SL_label;

	dsp_SH     = TS480HX_SH;
	SH_tooltip = TS480HX_SH_tooltip;
	SH_label   = TS480HX_btn_SH_label;

	widgets = rig_widgets;

	serial_baudrate = BR57600;
	stopbits = 1;
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
	B.iBW = A.iBW = 0x8A03;
	B.freq = A.freq = 14070000ULL;

	can_change_alt_vfo = true;

	has_extras = true;

	has_noise_reduction =
	has_noise_reduction_control =
	has_auto_notch =
	has_noise_control =
	has_sql_control =

	has_split = true;
	has_split_AB = true;
	has_data_port = true;
	has_micgain_control = true;
	has_ifshift_control = true;
	has_rf_control = true;
	has_agc_control = true;
	has_swr_control = true;
	has_alc_control = true;
	has_power_out = true;
	has_dsp_controls = true;
	has_smeter = true;
	has_attenuator_control = true;
	has_preamp_control = true;
	has_mode_control = true;
	has_bandwidth_control = true;
	has_volume_control = true;
	has_power_control = true;
	has_tune_control = true;
	has_ptt_control = true;

	precision = 1;
	ndigits = 8;

	_noise_reduction_level = 0;
	_nrval1 = 2;
	_nrval2 = 4;
	preamp_level = atten_level = 0;
}

static int ret = 0;

const char * RIG_TS480HX::get_bwname_(int n, int md) 
{
	static char bwname[20];
	if (n > 256) {
		int hi = (n >> 8) & 0x7F;
		int lo = n & 0xFF;
		snprintf(bwname, sizeof(bwname), "%s/%s",
			(md == 0 || md == 1 || md == 3) ? dsp_SL[lo] : TS480HX_AM_SL[lo],
			(md == 0 || md == 1 || md == 3) ? dsp_SH[hi] : TS480HX_AM_SH[hi] );
	} else {
		snprintf(bwname, sizeof(bwname), "%s",
			(md == 2 || md == 6) ? TS480HX_CWwidths[n] : TS480HX_FSKwidths[n]);
	}
	return bwname;
}

void RIG_TS480HX::check_menu_45()
{
// read current switch 45 setting
	menu_45 = false;
	cmd = "EX0450000;";
	if (wait_char(';', 11, 100, "Check menu item 45", ASC) >= 11) {
		size_t p = replystr.rfind("EX045");
		if (p != std::string::npos)
			menu_45 = (replystr[p+9] == '1');
	}

	if (menu_45) {
		dsp_SL     = TS480HX_dataW;
		SL_tooltip = TS480HX_dataW_tooltip;
		SL_label   = TS480HX_dataW_label;
		dsp_SH     = TS480HX_dataC;
		SH_tooltip = TS480HX_dataC_tooltip;
		SH_label   = TS480HX_dataC_label;
		B.iBW = A.iBW = 0x8106;
	} else {
		dsp_SL     = TS480HX_SL;
		SL_tooltip = TS480HX_SL_tooltip;
		SL_label   = TS480HX_btn_SL_label;
		dsp_SH     = TS480HX_SH;
		SH_tooltip = TS480HX_SH_tooltip;
		SH_label   = TS480HX_btn_SH_label;
		B.iBW = A.iBW = 0x8A03;
	}
}

void RIG_TS480HX::shutdown()
{
// restore state of xcvr beeps
	if (menu012.empty()) return;
	cmd = menu012;
	sendCommand(cmd);
}

// SM cmd 0 ... 100 (rig values 0 ... 15)
int RIG_TS480HX::get_smeter()
{
	int mtr = 0;
	cmd = "SM0;";
	get_trace(1, "get_smeter");
	ret = wait_char(';', 8, 100, "get Smeter", ASC);
	gett("");
	if (ret < 8) return 0;

	size_t p = replystr.rfind("SM");
	if (p != std::string::npos)
		mtr = 5 * atoi(&replystr[p + 3]);
	return mtr;
}

struct pwrpair {int mtr; float pwr;};

static pwrpair pwrtbl[] = { 
	{0, 0.0}, 
	{2, 5.0},
	{4, 10.0}, 
	{7, 25.0}, 
	{11, 50.0}, 
	{16, 100.0}, 
	{20, 200.0} };

int RIG_TS480HX::get_power_out()
{
	int mtr = 0;
	cmd = "SM0;";
	get_trace(1, "get_power_out");
	ret = wait_char(';', 8, 100, "get power", ASC);
	gett("");
	if (ret < 8) return mtr;

	size_t p = replystr.rfind("SM");
	if (p != std::string::npos) {
		mtr = atoi(&replystr[p + 3]);

		size_t i = 0;
		for (i = 0; i < sizeof(pwrtbl) / sizeof(pwrpair) - 1; i++)
			if (mtr >= pwrtbl[i].mtr && mtr < pwrtbl[i+1].mtr)
				break;
		if (mtr < 0) mtr = 0;
		if (mtr > 20) mtr = 20;
		mtr = (int)ceil(pwrtbl[i].pwr + 
			(pwrtbl[i+1].pwr - pwrtbl[i].pwr)*(mtr - pwrtbl[i].mtr)/(pwrtbl[i+1].mtr - pwrtbl[i].mtr));
		if (mtr > 200) mtr = 200;
	}
	return mtr;
}

// RM cmd 0 ... 100 (rig values 0 ... 8)
// User report of RM; command using Send Cmd tab
// RM10000;RM20000;RM30000;
// RM1nnnn; => SWR
// RM2nnnn; => COMP
// RM3nnnn; => ALC

int RIG_TS480HX::get_swr()
{
	int mtr = 0;
	cmd = "RM;";
	get_trace(1, "get_swr");
	ret = wait_char(';', 8, 100, "get SWR/ALC", ASC);
	gett("");
	if (ret < 8) return (int)mtr;

	size_t p = replystr.rfind("RM1");
	if (p != std::string::npos)
		mtr = 66 * atoi(&replystr[p + 3]) / 10;
	p = replystr.rfind("RM3");
	if (p != std::string::npos)
		alc = 66 * atoi(&replystr[p+3]) / 10;
	else
		alc = 0;
	swralc_polled = true;
	return mtr;
}

int  RIG_TS480HX::get_alc(void) 
{
	if (!swralc_polled) get_swr();
	swralc_polled = false;
	return alc;
}

int RIG_TS480HX::set_widths(int val)
{
	int bw;
	if (val == 0 || val == 1 || val == 3) {
		if (menu_45) {
			bw = 0x8106; // 1500 Hz 2400 wide
			dsp_SL     = TS480HX_dataW;
			SL_tooltip = TS480HX_dataW_tooltip;
			SL_label   = TS480HX_dataW_label;
			dsp_SH     = TS480HX_dataC;
			SH_tooltip = TS480HX_dataC_tooltip;
			SH_label   = TS480HX_dataC_label;
			bandwidths_ = TS480HX_dataW;
			bw_vals_ = TS480HX_data_bw_vals;
		} else {
			bw = 0x8A03; // 200 ... 3000 Hz
			dsp_SL     = TS480HX_SL;
			SL_tooltip = TS480HX_SL_tooltip;
			SL_label   = TS480HX_btn_SL_label;
			dsp_SH     = TS480HX_SH;
			SH_tooltip = TS480HX_SH_tooltip;
			SH_label   = TS480HX_btn_SH_label;
			bandwidths_ = TS480HX_SH;
			bw_vals_ = TS480HX_HI_bw_vals;
		}
	} else if (val == 2 || val == 6) {
		bandwidths_ = TS480HX_CWwidths;
		bw_vals_ = TS480HX_CW_bw_vals;
		dsp_SL = TS480HX_empty;
		dsp_SH = TS480HX_empty;
		bw = 7;
	} else if (val == 5 || val == 7) {
		bandwidths_ = TS480HX_FSKwidths;
		bw_vals_ = TS480HX_FSK_bw_vals;
		dsp_SL = TS480HX_empty;
		dsp_SH = TS480HX_empty;
		bw = 1;
	} else { // val == 4 ==> AM
		bandwidths_ = TS480HX_empty;
		bw_vals_ = TS480HX_bw_vals;
		dsp_SL = TS480HX_AM_SL;
		dsp_SH = TS480HX_AM_SH;
		bw = 0x8201;
	}
	return bw;
}

const char **RIG_TS480HX::bwtable(int m)
{
	if (m == 0 || m == 1 || m == 3)
		return TS480HX_empty;
	else if (m == 2 || m == 6)
		return TS480HX_CWwidths;
	else if (m == 5 || m == 7)
		return TS480HX_FSKwidths;
//else AM m == 4
	return TS480HX_empty;
}

const char **RIG_TS480HX::lotable(int m)
{
	if (m == 0 || m == 1 || m == 3)
		return TS480HX_SL;
	else if (m == 2 || m == 6)
		return NULL;
	else if (m == 5 || m == 7)
		return NULL;
	return TS480HX_AM_SL;
}

const char **RIG_TS480HX::hitable(int m)
{
	if (m == 0 || m == 1 || m == 3)
		return TS480HX_SH;
	else if (m == 2 || m == 6)
		return NULL;
	else if (m == 5 || m == 7)
		return NULL;
	return TS480HX_AM_SH;
}

void RIG_TS480HX::set_modeA(int val)
{
	if (val == 3) fm_mode = true;
	else fm_mode = false;
	A.imode = val;
	cmd = "MD";
	cmd += TS480HX_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "set mode", cmd, "");
	A.iBW = set_widths(val);
}

int RIG_TS480HX::get_modeA()
{
	cmd = "MD;";
	get_trace(1, "get_modeA");
	ret = wait_char(';', 4, 100, "get modeA", ASC);
	gett("");
	if (ret < 4) return A.imode;

	size_t p = replystr.rfind("MD");
	if (p != std::string::npos && (p + 2 < replystr.length())) {
		int md = replystr[p+2];
		md = md - '1';
		if (md == 8) md = 7;
		A.imode = md;
		A.iBW = set_widths(A.imode);
	}
	if (A.imode == 3) fm_mode = true;
	else fm_mode = false;
	return A.imode;
}

void RIG_TS480HX::set_modeB(int val)
{
	if (val == 3) fm_mode = true;
	else fm_mode = false;
	B.imode = val;
	cmd = "MD";
	cmd += TS480HX_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "set mode B", cmd, "");
	B.iBW = set_widths(val);
}

int RIG_TS480HX::get_modeB()
{
	cmd = "MD;";
	get_trace(1, "get_modeB");
	ret = wait_char(';', 4, 100, "get modeB", ASC);
	gett("");
	if (ret < 4) return B.imode;

	size_t p = replystr.rfind("MD");
	if (p != std::string::npos && (p + 2 < replystr.length())) {
		int md = replystr[p+2];
		md = md - '1';
		if (md == 8) md = 7;
		B.imode = md;
		B.iBW = set_widths(B.imode);
	}
	if (B.imode == 3) fm_mode = true;
	else fm_mode = false;
	return B.imode;
}

int RIG_TS480HX::get_modetype(int n)
{
	return _mode_type[n];
}

void RIG_TS480HX::set_bwA(int val)
{
	if (A.imode == 0 || A.imode == 1 || A.imode == 3 || A.imode == 4) {
		if (val < 256) return;
		A.iBW = val;
		cmd = "SL";
		cmd.append(to_decimal(A.iBW & 0xFF, 2)).append(";");
		sendCommand(cmd);
		showresp(WARN, ASC, SL_tooltip, cmd, "");
		cmd = "SH";
		cmd.append(to_decimal(((A.iBW >> 8) & 0x7F), 2)).append(";");
		sendCommand(cmd);
		showresp(WARN, ASC, SH_tooltip, cmd, "");
	}
	if (val > 256) return;
	else if (A.imode == 2 || A.imode == 6) {
		A.iBW = val;
		cmd = TS480HX_CWbw[A.iBW];
		sendCommand(cmd);
		showresp(WARN, ASC, "set CW bw", cmd, "");
	}else if (A.imode == 5 || A.imode == 7) {
		A.iBW = val;
		cmd = TS480HX_FSKbw[A.iBW];
		sendCommand(cmd);
		showresp(WARN, ASC, "set FSK bw", cmd, "");
	}
}

int RIG_TS480HX::get_bwA()
{
	int i = 0;
	size_t p;

	bool menu45 = menu_45;

	check_menu_45();
	if (menu45 != menu_45)
		Fl::awake(updateBandwidthControl);

	if (A.imode == 0 || A.imode == 1 || A.imode == 3 || A.imode == 4) {
		int lo = A.iBW & 0xFF, hi = (A.iBW >> 8) & 0x7F;
		cmd = "SL;";
		get_trace(1, "get_bwA, SL");
		ret = wait_char(';', 5, 100, "get SL", ASC);
		gett("");
		if (ret == 5) {
			p = replystr.rfind("SL");
			if (p != std::string::npos)
				lo = fm_decimal(replystr.substr(p+2), 2);
		}
		cmd = "SH;";
		if (wait_char(';', 5, 100, "get SH", ASC) == 5) {
			p = replystr.rfind("SH");
			if (p != std::string::npos)
				hi = fm_decimal(replystr.substr(p+2), 2);
			A.iBW = ((hi << 8) | (lo & 0xFF)) | 0x8000;
		}
	} else if (A.imode == 2 || A.imode == 6) {
		cmd = "FW;";
		get_trace(1, "get_bwA, FW");
		ret = wait_char(';', 7, 100, "get FW", ASC);
		gett("");
		if (ret == 7) {
			p = replystr.rfind("FW");
			if (p != std::string::npos) {
				for (i = 0; i < 11; i++)
					if (replystr.find(TS480HX_CWbw[i]) == p)
						break;
				if (i == 11) i = 10;
				A.iBW = i;
			}
		}
	} else if (A.imode == 5 || A.imode == 7) {
		cmd = "FW;";
		get_trace(1, "get_bwA, FW");
		ret = wait_char(';', 7, 100, "get FW", ASC);
		gett("");
		if (ret == 7) {
			p = replystr.rfind("FW");
			if (p != std::string::npos) {
				for (i = 0; i < 4; i++)
					if (replystr.find(TS480HX_FSKbw[i]) == p)
						break;
				if (i == 4) i = 3;
				A.iBW = i;
			}
		}
	}
	return A.iBW;
}

void RIG_TS480HX::set_bwB(int val)
{
	if (B.imode == 0 || B.imode == 1 || B.imode == 3 || B.imode == 4) {
		if (val < 256) return;
		B.iBW = val;
		cmd = "SL";
		cmd.append(to_decimal(B.iBW & 0xFF, 2)).append(";");
		sendCommand(cmd);
		showresp(WARN, ASC, SL_tooltip, cmd, "");
		cmd = "SH";
		cmd.append(to_decimal(((B.iBW >> 8) & 0x7F), 2)).append(";");
		sendCommand(cmd);
		showresp(WARN, ASC, SH_tooltip, cmd, "");
	}
	if (val > 256) return;
	else if (B.imode == 2 || B.imode == 6) { // CW
		B.iBW = val;
		cmd = TS480HX_CWbw[B.iBW];
		sendCommand(cmd);
		showresp(WARN, ASC, "set CW bw", cmd, "");
	}else if (B.imode == 5 || B.imode == 7) {
		B.iBW = val;
		cmd = TS480HX_FSKbw[B.iBW];
		sendCommand(cmd);
		showresp(WARN, ASC, "set FSK bw", cmd, "");
	}
}

int RIG_TS480HX::get_bwB()
{
	int i = 0;
	size_t p;
	bool menu45 = menu_45;

	check_menu_45();
	if (menu45 != menu_45)
		Fl::awake(updateBandwidthControl);

	if (B.imode == 0 || B.imode == 1 || B.imode == 3 || B.imode == 4) {
		int lo = B.iBW & 0xFF, hi = (B.iBW >> 8) & 0x7F;
		cmd = "SL;";
		get_trace(1, "get SL");
		ret = wait_char(';', 5, 100, "get SL", ASC);
		gett("");
		if (ret == 5) {
			p = replystr.rfind("SL");
			if (p != std::string::npos)
				lo = fm_decimal(replystr.substr(p+2), 2);
		}
		cmd = "SH;";
		get_trace(1, "get SH");
		ret = wait_char(';', 5, 100, "get SH", ASC);
		gett("");
		if (ret == 5) {
			p = replystr.rfind("SH");
			if (p != std::string::npos)
				hi = fm_decimal(replystr.substr(p+2), 2);
			B.iBW = ((hi << 8) | (lo & 0xFF)) | 0x8000;
		}
	} else if (B.imode == 2 || B.imode == 6) {
		cmd = "FW;";
		get_trace(1, "get FW");
		ret = wait_char(';', 7, 100, "get FW", ASC);
		gett("");
		if (ret == 7) {
			p = replystr.rfind("FW");
			if (p != std::string::npos) {
				for (i = 0; i < 11; i++)
					if (replystr.find(TS480HX_CWbw[i]) == p)
						break;
				if (i == 11) i = 10;
				B.iBW = i;
			}
		}
	} else if (B.imode == 5 || B.imode == 7) {
		cmd = "FW;";
		get_trace(1, "get FW");
		ret = wait_char(';', 7, 100, "get FW", ASC);
		gett("");
		if (ret == 7) {
			p = replystr.rfind("FW");
			if (p != std::string::npos) {
				for (i = 0; i < 4; i++)
					if (replystr.find(TS480HX_FSKbw[i]) == p)
						break;
				if (i == 4) i = 3;
				B.iBW = i;
			}
		}
	}
	return B.iBW;
}

int RIG_TS480HX::adjust_bandwidth(int val)
{
	int bw = 0;
	if (val == 0 || val == 1 || val == 3)
		bw = 0x8A03;
	else if (val == 4)
		bw = 0x8201;
	else if (val == 2 || val == 6)
		bw = 7;
	else if (val == 5 || val == 7)
		bw = 1;
	return bw;
}

int RIG_TS480HX::def_bandwidth(int val)
{
	return adjust_bandwidth(val);
}


void RIG_TS480HX::set_power_control(double val)
{
	cmd = "PC";
	char szval[4];
	if (modeA == 4 && val > 50) val = 50; // AM mode limitation
	snprintf(szval, sizeof(szval), "%03d", (int)val);
	cmd += szval;
	cmd += ';';
	LOG_WARN("%s", cmd.c_str());
	sendCommand(cmd);
}

double RIG_TS480HX::get_power_control()
{
	int val = progStatus.power_level;
	cmd = "PC;";
	get_trace(1, "get_power_control");
	ret = wait_char(';', 6, 100, "get Power control", ASC);
	gett("");
	if (ret < 6) return val;

	size_t p = replystr.rfind("PC");
	if (p == std::string::npos) return val;

	val = atoi(&replystr[p + 2]);

	return val;
}

void RIG_TS480HX::set_attenuator(int val)
{
	if (val)	cmd = "RA01;";
	else		cmd = "RA00;";
	LOG_WARN("%s", cmd.c_str());
	set_trace(1, "set attenuator");
	sendCommand(cmd);
	sett("");
	atten_level = val;
}

int RIG_TS480HX::get_attenuator()
{
	cmd = "RA;";
	get_trace(1, "get_attenuator");
	ret = wait_char(';', 7, 100, "get attenuator", ASC);
	gett("");
	if (ret < 7) return atten_level;

	size_t p = replystr.rfind("RA");
	if (p != std::string::npos)
		atten_level = (replystr[p+3] == '1');
	return atten_level;
}

void RIG_TS480HX::set_preamp(int val)
{
	if (val)	cmd = "PA1;";
	else		cmd = "PA0;";
	LOG_WARN("%s", cmd.c_str());
	set_trace(1, "set preamp");
	sendCommand(cmd);
	sett("");
	preamp_level = val;
}

int RIG_TS480HX::get_preamp()
{
	cmd = "PA;";
	get_trace(1, "get_preamp");
	ret = wait_char(';', 5, 100, "get preamp", ASC);
	gett("");
	if (ret < 5) return preamp_level;

	size_t p = replystr.rfind("PA");
	if (p != std::string::npos)
		preamp_level = (replystr[p+2] == '1');
	return preamp_level;
}

void RIG_TS480HX::set_if_shift(int val)
{
	cmd = "IS+";
	if (val < 0) cmd[2] = '-';
	cmd.append(to_decimal(abs(val),4)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set IF shift", cmd, "");
}

bool RIG_TS480HX::get_if_shift(int &val)
{
	cmd = "IS;";
	get_trace(1, "get_if_shift");
	ret = wait_char(';', 8, 100, "get IF shift", ASC);
	gett("");
	if (ret == 8) {
		size_t p = replystr.rfind("IS");
		if (p != std::string::npos) {
			val = fm_decimal(replystr.substr(p+3), 4);
			if (replystr[p+2] == '-') val *= -1;
			return (val != 0);
		}
	}
	val = progStatus.shift_val;
	return progStatus.shift;
}

void RIG_TS480HX::get_if_min_max_step(int &min, int &max, int &step)
{
	if_shift_min = min = -1100;
	if_shift_max = max = 1100;
	if_shift_step = step = 10;
	if_shift_mid = 0;
}

// Noise Reduction (TS2000.cxx) NR1 only works; no NR2 and don' no why
void RIG_TS480HX::set_noise_reduction(int val)
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
}

int  RIG_TS480HX::get_noise_reduction()
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

	if (_noise_reduction_level == 1) {
		nr_label("NR1", true);
	} else if (_noise_reduction_level == 2) {
		nr_label("NR2", true);
	} else {
		nr_label("NR", false);
	}

	return _noise_reduction_level;
}

void RIG_TS480HX::set_noise_reduction_val(int val)
{
	if (_noise_reduction_level == 0) return;
	if (_noise_reduction_level == 1) _nrval1 = val;
	else _nrval2 = val;

	cmd.assign("RL").append(to_decimal(val, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET_noise_reduction_val", cmd, "");
}

int  RIG_TS480HX::get_noise_reduction_val()
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

int  RIG_TS480HX::get_agc()
{
	cmd = "GT;";
	get_trace(1, "get_agc");
	ret = wait_char(';', 6, 100, "GET agc val", ASC);
	gett("");
	size_t p = replystr.rfind("GT");
	if (p == std::string::npos) return agcval;
	if (replystr[4] == ' ') return 0;
	agcval = replystr[4] - '0' + 1; // '0' == off, '1' = fast, '2' = slow
	return agcval;
}

int RIG_TS480HX::incr_agc()
{
	if (fm_mode) return 0;
	agcval++;
	if (agcval == 4) agcval = 1;
	cmd.assign("GT00");
	cmd += (agcval + '0' - 1);
	cmd += ";";
	sendCommand(cmd);
	showresp(WARN, ASC, "SET agc", cmd, replystr);
	return agcval;
}


static const char *agcstrs[] = {"FM", "AGC", "FST", "SLO"};
const char *RIG_TS480HX::agc_label()
{
	if (fm_mode) return agcstrs[0];
	return agcstrs[agcval];
}

int  RIG_TS480HX::agc_val()
{
	if (fm_mode) return 0;
	return agcval;
}

// Auto Notch, beat canceller (TS2000.cxx) BC1 only, not BC2
void RIG_TS480HX::set_auto_notch(int v)
{
	cmd = v ? "BC1;" : "BC0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set auto notch", cmd, "");

}

int  RIG_TS480HX::get_auto_notch()
{
	cmd = "BC;";
	get_trace(1, "get_auto_notch");
	ret = wait_char(';', 4, 100, "get auto notch", ASC);
	gett("");
	if (ret == 4) {
		int anotch = 0;
		size_t p = replystr.rfind("BC");
		if (p != std::string::npos) {
			anotch = (replystr[p+2] == '1');
			return anotch;
		}
	}
	return 0;
}

// Noise Blanker (TS2000.cxx)
void RIG_TS480HX::set_noise(bool b)
{
	if (b)
		cmd = "NB1;";
	else
		cmd = "NB0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set NB", cmd, "");
}

int RIG_TS480HX::get_noise()
{
	cmd = "NB;";
	get_trace(1, "get_noise");
	ret = wait_char(';', 4, 100, "get Noise Blanker", ASC);
	gett("");
	if (ret == 4) {
		size_t p = replystr.rfind("NB");
		if (p == std::string::npos) return 0;
		if (replystr[p+2] == '0') return 0;
	}
	return 1;
}

// Tranceiver PTT on/off
void RIG_TS480HX::set_PTT_control(int val)
{
	if (val) {
		if (progStatus.data_port) cmd = "TX1;"; // DTS transmission using ANI input
		else cmd = "TX0;"; // mic input
	} else cmd = "RX;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set PTT", cmd, "");
}

int RIG_TS480HX::get_PTT()
{
//	cmd = "IF;";
//	get_trace(1, "get_PTT");
//	ret = wait_char(';', 38, 100, "get VFO", ASC);
//	gett("");

	int ret = check_ifstr();
	if (ret < 38) return ptt_;
	ptt_ = (replystr[28] == '1');
	return ptt_;
}

void RIG_TS480HX::set_rf_gain(int val)
{
	cmd = "RG";
	cmd.append(to_decimal(val,3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set rf gain", cmd, "");
}

int  RIG_TS480HX::get_rf_gain()
{
	int val = progStatus.rfgain;
	cmd = "RG;";
	get_trace(1, "get_rf_gain");
	ret = wait_char(';', 6, 100, "get rf gain", ASC);
	gett("");
	if (ret < 6) return val;

	size_t p = replystr.rfind("RG");
	if (p != std::string::npos)
		val = fm_decimal(replystr.substr(p+2), 3);
	return val;
}

void RIG_TS480HX::get_rf_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

void RIG_TS480HX::selectA()
{
	cmd = "FR0;FT0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Rx on A, Tx on A", cmd, "");
	inuse = onA;
}

void RIG_TS480HX::selectB()
{
	cmd = "FR1;FT1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Rx on B, Tx on B", cmd, "");
	inuse = onB;
}

void RIG_TS480HX::set_split(bool val) 
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

bool RIG_TS480HX::can_split()
{
	return true;
}

int RIG_TS480HX::get_split()
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

unsigned long long RIG_TS480HX::get_vfoA ()
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

void RIG_TS480HX::set_vfoA (unsigned long long freq)
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

unsigned long long RIG_TS480HX::get_vfoB ()
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

void RIG_TS480HX::set_vfoB (unsigned long long freq)
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

// Squelch (TS990.cxx)
void RIG_TS480HX::set_squelch(int val)
{
		cmd = "SQ0";
		cmd.append(to_decimal(abs(val),3)).append(";");
		sendCommand(cmd);
		showresp(INFO, ASC, "set squelch", cmd, "");
}

int  RIG_TS480HX::get_squelch()
{
	int val = 0;
	cmd = "SQ0;";
		if (wait_char(';', 7, 20, "get squelch", ASC) >= 7) {
			size_t p = replystr.rfind("SQ0");
			if (p == std::string::npos) return val;
			replystr[p + 6] = 0;
			val = atoi(&replystr[p + 3]);
	}
	return val;
}

void RIG_TS480HX::get_squelch_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 255; step = 1;
}

void RIG_TS480HX::set_mic_gain(int val)
{
	cmd = "MG";
	cmd.append(to_decimal(val,3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set mic gain", cmd, "");
}

int  RIG_TS480HX::get_mic_gain()
{
	int val = progStatus.mic_gain;
	cmd = "MG;";
	if (wait_char(';', 6, 100, "get mic gain", ASC) < 6) return val;

	size_t p = replystr.rfind("MG");
	if (p != std::string::npos)
		val = fm_decimal(replystr.substr(p+2), 3);
	return val;
}

void RIG_TS480HX::get_mic_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}


void RIG_TS480HX::set_volume_control(int val)
{
	cmd = "AG";
	char szval[20];
	snprintf(szval, sizeof(szval), "%04d", val * 255 / 100);
	cmd += szval;
	cmd += ';';
	LOG_WARN("%s", cmd.c_str());
	sendCommand(cmd);
}

int RIG_TS480HX::get_volume_control()
{
	int val = progStatus.volume;
	cmd = "AG0;";
	if (wait_char(';', 7, 100, "get vol", ASC) < 7) return val;

	size_t p = replystr.rfind("AG");
	if (p == std::string::npos) return val;
	replystr[p + 6] = 0;
	val = atoi(&replystr[p + 3]);
	val = val * 100 / 255;
	return val;
}

void RIG_TS480HX::tune_rig()
{
	cmd = "AC111;";
	LOG_WARN("%s", cmd.c_str());
	sendCommand(cmd);
}

