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

#include "TS450S.h"
#include "support.h"

static const char TS450Sname_[] = "TS-450S";

static const char *TS450Smodes_[] = {
		"LSB", "USB", "CW", "FM", "AM", "FSK", "CW-R", "FSK-R", NULL};
static const char TS450S_mode_chr[] =  { '1', '2', '3', '4', '5', '6', '7', '9' };
static const char TS450S_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'L', 'L', 'U' };

static const char *TS450S_widths[] = {
"NONE", "FM-W", "FM-N", "AM", "SSB", "CW", NULL};
static int TS450S_bw_vals[] = { 1,2,3,4,5,6, WVALS_LIMIT};

static const char *TS450S_filters[] = {
"000", "002", "003", "005", "007", "009", NULL};

RIG_TS450S::RIG_TS450S() {
// base class values
	name_ = TS450Sname_;
	modes_ = TS450Smodes_;
	_mode_type = TS450S_mode_type;
	bandwidths_ = TS450S_widths;
	bw_vals_ = TS450S_bw_vals;
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

	has_smeter =
	has_swr_control =
	has_mode_control =
	has_bandwidth_control =
	has_ptt_control = true;

	precision = 10;
	ndigits = 7;

}

void RIG_TS450S::initialize()
{
	cmd = "RM1;"; // select measurement '1' (swr)
	sendCommand(cmd);
}

bool RIG_TS450S::check ()
{
	cmd = "FA;";
	int ret = wait_char(';', 14, 100, "check", ASC);
	if (ret < 14) return false;
	return true;
}

long RIG_TS450S::get_vfoA ()
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
	return freqA;
}

void RIG_TS450S::set_vfoA (long freq)
{
	freqA = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set vfo A", cmd, "");
}

long RIG_TS450S::get_vfoB ()
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
	return freqB;
}

void RIG_TS450S::set_vfoB (long freq)
{
	freqB = freq;
	cmd = "FB00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set vfo B", cmd, "");
}

// SM cmd 0 ... 100 (rig values 0 ... 15)
int RIG_TS450S::get_smeter()
{
	cmd = "SM;";
	int ret = wait_char(';', 7, 100, "get smeter", ASC);
	if (ret < 7) return 0;

	size_t p = replystr.rfind("SM");
	if (p == string::npos) return 0;

	replystr[p + 6] = 0;
	int mtr = atoi(&replystr[p + 2]);
	mtr = (mtr * 100) / 30;
	return mtr;
}

// RM cmd 0 ... 100 (rig values 0 ... 8)
int RIG_TS450S::get_swr()
{
	cmd = "RM;";
	int ret = wait_char(';', 8, 100, "get swr", ASC);
	if (ret < 8) return 0;

	size_t p = replystr.rfind("RM");
	if (p == string::npos) return 0;
	
	replystr[p + 7] = 0;
	int mtr = atoi(&replystr[p + 3]);
	mtr = (mtr * 50) / 30;
	return mtr;
}


void RIG_TS450S::set_modeA(int val)
{
	showresp(WARN, ASC, "set mode A", "", "");
	modeA = val;
	cmd = "MD";
	cmd += TS450S_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
}

int RIG_TS450S::get_modeA()
{
	cmd = "IF;";
	int ret = wait_char(';', 38, 100, "get modeA", ASC);
	if (ret < 38) return split;
	size_t p = replystr.rfind("IF");
	if (p == string::npos) return modeA;
	modeA = replystr[p+29] - '1'; // 0 - 6, 8
	if (modeA == 8) modeA = 7;
	return modeA;
}

void RIG_TS450S::set_modeB(int val)
{
	showresp(WARN, ASC, "set mode B", "", "");
	modeB = val;
	cmd = "MD";
	cmd += TS450S_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
}

int RIG_TS450S::get_modeB()
{
	cmd = "IF;";
	int ret = wait_char(';', 38, 100, "get mode B", ASC);
	if (ret < 38) return split;
	size_t p = replystr.rfind("IF");
	if (p == string::npos) return modeA;
	modeB = replystr[p+29] - '1'; // 0 - 6, 8
	if (modeB == 8) modeB = 7;
	return modeB;
}

int RIG_TS450S::get_modetype(int n)
{
	return _mode_type[n];
}

static string bw_str = "FL001001;";

void RIG_TS450S::set_bwA(int val)
{
	get_bwA();
	showresp(WARN, ASC, "set bw A", "", "");
	bwA = val;
	cmd = bw_str.substr(0, 5);
	cmd.append(TS450S_filters[val]).append(";");
	sendCommand(cmd);
}

int RIG_TS450S::get_bwA()
{
	cmd = "FL;";
	int ret = wait_char(';', 9, 100, "get bwA", ASC);
	if (ret < 9) return bwA;
	bw_str = replystr;
	size_t p = replystr.rfind("FL");
	if (p == string::npos) return bwA;
	
	replystr[p + 8] = 0;
	int bw = 0;
	while (TS450S_filters[bw]) {
		if (strcmp(&replystr[p + 5], TS450S_filters[bw]) == 0)
			return bwA = bw;
		bw++;
	}
	return bwA;
}

void RIG_TS450S::set_bwB(int val)
{
	get_bwB();
	showresp(WARN, ASC, "set bw B", "", "");
	bwB = val;
	cmd = bw_str.substr(0, 5);
	cmd.append(TS450S_filters[val]).append(";");
	sendCommand(cmd);
}

int RIG_TS450S::get_bwB()
{
	cmd = "FL;";
	int ret = wait_char(';', 9, 100, "get bwB", ASC);
	if (ret < 9) return bwB;
	bw_str = replystr;
	size_t p = replystr.rfind("FL");
	if (p == string::npos) return bwB;
	
	replystr[p + 8] = 0;
	int bw = 0;
	while (TS450S_filters[bw]) {
		if (strcmp(&replystr[p + 5], TS450S_filters[bw]) == 0)
			return bwB = bw;
		bw++;
	}
	return bwB;
}

int RIG_TS450S::def_bandwidth(int val)
{
	if (val == 0 || val == 1)
		return 4;
	else if (val == 2 || val == 6)
		return 5;
	else if (val == 3)
		return 1;
	else if (val == 4)
		return 3;
	else if (val == 5 || val == 7)
		return 2;
	return 4;
}

void RIG_TS450S::selectA()
{
	showresp(WARN, ASC, "select A", "", "");
	cmd = "FR0;";
	sendCommand(cmd);
	cmd = "FT0;";
	sendCommand(cmd);
}

void RIG_TS450S::selectB()
{
	showresp(WARN, ASC, "select B", "", "");
	cmd = "FR1;";
	sendCommand(cmd);
	cmd = "FT1;";
	sendCommand(cmd);
}

bool RIG_TS450S::can_split()
{
	return true;
}

void RIG_TS450S::set_split(bool val)
{
	if (val) {
		cmd = "FR0;";
		sendCommand(cmd);
		cmd = "FT1;";
		sendCommand(cmd);
	} else {
		cmd = "FR0;";
		sendCommand(cmd);
		cmd = "FT0;";
		sendCommand(cmd);
	}
}

int RIG_TS450S::get_split()
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
void RIG_TS450S::set_PTT_control(int val)
{
	if (val) sendCommand("TX;");
	else	 sendCommand("RX;");
	ptt_ = val;
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

int RIG_TS450S::get_PTT()
{
	cmd = "IF;";
	int ret = wait_char(';', 38, 100, "get VFO", ASC);
	if (ret < 38) return ptt_;
	ptt_ = (replybuff[28] == '1');
	return ptt_;
}
