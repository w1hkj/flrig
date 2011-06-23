/*
 * Kenwood TS590S driver
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */


#include "TS590S.h"
#include "support.h"

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
"900", "1000",
NULL };

static const char *TS590S_CAT_ssb_lo[] = {
"SL00", "SL01", "SL02", "SL03", "SL04", 
"SL05", "SL06", "SL07", "SL08", "SL09",
"SL10", "SL11", NULL };

static const char *TS590S_SSB_hi[] = {
"1000", "1200", "1400", "1600", "1800", 
"2000", "2200", "2400", "2600", "2800", 
"3000", "3400", "4000", "5000", NULL };

static const char *TS590S_CAT_ssb_hi[] = {
"SH00", "SH01", "SH02", "SH03", "SH04", 
"SH05", "SH06", "SH07", "SH08", "SH09",
"SH10", "SH11", "SH12", "SH13", NULL };

//----------------------------------------------------------------------
static const char *TS590S_DATA_width[] = {
  "50",   "80",  "100",  "150", "200", 
 "250",  "300",  "400",  "500", "600", 
"1000", "1500", "2000", "2500",  NULL };

static const char *TS590S_CAT_data_width[] = {
"SL00", "SL01", "SL02", "SL03", "SL04", 
"SL05", "SL06", "SL07", "SL08", "SL09",
"SL10", "SL11", "SL12", "SL13", NULL };

static const char *TS590S_DATA_shift[] = {
"1000", "1100", "1200", "1300", "1400", 
"1500", "1600", "1700", "1800", "1900", 
"2000", "2100", "2210", NULL };

static const char *TS590S_CAT_data_shift[] = {
"SH00", "SH01", "SH02", "SH03", "SH04", 
"SH05", "SH06", "SH07", "SH08", "SH09",
"SH10", "SH11", "SH12", NULL };

//----------------------------------------------------------------------
static const char *TS590S_AM_lo[] = {
"10", "100", "200", "500",
NULL };

static const char *TS590S_CAT_am_lo[] = {
"SL00", "SL01", "SL02", "SL03", NULL}; 

static const char *TS590S_AM_hi[] = {
"2500", "3000", "4000", "5000",
NULL };

static const char *TS590S_CAT_am_hi[] = {
"SH00", "SH01", "SH02", "SH03", NULL}; 

//----------------------------------------------------------------------
static const char *TS590S_CWwidths[] = {
  "50",   "80",  "100",  "150", "200", 
 "250",  "300",  "400",  "500", "600", 
"1000", "1500", "2000", "2500",  NULL};

static const char *TS590S_CWbw[] = {
"FW0050;", "FW0080;", "FW0100;", "FW0150;", "FW0200;",
"FW0250;", "FW0300;", "FW0400;", "FW0500;", "FW0600;", 
"FW1000;", "FW1500;", "FW2000", "FW2500" };

//----------------------------------------------------------------------
static const char *TS590S_FSKwidths[] = {
"250", "500", "1000", "1500", NULL};

static const char *TS590S_FSKbw[] = {
"FW0250;", "FW0500;", "FW1000;", "FW1500;" };

//----------------------------------------------------------------------

RIG_TS590S::RIG_TS590S() {

	name_ = TS590Sname_;
	modes_ = TS590Smodes_;
	bandwidths_ = TS590S_empty;
	dsp_lo = TS590S_SSB_lo;
	dsp_hi = TS590S_SSB_hi;
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
	B.iBW = A.iBW = 0x8A03;
	B.freq = A.freq = 14070000;
	can_change_alt_vfo = true;

	nb_level = 2;

	has_micgain_control =
	has_notch_control =
	has_ifshift_control =
	has_swr_control = false;

	has_dsp_controls =
	has_smeter =
	has_power_out =
	has_split =
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

void RIG_TS590S::initialize()
{
	selectA();
	cmd = "AC000;"; sendCommand(cmd);
	get_preamp();
	get_attenuator();
	RIG_DEBUG = true;
}

void RIG_TS590S::selectA()
{
	cmd = "FR0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Rx A", cmd, replystr);
	cmd = "FT0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Tx A", cmd, replystr);
}

void RIG_TS590S::selectB()
{
	cmd = "FR1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Rx B", cmd, replystr);
	cmd = "FT1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Tx B", cmd, replystr);
}

void RIG_TS590S::set_split(bool val) 
{
	split = val;
	if (val) {
		cmd = "FR0;";
		sendCommand(cmd);
		showresp(WARN, ASC, "Rx A", cmd, replystr);
		cmd = "FT1;";
		sendCommand(cmd);
		showresp(WARN, ASC, "Tx B", cmd, replystr);
	} else {
		cmd = "FR0;";
		sendCommand(cmd);
		showresp(WARN, ASC, "Rx A", cmd, replystr);
		cmd = "FT0;";
		sendCommand(cmd);
		showresp(WARN, ASC, "Tx A", cmd, replystr);
	}
}

bool RIG_TS590S::get_split()
{
	cmd = "IF;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get info", cmd, replystr);
	if (ret < 38) return split;
	size_t p = replystr.rfind("IF");
	if (p == string::npos) return split;
	split = replystr[p+32] ? true : false;
	return split;
}

long RIG_TS590S::get_vfoA ()
{
	cmd = "FA;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get vfoA", cmd, replystr);

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
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get vfoB", cmd, replystr);

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
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get smeter", cmd, replystr);

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
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get power", cmd, replystr);

	if (ret < 8) return mtr;
	size_t p = replystr.rfind("SM");
	if (p == string::npos) return mtr;

	mtr = atoi(&replystr[p + 3]);
	mtr *= 50;
	mtr /= 18;
	if (mtr > 100) mtr = 100;

	return mtr;
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
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get pwr ctrl", cmd, replystr);

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
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get vol ctrl", cmd, replystr);

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
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get att", cmd, replystr);

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
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get preamp", cmd, replystr);

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
	if (val > 7) {
		data_mode = true;
		cmd = "DA1;";
	} else {
		data_mode = false;
		cmd = "DA0;";
	}
	sendCommand(cmd, 0);
	showresp(WARN, ASC, "set data A", cmd, replystr);

	set_widths(val);
}

int RIG_TS590S::get_modeA()
{
	int md = A.imode;
	cmd = "MD;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get mode A", cmd, replystr);

	if (ret < 4) return md;
	size_t p = replystr.rfind("MD");
	if (p == string::npos) return md;

	md = replystr[p + 2];
	md = md - '1';
	if (md == 8) md = 7;
	
	if (md == 0 || md == 1) {
		cmd = "DA;";
		ret = sendCommand(cmd);
		showresp(WARN, ASC, "get data A", cmd, replystr);

		if (ret < 4) return A.imode;
		p = replystr.rfind("DA");
		if (p == string::npos) return A.imode;
		if (replystr[p + 2] == '1') {
			data_mode = true;
			md += 8;
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
	if (val > 7) {
		data_mode = true;
		cmd = "DA1;";
	} else {
		data_mode = false;
		cmd = "DA0;";
	}
	sendCommand(cmd, 0);
	showresp(WARN, ASC, "set data B", cmd, replystr);

	set_widths(val);
}

int RIG_TS590S::get_modeB()
{
	int md = B.imode;
	cmd = "MD;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get mode B", cmd, replystr);

	if (ret < 4) return md;
	size_t p = replystr.rfind("MD");
	if (p == string::npos) return md;

	md = replystr[p + 2];
	md = md - '1';
	if (md == 8) md = 7;
	
	if (md == 0 || md == 1) {
		cmd = "DA;";
		ret = sendCommand(cmd);
		showresp(WARN, ASC, "get data B", cmd, replystr);

		if (ret < 4) return B.imode;
		p = replystr.rfind("DA");
		if (p == string::npos) return B.imode;
		if (replystr[p + 2] == '1') {
			data_mode = true;
			md += 8;
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
	if (val == 0 || val == 1 || val == 3) {
		bandwidths_ = TS590S_empty;
		dsp_lo = TS590S_SSB_lo;
		dsp_hi = TS590S_SSB_hi;
		bw = 0x8A03; // 200 ... 3000 Hz
	} else if (val == 2 || val == 6) {
		bandwidths_ = TS590S_CWwidths;
		dsp_lo = TS590S_empty;
		dsp_hi = TS590S_empty;
		bw = 7;
	} else if (val == 5 || val == 7) {
		bandwidths_ = TS590S_FSKwidths;
		dsp_lo = TS590S_empty;
		dsp_hi = TS590S_empty;
		bw = 1;
	} else if (val == 4) { // val == 4 ==> AM
		bandwidths_ = TS590S_empty;
		dsp_lo = TS590S_AM_lo;
		dsp_hi = TS590S_AM_hi;
		bw = 0x8201;
	} else if (val == 8 || val == 9 || val == 10) {
		bandwidths_ = TS590S_empty;
		dsp_lo = TS590S_DATA_shift;
		dsp_hi = TS590S_DATA_width;
		bw = 0x8D05;
	}
	return bw;
}

const char **RIG_TS590S::bwtable(int m)
{
	if (m == 0 || m == 1 || m == 3)
		return TS590S_empty;
	else if (m == 2 || m == 6)
		return TS590S_CWwidths;
	else if (m == 5 || m == 7)
		return TS590S_FSKwidths;
	else if (m == 4)
		return TS590S_empty;
	else
		return TS590S_empty;
}

int RIG_TS590S::adjust_bandwidth(int val)
{
	if (val == 0 || val == 1 || val == 3)
		return 0x8A03;
	else if (val == 8 || val == 9 || val == 10)
		return 0x8D05;
	else if (val == 2 || val == 6)
		return 7;
	else if (val == 5 || val == 7)
		return 1;
//	else if (val == 4)
		return 0x8201;
}

int RIG_TS590S::def_bandwidth(int val)
{
	return adjust_bandwidth(val);
}

void RIG_TS590S::set_bwA(int val)
{
	if (A.imode == 0 || A.imode == 1 || A.imode == 3) {
		if (val < 256) return;
		A.iBW = val;
		cmd = TS590S_CAT_ssb_lo[A.iBW & 0xFF];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set lower", cmd, replystr);
		cmd = TS590S_CAT_ssb_hi[(A.iBW >> 8) & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set upper", cmd, replystr);
		return;
	}
	if (A.imode == 8 || A.imode == 9 || A.imode == 10) {
		if (val < 256) return;
		A.iBW = val;
		cmd = TS590S_CAT_data_shift[A.iBW & 0xFF];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set shift", cmd, replystr);
		cmd = TS590S_CAT_data_width[(A.iBW >> 8) & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set width", cmd, replystr);
		return;
	}
	if (A.imode == 4) {
		if (val < 256) return;
		A.iBW = val;
		cmd = TS590S_CAT_am_lo[A.iBW & 0xFF];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set lower", cmd, replystr);
		cmd = TS590S_CAT_am_hi[(A.iBW >> 8) & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set upper", cmd, replystr);
		return;
	}

	if (val > 256) return;
	if (A.imode == 2 || A.imode == 6) {
		A.iBW = val;
		cmd = TS590S_CWbw[A.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set CW bw", cmd, replystr);
		return;
	}

	if (A.imode == 5 || A.imode == 7) {
		A.iBW = val;
		cmd = TS590S_FSKbw[A.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set FSK bw", cmd, replystr);
		return;
	}
}

void RIG_TS590S::set_bwB(int val)
{
	if (B.imode == 0 || B.imode == 1 || B.imode == 3) {
		if (val < 256) return;
		B.iBW = val;
		cmd = TS590S_CAT_ssb_lo[B.iBW & 0xFF];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set lower", cmd, replystr);
		cmd = TS590S_CAT_ssb_hi[(B.iBW >> 8) & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set upper", cmd, replystr);
		return;
	}
	if (B.imode == 8 || B.imode == 9 || B.imode == 10) {
		if (val < 256) return;
		B.iBW = val;
		cmd = TS590S_CAT_data_shift[B.iBW & 0xFF];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set shift", cmd, replystr);
		cmd = TS590S_CAT_data_width[(B.iBW >> 8) & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set width", cmd, replystr);
		return;
	}
	if (B.imode == 4) {
		if (val < 256) return;
		B.iBW = val;
		cmd = TS590S_AM_lo[B.iBW & 0xFF];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set lower", cmd, replystr);
		cmd = TS590S_AM_hi[(B.iBW >> 8) & 0x7F];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set upper", cmd, replystr);
		return;
	}

	if (val > 256) return;
	if (B.imode == 2 || B.imode == 6) {
		B.iBW = val;
		cmd = TS590S_CWbw[B.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set CW bw", cmd, replystr);
		return;
	}

	if (B.imode == 5 || B.imode == 7) {
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
	if (A.imode == 2 || A.imode == 6) {
		cmd = "FW;";
		sendCommand(cmd);
		showresp(WARN, ASC, "get CW width", cmd, replystr);
		p = replystr.rfind("FW");
		if (p != string::npos) {
			for (i = 0; i < 11; i++)
				if (replystr.find(TS590S_CWbw[i]) == p)
					break;
			if (i == 11) i = 10;
			A.iBW = i;
		}
	} else if (A.imode == 5 || A.imode == 7) {
		cmd = "FW;";
		sendCommand(cmd);
		showresp(WARN, ASC, "get FSK width", cmd, replystr);
		p = replystr.rfind("FW");
		if (p != string::npos) {
			for (i = 0; i < 4; i++)
				if (replystr.find(TS590S_FSKbw[i]) == p)
					break;
			if (i == 4) i = 3;
			A.iBW = i;
		}
	}
	else {
		int lo = A.iBW & 0xFF, hi = (A.iBW >> 8) & 0x7F;
		cmd = "SL;";
		sendCommand(cmd);
		showresp(WARN, ASC, "get SL", cmd, replystr);
		p = replystr.rfind("SL");
		if (p != string::npos)
			lo = fm_decimal(&replystr[2], 2);
		cmd = "SH;";
		sendCommand(cmd);
		showresp(WARN, ASC, "get SH", cmd, replystr);
		p = replystr.rfind("SH");
		if (p != string::npos)
			hi = fm_decimal(&replystr[2], 2);
		A.iBW = ((hi << 8) | (lo & 0xFF)) | 0x8000;
	}
	return A.iBW;
}

int RIG_TS590S::get_bwB()
{
	int i = 0;
	size_t p;
	if (B.imode == 2 || B.imode == 6) {
		cmd = "FW;";
		sendCommand(cmd);
		showresp(WARN, ASC, "get CW width", cmd, replystr);
		p = replystr.rfind("FW");
		if (p != string::npos) {
			for (i = 0; i < 11; i++)
				if (replystr.find(TS590S_CWbw[i]) == p)
					break;
			if (i == 11) i = 10;
			B.iBW = i;
		}
	} else if (B.imode == 5 || B.imode == 7) {
		cmd = "FW;";
		sendCommand(cmd);
		showresp(WARN, ASC, "get FSK width", cmd, replystr);
		p = replystr.rfind("FW");
		if (p != string::npos) {
			for (i = 0; i < 4; i++)
				if (replystr.find(TS590S_FSKbw[i]) == p)
					break;
			if (i == 4) i = 3;
			B.iBW = i;
		}
	}
	else {
		int lo = B.iBW & 0xFF, hi = (B.iBW >> 8) & 0x7F;
		cmd = "SL;";
		sendCommand(cmd);
		showresp(WARN, ASC, "get SL", cmd, replystr);
		p = replystr.rfind("SL");
		if (p != string::npos)
			lo = fm_decimal(&replystr[2], 2);
		cmd = "SH;";
		sendCommand(cmd);
		showresp(WARN, ASC, "get SH", cmd, replystr);
		p = replystr.rfind("SH");
		if (p != string::npos)
			hi = fm_decimal(&replystr[2], 2);
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
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get mic ctrl", cmd, replystr);

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
	if (active_mode == 2 || active_mode == 6) { // cw modes
		cmd = "IS 0000;";
		if (val < 0) cmd[3] = '-';
		val = abs(val);
		for (int i = 4; i > 0; i--) {
			cmd[3+i] += val % 10;
			val /= 10;
		}
		sendCommand(cmd, 0);
	}
}

bool RIG_TS590S::get_if_shift(int &val)
{
	static int oldval = 0;
	size_t p = 0;
	cmd = "IS;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get IF shift", cmd, replystr);

	if (ret >= 8) {
		p = replystr.rfind("IS");
		if (p == string::npos) return false;
		replystr[p + 7] = 0;
		val = atoi(&replystr[p + 3]);
		if (val != 0 || oldval != val) {
			oldval = val;
			return true;
		}
	}
	oldval = val;
	return false;
}

void RIG_TS590S::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -1000;
	max = 1000;
	step = 100;
}

void RIG_TS590S::set_notch(bool on, int val)
{
	cmd = "BP00000;";
	if (on == false) {
		sendCommand(cmd, 0);
		notch_on = false;
		return;
	}
	if (!notch_on) {
		cmd[6] = '1'; // notch ON
		sendCommand(cmd, 0);
		cmd[6] = '0';
		notch_on = true;
	}
	cmd[3] = '1'; // manual NOTCH position
// set notch value offset by 200, ie: 001 -> 400
	val = (-val / 9) + 200;
	if (val < 1) val = 1;
	if (val > 400) val = 400;
	for (int i = 3; i > 0; i--) {
		cmd[3 + i] += val % 10;
		val /=10;
	}
	sendCommand(cmd, 0);
}

//tbd

bool  RIG_TS590S::get_notch(int &val)
{
	return false;
//	bool ison = false;
//	cmd = "BP;";
//	int ret = sendCommand(cmd);
//	return ison;
}

void RIG_TS590S::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = -1143;
	max = +1143;
	step = 9;
}

