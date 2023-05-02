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

#include "icom/IC728.h"

//=============================================================================
// IC-728
//
const char IC728name_[] = "IC-728";

static std::vector<std::string>IC728modes_;
static const char *vIC728modes_[] =
{ "LSB", "USB", "AM", "CW", "RTTY", "FM"};
const char IC728_mode_type[] = { 'L', 'U', 'U', 'L', 'L', 'U' };

static std::vector<std::string>IC728_widths;
static const char *vIC728_widths[] =
{ "NARR", "WIDE"};
static int IC728_bw_vals[] = {1,2, WVALS_LIMIT};

RIG_IC728::RIG_IC728() {
	name_ = IC728name_;
	modes_ = IC728modes_;
	_mode_type = IC728_mode_type;
	bandwidths_ = IC728_widths;
	bw_vals_ = IC728_bw_vals;
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
	serial_catptt = true;
	serial_rtsptt = false;
	serial_dtrptt = false;
	modeA = 1;
	bwA = 0;

	defaultCIV = 0x38;
	adjustCIV(defaultCIV);

	precision = 10;
	ndigits = 7;

};

//=============================================================================
void RIG_IC728::initialize()
{
	VECTOR (IC728modes_, vIC728modes_);
	VECTOR (IC728_widths, vIC728_widths);

	modes_ = IC728modes_;
	_mode_type = IC728_mode_type;
	bandwidths_ = IC728_widths;
	bw_vals_ = IC728_bw_vals;
}

bool RIG_IC728::check ()
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

unsigned long long RIG_IC728::get_vfoA ()
{
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

void RIG_IC728::set_vfoA (unsigned long long freq)
{
	freqA = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 8 ) );
	cmd.append( post );
	waitFB("set vfo A");
}

