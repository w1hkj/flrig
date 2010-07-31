/*
 * Kenwood TS480HX driver
 * originally based on Kenwood TS2000 driver
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */


#include "TS480HX.h"
#include "support.h"

static const char TS480HXname_[] = "TS-480HX";

static const char *TS480HXmodes_[] = {
		"LSB", "USB", "CW", "FM", "AM", "FSK", "CW-R", "FSK-R", NULL};
static const char TS480HX_mode_chr[] =  { '1', '2', '3', '4', '5', '6', '7', '9' };
static const char TS480HX_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'L', 'L', 'U' };

static const char *TS480HX_widths[] = {
"50", "100", "250", "500", "1000", "1500", "2400", NULL};

RIG_TS480HX::RIG_TS480HX() {
// base class values
	name_ = TS480HXname_;
	modes_ = TS480HXmodes_;
	_mode_type = TS480HX_mode_type;
	bandwidths_ = TS480HX_widths;
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
	has_ptt_control = true;
}

bool RIG_TS480HX::sendTScommand(string cmd, int retnbr, bool loghex)
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

long RIG_TS480HX::get_vfoA ()
{
	cmd = "FA;";
	if (!sendTScommand(cmd, 14, false))
		return freq_;
	if (replystr.find("FA") != 0) {
		clearSerialPort();
		return freq_;
	}
	int f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replybuff[n] - '0';
	freq_ = f;
	return freq_;
}

void RIG_TS480HX::set_vfoA (long freq)
{
	freq_ = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendTScommand(cmd, 0, false);
}

// SM cmd 0 ... 100 (rig values 0 ... 15)
int RIG_TS480HX::get_smeter()
{
	cmd = "SM0;";
	if(!sendTScommand(cmd, 8, false))
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
int RIG_TS480HX::get_swr()
{
	cmd = "RM1;RM;"; // select measurement '1' (swr) and read meter
	if (!sendTScommand(cmd, 8, false))
		return 0;
	if (replystr.find("RM") != 0) {
		clearSerialPort();
		return 0;
	}
	replybuff[8] = 0;
	int mtr = atoi(&replybuff[3]);
	mtr = (mtr * 100) / 10;
	return mtr;
}


// Tranceiver PTT on/off
void RIG_TS480HX::set_PTT_control(int val)
{
	if (val)	cmd = "TX1;"; // DTS transmission using ANI input
	else	 	cmd = "RX;";
	sendTScommand(cmd, 4, false);
}

void RIG_TS480HX::set_mode(int val)
{
	mode_ = val;
	cmd = "MD";
	cmd += TS480HX_mode_chr[val];
	cmd += ';';
	sendTScommand(cmd, 4, false);
}

int RIG_TS480HX::get_mode()
{
	if (!sendTScommand("MD;", 4, false))
		return mode_;
	if (replystr.find("MD") != 0) {
		clearSerialPort();
		return mode_;
	}
	int md = replybuff[2];
	md = md - '1';
	if (md == 8) md = 7;
	mode_ = md;
	return mode_;
}

int RIG_TS480HX::get_modetype(int n)
{
	return _mode_type[n];
}

void RIG_TS480HX::set_bandwidth(int val)
{
	bw_ = val;
	cmd = "SH01"; // set center frequency to 1500
	sendTScommand(cmd, 0, false);
	cmd = "SL00";
	cmd[3] = '0' + val;
	sendTScommand(cmd, 0, false);
}

int RIG_TS480HX::get_bandwidth()
{
	cmd = "SL;";
	if (sendTScommand(cmd, 4, false)) {
		bw_ = replybuff[3] - '0';
		if (bw_ < 0) bw_ = 0;
		if (bw_ > 6) bw_ = 6;
	}
	return bw_;
}

void RIG_TS480HX::set_volume_control(int val)
{
	cmd = "AG";
	char szval[5];
	snprintf(szval, sizeof(szval), "%04d", val * 255 / 100);
	cmd += szval;
	cmd += ';';
	sendTScommand(cmd, 0, false);
}

int RIG_TS480HX::get_volume_control()
{
	cmd = "AG0";
	int val = 0;
	if (sendTScommand(cmd, 7, false)) {
		replybuff[6] = 0;
		val = atoi(&replybuff[3]);
		val = val * 100 / 255;
	}
	return val;
}

void RIG_TS480HX::set_power_control(double val)
{
	cmd = "PC";
	char szval[4];
	if (mode_ == 4 && val > 50) val = 50; // AM mode limitation
	snprintf(szval, sizeof(szval), "%03d", (int)val);
	cmd += szval;
	cmd += ';';
	sendTScommand(cmd, 0, false);
}

int RIG_TS480HX::get_power_control()
{
	cmd = "PC;";
	int val = 5;
	if (sendTScommand(cmd, 6, false)) {
		replybuff[5] = 0;
		val = atoi(&replybuff[2]);
	}
	return val;
}

void RIG_TS480HX::set_attenuator(int val)
{
	if (val)	cmd = "RA01";
	else		cmd = "RA00";
	sendTScommand(cmd, 0, false);
}

int RIG_TS480HX::get_attenuator()
{
	cmd = "RA;";
	if (sendTScommand(cmd, 7, false)) {
		return replybuff[3] - '0';
	}
	return 0;
}

