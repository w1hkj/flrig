/*
 * 7600 ... drivers
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */

#include "IC7600.h"

bool IC7600_DEBUG = true;

//=============================================================================
// IC-7600

const char IC7600name_[] = "IC-7600";

const char *IC7600modes_[] = {
	"LSB", "USB", "AM", "CW", "RTTY", "FM", "CW-R", "RTTY-R", "PSK", "PSK-R", 
	"LSB-D1", "LSB-D2", "LSB-D3",
	"USB-D1", "USB-D2", "USB-D3", NULL};

const char IC7600_mode_type[] = {
	'L', 'U', 'U', 'U', 'L', 'U', 'L', 'U', 'U', 'L', 
	'L', 'L', 'L',
	'U', 'U', 'U' };

const char IC7600_mode_nbr[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x07, 0x08, 0x12, 0x13, 
	0x00, 0x00, 0x00,
	0x01, 0x01, 0x01 };

const char *IC7600_ssb_bws[] = {
"50",    "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900",
"1000", "1100", "1200", "1300", "1400", "1500", "1600", "1700", "1800", "1900",
"2000", "2100", "2200", "2300", "2400", "2500", "2600", "2700", "2800", "2900",
"3000", "3100", "3200", "3300", "3400", "3500", "3600", NULL };

const char *IC7600_rtty_bws[] = {
"50",    "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900",
"1000", "1100", "1200", "1300", "1400", "1500", "1600", "1700", "1800", "1900",
"2000", "2100", "2200", "2300", "2400", "2500", "2600", "2700", NULL };

const char *IC7600_am_bws[] = {
"200",   "400",  "600",  "800", "1000", "1200", "1400", "1600", "1800", "2000",
"2200", "2400", "2600", "2800", "3000", "3200", "3400", "3600", "3800", "4000",
"4200", "4400", "4600", "4800", "5000", "5200", "5400", "5600", "5800", "6000",
"6200", "6400", "6600", "6800", "7000", "7200", "7400", "7600", "7800", "8000",
"8200", "8400", "8600", "8800", "9000", "9200", "9400", "9600", "9800", "10000", NULL };

const char *IC7600_fm_bws[] = {
"FIXED", NULL };

RIG_IC7600::RIG_IC7600() {
	defaultCIV = 0x7A;
	name_ = IC7600name_;
	modes_ = IC7600modes_;
	bandwidths_ = IC7600_ssb_bws;
	_mode_type = IC7600_mode_type;
	adjustCIV(defaultCIV);

	has_bandwidth_control = true;
};

//======================================================================
// IC7600 unique commands
//======================================================================

void RIG_IC7600::selectA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\xD0';
	cmd.append(post);
	sendICcommand(cmd, 6);
	checkresponse(6);
}

void RIG_IC7600::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\xD1';
	cmd.append(post);
	sendICcommand(cmd, 6);
	checkresponse(6);
}

void RIG_IC7600::set_modeA(int val)
{
	A.imode = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += IC7600_mode_nbr[val];
	cmd.append( post );
	if (IC7600_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	sendICcommand (cmd, 6);
	checkresponse(6);
// digital set / clear
	cmd = pre_to;
	cmd += '\x1A'; cmd += '\x06';
	switch (A.imode) {
		case 10 : case 13 : cmd += '\x01'; cmd += '\x01';break;
		case 11 : case 14 : cmd += '\x02'; cmd += '\x01';break;
		case 12 : case 15 : cmd += '\x03'; cmd += '\x01';break;
		default :
			cmd += '\x00'; cmd += '\x00';
	}
	cmd.append( post);
	if (IC7600_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	sendICcommand (cmd, 6);
	checkresponse(6);
}

int RIG_IC7600::get_modeA()
{
	int md = 0;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	if (sendICcommand (cmd, 8 )) {
		for (md = 0; md < 10; md++) if (replystr[5] == IC7600_mode_nbr[md]) break;
		if (md == 10) md = 0;
		A.imode = md;
	} else
		checkresponse(8);
	cmd = pre_to;
	cmd += '\x1A'; cmd += '\x06';
	cmd.append(post);
	if (sendICcommand(cmd, 9)) {
		if (replystr[6] > 0 && A.imode < 2) {
			if (replystr[6] < 4)
				A.imode = 9 + A.imode * 3 + replystr[6];
		}
	}
	return A.imode;
}

void RIG_IC7600::set_modeB(int val)
{
	B.imode = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += IC7600_mode_nbr[val];
	cmd.append( post );
	if (IC7600_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	sendICcommand (cmd, 6);
	checkresponse(6);
// digital set / clear
	cmd = pre_to;
	cmd += '\x1A'; cmd += '\x06';
	switch (B.imode) {
		case 10 : case 13 : cmd += '\x01'; cmd += '\x01';break;
		case 11 : case 14 : cmd += '\x02'; cmd += '\x01';break;
		case 12 : case 15 : cmd += '\x03'; cmd += '\x01';break;
		default :
			cmd += '\x00'; cmd += '\x00';
	}
	cmd.append( post);
	if (IC7600_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	sendICcommand (cmd, 6);
	checkresponse(6);
}

int RIG_IC7600::get_modeB()
{
	int md = 0;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	if (sendICcommand (cmd, 8 )) {
		for (md = 0; md < 10; md++) if (replystr[5] == IC7600_mode_nbr[md]) break;
		if (md == 10) md = 0;
		B.imode = md;
	} else
		checkresponse(8);
	cmd = pre_to;
	cmd += '\x1A'; cmd += '\x06';
	cmd.append(post);
	if (sendICcommand(cmd, 9)) {
		if (replystr[6] > 0 && B.imode < 2) {
			if (replystr[6] < 4)
				B.imode = 9 + B.imode * 3 + replystr[6];
		}
	}
	return B.imode;
}

int RIG_IC7600::get_bwA()
{
	if (A.imode == 5) return 0;
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(post);
	if (sendICcommand (cmd, 9)) {
		A.iBW = fm_bcd(&replystr[7], 2);
	}
	return A.iBW;
}

void RIG_IC7600::set_bwA(int val)
{
	A.iBW = val;
	if (A.imode == 5) return;
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(to_bcd(A.iBW, 2));
	cmd.append(post);
	if (IC7600_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	sendICcommand (cmd, 6);
	checkresponse(6);
}

int RIG_IC7600::get_bwB()
{
	if (B.imode == 5) return 0;
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(post);
	if (sendICcommand (cmd, 9)) {
		B.iBW = fm_bcd(&replystr[7], 2);
	}
	return B.iBW;
}

void RIG_IC7600::set_bwB(int val)
{
	B.iBW = val;
	if (B.imode == 5) return;
	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(to_bcd(A.iBW, 2));
	cmd.append(post);
	if (IC7600_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	sendICcommand (cmd, 6);
	checkresponse(6);
}

int RIG_IC7600::adjust_bandwidth(int m)
{
	int bw = 0;
	switch (m) {
		case 2: // AM
			bandwidths_ = IC7600_am_bws;
			bw = 19;
			break;
		case 5: // FM
			bandwidths_ = IC7600_fm_bws;
			bw = 0;
			break;
		case 4: case 7: // RTTY
			bandwidths_ = IC7600_rtty_bws;
			bw = 12;
			break;
		case 3: case 6: // CW
			bandwidths_ = IC7600_ssb_bws;
			bw = 12;
			break;
		case 8: case 9: // PKT
			bandwidths_ = IC7600_ssb_bws;
			bw = 34;
			break;
		case 0: case 1: // SSB
		default:
		case 10: case 11 : case 12 :
		case 13: case 14 : case 15 :
			bandwidths_ = IC7600_ssb_bws;
			bw = 34;
	}
	return bw;
}

int RIG_IC7600::def_bandwidth(int m)
{
	int bw = 0;
	switch (m) {
		case 2: // AM
			bw = 19;
			break;
		case 5: // FM
			bw = 0;
			break;
		case 4: case 7: // RTTY
			bw = 12;
			break;
		case 3: case 6: // CW
			bw = 12;
			break;
		case 8: case 9: // PKT
			bw = 34;
			break;
		case 0: case 1: // SSB
		case 10: case 11 : case 12 :
		case 13: case 14 : case 15 :
		default:
			bw = 34;
	}
	return bw;
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
