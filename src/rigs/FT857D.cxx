/*
 * Yaesu FT-857D drivers
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */
#include <stdio.h>
#include <stdlib.h>

#include "FT857D.h"

static const char FT857Dname_[] = "FT-857D";
static const char FT897Dname_[] = "FT-897D";

static const char *FT857Dmodes_[] = {
		"LSB", "USB", "CW", "CW-R", "AM", "FM", "DIG", "PKT", NULL};
static const int FT857D_mode_val[] =  { 0, 1, 2, 3, 4, 8, 0x0A, 0x0C };
static const char FT857D_mode_type[] = { 'L', 'U', 'U', 'L', 'U', 'U', 'U', 'U' };

RIG_FT857D::RIG_FT857D() {
// base class values
	name_ = FT857Dname_;
	modes_ = FT857Dmodes_;
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
	modeA = 1;
	bwA = 0;
	has_mode_control = true;
};

RIG_FT897D::RIG_FT897D() {
	name_ = FT897Dname_;
};

void RIG_FT857D::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

long RIG_FT857D::get_vfoA ()
{
	init_cmd();
	cmd[4] = 0x03;
	int ret = sendCommand(cmd);
	if (ret == 5) {
		freqA = fm_bcd(replybuff, 8) * 10;
		int mode = replybuff[4];
		for (int i = 0; i < 8; i++)
			if (FT857D_mode_val[i] == mode) {
				modeA = i;
				break;
			}
	}
	return freqA;
}

void RIG_FT857D::set_vfoA (long freq)
{
	freqA = freq;
	freq /=10; // 857D does not support 1 Hz resolution
	cmd = to_bcd(freq, 8);
	cmd += 0x01;
	sendCommand(cmd, 0);
}

int RIG_FT857D::get_modeA()
{
// read by get_vfoA
	return modeA;
}

int RIG_FT857D::get_modetype(int n)
{
	return FT857D_mode_type[n];
}


void RIG_FT857D::set_modeA(int val)
{
	modeA = val;
	init_cmd();
	cmd[0] = FT857D_mode_val[val];
	cmd[4] = 0x07;
	sendCommand(cmd, 0);
}

// Tranceiver PTT on/off
void RIG_FT857D::set_PTT_control(int val)
{
	init_cmd();
	if (val) cmd[4] = 0x08;
	else	 cmd[4] = 0x88;
	sendCommand(cmd, 0);
}

// mod submitted by Rich, WA4SXZ, for power_out and smeter

int  RIG_FT857D::get_power_out(void)
{
   init_cmd();
   cmd[4] = 0xF7;
   int ret = sendCommand(cmd);
LOG_INFO("%d %x", ret, replybuff[0]);
//   if (ret == 1) {
       int fwdpwr = replybuff[0];
       fwdpwr = fwdpwr * 100 / 15;
       return fwdpwr;
//   }
//   return 0;
}

int  RIG_FT857D::get_smeter(void)
{
   init_cmd();
   cmd[4] = 0xE7;
   int ret = sendCommand(cmd);
LOG_INFO("%d %x", ret, replybuff[0]);
//   if (ret == 1) {
       int sval = replybuff[0];
       sval = (sval-1) * 100 / 15;
       return sval;
//   }
//   return 0;
}
