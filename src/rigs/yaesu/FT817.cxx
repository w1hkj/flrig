// ----------------------------------------------------------------------------
// Copyright (C) 2021
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
#include <iostream>
#include <sstream>

#include "yaesu/FT817.h"

#include "rigpanel.h"

//=============================================================================
// FT817
//=============================================================================

static const char FT817name_[] = "FT-817";
static const char *FT817modes_[] = {
		"LSB", "USB", "CW", "CW-R", "AM", "FM", "DIG", "PKT", NULL};
static const int FT817_mode_val[] =  { 0, 1, 2, 3, 4, 8, 0x0A, 0x0C };
static const char FT817_mode_type[] = { 'L', 'U', 'U', 'L', 'U', 'U', 'U', 'U' };

RIG_FT817::RIG_FT817() {
	name_ = FT817name_;
	modes_ = FT817modes_;
	comm_baudrate = BR4800;
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
	modeA = 1;
	bwA = 0;

	has_split =
	has_split_AB =
	has_swr_control =
	has_alc_control =
	has_smeter =
	has_power_out =
	has_ptt_control =
	has_mode_control = true;

	precision = 10;
	ndigits = 8;

	inuse = onNIL;
}

void RIG_FT817::init_cmd()
{
	cmd = "00000";
	replystr = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = replystr[i] = 0;
	MilliSleep(5);  // Needed for the old slow '817
}

void RIG_FT817::selectA()
{
	if (inuse == onA) return;
	init_cmd();
	cmd[4] = 0x81;
	sendCommand(cmd);
	inuse = onA;
	get_vfoA();
}

void RIG_FT817::selectB()
{
	if (inuse == onB) return;
	init_cmd();
	cmd[4] = 0x81;
	sendCommand(cmd);
	inuse = onB;
	get_vfoB();
}

bool RIG_FT817::check ()
{
	return true;
}

unsigned long int RIG_FT817::get_vfoA()
{
	if (inuse == onB) return freqA;

	init_cmd();
	cmd[4] = 0x03;
	int ret = 0;
	int repeat = 5;
	do {
		ret = waitN(5, 100, "get vfoA", HEX);
		getthex("get_vfoA");
		MilliSleep(100);
	} while (ret < 5 && repeat--);

	if (ret < 5) {
		gett("get vfoA FAILED");
		return freqA;
	}

	freqA = fm_bcd(replystr, 8) * 10;

	int mode = (replystr[4] & 0x0F);
	int i = 0;
	for (; i < 8; i++)
		if (FT817_mode_val[i] == mode) {
			modeA = i;
			break;
		}
	static char msg[50];
	snprintf(msg, sizeof(msg), "get vfoA: %lu, %s", freqA, FT817modes_[i]);
	getr(msg);

	return freqA;
}

void RIG_FT817::set_vfoA (unsigned long int freq)
{
	freqA = freq;
	freq /=10; // 817 does not support 1 Hz resolution
	cmd = to_bcd(freq, 8);
	cmd += 0x01;
	sendCommand(cmd);
	setthex("set_vfoA");
}

int RIG_FT817::get_modeA()
{
	return modeA;
}

int RIG_FT817::get_modetype(int n)
{
	return FT817_mode_type[n];
}

void RIG_FT817::set_modeA(int val)
{
	init_cmd();
	cmd[0] = FT817_mode_val[val];
	cmd[4] = 0x07;
	sendCommand(cmd);
	setthex("set_modeA");

	get_vfoA();
	int n = 0;
	while (modeA != val && n++ < 10) {
		MilliSleep(50);
		get_vfoA();
	}
	if (n == 10) LOG_ERROR("set_modeA failed");
}

// VFO B ===============================================================
unsigned long int RIG_FT817::get_vfoB ()
{
	if (inuse == onA) return freqB;
	init_cmd();
	cmd[4] = 0x03;
	int ret = 0;
	int repeat = 5;
	do {
		ret = waitN(5, 100, "get vfoB", HEX);
		getthex("get_vfoB");
		MilliSleep(100);
	} while (ret < 5 && repeat--);

	if (ret < 5) {
		gett("get vfoB FAILED");
		return freqB;
	}

	freqB = fm_bcd(replystr, 8) * 10;

	int mode = (replystr[4] & 0x0F);
	int i = 0;
	for ( ; i < 8; i++ )
		if (FT817_mode_val[i] == mode) {
			modeB = i;
			break;
		}
	static char msg[50];
	snprintf(msg, sizeof(msg), "get vfoB: %lu, %s", freqB, FT817modes_[i]);
	getr(msg);

	return freqB;
}

void RIG_FT817::set_vfoB (unsigned long int freq)
{
	freqB = freq;
	freq /=10; // 817 does not support 1 Hz resolution
	cmd = to_bcd(freq, 8);
	cmd += 0x01;
	sendCommand(cmd);
	setthex("set_vfoB");
}

int RIG_FT817::get_modeB()
{
	return modeB;
}

void RIG_FT817::set_modeB(int val)
{
	init_cmd();
	cmd[0] = FT817_mode_val[val];
	cmd[4] = 0x07;
	sendCommand(cmd);
	setthex("set_modeB");

	get_vfoB();
	int n = 0;
	while (modeB != val && n++ < 10) {
		MilliSleep(50);
		get_vfoB();
	}
	if (n == 10) LOG_ERROR("set_modeB failed");
}


//======================================================================
// Tranceiver PTT on/off
void RIG_FT817::set_PTT_control(int val)
{
	init_cmd();
	if (val) cmd[4] = 0x08;
	else	 cmd[4] = 0x88;
	sendCommand(cmd);
	setthex("set_PTT_control");
	ptt_ = val;
}

// mapping for smeter and power out
static int smeter_map[] = {
0, 7, 13, 19, 25, 30, 35, 40, 45, 50, 55, 61, 68, 77, 88, 100
};

// power out is scaled by 10 to allow display on flrig power scales
static int pmeter_map[] = {
  0,   5,    7,  10,  17,  25,  33,  41,  50 };
//0, 0.5, 0.75, 1.0, 1.7, 2.5, 3.3, 4.1, 5.0

static int swr_map[] = {
   0,  4,  8, 13, 25, 37, 60, 70, 80, 90, 100, 100, 100, 100, 100, 100};
// 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,  11,  12,  13,  14,  15

static int alc_map[] = {
   0,  20,  40,  60,  80, 100, 100, 100,  100, 100, 100, 100, 100, 100};
// 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,  11,  12,  13,  14,  15

static int swr;
static int alc;

// uses undocumented command 0xBD
// returns two bytes b0 b1
// b0 PWR|SWR
// b1 ALC|MOD

int  RIG_FT817::get_power_out()
{
	init_cmd();
	cmd[4] = 0xBD;
	int ret = waitN(2, 100, "get PWR/SWR/ALC", HEX);
	getthex("get_power_out");

	if (ret < 2) return 0;

	int fwdpwr = (replystr[0] & 0xF0) >> 4;
	swr = (replystr[1] & 0xF0) >> 4;
	alc = (replystr[0] & 0x0F);

	if (fwdpwr > 8) fwdpwr = 8;
	if (fwdpwr < 0) fwdpwr = 0;
	return pmeter_map[fwdpwr];
}

int  RIG_FT817::get_swr()
{
	return swr_map[swr];
}

int  RIG_FT817::get_alc()
{
	return alc_map[alc];
}

int  RIG_FT817::get_smeter()
{
	init_cmd();
	cmd[4] = 0xE7;
	int ret = waitN(1, 100, "get smeter", HEX);
	getthex("get_smeter");
	if (!ret) return 0;
	int sval = replystr[0] & 0x0F;
	if (sval < 0) sval = 0;
	if (sval > 15) sval = 15;
	return smeter_map[sval];
}

void RIG_FT817::set_split(bool val)
{
	init_cmd();
	if (val) cmd[4] = 0x02;
	else     cmd[4] = 0x82;
	split = val;
	sendCommand(cmd);
	setthex("set_split");
	return;
}

extern bool PTT;
int  RIG_FT817::get_split()
{
	if (!PTT) return split;
	init_cmd();
	cmd[4] = 0xF7; // get transmit status
	int ret = waitN(1, 100, "get TX status", HEX);
	getthex("get_split");
	if (ret == 0) return 0;
	split = (replystr[0] & 0x20) == 0x20;
	return split;
}

int RIG_FT817::power_scale()
{
    return 10;
}
