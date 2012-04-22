/*
 * Kenwood TS590S driver
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */

#include "config.h"

#include "TS590S.h"
#include "support.h"

enum TS590MODES { tsLSB, tsUSB, tsCW, tsFM, tsAM, tsFSK, tsCWR, tsFSKR, tsLSBD, tsUSBD, tsFMD };

static const char TS590Sname_[] = "TS-590S";

static const char *TS590Smodes_[] = {
		"LSB", "USB", "CW", "FM", "AM", "FSK", "CW-R", "FSK-R", "LSB-D", "USB-D", "FM-D", NULL};

static const char TS590S_mode_chr[] =  { '1', '2', '3', '4', '5', '6', '7', '9', '1', '2', '4' };
static const char TS590S_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'L', 'L', 'U', 'L', 'U', 'U' };

//----------------------------------------------------------------------
static const char *TS590S_empty[] = { "N/A", NULL };

//----------------------------------------------------------------------
static const char *TS590S_SSB_lo[] = {
  "0",   "50", "100", "200", "300", 
"400",  "500", "600", "700", "800", 
"900", "1000", NULL };

static const char *TS590S_CAT_ssb_lo[] = {
"SL00;", "SL01;", "SL02;", "SL03;", "SL04;", 
"SL05;", "SL06;", "SL07;", "SL08;", "SL09;",
"SL10;", "SL11;", NULL };
static const char *TS590S_SSB_lo_tooltip = "lo cutoff";
static const char *TS590S_SSB_btn_lo_label = "L";

static const char *TS590S_SSB_hi[] = {
"1000", "1200", "1400", "1600", "1800", 
"2000", "2200", "2400", "2600", "2800", 
"3000", "3400", "4000", "5000", NULL };

static const char *TS590S_CAT_ssb_hi[] = {
"SH00;", "SH01;", "SH02;", "SH03;", "SH04;", 
"SH05;", "SH06;", "SH07;", "SH08;", "SH09;",
"SH10;", "SH11;", "SH12;", "SH13;", NULL };
static const char *TS590S_SSB_hi_tooltip = "hi cutoff";
static const char *TS590S_SSB_btn_hi_label = "H";

//----------------------------------------------------------------------
static const char *TS590S_DATA_width[] = {
  "50",   "80",  "100",  "150", "200", 
 "250",  "300",  "400",  "500", "600", 
"1000", "1500", "2000", "2500",  NULL };

static const char *TS590S_CAT_data_width[] = {
"SL00;", "SL01;", "SL02;", "SL03;", "SL04;", 
"SL05;", "SL06;", "SL07;", "SL08;", "SL09;",
"SL10;", "SL11;", "SL12;", "SL13;", NULL };
static const char *TS590S_DATA_W_tooltip = "filter width";
static const char *TS590S_DATA_W_btn_label = "W";

static const char *TS590S_DATA_shift[] = {
"1000", "1100", "1200", "1300", "1400", 
"1500", "1600", "1700", "1800", "1900", 
"2000", "2100", "2210", NULL };

static const char *TS590S_CAT_data_shift[] = {
"SH00;", "SH01;", "SH02;", "SH03;", "SH04;", 
"SH05;", "SH06;", "SH07;", "SH08;", "SH09;",
"SH10;", "SH11;", "SH12;", NULL };
static const char *TS590S_DATA_S_tooltip = "shift frequency";
static const char *TS590S_DATA_S_btn_label = "S";

//----------------------------------------------------------------------
static const char *TS590S_AM_lo[] = {
"10", "100", "200", "500", NULL };

static const char *TS590S_CAT_am_lo[] = {
"SL00;", "SL01;", "SL02;", "SL03;", NULL}; 
static const char *TS590S_AM_lo_tooltip = "lo cutoff";
static const char *TS590S_AM_btn_lo_label = "L";

static const char *TS590S_AM_hi[] = {
"2500", "3000", "4000", "5000", NULL };

static const char *TS590S_CAT_am_hi[] = {
"SH00;", "SH01;", "SH02;", "SH03;", NULL}; 
static const char *TS590S_AM_hi_tooltip = "hi cutoff";
static const char *TS590S_AM_btn_hi_label = "H";

//----------------------------------------------------------------------
static const char *TS590S_CWwidths[] = {
  "50",   "80",  "100",  "150", "200", 
 "250",  "300",  "400",  "500", "600", 
"1000", "1500", "2000", "2500",  NULL};

static const char *TS590S_CWbw[] = {
"FW0050;", "FW0080;", "FW0100;", "FW0150;", "FW0200;",
"FW0250;", "FW0300;", "FW0400;", "FW0500;", "FW0600;", 
"FW1000;", "FW1500;", "FW2000;", "FW2500;", NULL};

//----------------------------------------------------------------------
static const char *TS590S_FSKwidths[] = {
"250", "500", "1000", "1500", NULL};

static const char *TS590S_FSKbw[] = {
"FW0250;", "FW0500;", "FW1000;", "FW1500;", NULL };

//----------------------------------------------------------------------

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 },
	{ (Fl_Widget *)sldrRFGAIN,   54, 145, 156 },
	{ (Fl_Widget *)btnIFsh,     214, 105,  50 },
	{ (Fl_Widget *)sldrIFSHIFT, 266, 105, 156 },
	{ (Fl_Widget *)btnNotch,    214, 125,  50 },
	{ (Fl_Widget *)sldrNOTCH,   266, 125, 156 },
	{ (Fl_Widget *)sldrMICGAIN, 266, 145, 156 },
	{ (Fl_Widget *)sldrPOWER,   266, 165, 156 },
	{ (Fl_Widget *)btnNR,         2, 165,  50 },
	{ (Fl_Widget *)sldrNR,       54, 165, 156 },
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

void RIG_TS590S::initialize()
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
	rig_widgets[9].W = btnNR;
	rig_widgets[10].W = sldrNR;

	selectA();
	cmd = "AC000;"; sendCommand(cmd);
	get_preamp();
	get_attenuator();
	RIG_DEBUG = true;
}


RIG_TS590S::RIG_TS590S() {

	name_ = TS590Sname_;
	modes_ = TS590Smodes_;
	bandwidths_ = TS590S_empty;

	dsp_lo     = TS590S_SSB_lo;
	lo_tooltip = TS590S_SSB_lo_tooltip;
	lo_label   = TS590S_SSB_btn_lo_label;

	dsp_hi     = TS590S_SSB_hi;
	hi_tooltip = TS590S_SSB_hi_tooltip;
	hi_label   = TS590S_SSB_btn_hi_label;

	widgets = rig_widgets;

	comm_baudrate = BR115200;
	stopbits = 1;
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
	B.iBW = A.iBW = 0x8A03;
	B.freq = A.freq = 14070000;
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

}

const char * RIG_TS590S::get_bwname_(int n, int md) 
{
	static char bwname[20];
	if (n > 256) {
		int hi = (n >> 8) & 0x7F;
		int lo = n & 0xFF;
		snprintf(bwname, sizeof(bwname), "%s/%s",
			(md == 0 || md == 1 || md == 3) ? TS590S_SSB_lo[lo] :
			(md == 4) ? TS590S_AM_lo[lo] :
			TS590S_DATA_shift[lo],
			(md == 0 || md == 1 || md == 3) ? TS590S_SSB_hi[hi] :
			(md == 4) ? TS590S_AM_hi[hi] :
			TS590S_DATA_width[hi] );
	} else {
		snprintf(bwname, sizeof(bwname), "%s",
			(md == 2 || md == 6) ? TS590S_CWwidths[n] : TS590S_FSKwidths[n]);
	}
	return bwname;
}

void RIG_TS590S::selectA()
{
	cmd = "FR0;";
	sendCommand(cmd, 0);
	showresp(WARN, ASC, "Rx A", cmd, replystr);
	cmd = "FT0;";
	sendCommand(cmd, 0);
	showresp(WARN, ASC, "Tx A", cmd, replystr);
	rxtxa = true;
}

void RIG_TS590S::selectB()
{
	cmd = "FR1;";
	sendCommand(cmd, 0);
	showresp(WARN, ASC, "Rx B", cmd, replystr);
	cmd = "FT1;";
	sendCommand(cmd, 0);
	showresp(WARN, ASC, "Tx B", cmd, replystr);
	rxtxa = false;
}

void RIG_TS590S::set_split(bool val) 
{
	split = val;
	if (useB) {
		if (val) {
			cmd = "FR1;FT0;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on B, Tx on A", cmd, replystr);
		} else {
			cmd = "FR1;FT1;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on B, Tx on B", cmd, replystr);
		}
	} else {
		if (val) {
			cmd = "FR0;FT1;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on A, Tx on B", cmd, replystr);
		} else {
			cmd = "FR0;FT0;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on A, Tx on A", cmd, replystr);
		}
	}
	Fl::awake(highlight_vfo, (void *)0);
}

int RIG_TS590S::get_split()
{
	size_t p;
	bool split = false;
	char rx, tx;
// tx vfo
	cmd = rsp = "FT";
	cmd.append(";");
	waitN(4, 100, "get split tx vfo", ASC);
	p = replystr.rfind(rsp);
	if (p == string::npos) return split;
	tx = replystr[p+2];

// rx vfo
	cmd = rsp = "FR";
	cmd.append(";");
	waitN(4, 100, "get split rx vfo", ASC);

	p = replystr.rfind(rsp);
	if (p == string::npos) return split;
	rx = replystr[p+2];
// split test
	split = (tx == '1' ? 2 : 0) + (rx == '1' ? 1 : 0);

	return split;
}

long RIG_TS590S::get_vfoA ()
{
	cmd = "FA;";
	int ret = waitN(14, 100, "get vfoA", ASC);

	if (ret < 14) return A.freq;
	size_t p = replystr.rfind("FA");
	if (p == string::npos) return A.freq;

	long f = 0L;
	long mul = 1L;
	for (size_t n = 12; n > 1; n--) {
		f += (replystr[p + n] - '0') * mul;
		mul *= 10;
	}
	A.freq = f;
	return A.freq;
}

void RIG_TS590S::set_vfoA (long freq)
{
	A.freq = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd, 0);
	showresp(WARN, ASC, "set vfo A", cmd, replystr);
}

long RIG_TS590S::get_vfoB ()
{
	cmd = "FB;";
	int ret = waitN(14, 100, "get vfoB", ASC);

	if (ret < 14) return B.freq;
	size_t p = replystr.rfind("FB");
	if (p == string::npos) return B.freq;

	long f = 0L;
	long mul = 1L;
	for (size_t n = 12; n > 1; n--) {
		f += (replystr[p + n] - '0') * mul;
		mul *= 10;
	}
	B.freq = f;

	return B.freq;
}

void RIG_TS590S::set_vfoB (long freq)
{
	B.freq = freq;
	cmd = "FB00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd, 0);
	showresp(WARN, ASC, "set vfo B", cmd, replystr);
}

int RIG_TS590S::get_smeter()
{
	int mtr = 0;
	cmd = "SM0;";
	int ret = waitN(8, 100, "get", ASC);
	if (ret < 8) return 0;
	size_t p = replystr.find("SM0");
	if (p == string::npos) return 0;

	replystr[p + 7] = 0;
	mtr = atoi(&replystr[p + 3]);
	mtr *= 50;
	mtr /= 15;
	if (mtr > 100) mtr = 100;
	return mtr;
}

int RIG_TS590S::get_power_out()
{
	int mtr = 0;
	cmd = "SM0;";
	int ret = waitN(8, 100, "get power", ASC);

	if (ret < 8) return mtr;
	size_t p = replystr.rfind("SM0");
	if (p == string::npos) return mtr;

	mtr = atoi(&replystr[p + 3]);
	mtr *= 50;
	mtr /= 18;
	if (mtr > 100) mtr = 100;

	return mtr;
}

static bool read_alc = false;
static int alc_val = 0;

int RIG_TS590S::get_swr(void)
{
	int mtr = 0;

	read_alc = false;

	cmd = "RM;";
	int ret = waitN(8, 100, "get swr/alc", ASC);
	if (ret < 24) return 0;

	size_t p = replystr.find("RM3");
	if (p != string::npos) {
		replystr[p + 7] = 0;
		alc_val = atoi(&replystr[p + 3]);
		alc_val *= 100;
		alc_val /= 15;
		if (alc_val > 100) alc_val = 100;
		read_alc = true;
	}

	p = replystr.find("RM1");
	if (p == string::npos) return 0;

	replystr[p + 7] = 0;
	mtr = atoi(&replystr[p + 3]);
	mtr *= 50;
	mtr /= 15;
	if (mtr > 100) mtr = 100;

	return mtr;
}

int RIG_TS590S::get_alc(void)
{
	if (read_alc) {
		read_alc = false;
		return alc_val;
	}
	cmd = "RM;";
	int ret = waitN(8, 100, "get alc", ASC);
	if (ret < 24) return 0;

	size_t p = replystr.find("RM3");
	if (p == string::npos) return 0;

	replystr[p + 7] = 0;
	alc_val = atoi(&replystr[p + 3]);
	alc_val *= 100;
	alc_val /= 15;
	if (alc_val > 100) alc_val = 100;
	return alc_val;
}

// Transceiver power level
void RIG_TS590S::set_power_control(double val)
{
	int ival = (int)val;
	cmd = "PC000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ival % 10;
		ival /= 10;
	}
	sendCommand(cmd, 0);
}

int RIG_TS590S::get_power_control()
{
	cmd = "PC;";
	int ret = waitN(6, 100, "get pwr ctrl", ASC);

	if (ret < 6) return 0;
	size_t p = replystr.rfind("PC");
	if (p == string::npos) return 0;
	int mtr = 0;
	if (ret >= 6) {
		replystr[p + 5] = 0;
		mtr = atoi(&replystr[p + 2]);
	}
	return mtr;
}

// Volume control return 0 ... 100
int RIG_TS590S::get_volume_control()
{
	cmd = "AG0;";
	int ret = waitN(7, 100, "get vol ctrl", ASC);

	if (ret < 7) return 0;
	size_t p = replystr.rfind("AG");
	if (p == string::npos) return 0;

	replystr[p + 6] = 0;
	int val = atoi(&replystr[p + 3]);
	return (int)(val / 2.55);
}

void RIG_TS590S::set_volume_control(int val) 
{
	int ivol = (int)(val * 2.55);
	cmd = "AG0000;";
	for (int i = 5; i > 2; i--) {
		cmd[i] += ivol % 10;
		ivol /= 10;
	}
	sendCommand(cmd, 0);
}

// Tranceiver PTT on/off
void RIG_TS590S::set_PTT_control(int val)
{
	if (val) cmd = "TX1;";
	else	 cmd = "RX;";
	sendCommand(cmd, 0);
}

void RIG_TS590S::tune_rig()
{
	cmd = "AC111;";
	sendCommand(cmd, 0);
}

void RIG_TS590S::set_attenuator(int val)
{
	att_level = val;
	if (val) cmd = "RA01;";
	else     cmd = "RA00;";
	sendCommand(cmd, 0);
}

int RIG_TS590S::get_attenuator()
{
	cmd = "RA;";
	int ret = waitN(7, 100, "get att", ASC);

	if (ret < 7) return 0;
	size_t p = replystr.rfind("RA");
	if (p == string::npos) return 0;

	if (replystr[p + 2] == '0' && 
		replystr[p + 3] == '0')
		att_level = 0;
	else
		att_level = 1;

	return att_level;
}

void RIG_TS590S::set_preamp(int val)
{
	preamp_level = val;
	if (val) cmd = "PA1;";
	else     cmd = "PA0;";
	sendCommand(cmd, 0);
}

int RIG_TS590S::get_preamp()
{
	cmd = "PA;";
	int ret = waitN(5, 100, "get preamp", ASC);

	if (ret < 5) return 0;
	size_t p = replystr.rfind("PA");
	if (p == string::npos) return 0;

	if (replystr[p  + 2] == '1') 
		preamp_level = 1;
	else
		preamp_level = 0;
	return preamp_level;
}

//======================================================================
// mode commands
//======================================================================

void RIG_TS590S::set_modeA(int val)
{
	active_mode = A.imode = val;
	cmd = "MD";
	cmd += TS590S_mode_chr[val];
	cmd += ';';
	sendCommand(cmd, 0);
	showresp(WARN, ASC, "set mode A", cmd, replystr);
	if (val > tsFSKR) {
		data_mode = true;
		cmd = "DA1;";
		sendCommand(cmd, 0);
		showresp(WARN, ASC, "set data A", cmd, replystr);
	} else {
		data_mode = false;
		cmd = "DA0;";
		sendCommand(cmd, 0);
		showresp(WARN, ASC, "set data A", cmd, replystr);
	}

	set_widths(val);
}

int RIG_TS590S::get_modeA()
{
	int md = A.imode;
	cmd = "MD;";
	int ret = waitN(4, 100, "get mode A", ASC);

	if (ret < 4) return A.imode;
	size_t p = replystr.rfind("MD");
	if (p == string::npos) return A.imode;

	switch (replystr[p + 2]) {
		case '1' : md = tsLSB; break;
		case '2' : md = tsUSB; break;
		case '3' : md = tsCW; break;
		case '4' : md = tsAM; break;
		case '5' : md = tsFM; break;
		case '6' : md = tsFSK; break;
		case '7' : md = tsCWR; break;
		case '9' : md = tsFSKR; break;
		default : md = A.imode;
	}

	if (md == tsLSB || md == tsUSB || md == tsFM) {
		cmd = "DA;";
		ret = waitN(4, 100, "get data A", ASC);

		if (ret < 4) return A.imode;
		p = replystr.rfind("DA");
		if (p == string::npos) return A.imode;
		if (replystr[p + 2] == '1') {
			data_mode = true;
			if (md == tsLSB) md = tsLSBD;
			else if (md == tsUSB) md = tsUSBD;
			else if (md == tsFM) md = tsFMD;
		}
	}
	if (md != A.imode) {
		active_mode = A.imode = md;
		set_widths(md);
	}
	return A.imode;
}

void RIG_TS590S::set_modeB(int val)
{
	active_mode = B.imode = val;
	cmd = "MD";
	cmd += TS590S_mode_chr[val];
	cmd += ';';
	sendCommand(cmd, 0);
	showresp(WARN, ASC, "set mode B", cmd, replystr);
	if (val > tsFSKR) {
		data_mode = true;
		cmd = "DA1;";
		sendCommand(cmd, 0);
		showresp(WARN, ASC, "set data B", cmd, replystr);
	} else if (val != tsAM && val != tsFSK && val != tsFSKR) {
		cmd = "DA0;";
		sendCommand(cmd, 0);
		showresp(WARN, ASC, "set data B", cmd, replystr);
	}

	set_widths(val);
}

int RIG_TS590S::get_modeB()
{
	int md = B.imode;
	cmd = "MD;";
	int ret = waitN(4, 100, "get mode B", ASC);

	if (ret < 4) return B.imode;
	size_t p = replystr.rfind("MD");
	if (p == string::npos) return B.imode;

	switch (replystr[p + 2]) {
		case '1' : md = tsLSB; break;
		case '2' : md = tsUSB; break;
		case '3' : md = tsCW; break;
		case '4' : md = tsAM; break;
		case '5' : md = tsFM; break;
		case '6' : md = tsFSK; break;
		case '7' : md = tsCWR; break;
		case '9' : md = tsFSKR; break;
		default : md = B.imode;
	}

	if (md == tsLSB || md == tsUSB || md == tsFM) {
		cmd = "DA;";
		ret = waitN(4, 100, "get dat B", ASC);

		if (ret < 4) return B.imode;
		p = replystr.rfind("DA");
		if (p == string::npos) return B.imode;
		if (replystr[p + 2] == '1') {
			data_mode = true;
			if (md == tsLSB) md = tsLSBD;
			else if (md == tsUSB) md = tsUSBD;
			else if (md == tsFM) md = tsFMD;
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

int RIG_TS590S::set_widths(int val)
{
	int bw = 0;
	if (val == tsLSB || val == tsUSB || val == tsFM || val == tsFMD) {
		bandwidths_ = TS590S_empty;
		dsp_lo = TS590S_SSB_lo;
		dsp_hi = TS590S_SSB_hi;
		lo_tooltip = TS590S_SSB_lo_tooltip;
		lo_label   = TS590S_SSB_btn_lo_label;
		hi_tooltip = TS590S_SSB_hi_tooltip;
		hi_label   = TS590S_SSB_btn_hi_label;
		bw = 0x8A03; // 200 ... 3000 Hz
	} else if (val == tsCW || val == tsCWR) {
		bandwidths_ = TS590S_CWwidths;
		dsp_lo = TS590S_empty;
		dsp_hi = TS590S_empty;
		bw = 7;
	} else if (val == tsFSK || val == tsFSKR) {
		bandwidths_ = TS590S_FSKwidths;
		dsp_lo = TS590S_empty;
		dsp_hi = TS590S_empty;
		bw = 1;
	} else if (val == tsAM) { // val == 4 ==> AM
		bandwidths_ = TS590S_empty;
		dsp_lo = TS590S_AM_lo;
		dsp_hi = TS590S_AM_hi;
		lo_tooltip = TS590S_AM_lo_tooltip;
		lo_label   = TS590S_AM_btn_lo_label;
		hi_tooltip = TS590S_AM_hi_tooltip;
		hi_label   = TS590S_AM_btn_hi_label;
		bw = 0x8201;
	} else if (val == tsLSBD || val == tsUSBD) {
		bandwidths_ = TS590S_empty;
		dsp_lo = TS590S_DATA_shift;
		dsp_hi = TS590S_DATA_width;
		lo_tooltip = TS590S_DATA_S_tooltip;
		lo_label   = TS590S_DATA_S_btn_label;
		hi_tooltip = TS590S_DATA_W_tooltip;
		hi_label   = TS590S_DATA_W_btn_label;
		bw = 0x8D05;
	}
	return bw;
}

const char **RIG_TS590S::bwtable(int m)
{
	if (m == tsLSB || m == tsUSB || m == tsFM || m == tsFMD)
		return TS590S_empty;
	else if (m == tsCW || m == tsCWR)
		return TS590S_CWwidths;
	else if (m == tsFSK || m == tsFSKR)
		return TS590S_FSKwidths;
	else if (m == tsAM)
		return TS590S_empty;
	else
		return TS590S_empty;
}

int RIG_TS590S::adjust_bandwidth(int val)
{
	if (val == tsLSB || val == tsUSB || val == tsFM || val == tsFMD)
		return 0x8A03;
	else if (val == tsLSBD || val == tsUSBD)
		return 0x8D05;
	else if (val == tsCW || val == tsCWR)
		return 7;
	else if (val == tsFSK || val == tsFSKR)
		return 1;
//	else if (val == AM)
		return 0x8201;
}

int RIG_TS590S::def_bandwidth(int val)
{
	return adjust_bandwidth(val);
}

void RIG_TS590S::set_bwA(int val)
{
// LSB, USB, FM, FM-D
	if (A.imode == tsLSB || A.imode == tsUSB || A.imode == tsFM || A.imode == tsFMD) {
		if (val < 256) return;
		A.iBW = val;
		cmd = TS590S_CAT_ssb_lo[A.iBW  & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set lower", cmd, replystr);
		cmd = TS590S_CAT_ssb_hi[(A.iBW >> 8) & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set upper", cmd, replystr);
		return;
	}
// LSB-D, USB-D
	if (A.imode == tsLSBD || A.imode == tsUSBD) {
		if (val < 256) return;
		A.iBW = val;
		cmd = TS590S_CAT_data_shift[A.iBW & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set shift", cmd, replystr);
		cmd = TS590S_CAT_data_width[(A.iBW >> 8) & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set width", cmd, replystr);
		return;
	}
// AM
	if (A.imode == tsAM) {
		if (val < 256) return;
		A.iBW = val;
		cmd = TS590S_CAT_am_lo[(A.iBW >> 8) & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set lower", cmd, replystr);
		cmd = TS590S_CAT_am_hi[A.iBW & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set upper", cmd, replystr);
		return;
	}
	if (val > 256) return;
// CW
	if (A.imode == tsCW || A.imode == tsCWR) {
		A.iBW = val;
		cmd = TS590S_CWbw[A.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set CW bw", cmd, replystr);
		return;
	}
// FSK
	if (A.imode == tsFSK || A.imode == tsFSKR) {
		A.iBW = val;
		cmd = TS590S_FSKbw[A.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set FSK bw", cmd, replystr);
		return;
	}
}

void RIG_TS590S::set_bwB(int val)
{
	if (B.imode == tsLSB || B.imode == tsUSB || B.imode == tsFM || B.imode == tsFMD) {
		if (val < 256) return;
		B.iBW = val;
		cmd = TS590S_CAT_ssb_lo[B.iBW & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set lower", cmd, replystr);
		cmd = TS590S_CAT_ssb_hi[(B.iBW >> 8) & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set upper", cmd, replystr);
		return;
	}
	if (B.imode == tsLSBD || B.imode == tsUSBD) { // SSB data mode
		if (val < 256) return;
		B.iBW = val;
		cmd = TS590S_CAT_data_shift[B.iBW & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set shift", cmd, replystr);
		cmd = TS590S_CAT_data_width[(B.iBW >> 8) & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set width", cmd, replystr);
		return;
	}
	if (B.imode == tsAM) {
		if (val < 256) return;
		B.iBW = val;
		cmd = TS590S_AM_lo[(B.iBW >> 8) & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set lower", cmd, replystr);
		cmd = TS590S_AM_hi[B.iBW & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set upper", cmd, replystr);
		return;
	}

	if (val > 256) return;
	if (B.imode == tsCW || B.imode == tsCWR) {
		B.iBW = val;
		cmd = TS590S_CWbw[B.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set CW bw", cmd, replystr);
		return;
	}

	if (B.imode == tsFSK || B.imode == tsFSKR) {
		B.iBW = val;
		cmd = TS590S_FSKbw[B.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set FSK bw", cmd, replystr);
		return;
	}
}

int RIG_TS590S::get_bwA()
{
	int i = 0;
	size_t p;
	if (A.imode == tsCW || A.imode == tsCWR) {
		cmd = "FW;";
		waitN(7, 100, "get CW width", ASC);
		p = replystr.rfind("FW");
		if (p != string::npos) {
			for (i = 0; i < 14; i++)
				if (replystr.find(TS590S_CWbw[i]) == p)
					break;
			if (i == 14) i = 13;
			A.iBW = i;
		}
	} else if (A.imode == tsFSK || A.imode == tsFSKR) {
		cmd = "FW;";
		waitN(7, 100, "get FSK width", ASC);
		p = replystr.rfind("FW");
		if (p != string::npos) {
			for (i = 0; i < 4; i++)
				if (replystr.find(TS590S_FSKbw[i]) == p)
					break;
			if (i == 4) i = 3;
			A.iBW = i;
		}
	}
	else if (A.imode == tsLSB || A.imode == tsUSB || A.imode == tsFM || A.imode == tsFMD) {
		int lo = A.iBW & 0x7F, hi = (A.iBW >> 8) & 0x7F;
		cmd = "SL;";
		waitN(5, 100, "get lower", ASC);
		p = replystr.rfind("SL");
		if (p != string::npos)
			lo = fm_decimal(&replystr[2], 2);
		cmd = "SH;";
		waitN(5, 100, "get upper", ASC);
		p = replystr.rfind("SH");
		if (p != string::npos)
			hi = fm_decimal(&replystr[2], 2);
		A.iBW = ((hi << 8) | (lo & 0x7F)) | 0x8000;
	} else if (A.imode == tsLSBD || A.imode == tsUSBD) {
		int lo = A.iBW & 0x7F, hi = (A.iBW >> 8) & 0x7F;
		cmd = "SL;";
		waitN(5, 100, "get width", ASC);
		p = replystr.rfind("SL");
		if (p != string::npos)
			hi = fm_decimal(&replystr[2], 2);
		cmd = "SH;";
		waitN(5, 100, "get shift", ASC);
		p = replystr.rfind("SH");
		if (p != string::npos)
			lo = fm_decimal(&replystr[2], 2);
		A.iBW = ((hi << 8) | (lo & 0x7F)) | 0x8000;
	}
	return A.iBW;
}

int RIG_TS590S::get_bwB()
{
	int i = 0;
	size_t p;
	if (B.imode == tsCW || B.imode == tsCWR) {
		cmd = "FW;";
		waitN(7, 100, "get CW width", ASC);
		p = replystr.rfind("FW");
		if (p != string::npos) {
			for (i = 0; i < 14; i++)
				if (replystr.find(TS590S_CWbw[i]) == p)
					break;
			if (i == 14) i = 13;
			B.iBW = i;
		}
	} else if (B.imode == tsFSK || B.imode == tsFSKR) {
		cmd = "FW;";
		waitN(7, 100, "get FSK width", ASC);
		p = replystr.rfind("FW");
		if (p != string::npos) {
			for (i = 0; i < 4; i++)
				if (replystr.find(TS590S_FSKbw[i]) == p)
					break;
			if (i == 4) i = 3;
			B.iBW = i;
		}
	}
	else if (B.imode == tsLSB || B.imode == tsUSB || B.imode == tsFM || B.imode == tsFMD) {
		int lo = B.iBW & 0xFF, hi = (B.iBW >> 8) & 0x7F;
		cmd = "SL;";
		waitN(5, 100, "get lower", ASC);
		p = replystr.rfind("SL");
		if (p != string::npos)
			lo = fm_decimal(&replystr[2], 2);
		cmd = "SH;";
		waitN(5, 100, "get upper", ASC);
		p = replystr.rfind("SH");
		if (p != string::npos)
			hi = fm_decimal(&replystr[2], 2);
		B.iBW = ((hi << 8) | (lo & 0x7F)) | 0x8000;
	} else if (B.imode == tsLSBD || B.imode == tsUSBD) {
		int lo = B.iBW & 0x7F, hi = (B.iBW >> 8) & 0x7F;
		cmd = "SL;";
		waitN(5, 100, "get width", ASC);
		p = replystr.rfind("SL");
		if (p != string::npos)
			hi = fm_decimal(&replystr[2], 2);
		cmd = "SH;";
		waitN(5, 100, "get shift", ASC);
		p = replystr.rfind("SH");
		if (p != string::npos)
			lo = fm_decimal(&replystr[2], 2);
		B.iBW = ((hi << 8) | (lo & 0xFF)) | 0x8000;
	}
	return B.iBW;
}

int RIG_TS590S::get_modetype(int n)
{
	return TS590S_mode_type[n];
}

// val 0 .. 100
void RIG_TS590S::set_mic_gain(int val)
{
	cmd = "MG000;";
	for (int i = 3; i > 0; i--) {
		cmd[1+i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd, 0);
}

int RIG_TS590S::get_mic_gain()
{
	int val = 0;
	cmd = "MG;";
	int ret = waitN(6, 100, "get mic ctrl", ASC);

	if (ret >= 6) {
		size_t p = replystr.rfind("MG");
		if (p == string::npos) return val;
		replystr[p + 5] = 0;
		val = atoi(&replystr[p + 2]);
	}
	return val;
}

void RIG_TS590S::get_mic_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_TS590S::set_noise(bool val)
{
	if (nb_level == 0) {
		nb_level = 1;
		nb_label("NB 1", true);
	} else if (nb_level == 1) {
		nb_level = 2;
		nb_label("NB 2", true);
	} else if (nb_level == 2) {
		nb_level = 0;
		nb_label("NB", false);
	}
	cmd = "NB0;";
	cmd[2] += nb_level;
	LOG_INFO("%s", cmd.c_str());
	sendCommand(cmd, 0);
}

//======================================================================
// IF shift command only available if the transceiver is in the CW mode
// step size is 50 Hz
//======================================================================
void RIG_TS590S::set_if_shift(int val)
{
	if (active_mode == tsCW || active_mode == tsCWR) { // cw modes
		progStatus.shift_val = val;
		cmd = "IS ";
		cmd.append(to_decimal(abs(val),4)).append(";");
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set IF shift", cmd, replystr);
	}
}

bool RIG_TS590S::get_if_shift(int &val)
{
	if (active_mode == tsCW || active_mode == tsCWR) { // cw modes
		cmd = "IS;";
		waitN(8, 100, "get IF shift", ASC);
		size_t p = replystr.rfind("IS");
		if (p != string::npos) {
			val = fm_decimal(&replystr[p+3], 4);
		} else
			val = progStatus.shift_val;
		return true;
	}
	val = progStatus.shift_val;
	return false;
}

void RIG_TS590S::get_if_min_max_step(int &min, int &max, int &step)
{
	if_shift_min = min = 300;
	if_shift_max = max = 1000;
	if_shift_step = step = 50;
	if_shift_mid = 800;
}

void RIG_TS590S::set_squelch(int val)
{
	cmd = "SQ0";
	cmd.append(to_decimal(abs(val),3)).append(";");
	sendCommand(cmd,0);
	showresp(WARN, ASC, "set squelch", cmd, replystr);
}

int  RIG_TS590S::get_squelch()
{
	int val = 0;
	cmd = "SQ0;";
	int ret = waitN(7, 100, "get squelch", ASC);

	if (ret >= 7) {
		size_t p = replystr.rfind("SQ0");
		if (p == string::npos) return val;
		replystr[p + 6] = 0;
		val = atoi(&replystr[p + 3]);
	}
	return val;
}

void RIG_TS590S::get_squelch_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 255; step = 1;
}

void RIG_TS590S::set_rf_gain(int val)
{
	cmd = "RG000;";
	int rfval = 255 - val;
	for (int i = 4; i > 1; i--) {
		cmd[i] = rfval % 10 + '0';
		rfval /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "SET rfgain", cmd, replystr);
}

int  RIG_TS590S::get_rf_gain()
{
	int rfval = 0;
	cmd = rsp = "RG";
	cmd += ';';
	waitN(6, 100, "get rfgain", ASC);

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return progStatus.rfgain;
	for (int i = 2; i < 5; i++) {
		rfval *= 10;
		rfval += replystr[p+i] - '0';
	}
	return 255 - rfval;
}

void RIG_TS590S::get_rf_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 255;
	step = 1;
}

static bool nr_on = false;

void RIG_TS590S::set_noise_reduction(int val)
{
	cmd.assign("NR").append(val ? "1" : "0" ).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET noise reduction", cmd, replystr);
	if (val) nr_on = true;
	else nr_on = false;
}

int  RIG_TS590S::get_noise_reduction()
{
	int val;
	cmd = rsp = "NR";
	cmd.append(";");
	waitN(4, 100, "GET noise reduction", ASC);
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	val = replystr[p+2] - '0';
	if (val == 2) nr_on = true;
	else nr_on = false;
	return val;
}

void RIG_TS590S::set_noise_reduction_val(int val)
{
	cmd.assign("RL").append(to_decimal(val, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET_noise_reduction_val", cmd, replystr);
}

int  RIG_TS590S::get_noise_reduction_val()
{
	if (!nr_on) return 0;
	int val = 1;
	cmd = rsp = "RL";
	cmd.append(";");
	waitN(5, 100, "GET noise reduction val", ASC);
	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return val;
	val = atoi(&replystr[p+2]);
	return val;
}

void RIG_TS590S::set_auto_notch(int v)
{
	cmd.assign("NT").append(v ? "1" : "0" ).append("0;");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET Auto Notch", cmd, replystr);
}

int  RIG_TS590S::get_auto_notch()
{
	cmd = "NT;";
	waitN(5, 100, "get auto notch", ASC);
	size_t p = replystr.rfind("NT");
	if (p == string::npos) return 0;
	if (replystr[p+2] == '1') return 1;
	return 0;
}

void RIG_TS590S::set_notch(bool on, int val)
{
	if (on) {
		cmd.assign("NT20;");
		sendCommand(cmd);
		showresp(WARN, ASC, "Set notch ON", cmd, replystr);
		int bp = (int)((val - 300.0) * 128.0 / 2700.0);
		cmd.assign("BP").append(to_decimal(bp, 3)).append(";");
		sendCommand(cmd);
		showresp(WARN, ASC, "set notch freq", cmd, replystr);
	} else {
		cmd.assign("NT00;");
		sendCommand(cmd);
		showresp(WARN, ASC, "Set notch OFF", cmd, replystr);
	}
}

bool  RIG_TS590S::get_notch(int &val)
{
	val = 300;
	cmd = "NT;";
	waitN(5, 100, "get notch state", ASC);
	size_t p = replystr.rfind("NT");
	if (p == string::npos)
		return 0;
	if (replystr[p+2] == '2') {
		cmd.assign("BP;");
		waitN(6, 100, "get notch freq", ASC);
		size_t p = replystr.rfind("BP");
		if (p != string::npos)
			val = (int)((atoi(&replystr[p+2]) * 2700.0 / 128.0) + 300.0);
		return 1;
	}
	return 0;
}

void RIG_TS590S::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = 300;
	max = 3000;
	step = 20;
}
