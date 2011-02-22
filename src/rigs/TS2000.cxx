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

static const char *TS2000_SSBwidths[] = {
"400", "800", "1200", "1600", "2000", "2200", "2400", "2600", "2800", NULL};

static const char *TS2000_SSBlower[] = {
"SL11;", "SL09;", "SL07;", "SL05;", "SL03;", "SL03;", "SL03;", "SL03;", "SL03;", NULL };

static const char *TS2000_SSBupper[] = {
"SH00;", "SH01;", "SH02;", "SH03;", "SH04;", "SH05;", "SH06;", "SH07;", "SH08;", NULL };

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
	B.imode = A.imode = 1;
	B.iBW = A.iBW = 8;
	B.freq = A.freq = 14070000;

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
	if (sendTScommand(cmd, 14, false) == 14) {
		int f = 0;
		for (size_t n = 2; n < 13; n++)
			f = f*10 + replybuff[n] - '0';
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
if (RIG_DEBUG)
	LOG_INFO("%s", cmd.c_str());
	sendTScommand(cmd, 0, false);
}

long RIG_TS2000::get_vfoB ()
{
	cmd = "FB;";
	if (sendTScommand(cmd, 14, false) == 14) {
		int f = 0;
		for (size_t n = 2; n < 13; n++)
			f = f*10 + replybuff[n] - '0';
		B.freq = f;
	}
	return B.freq;
}

void RIG_TS2000::set_vfoB (long freq)
{
	B.freq = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
if (RIG_DEBUG)
	LOG_INFO("%s", cmd.c_str());
	sendTScommand(cmd, 0, false);
}

int RIG_TS2000::get_smeter()
{
	cmd = "SM0;";
	if(sendTScommand(cmd, 8, false) == 8) {
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
	if (sendTScommand(cmd, 8, false) == 8) {
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
if (RIG_DEBUG)
	LOG_INFO("%s", cmd.c_str());
	sendTScommand(cmd, 0, false);
}

int RIG_TS2000::get_power_out()
{
	cmd = "PC;";
	if (sendTScommand(cmd, 6, false) == 6) {
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
	if (sendTScommand(cmd, 8, false) == 8)  {
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
if (RIG_DEBUG)
	LOG_INFO("%s", cmd.c_str());
	sendTScommand(cmd, 0, false);
}

// Tranceiver PTT on/off
void RIG_TS2000::set_PTT_control(int val)
{
	if (val) cmd = "TX;";
	else	 cmd = "RX;";
if (RIG_DEBUG)
	LOG_INFO("%s", cmd.c_str());
	sendTScommand(cmd, 4, false);
}

void RIG_TS2000::tune_rig()
{
	cmd = "AC111;";
if (RIG_DEBUG)
	LOG_INFO("%s", cmd.c_str());
	sendTScommand(cmd, 0, false);
}

void RIG_TS2000::set_attenuator(int val)
{
	att_level = val;
	if (val) cmd = "RA01;";
	else     cmd = "RA00;";
if (RIG_DEBUG)
	LOG_INFO("%s", cmd.c_str());
	sendTScommand(cmd, 0, false);
}

int RIG_TS2000::get_attenuator()
{
	cmd = "RA;";
	if (sendTScommand(cmd, 7, false) == 7) {
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
if (RIG_DEBUG)
	LOG_INFO("%s", cmd.c_str());
	sendTScommand(cmd, 0, false);
}

int RIG_TS2000::get_preamp()
{
	cmd = "PA;";
	if (sendTScommand(cmd, 5, false) == 5) {
		if (replystr[2] == '1') 
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
		bandwidths_ = TS2000_SSBwidths;
		bw = 8;
	} else if (val == 2 || val == 6) {
		bandwidths_ = TS2000_CWwidths;
		bw = 7;
	} else if (val == 5 || val == 7) {
		bandwidths_ = TS2000_FSKwidths;
		bw = 1;
	} else {
		bandwidths_ = TS2000_AMwidths;
		bw = 5;
	}
	return bw;
}

const char **RIG_TS2000::bwtable(int m)
{
	if (m == 0 || m == 1 || m == 3)
		return TS2000_SSBwidths;
	else if (m == 2 || m == 6)
		return TS2000_CWwidths;
	else if (m == 5 || m == 7)
		return TS2000_FSKwidths;
	return TS2000_AMwidths;
}

void RIG_TS2000::set_modeA(int val)
{
	A.imode = val;
	cmd = "MD";
	cmd += TS2000_mode_chr[val];
	cmd += ';';
if (RIG_DEBUG)
	LOG_INFO("%s", cmd.c_str());
	sendTScommand(cmd, 4, false);
	A.iBW = set_widths(val);
}

int RIG_TS2000::get_modeA()
{
	if (sendTScommand("MD;", 4, false) == 4) {
		int md = replybuff[2];
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
if (RIG_DEBUG)
	LOG_INFO("%s", cmd.c_str());
	sendTScommand(cmd, 4, false);
	B.iBW = set_widths(val);
}

int RIG_TS2000::get_modeB()
{
	if (sendTScommand("MD;", 4, false) == 4) {
		int md = replybuff[2];
		md = md - '1';
		if (md == 8) md = 7;
		B.imode = md;
		B.iBW = set_widths(B.imode);
	}
	return B.imode;
}

int RIG_TS2000::adjust_bandwidth(int val)
{
	int bw;
	if (val == 0 || val == 1 || val == 3) {
		bw = 8;
	} else if (val == 2 || val == 6) {
		bw = 7;
	} else if (val == 5 || val == 7) {
		bw = 1;
	} else {
		bw = 5;
	}
	return bw;
}

int RIG_TS2000::def_bandwidth(int val)
{
	return adjust_bandwidth(val);
}

void RIG_TS2000::set_bwA(int val)
{
	A.iBW = val;
	string sent = "";
	if (A.imode == 0 || A.imode == 1 || A.imode == 3) {
		cmd = TS2000_SSBlower[A.iBW];
		sendTScommand(cmd, 5, false);
		sent.append(cmd);
		cmd = TS2000_SSBupper[A.iBW];
		sendTScommand(cmd, 5, false);
		sent.append(cmd);
	}
	else if (A.imode == 2 || A.imode == 6) {
		cmd = TS2000_CWbw[A.iBW];
		sendTScommand(cmd, 5, false);
		sent.append(cmd);
	}else if (A.imode == 5 || A.imode == 7) {
		cmd = TS2000_FSKbw[A.iBW];
		sendTScommand(cmd, 7, false);
		sent.append(cmd);
	} else {
		cmd = TS2000_AMbw[A.iBW];
		sendTScommand(cmd, 5, false);
		sent.append(cmd);
	}
if (RIG_DEBUG)
	LOG_INFO("%s", sent.c_str());
}

int RIG_TS2000::get_bwA()
{
	int i = 0;
	if (A.imode == 0 || A.imode == 1 || A.imode == 3) {
		if (sendTScommand("SH;", 5, false) == 5) {
			for (i = 0; i < 9; i++)
				if (strcmp(replybuff, TS2000_SSBupper[i]) == 0)
					break;
			if (i == 9) i = 8;
			A.iBW = i;
		}
	} else if (A.imode == 2) {
		if (sendTScommand("FW;", 7, false) == 7) {
			for (i = 0; i < 11; i++)
				if (strcmp(replybuff, TS2000_CWbw[i]) == 0)
					break;
			if (i == 11) i = 10;
			A.iBW = i;
		}
	} else if (A.imode == 5 || A.imode == 7) {
		if (sendTScommand("FW;", 7, false) == 7) {
			for (i = 0; i < 4; i++)
				if (strcmp(replybuff, TS2000_FSKbw[i]) == 0)
					break;
			if (i == 4) i = 3;
			A.iBW = i;
		}
	} else {
		if (sendTScommand("SL;", 5, false) == 5) {
			for (i = 0; i < 8; i++)
				if (strcmp(replybuff, TS2000_AMbw[i]) == 0)
					break;
			if (i == 8) i = 7;
			A.iBW = i;
		}
	}
	return A.iBW;
}

void RIG_TS2000::set_bwB(int val)
{
	B.iBW = val;
	string sent = "";
	if (B.imode == 0 || B.imode == 1 || B.imode == 3) {
		cmd = TS2000_SSBlower[B.iBW];
		sendTScommand(cmd, 5, false);
		sent.append(cmd);
		cmd = TS2000_SSBupper[B.iBW];
		sendTScommand(cmd, 5, false);
		sent.append(cmd);
	}
	else if (A.imode == 2 || B.imode == 6) {
		cmd = TS2000_CWbw[B.iBW];
		sendTScommand(cmd, 5, false);
		sent.append(cmd);
	}else if (B.imode == 5 || B.imode == 7) {
		cmd = TS2000_FSKbw[B.iBW];
		sendTScommand(cmd, 7, false);
		sent.append(cmd);
	} else {
		cmd = TS2000_AMbw[B.iBW];
		sendTScommand(cmd, 5, false);
		sent.append(cmd);
	}
if (RIG_DEBUG)
	LOG_INFO("%s", sent.c_str());
}

int RIG_TS2000::get_bwB()
{
	int i = 0;
	if (B.imode == 0 || B.imode == 1 || B.imode == 3) {
		if (sendTScommand("SH;", 5, false) == 5) {
			for (i = 0; i < 9; i++)
				if (strcmp(replybuff, TS2000_SSBupper[i]) == 0)
					break;
			if (i == 9) i = 8;
			B.iBW = i;
		}
	} else if (B.imode == 2) {
		if (sendTScommand("FW;", 7, false) == 7) {
			for (i = 0; i < 11; i++)
				if (strcmp(replybuff, TS2000_CWbw[i]) == 0)
					break;
			if (i == 11) i = 10;
			B.iBW = i;
		}
	} else if (B.imode == 5 || B.imode == 7) {
		if (sendTScommand("FW;", 7, false) == 7) {
			for (i = 0; i < 4; i++)
				if (strcmp(replybuff, TS2000_FSKbw[i]) == 0)
					break;
			if (i == 4) i = 3;
			B.iBW = i;
		}
	} else {
		if (sendTScommand("SL;", 5, false) == 5) {
			for (i = 0; i < 8; i++)
				if (strcmp(replybuff, TS2000_AMbw[i]) == 0)
					break;
			if (i == 8) i = 7;
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
if (RIG_DEBUG)
	LOG_INFO("%s", cmd.c_str());
	sendTScommand(cmd, 0, false);
}

int RIG_TS2000::get_mic_gain()
{
	if (sendTScommand("MG;", 6, false) == 6) {
		replybuff[5] = 0;
		int val = atoi(&replybuff[2]);
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
if (RIG_DEBUG)
	LOG_INFO("%s", cmd.c_str());
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
if (RIG_DEBUG)
	LOG_INFO("%s", cmd.c_str());
	sendTScommand(cmd, 0, false);
}

bool RIG_TS2000::get_if_shift(int &val)
{
	static int oldval = 0;
	if (sendTScommand("IS;", 8, false) == 8) {
		replybuff[8] = 0;
		val = atoi(&replybuff[3]);
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
if (RIG_DEBUG)
	LOG_INFO("%s", cmd.c_str());
		sendTScommand(cmd, 0, false);
		notch_on = false;
		return;
	}
	if (!notch_on) {
		cmd[6] = '1'; // notch ON
if (RIG_DEBUG)
	LOG_INFO("%s", cmd.c_str());
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
if (RIG_DEBUG)
	LOG_INFO("%s", cmd.c_str());
	sendTScommand(cmd, 0, false);
}

bool  RIG_TS2000::get_notch(int &val)
{
	bool ison = false;
	cmd = "BP00;";
	if (sendTScommand(cmd, 8, false) == 8) {
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

