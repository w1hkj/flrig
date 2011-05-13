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
// 756PRO

const char IC756PROname_[] = "IC-756PRO";

//=============================================================================
const char *IC756PROmodes_[] = {
		"LSB", "USB", "AM", "CW", "RTTY", "FM", "CW-R", "RTTY-R", NULL};

const char IC756PRO_mode_type[] =
	{ 'L', 'U', 'U', 'U', 'L', 'U', 'L', 'U' };

const char *IC756PRO_widths[] = { "FIXED", NULL};


RIG_IC756PRO::RIG_IC756PRO() {
	defaultCIV = 0x5C;
	name_ = IC756PROname_;
	modes_ = IC756PROmodes_;
	bandwidths_ = IC756PRO_widths;
	_mode_type = IC756PRO_mode_type;

	def_freq = freqA = freqB = A.freq = 14070000;
	def_mode = modeA = modeB = B.imode = 1;
	def_bw = bwA = bwB = A.iBW = B.iBW = 0;
	atten_level = 0;
	preamp_level = 0;
	adjustCIV(defaultCIV);

//	has_bandwidth_control =
//	has_ifshift_control =
//	has_tune_control =
//	has_swr_control =
//	has_noise_reduction =
//	has_noise_reduction_control =
//	has_alc_control = 
	has_smeter =
	has_power_control =
	has_volume_control =
	has_mode_control =
	has_micgain_control =
	has_auto_notch =
	has_attenuator_control =
	has_preamp_control =
	has_ptt_control =
	has_noise_control =
	has_rf_control = true;
};

void RIG_IC756PRO::selectA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\xD0';
	cmd.append(post);
	waitFB("sel A");
}

void RIG_IC756PRO::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\xD1';
	cmd.append(post);
	waitFB("sel B");
}

long RIG_IC756PRO::get_vfoA ()
{
	string cstr = "\x03";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(11, "get vfo A")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			A.freq = fm_bcd_be(&replystr[p+5], 10);
	}
	return A.freq;
}

void RIG_IC756PRO::set_vfoA (long freq)
{
	A.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	waitFB("set vfo A");
}

long RIG_IC756PRO::get_vfoB ()
{
	string cstr = "\x03";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(11, "get vfo B")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			B.freq = fm_bcd_be(&replystr[p+5], 10);
	}
	return B.freq;
}

void RIG_IC756PRO::set_vfoB (long freq)
{
	B.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	waitFB("set vfo B");
}

int RIG_IC756PRO::get_smeter()
{
	string cstr = "\x15\x02";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get smeter")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return fm_bcd(&replystr[p+6], 3) / 2.55;
	}
	return 0;
}

// Volume control val 0 ... 100

void RIG_IC756PRO::set_volume_control(int val)
{
	ICvol = (int)(val);
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append(to_bcd(ICvol, 3));
	cmd.append( post );
	waitFB("set vol");
}

int RIG_IC756PRO::get_volume_control()
{
	string cstr = "\x14\x01";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get vol")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return ((int)(fm_bcd(&replystr[p+6],3)));
	}
	return 0;
}

void RIG_IC756PRO::get_vol_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 255; step = 1;
}

// Tranceiver PTT on/off
void RIG_IC756PRO::set_PTT_control(int val)
{
	cmd = pre_to;
	cmd += '\x1c';
	cmd += '\x00';
	cmd += (unsigned char) val;
	cmd.append( post );
	waitFB("set PTT");
}

// changed noise blanker to noise reduction
void RIG_IC756PRO::set_noise(bool val)
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set noise");
}

int RIG_IC756PRO::get_noise()
{
	string cstr = "\x16\x22";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get noise")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (replystr[p+6] ? 1 : 0);
	}
	return 0;
}

int RIG_IC756PRO::get_modetype(int n)
{
	return _mode_type[n];
}

void RIG_IC756PRO::set_mic_gain(int val)
{
	val = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x0B");
	cmd.append(to_bcd(val,3));
	cmd.append(post);
	waitFB("set mic");
}

void RIG_IC756PRO::get_mic_gain_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_IC756PRO::set_if_shift(int val)
{
	int shift = (int)((val + 50) * 2.56 );
	if (shift == 256) shift = 255;
	cmd = pre_to;
	cmd.append("\x14\x07");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set IF on/off");

	cmd = pre_to;
	cmd.append("\x14\x08");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set IF val");
}

void RIG_IC756PRO::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -50;
	max = +50;
	step = 2;
}

int IC756PROsql = 0;
void RIG_IC756PRO::set_squelch(int val)
{
	IC756PROsql = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x03");
	cmd.append(to_bcd(IC756PROsql, 3));
	cmd.append( post );
	waitFB("set sql");
}

int IC756PROrfg = 0;
void RIG_IC756PRO::set_rf_gain(int val)
{
	IC756PROrfg = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x02");
	cmd.append(to_bcd(IC756PROrfg, 3));
	cmd.append( post );
	waitFB("set rf gain");
}

void RIG_IC756PRO::set_power_control(double val)
{
	cmd = pre_to;
	cmd.append("\x14\x0A");
	cmd.append(to_bcd((int)(val * 255 / 100), 3));
	cmd.append( post );
	waitFB("set power");
}

void RIG_IC756PRO::set_split(bool val)
{
	cmd = pre_to;
	cmd += 0x0F;
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	waitFB("set split");
}

//======================================================================
// IC756PRO unique commands
//======================================================================

void RIG_IC756PRO::set_modeA(int val)
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
	waitFB("set mode A");
}

int RIG_IC756PRO::get_modeA()
{
	int md;
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
			A.imode = md;
		}
	}
	return A.imode;
}

void RIG_IC756PRO::set_modeB(int val)
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
	waitFB("set mode B");
}

int RIG_IC756PRO::get_modeB()
{
	int md;
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
			B.imode = md;
		}
	}
	return B.imode;
}

int RIG_IC756PRO::adjust_bandwidth(int m)
{
	return 0;
}

int RIG_IC756PRO::def_bandwidth(int m)
{
	return (0);
}

const char **RIG_IC756PRO::bwtable(int m)
{
	return IC756PRO_widths;
}

void RIG_IC756PRO::set_auto_notch(int v)
{
	progStatus.auto_notch = v;
	cmd = pre_to;
	cmd.append("\x16\x48");
	cmd += v ? '\x01' : '\x00';
	cmd.append(post);
	waitFB("set auto notch");
}

int  RIG_IC756PRO::get_auto_notch()
{
	return progStatus.auto_notch;
}

void RIG_IC756PRO::set_attenuator(int val)
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

int RIG_IC756PRO::get_attenuator()
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

void RIG_IC756PRO::set_preamp(int val)
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

int RIG_IC756PRO::get_preamp()
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


