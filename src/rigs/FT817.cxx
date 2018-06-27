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
#include <iostream>
#include <sstream>

#include "FT817.h"

#include "rigpanel.h"

static const char FT817name_[] = "FT-817";
static const char *FT817modes_[] = {
		"LSB", "USB", "CW", "CW-R", "AM", "FM", "DIG", "PKT", NULL};
static const int FT817_mode_val[] =  { 0, 1, 2, 3, 4, 8, 0x0A, 0x0C };
static const char FT817_mode_type[] = { 'L', 'U', 'U', 'L', 'U', 'U', 'U', 'U' };

RIG_FT817::RIG_FT817() {
// base class values	
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
	has_getvfoAorB =
	has_vfoAB = 
	has_smeter =
	has_power_out =
	has_ptt_control =
	has_mode_control = true;

	precision = 10;
	ndigits = 8;

}

void RIG_FT817::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

// this uses an undocumented CAT command which
// reades the byte located at 0x0055 in the xcvr EEPROM
// bit 7 set == Vfo A/B in use, unset == Memory operation
// bit 0 set == Vfo A, unset == Vfo B

static bool ft817_memory_mode = false;

void memory_label(void *)
{
	if (ft817_memory_mode) labelMEMORY->show();
	else  labelMEMORY->hide();
}

int RIG_FT817::get_vfoAorB()
{
	init_cmd();
	cmd[1] = 0x55;
	cmd[4] = 0xBB;
	int ret = waitN(2, 100, "get active VFO", HEX);
	int i = 0;
	while (ret != 2 && i++ < 10) {
		ret = waitN(2, 100, "get active VFO", HEX);
	}
	if (i == 10) {
		return -1;
	}

	ft817_memory_mode = ((replystr[0] & 0x80) == 0x00);
	Fl::awake(memory_label);

	return replystr[0] & 0x01;
}

void RIG_FT817::selectA()
{
	if (get_vfoAorB() == 0) return;
	if (ft817_memory_mode) return;
	init_cmd();
	cmd[4] = 0x81;
	sendCommand(cmd);
	showresp(INFO, HEX, "select VFO A", cmd, replystr);
	if (get_vfoAorB() != 0) {
		selectA();
	}
}

void RIG_FT817::selectB()
{
	if (get_vfoAorB() == 1) return;
	if (ft817_memory_mode) return;
	init_cmd();
	cmd[4] = 0x81;
	sendCommand(cmd);
	showresp(INFO, HEX, "select VFO B", cmd, replystr);
	if (get_vfoAorB() != 1) {
		selectB();
	}
}

bool RIG_FT817::check ()
{
	init_cmd();
	cmd[4] = 0x03;
	int ret = waitN(5, 100, "check", HEX);
	if (ret >= 5) return true;
	return false;
}

long RIG_FT817::get_vfoA ()
{
	if (get_vfoAorB() == 1) return freqA;
	if (ft817_memory_mode) return freqA;
	init_cmd();
	cmd[4] = 0x03;
	int ret = waitN(5, 100, "get vfoA", HEX);
	if (ret < 5) {
		return freqA;
	}
	freqA = fm_bcd(replystr, 8) * 10;
	return freqA;
}

void RIG_FT817::set_getACK() {
	for (int i = 0; i < 5; i++) {
		sendCommand(cmd, 0);
		for (int j = 0; j < 10; j++) {
			if (readResponse() == 1) return;
			MilliSleep(50);
		}
	}
}

void RIG_FT817::set_vfoA (long freq)
{
	if (get_vfoAorB() == 1) return;
	if (ft817_memory_mode) return;
	freqA = freq;
	freq /=10; // 817 does not support 1 Hz resolution
	cmd = to_bcd(freq, 8);
	cmd += 0x01;
	sendCommand(cmd);
	showresp(INFO, HEX, "set freq A", cmd, replystr);
}

int RIG_FT817::get_modeA()
{
	if (get_vfoAorB() == 1) return modeA;
	if (ft817_memory_mode) return modeA;
	init_cmd();
	cmd[4] = 0x03;
	int ret = waitN(5, 100, "get mode A", HEX);
	if (ret < 5) {
		return modeA;
	}
	int mode = replystr[4];
	for (int i = 0; i < 8; i++)
		if (FT817_mode_val[i] == mode) {
			modeA = i;
			break;
		}
	return modeA;
}

int RIG_FT817::get_modetype(int n)
{
	return FT817_mode_type[n];
}

void RIG_FT817::set_modeA(int val)
{
	if (ft817_memory_mode) return;
	init_cmd();
	cmd[0] = FT817_mode_val[val];
	cmd[4] = 0x07;
	sendCommand(cmd);
	showresp(INFO, HEX, "set mode A", cmd, replystr);
MilliSleep(100);
	get_modeA();
	int n = 0;
	while (modeA != val && n++ < 10) {
		get_modeA();
	}
	if (n == 10) LOG_ERROR("set_modeA failed");
}

// VFO B ===============================================================
long RIG_FT817::get_vfoB ()
{
	if (get_vfoAorB() == 0) return freqB;
	if (ft817_memory_mode) return freqB;
	init_cmd();
	cmd[4] = 0x03;
	int ret = waitN(5, 100, "get vfoB", HEX);
	if (ret < 5) {
		LOG_ERROR("get_vfoB failed");
		return freqB;
	}
	freqB = fm_bcd(replystr, 8) * 10;
	return freqB;
}

void RIG_FT817::set_vfoB (long freq)
{
	if (get_vfoAorB() == 0) return;
	if (ft817_memory_mode) return;
	freqB = freq;
	freq /=10; // 817 does not support 1 Hz resolution
	cmd = to_bcd(freq, 8);
	cmd += 0x01;
	sendCommand(cmd);
	showresp(INFO, HEX, "set freq B", cmd, replystr);
}

int RIG_FT817::get_modeB()
{
	if (get_vfoAorB() == 0) return modeB;
	if (ft817_memory_mode) return modeB;
	init_cmd();
	cmd[4] = 0x03;
	int ret = waitN(5, 100, "get mode B", HEX);
	if (ret < 5) {
		LOG_ERROR("get mode B failed");
		return modeB;
	}
	int mode = replystr[4];
	for (int i = 0; i < 8; i++)
		if (FT817_mode_val[i] == mode) {
			modeB = i;
			break;
		}
	return modeB;
}

void RIG_FT817::set_modeB(int val)
{
	if (ft817_memory_mode) return;
	init_cmd();
	cmd[0] = FT817_mode_val[val];
	cmd[4] = 0x07;
	sendCommand(cmd);
	showresp(INFO, HEX, "set mode B", cmd, replystr);
MilliSleep(100);
	get_modeB();
	int n = 0;
	while (modeB != val && n++ < 10) {
		get_modeB();
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
	std::string s = "set PTT ";
	s.append(val ? "ON":"OFF");
	showresp(INFO, HEX, s.c_str(), cmd, replystr);
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
	if (ret != 2) return 0;

	int fwdpwr = (replystr[0] & 0xF0) >> 4;
	swr = (replystr[1] & 0xF0) >> 4;
	alc = (replystr[0] & 0x0F);
//	int mod = (replystr[1] & 0x0F);

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
	showresp(INFO, HEX, "set SPLIT", cmd, replystr);
	return;
}

extern bool PTT;
int  RIG_FT817::get_split()
{
	if (!PTT) return split;
	init_cmd();
	cmd[4] = 0xF7; // get transmit status
	int ret = waitN(1, 100, "get TX status", HEX);
	if (ret == 0) return 0;
	split = (replystr[0] & 0x20) == 0x20;
	return split;
}
