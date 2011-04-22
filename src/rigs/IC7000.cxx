/*
 * Icom IC-7000 driver
 *
 * a part of flrig
 *
 * Copyright 2011, Dave Freese, W1HKJ
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

const char *IC7000_SSB_CWwidths[] = {
  "50",  "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
 "600",  "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700", "2800", "2900", "3000", "3100", "3200", "3300", "3400", "3500",
"3600",
NULL};

const char *IC7000_RTTYwidths[] = {
  "50",  "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
 "600",  "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700", 
NULL};

const char *IC7000_AMwidths[] = {
 "200",  "400",  "600",  "800", "1000", "1200", "1400", "1600", "1800", "2000",
"2200", "2400", "2600", "2800", "3000", "3200", "3400", "3600", "3800", "4000",
"4200", "4400", "4600", "4800", "5000", "5200", "5400", "5600", "5800", "6000",
"6200", "6400", "6600", "6800", "7000", "7200", "7400", "7600", "7800", "8000",
"8200", "8400", "8600", "8800", "9000", "9200", "9400", "9600", "9800", "10000",
NULL};

const char *IC7000_FMwidths[] = { "FIXED", NULL };

RIG_IC7000::RIG_IC7000() {
	defaultCIV = 0x70;
	name_ = IC7000name_;
	modes_ = IC7000modes_;
	bandwidths_ = IC7000_SSB_CWwidths;
	_mode_type = IC7000_mode_type;

	A.freq = 14070000;
	A.imode = 1;
	A.iBW = 28;
	B.freq = 14070000;
	B.imode = 1;
	B.iBW = 28;

	adjustCIV(defaultCIV);


	restore_mbw = false;

	has_auto_notch = true;

};

//======================================================================
// IC7000 unique commands
//======================================================================

void RIG_IC7000::initialize()
{
	cmd = "";
	showresp(WARN, HEX, "IC7000 init");
}

void RIG_IC7000::selectA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x00';
	cmd.append(post);
	sendICcommand(cmd, 6);
	showresp(WARN, HEX, "select A");
}

void RIG_IC7000::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x01';
	cmd.append(post);
	sendICcommand(cmd, 6);
	showresp(WARN, HEX, "select B");
}

long RIG_IC7000::get_vfoA ()
{
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	int ret = !sendICcommand(cmd, 11);
	showresp(WARN, HEX, "get vfoA");
	if (ret == 11)
		A.freq = fm_bcd_be(&replystr[5], 10);
	return A.freq;
}

void RIG_IC7000::set_vfoA (long freq)
{
return;
	A.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	sendICcommand(cmd, 6);
	showresp(WARN, HEX, "set vfoA");
}

long RIG_IC7000::get_vfoB ()
{
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	int ret = !sendICcommand(cmd, 11);
	showresp(WARN, HEX, "get vfoB");
	if (ret == 11)
		B.freq = fm_bcd_be(&replystr[5], 10);
	return B.freq;
}

void RIG_IC7000::set_vfoB (long freq)
{
return;
	B.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	sendICcommand(cmd, 6);
	showresp(WARN, HEX, "set vfoB");
}

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
	cmd.append( post );
	sendICcommand (cmd, 6);
	showresp(WARN, HEX, "set mode A");
}

int RIG_IC7000::get_modeA()
{
	int md = A.imode;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	int ret = sendICcommand (cmd, 8);
	showresp(WARN, HEX, "get mode A");
	if (ret == 8) {
		md = replystr[5];
		if (md > 6) md--;
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
	cmd.append( post );
	sendICcommand (cmd, 6);
	showresp(WARN, HEX, "set mode B");
}

int RIG_IC7000::get_modeB()
{
	int md = B.imode;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	int ret = sendCommand (cmd);
	showresp(WARN, HEX, "get mode B");
	if (ret == 8) {
		md = replystr[5];
		if (md > 6) md--;
	}
	return (B.imode = md);
}

int  RIG_IC7000::get_bwA()
{
	cmd = pre_to;
	cmd += "\x1A\x03";
	cmd.append( post );
	int ret = sendICcommand(cmd, 8);
	showresp(WARN, HEX, "get bwA");
	if (ret == 8)
		A.iBW = (fm_bcd(&replystr[6],2));
	return A.iBW;
}

int  RIG_IC7000::get_bwB()
{
	cmd = pre_to;
	cmd += "\x1A\x03";
	cmd.append( post );
	int ret = sendICcommand(cmd, 8);
	showresp(WARN, HEX, "get bwB");
	if (ret == 8)
		B.iBW = (fm_bcd(&replystr[6],2));
	return B.iBW;
}

void RIG_IC7000::set_bwA(int val)
{
	A.iBW = val;
	cmd = pre_to;
	cmd.append("\x1A\x03");
	cmd.append(to_bcd(val, 2));
	cmd.append( post );
	sendICcommand(cmd, 6);
	showresp(WARN, HEX, "set bw A");
}

void RIG_IC7000::set_bwB(int val)
{
	B.iBW = val;
	cmd = pre_to;
	cmd.append("\x1A\x03");
	cmd.append(to_bcd(val, 2));
	cmd.append( post );
	sendICcommand(cmd, 6);
	showresp(WARN, HEX, "set bw B");
}

int RIG_IC7000::adjust_bandwidth(int m)
{
	if (m == 3 || m == 6) { //CW
		bandwidths_ = IC7000_SSB_CWwidths;
		return 12;
	}
	if (m == 4 || m == 7) { //RTTY
		bandwidths_ = IC7000_RTTYwidths;
		return 12;
	}
	if (m == 2) { // AM
		bandwidths_ = IC7000_AMwidths;
		return 28;
	}
	if (m == 5) { // FM
		bandwidths_ = IC746PRO_AMFMwidths;
		return 0;
	}
// default
//	if (m == 0 || m == 1){ //SSB
	bandwidths_ = IC7000_SSB_CWwidths;
	return 32;
}

int RIG_IC7000::def_bandwidth(int m)
{
	return adjust_bandwidth(m);
}


int RIG_IC7000::get_attenuator()
{
	cmd = pre_to;
	cmd += '\x11';
	cmd.append( post );
	int ret = sendICcommand(cmd,7);
	showresp(WARN, HEX, "get att");
	if (ret == 7)
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
	showresp(WARN, HEX, "set att");
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
	if (val)
		showresp(WARN, HEX, "set preON");
	else
		showresp(WARN, HEX, "set preOFF");
}

int RIG_IC7000::get_preamp()
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd.append( post );
	int ret = sendICcommand (cmd, 8);
	showresp(WARN, HEX, "get pre");
	if (ret == 8)
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
	showresp(WARN, HEX, "set notch");
}

void RIG_IC7000::set_split(bool val)
{
	cmd = pre_to;
	cmd += 0x0F;
	cmd += val ? 0x10 : 0x00;
	cmd.append(post);
	sendICcommand(cmd, 6);
	checkresponse();
	showresp(WARN, HEX, "set split");
}

