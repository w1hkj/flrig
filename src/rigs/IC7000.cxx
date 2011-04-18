/*
 * Icom IC-746, 746PRO, 756, 756PRO
 * 7000, 7200, 7700 ... drivers
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */

#include "IC7000.h"

//=============================================================================
// IC-7000

const char IC7000name_[] = "IC-7000";

const char *IC7000modes_[] = {
		"LSB", "USB", "AM", "CW", "RTTY", "FM", "CW-R", "RTTY-R", NULL};

const char IC7000_mode_type[] =
	{ 'L', 'U', 'U', 'U', 'L', 'U', 'L', 'U' };

RIG_IC7000::RIG_IC7000() {
	defaultCIV = 0x70;
	name_ = IC7000name_;
	modes_ = IC7000modes_;
	_mode_type = IC7000_mode_type;
	adjustCIV(defaultCIV);
	restore_mbw = false;

	has_auto_notch = true;

};

//======================================================================
// IC7000 unique commands
//======================================================================

void RIG_IC7000::set_modeA(int val)
{
	A.imode = val;
	switch (val) {
		case 7  : val = 8; break;
		case 6  : val = 7; break;
		default: break;
	}
	cmd = pre_to;
	cmd += '\x06';
	cmd += val;
	cmd += A.iBW;
	cmd.append( post );
	sendICcommand (cmd, 6);
	checkresponse();
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

int RIG_IC7000::get_modeA()
{
	int md = A.imode;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	if (sendICcommand (cmd, 8 )) {
		md = replystr[5];
		if (md > 6) md--;
		A.iBW = replystr[6];
	} else {
		checkresponse();
	}
	return (A.imode = md);
}

void RIG_IC7000::set_modeB(int val)
{
	B.imode = val;
	switch (val) {
		case 7  : val = 8; break;
		case 6  : val = 7; break;
		default: break;
	}
	cmd = pre_to;
	cmd += '\x06';
	cmd += val;
	cmd += A.iBW;
	cmd.append( post );
	sendICcommand (cmd, 6);
	checkresponse();
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

int RIG_IC7000::get_modeB()
{
	int md = B.imode;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	if (sendICcommand (cmd, 8 )) {
		md = replystr[5];
		if (md > 6) md--;
		B.iBW = replystr[6];
	} else {
		checkresponse();
	}
	return (B.imode = md);
}

int RIG_IC7000::adjust_bandwidth(int m)
{
	A.iBW = 1;
	if (m == 0 || m == 1) { //SSB
		bandwidths_ = IC746PRO_SSBwidths;
		return (bwA = 32);
	}
	if (m == 3 || m == 6) { //CW
		bandwidths_ = IC746PRO_SSBwidths;
		return (bwA = 14);
	}
	if (m == 4 || m == 7) { //RTTY
		bandwidths_ = IC746PRO_RTTYwidths;
		return (bwA = 28);
	}
	bandwidths_ = IC746PRO_AMFMwidths;
	return (bwA = 0);
}

int RIG_IC7000::def_bandwidth(int m)
{
	if (m == 0 || m == 1) { //SSB
		return 32;
	}
	if (m == 3 || m == 6) { //CW
		return 14;
	}
	if (m == 4 || m == 7) { //RTTY
		return 28;
	}
	return 0;
}


int RIG_IC7000::get_attenuator()
{
	cmd = pre_to;
	cmd += '\x11';
	cmd.append( post );
	if (sendICcommand(cmd,7))
		return (replystr[5] ? 1 : 0);
	return 0;
}

void RIG_IC7000::set_attenuator(int val)
{
	cmd = pre_to;
	cmd += '\x11';
	cmd += val ? '\x12' : '\x00';
	cmd.append( post );
	sendICcommand(cmd,6);
	checkresponse();
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

void RIG_IC7000::set_preamp(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd += val ? 0x01 : 0x00;
	cmd.append( post );
	sendICcommand (cmd, 6);
	checkresponse();
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

int RIG_IC7000::get_preamp()
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd.append( post );
	if (sendICcommand (cmd, 8))
		return replystr[6] ? 1 : 0;
	return 0;
}

void RIG_IC7000::set_auto_notch(int val)
{
	cmd = pre_to;
	cmd.append("\x16\x41");
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	sendICcommand (cmd, 6);
	checkresponse();
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}
