/*
 * Icom IC-735, 735PRO, 756, 756PRO ... drivers
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */

#include "IC735.h"

//=============================================================================
// IC-735
//
const char IC735name_[] = "IC-735";
const char *IC735modes_[] = { "LSB", "USB", "AM", "CW", "RTTY", "FM", NULL};

RIG_IC735::RIG_IC735() {
	name_ = IC735name_;
	modes_ = IC735modes_;
	comm_baudrate = BR1200;
	stopbits = 2;
	comm_retries = 2;
	comm_wait = 10;
	comm_timeout = 50;
	comm_echo = true;
	comm_rtscts = false;
	comm_rtsplus = true;
	comm_dtrplus = true;
	comm_catptt = false;
	comm_rtsptt = false;
	comm_dtrptt = false;
	modeA = 1;
	bwA = 0;

	has_mode_control = true;
	has_bandwidth_control = true;

	defaultCIV = 0x04;
	adjustCIV(defaultCIV);

	precision = 100;
	ndigits = 6;

};

//=============================================================================
void RIG_IC735::selectA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x00';
	cmd.append(post);
	waitFB("select A");
}

void RIG_IC735::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x01';
	cmd.append(post);
	waitFB("select B");
}

long RIG_IC735::get_vfoA ()
{
	string cstr = "\x03";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(10, "get vfo A")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			freqA = fm_bcd_be(&replystr[p+5], 8);
	}
	return freqA;
}

void RIG_IC735::set_vfoA (long freq)
{
	freqA = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 8 ) );
	cmd.append( post );
	waitFB("set vfo A");
}

long RIG_IC735::get_vfoB ()
{
	string cstr = "\x03";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(10, "get vfo B")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			freqB = fm_bcd_be(&replystr[p+5], 8);
	}
	return freqB;
}

void RIG_IC735::set_vfoB (long freq)
{
	freqA = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 8 ) );
	cmd.append( post );
	waitFB("set vfo B");
}

void RIG_IC735::set_modeA(int val)
{
	modeA = val;
	cmd = pre_to;
	cmd += "\x06";
	cmd += modeA;		   // set the mode byte
	cmd.append( post );
	waitFB("set mode");
}

int RIG_IC735::get_modeA()
{
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	string resp = pre_fm;
	resp += '\x04';
	if (waitFOR(7, "get mode")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			modeA = replystr[p+5];
		}
	}
	return modeA;
}
