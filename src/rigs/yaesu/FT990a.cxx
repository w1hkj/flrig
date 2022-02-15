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

#include "yaesu/FT990a.h"
#include "rig.h"

static const char FT990Aname_[] = "FT-990A";

enum {
FT990A_LSB, FT990A_USB, FT990A_CW1, FT990A_CW2,
FT990A_AM1, FT990A_AM2, FT990A_FM1, FT990A_FM2,
FT990A_TTYL, FT990A_TTYU, FT990A_PKTL, FT990A_PKTU
};

static const char *FT990Amodes_[] =
{ "LSB", "USB", "CW2.4", "CW500",
  "AM6.0", "AM2.4", "FM(1)", "FM(2)",
  "RTTY(L)", "RTTY(U)", "PKT(L)", "PKT(FM)", NULL};

static const int FT990A_def_bw[] = {
0, 0, 0, 2,
4, 0, 0, 0,
0, 0, 0, 0 };

static const int FT990A_mode_val[] = {
0, 1, 2, 3,
4, 5, 6, 7,
8, 9, 10, 11 };

static const char FT990A_mode_type[] = {
'L', 'U', 'L', 'L',
'U', 'U', 'U', 'U',
'L', 'U', 'L', 'U' };

static const char *FT990Awidths_[] =
{ "2400", "2000", "500", "250", NULL};

static int FT990A_bw_vals[] = {
0,1,2,3,WVALS_LIMIT};

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


	has_get_info = true;

	has_split = has_split_AB =
	has_smeter =
	has_power_out =
	has_swr_control =
	has_mode_control =
	has_bandwidth_control =
	has_ptt_control =
	has_tune_control = true;

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
	setthex("select A");
	inuse = onA;
}

void RIG_FT990A::selectB()
{
	init_cmd();
	cmd[3] = 0x01;
	cmd[4] = 0x05;
	sendCommand(cmd);
	showresp(WARN, HEX, "select B", cmd, "");
	setthex("select B");
	inuse = onB;
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
	setthex("split A/B");
}

bool RIG_FT990A::check()
{
	init_cmd();
	cmd[3] = 0x00;
	cmd[4] = 0xFA;
	int ret = waitN(5, 100, "check", HEX);
	if (ret >= 5) return true;
	return false;
}

int  RIG_FT990A::get_vfoAorB()
{
	bool memmode = false;
	int  vfobmode = onA;
	init_cmd();
	cmd[3] = 0x00;
	cmd[4] = 0xFA;
	int ret = waitN(5, 100, "Read flags", HEX);
	getthex("read flags");

	if (ret >= 5) {
		size_t p = ret - 5;
		memmode = ((replystr[p+1] & 0x10) == 0x10);
		vfobmode = ((replystr[p] & 0x02) == 0x02) ? onB : onA;

		if (memmode)
			return inuse;

		inuse = vfobmode;
	}
	return inuse;
}

bool RIG_FT990A::get_info()
{
	int pmode, pbw;

	init_cmd();
	cmd[3] = 0x03;  // read 32 bytes of data for status of both vfos
	cmd[4] = 0x10;
	int ret = waitN(32, 100, "get info", ASC);
	getthex("Get info");

	if (ret >= 32) {
		size_t da = ret - 32;
		size_t db = ret - 16;

		// vfo A data std::string
		A.freq = (replystr[da + 1] & 0xFF);
		A.freq <<= 8;
		A.freq += (replystr[da + 2]) & 0xFF;
		A.freq <<= 8;
		A.freq += (replystr[da + 3] & 0xFF);
		A.freq *= 10;

		int rmode = replystr[da + 7] & 0x07;
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
		int rpbw = replystr[da + 8];
		pbw = rpbw & 0x05;
		if (pbw > 4) pbw = 4;
		if ((rpbw & 0x80) == 0x80) {
			if (pmode == 10) pmode = 11;
			if (pmode == 8) pmode = 9;
		}
		if (pmode == 6) pbw = 0;
		A.imode = pmode;
		A.iBW = pbw;

		// vfo B data std::string
		B.freq = (replystr[db + 1] & 0xFF);
		B.freq <<= 8;
		B.freq += (replystr[db + 2]) & 0xFF;
		B.freq <<= 8;
		B.freq += (replystr[db + 3] & 0xFF);
		B.freq *= 10;

		rmode = replystr[db + 7] & 0x07;
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

		rpbw = replystr[db + 8];
		pbw = rpbw & 0x05;
		if (pbw > 4) pbw = 4;
		if ((rpbw & 0x80) == 0x80) {
			if (pmode == 10) pmode = 11;
			if (pmode == 8) pmode = 9;
		}
		if (pmode == 6) pbw = 0;
		B.imode = pmode;
		B.iBW = pbw;

		return true;
	}
	return false;
}

unsigned long int RIG_FT990A::get_vfoA ()
{
	return A.freq;
}

void RIG_FT990A::set_vfoA (unsigned long int freq)
{
	int current_vfo = inuse;
	if (current_vfo == onB) selectA();

	A.freq = freq;
	freq /=10;
	cmd = to_bcd_be(freq, 8);
	cmd += 0x0A;
	sendCommand(cmd);
	showresp(WARN, HEX, "set freq A", cmd, replystr);
	setthex("Set freq A");

	if (current_vfo == onB) selectB();
}

int RIG_FT990A::get_modeA()
{
	return A.imode;
}

void RIG_FT990A::set_modeA(int val)
{
	int current_vfo = inuse;
	if (current_vfo == onB) selectA();

	A.imode = val;
	init_cmd();
	cmd[3] = FT990A_mode_val[val];
	cmd[4] = 0x0C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set mode A", cmd, "");
	setthex("set mode A");

	if (current_vfo == onB) selectB();
}

void RIG_FT990A::set_bwA (int val)
{
	int current_vfo = inuse;
	if (current_vfo == onB) selectA();

	A.iBW = val;
	init_cmd();
	cmd[3] = FT990A_bw_val[val];
	cmd[4] = 0x8C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set BW A", cmd, "");
	setthex("set BW A");

	if (current_vfo == onB) selectB();
}


int RIG_FT990A::get_bwA()
{
	return A.iBW;
}

unsigned long int RIG_FT990A::get_vfoB()
{
	return B.freq;
}

void RIG_FT990A::set_vfoB(unsigned long int freq)
{
	int current_vfo = inuse;
	if (current_vfo == onA) selectB();

	B.freq = freq;
	freq /=10;
	cmd = to_bcd_be(freq, 8);
	cmd += 0x0A;
	sendCommand(cmd);
//	showresp(WARN, HEX, "set freq B", cmd, replystr);
	setthex("Set freq B");

	if (current_vfo == onA) selectA();
}

void RIG_FT990A::set_modeB(int val)
{
	int current_vfo = inuse;
	if (current_vfo == onA) selectB();

	B.imode = val;
	init_cmd();
	cmd[3] = FT990A_mode_val[val];
	cmd[4] = 0x0C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set mode B", cmd, "");
	setthex("set mode B");

	if (current_vfo == onA) selectA();
}

int  RIG_FT990A::get_modeB()
{
	return B.imode;
}

void RIG_FT990A::set_bwB(int val)
{
	int current_vfo = inuse;
	if (current_vfo == onA) selectB();

	B.iBW = val;
	init_cmd();
	cmd[3] = FT990A_bw_val[val];
	cmd[4] = 0x8C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set bw B", cmd, "");
	setthex("set BW B");

	if (current_vfo == onA) selectA();
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
	ptt_ = val;
	setthex("set PTT On/Off");
}

void RIG_FT990A::tune_rig(int)
{
	init_cmd();
	cmd[4] = 0x82; // initiate tuner cycle
	sendCommand(cmd,0);
	setthex("Tune xcvr");
}

int RIG_FT990A::get_smeter()
{
	float val = 0;
	init_cmd();
	//cmd[0] = 0x00;
	cmd[4] = 0xF7;
	int ret = waitN(5, 100, "get smeter", HEX);
	if (ret < 5) return 0;

	val = (unsigned char)(replystr[ret-5]);
	if (val <= 15) val = 5;
	else if (val <=154) val = 5 + 80 * (val - 15) / (154 - 15);
	else val = 50 + 50 * (val - 154.0) / (255.0 - 154.0);

	char szmeter[5];
	snprintf(szmeter, sizeof(szmeter), "%d", (int)val);
	get_trace(3, "Smeter", str2hex(replystr.c_str(), 1), szmeter);

	return (int)val;
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
	getthex("get power out");

	if (ret < 5) return 0;
	int sval = (unsigned char)replystr[0];
	if (sval < 90) sval = 90;
	if (sval > 200) sval = 200;
	if (sval < 120) sval = 250 - 5 * sval / 3;
	else sval = 125 - 5 * sval / 8;
	return sval;
}

