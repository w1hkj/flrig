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

static const char *FT1000MP_modes[] = {
	"LSB", "USB", 
	"CW-U", "CW-L", 
	"AM", "AM-syn", 
	"FM", "FM-alt", 
	"RTTY-L", "RTTY-U", 
	"PKT-L", "PKT-FM", NULL};

static const char FT1000MP_mode_type[] = {
	'L', 'U', 
	'U', 'L', 
	'U', 'U', 
	'U', 'U',
	'L', 'U', 
	'L', 'U' };

static const char *FT1000MP_widths[] = {
"6.0 K", "2.4 K", "2.0 K", "500", "250", NULL };

RIG_FT1000MP::RIG_FT1000MP() {
// base class values
	name_ = FT1000MPname_;
	modes_ = FT1000MP_modes;
	bandwidths_ = FT1000MP_widths;
	comm_baudrate = BR4800;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	serloop_timing = 200;
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	mode_ = 1;
	bw_ = 1;

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

// returns 16 bytes
// 0 - band selection
// 1,2,3,4 operating frequency
//    00 50 42 01 ==> 01425000
// 5,6 clarifier offset
// 7 operating mode
//    b7 - alternate user mode
//    0 = CW-U, AM-ENV, RTTY-L, PKT-L
//    1 = CW-L, AM-SYNC, RTTY-U, PKT-FM
// 8 if filter selection
// 9 vfo/mem operating flags
// a,b,c,d,e,f NOT used
//
long RIG_FT1000MP::get_vfoA ()
{
	int ret = 0;
	int alt = 0;
	init_cmd();
	cmd[3] = 2; 
	cmd[4] = 0x10;
	ret = sendCommand(cmd, 16);
	if (ret == 16) {
//{
//replybuff[1] = 0x00; replybuff[2] = 0x50; replybuff[3] = 0x42; replybuff[4] = 0x01;
//replybuff[7] = 0x82;
//replybuff[8] = 0x03;
		freq_ = 0;
		for (int i = 4; i > 0; i--) {
			freq_ = freq_ * 10 + ((replybuff[i] & 0xF0) >> 4);
			freq_ = freq_ * 10 + (replybuff[i] & 0x0F);
		}
		freq_ *= 10;
		mode_ = replybuff[7] & 0x07;
		alt = (replybuff[8] & 0x80) == 0x80 ? 1 : 0;
		if (mode_ > 1)
			mode_ = 2 * mode_ + alt - 2;
		bw_ = replybuff[8] & 0x07;
		bw_ = (bw_ > 4) ? 4 : bw_;
	}
LOG_INFO(
"\ndata: %s\nfreq: %ld\nmode: %s\nbw: %s", 
str2hex(replybuff,16), 
freq_,
FT1000MP_modes[mode_],
FT1000MP_widths[bw_]
);
	return freq_;
}

void RIG_FT1000MP::set_vfoA (long freq)
{
	freq_ = freq;
	init_cmd();
	freq /=10; // 1000MP does not support 1 Hz resolution
	for (int i = 0; i < 4; i++) {
		cmd[i] = (unsigned char)(freq % 10); freq /= 10;
		cmd[i] |= (unsigned char)((freq % 10) * 16); freq /= 10;
	}
	cmd[4] = 0x0A;
LOG_INFO("%s", str2hex(cmd.c_str(), cmd.length()));
	sendCommand(cmd, 0);
}

int RIG_FT1000MP::get_mode()
{
	return mode_;
}

void RIG_FT1000MP::set_mode(int val)
{
	mode_ = val;
	init_cmd();
	cmd[3] = val;
	cmd[4] = 0x0C;
	sendCommand(cmd, 0);
LOG_INFO("%s, %s", FT1000MP_modes[mode_], str2hex(cmd.c_str(),5));
}

int RIG_FT1000MP::get_modetype(int n)
{
	return FT1000MP_mode_type[n];
}

int RIG_FT1000MP::get_bandwidth()
{
	return bw_;
}

void RIG_FT1000MP::set_bandwidth(int val)
{
	bw_ = val;
	int bw = val--;
	if (bw < 0) bw = 4;
	init_cmd();
	cmd[3] = bw;
	cmd[4] = 0x8C;
	sendCommand(cmd, 0);
LOG_INFO("%s, %s", FT1000MP_widths[bw_], str2hex(cmd.c_str(), 5));
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
	cmd[0] = 0x80;
	cmd[4] = 0xF7;
	if (sendCommand(cmd,5)) {
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

