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
	modeA = 1;
	bwA = 1;
	def_mode = 1;
	defbw_ = 1;
	deffreq_ = 14070000;

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

bool RIG_TS450S::sendTScommand(string cmd, int retnbr, bool loghex)
{
	int ret = sendCommand(cmd, retnbr, loghex);
	if (RigSerial.IsOpen()) {
		LOG_INFO("%s", cmd.c_str());
		if (retnbr)
			LOG_INFO("%s", replybuff);
		return ret;
	}
	return 0;
}

void RIG_TS450S::initialize()
{
	cmd = "RM1;"; // select measurement '1' (swr)
	sendTScommand(cmd, 0, false);
}

long RIG_TS450S::get_vfoA ()
{
	cmd = "FA;";
	if (!sendTScommand(cmd, 14, false))
		return freqA;
	if (replystr.find("FA") != 0) {
		clearSerialPort();
		return freqA;
	}
	int f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replybuff[n] - '0';
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
	sendTScommand(cmd, 0, false);
}

// SM cmd 0 ... 100 (rig values 0 ... 15)
int RIG_TS450S::get_smeter()
{
	cmd = "SM;";
	if(!sendTScommand(cmd, 7, false))
		return 0;
	if (replystr.find("SM") != 0) {
		clearSerialPort();
		return 0;
	}
	replybuff[6] = 0;
	int mtr = atoi(&replybuff[2]);
	mtr = (mtr * 100) / 30;
	return mtr;
}

// RM cmd 0 ... 100 (rig values 0 ... 8)
int RIG_TS450S::get_swr()
{
	cmd = "RM;";
	if (!sendTScommand(cmd, 8, false))
		return 0;
	if (replystr.find("RM") != 0) {
		clearSerialPort();
		return 0;
	}
	replybuff[7] = 0;
	int mtr = atoi(&replybuff[3]);
	mtr = (mtr * 50) / 30;
	return mtr;
}


// Tranceiver PTT on/off
void RIG_TS450S::set_PTT_control(int val)
{
	if (val) cmd = "TX;";
	else	 cmd = "RX;";
	sendTScommand(cmd, 4, false);
}

void RIG_TS450S::set_modeA(int val)
{
	modeA = val;
	cmd = "MD";
	cmd += TS450S_mode_chr[val];
	cmd += ';';
	sendTScommand(cmd, 4, false);
}

int RIG_TS450S::get_modeA()
{
	if (!sendTScommand("MD;", 4, false))
		return modeA;
	if (replystr.find("MD") != 0) {
		clearSerialPort();
		return modeA;
	}
	int md = replybuff[2];
	md = md - '1';
	if (md == 8) md = 7;
	modeA = md;
	return modeA;
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
	sendTScommand(cmd, 0, false);
}

int RIG_TS450S::get_bwA()
{
	cmd = "FL;";
	if (!sendTScommand(cmd, 9, false))
		return bwA;
	replybuff[8] = 0;
	bwA = 0;
	while (TS450S_filters[bwA]) {
		if (strcmp(&replybuff[5], TS450S_filters[bwA]) == 0)
			return bwA;
		bwA++;
	}
	bwA = 0;
	return bwA;
}
