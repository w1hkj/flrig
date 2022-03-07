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

#include <stdio.h>
#include <stdlib.h>

#include <iostream>

#include "yaesu/FT857D.h"
#include "support.h"

#include "trace.h"

static const char FT857Dname_[] = "FT-857D";

static const char *FT857Dmodes_[] = {
		"LSB", "USB", "CW", "CW-R", "AM", "FM", "FM-N", "DIG", "PKT", NULL};
static const int FT857D_mode_val[] =  { 0x00, 0x01, 0x02, 0x03, 0x04, 0x08, 0X88, 0x0A, 0x0C };
static const char FT857D_mode_type[] = { 'L', 'U', 'U', 'L', 'U', 'U', 'U', 'U', 'U' };


static const char FT897Dname_[] = "FT-897D";

static const char *FT897Dmodes_[] = {
		"LSB", "USB", "CW", "CW-R", "AM", "FM", "FM-N", "DIG", "PKT", NULL};
static const int FT897D_mode_val[] =  { 0x00, 0x01, 0x02, 0x03, 0x04, 0x08, 0x88, 0x0A, 0x0C };
static const char FT897D_mode_type[] = { 'L', 'U', 'U', 'L', 'U', 'U', 'U', 'U', 'U' };
// note:
// 897D returns 0xFC for 9600 baud pkt, 0x0C for 1200 pkt
// ONLY accepts 0x0C for PKT mode selection ... go figure

static int num_modes = 9;
static const int *mode_vals;
static const char *mode_type;

RIG_FT857D::RIG_FT857D() {
// base class values
	name_ = FT857Dname_;
	modes_ = FT857Dmodes_;
	num_modes = 9;
	mode_vals = FT857D_mode_val;
	mode_type = FT857D_mode_type;

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

	has_ptt_control =
	has_split =
	has_split_AB =
	has_smeter =
	has_power_out =
	has_mode_control = 
	has_band_selection = true;

	precision = 10;
	ndigits = 9;

};

RIG_FT897D::RIG_FT897D() {
	name_ = FT897Dname_;
	modes_ = FT897Dmodes_;
	mode_vals = FT897D_mode_val;
	mode_type = FT897D_mode_type;

	num_modes = 9;
};

static void settle(int n)
{
	for (int i = 0; i < n/50; i++) {MilliSleep(50); Fl::awake();}
}

void RIG_FT857D::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

bool RIG_FT857D::check ()
{
	int wait = 5, ret = 0;
	init_cmd();
	cmd[4] = 0x03; // get vfo
	while ((ret = waitN(5, 100, "check")) < 5 && wait > 0) {
		init_cmd();
		cmd[4] = 0x03;
		wait--;
	}
	getthex("check");
	if (ret < 5) return false;
	return true;

}

unsigned long int RIG_FT857D::get_vfoA ()
{
	if (inuse == onB) return freqA;
	init_cmd();
	cmd[4] = 0x03;
	getr("get vfoA");
	int ret = waitN(5, 100, "get vfo A", HEX);
	getthex("  : ");
	if (ret == 5) {
		freqA = fm_bcd(replystr, 8) * 10;
		int mode = replystr[4] & 0xFF;
		for (int i = 0; i < num_modes; i++) {
			if (i == (num_modes - 1)) mode &= 0x0F;
			if (mode_vals[i] == mode) {
				modeA = i;
				break;
			}
		}
	}

	static char msg[50];
	snprintf(msg, sizeof(msg), "get vfoA: %lu, %s", freqA, modes_[modeA]);
	getr(msg);

	return freqA;
}

void RIG_FT857D::set_vfoA (unsigned long int freq)
{
	freqA = freq;
	freq /=10; // 857D does not support 1 Hz resolution
	cmd = to_bcd(freq, 8);
	cmd += 0x01;
	replystr.clear();
	sendCommand(cmd, 0, 50);
	settle(200);
	setthex("set_vfoA");
}

unsigned long int RIG_FT857D::get_vfoB ()
{
	if (inuse == onA) return freqB;
	init_cmd();
	cmd[4] = 0x03;
	getr("get vfoB");
	int ret = waitN(5, 100, "get vfo B", HEX);
	getthex("  : ");
	if (ret == 5) {
		freqB = fm_bcd(replystr, 8) * 10;
		int mode = replystr[4] & 0xFF;
		for (int i = 0; i < num_modes; i++) {
			if (i == (num_modes - 1)) mode &= 0x0F;
			if (mode_vals[i] == mode) {
				modeB = i;
				break;
			}
		}
	}

	static char msg[50];
	snprintf(msg, sizeof(msg), "get vfoB: %lu, %s", freqA, modes_[modeB]);
	getr(msg);

	return freqB;
}

void RIG_FT857D::set_vfoB (unsigned long int freq)
{
	freqB = freq;
	freq /=10; // 857D does not support 1 Hz resolution
	cmd = to_bcd(freq, 8);
	cmd += 0x01;
	replystr.clear();
	sendCommand(cmd, 0, 50);
	settle(200);
	setthex("set_vfoB");
}

int RIG_FT857D::get_modeA()
{
// read by get_vfoA
	getr("get modeA");
	return modeA;
}

int RIG_FT857D::get_modeB()
{
// read by get_vfoB
	getr("get modeB");
	return modeB;
}

int RIG_FT857D::get_modetype(int n)
{
	return mode_type[n];
}

void RIG_FT857D::set_modeA(int val)
{
	modeA = val;
	init_cmd();
	cmd[0] = mode_vals[val];
	cmd[4] = 0x07;
	replystr.clear();
	sendCommand(cmd, 0, 50);
	settle(200);
	setthex("set_modeA");
}

void RIG_FT857D::set_modeB(int val)
{
	modeB = val;
	init_cmd();
	cmd[0] = mode_vals[val];
	cmd[4] = 0x07;
	replystr.clear();
	sendCommand(cmd, 0, 50);
	settle(200);
	setthex("set_modeB");
}

// Tranceiver PTT on/off
void RIG_FT857D::set_PTT_control(int val)
{
	init_cmd();
	if (val) cmd[4] = 0x08;
	else	 cmd[4] = 0x88;
	replystr.clear();
	sendCommand(cmd, 0, 20);
	setthex("set_PTT");
	ptt_ = val;
}

// mod submitted by Rich, WA4SXZ, for power_out and smeter

int  RIG_FT857D::get_power_out(void)
{
	init_cmd();
	cmd[4] = 0xF7;
	int ret = waitN(1, 100, "get pout", HEX);
	getthex("get pout");
	if (ret == 1) {
		int fwdpwr = replystr[0] & 0x0F;
		fwdpwr = fwdpwr * 100 / 15;
		return fwdpwr;
	}
	return 0;
}

int  RIG_FT857D::get_smeter(void)
{
	init_cmd();
	cmd[4] = 0xE7;
	int ret = waitN(1, 100, "get smeter", HEX);
	getthex("get smeter");
	if (ret == 1) {
		int sval = replystr[0] & 0x0F;
		sval = (sval-1) * 100 / 15;
		return sval;
	}
	return 0;
}

void RIG_FT857D::selectA()
{
	init_cmd();
	cmd[4] = 0x81;
	sendCommand(cmd);
	setthex("Select VFO A");
	check();
	inuse = onA;
}

void RIG_FT857D::selectB()
{
	init_cmd();
	cmd[4] = 0x81;
	sendCommand(cmd);
	setthex("Select VFO B");
	check();
	inuse = onB;
}

void RIG_FT857D::set_split(bool val)
{
	split = val;
	replystr.clear();
	init_cmd();
	if (val) {
		cmd[4] = 0x02;
		sendCommand(cmd, 0, 100);
		showresp(INFO, HEX, "set split ON", cmd, "");
	} else {
		cmd[4] = 0x82;
		sendCommand(cmd, 0, 100);
		showresp(INFO, HEX, "set split OFF", cmd, "");
	}
	setthex("set_split");
}

extern bool PTT;
int  RIG_FT857D::get_split()
{
	init_cmd();
	cmd[4] = 0xF7; // get transmit status
	int ret = waitN(1, 100, "get TX status");
	getthex("get_split");
	if (ret == 0) return 0;
	split = (replystr[0] & 0x20) == 0x20;
	return split;
}

void RIG_FT857D::set_band_selection(int v)
{
	unsigned long int freq = 14070000L;
	int  mode = 0;
	switch (v) {
		case 1: freq = progStatus.f160; mode = progStatus.m160; break; // 160 meters
		case 2: freq = progStatus.f80;  mode = progStatus.m80;  break; // 80 meters
		case 3: freq = progStatus.f40;  mode = progStatus.m40;  break; // 40 meters
		case 4: freq = progStatus.f30;  mode = progStatus.m30;  break; // 30 meters
		case 5: freq = progStatus.f20;  mode = progStatus.m20;  break; // 20 meters
		case 6: freq = progStatus.f17;  mode = progStatus.m17;  break; // 17 meters
		case 7: freq = progStatus.f15;  mode = progStatus.m15;  break; // 15 meters
		case 8: freq = progStatus.f12;  mode = progStatus.m12;  break; // 12 meters
		case 9: freq = progStatus.f10;  mode = progStatus.m10;  break; // 10 meters
		case 10: freq = progStatus.f6;  mode = progStatus.m6;   break; // 6 meters
		case 11: freq = progStatus.f2;  mode = progStatus.m2;   break; // 2 meters
		case 12: freq = progStatus.f70; mode = progStatus.m70;  break; // 70 cent'
	}
	if (inuse == onB) {
		set_modeB(mode);
		set_vfoB(freq);
	} else {
		set_modeA(mode);
		set_vfoA(freq);
	}
}

void RIG_FT857D::set_tones(int tx, int rx)
{
	cmd.clear();
	cmd.assign(to_bcd(tx, 4));
	cmd.append(to_bcd(rx, 4));
	cmd += 0x0B;
	sendCommand(cmd, 0, 50);
	setthex("set tones");
}

// 0 - simplex, 1 - minus, 2 - plus
void RIG_FT857D::set_offset(int indx, int offset)
{
	cmd.clear();
	cmd += (indx == 0) ? 0x89 : (indx == 1) ? 0x09 : 0x49;
	cmd.append("   ");
	cmd += 0x09;
	sendCommand(cmd, 0, 50);
	setthex("set offset 1");

	offset *= 100;
	cmd.clear();
	cmd.assign(to_bcd(offset - (offset % 100), 8));
	cmd += 0xF9;
	sendCommand(cmd, 0, 50);
	setthex("set offset 2");
}
