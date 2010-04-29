/*
 * Icom 7200, 7700 ... drivers
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */

#include "IC7200.h"

//=============================================================================
// IC-7200

const char IC7200name_[] = "IC-7200";

RIG_IC7200::RIG_IC7200() {
	defaultCIV = 0x76;
	name_ = IC7200name_;
	adjustCIV(defaultCIV);
};

//======================================================================
// IC7200 unique commands
//======================================================================

void RIG_IC7200::set_mode(int val)
{
	mode_ = val;
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
	cmd += filter_nbr;
	cmd.append( post );
	sendICcommand (cmd, 6);
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	checkresponse(6);
	if (datamode) { // LSB / USB ==> use DATA mode
		cmd = pre_to;
		cmd.append("\x1A\x04\x01\0x01");
		cmd.append(post);
		sendICcommand(cmd, 6);
		checkresponse(6);
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	}
}

int RIG_IC7200::get_mode()
{
	int md;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	if (sendICcommand (cmd, 8 )) {
		md = replystr[5];
		if (md > 6) md--;
		filter_nbr = replystr[6];
		cmd = pre_to;
		cmd.append("\x1A\x04");
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
			mode_ = md;
		} else {
			checkresponse(8);
		}
	} else {
		checkresponse(8);
	}
	return mode_;
}

void RIG_IC7200::set_mic_gain(int v)
{
	ICvol = (int)(v * 255 / 100);
	if (!progStatus.USBaudio) {
		cmd = pre_to;
		cmd.append("\x14\x01");
		cmd.append(to_bcd(ICvol, 3));
		cmd.append( post );
	} else {
		cmd = pre_to;
		cmd.append("\x1A\x03\x25");
		cmd.append(to_bcd(ICvol, 3));
		cmd.append( post );
	}
	sendICcommand (cmd, 6);
	checkresponse(6);
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

