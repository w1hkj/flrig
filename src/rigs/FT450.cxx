/*
 * Yaesu FT-450 drivers
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */


#include "FT450.h"
#include "rig.h"

static const char FT450name_[] = "FT-450";

static const char *FT450modes_[] = {
		"LSB", "USB", "CW", "FM", "AM", "RTTY-L", "CW-R", "USER-L", "RTTY-U", "FM-N", "USER-U", NULL};
static const char mode_chr[] =  { '1', '2', '3', '4', '5', '6', '7', '8', '9', 'B', 'C' };
static const char mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'L', 'L', 'L', 'U', 'U', 'U' };

static const char *FT450_widths[] = {"NARR", "NORM", "WIDE", NULL};

RIG_FT450::RIG_FT450() {
// base class values	
	name_ = FT450name_;
	modes_ = FT450modes_;
	bandwidths_ = FT450_widths;
	comm_baudrate = BR38400;
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
	modeA = 1;
	bwA = 2;
	def_mode = 10;
	defbw_ = 2;
	deffreq_ = 14070000;


	has_power_control =
	has_volume_control =
	has_mode_control =
	has_noise_control =
	has_bandwidth_control =
	has_micgain_control =
	has_notch_control =
	has_attenuator_control =
	has_preamp_control =
	has_ifshift_control =
	has_ptt_control =
	has_tune_control =
	has_swr_control =
	has_special = true;
	
// derived specific
	notch_on = false;
}

int  RIG_FT450::adjust_bandwidth(int m)
{
	return 1;
}

void RIG_FT450::select_vfoA()
{
	cmd = "SV0;";
	sendCommand(cmd, 0, false);
}

void RIG_FT450::select_vfoB()
{
	cmd = "SV1;";
	sendCommand(cmd, 0, false);
}

void RIG_FT450::initialize()
{
	select_vfoA();
}

long RIG_FT450::get_vfoA ()
{
	cmd = "FA;";
	if (sendCommand(cmd, 11, false)) {
		int f = 0;
		for (size_t n = 2; n < 10; n++)
			f = f*10 + replybuff[n] - '0';
		freqA = f;
	}
	return freqA;
}

void RIG_FT450::set_vfoA (long freq)
{
	freqA = freq;
	cmd = "FA00000000;";
	for (int i = 9; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd, 0, false);
	sendCommand(cmd, 0, false);
}

long RIG_FT450::get_vfoB ()
{
	cmd = "FB;";
	if (sendCommand(cmd, 11, false)) {
		int f = 0;
		for (size_t n = 2; n < 10; n++)
			f = f*10 + replybuff[n] - '0';
		freqB = f;
	}
	return freqB;
}

void RIG_FT450::set_vfoB (long freq)
{
	freqB = freq;
	cmd = "FB00000000;";
	for (int i = 9; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd, 0, false);
}

void RIG_FT450::set_split(bool on)
{
	if (on) cmd = "FT1;";
	else cmd = "FT0;";
	sendCommand(cmd, 0, false);
	LOG_INFO("cmd: %s\nreply: %s", cmd.c_str(), replystr.c_str());
}

int RIG_FT450::get_smeter()
{
	cmd = "SM0;";
	if(sendCommand(cmd, 7, false)) {
		replybuff[6] = 0;
		int mtr = atoi(&replybuff[3]);
		mtr = mtr * 100.0 / 256.0;
		return mtr;
	}
	return 0;
}

// measured by W3NR
//  SWR..... mtr ... display
//  6:1..... 255 ... 100
//  3:1..... 132 ...  50
//  2:1..... 066 ...  26
//  2.5:1... 099 ...  39
//  1.5:1... 033 ...  13
//  1.1:1... 008 ...   3

int RIG_FT450::get_swr()
{
	cmd = "RM6;";
	if (sendCommand(cmd, 7, false)) {
		replybuff[6] = 0;
		int mtr = atoi(&replybuff[3]);
		return mtr / 2.55;
	}
	return 0;
}


int RIG_FT450::get_power_out()
{
	cmd = "RM5;";
	if (sendCommand(cmd, 7, false)) {
		replybuff[6] = 0;
		double mtr = (double)(atoi(&replybuff[3]));
		mtr = -6.6263535 + .11813178 * mtr + .0013607405 * mtr * mtr;
		return (int)mtr;
	}
	return 0;
}

int RIG_FT450::get_power_control()
{
	cmd = "PC;";
	if (sendCommand(cmd, 6, false)) {
		replybuff[5] = 0;
		int mtr = atoi(&replybuff[2]);
		return mtr;
	}
	return 0;
}

// Volume control return 0 ... 100
int RIG_FT450::get_volume_control()
{
	cmd = "AG0;";
	if (sendCommand(cmd, 7, false))  {
		cmd[6] = 0;
		return atoi(&replybuff[3]);
	}
	return 0;
}

void RIG_FT450::set_volume_control(int val) 
{
	int ivol = val;
	cmd = "AG0000;";
	for (int i = 5; i > 2; i--) {
		cmd[i] += ivol % 10;
		ivol /= 10;
	}
	sendCommand(cmd, 0, false);
}

void RIG_FT450::get_vol_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 255; step = 1;
}

// Transceiver power level
void RIG_FT450::set_power_control(double val)
{
	int ival = (int)val;
	cmd = "PC000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ival % 10;
		ival /= 10;
	}
	sendCommand(cmd, 0, false);
}

// Tranceiver PTT on/off
void RIG_FT450::set_PTT_control(int val)
{
	if (val) sendCommand("TX1;", 0, false);
	else	 sendCommand("TX0;", 0, false);
}

void RIG_FT450::tune_rig()
{
	sendCommand("AC002;",0, false);
}

void RIG_FT450::set_attenuator(int val)
{
	if (val) sendCommand("RA01;", 0, false);
	else	 sendCommand("RA00;", 0, false);
}

int RIG_FT450::get_attenuator()
{
	if (sendCommand("RA0;", 5, false))
		return (replybuff[3] == '1' ? 1 : 0);
	return 0;
}

void RIG_FT450::set_preamp(int val)
{
	if (val) sendCommand("PA00;", 0, false);
	else	 sendCommand("PA01;", 0, false);
}

int RIG_FT450::get_preamp()
{
	if (sendCommand("PA0;", 5, false))
		return (replybuff[3] == '1' ? 0 : 1);
	return 0;
}


void RIG_FT450::set_modeA(int val)
{
	modeA = val;
	cmd = "MD0";
	cmd += mode_chr[val];
	cmd += ';';
	sendCommand(cmd, 0, false);
}

int RIG_FT450::get_modeA()
{
	if (sendCommand("MD0;", 5, false)) {
		int md = replybuff[3];
		if (md <= '9') md = md - '1';
		else md = 9 + md - 'B';
		modeA = md;
	}
	return modeA;
}

void RIG_FT450::set_bwA(int val)
{
	bwA = val;
	switch (val) {
		case 0 : cmd = "SH000;"; break;
		case 1 : cmd = "SH016;"; break;
		case 2 : cmd = "SH031;"; break;
		default: cmd = "SH031;";
	}
	sendCommand(cmd, 0, false);
}

int RIG_FT450::get_bwA()
{
	if (sendCommand("SH0;", 6, false)) {
		replybuff[5] = 0;
		if (strcmp(&replybuff[3],"00") == 0) bwA = 0;
		if (strcmp(&replybuff[3],"16") == 0) bwA = 1;
		if (strcmp(&replybuff[3],"31") == 0) bwA = 2;
	}
	return bwA;
}

int RIG_FT450::get_modetype(int n)
{
	return mode_type[n];
}

void RIG_FT450::set_if_shift(int val)
{
	cmd = "IS0+0000;";
	if (val < 0) cmd[3] = '-';
	val = abs(val);
	for (int i = 4; i > 0; i--) {
		cmd[3+i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd, 0, false);
}

bool RIG_FT450::get_if_shift(int &val)
{
	static int oldval = 0;
	sendCommand("IS0;", 9, false);
	replybuff[8] = 0;
	val = atoi(&replybuff[3]);
	if (val != 0 || oldval != val) {
		oldval = val;
		return true;
	}
	oldval = val;
	return false;
}

void RIG_FT450::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -1000;
	max = 1000;
	step = 100;
}

void RIG_FT450::set_notch(bool on, int val)
{
	cmd = "BP00000;";
	if (on == false) {
		sendCommand(cmd, 0, false);
		notch_on = false;
		return;
	}
	if (!notch_on) {
		cmd[6] = '1'; // notch ON
		sendCommand(cmd, 0, false);
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
	sendCommand(cmd, 0, false);
}

bool  RIG_FT450::get_notch(int &val)
{
	bool ison = false;
	cmd = "BP00;";
	if (sendCommand(cmd, 8, false)) {
		if (replybuff[6] == '1') {
			ison = true;
			cmd = "BP01;";
			if (sendCommand(cmd, 8, false)) {
				replybuff[7] = 0;
				val = atoi(&replybuff[4]);
				val -= 200;
				val *= -9;
			}
		}
	}
	return ison;
}

void RIG_FT450::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = -1143;
	max = +1143;
	step = 9;
}

void RIG_FT450::set_noise(bool b)
{
	if (b)
		cmd = "NB01;";
	else
		cmd = "NB00;";
	sendCommand(cmd, 0, false);
}

// val 0 .. 100
void RIG_FT450::set_mic_gain(int val)
{
	cmd = "MG000;";
	val = (int)(val * 2.55); // convert to 0 .. 255
	for (int i = 3; i > 0; i--) {
		cmd[1+i] += val % 10;
		val /= 10;
	}
	sendCommand(cmd, 0, false);
}

int RIG_FT450::get_mic_gain()
{
	sendCommand("MG;", 6, false);
	replybuff[5] = 0;
	int val = atoi(&replybuff[2]);
	return val;
}

void RIG_FT450::get_mic_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_FT450::set_special(int v)
{
	if (v) cmd = "VR1;";
	else   cmd = "VR0;";
	sendCommand(cmd, 0, false);
}
