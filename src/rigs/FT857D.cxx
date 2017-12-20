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

#include "FT857D.h"
#include "support.h"

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

	has_split_AB =
	has_smeter =
	has_power_out =
	has_mode_control = true;

	precision = 10;
	ndigits = 8;

};

RIG_FT897D::RIG_FT897D() {
	name_ = FT897Dname_;
	onB = false;
};

void RIG_FT857D::set_getACK() {
	for (int i = 0; i < 5; i++) {
		sendCommand(cmd, 0);
		for (int j = 0; j < 10; j++) {
			if (readResponse() == 1) return;
			MilliSleep(50);
		}
	}
}

void RIG_FT857D::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

long RIG_FT857D::get_vfoA ()
{
	init_cmd();
	cmd[4] = 0x03;
	int ret = waitN(5, 100, "get vfo A", HEX);
	if (ret == 5) {
		freqA = fm_bcd(replystr, 8) * 10;
		int mode = replystr[4];
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
	replystr.clear();
	set_getACK();
	showresp(WARN, HEX, "set vfo A", cmd, replystr);
}

long RIG_FT857D::get_vfoB ()
{
	init_cmd();
	cmd[4] = 0x03;
	int ret = waitN(5, 100, "get vfo B", HEX);
	if (ret == 5) {
		freqB = fm_bcd(replystr, 8) * 10;
		int mode = replystr[4];
		for (int i = 0; i < 8; i++)
			if (FT857D_mode_val[i] == mode) {
				modeB = i;
				break;
			}
	}
	return freqB;
}

void RIG_FT857D::set_vfoB (long freq)
{
	freqB = freq;
	freq /=10; // 857D does not support 1 Hz resolution
	cmd = to_bcd(freq, 8);
	cmd += 0x01;
	replystr.clear();
	set_getACK();
	showresp(WARN, HEX, "set vfo B", cmd, replystr);
}

int RIG_FT857D::get_modeA()
{
// read by get_vfoA
	return modeA;
}

int RIG_FT857D::get_modeB()
{
// read by get_vfoB
	return modeB;
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
	replystr.clear();
	set_getACK();
	showresp(WARN, HEX, "set mode A", cmd, replystr);
}

void RIG_FT857D::set_modeB(int val)
{
	modeB = val;
	init_cmd();
	cmd[0] = FT857D_mode_val[val];
	cmd[4] = 0x07;
	replystr.clear();
	set_getACK();
	showresp(WARN, HEX, "set mode B", cmd, replystr);
}

// Tranceiver PTT on/off
void RIG_FT857D::set_PTT_control(int val)
{
	init_cmd();
	if (val) cmd[4] = 0x08;
	else	 cmd[4] = 0x88;
	replystr.clear();
	set_getACK();
	showresp(WARN, HEX, "set PTT", cmd, replystr);
	ptt_ = val;
}

// mod submitted by Rich, WA4SXZ, for power_out and smeter

int  RIG_FT857D::get_power_out(void)
{
	init_cmd();
	cmd[4] = 0xF7;
	int ret = waitN(1, 100, "get pout", HEX);
	if (ret == 1) {
		int fwdpwr = replystr[0] & 0x0F;
		fwdpwr = fwdpwr * 100 / 15;
		return fwdpwr;
	}
	return 0;
}

int  RIG_FT857D::get_smeter(void)
{
	init_cmd();
	cmd[4] = 0xE7;
	int ret = waitN(1, 100, "get smeter", HEX);
	if (ret == 1) {
		int sval = replystr[0] & 0x0F;
		sval = (sval-1) * 100 / 15;
		return sval;
	}
	return 0;
}

void RIG_FT857D::selectA()
{
	if (!onB) return;
	onB = false;
	init_cmd();
	cmd[4] = 0x81; // this is a toggle ... no way of knowing which is A or B
	replystr.clear();
	set_getACK();
	showresp(WARN, HEX, "select A", cmd, replystr);
}

void RIG_FT857D::selectB()
{
	if (onB) return;
	onB = true;
	init_cmd();
	cmd[4] = 0x81; // this is a toggle ... no way of knowing which is A or B
	replystr.clear();
	set_getACK();
	showresp(WARN, HEX, "select B", cmd, replystr);
}

bool RIG_FT857D::can_split()
{ 
	return true;
}

void RIG_FT857D::set_split(bool val)
{
	split = val;
	replystr.clear();
	init_cmd();
	if (val) {
		cmd[4] = 0x02;
		set_getACK();
		showresp(WARN, HEX, "set split ON", cmd, replystr);
	} else {
		cmd[4] = 0x82;
		set_getACK();
		showresp(WARN, HEX, "set split OFF", cmd, replystr);
	}
}
