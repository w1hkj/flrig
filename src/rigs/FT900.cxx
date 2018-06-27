// ----------------------------------------------------------------------------
// Copyright (C) 2014
//              David Freese, W1HKJ
//
// 2015-10-04 adapted from FT890.cxx by Ernst F. Schroeder DJ7HS
//            the FT-900 has two vfos and can work split
//            but it cannot change the (hidden) alternate vfo
// 2015-12-03 1st stable version  DJ7HS
// 2016-04-03 call get_info() within get_vfoA and get_vfoB  DJ7HS
// 2017-06-16 changes in support.cxx made adaptations necessary  DJ7HS
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
#include "status.h"

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
	comm_dtrplus = false;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;

	afreq = bfreq = A.freq = B.freq = 14070000;
	amode = bmode = A.imode = B.imode = 1;
	aBW = bBW = A.iBW = B.iBW = 0;
	precision = 10;

	has_smeter =
	has_power_out =
	has_ptt_control =
	has_split = 
	has_split_AB =
	has_getvfoAorB =
	has_get_info = 
	has_mode_control = true;

	precision = 10;
	ndigits = 7;

}

void RIG_FT900::initialize()
{
	progStatus.poll_split = 1;     // allow pollimg for split info
	progStatus.poll_vfoAorB = 1;   // allow pollimg for vfo info
}

void RIG_FT900::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

void RIG_FT900::selectA()
{
	init_cmd();
	cmd[4] = 0x05;
	sendCommand(cmd);
	showresp(WARN, HEX, "select vfo A", cmd, replystr);
}

void RIG_FT900::selectB()
{
	init_cmd();
	cmd[3] = 0x01;
	cmd[4] = 0x05;
	sendCommand(cmd);
	showresp(WARN, HEX, "select vfo B", cmd, replystr);
}

void RIG_FT900::set_split(bool val)
{
	init_cmd();
	cmd[3] = val ? 0x01 : 0x00;
	cmd[4] = 0x01;
	sendCommand(cmd);
	if (val)
		showresp(WARN, HEX, "set split ON", cmd, replystr);
	else
		showresp(WARN, HEX, "set split OFF", cmd, replystr);
}

int RIG_FT900::get_split()
{
	return splitison;
}

int  RIG_FT900::get_vfoAorB()
{
//  get flags for vfoAorB and split
	init_cmd();
	cmd[4] = 0xFA;
	int ret = waitN(5, 100, "get flags info", HEX);
//  after this command the FT-900 replies with 3 bytes of flags and 2 bytes of dummy data
	if (ret >= 5) {
		size_t p = ret - 5;
		int sp = replybuff[p];
		splitison = (sp & 0x04) ? 1 : 0;     // 1 if split is set
		vfoAorB = (sp & 0x40) ? 1 : 0;       // 0 if vfoA, 1 if vfoB is in use
     	return vfoAorB;       
	}
	return -1;                                // -1 signals error
}

//void RIG_FT900::swapAB()    // works with a simple trick
//{
//	init_cmd();
//	cmd[4] = 0x85;			// copy active vfo to background vfo
//	sendCommand(cmd);
//	showresp(WARN, HEX, "copy active vfo to background vfo", cmd, replystr);

//	if (!useB) {
//		queA.push(vfoB);
//		B = vfoA;
//	} else {
//		queB.push(vfoA);
//		A = vfoB;
//	}
//}

bool RIG_FT900::check()
{
	init_cmd();
	cmd[3] = 0x03;
	cmd[4] = 0x10;
	int ret = waitN(18, 100, "check", HEX);
	if (ret >= 18) return true;
	return false;
}

bool RIG_FT900::get_info()
{
//  get the vfo, mode and bandwidth information
	init_cmd();
	cmd[3] = 0x03;
	cmd[4] = 0x10;
//  after this command the FT-900 replies with 2 x 9 bytes of data
//  bytes 1..3 contain binary data for vfoA with 10 Hz resolution
//  bytes 10..12 contain binary data for vfoB with 10 Hz resolution
//  bytes 6 and 15 contain the mode and bytes 8 and 17 contain the bandwidth

	int ret = waitN(18, 100, "get info", HEX);

	if (ret >= 18) {
		size_t p = ret - 18;
		afreq = 0;
		bfreq = 0;
		for (size_t n = 1; n < 4; n++) {
			afreq = afreq * 256 + (unsigned char)replybuff[p + n];
			bfreq = bfreq * 256 + (unsigned char)replybuff[p + 9 + n];
		}
		afreq = afreq * 10.0;
		bfreq = bfreq * 10.0;
		aBW = 0;   // normal BW
//		mode data for vfoA is in byte 6
//      	bandwidth data is in byte 8
		int md = replybuff[p + 6];
		int bw = replybuff[p + 8];
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
//		mode data for vfoB is in byte 15
//      	bandwidth data is in byte 17
		md = replybuff[p + 15];
		bw = replybuff[p + 17];
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

// Transceiver PTT on/off
void RIG_FT900::set_PTT_control(int val)
{
// make sure no other vfo except either vfoA or vfoB is used for transmit
     if (val) {	
          if (!useB) {	    
               selectA();
          } else {
               selectB();
          }
     }
// make sure that in case of split the transmit mode is shown correctly 
     if (splitison) {
          if (val) {
               if (!useB) {
                    vfo = &vfoB;
               } else {
                    vfo = &vfoA;
               }
          } else {
               if (!useB) {
                    vfo = &vfoA;
               } else {
                    vfo = &vfoB;
               }
          }
          Fl::awake(setModeControl);
     }
	init_cmd();
	if (val) cmd[3] = 1;
	else	 cmd[3] = 0;
	cmd[4] = 0x0F;
	sendCommand(cmd, 0);
	LOG_INFO("%s", str2hex(cmd.c_str(), 5));
	ptt_ = val;
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
