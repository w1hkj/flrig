/*
 * Icom IC-703
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */

#include "IC703.h"

//=============================================================================
// IC-703
//
const char IC703name_[] = "IC-703";
const char *IC703modes_[] = { "LSB", "USB", "AM", "CW", "RTTY", "FM", "CW-R", "RTTY-R",
	"D-LSB", "D-USB", NULL};
const char IC703_mode_type[] = {'L', 'U', 'U', 'L', 'L', 'U', 'U', 'U', 'L', 'U' };

const char *IC703_widths[] = { "NARR", "MED", "WIDE", NULL};

RIG_IC703::RIG_IC703() {
	name_ = IC703name_;
	modes_ = IC703modes_;
 	_mode_type = IC703_mode_type;
	bandwidths_ = IC703_widths;
	comm_baudrate = BR9600;
	stopbits = 2;
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

	has_ptt_control =
	has_mode_control =
	has_bandwidth_control =
	has_compON =
	has_extras =
	has_vox_onoff =
	has_preamp_control =
	has_attenuator_control = true;

	defaultCIV = 0x68;
	adjustCIV(defaultCIV);
};

//=============================================================================

long RIG_IC703::get_vfoA ()
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

void RIG_IC703::set_vfoA (long freq)
{
	freqA = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	waitFB("set vfo A");
}

void RIG_IC703::set_modeA(int val)
{
	bool data_mode = val > 7 ? true : false;
	modeA = val;
	cmd = pre_to;
	cmd += '\x06';
	if (val > 7) val -= 8;
	else if (val > 5) val++;
	cmd += val;
	cmd += bwA + 1;
	cmd.append( post );
	waitFB("set mode A");
	if (val < 2) {
		cmd = pre_to;
		cmd.append("\x1A\x04");
		cmd += data_mode ? 0x01 : 0x00;
		cmd.append( post );
		waitFB("data mode");
	}
}

int RIG_IC703::get_modeA()
{
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	string resp = pre_fm;
	resp += '\x04';
	if (waitFOR(8, "get mode A")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			modeA = replystr[p+5];
			if (modeA > 6) modeA--;
			bwA = replystr[p+6] - 1;
			if (modeA < 2) {
				cmd = pre_to;
				cmd.append("\x1A\x04");
				cmd.append(post);
				resp = pre_fm;
				resp.append("\x1A\x04");
				if (waitFOR(8, "data mode?")) {
					p = replystr.rfind(resp);
					if (p != string::npos)
						if (replystr[p+6])
							modeA += 8;
				}
			}
		}
	}
	return modeA;
}

void RIG_IC703::set_bwA(int val)
{
	bwA = val;
	set_modeA(modeA);
}

int RIG_IC703::get_modetype(int n)
{
	return _mode_type[n];
}

int RIG_IC703::get_bwA()
{
	return bwA;
}

void RIG_IC703::set_attenuator(int val)
{
	cmd = pre_to;
	cmd += '\x11';
	cmd += val ? '\x20' : '\x00';
	cmd.append( post );
	waitFB("set att");
}

int RIG_IC703::get_attenuator()
{
	cmd = pre_to;
	cmd += '\x11';
	cmd.append( post );
	string resp = pre_fm;
	resp += '\x11';
	if (waitFOR(7, "get att")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return replystr[p+6] == '\x20' ? 1 : 0;
	}
	return 0;
}

void RIG_IC703::set_preamp(int val)
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

int RIG_IC703::get_preamp()
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd.append( post );
	string resp = pre_fm;
	resp += '\x16';
	resp += '\x02';
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

int RIG_IC703::get_smeter()
{
	cmd = pre_to;
	cmd.append("\x15\x02").append(post);
	string resp = pre_fm;
	resp.append("\x15\x02");
	if (waitFOR(9, "get smeter")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return fm_bcd(&replystr[p+6], 3) * 100 / 255;
	}
	return -1;
}

int RIG_IC703::get_power_out()
{
	cmd = pre_to;
	cmd.append("\x15\x11").append(post);
	string resp = pre_fm;
	resp.append("\x15\x11");
	if (waitFOR(9, "get power")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return fm_bcd(&replystr[p+6], 3) * 100 / 255;
	}
	return 0;
}

int RIG_IC703::get_swr()
{
	cmd = pre_to;
	cmd.append("\x15\x12").append(post);
	string resp = pre_fm;
	resp.append("\x15\x12");
	if (waitFOR(9, "get swr")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return fm_bcd(&replystr[p+6], 3) * 100 / 255;
	}
	return -1;
}

void RIG_IC703::set_compression()
{
	if (progStatus.compON) {
		cmd = pre_to;
		cmd.append("\x16\x44");
		cmd += '\x01';
		cmd.append(post);
		waitFB("set comp ON");
	} else {
		cmd = pre_to;
		cmd.append("\x16\x44");
		cmd += '\x00';
		cmd.append(post);
		waitFB("set comp OFF");
	}
}

void RIG_IC703::set_vox_onoff()
{
	if (progStatus.vox_onoff) {
		cmd = pre_to;
		cmd.append("\x16\x46");
		cmd += '\x01';
		cmd.append(post);
		waitFB("set vox ON");
	} else {
		cmd = pre_to;
		cmd.append("\x16\x46");
		cmd += '\x00';
		cmd.append(post);
		waitFB("set vox OFF");
	}
}

// Tranceiver PTT on/off
void RIG_IC703::set_PTT_control(int val)
{
	cmd = pre_to;
	cmd += '\x1c';
	cmd += '\x00';
	cmd += (unsigned char) val;
	cmd.append( post );
	waitFB("set PTT");
}

