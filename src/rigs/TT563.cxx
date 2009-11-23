/*
 * TenTec 563 (Omni-VI) driver
 * based on the IC746 driver
 *
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */

#include "TT563.h"

//=============================================================================
// TT-563

const char RIG_TT563name_[] = "OMNI-VI";

const char *RIG_TT563modes_[] = {
		"LSB", "USB", "AM", "CW", "RTTY", "FM", NULL};
static const char RIG_TT563_mode_type[] = {'L', 'U', 'U', 'U', 'L', 'U'};
const char *RIG_TT563widths[] = { "NARR", "WIDE", NULL};

RIG_TT563::RIG_TT563() {
	name_ = RIG_TT563name_;
	modes_ = RIG_TT563modes_;
	bandwidths_ = RIG_TT563widths;
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
	mode_ = 1;
	bw_ = 0;

	has_mode_control = true;
	has_ptt_control = true;

	pre_to[2] = ok[3] = bad[3] = pre_fm[3] = 0x04;

};

long RIG_TT563::get_vfoA ()
{
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	if (!sendICcommand(cmd, 11)) {
		checkresponse(11);
		return freq_;
	}
	freq_ = fm_bcd_be(&replystr[5], 10);
	return freq_;
}

void RIG_TT563::set_vfoA (long freq)
{
	freq_ = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 8 ) );
	cmd.append( post );
	sendICcommand(cmd, 6);
	checkresponse(6);
}

void RIG_TT563::set_PTT_control(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += val ? '\x01' : '\x02';
	cmd.append( post );
	sendICcommand(cmd,6);
	checkresponse(6);
}

void RIG_TT563::set_mode(int md)
{
	mode_ = md;
	cmd = pre_to;
	cmd += '\x06';
	cmd += mode_;
	cmd.append(post);
	sendICcommand(cmd, 6);
	checkresponse(6);
}

int RIG_TT563::get_mode()
{
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	if( sendICcommand (cmd, 8 )) {
		mode_ = replystr[5];
		bw_ = replystr[6];
	}
	return mode_;
}

int RIG_TT563::get_modetype(int n)
{
	return RIG_TT563_mode_type[n];
}


