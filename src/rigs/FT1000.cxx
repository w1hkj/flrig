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

#include "FT1000.h"

static const char FT1000name_[] = "FT-1000 **";

static const char *FT1000modes_[] = {
	"LSB", "USB", "CW-2.4", "CW-500", "AM-6", 
	"AM-2.4", "FM", "FM", "RTTY-L", "RTTY-U",
	"PKT-LSB", "PKT-FM", NULL};

static const char FT1000_mode_type[] = {
	'L', 'U', 'L', 'L', 'U', 
	'U', 'U', 'U', 'L', 'U',
	'L', 'U' };

static const char *FT1000_widths[] = {
"2400", "2000", "500", "250", "6000", NULL };

static int FT1000_bw_vals[] = {
1,2,3,4,5, WVALS_LIMIT};


// FT1000 class
RIG_FT1000::RIG_FT1000() {
// base class values
	name_ = FT1000name_;
	modes_ = FT1000modes_;
	bandwidths_ = FT1000_widths;
	comm_baudrate = BR4800;
	stopbits = 2;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	serloop_timing = 5000; // every 5 seconds FT1000 is a very slow beast!
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	modeA = 1;
	bwA = 0;

	has_mode_control =
	has_bandwidth_control =
	has_ptt_control =
	has_tune_control = true;

	precision = 10;
	ndigits = 7;

};

// from hamlib
/*
yaesu_cmd_set_t
  { 0x00, 0x00, 0x00, 0x00, 0x01 }, // Split (OFF) 
  { 0x00, 0x00, 0x00, 0x01, 0x01 }, // Split (On)  
  { 0x00, 0x00, 0x00, 0x00, 0x02 }, // Recall Memory 
  { 0x00, 0x00, 0x00, 0x00, 0x03 }, // Memory Operations 
  { 0x00, 0x00, 0x00, 0x00, 0x04 }, // Lock (OFF) 
  { 0x00, 0x00, 0x00, 0x01, 0x04 }, // Lock (ON) 
  { 0x00, 0x00, 0x00, 0x00, 0x05 }, // Select VFO (A) 
  { 0x00, 0x00, 0x00, 0x01, 0x05 }, // Select VFO (B) 
  { 0x00, 0x00, 0x00, 0x00, 0x06 }, // Copy Memory Data to VFO A 
  { 0x00, 0x00, 0x00, 0x00, 0x07 }, // OP Freq Up 0.1MHz 
  { 0x00, 0x00, 0x01, 0x00, 0x07 }, // OP Freq Up 1MHz 
  { 0x00, 0x00, 0x00, 0x00, 0x08 }, // OP Freq Down 0.1MHz 
  { 0x00, 0x00, 0x01, 0x00, 0x08 }, // OP Freq Down 1MHz 
  { 0x00, 0x00, 0x00, 0x00, 0x09 }, // RX Clarifier (OFF) 
  { 0x00, 0x00, 0x00, 0x01, 0x09 }, // RX Clarifier (ON) 
  { 0x00, 0x00, 0x00, 0x80, 0x09 }, // TX Clarifier (OFF) 
  { 0x00, 0x00, 0x00, 0x81, 0x09 }, // TX Clarifier (ON) 
  { 0x00, 0x00, 0x00, 0xff, 0x09 }, // Clear Clarifier Offset 
  { 0x00, 0x00, 0x00, 0x00, 0x09 }, // Clarifier 
  { 0x00, 0x00, 0x00, 0x00, 0x0a }, // Set Op Freq 
  { 0x00, 0x00, 0x00, 0x00, 0x0c }, // OP Mode Set LSB 
  { 0x00, 0x00, 0x00, 0x01, 0x0c }, // OP Mode Set USB 
  { 0x00, 0x00, 0x00, 0x02, 0x0c }, // OP Mode Set CW 2.4KHz 
  { 0x00, 0x00, 0x00, 0x03, 0x0c }, // OP Mode Set CW 500Hz 
  { 0x00, 0x00, 0x00, 0x04, 0x0c }, // OP Mode Set AM 6KHz 
  { 0x00, 0x00, 0x00, 0x05, 0x0c }, // OP Mode Set AM 2.4KHz 
  { 0x00, 0x00, 0x00, 0x06, 0x0c }, // OP Mode Set FM 
  { 0x00, 0x00, 0x00, 0x08, 0x0c }, // OP Mode Set RTTY LSB 
  { 0x00, 0x00, 0x00, 0x09, 0x0c }, // OP Mode Set RTTY USB 
  { 0x00, 0x00, 0x00, 0x0a, 0x0c }, // OP Mode Set PKT LSB 
  { 0x00, 0x00, 0x00, 0x0b, 0x0c }, // OP Mode Set PKT FM 
  { 0x00, 0x00, 0x00, 0x00, 0x0e }, // Pacing 
  { 0x00, 0x00, 0x00, 0x00, 0x0f }, // PTT (OFF) 
  { 0x00, 0x00, 0x00, 0x01, 0x0f }, // PTT (ON) 
  { 0x00, 0x00, 0x00, 0x00, 0x10 }, // Update All Data (1508 bytes) 
  { 0x00, 0x00, 0x00, 0x01, 0x10 }, // Update Memory Ch Number 
  { 0x00, 0x00, 0x00, 0x02, 0x10 }, // Update Op Data 
  { 0x00, 0x00, 0x00, 0x03, 0x10 }, // Update VFO Data 
  { 0x00, 0x00, 0x00, 0x04, 0x10 }, // Update Memory Ch Data 
  { 0x00, 0x00, 0x00, 0x00, 0x81 }, // Tuner (OFF) 
  { 0x00, 0x00, 0x00, 0x01, 0x81 }, // Tuner (ON) 
  { 0x00, 0x00, 0x00, 0x00, 0x82 }, // Tuner (Start) 
  { 0x00, 0x00, 0x00, 0x00, 0x84 }, // Repeater Mode (OFF) 
  { 0x00, 0x00, 0x00, 0x01, 0x84 }, // Repeater Mode (Minus) 
  { 0x00, 0x00, 0x00, 0x02, 0x84 }, // Repeater Mode (Plus) 
  { 0x00, 0x00, 0x00, 0x00, 0x85 }, // Copy displayed VFO (A=B || B=A) 
  { 0x00, 0x00, 0x00, 0x00, 0x8C }, // Select Bandwidth 
  { 0x00, 0x00, 0x00, 0x00, 0x8E }, // Step Operating Frequency Up 
  { 0x00, 0x00, 0x00, 0x01, 0x8E }, // Step Operating Frequency Down 
  { 0x00, 0x00, 0x00, 0x00, 0xf7 }, // Read Meter 
  { 0x00, 0x00, 0x00, 0x00, 0xf8 }, // DIM Level 
  { 0x00, 0x00, 0x00, 0x00, 0xf9 }, // Set Offset for Repeater Shift 
  { 0x00, 0x00, 0x00, 0x00, 0xfa }, // Read Status Flags 
*/

void RIG_FT1000::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

int  RIG_FT1000::adjust_bandwidth(int m)
{
	if (m == 0 || m == 1 || m == 8 || m == 9 || m == 10 || m == 11)
		return 0;
	if ( m == 3 ) return 0;
	if ( m == 4 ) return 2;
	if ( m == 4 ) return 4;
	if ( m == 5 ) return 0;
	if ( m == 6 || m == 7 || m == 11) return 0;
	return 0;
}

long RIG_FT1000::get_vfoA ()
{
	init_cmd();
	cmd[3] = 2; cmd[4] = 0x10;
	int ret = waitN(17, 100, "get vfoA", HEX);
	if (ret < 17) return freqA;
	replybuff[9] = 0;
	freqA = fm_bcd(&replybuff[6], 8) * 10;
	bwA = replybuff[13] & 0x07;
	return freqA;
}

void RIG_FT1000::set_vfoA (long freq)
{
	freqA = freq;
	freq /=10; // 1000 does not support 1 Hz resolution
	cmd = to_bcd(freq, 8);
	cmd += 0x0A;
	sendCommand(cmd, 0);
}

int RIG_FT1000::get_modeA()
{
	init_cmd();
	cmd[4] = 0x0C;
	int ret = waitN(5, 100, "get modeA", HEX);
	if (ret < 5) return modeA;
	modeA = cmd[4];
	return modeA;
}

void RIG_FT1000::set_modeA(int val)
{
	modeA = val;
	init_cmd();
	cmd[3] = val;
	cmd[4] = 0x0C;
	sendCommand(cmd, 0);
}

int RIG_FT1000::get_modetype(int n)
{
	return FT1000_mode_type[n];
}

void RIG_FT1000::set_bwA(int n)
{
	init_cmd();
	cmd[3] = n;
	cmd[4] = 0x8C;
	sendCommand(cmd, 0);
	bwA = n;
}

int RIG_FT1000::get_bwA()
{
	return bwA;
}

// Tranceiver PTT on/off
void RIG_FT1000::set_PTT_control(int val)
{
	init_cmd();
	if (val) cmd[3] = 1;
	else	 cmd[3] = 0;
	cmd[4] = 0x0F;
	sendCommand(cmd, 0);
}

void RIG_FT1000::tune_rig()
{
	init_cmd();
	cmd[4] = 0x82;
	sendCommand(cmd, 0);
}

int  RIG_FT1000::get_power_out(void)
{
	init_cmd();
	cmd[4] = 0xF7;
	int ret = waitN(5, 100, "get pwr out", HEX);
	if (ret < 5) return 0;
	return replybuff[0] && 0x0F;
}

int  RIG_FT1000::get_smeter(void)
{
	init_cmd();
	cmd[4] = 0xF7;
	int ret = waitN(5, 100, "get smeter", HEX);
	if (ret < 5) return 0;
	return replybuff[0] && 0x0F;
}

