/*
 * Kenwood TS480HX driver
 * originally based on Kenwood TS480HX driver
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */


#include "TS480HX.h"
#include "support.h"

static const char TS480HXname_[] = "TS-480HX";

static const char *TS480HXmodes_[] = {
		"LSB", "USB", "CW", "FM", "AM", "FSK", "CW-R", "FSK-R", NULL};
static const char TS480HX_mode_chr[] =  { '1', '2', '3', '4', '5', '6', '7', '9' };
static const char TS480HX_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'L', 'L', 'U' };

static const char *TS480HX_empty[] = { "N/A", NULL };

static const char *TS480HX_lo[] = {
 "10",   "50", "100", "200", "300", 
"400",  "500", "600", "700", "800", 
"900", "1000",
NULL };

static const char *TS480HX_hi[] = {
"1400", "1600", "1800", "2000", "2200", 
"2400", "2600", "2800", "3000", "3400", 
"4000", "5000",
NULL };

static const char *TS480HX_AM_lo[] = {
"10", "100", "200", "500",
NULL };

static const char *TS480HX_AM_hi[] = {
"2500", "3000", "4000", "5000",
NULL };

static const char *TS480HX_CWwidths[] = {
"50", "80", "100", "150", "200", 
"300", "400", "500", "600", "1000", 
"2000", NULL};

static const char *TS480HX_CWbw[] = {
"FW0050;", "FW0080;", "FW0100;", "FW0150;", "FW0200;", 
"FW0300;", "FW0400;", "FW0500;", "FW0600;", "FW1000;", 
"FW2000;" };

static const char *TS480HX_FSKwidths[] = {
"250", "500", "1000", "1500", NULL};
static const char *TS480HX_FSKbw[] = {
"FW0250;", "FW0500;", "FW1000;", "FW1500;" };

RIG_TS480HX::RIG_TS480HX() {
// base class values
	name_ = TS480HXname_;
	modes_ = TS480HXmodes_;
	_mode_type = TS480HX_mode_type;
	bandwidths_ = TS480HX_empty;
	dsp_lo = TS480HX_lo;
	dsp_hi = TS480HX_hi;
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

	has_noise_control =
	has_micgain_control =
	has_preamp_control =
	has_notch_control =
	has_ifshift_control =
	has_swr_control = false;

	has_dsp_controls =
	has_smeter =
	has_swr_control =
	has_attenuator_control =
	has_mode_control =
	has_bandwidth_control =
	has_volume_control =
	has_power_control =
	has_tune_control = 
	has_ptt_control = true;
}

void RIG_TS480HX::initialize()
{
	cmd = "FR0;"; sendCommand(cmd, 0);
	cmd = "FT0;"; sendCommand(cmd, 0);
	cmd = "AC001;"; sendCommand(cmd, 0);
	get_preamp();
	get_attenuator();
}

void RIG_TS480HX::selectA()
{
	cmd = "FR0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Rx A", cmd, replystr);
	cmd = "FT0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Tx A", cmd, replystr);
}

void RIG_TS480HX::selectB()
{
	cmd = "FR1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Rx B", cmd, replystr);
	cmd = "FT1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Tx B", cmd, replystr);
}

void RIG_TS480HX::set_split(bool val) 
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

bool RIG_TS480HX::get_split()
{
	cmd = "IF;";
	int ret = sendCommand(cmd);
	showresp(INFO, ASC, "get info", cmd, replystr);
	if (ret < 38) return split;
	size_t p = replystr.rfind("IF");
	if (p == string::npos) return split;
	split = replystr[p+32] ? true : false;
	return split;
}

long RIG_TS480HX::get_vfoA ()
{
	cmd = "FA;";
	sendCommand(cmd);
	showresp(WARN, ASC, "get vfo A", cmd, replystr);
	size_t p = replystr.rfind("FA");
	if (p != string::npos && (p + 12 < replystr.length())) {
		int f = 0;
		for (size_t n = 2; n < 13; n++)
			f = f*10 + replystr[p+n] - '0';
		A.freq = f;
	}
	return A.freq;
}

void RIG_TS480HX::set_vfoA (long freq)
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

long RIG_TS480HX::get_vfoB ()
{
	cmd = "FB;";
	sendCommand(cmd);
	showresp(WARN, ASC, "get vfo B", cmd, replystr);
	size_t p = replystr.rfind("FB");
	if (p != string::npos && (p + 12 < replystr.length())) {
		int f = 0;
		for (size_t n = 2; n < 13; n++)
			f = f*10 + replystr[p+n] - '0';
		B.freq = f;
	}
	return B.freq;
}

void RIG_TS480HX::set_vfoB (long freq)
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

// SM cmd 0 ... 100 (rig values 0 ... 15)
int RIG_TS480HX::get_smeter()
{
	cmd = "SM0;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get s-meter", cmd, replystr);
	if (ret < 8) return 0;

	size_t p = replystr.rfind("SM");
	if (p == string::npos) return 0;

	replystr[p + 7] = 0;
	int mtr = atoi(&replystr[p + 3]);
	mtr = (mtr * 100) / 20;
	return mtr;
}

// RM cmd 0 ... 100 (rig values 0 ... 8)
int RIG_TS480HX::get_swr()
{
	return -1; // disable to see if beeps go away
	int mtr = 0;
	cmd = "RM1;"; // select measurement '1' (swr) and read meter
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get swr", cmd, replystr);
	if (ret < 8) return mtr;
	size_t p = replystr.rfind("RM");
	if (p == string::npos) return mtr;

	replystr[p + 7] = 0;
	mtr = atoi(&replystr[p + 4]);
	mtr *= 10;

	return mtr;
}


// Tranceiver PTT on/off
void RIG_TS480HX::set_PTT_control(int val)
{
	if (val)	cmd = "TX1;"; // DTS transmission using ANI input
	else	 	cmd = "RX;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set PTT", cmd, replystr);
}

int RIG_TS480HX::set_widths(int val)
{
	int bw;
	if (val == 0 || val == 1 || val == 3) {
		bandwidths_ = TS480HX_empty;
		dsp_lo = TS480HX_lo;
		dsp_hi = TS480HX_hi;
		bw = 0x8803; // 200 ... 3000 Hz
	} else if (val == 2 || val == 6) {
		bandwidths_ = TS480HX_CWwidths;
		dsp_lo = TS480HX_empty;
		dsp_hi = TS480HX_empty;
		bw = 7;
	} else if (val == 5 || val == 7) {
		bandwidths_ = TS480HX_FSKwidths;
		dsp_lo = TS480HX_empty;
		dsp_hi = TS480HX_empty;
		bw = 1;
	} else { // val == 4 ==> AM
		bandwidths_ = TS480HX_empty;
		dsp_lo = TS480HX_AM_lo;
		dsp_hi = TS480HX_AM_hi;
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

void RIG_TS480HX::set_modeA(int val)
{
	A.imode = val;
	cmd = "MD";
	cmd += TS480HX_mode_chr[val];
	cmd += ';';
	sendCommand(cmd,0);
	showresp(WARN, ASC, "set mode", cmd, replystr);
	A.iBW = set_widths(val);
}

int RIG_TS480HX::get_modeA()
{
	cmd = "MD;";
	sendCommand(cmd);
	showresp(WARN, ASC, "get mode A", cmd, replystr);
	size_t p = replystr.rfind("MD");
	if (p != string::npos && (p + 2 < replystr.length())) {
		int md = replystr[p+2];
		md = md - '1';
		if (md == 8) md = 7;
		A.imode = md;
		A.iBW = set_widths(A.imode);
	}
	return A.imode;
}

void RIG_TS480HX::set_modeB(int val)
{
	B.imode = val;
	cmd = "MD";
	cmd += TS480HX_mode_chr[val];
	cmd += ';';
	sendCommand(cmd, 0);
	showresp(WARN, ASC, "set mode B", cmd, replystr);
	B.iBW = set_widths(val);
}

int RIG_TS480HX::get_modeB()
{
	cmd = "MD;";
	sendCommand(cmd);
	showresp(WARN, ASC, "get mode B", cmd, replystr);
	size_t p = replystr.rfind("MD");
	if (p != string::npos && (p + 2 < replystr.length())) {
		int md = replystr[p+2];
		md = md - '1';
		if (md == 8) md = 7;
		B.imode = md;
		B.iBW = set_widths(B.imode);
	}
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
		cmd = "FW0001;"; // wide filter 
		showresp(WARN, ASC, "wide filter", cmd, replystr);
		cmd = "SL";
		cmd.append(to_decimal(A.iBW & 0xFF, 2)).append(";");
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set lower", cmd, replystr);
		cmd = "SH";
		cmd.append(to_decimal(((A.iBW >> 8) & 0x7F), 2)).append(";");
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set upper", cmd, replystr);
	}
	if (val > 256) return;
	else if (A.imode == 2 || A.imode == 6) {
		A.iBW = val;
		cmd = TS480HX_CWbw[A.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set CW bw", cmd, replystr);
	}else if (A.imode == 5 || A.imode == 7) {
		A.iBW = val;
		cmd = TS480HX_FSKbw[A.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set FSK bw", cmd, replystr);
	}
}

int RIG_TS480HX::get_bwA()
{
	int i = 0;
	size_t p;
	if (A.imode == 0 || A.imode == 1 || A.imode == 3 || A.imode == 4) {
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
	} else if (A.imode == 2 || A.imode == 6) {
		cmd = "FW;";
		sendCommand(cmd);
		showresp(WARN, ASC, "get FW", cmd, replystr);
		p = replystr.rfind("FW");
		if (p != string::npos) {
			for (i = 0; i < 11; i++)
				if (replystr.find(TS480HX_CWbw[i]) == p+2)
					break;
			if (i == 11) i = 10;
			A.iBW = i;
		}
	} else if (A.imode == 5 || A.imode == 7) {
		cmd = "FW;";
		sendCommand(cmd);
		showresp(WARN, ASC, "get FW", cmd, replystr);
		p = replystr.rfind("FW");
		if (p != string::npos) {
			for (i = 0; i < 4; i++)
				if (replystr.find(TS480HX_FSKbw[i]) == p+2)
					break;
			if (i == 4) i = 3;
			A.iBW = i;
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
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set lower", cmd, replystr);
		cmd = "SH";
		cmd.append(to_decimal(((B.iBW >> 8) & 0x7F), 2)).append(";");
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set upper", cmd, replystr);
	}
	if (val > 256) return;
	else if (B.imode == 2 || B.imode == 6) { // CW
		B.iBW = val;
		cmd = TS480HX_CWbw[B.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set CW bw", cmd, replystr);
	}else if (B.imode == 5 || B.imode == 7) {
		B.iBW = val;
		cmd = TS480HX_FSKbw[B.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set FSK bw", cmd, replystr);
	}
}

int RIG_TS480HX::get_bwB()
{
	int i = 0;
	size_t p;
	if (B.imode == 0 || B.imode == 1 || B.imode == 3 || B.imode == 4) {
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
	} else if (B.imode == 2 || B.imode == 6) {
		cmd = "FW;";
		sendCommand(cmd);
		showresp(WARN, ASC, "get FW", cmd, replystr);
		p = replystr.rfind("FW");
		if (p != string::npos) {
			for (i = 0; i < 11; i++)
				if (replystr.find(TS480HX_CWbw[i]) == p+2)
					break;
			if (i == 11) i = 10;
			B.iBW = i;
		}
	} else if (B.imode == 5 || B.imode == 7) {
		cmd = "FW;";
		showresp(WARN, ASC, "get FW", cmd, replystr);
		p = replystr.rfind("FW");
		if (p != string::npos) {
			for (i = 0; i < 4; i++)
				if (replystr.find(TS480HX_FSKbw[i]) == p+2)
					break;
			if (i == 4) i = 3;
			B.iBW = i;
		}
	}
	return B.iBW;
}

int RIG_TS480HX::adjust_bandwidth(int val)
{
	int bw = 0;
	if (val == 0 || val == 1 || val == 3)
		bw = 0x8803;
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

void RIG_TS480HX::set_volume_control(int val)
{
	cmd = "AG";
	char szval[5];
	snprintf(szval, sizeof(szval), "%04d", val * 255 / 100);
	cmd += szval;
	cmd += ';';
	sendCommand(cmd, 0);
	showresp(WARN, ASC, "set vol", cmd, replystr);
}

int RIG_TS480HX::get_volume_control()
{
	int val = 0;
	cmd = "AG0";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get vol", cmd, replystr);
	if (ret < 7) return val;
	size_t p = replystr.rfind("AG");
	if (p == string::npos) return val;

	replystr[p + 6] = 0;
	val = atoi(&replystr[p + 3]);
	val = val * 100 / 255;

	return val;
}

void RIG_TS480HX::set_power_control(double val)
{
	cmd = "PC";
	char szval[4];
	if (modeA == 4 && val > 50) val = 50; // AM mode limitation
	snprintf(szval, sizeof(szval), "%03d", (int)val);
	cmd += szval;
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "set power", cmd, replystr);
}

int RIG_TS480HX::get_power_control()
{
	int val = 5;
	cmd = "PC;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get power", cmd, replystr);
	if (ret < 6) return val;
	size_t p = replystr.rfind("PC");
	if (p == string::npos) return val;

	replystr[p + 5] = 0;
	val = atoi(&replystr[p + 2]);

	return val;
}

void RIG_TS480HX::set_attenuator(int val)
{
	if (val)	cmd = "RA01";
	else		cmd = "RA00";
	sendCommand(cmd);
	showresp(WARN, ASC, "set att", cmd, replystr);
}

int RIG_TS480HX::get_attenuator()
{
	cmd = "RA;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get att", cmd, replystr);
	if (ret < 7) return 0;
	size_t p = replystr.rfind("RA");
	if (p == string::npos) return 0;
	
	return replystr[p + 3] - '0';

}

void RIG_TS480HX::tune_rig()
{
	cmd = "AC111;";
	sendCommand(cmd);
	showresp(WARN, ASC, "tune", cmd, replystr);
}

