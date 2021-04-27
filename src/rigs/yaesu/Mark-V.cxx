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

#include "yaesu/MARK_V.h"

static const char MARK_Vname_[] = "Mark-V";

static const char *MARK_Vmodes_[] = {
	"LSB", "USB", "CW", "CW-R", "AM", 
	"AM(Sync)", "FM", "FM-M", "RTTY-L", "RTTY-U",
	"PKT-L", "PKT", NULL};

static const char MARK_V_mode_type[] = {
	'L', 'U', 'U', 'L', 'U', 
	'U', 'U', 'U', 'L', 'U',
	'L', 'U' };

RIG_MARK_V::RIG_MARK_V() {
// base class values
	name_ = MARK_Vname_;
	modes_ = MARK_Vmodes_;
	comm_baudrate = BR4800;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	serloop_timing = 1000; // every 1 seconds
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	modeA = 1;
	bwA = 0;

	has_smeter =
	has_power_out =
	has_mode_control =
	has_bandwidth_control =
	has_ptt_control =
	has_tune_control = true;

	precision = 1;
	ndigits = 8;

};

void RIG_MARK_V::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

long RIG_MARK_V::get_vfoA ()
{
	init_cmd();
	cmd[3] = 2; 
	cmd[4] = 0x10;
	if (waitN(32, 200, "get vfoA", HEX) >= 32) {
		freqA = 0;
		for (int i = 4; i > 0; i--) {
			freqA = freqA * 10 + (replybuff[i] & 0x0F);
			freqA = freqA * 10 + ((replybuff[i] & 0xF0) >> 4);
		}
		freqA *= 10;
	}
	return freqA;
}

void RIG_MARK_V::set_vfoA (long freq)
{
	freqA = freq;
	freq /=10; // 1000MP does not support 1 Hz resolution
	cmd = to_bcd_be(freq, 8);
	cmd += 0x0A;
LOG_INFO("%s", str2hex(cmd.c_str(), cmd.length()));
	sendCommand(cmd, 0);
}

int RIG_MARK_V::get_modeA()
{
	init_cmd();
	cmd[4] = 0x0C;
	if (waitN(5, 100, "get modeA", HEX) >= 5)
		modeA = cmd[4];
	return modeA;
}

void RIG_MARK_V::set_modeA(int val)
{
	modeA = val;
	init_cmd();
	cmd[3] = val;
	cmd[4] = 0x0C;
	sendCommand(cmd, 0);
}

int RIG_MARK_V::get_modetype(int n)
{
	return MARK_V_mode_type[n];
}

// Tranceiver PTT on/off
void RIG_MARK_V::set_PTT_control(int val)
{
	init_cmd();
	if (val) cmd[3] = 1;
	else	 cmd[3] = 0;
	cmd[4] = 0x0F;
	sendCommand(cmd, 0);
	ptt_ = val;
}

int  RIG_MARK_V::get_power_out(void)
{
	int val = 0;
	init_cmd();
	cmd[00] = cmd[01] = cmd[02] = cmd[03] = 0x80H;
	cmd[4] = 0xF7;
	if (waitN(1, 100, "get pwr out", HEX) >= 1)
		val = replybuff[0] && 0x0F;
	return val;
}

int  RIG_MARK_V::get_smeter(void)
{
	int val = 0;
	init_cmd();
	cmd[4] = 0xF7;
	if (waitN(1, 100, "get vfoA", HEX) >= 1)
		val = replybuff[0] && 0x0F;
	return val;
}

