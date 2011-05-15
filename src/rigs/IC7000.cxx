/*
 * Icom IC-7000 driver
 *
 * a part of flrig
 *
 * Copyright 2011, Dave Freese, W1HKJ
 *
 */

#include "IC7000.h"
#include "debug.h"

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

	A.freq = 432399230;
	A.imode = 1;
	A.iBW = 28;
	B.freq = 432399230;
	B.imode = 1;
	B.iBW = 28;

	adjustCIV(defaultCIV);


	restore_mbw = false;

	has_ifshift_control = false;

	has_smeter =
	has_power_out =
	has_alc_control =
	has_swr_control =
	has_split =
	has_auto_notch = 
	has_power_control = 
	has_volume_control = 
	has_mode_control = 
	has_bandwidth_control = 
	has_micgain_control = 
	has_notch_control = 
	has_noise_control = 
	has_noise_reduction_control = 
	has_noise_reduction = 
	has_attenuator_control = 
	has_preamp_control = 
	has_ptt_control = 
	has_tune_control = 
	has_rf_control = 
	has_sql_control = true;

};

//======================================================================
// IC7000 unique commands
//======================================================================

#include "debug.h"

void RIG_IC7000::initialize()
{
	cmd = pre_to;
	cmd += '\x1A';
	cmd += '\x05';
	cmd += '\x00';
	cmd += '\x92';
	cmd += '\x00';
	cmd.append(post);
	if (!waitFB("CI-V") && RigSerial.IsOpen()) {
		flrig_abort = true;
		return;
	}
	cmd = pre_to;
	cmd.append("\x16\x51");
	cmd += '\x00';
	cmd.append(post);
	if (!waitFB("NF2 OFF") && RigSerial.IsOpen()) flrig_abort = true;
}

void RIG_IC7000::selectA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x00';
	cmd.append(post);
	if (!waitFB("sel A") && RigSerial.IsOpen()) flrig_abort = true;
}

void RIG_IC7000::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x01';
	cmd.append(post);
	if (!waitFB("sel B") && RigSerial.IsOpen()) flrig_abort = true;
}

long RIG_IC7000::get_vfoA ()
{
	string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	if (waitFOR(11, "get vfo A")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			A.freq = fm_bcd_be(&replystr[p+5], 10);
	} else if (RigSerial.IsOpen())
		flrig_abort = true;
	return A.freq;
}

void RIG_IC7000::set_vfoA (long freq)
{
	A.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append(post);
	waitFB("set vfo A");
}

long RIG_IC7000::get_vfoB ()
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
	} else if (RigSerial.IsOpen())
		flrig_abort = true;
	return B.freq;
}

void RIG_IC7000::set_vfoB (long freq)
{
	B.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	waitFB("set vfo B");
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
	waitFB("set mode A");
}

int RIG_IC7000::get_modeA()
{
	int md;
	string resp = pre_fm;
	resp += '\x04';
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	if (waitFOR(8, "get mode A")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			md = replystr[p+5];
			if (md > 6) md--;
			A.imode = md;
		}
	} else if (RigSerial.IsOpen())
		flrig_abort = true;
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
	waitFB("set mode B");
}

int RIG_IC7000::get_modeB()
{
	int md;
	string resp = pre_fm;
	resp += '\x04';
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	if (waitFOR(8, "get mode B")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			md = replystr[p+5];
			if (md > 6) md--;
			B.imode = md;
		}
	} else if (RigSerial.IsOpen())
		flrig_abort = true;
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
	if (waitFOR(8, "get bw A")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			A.iBW = (fm_bcd(&replystr[p+6],2));
	} else if (RigSerial.IsOpen())
		flrig_abort = true;
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
	if (waitFOR(8, "get bw B")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			B.iBW = (fm_bcd(&replystr[p+6],2));
	} else if (RigSerial.IsOpen())
		flrig_abort = true;
	return B.iBW;
}

void RIG_IC7000::set_bwA(int val)
{
	A.iBW = val;
	cmd = pre_to;
	cmd.append("\x1A\x03");
	cmd.append(to_bcd(val, 2));
	cmd.append( post );
	waitFB("set bw A");
}

void RIG_IC7000::set_bwB(int val)
{
	B.iBW = val;
	cmd = pre_to;
	cmd.append("\x1A\x03");
	cmd.append(to_bcd(val, 2));
	cmd.append( post );
	waitFB("set bw B");
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
	if (waitFOR(7, "get att")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (replystr[p+5] ? 1 : 0);
	}
	return progStatus.attenuator;
}

void RIG_IC7000::set_attenuator(int val)
{
	cmd = pre_to;
	cmd += '\x11';
	cmd += val ? '\x12' : '\x00';
	cmd.append( post );
	waitFB("set att");
}

void RIG_IC7000::set_preamp(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd += val ? 0x01 : 0x00;
	cmd.append( post );
	waitFB("set preamp");
}

int RIG_IC7000::get_preamp()
{
	string cstr = "\x16\x02";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get preamp")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return replystr[p+6] ? 1 : 0;
	}
	return progStatus.preamp;
}

void RIG_IC7000::set_auto_notch(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x41';
	cmd += val ? 0x01 : 0x00;
	cmd.append( post );
	waitFB("set AN");
}

int RIG_IC7000::get_auto_notch()
{
	string cstr = "\x16\x41";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get AN")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			if (replystr[p+6] == 0x01) {
				auto_notch_label("AN", true);
				return true;
			} else {
				auto_notch_label("AN", false);
				return false;
			}
		}
	}
	return progStatus.auto_notch;
}

void RIG_IC7000::set_split(bool val)
{
	cmd = pre_to;
	cmd += 0x0F;
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	waitFB("set split");
}

bool RIG_IC7000::get_split()
{
	cmd = pre_to;
	cmd += 0x0F;
	cmd.append(post);
	string resp = pre_fm;
	resp += 0x0F;
	if (waitFOR(8, "get split")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (replystr[p+6] ? 1 : 0);
	}
	return 0;
}

// Volume control val 0 ... 100
void RIG_IC7000::set_volume_control(int val)
{
	int ICvol = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append(to_bcd(ICvol, 3));
	cmd.append( post );
	waitFB("set vol");
}

int RIG_IC7000::get_volume_control()
{
	string cstr = "\x14\x01";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(9, "get vol")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return ((int)(fm_bcd(&replystr[p + 6],3) * 100 / 255));
	}
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
	waitFB("set RF");
}

int RIG_IC7000::get_rf_gain()
{
	string cstr = "\x14\x02";
	string resp = pre_fm;
	cmd = pre_to;
	cmd.append(cstr).append(post);
	resp.append(cstr);
	if (waitFOR(9, "get RF")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return ((int)(fm_bcd(&replystr[p + 6],3) * 100 / 255));
	}
	return progStatus.rfgain;
}

void RIG_IC7000::set_squelch(int val)
{
	int ICsql = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x03");
	cmd.append(to_bcd(ICsql, 3));
	cmd.append( post );
	waitFB("set sql");
}

int  RIG_IC7000::get_squelch()
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
			return ((int)(fm_bcd(&replystr[p+6], 3) * 100 / 255));
	}
	return progStatus.squelch;
}

void RIG_IC7000::set_power_control(double val)
{
	cmd = pre_to;
	cmd.append("\x14\x0A");
	cmd.append(to_bcd((int)(val * 255 / 100), 3));
	cmd.append( post );
	waitFB("set power");
}

int RIG_IC7000::get_power_control()
{
	string cstr = "\x14\x0A";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr).append(post);
	if (waitFOR(9, "get power")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return ((int)(fm_bcd(&replystr[p + 6],3) * 100 / 255));
	}
	return progStatus.power_level;
}

int RIG_IC7000::get_smeter()
{
	string cstr = "\x15\x02";
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

int RIG_IC7000::get_power_out()
{
	string cstr = "\x15\x11";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;
	if (waitFOR(9, "get pout")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			mtr = fm_bcd(&replystr[p+6], 3);
			mtr = (int)(mtr /2.55);
			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

int RIG_IC7000::get_alc()
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
			mtr = (int)(mtr /2.55);
			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

int RIG_IC7000::get_mic_gain()
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
			return ((int)(fm_bcd(&replystr[p+6],3) / 2.55));
	}
	return 0;
}

void RIG_IC7000::set_mic_gain(int val)
{
	val = (int)(val * 255 / 100);
	cmd = pre_to;
	cmd.append("\x14\x0B");
	cmd.append(to_bcd(val,3));
	cmd.append(post);
	waitFB("set mic");
}

void RIG_IC7000::get_mic_gain_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_IC7000::set_notch(bool on, int val)
{
	int notch = (int)(val * 256.0 / 3000.0);

	cmd = pre_to;
	cmd.append("\x16\x48");
	cmd += on ? '\x01' : '\x00';
	cmd.append(post);
	waitFB("set notch");

	cmd = pre_to;
	cmd.append("\x14\x0D");
	cmd.append(to_bcd(notch,3));
	cmd.append(post);
	waitFB("set notch val");

}

bool RIG_IC7000::get_notch(int &val)
{
	bool on = false;
	val = 0;

	string cstr = "\x16\x48";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get notch")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			on = replystr[p + 6];
		cmd = pre_to;
		resp = pre_fm;
		cstr = "\x14\x0D";
		cmd.append(cstr);
		resp.append(cstr);
		cmd.append(post);
		if (waitFOR(9, "notch val")) {
			size_t p = replystr.rfind(resp);
			if (p != string::npos)
				val = (int)(3000.0*(fm_bcd(&replystr[p + 6],3)) / 256.0);
		}
	}
	return on;
}

void RIG_IC7000::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 3000;
	step = 20;
}

void RIG_IC7000::set_noise(bool val)
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set noise");
}

int RIG_IC7000::get_noise()
{
	string cstr = "\x16\x22";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(8, "get noise")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (replystr[p+6] ? 1 : 0);
	}
	return 0;
}

void RIG_IC7000::set_noise_reduction(int val)
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	waitFB("set NR");
}

int RIG_IC7000::get_noise_reduction()
{
	string cstr = "\x16\x40";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(8, "get NR")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (replystr[p+6] == 0x01 ? 1 : 0);
	}
	return 0;
}

// 0 < val < 100
void RIG_IC7000::set_noise_reduction_val(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x06");
	cmd.append(to_bcd(val * 255 / 100, 3));
	cmd.append(post);
	waitFB("set NR val");
}

int RIG_IC7000::get_noise_reduction_val()
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
			return ((int)(fm_bcd(&replystr[p+6],3) / 2.55));
	}
	return 0;
}

