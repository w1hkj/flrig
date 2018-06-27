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

#include "FT847.h"
#include "rig.h"

static const char FT847name_[] = "FT-847";
static const char *FT847modes_[] = 
{ "LSB", "USB", "CW", "CW-R", "AM", "FM", "CW-N", "CW-NR", "AM-N", "FM-N", NULL};

static const int FT847_mode_val[] =
{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x08, 0x82, 0x83, 0x84, 0x88 };

static const char FT847_mode_type[] =
{ 'L', 'U', 'L', 'U', 'U', 'U', 'L', 'U', 'U', 'U' };

//static const int FT847_def_bw[] = { 2, 2, 1, 1, 3, 2, 2, 3 };
//static const char *FT847widths_[] = { "300", "500", "2400", "6000", NULL};
//static const int FT847_bw_val[] = { 0, 1, 2, 3 };

static const int sm[] = {0,2,4,6,8,11,14,16,19,22,25,28,31,34,37,40,
43,45,47,50,54,58,62,66,70,74,78,82,86,90,95,100};

static const int po[] = {0,2,3,4,5,6,7,8,9,10,11,12,14,16,18,20,
23,26,30,34,39,44,50,56,62,69,76,84,92,100,109,120};



RIG_FT847::RIG_FT847() {
	name_ = FT847name_;
	modes_ = FT847modes_;
//	bandwidths_ = FT847widths_;
	comm_baudrate = BR9600;
	stopbits = 2;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	afreq = A.freq = B.freq = 14070000;
	amode = A.imode = B.imode = 1;

	precision = 10;
	ndigits = 8;

	has_smeter =
	has_power_out =
	has_get_info =
//	has_bandwidth_control =
	has_mode_control =
	has_ptt_control = true;

}

void RIG_FT847::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

void RIG_FT847::initialize()
{
	init_cmd();
	sendCommand(cmd, 0); // CAT on
	cmd[4] = 0x8E; // satellite mode off
	replystr.clear();
	sendCommand(cmd);
	showresp(WARN, HEX, "init", cmd, replystr);
}

bool RIG_FT847::get_info()
{
	int ret = 0, i = 0;

	init_cmd();
	cmd[4] = 0x03;
	ret = waitN(5, 100, "get info", HEX);
	if (ret >= 5) {
		afreq = fm_bcd(replystr.substr(ret - 5), 8)*10;
		amode = replystr[ret - 1];
		for (i = 0; i < 10; i++) if (FT847_mode_val[i] == amode) break;
		if (i == 10) i = 1;
		amode = i;
		return true;
	}
	return false;
}

bool RIG_FT847::check ()
{
	init_cmd();
	cmd[4] = 0x03;
	int ret = waitN(5, 100, "check", HEX);
	if (ret >= 5) return true;
	return false;
}

long RIG_FT847::get_vfoA ()
{
	if (useB) return A.freq;
	if (get_info()) {
		A.freq = afreq;
		A.imode = amode;
		A.iBW = aBW;
	}
	return A.freq;
}

void RIG_FT847::set_vfoA (long freq)
{
	A.freq = freq;
	freq /=10; // 847 does not support 1 Hz resolution
	cmd = to_bcd(freq, 8);
	cmd += 0x01;
	replystr.clear();
	sendCommand(cmd);
	showresp(WARN, HEX, "set vfo A", cmd, replystr);
}

int RIG_FT847::get_modeA()
{
	return A.imode;
}

void RIG_FT847::set_modeA(int val)
{
	A.imode = val;
	init_cmd();
	cmd[0] = FT847_mode_val[val];
	cmd[4] = 0x07;
	replystr.clear();
	sendCommand(cmd);
	showresp(WARN, HEX, "set mode A", cmd, replystr);
}

long RIG_FT847::get_vfoB()
{
	if (!useB) return B.freq;
	if (get_info()) {
		B.freq = afreq;
		B.imode = amode;
		B.iBW = aBW;
	}
	return B.freq;
}

void RIG_FT847::set_vfoB(long freq)
{
	B.freq = freq;
	freq /=10; // 847 does not support 1 Hz resolution
	cmd = to_bcd(freq, 8);
	cmd += 0x01;
	replystr.clear();
	sendCommand(cmd);
	showresp(WARN, HEX, "set vfo B", cmd, replystr);
}

void RIG_FT847::set_modeB(int val)
{
	B.imode = val;
	init_cmd();
	cmd[0] = FT847_mode_val[val];
	cmd[4] = 0x07;
	replystr.clear();
	sendCommand(cmd);
	showresp(WARN, HEX, "set mode B", cmd, replystr);
}

int  RIG_FT847::get_modeB()
{
	return B.imode;
}

// Tranceiver PTT on/off
void RIG_FT847::set_PTT_control(int val)
{
	init_cmd();
	if (val) cmd[4] = 0x08;
	else cmd[4] = 0x88;
	replystr.clear();
	sendCommand(cmd);
	showresp(WARN, HEX, "set PTT", cmd, replystr);
	ptt_ = val;
}

int RIG_FT847::get_smeter()
{
	init_cmd();
	cmd[4] = 0xE7;
	int sval = 0;
	int ret = waitN(1, 100, "get smeter", HEX);
	if (ret >= 1)
		sval = sm[(replystr[ret - 1] & 0x1F)];
	return sval;
}

int RIG_FT847::get_power_out()
{
	init_cmd();
	cmd[4] = 0xF7;
	fwdpwr = 0;
	int ret = waitN(1, 100, "get power", HEX);
	if (ret >= 1)
		fwdpwr = po[(replystr[ret - 1] & 0x1F)];
	return fwdpwr;
}

