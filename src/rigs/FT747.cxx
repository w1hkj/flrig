// ----------------------------------------------------------------------------
// Copyright (C) 2014
//              David Freese, W1HKJ
//
// This file is part of flrig.
//
// flrig is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// flrig is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "util.h"

#include "FT747.h"

static const char FT747name_[] = "FT-747";

static const char *FT747modes_[] = {
		"LSB", "USB", "CW", "CWN", "AMW", "AMN", "FMW", "FMN", NULL};
static const char FT747_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'U', 'U', 'U' };

RIG_FT747::RIG_FT747() {
// base class values
	name_ = FT747name_;
	modes_ = FT747modes_;
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

	has_split = true;
	has_split_AB = true;
	has_get_info = true;
	has_ptt_control = true;
	has_mode_control = true;

	precision = 10;
	ndigits = 8;

};

void RIG_FT747::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

void RIG_FT747::selectA()
{
	init_cmd();
	cmd[4] = 0x05;
	sendCommand(cmd);
	showresp(INFO, HEX, "select A", cmd, replystr);
}

void RIG_FT747::selectB()
{
	init_cmd();
	cmd[3] = 0x01;
	cmd[4] = 0x05;
	sendCommand(cmd);
	showresp(INFO, HEX, "select B", cmd, replystr);
}

void RIG_FT747::set_split(bool val)
{
	split = val;
	init_cmd();
	cmd[3] = val ? 0x01 : 0x00;
	cmd[4] = 0x01;
	sendCommand(cmd);
	if (val)
		showresp(INFO, HEX, "set split ON", cmd, replystr);
	else
		showresp(INFO, HEX, "set split OFF", cmd, replystr);
}

bool RIG_FT747::check()
{
	init_cmd();
	cmd[3] = 0x01;
	cmd[4] = 0xFA;
	int ret = waitN(28, 100, "check", HEX);
	if (ret >= 28) return true;
	return false;
}

bool RIG_FT747::get_info()
{
	long int afreq = A.freq, bfreq = B.freq;
	int amode = A.imode;
	init_cmd();
	cmd[4] = 0x10;
	int ret = waitN(28, 100, "get info", HEX);
	showresp(DEBUG, HEX, "get info", cmd, replystr);

	if (ret >= 28) {

// GET FREQUENCY bytes ARE NOT the same as SET FREQUENCY bytes
// returned values are packed BCD 
// | 00 00 | f2 f3 | f4 f5 | f6 f7 | f8 f9 | where
//          f2 - 10's of MHz
//          f3 - MHz
//      ... f9 - Hz
// | 00 00 | 01 04 | 02 05 | 00 00 | 00 00 | = 14.250.000 Hz

		afreq = fm_bcd(replystr.substr(9), 8);
		bfreq = fm_bcd(replystr.substr(16), 8);

		int md = replystr[22];
		switch (md) {
			case 0x01 : amode = 6; // FMW
						break;
			case 0x81 : amode = 7; // FMN
						break;
			case 0x02 : amode = 4; // AMW
						break;
			case 0x82 : amode = 5; // AMN
						break;
			case 0x04 : amode = 2; // CW
						break;
			case 0x84 : amode = 3; // CWN
						break;
			case 0x08 : amode = 1; // USB
						break;
			case 0x10 : amode = 0; // LSB
						break;
		}

		A.freq = afreq;
		A.imode = amode;

		B.freq = bfreq;
		B.imode = amode;

		return true;
	}
	A.freq = afreq;
	A.imode = amode;

	B.freq = bfreq;
	B.imode = amode;

	return false;
}

long RIG_FT747::get_vfoA ()
{
	return A.freq;
}

void RIG_FT747::set_vfoA (long freq)
{
	A.freq = freq;
	freq /=10; // 747 does not support 1 Hz resolution
	cmd = to_bcd_be(freq, 8);
	cmd += 0x0A; // SET FREQUENCY
	SLOG_INFO("cmd: %s", str2hex(cmd.c_str(), cmd.length()));
	sendCommand(cmd);
}

int RIG_FT747::get_modeA()
{
	return A.imode;
}

void RIG_FT747::set_modeA(int val)
{
	A.imode = val;
	init_cmd();
	cmd[3] = val;
	cmd[4] = 0x0C; // MODESEL
	SLOG_INFO("cmd: %s", str2hex(cmd.c_str(), cmd.length()));
	sendCommand(cmd);
}

long RIG_FT747::get_vfoB ()
{
	return B.freq;
}

void RIG_FT747::set_vfoB (long freq)
{
	B.freq = freq;
	freq /=10;
	cmd = to_bcd_be(freq, 8);
	cmd += 0x0A; // SET FREQUENCY
	SLOG_INFO("cmd: %s", str2hex(cmd.c_str(), cmd.length()));
	sendCommand(cmd);
}

int RIG_FT747::get_modeB()
{
	return B.imode;
}

void RIG_FT747::set_modeB(int val)
{
	B.imode = val;
	init_cmd();
	cmd[3] = val;
	cmd[4] = 0x0C; // MODESEL
	SLOG_INFO("cmd: %s", str2hex(cmd.c_str(), cmd.length()));
	sendCommand(cmd);
}

void RIG_FT747::set_PTT_control(int val)
{
	init_cmd();
	cmd[3] = val ? 0x01 : 0x00;
	cmd[4] = 0x0F;
	sendCommand(cmd);
	ptt_ = val;
}

