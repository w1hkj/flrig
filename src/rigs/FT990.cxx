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

#include "FT990.h"
#include "rig.h"

static const char FT990name_[] = "FT-990";
static const char *FT990modes_[] =
{ "LSB", "USB", "CW2.4", "CW500",
  "AM6.0", "AM2.4", "FM",
  "RTTY(L)", "RTTY(U)", "PKT(L)", "PKT(FM)", NULL};

static const int FT990_def_bw[] = {
0, 0, 0, 2,
4, 0, 0,
0, 0, 0, 0 };

static const int FT990_mode_val[] = {
0, 1, 2, 3,
4, 5, 6,
8, 9, 10, 11 };

static const char FT990_mode_type[] = {
'L', 'U', 'L', 'L',
'U', 'U', 'U',
'L', 'U', 'L', 'U' };

static const char *FT990widths_[] =
{ "2400", "2000", "500", "250", "6000", NULL};

static int FT990_bw_vals[] = {
1,2,3,4,5,WVALS_LIMIT};

static const int FT990_bw_val[] =
{ 0, 1, 2, 3, 4 };

RIG_FT990::RIG_FT990() {
	name_ = FT990name_;
	modes_ = FT990modes_;
	bandwidths_ = FT990widths_;
	bw_vals_ = FT990_bw_vals;

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


	has_split = has_split_AB =
	has_get_info =
	has_smeter =
	has_power_out =
	has_swr_control =
	has_mode_control =
	has_bandwidth_control =
	has_ptt_control = true;

}

int  RIG_FT990::adjust_bandwidth(int m)
{
	if (m == 0 || m == 1 || m == 5) return 2;
	if (m == 2 || m == 3 ) return 1;
	if (m == 4 || m == 6 || m == 7) return 3;
	return 2;
}

void RIG_FT990::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
	replystr.clear();
}

void RIG_FT990::initialize()
{
}

void RIG_FT990::selectA()
{
	init_cmd();
	cmd[4] = 0x05;
	sendCommand(cmd);
	showresp(WARN, HEX, "select A", cmd, replystr);
}

void RIG_FT990::selectB()
{
	init_cmd();
	cmd[3] = 0x01;
	cmd[4] = 0x05;
	sendCommand(cmd);
	showresp(WARN, HEX, "select B", cmd, replystr);
}

void RIG_FT990::set_split(bool val)
{
	split = val;
	init_cmd();
	cmd[3] = val ? 0x01 : 0x00;
	cmd[4] = 0x01;
	sendCommand(cmd);
	if (val)
		showresp(WARN, HEX, "set split ON", cmd, replystr);
	else
		showresp(WARN, HEX, "set split OFF", cmd, replystr);
}

bool RIG_FT990::check()
{
	init_cmd();
	cmd[3] = 0x00;
	cmd[4] = 0xFA;
	int ret = waitN(5, 100, "check");
	if (ret >= 5) return true;
	return false;
}

bool RIG_FT990::get_info()
{
	bool memmode = false, vfobmode = false;
	int pfreq, pmode, pbw;
	init_cmd();
	cmd[3] = 0x00;
	cmd[4] = 0xFA;
	int ret = waitN(5, 100, "Read flags");

	if (ret >= 5) {
		size_t p = ret - 5;
		memmode = ((replystr[p+1] & 0x10) == 0x10);
		vfobmode = ((replystr[p] & 0x02) == 0x02);
		if (memmode) return false;
		if (vfobmode && !useB) {
			useB = true;
			Fl::awake(highlight_vfo, (void *)0);
		} else if (!vfobmode && useB) {
			useB = false;
			Fl::awake(highlight_vfo, (void *)0);
		}
	}

	init_cmd();
	cmd[4] = 0x10; // update info
	cmd[0] = 0x02; // 1 16 byte sequences for current VFO / MEM
	ret = waitN(16, 100, "Read info");

	if (ret >= 16) {
		size_t p = ret - 16;
		// current VFO / MEM
		pfreq = 0;
		for (size_t n = 1; n < 5; n++)
			pfreq = pfreq * 256 + (unsigned char)replystr[p + n];
		pfreq = pfreq * 1.25; // 100D resolution is 1.25 Hz / bit for read

		int rmode = replystr[p + 7] & 0x07;
		switch (rmode) {
			case 0 : pmode = 0; break; // LSB
			case 1 : pmode = 1; break; // USB
			case 2 : pmode = 2; break; // CW
			case 3 : pmode = 5; break; // AM
			case 4 : pmode = 6; break; // FM
			case 5 : pmode = 8; break; // RTTY
			case 6 : pmode = 9; break; // PKT
			default : pmode = 1; break;
		}

		int rpbw = replystr[p + 8];
		pbw = rpbw & 0x05;
		if (pbw > 4) pbw = 4;
		if ((rpbw & 0x80) == 0x80) {
			if (pmode == 10) pmode = 11;
			if (pmode == 8) pmode = 9;
		}
		if (pmode == 6) pbw = 0;
		if (useB) {
			B.freq = pfreq; B.imode = pmode; B.iBW = pbw;
		} else {
			A.freq = pfreq; A.imode = pmode; A.iBW = pbw;
		}
LOG_WARN("Vfo %c = %d, BW %s", vfobmode ? 'B' : 'A', pfreq, FT990widths_[pbw]);
		return true;
	}
	return false;
}

long RIG_FT990::get_vfoA ()
{
	return A.freq;
}

void RIG_FT990::set_vfoA (long freq)
{
	A.freq = freq;
	freq /=10; // 100D does not support 1 Hz resolution
	cmd = to_bcd_be(freq, 8);
	cmd += 0x0A;
	sendCommand(cmd);
	showresp(WARN, HEX, "set freq A", cmd, replystr);
}

int RIG_FT990::get_modeA()
{
	return A.imode;
}

void RIG_FT990::set_modeA(int val)
{
	A.imode = val;
	init_cmd();
	cmd[3] = FT990_mode_val[val];
	cmd[4] = 0x0C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set mode A", cmd, replystr);
}

void RIG_FT990::set_bwA (int val)
{
	A.iBW = val;
	init_cmd();
	cmd[3] = FT990_bw_val[val];
	cmd[4] = 0x8C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set BW A", cmd, replystr);
}


int RIG_FT990::get_bwA()
{
	return A.iBW;
}

long RIG_FT990::get_vfoB()
{
	return B.freq;
}

void RIG_FT990::set_vfoB(long freq)
{
	B.freq = freq;
	freq /=10;
	cmd = to_bcd_be(freq, 8);
	cmd += 0x0A;
	sendCommand(cmd);
	showresp(WARN, HEX, "set freq B", cmd, replystr);
}

void RIG_FT990::set_modeB(int val)
{
	B.imode = val;
	init_cmd();
	cmd[3] = FT990_mode_val[val];
	cmd[4] = 0x0C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set mode B", cmd, replystr);
}

int  RIG_FT990::get_modeB()
{
	return B.imode;
}

void RIG_FT990::set_bwB(int val)
{
	B.iBW = val;
	init_cmd();
	cmd[3] = FT990_bw_val[val];
	cmd[4] = 0x8C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set bw B", cmd, replystr);
}

int  RIG_FT990::get_bwB()
{
	return B.iBW;
}

int  RIG_FT990::def_bandwidth(int m)
{
	return FT990_def_bw[m];
}

// Tranceiver PTT on/off
void RIG_FT990::set_PTT_control(int val)
{
	init_cmd();
	if (val) cmd[3] = 1;
	cmd[4] = 0x0F;
	sendCommand(cmd);
	if (val)
		showresp(WARN, HEX, "set PTT ON", cmd, replystr);
	else
		showresp(WARN, HEX, "set PTT OFF", cmd, replystr);
	ptt_ = val;
}

int RIG_FT990::get_smeter()
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

int RIG_FT990::get_swr()
{
	return 0;
}

int RIG_FT990::get_power_out()
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

