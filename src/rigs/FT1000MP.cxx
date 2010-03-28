/*
 * Yaesu FT-1000MP drivers
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */

#include "FT1000MP.h"

static const char FT1000MPname_[] = "FT-1000MP **";

static const char *FT1000MPmodes_[] = {
	"LSB", "USB", "CW", "CW-R", "AM", 
	"AM(Sync)", "FM", "FM-M", "RTTY-L", "RTTY-U",
	"PKT-L", "PKT", NULL};

static const char FT1000MP_mode_type[] = {
	'L', 'U', 'U', 'L', 'U', 
	'U', 'U', 'U', 'L', 'U',
	'L', 'U' };

RIG_FT1000MP::RIG_FT1000MP() {
// base class values
	name_ = FT1000MPname_;
	modes_ = FT1000MPmodes_;
	comm_baudrate = BR4800;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	serloop_timing = 1000; // every 1 seconds 1000MP not as stupid as the 1000
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	mode_ = 1;
	bw_ = 0;

	has_mode_control =
	has_bandwidth_control =
	has_ptt_control =
	has_tune_control = true;

};

void RIG_FT1000MP::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

long RIG_FT1000MP::get_vfoA ()
{
	init_cmd();
	cmd[3] = 2; 
	cmd[4] = 0x10;
	if (sendCommand(cmd, 32)) {
		freq_ = 0;
		for (int i = 4; i > 0; i--) {
			freq_ = freq_ * 10 + (replybuff[i] & 0x0F);
			freq_ = freq_ * 10 + ((replybuff[i] & 0xF0) >> 4);
		}
		freq_ *= 10;
	}
	return freq_;
}

void RIG_FT1000MP::set_vfoA (long freq)
{
	freq_ = freq;
	freq /=10; // 1000MP does not support 1 Hz resolution
	cmd = to_bcd_be(freq, 8);
	cmd += 0x0A;
LOG_INFO("%s", str2hex(cmd.c_str(), cmd.length()));
	sendCommand(cmd, 0);
}

int RIG_FT1000MP::get_mode()
{
	init_cmd();
	cmd[4] = 0x0C;
	if (sendCommand(cmd, 5))
		mode_ = cmd[4];
	return mode_;
}

void RIG_FT1000MP::set_mode(int val)
{
	mode_ = val;
	init_cmd();
	cmd[3] = val;
	cmd[4] = 0x0C;
	sendCommand(cmd, 0);
}

int RIG_FT1000MP::get_modetype(int n)
{
	return FT1000MP_mode_type[n];
}

// Tranceiver PTT on/off
void RIG_FT1000MP::set_PTT_control(int val)
{
	init_cmd();
	if (val) cmd[3] = 1;
	else	 cmd[3] = 0;
	cmd[4] = 0x0F;
	sendCommand(cmd, 0);
}

int  RIG_FT1000MP::get_power_out(void)
{
	int val = 0;
	init_cmd();
	cmd[4] = 0xF7;
	if (sendCommand(cmd,1)) {
LOG_INFO("%s => %d",str2hex(replybuff,1), (val = replybuff[0] && 0x0F));
	}
	return 0;
}

int  RIG_FT1000MP::get_smeter(void)
{
	int val = 0;
	init_cmd();
	cmd[4] = 0xF7;
	if (sendCommand(cmd,5)) {
LOG_INFO("%s => %d",str2hex(replybuff,1), (val = replybuff[0] && 0x0F));
	}
	return 0;
}

