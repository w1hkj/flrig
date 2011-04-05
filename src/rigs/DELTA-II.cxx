/*
 * TenTec Delta-II driver
 *
 * a part of flrig
 * 
 * Copyright 2011, Dave Freese, W1HKJ
 * 
 */

#include "DELTA-II.h"

//=============================================================================
// TT-535

const char RIG_TT535name_[] = "DELTA-II";

const char *RIG_TT535modes_[] = {
		"LSB", "USB", "AM", "CW", "FM", NULL};
static const char RIG_TT535_mode_type[] = {'L', 'U', 'U', 'L', 'U'};

RIG_TT535::RIG_TT535() {
	name_ = RIG_TT535name_;
	modes_ = RIG_TT535modes_;
	comm_baudrate = BR1200;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 10;
	comm_timeout = 50;
	comm_echo = true;
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = false;
	comm_rtsptt = false;
	comm_dtrptt = false;
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

long RIG_TT535::get_vfoA ()
{
//	cmd = pre_to;
//	cmd += '\x03';
//	cmd.append( post );
//	if (!sendICcommand(cmd, 11)) {
//		checkresponse(11);
//		return freqA;
//	}
//	freqA = fm_bcd_be(&replystr[5], 10);
	return freqA;
}


void RIG_TT535::set_vfoA (long freq)
{
	freqA = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 8 ) );
	cmd.append( post );
	sendICcommand(cmd, 6);
	checkresponse(6);
}

void RIG_TT535::set_vfoB (long freq)
{
	freqB = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 8 ) );
	cmd.append( post );
	sendICcommand(cmd, 6);
	checkresponse(6);
}

long RIG_TT535::get_vfoB ()
{
//	cmd = pre_to;
//	cmd += '\x03';
//	cmd.append( post );
//	if (!sendICcommand(cmd, 11)) {
//		checkresponse(11);
//		return freqA;
//	}
//	freqA = fm_bcd_be(&replystr[5], 10);
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
	sendICcommand(cmd, 6);
	checkresponse(6);
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
	sendICcommand(cmd, 6);
	checkresponse(6);
}

// same with get mode
int RIG_TT535::get_modeB()
{
//	cmd = pre_to;
//	cmd += '\x04';
//	cmd.append(post);
//	if( sendICcommand (cmd, 8 )) {
//		modeA = replystr[5];
//		bwA = replystr[6];
//	}
	return modeB;
}


int RIG_TT535::get_modetype(int n)
{
	return RIG_TT535_mode_type[n];
}


