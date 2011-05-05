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
	waitFB("set att");
}

int RIG_IC756PRO2::get_attenuator()
{
	string cstr = "\x11";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get att")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			if (replystr[p+6] == 0x06) {
				atten_level = 1;
				atten_label("6 dB", true);
			} else if (replystr[p+6] == 0x12) {
				atten_level = 2;
				atten_label("12 dB", true);
			} else if (replystr[p+6] == 0x18) {
				atten_level = 3;
				atten_label("18 dB", true);
			} else if (replystr[p+6] == 0x00) {
				atten_level = 0;
				atten_label("Att", false);
			}
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
	waitFB("set preamp");
}

int RIG_IC756PRO2::get_preamp()
{
	string cstr = "\x16\x02";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get preamp")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			if (replystr[p+6] == 0x01) {
				preamp_label("Pre 1", true);
				preamp_level = 1;
			} else if (replystr[p+6] == 0x02) {
				preamp_label("Pre 2", true);
				preamp_level = 2;
			} else {
				preamp_label("Pre", false);
				preamp_level = 0;
			}
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
	waitFB("set mode A");
	if (datamode) { // LSB / USB ==> use DATA mode
		cmd = pre_to;
		cmd.append("\x1A\x06\x01");
		cmd.append(post);
		waitFB("data mode");
	}
}

int RIG_IC756PRO3::get_modeA()
{
	int md = 0;
	string cstr = "\x04";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(8, "get mode A")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			md = replystr[p+5];
			if (md > 6) md--;
			A.iBW = replystr[p+6];
			cstr = "\x1A\x06";
			resp = pre_fm;
			resp.append(cstr);
			cmd = pre_to;
			cmd.append(cstr);
			cmd.append(post);
			if (waitFOR(9, "data mode?")) {
				p = replystr.rfind(resp);
				if (p != string::npos) {
					if (replystr[p+6]) {
						switch (md) {
							case 0 : md = 8; break;
							case 1 : md = 9; break;
							case 5 : md = 10; break;
							default : break;
						}
					}
				}
			}
			A.imode = md;
		}
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
	waitFB("set mode B");
	if (datamode) { // LSB / USB ==> use DATA mode
		cmd = pre_to;
		cmd.append("\x1A\x06\x01");
		cmd.append(post);
		waitFB("data mode");
	}
}

int RIG_IC756PRO3::get_modeB()
{
	int md = 0;
	string cstr = "\x04";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(8, "get mode B")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			md = replystr[p+5];
			if (md > 6) md--;
			B.iBW = replystr[p+6];
			cstr = "\x1A\x06";
			resp = pre_fm;
			resp.append(cstr);
			cmd = pre_to;
			cmd.append(cstr);
			cmd.append(post);
			if (waitFOR(9, "data mode?")) {
				p = replystr.rfind(resp);
				if (p != string::npos) {
					if (replystr[p+6]) {
						switch (md) {
							case 0 : md = 8; break;
							case 1 : md = 9; break;
							case 5 : md = 10; break;
							default : break;
						}
					}
				}
			}
			B.imode = md;
		}
	}
	return B.imode;
}

