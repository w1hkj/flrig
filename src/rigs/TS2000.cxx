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
"900", "1000",
NULL };

static const char *TS2000_hi[] = {
"1400", "1600", "1800", "2000", "2200", 
"2400", "2600", "2800", "3000", "3400", 
"4000", "5000",
NULL };

static const char *TS2000_AM_lo[] = {
"10", "100", "200", "500",
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

RIG_TS2000::RIG_TS2000() {
// base class values	
	name_ = TS2000name_;
	modes_ = TS2000modes_;
	bandwidths_ = TS2000_empty;
	dsp_lo = TS2000_lo;
	dsp_hi = TS2000_hi;
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

	has_micgain_control =
	has_notch_control =
	has_ifshift_control =
	has_swr_control = false;

	has_dsp_controls =
	has_smeter =
	has_power_out =
	has_swr_control =
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

void RIG_TS2000::initialize()
{
	cmd = "FR0;"; sendCommand(cmd, 0);
	cmd = "FT0;"; sendCommand(cmd, 0);
	cmd = "AC001;"; sendCommand(cmd, 0);
	get_preamp();
	get_attenuator();
}

void RIG_TS2000::selectA()
{
	cmd = "FR0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Rx A", cmd, replystr);
	cmd = "FT0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Tx A", cmd, replystr);
}

void RIG_TS2000::selectB()
{
	cmd = "FR1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Rx B", cmd, replystr);
	cmd = "FT1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Tx B", cmd, replystr);
}

void RIG_TS2000::set_split(bool val) 
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

bool RIG_TS2000::get_split()
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

long RIG_TS2000::get_vfoA ()
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
	sendCommand(cmd);
	showresp(WARN, ASC, "get smeter", cmd, replystr);
	size_t p = replystr.rfind("SM");
	if (p != string::npos && (p + 7 < replystr.length())) {
		replystr[p+7] = 0;
		int mtr = atoi(&replystr[p+5]);
		mtr = (mtr * 100) / 30;
		return mtr;
	}
	return 0;
}

int RIG_TS2000::get_swr()
{
	cmd = "RM1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "get swr", cmd, replystr);
	size_t p = replystr.rfind("RM");
	if (p != string::npos && (p + 7 < replystr.length())) {
		replystr[p+7] = 0;
		int mtr = atoi(&replystr[p+5]);
		mtr = (mtr * 100) / 30;
	}
	return 0;
}

// Transceiver power level
void RIG_TS2000::set_power_control(double val)
{
	int ival = (int)val;
	cmd = "PC000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ival % 10;
		ival /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set pwr ctrl", cmd, replystr);
}

int RIG_TS2000::get_power_out()
{
	return 0;
}

int RIG_TS2000::get_power_control()
{
	cmd = "PC;";
	sendCommand(cmd);
	showresp(WARN, ASC, "get pout", cmd, replystr);
	size_t p = replystr.rfind("PC");
	if (p != string::npos && (p + 5 < replystr.length())) {
		replystr[p+5] = 0;
		int mtr = atoi(&replystr[p+2]);
		return mtr;
	}
	return 0;
}

// Volume control return 0 ... 100
int RIG_TS2000::get_volume_control()
{
	cmd = "AG0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "get vol", cmd, replystr);
	size_t p = replystr.rfind("AG");
	if (p != string::npos && (p + 7 < replystr.length())) {
		cmd[p+7] = 0;
		int val = atoi(&replystr[p+3]);
		return (int)(val / 2.55);
	}
	return 0;
}

void RIG_TS2000::set_volume_control(int val) 
{
	int ivol = (int)(val * 2.55);
	cmd = "AG0000;";
	for (int i = 5; i > 2; i--) {
		cmd[i] += ivol % 10;
		ivol /= 10;
	}
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
	sendCommand(cmd);
	showresp(WARN, ASC, "get ATT", cmd, replystr);
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
	sendCommand(cmd);
	showresp(WARN, ASC, "get PRE", cmd, replystr);
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
	if (val == 0 || val == 1 || val == 3) {
		bandwidths_ = TS2000_empty;
		dsp_lo = TS2000_lo;
		dsp_hi = TS2000_hi;
		bw = 0x8803; // 200 ... 3000 Hz
	} else if (val == 2 || val == 6) {
		bandwidths_ = TS2000_CWwidths;
		dsp_lo = TS2000_empty;
		dsp_hi = TS2000_empty;
		bw = 7;
	} else if (val == 5 || val == 7) {
		bandwidths_ = TS2000_FSKwidths;
		dsp_lo = TS2000_empty;
		dsp_hi = TS2000_empty;
		bw = 1;
	} else { // val == 4 ==> AM
		bandwidths_ = TS2000_empty;
		dsp_lo = TS2000_AM_lo;
		dsp_hi = TS2000_AM_hi;
		bw = 0x8201;
	}
	return bw;
}

const char **RIG_TS2000::bwtable(int m)
{
	if (m == 0 || m == 1 || m == 3)
		return TS2000_empty;
	else if (m == 2 || m == 6)
		return TS2000_CWwidths;
	else if (m == 5 || m == 7)
		return TS2000_FSKwidths;
//else AM m == 4
	return TS2000_empty;
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

void RIG_TS2000::set_modeB(int val)
{
	B.imode = val;
	cmd = "MD";
	cmd += TS2000_mode_chr[val];
	cmd += ';';
	sendCommand(cmd, 0);
	showresp(WARN, ASC, "set mode B", cmd, replystr);
	B.iBW = set_widths(val);
}

int RIG_TS2000::get_modeB()
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

int RIG_TS2000::adjust_bandwidth(int val)
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

int RIG_TS2000::def_bandwidth(int val)
{
	return adjust_bandwidth(val);
}

void RIG_TS2000::set_bwA(int val)
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
		cmd = TS2000_CWbw[A.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set CW bw", cmd, replystr);
	}else if (A.imode == 5 || A.imode == 7) {
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
				if (replystr.find(TS2000_CWbw[i]) == p+2)
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
				if (replystr.find(TS2000_FSKbw[i]) == p+2)
					break;
			if (i == 4) i = 3;
			A.iBW = i;
		}
	}
	return A.iBW;
}

void RIG_TS2000::set_bwB(int val)
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
		cmd = TS2000_CWbw[B.iBW];
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set CW bw", cmd, replystr);
	}else if (B.imode == 5 || B.imode == 7) {
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
				if (replystr.find(TS2000_CWbw[i]) == p+2)
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
				if (replystr.find(TS2000_FSKbw[i]) == p+2)
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
	cmd = "MG000;";
	for (int i = 3; i > 0; i--) {
		cmd[1+i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd, 0);
	showresp(WARN, ASC, "set mic", cmd, replystr);
}

int RIG_TS2000::get_mic_gain()
{
	cmd = "MG;";
	sendCommand(cmd);
	showresp(WARN, ASC, "get mic", cmd, replystr);
	size_t p = replystr.rfind("MG");
	if (p != string::npos && (p + 5 < replystr.length())) {
		replystr[p+5] = 0;
		int val = atoi(&replystr[p+2]);
		return val;
	}
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

///////////////////
// IF shift only available if the transceiver is in the CW mode
// step size is 50 Hz
// not implemented for flrig since it cannot be used in USB the
// normal setting for digital modes
//
void RIG_TS2000::set_if_shift(int val)
{
	cmd = "IS 0000;";
	if (val < 0) cmd[3] = '-';
	val = abs(val);
	for (int i = 4; i > 0; i--) {
		cmd[3+i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd,0);
	showresp(WARN, ASC, "set IF shift", cmd, replystr);
}

bool RIG_TS2000::get_if_shift(int &val)
{
	static int oldval = 0;
	cmd = "IS;";
	sendCommand(cmd);
	showresp(WARN, ASC, "get IF shift", cmd, replystr);
	size_t p = replystr.rfind("IS");
	if (p != string::npos && (p + 8 < replystr.length())) {
		replystr[p+8] = 0;
		val = atoi(&replystr[p+3]);
		if (val != 0 || oldval != val) {
			oldval = val;
			return true;
		}
	}
	oldval = val;
	return false;
}

void RIG_TS2000::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -1000;
	max = 1000;
	step = 100;
}

void RIG_TS2000::set_notch(bool on, int val)
{
	cmd = "BP00000;";
	if (on == false) {
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set Notch off", cmd, replystr);
		notch_on = false;
		return;
	}
	if (!notch_on) {
		cmd[6] = '1'; // notch ON
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set Notch on", cmd, replystr);
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
	sendCommand(cmd,0);
	showresp(WARN, ASC, "set Notch val", cmd, replystr);
}

bool  RIG_TS2000::get_notch(int &val)
{
	bool ison = false;
	cmd = "BP00;";
	sendCommand(cmd);
	showresp(WARN, ASC, "get Notch", cmd, replystr);
	size_t p = replystr.rfind("BP");
	if (p != string::npos && (p + 6 < replystr.length())) {
		if (replystr[p+6] == '1') {
			ison = true;
			cmd = "BP01;";
			sendCommand(cmd);
			showresp(WARN, ASC, "get Notch val", cmd, replystr);
			p = replystr.rfind("BP");
			if (p != string::npos && (p + 7 < replystr.length())) {
				replystr[p+7] = 0;
				val = atoi(&replystr[p+4]);
				val -= 200;
				val *= -9;
			}
		}
	}
	return ison;
}

void RIG_TS2000::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = -1143;
	max = +1143;
	step = 9;
}

