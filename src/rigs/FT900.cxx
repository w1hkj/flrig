// ----------------------------------------------------------------------------
// Copyright (C) 2014
//              David Freese, W1HKJ
//
// 2015-10-04
// adapted from FT890.cxx by Ernst F. Schroeder DJ7HS
// 2015-10-09   DJ7HS
// added rig capability   has_split = has_split_AB = true
// set_vfoB must be same as set_vfoA  (cmd = 0x0A)
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

#include "FT900.h"

const char FT900name_[] = "FT-900";

const char *FT900modes_[] = {
		"LSB", "USB", "CW", "CW-N", "AM", "AM-N", "FM", NULL};
static const int FT900_mode_val[] =  { 0, 1, 2, 3, 4, 5, 6 };

static const char FT900_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'U', 'U' };

static const char *FT900widths_[] =
{ "wide", "narr", NULL};

static const int FT900_bw_val[] =
{ 0, 1 };

RIG_FT900::RIG_FT900() {
	name_ = FT900name_;
	modes_ = FT900modes_;
	bandwidths_ = FT900widths_;
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

	afreq = bfreq = A.freq = B.freq = 14070000;
	amode = bmode = A.imode = B.imode = 1;
	aBW = bBW = A.iBW = B.iBW = 0;
	precision = 10;

	has_smeter =
	has_power_out =
	has_get_info =
	has_ptt_control =
	has_split = has_split_AB =
	has_mode_control = true;

	precision = 10;
	ndigits = 7;

}

void RIG_FT900::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

void RIG_FT900::initialize()
{
}

void RIG_FT900::selectA()
{
	init_cmd();
	cmd[4] = 0x05;
	sendCommand(cmd);
	showresp(WARN, HEX, "select A", cmd, replystr);
}

void RIG_FT900::selectB()
{
	init_cmd();
	cmd[3] = 0x01;
	cmd[4] = 0x05;
	sendCommand(cmd);
	showresp(WARN, HEX, "select B", cmd, replystr);
}

void RIG_FT900::set_split(bool val)
{
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

int RIG_FT900::get_split()
{
	return split;
}

bool RIG_FT900::get_info()
{
	init_cmd();
	cmd[3] = 0x02;
	cmd[4] = 0x10;
//  after this command the FT-900 replies with 1 + 2 x 9 bytes of data
//  bytes 2..4 and 11..13 contain binary VFO data with 10 Hz resolution
//  byte 0 contains the split flag
//  bytes 7 and 16 contain the mode and bytes 9 and 18 contain the bandwidth
	int ret = waitN(19, 100, "get info", HEX);

	if (ret >= 19) {
		size_t p = ret - 19;
		afreq = 0;
		bfreq = 0;
		for (size_t n = 2; n < 5; n++) {
			afreq = afreq * 256 + (unsigned char)replybuff[p + n];
			bfreq = bfreq * 256 + (unsigned char)replybuff[p + 9 + n];
		}
		afreq = afreq * 10.0;
		bfreq = bfreq * 10.0;
		aBW = 0;   // normal BW
//		mode data is in bytes 7 and 16
//      bandwidth data is in bytes 9 and 18
		int sp = replybuff[p];
		int md = replybuff[p + 7];
		int bw = replybuff[p + 9];
		switch (md) {
			case 0 :   // LSB
				amode = 0;
				break;
			case 1 :   // USB
				amode = 1;
				break;
			case 2 :   // CW
				amode = (bw & 0x80) ? 3 : 2;
				aBW = (bw & 0x80) ? 1 : 0;
				break;
			case 3 :   // AM
				amode = (bw & 0x40) ? 5 : 4;
				aBW = (bw & 0x40) ? 1 : 0;
				break;
			case 4 :   // FM
				amode = 6;
				break;
			default :
				amode = 1;
		}

		bBW = 0;
		md = replybuff[p + 16];
		bw = replybuff[p + 18];
		switch (md) {
			case 0 :   // LSB
				bmode = 0;
				break;
			case 1 :   // USB
				bmode = 1;
				break;
			case 2 :   // CW
				bmode = (bw & 0x80) ? 3 : 2;
				bBW = (bw & 0x80) ? 1 : 0;
				break;
			case 3 :   // AM
				bmode = (bw & 0x40) ? 5 : 4;
				bBW = (bw & 0x40) ? 1 : 0;
				break;
			case 4 :   // FM
				bmode = 6;
				break;
			default :
				bmode = 1;
		}

		A.freq = afreq;
		A.imode = amode;
		A.iBW = aBW;

		B.freq = bfreq;
		B.imode = bmode;
		B.iBW = bBW;

		split = (sp & 0x40) ? 1 : 0;

		return true;
	}
	return false;
}

long RIG_FT900::get_vfoA ()
{
	return A.freq;
}

void RIG_FT900::set_vfoA (long freq)
{
	A.freq = freq;
	freq /=10; // FT-900 does not support 1 Hz resolution
	cmd = to_bcd_be(freq, 8);
	cmd += 0x0A;
	sendCommand(cmd);
	showresp(WARN, HEX, "set vfo A", cmd, replystr);
}

int RIG_FT900::get_modeA()
{
	return A.imode;
}

void RIG_FT900::set_modeA(int val)
{
	A.imode = val;
	init_cmd();
	cmd[3] = FT900_mode_val[val];
	cmd[4] = 0x0C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set mode A", cmd, replystr);
}

long RIG_FT900::get_vfoB()
{
	return B.freq;
}

void RIG_FT900::set_vfoB(long freq)
{
	B.freq = freq;
	freq /=10; // FT-900 does not support 1 Hz resolution
	cmd = to_bcd_be(freq, 8);
	cmd += 0x0A;
	sendCommand(cmd);
	showresp(WARN, HEX, "set vfo B", cmd, replystr);
}

void RIG_FT900::set_modeB(int val)
{
	B.imode = val;
	init_cmd();
	cmd[3] = FT900_mode_val[val];
	cmd[4] = 0x0C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set mode B", cmd, replystr);
}

int  RIG_FT900::get_modeB()
{
	return B.imode;
}

// Tranceiver PTT on/off
void RIG_FT900::set_PTT_control(int val)
{
	init_cmd();
	if (val) cmd[3] = 1;
	else	 cmd[3] = 0;
	cmd[4] = 0x0F;
	sendCommand(cmd, 0);
	LOG_INFO("%s", str2hex(cmd.c_str(), 5));
}

int RIG_FT900::get_smeter()
{
	init_cmd();
	cmd[4] = 0xF7;
	int ret = waitN(5, 100, "get smeter", HEX);
	if (ret < 5) return 0;
	int sval = (unsigned char)(replybuff[ret - 2]);
	sval = sval * 100 / 255;
	return sval;
}

int RIG_FT900::get_power_out()
{
	init_cmd();
	cmd[4] = 0xF7;
	int ret = waitN(5, 100, "get pwr out", HEX);
	if (ret < 5) return 0;
	int sval = (unsigned char)(replybuff[ret - 2]);
	sval = sval * 100 / 255;
	return sval;
}
