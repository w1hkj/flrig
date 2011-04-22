/*
 * Icom IC-718
 * 
 * a part of flrig
 * 
 * Copyright 2011, Dave Freese, W1HKJ
 * 
 */

#include "IC718.h"

bool DEBUG_718 = true;

//=============================================================================
// IC-718
//
const char IC718name_[] = "IC-718";
const char *IC718modes_[] = { "LSB", "USB", "AM", "CW", "RTTY", "CW-R", "RTTY-R", NULL};
const char IC718_mode_type[] = { 'L', 'U', 'U', 'L', 'L', 'U', 'U'};
const char *IC718_widths[] = { "Wide", "Med", "Narr", NULL};

RIG_IC718::RIG_IC718() {
	name_ = IC718name_;
	modes_ = IC718modes_;
	_mode_type = IC718_mode_type;
	bandwidths_ = IC718_widths;

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
	modeA = 1;
	bwA = 0;
	filter_nbr = 1;

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

	defaultCIV = 0x5E;
	adjustCIV(defaultCIV);
};

//=============================================================================
int  RIG_IC718::adjust_bandwidth(int m)
{
	return 0;
}

long RIG_IC718::get_vfoA ()
{
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	int ret = sendCommand(cmd, 11);
	showresp(WARN, HEX, "get vfoA");
	if (ret == 11)
		freqA = fm_bcd_be(&replystr[5], 10);
	return freqA;
}

void RIG_IC718::set_vfoA (long freq)
{
	freqA = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	sendICcommand(cmd, 6);
	showresp(WARN, HEX, "set vfoA");
}

// Volume control val 0 ... 100
void RIG_IC718::set_volume_control(int val)
{
	int ICvol = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append(to_bcd(ICvol, 3));
	cmd.append( post );
	sendICcommand (cmd, 6);
	showresp(WARN, HEX, "set vol");
}

int RIG_IC718::get_volume_control()
{
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append( post );
	int ret = sendICcommand (cmd, 9);
	showresp(WARN, HEX, "get vol");
	if (ret == 9)
		return ((int)(fm_bcd(&replystr[6],3) * 100 / 255));
	return 0;
}

void RIG_IC718::get_vol_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

int RIG_IC718::get_smeter()
{
	cmd = pre_to;
	cmd.append("\x15\x02");
	cmd.append( post );
	int ret = sendICcommand (cmd, 9);
	int mtr = 0;
	showresp(WARN, HEX, "get smeter");
	if (ret == 9) {
		mtr = fm_bcd(&replystr[6], 3);
		mtr = (int)(1.3 * mtr - 36.0);
		if (mtr < 0) mtr = 0;
		if (mtr > 255) mtr = 255;
		return (mtr * 100 / 255);
	} else
		return 0;
}

void RIG_IC718::set_attenuator(int val)
{
	cmd = pre_to;
	cmd += '\x11';
	cmd += val ? '\x20' : '\x00';
	cmd.append( post );
	sendCommand (cmd, 6);
	if (val)
		showresp(WARN, HEX, "set att ON");
	else
		showresp(WARN, HEX, "set att OFF");
}

void RIG_IC718::set_noise(bool val)
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd += val ? 1 : 0;
	cmd.append(post);
	sendICcommand(cmd, 6);
	showresp(WARN, HEX, "set noise");
}

int RIG_IC718::get_noise()
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd.append(post);
	int ret = sendICcommand(cmd, 8);
	showresp(WARN, HEX, "get noise");
	if (ret == 8)
		return (replystr[6] ? 1 : 0);
	return 0;
}

void RIG_IC718::set_noise_reduction(int val)
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd += val ? 1 : 0;
	cmd.append(post);
	sendICcommand(cmd, 6);
	showresp(WARN, HEX, "set nr");
}

int RIG_IC718::get_noise_reduction()
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd.append(post);
	int ret = sendICcommand(cmd, 8);
	showresp(WARN, HEX, "get nr");
	if (ret == 8)
		return (replystr[6] ? 1 : 0);
	return 0;
}

// 0 < val < 100
void RIG_IC718::set_noise_reduction_val(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x06");
	cmd.append(to_bcd(val * 255 / 100, 3));
	cmd.append(post);
	sendICcommand(cmd,6);
	showresp(WARN, HEX, "set nr val");
}

int RIG_IC718::get_noise_reduction_val()
{
	cmd = pre_to;
	cmd.append("\x14\x06");
	cmd.append(post);
	int ret = sendICcommand (cmd, 9);
	showresp(WARN, HEX, "get nr val");
	if (ret == 9)
		return ((int)(fm_bcd(&replystr[6],3) * 100 / 255));
	return 0;
}

void RIG_IC718::set_preamp(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd += (unsigned char)val;
	cmd.append( post );
	sendICcommand (cmd, 6);
	showresp(WARN, HEX, "set pre");
}

int RIG_IC718::get_preamp()
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd.append( post );
	int ret = sendICcommand (cmd, 8);
	showresp(WARN, HEX, "get pre");
	if (ret == 8) {
		if (replystr[6] == 0x01) {
			preamp_label("Pre", true);
		} else {
			preamp_label("Pre", false);
		}
	}
	return replystr[6];
}

void RIG_IC718::set_rf_gain(int val)
{
	int ICrfg = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x02");
	cmd.append(to_bcd(ICrfg, 3));
	cmd.append( post );
	sendICcommand (cmd, 6);
	showresp(WARN, HEX, "set rfg");
}

int RIG_IC718::get_rf_gain()
{
	cmd = pre_to;
	cmd.append("\x14\x02").append(post);
	int ret = sendICcommand (cmd, 9);
	showresp(WARN, HEX, "get rfg");
	if (ret == 9)
		return ((int)(fm_bcd(&replystr[6],3) * 100 / 255));
	return 0;
}

void RIG_IC718::set_squelch(int val)
{
	int ICsql = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x03");
	cmd.append(to_bcd(ICsql, 3));
	cmd.append( post );
	sendICcommand (cmd, 6);
	showresp(WARN, HEX, "set sql");
}

void RIG_IC718::set_power_control(double val)
{
	cmd = pre_to;
	cmd.append("\x14\x0A");
	cmd.append(to_bcd((int)(val * 255 / 100), 3));
	cmd.append( post );
	sendICcommand (cmd, 6);
	showresp(WARN, HEX, "set power");
}

void RIG_IC718::set_mic_gain(int val)
{
	val = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x0B");
	cmd.append(to_bcd(val,3));
	cmd.append(post);
	sendICcommand(cmd, 6);
	showresp(WARN, HEX, "set mic");
}

void RIG_IC718::set_modeA(int val)
{
	modeA = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += val > 5 ? val + 2 : val;
	cmd += filter_nbr;
	cmd.append( post );
	sendICcommand (cmd, 6);
	showresp(WARN, HEX, "set modeA");
}

int RIG_IC718::get_modeA()
{
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	int ret = sendICcommand (cmd, 8 );
	showresp(WARN, HEX, "get modeA");
	if (ret == 8) {
		modeA = replystr[5];
		if (modeA > 6) modeA -= 2;
		filter_nbr = replystr[6];
	}
	return modeA;
}

int RIG_IC718::get_modetype(int n)
{
	return _mode_type[n];
}

void RIG_IC718::set_bwA(int val)
{
	filter_nbr = val + 1;
	set_modeA(modeA);
}

int RIG_IC718::get_bwA()
{
	return filter_nbr - 1;
}

// added by Jason Turning - N6WBL
void RIG_IC718::set_auto_notch(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x41';
	cmd += (unsigned char)val;
	cmd.append( post );
	sendICcommand (cmd, 6);
	showresp(WARN, HEX, "set AN");
}

int RIG_IC718::get_auto_notch()
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x41';
	cmd.append( post );
	int ret = sendICcommand (cmd, 8);
	showresp(WARN, HEX, "get AN");
	if (ret == 8) {
		if (replystr[6] == 0x01)
			auto_notch_label("AN", true);
		else
			auto_notch_label("AN", false);
	}
	return replystr[6];
}

void RIG_IC718::set_compression()
{
	if (progStatus.compON) {
		cmd = pre_to;
		cmd.append("\x16\x44");
		cmd += '\x01';
		cmd.append(post);
		sendICcommand(cmd, 6);
		showresp(WARN, HEX, "set compON");
	} else {
		cmd = pre_to;
		cmd.append("\x16\x44");
		cmd += '\x00';
		cmd.append(post);
		sendICcommand(cmd, 6);
		showresp(WARN, HEX, "set compOFF");
	}
}

void RIG_IC718::set_vox_onoff()
{
	if (progStatus.vox_onoff) {
		cmd = pre_to;
		cmd.append("\x16\x46");
		cmd += '\x01';
		cmd.append(post);
		sendICcommand(cmd, 6);
		showresp(WARN, HEX, "set voxON");
	} else {
		cmd = pre_to;
		cmd.append("\x16\x46");
		cmd += '\x00';
		cmd.append(post);
		sendICcommand(cmd, 6);
		showresp(WARN, HEX, "set voxOFF");
	}
}

// N6WBL
