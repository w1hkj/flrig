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

static GUI IC703_widgets[]= {
	{ (Fl_Widget *)btnVol, 2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME, 54, 125, 156 },
	{ (Fl_Widget *)sldrRFGAIN, 54, 145, 156 },
	{ (Fl_Widget *)sldrSQUELCH, 54, 165, 156 },
	{ (Fl_Widget *)btnNR, 214, 125,  50 },
	{ (Fl_Widget *)sldrNR, 266, 125, 156 },
	{ (Fl_Widget *)btnIFsh, 214, 105,  50 },
	{ (Fl_Widget *)sldrIFSHIFT, 266, 105, 156 },
	{ (Fl_Widget *)sldrMICGAIN, 266, 145, 156 },
	{ (Fl_Widget *)sldrPOWER, 266, 165, 156 },
	{ (Fl_Widget *)NULL, 0, 0, 0 }
};

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

	widgets = IC703_widgets;

	has_smeter =
	has_power_out =
	has_swr_control =
	has_alc_control =

	has_volume_control =
	has_rf_control = 
	has_sql_control =
	has_noise_reduction =
	has_noise_reduction_control =
	has_noise_control =
	has_ifshift_control =
	has_micgain_control =
	has_power_control =

	has_vox_onoff =
	has_vox_gain =
	has_vox_anti =
	has_vox_hang =

	has_cw_wpm =
//	has_cw_keyer =
//	has_cw_spot =
	has_cw_spot_tone =
	has_cw_qsk =
//	has_cw_weight =

	has_tune_control =
	has_ptt_control =
	has_mode_control =
	has_bandwidth_control =
	has_extras =
	has_compON =
	has_compression =
	has_preamp_control =
	has_attenuator_control = true;

	defaultCIV = 0x68;
	adjustCIV(defaultCIV);
};

//=============================================================================

void RIG_IC703::initialize()
{
	IC703_widgets[0].W = btnVol;
	IC703_widgets[1].W = sldrVOLUME;
	IC703_widgets[2].W = sldrRFGAIN;
	IC703_widgets[3].W = sldrSQUELCH;
	IC703_widgets[4].W = btnNR;
	IC703_widgets[5].W = sldrNR;
	IC703_widgets[6].W = btnIFsh;
	IC703_widgets[7].W = sldrIFSHIFT;
	IC703_widgets[8].W = sldrMICGAIN;
	IC703_widgets[9].W = sldrPOWER;
}

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
			return (int)ceil(fm_bcd(&replystr[p+6], 3) * 100 / 255);
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
			return (int)ceil(fm_bcd(&replystr[p+6], 3) * 100 / 255);
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
			return (int)ceil(fm_bcd(&replystr[p+6], 3) * 100 / 255);
	}
	return -1;
}

int RIG_IC703::get_alc()
{
	string cstr = "\x15\x13";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;
	if (waitFOR(9, "get alc")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			mtr = fm_bcd(&replystr[p+6], 3);
			mtr = (int)ceil(mtr /2.55);
			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
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

// changed noise blanker to noise reduction
void RIG_IC703::set_noise(bool val)
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set noise");
}

int RIG_IC703::get_noise()
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
	return progStatus.noise;
}

void RIG_IC703::set_noise_reduction(int val)
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set NR");
}

int RIG_IC703::get_noise_reduction()
{
	string cstr = "\x16\x40";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get NR")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (replystr[p+6] ? 1 : 0);
	}
	return progStatus.noise_reduction;
}

// 0 < val < 100
void RIG_IC703::set_noise_reduction_val(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x06");
	cmd.append(to_bcd(val * 2.55, 3));
	cmd.append(post);
	waitFB("set NR val");
}

int RIG_IC703::get_noise_reduction_val()
{
	string cstr = "\x14\x06";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get NR val")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (int)ceil(fm_bcd(&replystr[p+6],3) / 2.55);
	}
	return progStatus.noise_reduction_val;
}

int RIG_IC703::get_mic_gain()
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

void RIG_IC703::set_mic_gain(int val)
{
	val = (int)(val * 2.55);
	cmd = pre_to;
	cmd.append("\x14\x0B");
	cmd.append(to_bcd(val,3));
	cmd.append(post);
	waitFB("set mic");
}

void RIG_IC703::get_mic_gain_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_IC703::set_if_shift(int val)
{
	int shift = (int)((val + 50) * 2.55 );
	cmd.assign(pre_to).append("\x14\x04").append(to_bcd(shift, 3)).append(post);
	waitFB("set if-shift");
}

bool  RIG_IC703::get_if_shift(int &val)
{
	string cstr = "\x14\x07";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	val = progStatus.shift_val;
	if (waitFOR(9, "get if-shift")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			val = (int)ceil(fm_bcd(&replystr[p+6], 3) / 2.55 - 50);
	}
	return (progStatus.shift = (val != 0));
}

void RIG_IC703::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -50;
	max = +50;
	step = 1;
}

void RIG_IC703::set_squelch(int val)
{
	int IC703sql = (int)(val * 2.55);
	cmd = pre_to;
	cmd.append("\x14\x03");
	cmd.append(to_bcd(IC703sql, 3));
	cmd.append( post );
	waitFB("set sql");
}

int  RIG_IC703::get_squelch()
{
	string cstr = "\x14\x03";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get sql")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (int)ceil(fm_bcd(&replystr[p+6], 3) / 2.55);
	}
	return progStatus.squelch;
}

void RIG_IC703::set_rf_gain(int val)
{
	int IC703rfg = (int)(val * 2.55);
	cmd = pre_to;
	cmd.append("\x14\x02");
	cmd.append(to_bcd(IC703rfg, 3));
	cmd.append( post );
	waitFB("set rf gain");
}

int RIG_IC703::get_rf_gain()
{
	string cstr = "\x14\x02";
	string resp = pre_fm;
	cmd = pre_to;
	cmd.append(cstr).append(post);
	resp.append(cstr);
	if (waitFOR(9, "get rfgain")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (int)ceil(fm_bcd(&replystr[p + 6],3) / 2.55);
	}
	return progStatus.rfgain;
}

void RIG_IC703::set_power_control(double val)
{
	cmd = pre_to;
	cmd.append("\x14\x0A");
	cmd.append(to_bcd((int)(val * 2.55), 3));
	cmd.append( post );
	waitFB("set power");
}

int RIG_IC703::get_power_control()
{
	string cstr = "\x14\x0A";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr).append(post);
	if (waitFOR(9, "get power")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (int)ceil(fm_bcd(&replystr[p + 6],3) / 2.55);
	}
	return progStatus.power_level;
}

void RIG_IC703::set_split(bool val)
{
	cmd = pre_to;
	cmd += 0x0F;
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	waitFB("set split");
}

bool RIG_IC703::get_split()
{
	LOG_WARN("%s", "get split - not implemented");
	return progStatus.split;
}

void RIG_IC703::tune_rig()
{
	cmd = pre_to;
	cmd.append("\x1c\x01\x02");
	cmd.append( post );
	waitFB("tune");
}

void RIG_IC703::set_compression()
{
	if (progStatus.compON) {
		cmd.assign(pre_to).append("\x14\x0E");
		cmd.append(to_bcd(progStatus.compression * 255 / 100, 3));
		cmd.append( post );
		waitFB("set comp");

		cmd = pre_to;
		cmd.append("\x16\x44");
		cmd += '\x01';
		cmd.append(post);
		waitFB("set Comp ON");

	} else{
		cmd.assign(pre_to).append("\x16\x44");
		cmd += '\x00';
		cmd.append(post);
		waitFB("set Comp OFF");
	}
}

void RIG_IC703::set_vox_onoff()
{
	if (progStatus.vox_onoff) {
		cmd.assign(pre_to).append("\x16\x46\x01").append(post);
		waitFB("set vox ON");
	} else {
		cmd.assign(pre_to).append("\x16\x46\x00").append(post);
		waitFB("set vox OFF");
	}
}

void RIG_IC703::set_vox_gain()
{
	cmd.assign(pre_to).append("\x1A\x03\x09");
	cmd.append(to_bcd((int)(progStatus.vox_gain * 2.55), 3));
	cmd.append( post );
	waitFB("SET vox gain");
}

void RIG_IC703::set_vox_anti()
{
	cmd.assign(pre_to).append("\x1A\x03\x10");
	cmd.append(to_bcd((int)(progStatus.vox_anti * 2.55), 3));
	cmd.append( post );
	waitFB("SET anti-vox");
}

void RIG_IC703::set_vox_hang()
{
	cmd.assign(pre_to).append("\x1A\x03\x11");
	cmd.append(to_bcd((int)(progStatus.vox_hang * 2.55), 3));
	cmd.append( post );
	waitFB("SET vox hang");
}

// CW controls

void RIG_IC703::set_cw_wpm()
{
	cmd.assign(pre_to).append("\x14\x0C");
	cmd.append(to_bcd(round((progStatus.cw_wpm - 6) * 255 / (60 - 6)), 3));
	cmd.append( post );
	waitFB("SET cw wpm");
}

void RIG_IC703::set_cw_qsk()
{
	int n = round(progStatus.cw_qsk * 10);
	cmd.assign(pre_to).append("\x14\x0F");
	cmd.append(to_bcd(n, 3));
	cmd.append(post);
	waitFB("Set cw qsk delay");
}

void RIG_IC703::set_cw_spot_tone()
{
	cmd.assign(pre_to).append("\x14\x09 ");
	int n = round((progStatus.cw_spot_tone - 300) * 255.0 / 600.0);
	if (n > 255) n = 255;
	if (n < 0) n = 0;
	cmd.append(to_bcd(n, 3)).append(post);
	waitFB("SET cw spot tone");
}

void RIG_IC703::set_cw_vol()
{
	cmd.assign(pre_to).append("\x1A\x03\0x06");
	cmd.append(to_bcd((int)(progStatus.cw_vol * 2.55), 3));
	cmd.append( post );
	waitFB("SET cw sidetone volume");
}
