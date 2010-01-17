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

/*
static const char *TS2000_SSBwidths[] = {
  "10",   "50",  "100",  "200",  "300",  "400", 
 "500",  "600",  "700",  "800",  "900", "1000",
"1400", "1600", "1800", "2000", "2200", "2400",
"2600", "2800", "3000", "3400", "4000", "5000", NULL};
static const char *TS2000_SSBbw[] = {
"SL00;", "SL01;", "SL02;", "SL03;", "SL04;", "SL05;", 
"SL06;", "SL07;", "SL08;", "SL09;", "SL10;", "SL11;",
"SH00;", "SH01;", "SH02;", "SH03;", "SH04;", "SH05;", 
"SH06;", "SH07;", "SH08;", "SH09;", "SH10;", "SH11;" };
*/
static const char *TS2000_SSBwidths[] = {
"400", "800", "1200", "1600", "2000", "2200", "2400", "2600", "2800", NULL};

static const char *TS2000_SSBlower[] = {
"SL11", "SL09", "SL07", "SL05", "SL03", "SL03", "SL03", "SL03", "SL03", NULL };

static const char *TS2000_SSBupper[] = {
"SH00", "SH01", "SH02", "SH03", "SH04", "SH05", "SH06", "SH07", "SH08", NULL };

static const char *TS2000_CWwidths[] = {
"50", "80", "100", "150", "200", 
"300", "400", "500", "600", "1000", 
"1500", NULL};
static const char *TS2000_CWbw[] = {
"FW0050;", "FW0080;", "FW0100;", "FW0150;", "FW0200;", 
"FW0300;", "FW0400;", "FW0500;", "FW0600;", "FW1000;", 
"FW1500;" };

static const char *TS2000_AMwidths[] = {
"10",  "100",  "200",  "500",
"2500", "3000", "4000", "5000", NULL  };
static const char *TS2000_AMbw[] = {
"SL00;", "SL01;", "SL02;", "SL03;",
"SH00;", "SH01;", "SH02;", "SH03;" };

static const char *TS2000_FSKwidths[] = {
"250", "500", "1000", "1500", NULL};
static const char *TS2000_FSKbw[] = {
"FW0000;", "FW0001;", "FW0002;", "FW0003;" };

RIG_TS2000::RIG_TS2000() {
// base class values	
	name_ = TS2000name_;
	modes_ = TS2000modes_;
	bandwidths_ = TS2000_SSBwidths;
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
	bw_ = 8;
	def_mode = 1;
	defbw_ = 8;
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

bool RIG_TS2000::sendTScommand(string cmd, int retnbr, bool loghex)
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

void RIG_TS2000::initialize()
{
	cmd = "FR0;"; sendTScommand(cmd, 4, false);
	cmd = "FT0;"; sendTScommand(cmd, 4, false);
	cmd = "AC001;"; sendTScommand(cmd, 6, false);
	get_preamp();
	get_attenuator();
}

long RIG_TS2000::get_vfoA ()
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

void RIG_TS2000::set_vfoA (long freq)
{
	freq_ = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendTScommand(cmd, 0, false);
}

int RIG_TS2000::get_smeter()
{
	cmd = "SM0;";
	if(sendTScommand(cmd, 8, false)) {
		replybuff[7] = 0;
		int mtr = atoi(&replybuff[5]);
		mtr = (mtr * 100) / 30;
		return mtr;
	}
	return 0;
}

int RIG_TS2000::get_swr()
{
	cmd = "RM1;";
	if (sendTScommand(cmd, 8, false)) {
		replybuff[7] = 0;
		int mtr = atoi(&replybuff[5]);
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
	sendTScommand(cmd, 0, false);
}

int RIG_TS2000::get_power_out()
{
	cmd = "PC;";
	if (sendTScommand(cmd, 6, false)) {
		replybuff[5] = 0;
		int mtr = atoi(&replybuff[2]);
		return mtr;
	}
	return 0;
}

int RIG_TS2000::get_power_control()
{
	return 0;
}

// Volume control return 0 ... 100
int RIG_TS2000::get_volume_control()
{
	cmd = "AG0;";
	if (sendTScommand(cmd, 8, false))  {
		cmd[7] = 0;
		int val = atoi(&replybuff[3]);
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
	sendTScommand(cmd, 0, false);
}

// Tranceiver PTT on/off
void RIG_TS2000::set_PTT_control(int val)
{
	if (val) cmd = "TX;";
	else	 cmd = "RX;";
	sendTScommand(cmd, 4, false);
}

void RIG_TS2000::tune_rig()
{
	cmd = "AC111;";
	sendTScommand(cmd, 0, false);
}

void RIG_TS2000::set_attenuator(int val)
{
	att_level = val;
	if (val) cmd = "RA01;";
	else     cmd = "RA00;";
	sendTScommand(cmd, 0, false);
}

int RIG_TS2000::get_attenuator()
{
	cmd = "RA;";
	if (sendTScommand(cmd, 7, false)) {
		if (replybuff[2] == '0' && replybuff[3] == '0')
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
	sendTScommand(cmd, 0, false);
}

int RIG_TS2000::get_preamp()
{
	cmd = "PA;";
	if (sendTScommand(cmd, 5, false) ) {
		if (replystr[2] == '1') 
			preamp_level = 1;
		else
			preamp_level = 0;
	}
	return preamp_level;
}

void RIG_TS2000::set_widths()
{
	if (mode_ == 0 || mode_ == 1 || mode_ == 3) {
		bandwidths_ = TS2000_SSBwidths;
		bw_ = 8;
	} else if (mode_ == 2 || mode_ == 6) {
		bandwidths_ = TS2000_CWwidths;
		bw_ = 7;
	} else if (mode_ == 5 || mode_ == 7) {
		bandwidths_ = TS2000_FSKwidths;
		bw_ = 1;
	} else {
		bandwidths_ = TS2000_AMwidths;
		bw_ = 5;
	}
}

void RIG_TS2000::set_mode(int val)
{
	mode_ = val;
	cmd = "MD";
	cmd += TS2000_mode_chr[val];
	cmd += ';';
	sendTScommand(cmd, 4, false);
	set_widths();
}

int RIG_TS2000::get_mode()
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

int RIG_TS2000::adjust_bandwidth(int val)
{
	return bw_;
}
	
void RIG_TS2000::set_bandwidth(int val)
{
	bw_ = val;
	if (mode_ == 0 || mode_ == 1 || mode_ == 3) {
		sendTScommand(TS2000_SSBlower[bw_], 5, false);
		sendTScommand(TS2000_SSBupper[bw_], 5, false);
	}
	else if (mode_ == 2 || mode_ == 6)
		sendTScommand(TS2000_CWbw[bw_], 5, false);
	else if (mode_ == 5 || mode_ == 7)
		sendTScommand(TS2000_FSKbw[bw_], 7, false);
	else
		sendTScommand(TS2000_AMbw[bw_], 5, false);
}

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

int RIG_TS2000::get_modetype(int n)
{
	return TS2000_mode_type[n];
}

// val 0 .. 100
void RIG_TS2000::set_mic_gain(int val)
{
	cmd = "MG000;";
	val = (int)(val * 2.55); // convert to 0 .. 255
	for (int i = 3; i > 0; i--) {
		cmd[1+i] += val % 10;
		val /= 10;
	}
	sendTScommand(cmd, 0, false);
}

int RIG_TS2000::get_mic_gain()
{
	sendTScommand("MG;", 6, false);
	replybuff[5] = 0;
	int val = atoi(&replybuff[2]);
	return val;
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
	sendTScommand(cmd, 0, false);
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

