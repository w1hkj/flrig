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
	mode_ = 1;
	bw_ = 0;
	has_mode_control = true;
};

void RIG_FT767::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

bool RIG_FT767::snd_cmd(string cmd, size_t n)
{
	if (!sendCommand(cmd, 5)) return false; //must echo 5 chars
	init_cmd();
	cmd[4] = 0x0B; // ACK string
	if (!sendCommand(cmd, n)) return false; //must return n chars
	return true;
}

long RIG_FT767::get_vfoA ()
{
	init_cmd();
	cmd[4] = 0x01; // CHECK command

	if (snd_cmd(cmd, 86)) {
		freq_ = fm_bcd(replybuff, 14) * 10; // VFO-A in positions 14-17
		mode_ = replybuff[19];
	}
	return freq_;
}

void RIG_FT767::set_vfoA (long freq)
{
	freq_ = freq;
	freq /=10; // 767 does not support 1 Hz resolution
	cmd = to_bcd(freq, 8);
	cmd += 0x08; // SET FREQUENCY
	snd_cmd(cmd, 5); // value returned is discarded -- might need to check for correct
}

int RIG_FT767::get_mode()
{
// read by get_vfoA
	return mode_;
}

void RIG_FT767::set_mode(int val)
{
	mode_ = val;
	init_cmd();
	cmd[3] = 0x10 + val; // 0x10 = LSB ... 0x15 = FSK
	cmd[4] = 0x0A; // MODESEL
	snd_cmd(cmd, 8); // discard the return values
}

