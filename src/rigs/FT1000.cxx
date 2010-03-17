/*
 * Yaesu FT-1000 drivers
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */

#include "FT1000.h"

static const char FT1000name_[] = "FT-1000 **";

static const char *FT1000modes_[] = {
	"LSB", "USB", "CW-2.4", "CW-500", "AM-6", 
	"AM-2.4", "FM", "FM", "RTTY-L", "RTTY-U",
	"PKT-LSB", "PKT-FM", NULL};

static const char FT1000_mode_type[] = {
	'L', 'U', 'L', 'L', 'U', 
	'U', 'U', 'U', 'L', 'U',
	'L', 'U' };

static const char *FT1000_widths[] = {
"2400", "2000", "500", "250", "6000", NULL };

// FT1000 class
RIG_FT1000::RIG_FT1000() {
// base class values
	name_ = FT1000name_;
	modes_ = FT1000modes_;
	bandwidths_ = FT1000_widths;
	comm_baudrate = BR4800;
	stopbits = 2;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	serloop_timing = 5000; // every 5 seconds FT1000 is a very slow beast!
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

void RIG_FT1000::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

long RIG_FT1000::get_vfoA ()
{
	init_cmd();
	cmd[3] = 2; cmd[4] = 0x10;
	if (sendCommand(cmd, 1636)) {
		replybuff[9] = 0;
		freq_ = fm_bcd(&replybuff[6], 8) * 10;
		bw_ = replybuff[13] & 0x07;
	}
	return freq_;
}

void RIG_FT1000::set_vfoA (long freq)
{
	freq_ = freq;
	freq /=10; // 1000 does not support 1 Hz resolution
	cmd = to_bcd(freq, 8);
	cmd += 0x0A;
	sendCommand(cmd, 0);
}

int RIG_FT1000::get_mode()
{
	init_cmd();
	cmd[4] = 0x0C;
	if (sendCommand(cmd, 5))
		mode_ = cmd[4];
	return mode_;
}

void RIG_FT1000::set_mode(int val)
{
	mode_ = val;
	init_cmd();
	cmd[3] = val;
	cmd[4] = 0x0C;
	sendCommand(cmd, 0);
}

int RIG_FT1000::get_modetype(int n)
{
	return FT1000_mode_type[n];
}

void RIG_FT1000::set_bandwidth(int n)
{
	init_cmd();
	cmd[3] = n;
	cmd[4] = 0x8C;
	sendCommand(cmd, 0);
	bw_ = n;
}

int RIG_FT1000::get_bandwidth()
{
	return bw_;
}

// Tranceiver PTT on/off
void RIG_FT1000::set_PTT_control(int val)
{
	init_cmd();
	if (val) cmd[3] = 1;
	else	 cmd[3] = 0;
	cmd[4] = 0x0F;
	sendCommand(cmd, 0);
}

void RIG_FT1000::tune_rig()
{
	init_cmd();
	cmd[4] = 0x82;
	sendCommand(cmd, 0);
}

int  RIG_FT1000::get_power_out(void)
{
	int val = 0;
	init_cmd();
	cmd[4] = 0xF7;
	if (sendCommand(cmd,1)) {
LOG_INFO("%s => %d",str2hex(replybuff,1), (val = replybuff[0] && 0x0F));
	}
	return 0;
}

int  RIG_FT1000::get_smeter(void)
{
	int val = 0;
	init_cmd();
	cmd[4] = 0xF7;
	if (sendCommand(cmd,5)) {
LOG_INFO("%s => %d",str2hex(replybuff,1), (val = replybuff[0] && 0x0F));
	}
	return 0;
}

