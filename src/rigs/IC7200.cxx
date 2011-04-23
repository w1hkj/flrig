/*
 * Icom 7200
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */

#include "IC7200.h"

bool DEBUG_7200 = true;

//=============================================================================
// IC-7200

const char IC7200name_[] = "IC-7200";
const char *IC7200modes_[] = { 
"LSB", "USB", "AM", "CW", "RTTY", "CW-R", "RTTY-R", 
"LSB-D", "USB-D", NULL};

const char IC7200_mode_type[] = { 
'L', 'U', 'U', 'L', 'L', 'U', 'U',
'L', 'U' };

const char *IC7200_SSBwidths[] = {
  "50",  "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700", "2800", "2900", "3000", "3100", "3200", "3300", "3400", "3500",
"3600",
NULL};

const char *IC7200_RTTYwidths[] = {
  "50",  "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
 "600",  "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700",
NULL};

const char *IC7200_AMwidths[] = {
  "200",  "400",  "600",  "800", "1000", "1200", "1400", "1600", "1800", "2000",
 "2200", "2400", "2600", "2800", "3000", "3200", "3400", "3600", "3800", "4000",
 "4200", "4400", "4600", "4800", "5000", "5200", "5400", "5600", "5800", "5000",
 "6200", "6400", "6600", "6800", "6000", "6200", "6400", "6600", "6800", "6000",
NULL};


//======================================================================
// IC7200 unique commands
//======================================================================

RIG_IC7200::RIG_IC7200() {
	name_ = IC7200name_;
	modes_ = IC7200modes_;
	_mode_type = IC7200_mode_type;
	bandwidths_ = IC7200_SSBwidths;

	comm_baudrate = BR9600;
	stopbits = 1;
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

	A.freq = 14070000;
	A.imode = 1;
	A.iBW = 32;
	B.freq = 7015000;
	B.imode = 3;
	B.iBW = 12;

	has_sql_control = true;
	has_power_control = true;
	has_volume_control = true;
	has_mode_control = true;
	has_bandwidth_control = true;
	has_micgain_control = true;
	has_attenuator_control = true;
	has_preamp_control = true;
	has_noise_control = true;
	has_noise_reduction = true;
	has_noise_reduction_control = true;
	has_auto_notch = true;
	has_rf_control = true;
	has_compON = true;
	has_vox_onoff = true;
	has_ptt_control = true;
	has_tune_control = true;

	defaultCIV = 0x76;
	adjustCIV(defaultCIV);
};

//=============================================================================
void RIG_IC7200::selectA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x00';
	cmd.append(post);
	sendICcommand(cmd, 6);
	showresp(WARN, HEX, "select A");
}

void RIG_IC7200::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x01';
	cmd.append(post);
	sendICcommand(cmd, 6);
	showresp(WARN, HEX, "select B");
}

long RIG_IC7200::get_vfoA ()
{
	string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	sendCommand(cmd);
	showresp(WARN, HEX, "get_A");
	size_t p = replystr.rfind(resp);
	if (p != string::npos && ((p+11 <= replystr.length())))
		A.freq = fm_bcd_be(&replystr[p+5], 10);
	return A.freq;
}

void RIG_IC7200::set_vfoA (long freq)
{
	A.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	sendICcommand(cmd, 6);
	showresp(WARN, HEX, "set A");
}

long RIG_IC7200::get_vfoB ()
{
	string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	sendCommand(cmd);
	showresp(WARN, HEX, "get B");
	size_t p = replystr.rfind(resp);
	if (p != string::npos && ((p+11 <= replystr.length())))
		B.freq = fm_bcd_be(&replystr[p+5], 10);
	return B.freq;
}

void RIG_IC7200::set_vfoB (long freq)
{
	selectB();
	B.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	sendICcommand(cmd, 6);
	showresp(WARN, HEX, "set B");
}

void RIG_IC7200::set_split(bool b)
{
	cmd = pre_to;
	cmd += '\x0F';
	cmd += b ? '\x01' : '\x00';
	cmd.append( post );
	sendICcommand(cmd, 6);
	showresp(WARN, HEX, "set split");
}

// Tranceiver PTT on/off
void RIG_IC7200::set_PTT_control(int val)
{
	cmd = pre_to;
	cmd += '\x1c';
	cmd += '\x00';
	cmd += (unsigned char) val;
	cmd.append( post );
	sendICcommand (cmd, 6);
	showresp(WARN, HEX, "set ptt");
}


// Volume control val 0 ... 100
void RIG_IC7200::set_volume_control(int val)
{
	int ICvol = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append(to_bcd(ICvol, 3));
	cmd.append( post );
	sendICcommand (cmd, 6);
	showresp(WARN, HEX, "set vol");
}

int RIG_IC7200::get_volume_control()
{
	string cstr = "\x14\x01";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	sendCommand(cmd);//sendICcommand(cmd, 9);
	showresp(WARN, HEX, "get vol");
	size_t p = replystr.rfind(resp);
	if (p != string::npos && ((p + 9) < replystr.length()))
		return ((int)(fm_bcd(&replystr[p + 2],3) * 100 / 255));
	return progStatus.volume;
}

void RIG_IC7200::get_vol_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

int RIG_IC7200::get_smeter()
{
	string cstr = "\x15\x02";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	sendCommand(cmd);//sendICcommand(cmd, 9);
	showresp(WARN, HEX, "get smeter");
	int mtr= -1;
	size_t p = replystr.rfind(resp);
	if (p != string::npos && ((p+9) < replystr.length())) {
		mtr = fm_bcd(&replystr[p+6], 3);
		mtr = (int)(mtr /2.55);
		if (mtr > 100) mtr = 100;
	}
	return mtr;
}

int RIG_IC7200::get_power_out(void) 
{
	string cstr = "\x15\x11";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	sendCommand(cmd);
	showresp(WARN, HEX, "get power out");
	int mtr= -1;
	size_t p = replystr.rfind(resp);
	if (p != string::npos && ((p+9) < replystr.length())) {
		mtr = fm_bcd(&replystr[p+6], 3);
		mtr = (int)(mtr /2.55);
		if (mtr > 100) mtr = 100;
	}
	return mtr;
}

int RIG_IC7200::get_swr(void) 
{
	string cstr = "\x15\x12";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	sendCommand(cmd);
	showresp(WARN, HEX, "get swr");
	int mtr= -1;
	size_t p = replystr.rfind(resp);
	if (p != string::npos && ((p+9) < replystr.length())) {
		mtr = fm_bcd(&replystr[p+6], 3);
		mtr = (int)(mtr /2.55);
		if (mtr > 100) mtr = 100;
	}
	return mtr;
}

int RIG_IC7200::get_alc(void)
{
	string cstr = "\x15\x13";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	sendCommand(cmd);
	showresp(WARN, HEX, "get alc");
	size_t p = replystr.rfind(resp);
	int mtr= -1;
	if (p != string::npos && ((p+9) < replystr.length())) {
		mtr = fm_bcd(&replystr[p+6], 3);
		mtr = (int)(mtr /2.55);
		if (mtr > 100) mtr = 100;
	}
	return mtr;
}

void RIG_IC7200::set_attenuator(int val)
{
	cmd = pre_to;
	cmd += '\x11';
	cmd += val ? '\x20' : '\x00';
	cmd.append( post );
	sendICcommand (cmd, 6);
	showresp(WARN, HEX, "set att");
}

void RIG_IC7200::set_noise(bool val)
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd += val ? 1 : 0;
	cmd.append(post);
	sendICcommand(cmd, 6);
	showresp(WARN, HEX, "set noise");
}

int RIG_IC7200::get_noise()
{
	string cstr = "\x16\x22";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	sendCommand(cmd);
	showresp(WARN, HEX, "get noise");
	size_t p = replystr.rfind(resp);
	if (p != string::npos && ((p + 6) < replystr.length()))
		return (replystr[p+6] ? 1 : 0);
	return progStatus.noise;
}

void RIG_IC7200::set_noise_reduction(int val)
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd += val ? 1 : 0;
	cmd.append(post);
	sendICcommand(cmd, 6);
	showresp(WARN, HEX, "set NR");
}

int RIG_IC7200::get_noise_reduction()
{
	string cstr = "\x16\x40";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	sendCommand(cmd);
	showresp(WARN, HEX, "get NR");
	size_t p = replystr.rfind(resp);
	if (p != string::npos && ((p+6) < replystr.length()))
		return (replystr[p+6] ? 1 : 0);
	return progStatus.noise_reduction;
}

// 0 < val < 100
void RIG_IC7200::set_noise_reduction_val(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x06");
	cmd.append(to_bcd(val * 255 / 100, 3));
	cmd.append(post);
	sendICcommand(cmd,6);
	showresp(WARN, HEX, "set NRval");
}

int RIG_IC7200::get_noise_reduction_val()
{
	string cstr = "\x14\x06";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	sendCommand(cmd);
	showresp(WARN, HEX, "get NRval");
	size_t p = replystr.rfind(resp);
	if (p != string::npos && ((p+9) < replystr.length()))
		return ((int)(fm_bcd(&replystr[p+6],3) * 100 / 255));
	return progStatus.noise_reduction_val;
}

void RIG_IC7200::set_preamp(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd += (unsigned char)val;
	cmd.append( post );
	sendICcommand (cmd, 6);
	showresp(WARN, HEX, "set Pre");
}

int RIG_IC7200::get_preamp()
{
	string cstr = "\x16\x02";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	sendCommand(cmd);
	showresp(WARN, HEX, "get Pre");
	size_t p = replystr.rfind(resp);
	if (p != string::npos && ((p+6) < replystr.length())) {
		if (replystr[p+6] == 0x01) {
			preamp_label("Pre", true);
			return true;
		} else {
			preamp_label("Pre", false);
			return false;
		}
	}
	return progStatus.preamp;
}

void RIG_IC7200::set_rf_gain(int val)
{
	int ICrfg = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x02");
	cmd.append(to_bcd(ICrfg, 3));
	cmd.append( post );
	sendICcommand (cmd, 6);
	showresp(WARN, HEX, "set RF");
}

int RIG_IC7200::get_rf_gain()
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

void RIG_IC7200::set_squelch(int val)
{
	int ICsql = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x03");
	cmd.append(to_bcd(ICsql, 3));
	cmd.append( post );
	sendICcommand (cmd, 6);
	showresp(WARN, HEX, "set Sqlch");
}

int  RIG_IC7200::get_squelch()
{
	string cstr = "\x14\x03";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	sendCommand(cmd);
	showresp(WARN, HEX, "get squelch");
	size_t p = replystr.rfind(resp);
	if (p != string::npos && ((p + 9) < replystr.length()))
		return ((int)(fm_bcd(&replystr[p+6], 3) * 100 / 255));
	return progStatus.squelch;
}

void RIG_IC7200::set_power_control(double val)
{
	cmd = pre_to;
	cmd.append("\x14\x0A");
	cmd.append(to_bcd((int)(val * 255 / 100), 3));
	cmd.append( post );
	sendICcommand (cmd, 6);
	showresp(WARN, HEX, "set power");
}

int RIG_IC7200::get_power_control()
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

void RIG_IC7200::set_mic_gain(int val)
{
	val = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x0B");
	cmd.append(to_bcd(val,3));
	cmd.append(post);
	sendICcommand(cmd, 6);
	showresp(WARN, HEX, "set mic");
}

int RIG_IC7200::get_modeA()
{
	int md = A.imode;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	int ret = sendICcommand(cmd, 8);
	showresp(WARN, HEX, "get mode A");
	if (ret == 8) {
		md = replystr[5];
		if (md > 6) md -= 2;
		A.iBW = replystr[6];
	}
	cmd = pre_to;
	cmd += '\x1A'; cmd += '\04';
	cmd.append(post);
	ret = sendICcommand(cmd, 9);
	showresp(WARN, HEX, "get data A");
	if (ret == 9) {
		if ((replystr[6] & 0x01) == 0x01) {
			if (md == 0) md = 7;
			if (md == 1) md = 8;
		}
	}
	return (A.imode = md);
}

void RIG_IC7200::set_modeA(int val)
{
	A.imode = val;
	if (val > 6) val -= 7;
	cmd = pre_to;
	cmd += '\x06';
	cmd += val > 4 ? val + 2 : val;
	cmd.append( post );
	sendICcommand (cmd, 6);
	showresp(WARN, HEX, "set mode A");

	cmd = pre_to;
	cmd += '\x1A'; cmd += '\x04';
	if (A.imode > 6) cmd += '\x01';
	else cmd += '\x00';
	cmd.append( post);
	sendICcommand (cmd, 6);
	showresp(WARN, HEX, "set data A");
}

int RIG_IC7200::get_modeB()
{
	int md = B.imode;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	int ret = sendICcommand(cmd, 8);
	showresp(WARN, HEX, "get mode B");
	if (ret == 8) {
		md = replystr[5];
		if (md > 6) md -= 2;
		B.iBW = replystr[6];
	}
	cmd = pre_to;
	cmd += '\x1A'; cmd += '\04';
	cmd.append(post);
	ret = sendICcommand(cmd, 9);
	showresp(WARN, HEX, "get data B");
	if (ret == 9) {
		if ((replystr[6] & 0x01) == 0x01) {
			if (md == 0) md = 7;
			if (md == 1) md = 8;
		}
	}
	return (B.imode = md);
}

void RIG_IC7200::set_modeB(int val)
{
	B.imode = val;
	if (val > 6) val -= 7;
	cmd = pre_to;
	cmd += '\x06';
	cmd += val > 4 ? val + 2 : val;
	cmd.append( post );
	sendICcommand (cmd, 6);
	showresp(WARN, HEX, "set mode B");

	cmd = pre_to;
	cmd += '\x1A'; cmd += '\x04';
	if (B.imode > 6) cmd += '\x01';
	else cmd += '\x00';
	cmd.append( post);
	sendICcommand (cmd, 6);
	showresp(WARN, HEX, "set data B");
}

int RIG_IC7200::get_modetype(int n)
{
	return _mode_type[n];
}

int RIG_IC7200::adjust_bandwidth(int m)
{
	switch (m) {
		case 2: // AM
			bandwidths_ = IC7200_AMwidths;
			bwA = 30;
			break;
		case 3:
		case 5: // CW
			bandwidths_ = IC7200_SSBwidths;
			bwA = 14;
			break;
		case 4:
		case 6: // RTTY
			bandwidths_ = IC7200_RTTYwidths;
			bwA = 28;
			break;
		case 0:
		case 1:
		case 7:
		case 8: 
		default: // SSB
			bandwidths_ = IC7200_SSBwidths;
			bwA = 32;
	}
	return bwA;
}

int RIG_IC7200::def_bandwidth(int m)
{
	return adjust_bandwidth(m);
}

const char ** RIG_IC7200::bwtable(int m)
{
	switch (m) {
		case 2: // AM
			return IC7200_AMwidths;
			break;
		case 3:
		case 5: // CW
			return IC7200_SSBwidths;
			break;
		case 4:
		case 6: // RTTY
			return IC7200_RTTYwidths;
			break;
		case 0:
		case 1:
		case 7:
		case 8: 
		default: // SSB
			return IC7200_SSBwidths;
	}
	return IC7200_SSBwidths;
}

void RIG_IC7200::set_bwA(int val)
{
	A.iBW = val;
	cmd = pre_to;
	cmd.append("\x1A\x02");
	cmd.append(to_bcd(val, 2));
	cmd.append( post );
	sendICcommand(cmd, 6);
	showresp(WARN, HEX, "set BW A");
}

int  RIG_IC7200::get_bwA()
{
	cmd = pre_to;
	cmd += "\x1A\x02";
	cmd.append( post );
	int ret = sendICcommand(cmd, 8);
	showresp(WARN, HEX, "get BW A");
	if (ret == 8)
		A.iBW = (fm_bcd(&replystr[6],2));
	return A.iBW;
}

void RIG_IC7200::set_bwB(int val)
{
	B.iBW = val;
	cmd = pre_to;
	cmd.append("\x1A\x02");
	cmd.append(to_bcd(val, 2));
	cmd.append( post );
	sendICcommand(cmd, 6);
	showresp(WARN, HEX, "set BW B");
}

int  RIG_IC7200::get_bwB()
{
	cmd = pre_to;
	cmd += "\x1A\x02";
	cmd.append( post );
	int ret = sendICcommand(cmd, 8);
	showresp(WARN, HEX, "get BW B");
	if (ret == 8)
		B.iBW = (fm_bcd(&replystr[6],2));
	return B.iBW;
}

void RIG_IC7200::set_auto_notch(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x41';
	cmd += (unsigned char)val;
	cmd.append( post );
	sendICcommand (cmd, 6);
	showresp(WARN, HEX, "set AN");
}

int RIG_IC7200::get_auto_notch()
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

void RIG_IC7200::set_compression()
{
	if (progStatus.compON) {
		cmd = pre_to;
		cmd.append("\x16\x44");
		cmd += '\x01';
		cmd.append(post);
		sendICcommand(cmd, 6);
		showresp(WARN, HEX, "set Comp ON");
	} else {
		cmd = pre_to;
		cmd.append("\x16\x44");
		cmd += '\x00';
		cmd.append(post);
		sendICcommand(cmd, 6);
		showresp(WARN, HEX, "set Comp OFF");
	}
}

void RIG_IC7200::set_vox_onoff()
{
	if (progStatus.vox_onoff) {
		cmd = pre_to;
		cmd.append("\x16\x46");
		cmd += '\x01';
		cmd.append(post);
		sendICcommand(cmd, 6);
		showresp(WARN, HEX, "set Vox ON");
	} else {
		cmd = pre_to;
		cmd.append("\x16\x46");
		cmd += '\x00';
		cmd.append(post);
		sendICcommand(cmd, 6);
		showresp(WARN, HEX, "set Vox OFF");
	}
}

void RIG_IC7200::tune_rig()
{
	cmd = pre_to;
	cmd.append("\x1c\x01\x02");
	cmd.append( post );
	sendCommand (cmd);
	showresp(WARN, HEX, "tune rig");
}

