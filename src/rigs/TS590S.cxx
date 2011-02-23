/*
 * Kenwood TS590S driver
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */


#include "TS590S.h"
#include "support.h"

static const char TS590Sname_[] = "TS-590S";

static const char *TS590Smodes_[] = {
		"LSB", "USB", "CW", "FM", "AM", "FSK", "CW-R", "FSK-R", "LSB-D", "USB-D", "FM-D", NULL};
static const char TS590S_mode_chr[] =  { '1', '2', '3', '4', '5', '6', '7', '9', '1', '2', '4' };
static const char TS590S_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'L', 'L', 'U', 'L', 'U', 'U' };

/*
static const char *TS590S_SSBwidths[] = {
  "10",   "50",  "100",  "200",  "300",  "400", 
 "500",  "600",  "700",  "800",  "900", "1000",
"1400", "1600", "1800", "590S", "2200", "2400",
"2600", "2800", "3000", "3400", "4000", "5000", NULL};
static const char *TS590S_SSBbw[] = {
"SL00;", "SL01;", "SL02;", "SL03;", "SL04;", "SL05;", 
"SL06;", "SL07;", "SL08;", "SL09;", "SL10;", "SL11;",
"SH00;", "SH01;", "SH02;", "SH03;", "SH04;", "SH05;", 
"SH06;", "SH07;", "SH08;", "SH09;", "SH10;", "SH11;" };
*/
static const char *TS590S_SSBwidths[] = {
"400", "800", "1200", "1600", "2000", "2200", "2400", "2600", "2800", NULL};

static const char *TS590S_SSBlower[] = {
"SL11", "SL09", "SL07", "SL05", "SL03", "SL03", "SL03", "SL03", "SL03", NULL };

static const char *TS590S_SSBupper[] = {
"SH00", "SH01", "SH02", "SH03", "SH04", "SH05", "SH06", "SH07", "SH08", NULL };

static const char *TS590S_CWwidths[] = {
"50", "80", "100", "150", "200", 
"300", "400", "500", "600", "1000", 
"1500", NULL};
static const char *TS590S_CWbw[] = {
"FW0050;", "FW0080;", "FW0100;", "FW0150;", "FW0200;", 
"FW0300;", "FW0400;", "FW0500;", "FW0600;", "FW1000;", 
"FW1500;" };

static const char *TS590S_AMwidths[] = {
"10",  "100",  "200",  "500",
"2500", "3000", "4000", "5000", NULL  };
static const char *TS590S_AMbw[] = {
"SL00;", "SL01;", "SL02;", "SL03;",
"SH00;", "SH01;", "SH02;", "SH03;" };

static const char *TS590S_FSKwidths[] = {
"250", "500", "1000", "1500", NULL};
static const char *TS590S_FSKbw[] = {
"FW0000;", "FW0001;", "FW0002;", "FW0003;" };

RIG_TS590S::RIG_TS590S() {
// base class values	
	name_ = TS590Sname_;
	modes_ = TS590Smodes_;
	bandwidths_ = TS590S_SSBwidths;
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
	A.imode = 1;
	A.iBW = 8;
	B.imode = 1;
	B.iBW = 8;
	active_mode = 1;
	active_bandwidth = 8;

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

bool RIG_TS590S::sendTScommand(string cmd, int retnbr, bool loghex, bool debug = false)
{
	int ret = sendCommand(cmd, retnbr, loghex);
	if (!debug) return retnbr ? ret : retnbr;
	if (RigSerial.IsOpen()) {
		LOG_INFO("%s", cmd.c_str());
		if (retnbr)
			LOG_INFO("%s", replybuff);
		return ret;
	} else
		LOG_INFO("%s", cmd.c_str());
	return 0;
}

void RIG_TS590S::initialize()
{
	cmd = "FR0;"; sendTScommand(cmd, 4, false);
	cmd = "AC000;"; sendTScommand(cmd, 6, false);
	get_preamp();
	get_attenuator();
}

long RIG_TS590S::get_vfoA ()
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

void RIG_TS590S::set_vfoA (long freq)
{
	A.freq = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendTScommand(cmd, 0, false);
}

long RIG_TS590S::get_vfoB ()
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

void RIG_TS590S::set_vfoB (long freq)
{
	B.freq = freq;
	cmd = "FB00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendTScommand(cmd, 0, false);
}

int RIG_TS590S::get_smeter()
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

int RIG_TS590S::get_swr()
{
	cmd = "RM1;";
	if (sendTScommand(cmd, 8, false) == 8) {
		replybuff[7] = 0;
		int mtr = atoi(&replybuff[5]);
		mtr = (mtr * 100) / 30;
		return mtr;
	}
	return 0;
}

int RIG_TS590S::get_power_out()
{
	cmd = "SM0;";
	if (sendTScommand(cmd, 8, false) == 8) {
		replybuff[7] = 0;
		int mtr = atoi(&replybuff[5]);
		mtr = (mtr * 100) / 30;
		return mtr;
	}
	return 0;
}

// Transceiver power level
void RIG_TS590S::set_power_control(double val)
{
	int ival = (int)val;
	cmd = "PC000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ival % 10;
		ival /= 10;
	}
	sendTScommand(cmd, 0, false);
}

int RIG_TS590S::get_power_control()
{
	cmd = "PC;";
	if (sendTScommand(cmd, 6, false) == 6) {
		replybuff[5] = 0;
		int mtr = atoi(&replybuff[2]);
		return mtr;
	}
	return 0;
}

// Volume control return 0 ... 100
int RIG_TS590S::get_volume_control()
{
	cmd = "AG0;";
	if (sendTScommand(cmd, 8, false) == 8)  {
		cmd[7] = 0;
		int val = atoi(&replybuff[3]);
		return (int)(val / 2.55);
	}
	return 0;
}

void RIG_TS590S::set_volume_control(int val) 
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
void RIG_TS590S::set_PTT_control(int val)
{
	if (val) cmd = "TX1;";
	else	 cmd = "RX;";
	sendTScommand(cmd, 4, false);
}

void RIG_TS590S::tune_rig()
{
	cmd = "AC111;";
	sendTScommand(cmd, 0, false);
}

void RIG_TS590S::set_attenuator(int val)
{
	att_level = val;
	if (val) cmd = "RA01;";
	else     cmd = "RA00;";
	sendTScommand(cmd, 0, false);
}

int RIG_TS590S::get_attenuator()
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

void RIG_TS590S::set_preamp(int val)
{
	preamp_level = val;
	if (val) cmd = "PA1;";
	else     cmd = "PA0;";
	sendTScommand(cmd, 0, false);
}

int RIG_TS590S::get_preamp()
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

//======================================================================
// mode commands
//======================================================================

void RIG_TS590S::set_modeA(int val)
{
	active_mode = A.imode = val;
	cmd = "MD";
	cmd += TS590S_mode_chr[val];
	cmd += ';';
	sendTScommand(cmd, 4, false, true);
	if (val > 7) {
		data_mode = true;
		cmd = "DA1;";
	} else {
		data_mode = false;
		cmd = "DA0;";
	}
	sendTScommand(cmd, 4, false, true);

	set_widths();
}

int RIG_TS590S::get_modeA()
{
	int md = A.imode;
	if (sendTScommand("MD;", 4, false) == 4) {
		md = replybuff[2];
		md = md - '1';
		if (md == 8) md = 7;
	}
	if (sendTScommand("DA;", 4, false) == 4) {
		if (replybuff[2] == '1') {
			data_mode = true;
			if (md == 0) md = 8;
			if (md == 1) md = 9;
		}
	}
	if (md != A.imode) {
		active_mode = A.imode = md;
		set_widths();
	}
	return A.imode;
}

void RIG_TS590S::set_modeB(int val)
{
	active_mode = B.imode = val;
	cmd = "MD";
	cmd += TS590S_mode_chr[val];
	cmd += ';';
	sendTScommand(cmd, 4, false, true);
	if (val > 7) {
		data_mode = true;
		cmd = "DA1;";
	} else {
		data_mode = false;
		cmd = "DA0;";
	}
	sendTScommand(cmd, 4, false, true);
	set_widths();
}

int RIG_TS590S::get_modeB()
{
	int md = B.imode;
	if (sendTScommand("MD;", 4, false) == 4) {
		md = replybuff[2];
		md = md - '1';
		if (md == 8) md = 7;
	}
	if (sendTScommand("DA;", 4, false) == 4) {
		if (replybuff[2] == '1') {
			data_mode = true;
			if (md == 0) md = 8;
			if (md == 1) md = 9;
		}
	}
	if (md != B.imode) {
		active_mode = B.imode = md;
		set_widths();
	}
	return B.imode;
}

//======================================================================
// Bandpass filter commands
//======================================================================

void RIG_TS590S::set_widths()
{
	if (active_mode == 0 || active_mode == 1 || active_mode == 3 ||
		active_mode == 8 || active_mode == 9 || active_mode == 10) {
		bandwidths_ = TS590S_SSBwidths;
		active_bandwidth = 8;
	} else if (active_mode == 2 || active_mode == 6) {
		bandwidths_ = TS590S_CWwidths;
		active_bandwidth = 7;
	} else if (active_mode == 5 || active_mode == 7) {
		bandwidths_ = TS590S_FSKwidths;
		active_bandwidth = 1;
	} else {
		bandwidths_ = TS590S_AMwidths;
		active_bandwidth = 5;
	}
}

const char **RIG_TS590S::bwtable(int m)
{
	if (m == 0 || m == 1 || m == 3 || m == 8 || m == 9 || m == 10)
		return TS590S_SSBwidths;
	else if (m == 2 || m == 6)
		return TS590S_CWwidths;
	else if (m == 5 || m == 7)
		return TS590S_FSKwidths;
	return TS590S_AMwidths;
}

int RIG_TS590S::def_bandwidth(int val)
{
	if (val == 0 || val == 1 || val == 3 || val == 8 || val == 9 || val == 10)
		return 8;
	else if (val == 2 || val == 6)
		return 7;
	else if (val == 5 || val == 7)
		return 1;
	return 5;
}

int RIG_TS590S::adjust_bandwidth(int val)
{
	bandwidths_ = bwtable(val);
	return def_bandwidth(val);
}

void RIG_TS590S::set_active_bandwidth()
{
	if (active_mode == 0 || active_mode == 1 || active_mode == 3 ||
		active_mode == 8 || active_mode == 9 || active_mode == 10) {
		sendTScommand(TS590S_SSBlower[active_bandwidth], 5, false, true);
		sendTScommand(TS590S_SSBupper[active_bandwidth], 5, false, true);
	}
	else if (active_mode == 2 || active_mode == 6)
		sendTScommand(TS590S_CWbw[active_bandwidth], 5, false, true);
	else if (active_mode == 5 || active_mode == 7)
		sendTScommand(TS590S_FSKbw[active_bandwidth], 7, false, true);
	else
		sendTScommand(TS590S_AMbw[active_bandwidth], 5, false, true);
}

void RIG_TS590S::set_bwA(int val)
{
	active_bandwidth = A.iBW = val;
	set_active_bandwidth();
}

void RIG_TS590S::set_bwB(int val)
{
	active_bandwidth = B.iBW = val;
	set_active_bandwidth();
}

int RIG_TS590S::get_active_bandwidth()
{
	int i = 0;
	if (active_mode == 0 || active_mode == 1 || active_mode == 3) {
		if (sendTScommand("SH;", 5, false) == 5) {
			for (i = 0; i < 9; i++)
				if (strcmp(replybuff, TS590S_SSBupper[i]) == 0)
					break;
			if (i == 9) i = 8;
			active_bandwidth = i;
		}
	} else if (active_mode == 2) {
		if (sendTScommand("FW;", 7, false) == 7) {
			for (i = 0; i < 11; i++)
				if (strcmp(replybuff, TS590S_CWbw[i]) == 0)
					break;
			if (i == 11) i = 10;
			active_bandwidth = i;
		}
	} else if (active_mode == 5 || active_mode == 7) {
		if (sendTScommand("FW;", 7, false) == 7) {
			for (i = 0; i < 4; i++)
				if (strcmp(replybuff, TS590S_FSKbw[i]) == 0)
					break;
			if (i == 4) i = 3;
			active_bandwidth = i;
		}
	} else {
		if (sendTScommand("SL;", 5, false) == 5) {
			for (i = 0; i < 8; i++)
				if (strcmp(replybuff, TS590S_AMbw[i]) == 0)
					break;
			if (i == 8) i = 7;
			active_bandwidth = i;
		}
	}
	return active_bandwidth;
}

int RIG_TS590S::get_bwA()
{
	return (A.iBW = get_active_bandwidth());
}

int RIG_TS590S::get_bwB()
{
	return (B.iBW = get_active_bandwidth());
}

int RIG_TS590S::get_modetype(int n)
{
	return TS590S_mode_type[n];
}

// val 0 .. 100
void RIG_TS590S::set_mic_gain(int val)
{
	cmd = "MG000;";
	for (int i = 3; i > 0; i--) {
		cmd[1+i] += val % 10;
		val /= 10;
	}
	sendTScommand(cmd, 0, false);
}

int RIG_TS590S::get_mic_gain()
{
	int val = 0;
	if (sendTScommand("MG;", 6, false) == 6) {
		replybuff[5] = 0;
		val = atoi(&replybuff[2]);
	}
	return val;
}

void RIG_TS590S::get_mic_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_TS590S::set_noise(bool b)
{
	if (b)
		cmd = "NB1;";
	else
		cmd = "NB0;";
	sendTScommand(cmd, 0, false);
}

//======================================================================
// IF shift command only available if the transceiver is in the CW mode
// step size is 50 Hz
// same in 590S as 2000
// Use the SH / SL commands for slope tuning in the digital modes
// LSB-D, USB-D and FM-D
//======================================================================
void RIG_TS590S::set_if_shift(int val)
{
	if (active_mode == 2 || active_mode == 6) { // cw modes
		cmd = "IS 0000;";
		if (val < 0) cmd[3] = '-';
		val = abs(val);
		for (int i = 4; i > 0; i--) {
			cmd[3+i] += val % 10;
			val /= 10;
		}
		sendTScommand(cmd, 0, false);
	}
}

bool RIG_TS590S::get_if_shift(int &val)
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

void RIG_TS590S::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -1000;
	max = 1000;
	step = 100;
}

void RIG_TS590S::set_notch(bool on, int val)
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

bool  RIG_TS590S::get_notch(int &val)
{
	bool ison = false;
	cmd = "BP00;";
	if (sendTScommand(cmd, 8, false) == 8) {
		if (replybuff[6] == '1') {
			ison = true;
			cmd = "BP01;";
			if (sendTScommand(cmd, 8, false) == 8) {
				replybuff[7] = 0;
				val = atoi(&replybuff[4]);
				val -= 200;
				val *= -9;
			}
		}
	}
	return ison;
}

void RIG_TS590S::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = -1143;
	max = +1143;
	step = 9;
}

