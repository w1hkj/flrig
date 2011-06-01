/*
 * Icom IC-910H
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */

#include "IC910.h"

const char IC910Hname_[] = "IC-910H";
const char *IC910Hmodes_[] = {
		"LSB", "USB", "AM", "CW", "FM", NULL};
// mode values are 0, 1, 2, 3, 4, 5, 7, 8
const char IC910H_mode_type[] =
	{ 'L', 'U', 'U', 'U', 'L', 'U'};

const char *IC910H_widths[] = {"none", NULL};


RIG_IC910H::RIG_IC910H() {
	defaultCIV = 0x60;
	name_ = IC910Hname_;
	modes_ = IC910Hmodes_;
	_mode_type = IC910H_mode_type;
	bandwidths_ = IC910H_widths;

	def_freq = 1296070000L;
	def_mode = 1;

	has_notch_control =
	has_tune_control =
	has_alc_control =
	has_bandwidth_control = false;

	has_smeter =
	has_extras =
	has_vox_onoff =
	has_vox_gain =
	has_vox_anti =
	has_vox_hang =
	has_compON =
	has_compression =
	has_ptt_control =
	has_power_control =
	has_volume_control =
	has_mode_control =
	has_micgain_control =
	has_attenuator_control =
	has_preamp_control =
	has_ifshift_control =
	has_swr_control =
	has_noise_control =
	has_noise_reduction =
	has_noise_reduction_control =
	has_rf_control =
	has_sql_control =
	restore_mbw = true;

	comp_is_on = !progStatus.compON;
	adjustCIV(defaultCIV);
};


// this looks like trouble
void RIG_IC910H::set_vfoA (long freq)
{
	long nufreq;
	if (freq > 1300000000L) nufreq = 1300000000L;
	else if (freq > 450000000L && freq < 1240000000L && freqA <= 450000000L)
		nufreq = 1240000000L;
	else if (freq > 450000000L && freq < 1240000000L && freqA >= 1240000000L)
		nufreq = 450000000L;
	else if (freq > 148000000L && freq < 430000000L && freqA <= 148000000L)
		nufreq = 430000000L;
	else if (freq > 148000000L && freq < 430000000L && freqA >= 430000000L)
		nufreq = 148000000L;
	else if (freq < 144000000L) nufreq = 144000000L;
	else nufreq = freq;
	freqA = nufreq;

	if (freqA != freq) {
		vfoA.freq = freqA;
		setFreqDispA((void*)0);
	}
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freqA, 10 ) );
	cmd.append( post );
	waitFB("set vfo A");}


void RIG_IC910H::set_compression()
{
	if (progStatus.compON) {
		cmd = pre_to;
		cmd.append("\x14\x0E");
		cmd.append(to_bcd(progStatus.compression * 255 / 100, 3));
		cmd.append( post );
		waitFB("set comp");
		if (comp_is_on != progStatus.compON) {
			comp_is_on = progStatus.compON;
			cmd = pre_to;
			cmd.append("\x16\01");
			cmd.append( post );
			waitFB("set comp val");
		}
	} else if (comp_is_on != progStatus.compON) {
		comp_is_on = progStatus.compON;
		cmd = pre_to;
		cmd.append("\x16\00");
		cmd.append( post );
		waitFB("set comp");
	}
}

void RIG_IC910H::set_vox_onoff()
{
	cmd = pre_to;
	cmd.append("\x16\x46");
	cmd += progStatus.vox_onoff ? 1 : 0;
	cmd.append(post);
	waitFB("set vox");
}

void RIG_IC910H::set_vox_gain()
{
	cmd = pre_to;
	cmd.append("\x1A\x02");
	cmd.append(to_bcd(progStatus.vox_gain * 255 / 100, 3));
	cmd.append(post);
	waitFB("set vox gain");
}

void RIG_IC910H::set_vox_anti()
{
	cmd = pre_to;
	cmd.append("\x1A\x04");
	cmd.append(to_bcd(progStatus.vox_anti * 255 / 100, 3));
	cmd.append(post);
	waitFB("set antivox");
}

void RIG_IC910H::set_vox_hang()
{
	cmd = pre_to;
	cmd.append("\x1A\x03");
	cmd.append(to_bcd(progStatus.vox_hang * 255 / 100, 3));
	cmd.append(post);
	waitFB("set vox hang");
}

int RIG_IC910H::get_smeter()
{
	string cstr = "\x16\x02";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;
	if (waitFOR(9, "get smeter")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			mtr = fm_bcd(&replystr[p+6], 3);
			mtr = (int)(mtr /2.55);
			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

