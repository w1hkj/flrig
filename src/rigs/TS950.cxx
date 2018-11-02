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

#include "TS950.h"
#include "support.h"
#include "trace.h"

struct pwrpair {int mtr; float pwr;};

static const char TS950name_[] = "TS-950";

enum {LSB950, USB950, CW950, FM950, AM950, FSK950};

static const char *TS950modes_[] = {
		"LSB", "USB", "CW", "FM", "AM", "FSK", NULL};
static const char TS950_mode_chr[] =  { '1', '2', '3', '4', '5', '6' };
static const char TS950_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'L' };

enum {NONE_950, FM_W_950, FM_N_950, AM_950, SSB_950, SSB_N_950, CW_950, CW_N_950};
static const char *TS950_widths[] = {
"NONE", "FM-W", "FM-N", "AM", "SSB", "SSB-N", "CW", "CW-N", NULL};
static int TS950_bw_vals[] = { 1,2,3,4,5,6,7,8, WVALS_LIMIT};

static const char *TS950_filters[] = {
"000", "002", "003", "005", "007", "008", "009", "010", NULL};

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnDataPort, 214, 105,  50 },
	{ (Fl_Widget *)NULL,        0,   0,   0 }
};

enum {NO_METER, SWR_METER, ALC_METER};

static int selected_meter = NO_METER;

void RIG_TS950::initialize()
{
	rig_widgets[0].W = btnDataPort;

	cmd = "AI0;"; // auto information OFF
	sendCommand(cmd);
	sett("AI off");
	MilliSleep(100);
	cmd = "RM1;"; // select measurement '1' SWR
	sendCommand(cmd);
	sett("SWR/PWR meter");
	set_data_port();
}

RIG_TS950::RIG_TS950() {
// base class values
	name_ = TS950name_;
	modes_ = TS950modes_;
	_mode_type = TS950_mode_type;
	bandwidths_ = TS950_widths;
	bw_vals_ = TS950_bw_vals;
	comm_baudrate = BR4800;
	stopbits = 2;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_rtscts = true;
	comm_rtsplus = false;
	comm_dtrplus = false;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;

	widgets = rig_widgets;

	modeB = modeA = def_mode = 1;
	bwB = bwA = def_bw = 1;
	freqB = freqA = def_freq = 14070000;
	can_change_alt_vfo = true;

	has_noise_control =
	has_micgain_control =
	has_volume_control =
	has_power_control =
	has_tune_control =
	has_attenuator_control =
	has_preamp_control =
	has_notch_control =
	has_ifshift_control =
	has_swr_control = false;

	has_power_out =
	has_data_port =
	has_smeter =
	has_swr_control =
	has_alc_control =
	has_mode_control =
	has_bandwidth_control =
	has_ptt_control = true;

	precision = 10;
	ndigits = 7;

}

bool RIG_TS950::check ()
{
	cmd = "FA;";
	int ret = wait_char(';', 14, 100, "check", ASC);
	if (ret < 14) return false;
	return true;
}

void RIG_TS950::set_data_port()
{
	if (progStatus.data_port) {
		cmd = "DT1;";
		sett("Data Port ON");
	} else {
		cmd = "DT0;";
		sett("Data Port OFF");
	}
	sendCommand(cmd);
}

long RIG_TS950::get_vfoA ()
{
	cmd = "FA;";

	int ret = wait_char(';', 14, 100, "get vfo A", ASC);
	if (ret < 14) return freqA;

	size_t p = replystr.rfind("FA");
	if (p == string::npos) return freqA;

	int f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replystr[p + n] - '0';
	freqA = f;
	gett("vfo A");
	return freqA;
}

void RIG_TS950::set_vfoA (long freq)
{
	freqA = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	sett("vfo A");
}

long RIG_TS950::get_vfoB ()
{
	cmd = "FB;";
	int ret = wait_char(';', 14, 100, "get vfo B", ASC);
	if (ret < 14) return freqB;

	size_t p = replystr.rfind("FB");
	if (p == string::npos) return freqB;

	int f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replystr[p + n] - '0';
	freqB = f;
	gett("vfo B");
	return freqB;
}

void RIG_TS950::set_vfoB (long freq)
{
	freqB = freq;
	cmd = "FB00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	sett("vfo B");
}

// SM cmd 0 ... 100 (rig values 0 ... 30)
int RIG_TS950::get_smeter()
{
	cmd = "SM;";
	int ret = wait_char(';', 7, 100, "get smeter", ASC);
	if (ret < 7) return 0;

	size_t p = replystr.rfind("SM");
	if (p == string::npos) return 0;

	int mtr = 0;
	for (size_t n = 2; n < 6; n++)
		mtr = mtr*10 + replystr[p + n] - '0';
	mtr = (mtr * 100) / 30;

	gett("Smeter");
	return mtr;
}

// RM cmd 0 ... 100 (rig values 0 ... 30)
int RIG_TS950::get_swr()
{
	int ret = 0;
	if (selected_meter != SWR_METER) {
		cmd = "RM1;"; // select measurement '1' swr
		sendCommand(cmd);
		selected_meter = SWR_METER;
		MilliSleep(200);
		cmd = "RM;";
		ret = wait_char(';', 8, 500, "get alc", ASC);
	} else {
		cmd = "RM;";
		ret = wait_char(';', 8, 100, "get alc", ASC);
	}
	if (ret < 8) return 0;

	size_t p = replystr.rfind("RM");
	if (p == string::npos) return 0;

	int mtr = 0;
	for (size_t n = 3; n < 7; n++)
		mtr = mtr*10 + replystr[p + n] - '0';
	mtr = (mtr * 50) / 30;

	gett("SWR");
	return mtr;
}

static pwrpair pwrtbl[] = { 
{0, 0.0},
{3, 5.0},
{6, 10.0},
{7, 15.0},
{16, 100.0},
{30, 250.0}
};
// SMpppp;

int RIG_TS950::get_power_out(void)
{
	cmd = "SM;";
	int ret = wait_char(';', 7, 100, "get smeter", ASC);
	if (ret < 7) return 0;

	size_t p = replystr.rfind("SM");
	if (p == string::npos) return 0;

	int mtr = 0;
	sscanf(&replystr[2], "%d", &mtr);

	size_t i = 0;
	for (i = 0; i < sizeof(pwrtbl) / sizeof(pwrpair) - 1; i++)
		if (mtr >= pwrtbl[i].mtr && mtr < pwrtbl[i+1].mtr)
			break;

	if (mtr < 0) mtr = 0;
	if (mtr > 30) mtr = 30;
	int pwr = (int)ceil(
		   pwrtbl[i].pwr
		   + (pwrtbl[i+1].pwr - pwrtbl[i].pwr)
		     * (mtr - pwrtbl[i].mtr)
		     / (pwrtbl[i+1].mtr - pwrtbl[i].mtr)
		  );

	if (pwr > 250) pwr = 250;
	gett("Power out");
	return pwr;
}

int RIG_TS950::get_alc()
{
	int ret = 0;
	if (selected_meter != ALC_METER) {
		cmd = "RM3;"; // select measurement '3' ALC
		sendCommand(cmd);
		selected_meter = ALC_METER;
		MilliSleep(200);
		cmd = "RM;";
		ret = wait_char(';', 8, 500, "get alc", ASC);
	} else {
		cmd = "RM;";
		ret = wait_char(';', 8, 100, "get alc", ASC);
	}
	if (ret < 8) return 0;

	size_t p = replystr.rfind("RM");
	if (p == string::npos) return 0;

	int mtr = 0;
	for (size_t n = 3; n < 7; n++)
		mtr = mtr*10 + replystr[p + n] - '0';
	mtr = (mtr * 50) / 30;

	gett("ALC");
	return mtr;
}


void RIG_TS950::set_modeA(int val)
{
	modeA = val;
	cmd = "MD";
	cmd += TS950_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	sett("mode A");
}

int RIG_TS950::get_modeA()
{
	cmd = "IF;";
	int ret = wait_char(';', 38, 100, "get modeA", ASC);
	if (ret < 38) return modeA;
	size_t p = replystr.rfind("IF");
	if (p == string::npos) return modeA;
	modeA = replystr[p+29] - '1'; // 0 - 8
	if (modeA == 8) modeA = 7;
	gett("mode A");
	return modeA;
}

void RIG_TS950::set_modeB(int val)
{
	modeB = val;
	cmd = "MD";
	cmd += TS950_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	sett("mode B");
}

int RIG_TS950::get_modeB()
{
	cmd = "IF;";
	int ret = wait_char(';', 38, 100, "get mode B", ASC);
	if (ret < 38) return modeB;
	size_t p = replystr.rfind("IF");
	if (p == string::npos) return modeB;
	modeB = replystr[p+29] - '1'; // 0 - 8
	if (modeB == 8) modeB = 7;
	gett("mode B");
	return modeB;
}

int RIG_TS950::get_modetype(int n)
{
	return _mode_type[n];
}

static string bw_str = "FL001001;";

void RIG_TS950::set_bwA(int val)
{
	get_bwA();
	bwA = val;
	cmd = bw_str.substr(0, 5);
	cmd.append(TS950_filters[val]).append(";");
	sendCommand(cmd);
	sett("BW A");
}

int RIG_TS950::get_bwA()
{
	cmd = "FL;";
	int ret = wait_char(';', 9, 100, "get bwA", ASC);
	if (ret < 9) return bwA;
	bw_str = replystr;
	size_t p = replystr.rfind("FL");
	if (p == string::npos) return bwA;

	replystr[p + 8] = 0;
	int bw = 0;
	while (TS950_filters[bw]) {
		if (strcmp(&replystr[p + 5], TS950_filters[bw]) == 0)
			return bwA = bw;
		bw++;
	}
	gett("BW A");
	return bwA;
}

void RIG_TS950::set_bwB(int val)
{
	get_bwB();
	bwB = val;
	cmd = bw_str.substr(0, 5);
	cmd.append(TS950_filters[val]).append(";");
	sendCommand(cmd);
	sett("BW B");
}

int RIG_TS950::get_bwB()
{
	cmd = "FL;";
	int ret = wait_char(';', 9, 100, "get bwB", ASC);
	if (ret < 9) return bwB;
	bw_str = replystr;
	size_t p = replystr.rfind("FL");
	if (p == string::npos) return bwB;

	replystr[p + 8] = 0;
	int bw = 0;
	while (TS950_filters[bw]) {
		if (strcmp(&replystr[p + 5], TS950_filters[bw]) == 0)
			return bwB = bw;
		bw++;
	}
	gett("BW B");
	return bwB;
}


//LSB950, USB950, CW950, FM950, AM950, FSK950
//	"LSB", "USB", "CW", "FM", "AM", "FSK"
//	'1',   '2',   '3',   '4',  '5',  '6'
//	 0      1      2      3     4     5
//NONE_950, FM_W_950, FM_N_950, AM_950, SSB_950, SSB_N_950, CW_950, CW_N_950

int RIG_TS950::def_bandwidth(int val)
{
	if (val == LSB950 || val == USB950)
		return SSB_950;
	else if (val == CW950)
		return CW_950;
	else if (val == FM950)
		return FM_W_950;
	else if (val == AM950)
		return AM_950;
	else if (val == FSK950)
		return SSB_N_950;
	return NONE_950;
}

void RIG_TS950::selectA()
{
	cmd = "FR0;";
	sendCommand(cmd);
	sett("Rx on A");
	MilliSleep(100);
	cmd = "FT0;";
	sendCommand(cmd);
	sett("Tx on A");
	MilliSleep(100);
}

void RIG_TS950::selectB()
{
	cmd = "FR1;";
	sendCommand(cmd);
	sett("Rx on B");
	MilliSleep(100);
	cmd = "FT1;";
	sendCommand(cmd);
	sett("Tx on B");
	MilliSleep(100);
}

bool RIG_TS950::can_split()
{
	return true;
}

void RIG_TS950::set_split(bool val)
{
	if (useB) {
		if (val) {
			cmd = "FR1;"; sendCommand(cmd); sett("Rx on B"); MilliSleep(100);
			cmd = "FT0;"; sendCommand(cmd); sett("Tx on A"); MilliSleep(100);
		} else {
			cmd = "FR1;"; sendCommand(cmd); sett("Rx on B"); MilliSleep(100);
			cmd = "FT1;"; sendCommand(cmd); sett("Tx on B"); MilliSleep(100);
		}
	} else {
		if (val) {
			cmd = "FR0;"; sendCommand(cmd); sett("Rx on A"); MilliSleep(100);
			cmd = "FT1;"; sendCommand(cmd); sett("Tx on B"); MilliSleep(100);
		} else {
			cmd = "FR0;"; sendCommand(cmd); sett("Rx on A"); MilliSleep(100);
			cmd = "FT0;"; sendCommand(cmd); sett("Tx on A"); MilliSleep(100);
		}
	}
}

int RIG_TS950::get_split()
{
	cmd = "IF;";
	int ret = wait_char(';', 38, 100, "get split", ASC);
	if (ret < 38) return split;
	size_t p = replystr.rfind("IF");
	if (p == string::npos) return split;
	split = replystr[p+32] ? true : false;
	return split;
}

// Tranceiver PTT on/off
void RIG_TS950::set_PTT_control(int val)
{
	if (val) {
		cmd = "TX;";
		sett("ptt ON");
	}
	else {
		cmd = "RX;";
		sett("ptt OFF");
	}
	sendCommand(cmd);
}

/*
========================================================================
	frequency & mode data are contained in the IF; response
		IFaaaaaaaaaaaXXXXXbbbbbcdXeefghjklmmX;
		12345678901234567890123456789012345678
		01234567890123456789012345678901234567 byte #
		          1         2         3
		                            ^ position 28
		where:
			aaaaaaaaaaa => decimal value of vfo frequency
			bbbbb => rit/xit frequency
			c => rit off/on
			d => xit off/on
			e => memory channel
			f => tx/rx
			g => mode
			h => function
			j => scan off/on
			k => split off /on
			l => tone off /on
			m => tone number
			X => unused characters
		 
========================================================================
*/ 

int RIG_TS950::get_PTT()
{
	cmd = "IF;";
	int ret = wait_char(';', 38, 100, "get VFO", ASC);
	if (ret < 38) return ptt_;
	ptt_ = (replybuff[28] == '1');
	gett("PTT");
	return ptt_;
}
