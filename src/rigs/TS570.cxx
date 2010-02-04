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

static const char TS570name_[] = "TS-570D";
static const char TS570Sname_[] = "TS-570S";

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
	mode_ = 1;
	bw_ = 1;
	def_mode = 1;
	defbw_ = 1;
	deffreq_ = 14070000;

	has_micgain_control =
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
	if ((is_TS570S = get_ts570id()) == true)
	  name_ = TS570Sname_;
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
	if (sendTScommand(cmd, 14, false)) {
		int f = 0;
		for (size_t n = 2; n < 13; n++)
			f = f*10 + replybuff[n] - '0';
		freq_ = f;
	}
	return freq_;
}

void RIG_TS570::set_vfoA (long freq)
{
	freq_ = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendTScommand(cmd, 0, false);
}

int RIG_TS570::get_smeter()
{
	cmd = "SM;";
	if(sendTScommand(cmd, 7, false)) {
		replybuff[6] = 0;
		int mtr = atoi(&replybuff[2]);
		mtr = (mtr * 100) / 30;
		return mtr;
	}
	return 0;
}

int RIG_TS570::get_swr()
{
	cmd = "RM1;";
	if (sendTScommand(cmd, 8, false)) {
		replybuff[7] = 0;
		int mtr = atoi(&replybuff[3]);
		mtr = (mtr * 100) / 30;
	}
	return 0;
}

// Transceiver power level
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

int RIG_TS570::get_power_out()
{
	cmd = "PC;";
	if (sendTScommand(cmd, 6, false)) {
		replybuff[5] = 0;
		int mtr = atoi(&replybuff[2]);
		return mtr;
	}
	return 0;
}

int RIG_TS570::get_power_control()
{
	return 0;
}

// Volume control return 0 ... 100
int RIG_TS570::get_volume_control()
{
	cmd = "AG;";
	if (sendTScommand(cmd, 6, false))  {
		cmd[5] = 0;
		int val = atoi(&replybuff[2]);
		return (int)(val / 2.55);
	}
	return 0;
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
	else     cmd = "RA00;";
	sendTScommand(cmd, 0, false);
}

int RIG_TS570::get_attenuator()
{
	cmd = "RA;";
	if (sendTScommand(cmd, 7, false)) {
		if (replybuff[2] == '0' && replybuff[3] == '0')
			att_on = 0;
		else
			att_on = 1;
	}
	return att_on;
}

void RIG_TS570::set_preamp(int val)
{
	preamp_on = val;
	if (val) cmd = "PA1;";
	else     cmd = "PA0;";
	sendTScommand(cmd, 0, false);
}

int RIG_TS570::get_preamp()
{
	cmd = "PA;";
	if (sendTScommand(cmd, 5, false) ) {
		if (replystr[2] == '1') 
			preamp_on = 1;
		else
			preamp_on = 0;
	}
	return preamp_on;
}

void RIG_TS570::set_widths()
{
	if (mode_ == 0 || mode_ == 1 || mode_ == 3) {
		bandwidths_ = TS570_SSBwidths;
		bw_ = 1;
	} else if (mode_ == 2 || mode_ == 6) {
		bandwidths_ = TS570_CWwidths;
		bw_ = 5;
	} else if (mode_ == 5 || mode_ == 7) {
		bandwidths_ = TS570_FSKwidths;
		bw_ = 2;
	} else {
		bandwidths_ = TS570_SSBwidths;
		bw_ = 1;
	}
}

void RIG_TS570::set_mode(int val)
{
	mode_ = val;
	cmd = "MD";
	cmd += TS570_mode_chr[val];
	cmd += ';';
	sendTScommand(cmd, 4, false);
	set_widths();
}

int RIG_TS570::get_mode()
{
	if (sendTScommand("MD;", 4, false)) {
		int md = replybuff[2];
		md = md - '1';
		if (md == 8) md = 7;
		mode_ = md;
	}
	set_widths();
	return mode_;
}

int RIG_TS570::adjust_bandwidth(int val)
{
	return bw_;
}
	
void RIG_TS570::set_bandwidth(int val)
{
	bw_ = val;
	if (mode_ == 0 || mode_ == 1 || mode_ == 3)
		sendTScommand(TS570_SSBbw[bw_], 5, false);
	else if (mode_ == 2 || mode_ == 6)
		sendTScommand(TS570_CWbw[bw_], 5, false);
	else if (mode_ == 5 || mode_ == 7)
		sendTScommand(TS570_FSKbw[bw_], 7, false);
	else
		sendTScommand(TS570_SSBbw[bw_], 5, false);
}

int RIG_TS570::get_bandwidth()
{
  bw_ = 0;
  return bw_;
}
/*
int RIG_TS2000::get_bandwidth()
{
	int i = 0;
	if (mode_ == 0 || mode_ == 1 || mode_ == 3) {
		sendTScommand("SH;", 5, false);
		for (i = 0; i < 9; i++)
			if (strcmp(replybuff, TS2000_SSBupper[i]) == 0)
				break;
		if (i == 9) i = 8;
		bw_ = i;
	} else if (mode_ == 2) {
		sendTScommand("FW;", 7, false);
		for (i = 0; i < 11; i++)
			if (strcmp(replybuff, TS2000_CWbw[i]) == 0)
				break;
		if (i == 11) i = 10;
		bw_ = i;
	} else if (mode_ == 5 || mode_ == 7) {
		sendTScommand("FW;", 7, false);
		for (i = 0; i < 4; i++)
			if (strcmp(replybuff, TS2000_FSKbw[i]) == 0)
				break;
		if (i == 4) i = 3;
		bw_ = i;
	} else {
		sendTScommand("SL;", 5, false);
		for (i = 0; i < 8; i++)
			if (strcmp(replybuff, TS2000_AMbw[i]) == 0)
				break;
		if (i == 8) i = 7;
		bw_ = i;
	}
	return bw_;
}
*/

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
	sendTScommand("MG;", 6, false);
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
	if (sendTScommand(cmd, 5, false) ) {
		if (replystr[2] == '1') 
			beatcancel_on = 1;
		else
			beatcancel_on = 0;
	}
	return beatcancel_on;
}

/*
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
	sendTScommand(cmd, 0, false);
}

bool RIG_TS2000::get_if_shift(int &val)
{
	static int oldval = 0;
	sendTScommand("IS;", 8, false);
	replybuff[8] = 0;
	val = atoi(&replybuff[3]);
	if (val != 0 || oldval != val) {
		oldval = val;
		return true;
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
		sendTScommand(cmd, 0, false);
		notch_on = false;
		return;
	}
	if (!notch_on) {
		cmd[6] = '1'; // notch ON
		sendTScommand(cmd, 0, false);
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
	sendTScommand(cmd, 0, false);
}

bool  RIG_TS2000::get_notch(int &val)
{
	bool ison = false;
	cmd = "BP00;";
	if (sendTScommand(cmd, 8, false)) {
		if (replybuff[6] == '1') {
			ison = true;
			cmd = "BP01;";
			if (sendTScommand(cmd, 8, false)) {
				replybuff[7] = 0;
				val = atoi(&replybuff[4]);
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
*/

