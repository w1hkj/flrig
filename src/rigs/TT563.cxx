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

#include "TT563.h"

//=============================================================================
// TT-563

const char RIG_TT563name_[] = "OMNI-VI";

const char *RIG_TT563modes_[] = {
		"LSB", "USB", "AM", "CW", "RTTY", "FM", NULL};
static const char RIG_TT563_mode_type[] = {'L', 'U', 'U', 'U', 'L', 'U'};
const char *RIG_TT563widths[] = { "NARR", "WIDE", NULL};
static int TT563_bw_vals[] = {1, 2, WVALS_LIMIT};

RIG_TT563::RIG_TT563() {
	name_ = RIG_TT563name_;
	modes_ = RIG_TT563modes_;
	bandwidths_ = RIG_TT563widths;
	bw_vals_ = TT563_bw_vals;

	comm_baudrate = BR9600;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 10;
	comm_timeout = 50;
	comm_echo = true;
	comm_rtscts = false;
	comm_rtsplus = true;
	comm_dtrplus = true;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;

	def_freq = A.freq = 14070000;
	def_mode = A.imode = 1;
	def_bw = A.iBW = 1;

	B.freq = 7070000;
	B.imode = 1;
	B.iBW = 1;

	has_mode_control = true;
	has_ptt_control = true;

	has_a2b = true;
	has_getvfoAorB = true;
	has_split_AB = true;

	defaultCIV = 0x04;
	adjustCIV(defaultCIV);

	precision = 10;
	ndigits = 7;

};

void RIG_TT563::selectA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x00';
	cmd.append(post);
	set_trace(2, "selectA()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("select A");
	inuse = onA;
}

void RIG_TT563::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x01';
	cmd.append(post);
	set_trace(2, "selectB()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("select B");
	inuse = onB;
}

void RIG_TT563::swapAB()
{
	cmd = pre_to;
	cmd += 0x07;
	cmd += 0xB0;
	cmd.append(post);

	set_trace(2, "swapAB()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("swapAB()");
}

void RIG_TT563::A2B()
{
	cmd = pre_to;
	cmd += 0x07;
	cmd += 0xA0;
	cmd.append(post);

	set_trace(2, "A2B()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("A2B()");
}

bool RIG_TT563::can_split()
{
	return true;
}

void RIG_TT563::set_split(bool val)
{
	split = val;
	cmd = pre_to;
	cmd += 0x0F;
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	waitFB(val ? "set_split(ON)" : "set_split(OFF)");
	set_trace(2, 
		(val ? "set_split(ON)" : "set_split(OFF)"), 
		str2hex(replystr.c_str(), replystr.length()));
}

// 7200 does not respond to get split CAT command
int RIG_TT563::get_split()
{
	return split;
}

bool RIG_TT563::check ()
{
	string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	bool ok = waitFOR(11, "check vfo");
	get_trace(2, "check()", str2hex(replystr.c_str(), replystr.length()));
	return ok;
}

long RIG_TT563::get_vfoA ()
{
	if (useB) return A.freq;
	string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	if (waitFOR(11, "get vfo A")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			A.freq = fm_bcd_be(replystr.substr(p+5), 10);
	}
	get_trace(2, "get_vfoA()", str2hex(replystr.c_str(), replystr.length()));
	return A.freq;
}

void RIG_TT563::set_vfoA (long freq)
{
	A.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	set_trace(2, "set_vfoA()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("set vfo A");
}

long RIG_TT563::get_vfoB ()
{
	if (!useB) return B.freq;
	string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	if (waitFOR(11, "get vfo B")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			B.freq = fm_bcd_be(replystr.substr(p+5), 10);
	}
	get_trace(2, "get_vfoB()", str2hex(replystr.c_str(), replystr.length()));
	return B.freq;
}

void RIG_TT563::set_vfoB (long freq)
{
	B.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	set_trace(2, "set_vfoB()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("set vfo B");
}

int  RIG_TT563::get_vfoAorB()
{
	int ret = useB;
	cmd = pre_to;
	cmd += '\x17';
	cmd.append(post);

	string resp = pre_fm;
	resp += '\x17';

	if (waitFOR(6, "get_PTT()")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			ret = ((replystr[p+4] & 0x02) == 0x02);
		}
	}

	get_trace(2, "get_vfoAorB()", str2hex(replystr.c_str(), replystr.length()));
	return ret;
}


void RIG_TT563::set_PTT_control(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += val ? '\x01' : '\x02';
	cmd.append( post );
	sendCommand(cmd);
	set_trace(2, "set_PTT_control()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("set_PTT_control()");
}

int RIG_TT563::get_PTT()
{
	int ret = false;
	cmd = pre_to;
	cmd += '\x17';
	cmd.append(post);

	string resp = pre_fm;
	resp += '\x17';

	if (waitFOR(6, "get_PTT()")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			ret = ((replystr[p+4] & 0x04) == 0x04);
		}
	}

	get_trace(2, "get_PTT()", str2hex(replystr.c_str(), replystr.length()));
	return ret;
}

void RIG_TT563::set_modeA(int md)
{
	A.imode = md;
	cmd = pre_to;
	cmd += '\x06';
	cmd += A.imode;
	cmd.append(post);
	sendCommand(cmd);
	set_trace(2, "set_modeA()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("set_modeA()");
}

int RIG_TT563::get_modeA()
{
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);

	string resp = pre_fm;
	resp += '\x04';

	if (waitFOR(7, "get modeA")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			A.imode = replystr[p+5];
		}
	}
	get_trace(2, "get_modeA()", str2hex(replystr.c_str(), replystr.length()));
	return A.imode;
}

void RIG_TT563::set_modeB(int md)
{
	B.imode = md;
	cmd = pre_to;
	cmd += '\x06';
	cmd += B.imode;
	cmd.append(post);
	sendCommand(cmd);
	set_trace(2, "set_modeB()", str2hex(cmd.c_str(), cmd.length()));
	waitFB("set_modeB()");
}

int RIG_TT563::get_modeB()
{
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);

	string resp = pre_fm;
	resp += '\x04';

	if (waitFOR(7, "get mode")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			B.imode = replystr[p+5];
		}
	}
	get_trace(2, "get_modeB()", str2hex(replystr.c_str(), replystr.length()));
	return B.imode;
}

int RIG_TT563::get_modetype(int n)
{
	return RIG_TT563_mode_type[n];
}


