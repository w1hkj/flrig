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

#include "FT990a.h"
#include "rig.h"

static const char FT990Aname_[] = "FT-990A";
static const char *FT990Amodes_[] =
{ "LSB", "USB", "CW2.4", "CW500",
  "AM2.4", "FM",
  "RTTY(L)", "RTTY(U)", "PKT(L)", "PKT(FM)", NULL};

static const int FT990A_def_bw[] = {
0, 0, 0, 2,
0, 0, 
0, 0, 0, 0 };

static const int FT990A_mode_val[] = {
0, 1, 2, 3,
4, 5, 6,
8, 9, 10, 11 };

static const char FT990A_mode_type[] = {
'L', 'U', 'L', 'L',
'U', 'U',
'L', 'U', 'L', 'U' };

static const char *FT990Awidths_[] =
{ "2400", "2000", "500", "250", NULL};

static int FT990A_bw_vals[] = {
1,2,3,4,WVALS_LIMIT};

static const int FT990A_bw_val[] =
{ 0, 1, 2, 3 };

RIG_FT990A::RIG_FT990A() {
	name_ = FT990Aname_;
	modes_ = FT990Amodes_;
	bandwidths_ = FT990Awidths_;
	bw_vals_ = FT990A_bw_vals;

	comm_baudrate = BR4800;
	stopbits = 2;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 100;
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	afreq = bfreq = A.freq = B.freq = 14070000;
	amode = bmode = A.imode = B.imode = 1;
	aBW = bBW = A.iBW = B.iBW = 2;

	precision = 10;
	ndigits = 9;


	has_get_info = false;

	has_split = has_split_AB =
	has_smeter =
	has_power_out =
	has_swr_control =
	has_mode_control =
	has_bandwidth_control =
	has_ptt_control = true;

}

int  RIG_FT990A::adjust_bandwidth(int m)
{
	if (m == 0 || m == 1 || m == 5) return 2;
	if (m == 2 || m == 3 ) return 1;
	if (m == 4 || m == 6 || m == 7) return 3;
	return 2;
}

void RIG_FT990A::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
	replystr.clear();
}

void RIG_FT990A::initialize()
{
	init_cmd();
	cmd[4] = 0x0E;
	sendCommand(cmd);
}

void RIG_FT990A::selectA()
{
	init_cmd();
	cmd[4] = 0x05;
	sendCommand(cmd);
	showresp(WARN, HEX, "select A", cmd, "");
}

void RIG_FT990A::selectB()
{
	init_cmd();
	cmd[3] = 0x01;
	cmd[4] = 0x05;
	sendCommand(cmd);
	showresp(WARN, HEX, "select B", cmd, "");
}

void RIG_FT990A::set_split(bool val)
{
	split = val;
	init_cmd();
	cmd[3] = val ? 0x01 : 0x00;
	cmd[4] = 0x01;
	sendCommand(cmd);
	if (val)
		showresp(WARN, HEX, "set split ON", cmd, "");
	else
		showresp(WARN, HEX, "set split OFF", cmd, "");
}

bool RIG_FT990A::get_info()
{
	return false;
}

long RIG_FT990A::get_vfoA ()
{
	return A.freq;
}

void RIG_FT990A::set_vfoA (long freq)
{
	A.freq = freq;
	freq /=10;
	cmd = to_bcd_be(freq, 8);
	cmd += 0x0A;
	sendCommand(cmd);
	showresp(WARN, HEX, "set freq A", cmd, "");
}

int RIG_FT990A::get_modeA()
{
	return A.imode;
}

void RIG_FT990A::set_modeA(int val)
{
	A.imode = val;
	init_cmd();
	cmd[3] = FT990A_mode_val[val];
	cmd[4] = 0x0C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set mode A", cmd, "");
}

void RIG_FT990A::set_bwA (int val)
{
	A.iBW = val;
	init_cmd();
	cmd[3] = FT990A_bw_val[val];
	cmd[4] = 0x8C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set BW A", cmd, "");
}


int RIG_FT990A::get_bwA()
{
	return A.iBW;
}

long RIG_FT990A::get_vfoB()
{
	return B.freq;
}

void RIG_FT990A::set_vfoB(long freq)
{
	B.freq = freq;
	freq /=10;
	cmd = to_bcd_be(freq, 8);
	cmd += 0x0A;
	sendCommand(cmd);
	showresp(WARN, HEX, "set freq B", cmd, "");
}

void RIG_FT990A::set_modeB(int val)
{
	B.imode = val;
	init_cmd();
	cmd[3] = FT990A_mode_val[val];
	cmd[4] = 0x0C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set mode B", cmd, "");
}

int  RIG_FT990A::get_modeB()
{
	return B.imode;
}

void RIG_FT990A::set_bwB(int val)
{
	B.iBW = val;
	init_cmd();
	cmd[3] = FT990A_bw_val[val];
	cmd[4] = 0x8C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set bw B", cmd, "");
}

int  RIG_FT990A::get_bwB()
{
	return B.iBW;
}

int  RIG_FT990A::def_bandwidth(int m)
{
	return FT990A_def_bw[m];
}

// Tranceiver PTT on/off
void RIG_FT990A::set_PTT_control(int val)
{
	init_cmd();
	if (val) cmd[3] = 1;
	cmd[4] = 0x0F;
	sendCommand(cmd);
	if (val)
		showresp(WARN, HEX, "set PTT ON", cmd, "");
	else
		showresp(WARN, HEX, "set PTT OFF", cmd, "");
}

int RIG_FT990A::get_smeter()
{
	init_cmd();
	cmd[4] = 0xF7;
	int ret = waitN(5, 100, "S-meter");
	if (ret < 5) return 0;
	int sval = (unsigned char)replybuff[0];
	if (sval < 90) sval = 90;
	if (sval > 200) sval = 200;
	if (sval < 120) sval = 250 - 5 * sval / 3;
	else sval = 125 - 5 * sval / 8;
	return sval;
}

int RIG_FT990A::get_swr()
{
	return 0;
}

int RIG_FT990A::get_power_out()
{
	init_cmd();
	cmd[4] = 0xF7;
	int ret = waitN(5, 100, "Power out");
	if (ret < 5) return 0;
	int sval = (unsigned char)replybuff[0];
	if (sval < 90) sval = 90;
	if (sval > 200) sval = 200;
	if (sval < 120) sval = 250 - 5 * sval / 3;
	else sval = 125 - 5 * sval / 8;
	return sval;
}

