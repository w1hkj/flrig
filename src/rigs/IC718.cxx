/*
 * Icom IC-718
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */

#include "IC718.h"
#include "debug.h"
#include "rig.h"

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
	mode_ = 1;
	bw_ = 0;
	filter_nbr = 1;

//	has_swr_control = true;
	has_power_control = true;
	has_volume_control = true;
	has_mode_control = true;
	has_bandwidth_control = true;
	has_micgain_control = true;
//	has_notch_control = true;
	has_attenuator_control = true;
	has_preamp_control = true;
//	has_ifshift_control = true;
//	has_ptt_control = true;
//	has_tune_control = true;
//	has_swr_control = true;
	has_noise_control = true;
	has_noise_reduction = true;
	has_noise_reduction_control = true;
//	has_alc_control = true;
	has_rf_control = true;
//	has_sql_control = true;

	defaultCIV = 0x5E;
	adjustCIV(defaultCIV);
};

//=============================================================================

long RIG_IC718::get_vfoA ()
{
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	if (!sendCommand(cmd, 11)) {
		checkresponse(11);
		return freq_;
	}
	freq_ = fm_bcd_be(&replystr[5], 10);
	return freq_;
}

void RIG_IC718::set_vfoA (long freq)
{
	freq_ = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	sendICcommand(cmd, 6);
	checkresponse(6);
	if (DEBUG_718)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
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
	checkresponse(6);
	if (DEBUG_718)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

int RIG_IC718::get_volume_control()
{
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append( post );
	if(sendICcommand (cmd, 9))
		return ((int)(fm_bcd(&replystr[6],3) * 100 / 255));
	checkresponse(9);
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
	if (sendICcommand (cmd, 9)) {
		int mtr = fm_bcd(&replystr[6], 3);
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
	checkresponse(6);
	if (DEBUG_718)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

void RIG_IC718::set_noise(bool val)
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd += val ? 1 : 0;
	cmd.append(post);
	sendICcommand(cmd, 6);
	checkresponse(6);
	if (DEBUG_718)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

int RIG_IC718::get_noise()
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd.append(post);
	if (sendICcommand(cmd, 8))
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
	checkresponse(6);
	if (DEBUG_718)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

int RIG_IC718::get_noise_reduction()
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd.append(post);
	if (sendICcommand(cmd, 8))
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
	checkresponse(6);
	if (DEBUG_718)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

int RIG_IC718::get_noise_reduction_val()
{
	cmd = pre_to;
	cmd.append("\x14\x06");
	cmd.append(post);
	if(sendICcommand (cmd, 9))
		return ((int)(fm_bcd(&replystr[6],3) * 100 / 255));
	checkresponse(9);
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
	checkresponse(6);
	if (DEBUG_718)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

int RIG_IC718::get_preamp()
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd.append( post );
	if (sendICcommand (cmd, 8)) {
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
	checkresponse(6);
	if (DEBUG_718)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

int RIG_IC718::get_rf_gain()
{
	cmd = pre_to;
	cmd.append("\x14\x02").append(post);
	if (sendICcommand (cmd, 9))
		return ((int)(fm_bcd(&replystr[6],3) * 100 / 255));
	checkresponse(9);
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
	checkresponse(6);
	if (DEBUG_718)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

void RIG_IC718::set_power_control(double val)
{
	cmd = pre_to;
	cmd.append("\x14\x0A");
	cmd.append(to_bcd((int)(val * 255 / 100), 3));
	cmd.append( post );
	sendICcommand (cmd, 6);
	checkresponse(6);
	if (DEBUG_718)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

void RIG_IC718::set_mic_gain(int val)
{
	val = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x0B");
	cmd.append(to_bcd(val,3));
	cmd.append(post);
	sendICcommand(cmd, 6);
	checkresponse(6);
	if (DEBUG_718)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

void RIG_IC718::set_mode(int val)
{
	mode_ = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += val > 5 ? val + 1 : val;
	cmd += filter_nbr;
	cmd.append( post );
	sendICcommand (cmd, 6);
	checkresponse(6);
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

int RIG_IC718::get_mode()
{
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	if (sendICcommand (cmd, 8 )) {
		mode_ = replystr[5];
		if (mode_ > 6) mode_--;
		filter_nbr = replystr[6];
	}
	return mode_;
}

int RIG_IC718::get_modetype(int n)
{
	return _mode_type[n];
}

void RIG_IC718::set_bandwidth(int val)
{
	filter_nbr = val + 1;
	set_mode(mode_);
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

int RIG_IC718::get_bandwidth()
{
	return filter_nbr - 1;
}

