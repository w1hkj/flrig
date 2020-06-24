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
// aunsigned long int with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include <iostream>

#include <stdio.h>
#include <stdlib.h>

#include "FT736R.h"

// transceiver DOES NOT return any operating parameters other than
// S-meter

static const char FT736Rname_[] = "FT-736R";

static const char *FT736Rmodes_[] = {
	"LSB", "USB", "CW", "CW-N", "FM", "FM-N", NULL};
static const int FT736Rmode_val_[] = {
	0x00, 0x01, 0x02, 0x82, 0x08, 0x88 };

static const char FT736R_mode_type[] = { 'L', 'U', 'L', 'L', 'U', 'U' };

RIG_FT736R::RIG_FT736R() {
// base class values
	name_ = FT736Rname_;
	modes_ = FT736Rmodes_;
	comm_baudrate = BR4800;
	stopbits = 2;
	comm_retries = 2;
	comm_wait = 50;
	comm_timeout = 200;
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	serloop_timing = 500;

	comm_catptt = true;

	A.freq = 144070000L;
	A.imode = 1;
	B.freq = 430001000L;
	B.imode = 2;

	has_mode_control = true;
	has_ptt_control = true;
	
	precision = 10;
	ndigits = 10;

};

void RIG_FT736R::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

void RIG_FT736R::send()
{
	sendCommand(cmd);
}

void RIG_FT736R::initialize()
{
	init_cmd();
	send();
	MilliSleep(200);
	set_PTT_control(0);
}

void RIG_FT736R::shutdown()
{
	init_cmd();
	cmd[4] = 0x80;
	send();
}

bool RIG_FT736R::check()
{
	return true;
}

void RIG_FT736R::selectA()
{
	inuse = onA;
	set_vfo(A.freq);
	MilliSleep(200);
	set_mode(A.imode);
}

void RIG_FT736R::selectB()
{
	inuse = onB;
	set_vfo(B.freq);
	MilliSleep(200);
	set_mode(B.imode);
}

unsigned long int RIG_FT736R::get_vfoA ()
{
	return A.freq;
}

// transceiver supports frequency ranges:
// 50.0 <= f < 54.0 MHz
// 144.0 <= f < 148.0 MHz
// 220.0 <= f < 225.0 MHz
// 430.0 <= f < 450.0 MHz
// 1.24 <= f < 1.30 GHz

void RIG_FT736R::set_vfo(unsigned long int freq)
{
	if ( freq < 50000000L ) return;
	if ( freq >= 54000000L && freq < 144000000L ) return;
	if ( freq >= 148000000L && freq < 220000000L ) return;
	if ( freq >= 225000000L && freq < 430000000L ) return;
	if ( freq >= 450000000L && freq < 1240000000L ) return;
	if ( freq > 1300000000L ) return;

	init_cmd();

	freq /=10; // 736R does not support 1 Hz resolution

	for (int i = 3; i >= 0; i--) {
		cmd[i] = freq % 10;
		freq /= 10;
		if (i == 0 && freq >= 12)
			cmd[i] |= 0xC0;
		else
			cmd[i] |= (freq % 10) << 4;
		freq /= 10;
	}

	cmd[4] = 0x01;
	send();
}

void RIG_FT736R::set_vfoA (unsigned long int freq)
{
	A.freq = freq;
	if (inuse == onB)
		return;
	set_vfo(freq);
}

int RIG_FT736R::get_modeA()
{
	return A.imode;
}

void RIG_FT736R::set_mode(int val)
{
	init_cmd();
	cmd[3] = FT736Rmode_val_[val];
	cmd[4] = 0x07;
	send();
}
 
void RIG_FT736R::set_modeA(int val)
{
	A.imode = val;
	if (inuse == onB)
		return;
	set_mode(val);
}

unsigned long int RIG_FT736R::get_vfoB ()
{
	return B.freq;
}

void RIG_FT736R::set_vfoB (unsigned long int freq)
{
	B.freq = freq;
	if (inuse == onA)
		return;
	set_vfo(freq);
}

int RIG_FT736R::get_modeB()
{
	return B.imode;
}

void RIG_FT736R::set_modeB(int val)
{
	B.imode = val;
	if (inuse == onA)
		return;
	set_mode(val);
}

void RIG_FT736R::set_PTT_control(int val)
{
	ptt_ = val;
	init_cmd();
	if (val)
		cmd[4] = 0x08;
	else
		cmd[4] = 0x88;
	send();
}

int  RIG_FT736R::get_PTT()
{
	return ptt_;
}

int RIG_FT736R::get_smeter()
{
	init_cmd();
	cmd[4] = 0xF7;
	int ret = waitN(1, 100, "get smeter", HEX);
	if (ret == 1) {
		int sval = replystr[0] - 0x30;
		sval = sval * 100 / (0xAD - 0x30);
		return sval;
	}
	return 0;
}
