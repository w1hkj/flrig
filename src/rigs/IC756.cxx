/*
 * Icom 756, 756PRO
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */

#include "IC756.h"
#include "debug.h"
#include "support.h"

//=============================================================================
// 756PRO-II

const char IC756PRO2name_[] = "IC-756PRO-II";

RIG_IC756PRO2::RIG_IC756PRO2() {
	defaultCIV = 0x64;
	name_ = IC756PRO2name_;
	atten_level = 0;
	preamp_level = 0;
	adjustCIV(defaultCIV);
};

//=============================================================================
// 756PRO-III

const char IC756PRO3name_[] = "IC-756PRO-III";

RIG_IC756PRO3::RIG_IC756PRO3() {
	defaultCIV = 0x6E;
	name_ = IC756PRO3name_;
	adjustCIV(defaultCIV);
};

//======================================================================
// IC756PRO2 unique commands
//======================================================================

void RIG_IC756PRO2::set_attenuator(int val)
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
	checkresponse();
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

int RIG_IC756PRO2::get_attenuator()
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

void RIG_IC756PRO2::set_preamp(int val)
{
	if (preamp_level == 0) {
		preamp_level = 1;
		preamp_label("Pre 1", true);
	} else if (preamp_level == 1) {
		preamp_level = 2;
		preamp_label("Pre 2", true);
	} else if (preamp_level == 2) {
		preamp_level = 0;
		preamp_label("Pre", false);
	}
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd += (unsigned char) preamp_level;
	cmd.append( post );
	sendICcommand (cmd, 6);
	checkresponse();
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

int RIG_IC756PRO2::get_preamp()
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd.append( post );
	if (sendICcommand (cmd, 8)) {
		if (replystr[6] == 0x01) {
			preamp_label("Pre 1", true);
			preamp_level = 1;
		} else if (replystr[6] == 0x02) {
			preamp_label("Pre 2", true);
			preamp_level = 2;
		} else {
			preamp_label("Pre", false);
			preamp_level = 0;
		}
	}
	return preamp_level;
}

//======================================================================
// IC756ProIII unique commands
//======================================================================

void RIG_IC756PRO3::set_modeA(int val)
{
	A.imode = val;
	bool datamode = false;
	switch (val) {
		case 10 : val = 5; datamode = true; break;
		case 9  : val = 1; datamode = true; break;
		case 8  : val = 0; datamode = true; break;
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
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	checkresponse();
	if (datamode) { // LSB / USB ==> use DATA mode
		cmd = pre_to;
		cmd.append("\x1A\x06\x01");
		cmd.append(post);
		sendICcommand(cmd, 6);
		checkresponse();
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	}
}

int RIG_IC756PRO3::get_modeA()
{
	int md;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	if (sendICcommand (cmd, 8 )) {
		md = replystr[5];
		if (md > 6) md--;
		A.iBW = replystr[6];
		cmd = pre_to;
		cmd.append("\x1A\x06");
		cmd.append(post);
		if (sendICcommand(cmd, 9)) {
			if (replystr[6]) {
				switch (md) {
					case 0 : md = 8; break;
					case 1 : md = 9; break;
					case 5 : md = 10; break;
					default : break;
				}
			}
			A.imode = md;
		} else {
			checkresponse();
		}
	} else {
		checkresponse();
	}
	return A.imode;
}

void RIG_IC756PRO3::set_modeB(int val)
{
	B.imode = val;
	bool datamode = false;
	switch (val) {
		case 10 : val = 5; datamode = true; break;
		case 9  : val = 1; datamode = true; break;
		case 8  : val = 0; datamode = true; break;
		case 7  : val = 8; break;
		case 6  : val = 7; break;
		default: break;
	}
	cmd = pre_to;
	cmd += '\x06';
	cmd += val;
	cmd += B.iBW;
	cmd.append( post );
	sendICcommand (cmd, 6);
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	checkresponse();
	if (datamode) { // LSB / USB ==> use DATA mode
		cmd = pre_to;
		cmd.append("\x1A\x06\x01");
		cmd.append(post);
		sendICcommand(cmd, 6);
		checkresponse();
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	}
}

int RIG_IC756PRO3::get_modeB()
{
	int md;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	if (sendICcommand (cmd, 8 )) {
		md = replystr[5];
		if (md > 6) md--;
		B.iBW = replystr[6];
		cmd = pre_to;
		cmd.append("\x1A\x06");
		cmd.append(post);
		if (sendICcommand(cmd, 9)) {
			if (replystr[6]) {
				switch (md) {
					case 0 : md = 8; break;
					case 1 : md = 9; break;
					case 5 : md = 10; break;
					default : break;
				}
			}
			B.imode = md;
		} else {
			checkresponse();
		}
	} else {
		checkresponse();
	}
	return B.imode;
}

