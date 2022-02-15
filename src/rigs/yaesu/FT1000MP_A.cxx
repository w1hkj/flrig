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

#include "status.h"
#include "trace.h"

#include "yaesu/FT1000MP_A.h"

static const char FT1000MP_Aname_[] = "FT-1000MP-A";

static const char *FT1000MP_A_modes[] = {
	"LSB",    "USB",
	"CW-U",   "CW-L",
	"AM",     "AM-syn",
	"FM",     "FM-W",
	"RTTY-L", "RTTY-U",
	"PKT-L",  "PKT-U",
	"PKT-FM", NULL
};

static const char FT1000MP_A_setmode[] = {
	0x00, 0x01,
	0x02, 0x03,
	0x04, 0x05,
	0x06, 0X07,
	0x08, 0x09,
	0x0A, 0x0A,
	0x0B
};

static const int FT1000MP_A_def_bw[] = {
	 6,  6,
	18, 18,
	 0,  0,
	 0,  0,
	18, 18,
	 6,  6,
	 0
};

static const char FT1000MP_A_mode_type[] = {
	'L', 'U',
	'U', 'L',
	'U', 'U',
	'U', 'U',
	'L', 'U',
	'L', 'U'
};

struct mode_pair { char a; char b;};

static const mode_pair FT1000MP_A_mode[] = {
{ '\x00', '\x00' }, // LSB
{ '\x01', '\x00' }, // USB
{ '\x02', '\x00' }, // CW-U
{ '\x02', '\x80' }, // CW-L
{ '\x03', '\x00' }, // AM
{ '\x03', '\x80' }, // AM-syn
{ '\x04', '\x00' }, // FM
{ '\x07', '\x00' }, // FM-W
{ '\x05', '\x00' }, // RTTY-L
{ '\x05', '\x80' }, // RTTY-U
{ '\x06', '\x00' }, // PKT-L
{ '\x86', '\x00' }, // PKT-U
{ '\x06', '\x80' }, // PKT-FM
{ '\xFF', '\xFF' }  // end of list
};

//LSB    : 11 01 57 81 80 00 00 00 11 20 11 31 91 11 11 00
//USB    : 11 01 57 81 80 00 00 01 11 20 11 31 91 11 11 00
//CW-L   : 11 01 57 81 80 00 00 02 91 20 11 11 91 11 11 00
//CW-U   : 11 01 57 81 80 00 00 02 11 20 11 91 91 11 11 00
//RTTY-L : 11 01 57 81 80 00 00 05 11 20 11 11 91 11 11 00
//RTTY-U : 11 01 57 81 80 00 00 05 91 20 11 11 11 11 11 00
//PKT-FM : 11 01 57 81 80 00 00 06 91 20 11 31 11 11 11 00
//         11 01 57 81 80 00 00 06 91 20 11 11 91 11 11 00
//PKT-L  : 11 01 57 81 80 00 00 06 11 20 11 31 11 91 11 00
//         11 01 57 81 80 00 00 06 11 20 11 11 91 91 11 00
//PKT-U  : 11 01 57 81 80 00 00 86 11 20 11 31 11 91 11 00
//         11 01 57 81 80 00 00 86 11 20 11 11 91 11 11 00

static const char *FT1000MP_A_widths[] = {
"---/6.0", "---/2.4", "---/2.0", "---/500", "---/250",
"2.4/6.0", "2.4/2.4", "2.4/2.0", "2.4/500", "2.4/250",
"2.0/6.0", "2.0/2.4", "2.0/2.0", "2.0/500", "2.0/250",
"500/6.0", "500/2.4", "500/2.0", "500/500", "500/250",
"250/6.0", "250/2.4", "250/2.0", "250/500", "250/250", NULL };

static int FT1000MP_A_bw_vals[] = {
1,2,3,4,5,6,7,8,9,0,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25, WVALS_LIMIT};


RIG_FT1000MP_A::RIG_FT1000MP_A() {
// base class values
	name_ = FT1000MP_Aname_;
	modes_ = FT1000MP_A_modes;
	bandwidths_ = FT1000MP_A_widths;
	bw_vals_ = FT1000MP_A_bw_vals;

	comm_baudrate = BR4800;
	stopbits = 2;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	serloop_timing = 200;
	comm_rtscts = true;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	A.freq = 14070000;
	A.imode = 1;
	A.iBW = 1;
	B.freq = 3580000;
	B.imode = 1;
	B.iBW = 1;
	precision = 10;
	max_power = 200;
	tuner_on = false;

	has_smeter =
	has_power_out =
	has_swr_control =
	has_alc_control =
	has_mode_control =
	has_bandwidth_control =
	has_ptt_control =
	has_tune_control =
	has_split =
	has_get_info = true;
	has_auto_notch = true;

	precision = 10;
	ndigits = 7;

	progStatus.gettrace   =
	progStatus.settrace   =
	progStatus.serialtrace =
	progStatus.rigtrace    =
	progStatus.xmltrace    =
	progStatus.trace       = false;

	progStatus.gettrace   = true;
//	progStatus.settrace   = true;
//	progStatus.serialtrace = true;
//	progStatus.rigtrace    = true;
//	progStatus.xmltrace    = true;
//	progStatus.trace       = true;
};

void RIG_FT1000MP_A::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

void RIG_FT1000MP_A::initialize()
{
//	init_cmd();
//	cmd[4] = 0x81; // switch antenna tuner on
//	sendCommand(cmd,0);
//LOG_DEBUG("%s", str2hex(cmd.c_str(), 5));
//	selectA();
}

/*
 returns 32 bytes
 first 16 for vfo-A
 0 - band selection
 1,2,3,4 operating frequency
 5,6 clarifier offset
 7 operating mode
 8 if filter selection
 9 vfo/mem operating flags
 a,b,c,d,e,f NOT used
 repeated for vfo-B

Data captured by AF2M, Mitch

Mode changes bytes 7 / 8; counting from 0
         ---------------------||-||---------------------------------------------------------------------
LSB    : 08 00 57 71 00 00 00 00 11 00 11 01 81 81 11 0A 08 00 57 72 60 00 00 00 30 00 30 91 11 11 11 48
USB    : 08 00 57 71 00 00 00 01 11 00 11 01 81 81 11 0A 08 00 57 72 60 00 00 00 30 00 30 91 11 11 11 48
CW-L   : 08 00 57 75 10 00 00 02 01 00 11 01 81 81 11 0A 08 00 57 72 60 00 00 00 30 00 30 91 11 11 11 48
CW-U   : 08 00 57 75 10 00 00 02 11 00 11 91 81 81 11 0A 08 00 57 72 60 00 00 00 30 00 30 91 11 11 11 48
LSB    : 08 00 57 71 00 00 00 00 11 00 11 11 81 81 11 0A 08 00 57 72 60 00 00 00 30 00 30 91 11 11 11 48
RTTY-L : 08 00 57 63 B8 00 00 05 11 00 11 11 11 81 11 0A 08 00 57 72 60 00 00 00 30 00 30 91 11 11 11 48
RTTY-U : 08 00 57 7E 48 00 00 05 91 00 11 11 91 81 11 0A 08 00 57 72 60 00 00 00 30 00 30 91 11 11 11 48

Data captured by Peter, PA1POS

Query 0x00 0x00 0x00 0x02 0x10
                             |  |  |
LSB    : 11 01 57 81 80 00 00 00 11 20 11 31 91 11 11 00
USB    : 11 01 57 81 80 00 00 01 11 20 11 31 91 11 11 00
CW-L   : 11 01 57 81 80 00 00 02 91 20 11 11 91 11 11 00
CW-U   : 11 01 57 81 80 00 00 02 11 20 11 91 91 11 11 00
RTTY-L : 11 01 57 81 80 00 00 05 11 20 11 11 91 11 11 00
RTTY-U : 11 01 57 81 80 00 00 05 91 20 11 11 11 11 11 00
PKT-FM : 11 01 57 81 80 00 00 06 91 20 11 31 11 11 11 00
         11 01 57 81 80 00 00 06 91 20 11 11 91 11 11 00
PKT-L  : 11 01 57 81 80 00 00 06 11 20 11 31 11 91 11 00
         11 01 57 81 80 00 00 06 11 20 11 11 91 91 11 00
PKT-U  : 11 01 57 81 80 00 00 86 11 20 11 31 11 91 11 00
         11 01 57 81 80 00 00 86 11 20 11 11 91 11 11 00
                                  |______________________ IF SELECTOR (BW)
                               |_________________________ MODE
                         |__|____________________________ CLARIFIER OFFSET
             |__|__|__|__________________________________ FREQ
          |______________________________________________ BAND

Bandwidth changes (in LSB MODE)
8.215 filter is in thru mode
Only changing 455 filter, cycling thru from 6k, 2.4k, 2.0k,500, 250.
         ------------------------||---------------------------------------------------------------------
6k     : 08 00 57 71 00 00 00 00 00 00 00 01 81 81 11 0A 08 00 57 72 60 00 00 00 30 00 30 91 11 11 11 48
2.4k   : 08 00 57 71 00 00 00 00 01 00 00 01 81 81 11 0A 08 00 57 72 60 00 00 00 30 00 30 91 11 11 11 48
2.0k   : 08 00 57 71 00 00 00 00 02 00 00 01 81 81 11 0A 08 00 57 72 60 00 00 00 30 00 30 91 11 11 11 48
500    : 08 00 57 71 00 00 00 00 03 00 00 01 81 81 11 0A 08 00 57 72 60 00 00 00 30 00 30 91 11 11 11 48
250    : 08 00 57 71 00 00 00 00 04 00 00 01 81 81 11 0A 08 00 57 72 60 00 00 00 30 00 30 91 11 11 11 48

8.215 filter is in 500 hz mode.
         ------------------------||---------------------------------------------------------------------
6k     : 08 00 57 71 00 00 00 00 40 00 00 01 81 81 11 0A 08 00 57 72 60 00 00 00 30 00 30 91 11 11 11 48
2.4k   : 08 00 57 71 00 00 00 00 41 00 00 01 81 81 11 0A 08 00 57 72 60 00 00 00 30 00 30 91 11 11 11 48
2.0k   : 08 00 57 71 00 00 00 00 42 00 00 01 81 81 11 0A 08 00 57 72 60 00 00 00 30 00 30 91 11 11 11 48
500    : 08 00 57 71 00 00 00 00 43 00 00 01 81 81 11 0A 08 00 57 72 60 00 00 00 30 00 30 91 11 11 11 48
250    : 08 00 57 71 00 00 00 00 44 00 00 01 81 81 11 0A 08 00 57 72 60 00 00 00 30 00 30 91 11 11 11 48

Mode changes bytes 7 / 8; counting from 0
         |byte 7| |byte 8|
LSB    : 00000000 00010001
USB    : 00000001 00010001
CW-L   : 00000010 00000001
CW-U   : 00000010 00010001
LSB    : 00000000 00010001
RTTY-L : 00000101 00010001
RTTY-U : 00000101 10010001

         |byte 8|
6k     : 00000000
2.4k   : 00000001
2.0k   : 00000010
500    : 00000011
250    : 00000100

8.215 filter is in 500 hz mode.
         |byte 8|
6k     : 01000000
2.4k   : 01000001
2.0k   : 01000010
500    : 01000011
250    : 01000100
*/
/*
const std::string LSBstr   = { '\x08', '\x00', '\x50', '\x42', '\x01', '\x00', '\x00', '\x00', '\x11', '\x00', '\x11', '\x01', '\x81', '\x81', '\x11', '\x0A', '\x08', '\x00', '\x57', '\x72', '\x60', '\x00', '\x00', '\x00', '\x30', '\x00', '\x30', '\x91', '\x11', '\x11', '\x11', '\x48' };
const std::string USBstr   = { '\x08', '\x00', '\x57', '\x71', '\x00', '\x00', '\x00', '\x01', '\x11', '\x00', '\x11', '\x01', '\x81', '\x81', '\x11', '\x0A', '\x08', '\x00', '\x57', '\x72', '\x60', '\x00', '\x00', '\x00', '\x30', '\x00', '\x30', '\x91', '\x11', '\x11', '\x11', '\x48' };
const std::string CWUstr   = { '\x08', '\x00', '\x57', '\x71', '\x00', '\x00', '\x00', '\x02', '\x11', '\x00', '\x11', '\x01', '\x81', '\x81', '\x11', '\x0A', '\x08', '\x00', '\x57', '\x72', '\x60', '\x00', '\x00', '\x00', '\x30', '\x00', '\x30', '\x91', '\x11', '\x11', '\x11', '\x48' };
const std::string CWLstr   = { '\x08', '\x00', '\x57', '\x71', '\x00', '\x00', '\x00', '\x02', '\x91', '\x00', '\x11', '\x01', '\x81', '\x81', '\x11', '\x0A', '\x08', '\x00', '\x57', '\x72', '\x60', '\x00', '\x00', '\x00', '\x30', '\x00', '\x30', '\x91', '\x11', '\x11', '\x11', '\x48' };
const std::string AMIstr   = { '\x08', '\x00', '\x57', '\x71', '\x00', '\x00', '\x00', '\x03', '\x11', '\x00', '\x11', '\x01', '\x81', '\x81', '\x11', '\x0A', '\x08', '\x00', '\x57', '\x72', '\x60', '\x00', '\x00', '\x00', '\x30', '\x00', '\x30', '\x91', '\x11', '\x11', '\x11', '\x48' };
const std::string AMSstr   = { '\x08', '\x00', '\x57', '\x71', '\x00', '\x00', '\x00', '\x03', '\x91', '\x00', '\x11', '\x01', '\x81', '\x81', '\x11', '\x0A', '\x08', '\x00', '\x57', '\x72', '\x60', '\x00', '\x00', '\x00', '\x30', '\x00', '\x30', '\x91', '\x11', '\x11', '\x11', '\x48' };
const std::string FMstr    = { '\x08', '\x00', '\x57', '\x71', '\x00', '\x00', '\x00', '\x04', '\x11', '\x00', '\x11', '\x01', '\x81', '\x81', '\x11', '\x0A', '\x08', '\x00', '\x57', '\x72', '\x60', '\x00', '\x00', '\x00', '\x30', '\x00', '\x30', '\x91', '\x11', '\x11', '\x11', '\x48' };
const std::string FMWstr   = { '\x08', '\x00', '\x57', '\x71', '\x00', '\x00', '\x00', '\x07', '\x11', '\x00', '\x11', '\x01', '\x81', '\x81', '\x11', '\x0A', '\x08', '\x00', '\x57', '\x72', '\x60', '\x00', '\x00', '\x00', '\x30', '\x00', '\x30', '\x91', '\x11', '\x11', '\x11', '\x48' };
const std::string RTTYLstr = { '\x08', '\x00', '\x57', '\x71', '\x00', '\x00', '\x00', '\x05', '\x11', '\x00', '\x11', '\x01', '\x81', '\x81', '\x11', '\x0A', '\x08', '\x00', '\x57', '\x72', '\x60', '\x00', '\x00', '\x00', '\x30', '\x00', '\x30', '\x91', '\x11', '\x11', '\x11', '\x48' };
const std::string RTTYUstr = { '\x08', '\x00', '\x57', '\x71', '\x00', '\x00', '\x00', '\x05', '\x91', '\x00', '\x11', '\x01', '\x81', '\x81', '\x11', '\x0A', '\x08', '\x00', '\x57', '\x72', '\x60', '\x00', '\x00', '\x00', '\x30', '\x00', '\x30', '\x91', '\x11', '\x11', '\x11', '\x48' };
const std::string PKLstr   = { '\x08', '\x00', '\x57', '\x71', '\x00', '\x00', '\x00', '\x06', '\x11', '\x00', '\x11', '\x01', '\x81', '\x81', '\x11', '\x0A', '\x08', '\x00', '\x57', '\x72', '\x60', '\x00', '\x00', '\x00', '\x30', '\x00', '\x30', '\x91', '\x11', '\x11', '\x11', '\x48' };
const std::string PKUstr   = { '\x08', '\x00', '\x57', '\x71', '\x00', '\x00', '\x00', '\x86', '\x11', '\x00', '\x11', '\x01', '\x81', '\x81', '\x11', '\x0A', '\x08', '\x00', '\x57', '\x72', '\x60', '\x00', '\x00', '\x00', '\x30', '\x00', '\x30', '\x91', '\x11', '\x11', '\x11', '\x48' };
const std::string PKFstr   = { '\x08', '\x00', '\x57', '\x71', '\x00', '\x00', '\x00', '\x06', '\x91', '\x00', '\x11', '\x01', '\x81', '\x81', '\x11', '\x0A', '\x08', '\x00', '\x57', '\x72', '\x60', '\x00', '\x00', '\x00', '\x30', '\x00', '\x30', '\x91', '\x11', '\x11', '\x11', '\x48' };
const std::string BOGUSstr = { '\x08', '\x00', '\x57', '\x71', '\x00', '\x00', '\x00', '\x24', '\x99', '\x00', '\x11', '\x01', '\x81', '\x81', '\x11', '\x0A', '\x08', '\x00', '\x57', '\x72', '\x60', '\x00', '\x00', '\x00', '\x30', '\x00', '\x30', '\x91', '\x11', '\x11', '\x11', '\x48' };
*/
/*
const std::string data1 = {
0x08, 0x00, 0x57, 0x71, 0x00, 0x00, 0x00, 0x00, 0x43, 0x00, 0x00, 0x01, 0x81, 0x81, 0x11, 0x0A,
0x08, 0x00, 0x57, 0x72, 0x60, 0x00, 0x00, 0x00, 0x30, 0x00, 0x30, 0x91, 0x11, 0x11, 0x11, 0x48
};
const std::string data2[] = {
0x08, 0x00, 0x57, 0x71, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x81, 0x81, 0x11, 0x0A,
0x08, 0x00, 0x57, 0x72, 0x60, 0x00, 0x00, 0x00, 0x30, 0x00, 0x30, 0x91, 0x11, 0x11, 0x11, 0x48
};
const std::string data3[] = {
0x08, 0x00, 0x57, 0x71, 0x00, 0x00, 0x00, 0x00, 0x33, 0x00, 0x00, 0x01, 0x81, 0x81, 0x11, 0x0A,
0x08, 0x00, 0x57, 0x72, 0x60, 0x00, 0x00, 0x03, 0xB0, 0x00, 0x30, 0x91, 0x11, 0x11, 0x11, 0x48
};
const std::string amsync[] = {
0x08, 0x00, 0x57, 0x67, 0x01, 0x00, 0x00, 0x03, 0x80, 0x00, 0x11, 0x11, 0x91, 0x91, 0x11, 0x08,
0x08, 0x00, 0x57, 0x67, 0x02, 0x00, 0x00, 0x01, 0x11, 0x00, 0x11, 0x91, 0x11, 0x11, 0x11, 0xFA
};
*/

/*
 * Ken Rawlings tests
  
 Band     Radio             Flrig                Fldigi
160m     1.840.00       29449.08       29.440.080
  80m    3.547.92       56776.72        56766.720
  40m    7.036.00      112576.00       112576.000
  30m   10.113.25      161812.06       161812.060  
  20m   14.052.16      224834.60       224834.600
  17m   18.076.25      289220.12       289220.120
  15m   21.009.30      336148.94       336148.940
  12m   24.900.00      398400.00       398400.000
  10m   28.072.10      449153.64       449153.640
  10m   29.099.40      465590.40       465590.400 

08 00 55 73 04 00 00 06 11 00 11 B3 33
0B 00 AA E6 0A 00 00 02 B3 00 11 B4 11
11 01 55 CC 02 00 00 02 B3 00 22 33 11

3.500.00
08 00 55 73 0E 00 00 01 00 00 11 B3 33

7.000.00
0B 00 AA E6 0C 00 00 02 B3 00 11 B3 33

14.000.00
11 01 55 CC 06 00 00 02 B3 00 11 B3 33

const unsigned int testints[] = {\
0x08,0x00,0x55,0x73,0x0E,0x00,0x00,0x01,0x00,0x00,0x11,0xB3,0x33,0x91,0x11,0x08,\
0x08,0x01,0x55,0xCC,0x06,0x00,0x00,0x01,0x11,0x00,0x11,0x91,0x11,0x11,0x11,0x00 };

11 01 5B E8 10 00 00 02 B3 00 11 B3 11 11 11 00 0B 00 AB 4E B0 00 00 02 B3 00 11 B3 11 91 11 00
   -- -- -- -- frequency A = 

*/
unsigned int info[] = {
0x11, 0x01, 0x5B, 0xE8, 0x10, 0x00, 0x00, 0x02, 0xB3, 0x00, 0x11, 0xB3, 0x11, 0x11, 0x11, 0x00,
0x0B, 0x00, 0xAB, 0x4E, 0xB0, 0x00, 0x00, 0x02, 0xB3, 0x00, 0x11, 0xB3, 0x11, 0x91, 0x11, 0x00
};

bool RIG_FT1000MP_A::check()
{
	init_cmd();
	cmd[3] = 0x03;
	cmd[4] = 0x10;

	getr("check");
	int ret = waitN(32, 100, "check", ASC);
	geth();

	if (ret >= 32) return true;
	return false;
}

bool RIG_FT1000MP_A::get_info(void)
{
	int ret = 0;
	int md = 0;

	init_cmd();
	cmd[3] = 0x03;  // read both vfo's
	cmd[4] = 0x10;
	getr("get info");
	ret = waitN(32, 100, "get info", ASC);
	geth();

	if (ret >= 32) {

		size_t p = replystr.length() - 32;

		// vfo A data std::string
		A.freq = (replystr[p + 1] & 0x7F) << 8;
		A.freq = (A.freq + (replystr[p + 2] & 0xFF)) << 8;
		A.freq = (A.freq + (replystr[p + 3] & 0xFF)) <<8;
		A.freq = ((A.freq + (replystr[p + 4] & 0xF0)) >> 4) * 10;

		for (md = 0; md < 14; md++) {
			if ( ((FT1000MP_A_mode[md].a & 0xFF) == (replystr[p + 7] & 0xFF)) &&
				 ((FT1000MP_A_mode[md].b & 0xFF) == (replystr[p + 8] & 0x80)))
				break;
		}
		if (md == 13) md = 0;
		A.imode = md;

		A.iBW = 5*((replystr[p + 8] & 0x70) >> 4) + (replystr[p + 8] & 0x07);
		if (A.iBW > 24) A.iBW = 24;

		B.freq = (replystr[p + 17] & 0x7F) << 8;
		B.freq = (B.freq + (replystr[p + 18] & 0xFF)) << 8;
		B.freq = (B.freq + (replystr[p + 19] & 0xFF)) <<8;
		B.freq = ((B.freq + (replystr[p +20] & 0xF0)) >> 4) * 10;

		for (md = 0; md < 14; md++) {
			if ( ((FT1000MP_A_mode[md].a & 0xFF) == (replystr[p + 23] & 0xFF)) &&
				 ((FT1000MP_A_mode[md].b & 0xFF) == (replystr[p + 24] & 0x80)))
				break;
		}
		if (md == 13) md = 0;
		B.imode = md;

		B.iBW = 5*((replystr[p + 24] & 0x70) >> 4) + (replystr[p + 24] & 0x07);
		if (B.iBW > 24) B.iBW = 24;

		return true;
	}
	return false;
}

unsigned long int RIG_FT1000MP_A::get_vfoA ()
{
	get_info();
	return A.freq;
}

unsigned long int RIG_FT1000MP_A::get_vfoB ()
{
	get_info();
	return B.freq;
}

void RIG_FT1000MP_A::set_vfoA (unsigned long int freq)
{
	A.freq = freq;
	init_cmd();
	freq /=10; // 1000MP does not support 1 Hz resolution
	for (int i = 0; i < 4; i++) {
		cmd[i] = (unsigned char)(freq % 10); freq /= 10;
		cmd[i] |= (unsigned char)((freq % 10) * 16); freq /= 10;
	}
	cmd[4] = 0x0A;
	setr("set vfo A");
	sendCommand(cmd, 0);
	seth();
}

void RIG_FT1000MP_A::set_vfoB (unsigned long int freq)
{
	B.freq = freq;
	init_cmd();
	freq /=10; // 1000MP does not support 1 Hz resolution
	for (int i = 0; i < 4; i++) {
		cmd[i] = (unsigned char)(freq % 10); freq /= 10;
		cmd[i] |= (unsigned char)((freq % 10) * 16); freq /= 10;
	}
	cmd[4] = 0x8A;
	setr("set vfo B");
	sendCommand(cmd, 0);
	seth();
}

int RIG_FT1000MP_A::get_modeA()
{
	return A.imode;
}

int RIG_FT1000MP_A::get_modeB()
{
	return B.imode;
}

void RIG_FT1000MP_A::set_modeA(int val)
{
	A.imode = val;
	init_cmd();
	cmd[3] = val;
	if (val > 6) cmd[3]++;
	cmd[4] = 0x0C;
	setr("set mode A");
	sendCommand(cmd, 0);
	seth();
	get_info();
}

void RIG_FT1000MP_A::set_modeB(int val)
{
	B.imode = val;
	init_cmd();
	cmd[3] = val;
	if (val > 6) cmd[3]++;
	cmd[4] = 0x0C;
	setr("set mode B");
	sendCommand(cmd, 0);
	seth();
	get_info();
}

int RIG_FT1000MP_A::get_modetype(int n)
{
	return FT1000MP_A_mode_type[n];
}

int RIG_FT1000MP_A::get_bwA()
{
	return A.iBW;
}

void RIG_FT1000MP_A::set_bwA(int val)
{
	int first_if = val / 5;
	int second_if = val % 5;

	if (!first_if) first_if += 5;
	--first_if;

	if (!second_if) second_if += 5;
	--second_if;

	A.iBW = val;

	init_cmd();
	cmd[0] = 0x01; // 1st IF
	cmd[3] = first_if;
	cmd[4] = 0x8C;
	setr("set bw A 1st IF");
	sendCommand(cmd, 0);
	seth();

	cmd[0] = 0x02; // 2nd IF
	cmd[3] = second_if;
	setr("set bw A 2nd IF");
	sendCommand(cmd, 0);
	seth();
}

int RIG_FT1000MP_A::get_bwB()
{
	return B.iBW;
}

void RIG_FT1000MP_A::set_bwB(int val)
{
	B.iBW = val;
	int first_if = val / 5;
	int second_if = val % 5;
	if (!first_if) first_if += 5;
	--first_if;
	first_if += 0x80; // vfo-B
	if (!second_if) second_if += 5;
	--second_if;
	second_if += 0x80; // vfo-B

	init_cmd();
	cmd[0] = 0x01; // 1st IF
	cmd[3] = first_if;
	cmd[4] = 0x8C;
	setr("set bw B 1st IF");
	sendCommand(cmd, 0);
	seth();

	cmd[0] = 0x02; // 2nd IF
	cmd[3] = second_if;
	setr("set bw B 2nd IF");
	sendCommand(cmd, 0);
	seth();
}

int  RIG_FT1000MP_A::def_bandwidth(int m)
{
	return FT1000MP_A_def_bw[m];
}

int  RIG_FT1000MP_A::adjust_bandwidth(int m)
{
	return FT1000MP_A_def_bw[m];
}

void RIG_FT1000MP_A::selectA()
{
	init_cmd();
	cmd[4] = 0x05;
	setr("select A");
	sendCommand(cmd, 0);
	seth();
}

void RIG_FT1000MP_A::selectB()
{
	init_cmd();
	cmd[3] = 0x01;
	cmd[4] = 0x05;
	setr("select B");
	sendCommand(cmd, 0);
	seth();
}

void RIG_FT1000MP_A::set_split(bool val)
{
	split = val;
	init_cmd();
	cmd[3] = val ? 0x01 : 0x00;
	cmd[4] = 0x01;
	setr("set split");
	sendCommand(cmd, 0);
	seth();
}


// Tranceiver PTT on/off
void RIG_FT1000MP_A::set_PTT_control(int val)
{
	init_cmd();
	if (val) cmd[3] = 1;
	else	 cmd[3] = 0;
	cmd[4] = 0x0F;
	setr("set PTT");
	sendCommand(cmd, 0);
	seth();
	ptt_ = val;
}

void RIG_FT1000MP_A::tune_rig()
{
	init_cmd();
	cmd[4] = 0x82; // initiate tuner cycle
	setr("tune");
	sendCommand(cmd,0);
	seth();
}

// used to turn tuner ON/OFF
void RIG_FT1000MP_A::set_auto_notch(int v)
{
	tuner_on = v;
	init_cmd();
	cmd[3] = v ? 0x01 : 0x00;
	cmd[4] = 0x81; // start antenna tuner
	setr("tuner on/off");
	sendCommand(cmd,0);
	seth();
}

int RIG_FT1000MP_A::get_auto_notch()
{
	return tuner_on;
}

int  RIG_FT1000MP_A::get_power_out(void)
{
	float pwr;
	init_cmd();
	cmd[0] = 0x80;
	cmd[4] = 0xF7;

	getr("get power out");
	int ret = sendCommand(cmd);
	geth();

	if (ret < 5) return 0;

	pwr = (unsigned char)(replystr[ret - 5]);

	if (pwr <=53) {pwr /= 53; pwr = 10 * pwr * pwr; }
	else if (pwr <= 77) {pwr /= 77; pwr = 20 * pwr * pwr; }
	else if (pwr <= 98) {pwr /= 98; pwr = 30 * pwr * pwr; }
	else if (pwr <= 114) {pwr /= 114; pwr = 40 * pwr * pwr; }
	else if (pwr <= 130) {pwr /= 130; pwr = 50 * pwr * pwr; }
	else {pwr /= 177; pwr = 100 * pwr * pwr; }

	return (int)pwr;
}

int  RIG_FT1000MP_A::get_smeter(void)
{
	float val = 0;
	init_cmd();
	cmd[0] = 0x00;
	cmd[4] = 0xF7;

	getr("get smeter");
	int ret = waitN(5, 100, "get smeter", HEX);
	geth();

	if (ret < 5) return 0;

	val = (unsigned char)(replystr[ret-5]);
	if (val <= 15) val = 5;
	else if (val <=154) val = 5 + 45 * (val - 15) / (154 - 15);
	else val = 50 + 50 * (val - 154.0) / (255.0 - 154.0);

	return (int)val;
}

int  RIG_FT1000MP_A::get_swr(void)
{
	float val = 0;
	init_cmd();
	cmd[0] = 0x85;
	cmd[4] = 0xF7;

	getr("get swr");
	int ret = waitN(5, 100, "get swr", HEX);
	geth();

	if (ret < 5) return 0;

	val = (unsigned char)(replystr[ret-5]) - 10;
	val *=  (50.0 / 122.0);

	if (val < 0) val = 0;
	if (val > 100) val = 100;

	return (int)val;
}

int  RIG_FT1000MP_A::get_alc(void)
{
	unsigned char val = 0;
	init_cmd();
	cmd[0] = 0x81;
	cmd[4] = 0xF7;

	getr("get alc");
	int ret = waitN(5, 100, "get alc", HEX);
	geth();

	if (ret < 5) return 0;

	val = (unsigned char)(replystr[ret-5]);

	return val * 100 / 255;
}

