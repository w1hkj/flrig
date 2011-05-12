/*
 * K3 drivers
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */

#include "K3.h"
#include "status.h"

const char K3name_[] = "K3";

const char *K3modes_[] = 
	{ "LSB", "USB", "CW", "FM", "AM", "DATA", "CW-R", "DATA-R", NULL};
const char modenbr[] = 
	{ '1', '2', '3', '4', '5', '6', '7', '9' };
static const char K3_mode_type[] =
	{ 'L', 'U', 'U', 'U', 'U', 'U', 'L', 'L' };

static const char *K3_widths[] = {
   "50",  "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
  "550",  "600",  "650",  "700",  "750",  "800",  "850",  "900",  "950", "1000",
 "1100", "1200", "1300", "1400", "1500", "1600", "1700", "1800", "1900", "2000",
 "2200", "2400", "2600", "2800", "3000", "3200", "3400", "3600", NULL};

static int def_mode_width[] = { 34, 34, 15, 37, 37, 34, 15, 34 };

RIG_K3::RIG_K3() {
// base class values	
	name_ = K3name_;
	modes_ = K3modes_;
	bandwidths_ = K3_widths;//szNOBWS;
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
	def_freq = freqA = freqB = 14070000;
	def_mode = modeA = modeB = 1;
	def_bw = bwA = bwB = 34;

	can_change_alt_vfo =

	has_micgain_control =
	has_rf_control =
	has_bandwidth_control =
	has_power_control =
	has_volume_control =
	has_mode_control =
	has_ptt_control =
	has_noise_control =
	has_attenuator_control =
	has_smeter =
	has_power_out =
	has_split =
	has_preamp_control = true;

	has_notch_control =
	has_ifshift_control =
	has_tune_control =
	has_swr_control = false;

}

void RIG_K3::showparse(int level, string s, string rx) 
{
	time_t now;
	time(&now);
	struct tm *local = localtime(&now);
	char sztm[20];
	strftime(sztm, sizeof(sztm), "%H:%M:%S", local);

	size_t p;
	while((p = rx.find('\r')) != string::npos)
		rx.replace(p, 1, "<cr>");
	while((p = rx.find('\n')) != string::npos)
		rx.replace(p, 1, "<lf>");
	while((p = rx.find('\r')) != string::npos)
		rx.replace(p, 1, "<cr>");
	while((p = rx.find('\n')) != string::npos)
		rx.replace(p, 1, "<lf>");

	switch (level) {
	case ERR:
		SLOG_ERROR("%s: %10s, %s", sztm, s.c_str(), rx.c_str());
		break;
	case WARN:
		SLOG_WARN("%s: %10s, %s", sztm, s.c_str(), rx.c_str());
		break;
	case INFO:
	default:
		SLOG_INFO("%s: %10s, %s", sztm, s.c_str(), rx.c_str());
	}
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

	cmd = "K31;"; // K3 extended mode
	sendCommand(cmd, 0);

	cmd = "SWT49;"; // Fine tuning (1 Hz mode)
	sendCommand(cmd, 0);

	set_split(false); // normal ops

	cmd = "OM;"; // request options to get power level
	sendCommand(cmd, 0);   // was missing !!
	waitResponse(100);
	showresp(WARN, ASC, "options", cmd, replystr);
	if (replystr.find("P") == string::npos) {
		minpwr = 0;
		maxpwr = 12;
		steppwr = 1;
	} else {
		minpwr = 0;
		maxpwr = 106;
		steppwr = 1;
	}

	modeA = 1;
	bwA = 19;
	modeB = 1;
	bwB = 19;
}

void RIG_K3::shutdown()
{
}

long RIG_K3::get_vfoA ()
{
	cmd = "FA;";
	sendCommand(cmd, 0);
	return freqA;
}

// s will be in form "FA00014060000;"
int RIG_K3::parse_vfoA(string s)
{
	showparse(WARN, "xcvr vfo A", s);
	if (s.length() < 14) return freqA;
	long f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + s[n] - '0';
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
	showparse(WARN, "set vfo A", cmd);
}

// s will be in form "FB00003580000;"
int RIG_K3::parse_vfoB(string s)
{
	showparse(WARN, "xcvr vfo B", s);
	if (s.length() < 14) return freqB;
	long f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + s[n] - '0';
	freqB = f;
	return freqB;
}

long RIG_K3::get_vfoB ()
{
	cmd = "FB;";
	sendCommand(cmd, 0);
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
	showparse(WARN, "set vfo B", cmd);
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
	showparse(WARN, "set vol", cmd);
}

int RIG_K3::get_volume_control()
{
	cmd = "AG;";
	sendCommand(cmd, 0);
	return progStatus.volume;
}

// s in format AG000;
int RIG_K3::parse_volume_control(string s)
{
	showparse(WARN, "xcvr vol", s);
	if (s.length() < 6) return progStatus.volume;

	s[5] = 0;
	int v = atoi(&s[2]);
	return (int)(v / 2.55);
}

void RIG_K3::set_modeA(int val)
{
	modeA = val;
	cmd = "MD0;";
	cmd[2] = modenbr[val];
	sendCommand(cmd, 0);
	showparse(WARN, "set mode A", cmd);
}

int RIG_K3::get_modeA()
{
	cmd = "MD;";
	sendCommand(cmd, 0);
	return modeA;
}

int RIG_K3::parse_modeA(string s)
{
	if (s.length() < 4) return modeA;
	int md = s[2] - '1';
	if (md == 8) md--;
	return (modeA = md);
}

void RIG_K3::set_modeB(int val)
{
	modeB = val;
	cmd = "MD$0;";
	cmd[3] = modenbr[val];
	sendCommand(cmd, 0);
	showparse(WARN, "set mode B", cmd);
}

int RIG_K3::get_modeB()
{
	cmd = "MD$;";
	sendCommand(cmd, 0);
	return modeB;
}

int RIG_K3::parse_modeB(string s)
{
	showparse(WARN, "xcvr mode B", s);
	if (s.length() < 5) return modeB;
	int md = s[3] - '1';
	if (md == 8) md--;
	return (modeB = md);
}

int RIG_K3::get_modetype(int n)
{
	return K3_mode_type[n];
}

void RIG_K3::set_preamp(int val)
{
	progStatus.preamp = val;
	if (val) sendCommand("PA1;", 0);
	else	 sendCommand("PA0;", 0);
}

int RIG_K3::get_preamp()
{
	cmd = "PA;";
	sendCommand(cmd, 0);
	return progStatus.preamp;
}

int RIG_K3::parse_preamp(string s)
{
	showparse(WARN, "xcvr preamp", s);
	if (s.length() < 4) return progStatus.preamp;
	progStatus.preamp = (s[2] == '1' ? 1 : 0);
	return progStatus.preamp;
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
	sendCommand(cmd, 0);
	return progStatus.attenuator;
}

int RIG_K3::parse_attenuator(string s)
{
	showparse(WARN, "xcvr att", s);
	if (s.length() < 5) return progStatus.attenuator;
	progStatus.attenuator = (s[3] == '1' ? 1 : 0);
	return progStatus.attenuator;
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
	showparse(WARN, "power level", cmd);
}

int RIG_K3::get_power_control()
{
	cmd = "PC;";
	sendCommand(cmd, 0);
	return progStatus.power_level;
}

int RIG_K3::parse_power_control(string s)
{
	showparse(WARN, "xcvr power ctrl", s);
	if (s.length() < 6) return progStatus.power_level;
	s[5] = 0;
	int v = atoi(&s[2]);
	progStatus.power_level = (int)(v / 2.55);
	return progStatus.power_level;
}

void RIG_K3::get_pc_min_max_step(double &min, double &max, double &step)
{
   min = minpwr; max = maxpwr; step = steppwr; 
}

// Transceiver rf control
void RIG_K3::set_rf_gain(int val)
{
	int ival = (int)val;
	cmd = "RG000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ival % 10;
		ival /= 10;
	}
	sendCommand(cmd, 0);
	showparse(WARN, "set rfgain ctrl", cmd);
}

int RIG_K3::get_rf_gain()
{
	cmd = "RG;";
	sendCommand(cmd, 0);
	return progStatus.rfgain;
}

int RIG_K3::parse_rf_gain(string s)
{
	showparse(WARN, "xcvr rfgain ctrl", s);
	if (s.length() < 6) return progStatus.rfgain;
	s[5] = 0;
	progStatus.rfgain = atoi(&s[2]);
	return progStatus.rfgain;
}

void RIG_K3::get_rf_min_max_step(int &min, int &max, int &step)
{
   min = 0; max = 250; step = 1; 
}

// Transceiver mic control
void RIG_K3::set_mic_gain(int val)
{
	int ival = (int)val;
	cmd = "MG000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ival % 10;
		ival /= 10;
	}
	sendCommand(cmd, 0);
	showparse(WARN, "set mic ctrl", cmd);
}

int RIG_K3::get_mic_gain()
{
	cmd = "MG;";
	sendCommand(cmd, 0);
	return progStatus.mic_gain;
}

int RIG_K3::parse_mic_gain(string s)
{
	showparse(WARN, "xcvr mic ctrl", s);
	if (s.length() < 6) return progStatus.mic_gain;
	s[5] = 0;
	progStatus.mic_gain = atoi(&s[2]);
	return progStatus.mic_gain;
}

void RIG_K3::get_mic_min_max_step(int &min, int &max, int &step)
{
   min = 0; max = 60; step = 1; 
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
	sendCommand(cmd, 0);
	return 0;
}

int RIG_K3::parse_smeter(string s)
{
	showparse(WARN, "xcvr s-meter", s);
	if (s.length() < 7) return 0;
	s[6] = 0;
	int mtr = atoi(&s[3]);
	if (mtr <= 6) mtr = (int) (50.0 * mtr / 6.0);
	else mtr = (int)(50 + (mtr - 6.0) * 50.0 / 9.0);
	return mtr;
}

void RIG_K3::set_noise(bool on)
{
	if (on) sendCommand("NB1;", 0);
	else	sendCommand("NB0;", 0);
}

int RIG_K3::get_noise()
{
	cmd = "NB;";
	sendCommand(cmd, 0);
	return progStatus.noise;
}

int RIG_K3::parse_noise(string s)
{
	showparse(WARN, "xcvr noise", s);
	if (s.length() < 4) return progStatus.noise;
	progStatus.noise = (s[2] == '1' ? 1 : 0);
	return progStatus.noise;
}

//FW $ (Filter Bandwidth and Number; GET/SET)
//K3 Extended SET/RSP format (K31): FWxxxx; where xxxx is 0-9999, the bandwidth in 10-Hz units. May be
//quantized and/or range limited based on the present operating mode.

void RIG_K3::set_bwA(int val)
{
	cmd = "FW0000;";
	bwA = val;
	val = atoi(K3_widths[val]);
	val /= 10; cmd[5] += val % 10;
	val /= 10; cmd[4] += val % 10;
	val /= 10; cmd[3] += val % 10;
	val /= 10; cmd[2] += val % 10;
	sendCommand(cmd, 0);
	showparse(WARN, "set bw A", cmd);
}

int RIG_K3::get_bwA()
{
	cmd = "FW;";
	sendCommand(cmd, 0);
	return bwA;
}

int RIG_K3::parse_bwA(string s)
{
	showparse(WARN, "xcvr bw A", s);
	if (s.length() < 7) return bwA;
	int bw = 0;
	for (int i = 2; i < 6; i++) bw = bw * 10 + s[i] - '0';
	bw *= 10;
	for (bwA = 0; bwA < 36; bwA++)
		if (bw <= atoi(K3_widths[bwA])) break;
	return bwA;
}

void RIG_K3::set_bwB(int val)
{
	cmd = "FW$0000;";
	bwB = val;
	val = atoi(K3_widths[val]);
	val /= 10; cmd[6] += val % 10;
	val /= 10; cmd[5] += val % 10;
	val /= 10; cmd[4] += val % 10;
	val /= 10; cmd[3] += val % 10;
	sendCommand(cmd, 0);
	showparse(WARN, "set bw B", cmd);
}

int RIG_K3::get_bwB()
{
	cmd = "FW$;";
	sendCommand(cmd, 0);
	return bwB;
}

int RIG_K3::parse_bwB(string s)
{
	showparse(WARN, "xcvr bw B", s);
	if (s.length() < 8) return bwB;
	int bw = 0;
	for (int i = 2; i < 6; i++) bw = bw * 10 + s[i] - '0';
	bw *= 10;
	for (bwB = 0; bwB < 36; bwB++)
		if (bw <= atoi(K3_widths[bwB])) break;
	return bwB;
}

int RIG_K3::get_power_out()
{
	cmd = "BG;"; // responds BGnn; 0 < nn < 10
	sendCommand(cmd, 0);
	return 0;
}

int RIG_K3::parse_power_out(string s)
{
	showparse(WARN, "xcvr power out", s);
	if (s.length() < 5) return 0;
	s[4] = 0;
	int mtr = atoi(&s[2]) * 10;
	if (mtr > 100) mtr = 100;
	return mtr;
}

bool RIG_K3::can_split()
{
	return true;
}

void RIG_K3::set_split(bool val)
{
	if (val) {
		cmd = "FT1;";
		sendCommand(cmd, 0);
	} else {
		cmd = "FR0;";
		sendCommand(cmd, 0);
	}
}

bool RIG_K3::get_split()
{
	cmd = "IF;";
	sendCommand(cmd, 0);
	return false;
}

int RIG_K3::parse_split(string s)
{
	showparse(WARN, "xcvr info", s);
	if (s.length() < 38) return false;
	if (s[32] == '1') return true;
	return false;
}

