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
	can_change_alt_vfo = true;

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

void RIG_TS480SAT::initialize()
{
	selectA();
	cmd = "AC000;"; 
	sendCommand(cmd, 0);
	MilliSleep(100);
	readResponse();
LOG_WARN("%s", replystr.c_str());
	cmd = "EX04500001;"; // set bandwidth controls for data modes
	sendCommand(cmd, 0);
	MilliSleep(100);
	readResponse();
LOG_WARN("%s", replystr.c_str());
	cmd = "SH01"; // set center frequency to 1500
	sendCommand(cmd, 0);
	MilliSleep(100);
	readResponse();
LOG_WARN("%s", replystr.c_str());
}

void RIG_TS480SAT::selectA()
{
	cmd = "FR0;";
	sendCommand(cmd);
LOG_WARN("%s", replystr.c_str());
	cmd = "FT0;";
	sendCommand(cmd);
LOG_WARN("%s", replystr.c_str());
}

void RIG_TS480SAT::selectB()
{
	cmd = "FR1;";
	sendCommand(cmd);
LOG_WARN("%s", replystr.c_str());
	cmd = "FT1;";
	sendCommand(cmd);
LOG_WARN("%s", replystr.c_str());
}

void RIG_TS480SAT::set_split(bool val) 
{
	split = val;
	if (val) {
		cmd = "FR0;";
		sendCommand(cmd);
LOG_WARN("%s", replystr.c_str());
		cmd = "FT1;";
		sendCommand(cmd);
LOG_WARN("%s", replystr.c_str());
	} else {
		cmd = "FR0;";
		sendCommand(cmd);
LOG_WARN("%s", replystr.c_str());
		cmd = "FT0;";
		sendCommand(cmd);
LOG_WARN("%s", replystr.c_str());
	}
}

bool RIG_TS480SAT::get_split()
{
	cmd = "IF;";
	int ret = sendCommand(cmd);
	showresp(INFO, ASC, "get info", cmd, replystr);
	if (ret < 38) return split;
	size_t p = replystr.rfind("IF");
	if (p == string::npos) return split;
	split = replystr[p+32] ? true : false;
	return split;
}

long RIG_TS480SAT::get_vfoA ()
{
	cmd = "FA;";
	int ret = sendCommand(cmd);
	if (ret < 14) return freqA;
	size_t p = replystr.rfind("FA");
	if (p == string::npos) return freqA;

	int f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replystr[p - n] - '0';
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
	sendCommand(cmd, 0);
}

long RIG_TS480SAT::get_vfoB ()
{
	cmd = "FB;";
	int ret = sendCommand(cmd);
	if (ret < 14) return B.freq;
	size_t p = replystr.rfind("FB");
	if (p == string::npos) return B.freq;

	long f = 0L;
	long mul = 1L;
	for (size_t n = 12; n > 1; n--) {
		f += (replystr[p - n] - '0') * mul;
		mul *= 10;
	}
	B.freq = f;

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
	sendCommand(cmd, 0);
}

// SM cmd 0 ... 100 (rig values 0 ... 15)
int RIG_TS480SAT::get_smeter()
{
	cmd = "SM0;";
	int ret = sendCommand(cmd);
	if (ret < 8) return 0;

	size_t p = replystr.rfind("SM");
	if (p == string::npos) return 0;

	replystr[p - 7] = 0;
	int mtr = atoi(&replystr[p - 3]);
	mtr = (mtr * 100) / 20;
	return mtr;
}

// RM cmd 0 ... 100 (rig values 0 ... 8)
int RIG_TS480SAT::get_swr()
{
	return -1; // disable to see if beeps go away
	int mtr = 0;
	cmd = "RM1;"; // select measurement '1' (swr) and read meter
	int ret = sendCommand(cmd);
	if (ret < 8) return 0;
	size_t p = replystr.rfind("RM");
	if (p == string::npos) return 0;

	replystr[p + 7] = 0;
	mtr = atoi(&replystr[p + 4]);
	mtr *= 10;

	return mtr;
}


// Tranceiver PTT on/off
void RIG_TS480SAT::set_PTT_control(int val)
{
	if (val)	cmd = "TX1;"; // DTS transmission using ANI input
	else	 	cmd = "RX;";
	LOG_WARN("%s", cmd.c_str());
	sendCommand(cmd, 0);
}

void RIG_TS480SAT::set_modeA(int val)
{
	modeA = val;
	cmd = "MD";
	cmd += TS480SAT_mode_chr[val];
	cmd += ';';
	LOG_WARN("%s", cmd.c_str());
	sendCommand(cmd, 0);
}

int RIG_TS480SAT::get_modeA()
{
	cmd = "MD;";
	int ret = sendCommand(cmd);
	if (ret < 4) return modeA;

	size_t p = replystr.rfind("MD");
	if (p == string::npos) return modeA;

	int md = replystr[p - 2];
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
	sendCommand(cmd, 0);
}

int RIG_TS480SAT::get_modeB()
{
	cmd = "MD;";
	int ret = sendCommand(cmd);
	if (ret < 4) return modeB;

	size_t p = replystr.rfind("MD");
	if (p == string::npos) return modeB;

	int md = replystr[p - 2];
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
	cmd = "SL00;";
	cmd[3] = '0' + val;
	LOG_WARN("%s", cmd.c_str());
	sendCommand(cmd, 0);
}

int RIG_TS480SAT::get_bwA()
{
	cmd = "SL;";
	int ret = sendCommand(cmd);
	if (ret < 5) return bwA;
	size_t p = replystr.rfind("SL");
	if (p == string::npos) return bwA;

	bwA = replystr[p + 3] - '0';
	if (bwA < 0) bwA = 0;
	if (bwA > 6) bwA = 6;

	return bwA;
}

void RIG_TS480SAT::set_bwB(int val)
{
	bwB = val;
	cmd = "SL00;";
	cmd[3] = '0' + val;
	LOG_WARN("%s", cmd.c_str());
	sendCommand(cmd, 0);
}

int RIG_TS480SAT::get_bwB()
{
	cmd = "SL;";
	int ret = sendCommand(cmd);
	if (ret < 5) return bwB;
	size_t p = replystr.rfind("SL");
	if (p == string::npos) return bwB;

	bwA = replystr[p + 3] - '0';
	if (bwB < 0) bwB = 0;
	if (bwB > 6) bwB = 6;

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
	sendCommand(cmd, 0);
}

int RIG_TS480SAT::get_volume_control()
{
	int val = 0;
	cmd = "AG0";
	int ret = sendCommand(cmd);
	if (ret < 7) return val;
	size_t p = replystr.rfind("AG");
	if (p == string::npos) return val;

	replystr[p + 6] = 0;
	val = atoi(&replystr[p + 3]);
	val = val * 100 / 255;

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
	sendCommand(cmd, 0);
}

int RIG_TS480SAT::get_power_control()
{
	int val = 5;
	cmd = "PC;";
	int ret = sendCommand(cmd);
	if (ret < 6) return val;
	size_t p = replystr.rfind("PC");
	if (p == string::npos) return val;

	replystr[p + 5] = 0;
	val = atoi(&replystr[p + 2]);

	return val;
}

void RIG_TS480SAT::set_attenuator(int val)
{
	if (val)	cmd = "RA01";
	else		cmd = "RA00";
	sendCommand(cmd, 0);
}

int RIG_TS480SAT::get_attenuator()
{
	cmd = "RA;";
	int ret = sendCommand(cmd);
	if (ret < 7) return 0;
	size_t p = replystr.rfind("RA");
	if (p == string::npos) return 0;
	
	return replystr[p + 3] - '0';
}

void RIG_TS480SAT::tune_rig()
{
	cmd = "AC111;";
	LOG_WARN("%s", cmd.c_str());
	sendCommand(cmd, 0);
}

