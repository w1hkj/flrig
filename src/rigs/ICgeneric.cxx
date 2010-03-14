/*
 * Icom IC-746, 746PRO, 756, 756PRO
 * 7000, 7200, 7700 ... drivers
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */

#include "ICgeneric.h"
#include "debug.h"
#include "support.h"


//=============================================================================
// IC-746
//
//IC746/756/7000... series command string

const char IC746name_[] = "IC-746";

const char *IC746modes_[] = {
		"LSB", "USB", "AM", "CW", "RTTY", "FM", "CW-R", "RTTY-R", NULL};
// mode values are 0, 1, 2, 3, 4, 5, 7, 8
const char IC746_mode_type[] =
	{ 'L', 'U', 'U', 'U', 'L', 'U', 'L', 'U'};

const char *IC746_widths[] = { "NARR", "WIDE", NULL};

const char *IC746_AMFMwidths[] = { "FILT-1", "FILT-2", "FILT-3", NULL };

RIG_IC746::RIG_IC746() {
	name_ = IC746name_;
	modes_ = IC746modes_;
	bandwidths_ = IC746_widths;
	_mode_type = IC746_mode_type;

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
	mode_ = 1;
	bw_ = 28;
	deffreq_ = 14070000L;
	def_mode = 1;
	defbw_ = 28;

	has_power_control = true;
	has_volume_control = true;
	has_mode_control = true;
	has_bandwidth_control = true;
	has_micgain_control = true;
	has_notch_control = true;
	has_attenuator_control = true;
	has_preamp_control = true;
	has_ifshift_control = true;
	has_ptt_control = true;
	has_tune_control = true;
	has_swr_control = true;
	has_noise_control = true;
	has_noise_reduction = true;
	has_noise_reduction_control = true;
	has_alc_control = true;
	has_rf_control = true;
	has_sql_control = true;
	restore_mbw = true;

};

//=============================================================================
// 746PRO
const char IC746PROname_[] = "IC-746PRO";

const char *IC746PROmodes_[] = {
		"LSB", "USB", "AM", "CW", "RTTY", "FM", "CW-R", "RTTY-R",
		"D-LSB", "D-USB", "D-FM", NULL};

const char IC746PRO_mode_type[] =
	{ 'L', 'U', 'U', 'U', 'L', 'U', 'L', 'U',
	  'L', 'U', 'U' };

const char *IC746PRO_SSBwidths[] = {
  "50",  "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700", "2800", "2900", "3000", "3100", "3200", "3300", "3400", "3500",
"3600",
NULL};

const char *IC746PRO_RTTYwidths[] = {
  "50",  "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
 "600",  "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700",
NULL};

RIG_IC746PRO::RIG_IC746PRO() {
	name_ = IC746PROname_;
	modes_ = IC746PROmodes_;
	bandwidths_ = IC746PRO_SSBwidths;
	_mode_type = IC746PRO_mode_type;
	pre_to[2] = ok[3] = bad[3] = pre_fm[3] = 0x66;
	atten_level = 0;
	preamp_level = 0;
	def_mode = 9;
	defbw_ = 32;
	deffreq_ = 14070000;
};

//=============================================================================
// 756PRO-II

const char IC756PRO2name_[] = "IC-756PRO-II";

RIG_IC756PRO2::RIG_IC756PRO2() {
	name_ = IC756PRO2name_;
	pre_to[2] = ok[3] = bad[3] = pre_fm[3] = 0x64;
	atten_level = 0;
	preamp_level = 0;
};

//=============================================================================
// 756PRO-III

const char IC756PRO3name_[] = "IC-756PRO-III";

RIG_IC756PRO3::RIG_IC756PRO3() {
	name_ = IC756PRO3name_;
	pre_to[2] = ok[3] = bad[3] = pre_fm[3] = 0x6E;
};

//=============================================================================
// IC-7000

const char IC7000name_[] = "IC-7000";

RIG_IC7000::RIG_IC7000() {
	name_ = IC7000name_;
	pre_to[2] = ok[3] = bad[3] = pre_fm[3] = 0x70;
};

//=============================================================================
// IC-7200

const char IC7200name_[] = "IC-7200";

RIG_IC7200::RIG_IC7200() {
	name_ = IC7200name_;
	pre_to[2] = ok[3] = bad[3] = pre_fm[3] = 0x76;
};

//=============================================================================
// IC-7600

const char IC7600name_[] = "IC-7600";

const char *IC7600modes_[] = {
		"LSB", "USB", "AM", "CW", "RTTY", "FM", "CW-R", "RTTY-R",
		"D-LSB", "D-USB", NULL};

const char IC7600_mode_type[] =
	{ 'L', 'U', 'U', 'U', 'L', 'U', 'L', 'U',
	  'L', 'U' };

RIG_IC7600::RIG_IC7600() {
	name_ = IC7600name_;
	modes_ = IC7600modes_;
	_mode_type = IC7600_mode_type;
//	bandwidths_ = IC7600_widths;
	pre_to[2] = ok[3] = bad[3] = pre_fm[3] = 0x7A;
};

//=============================================================================
// IC-7700

const char IC7700name_[] = "IC-7700";

RIG_IC7700::RIG_IC7700() {
	name_ = IC7700name_;
	pre_to[2] = ok[3] = bad[3] = pre_fm[3] = 0x74;
};

//=============================================================================
// IC-910H

const char IC910Hname_[] = "IC-910H";
const char *IC910Hmodes_[] = {
		"LSB", "USB", "AM", "CW", "FM", NULL};
// mode values are 0, 1, 2, 3, 4, 5, 7, 8
const char IC910H_mode_type[] =
	{ 'L', 'U', 'U', 'U', 'L', 'U'};

const char *IC910H_widths[] = {"none", NULL};


RIG_IC910H::RIG_IC910H() {
	name_ = IC910Hname_;
	modes_ = IC910Hmodes_;
	_mode_type = IC910H_mode_type;
	bandwidths_ = IC910H_widths;

	pre_to[2] = ok[3] = bad[3] = pre_fm[3] = 0x60;

	deffreq_ = 1296070000L;
	def_mode = 1;

	has_notch_control =
	has_tune_control =
	has_alc_control =
	has_bandwidth_control = false;

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
};

//=============================================================================

void RIG_ICOM::checkresponse(int n)
{
	if (RigSerial.IsOpen() == false)
		return;

	if (replystr.find(ok) != string::npos)
//	if (strcmp( replybuff, ok.c_str()) == 0)
		return;

	LOG_ERROR("\nsent  %s\nreply %s",
		str2hex(cmd.c_str(), cmd.length()),
		str2hex(replybuff, n));
}

bool RIG_ICOM::sendICcommand(string cmd, int nbr)
{
	for (int i = 0; i < progStatus.comm_retries; i++) {
		if (i) clearSerialPort();
		if (sendCommand(cmd, nbr, true) != nbr) {
			if (RigSerial.IsOpen())
				LOG_ERROR("sendCommand() failed");
			continue;
		}

// look for preamble at beginning
//		LOG_INFO("got %s", str2hex(replystr.c_str(), replystr.length()));
		if (replystr.find(pre_fm) == string::npos)  {
			LOG_ERROR("preamble");
			continue;
		}

// look for postamble
		if (replystr.find(post) == string::npos) {
			LOG_ERROR("postample 0x%X", (unsigned char)post[0]);
			continue;
		}
		return true;
	}
	LOG_ERROR("Exceeded retry count");

	return false;
}

long RIG_IC746::get_vfoA ()
{
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	if (!sendICcommand(cmd, 11))
		return freq_;
	freq_ = fm_bcd_be(&replystr[5], 10);
	return freq_;
}

void RIG_IC746::set_vfoA (long freq)
{
	freq_ = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	sendICcommand(cmd, 6);
	checkresponse(6);
}

int RIG_IC746::get_smeter()
{
	cmd = pre_to;
	cmd.append("\x15\x02");
	cmd.append( post );
	if (sendICcommand (cmd, 9)) {
		return fm_bcd(&replystr[6], 3) / 2.55;
	} else
		return 0;
}

int RIG_IC746::get_swr()
{
	cmd = pre_to;
	cmd.append("\x15\x12");
	cmd.append( post );
	sendICcommand (cmd, 9);
	return fm_bcd(&replystr[6],3) / 3.6;
}

int RIG_IC746::get_alc()
{
	cmd = pre_to;
	cmd.append("\x15\x13");
	cmd.append( post );
	sendICcommand (cmd, 9);
	return fm_bcd(&replystr[6],3);
}

// Volume control val 0 ... 100
int ICvol = 0;
void RIG_IC746::set_volume_control(int val)
{
	ICvol = (int)(val * 2.55);
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append(to_bcd(ICvol, 3));
	cmd.append( post );
	sendICcommand (cmd, 6);
	checkresponse(6);
}

int RIG_IC746::get_volume_control()
{
	cmd = pre_to;
	cmd.append("\x14\x01");
	cmd.append( post );
	if(sendICcommand (cmd, 9))
		return ((int)(fm_bcd(&replystr[6],3) / 2.55));
	checkresponse(9);
	return ICvol / 2.55;
}

// Transceiver power level return power in watts
int RIG_IC746::get_power_out()
{
	cmd = pre_to;
	cmd.append("\x15\x11");
	cmd.append( post );
	if (sendICcommand (cmd, 9)) {
		int pwr = (int)(fm_bcd(&replystr[6],3) / 2.55 );
		return pwr;
	}
	return 0;
}

int RIG_IC746::get_power_control()
{
	cmd = pre_to;
	cmd.append("\x14\x0A");
	cmd.append( post );
	if(sendICcommand (cmd, 9))
		return (int)(fm_bcd(&replystr[6],3) / 2.55);
	return 0;
}

// Tranceiver PTT on/off
void RIG_IC746::set_PTT_control(int val)
{
	cmd = pre_to;
	cmd += '\x1c';
	cmd += '\x00';
	cmd += (unsigned char) val;
	cmd.append( post );
	sendICcommand (cmd, 6);
	checkresponse(6);
}

void RIG_IC746::tune_rig()
{
	cmd = pre_to;
	cmd.append("\x1c\x01\x02");
	cmd.append( post );
	sendICcommand (cmd, 6);
	checkresponse(6);
}

void RIG_IC746::set_attenuator(int val)
{
	cmd = pre_to;
	cmd += '\x11';
	cmd += val ? 0x20 : 0x00;
	cmd.append( post );
	sendICcommand(cmd,6);
	checkresponse(6);
	if (!val) {
		preamp_level--;
		if (preamp_level < 0) preamp_level = 2;
		set_preamp(1);
	}
}

int RIG_IC746::get_attenuator()
{
	cmd = pre_to;
	cmd += '\x11';
	cmd.append( post );
	if (sendICcommand(cmd,7))
		return (replystr[5] ? 1 : 0);
	return 0;
}

void RIG_IC746::set_preamp(int val)
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
	sendICcommand (cmd, 6);
	checkresponse(6);
}

int RIG_IC746::get_preamp()
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd.append( post );
	if (sendICcommand (cmd, 8)) {
		if (replystr[6] == 0x01) {
			preamp_label("Pre 1", true);
			preamp_level = 1;
		} else if (replystr[6] == 0x02) {
			preamp_label("Pre 2", true);
			preamp_level = 2;
		} else {
			preamp_label("Pre", false);
			preamp_level = 0;
		}
	}
	return preamp_level;
}

// changed noise blanker to noise reduction
void RIG_IC746::set_noise(bool val)
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd += val ? 1 : 0;
	cmd.append(post);
	sendICcommand(cmd, 6);
	checkresponse(6);
}

int RIG_IC746::get_noise()
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd.append(post);
	if (sendICcommand(cmd, 8))
		return (replystr[6] ? 1 : 0);
	return 0;
}

void RIG_IC746::set_noise_reduction(int val)
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd += val ? 1 : 0;
	cmd.append(post);
	sendICcommand(cmd, 6);
	checkresponse(6);
}

int RIG_IC746::get_noise_reduction()
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd.append(post);
	if (sendICcommand(cmd, 8))
		return (replystr[6] ? 1 : 0);
	return 0;
}

// 0 < val < 100
void RIG_IC746::set_noise_reduction_val(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x06");
	cmd.append(to_bcd(val * 2.55, 3));
	cmd.append(post);
	sendICcommand(cmd,6);
	checkresponse(6);
}

int RIG_IC746::get_noise_reduction_val()
{
	cmd = pre_to;
	cmd.append("\x14\x06");
	cmd.append(post);
	if(sendICcommand (cmd, 9))
		return ((int)(fm_bcd(&replystr[6],3) / 2.55));
	checkresponse(9);
	return 0;
}

void RIG_IC746::set_mode(int val)
{
	mode_ = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += val > 5 ? val + 1 : val;
	cmd += filter_nbr; // filter #1
	cmd.append( post );
	sendICcommand (cmd, 6);
	checkresponse(6);
}

int RIG_IC746::get_mode()
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

void RIG_IC746::set_bandwidth(int val)
{
//	if (bandwidths_ == IC746_AMFMwidths) {
//		filter_nbr = val + 1;
//		set_mode(mode_);
//		return;
//	}

	bw_ = val;
	cmd = pre_to;
	cmd.append("\x1A\x03");
	cmd.append(to_bcd(val, 2));
	cmd.append( post );
	sendICcommand(cmd, 6);
	checkresponse(6);
}

int RIG_IC746::get_modetype(int n)
{
	return _mode_type[n];
}

int  RIG_IC746::get_bandwidth()
{
	cmd = pre_to;
	cmd += "\x1A\x03";
	cmd.append( post );
	if (sendICcommand(cmd, 8))
		bw_ = (fm_bcd(&replystr[6],2));
	return bw_;
}

int RIG_IC746::adjust_bandwidth(int m)
{
	bandwidths_ = IC746_widths;
	return bw_ = 1;
//	if (m == 0 || m == 1 || m == 8 || m == 9) { //SSB
//		bandwidths_ = IC746_widths;
//		return (bw_ = 1);
//	}
//	if (m == 3 || m == 6) { //CW
//		bandwidths_ = IC746_widths;
//		return (bw_ = 14);
//	}
//	if (m == 4 || m == 7) { //RTTY
//		bandwidths_ = IC746_widths;
//		return (bw_ = 28);
//	}
//	bandwidths_ = IC746_AMFMwidths;
//	return (bw_ = 0);
}


bool IC_notchon = false;
void RIG_IC746::set_notch(bool on, int val)
{
	int notch = (int)(val/20.0 + 128);
	if (notch > 256) notch = 255;
	if (on != IC_notchon) {
		cmd = pre_to;
		cmd.append("\x16\x48");
		cmd += on ? '\x01' : '\x00';
		cmd.append(post);
		sendICcommand(cmd,6);
		checkresponse(6);
		IC_notchon = on;
	}

	if (on) {
		cmd = pre_to;
		cmd.append("\x14\x0D");
		cmd.append(to_bcd(notch,3));
		cmd.append(post);
		sendICcommand(cmd,6);
		checkresponse(6);
	}

}

bool RIG_IC746::get_notch(int &val)
{
	bool on = false;
	val = 0;
	cmd = pre_to;
	cmd.append("\x16\x48");
	cmd.append(post);
	if (sendICcommand(cmd,8)) {
		on = replystr[6] ? 1 : 0;
		cmd = pre_to;
		cmd.append("\x14\x0D");
		cmd.append(post);
		if (sendICcommand(cmd,9))
			val = 20*(fm_bcd(&replystr[6],3) - 128);
		else
			checkresponse(9);
		return on;
	}
	return on;
}

void RIG_IC746::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = -1280;
	max = 1280;
	step = 20;
}

void RIG_IC746::set_mic_gain(int val)
{
	val = (int)(val * 2.55);
	cmd = pre_to;
	cmd.append("\x14\x0B");
	cmd.append(to_bcd(val,3));
	cmd.append(post);
	sendICcommand(cmd, 6);
	checkresponse(6);
}

int RIG_IC746::get_mic_gain()
{
	cmd = pre_to;
	cmd.append("\x14\x0B");
	cmd.append(post);
	if (sendICcommand (cmd, 9))
		return ((int)(fm_bcd(&replystr[6],3) / 2.55));
	checkresponse(9);
	return 0;
}

void RIG_IC746::get_mic_gain_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_IC746::set_if_shift(int val)
{
	int shift = (int)((val + 50) * 2.56 );
	if (shift == 256) shift = 255;
	cmd = pre_to;
	cmd.append("\x14\x07");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	if (sendICcommand(cmd,6)) {
		cmd = pre_to;
		cmd.append("\x14\x08");
		cmd.append(to_bcd(shift, 3));
		cmd.append(post);
		sendICcommand(cmd,6);
	}
	checkresponse(6);
}

bool RIG_IC746::get_if_shift(int val)
{
	val = 0;
	cmd = pre_to;
	cmd.append("\x14\x07");
	cmd.append(post);
	if (sendICcommand(cmd,9)) {
		val = (int)((fm_bcd(&replystr[6],3) - 128) * 50 / 128.0);
		return true;
	}
	return false;
}

void RIG_IC746::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -50;
	max = +50;
	step = 2;
}

int ICsql = 0;
void RIG_IC746::set_squelch(int val)
{
	ICsql = (int)(val * 2.55);
	cmd = pre_to;
	cmd.append("\x14\x03");
	cmd.append(to_bcd(ICsql, 3));
	cmd.append( post );
	sendICcommand (cmd, 6);
	checkresponse(6);
}

int  RIG_IC746::get_squelch()
{
	cmd = pre_to;
	cmd.append("\x14\x03");
	cmd.append( post );
	if(sendICcommand (cmd, 9))
		return ((int)(fm_bcd(&replystr[6],3) / 2.55));
	checkresponse(9);
	return ICvol;
	return 0;
}

int ICrfg = 0;
void RIG_IC746::set_rf_gain(int val)
{
	ICrfg = (int)(val * 2.55);
	cmd = pre_to;
	cmd.append("\x14\x02");
	cmd.append(to_bcd(ICrfg, 3));
	cmd.append( post );
	sendICcommand (cmd, 6);
	checkresponse(6);
}

int  RIG_IC746::get_rf_gain()
{
	cmd = pre_to;
	cmd.append("\x14\x02");
	cmd.append( post );
	if(sendICcommand (cmd, 9))
		return ((int)(fm_bcd(&replystr[6],3) / 2.55));
	checkresponse(9);
	return ICvol;
	return 0;
}

void RIG_IC746::set_power_control(double val)
{
	cmd = pre_to;
	cmd.append("\x14\x0A");
	cmd.append(to_bcd((int)(val * 2.55), 3));
	cmd.append( post );
	sendICcommand (cmd, 6);
	checkresponse(6);
}

//======================================================================
// IC746PRO unique commands
//======================================================================

void RIG_IC746PRO::set_mode(int val)
{
	mode_ = val;
	bool datamode = false;
	switch (val) {
		case 10 : val = 5; datamode = true; break;
		case 9  : val = 1; datamode = true; break;
		case 8  : val = 0; datamode = true; break;
		case 7  : val = 8; break;
		case 6  : val = 7; break;
		default: break;
	}
	cmd = pre_to;
	cmd += '\x06';
	cmd += val;
	cmd += filter_nbr;
	cmd.append( post );
	sendICcommand (cmd, 6);
	checkresponse(6);
	if (datamode) { // LSB / USB ==> use DATA mode
		cmd = pre_to;
		cmd.append("\x1A\x06\x01");
		cmd.append(post);
		sendICcommand(cmd, 6);
		checkresponse(6);
	}
}

int RIG_IC746PRO::get_mode()
{
	int md;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	if (sendICcommand (cmd, 8 )) {
		md = replystr[5];
		if (md > 6) md--;
		filter_nbr = replystr[6];
		cmd = pre_to;
		cmd.append("\x1A\x06");
		cmd.append(post);
		if (sendICcommand(cmd, 8)) {
			if (replystr[6]) {
				switch (md) {
					case 0 : md = 8; break;
					case 1 : md = 9; break;
					case 5 : md = 10; break;
					default : break;
				}
			}
			mode_ = md;
		} else {
			checkresponse(8);
		}
	} else {
		checkresponse(8);
	}
	return mode_;
}

int RIG_IC746PRO::adjust_bandwidth(int m)
{
	if (m == 0 || m == 1 || m == 8 || m == 9) { //SSB
		bandwidths_ = IC746PRO_SSBwidths;
		return (bw_ = 32);
	}
	if (m == 3 || m == 6) { //CW
		bandwidths_ = IC746PRO_SSBwidths;
		return (bw_ = 14);
	}
	if (m == 4 || m == 7) { //RTTY
		bandwidths_ = IC746PRO_RTTYwidths;
		return (bw_ = 28);
	}
	bandwidths_ = IC746_AMFMwidths;
	return (bw_ = 0);
}

int RIG_IC746PRO::get_attenuator()
{
	cmd = pre_to;
	cmd += '\x11';
	cmd.append( post );
	if (sendICcommand(cmd,7))
		return (replystr[5] ? 1 : 0);
	return 0;
}

void RIG_IC746PRO::set_attenuator(int val)
{
	int cmdval = val ? 0x20 : 0x00;
	cmd = pre_to;
	cmd += '\x11';
	cmd += cmdval;
	cmd.append( post );
	sendICcommand(cmd,6);
	checkresponse(6);
}

//======================================================================
// IC756PRO2 unique commands
//======================================================================

void RIG_IC756PRO2::set_attenuator(int val)
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
	sendICcommand(cmd,6);
	checkresponse(6);
}

int RIG_IC756PRO2::get_attenuator()
{
	cmd = pre_to;
	cmd += '\x11';
	cmd.append( post );
	if (sendICcommand(cmd,7)) {
		if (replystr[5] == 0x06) {
			atten_level = 1;
			atten_label("6 dB", true);
		} else if (replystr[6] == 0x12) {
			atten_level = 2;
			atten_label("12 dB", true);
		} else if (replystr[6] == 0x18) {
			atten_level = 3;
			atten_label("18 dB", true);
		} else if (replystr[6] == 0x00) {
			atten_level = 0;
			atten_label("Att", false);
		}
	}
	return atten_level;
}

void RIG_IC756PRO2::set_preamp(int val)
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
	sendICcommand (cmd, 6);
	checkresponse(6);
}

int RIG_IC756PRO2::get_preamp()
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd.append( post );
	if (sendICcommand (cmd, 8)) {
		if (replystr[6] == 0x01) {
			preamp_label("Pre 1", true);
			preamp_level = 1;
		} else if (replystr[6] == 0x02) {
			preamp_label("Pre 2", true);
			preamp_level = 2;
		} else {
			preamp_label("Pre", false);
			preamp_level = 0;
		}
	}
	return preamp_level;
}

//======================================================================
// IC756ProIII unique commands
//======================================================================
void RIG_IC756PRO3::set_mode(int val)
{
	mode_ = val;
	bool datamode = false;
	switch (val) {
		case 10 : val = 5; datamode = true; break;
		case 9  : val = 1; datamode = true; break;
		case 8  : val = 0; datamode = true; break;
		case 7  : val = 8; break;
		case 6  : val = 7; break;
		default: break;
	}
	cmd = pre_to;
	cmd += '\x06';
	cmd += val;
	cmd += filter_nbr;
	cmd.append( post );
	sendICcommand (cmd, 6);
	checkresponse(6);
	if (datamode) { // LSB / USB ==> use DATA mode
		cmd = pre_to;
		cmd.append("\x1A\x06\x01");
		cmd.append(post);
		sendICcommand(cmd, 6);
		checkresponse(6);
	}
}

int RIG_IC756PRO3::get_mode()
{
	int md;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	if (sendICcommand (cmd, 8 )) {
		md = replystr[5];
		if (md > 6) md--;
		filter_nbr = replystr[6];
		cmd = pre_to;
		cmd.append("\x1A\x06");
		cmd.append(post);
		if (sendICcommand(cmd, 9)) {
			if (replystr[6]) {
				switch (md) {
					case 0 : md = 8; break;
					case 1 : md = 9; break;
					case 5 : md = 10; break;
					default : break;
				}
			}
			mode_ = md;
		} else {
			checkresponse(8);
		}
	} else {
		checkresponse(8);
	}
	return mode_;
}


//======================================================================
// IC7000 unique commands
//======================================================================

int RIG_IC7000::get_attenuator()
{
	cmd = pre_to;
	cmd += '\x11';
	cmd.append( post );
	if (sendICcommand(cmd,7))
		return (replystr[5] ? 1 : 0);
	return 0;
}

void RIG_IC7000::set_attenuator(int val)
{
	cmd = pre_to;
	cmd += '\x11';
	cmd += val ? '\x12' : '\x00';
	cmd.append( post );
	sendICcommand(cmd,6);
	checkresponse(6);
}

void RIG_IC7000::set_preamp(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd += val ? 0x01 : 0x00;
	cmd.append( post );
	sendICcommand (cmd, 6);
	checkresponse(6);
}

int RIG_IC7000::get_preamp()
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd.append( post );
	if (sendICcommand (cmd, 8))
		return replystr[6] ? 1 : 0;
	return 0;
}

//======================================================================
// IC7600 unique commands
//======================================================================
void RIG_IC7600::set_mode(int val)
{
	mode_ = val;
	bool datamode = false;
	switch (val) {
		case 9  : val = 1; datamode = true; break;
		case 8  : val = 0; datamode = true; break;
		case 7  : val = 8; break;
		case 6  : val = 7; break;
		default: break;
	}
	cmd = pre_to;
	cmd += '\x06';
	cmd += val;
	cmd += filter_nbr;
	cmd.append( post );
	sendICcommand (cmd, 6);
	checkresponse(6);
	if (datamode) { // LSB / USB ==> use DATA mode
		cmd = pre_to;
		cmd.append("\x1A\x06\x01\x01");
		cmd.append(post);
		sendICcommand(cmd, 6);
		checkresponse(6);
	}
}

int RIG_IC7600::get_mode()
{
	int md;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	if (sendICcommand (cmd, 8 )) {
		md = replystr[5];
		if (md > 6) md--;
		filter_nbr = replystr[6];
		cmd = pre_to;
		cmd.append("\x1A\x06");
		cmd.append(post);
		if (sendICcommand(cmd, 9)) {
			if (replystr[6]) {
				switch (md) {
					case 0 : md = 8; break;
					case 1 : md = 9; break;
					default : break;
				}
			}
			mode_ = md;
		} else {
			checkresponse(8);
		}
	} else {
		checkresponse(8);
	}
	return mode_;
}


//======================================================================
// IC910H
//======================================================================

void RIG_IC910H::set_vfoA (long freq)
{
	long nufreq;
	if (freq > 1300000000L) nufreq = 1300000000L;
	else if (freq > 450000000L && freq < 1240000000L && freq_ <= 450000000L)
		nufreq = 1240000000L;
	else if (freq > 450000000L && freq < 1240000000L && freq_ >= 1240000000L)
		nufreq = 450000000L;
	else if (freq > 148000000L && freq < 430000000L && freq_ <= 148000000L)
		nufreq = 430000000L;
	else if (freq > 148000000L && freq < 430000000L && freq_ >= 430000000L)
		nufreq = 148000000L;
	else if (freq < 144000000L) nufreq = 144000000L;
	else nufreq = freq;
	freq_ = nufreq;

	if (freq_ != freq) {
		vfoA.freq = freq_;
		setFreqDisp((void*)0);
	}
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq_, 10 ) );
	cmd.append( post );
	sendICcommand(cmd, 6);
	checkresponse(6);
}


void RIG_IC910H::set_compression()
{
	if (progStatus.compON) {
		cmd = pre_to;
		cmd.append("\x14\x0E");
		cmd.append(to_bcd(progStatus.compression * 2.55, 3));
		cmd.append( post );
		sendICcommand(cmd, 6);
		checkresponse(6);
		if (comp_is_on != progStatus.compON) {
			comp_is_on = progStatus.compON;
			cmd = pre_to;
			cmd.append("\x16\01");
			cmd.append( post );
			sendICcommand(cmd, 6);
			checkresponse(6);
		}
	} else if (comp_is_on != progStatus.compON) {
		comp_is_on = progStatus.compON;
		cmd = pre_to;
		cmd.append("\x16\00");
		cmd.append( post );
		sendICcommand(cmd, 6);
		checkresponse(6);
	}
}

void RIG_IC910H::set_vox_onoff()
{
	cmd = pre_to;
	cmd.append("\x16\x46");
	cmd += progStatus.vox_onoff ? 1 : 0;
	cmd.append(post);
	sendCommand(cmd, 6, true);
}

void RIG_IC910H::set_vox_gain()
{
	cmd = pre_to;
	cmd.append("\x1A\x02");
	cmd.append(to_bcd(progStatus.vox_gain * 2.55, 3));
	cmd.append(post);
	sendCommand(cmd, 6, true);
}

void RIG_IC910H::set_vox_anti()
{
	cmd = pre_to;
	cmd.append("\x1A\x04");
	cmd.append(to_bcd(progStatus.vox_anti * 2.55, 3));
	cmd.append(post);
	sendCommand(cmd, 6, true);
}

void RIG_IC910H::set_vox_hang()
{
	cmd = pre_to;
	cmd.append("\x1A\x03");
	cmd.append(to_bcd(progStatus.vox_hang * 2.55, 3));
	cmd.append(post);
	sendCommand(cmd, 6, true);
}

