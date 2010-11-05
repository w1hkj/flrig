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
	modeA = 1;
	bwA = 0;

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

	if (sendCommand(cmd, 5)) {
		freqA = fm_bcd(replybuff, 8) * 10;
		int mode = replybuff[4];
		for (int i = 0; i < 8; i++)
			if (FT817_mode_val[i] == mode) {
				modeA = i;
				break;
			}
	}
	return freqA;
}

void RIG_FT817::set_vfoA (long freq)
{
	freqA = freq;
	freq /=10; // 817 does not support 1 Hz resolution
	cmd = to_bcd(freq, 8);
	cmd += 0x01;
	sendCommand(cmd, 0);
}

int RIG_FT817::get_modeA()
{
// read by get_vfoA
	return modeA;
}

int RIG_FT817::get_modetype(int n)
{
	return FT817_mode_type[n];
}


void RIG_FT817::set_modeA(int val)
{
	modeA = val;
	init_cmd();
	cmd[0] = FT817_mode_val[val];
	cmd[4] = 0x07;
	sendCommand(cmd, 0);
}

// Tranceiver PTT on/off
void RIG_FT817::set_PTT_control(int val)
{
	init_cmd();
	if (val) cmd[4] = 0x08;
	else	 cmd[4] = 0x88;
	sendCommand(cmd, 0);
}

int  RIG_FT817::get_power_out(void)
{
   init_cmd();
   cmd[4] = 0xF7;
   if (sendCommand(cmd,1)) {
       int fwdpwr = replybuff[0];
       fwdpwr = fwdpwr * 100 / 15;
       return fwdpwr;
   }
   return 0;
}

int  RIG_FT817::get_smeter(void)
{
   init_cmd();
   cmd[4] = 0xE7;
   if (sendCommand(cmd,1)) {
       int sval = replybuff[0];
       sval = (sval-1) * 100 / 15;
       return sval;
   }
   return 0;
}
