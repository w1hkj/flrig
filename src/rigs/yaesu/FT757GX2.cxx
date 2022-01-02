// ----------------------------------------------------------------------------
// Copyright (C) 2020
//              David Freese, W1HKJ
//              Michael Keitz, KD4QDM
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
// with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------
//
// Functions supported by this driver:
//  Read VFO A and B frequencies and mode.  Mode may be different in each VFO.
//  Read which VFO is controlling the on-air frequency.  This properly follows the transmit during split operation.
//  Select VFO A or B.  This must be done before entering a frequency
//  Enter frequency or mode into a VFO.  There are two routines for this but only work if the target VFO is active.	
//  Read / set split mode.
//  Read S meter (receiving only).  The radio does not digitize the power meter during transmit.
//

#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "util.h"

#include "yaesu/FT757GX2.h"

static const char FT757GX2name_[] = "FT-757GX2";

static const char *FT757GX2modes_[] = {
		"LSB", "USB", "CW", "CWN", "AM", "FM", NULL};
static const char FT757GX2_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'U' };

RIG_FT757GX2::RIG_FT757GX2() {
// base class values
	name_ = FT757GX2name_;
	modes_ = FT757GX2modes_;
	comm_baudrate = BR4800;
	stopbits = 2;
	comm_retries = 3;
	comm_wait = 100;   // UI: "Cmds" : time between commands.  Rig logic will reset if no byte received for 100 ms.
	comm_timeout = 200; // UI: "Retry intvl"
	serloop_timing = 500;  // UI: "Poll Interval"
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = false;
	comm_rtsptt = true;
	comm_dtrptt = false;
	modeA = 1;
	bwA = 0;

	has_split = true;
	has_split_AB = false;
	has_getvfoAorB = true;
	has_get_info = true;
	has_ptt_control = false;
	has_mode_control = true;
	has_smeter = true;
	

	precision = 10;
	ndigits = 8;
};

void RIG_FT757GX2::init_cmd() {
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

void RIG_FT757GX2::selectA() {
	init_cmd();
	cmd[3] = 0x00;
	cmd[4] = 0x05;
	sendCommand(cmd);
	showresp(INFO, HEX, "select A", cmd, replystr);
	inuse = onA;
}

void RIG_FT757GX2::selectB() {
	init_cmd();
	cmd[3] = 0x01;
	cmd[4] = 0x05;
	sendCommand(cmd);
	showresp(INFO, HEX, "select B", cmd, replystr);
	inuse = onB;
}

int RIG_FT757GX2::get_vfoAorB() {
	return inuse;
}

void RIG_FT757GX2::set_split(bool val) {
	split = val;
	init_cmd();
	cmd[3] = val ? 0x01 : 0x00;
	cmd[4] = 0x01;
	sendCommand(cmd);
	if (val)
		showresp(INFO, HEX, "set split ON", cmd, replystr);
	else
		showresp(INFO, HEX, "set split OFF", cmd, replystr);
}

int RIG_FT757GX2::get_split() {
	return split;
}

bool RIG_FT757GX2::check() {
	init_cmd();
	cmd[3] = 0x01;  // dummy poll the S-meter, see if rig responds.
	cmd[4] = 0x10;
	int ret = waitN(1, 100, "check", HEX);
	if (ret >= 1) return true;
	return false;
}

int RIG_FT757GX2::get_smeter() {
	// return value of this routine should be 0 to 100.
	// the radio outputs 10 with no signal and 0 with full signal.	
	int meter;
	
	init_cmd();
	cmd[3] = 0x01;  // Poll the S-meter, should return one byte.
	cmd[4] = 0x10;
	int ret = waitN(1, 100, "S meter", HEX);
	if (ret >= 1) {
		meter = (int)(replystr[0] & 0x0f);
		if (meter > 10) meter = 10;
		return 100 - (meter * 10);
	}
	return -1;
}

bool RIG_FT757GX2::get_info() {
	init_cmd();
	cmd[4] = 0x10;   // Returns 75 bytes; we only  care about the first 20
	int ret = waitN(75, 250, "get info", HEX);  // 75 bytes at 4800-8-N-2 takes at least 172 ms.
	showresp(DEBUG, HEX, "get info", cmd, replystr); 

	if (ret >= 20) {		
// returned frequency is same as set freq -- 4 bytes packed BCD 10 Hz resolution. 

		A.freq = fm_bcd_be(replystr.substr(10), 8) * 10;
		A.imode = replystr[14];
		B.freq = fm_bcd_be(replystr.substr(15), 8) * 10;
		B.imode = replystr[19];
		
	// not sure if needed to support legacy API?
		freqA = A.freq; freqB = B.freq; modeA = A.imode; modeB = B.imode;
		
	// parse the radio status flags.	(we also get dial lock, clarifier, M/VFO, Ham/Gen, and Scan Mode)
		split = (replystr[0] & 0x02) ? 1 : 0;  
		inuse = (replystr[0] & 0x08) ? 1 : 0;  // VFO B or A selected?
	// CAT cannot initiate PTT on the FT757GX2, but it can poll if rig is in TX mode.	
	// But Do not set ptt_  It causes flrig to stop polling when PTT (ptt_) goes active.	
	//	ptt_ = (replystr[0] & 0x20) ? 1 : 0;

		return true;
	}
	return false;
}

unsigned long int RIG_FT757GX2::get_vfoA () {
	return A.freq;
}

void RIG_FT757GX2::set_vfoA (unsigned long int freq) {
	if (inuse) return;  // B is in use, don't set it.
	A.freq = freq;
	freq /=10; // set with 10 Hz resolution
	cmd = to_bcd_be(freq, 8);
	cmd += 0x0A; // SET FREQUENCY (This will load the active VFO-- either A or B)
	SLOG_INFO("cmd: %s", str2hex(cmd.c_str(), cmd.length()));
	sendCommand(cmd);
}

int RIG_FT757GX2::get_modeA() {
	return A.imode;
}

void RIG_FT757GX2::set_modeA(int val) {
	if (inuse) return;  //  B in use, don't set.
	A.imode = val;
	init_cmd();
	cmd[3] = val;
	cmd[4] = 0x0C; // MODESEL  -- affects active VFO
	SLOG_INFO("cmd: %s", str2hex(cmd.c_str(), cmd.length()));
	sendCommand(cmd);
}

unsigned long int RIG_FT757GX2::get_vfoB () {
	return B.freq;
}

void RIG_FT757GX2::set_vfoB (unsigned long int freq) {  
	if (!inuse) return;  // inuse = A, so don't set.  
	B.freq = freq;
	freq /=10;
	cmd = to_bcd_be(freq, 8);
	cmd += 0x0A; // SET FREQUENCY
	SLOG_INFO("cmd: %s", str2hex(cmd.c_str(), cmd.length()));
	sendCommand(cmd);
}

int RIG_FT757GX2::get_modeB() {
	return B.imode;
}

void RIG_FT757GX2::set_modeB(int val) {
	if (!inuse) return;
	B.imode = val;
	init_cmd();
	cmd[3] = val;
	cmd[4] = 0x0C; // MODESEL
	SLOG_INFO("cmd: %s", str2hex(cmd.c_str(), cmd.length()));
	sendCommand(cmd);
}

