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

// this uses an undocumented CAT READ command which
// reads but does not set the byte located at 0x0055 in the xcvr EEPROM
// bit 7 set == Vfo A/B in use, unset == Memory operation
// bit 0 set == Vfo A, unset == Vfo B

#include <iostream>
#include <sstream>

#include "yaesu/FT817BB.h"

#include "rigpanel.h"

static const char FT817BBname_[] = "FT-817BB";
static const char *FT817BBmodes_[] = {
		"LSB", "USB", "CW", "CW-R", "AM", "FM", "DIG", "PKT", NULL};
static const int FT817BB_mode_val[] =  { 0, 1, 2, 3, 4, 8, 0x0A, 0x0C };
static const char FT817BB_mode_type[] = { 'L', 'U', 'U', 'L', 'U', 'U', 'U', 'U' };

RIG_FT817BB::RIG_FT817BB() {
// base class values	
	name_ = FT817BBname_;
	modes_ = FT817BBmodes_;
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

//#define TEST 1

void RIG_FT817BB::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

static bool ft817BB_memory_mode = false;

void memory_label(void *)
{
	if (ft817BB_memory_mode) labelMEMORY->show();
	else  labelMEMORY->hide();
}

int ab = 0;

int RIG_FT817BB::get_vfoAorB()
{
#ifdef TEST
std::cout << "get_vfoAorB()" << std::endl;
return ab;
#endif
	return replystr[0] & 0x01;
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
	getthex("get active VFO");

	ft817BB_memory_mode = ((replystr[0] & 0x80) == 0x00);
	Fl::awake(memory_label);

	return replystr[0] & 0x01;
}

void RIG_FT817BB::selectA()
{
#ifdef TEST
std::cout << "selectA()" << std::endl;
ab = 0;
return;
#endif

	if (get_vfoAorB() == 0) return;
	if (ft817BB_memory_mode) return;
	init_cmd();
	cmd[4] = 0x81;
	sendCommand(cmd);
	showresp(INFO, HEX, "select VFO A", cmd, replystr);
	setthex("Select VFO A");
	MilliSleep(100);
	if (get_vfoAorB() == 0)
		sett("selectA() SUCCESS");
	else
		sett("selectA() FAILED");
}

void RIG_FT817BB::selectB()
{
#ifdef TEST
std::cout << "selectB()" << std::endl;
ab = 1;
return;
#endif
	if (get_vfoAorB() == 1) return;
	if (ft817BB_memory_mode) return;
	init_cmd();
	cmd[4] = 0x81;
	sendCommand(cmd);
	showresp(INFO, HEX, "select VFO B", cmd, replystr);
	setthex("Select VFO B");
	MilliSleep(100);
	if (get_vfoAorB() == 1)
		sett("selectB() SUCCESS");
	else
		sett("selectB() FAILED");
}

bool RIG_FT817BB::check ()
{
#ifdef TEST
std::cout << "check()" << std::endl;
return true;
#endif
	init_cmd();
	cmd[4] = 0x03;
	int ret = waitN(5, 100, "check");
	if (ret < 5) return false;
	getthex("check");
	return true;
}

unsigned long int RIG_FT817BB::get_vfoA ()
{
#ifdef TEST
std::cout << "get_vfoA()" << std::endl;
return freqA;
#endif
	if (get_vfoAorB() != 0) return freqA;
	if (ft817BB_memory_mode) return freqA;
	init_cmd();
	cmd[4] = 0x03;
	int ret = waitN(5, 100, "get vfoA");
	getthex("get_vfoA");
	if (ret < 5) {
		return freqA;
	}
	freqA = fm_bcd(replystr, 8) * 10;
	return freqA;
}

//void RIG_FT817BB::set_getACK() {
//	for (int i = 0; i < 5; i++) {
//		sendCommand(cmd, 0);
//		for (int j = 0; j < 10; j++) {
//			if (readResponse() == 1) return;
//			MilliSleep(50);
//		}
//	}
//}

void RIG_FT817BB::set_vfoA (unsigned long int freq)
{
#ifdef TEST
std::cout << "set_vfoA(" << freq << ")" << std::endl;
freqA = freq;
return;
#endif

	if (get_vfoAorB() != 0) return;
	if (ft817BB_memory_mode) return;
	freqA = freq;
	freq /=10; // 817BB does not support 1 Hz resolution
	cmd = to_bcd(freq, 8);
	cmd += 0x01;
	sendCommand(cmd);
	setthex("set_vfoA");
}

int RIG_FT817BB::get_modeA()
{
#ifdef TEST
std::cout << "get_modeA()" << std::endl;
return modeA;
#endif
	if (get_vfoAorB() != 0) return modeA;
	if (ft817BB_memory_mode) return modeA;
	init_cmd();
	cmd[4] = 0x03;
	int ret = waitN(5, 100, "get mode A");
	getthex("get_modeA");
	if (ret < 5) {
		return modeA;
	}
	int mode = replystr[4];
	for (int i = 0; i < 8; i++)
		if (FT817BB_mode_val[i] == mode) {
			modeA = i;
			break;
		}
	return modeA;
}

int RIG_FT817BB::get_modetype(int n)
{
	return FT817BB_mode_type[n];
}

void RIG_FT817BB::set_modeA(int val)
{
#ifdef TEST
std::cout << "set_modeA(" << val << ")" << std::endl;
modeA = val;
return;
#endif
	if (ft817BB_memory_mode) return;
	if (get_vfoAorB() != 0) return;

	init_cmd();
	cmd[0] = FT817BB_mode_val[val];
	cmd[4] = 0x07;
	sendCommand(cmd);
	setthex("set_modeA");
}

// VFO B ===============================================================
unsigned long int RIG_FT817BB::get_vfoB ()
{
#ifdef TEST
std::cout << "get_vfoB()" << std::endl;
return freqB;
#endif

	if (get_vfoAorB() != 1) return freqB;
	if (ft817BB_memory_mode) return freqB;
	init_cmd();
	cmd[4] = 0x03;
	int ret = waitN(5, 100, "get vfoB");
	getthex("get_vfoB");
	if (ret < 5) {
		return freqB;
	}
	freqB = fm_bcd(replystr, 8) * 10;
	return freqB;
}

void RIG_FT817BB::set_vfoB (unsigned long int freq)
{
std::cout << "set_vfoB(" << freq << ")" << std::endl;
#ifdef TEST
freqB = freq;
return;
#endif

	if (get_vfoAorB() != 1) return;
	if (ft817BB_memory_mode) return;
	freqB = freq;
	freq /=10; // 817BB does not support 1 Hz resolution
	cmd = to_bcd(freq, 8);
	cmd += 0x01;
	sendCommand(cmd);
	setthex("set_vfoB");
}

int RIG_FT817BB::get_modeB()
{
#ifdef TEST
std::cout << "get_modeB()" << std::endl;
return modeB;
#endif
	if (get_vfoAorB() != 1) return modeB;
	if (ft817BB_memory_mode) return modeB;
	init_cmd();
	cmd[4] = 0x03;
	int ret = waitN(5, 100, "get mode B");
	getthex("get_modeB");
	if (ret < 5) {
		return modeB;
	}
	int mode = replystr[4];
	for (int i = 0; i < 8; i++)
		if (FT817BB_mode_val[i] == mode) {
			modeB = i;
			break;
		}
	return modeB;
}

void RIG_FT817BB::set_modeB(int val)
{
#ifdef TEST
std::cout << "set_modeB(" << val << ")" << std::endl;
modeB = val;
return;
#endif
	int use = get_vfoAorB();
	if (get_vfoAorB() != 1) return;
	init_cmd();
	cmd[0] = FT817BB_mode_val[val];
	cmd[4] = 0x07;
	sendCommand(cmd);
	setthex("set_modeB");
}


//======================================================================
// Tranceiver PTT on/off
void RIG_FT817BB::set_PTT_control(int val)
{
#ifdef TEST
std::cout << "set_PTT_control(" << val << ")" << std::endl;
ptt_ = val;
return;
#endif
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

int  RIG_FT817BB::get_power_out()
{
#ifdef TEST
std::cout << "get_power_out()" << std::endl;
return 25;
#endif
	init_cmd();
	cmd[4] = 0xBD;
	int ret = waitN(2, 100, "get PWR/SWR/ALC");
	getthex("get_power_out");
	if (ret != 2) return 0;

	int fwdpwr = (replystr[0] & 0xF0) >> 4;
	swr = (replystr[1] & 0xF0) >> 4;
	alc = (replystr[0] & 0x0F);

	if (fwdpwr > 8) fwdpwr = 8;
	if (fwdpwr < 0) fwdpwr = 0;
	return pmeter_map[fwdpwr];
}

int  RIG_FT817BB::get_swr()
{
	return swr_map[swr];
}

int  RIG_FT817BB::get_alc()
{
	return alc_map[alc];
}

int  RIG_FT817BB::get_smeter()
{
#ifdef TEST
std::cout << "get_smeter()" << std::endl;
return 30;
#endif
	init_cmd();
	cmd[4] = 0xE7;
	int ret = waitN(1, 100, "get smeter");
	getthex("get_smeter");
	if (!ret) return 0;
	int sval = replystr[0] & 0x0F;
	if (sval < 0) sval = 0;
	if (sval > 15) sval = 15;
	return smeter_map[sval];
}

void RIG_FT817BB::set_split(bool val)
{
#ifdef TEST
std::cout << "set_split(" << val << ")" << std::endl;
split = val;
return;
#endif
	init_cmd();
	if (val) cmd[4] = 0x02;
	else     cmd[4] = 0x82;
	split = val;
	sendCommand(cmd);
	setthex("set_split");
	return;
}

extern bool PTT;
int  RIG_FT817BB::get_split()
{
#ifdef TEST
std::cout << "get_split()" << std::endl;
return split;
#endif
	if (!PTT) return split;
	init_cmd();
	cmd[4] = 0xF7; // get transmit status
	int ret = waitN(1, 100, "get TX status");
	getthex("get_split");
	if (ret == 0) return 0;
	split = (replystr[0] & 0x20) == 0x20;
	return split;
}

int RIG_FT817BB::power_scale()
{
    return 10;
}
