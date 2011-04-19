/*
 * K3 drivers
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */

#include "K3.h"

const char K3name_[] = "K3";

const char *K3modes_[] = 
	{ "LSB", "USB", "CW", "FM", "AM", "DATA", "CW-R", "DATA-R", NULL};
const char modenbr[] = 
	{ '1', '2', '3', '4', '5', '6', '7', '9' };
static const char K3_mode_type[] =
	{ 'L', 'U', 'U', 'U', 'U', 'U', 'L', 'L' };
static int def_mode_width[] = {
39, 39, 15, 42, 42, 39, 15, 39 };

const char *K3_widths[] = {
  "50",  "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500", 
 "550",  "600",  "650",  "700",  "750",  "800",  "850",  "900",  "950", "1000",
"1100", "1200", "1300", "1400", "1500", "1600", "1700", "1800", "1900", "2000",
"2100", "2200", "2300", "2400", "2500", "2600", "2700", "2800", "2890", "3000",
"3200", "3400", "3600", "3800", "4000",
NULL};

const char *K3_bwval[] = {
"0005", "0010", "0015", "0020", "0025", "0030", "0035", "0040", "0045", "0050", 
"0055", "0060", "0065", "0070", "0075", "0080", "0085", "0090", "0095", "0100",
"0110", "0120", "0130", "0140", "0150", "0160", "0170", "0180", "0190", "0200",
"0210", "0220", "0230", "0240", "0250", "0260", "0270", "0280", "0290", "0300",
"0320", "0340", "0360", "0380", "0400",
 NULL };

RIG_K3::RIG_K3() {
// base class values	
	name_ = K3name_;
	modes_ = K3modes_;
	bandwidths_ = K3_widths;
	comm_baudrate = BR38400;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = false;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	modeA = 1;
	bwA = 39;
	modeB = 1;
	bwB = 39;

	has_bandwidth_control =
	has_power_control =
	has_volume_control =
	has_mode_control =
	has_ptt_control =
	has_attenuator_control =
	has_preamp_control = true;

	has_micgain_control =
	has_notch_control =
	has_ifshift_control =
	has_tune_control =
	has_swr_control = false;

}

int  RIG_K3::adjust_bandwidth(int m)
{
	return def_mode_width[m];
}

int  RIG_K3::def_bandwidth(int m)
{
	return def_mode_width[m];
}

void RIG_K3::initialize()
{
	cmd = "AI0;"; // disable auto-info
	sendCommand(cmd, 0);
	MilliSleep(100);
	showresp("disable auto-info");

	cmd = "K31;"; // K3 extended mode
	sendCommand(cmd, 0);
	MilliSleep(100);
	showresp("K3 extended mode");

	cmd = "SWT49;"; // Fine tuning (1 Hz mode)
	sendCommand(cmd, 0);
	MilliSleep(100);
	showresp("1 Hz fine tune mode");

	cmd = "OM;"; // request options
	sendCommand(cmd);
	MilliSleep(50);
	showresp("options");
	if (replystr.find("P") == string::npos) {
		minpwr = 0;
		maxpwr = 12;
		steppwr = 0.1;
	} else {
		minpwr = 0;
		maxpwr = 120;
		steppwr = 1;
	}

	modeA = 1;
	bwA = 19;
	modeB = 1;
	bwB = 19;
}

long RIG_K3::get_vfoA ()
{
	cmd = "FA;";
	int ret = sendCommand(cmd);
	showresp("vfo A");
	if (ret < 14) return freqA;
	size_t p = replystr.rfind("FA");
	if (p == string::npos) return freqA;
	
	long f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replystr[p + n] - '0';
	freqA = f;
	return freqA;
}

void RIG_K3::set_vfoA (long freq)
{
	freqA = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd, 0);
}

long RIG_K3::get_vfoB ()
{
	cmd = "FB;";
	int ret = sendCommand(cmd);
	showresp("vfo B");
	if (ret < 14) return freqB;
	size_t p = replystr.rfind("FB");
	if (p == string::npos) return freqB;
	
	long f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replystr[p + n] - '0';
	freqB = f;
	return freqB;
}

void RIG_K3::set_vfoB (long freq)
{
	freqB = freq;
	cmd = "FB00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd, 0);
}

// Volume control
void RIG_K3::set_volume_control(int val) 
{
	int ivol = (int)(val * 2.55);
	cmd = "AG000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ivol % 10;
		ivol /= 10;
	}
	sendCommand(cmd, 0);
}

int RIG_K3::get_volume_control()
{
	cmd = "AG;";
	int ret = sendCommand(cmd);
	showresp("audio volume");
	if (ret < 6) return 0;
	size_t p = replystr.rfind("AG");
	if (p == string::npos) return 0;

	replystr[p + 5] = 0;
	int v = atoi(&replystr[p + 2]);
	return (int)(v / 2.55);
}

void RIG_K3::set_modeA(int val)
{
	modeA = val;
	cmd = "MD0;";
	cmd[2] = modenbr[val];
	sendCommand(cmd, 0);
}

int RIG_K3::get_modeA()
{
	cmd = "MD;";
	int ret = sendCommand(cmd);
	showresp("mode A");
	if (ret < 4) return modeA;
	size_t p = replystr.rfind("MD");
	if (p == string::npos) return modeA;
	
	int md = replystr[p + 2] - '1';
	if (md == 8) md--;
	return (modeA = md);
}

void RIG_K3::set_modeB(int val)
{
	modeB = val;
	cmd = "MD$0;";
	cmd[3] = modenbr[val];
	sendCommand(cmd, 0);
}

int RIG_K3::get_modeB()
{
	cmd = "MD$;";
	int ret = sendCommand(cmd);
	showresp("mode B");
	if (ret < 4) return modeB;
	size_t p = replystr.rfind("MD$");
	if (p == string::npos) return modeB;
	
	int md = replystr[p + 3] - '1';
	if (md == 8) md--;
	return (modeB = md);
}

int RIG_K3::get_modetype(int n)
{
	return K3_mode_type[n];
}

void RIG_K3::set_preamp(int val)
{
	if (val) sendCommand("PA1;", 0);
	else	 sendCommand("PA0;", 0);
}

int RIG_K3::get_preamp()
{
	cmd = "PA;";
	int ret = sendCommand(cmd);
	showresp("preamp");
	if (ret < 4) return 0;
	size_t p = replystr.rfind("PA");
	if (p == string::npos) return 0;
	return (replystr[p + 2] == '1' ? 1 : 0);
}

//
void RIG_K3::set_attenuator(int val)
{
	if (val) sendCommand("RA01;", 0);
	else	 sendCommand("RA00;", 0);
}

int RIG_K3::get_attenuator()
{
	cmd = "RA;";
	int ret = sendCommand(cmd);
	showresp("attenuator");
	if (ret < 5) return 0;
	size_t p = replystr.rfind("RA");
	if (p == string::npos) return 0;
	return (replystr[p + 3] == '1' ? 1 : 0);
}

// Transceiver power level
void RIG_K3::set_power_control(double val)
{
	int ival = (int)val;
	cmd = "PC000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ival % 10;
		ival /= 10;
	}
	sendCommand(cmd, 0);
}

void RIG_K3::get_pc_min_max_step(double &min, double &max, double &step)
{
   min = minpwr; max = maxpwr; step = steppwr; 
}

// Tranceiver PTT on/off
void RIG_K3::set_PTT_control(int val)
{
	if (val) sendCommand("TX;", 0);
	else	 sendCommand("RX;", 0);
}

//BG (Bargraph Read; GET only)
//RSP format: BGnn; where <nn> is 00 (no bars) through 10 (bar 10) if the bargraph is in DOT mode, and 12 (no
//bars) through 22 (all 10 bars) if the bargraph is in BAR mode. Reads the S-meter level on receive. Reads the
//power output level or ALC level on transmit, depending on the RF/ALC selection. Also see SM/SM$ command,
//which can read either main or sub RX S-meter level.

//SM $ (S-meter Read; GET only)
//Basic RSP format: SMnnnn; where nnnn is 0000-0015. S9=6; S9+20=9; S9+40=12; S9+60=15.
//K3 Extended RSP format (K31): nnnn is 0000-0021. S9=9; S9+20=13; S9+40=17; S9+60=21.
//This command can be used to obtain either the main (SM) or sub (SM$) S-meter readings. Returns 0000 in transmit
//mode. BG can be used to simply emulate the bar graph level, and applies to either RX or TX mode.

int RIG_K3::get_smeter()
{
	cmd = "SM;";
	int ret = sendCommand(cmd);
	showresp("s-meter");
	if (ret < 7) return 0;
	size_t p = replystr.rfind("SM");
	if (p == string::npos) return 0;

	replystr[p + 6] = 0;
	int mtr = atoi(&replystr[p + 3]);
	if (mtr <= 6) mtr = (int) (50.0 * mtr / 6.0);
	else mtr = (int)(50 + (mtr - 6.0) * 50.0 / 9.0);
	return mtr;
}

void RIG_K3::set_noise(bool on)
{
	if (on) sendCommand("NB1;", 0);
	else	sendCommand("NB0;", 0);
}

//FW $ (Filter Bandwidth and Number; GET/SET)
//K3 Extended SET/RSP format (K31): FWxxxx; where xxxx is 0-9999, the bandwidth in 10-Hz units. May be
//quantized and/or range limited based on the present operating mode.

void RIG_K3::set_bwA(int val)
{
	cmd = "FW";
	bwA = val;
	cmd.append(K3_bwval[val]).append(";");
	sendCommand(cmd, 0);
}

int RIG_K3::get_bwA()
{
	cmd = "FW;";
	int ret = sendCommand(cmd);
	showresp("bandwidth A");
	if (ret < 6) return bwA;
	size_t p = replystr.rfind("FW");
	if (p == string::npos) return bwA;
	string bws = replystr.substr(p+2, 4); // "xxxx" iaw above manual exerpt
	int n = 0;
	while (K3_bwval[n] != NULL) {
		if (bws <= K3_bwval[n]) break;
		n++;
	}
	if (K3_bwval[n] != NULL) bwA = n;
	return bwA;
}
void RIG_K3::set_bwB(int val)
{
	cmd = "FW$";
	bwB = val;
	cmd.append(K3_bwval[val]).append(";");
	sendCommand(cmd, 0);
}

int RIG_K3::get_bwB()
{
	cmd = "FW$;";
	int ret = sendCommand(cmd);
	showresp("bandwidth B");
	if (ret < 6) return bwB;
	size_t p = replystr.rfind("FW$");
	if (p == string::npos) return bwB;
	string bws = replystr.substr(p+3, 4); // "xxxx" iaw above manual exerpt
	int n = 0;
	while (K3_bwval[n] != NULL) {
		if (bws <= K3_bwval[n]) break;
		n++;
	}
	if (K3_bwval[n] != NULL) bwB = n;
	return bwB;
}

int RIG_K3::get_power_out()
{
	cmd = "BG;"; // responds BGnn; 0 < nn < 10
	int ret = sendCommand(cmd);
	showresp("power out");
	if (ret < 5) return 0;
	size_t p = replystr.rfind("BG");
	if (p == string::npos) return 0;
	replystr[p + 4] = 0;
	int mtr = atoi(&replystr[p + 2]) * 10;
	if (mtr > 100) mtr = 100;
	return mtr;
}

