/*
 * Yaesu FT-847 drivers
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */

#include "FT847.h"
#include "rig.h"

static const char FT847name_[] = "FT-847";
static const char *FT847modes_[] = 
{ "LSB", "USB", "CW", "CW-R", "AM", "FM", "CW-N", "CW-NR", "AM-N", "FM-N", NULL};

static const int FT847_mode_val[] =
{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x08, 0x82, 0x83, 0x84, 0x88 };

static const char FT847_mode_type[] =
{ 'L', 'U', 'L', 'U', 'U', 'U', 'L', 'U', 'U', 'U' };

//static const int FT847_def_bw[] = { 2, 2, 1, 1, 3, 2, 2, 3 };
//static const char *FT847widths_[] = { "300", "500", "2400", "6000", NULL};
//static const int FT847_bw_val[] = { 0, 1, 2, 3 };

RIG_FT847::RIG_FT847() {
	name_ = FT847name_;
	modes_ = FT847modes_;
//	bandwidths_ = FT847widths_;
	comm_baudrate = BR9600;
	stopbits = 2;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	afreq = A.freq = B.freq = 14070000;
	amode = A.imode = B.imode = 1;
	precision = 10;

//	has_get_info =
//	has_bandwidth_control =
	has_mode_control =
	has_ptt_control = true;

}

void RIG_FT847::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

void RIG_FT847::initialize()
{
	init_cmd();
	sendCommand(cmd, 0); // CAT on
	cmd[4] = 0x8E; // satellite mode off
	replystr.clear();
	sendCommand(cmd);
	showresp(WARN, HEX, "init", cmd, replystr);
}

bool RIG_FT847::get_info()
{
	int ret = 0, i = 0;

	init_cmd();
	cmd[4] = 0x03;
	ret = waitN(5, 100, "get info", HEX);
	if (ret >= 5) {
		afreq = fm_bcd(&replystr[ret - 5], 8)*10;
		amode = replystr[ret - 1];
		for (i = 0; i < 10; i++) if (FT847_mode_val[i] == amode) break;
		if (i == 10) i = 1;
		amode = i;
		return true;
	}
	return false;
}

long RIG_FT847::get_vfoA ()
{
	if (get_info()) {
		A.freq = afreq;
		A.imode = amode;
		A.iBW = aBW;
	}
	return A.freq;
}

void RIG_FT847::set_vfoA (long freq)
{
	A.freq = freq;
	freq /=10; // 847 does not support 1 Hz resolution
	cmd = to_bcd(freq, 8);
	cmd += 0x01;
	replystr.clear();
	sendCommand(cmd);
	showresp(WARN, HEX, "set vfo A", cmd, replystr);
}

int RIG_FT847::get_modeA()
{
	return A.imode;
}

void RIG_FT847::set_modeA(int val)
{
	A.imode = val;
	init_cmd();
	cmd[0] = FT847_mode_val[val];
	cmd[4] = 0x07;
	replystr.clear();
	sendCommand(cmd);
	showresp(WARN, HEX, "set mode A", cmd, replystr);
}

long RIG_FT847::get_vfoB()
{
	if (get_info()) {
		B.freq = afreq;
		B.imode = amode;
		B.iBW = aBW;
	}
	return B.freq;
}

void RIG_FT847::set_vfoB(long freq)
{
	B.freq = freq;
	freq /=10; // 847 does not support 1 Hz resolution
	cmd = to_bcd(freq, 8);
	cmd += 0x01;
	replystr.clear();
	sendCommand(cmd);
	showresp(WARN, HEX, "set vfo B", cmd, replystr);
}

void RIG_FT847::set_modeB(int val)
{
	B.imode = val;
	init_cmd();
	cmd[0] = FT847_mode_val[val];
	cmd[4] = 0x07;
	replystr.clear();
	sendCommand(cmd);
	showresp(WARN, HEX, "set mode B", cmd, replystr);
}

int  RIG_FT847::get_modeB()
{
	return B.imode;
}

// Tranceiver PTT on/off
void RIG_FT847::set_PTT_control(int val)
{
	init_cmd();
	if (val) cmd[4] = 0x08;
	else cmd[4] = 0x88;
	replystr.clear();
	sendCommand(cmd);
	showresp(WARN, HEX, "set PTT", cmd, replystr);
}

int RIG_FT847::get_smeter()
{
	init_cmd();
	cmd[4] = 0xE7;
	int sval = 0;
	int ret = waitN(1, 100, "get smeter", HEX);
	if (ret >= 1)
		sval = (replystr[ret - 1] & 0x1F) / 32.0;
	return sval;
}

int RIG_FT847::get_power_out()
{
	init_cmd();
	cmd[4] = 0xF7;
	fwdpwr = 0;
	int ret = waitN(1, 100, "get power", HEX);
	if (ret >= 1)
		fwdpwr = (replystr[ret - 1] & 0x1F) / 32.0;
	return fwdpwr;
}

