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

void RIG_TS570::initialize()
{
	cmd = "FR0;"; sendCommand(cmd, 0);
	cmd = "FT0;"; sendCommand(cmd, 0);
	cmd = "AC001;"; sendCommand(cmd, 0);
	get_preamp();
	get_attenuator();
	is_TS570S = get_ts570id();
}

bool RIG_TS570::get_ts570id()
{
	cmd = "ID;";
	int ret = sendCommand(cmd);
	if (ret < 6) return false;
	size_t p = replystr.rfind("ID");
	if (p == string::npos) return false;
	if (replystr[p + 3] == '1' && 
		replystr[p + 4] == '8')  return true;
	return false;
}

long RIG_TS570::get_vfoA ()
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

void RIG_TS570::set_vfoA (long freq)
{
	freqA = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd, 0);
}

// SM cmd 0 ... 100 (rig values 0 ... 15)
int RIG_TS570::get_smeter()
{
	cmd = "SM;";
	int ret = sendCommand(cmd);
	if (ret < 7) return 0;
	size_t p = replystr.rfind("SM");
	if (p == string::npos) return 0;
	
	replystr[p + 6] = 0;
	int mtr = atoi(&replystr[p + 2]);
	mtr = (mtr * 100) / 16;
	return mtr;
}

// RM cmd 0 ... 100 (rig values 0 ... 8)
int RIG_TS570::get_swr()
{
	cmd = "RM1;RM;"; // select measurement '1' (swr) and read meter
	int ret = sendCommand(cmd);
	if (ret < 8) return 0;
	size_t p = replystr.rfind("RM");
	if (p == string::npos) return 0;
	
	replystr[p + 7] = 0;
	int mtr = atoi(&replystr[p + 3]);
	mtr = (mtr * 100) / 9;
	return mtr;
}

// power output measurement 0 ... 15
int RIG_TS570::get_power_out()
{
	cmd = "SM;";
	int ret = sendCommand(cmd);
	if (ret < 6) return 0;
	size_t p = replystr.rfind("SM");
	if (p == string::npos) return 0;
	
	replystr[p + 5] = 0;
	int mtr = atoi(&replystr[p + 2]);
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
	sendCommand(cmd, 0);
}

int RIG_TS570::get_power_control()
{
	cmd = "PC;";
	int ret = sendCommand(cmd);
	if (ret < 6) return 0;
	size_t p = replystr.rfind("PC");
	if (p == string::npos) return 0;
	
	replystr[p + 5] = 0;
	int mtr = atoi(&replystr[p + 2]);
	return mtr;
}

// Volume control return 0 ... 100  (rig values 0 ... 255)
int RIG_TS570::get_volume_control()
{
	cmd = "AG;";
	int ret = sendCommand(cmd);
	if (ret < 6) return 0;
	size_t p = replystr.rfind("AG");
	if (p == string::npos) return 0;
	
	replystr[p + 5] = 0;
	int val = atoi(&replystr[p + 2]);
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
	sendCommand(cmd, 0);
}

// Tranceiver PTT on/off
void RIG_TS570::set_PTT_control(int val)
{
	if (val) cmd = "TX;";
	else	 cmd = "RX;";
	sendCommand(cmd, 0);
}

void RIG_TS570::tune_rig()
{
	cmd = "AC 11;";
	sendCommand(cmd, 0);
}

void RIG_TS570::set_attenuator(int val)
{
	att_on = val;
	if (val) cmd = "RA01;";
	else	 cmd = "RA00;";
	sendCommand(cmd, 0);
}

int RIG_TS570::get_attenuator()
{
	cmd = "RA;";
	int ret = sendCommand(cmd);
	if (ret < 7) return att_on;
	size_t p = replystr.rfind("RA");
	if (p == string::npos) return att_on;

	if (replystr[p + 2] == '0' && 
		replystr[p + 3] == '0')
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
	sendCommand(cmd, 0);
}

int RIG_TS570::get_preamp()
{
	cmd = "PA;";
	int ret = sendCommand(cmd);
	if (ret < 5 ) return preamp_on;
	size_t p = replystr.rfind("PA");
	if (p == string::npos) return preamp_on;

	if (replystr[p + 2] == '1')
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

const char **RIG_TS570::bwtable(int m)
{
	switch (m) {
		case 0:
		case 1:
		case 3:
		case 4:
			return TS570_SSBwidths;
			break;
		case 2:
		case 6:
			return TS570_CWwidths;
			break;
		case 5:
		case 7:
			return TS570_FSKwidths;
			break;
	}
	return TS570_SSBwidths;
}

void RIG_TS570::set_modeA(int val)
{
	modeA = val;
	cmd = "MD";
	cmd += TS570_mode_chr[val];
	cmd += ';';
	sendCommand(cmd, 0);
	set_widths();
}

int RIG_TS570::get_modeA()
{
	cmd = "MD;";
	int ret = sendCommand(cmd);
	if (ret < 4) return modeA;
	size_t p = replystr.rfind("MD");
	if (p == string::npos) return modeA;

	int md = replystr[p + 2];
	md = md - '1';
	if (md == 8) md = 7;
	modeA = md;
	set_widths();
	return modeA;
}

int RIG_TS570::adjust_bandwidth(int val)
{
	switch (val) {
	case 0:
	case 1:
	case 3:
	case 4:
		bandwidths_ = TS570_SSBwidths;
		return 1;
	case 2:
	case 6:
		bandwidths_ = TS570_CWwidths;
		return 5;
	case 5:
	case 7:
		bandwidths_ = TS570_FSKwidths;
		return 2;
	}
	return 1;
}

int RIG_TS570::def_bandwidth(int val)
{
	switch (val) {
	case 0:
	case 1:
	case 3:
	case 4:
		return 1;
	case 2:
	case 6:
		return 5;
	case 5:
	case 7:
		return 2;
	}
	return 2;
}

void RIG_TS570::set_bwA(int val)
{
	bwA = val;

	switch (modeA) {
	case 0:
	case 1:
	case 3:
	case 4:
	sendCommand(TS570_SSBbw[bwA], 0);
	break;
	case 2:
	case 6:
	sendCommand(TS570_CWbw[bwA], 0);
	break;
	case 5:
	case 7:
	sendCommand(TS570_FSKbw[bwA], 0);
	break;
	default:
	break;
	}
}

int RIG_TS570::get_bwA()
{
	int i;

	cmd = "FW;";
	int ret = sendCommand(cmd);
	if (ret < 7) return bwA;
	size_t p = replystr.rfind("FW");
	if (p == string::npos) return bwA;
	string test = replystr.substr(p+2);
	
	switch (modeA) {
	case 0:
	case 1:
	case 3:
	case 4:
		for (i = 0; TS570_SSBbw[i] != NULL; i++)
			if (test.find(TS570_SSBbw[i]) == 0)  break;
		if (TS570_SSBbw[i] != NULL) bwA = i;
		else bwA = 1;
		break;
	case 2:
	case 6:
		for (i = 0; TS570_CWbw[i] != NULL; i++)
			if (test.rfind(TS570_CWbw[i]) == 0)  break;
		if (TS570_CWbw[i] != NULL) bwA = i;
		else bwA = 1;
		break;
	case 5:
	case 7:
		for (i = 0; TS570_FSKbw[i] != NULL; i++)
			if (test.rfind(TS570_FSKbw[i]) == 0)  break;
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
	sendCommand(cmd, 0);
}

int RIG_TS570::get_mic_gain()
{
	cmd = "MG;";
	int ret = sendCommand(cmd);
	if (ret < 6) return 0;
	size_t p = replystr.rfind("MG");
	if (p == string::npos) return 0;

	replystr[p + 5] = 0;
	int val = atoi(&replystr[p + 2]);
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
	sendCommand(cmd, 0);
}

int RIG_TS570::get_beatcancel()
{
	cmd = "BC;";
	int ret = sendCommand(cmd);
	if (ret < 5) return beatcancel_on;
	if (replystr.rfind("BC") == string::npos) return beatcancel_on;

	if (replystr[ret - 5 + 2] == '1')
		beatcancel_on = 1;
	else
		beatcancel_on = 0;
	return beatcancel_on;
}

