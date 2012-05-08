/*
 * Kenwood TS2000 driver
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */


#include "TS2000.h"
#include "support.h"

static const char TS2000name_[] = "TS-2000";

static const char *TS2000modes_[] = {
	"LSB", "USB", "CW", "FM", "AM", "FSK", "CW-R", "FSK-R", NULL};

static const char TS2000_mode_chr[] =  { '1', '2', '3', '4', '5', '6', '7', '9' };
static const char TS2000_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'L', 'L', 'U' };

static const char *TS2000_empty[] = { "N/A", NULL };

static const char *TS2000_lo[] = {
 "10",   "50", "100", "200", "300", 
"400",  "500", "600", "700", "800", 
"900", "1000", NULL };

static const char *TS2000_CAT_lo[] = {
"SL00;", "SL01;", "SL02;", "SL03;", "SL04;", 
"SL05;", "SL06;", "SL07;", "SL08;", "SL09;",
"SL10;", "SL11;", NULL };
static const char *TS2000_lo_tooltip = "lo cutoff";
static const char *TS2000_SSB_btn_lo_label = "L";

static const char *TS2000_hi[] = {
"1400", "1600", "1800", "2000", "2200",
"2400", "2600", "2800", "3000", "3400",
"4000", "5000", NULL };

static const char *TS2000_CAT_hi[] = {
"SH00;", "SH01;", "SH02;", "SH03;", "SH04;", 
"SH05;", "SH06;", "SH07;", "SH08;", "SH09;",
"SH10;", "SH11;", NULL };
static const char *TS2000_hi_tooltip = "hi cutoff";
static const char *TS2000_SSB_btn_hi_label = "H";

static const char *TS2000_AM_lo[] = {
"0", "100", "200", "500",
NULL };

static const char *TS2000_AM_hi[] = {
"2500", "3000", "4000", "5000",
NULL };

static const char *TS2000_CWwidths[] = {
"50", "80", "100", "150", "200", 
"300", "400", "500", "600", "1000", 
"1500", NULL};
static const char *TS2000_CWbw[] = {
"FW0050;", "FW0080;", "FW0100;", "FW0150;", "FW0200;", 
"FW0300;", "FW0400;", "FW0500;", "FW0600;", "FW1000;", 
"FW1500;" };

static const char *TS2000_FSKwidths[] = {
"250", "500", "1000", "1500", NULL};
static const char *TS2000_FSKbw[] = {
"FW0250;", "FW0500;", "FW1000;", "FW1500;" };

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 },
	{ (Fl_Widget *)sldrRFGAIN,   54, 145, 156 },
	{ (Fl_Widget *)btnIFsh,     214, 105,  50 },
	{ (Fl_Widget *)sldrIFSHIFT, 266, 105, 156 },
	{ (Fl_Widget *)btnNotch,    214, 125,  50 },
	{ (Fl_Widget *)sldrNOTCH,   266, 125, 156 },
	{ (Fl_Widget *)sldrMICGAIN, 266, 145, 156 },
	{ (Fl_Widget *)sldrPOWER,    54, 165, 368 },
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

// mid range on loudness
static string menu012 = "EX01200004";

void RIG_TS2000::initialize()
{
	rig_widgets[0].W = btnVol;
	rig_widgets[1].W = sldrVOLUME;
	rig_widgets[2].W = sldrRFGAIN;
	rig_widgets[3].W = btnIFsh;
	rig_widgets[4].W = sldrIFSHIFT;
	rig_widgets[5].W = btnNotch;
	rig_widgets[6].W = sldrNOTCH;
	rig_widgets[7].W = sldrMICGAIN;
	rig_widgets[8].W = sldrPOWER;

	menu012.clear();
	cmd = "EX0120000;"; // read menu 012 state
//might return something like EX01200004;

	if (waitN(11, 100, "read ex 012", ASC) == 11)
		menu012 = replystr;

// disable beeps before resetting front panel display to SWR
	cmd = "EX01200000;";
	sendCommand(cmd);
	select_swr();

// restore state of xcvr beeps
	cmd = menu012;
	sendCommand(cmd);
}

void RIG_TS2000::shutdown()
{
// restore state of xcvr beeps
//	cmd = menu012;
//	sendCommand(cmd);
}


RIG_TS2000::RIG_TS2000() {
// base class values	
	name_ = TS2000name_;
	modes_ = TS2000modes_;
	bandwidths_ = TS2000_empty;

	dsp_lo     = TS2000_lo;
	lo_tooltip = TS2000_lo_tooltip;
	lo_label   = TS2000_SSB_btn_lo_label;

	dsp_hi     = TS2000_hi;
	hi_tooltip = TS2000_hi_tooltip;
	hi_label   = TS2000_SSB_btn_hi_label;

	widgets = rig_widgets;

	comm_baudrate = BR4800;
	stopbits = 2;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_rtscts = true;
	comm_rtsplus = false;
	comm_dtrplus = false;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	B.imode = A.imode = 1;
	B.iBW = A.iBW = 0x8803;
	B.freq = A.freq = 14070000;
	can_change_alt_vfo = true;

	has_power_out =
	has_swr_control =
	has_alc_control =
	has_split =
	has_split_AB =
	has_dsp_controls =
	has_rf_control =
	has_micgain_control =
	has_notch_control =
	has_auto_notch =
	has_ifshift_control =
	has_smeter =
	has_noise_control =
	has_micgain_control =
	has_volume_control =
	has_power_control =
	has_tune_control =
	has_attenuator_control =
	has_preamp_control =
	has_mode_control =
	has_bandwidth_control =
	has_ptt_control = true;

	rxtxa = true;

	precision = 1;
	ndigits = 9;

}

const char * RIG_TS2000::get_bwname_(int n, int md) 
{
	static char bwname[20];
	if (n > 256) {
		int hi = (n >> 8) & 0x7F;
		int lo = n & 0xFF;
		snprintf(bwname, sizeof(bwname), "%s/%s",
			(md == 0 || md == 1 || md == 3) ? TS2000_lo[lo] : TS2000_AM_lo[lo],
			(md == 0 || md == 1 || md == 3) ? TS2000_hi[hi] : TS2000_AM_hi[hi] );
	} else {
		snprintf(bwname, sizeof(bwname), "%s",
			(md == 2 || md == 6) ? TS2000_CWwidths[n] : TS2000_FSKwidths[n]);
	}
	return bwname;
}

void RIG_TS2000::selectA()
{
	cmd = "FR0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Rx on A", cmd, replystr);
	cmd = "FT0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Tx on A", cmd, replystr);
}

void RIG_TS2000::selectB()
{
	cmd = "FR1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Rx on B", cmd, replystr);
	cmd = "FT1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Tx on B", cmd, replystr);
}

void RIG_TS2000::set_split(bool val) 
{
	split = val;
	if (useB) {
		if (val) {
			cmd = "FR1;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on B", cmd, replystr);
			cmd = "FT0;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Tx on A", cmd, replystr);
		} else {
			cmd = "FR1;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on B", cmd, replystr);
			cmd = "FT1;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Tx on B", cmd, replystr);
		}
	} else {
		if (val) {
			cmd = "FR0;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on A", cmd, replystr);
			cmd = "FT1;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Tx on B", cmd, replystr);
		} else {
			cmd = "FR0;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on A", cmd, replystr);
			cmd = "FT0;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Tx on A", cmd, replystr);
		}
	}
	Fl::awake(highlight_vfo, (void *)0);
}

int RIG_TS2000::get_split()
{
	size_t p;
	int split = 0;
	int rx, tx;
// tx vfo
	cmd = "FT;";
	waitN(4, 100, "get split tx vfo", ASC);
	p = replystr.rfind("FT");
	if (p == string::npos) return split;
	tx = replystr[p+2] - '0';

// rx vfo
	cmd = "FR;";
	waitN(4, 100, "get split rx vfo", ASC);

	p = replystr.rfind("FR");
	if (p == string::npos) return split;
	rx = replystr[p+2] - '0';
// split test
	split = tx * 2 + rx;

	return split;
}

long RIG_TS2000::get_vfoA ()
{
	cmd = "FA;";
	waitN(14, 100, "get vfo A", ASC);
	size_t p = replystr.rfind("FA");
	if (p != string::npos) {
		int f = 0;
		for (size_t n = 2; n < 13; n++)
			f = f*10 + replystr[p+n] - '0';
		A.freq = f;
	}
	return A.freq;
}

void RIG_TS2000::set_vfoA (long freq)
{
	A.freq = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd,0);
	showresp(WARN, ASC, "set vfo A", cmd, replystr);
}

long RIG_TS2000::get_vfoB ()
{
	cmd = "FB;";
	waitN(14, 100, "get vfo B", ASC);
	size_t p = replystr.rfind("FB");
	if (p != string::npos) {
		int f = 0;
		for (size_t n = 2; n < 13; n++)
			f = f*10 + replystr[p+n] - '0';
		B.freq = f;
	}
	return B.freq;
}

void RIG_TS2000::set_vfoB (long freq)
{
	B.freq = freq;
	cmd = "FB00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd,0);
	showresp(WARN, ASC, "set vfo B", cmd, replystr);
}

int RIG_TS2000::get_smeter()
{
	cmd = "SM0;";
	waitN(8, 100, "get smeter", ASC);
	size_t p = replystr.rfind("SM");
	if (p != string::npos) {
		int mtr = fm_decimal(&replystr[p+3],4);
		mtr = (mtr * 100) / 30;
		return mtr;
	}
	return 0;
}

int RIG_TS2000::get_swr()
{
	cmd = "RM;";
	waitN(8, 100, "get swr", ASC);
	size_t p = replystr.rfind("RM1");
	if (p != string::npos) {
		int mtr = fm_decimal(&replystr[p+3], 4);
		mtr = (mtr * 10) / 3;
		return mtr;
	}
	return 0;
}

int  RIG_TS2000::get_alc(void) 
{
	cmd = "RM;";
	waitN(8, 100, "get alc", ASC);
	size_t p = replystr.rfind("RM3");
	if (p != string::npos) {
		int mtr = fm_decimal(&replystr[p+3], 4);
		mtr = (mtr * 10) / 3;
		return mtr;
	}
	return 0;
}

void  RIG_TS2000::select_swr()
{
	cmd = "RM1;";
	sendCommand(cmd);
}

void  RIG_TS2000::select_alc()
{
	cmd = "RM3;";
	sendCommand(cmd);
}

// Transceiver power level
void RIG_TS2000::set_power_control(double val)
{
	int ival = (int)val;
	cmd = "PC";
	cmd.append(to_decimal(ival, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set pwr ctrl", cmd, replystr);
}

int RIG_TS2000::get_power_out()
{
	cmd = "SM0;";
	waitN(8, 100, "get power out", ASC);
	size_t p = replystr.rfind("SM0");
	if (p != string::npos) {
		int mtr = fm_decimal(&replystr[p+3],4);
		if (mtr <= 6) mtr = mtr * 2;
		else if (mtr <= 11) mtr = 11 + (mtr - 6)*(26 - 11)/(11 - 6);
		else if (mtr <= 18) mtr = 26 + (mtr - 11)*(50 - 26)/(18 - 11);
		else mtr = 50 + (mtr - 18)*(100 - 50)/(27 - 18);
		if (mtr > 100) mtr = 100;
		return mtr;
	}
	return 0;
}

int RIG_TS2000::get_power_control()
{
	cmd = "PC;";
	waitN(6, 100, "get pout", ASC);
	size_t p = replystr.rfind("PC");
	if (p != string::npos) {
		int mtr = fm_decimal(&replystr[p+2], 3);
		return mtr;
	}
	return 0;
}

// Volume control return 0 ... 100
int RIG_TS2000::get_volume_control()
{
	cmd = "AG0;";
	waitN(7, 100, "get vol", ASC);
	size_t p = replystr.rfind("AG");
	if (p != string::npos) {
		int val = fm_decimal(&replystr[p+3],3);
		return (int)(val / 2.55);
	}
	return 0;
}

void RIG_TS2000::set_volume_control(int val) 
{
	int ivol = (int)(val * 2.55);
	cmd = "AG0";
	cmd.append(to_decimal(ivol, 3)).append(";");
	sendCommand(cmd,0);
	showresp(WARN, ASC, "set vol", cmd, replystr);
}

// Tranceiver PTT on/off
void RIG_TS2000::set_PTT_control(int val)
{
	if (val) cmd = "TX;";
	else	 cmd = "RX;";
	sendCommand(cmd,0);
	showresp(WARN, ASC, "set PTT", cmd, replystr);
}

void RIG_TS2000::tune_rig()
{
	cmd = "AC111;";
	sendCommand(cmd,0);
	showresp(WARN, ASC, "tune", cmd, replystr);
}

void RIG_TS2000::set_attenuator(int val)
{
	att_level = val;
	if (val) cmd = "RA01;";
	else     cmd = "RA00;";
	sendCommand(cmd,0);
	showresp(WARN, ASC, "set ATT", cmd, replystr);
}

int RIG_TS2000::get_attenuator()
{
	cmd = "RA;";
	waitN(7, 100, "get ATT", ASC);
	size_t p = replystr.rfind("RA");
	if (p != string::npos && (p+3 < replystr.length())) {
		if (replystr[p+2] == '0' && replystr[p+3] == '0')
			att_level = 0;
		else
			att_level = 1;
	}
	return att_level;
}

void RIG_TS2000::set_preamp(int val)
{
	preamp_level = val;
	if (val) cmd = "PA1;";
	else     cmd = "PA0;";
	sendCommand(cmd,0);
	showresp(WARN, ASC, "set PRE", cmd, replystr);
}

int RIG_TS2000::get_preamp()
{
	cmd = "PA;";
	waitN(5, 100, "get PRE", ASC);
	size_t p = replystr.rfind("PA");
	if (p != string::npos && (p+2 < replystr.length())) {
		if (replystr[p+2] == '1') 
			preamp_level = 1;
		else
			preamp_level = 0;
	}
	return preamp_level;
}

int RIG_TS2000::set_widths(int val)
{
	int bw;
	if (val == LSB || val == USB || val == FM) {
		bandwidths_ = TS2000_empty;
		dsp_lo = TS2000_lo;
		dsp_hi = TS2000_hi;
		lo_tooltip = TS2000_lo_tooltip;
		lo_label   = TS2000_SSB_btn_lo_label;
		hi_tooltip = TS2000_hi_tooltip;
		hi_label   = TS2000_SSB_btn_hi_label;
		if (val == FM) bw = 0x8A03; // 200 ... 4000 Hz
		else bw = 0x8803; // 200 ... 3000 Hz
	} else if (val == CW || val == CWR) {
		bandwidths_ = TS2000_CWwidths;
		dsp_lo = TS2000_empty;
		dsp_hi = TS2000_empty;
		bw = 7;
	} else if (val == FSK || val == FSKR) {
		bandwidths_ = TS2000_FSKwidths;
		dsp_lo = TS2000_empty;
		dsp_hi = TS2000_empty;
		bw = 1;
	} else { // val == AM
		bandwidths_ = TS2000_empty;
		dsp_lo = TS2000_AM_lo;
		dsp_hi = TS2000_AM_hi;
		bw = 0x8201;
	}
	return bw;
}

const char **RIG_TS2000::bwtable(int val)
{
	if (val == LSB || val == USB || val == FM)
		return NULL;
	else if (val == CW || val == CWR)
		return TS2000_CWwidths;
	else if (val == FSK || val == FSKR)
		return TS2000_FSKwidths;
//else AM m == 4
	return NULL;
}

const char **RIG_TS2000::lotable(int val)
{
	if (val == LSB || val == USB || val == FM)
		return TS2000_lo;
	else if (val == CW || val == CWR)
		return NULL;
	else if (val == FSK || val == FSKR)
		return NULL;
	return TS2000_AM_lo;
}

const char **RIG_TS2000::hitable(int val)
{
	if (val == LSB || val == USB || val == FM)
		return TS2000_hi;
	else if (val == CW || val == CWR)
		return NULL;
	else if (val == FSK || val == FSKR)
		return NULL;
	return TS2000_AM_hi;
}

void RIG_TS2000::set_modeA(int val)
{
	A.imode = val;
	cmd = "MD";
	cmd += TS2000_mode_chr[val];
	cmd += ';';
	sendCommand(cmd,0);
	showresp(WARN, ASC, "set mode", cmd, replystr);
	A.iBW = set_widths(val);
}

int RIG_TS2000::get_modeA()
{
	cmd = "MD;";
	waitN(4, 100, "get mode A", ASC);
	size_t p = replystr.rfind("MD");
	if (p != string::npos) {
		int md = replystr[p+2];
		md = md - '1';
		if (md == 8) md = 7;
		A.imode = md;
		A.iBW = set_widths(A.imode);
	}
	return A.imode;
}

void RIG_TS2000::set_modeB(int val)
{
	B.imode = val;
	cmd = "MD";
	cmd += TS2000_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "set mode B", cmd, replystr);
	B.iBW = set_widths(val);
}

int RIG_TS2000::get_modeB()
{
	cmd = "MD;";
	waitN(4, 100, "get mode B", ASC);
	size_t p = replystr.rfind("MD");
	if (p != string::npos) {
		int md = replystr[p+2];
		md = md - '1';
		if (md == 8) md = 7;
		B.imode = md;
		B.iBW = set_widths(B.imode);
	}
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
		cmd = TS2000_CAT_lo[A.iBW & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set lower", cmd, replystr);
		cmd = "SH";
		cmd = TS2000_CAT_hi[(A.iBW >> 8) & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set upper", cmd, replystr);
	}
	if (val > 256) return;
	else if (A.imode == CW || A.imode == CWR) {
		A.iBW = val;
		cmd = TS2000_CWbw[A.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set CW bw", cmd, replystr);
	}else if (A.imode == FSK || A.imode == FSKR) {
		A.iBW = val;
		cmd = TS2000_FSKbw[A.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set FSK bw", cmd, replystr);
	}
}

int RIG_TS2000::get_bwA()
{
	int i = 0;
	size_t p;
	if (A.imode == LSB || A.imode == USB || A.imode == FM || A.imode == AM) {
		int lo = A.iBW & 0xFF, hi = (A.iBW >> 8) & 0x7F;
		cmd = "SL;";
		waitN(5, 100, "get SL", ASC);
		p = replystr.rfind("SL");
		if (p != string::npos)
			lo = fm_decimal(&replystr[2], 2);
		cmd = "SH;";
		waitN(5, 100, "get SH", ASC);
		p = replystr.rfind("SH");
		if (p != string::npos)
			hi = fm_decimal(&replystr[2], 2);
		A.iBW = ((hi << 8) | (lo & 0xFF)) | 0x8000;
	} else if (A.imode == CW || A.imode == CWR) { // CW
		cmd = "FW;";
		waitN(7, 100, "get FW", ASC);
		p = replystr.rfind("FW");
		if (p != string::npos) {
			for (i = 0; i < 11; i++)
				if (replystr.find(TS2000_CWbw[i]) == p)
					break;
			if (i == 11) i = 10;
			A.iBW = i;
		}
	} else if (A.imode == FSK || A.imode == FSKR) {
		cmd = "FW;";
		waitN(7, 100, "get FW", ASC);
		p = replystr.rfind("FW");
		if (p != string::npos) {
			for (i = 0; i < 4; i++)
				if (replystr.find(TS2000_FSKbw[i]) == p)
					break;
			if (i == 4) i = 3;
			A.iBW = i;
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
		cmd = TS2000_CAT_lo[B.iBW & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set lower", cmd, replystr);
		cmd = "SH";
		cmd = TS2000_CAT_hi[(B.iBW >> 8) & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set upper", cmd, replystr);
	}
	if (val > 256) return;
	else if (B.imode == CW || B.imode == CWR) { // CW
		B.iBW = val;
		cmd = TS2000_CWbw[B.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set CW bw", cmd, replystr);
	}else if (B.imode == FSK || B.imode == FSKR) {
		B.iBW = val;
		cmd = TS2000_FSKbw[B.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set FSK bw", cmd, replystr);
	}
}

int RIG_TS2000::get_bwB()
{
	int i = 0;
	size_t p;
	if (B.imode == LSB || B.imode == USB || B.imode == FM || B.imode == AM) {
		int lo = B.iBW & 0xFF, hi = (B.iBW >> 8) & 0x7F;
		cmd = "SL;";
		waitN(5, 100, "get SL", ASC);
		p = replystr.rfind("SL");
		if (p != string::npos)
			lo = fm_decimal(&replystr[2], 2);
		cmd = "SH;";
		waitN(5, 100, "get SH", ASC);
		p = replystr.rfind("SH");
		if (p != string::npos)
			hi = fm_decimal(&replystr[2], 2);
		B.iBW = ((hi << 8) | (lo & 0xFF)) | 0x8000;
	} else if (B.imode == CW || B.imode == CWR) {
		cmd = "FW;";
		waitN(7, 100, "get FW", ASC);
		p = replystr.rfind("FW");
		if (p != string::npos) {
			for (i = 0; i < 11; i++)
				if (replystr.find(TS2000_CWbw[i]) == p)
					break;
			if (i == 11) i = 10;
			B.iBW = i;
		}
	} else if (B.imode == FSK || B.imode == FSKR) {
		cmd = "FW;";
		waitN(7, 100, "get FW", ASC);
		p = replystr.rfind("FW");
		if (p != string::npos) {
			for (i = 0; i < 4; i++)
				if (replystr.find(TS2000_FSKbw[i]) == p)
					break;
			if (i == 4) i = 3;
			B.iBW = i;
		}
	}
	return B.iBW;
}

int RIG_TS2000::get_modetype(int n)
{
	return TS2000_mode_type[n];
}

// val 0 .. 100
void RIG_TS2000::set_mic_gain(int val)
{
	cmd = "MG";
	cmd.append(to_decimal(val,3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set mic", cmd, replystr);
}

int RIG_TS2000::get_mic_gain()
{
	cmd = "MG;";
	waitN(6, 100, "get mic", ASC);
	size_t p = replystr.rfind("MG");
	if (p != string::npos)
		return fm_decimal(&replystr[p+2], 3);
	return 0;
}

void RIG_TS2000::get_mic_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_TS2000::set_noise(bool b)
{
	if (b)
		cmd = "NB1;";
	else
		cmd = "NB0;";
	sendCommand(cmd,0);
	showresp(WARN, ASC, "set NB", cmd, replystr);
}

int RIG_TS2000::get_noise()
{
	cmd = "NB;";
	waitN(4, 100, "get Noise Blanker", ASC);
	size_t p = replystr.rfind("NB");
	if (p == string::npos) return 0;
	if (replystr[p+2] == '0') return 0;
	return 1;
}

//======================================================================
// IF shift only available if the transceiver is in the CW mode
// step size is 50 Hz
//======================================================================

void RIG_TS2000::set_if_shift(int val)
{
	bool cw = (useB && (B.imode == 2 || B.imode == 6)) ||
	          (!useB && (A.imode == 2 || A.imode == 6));
	if (!cw) {
		setIFshiftButton((void*)0);
		return;
	}

	cmd = "IS ";
	cmd.append(to_decimal(val,4)).append(";");
	sendCommand(cmd,0);
	showresp(WARN, ASC, "set IF shift", cmd, replystr);
}

bool RIG_TS2000::get_if_shift(int &val)
{
	cmd = "IS;";
	waitN(8, 100, "get IF shift", ASC);
	size_t p = replystr.rfind("IS ");
	if (p != string::npos) {
		val = fm_decimal(&replystr[p+3], 4);
		return true;
	}
	val = if_shift_mid;
	return false;
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
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set notch on", cmd, replystr);
		cmd = "BP";
		cmd.append(to_decimal(val, 3)).append(";");
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set notch val", cmd, replystr);
	} else {
		cmd = "BC0;"; // no notch action
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set notch off", cmd, replystr);
	}
}

bool  RIG_TS2000::get_notch(int &val)
{
	bool ison = false;
	cmd = "BC;";
	waitN(4, 100, "get notch on/off", ASC);
	size_t p = replystr.rfind("BC");
	if (p != string::npos) {
		if (replystr[p+2] == '2') {
			ison = true;
			cmd = "BP;";
			waitN(6, 100, "get notch val", ASC);
			p = replystr.rfind("BP");
			if (p != string::npos)
				val = fm_decimal(&replystr[p+2],3);
		}
	}
	return ison;
}

void RIG_TS2000::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 63;
	step = 1;
}

void RIG_TS2000::set_auto_notch(int v)
{
	cmd = v ? "NT1;" : "NT0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set auto notch", cmd, replystr);
}

int  RIG_TS2000::get_auto_notch()
{
	cmd = "NT;";
	waitN(4, 100, "get auto notch", ASC);
	size_t p = replystr.rfind("NT");
	if (p != string::npos)
		return (replystr[p+2] == '1');
	return 0;
}

void RIG_TS2000::set_rf_gain(int val)
{
	cmd = "RG";
	cmd.append(to_decimal(val * 255 / 100, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set rf gain", cmd, replystr);
}

int  RIG_TS2000::get_rf_gain()
{
	cmd = "RG;";
	waitN(6, 100, "get rf gain", ASC);
	size_t p = replystr.rfind("RG");
	if (p != string::npos)
		return fm_decimal(&replystr[p+2] ,3) * 100 / 255;
	return 100;
}

