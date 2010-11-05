/*
 * Kenwood TS570 driver
 * originally based on Kenwood TS2000 driver
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */


#include "TS570.h"
#include "support.h"

static const char TS570name_[] = "TS-570";

static const char *TS570modes_[] = {
		"LSB", "USB", "CW", "FM", "AM", "FSK", "CW-R", "FSK-R", NULL};
static const char TS570_mode_chr[] =  { '1', '2', '3', '4', '5', '6', '7', '9' };
static const char TS570_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'L', 'L', 'U' };

static const char *TS570_SSBwidths[] = { // same for AM and FM
"NARR", "WIDE", NULL};

static const char *TS570_SSBbw[] = {
"FW0000;", "FW0001;", NULL};

static const char *TS570_CWwidths[] = {
"50", "100", "200", "300", "400", "600", "1000", "2000", NULL};
static const char *TS570_CWbw[] = {
"FW0050;", "FW0100;", "FW0200;", "FW0300;",
"FW0400;", "FW0600;", "FW1000;", "FW2000;", NULL};

static const char *TS570_FSKwidths[] = {
"250", "500", "1000", "1500", NULL};
static const char *TS570_FSKbw[] = {
  "FW0250;", "FW0500;", "FW1000;", "FW1500;", NULL};

RIG_TS570::RIG_TS570() {
// base class values
	name_ = TS570name_;
	modes_ = TS570modes_;
	bandwidths_ = TS570_SSBwidths;
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

	has_notch_control =
	has_ifshift_control =
	has_swr_control = false;

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

bool RIG_TS570::sendTScommand(string cmd, int retnbr, bool loghex)
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

void RIG_TS570::initialize()
{
	cmd = "FR0;"; sendTScommand(cmd, 4, false);
	cmd = "FT0;"; sendTScommand(cmd, 4, false);
	cmd = "AC001;"; sendTScommand(cmd, 6, false);
	get_preamp();
	get_attenuator();
	is_TS570S = get_ts570id();
}

bool RIG_TS570::get_ts570id()
{
	cmd = "ID;";
	if (sendTScommand(cmd, 6, false)) {
	if (replybuff[3] == '1' && replybuff[4] == '8')  return true;
	}
	return false;
}

long RIG_TS570::get_vfoA ()
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

void RIG_TS570::set_vfoA (long freq)
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
int RIG_TS570::get_smeter()
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
	mtr = (mtr * 100) / 16;
	return mtr;
}

// RM cmd 0 ... 100 (rig values 0 ... 8)
int RIG_TS570::get_swr()
{
	cmd = "RM1;RM;"; // select measurement '1' (swr) and read meter
	if (!sendTScommand(cmd, 8, false))
		return 0;
	if (replystr.find("RM") != 0) {
		clearSerialPort();
		return 0;
	}
	replybuff[7] = 0;
	int mtr = atoi(&replybuff[3]);
	mtr = (mtr * 100) / 9;
	return mtr;
}

// power output measurement 0 ... 15
int RIG_TS570::get_power_out()
{
	cmd = "SM;";
	if (!sendTScommand(cmd, 6, false))
		return 0;
	if (replystr.find("SM") != 0) {
		clearSerialPort();
		return 0;
	}
	replybuff[5] = 0;
	int mtr = atoi(&replybuff[2]);
	mtr = (mtr * 100) / 16;
	return mtr;
}

// (xcvr power level is in 5W increments)
void RIG_TS570::set_power_control(double val)
{
	int ival = (int)val;
	cmd = "PC000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ival % 10;
		ival /= 10;
	}
	sendTScommand(cmd, 0, false);
}

int RIG_TS570::get_power_control()
{
	cmd = "PC;";
	if (!sendTScommand(cmd, 6, false))
		return 0;
	if (replystr.find("PC") != 0) {
		clearSerialPort();
		return 0;
	}
	replybuff[5] = 0;
	int mtr = atoi(&replybuff[2]);
	return mtr;
}

// Volume control return 0 ... 100  (rig values 0 ... 255)
int RIG_TS570::get_volume_control()
{
	cmd = "AG;";
	if (!sendTScommand(cmd, 6, false))
		return 0;
	if (replystr.find("AG") != 0) {
		clearSerialPort();
		return 0;
	}
	cmd[5] = 0;
	int val = atoi(&replybuff[2]);
	return (int)(val / 2.55);
}

void RIG_TS570::set_volume_control(int val)
{
	int ivol = (int)(val * 2.55);
	cmd = "AG000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ivol % 10;
		ivol /= 10;
	}
	sendTScommand(cmd, 0, false);
}

// Tranceiver PTT on/off
void RIG_TS570::set_PTT_control(int val)
{
	if (val) cmd = "TX;";
	else	 cmd = "RX;";
	sendTScommand(cmd, 4, false);
}

void RIG_TS570::tune_rig()
{
	cmd = "AC 11;";
	sendTScommand(cmd, 0, false);
}

void RIG_TS570::set_attenuator(int val)
{
	att_on = val;
	if (val) cmd = "RA01;";
	else	 cmd = "RA00;";
	sendTScommand(cmd, 0, false);
}

int RIG_TS570::get_attenuator()
{
	cmd = "RA;";
	if (!sendTScommand(cmd, 7, false))
		return att_on;
	if (replystr.find("RA") != 0) {
		clearSerialPort();
		return att_on;
	}
	if (replybuff[2] == '0' && replybuff[3] == '0')
		att_on = 0;
	else
		att_on = 1;
	return att_on;
}

void RIG_TS570::set_preamp(int val)
{
	preamp_on = val;
	if (val) cmd = "PA1;";
	else	 cmd = "PA0;";
	sendTScommand(cmd, 0, false);
}

int RIG_TS570::get_preamp()
{
	cmd = "PA;";
	if (!sendTScommand(cmd, 5, false) )
		return preamp_on;
	if (replystr.find("PA") != 0) {
		clearSerialPort();
		return preamp_on;
	}
	if (replystr[2] == '1')
		preamp_on = 1;
	else
		preamp_on = 0;
	return preamp_on;
}

void RIG_TS570::set_widths()
{
	switch (modeA) {
	case 0:
	case 1:
	case 3:
	case 4:
	bandwidths_ = TS570_SSBwidths;
	bwA = 1;
	break;
	case 2:
	case 6:
	bandwidths_ = TS570_CWwidths;
	bwA = 5;
	break;
	case 5:
	case 7:
	bandwidths_ = TS570_FSKwidths;
	bwA = 2;
	break;
	default:
	break;
	}
}

void RIG_TS570::set_modeA(int val)
{
	modeA = val;
	cmd = "MD";
	cmd += TS570_mode_chr[val];
	cmd += ';';
	sendTScommand(cmd, 4, false);
	set_widths();
}

int RIG_TS570::get_modeA()
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
	set_widths();
	return modeA;
}

int RIG_TS570::adjust_bandwidth(int val)
{
	return bwA;
}

void RIG_TS570::set_bwA(int val)
{
	bwA = val;

	switch (modeA) {
	case 0:
	case 1:
	case 3:
	case 4:
	sendTScommand(TS570_SSBbw[bwA], 5, false);
	break;
	case 2:
	case 6:
	sendTScommand(TS570_CWbw[bwA], 5, false);
	break;
	case 5:
	case 7:
	sendTScommand(TS570_FSKbw[bwA], 7, false);
	break;
	default:
	break;
	}
}

int RIG_TS570::get_bwA()
{
	int i;

	if (!sendTScommand("FW;", 7, false)) return bwA;
	if (replystr.find("FW") != 0) {
		clearSerialPort();
		return bwA;
	}

	switch (modeA) {
	case 0:
	case 1:
	case 3:
	case 4:
	for (i = 0; TS570_SSBbw[i] != NULL; i++)
		if (strncmp(replybuff, TS570_SSBbw[i], 7) == 0)  break;
	if (TS570_SSBbw[i] != NULL) bwA = i;
	else bwA = 1;
	break;
	case 2:
	case 6:
	for (i = 0; TS570_CWbw[i] != NULL; i++)
		if (strncmp(replybuff, TS570_CWbw[i], 7) == 0)  break;
	if (TS570_CWbw[i] != NULL) bwA = i;
	else bwA = 1;
	break;
	case 5:
	case 7:
	for (i = 0; TS570_FSKbw[i] != NULL; i++)
		if (strncmp(replybuff, TS570_FSKbw[i], 7) == 0)  break;
	if (TS570_FSKbw[i] != NULL) bwA = i;
	else bwA = 1;
	break;
	default:
	break;
	}

	return bwA;
}

int RIG_TS570::get_modetype(int n)
{
	return TS570_mode_type[n];
}

// val 0 .. 100
void RIG_TS570::set_mic_gain(int val)
{
	cmd = "MG000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += val % 10;
		val /= 10;
	}
	sendTScommand(cmd, 0, false);
}

int RIG_TS570::get_mic_gain()
{
	if (!sendTScommand("MG;", 6, false)) return 0;
	if (replystr.find("MG") != 0) {
		clearSerialPort();
		return 0;
	}
	replybuff[5] = 0;
	int val = atoi(&replybuff[2]);
	return val;
}

void RIG_TS570::get_mic_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_TS570::set_beatcancel(int val)
{
	if (val)
		cmd = "BC1;";
	else
		cmd = "BC0;";
	sendTScommand(cmd, 0, false);
}

int RIG_TS570::get_beatcancel()
{
	cmd = "BC;";
	if (!sendTScommand(cmd, 5, false) )
		return beatcancel_on;
	if (replystr.find("BC") != 0) {
		clearSerialPort();
		return beatcancel_on;
	}
	if (replystr[2] == '1')
		beatcancel_on = 1;
	else
		beatcancel_on = 0;
	return beatcancel_on;
}

