/*
 * Yaesu FT-897D drivers
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */

#include "FT897D.h"

static const char FT897Dname_[] = "FT-897D";

static const char *FT897Dmodes_[] = {
	"LSB", "USB", "CW", "CW-R", "AM", 
	"AM(Sync)", "FM", "FM-M", "RTTY-L", "RTTY-U",
	"PKT-L", "PKT", NULL};

static const char FT897D_mode_type[] = {
	'L', 'U', 'U', 'L', 'U', 
	'U', 'U', 'U', 'L', 'U',
	'L', 'U' };

static const char FT1000MVname_[] = "FT-1000MV";

// FT897D class
RIG_FT897D::RIG_FT897D() {
// base class values
	name_ = FT897Dname_;
	modes_ = FT897Dmodes_;
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

	has_mode_control =
	has_bandwidth_control =
	has_ptt_control =
	has_tune_control = true;

};

RIG_FT1000MV::RIG_FT1000MV() {
// base class values
	name_ = FT1000MVname_;
	modes_ = FT1000MVmodes_;
	comm_baudrate = BR4800;
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

	has_mode_control =
	has_bandwidth_control =
	has_ptt_control =
	has_tune_control = true;

};

void RIG_FT897D::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

long RIG_FT897D::get_vfoA ()
{
	init_cmd();
	cmd[3] = 2; cmd[4] = 0x10;
	if (sendCommand(cmd, 16)) {
		replybuff[5] = 0;
		freq_ = fm_bcd(&replybuff[1], 8) * 10;
	}
	return freq_;
}

void RIG_FT897D::set_vfoA (long freq)
{
	freq_ = freq;
	freq /=10; // 897D does not support 1 Hz resolution
	cmd = to_bcd(freq, 8);
	cmd += 0x0A;
	sendCommand(cmd, 0);
}

int RIG_FT897D::get_mode()
{
	init_cmd();
	cmd[4] = 0x0C;
	if (sendCommand(cmd, 5))
		mode_ = cmd[4];
	return mode_;
}

void RIG_FT897D::set_mode(int val)
{
	mode_ = val;
	init_cmd();
	cmd[3] = val;
	cmd[4] = 0x0C;
	sendCommand(cmd, 0);
}

int RIG_FT897D::get_modetype(int n)
{
	return FT897D_mode_type[n];
}

// Tranceiver PTT on/off
void RIG_FT897D::set_PTT_control(int val)
{
	init_cmd();
	if (val) cmd[3] = 1;
	else	 cmd[3] = 0;
	cmd[4] = 0x0F;
	sendCommand(cmd, 0);
}

int  RIG_FT897D::get_power_out(void)
{
	int val = 0;
	init_cmd();
	cmd[4] = 0xF7;
	if (sendCommand(cmd,1)) {
LOG_INFO("%s => %d",str2hex(replybuff,1), (val = replybuff[0] && 0x0F));
	}
	return 0;
}

int  RIG_FT897D::get_smeter(void)
{
	int val = 0;
	init_cmd();
	cmd[4] = 0xF7;
	if (sendCommand(cmd,5)) {
LOG_INFO("%s => %d",str2hex(replybuff,1), (val = replybuff[0] && 0x0F));
		return 
	}
	return 0;
}

