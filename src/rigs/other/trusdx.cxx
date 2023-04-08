// ----------------------------------------------------------------------------
// Copyright (C) 2022
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
// with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include "other/trusdx.h"
#include "support.h"

static const char TRUSDXname_[] = "(tr)uSDX";

enum { SDR2_LSB, SDR2_USB, SDR2_CW, SDR2_FM, SDR2_AM };

static const char *TRUSDXmodes_[] = {
"LSB", "USB", "CW", "FM", "AM", NULL};

static const char TRUSDX_mode_type[] = { 'L', 'U', 'U', 'U', 'U' };

RIG_TRUSDX::RIG_TRUSDX() {
// base class values
	name_ = TRUSDXname_;
	modes_ = TRUSDXmodes_;

	bandwidths_ = NULL;

	serial_baudrate = BR38400;
	stopbits = 1;
	serial_retries = 2;

//	serial_write_delay = 0;
//	serial_post_write_delay = 0;

	serial_timeout = 50;
	serial_rtscts = false;
	serial_rtsplus = false;
	serial_dtrplus = false;
	serial_catptt = true;
	serial_rtsptt = false;
	serial_dtrptt = false;

	A.imode = 1;
	A.freq = 14070000ULL;

	has_mode_control = true;
	has_ptt_control = true;

	precision = 1;
	ndigits = 8;

}

static int ret = 0;

void RIG_TRUSDX::initialize()
{
}

void RIG_TRUSDX::shutdown()
{
}

bool RIG_TRUSDX::check ()
{
	MilliSleep(2000); // xcvr needs time to wake up
	cmd = "ID;";
	get_trace(1, "check()");
	ret = wait_char(';', 6, 100, "get ID", ASC);
	gett("");
	if (replystr.find("ID020;") != std::string::npos)
		return true;
	return false;
}

/*
========================================================================
	frequency & mode data are contained in the IF; response

	0         1         2         3
    012345678901234567890123456789012345678
	IFaaaaaaaaaaaXXXXXbbbbbcdXeefgHJKLMMX;
	IFggmmmkkkhhh00000+0000000000d0000000;
	IF0001407000000000+000000000020000200;

	where:
		aaaaaaaaaaa => decimal value of vfo frequency
		bbbbb => rit/xit frequency
		c => rit off/on
		d => xit off/on
		e => memory channel
		f => tx/rx
		g => mode
		H => function
		J => scan off/on
		K => split off /on
		L => tone off /on
		M => tone number
		X => unused characters

	d = Value as defined in MD Command
========================================================================
*/

void RIG_TRUSDX::get_IF()
{
	cmd = "IF;";
	get_trace(1, "get_IF()");
	wait_char(';', 38, 100, "get IF", ASC);	// This function called for its side effect, not its return.
	gett("");
	size_t p = replystr.rfind("IF");
	A.imode = replystr[p + 29] - '1';
	return;
}

unsigned long long RIG_TRUSDX::get_vfoA ()
{
	cmd = "FA;";
	get_trace(1, "get_vfoA()");
	if (wait_char(';', 14, 100, "get vfo A", ASC) < 14) return A.freq;
	gett("");

	size_t p = replystr.rfind("FA");
	if (p != std::string::npos && (p + 12 < replystr.length())) {
		unsigned long long f = 0;
		for (size_t n = 2; n < 13; n++)
			f = f*10 + replystr[p+n] - '0';
		A.freq = f;
	}
	return A.freq;
}

void RIG_TRUSDX::set_vfoA (unsigned long long freq)
{
	A.freq = freq;
	set_trace(1, "set_vfoA(...)");
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	sett("");
}

int RIG_TRUSDX::get_modeA()
{
	cmd = "MD;";
	get_trace(1, "get_modeA");
	ret = wait_char(';', 4, 100, "get modeA", ASC);
	gett("");
	if (ret < 4) return A.imode;
	size_t p = replystr.rfind("MD");
	A.imode = replystr[p + 2] - '1';
	return A.imode;
}

void RIG_TRUSDX::set_modeA(int val)
{
	A.imode = val;
	cmd = "MD";
	cmd += '1' + val;
	cmd += ';';
	set_trace(1, "set_modeA");
	sendCommand(cmd);
	sett("");
}

int RIG_TRUSDX::get_modetype(int n)
{
	return TRUSDX_mode_type[n];
}

void RIG_TRUSDX::set_PTT_control(int val)
{
	set_trace(3, "set_PTT(", (val ? "ON" : "OFF"), ")");
	if (val) sendCommand("TX;");
	else	 sendCommand("RX;");
	sett("");
	ptt_ = val;
}
