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
// aunsigned long int with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include "kenwood/TS440.h"
#include "support.h"

static const char TS440name_[] = "TS-440";

static const char *TS440modes_[] = {
		"LSB", "USB", "CW", "FM", "AM", "FSK", NULL};
static const char TS440_mode_chr[] =  { '1', '2', '3', '4', '5', '6' };
static const char TS440_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'L' };

RIG_TS440::RIG_TS440() {
// base class values
	name_ = TS440name_;
	modes_ = TS440modes_;
	_mode_type = TS440_mode_type;
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
	freqA = def_freq = 14070000;
	freqB = 7070000;
	can_change_alt_vfo = true;

	has_mode_control =
	has_ptt_control =
		true;

	precision = 10;
	ndigits = 7;

}

void RIG_TS440::initialize()
{
	cmd = "AI0;"; // disable auto status
	sett("");
	sendCommand(cmd);
}

bool RIG_TS440::check ()
{
	return true;
	cmd = "ID;";
	get_trace(1, "check ID");
	int ret = wait_char(';', 6, 100, "check ID", ASC);
	gett("");
	if (ret < 6) return false;
	return true;
}

/*
========================================================================
	frequency & mode data are contained in the IF; response

	IFggmmmkkkhhh snnnzrx yytdfcp;    30 characters including the ;

	gg~~hhh = Value as defined in FA Command
	s = "+" or "-" Value of RIT/XIT
	nnn = Value of RIT/XIT (n.nn kHz.)
	z = "0" (Not Used in TS-440)
	r = Value as defined in RT Command
	x = Value as defined in XT Command
	yy = Memory Channel No.
	t = 0 for Receive
		1 for Transmit
	d = Value as defined in MD Command
	f = Value as defined in FN Command
	c = Value as defined in SC Command
	p = Value as defined in SP Command

========================================================================
*/

int RIG_TS440::get_IF()
{
	cmd = "IF;";
	int ret = wait_char(';', 29, 100, "get PTT", ASC);
	gett("");
	return ret;
}

unsigned long int RIG_TS440::get_vfoA ()
{
	cmd = "FA;";

	get_trace(1, "get vfoA");
	int ret = wait_char(';', 14, 100, "get vfo A", ASC);
	gett("");
	if (ret < 14) return freqA;

	size_t p = replystr.rfind("FA");
	if (p == std::string::npos) return freqA;

	int f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replystr[p + n] - '0';
	freqA = f;
	return freqA;
}

void RIG_TS440::set_vfoA (unsigned long int freq)
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
	showresp(WARN, ASC, "set vfo A", cmd, "");
}

unsigned long int RIG_TS440::get_vfoB ()
{
	cmd = "FB;";
	get_trace(1, "get vfoB");
	int ret = wait_char(';', 14, 100, "get vfo B", ASC);
	gett("");
	if (ret < 14) return freqB;

	size_t p = replystr.rfind("FB");
	if (p == std::string::npos) return freqB;

	int f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replystr[p + n] - '0';
	freqB = f;
	return freqB;
}

void RIG_TS440::set_vfoB (unsigned long int freq)
{
	freqB = freq;
	cmd = "FB00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	set_trace(1, "set vfoA");
	sendCommand(cmd);
	sett("");
	showresp(WARN, ASC, "set vfo B", cmd, "");
}

void RIG_TS440::set_modeA(int val)
{
	showresp(WARN, ASC, "set mode A", "", "");
	modeA = val;
	cmd = "MD";
	cmd += TS440_mode_chr[val];
	cmd += ';';
	set_trace(1, "get modeA");
	sendCommand(cmd);
	sett("");
}

int RIG_TS440::get_modeA()
{
	int ret = get_IF();
	if (ret < 26) return modeA;
	size_t p = replystr.rfind("IF");
	if (p == std::string::npos) return modeA;
	modeA = replystr[p+26] - '1'; // 0 - 5
	return modeA;
}

void RIG_TS440::set_modeB(int val)
{
	showresp(WARN, ASC, "set mode B", "", "");
	modeB = val;
	cmd = "MD";
	cmd += TS440_mode_chr[val];
	cmd += ';';
	set_trace(1, "set modeB");
	sendCommand(cmd);
	sett("");
}

int RIG_TS440::get_modeB()
{
	int ret = get_IF();
	if (ret < 26) return modeB;
	size_t p = replystr.rfind("IF");
	if (p == std::string::npos) return modeB;
	modeB = replystr[p+26] - '1'; // 0 - 5
	return modeB;
}

int RIG_TS440::get_modetype(int n)
{
	return _mode_type[n];
}

void RIG_TS440::selectA()
{
	set_trace(1, "selectA");
	cmd = "FN0;";
	sendCommand(cmd);
	sett("");
	showresp(WARN, ASC, "select A", "", "");
	inuse = onA;
}

void RIG_TS440::selectB()
{
	set_trace(1, "selectB");
	cmd = "FN1;";
	sendCommand(cmd);
	sett("");
	showresp(WARN, ASC, "select A", "", "");
	inuse = onB;
}

int RIG_TS440::get_PTT()
{
	get_trace(1, "get PTT");
	int ret = get_IF();
	if (ret < 26) return ptt_;
	ptt_ = (replystr[25] == '1');
	return ptt_;
}

/*   NOT AVAILABLE IN TS440

// SM cmd 0 ... 100 (rig values 0 ... 15)
int RIG_TS440::get_smeter()
{
	cmd = "SM;";
	get_trace(1, "get Smeter");
	int ret = wait_char(';', 7, 100, "get smeter", ASC);
	gett("");
	if (ret < 7) return 0;

	size_t p = replystr.rfind("SM");
	if (p == std::string::npos) return 0;

	replystr[p + 6] = 0;
	int mtr = atoi(&replystr[p + 2]);
	mtr = (mtr * 100) / 30;
	return mtr;
}

// RM cmd 0 ... 100 (rig values 0 ... 8)
int RIG_TS440::get_swr()
{
	cmd = "RM;";
	get_trace(1, "get SWR");
	int ret = wait_char(';', 8, 100, "get swr", ASC);
	gett("");
	if (ret < 8) return 0;

	size_t p = replystr.rfind("RM");
	if (p == std::string::npos) return 0;

	replystr[p + 7] = 0;
	int mtr = atoi(&replystr[p + 3]);
	mtr = (mtr * 50) / 30;
	return mtr;
}

static std::string bw_str = "FL001001;";

int RIG_TS440::get_bwA()
{
	cmd = "FL;";
	get_trace(1, "get bwA");
	int ret = wait_char(';', 9, 100, "get bwA", ASC);
	gett("");
	if (ret < 9) return bwA;
	bw_str = replystr;
	size_t p = replystr.rfind("FL");
	if (p == std::string::npos) return bwA;

	replystr[p + 8] = 0;
	int bw = 0;
	while (TS440_filters[bw]) {
		if (strcmp(&replystr[p + 5], TS440_filters[bw]) == 0)
			return bwA = bw;
		bw++;
	}
	return bwA;
}

void RIG_TS440::set_bwA(int val)
{
	get_bwA();
	showresp(WARN, ASC, "set bw A", "", "");
	bwA = val;
	cmd = bw_str.substr(0, 5);
	cmd.append(TS440_filters[val]).append(";");
	set_trace(1, "set bwA");
	sendCommand(cmd);
	sett("");
}

int RIG_TS440::get_bwB()
{
	cmd = "FL;";
	get_trace(1, "get bwB");
	int ret = wait_char(';', 9, 100, "get bwB", ASC);
	gett("");
	if (ret < 9) return bwB;
	bw_str = replystr;
	size_t p = replystr.rfind("FL");
	if (p == std::string::npos) return bwB;

	replystr[p + 8] = 0;
	int bw = 0;
	while (TS440_filters[bw]) {
		if (strcmp(&replystr[p + 5], TS440_filters[bw]) == 0)
			return bwB = bw;
		bw++;
	}
	return bwB;
}

void RIG_TS440::set_bwB(int val)
{
	get_bwB();
	showresp(WARN, ASC, "set bw B", "", "");
	bwB = val;
	cmd = bw_str.substr(0, 5);
	cmd.append(TS440_filters[val]).append(";");
	set_trace(1, "set bwB");
	sendCommand(cmd);
	sett("");
}

int RIG_TS440::def_bandwidth(int val)
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

bool RIG_TS440::can_split()
{
	return true;
}

void RIG_TS440::set_split(bool val)
{
	set_trace(1, "set split");
	if (val) {
		cmd = "FR0;";
		sendCommand(cmd);
	sett("");
		cmd = "FT1;";
		sendCommand(cmd);
	sett("");
	} else {
		cmd = "FR0;";
		sendCommand(cmd);
	sett("");
		cmd = "FT0;";
		sendCommand(cmd);
	sett("");
	}
}

int RIG_TS440::get_split()
{
	int ret = get_IF();
	if (ret < 38) return split;
	size_t p = replystr.rfind("IF");
	if (p == std::string::npos) return split;
	split = replystr[p+32] ? true : false;
	return split;
}

// Tranceiver PTT on/off
void RIG_TS440::set_PTT_control(int val)
{
	set_trace(1, "set PTT");
	if (val) sendCommand("TX;");
	else	 sendCommand("RX;");
	sett("");
	ptt_ = val;
}


*/
