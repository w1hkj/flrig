/*
 * Yaesu FT-817 drivers
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */

#include "FT817.h"

static const char FT817name_[] = "FT-817";
static const char *FT817modes_[] = {
		"LSB", "USB", "CW", "CW-R", "AM", "FM", "DIG", "PKT", NULL};
static const int FT817_mode_val[] =  { 0, 1, 2, 3, 4, 8, 0x0A, 0x0C };
static const char FT817_mode_type[] = { 'L', 'U', 'U', 'L', 'U', 'U', 'U', 'U' };

RIG_FT817::RIG_FT817() {
// base class values	
	name_ = FT817name_;
	modes_ = FT817modes_;
	comm_baudrate = BR4800;
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
	mode_ = 1;
	bw_ = 0;

	has_ptt_control =
	has_mode_control = true;
	
}

void RIG_FT817::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

long RIG_FT817::get_vfoA ()
{
	init_cmd();
	cmd[4] = 0x03;

	if (sendCommand(cmd, 5))
		freq_ = fm_bcd(replybuff, 8) * 10;

	return freq_;
}

void RIG_FT817::set_vfoA (long freq)
{
	freq_ = freq;
	freq /=10; // 817 does not support 1 Hz resolution
	cmd = to_bcd(freq, 8);
	cmd += 0x01;
	sendCommand(cmd, 0);
	LOG_INFO("%s", str2hex(cmd.c_str(), 5));
}

int RIG_FT817::get_mode()
{
	init_cmd();
	cmd[4] = 0x03;

	if (sendCommand(cmd, 5)) {
		int mode = cmd[4];
		for (int i = 0; i < 8; i++)
			if (FT817_mode_val[i] == mode) {
				mode_ = i;
				break;
			}
	}
	return mode_;
}

int RIG_FT817::get_modetype(int n)
{
	return FT817_mode_type[n];
}


void RIG_FT817::set_mode(int val)
{
	mode_ = val;
	init_cmd();
	cmd[0] = FT817_mode_val[val];
	cmd[4] = 7;
	sendCommand(cmd, 0);
	LOG_INFO("%s", str2hex(cmd.c_str(), 5));
}

// Tranceiver PTT on/off
void RIG_FT817::set_PTT_control(int val)
{
	init_cmd();
	if (val) cmd[4] = 0x88;
	else	 cmd[4] = 0x08;
	sendCommand(cmd, 0);
}

int  RIG_FT817::get_power_out(void)
{
	int val = 0;
	init_cmd();
	cmd[4] = 0xF7;
	if (sendCommand(cmd,5)) {
LOG_INFO("%s => %d",str2hex(replybuff,5), (val = replybuff[0] && 0x0F));		
	}
	return 0;
}

int  RIG_FT817::get_smeter(void)
{
	int val = 0;
	init_cmd();
	cmd[4] = 0xE7;
	if (sendCommand(cmd,5)) {
LOG_INFO("%s => %d",str2hex(replybuff,5), (val = replybuff[0] && 0x0F));
	}
	return 0;
}
