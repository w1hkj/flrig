/*
 * Icom IC-728
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */

#include "IC728.h"

//=============================================================================
// IC-728
//
const char IC728name_[] = "IC-728";
const char *IC728modes_[] = { "LSB", "USB", "AM", "CW", "RTTY", "FM", NULL};
const char *IC728_widths[] = { "NARR", "WIDE", NULL};

RIG_IC728::RIG_IC728() {
	name_ = IC728name_;
	modes_ = IC728modes_;
	bandwidths_ = IC728_widths;
	comm_baudrate = BR1200;
	stopbits = 2;
	comm_retries = 2;
	comm_wait = 5;
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

	defaultCIV = 0x38;
	adjustCIV(defaultCIV);

};

//=============================================================================

long RIG_IC728::get_vfoA ()
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

void RIG_IC728::set_vfoA (long freq)
{
	freqA = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 8 ) );
	cmd.append( post );
	waitFB("set vfo A");
}

