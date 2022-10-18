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

#include "config.h"

#include "kenwood/TS590SG.h"
#include "support.h"

static const char TS590SGname_[] = "TS-590SG";

static const char *TS590SGmodes_[] = {
"LSB", "USB",  "CW", "FM", "AM", "FSK", "CW-R", "FSK-R", 
"LSB-D", "USB-D", "FM-D", "AM-D", NULL};

static const char TS590SG_mode_chr[] =  { 
'1', '2', '3', '4', '5', '6', '7', '9',
'1', '2', '4', '5' };
static const char TS590SG_mode_type[] = { 
'L', 'U', 'U', 'U', 'U', 'L', 'L', 'U', 
'L', 'U', 'U', 'U' };

//----------------------------------------------------------------------
static const char *TS590SG_empty[] = { "N/A", NULL };

//----------------------------------------------------------------------
static int DEF_SL_SH = 0x8A03;
static const char *TS590SG_SSB_SL[] = {
  "0",   "50", "100", "200", "300", 
"400",  "500", "600", "700", "800", 
"900", "1000", NULL };

static const char *TS590SG_CAT_ssb_SL[] = {
"SL00;", "SL01;", "SL02;", "SL03;", "SL04;", 
"SL05;", "SL06;", "SL07;", "SL08;", "SL09;",
"SL10;", "SL11;", NULL };
static const char *TS590SG_SSB_SL_tooltip = "lo cut";
static const char *TS590SG_SSB_btn_SL_label = "L";

static const char *TS590SG_SSB_SH[] = {
"1000", "1200", "1400", "1600", "1800", 
"2000", "2200", "2400", "2600", "2800", 
"3000", "3400", "4000", "5000", NULL };

static const char *TS590SG_CAT_ssb_SH[] = {
"SH00;", "SH01;", "SH02;", "SH03;", "SH04;", 
"SH05;", "SH06;", "SH07;", "SH08;", "SH09;",
"SH10;", "SH11;", "SH12;", "SH13;", NULL };
static const char *TS590SG_SSB_SH_tooltip = "hi cut";
static const char *TS590SG_SSB_btn_SH_label = "H";

//----------------------------------------------------------------------
static int DEF_width_shift = 0x8D05;
static const char *TS590SG_DATA_width[] = {
  "50",   "80",  "100",  "150", "200", 
 "250",  "300",  "400",  "500", "600", 
"1000", "1500", "2000", "2500",  NULL };

static const char *TS590SG_CAT_data_width[] = {
"SL00;", "SL01;", "SL02;", "SL03;", "SL04;", 
"SL05;", "SL06;", "SL07;", "SL08;", "SL09;",
"SL10;", "SL11;", "SL12;", "SL13;", NULL };
static const char *TS590SG_DATA_W_tooltip = "width";
static const char *TS590SG_DATA_W_btn_label = "W";

static const char *TS590SG_DATA_shift[] = {
"1000", "1100", "1200", "1300", "1400", 
"1500", "1600", "1700", "1800", "1900", 
"2000", "2100", "2210", NULL };

static const char *TS590SG_CAT_data_shift[] = {
"SH00;", "SH01;", "SH02;", "SH03;", "SH04;", 
"SH05;", "SH06;", "SH07;", "SH08;", "SH09;",
"SH10;", "SH11;", "SH12;", NULL };
static const char *TS590SG_DATA_S_tooltip = "shift";
static const char *TS590SG_DATA_S_btn_label = "S";

//----------------------------------------------------------------------
static int DEF_am = 0x8201;
static const char *TS590SG_AM_SL[] = {
"10", "100", "200", "500", NULL };

static const char *TS590SG_CAT_am_SL[] = {
"SL00;", "SL01;", "SL02;", "SL03;", NULL}; 
static const char *TS590SG_AM_SL_tooltip = "lo cut";
static const char *TS590SG_AM_btn_SL_label = "L";

static const char *TS590SG_AM_SH[] = {
"2500", "3000", "4000", "5000", NULL };

static const char *TS590SG_CAT_am_SH[] = {
"SH00;", "SH01;", "SH02;", "SH03;", NULL}; 
static const char *TS590SG_AM_SH_tooltip = "hi cut";
static const char *TS590SG_AM_btn_SH_label = "H";

//----------------------------------------------------------------------
static int  DEF_cw = 7;
static const char *TS590SG_CWwidths[] = {
  "50",   "80",  "100",  "150", "200", 
 "250",  "300",  "400",  "500", "600", 
"1000", "1500", "2000", "2500",  NULL};

static const char *TS590SG_CWbw[] = {
"FW0050;", "FW0080;", "FW0100;", "FW0150;", "FW0200;",
"FW0250;", "FW0300;", "FW0400;", "FW0500;", "FW0600;", 
"FW1000;", "FW1500;", "FW2000;", "FW2500;", NULL};

//----------------------------------------------------------------------
static int  DEF_fsk = 1;
static const char *TS590SG_FSKwidths[] = {
"250", "500", "1000", "1500", NULL};

static const char *TS590SG_FSKbw[] = {
"FW0250;", "FW0500;", "FW1000;", "FW1500;", NULL };

//----------------------------------------------------------------------

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 }, // 0
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 }, // 1
	{ (Fl_Widget *)sldrRFGAIN,   54, 145, 156 }, // 2
	{ (Fl_Widget *)btnNR,         2, 165,  50 }, // 3
	{ (Fl_Widget *)sldrNR,       54, 165, 156 }, // 4
	{ (Fl_Widget *)sldrPOWER,    54, 185, 368 }, // 5
	{ (Fl_Widget *)btnIFsh,     214, 105,  50 }, // 6
	{ (Fl_Widget *)sldrIFSHIFT, 266, 105, 156 }, // 7
	{ (Fl_Widget *)btnNotch,    214, 125,  50 }, // 8
	{ (Fl_Widget *)sldrNOTCH,   266, 125, 156 }, // 9
	{ (Fl_Widget *)sldrSQUELCH, 266, 145, 156 }, // 10
	{ (Fl_Widget *)sldrMICGAIN, 266, 165, 156 }, // 11
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

static std::string menu005;

void RIG_TS590SG::initialize()
{
	rig_widgets[0].W = btnVol;
	rig_widgets[1].W = sldrVOLUME;
	rig_widgets[2].W = sldrRFGAIN;
	rig_widgets[3].W = btnNR;
	rig_widgets[4].W = sldrNR;
	rig_widgets[5].W = sldrPOWER;
	rig_widgets[6].W = btnIFsh;
	rig_widgets[7].W = sldrIFSHIFT;
	rig_widgets[8].W = btnNotch;
	rig_widgets[9].W = sldrNOTCH;
	rig_widgets[10].W = sldrSQUELCH;
	rig_widgets[11].W = sldrMICGAIN;

//	cmd = "EX0050000;";
//	if (wait_char(';', 11, 100, "read ex 003", ASC) == 11)
//		menu005 = replystr;
//	cmd = "EX005000000;";
//	sendCommand(cmd);

	cmd = "AC000;"; sendCommand(cmd);
	selectA();
	get_preamp();
	get_attenuator();
}

void RIG_TS590SG::shutdown()
{
// restore state of xcvr beeps
//	cmd = menu005;
//	sendCommand(cmd);
}


RIG_TS590SG::RIG_TS590SG() {

	name_ = TS590SGname_;
	modes_ = TS590SGmodes_;
	bandwidths_ = TS590SG_SSB_SH;

	dsp_SL     = TS590SG_SSB_SL;
	SL_tooltip = TS590SG_SSB_SL_tooltip;
	SL_label   = TS590SG_SSB_btn_SL_label;

	dsp_SH     = TS590SG_SSB_SH;
	SH_tooltip = TS590SG_SSB_SH_tooltip;
	SH_label   = TS590SG_SSB_btn_SH_label;

	widgets = rig_widgets;

	serial_baudrate = BR115200;
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

	B.imode = A.imode = USB;
	B.iBW = A.iBW = DEF_SL_SH;
	B.freq = A.freq = 14070000ULL;
	can_change_alt_vfo = true;

	nb_level = 2;

	has_micgain_control =
	has_ifshift_control = false;

	has_auto_notch =
	has_notch_control =
	has_sql_control =
	has_swr_control =
	has_noise_reduction =
	has_noise_reduction_control =
	has_alc_control =
	has_dsp_controls =
	has_smeter =
	has_power_out =
	has_split =
	has_split_AB =
	has_noise_control =
	has_micgain_control =
	has_rf_control =
	has_volume_control =
	has_power_control =
	has_tune_control =
	has_attenuator_control =
	has_preamp_control =
	has_mode_control =
	has_bandwidth_control =
	has_ifshift_control =
	has_ptt_control = true;

	rxtxa = true;

	precision = 1;
	ndigits = 8;

	att_level = 0;
	preamp_level = 0;
	noise_reduction_level = 0;
}

static int ret = 0;

const char * RIG_TS590SG::get_bwname_(int n, int md) 
{
	static char bwname[20];
	if (n > 256) {
		int hi = (n >> 8) & 0x7F;
		int lo = n & 0xFF;
		snprintf(bwname, sizeof(bwname), "%s/%s",
			(md == LSB || md == USB || md == FM) ? TS590SG_SSB_SL[lo] :
			(md == AM || md == AMD) ? TS590SG_AM_SL[lo] :
			TS590SG_DATA_width[lo],
			(md == LSB || md == USB || md == FM) ? TS590SG_SSB_SH[hi] :
			(md == AM || md == AMD) ? TS590SG_AM_SH[hi] :
			TS590SG_DATA_shift[hi] );
	} else {
		snprintf(bwname, sizeof(bwname), "%s",
			(md == CW || md == CWR) ? TS590SG_CWwidths[n] : TS590SG_FSKwidths[n]);
	}
	return bwname;
}

int RIG_TS590SG::get_smeter()
{
	int mtr = 0;
	cmd = "SM0;";
	get_trace(1, "get_smeter");
	ret = wait_char(';', 8, 100, "get", ASC);
	gett("");
	if (ret < 8) return 0;

	size_t p = replystr.find("SM0");
	if (p == std::string::npos) return 0;

	replystr[p + 7] = 0;
	mtr = atoi(&replystr[p + 3]);
	mtr *= 50;
	mtr /= 15;
	if (mtr > 100) mtr = 100;
	return mtr;
}

int RIG_TS590SG::get_power_out()
{
	int mtr = 0;
	cmd = "SM0;";
	get_trace(1, "get_power_out");
	ret = wait_char(';', 8, 100, "get power", ASC);
	gett("");
	if (ret < 8) return mtr;

	size_t p = replystr.rfind("SM0");
	if (p == std::string::npos) return mtr;

	mtr = atoi(&replystr[p + 3]);
	mtr *= 50;
	mtr /= 18;
	if (mtr > 100) mtr = 100;

	return mtr;
}

// Transceiver power level
void RIG_TS590SG::set_power_control(double val)
{
	int ival = (int)val;
	cmd = "PC000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ival % 10;
		ival /= 10;
	}
	sendCommand(cmd, 0);
}

double RIG_TS590SG::get_power_control()
{
	cmd = "PC;";
	get_trace(1, "get_power_control");
	ret = wait_char(';', 6, 100, "get pwr ctrl", ASC);
	gett("");
	if (ret < 6) return 0;

	size_t p = replystr.rfind("PC");
	if (p == std::string::npos) return 0;
	int mtr = 0;
	replystr[p + 5] = 0;
	mtr = atoi(&replystr[p + 2]);
	return mtr;
}

void RIG_TS590SG::set_attenuator(int val)
{
	att_level = val;
	if (val) cmd = "RA01;";
	else     cmd = "RA00;";
	sendCommand(cmd, 0);
}

int RIG_TS590SG::get_attenuator()
{
	cmd = "RA;";
	get_trace(1, "get_attenuator");
	ret = wait_char(';', 7, 100, "get att", ASC);
	gett("");
	if (ret < 7) return 0;

	size_t p = replystr.rfind("RA");
	if (p == std::string::npos) return 0;

	if (replystr[p + 2] == '0' && 
		replystr[p + 3] == '0')
		att_level = 0;
	else
		att_level = 1;

	return att_level;
}

void RIG_TS590SG::set_preamp(int val)
{
	preamp_level = val;
	if (val) cmd = "PA1;";
	else     cmd = "PA0;";
	sendCommand(cmd, 0);
}

int RIG_TS590SG::get_preamp()
{
	cmd = "PA;";
	get_trace(1, "get_preamp");
	ret = wait_char(';', 5, 100, "get preamp", ASC);
	gett("");
	if (ret < 5) return 0;

	size_t p = replystr.rfind("PA");
	if (p == std::string::npos) return 0;

	if (replystr[p  + 2] == '1') 
		preamp_level = 1;
	else
		preamp_level = 0;
	return preamp_level;
}

//======================================================================
// mode commands
//======================================================================

void RIG_TS590SG::set_modeA(int val)
{
	active_mode = A.imode = val;
	cmd = "MD";
	cmd += TS590SG_mode_chr[val];
	cmd += ';';
	sendCommand(cmd, 0);
	showresp(ERR, ASC, "set mode A", cmd, "");
	if ( val == LSBD || val == USBD || val == FMD || val == AMD) {
		data_mode = true;
		cmd = "DA1;";
		sendCommand(cmd, 0);
		showresp(WARN, ASC, "set data A", cmd, "");
	} else if (val == LSB || val == USB || val == FM || val == AM) {
		data_mode = false;
		cmd = "DA0;";
		sendCommand(cmd, 0);
		showresp(WARN, ASC, "set data A", cmd, "");
	}
	set_widths(val);
}

int RIG_TS590SG::get_modeA()
{
	int md = A.imode;
	cmd = "MD;";
	get_trace(1, "get_modeA");
	ret = wait_char(';', 4, 100, "get mode A", ASC);
	gett("");
	if (ret < 4) return A.imode;

	size_t p = replystr.rfind("MD");
	if (p == std::string::npos) return A.imode;

	switch (replystr[p + 2]) {
		case '1' : md = LSB; break;
		case '2' : md = USB; break;
		case '3' : md = CW; break;
		case '4' : md = FM; break;
		case '5' : md = AM; break;
		case '6' : md = FSK; break;
		case '7' : md = CWR; break;
		case '9' : md = FSKR; break;
		default : md = A.imode;
	}

	if (md == LSB || md == USB || md == FM || md == AM) {
		cmd = "DA;";
		get_trace(1, "get_modeA DATA");
		ret = wait_char(';', 4, 100, "get data A", ASC);
		gett("");
		if (ret < 4) return A.imode;

		p = replystr.rfind("DA");
		if (p == std::string::npos) return A.imode;
		if (replystr[p + 2] == '1') {
			data_mode = true;
			if (md == LSB) md = LSBD;
			else if (md == USB) md = USBD;
			else if (md == FM) md = FMD;
			else if (md == AM) md = AMD;
		}
	}
	if (md != A.imode) {
		active_mode = A.imode = md;
		set_widths(md);
	}
	return A.imode;
}

void RIG_TS590SG::set_modeB(int val)
{
	active_mode = B.imode = val;
	cmd = "MD";
	cmd += TS590SG_mode_chr[val];
	cmd += ';';
	sendCommand(cmd, 0);
	showresp(WARN, ASC, "set mode B", cmd, "");
	if ( val == LSBD || val == USBD || val == FMD || val == AMD) {
		data_mode = true;
		cmd = "DA1;";
		sendCommand(cmd, 0);
		showresp(WARN, ASC, "set data B", cmd, "");
	} else if (val == LSB || val == USB || val == FM || val == AM) {
		cmd = "DA0;";
		sendCommand(cmd, 0);
		showresp(WARN, ASC, "set data B", cmd, "");
	}
	set_widths(val);
}

int RIG_TS590SG::get_modeB()
{
	int md = B.imode;
	cmd = "MD;";
	get_trace(1, "get_modeB");
	ret = wait_char(';', 4, 100, "get mode B", ASC);
	gett("");
	if (ret < 4) return B.imode;

	size_t p = replystr.rfind("MD");
	if (p == std::string::npos) return B.imode;

	switch (replystr[p + 2]) {
		case '1' : md = LSB; break;
		case '2' : md = USB; break;
		case '3' : md = CW; break;
		case '4' : md = FM; break;
		case '5' : md = AM; break;
		case '6' : md = FSK; break;
		case '7' : md = CWR; break;
		case '9' : md = FSKR; break;
		default : md = B.imode;
	}

	if (md == LSB || md == USB || md == FM || md == AM) {
		cmd = "DA;";
		get_trace(1, "get_modeB DATA");
		ret = wait_char(';', 4, 100, "get dat B", ASC);
		gett("");
		if (ret < 4) return B.imode;

		p = replystr.rfind("DA");
		if (p == std::string::npos) return B.imode;
		if (replystr[p + 2] == '1') {
			data_mode = true;
			if (md == LSB) md = LSBD;
			else if (md == USB) md = USBD;
			else if (md == FM) md = FMD;
			else if (md == AM) md = AMD;
		}
	}
	if (md != B.imode) {
		active_mode = B.imode = md;
		set_widths(md);
	}
	return B.imode;
}

//======================================================================
// Bandpass filter commands
//======================================================================

int RIG_TS590SG::set_widths(int val)
{
	int bw = 0;
	if (val == LSB || val == USB || val == FM || val == FMD) {
		bandwidths_ = TS590SG_SSB_SH;
		dsp_SL = TS590SG_SSB_SL;
		dsp_SH = TS590SG_SSB_SH;
		SL_tooltip = TS590SG_SSB_SL_tooltip;
		SL_label   = TS590SG_SSB_btn_SL_label;
		SH_tooltip = TS590SG_SSB_SH_tooltip;
		SH_label   = TS590SG_SSB_btn_SH_label;
		bw = DEF_SL_SH; // 200 lower, 3000 upper
	} else if (val == CW || val == CWR) {
		bandwidths_ = TS590SG_CWwidths;
		dsp_SL = TS590SG_empty;
		dsp_SH = TS590SG_empty;
		bw = DEF_cw;
	} else if (val == FSK || val == FSKR) {
		bandwidths_ = TS590SG_FSKwidths;
		dsp_SL = TS590SG_empty;
		dsp_SH = TS590SG_empty;
		bw = 1;
	} else if (val == AM || val == AMD) { // val == 4 ==> AM
		bandwidths_ = TS590SG_AM_SH;
		dsp_SL = TS590SG_AM_SL;
		dsp_SH = TS590SG_AM_SH;
		SL_tooltip = TS590SG_AM_SL_tooltip;
		SL_label   = TS590SG_AM_btn_SL_label;
		SH_tooltip = TS590SG_AM_SH_tooltip;
		SH_label   = TS590SG_AM_btn_SH_label;
		bw = DEF_am;
	} else if (val == LSBD || val == USBD) {
		bandwidths_ = TS590SG_DATA_width;

		dsp_SL = TS590SG_DATA_shift;
		SL_tooltip = TS590SG_DATA_S_tooltip;
		SL_label   = TS590SG_DATA_S_btn_label;

		dsp_SH = TS590SG_DATA_width;
		SH_tooltip = TS590SG_DATA_W_tooltip;
		SH_label   = TS590SG_DATA_W_btn_label;
		bw = DEF_width_shift;
	}
	return bw;
}

const char **RIG_TS590SG::bwtable(int m)
{
	if (m == LSB || m == USB || m == FM || m == FMD)
		return TS590SG_SSB_SH;
	else if (m == CW || m == CWR)
		return TS590SG_CWwidths;
	else if (m == FSK || m == FSKR)
		return TS590SG_FSKwidths;
	else if (m == AM || m == AMD)
		return TS590SG_AM_SH;
	else
		return TS590SG_DATA_width;
}

const char **RIG_TS590SG::lotable(int m)
{
	if (m == LSB || m == USB || m == FM || m == FMD)
		return TS590SG_SSB_SL;
	else if (m == AM || m == AMD)
		return TS590SG_AM_SL;
	else if (m == LSBD || m == USBD)
		return TS590SG_DATA_shift;
// CW CWR FSK FSKR
	return NULL;
}

const char **RIG_TS590SG::hitable(int m)
{
	if (m == LSB || m == USB || m == FM || m == FMD)
		return TS590SG_SSB_SH;
	else if (m == AM || m == AMD)
		return TS590SG_AM_SH;
	else if (m == LSBD || m == USBD)
		return TS590SG_DATA_width;
// CW CWR FSK FSKR
	return NULL;
}

int RIG_TS590SG::adjust_bandwidth(int val)
{
	if (val == LSB || val == USB || val == FM || val == FMD)
		return DEF_SL_SH;
	else if (val == LSBD || val == USBD)
		return DEF_width_shift;
	else if (val == CW || val == CWR)
		return DEF_cw;
	else if (val == FSK || val == FSKR)
		return DEF_fsk;
//	else if (val == AM)
	return DEF_am;
}

int RIG_TS590SG::def_bandwidth(int val)
{
	return adjust_bandwidth(val);
}

void RIG_TS590SG::set_bwA(int val)
{
// LSB, USB, FM, FM-D
	if (A.imode == LSB || A.imode == USB || A.imode == FM || A.imode == FMD) {
		if (val < 256) return;
		A.iBW = val;
		cmd = TS590SG_CAT_ssb_SL[A.iBW & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set lower", cmd, "");
		cmd = TS590SG_CAT_ssb_SH[(A.iBW >> 8) & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set upper", cmd, "");
		return;
	}
// LSB-D, USB-D
	if (A.imode == LSBD || A.imode == USBD) {
		if (val < 256) return;
		A.iBW = val;
		cmd = TS590SG_CAT_data_shift[A.iBW & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set shift", cmd, "");
		cmd = TS590SG_CAT_data_width[(A.iBW >> 8) & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set width", cmd, "");
		return;
	}
// AM
	if (A.imode == AM || A.imode == AMD) {
		if (val < 256) return;
		A.iBW = val;
		cmd = TS590SG_CAT_am_SL[A.iBW & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set lower", cmd, "");
		cmd = TS590SG_CAT_am_SH[(A.iBW >> 8) & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set upper", cmd, "");
		return;
	}
	if (val > 256) return;
// CW
	if (A.imode == CW || A.imode == CWR) {
		A.iBW = val;
		cmd = TS590SG_CWbw[A.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set CW bw", cmd, "");
		return;
	}
// FSK
	if (A.imode == FSK || A.imode == FSKR) {
		A.iBW = val;
		cmd = TS590SG_FSKbw[A.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set FSK bw", cmd, "");
		return;
	}
}

void RIG_TS590SG::set_bwB(int val)
{
	if (B.imode == LSB || B.imode == USB || B.imode == FM || B.imode == FMD) {
		if (val < 256) return;
		B.iBW = val;
		cmd = TS590SG_CAT_ssb_SL[B.iBW & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set lower", cmd, "");
		cmd = TS590SG_CAT_ssb_SH[(B.iBW >> 8) & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set upper", cmd, "");
		return;
	}
	if (B.imode == LSBD || B.imode == USBD) { // SSB data mode
		if (val < 256) return;
		B.iBW = val;
		cmd = TS590SG_CAT_data_shift[B.iBW  & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set shift", cmd, "");
		cmd = TS590SG_CAT_data_width[(B.iBW >> 8) & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set width", cmd, "");
		return;
	}
	if (B.imode == AM || B.imode == AMD) {
		if (val < 256) return;
		B.iBW = val;
		cmd = TS590SG_AM_SL[B.iBW & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set lower", cmd, "");
		cmd = TS590SG_AM_SH[(B.iBW >> 8) & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set upper", cmd, "");
		return;
	}

	if (val > 256) return;
	if (B.imode == CW || B.imode == CWR) {
		B.iBW = val;
		cmd = TS590SG_CWbw[B.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set CW bw", cmd, "");
		return;
	}

	if (B.imode == FSK || B.imode == FSKR) {
		B.iBW = val;
		cmd = TS590SG_FSKbw[B.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set FSK bw", cmd, "");
		return;
	}
}

int RIG_TS590SG::get_bwA()
{
	int i = 0, lo = 0, hi = 0;
	size_t p;
	switch (A.imode) {
	case CW: case CWR:
		A.iBW = DEF_cw;
		cmd = "FW;";
		get_trace(1, "get_bwA FW");
		ret = wait_char(';', 7, 100, "get CW width", ASC);
		gett("");
		if (ret == 7) {
			p = replystr.rfind("FW");
			if (p != std::string::npos) {
				for (i = 0; i < 14; i++)
					if (replystr.find(TS590SG_CWbw[i]) == p)
						break;
				if (i == 14) i = 13;
				A.iBW = i;
			}
		}
		break;
	case FSK: case FSKR:
		A.iBW = DEF_fsk;
		cmd = "FW;";
		get_trace(1, "get_bwA FW");
		ret = wait_char(';', 7, 100, "get FSK width", ASC);
		gett("");
		if (ret == 7 ) {
			p = replystr.rfind("FW");
			if (p != std::string::npos) {
				for (i = 0; i < 4; i++)
					if (replystr.find(TS590SG_FSKbw[i]) == p)
						break;
				if (i == 4) i = 3;
				A.iBW = i;
			}
		}
		break;
	case LSB: case USB: case FM:
		A.iBW = DEF_SL_SH;
		lo = A.iBW & 0x7F;
		hi = (A.iBW >> 8) & 0x7F;
		cmd = "SL;";
		get_trace(1, "get_bwA SSB SL");
		ret = wait_char(';', 5, 100, "get lower", ASC);
		gett("");
		if (ret == 5) {
			p = replystr.rfind("SL");
			if (p == std::string::npos) break;
			lo = fm_decimal(replystr.substr(2), 2);
			cmd = "SH;";
			get_trace(1, "get_bwA SSB SL");
			ret = wait_char(';', 5, 100, "get upper", ASC);
			gett("");
			if (ret == 5) {
				p = replystr.rfind("SH");
				if (p == std::string::npos) break;
				hi = fm_decimal(replystr.substr(2), 2);
				A.iBW = ((hi << 8) | (lo & 0x7F)) | 0x8000;
			}
		}
		break;
	case LSBD: case USBD: case FMD:
		A.iBW = DEF_width_shift;
		lo = A.iBW & 0x7F;
		hi = (A.iBW >> 8) & 0x7F;
		cmd = "SL;";
		get_trace(1, "get_bwA DIGI");
		ret = wait_char(';', 5, 100, "get width", ASC);
		gett("");
		if (ret == 5) {
			p = replystr.rfind("SL");
			if (p == std::string::npos) break;
			hi = fm_decimal(replystr.substr(2), 2);
			cmd = "SH;";
			get_trace(1, "get_bwA SSB SL");
			ret = wait_char(';', 5, 100, "get shift", ASC);
			gett("");
			if (ret == 5) {
				p = replystr.rfind("SH");
				if (p == std::string::npos) break;
				lo = fm_decimal(replystr.substr(2), 2);
				A.iBW = ((hi << 8) | (lo & 0x7F)) | 0x8000;
			}
		}
		break;
	}
	return A.iBW;
}

int RIG_TS590SG::get_bwB()
{
	int i = 0, lo = 0, hi = 0;
	size_t p;
	switch (B.imode) {
	case CW: case CWR:
		B.iBW = DEF_cw;
		cmd = "FW;";
		get_trace(1, "get_bwB FW");
		ret = wait_char(';', 7, 100, "get CW width", ASC);
		gett("");
		if (ret == 7) {
			p = replystr.rfind("FW");
			if (p != std::string::npos) {
				for (i = 0; i < 14; i++)
					if (replystr.find(TS590SG_CWbw[i]) == p)
						break;
				if (i == 14) i = 13;
				B.iBW = i;
			}
		}
		break;
	case FSK: case FSKR:
		B.iBW = DEF_fsk;
		cmd = "FW;";
		get_trace(1, "get_bwB FW");
		ret = wait_char(';', 7, 100, "get FSK width", ASC);
		gett("");
		if (ret == 7) {
			p = replystr.rfind("FW");
			if (p != std::string::npos) {
				for (i = 0; i < 4; i++)
					if (replystr.find(TS590SG_FSKbw[i]) == p)
						break;
				if (i == 4) i = 3;
				B.iBW = i;
			}
		}
		break;
	case LSB: case USB: case FM:
		B.iBW = DEF_SL_SH;
		lo = B.iBW & 0xFF;
		hi = (B.iBW >> 8) & 0x7F;
		cmd = "SL;";
		get_trace(1, "get_bwB SL");
		ret = wait_char(';', 5, 100, "get lower", ASC);
		gett("");
		if (ret == 5) {
			p = replystr.rfind("SL");
			if (p == std::string::npos) break;
			lo = fm_decimal(replystr.substr(2), 2);
			cmd = "SH;";
			get_trace(1, "get_bwB SH");
			ret = wait_char(';', 5, 100, "get upper", ASC);
			gett("");
			if (ret == 5) {
				p = replystr.rfind("SH");
				if (p == std::string::npos) break;
				hi = fm_decimal(replystr.substr(2), 2);
				B.iBW = ((hi << 8) | (lo & 0x7F)) | 0x8000;
			}
		}
		break;
	case LSBD: case USBD: case FMD:
		B.iBW = DEF_width_shift;
		lo = B.iBW & 0x7F;
		hi = (B.iBW >> 8) & 0x7F;
		cmd = "SL;";
		get_trace(1, "get_bwB Sl");
		ret = wait_char(';', 5, 100, "get width", ASC);
		gett("");
		if (ret == 5) {
			p = replystr.rfind("SL");
			if (p == std::string::npos) break;
			hi = fm_decimal(replystr.substr(2), 2);
			cmd = "SH;";
			get_trace(1, "get_bwB SH");
			ret = wait_char(';', 5, 100, "get shift", ASC);
			gett("");
			if (ret == 5 ) {
				p = replystr.rfind("SH");
				if (p == std::string::npos) break;
				lo = fm_decimal(replystr.substr(2), 2);
				B.iBW = ((hi << 8) | (lo & 0xFF)) | 0x8000;
			}
		}
		break;
	}
	return B.iBW;
}

int RIG_TS590SG::get_modetype(int n)
{
	return TS590SG_mode_type[n];
}

//======================================================================
// IF shift command only available if the transceiver is in the CW mode
// step size is 50 Hz
//======================================================================
void RIG_TS590SG::set_if_shift(int val)
{
	if (active_mode == CW || active_mode == CWR) { // cw modes
		progStatus.shift_val = val;
		cmd = "IS ";
		cmd.append(to_decimal(abs(val),4)).append(";");
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set IF shift", cmd, "");
	}
}

bool RIG_TS590SG::get_if_shift(int &val)
{
	if (active_mode == CW || active_mode == CWR) { // cw modes
		cmd = "IS;";
		get_trace(1, "get_if_shift");
		ret = wait_char(';', 8, 100, "get IF shift", ASC);
		gett("");
		if (ret == 8) {
			size_t p = replystr.rfind("IS");
			if (p != std::string::npos) {
				val = fm_decimal(replystr.substr(p+3), 4);
			} else
				val = progStatus.shift_val;
			return true;
		}
	}
	val = progStatus.shift_val;
	return false;
}

void RIG_TS590SG::get_if_min_max_step(int &min, int &max, int &step)
{
	if_shift_min = min = 300;
	if_shift_max = max = 1000;
	if_shift_step = step = 50;
	if_shift_mid = 800;
}

void RIG_TS590SG::set_noise_reduction(int val)
{
	if (val == -1) {
		if (noise_reduction_level == 1) {
			nr_label("NR1", true);
		} else if (noise_reduction_level == 2) {
			nr_label("NR2", true);
		}
		return;
	}
	if (noise_reduction_level == 0) {
		noise_reduction_level = 1;
		nr_label("NR1", true);
	} else if (noise_reduction_level == 1) {
		noise_reduction_level = 2;
		nr_label("NR2", true);
	} else if (noise_reduction_level == 2) {
		noise_reduction_level = 0;
		nr_label("NR", false);
	}
	cmd.assign("NR");
	cmd += '0' + noise_reduction_level;
	sendCommand (cmd);
	showresp(WARN, ASC, "SET noise reduction", cmd, "");
}

int  RIG_TS590SG::get_noise_reduction()
{
	cmd = rsp = "NR";
	cmd.append(";");
	get_trace(1, "get_noise_reduction");
	ret = wait_char(';', 4, 100, "GET noise reduction", ASC);
	gett("");
	if (ret == 4 ) {
		size_t p = replystr.rfind(rsp);
		if (p == std::string::npos) return noise_reduction_level;
		noise_reduction_level = replystr[p+2] - '0';

		if (noise_reduction_level == 1) {
			nr_label("NR1", true);
		} else if (noise_reduction_level == 2) {
			nr_label("NR2", true);
		} else {
			nr_label("NR", false);
		}
	}

	return noise_reduction_level;
}

void RIG_TS590SG::set_noise_reduction_val(int val)
{
	cmd.assign("RL").append(to_decimal(val, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET_noise_reduction_val", cmd, "");
}

int  RIG_TS590SG::get_noise_reduction_val()
{
	if (noise_reduction_level == 0) return 0;
	int val = 1;
	cmd = rsp = "RL";
	cmd.append(";");
	get_trace(1, "get_noise_reduction_val");
	ret = wait_char(';', 5, 100, "GET noise reduction val", ASC);
	gett("");
	if (ret == 5) {
		size_t p = replystr.rfind(rsp);
		if (p == std::string::npos) return progStatus.noise_reduction_val;
		val = atoi(&replystr[p+2]);
	}
	return val;
}

void RIG_TS590SG::set_auto_notch(int v)
{
	cmd.assign("NT").append(v ? "1" : "0" ).append("0;");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET Auto Notch", cmd, "");
}

int  RIG_TS590SG::get_auto_notch()
{
	cmd = "NT;";
	get_trace(1, "get_auto_notch");
	ret = wait_char(';', 5, 100, "get auto notch", ASC);
	gett("");
	if (ret == 5) {
		size_t p = replystr.rfind("NT");
		if (p == std::string::npos) return 0;
		if (replystr[p+2] == '1') return 1;
	}
	return 0;
}

void RIG_TS590SG::set_notch(bool on, int val)
{
	if (on) {
		cmd.assign("NT20;");
		sendCommand(cmd);
		showresp(WARN, ASC, "Set notch ON", cmd, "");
		int bp = (int)((val - 300.0) * 128.0 / 2700.0);
		cmd.assign("BP").append(to_decimal(bp, 3)).append(";");
		sendCommand(cmd);
		showresp(WARN, ASC, "set notch freq", cmd, "");
	} else {
		cmd.assign("NT00;");
		sendCommand(cmd);
		showresp(WARN, ASC, "Set notch OFF", cmd, "");
	}
}

bool  RIG_TS590SG::get_notch(int &val)
{
	val = 300;
	cmd = "NT;";
	get_trace(1, "get_notch");
	ret = wait_char(';', 5, 100, "get notch state", ASC);
	gett("");
	if (ret == 5) {
		size_t p = replystr.rfind("NT");
		if (p == std::string::npos)
			return 0;
		if (replystr[p+2] == '2') {
			cmd.assign("BP;");
			get_trace(1, "get_notch_freq");
			ret = wait_char(';', 6, 100, "get notch freq", ASC);
			gett("");
			if (ret == 6) {
				size_t p = replystr.rfind("BP");
				if (p != std::string::npos)
					val = (int)((atoi(&replystr[p+2]) * 2700.0 / 128.0) + 300.0);
				return 1;
			}
		}
	}
	return 0;
}

void RIG_TS590SG::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = 300;
	max = 3000;
	step = 20;
}

void RIG_TS590SG::set_noise(bool val)
{
	if (nb_level == 0) {
		nb_level = 1;
		nb_label("NB 1", true);
	} else if (nb_level == 1) {
		nb_level = 2;
		nb_label("NB 2", true);
	} else if (nb_level == 2) {
		nb_level = 3;
		nb_label("NB 3", true);
	} else if (nb_level == 3) {
		nb_level = 0;
		nb_label("NB", false);
	}
	cmd = "NB0;";
	cmd[2] += nb_level;
	LOG_INFO("%s", cmd.c_str());
	sendCommand(cmd, 0);
}

int RIG_TS590SG::get_noise()
{
	cmd = "NB;";
	get_trace(1, "get_noise");
	ret = wait_char(';', 4, 100, "get Noise Blanker", ASC);
	gett("");
	if (ret == 4) {
		size_t p = replystr.rfind("NB");
		if (p == std::string::npos) return 0;
		if (replystr[p+2] == '0') return 0;
		nb_level = replystr[p+2] - '0';
		if (nb_level == 0) {
			nb_label("NB", false);
		} else if (nb_level == 1) {
			nb_label("NB 1", true);
		} else if (nb_level == 2) {
			nb_label("NB 2", true);
		} else if (nb_level == 3) {
			nb_label("NB 3", true);
		}
	}
	return nb_level;
}

void RIG_TS590SG::set_rf_gain(int val)
{
	cmd = "RG000;";
	int rfval = 255 - val;
	for (int i = 4; i > 1; i--) {
		cmd[i] = rfval % 10 + '0';
		rfval /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET rfgain", cmd, "");
}

int  RIG_TS590SG::get_rf_gain()
{
	int rfval = 0;
	cmd = rsp = "RG";
	cmd += ';';
	get_trace(1, "get_rf_gain");
	ret = wait_char(';', 6, 100, "get rfgain", ASC);
	gett("");
	if (ret == 6) {
		size_t p = replystr.rfind(rsp);
		if (p == std::string::npos) return progStatus.rfgain;
		for (int i = 2; i < 5; i++) {
			rfval *= 10;
			rfval += replystr[p+i] - '0';
		}
	}
	return 255 - rfval;
}

void RIG_TS590SG::get_rf_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 255;
	step = 1;
}

int RIG_TS590SG::get_PTT()
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

// Tranceiver PTT on/off
void RIG_TS590SG::set_PTT_control(int val)
{
	if (val) {
		if (data_mode)
			cmd = "TX1;"; 
		else 
			cmd = "TX0;";
	} else
		cmd = "RX;";
	sendCommand(cmd, 0);
}

