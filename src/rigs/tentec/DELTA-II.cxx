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


#include "tentec/DELTA-II.h"

//=============================================================================
// TT-535

const char RIG_TT535name_[] = "DELTA-II";

const char *RIG_TT535modes_[] = {
		"LSB", "USB", "AM", "CW", "FM", NULL};
static const char RIG_TT535_mode_type[] = {'L', 'U', 'U', 'L', 'U'};

RIG_TT535::RIG_TT535() {
	name_ = RIG_TT535name_;
	modes_ = RIG_TT535modes_;
	serial_baudrate = BR1200;
	stopbits = 1;
	serial_retries = 2;

//	serial_write_delay = 0;
//	serial_post_write_delay = 0;

	serial_timeout = 50;
	serial_echo = true;
	serial_rtscts = false;
	serial_rtsplus = false;
	serial_dtrplus = true;
	serial_catptt = false;
	serial_rtsptt = false;
	serial_dtrptt = false;
	modeA = 1;
	bwA = 0;

	has_mode_control = true;
//	has_ptt_control = true;

	pre_to[2] = ok[3] = bad[3] = pre_fm[3] = 0x01;

};

int  RIG_TT535::adjust_bandwidth(int m)
{
	return 0;
}

bool RIG_TT535::check()
{
	return true;
}

unsigned long long RIG_TT535::get_vfoA ()
{
	return freqA;
}


void RIG_TT535::set_vfoA (unsigned long long freq)
{
	freqA = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 8 ) );
	cmd.append( post );
	int ret = sendCommand(cmd);
	if (ret != 6)
		checkresponse();
}

void RIG_TT535::set_vfoB (unsigned long long freq)
{
	freqB = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 8 ) );
	cmd.append( post );
	if (sendCommand(cmd) != 6)
		checkresponse();
}

unsigned long long RIG_TT535::get_vfoB ()
{
	return freqB;
}

// ditto on CAT PTT
/*
void RIG_TT535::set_PTT_control(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += val ? '\x01' : '\x02';
	cmd.append( post );
	sendICcommand(cmd,6);
	checkresponse(6);
}
*/

void RIG_TT535::set_modeA(int md)
{
	modeA = md;
	cmd = pre_to;
	cmd += '\x06';
	cmd += modeA;
	cmd.append(post);
	if (sendCommand(cmd) != 6)
		checkresponse();
}

// same with get mode
int RIG_TT535::get_modeA()
{
//	cmd = pre_to;
//	cmd += '\x04';
//	cmd.append(post);
//	if( sendICcommand (cmd, 8 )) {
//		modeA = replystr[5];
//		bwA = replystr[6];
//	}
	return modeA;
}


void RIG_TT535::set_modeB(int md)
{
	modeB = md;
	cmd = pre_to;
	cmd += '\x06';
	cmd += modeB;
	cmd.append(post);
	if (sendCommand(cmd) != 6)
		checkresponse();
}

// same with get mode
int RIG_TT535::get_modeB()
{
	return modeB;
}


int RIG_TT535::get_modetype(int n)
{
	return RIG_TT535_mode_type[n];
}


