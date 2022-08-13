// ----------------------------------------------------------------------------
// Copyright (C) 2022
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
// aunsigned long int with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include "elecraft/K4.h"
#include "status.h"

#include "support.h"

const char K4name_[] = "K4";

// 0=N/A, 1=LSB, 2=USB, 3=CW, 4=FM, 5=AM, 6=DATA, 7=CW REV, 8=N/A, 9=DATA REV.
const char *K4modes_[] =
	{ "LSB", "USB", "CW", "FM", "AM", "DATA", "CW-R", "DATA-R", NULL};
const char modenbr[] =
	{ '1', '2', '3', '4', '5', '6', '7', '9' };
static const char K4_mode_type[] =
	{ 'L', 'U', 'L', 'U', 'U', 'U', 'U', 'L' };

static const char *K4_widths[] = {
   "50",  "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
  "550",  "600",  "650",  "700",  "750",  "800",  "850",  "900",  "950", "1000",
 "1100", "1200", "1300", "1400", "1500", "1600", "1700", "1800", "1900", "2000",
 "2200", "2400", "2600", "2800", "3000", "3100", "3200", "3300", "3400", "3500",
 "3600", "3700", "3800", "3900", "4000", NULL};
static int K4_bw_vals[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38, 39,40,
41,42,43,44,45, WVALS_LIMIT};

static int def_mode_width[] = { 34, 34, 15, 37, 37, 34, 15, 34 };

static GUI k4_widgets[]= {
	{ (Fl_Widget *)btnVol, 2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME, 54, 125, 156 },
	{ (Fl_Widget *)sldrRFGAIN, 54, 145, 156 },
	{ (Fl_Widget *)btnIFsh, 214, 105,  50 },
	{ (Fl_Widget *)sldrIFSHIFT, 266, 105, 156 },
	{ (Fl_Widget *)sldrMICGAIN, 266, 125, 156 },
	{ (Fl_Widget *)sldrPOWER, 266, 145, 156 },
	{ (Fl_Widget *)NULL, 0, 0, 0 }
};

static int agcval = 1;
static int agcvalA = 1;
static int agcvalB = 1;


int RIG_K4::power_scale()
{
	return powerScale;
}

RIG_K4::RIG_K4() {
// base class values
	name_ = K4name_;
	modes_ = K4modes_;
	bandwidths_ = K4_widths;
	bw_vals_ = K4_bw_vals;

	comm_baudrate = BR38400;

	widgets = k4_widgets;

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
	has_rf_control = true;
	has_bandwidth_control =
	has_power_control = true;
	has_volume_control =
	has_mode_control =
	has_ptt_control =
	has_noise_control =
	has_attenuator_control =
	has_smeter =
	has_power_out = true;
	has_split =
	has_ifshift_control =
	has_preamp_control = true;
	has_agc_control = true;

	has_notch_control =
	has_tune_control =
	has_swr_control = false;

	if_shift_min = 300;
	if_shift_max = 3000;
	if_shift_step = 10;
	if_shift_mid = 300;

	precision = 1;
	ndigits = 8;

}

int  RIG_K4::adjust_bandwidth(int m)
{
	return def_mode_width[m];
}

int  RIG_K4::def_bandwidth(int m)
{
	return def_mode_width[m];
}

#define K4_WAIT_TIME 800

void RIG_K4::initialize()
{
	debug::level = debug::INFO_LEVEL;

	LOG_INFO("K4");
	k4_widgets[0].W = btnVol;
	k4_widgets[1].W = sldrVOLUME;
	k4_widgets[2].W = sldrRFGAIN;
	k4_widgets[3].W = btnIFsh;
	k4_widgets[4].W = sldrIFSHIFT;
	k4_widgets[5].W = sldrMICGAIN;
	k4_widgets[6].W = sldrPOWER;

	powerScale = 1;

	cmd = "AI0;"; // disable auto-info
	set_trace(1, "disable auto info");
	sendCommand(cmd);
	sett("");

	cmd = "K41;"; // K4 advanced mode
	set_trace(1, "set K4 advanced mode");
	sendCommand(cmd);
	sett("");

//	cmd = "SWT49;"; // Fine tuning (1 Hz mode)
//	set_trace(1, "set 1 Hz fine tuning");
//	sendCommand(cmd);
//	sett("");

	cmd = "OM;"; // request options to get power level
	get_trace(1, "get options");
	wait_char(';', 16, K4_WAIT_TIME, "Options", ASC);
	gett("");

	if (replystr.rfind("OM") != std::string::npos) {
		if (replystr.rfind("P") == std::string::npos) { // KPA4 present
			minpwr = 0.0;
			maxpwr = 10.0;
			steppwr = 0.1;
		} else {
			minpwr = 0.0;
			maxpwr = 110.0;
			steppwr = 1.0;
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

void RIG_K4::shutdown()
{
}

bool RIG_K4::check ()
{
	return true;
	cmd = "FA;";
	get_trace(1, "check vfoA");
	int ret = wait_char(';', 14, 2000, "check", ASC);
	gett("");

	if (ret < 14) return false;
	return true;
}

unsigned long int RIG_K4::get_vfoA ()
{
	cmd = "FA;";
	get_trace(1, "get vfoA");
	wait_char(';', 14, K4_WAIT_TIME, "get vfo A", ASC);
	gett("");

	size_t p = replystr.rfind("FA");
	if (p == std::string::npos) return freqA;

	unsigned long int f = 0;
	for (size_t n = p + 2; n < replystr.length() - 1; n++)
		f = f*10 + replystr[n] - '0';
	freqA = f;
	return freqA;
}

void RIG_K4::set_vfoA (unsigned long int freq)
{
	freqA = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	set_trace(1, "set vfoA");
	sendCommand(cmd);
	sett("");
}

unsigned long int RIG_K4::get_vfoB ()
{
	cmd = "FB;";
	get_trace(1, "get vfoB");
	wait_char(';', 14, K4_WAIT_TIME, "get vfo B", ASC);
	gett("");

	size_t p = replystr.rfind("FB");
	if (p == std::string::npos) return freqB;

	unsigned long int f = 0;
	for (size_t n = p + 2; n < replystr.length() - 1; n++)
		f = f*10 + replystr[n] - '0';
	freqB = f;
	return freqB;
}

void RIG_K4::set_vfoB (unsigned long int freq)
{
	freqB = freq;
	cmd = "FB00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	set_trace(1, "set vfoB");
	sendCommand(cmd);
	sett("");
}

// Volume control
void RIG_K4::set_volume_control(int val)
{
	if (isOnA()) {
		cmd = "AG000;";
		cmd[4] += val % 10; val /= 10;
		cmd[3] += val % 10; val /= 10;
		cmd[2] += val % 10;

		set_trace(1, "set volume");
		sendCommand(cmd);
		sett("");
	} else {
		cmd = "AG$000;";
		cmd[5] += val % 10; val /= 10;
		cmd[4] += val % 10; val /= 10;
		cmd[3] += val % 10;

		set_trace(1, "set volume");
		sendCommand(cmd);
		sett("");
	}
}

int RIG_K4::get_volume_control()
{
	int v;

	if (isOnA()) {
		cmd = "AG;";
		get_trace(1, "get volume control");
		wait_char(';', 7, K4_WAIT_TIME, "get volume", ASC);
		gett("");

		size_t p = replystr.rfind("AG");
		if (p == std::string::npos) return 0;

		v = atoi(&replystr[p + 3]);
	} else {
		cmd = "AG$;";
		get_trace(1, "get volume control");
		wait_char(';', 8, K4_WAIT_TIME, "get volume", ASC);
		gett("");

		size_t p = replystr.rfind("AG$");
		if (p == std::string::npos) return 0;

		v = atoi(&replystr[p + 4]);
	}
	return v;
}

void RIG_K4::get_vol_min_max_step(double &min, double &max, double &step)
{
	min = 0; max = 60; step = 1;
}


//----------------------------------------------------------------------
static const char *agcstrs[] = {"AGC", "AG-S", "AG-F"};

const char *RIG_K4::agc_label()
{
	if (agcval >= 0 && agcval <= 2)
		return agcstrs[agcval];
	return "AGC";
}


int  RIG_K4::get_agc()
{
	if (isOnA()) {
		cmd = "GT;";
		wait_char(';', 4, 100, "get AGC", ASC);
		gett("get agc");
		size_t p = replystr.rfind("GT");
		if (p == std::string::npos) return agcvalA;
		switch (replystr[p+2]) {
			default:
			case '0': nb_label("AGC", false); agcvalA = 0; break;
			case '1': nb_label("AG-S", true); agcvalA = 1; break;
			case '2': nb_label("AG-F", true); agcvalA = 2; break;
		}
		agcval=agcvalA;
	} else {
		cmd = "GT$;";
		wait_char(';', 5, 100, "get AGC", ASC);
		gett("get agc");
		size_t p = replystr.rfind("GT");
		if (p == std::string::npos) return agcvalB;
		switch (replystr[p+3]) {
			default:
			case '0': nb_label("AGC", false); agcvalB = 0; break;
			case '1': nb_label("AG-S", true); agcvalB = 1; break;
			case '2': nb_label("AG-F", true); agcvalB = 2; break;
		}
		agcval=agcvalB;
	}
	if (agcval == 0) agcvalA = agcvalB = 0;

	return agcval;
}


void  RIG_K4::set_agc_level(int val)
{
	if (isOnA()) {
		agcvalA = val;
		cmd = "GT0;";
		switch (val) {
			default:
			case '0': nb_label("AGC", false); cmd[2] = '0'; break;
			case '1': nb_label("AG-S", true); cmd[2] = '1'; break;
			case '2': nb_label("AG-F", true); cmd[2] = '2'; break;
		}
		set_trace(1, "set agc");
		sendCommand(cmd);
		sett("");
	} else {
		agcvalB = val;
		cmd = "GT$0;";
		switch (val) {
			default:
			case '0': nb_label("AGC", false); cmd[3] = '0'; break;
			case '1': nb_label("AG-S", true); cmd[3] = '1'; break;
			case '2': nb_label("AG-F", true); cmd[3] = '2'; break;
		}
		set_trace(1, "set agc");
		sendCommand(cmd);
		sett("");
	}
	return;
}


// K4 agc control is WEIRD. Turning off AGC on either A or B
// results it AGC OFF for BOTH. 
// Turning on AGC Fast for either one turns on AGC Slow for the other one.
//   when starting from OFF, but if the Other is ON then it's fast or slow
//   setting remains.
// Turning on AGC SLow for either one Turns on AGC Slow for the other one.
// This is gross...

// Make the AGC button toggle between off and Slow.  Set fast with
//   on screen menus.

int RIG_K4::incr_agc()
{
	static const char ch[] = {'0', '1', '2'};

	if (isOnA()) {
	        agcvalA++;
	        if (agcvalA > 1) agcvalA = agcvalB = 0;
	        cmd = "GT0;";
	     	cmd[2] = ch[agcvalA];

	        sendCommand(cmd);
	        showresp(WARN, ASC, "SET agc", cmd, replystr);
	        sett("set_agc");
		agcval = agcvalA;
		return agcvalA;
	} else {
	        agcvalB++;
	        if (agcvalB > 1) agcvalA = agcvalB = 0;
	        cmd = "GT$0;";
	     	cmd[3] = ch[agcvalB];

	        sendCommand(cmd);
	        showresp(WARN, ASC, "SET agc", cmd, replystr);
	        sett("set_agc");
		agcval = agcvalB;
		return agcvalB;
	}
}

void RIG_K4::set_modeA(int val)
{
	modeA = val;
	cmd = "MD0;";
	cmd[2] = modenbr[val];
	set_trace(1, "set modeA");
	sendCommand(cmd);
	sett("");

	set_pbt_values(val);
}

int RIG_K4::get_modeA()
{
	cmd = "MD;";
	get_trace(1, "get modeA");
	wait_char(';', 4, K4_WAIT_TIME, "get mode A", ASC);
	gett("");

	size_t p = replystr.rfind("MD");
	if (replystr[p+2] == '0' || replystr[p+2] == '8') return modeA;
	if (p == std::string::npos) return modeA;

	int md = replystr[p + 2] - '1';
	if (md == 8) md--;
	if (md != modeA) set_pbt_values(md);
	return (modeA = md);
}

void RIG_K4::set_modeB(int val)
{
	modeB = val;
	cmd = "MD$0;";
	cmd[3] = modenbr[val];
	set_trace(1, "set modeB");
	sendCommand(cmd);
	sett("");

	set_pbt_values(val);
}

int RIG_K4::get_modeB()
{
	cmd = "MD$;";
	get_trace(1, "get modeB");
	wait_char(';', 4, K4_WAIT_TIME, "get mode B", ASC);
	gett("");

	size_t p = replystr.rfind("MD$");
	if (replystr[p+3] == '0' || replystr[p+3] == '8') return modeB;
	if (p == std::string::npos) return modeB;

	int md = replystr[p + 3] - '1';
	if (md == 8) md--;
	if (md != modeB) set_pbt_values(md);
	return (modeB = md);
}

int RIG_K4::get_modetype(int n)
{
	return K4_mode_type[n];
}

/*
PAn;
n = 0 (off)
n = 1 (10 dB regular preamp)
n = 2 (160-15 m: 18 dB regular preamp; 12-6 m: 20 dB LNA)
n = 3 (12-6 m only: 10 dB regular preamp + 20 dB LNA)
*/
void RIG_K4::set_preamp(int val)
{
	set_trace(1, "set preamp");
	if (val) sendCommand("PA1;", 0);
	else	 sendCommand("PA0;", 0);
	sett("");
}

int RIG_K4::get_preamp()
{
	cmd = "PA;";
	get_trace(1, "get_preamp");
	wait_char(';', 4, K4_WAIT_TIME, "get preamp", ASC);
	gett("");

	size_t p = replystr.rfind("PA");
	if (p == std::string::npos) return progStatus.preamp;
	return progStatus.preamp = (replystr[p + 2] == '1' ? 1 : 0);
}


void K4_atten_label(int val)
{
	switch (val) {
		default:
		case 0: atten_label("OFF", false); break;
		case 1: atten_label("3 db", true); break;
		case 2: atten_label("6 db", true); break;
		case 3: atten_label("9 db", true); break;
		case 4: atten_label("12 db", true); break;
		case 5: atten_label("15 db", true); break;
		case 6: atten_label("18 db", true); break;
		case 7: atten_label("21 db", true); break;
	}
}

int RIG_K4::next_attenuator()
{
	return 0;
}

/*
  RA$nnm; where nn is 0/3/6/9/12/15/18/21 (dB) and m = 0 (off), 1 (on)
*/
void RIG_K4::set_attenuator(int val)
{
	sett("set_attenuator(int val)");

	cmd = "RA";
	switch (val) {
		case 0: cmd.append("000;"); break;
		case 1: cmd.append("031;");break;
		case 2: cmd.append("061;"); break;
		case 3: cmd.append("091;"); break;
		case 4: cmd.append("121;"); break;
		case 5: cmd.append("151;"); break;
		case 6: cmd.append("181;"); break;
		case 7: cmd.append("211;"); break;
	}
	set_trace(1, "set attenuator");
	sendCommand(cmd);
	sett("");
	K4_atten_label(val);
}

int RIG_K4::get_attenuator()
{
	cmd = "RA;";
	get_trace(1, "get attenuator");
	wait_char(';', 5, K4_WAIT_TIME, "set ATT", ASC);
	gett("");

	size_t p = replystr.rfind("RA");
	if (p == std::string::npos) return 0;

	int val;
	val = (replystr[p+2] - '0')*10 + replystr[p+3] - '0';

	switch (val) {
		default:
		case 0: atten_level = 0; break;
		case 3: atten_level = 1; break;
		case 6: atten_level = 2; break;
		case 9: atten_level = 3; break;
		case 12: atten_level = 4; break;
		case 15: atten_level = 5; break;
		case 18: atten_level = 6; break;
		case 21: atten_level = 7; break;
	}
	K4_atten_label(atten_level);
	return atten_level;
}

// Transceiver power level
void RIG_K4::set_power_control(double val)
{
	int ival = val;
	cmd = "PC000";
        if (ival > 10) {
		for (int i = 4; i > 1; i--) {
			cmd[i] += ival % 10;
			ival /= 10;
		}
	} else {
		for (int i = 3; i > 1; i--) {
			cmd[i] += ival % 10;
			ival /= 10;
		}
	}

	if ((int)val > 10) cmd.append("H;");
	else cmd.append("L;");
	set_trace(1, "set power control");
	sendCommand(cmd);
	showresp(INFO, ASC, "set power ctrl", cmd, replystr);
	sett("");
}

double RIG_K4::get_power_control()
{
        cmd = "PCX;";
        get_trace(1, "get power control");
        wait_char(';', 8, K4_WAIT_TIME, "get power level", ASC);
        gett("");

        size_t p = replystr.rfind("PCX");
        if (p == std::string::npos) return progStatus.power_level;
        int level = fm_decimal(replystr.substr(p+3), 4);
        if (replystr[5] == 'L') {
                powerScale = 10;
                return level / 10.0;
        }
        powerScale = 1;
        return level;
}

void RIG_K4::get_pc_min_max_step(double &min, double &max, double &step)
{
	cmd = "OM;"; // request options to get power level
	get_trace(1, "get options/pc_pwr_level");
	wait_char(';', 16, K4_WAIT_TIME, "Options", ASC);
	gett("");

	minpwr = 0.0; maxpwr = 100.0; steppwr = 1.0;
	if (replystr.rfind("OM") == std::string::npos) return;

	if (replystr.find("P") == std::string::npos) {
		minpwr = 0.0;
		maxpwr = 10.0;
		steppwr = 0.1;
	} else {
		minpwr = 0.0;
		maxpwr = 110.0;
		steppwr = 1.0;
	}

	min = minpwr; max = maxpwr; step = steppwr;
}

// Transceiver rf control
void RIG_K4::set_rf_gain(int val)
{
	if (isOnA()) {
		int ival = abs(val);
		cmd = "RG-00;";
		cmd[3] += ival / 10;
		cmd[4] += ival % 10;

		set_trace(1, "set rf gain");
		sendCommand(cmd);
		sett("");
	} else {
		int ival = abs(val);
		cmd = "RG$-00;";
		cmd[4] += ival / 10;
		cmd[5] += ival % 10;

		set_trace(1, "set rf gain");
		sendCommand(cmd);
		sett("");
	}
}

int RIG_K4::get_rf_gain()
{
	int v;

	if (isOnA()) {
		cmd = "RG;";
		get_trace(1, "get rf gain");
		wait_char(';', 6, K4_WAIT_TIME, "get RF gain", ASC);
		gett("");

		size_t p = replystr.rfind("RG");
		if (p == std::string::npos) return progStatus.rfgain;

		replystr[p + 5] = 0;
		v = atoi(&replystr[p + 2]);
	} else {
		cmd = "RG$;";
		get_trace(1, "get rf gain");
		wait_char(';', 7, K4_WAIT_TIME, "get RF gain", ASC);
		gett("");

		size_t p = replystr.rfind("RG$");
		if (p == std::string::npos) return progStatus.rfgain;

		replystr[p + 6] = 0;
		v = atoi(&replystr[p + 3]);
	}
	return v;
}

void RIG_K4::get_rf_min_max_step(int &min, int &max, int &step)
{
   min = 0; max = -60; step = 1;
}

// Transceiver mic control
void RIG_K4::set_mic_gain(int val)
{
	cmd = "MG000;";
	cmd[4] += val % 10; val /= 10;
	cmd[3] += val % 10; val /= 10;
	cmd[2] += val % 10;

	set_trace(1, "set mic gain");
	sendCommand(cmd);
	sett("");
}

int RIG_K4::get_mic_gain()
{
	cmd = "MG;";
	get_trace(1, "get mic gain");
	wait_char(';', 6, K4_WAIT_TIME, "get MIC gain", ASC);
	gett("");

	size_t p = replystr.rfind("MG");
	if (p == std::string::npos) return progStatus.mic_gain;

	replystr[p + 5] = 0;
	int v = atoi(&replystr[p + 2]);
	return v;
}

void RIG_K4::get_mic_min_max_step(int &min, int &max, int &step)
{
   min = 0; max = 80; step = 1;
}

// Tranceiver PTT on/off
void RIG_K4::set_PTT_control(int val)
{
	set_trace(1, "set PTT");
	if (val) sendCommand("TX;", 0);
	else	 sendCommand("RX;", 0);
	sett("");
	ptt_ = val;
}

int RIG_K4::get_PTT()
{
	cmd = "TQ;";
	get_trace(1, "get PTT");
	wait_char(';', 4, K4_WAIT_TIME, "get PTT", ASC);
	gett("");

	size_t p = replystr.rfind("TQ");
	if (p == std::string::npos) return 0;

	if (fm_decimal(replystr.substr(p+2), 1) == 1)
           ptt_ = true;
        else
           ptt_ = false;

        return ptt_;
}

//SM $ (S-meter Read; GET only)
// SM$nn; where nn is 00-42. S9=17; S9+20=25; S9+40=33; S9+60=42.

int RIG_K4::get_smeter()
{
	cmd = "SMH;";
	get_trace(1, "get smeter");
	wait_char(';', 8, K4_WAIT_TIME, "get Smeter", ASC);
	gett("");

	size_t p = replystr.rfind("SMH");
	if (p == std::string::npos) return 0;

	int mtr = fm_decimal(replystr.substr(p+4), 3);
	mtr = (mtr - 150);

//	if (mtr <= 9) mtr = (int) (50.0 * mtr / 9.0);
//	else mtr = (int)(50 + (mtr - 9.0) * 50.0 / 12.0);

	return mtr;
}

void RIG_K4::set_noise(bool on)
{
	if (on) sendCommand("NB1;", 0);
	else	sendCommand("NB0;", 0);
	sett("set noise blanker");
}

int RIG_K4::get_noise()
{
	cmd = "NB;";
	wait_char(';', 4, K4_WAIT_TIME, "get Noise Blanker", ASC);
	gett("get noise blanker");

	size_t p = replystr.rfind("NB");
	if (p == std::string::npos) return progStatus.noise;
	return (replystr[p+2] == '1' ? 1 : 0);
}

// FW $ (Filter Bandwidth and Number; GET/SET)
// K4 Extended SET/RSP format (K41): FWxxxx; where xxxx is 0-9999, the bandwidth
// in 10-Hz units. May be quantized and/or range limited based on the present
// operating mode.

void RIG_K4::set_bwA(int val)
{
	cmd = "BW";
	bwA = val;
	std::string w = K4_widths[val];
	w.erase(w.length() - 1);
	while (w.length() < 4) w.insert(0, "0");
	cmd.append(w).append(";");

	set_trace(1, "set bwA");
	sendCommand(cmd);
	sett("");
	set_pbt_values(val);

}

int RIG_K4::get_bwA()
{
	cmd = "BW;";
	get_trace(1, "get bwA");
	wait_char(';', 7, K4_WAIT_TIME, "get bandwidth A", ASC);
	gett("");

	size_t p = replystr.rfind("BW");
	if (p == std::string::npos) return bwA;

	std::string w = replystr.substr(2);
	w[w.length() -1] = '0';
	while (w[0] == '0') w.erase(0, 1);

	bwA = 0;
	while (K4_widths[bwA] != NULL && w != K4_widths[bwA]) bwA++;
	if (K4_widths[bwA] == NULL) bwA = 0;

	return bwA;

}

void RIG_K4::set_bwB(int val)
{
	cmd = "BW$";
	bwB = val;
	std::string w = K4_widths[val];
	w.erase(w.length() - 1);
	while (w.length() < 4) w.insert(0, "0");
	cmd.append(w).append(";");

	set_trace(1, "set bwB");
	sendCommand(cmd);
	sett("");
	set_pbt_values(val);

}

int RIG_K4::get_bwB()
{
	cmd = "BW$;";
	get_trace(1, "get bwB");
	wait_char(';', 8, K4_WAIT_TIME, "get bandwidth B", ASC);
	gett("");

	size_t p = replystr.rfind("BW$");
	if (p == std::string::npos) return bwB;
	std::string w = replystr.substr(3);
	w[w.length() -1] = '0';
	while (w[0] == '0') w.erase(0, 1);

	bwB = 0;
	while (K4_widths[bwB] != NULL && w != K4_widths[bwB]) bwB++;
	if (K4_widths[bwB] == NULL) bwB = 0;

	return bwB;

}

int RIG_K4::get_power_out()
{
        cmd = "TM;";
        get_trace(1, "get power out");
        wait_char(';', 15, K4_WAIT_TIME, "get power out", ASC);
        gett("");

        size_t p = replystr.rfind("TM");
        if (p == std::string::npos) return progStatus.power_level;
        int level = fm_decimal(replystr.substr(p+8), 3);
        return level;
}

bool RIG_K4::can_split()
{
	return true;
}

void RIG_K4::set_split(bool val)
{
	set_trace(1, "set split");
	if (val) {
		cmd = "FT1;";
		sendCommand(cmd);
	} else {
		cmd = "FT0;";
		sendCommand(cmd);
	}
	sett("");

	split_on = val;
}

// 01234567890123456789012345678901234567
// 0         1         2         3      7
// IF00014070000*****+yyyyrx*00tmvspbd1*;
//   |---------|                   |
//     vfo a/b                     split on = '1', off = '0'
// IF00014070000     -000000 0002000011 ;  OFF
// IF00014070000     -000000 0002001011 ;  ON

int RIG_K4::get_split()
{
	cmd = "IF;";
	get_trace(1, "get split");
	wait_char(';', 38, K4_WAIT_TIME, "get split", ASC);
	gett("");

	size_t p = replystr.rfind("IF");
	if (p == std::string::npos) return split_on;
	split_on = replystr[p+32] - '0';
	return split_on;
}

void RIG_K4::set_pbt_values(int val)
{
	switch (val) {
		case 0 :
		case 1 :
		case 3 :
		case 4 :
			if_shift_min = 300; if_shift_max = 3000;
			if_shift_step = 10; if_shift_mid = 300;
			break;
		case 2 :
		case 6 :
			if_shift_min = 300; if_shift_max = 2000;
			if_shift_step = 10; if_shift_mid = 300;
			break;
		case 5 :
		case 7 :
			if_shift_min = 300; if_shift_max = 3000;
			if_shift_step = 10; if_shift_mid = 300;
			break;
	}

	progStatus.shift_val = if_shift_mid;
	Fl::awake(adjust_if_shift_control, (void *)0);
}

void RIG_K4::set_if_shift(int val)
{
        if (isOnA()) {
		cmd = "IS0000;";
        	val /= 10;
		cmd[5] += val % 10; val /= 10;
		cmd[4] += val % 10; val /= 10;
		cmd[3] += val % 10; val /= 10;
		cmd[2] += val % 10;
	} else {
		cmd = "IS$0000;";
        	val /= 10;
		cmd[6] += val % 10; val /= 10;
		cmd[5] += val % 10; val /= 10;
		cmd[4] += val % 10; val /= 10;
		cmd[3] += val % 10;
	}
	set_trace(1, "set if shift");
	sendCommand(cmd);
	sett("");
}

bool RIG_K4::get_if_shift(int &val)
{
	if (isOnA()) {
		cmd = "IS;";
		get_trace(1, "get if shift");
		wait_char(';', 7, K4_WAIT_TIME, "get IF shift", ASC);
		gett("");
		size_t p = replystr.rfind("IS");
		if (p == std::string::npos) return progStatus.shift;
		sscanf(&replystr[p + 3], "%d", &progStatus.shift_val);
		val = progStatus.shift_val*10;
	} else {
		cmd = "IS$;";
		get_trace(1, "get if shift");
		wait_char(';', 8, K4_WAIT_TIME, "get IF shift", ASC);
		gett("");
		size_t p = replystr.rfind("IS$");
		if (p == std::string::npos) return progStatus.shift;
		sscanf(&replystr[p + 4], "%d", &progStatus.shift_val);
		val = progStatus.shift_val*10;
	}
	if (val == if_shift_mid) progStatus.shift = false;
	else progStatus.shift = true;
	return progStatus.shift;
}

void RIG_K4::get_if_min_max_step(int &min, int &max, int &step)
{
	min = if_shift_min; max = if_shift_max; step = if_shift_step;
}

void  RIG_K4::get_if_mid()
{
	cmd = "IS;";
	get_trace(1, "get center pbt");
	wait_char(';', 8, 500, "get PBT center", ASC);
	gett("");

	size_t p = replystr.rfind("IS ");
	if (p == std::string::npos) return;
	sscanf(&replystr[p + 3], "%d", &if_shift_mid);
}

void RIG_K4::selectA()
{
        if (!isOnA()) {
	        cmd = "SW83;SW44;";
	        set_trace(1, "selectA");
	        sendCommand(cmd);
	        sett("");

//	        K4split = false;
	        showresp(WARN, ASC, "select A", cmd, replystr);
	        inuse = onA;
	}
}

void RIG_K4::selectB()
{
	if (!isOnB()) {
	        cmd = "SW83;SW44;";
		set_trace(1, "selectB");
	        sendCommand(cmd);
	        sett("");

//	        K4split = false;
	        showresp(WARN, ASC, "select B", cmd, replystr);
	        inuse = onB;
	}
}

