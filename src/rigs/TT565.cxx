/*
 * TenTec 565 (Omni-VI) driver
 * based on the IC746 driver
 *
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */

#include "TT565.h"

//=============================================================================
// TT-565

const char RIG_TT565name_[] = "OMNI-VI";

const char *RIG_TT565modes_[] = {
		"LSB", "USB", "AM", "CW", "RTTY", "FM", NULL};
static const char RIG_TT565_mode_type[] = {'L', 'U', 'U', 'U', 'L', 'U'};
const char *RIG_TT565widths[] = { "NARR", "WIDE", NULL};

RIG_TT565::RIG_TT565() {
	name_ = RIG_TT565name_;
	modes_ = RIG_TT565modes_;
	bandwidths_ = RIG_TT565widths;
	comm_baudrate = BR9600;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 10;
	comm_timeout = 50;
	comm_echo = true;
	comm_rtscts = false;
	comm_rtsplus = true;
	comm_dtrplus = true;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	modeA = 1;
	bwA = 0;

	has_mode_control = true;
	has_ptt_control = true;

	pre_to[2] = ok[3] = bad[3] = pre_fm[3] = 0x04;

};

long RIG_TT565::get_vfoA ()
{
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	if (!sendICcommand(cmd, 11)) {
		checkresponse(11);
		return freqA;
	}
	freqA = fm_bcd_be(&replystr[5], 10);
	return freqA;
}

void RIG_TT565::set_vfoA (long freq)
{
	freqA = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 8 ) );
	cmd.append( post );
	sendICcommand(cmd, 6);
	checkresponse(6);
}

void RIG_TT565::set_PTT_control(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += val ? '\x01' : '\x02';
	cmd.append( post );
	sendICcommand(cmd,6);
	checkresponse(6);
}

void RIG_TT565::set_modeA(int md)
{
	modeA = md;
	cmd = pre_to;
	cmd += '\x06';
	cmd += modeA;
	cmd.append(post);
	sendICcommand(cmd, 6);
	checkresponse(6);
}

int RIG_TT565::get_modeA()
{
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	if( sendICcommand (cmd, 8 )) {
		modeA = replystr[5];
		bwA = replystr[6];
	}
	return modeA;
}

int RIG_TT565::get_modetype(int n)
{
	return RIG_TT565_mode_type[n];
}


