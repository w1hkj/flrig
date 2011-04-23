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
	sendCommand(cmd);
	showresp(WARN, HEX, "select A");
}

void RIG_IC7000::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x01';
	cmd.append(post);
	sendCommand(cmd);
	showresp(WARN, HEX, "select B");
}

long RIG_IC7000::get_vfoA ()
{
	string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	sendCommand(cmd);
	showresp(WARN, HEX, "get vfoA");
	size_t p = replystr.rfind(resp);
	if (p != string::npos && ((p+11) <= replystr.length()))
		A.freq = fm_bcd_be(&replystr[p+5], 10);
	return A.freq;
}

void RIG_IC7000::set_vfoA (long freq)
{
	A.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	sendCommand(cmd);
	showresp(WARN, HEX, "set vfoA");
}

long RIG_IC7000::get_vfoB ()
{
	string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	sendCommand(cmd);
	showresp(WARN, HEX, "get vfoB");
	size_t p = replystr.rfind(resp);
	if (p != string::npos && ((p+11) <= replystr.length()))
		B.freq = fm_bcd_be(&replystr[p+5], 10);
	return B.freq;
}

void RIG_IC7000::set_vfoB (long freq)
{
	B.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	sendCommand(cmd);
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
	sendCommand (cmd);
	showresp(WARN, HEX, "set mode A");
}

int RIG_IC7000::get_modeA()
{
	int md;
	string resp = pre_fm;
	resp += '\x04';
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	sendCommand(cmd);
	showresp(WARN, HEX, "get mode A");
	size_t p = replystr.rfind(resp);
	if (p != string::npos && ((p+8) <= replystr.length())) {
		md = replystr[p+5];
		if (md > 6) md--;
		A.imode = md;
	}
	return A.imode;
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
	sendCommand (cmd);
	showresp(WARN, HEX, "set mode B");
}

int RIG_IC7000::get_modeB()
{
	int md;
	string resp = pre_fm;
	resp += '\x04';
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	sendCommand(cmd);
	showresp(WARN, HEX, "get mode B");
	size_t p = replystr.rfind(resp);
	if (p != string::npos && ((p+5) <= replystr.length())) {
		md = replystr[p+5];
		if (md > 6) md--;
		B.imode = md;
	}
	return B.imode;
}

int  RIG_IC7000::get_bwA()
{
	string cstr = "\x1A\x03";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	sendCommand(cmd);
	showresp(WARN, HEX, "get bwA");
	size_t p = replystr.rfind(resp);
	if (p != string::npos && ((p+8) <= replystr.length()))
		A.iBW = (fm_bcd(&replystr[p+6],2));
	return A.iBW;
}

int  RIG_IC7000::get_bwB()
{
	string cstr = "\x1A\x03";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	sendCommand(cmd);
	showresp(WARN, HEX, "get bwA");
	size_t p = replystr.rfind(resp);
	if (p != string::npos && ((p+8) <= replystr.length()))
		B.iBW = (fm_bcd(&replystr[p+6],2));
	return B.iBW;
}

void RIG_IC7000::set_bwA(int val)
{
	A.iBW = val;
	cmd = pre_to;
	cmd.append("\x1A\x03");
	cmd.append(to_bcd(val, 2));
	cmd.append( post );
	sendCommand(cmd);
	showresp(WARN, HEX, "set bw A");
}

void RIG_IC7000::set_bwB(int val)
{
	B.iBW = val;
	cmd = pre_to;
	cmd.append("\x1A\x03");
	cmd.append(to_bcd(val, 2));
	cmd.append( post );
	sendCommand(cmd);
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
	bandwidths_ = IC7000_SSB_CWwidths;
	return 32;
}

int RIG_IC7000::def_bandwidth(int m)
{
	return adjust_bandwidth(m);
}


int RIG_IC7000::get_attenuator()
{
	string cstr = "\x11";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	sendCommand(cmd);
	showresp(WARN, HEX, "get att");
	size_t p = replystr.rfind(resp);
	if (p != string::npos && ((p+5) <= replystr.length()))
		return (replystr[p+5] ? 1 : 0);
	return progStatus.attenuator;
}

void RIG_IC7000::set_attenuator(int val)
{
	cmd = pre_to;
	cmd += '\x11';
	cmd += val ? '\x12' : '\x00';
	cmd.append( post );
	sendCommand(cmd);
	showresp(WARN, HEX, "set att");
}

void RIG_IC7000::set_preamp(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd += val ? 0x01 : 0x00;
	cmd.append( post );
	sendCommand (cmd, 6);
	if (val)
		showresp(WARN, HEX, "set preON");
	else
		showresp(WARN, HEX, "set preOFF");
}

int RIG_IC7000::get_preamp()
{
	string cstr = "\x16\x02";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	sendCommand(cmd);
	showresp(WARN, HEX, "get pre");
	size_t p = replystr.rfind(resp);
	if (p != string::npos && ((p+6) <= replystr.length()))
		return replystr[6] ? 1 : 0;
	return progStatus.preamp;
}

void RIG_IC7000::set_auto_notch(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x41';
	cmd += (unsigned char)val;
	cmd.append( post );
	sendCommand (cmd, 6);
	showresp(WARN, HEX, "set AN");
}

int RIG_IC7000::get_auto_notch()
{
	string cstr = "\x16\x41";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	sendCommand(cmd);
	showresp(WARN, HEX, "get AN");
	size_t p = replystr.rfind(resp);
	if (p != string::npos && ((p+6) < replystr.length())) {
		if (replystr[p+6] == 0x01) {
			auto_notch_label("AN", true);
			return true;
		} else {
			auto_notch_label("AN", false);
			return false;
		}
	}
	return progStatus.auto_notch;
}

void RIG_IC7000::set_split(bool val)
{
	cmd = pre_to;
	cmd += 0x0F;
	cmd += val ? 0x10 : 0x00;
	cmd.append(post);
	sendCommand(cmd);
	showresp(WARN, HEX, "set split");
}

// Volume control val 0 ... 100
void RIG_IC7000::set_volume_control(int val)
{
	int ICvol = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append(to_bcd(ICvol, 3));
	cmd.append( post );
	sendICcommand (cmd, 6);
	showresp(WARN, HEX, "set vol");
}

int RIG_IC7000::get_volume_control()
{
	string cstr = "\x14\x01";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	sendCommand(cmd);
	showresp(WARN, HEX, "get vol");
	size_t p = replystr.rfind(resp);
	if (p != string::npos && ((p + 9) < replystr.length()))
		return ((int)(fm_bcd(&replystr[p + 2],3) * 100 / 255));
	return progStatus.volume;
}

void RIG_IC7000::get_vol_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

void RIG_IC7000::set_rf_gain(int val)
{
	int ICrfg = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x02");
	cmd.append(to_bcd(ICrfg, 3));
	cmd.append( post );
	sendICcommand (cmd, 6);
	showresp(WARN, HEX, "set RF");
}

int RIG_IC7000::get_rf_gain()
{
	string cstr = "\x14\x02";
	string resp = pre_fm;
	cmd = pre_to;
	cmd.append(cstr).append(post);
	resp.append(cstr);
	sendCommand(cmd);
	showresp(WARN, HEX, "get RF");
	size_t p = replystr.rfind(resp);
	if (p != string::npos && ((p + 9) < replystr.length()))
		return ((int)(fm_bcd(&replystr[p + 6],3) * 100 / 255));
	return progStatus.rfgain;
}

void RIG_IC7000::set_power_control(double val)
{
	cmd = pre_to;
	cmd.append("\x14\x0A");
	cmd.append(to_bcd((int)(val * 255 / 100), 3));
	cmd.append( post );
	sendICcommand (cmd, 6);
	showresp(WARN, HEX, "set power");
}

int RIG_IC7000::get_power_control()
{
	string cstr = "\x14\x0A";
	string resp = pre_fm;
	cmd = pre_to;
	cmd.append(cstr).append(post);
	resp.append(cstr);
	sendCommand(cmd);
	showresp(WARN, HEX, "get power");
	size_t p = replystr.rfind(resp);
	if (p != string::npos && ((p + 9) < replystr.length()))
		return ((int)(fm_bcd(&replystr[p + 6],3) * 100 / 255));
	return progStatus.power_level;
}

