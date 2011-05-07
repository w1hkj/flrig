/*
 * Kenwood TS450S driver
 * originally based on Kenwood TS2000 driver
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */


#include "TS450S.h"
#include "support.h"

static const char TS450Sname_[] = "TS-450S";

static const char *TS450Smodes_[] = {
		"LSB", "USB", "CW", "FM", "AM", "FSK", "CW-R", "FSK-R", NULL};
static const char TS450S_mode_chr[] =  { '1', '2', '3', '4', '5', '6', '7', '9' };
static const char TS450S_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'L', 'L', 'U' };

static const char *TS450S_widths[] = {
"NONE", "FM-W", "FM-N", "AM", "SSB", "CW", NULL};
static const char *TS450S_filters[] = {
"000", "002", "003", "005", "007", "009", NULL};

RIG_TS450S::RIG_TS450S() {
// base class values
	name_ = TS450Sname_;
	modes_ = TS450Smodes_;
	_mode_type = TS450S_mode_type;
	bandwidths_ = TS450S_widths;
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
	modeB = modeA = def_mode = 1;
	bwB = bwA = defbw_ = 1;
	freqB = freqA = deffreq_ = 14070000;

	has_noise_control =
	has_micgain_control =
	has_volume_control =
	has_power_control =
	has_tune_control =
	has_attenuator_control =
	has_preamp_control =
	has_notch_control =
	has_ifshift_control =
	has_swr_control = false;

	has_mode_control =
	has_bandwidth_control =
	has_ptt_control = true;
}

void RIG_TS450S::initialize()
{
	cmd = "RM1;"; // select measurement '1' (swr)
	sendCommand(cmd, 0);
}

long RIG_TS450S::get_vfoA ()
{
	cmd = "FA;";
	int ret = sendCommand(cmd);
	if (ret < 14) return freqA;

	size_t p = replystr.rfind("FA");
	if (p == string::npos) return freqA;

	int f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replystr[p + n] - '0';
	freqA = f;
	return freqA;
}

void RIG_TS450S::set_vfoA (long freq)
{
	freqA = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd, 0);
}

long RIG_TS450S::get_vfoB ()
{
	cmd = "FB;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get vfoB", cmd, replystr);
	if (ret < 14) return freqB;
	size_t p = replystr.rfind("FB");
	if (p == string::npos) return freqB;
	
	int f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replystr[p + n] - '0';
	freqB = f;
	return freqB;
}

void RIG_TS450S::set_vfoB (long freq)
{
	freqB = freq;
	cmd = "FB00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set vfoB", cmd, replystr);
}

// SM cmd 0 ... 100 (rig values 0 ... 15)
int RIG_TS450S::get_smeter()
{
	cmd = "SM;";
	int ret = sendCommand(cmd);
	if (ret < 7) return 0;

	size_t p = replystr.rfind("SM");
	if (p == string::npos) return 0;

	replystr[p + 6] = 0;
	int mtr = atoi(&replystr[p + 2]);
	mtr = (mtr * 100) / 30;
	return mtr;
}

// RM cmd 0 ... 100 (rig values 0 ... 8)
int RIG_TS450S::get_swr()
{
	cmd = "RM;";
	int ret = sendCommand(cmd);
	if (ret < 8) return 0;

	size_t p = replystr.rfind("RM");
	if (p == string::npos) return 0;
	
	replystr[p + 7] = 0;
	int mtr = atoi(&replystr[p + 3]);
	mtr = (mtr * 50) / 30;
	return mtr;
}


// Tranceiver PTT on/off
void RIG_TS450S::set_PTT_control(int val)
{
	if (val) cmd = "TX;";
	else	 cmd = "RX;";
	sendCommand(cmd, 0);
}

void RIG_TS450S::set_modeA(int val)
{
	modeA = val;
	cmd = "MD";
	cmd += TS450S_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "set modeA", cmd, replystr);
}

int RIG_TS450S::get_modeA()
{
	cmd = "MD;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get modeB", cmd, replystr);
	if (ret < 4) return modeA;

	size_t p = replystr.rfind("MD");
	if (p == string::npos) return modeA;

	int md = replystr[p + 2];
	md = md - '1';
	if (md == 8) md = 7;
	modeA = md;
	return modeA;
}

void RIG_TS450S::set_modeB(int val)
{
	modeB = val;
	cmd = "MD";
	cmd += TS450S_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "set modeB", cmd, replystr);
}

int RIG_TS450S::get_modeB()
{
	cmd = "MD;";
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get modeB", cmd, replystr);
	if (ret < 4) return B.imode;

	size_t p = replystr.rfind("MD");
	if (p == string::npos) return B.imode;

	int md = replystr[p + 2];
	md = md - '1';
	if (md == 8) md = 7;
	modeB = md;
	return modeB;
}

int RIG_TS450S::get_modetype(int n)
{
	return _mode_type[n];
}

void RIG_TS450S::set_bwA(int val)
{
	bwA = val;
	cmd = "FL";
	cmd.append(TS450S_filters[val]).append(TS450S_filters[val]);
	cmd += ';';
	sendCommand(cmd, 0);
}

int RIG_TS450S::get_bwA()
{
	cmd = "FL;";
	int ret = sendCommand(cmd);
	if (ret < 9) return bwA;
	size_t p = replystr.rfind("FL");
	if (p == string::npos) return bwA;
	
	replystr[p + 8] = 0;
	bwA = 0;
	while (TS450S_filters[bwA]) {
		if (strcmp(&replystr[p + 5], TS450S_filters[bwA]) == 0)
			return bwA;
		bwA++;
	}
	bwA = 0;
	return bwA;
}

void RIG_TS450S::set_bwB(int val)
{
	bwB = val;
	cmd = "FL";
	cmd.append(TS450S_filters[val]).append(TS450S_filters[val]);
	cmd += ';';
	sendCommand(cmd, 0);
}

int RIG_TS450S::get_bwB()
{
	cmd = "FL;";
	int ret = sendCommand(cmd);
	if (ret < 9) return bwB;
	size_t p = replystr.rfind("FL");
	if (p == string::npos) return bwB;
	
	replystr[p + 8] = 0;
	bwB = 0;
	while (TS450S_filters[bwB]) {
		if (strcmp(&replystr[p + 5], TS450S_filters[bwB]) == 0)
			return bwB;
		bwB++;
	}
	bwB = 0;
	return bwB;
}

bool RIG_TS450S::can_split()
{
	return true;
}

void RIG_TS450S::set_split(bool val)
{
	if (val) {
		cmd = "FT1;";
		sendCommand(cmd);
		showresp(WARN, ASC, "set split ON", cmd, replystr);
	} else {
		cmd = "FR0;";
		sendCommand(cmd);
		showresp(WARN, ASC, "set split OFF", cmd, replystr);
	}
}

