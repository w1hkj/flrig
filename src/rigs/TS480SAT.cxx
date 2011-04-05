/*
 * Kenwood TS480SAT driver
 * originally based on Kenwood TS2000 driver
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */


#include "TS480SAT.h"
#include "support.h"

static const char TS480SATname_[] = "TS-480SAT";

static const char *TS480SATmodes_[] = {
		"LSB", "USB", "CW", "FM", "AM", "FSK", "CW-R", "FSK-R", NULL};
static const char TS480SAT_mode_chr[] =  { '1', '2', '3', '4', '5', '6', '7', '9' };
static const char TS480SAT_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'L', 'L', 'U' };

static const char *TS480SAT_widths[] = {
"50", "100", "250", "500", "1000", "1500", "2400", NULL};

RIG_TS480SAT::RIG_TS480SAT() {
// base class values
	name_ = TS480SATname_;
	modes_ = TS480SATmodes_;
	_mode_type = TS480SAT_mode_type;
	bandwidths_ = TS480SAT_widths;
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
	freqA = 14070000;
	modeA = 1;
	bwA = 6;
	freqB = 7035000;
	modeB = 1;
	bwB = 6;
	def_mode = 1;
	defbw_ = 1;
	deffreq_ = 14070000;

	has_noise_control =
	has_micgain_control =
	has_tune_control =
	has_preamp_control =
	has_notch_control =
	has_ifshift_control =
	has_swr_control = false;

	has_attenuator_control =
	has_mode_control =
	has_bandwidth_control =
	has_volume_control =
	has_power_control =
	has_tune_control =
	has_ptt_control = true;
}

bool RIG_TS480SAT::sendTScommand(string cmd, int retnbr)
{
	int ret = sendCommand(cmd, retnbr, false);
	if (RigSerial.IsOpen())
		LOG_INFO("%s\n%s", cmd.c_str(), replystr.c_str());
	else
		LOG_INFO("%s", cmd.c_str());
	return ret;
}

void RIG_TS480SAT::initialize()
{
	selectA();
	cmd = "AC000;"; 
	sendTScommand(cmd, 0);
	cmd = "EX04500001;"; // set bandwidth controls for data modes
	LOG_WARN("%s", cmd.c_str());
	sendTScommand(cmd, 0);
	cmd = "SH01"; // set center frequency to 1500
	LOG_WARN("%s", cmd.c_str());
	sendTScommand(cmd, 0);
}

void RIG_TS480SAT::selectA()
{
	cmd = "FR0;";
	LOG_WARN("%s", cmd.c_str());
	sendTScommand(cmd, 0);
	cmd = "FT0;";
	LOG_WARN("%s", cmd.c_str());
	sendTScommand(cmd, 0);
}

void RIG_TS480SAT::selectB()
{
	cmd = "FR1;";
	LOG_WARN("%s", cmd.c_str());
	sendTScommand(cmd, 0);
	cmd = "FT1;";
	LOG_WARN("%s", cmd.c_str());
	sendTScommand(cmd, 0);
}

void RIG_TS480SAT::set_split(bool val) 
{
	split = val;
	if (val) {
		cmd = "FR0;";
	LOG_WARN("%s", cmd.c_str());
		sendTScommand(cmd, 0);
		cmd = "FT1;";
	LOG_WARN("%s", cmd.c_str());
		sendTScommand(cmd, 0);
	} else {
		cmd = "FR0;";
	LOG_WARN("%s", cmd.c_str());
		sendTScommand(cmd, 0);
		cmd = "FT0;";
	LOG_WARN("%s", cmd.c_str());
		sendTScommand(cmd, 0);
	}
}

long RIG_TS480SAT::get_vfoA ()
{
	cmd = "FA;";
	if (!sendTScommand(cmd, 14))
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

void RIG_TS480SAT::set_vfoA (long freq)
{
	freqA = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	LOG_WARN("%s", cmd.c_str());
	sendTScommand(cmd, 0);;
}

long RIG_TS480SAT::get_vfoB ()
{
	cmd = "FB;";
	if (sendTScommand(cmd, 14) == 14) {
		long f = 0L;
		long mul = 1L;
		for (size_t n = 12; n > 1; n--) {
			f += (replybuff[n] - '0') * mul;
			mul *= 10;
		}
		B.freq = f;
	}
	return B.freq;
}

void RIG_TS480SAT::set_vfoB (long freq)
{
	B.freq = freq;
	cmd = "FB00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	LOG_WARN("%s", cmd.c_str());
	sendTScommand(cmd, 0);
}

// SM cmd 0 ... 100 (rig values 0 ... 15)
int RIG_TS480SAT::get_smeter()
{
	cmd = "SM0;";
	if(!sendTScommand(cmd, 8))
		return 0;
	if (replystr.find("SM") != 0) {
		clearSerialPort();
		return 0;
	}
	replybuff[7] = 0;
	int mtr = atoi(&replybuff[3]);
	mtr = (mtr * 100) / 20;
	return mtr;
}

// RM cmd 0 ... 100 (rig values 0 ... 8)
int RIG_TS480SAT::get_swr()
{
	return -1; // disable to see if beeps go away
	int mtr = 0;
	cmd = "RM1;"; // select measurement '1' (swr) and read meter
	if (sendTScommand(cmd, 8) == 8) {
		replybuff[7] = 0;
		mtr = atoi(&replybuff[4]);
		mtr *= 10;
	}
	return mtr;
}


// Tranceiver PTT on/off
void RIG_TS480SAT::set_PTT_control(int val)
{
	if (val)	cmd = "TX1;"; // DTS transmission using ANI input
	else	 	cmd = "RX;";
	LOG_WARN("%s", cmd.c_str());
	sendTScommand(cmd, 4);;
}

void RIG_TS480SAT::set_modeA(int val)
{
	modeA = val;
	cmd = "MD";
	cmd += TS480SAT_mode_chr[val];
	cmd += ';';
	LOG_WARN("%s", cmd.c_str());
	sendTScommand(cmd, 4);;
}

int RIG_TS480SAT::get_modeA()
{
	if (!sendTScommand(cmd, 4))
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

void RIG_TS480SAT::set_modeB(int val)
{
	modeB = val;
	cmd = "MD";
	cmd += TS480SAT_mode_chr[val];
	cmd += ';';
	LOG_WARN("%s", cmd.c_str());
	sendTScommand(cmd, 4);;
}

int RIG_TS480SAT::get_modeB()
{
	if (!sendTScommand(cmd, 4))
		return modeB;
	if (replystr.find("MD") != 0) {
		clearSerialPort();
		return modeB;
	}
	int md = replybuff[2];
	md = md - '1';
	if (md == 8) md = 7;
	modeA = md;
	return modeB;
}

int RIG_TS480SAT::get_modetype(int n)
{
	return _mode_type[n];
}

void RIG_TS480SAT::set_bwA(int val)
{
	bwA = val;
	cmd = "SL00";
	cmd[3] = '0' + val;
	LOG_WARN("%s", cmd.c_str());
	sendTScommand(cmd, 0);;
}

int RIG_TS480SAT::get_bwA()
{
	cmd = "SL;";
	if (sendTScommand(cmd, 5) == 5) {
		bwA = replybuff[3] - '0';
		if (bwA < 0) bwA = 0;
		if (bwA > 6) bwA = 6;
	}
	return bwA;
}

void RIG_TS480SAT::set_bwB(int val)
{
	bwB = val;
	cmd = "SL00";
	cmd[3] = '0' + val;
	LOG_WARN("%s", cmd.c_str());
	sendTScommand(cmd, 0);;
}

int RIG_TS480SAT::get_bwB()
{
	cmd = "SL;";
	if (sendTScommand(cmd, 5) == 5) {
		bwA = replybuff[3] - '0';
		if (bwB < 0) bwB = 0;
		if (bwB > 6) bwB = 6;
	}
	return bwB;
}

int  RIG_TS480SAT::adjust_bandwidth(int m)
{
	if (m == 2 || m == 6) return 3;
	return 6;
}

void RIG_TS480SAT::set_volume_control(int val)
{
	cmd = "AG";
	char szval[5];
	snprintf(szval, sizeof(szval), "%04d", val * 255 / 100);
	cmd += szval;
	cmd += ';';
	LOG_WARN("%s", cmd.c_str());
	sendTScommand(cmd, 0);;
}

int RIG_TS480SAT::get_volume_control()
{
	cmd = "AG0";
	int val = 0;
	if (sendTScommand(cmd, 7) == 7) {
		replybuff[6] = 0;
		val = atoi(&replybuff[3]);
		val = val * 100 / 255;
	}
	return val;
}

void RIG_TS480SAT::set_power_control(double val)
{
	cmd = "PC";
	char szval[4];
	if (modeA == 4 && val > 50) val = 50; // AM mode limitation
	snprintf(szval, sizeof(szval), "%03d", (int)val);
	cmd += szval;
	cmd += ';';
	LOG_WARN("%s", cmd.c_str());
	sendTScommand(cmd, 0);;
}

int RIG_TS480SAT::get_power_control()
{
	cmd = "PC;";
	int val = 5;
	if (sendTScommand(cmd, 6) == 6) {
		replybuff[5] = 0;
		val = atoi(&replybuff[2]);
	}
	return val;
}

void RIG_TS480SAT::set_attenuator(int val)
{
	if (val)	cmd = "RA01";
	else		cmd = "RA00";
	sendTScommand(cmd, 0);;
}

int RIG_TS480SAT::get_attenuator()
{
	cmd = "RA;";
	if (sendTScommand(cmd, 7) == 7) {
		return replybuff[3] - '0';
	}
	return 0;
}

void RIG_TS480SAT::tune_rig()
{
	cmd = "AC111;";
	LOG_WARN("%s", cmd.c_str());
	sendTScommand(cmd, 0);
}

