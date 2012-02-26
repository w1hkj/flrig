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

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol,       2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME,  54, 125, 156 },
	{ (Fl_Widget *)sldrRFGAIN,  54, 145, 156 },
	{ (Fl_Widget *)btnNR,      214, 105,  50 },
	{ (Fl_Widget *)sldrNR,     266, 105, 156 },
	{ (Fl_Widget *)sldrMICGAIN,266, 125, 156 },
	{ (Fl_Widget *)sldrPOWER,  266, 145, 156 },
	{ (Fl_Widget *)NULL, 0, 0, 0 }
};

RIG_IC718::RIG_IC718() {
	name_ = IC718name_;
	modes_ = IC718modes_;
	_mode_type = IC718_mode_type;
	bandwidths_ = IC718_widths;
	widgets = rig_widgets;

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

	has_smeter =
	has_power_control =
	has_volume_control =
	has_mode_control =
	has_bandwidth_control =
	has_micgain_control =
	has_attenuator_control =
	has_preamp_control =
	has_noise_control =
	has_noise_reduction =
	has_noise_reduction_control =
	has_auto_notch =
	has_rf_control =
	has_split =
	has_compON =
	has_extras =
	has_vox_onoff = true;

	defaultCIV = 0x5E;
	adjustCIV(defaultCIV);
};

//=============================================================================

void RIG_IC718::initialize()
{
	rig_widgets[0].W = btnVol;
	rig_widgets[1].W = sldrVOLUME;
	rig_widgets[2].W = sldrRFGAIN;
	rig_widgets[3].W = btnNR;
	rig_widgets[4].W = sldrNR;
	rig_widgets[5].W = sldrMICGAIN;
	rig_widgets[6].W = sldrPOWER;
}

int  RIG_IC718::adjust_bandwidth(int m)
{
	return 0;
}

void RIG_IC718::selectA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x00';
	cmd.append(post);
	waitFB("sel A");
}

void RIG_IC718::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x01';
	cmd.append(post);
	waitFB("sel B");
}

long RIG_IC718::get_vfoA ()
{
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	string resp = pre_fm;
	resp += '\x03';
	if (waitFOR(11, "get vfo A")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			freqA = fm_bcd_be(&replystr[p+5], 10);
	}
	return freqA;
}

void RIG_IC718::set_vfoA (long freq)
{
	freqA = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	waitFB("set vfo A");
}

long RIG_IC718::get_vfoB ()
{
	string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	if (waitFOR(11, "get vfo B")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			B.freq = fm_bcd_be(&replystr[p+5], 10);
	}
	return B.freq;
}

void RIG_IC718::set_vfoB (long freq)
{
	B.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	waitFB("set vfo B");
}


// Volume control val 0 ... 100
void RIG_IC718::set_volume_control(int val)
{
	int ICvol = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append(to_bcd(ICvol, 3));
	cmd.append( post );
	waitFB("set vol");
}

int RIG_IC718::get_volume_control()
{
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append( post );
	string resp = pre_fm;
	resp.append("\x14\x01");
	if (waitFOR(9, "get vol")) {
		size_t p = replystr.rfind(resp);
		if ( p != string::npos)
			return (int)ceil(fm_bcd(&replystr[p+6],3) * 100 / 255);
	}
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
	string resp = pre_fm;
	resp.append("\x15\x02");
	int mtr = 0;
	if (waitFOR(9, "get smeter")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			mtr = fm_bcd(&replystr[p+6], 3);
			mtr = (int)ceil(1.3 * mtr - 36.0);
			if (mtr < 0) mtr = 0;
			if (mtr > 255) mtr = 255;
			return (mtr * 100 / 255);
		}
	}
	return -1;
}

void RIG_IC718::set_attenuator(int val)
{
	cmd = pre_to;
	cmd += '\x11';
	cmd += val ? '\x20' : '\x00';
	cmd.append( post );
	waitFB("set att");
}

void RIG_IC718::set_noise(bool val)
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set noise");
}

int RIG_IC718::get_noise()
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd.append(post);
	string resp = pre_fm;
	resp.append("\x16\x22");
	if (waitFOR(8, "get noise")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (replystr[p+6] ? 1 : 0);
	}
	return 0;
}

void RIG_IC718::set_noise_reduction(int val)
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set nr");
}

int RIG_IC718::get_noise_reduction()
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd.append(post);
	string resp = pre_fm;
	resp.append("\x16\x40");
	if (waitFOR(8, "get nr")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (replystr[p+6] ? 1 : 0);
	}
	return 0;
}

// 0 < val < 100
void RIG_IC718::set_noise_reduction_val(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x06");
	cmd.append(to_bcd(val * 255 / 100, 3));
	cmd.append(post);
	waitFB("set nr val");
}

int RIG_IC718::get_noise_reduction_val()
{
	cmd = pre_to;
	cmd.append("\x14\x06");
	cmd.append(post);
	string resp = pre_fm;
	resp.append("\x14\x06");
	if (waitFOR(9, "get nr val")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (int)ceil(fm_bcd(&replystr[p+6],3) * 100 / 255);
	}
	return 0;
}

void RIG_IC718::set_preamp(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd += (unsigned char)val;
	cmd.append( post );
	waitFB("set pre");
}

int RIG_IC718::get_preamp()
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd.append( post );
	string resp = pre_fm;
	resp += '\x16';
	resp += '\x02';
	if (waitFOR(8, "get pre")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			if (replystr[p+6] == 0x01) {
				preamp_label("Pre", true);
				return 1;
			} else {
				preamp_label("Pre", false);
				return 0;
			}
		}
	}
	return 0;
}

void RIG_IC718::set_rf_gain(int val)
{
	int ICrfg = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x02");
	cmd.append(to_bcd(ICrfg, 3));
	cmd.append( post );
	waitFB("set rf gain");
}

int RIG_IC718::get_rf_gain()
{
	cmd = pre_to;
	cmd.append("\x14\x02").append(post);
	string resp = pre_fm;
	cmd.append("\x14\x02");
	if (waitFOR(9, "get rfg")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (int)ceil(fm_bcd(&replystr[p+6],3) * 100 / 255);
	}
	return 0;
}

void RIG_IC718::set_squelch(int val)
{
	int ICsql = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x03");
	cmd.append(to_bcd(ICsql, 3));
	cmd.append( post );
	waitFB("set sql");
}

void RIG_IC718::set_power_control(double val)
{
	cmd = pre_to;
	cmd.append("\x14\x0A");
	cmd.append(to_bcd((int)(val * 255 / 100), 3));
	cmd.append( post );
	waitFB("set power");
}

int RIG_IC718::get_power_control()
{
	string cstr = "\x14\x0A";
	string resp = pre_fm;
	cmd = pre_to;
	cmd.append(cstr).append(post);
	resp.append(cstr);
	if (waitFOR(9, "get power")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (int)ceil(fm_bcd(&replystr[p + 6],3) * 100 / 255);
	}
	return progStatus.power_level;
}

void RIG_IC718::set_mic_gain(int val)
{
	val = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x0B");
	cmd.append(to_bcd(val,3));
	cmd.append(post);
	waitFB("set mic");
}

int RIG_IC718::get_mic_gain()
{
	string cstr = "\x14\x0B";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get mic")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (int)ceil(fm_bcd(&replystr[p+6],3) / 2.55);
	}
	return 0;
}

void RIG_IC718::get_mic_gain_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_IC718::set_modeA(int val)
{
	modeA = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += val > 4 ? val + 2 : val;
	cmd += filter_nbr;
	cmd.append( post );
	waitFB("set mode A");
}

int RIG_IC718::get_modeA()
{
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	string resp = pre_fm;
	resp += '\x04';
	if (waitFOR(8, "get modeA")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			modeA = replystr[p+5];
			if (modeA > 6) modeA -= 2;
			filter_nbr = replystr[p+6];
		}
	}
	return modeA;
}

void RIG_IC718::set_modeB(int val)
{
	modeB = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += val > 4 ? val + 2 : val;
	cmd += filter_nbr;
	cmd.append( post );
	waitFB("set mode A");
}

int RIG_IC718::get_modeB()
{
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	string resp = pre_fm;
	resp += '\x04';
	if (waitFOR(8, "get modeB")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			modeB = replystr[p+5];
			if (modeB > 6) modeB -= 2;
			filter_nbr = replystr[p+6];
		}
	}
	return modeB;
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

void RIG_IC718::set_bwB(int val)
{
	filter_nbr = val + 1;
	set_modeB(modeB);
}

int RIG_IC718::get_bwB()
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
	waitFB("set AN");
}

int RIG_IC718::get_auto_notch()
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x41';
	cmd.append( post );
	string resp = pre_fm;
	resp += '\x16';
	resp += '\x41';
	if (waitFOR(8, "get AN")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			if (replystr[6] == 0x01) {
				auto_notch_label("AN", true);
				return 1;
			} else {
				auto_notch_label("AN", false);
				return 0;
			}
		}
	}
	return 0;
}

void RIG_IC718::set_compression()
{
	if (progStatus.compON) {
		cmd = pre_to;
		cmd.append("\x16\x44");
		cmd += '\x01';
		cmd.append(post);
		waitFB("set compON");
	} else {
		cmd = pre_to;
		cmd.append("\x16\x44");
		cmd += '\x00';
		cmd.append(post);
		waitFB("set compOFF");
	}
}

void RIG_IC718::set_vox_onoff()
{
	if (progStatus.vox_onoff) {
		cmd = pre_to;
		cmd.append("\x16\x46");
		cmd += '\x01';
		cmd.append(post);
		waitFB("set voxON");
	} else {
		cmd = pre_to;
		cmd.append("\x16\x46");
		cmd += '\x00';
		cmd.append(post);
		waitFB("set voxOFF");
	}
}

void RIG_IC718::set_split(bool val)
{
	cmd = pre_to;
	cmd += 0x0F;
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	waitFB("set split");
}

int RIG_IC718::get_split()
{
	cmd = pre_to;
	cmd += 0x0F;
	cmd.append(post);
	string resp = pre_fm;
	resp += 0x0F;
	if (waitFOR(7, "get split")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			if (replystr[5] == 0x01) {
				return 1;
			} else {
				return 0;
			}
		}
	}
	return 0;
}

// N6WBL
