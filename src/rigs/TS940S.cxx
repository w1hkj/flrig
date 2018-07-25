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

#include "TS940S.h"

const char TS940Sname_[] = "TS940S";

const char *TS940Smodes_[] =             {"LSB", "USB", "CW", "FM", "AM", "FSK", NULL};
static const char TS940S_mode_type[] =   {'L',   'U',   'U',  'U',  'U',  'L'        };
static const char TS940S_mode_chr[] =    {'1',   '2',   '3',  '4',  '5',  '6',       };

RIG_TS940S::RIG_TS940S() {
// base class values
	name_ = TS940Sname_;
	modes_ = TS940Smodes_;
	bandwidths_ = NULL;
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
	modeA = 1;
	bwA = 2;

	has_mode_control =
	has_tune_control =
	has_split =
	has_split_AB =
	has_ptt_control = true;

	has_attenuator_control =
	has_preamp_control =
	has_power_control =
	has_volume_control =
	has_bandwidth_control =
	has_micgain_control =
	has_notch_control =
	has_ifshift_control =
	has_swr_control = false;

	precision = 10;
	ndigits = 7;

}

/*
========================================================================
	frequency & mode data are contained in the IF; response
		IFaaaaaaaaaaaXXXXXbbbbbcdXeefghjklmmX;
		12345678901234567890123456789012345678
		01234567890123456789012345678901234567 byte #
		IF00014070000       -00300     000200;

		where:
			aaaaaaaaaaa => decimal value of vfo frequency
			bbbbb => rit/xit frequency
			c => rit off/on
			d => xit off/on
			e => memory channel
			f => tx/rx
			g => mode
			h => function
			j => scan off/on
			k => split off /on
			l => tone off /on
			m => tone number
			X => unused characters

	Test output from Minicom to IF; command

	IF00014070000   -00300 000200;

	0001000 is vfoA in LSB
	0002000 is vfoA in USB
	0003000 CW
	0004000 FM
	0005000 AM
	0007000 CWN	(dont have narrow filter however)
	0002100 VFOB in USB
	0002001 VFOA in USB SPILT
	0012000 PTT on in USB
========================================================================
*/

bool RIG_TS940S::check ()
{
	cmd = "FA;";
	int ret = wait_char(';', 14, 100, "check", ASC);

	get_trace(2, "check()", replystr.c_str());

	if (ret < 14) return false;
	return true;
}

int RIG_TS940S::getvfoAorB()
{
        int vfovalue =0;
	cmd = "IF;";
	int ret = wait_char(';', 38, 100, "get vfo a or b", ASC);

	get_trace(2, "getvfoAorB()", replystr.c_str());

	if (ret < 38) return vfovalue;

	vfovalue = replybuff[ret - 38 + 30] - '0';

	showresp(WARN, ASC, "get vfo A or B", cmd, "");

	return vfovalue;
}


long RIG_TS940S::get_vfoA ()
{
	cmd = "FA;";
	if (wait_char(';', 14, 100, "get vfo A", ASC) < 14) return A.freq;

	get_trace(2, "get_vfoA()", replystr.c_str());

	size_t p = replystr.rfind("FA");
	if (p != string::npos && (p + 12 < replystr.length())) {
		int f = 0;
		for (size_t n = 2; n < 13; n++)
			f = f*10 + replystr[p+n] - '0';
		A.freq = f;
	}
	return A.freq;
}

void RIG_TS940S::set_vfoA (long freq)
{
	A.freq = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set vfo A", cmd, "");
	set_trace(2, "set_vfoA()", replystr.c_str());
}

long RIG_TS940S::get_vfoB ()
{
	cmd = "FB;";
	if (wait_char(';', 14, 100, "get vfo B", ASC) < 14) return B.freq;

	get_trace(2, "get_vfoB()", replystr.c_str());

	size_t p = replystr.rfind("FB");
	if (p != string::npos && (p + 12 < replystr.length())) {
		int f = 0;
		for (size_t n = 2; n < 13; n++)
			f = f*10 + replystr[p+n] - '0';
		B.freq = f;
	}
	return B.freq;
}

void RIG_TS940S::set_vfoB (long freq)
{
	B.freq = freq;
	cmd = "FB00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set vfo B", cmd, "");
	set_trace(2, "set_vfoB()", replystr.c_str());
}

int RIG_TS940S::get_modetype(int n)
{
	return TS940S_mode_type[n];
}

void RIG_TS940S::set_modeA(int val)
{
	A.imode = val;
	cmd = "MD";
	cmd += TS940S_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "set mode", cmd, "");
	set_trace(2, "set_mode()", replystr.c_str());
}

int RIG_TS940S::get_modeA()
{
	modeA = 0;
	cmd = "IF;";
	int ret = wait_char(';', 38, 100, "get mode", ASC);

	get_trace(2, "get_mode()", replystr.c_str());

	if (ret < 38) return modeA;

	int md = replybuff[ret - 38 + 29] - '1';
	if (md < 0) md = 0;
	if (md > 5) md = 5;
	modeA = md;

	showresp(WARN, ASC, "get mode", cmd, "");

	return modeA;
}

int RIG_TS940S::get_modeB()
{
  return get_modeA();
}

void RIG_TS940S::set_modeB(int val)
{
  set_modeA(val);
}

// Tranceiver PTT on/off
void RIG_TS940S::set_PTT_control(int val)
{
	if (val) {
		sendCommand("TX;");
		showresp(WARN, ASC, "TX on", cmd, "");
		set_trace(2, "set_PTT(on)", replystr.c_str());
	} else {
		sendCommand("RX;");
		showresp(WARN, ASC, "RX on", cmd, "");
		set_trace(2, "set_PTT(off)", replystr.c_str());
	}
	ptt_ = val;
}

/*
========================================================================
	frequency & mode data are contained in the IF; response
		IFaaaaaaaaaaaXXXXXbbbbbcdXeefghjklmmX;
		12345678901234567890123456789012345678
		01234567890123456789012345678901234567 byte #
		          1         2         3
		                            ^ position 28
		where:
			aaaaaaaaaaa => decimal value of vfo frequency
			bbbbb => rit/xit frequency
			c => rit off/on
			d => xit off/on
			e => memory channel
			f => tx/rx
			g => mode
			h => function
			j => scan off/on
			k => split off /on
			l => tone off /on
			m => tone number
			X => unused characters

========================================================================
*/

int RIG_TS940S::get_PTT()
{
	cmd = "IF;";
	int ret = wait_char(';', 38, 100, "get VFO", ASC);

	get_trace(2, "get_PTT()", replystr.c_str());

	if (ret < 38) return ptt_;
	ptt_ = (replybuff[28] == '1');
	showresp(WARN, ASC, "get ptt", cmd, "");
	return ptt_;
}


void RIG_TS940S::tune_rig()
{
	cmd = "AT1;";
	LOG_WARN("%s", cmd.c_str());
	sendCommand(cmd);
	showresp(WARN, ASC, "rig tune set", cmd, "");
	set_trace(2, "tune_rig()", replystr.c_str());
}

int RIG_TS940S::get_split()
{
	int splitvalue =0;
	cmd = "IF;";
	int ret = wait_char(';', 38, 100, "get split", ASC);

	get_trace(2, "get_split()", replystr.c_str());

	if (ret < 38) return splitvalue;

	splitvalue = replybuff[ret - 38 + 32] - '0';

	showresp(WARN, ASC, "get split", cmd, "");

	return splitvalue;
}

void RIG_TS940S::set_split(bool val)
{
	if (val) {
		cmd = "SP1;";
		sendCommand(cmd);
		showresp(WARN, ASC, "split on", cmd, "");
		set_trace(2, "set_split(on)", replystr.c_str());
	} else {
		cmd = "SP0;";
		sendCommand(cmd);
		showresp(WARN, ASC, "split off", cmd, "");
		set_trace(2, "set_split(off)", replystr.c_str());
	}
}

bool RIG_TS940S::can_split()
{
	return true;
}

void RIG_TS940S::selectA()
{
	set_split(0);
	cmd="FN0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "select VFO A", cmd, "");
	set_trace(2, "selectA()", replystr.c_str());
}

void RIG_TS940S::selectB()
{
	set_split(0);
	cmd="FN1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "select VFO B", cmd, "");
	set_trace(2, "selectB()", replystr.c_str());
}
