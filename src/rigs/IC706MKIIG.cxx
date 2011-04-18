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
const char IC706MKIIG_mode_type[] =
	{ 'L', 'U', 'U', 'L', 'L', 'U', 'U'};
const char *IC706MKIIG_widths[] = { "WIDE", "NARR", NULL};

RIG_IC706MKIIG::RIG_IC706MKIIG() {
	name_ = IC706MKIIGname_;
	modes_ = IC706MKIIGmodes_;
	bandwidths_ = IC706MKIIG_widths;
	_mode_type = IC706MKIIG_mode_type;
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
	modeA = 1;
	bwA = 0;

	has_mode_control = true;
	has_bandwidth_control = true;
	has_attenuator_control = true;

	defaultCIV = 0x58;
	adjustCIV(defaultCIV);
};

//=============================================================================

void RIG_IC706MKIIG::select_vfoA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x00';
	cmd.append(post);
	sendICcommand(cmd, 6);
	checkresponse();
}

void RIG_IC706MKIIG::select_vfoB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x01';
	cmd.append(post);
	sendICcommand(cmd, 6);
	checkresponse();
}

long RIG_IC706MKIIG::get_vfoA ()
{
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	if (!sendCommand(cmd, 11)) {
		checkresponse();
		return freqA;
	}
	freqA = fm_bcd_be(&replystr[5], 10);
	return freqA;
}

void RIG_IC706MKIIG::set_vfoA (long freq)
{
	freqA = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	sendICcommand(cmd, 6);
	checkresponse();
}

long RIG_IC706MKIIG::get_vfoB ()
{
	return freqB;
}

void RIG_IC706MKIIG::set_vfoB (long freq)
{
	select_vfoB();
	freqB = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	sendICcommand(cmd, 6);
	checkresponse();
	select_vfoA();
}

void RIG_IC706MKIIG::set_split(bool b)
{
	cmd = pre_to;
	cmd += '\x0F';
	cmd += b ? '\x01' : '\x00';
	cmd.append( post );
	sendICcommand(cmd, 6);
	checkresponse();
}

void RIG_IC706MKIIG::set_modeA(int val)
{
	modeA = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += val > 5 ? val + 2 : val;
	cmd += bwA;
	cmd.append( post );
	sendICcommand (cmd, 6);
	checkresponse();
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

int RIG_IC706MKIIG::get_modeA()
{
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	if (sendICcommand (cmd, 8 )) {
		modeA = replystr[5];
		if (modeA > 6) modeA -= 2;
		bwA = replystr[6];
	}
	return modeA;
}

int RIG_IC706MKIIG::get_modetype(int n)
{
	return _mode_type[n];
}

void RIG_IC706MKIIG::set_bwA(int val)
{
	bwA = val;
	set_modeA(modeA);
}

int  RIG_IC706MKIIG::get_bwA()
{
	return bwA;
}

void RIG_IC706MKIIG::set_attenuator(int val)
{
	cmd = pre_to;
	cmd += '\x11';
	cmd += val ? '\x20' : '\x00';
	cmd.append( post );
	sendCommand (cmd, 6);
	checkresponse();
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
