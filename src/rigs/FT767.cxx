/*
 * Yaesu FT-767 drivers
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */
#include <stdio.h>
#include <stdlib.h>

#include "FT767.h"

static const char FT767name_[] = "FT-767";

static const char *FT767modes_[] = {
		"LSB", "USB", "CW", "AM", "FM", "FSK", NULL};
static const char FT767_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'U' };

RIG_FT767::RIG_FT767() {
// base class values
	name_ = FT767name_;
	modes_ = FT767modes_;
	comm_baudrate = BR4800;
	stopbits = 2;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = false;
	comm_rtsptt = false;
	comm_dtrptt = false;
	modeA = 1;
	bwA = 0;
	has_mode_control = true;
};

void RIG_FT767::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

long RIG_FT767::get_vfoA ()
{
	init_cmd();
	cmd[4] = 0x01; // CHECK command

	int ret = sendCommand(cmd);
	if (ret < 20) return freqA;
	freqA = fm_bcd(replybuff, 14) * 10; // VFO-A in positions 14-17
	modeA = replybuff[19];
	return freqA;
}

void RIG_FT767::set_vfoA (long freq)
{
	freqA = freq;
	freq /=10; // 767 does not support 1 Hz resolution
	cmd = to_bcd(freq, 8);
	cmd += 0x08; // SET FREQUENCY
	sendCommand(cmd);
}

int RIG_FT767::get_modeA()
{
	return modeA;
}

void RIG_FT767::set_modeA(int val)
{
	modeA = val;
	init_cmd();
	cmd[3] = 0x10 + val; // 0x10 = LSB ... 0x15 = FSK
	cmd[4] = 0x0A; // MODESEL
	sendCommand(cmd);
}

