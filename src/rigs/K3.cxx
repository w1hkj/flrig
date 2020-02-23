// ----------------------------------------------------------------------------
// Copyright (C) 2014
//              David Freese, W1HKJ
//
// This file is part of flrig.
//
// flrig is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// flrig is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include "K3.h"
#include "status.h"

const char K3name_[] = "K3";

const char *K3modes_[] =
	{ "LSB", "USB", "CW", "FM", "AM", "DATA", "CW-R", "DATA-R", NULL};
const char modenbr[] =
	{ '1', '2', '3', '4', '5', '6', '7', '9' };
static const char K3_mode_type[] =
	{ 'L', 'U', 'L', 'U', 'U', 'U', 'U', 'L' };

static const char *K3_widths[] = {
   "50",  "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
  "550",  "600",  "650",  "700",  "750",  "800",  "850",  "900",  "950", "1000",
 "1100", "1200", "1300", "1400", "1500", "1600", "1700", "1800", "1900", "2000",
 "2200", "2400", "2600", "2800", "3000", "3200", "3400", "3600", NULL};
static int K3_bw_vals[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38, 39,40, WVALS_LIMIT};

static int def_mode_width[] = { 34, 34, 15, 37, 37, 34, 15, 34 };

static GUI k3_widgets[]= {
	{ (Fl_Widget *)btnVol, 2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME, 54, 125, 156 },
	{ (Fl_Widget *)sldrRFGAIN, 54, 145, 156 },
	{ (Fl_Widget *)btnIFsh, 214, 105,  50 },
	{ (Fl_Widget *)sldrIFSHIFT, 266, 105, 156 },
	{ (Fl_Widget *)sldrMICGAIN, 266, 125, 156 },
	{ (Fl_Widget *)sldrPOWER, 266, 145, 156 },
	{ (Fl_Widget *)NULL, 0, 0, 0 }
};

RIG_K3::RIG_K3() {
// base class values
	name_ = K3name_;
	modes_ = K3modes_;
	bandwidths_ = K3_widths;
	bw_vals_ = K3_bw_vals;

	comm_baudrate = BR38400;

	widgets = k3_widgets;

	stopbits = 1;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = false;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;

	def_freq = freqA = freqB = 14070000;
	def_mode = modeA = modeB = 1;
	def_bw = bwA = bwB = 34;

	can_change_alt_vfo =

	has_split_AB =
	has_micgain_control =
	has_rf_control =
	has_bandwidth_control =
	has_power_control =
	has_volume_control =
	has_mode_control =
	has_ptt_control =
	has_noise_control =
	has_attenuator_control =
	has_smeter =
	has_power_out =
	has_split =
	has_ifshift_control =
	has_preamp_control = true;

	has_notch_control =
	has_tune_control =
	has_swr_control = false;

	if_shift_min = 400;
	if_shift_max = 2600;
	if_shift_step = 10;
	if_shift_mid = 1500;

	precision = 1;
	ndigits = 8;

}

int  RIG_K3::adjust_bandwidth(int m)
{
	return def_mode_width[m];
}

int  RIG_K3::def_bandwidth(int m)
{
	return def_mode_width[m];
}

#define K3_WAIT_TIME 800

void RIG_K3::initialize()
{
	debug::level = debug::INFO_LEVEL;

	LOG_INFO("K3");
	k3_widgets[0].W = btnVol;
	k3_widgets[1].W = sldrVOLUME;
	k3_widgets[2].W = sldrRFGAIN;
	k3_widgets[3].W = btnIFsh;
	k3_widgets[4].W = sldrIFSHIFT;
	k3_widgets[5].W = sldrMICGAIN;
	k3_widgets[6].W = sldrPOWER;

	cmd = "AI0;"; // disable auto-info
	sendCommand(cmd);
	showresp(INFO, ASC, "disable auto-info", cmd, replystr);
	sett("disable auto info");

	cmd = "K31;"; // K3 extended mode
	sendCommand(cmd);
	showresp(INFO, ASC, "K3 extended mode", cmd, replystr);
	sett("K3 extended mode");

	cmd = "SWT49;"; // Fine tuning (1 Hz mode)
	sendCommand(cmd);
	showresp(INFO, ASC, "set fine tune", cmd, replystr);
	sett("1 Hz fine tuning");

	cmd = "OM;"; // request options to get power level
	int ret = wait_char(';', 16, K3_WAIT_TIME, "Options", ASC);
	gett("get options");

	if (ret) {
		if (replystr.find("P") == string::npos) {
			minpwr = 0;
			maxpwr = 12;
			steppwr = 1;
		} else {
			minpwr = 0;
			maxpwr = 106;
			steppwr = 1;
		}
	}

	get_vfoA();
	get_modeA();
	get_bwA();

	get_vfoB();
	get_modeB();
	get_bwB();

	set_split(false); // normal ops

}

void RIG_K3::shutdown()
{
}

bool RIG_K3::check ()
{
	cmd = "FA;";
	int ret = wait_char(';', 14, K3_WAIT_TIME, "check", ASC);
	gett("check vfoA");

	if (ret < 14) return false;
	return true;
}

long RIG_K3::get_vfoA ()
{
	cmd = "FA;";
	int ret = wait_char(';', 14, K3_WAIT_TIME, "get vfo A", ASC);
	gett("get vfoA");

	if (ret < 14) return freqA;
	size_t p = replystr.rfind("FA");
	if (p == string::npos) return freqA;

	long f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replystr[p + n] - '0';
	freqA = f;
	return freqA;
}

void RIG_K3::set_vfoA (long freq)
{
	freqA = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(INFO, ASC, "set vfo A", cmd, replystr);
	sett("set vfoA");
}

long RIG_K3::get_vfoB ()
{
	cmd = "FB;";
	int ret = wait_char(';', 14, K3_WAIT_TIME, "get vfo B", ASC);
	gett("get vfoB");

	if (ret < 14) return freqB;
	size_t p = replystr.rfind("FB");
	if (p == string::npos) return freqB;

	long f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replystr[p + n] - '0';
	freqB = f;
	return freqB;
}

void RIG_K3::set_vfoB (long freq)
{
	if (split_on == false) {
		LOG_INFO("split on");
		return;
	}
	freqB = freq;
	cmd = "FB00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(INFO, ASC, "set vfo B", cmd, replystr);
	sett("set vfoB");
}

// Volume control
void RIG_K3::set_volume_control(int val)
{
	int ivol = (int)(val * 2.55);
	cmd = "AG000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ivol % 10;
		ivol /= 10;
	}
	sendCommand(cmd);
	showresp(INFO, ASC, "set vol", cmd, replystr);
	sett("set volume");
}

int RIG_K3::get_volume_control()
{
	cmd = "AG;";
	int ret = wait_char(';', 6, K3_WAIT_TIME, "get volume", ASC);
	gett("get volume");

	if (ret < 6) return progStatus.volume;
	size_t p = replystr.rfind("AG");
	if (p == string::npos) return 0;

	replystr[p + 5] = 0;
	int v = atoi(&replystr[p + 2]);
	return (int)(v / 2.55);
}

void RIG_K3::set_pbt_values(int val)
{
	switch (val) {
		case 0 :
		case 1 :
		case 3 :
		case 4 :
			if_shift_min = 400; if_shift_max = 2600;
			if_shift_step = 10; if_shift_mid = 1500;
			break;
		case 2 :
		case 6 :
			if_shift_min = 300; if_shift_max = 1300;
			if_shift_step = 10; if_shift_mid = 800;
			break;
		case 5 :
		case 7 :
			if_shift_min = 100; if_shift_max = 2100;
			if_shift_step = 10; if_shift_mid = 1000;
			break;
	}
	progStatus.shift_val = if_shift_mid;
	Fl::awake(adjust_if_shift_control, (void *)0);
}

void RIG_K3::set_modeA(int val)
{
	modeA = val;
	cmd = "MD0;";
	cmd[2] = modenbr[val];
	sendCommand(cmd);
	showresp(INFO, ASC, "set mode A", cmd, replystr);
	sett("set modeA");

	set_pbt_values(val);
}

int RIG_K3::get_modeA()
{
	cmd = "MD;";
	int ret = wait_char(';', 4, K3_WAIT_TIME, "get mode A", ASC);
	gett("get modeA");

	if (ret < 4) return modeA;
	size_t p = replystr.rfind("MD");
	if (p == string::npos) return modeA;
	int md = replystr[p + 2] - '1';
	if (md == 8) md--;
	if (md != modeA) set_pbt_values(md);
	return (modeA = md);
}

void RIG_K3::set_modeB(int val)
{
	if (split_on == false) {
		LOG_INFO("split on");
		return;
	}
	modeB = val;
	cmd = "MD$0;";
	cmd[3] = modenbr[val];
	sendCommand(cmd);
	showresp(INFO, ASC, "set mode B", cmd, replystr);
	sett("set modeB");

	set_pbt_values(val);
}

int RIG_K3::get_modeB()
{
	cmd = "MD$;";
	int ret = wait_char(';', 4, K3_WAIT_TIME, "get mode B", ASC);
	gett("get modeB");

	if (ret < 4) return modeB;
	size_t p = replystr.rfind("MD$");
	if (p == string::npos) return modeB;
	int md = replystr[p + 3] - '1';
	if (md == 8) md--;
	if (md != modeB) set_pbt_values(md);
	return (modeB = md);
}

int RIG_K3::get_modetype(int n)
{
	return K3_mode_type[n];
}

void RIG_K3::set_preamp(int val)
{
	if (val) sendCommand("PA1;", 0);
	else	 sendCommand("PA0;", 0);
	sett("set preamp");
}

int RIG_K3::get_preamp()
{
	cmd = "PA;";
	int ret = wait_char(';', 4, K3_WAIT_TIME, "get preamp", ASC);
	gett("get preamp");

	if (ret < 4) return progStatus.preamp;
	size_t p = replystr.rfind("PA");
	if (p == string::npos) return 0;
	return (replystr[p + 2] == '1' ? 1 : 0);
}

//
void RIG_K3::set_attenuator(int val)
{
	if (val) sendCommand("RA01;", 0);
	else	 sendCommand("RA00;", 0);
	sett("set attenuator");
}

int RIG_K3::get_attenuator()
{
	cmd = "RA;";
	int ret = wait_char(';', 5, K3_WAIT_TIME, "set ATT", ASC);
	gett("get attenuator");

	if (ret < 5) return progStatus.attenuator;
	size_t p = replystr.rfind("RA");
	if (p == string::npos) return 0;
	return (replystr[p + 3] == '1' ? 1 : 0);
}

// Transceiver power level
void RIG_K3::set_power_control(double val)
{
	int ival = val;
	cmd = "PC000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ival % 10;
		ival /= 10;
	}
	sendCommand(cmd);
	showresp(INFO, ASC, "set power ctrl", cmd, replystr);
	sett("set power control");
}

int RIG_K3::get_power_control()
{
	cmd = "PC;";
	int ret = wait_char(';', 6, K3_WAIT_TIME, "get power level", ASC);
	gett("get power control");

	if (ret < 6) return progStatus.power_level;
	size_t p = replystr.rfind("PC");
	if (p == string::npos) return progStatus.power_level;
	return fm_decimal(replystr.substr(p+2), 3);
}

void RIG_K3::get_pc_min_max_step(double &min, double &max, double &step)
{
   min = minpwr; max = maxpwr; step = steppwr;
}

// Transceiver rf control
void RIG_K3::set_rf_gain(int val)
{
	int ival = val;
	cmd = "RG000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ival % 10;
		ival /= 10;
	}
	sendCommand(cmd);
	showresp(INFO, ASC, "set rfgain ctrl", cmd, replystr);
	sett("set rf gain");
}

int RIG_K3::get_rf_gain()
{
	cmd = "RG;";
	int ret = wait_char(';', 6, K3_WAIT_TIME, "get RF gain", ASC);
	gett("get rf gain");
	if (ret < 6) return progStatus.rfgain;
	size_t p = replystr.rfind("RG");
	if (p == string::npos) return progStatus.rfgain;

	replystr[p + 5] = 0;
	int v = atoi(&replystr[p + 2]);
	return v;
}

void RIG_K3::get_rf_min_max_step(int &min, int &max, int &step)
{
   min = 0; max = 250; step = 1;
}

// Transceiver mic control
void RIG_K3::set_mic_gain(int val)
{
	int ival = (int)val;
	cmd = "MG000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ival % 10;
		ival /= 10;
	}
	sendCommand(cmd);
	showresp(INFO, ASC, "set mic ctrl", cmd, replystr);
	sett("set mic gain");
}

int RIG_K3::get_mic_gain()
{
	cmd = "MG;";
	int ret = wait_char(';', 6, K3_WAIT_TIME, "get MIC gain", ASC);
	gett("get mic gain");

	if (ret < 6) return progStatus.mic_gain;
	size_t p = replystr.rfind("MG");
	if (p == string::npos) return progStatus.mic_gain;

	replystr[p + 5] = 0;
	int v = atoi(&replystr[p + 2]);
	return v;
}

void RIG_K3::get_mic_min_max_step(int &min, int &max, int &step)
{
   min = 0; max = 60; step = 1;
}

// Tranceiver PTT on/off
void RIG_K3::set_PTT_control(int val)
{
	if (val) sendCommand("TX;", 0);
	else	 sendCommand("RX;", 0);
	ptt_ = val;
	sett("set PTT");
}

//BG (Bargraph Read; GET only)
//RSP format: BGnn; where <nn> is 00 (no bars) through 10 (bar 10) if the bargraph is in DOT mode, and 12 (no
//bars) through 22 (all 10 bars) if the bargraph is in BAR mode. Reads the S-meter level on receive. Reads the
//power output level or ALC level on transmit, depending on the RF/ALC selection. Also see SM/SM$ command,
//which can read either main or sub RX S-meter level.

//SM $ (S-meter Read; GET only)
//Basic RSP format: SMnnnn; where nnnn is 0000-0015. S9=6; S9+20=9; S9+40=12; S9+60=15.
//K3 Extended RSP format (K31): nnnn is 0000-0021. S9=9; S9+20=13; S9+40=17; S9+60=21.
//This command can be used to obtain either the main (SM) or sub (SM$) S-meter readings. Returns 0000 in transmit
//mode. BG can be used to simply emulate the bar graph level, and applies to either RX or TX mode.

int RIG_K3::get_smeter()
{
	cmd = "SM;";
	int ret = wait_char(';', 7, K3_WAIT_TIME, "get Smeter", ASC);
	gett("get smeter");

	if (ret < 7) return 0;
	size_t p = replystr.rfind("SM");
	if (p == string::npos) return 0;

	int mtr = fm_decimal(replystr.substr(p+2), 4);

	if (mtr <= 6) mtr = (int) (50.0 * mtr / 6.0);
	else mtr = (int)(50 + (mtr - 6.0) * 50.0 / 9.0);
	return mtr;
}

void RIG_K3::set_noise(bool on)
{
	if (on) sendCommand("NB1;", 0);
	else	sendCommand("NB0;", 0);
	sett("set noise blanker");
}

int RIG_K3::get_noise()
{
	cmd = "NB;";
	int ret = wait_char(';', 4, K3_WAIT_TIME, "get Noise Blanker", ASC);
	gett("get noise blanker");

	if (ret < 4) return progStatus.noise;
	size_t p = replystr.rfind("NB");
	if (p == string::npos) return progStatus.noise;
	return (replystr[p+2] == '1' ? 1 : 0);
}

// FW $ (Filter Bandwidth and Number; GET/SET)
// K3 Extended SET/RSP format (K31): FWxxxx; where xxxx is 0-9999, the bandwidth
// in 10-Hz units. May be quantized and/or range limited based on the present
// operating mode.

void RIG_K3::set_bwA(int val)
{
	cmd = "FW0000;";
	bwA = val;
	val = atoi(K3_widths[val]);
	val /= 10; cmd[5] += val % 10;
	val /= 10; cmd[4] += val % 10;
	val /= 10; cmd[3] += val % 10;
	val /= 10; cmd[2] += val % 10;
	sendCommand(cmd);
	showresp(INFO, ASC, "set bw A", cmd, replystr);
	sett("set bwA");
}

int RIG_K3::get_bwA()
{
	cmd = "FW;";
	int ret = wait_char(';', 7, K3_WAIT_TIME, "get bandwidth A", ASC);
	gett("get bwA");

	if (ret < 7) return bwA;
	size_t p = replystr.rfind("FW");
	if (p == string::npos) return bwA;
	int bw = 0;
	for (int i = 2; i < 6; i++) bw = bw * 10 + replystr[p+i] - '0';
	bw *= 10;
	for (bwA = 0; bwA < 36; bwA++)
		if (bw <= atoi(K3_widths[bwA])) break;
	return bwA;

}

void RIG_K3::set_bwB(int val)
{
	if (split_on == false) {
		LOG_INFO("split on");
		return;
	}
	cmd = "FW$0000;";
	bwA = val;
	val = atoi(K3_widths[val]);
	val /= 10; cmd[6] += val % 10;
	val /= 10; cmd[5] += val % 10;
	val /= 10; cmd[4] += val % 10;
	val /= 10; cmd[3] += val % 10;
	sendCommand(cmd);
	showresp(INFO, ASC, "set bw B", cmd, replystr);
	sett("set bwB");
}

int RIG_K3::get_bwB()
{
	cmd = "FW$;";
	int ret = wait_char(';', 8, K3_WAIT_TIME, "get bandwidth B", ASC);
	gett("get bwB");

	if (ret < 8) return bwB;
	size_t p = replystr.rfind("FW$");
	if (p == string::npos) return bwB;
	int bw = 0;
	for (int i = 3; i < 7; i++) bw = bw * 10 + replystr[p+i] - '0';
	bw *= 10;
	for (bwB = 0; bwB < 36; bwB++)
		if (bw <= atoi(K3_widths[bwB])) break;
	return bwB;
}

int RIG_K3::get_power_out()
{
	cmd = "BG;"; // responds BGnn; 0 < nn < 10
	int ret = wait_char(';', 5, K3_WAIT_TIME, "get power out", ASC);
	gett("get power out");

	if (ret < 5) return 0;
	size_t p = replystr.rfind("BG");
	if (p == string::npos) return 0;
	replystr[p + 4] = 0;
	int mtr = atoi(&replystr[p + 2]) * 10;
	if (mtr > 100) mtr = 100;
	return mtr;
}

bool RIG_K3::can_split()
{
	return true;
}

void RIG_K3::set_split(bool val)
{
	if (val) {
		cmd = "FT1;";
		sendCommand(cmd);
		showresp(INFO, ASC, "set split ON", cmd, replystr);
		sett("set split ON");
	} else {
		cmd = "FR0;";
		sendCommand(cmd);
		showresp(INFO, ASC, "set split OFF", cmd, replystr);
		sett("set split OFF");
	}
	split_on = val;
}

// 01234567890123456789012345678901234567
// 0         1         2         3      7
// IF00014070000*****+yyyyrx*00tmvspbd1*;
//   |---------|                   |
//     vfo a/b                     split on = '1', off = '0'
// IF00014070000     -000000 0002000011 ;  OFF
// IF00014070000     -000000 0002001011 ;  ON

int RIG_K3::get_split()
{
	cmd = "IF;";
	int ret = wait_char(';', 38, K3_WAIT_TIME, "get split", ASC);
	gett("get split");

	if (ret < 38) return split_on;
	size_t p = replystr.rfind("IF");
	if (p == string::npos) return split_on;
	split_on = replystr[p+32] - '0';
	return split_on;
}

void RIG_K3::set_if_shift(int val)
{
	cmd = "IS 0000;";
	cmd[6] += val % 10; val /= 10;
	cmd[5] += val % 10; val /= 10;
	cmd[4] += val % 10; val /= 10;
	cmd[3] += val % 10;
	sendCommand(cmd);
	showresp(INFO, ASC, "set if shift", cmd, replystr);
	sett("set if shift");
}

bool RIG_K3::get_if_shift(int &val)
{
	cmd = "IS;";
	int ret = wait_char(';', 8, K3_WAIT_TIME, "get IF shift", ASC);
	gett("get if shift");

	val = progStatus.shift_val;
	if (ret < 8) return progStatus.shift;
	size_t p = replystr.rfind("IS ");
	if (p == string::npos) return progStatus.shift;
	sscanf(&replystr[p + 3], "%d", &progStatus.shift_val);
	val = progStatus.shift_val;
	if (val == if_shift_mid) progStatus.shift = false;
	else progStatus.shift = true;
	return progStatus.shift;
}

void RIG_K3::get_if_min_max_step(int &min, int &max, int &step)
{
	min = if_shift_min; max = if_shift_max; step = if_shift_step;
}

void  RIG_K3::get_if_mid()
{
	cmd = "IS 9999;";
	sendCommand(cmd);
	waitResponse(500);
	showresp(INFO, ASC, "center pbt", cmd, replystr);
	sett("set center pbt");

	cmd = "IS;";
	int ret = wait_char(';', 8, K3_WAIT_TIME, "get PBT center", ASC);
	gett("get center pbt");

	if (ret < 8) return;
	size_t p = replystr.rfind("IS ");
	if (p == string::npos) return;
	sscanf(&replystr[p + 3], "%d", &if_shift_mid);
}
