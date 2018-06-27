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

#include "FT100D.h"
#include "rig.h"

static const char FT100Dname_[] = "FT-100D";
static const char *FT100Dmodes_[] = 
{ "LSB", "USB", "CW", "CW-R", "AM", "DIG", "FM", "W-FM", NULL};

static const int FT100D_def_bw[] = { 2, 2, 1, 1, 3, 1, 2, 3 };

static const int FT100D_mode_val[] =
{ 0, 1, 2, 3, 4, 5, 6, 7 };

static const char FT100D_mode_type[] =
{ 'L', 'U', 'U', 'L', 'U', 'U', 'U', 'U' };

static const char *FT100Dwidths_[] =
{ "300", "500", "2400", "6000", NULL};

static const int FT100D_bw_val[] =
{ 0, 1, 2, 3 };

RIG_FT100D::RIG_FT100D() {
	name_ = FT100Dname_;
	modes_ = FT100Dmodes_;
	bandwidths_ = FT100Dwidths_;
	bw_vals_ = FT100D_bw_val;

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

int  RIG_FT100D::adjust_bandwidth(int m)
{
	if (m == 0 || m == 1 || m == 5) return 2;
	if (m == 2 || m == 3 ) return 1;
	if (m == 4 || m == 6 || m == 7) return 3;
	return 2;
}

void RIG_FT100D::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
	replystr.clear();
}

void RIG_FT100D::initialize()
{
}

void RIG_FT100D::selectA()
{
	init_cmd();
	cmd[4] = 0x05;
	sendCommand(cmd);
	showresp(WARN, HEX, "select A", cmd, replystr);
}

void RIG_FT100D::selectB()
{
	init_cmd();
	cmd[3] = 0x01;
	cmd[4] = 0x05;
	sendCommand(cmd);
	showresp(WARN, HEX, "select B", cmd, replystr);
}

void RIG_FT100D::set_split(bool val)
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

//======================================================================
// response to get_info()
/*
D: sendCommand:  cmd:  5, 00 00 00 01 FA
D: readResponse: rsp:  8, 28 20 04 00 00 00 88 04
W:07:47:40: status OK 50 ms
cmd 00 00 00 01 FA
ans 28 20 04 00 00 00 88 04
D: sendCommand:  cmd:  5, 00 00 00 00 10
D: readResponse: rsp: 32, 07 00 56 CB E8 11 08 00 03 81 00 00 03 33 21 22 0B 00 AB C0 C0 11 13 00 84 A0 00 00 03 33 01 21
W:07:47:40: info OK 50 ms
cmd 00 00 00 00 10
ans 07 00 56 CB E8 11 08 00 03 81 00 00 03 33 21 22 0B 00 AB C0 C0 11 13 00 84 A0 00 00 03 33 01 21
W: get_info: Vfo A = 14070000
*/
//======================================================================

bool RIG_FT100D::check()
{
	init_cmd();
	cmd[3] = 0x01;
	cmd[4] = 0xFA;
	int ret = waitN(8, 100, "check");
	if (ret >= 8) return true;
	return false;
}

bool RIG_FT100D::get_info()
{
	bool memmode = false, vfobmode = false;
	int pfreq, pmode, pbw;
	init_cmd();
	cmd[3] = 0x01;
	cmd[4] = 0xFA;
	int ret = waitN(8, 100, "status");

	if (ret >= 8) {
		size_t p = ret - 8;
		memmode = ((replystr[p+1] & 0x40) == 0x40);
		vfobmode = ((replystr[p+1] & 0x24) == 0x24);
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
	cmd[4] = 0x10;
	ret = waitN(32, 100, "info");

	if (ret == 32) {
		// primary
		pfreq = 0;
		for (size_t n = 1; n < 5; n++)
			pfreq = pfreq * 256 + (unsigned char)replystr[n];
		pfreq = pfreq * 1.25; // 100D resolution is 1.25 Hz / bit for read

		pmode = replystr[5] & 0x07;
		if (pmode > 7) pmode = 7;
		pbw = (replystr[5] >> 4) & 0x03;
		pbw = 3 - pbw;
		if (useB) {
			B.freq = pfreq; B.imode = pmode; B.iBW = pbw;
		} else {
			A.freq = pfreq; A.imode = pmode; A.iBW = pbw;
		}
		return true;
	}
	return false;
}

long RIG_FT100D::get_vfoA ()
{
	return A.freq;
}

void RIG_FT100D::set_vfoA (long freq)
{
	A.freq = freq;
	freq /=10; // 100D does not support 1 Hz resolution
	cmd = to_bcd_be(freq, 8);
	cmd += 0x0A;
	sendCommand(cmd);
	showresp(WARN, HEX, "set freq A", cmd, replystr);
}

int RIG_FT100D::get_modeA()
{
	return A.imode;
}

void RIG_FT100D::set_modeA(int val)
{
	A.imode = val;
	init_cmd();
	cmd[3] = FT100D_mode_val[val];
	cmd[4] = 0x0C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set mode A", cmd, replystr);
}

void RIG_FT100D::set_bwA (int val)
{
	A.iBW = val;
	init_cmd();
	cmd[3] = FT100D_bw_val[val];
	cmd[4] = 0x8C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set BW A", cmd, replystr);
}


int RIG_FT100D::get_bwA()
{
	return A.iBW;
}

long RIG_FT100D::get_vfoB()
{
	return B.freq;
}

void RIG_FT100D::set_vfoB(long freq)
{
	B.freq = freq;
	freq /=10; // 100D does not support 1 Hz resolution
	cmd = to_bcd_be(freq, 8);
	cmd += 0x0A;
	sendCommand(cmd);
	showresp(WARN, HEX, "set freq B", cmd, replystr);
}

void RIG_FT100D::set_modeB(int val)
{
	B.imode = val;
	init_cmd();
	cmd[3] = FT100D_mode_val[val];
	cmd[4] = 0x0C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set mode B", cmd, replystr);
}

int  RIG_FT100D::get_modeB()
{
	return B.imode;
}

void RIG_FT100D::set_bwB(int val)
{
	B.iBW = val;
	init_cmd();
	cmd[3] = FT100D_bw_val[val];
	cmd[4] = 0x8C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set bw B", cmd, replystr);
}

int  RIG_FT100D::get_bwB()
{
	return B.iBW;
}

int  RIG_FT100D::def_bandwidth(int m)
{
	return FT100D_def_bw[m];
}

// Tranceiver PTT on/off
void RIG_FT100D::set_PTT_control(int val)
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

int RIG_FT100D::get_smeter()
{
	init_cmd();
	cmd[4] = 0xF7;
	int ret = waitN(9, 100, "S-meter");
	if (ret < 9) return 0;
	int sval = (unsigned char)replybuff[ret - 6];
	if (sval < 90) sval = 90;
	if (sval > 200) sval = 200;
	if (sval < 120) sval = 250 - 5 * sval / 3;
	else sval = 125 - 5 * sval / 8;
	return sval;
}

int RIG_FT100D::get_swr()
{
	double swr = (fwdpwr + refpwr) / (fwdpwr - refpwr + .0001);
	swr -= 1.0;
	swr *= 25.0;
	if (swr < 0) swr = 0;
	if (swr > 100) swr = 100;
	return (int) swr;
}

int RIG_FT100D::get_power_out()
{
	init_cmd();
	cmd[4] = 0xF7;
	int ret = waitN(9, 100, "P-out");
	if (ret < 9) return 0;
	fwdpwr = replystr[ret - 9 + 1] / 2.56;
	refpwr = replystr[ret - 9 + 2] / 2.56;
	return (int) fwdpwr;
}

