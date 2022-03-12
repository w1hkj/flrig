// ----------------------------------------------------------------------------
// Copyright (C) 2021
//              David Freese, W1HKJ
// Code updates by
//              Harry McGavran, W5PNY
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

#include "tentec/TT566.h"
#include "rigbase.h"

#include "support.h"

//=============================================================================
// TT-566

#define LCW 3
#define UCW 2

static int cur_modeA = 0;
static int cur_modeB = 0;

const char RIG_TT566name_[] = "Orion-II";

const char *RIG_TT566modes_[] = {
		"USB", "LSB", "UCW", "LCW", "AM", "FM", "FSK", NULL};
static const char RIG_TT566_mode_type[] = {'U', 'L', 'U', 'L', 'U', 'U', 'L'};

const char *RIG_TT566widths[] = { 
"100",  "200",  "300",  "400",  "500", "600",  "700",  "800",  "900",  "1000",
"1200", "1400", "1600", "1800", "2000", "2200", "2400", "2600", "2800", "3000",
"3200", "3400", "3600", "3800", "4000", "4500", "5000", "5500", "6000",
NULL};
static int RIG_TT566_bw_vals[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,
WVALS_LIMIT};

static char TT566setFREQa[]		= "*AF"; // binary data
static char TT566getFREQa[]		= "?AF\r";
static char TT566rspFREQa[]		= "@AF";

static char TT566setFREQb[]		= "*BF";
static char TT566getFREQb[]		= "?BF\r";
static char TT566rspFREQb[]		= "@BF";

static char TT566setMODEa[]		= "*RMM";
static char TT566getMODEa[]		= "?RMM\r";
static char TT566rspMODEa[]		= "@RMM";

static char TT566setMODEb[]		= "*RSM";
static char TT566getMODEb[]		= "?RSM\r";
static char TT566rspMODEb[]		= "@RSM";

static char TT566setBWa[]		= "*RMF";
static char TT566getBWa[]		= "?RMF\r";
static char TT566rspBWa[]		= "@RMF";

static char TT566setBWb[]		= "*RSF";
static char TT566getBWb[]		= "?RSF\r";
static char TT566rspBWb[]		= "@RSF";

static char TT566setVOLa[]		= "*UM";
static char TT566getVOLa[]		= "?UM\r";
static char TT566rspVOLa[]		= "@UM";

static char TT566setRFGa[]		= "*RMG";
static char TT566getRFGa[]		= "?RMG\r";
static char TT566rspRFGa[]		= "@RMG";

static char TT566setATTa[]		= "*RMT";
static char TT566getATTa[]		= "?RMT\r";
static char TT566rspATTa[]		= "@RMT";

static char TT566setANa[]		= "*RMNA";

static char TT566setNBa[]		= "*RMNB";
static char TT566getNBa[]		= "?RMNB\r";
static char TT566rspNBa[]		= "@RMNB";

static char TT566setMIC[]		= "*TM";
static char TT566getMIC[]		= "?TM\r";
static char TT566rspMIC[]		= "@TM";

static char TT566setPWR[]		= "*TP";
static char TT566getPWR[]		= "?TP\r";
static char TT566rspPWR[]		= "@TP";

static char TT566getSMETER[]	= "?S\r";
static char TT566rspSMETER[]	= "@SRM";
static char TT566rspPOUT[]		= "@STF";

static char TT566setPREAMP[]	= "*RME"; // ONLY AVAILABLE ON PRIMARY RX
static char TT566getPREAMP[]	= "?RME\r";
static char TT566rspPREAMP[]	= "@RME";

static char TT566setPTT[]		= "*T";

//static char TT566setVOLb[]		= "*US";
//static char TT566getVOLb[]		= "?US\r";
//static char TT566rspVOLb[]		= "@US";

//static char TT566setPBTa[]		= "*RMP";
//static char TT566getPBTa[]		= "?RMP\r";
//static char TT566rspPBTa[]		= "@RMP";

//static char TT566setPBTb[]		= "*RSP";
//static char TT566getPBTb[]		= "?RSP\r";
//static char TT566rspPBTb[]		= "@RSP";

//static char TT566setAGCa[]		= "*RMA";
//static char TT566getAGCa[]		= "?RMA\r";
//static char TT566rspAGCa[]		= "@RMA";

//static char TT566setAGCb[]		= "*RSA";
//static char TT566getAGCb[]		= "?RSA\r";
//static char TT566rspAGCb[]		= "@RSA";

//static char TT566setRFGb[]		= "*RSG";
//static char TT566getRFGb[]		= "?RSG\r";
//static char TT566rspRFGb[]		= "@RSG";

//static char TT566setATTb[]		= "*RST";
//static char TT566getATTb[]		= "?RST\r";
//static char TT566rspATTb[]		= "@RST";

//static char TT566setSQLa[]		= "*RMS";
//static char TT566getSQLa[]		= "?RMS\r";
//static char TT566rspSQLa[]		= "@RMS";

//static char TT566setSQLb[]		= "*RSS";
//static char TT566getSQLb[]		= "?RSS\r";
//static char TT566rspSQLb[]		= "@RSS";

//static char TT566setANb[]		= "*RSNA";
//static char TT566getANb[]		= "?RSNA\r";
//static char TT566rspANb[]		= "@RSNA";

//static char TT566setNBb[]		= "*RSNB";
//static char TT566getNBb[]		= "?RSNB\r";
//static char TT566rspNBb[]		= "@RSNB";

//static char TT566setTBW[]		= "*TF";
//static char TT566getTBW[]		= "?TF\r";
//static char TT566rspTBW[]		= "@TF";

//static char TT566setTUNE[]		= "*TT";
//static char TT566getTUNE[]		= "?TT\r";
//static char TT566rspTUNE[]		= "@TT";

//static char TT566setRITa[]		= "*RMR";
//static char TT566getRITa[]		= "?RMR\r";
//static char TT566rspRITa[]		= "@RMR";

//static char TT566setRITb[]		= "*RSR";
//static char TT566getRITb[]		= "?RSR\r";
//static char TT566rspRITb[]		= "@RSR";

//static char TT566setXIT[]		= "*RMX"; // ONLY AVAILABLE ON PRIMARY RX
//static char TT566getXIT[]		= "?RMX\r";
//static char TT566rspXIT[]		= "@RMX";

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 },
	{ (Fl_Widget *)sldrRFGAIN,   54, 145, 156 },
	{ (Fl_Widget *)sldrMICGAIN, 266, 125, 156 },
	{ (Fl_Widget *)sldrPOWER,   266, 145, 156 },
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

RIG_TT566::RIG_TT566() {
	name_ = RIG_TT566name_;
	modes_ = RIG_TT566modes_;
	bandwidths_ = RIG_TT566widths;
	bw_vals_ = RIG_TT566_bw_vals;

	widgets = rig_widgets;

	comm_baudrate = BR57600;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 10;
	comm_timeout = 50;
	comm_echo = true;
	comm_rtscts = true;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	def_mode = modeB = modeA = A.imode = B.imode = 1;
	def_bw = bwB = bwA = A.iBW = B.iBW = 19;
	def_freq = freqB = freqA = A.freq = B.freq = 14070000;

	max_power = 100;
	atten_level = 0;

//	has_ifshift_control =
//	has_agc_level =

	can_change_alt_vfo =
	has_smeter =
	has_power_out =
	has_rf_control =
	has_volume_control =
	has_micgain_control =
	has_swr_control =
	has_mode_control =
	has_bandwidth_control =
	has_ptt_control =
	has_preamp_control =
	has_noise_control =
	has_auto_notch = 
	has_attenuator_control =
	has_power_control = true;

	precision = 1;
	ndigits = 8;

};

void RIG_TT566::initialize()
{
	rig_widgets[0].W = btnVol;
	rig_widgets[1].W = sldrVOLUME;
	rig_widgets[2].W = sldrRFGAIN;
	rig_widgets[3].W = sldrMICGAIN;
	rig_widgets[4].W = sldrPOWER;
}

int  RIG_TT566::adjust_bandwidth(int m)
{
	switch (m) {
		case 0 : case 1 : return 19;
		case 2 : case 3 : return 4;
		case 4 : case 5 : return 24;
		case 6 : default : return 7;
	}
	return 19;
}

#define waitcr(n) wait_char('\r', n, 100, "", ASC)

bool RIG_TT566::check ()
{
	cmd = TT566getFREQa;
//	replystr = "@AF14070050\r";
	waitcr(12);
	getcr("check vfo A");
	size_t p = replystr.rfind(TT566rspFREQa);
	if (p == std::string::npos) return false;
	return true;
}

unsigned long int RIG_TT566::get_vfoA ()
{
	cmd = TT566getFREQa; 
//	replystr = "@AF14070050\r";
	waitcr(12);
	getcr("get vfo A");

	size_t p = replystr.rfind(TT566rspFREQa);
	if (p == std::string::npos) return A.freq;

	int f = 0;
	sscanf(&replystr[p + strlen(TT566rspFREQa)], "%d", &f);
	A.freq = f;
	return A.freq;
}

void RIG_TT566::set_vfoA (unsigned long int freq)
{
	A.freq = freq;
	cmd = TT566setFREQa;
	cmd.append(to_decimal(freq, 8));
	cmd += '\r';
	sendCommand(cmd);
	setcr("set vfo A");
	return;
}

unsigned long int RIG_TT566::get_vfoB ()
{
	cmd = TT566getFREQb;
//	replystr = "@BF7070000\r";
	waitcr(12);
	getcr("get vfo B");

	size_t p = replystr.rfind(TT566rspFREQb);
	if (p == std::string::npos) return B.freq;

	int f = 0;
	sscanf(&replystr[p + strlen(TT566rspFREQb)], "%d", &f);
	B.freq = f;
	return B.freq;
}

void RIG_TT566::set_vfoB (unsigned long int freq)
{
	B.freq = freq;
	cmd = TT566setFREQb;
	cmd.append(to_decimal(freq, 8));
	cmd += '\r';
	sendCommand(cmd);
	setcr("set vfo B");
	return;
}

void RIG_TT566::set_modeA(int md)
{
	A.imode = md;
		cur_modeA = A.imode;
	cmd = TT566setMODEa;
	cmd += '0' + md;
	cmd += '\r';
	sendCommand(cmd);
	setcr("set mode A");
}

int RIG_TT566::get_modeA()
{
	cmd = TT566getMODEa;
//	replystr = "@RMM3\r";
	waitcr(6);
	getcr("get mode A");

	size_t p = replystr.rfind(TT566rspMODEa);
	if (p == std::string::npos) return A.imode;

	A.imode = replystr[p + strlen(TT566rspMODEa)] - '0';
	cur_modeA = A.imode;

	return A.imode;
}

void RIG_TT566::set_modeB(int md)
{
	B.imode = md;
		cur_modeB = B.imode;
	cmd = TT566setMODEb;
	cmd += '0' + md;
	cmd += '\r';
	sendCommand(cmd);
	setcr("set mode B");
}

int RIG_TT566::get_modeB()
{
	cmd = TT566getMODEb;
//	replystr = "@RSM2\r";
	waitcr(6);
	getcr("get mode B");

	size_t p = replystr.rfind(TT566rspMODEb);
	if (p == std::string::npos) return B.imode;

	B.imode = replystr[p + strlen(TT566rspMODEb)] - '0';
	cur_modeB = B.imode;

	return B.imode;
}

void RIG_TT566::set_bwA(int bw)
{
	A.iBW = bw;
	cmd = TT566setBWa;
	cmd.append(RIG_TT566widths[bw]);
	cmd += '\r';
	sendCommand(cmd);
	setcr("set bw A");
}

int RIG_TT566::get_bwA()
{
	cmd = TT566getBWa;
//	replystr = "@RMF3000\r";
	waitcr(9);
	getcr("get bw A");

	size_t p = replystr.rfind(TT566rspBWa);
	if (p == std::string::npos) return A.iBW;

	std::string bwstr = replystr.substr(p + strlen(TT566rspBWa));
	if (bwstr[bwstr.length() - 1] == '\r') bwstr.erase(bwstr.length() - 1);

	if (replystr.empty()) return A.iBW;

	int i = 0;
	while( RIG_TT566widths[i] != NULL) {
		if (bwstr == RIG_TT566widths[i]) {
			A.iBW = i;
			break;
		}
		i++;
	}

	return A.iBW;
}

void RIG_TT566::set_bwB(int bw)
{
	B.iBW = bw;
	cmd = TT566setBWb;
	cmd.append(RIG_TT566widths[bw]);
	cmd += '\r';
	sendCommand(cmd);
	setcr("set bw B");
}

int RIG_TT566::get_bwB()
{
	cmd = TT566getBWb;
//	replystr = "@RSF500\r";
	waitcr(9);
	getcr("get bw B");

	size_t p = replystr.rfind(TT566rspBWb);
	if (p == std::string::npos) return B.iBW;

	std::string bwstr = replystr.substr(p + strlen(TT566rspBWb));
	if (bwstr[bwstr.length() - 1] == '\r') bwstr.erase(bwstr.length() - 1);
	if (replystr.empty()) return B.iBW;

	int i = 0;
	while( RIG_TT566widths[i] != NULL) {
		if (bwstr == RIG_TT566widths[i]) {
			B.iBW = i;
			break;
		}
		i++;
	}

	return B.iBW;
}


int RIG_TT566::get_modetype(int n)
{
	return RIG_TT566_mode_type[n];
}

double RIG_TT566::get_power_control(void)
{
	cmd = TT566getPWR;
//	replystr = "";
	waitcr(9);
	getcr("get power control");

	size_t p = replystr.rfind(TT566rspPWR);

	if (p == std::string::npos) return 0;

	int pwr;
	sscanf(replystr.substr(p + strlen(TT566rspPWR)).c_str(), "%d", &pwr);

	return pwr;
}

void RIG_TT566::set_power_control(double val) 
{
	cmd = TT566setPWR;
	cmd.append(to_decimal(val,3));
	cmd += '\r';
	sendCommand(cmd);
	setcr("set power control");
}

void RIG_TT566::get_pc_min_max_step(double &min, double &max, double &step)
{
	min = 0; max = 100; step = 1;
}

static int auto_notch_state = 0;

void RIG_TT566::set_auto_notch(int v)
{
	cmd = TT566setANa;
	cmd += v ? "1\r" : "0\r";
	sendCommand(cmd);
	auto_notch_state = v;
	setcr("set auto notch");
}

int  RIG_TT566::get_auto_notch()
{
	return auto_notch_state;
}

void RIG_TT566::set_PTT_control(int val)
{
	ptt_ = val;
	cmd = TT566setPTT;
	cmd += val ? "K\r" : "U\r";
	sendCommand(cmd);
	setcr("set PTT");
}

static std::string pmeter = "";

int RIG_TT566::get_PTT()
{
	cmd = TT566getSMETER;
	waitcr(12);
	getcr("get PTT");

	pmeter = replystr;
	if (pmeter.rfind(TT566rspPOUT) != std::string::npos) {
		ptt_ = true;
	} else
		ptt_ = false;
	return ptt_;
}

// @SRM064S017
int  RIG_TT566::get_smeter()
{
	int dbm = 0;
	size_t p = pmeter.rfind(TT566rspSMETER);
	if (p == std::string::npos) {
		cmd = TT566getSMETER;
		waitcr(12);
		getcr("get smeter");
		pmeter = replystr;
		p = pmeter.rfind(TT566rspSMETER);
	} else {
		get_trace(2, "get_smeter", pmeter.c_str());
	}

	if (p != std::string::npos)
		sscanf(pmeter.substr(p + strlen(TT566rspSMETER)).c_str(), "%d", &dbm);

	pmeter.clear();

	return 5 * dbm / 9; // 90 = S9 --> 50% of full scale of flrig display
}

static float swr = 1.0;

int  RIG_TT566::get_power_out()
{
	getr("get power out");

	size_t p = pmeter.rfind(TT566rspPOUT);
	if (p == std::string::npos) {
		cmd = TT566getSMETER;
		waitcr(12);
		getcr("get smeter");
		pmeter = replystr;
		p = pmeter.rfind(TT566rspPOUT);
	} else {
		get_trace(2, "get_power_out", pmeter.c_str());
	}

	if (p != std::string::npos)
		sscanf(pmeter.substr(p + strlen(TT566rspPOUT)).c_str(), "%dR%dS%f", &fwdpwr, &refpwr, &swr);

	pmeter.clear();

	return 3 * fwdpwr / 2;
}

int  RIG_TT566::get_swr()
{
	if (fwdpwr == 0) return 0;

	float swr = (sqrtf(fwdpwr) + sqrtf(refpwr))/(sqrt(fwdpwr) - sqrt(refpwr) + .0001);
	swr = (swr - 1.0)*25;
	if (swr < 0) swr = 0;
	if (swr > 100) swr = 100;
	return (int)swr;
}

int RIG_TT566::get_volume_control()
{
	cmd = TT566getVOLa;
//	replystr = "";
	waitcr(9);
	getcr("get volume control");

	int val = 0;
	size_t p = replystr.rfind(TT566rspVOLa);
	if (p == std::string::npos) return 0;

	sscanf(replystr.substr(p + strlen(TT566rspVOLa)).c_str(), "%d", &val);
	val = (100*val)/255;
	return val;
}

void RIG_TT566::set_volume_control(int val)
{
	val = (255*val)/100;
	cmd = TT566setVOLa;
	cmd.append(to_decimal(val,3));
	cmd += '\r';
	sendCommand(cmd);
	setcr("set volume control");
}

void RIG_TT566::set_mic_gain(int val)
{
	cmd = TT566setMIC;
	cmd.append(to_decimal(val,3));
	cmd += '\r';
	sendCommand(cmd);
	setcr("set mic gain");
}

int  RIG_TT566::get_mic_gain()
{
	cmd = TT566getMIC;
//	replystr = "";
	waitcr(9);
	getcr("get mic gain");

	int val = 0;
	size_t p = replystr.rfind(TT566rspMIC);
	if (p == std::string::npos) return 0;

	sscanf(replystr.substr(p + strlen(TT566rspMIC)).c_str(), "%d", &val);
	return val;
}

/*
void RIG_TT566::set_if_shift(int val)
{
	char sznum[6];
	snprintf(sznum, sizeof(sznum), "%d", val);
	cmd = TT566getPBTa;
	cmd.append(sznum);
	cmd += '\r';
	sendCommand(cmd);
}

bool RIG_TT566::get_if_shift(int &val)
{
	int retval = 0;
	cmd = TT566getPBTa;
	sendCommand(cmd, strlen(TT566rspPBTa)+6);
	size_t p = replystr.rfind(TT566rspPBTa);
	if (p == std::string::npos) return false;
	sscanf(&replystr[p + 4], "%d", &retval);
	val = retval;
	if (val) return true;
	return false;
}
*/

void RIG_TT566::set_rf_gain(int val)
{
	cmd = TT566setRFGa;
	char sznum[5];
	snprintf(sznum, sizeof(sznum), "%d\r", val);
	cmd.append(sznum);
	sendCommand(cmd);
	setcr("set rf gain");
}

int  RIG_TT566::get_rf_gain()
{
	cmd = TT566getRFGa;
//	replystr = "";
	waitcr(9);
	getcr("get rf gain");

	size_t p = replystr.rfind(TT566rspRFGa);
	if (p == std::string::npos) return 0;

	int retval = 0;
	sscanf(replystr.substr(p + strlen(TT566rspRFGa)).c_str(), "%d", &retval);
	return retval;
}

int  RIG_TT566::next_attenuator()
{
	switch (atten_level) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 3;
		case 3: return 0;
	}
	return 0;
}

void RIG_TT566::set_attenuator(int val)
{
	atten_level = val;
	cmd = TT566setATTa;

	switch (atten_level) {
		case 0: atten_label("0 dB", false); cmd += '0'; break;
		case 1: atten_label("6 dB", true); 	cmd += '1'; break;
		case 2: atten_label("12 dB", true); cmd += '2'; break;
		case 3: atten_label("18 dB", true); cmd += '3'; break;
	}
	cmd += '\r';
	sendCommand(cmd);
	setcr("set attenuator");
}


int RIG_TT566::get_attenuator()
{
	cmd = TT566getATTa;
//	replystr = "";
	waitcr(7);

	int val = atten_level;

	size_t p = replystr.rfind(TT566rspATTa);
	if (p == std::string::npos) return val;

	val = (replystr[p + strlen(TT566rspATTa)] - '0');

	if (atten_level != val) atten_level = val;
	switch (atten_level) {
		case 0: atten_label("0 dB", false); break;
		case 1: atten_label("6 dB", true); break;
		case 2: atten_label("12 dB", true); break;
		case 3: atten_label("18 dB", true); break;
	}
	return atten_level;
}

void RIG_TT566::set_preamp(int val)
{
	cmd = TT566setPREAMP;
	cmd += val ? "1\r" : "0\r";
	sendCommand(cmd);
	setcr("set preamp");
}

int RIG_TT566::get_preamp()
{
	cmd = TT566getPREAMP;
//	replystr = "";
	waitcr(7);

	size_t p = replystr.rfind(TT566rspPREAMP);
	if (p == std::string::npos) return 0;

	return replystr[p + strlen(TT566rspPREAMP)] - '0';
}

void RIG_TT566::set_noise(bool val)
{
	cmd = TT566setNBa;
	cmd += val ? "1\r" : "0\r";
	sendCommand(cmd);
	setcr("set noise");
}

int  RIG_TT566::get_noise()
{
	cmd = TT566getNBa;
//	replystr = "";
	waitcr(7);

	size_t p = replystr.rfind(TT566rspNBa);
	if (p == std::string::npos) return 0;

	return replystr[p + strlen(TT566rspNBa)] - '0';
}

