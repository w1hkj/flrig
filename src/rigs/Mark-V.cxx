/*
 * Yaesu FT-1000MP drivers
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */

#include "MARK_V.h"

static const char MARK_Vname_[] = "Mark-V";

static const char *MARK_Vmodes_[] = {
	"LSB", "USB", "CW", "CW-R", "AM", 
	"AM(Sync)", "FM", "FM-M", "RTTY-L", "RTTY-U",
	"PKT-L", "PKT", NULL};

static const char MARK_V_mode_type[] = {
	'L', 'U', 'U', 'L', 'U', 
	'U', 'U', 'U', 'L', 'U',
	'L', 'U' };

RIG_MARK_V::RIG_MARK_V() {
// base class values
	name_ = MARK_Vname_;
	modes_ = MARK_Vmodes_;
	comm_baudrate = BR4800;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	serloop_timing = 1000; // every 1 seconds
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	modeA = 1;
	bwA = 0;

	has_mode_control =
	has_bandwidth_control =
	has_ptt_control =
	has_tune_control = true;

};

void RIG_MARK_V::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

long RIG_MARK_V::get_vfoA ()
{
	init_cmd();
	cmd[3] = 2; 
	cmd[4] = 0x10;
//	if (sendCommand(cmd, 32)) {
replybuff[1] = 0x01;
replybuff[2] = 0x08;
replybuff[3] = 0x53;
replybuff[4] = 0x00;
		freqA = 0;
		for (int i = 4; i > 0; i--) {
			freqA = freqA * 10 + (replybuff[i] & 0x0F);
			freqA = freqA * 10 + ((replybuff[i] & 0xF0) >> 4);
		}
		freqA *= 10;
//	}
	return freqA;
}

void RIG_MARK_V::set_vfoA (long freq)
{
	freqA = freq;
	freq /=10; // 1000MP does not support 1 Hz resolution
	cmd = to_bcd_be(freq, 8);
	cmd += 0x0A;
LOG_INFO("%s", str2hex(cmd.c_str(), cmd.length()));
	sendCommand(cmd, 0);
}

int RIG_MARK_V::get_modeA()
{
	init_cmd();
	cmd[4] = 0x0C;
	if (sendCommand(cmd, 5))
		modeA = cmd[4];
	return modeA;
}

void RIG_MARK_V::set_modeA(int val)
{
	modeA = val;
	init_cmd();
	cmd[3] = val;
	cmd[4] = 0x0C;
	sendCommand(cmd, 0);
}

int RIG_MARK_V::get_modetype(int n)
{
	return MARK_V_mode_type[n];
}

// Tranceiver PTT on/off
void RIG_MARK_V::set_PTT_control(int val)
{
	init_cmd();
	if (val) cmd[3] = 1;
	else	 cmd[3] = 0;
	cmd[4] = 0x0F;
	sendCommand(cmd, 0);
}

int  RIG_MARK_V::get_power_out(void)
{
	int val = 0;
	init_cmd();
	cmd[4] = 0xF7;
	if (sendCommand(cmd,1)) {
LOG_INFO("%s => %d",str2hex(replybuff,1), (val = replybuff[0] && 0x0F));
	}
	return 0;
}

int  RIG_MARK_V::get_smeter(void)
{
	int val = 0;
	init_cmd();
	cmd[4] = 0xF7;
	if (sendCommand(cmd,5)) {
LOG_INFO("%s => %d",str2hex(replybuff,1), (val = replybuff[0] && 0x0F));
	}
	return 0;
}

