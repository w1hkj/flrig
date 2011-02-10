/*
 * 7600 ... drivers
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */

#include "IC7600.h"

//=============================================================================
// IC-7600

const char IC7600name_[] = "IC-7600";

const char *IC7600modes_[] = {
	"LSB", "USB", "AM", "CW", "RTTY", "FM", "CW-R", "RTTY-R", "PSK", "PSK-R", NULL};

const char IC7600_mode_type[] = { 
	'L', 'U', 'U', 'U', 'L', 'U', 'L', 'U', 'U', 'L' };

const char IC7600_mode_nbr[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x07, 0x08, 0x12, 0x13 };

RIG_IC7600::RIG_IC7600() {
	defaultCIV = 0x7A;
	name_ = IC7600name_;
	modes_ = IC7600modes_;
	_mode_type = IC7600_mode_type;
	adjustCIV(defaultCIV);
};

//======================================================================
// IC7600 unique commands
//======================================================================

void RIG_IC7600::select_vfoA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\xD0';
	cmd.append(post);
	sendICcommand(cmd, 6);
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.c_str(), cmd.length()));
	checkresponse(6);
}

void RIG_IC7600::select_vfoB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\xD1';
	cmd.append(post);
	sendICcommand(cmd, 6);
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.c_str(), cmd.length()));
	checkresponse(6);
}

void RIG_IC7600::set_modeA(int val)
{
	A.imode = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += IC7600_mode_nbr[val];
	cmd += A.iBW;
	cmd.append( post );
	sendICcommand (cmd, 6);
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	checkresponse(6);
}

int RIG_IC7600::get_modeA()
{
	int md = 0;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	if (sendICcommand (cmd, 8 )) {
		for (md = 0; md < 10; md++) if (replystr[5] == IC7600_mode_nbr[md]) break;
		if (md == 10) md = 0;
		A.iBW = replystr[6];
		A.imode = md;
	} else
		checkresponse(8);
	return A.imode;
}

void RIG_IC7600::set_modeB(int val)
{
	B.imode = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += IC7600_mode_nbr[val];
	cmd += B.iBW;
	cmd.append( post );
	sendICcommand (cmd, 6);
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	checkresponse(6);
}

int RIG_IC7600::get_modeB()
{
	int md = 0;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	if (sendICcommand (cmd, 8 )) {
		for (md = 0; md < 10; md++) if (replystr[5] == IC7600_mode_nbr[md]) break;
		if (md == 10) md = 0;
		B.iBW = replystr[6];
		B.imode = md;
	} else
		checkresponse(8);
	return B.imode;
}

int RIG_IC7600::get_bwA()
{
	return A.iBW;
}

void RIG_IC7600::set_bwA(int val)
{
	A.iBW = val;
	set_modeA(A.imode);
}

int RIG_IC7600::get_bwB()
{
	return B.iBW;
}

void RIG_IC7600::set_bwB(int val)
{
	B.iBW = val;
	set_modeB(B.imode);
}

void RIG_IC7600::set_mic_gain(int v)
{
	ICvol = (int)(v * 255 / 100);
	if (!progStatus.USBaudio) {
		cmd = pre_to;
		cmd.append("\x14\x0B");
		cmd.append(to_bcd(ICvol, 3));
		cmd.append( post );
	} else {
		cmd = pre_to;
		cmd += '\x1A'; cmd += '\x05';
		cmd += '\x00'; cmd += '\x29';
		cmd.append(to_bcd(ICvol, 3));
		cmd.append( post );
	}
	sendICcommand (cmd, 6);
	checkresponse(6);
	if (RIG_DEBUG)
		LOG_WARN("%s", str2hex(cmd.data(), cmd.length()));
}

// alh added ++++++++++++++++++++++++++++

void RIG_IC7600::set_attenuator(int val)
{
	int cmdval = 0;
	if (atten_level == 0) {
		atten_level = 1;
		atten_label("6 dB", true);
		cmdval = 0x06;
	} else if (atten_level == 1) {
		atten_level = 2;
		atten_label("12 dB", true);
		cmdval = 0x12;
	} else if (atten_level == 2) {
		atten_level = 3;
		atten_label("18 dB", true);
		cmdval = 0x18;
	} else if (atten_level == 3) {
		atten_level = 0;
		atten_label("Att", false);
		cmdval = 0x00;
	}
	cmd = pre_to;
	cmd += '\x11';
	cmd += cmdval;
	cmd.append( post );
	sendICcommand(cmd,6);
	checkresponse(6);
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

int RIG_IC7600::get_attenuator()
{
	cmd = pre_to;
	cmd += '\x11';
	cmd.append( post );
	if (sendICcommand(cmd,7)) {
		if (replystr[5] == 0x06) {
			atten_level = 1;
			atten_label("6 dB", true);
		} else if (replystr[6] == 0x12) {
			atten_level = 2;
			atten_label("12 dB", true);
		} else if (replystr[6] == 0x18) {
			atten_level = 3;
			atten_label("18 dB", true);
		} else if (replystr[6] == 0x00) {
			atten_level = 0;
			atten_label("Att", false);
		}
	}
	return atten_level;
}
