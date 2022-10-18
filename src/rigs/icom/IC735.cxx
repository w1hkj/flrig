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

#include "icom/IC735.h"

//=============================================================================
// IC-735
//
const char IC735name_[] = "IC-735";
const char *IC735modes_[] = { "LSB", "USB", "AM", "CW", "RTTY", "FM", NULL};
const char IC735_mode_type[] = { 'L', 'U', 'U', 'L', 'L', 'U' };

RIG_IC735::RIG_IC735() {
	name_ = IC735name_;
	modes_ = IC735modes_;
	_mode_type = IC735_mode_type;
	serial_baudrate = BR1200;
	stopbits = 2;
	serial_retries = 2;
//	serial_write_delay = 0;
//	serial_post_write_delay = 0;
	serial_timeout = 50;
	serial_echo = true;
	serial_rtscts = false;
	serial_rtsplus = true;
	serial_dtrplus = true;
	serial_catptt = false;
	serial_rtsptt = false;
	serial_dtrptt = false;
	modeA = 1;
	bwA = 0;

	has_mode_control = true;
	has_bandwidth_control = true;

	defaultCIV = 0x04;
	adjustCIV(defaultCIV);

	precision = 10;
	ndigits = 7;

};

//=============================================================================
void RIG_IC735::selectA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x00';
	cmd.append(post);
	waitFB("select A");
}

void RIG_IC735::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x01';
	cmd.append(post);
	waitFB("select B");
}

bool RIG_IC735::check ()
{
	std::string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	bool ok = waitFOR(10, "check vfo");
	rig_trace(2, "check()", str2hex(replystr.c_str(), replystr.length()));
	return ok;
}

unsigned long long RIG_IC735::get_vfoA ()
{
	if (inuse == onB) return A.freq;
	std::string cstr = "\x03";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(10, "get vfo A")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			if (replystr[p+5] == -1)
				A.freq = 0;
			else
				freqA = fm_bcd_be(replystr.substr(p+5), 8);
		}
	}
	return freqA;
}

void RIG_IC735::set_vfoA (unsigned long long freq)
{
	freqA = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 8 ) );
	cmd.append( post );
	waitFB("set vfo A");
}

unsigned long long RIG_IC735::get_vfoB ()
{
	if (inuse == onA) return B.freq;
	std::string cstr = "\x03";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(10, "get vfo B")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			if (replystr[p+5] == -1)
				A.freq = 0;
			else
				freqB = fm_bcd_be(replystr.substr(p+5), 8);
		}
	}
	return freqB;
}

void RIG_IC735::set_vfoB (unsigned long long freq)
{
	freqA = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 8 ) );
	cmd.append( post );
	waitFB("set vfo B");
}

void RIG_IC735::set_modeA(int val)
{
	modeA = val;
	cmd = pre_to;
	cmd += "\x06";
	cmd += modeA;		   // set the mode byte
	cmd.append( post );
	waitFB("set mode");
}

int RIG_IC735::get_modeA()
{
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	std::string resp = pre_fm;
	resp += '\x04';
	if (waitFOR(7, "get mode")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			if (replystr[p+5] == -1) { modeA = 0; }
			else {
				modeA = replystr[p+5];
			}
		}
	}
	return modeA;
}
