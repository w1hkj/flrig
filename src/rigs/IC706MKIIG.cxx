/*
 * Icom IC-706MKIIG
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */

#include "IC706MKIIG.h"

//=============================================================================
// IC-706MKIIG
//
const char IC706MKIIGname_[] = "IC-706MKIIG";
const char *IC706MKIIGmodes_[] = { "LSB", "USB", "AM", "CW", "RTTY", "FM", "WFM", NULL};
const char *IC706MKIIG_widths[] = { "NARR", "WIDE", NULL};

RIG_IC706MKIIG::RIG_IC706MKIIG() {
	name_ = IC706MKIIGname_;
	modes_ = IC706MKIIGmodes_;
	bandwidths_ = IC706MKIIG_widths;
	comm_baudrate = BR9600;
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
	mode_ = 1;
	bw_ = 0;

	has_attenuator_control = true;
//	has_swr_control = true;
	
	pre_to[2] = ok[3] = bad[3] = pre_fm[3] = 0x58;
};

//=============================================================================

long RIG_IC706MKIIG::get_vfoA ()
{
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	if (!sendCommand(cmd, 11)) {
		checkresponse(11);
		return freq_;
	}
	freq_ = fm_bcd_be(&replystr[5], 10);
	return freq_;
}

void RIG_IC706MKIIG::set_vfoA (long freq)
{
	freq_ = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	sendICcommand(cmd, 6);
	checkresponse(6);
}

void RIG_IC706MKIIG::set_attenuator(int val)
{
	cmd = pre_to;
	cmd += '\x11';
	cmd += val ? '\x20' : '\x00';
	cmd.append( post );
	sendCommand (cmd, 6);
	checkresponse(6);
}

int RIG_IC706MKIIG::get_smeter()
{
	cmd = pre_to;
	cmd.append("\x15\x02");
	cmd.append( post );
	if (sendICcommand (cmd, 9)) {
		return fm_bcd(&replystr[6], 3) / 2.55;
	} else
		return 0;
}
