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

#include "kenwood/TS790.h"
#include "support.h"

static const char TS790name_[] = "TS-790";

static const char *TS790modes_[] = {
		"LSB", "USB", "CW", "FM", "CWN", NULL};
static const char TS790_mode_chr[] =  { '1', '2', '3', '4', '7' };
static const char TS790_mode_type[] = { 'L', 'U', 'L', 'U', 'L' };

//static const char *TS790_widths[] = {
//"NONE", "FM-W", "FM-N", "AM", "SSB", "CW", "CWN", NULL};
//static int TS790_bw_vals[] = { 1,2,3,4,5,6,7, WVALS_LIMIT};

RIG_TS790::RIG_TS790() {
// base class values
	name_ = TS790name_;
	modes_ = TS790modes_;
	_mode_type = TS790_mode_type;
	bandwidths_ = NULL;
	serial_baudrate = BR4800;
	stopbits = 2;
	serial_retries = 2;

//	serial_write_delay = 0;
//	serial_post_write_delay = 0;

	serial_timeout = 50;
	serial_rtscts = false;
	serial_rtsplus = false;
	serial_dtrplus = false;
	serial_catptt = true;
	serial_rtsptt = false;
	serial_dtrptt = false;
	modeB = modeA = def_mode = 1;
	bwB = bwA = def_bw = 1;
	freqB = freqA = def_freq = 146900000ULL;
	can_change_alt_vfo = true;

	has_bandwidth_control =
	has_swr_control =
	has_noise_control =
	has_micgain_control =
	has_volume_control =
	has_power_control =
	has_tune_control =
	has_attenuator_control =
	has_preamp_control =
	has_notch_control =
	has_ifshift_control = false;

	has_smeter =
	has_mode_control =
	has_ptt_control = true;

	precision = 10;
	ndigits = 10;

}

void RIG_TS790::initialize()
{
}

bool RIG_TS790::check ()
{
	cmd = "FA;";
	int ret = wait_char(';', 14, 100, "check", ASC);
	if (ret < 14) return false;
	return true;
}

unsigned long long RIG_TS790::get_vfoA ()
{
	cmd = "FA;";

	int ret = wait_char(';', 14, 100, "get vfo A", ASC);
	if (ret < 14) return freqA;

	size_t p = replystr.rfind("FA");
	if (p == std::string::npos) return freqA;

	unsigned long long f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replystr[p + n] - '0';
	freqA = f;
	return freqA;
}

void RIG_TS790::set_vfoA (unsigned long long freq)
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

unsigned long long RIG_TS790::get_vfoB ()
{
	cmd = "FB;";
	int ret = wait_char(';', 14, 100, "get vfo B", ASC);
	if (ret < 14) return freqB;

	size_t p = replystr.rfind("FB");
	if (p == std::string::npos) return freqB;

	unsigned long long f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replystr[p + n] - '0';
	freqB = f;
	return freqB;
}

void RIG_TS790::set_vfoB (unsigned long long freq)
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
int RIG_TS790::get_smeter()
{
	cmd = "SM;";
	int ret = wait_char(';', 7, 100, "get smeter", ASC);
	if (ret < 7) return 0;

	size_t p = replystr.rfind("SM");
	if (p == std::string::npos) return 0;

	int mtr = 0;
	for (size_t n = 2; n < 6; n++)
		mtr = mtr*10 + replystr[p + n] - '0';
	mtr = (mtr * 100) / 15;

	return mtr;
}

void RIG_TS790::set_modeA(int val)
{
	showresp(WARN, ASC, "set mode A", "", "");
	modeA = val;
	cmd = "MD";
	cmd += TS790_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
}

int RIG_TS790::get_modeA()
{
//	cmd = "IF;";
//	int ret = wait_char(';', 38, 100, "get modeA", ASC);

	int ret = check_ifstr();
	if (ret < 38) return split;
	size_t p = replystr.rfind("IF");
	if (p == std::string::npos) return modeA;
	int resp = replystr[p+29]; // 1, 2, 3, 4, 7
	for (int i = 0; i < 5; i++)
		if (resp == TS790_mode_chr[i]) {
			modeA = i;
			break;
		}
	return modeA;
}

void RIG_TS790::set_modeB(int val)
{
	showresp(WARN, ASC, "set mode B", "", "");
	modeB = val;
	cmd = "MD";
	cmd += TS790_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
}

int RIG_TS790::get_modeB()
{
//	cmd = "IF;";
//	int ret = wait_char(';', 38, 100, "get mode B", ASC);

	int ret = check_ifstr();
	if (ret < 38) return split;
	size_t p = replystr.rfind("IF");
	if (p == std::string::npos) return modeA;
	int resp = replystr[p+29]; // 1, 2, 3, 4, 7
	for (int i = 0; i < 5; i++)
		if (resp == TS790_mode_chr[i]) {
			modeB = i;
			break;
		}
	return modeB;
}

int RIG_TS790::get_modetype(int n)
{
	return _mode_type[n];
}

void RIG_TS790::selectA()
{
	showresp(WARN, ASC, "select A", "", "");
	cmd = "FN0;";
	sendCommand(cmd);
	inuse = onA;
}

void RIG_TS790::selectB()
{
	showresp(WARN, ASC, "select B", "", "");
	cmd = "FN1;";
	sendCommand(cmd);
	inuse = onB;
}

// Tranceiver PTT on/off
void RIG_TS790::set_PTT_control(int val)
{
	showresp(WARN, ASC, "PTT", val ? "on" : "off", "");
	if (val) cmd = "TX;";
	else	 cmd = "RX;";
	sendCommand(cmd);
}

/*
========================================================================
  frequency & mode data are contained in the IF; response
     IFaaaaaaaaaaaAAAAABBBBBcXXeefghjklmmC;
     0        1         2         3 
     12345678901234567890123456789012345678
     where:
         aaaaaaaaaaa => decimal value of vfo frequency
         AAAAA => step frequency
         BBBBB => RIT frequency
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
         c => offset
         X => unused characters
========================================================================
*/ 

int RIG_TS790::get_PTT()
{
//	cmd = "IF;";
//	int ret = wait_char(';', 38, 100, "get PTT", ASC);

	int ret = check_ifstr();
	if (ret < 38) return ptt_;
	ptt_ = (replystr[28] == '1');
	return ptt_;
}

bool RIG_TS790::can_split()
{
	return true;
}

void RIG_TS790::set_split(bool val)
{
	if (val) {
		cmd = "SP1;";
		sendCommand(cmd);
	} else {
		cmd = "SP0;";
		sendCommand(cmd);
	}
}

int RIG_TS790::get_split()
{
//	cmd = "IF;";
//	int ret = wait_char(';', 38, 100, "get split", ASC);

	int ret = check_ifstr();
	if (ret < 38) return split;
	size_t p = replystr.rfind("IF");
	if (p == std::string::npos) return split;
	split = (replystr[p+32] == 1);
	return split;
}


/*
static std::string bw_str = "FL001001;";

void RIG_TS790::set_bwA(int val)
{
	get_bwA();
	showresp(WARN, ASC, "set bw A", "", "");
	bwA = val;
	cmd = bw_str.substr(0, 5);
	cmd.append(TS790_filters[val]).append(";");
	sendCommand(cmd);
}

int RIG_TS790::get_bwA()
{
	cmd = "FL;";
	int ret = wait_char(';', 9, 100, "get bwA", ASC);
	if (ret < 9) return bwA;
	bw_str = replystr;
	size_t p = replystr.rfind("FL");
	if (p == std::string::npos) return bwA;

	replystr[p + 8] = 0;
	int bw = 0;
	while (TS790_filters[bw]) {
		if (strcmp(&replystr[p + 5], TS790_filters[bw]) == 0)
			return bwA = bw;
		bw++;
	}
	return bwA;
}

void RIG_TS790::set_bwB(int val)
{
	get_bwB();
	showresp(WARN, ASC, "set bw B", "", "");
	bwB = val;
	cmd = bw_str.substr(0, 5);
	cmd.append(TS790_filters[val]).append(";");
	sendCommand(cmd);
}

int RIG_TS790::get_bwB()
{
	cmd = "FL;";
	int ret = wait_char(';', 9, 100, "get bwB", ASC);
	if (ret < 9) return bwB;
	bw_str = replystr;
	size_t p = replystr.rfind("FL");
	if (p == std::string::npos) return bwB;

	replystr[p + 8] = 0;
	int bw = 0;
	while (TS790_filters[bw]) {
		if (strcmp(&replystr[p + 5], TS790_filters[bw]) == 0)
			return bwB = bw;
		bw++;
	}
	return bwB;
}

//	"LSB", "USB", "CW", "FM", "AM", "FSK", "CW-R", "FSK-R"
//	'1',   '2',   '3',   '4',  '5',  '6',   '7',     '9'
//	 0      1      2      3     4     5      6       7
int RIG_TS790::def_bandwidth(int val)
{
	if (val == 0 || val == 1) // LSB USB
		return 4;
	else if (val == 2 || val == 6) // CW
		return 5;
	else if (val == 3) // FM
		return 1;
	else if (val == 4) // AM
		return 3;
	else if (val == 5 || val == 7) // FSK
		return 2;
	return 0; // TUNE
}

// RM cmd 0 ... 100 (rig values 0 ... 30)
int RIG_TS790::get_swr()
{
	cmd = "RM;";
	int ret = wait_char(';', 8, 100, "get swr", ASC);
	if (ret < 8) return 0;

	size_t p = replystr.rfind("RM");
	if (p == std::string::npos) return 0;

	int mtr = 0;
	for (size_t n = 3; n < 7; n++)
		mtr = mtr*10 + replystr[p + n] - '0';
	mtr = (mtr * 50) / 30;

	return mtr;
}

int RIG_TS790::get_alc()
{
	cmd = "RM3;"; // select measurement '3' ALC
	sendCommand(cmd);
	cmd = "RM;";
	int ret = wait_char(';', 8, 100, "get alc", ASC);
	if (ret < 8) return 0;

	size_t p = replystr.rfind("RM");
	if (p == std::string::npos) return 0;

	int mtr = 0;
	for (size_t n = 3; n < 7; n++)
		mtr = mtr*10 + replystr[p + n] - '0';
	mtr = (mtr * 50) / 30;

	return mtr;
}

*/
