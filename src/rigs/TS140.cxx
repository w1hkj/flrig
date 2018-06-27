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

#include "TS140.h"

const char TS140name_[] = "TS140";

const char *TS140modes_[] = {
		"LSB", "USB", "CW", "FM", "AM", "CWN", NULL};
static const char TS140_mode_type[] =
	{'L', 'U', 'U', 'U', 'U', 'U'};

RIG_TS140::RIG_TS140() {
// base class values	
	name_ = TS140name_;
	modes_ = TS140modes_;
	bandwidths_ = NULL;
	comm_baudrate = BR9600;
	stopbits = 2;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_rtscts = true;
	comm_rtsplus = false;
	comm_dtrplus = false;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	modeA = 1;
	bwA = 2;

	has_mode_control =
	has_ptt_control = true;

	has_attenuator_control =
	has_preamp_control =
	has_power_control =
	has_volume_control =
	has_bandwidth_control =
	has_micgain_control =
	has_notch_control =
	has_ifshift_control =
	has_tune_control =
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

bool RIG_TS140::check ()
{
	cmd = "IF;";
	int ret = wait_char(';', 38, 100, "check", ASC);
	if (ret < 38) return false;
	return true;
}

long RIG_TS140::get_vfoA ()
{
	cmd = "IF;";
	int ret = wait_char(';', 38, 100, "get VFO", ASC);
	if (ret < 38) return freqA;

	long f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replybuff[ret - 38 + n] - '0';
	freqA = f;
	return freqA;
}

void RIG_TS140::set_vfoA (long freq)
{
	freqA = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
}

int RIG_TS140::get_modetype(int n)
{
	return TS140_mode_type[n];
}

void RIG_TS140::set_modeA(int val)
{
	if (val == 5) val++;
	cmd = "MD0;";
	cmd[2] = '1' + (val % 10);
	sendCommand(cmd);
}

int RIG_TS140::get_modeA()
{
	modeA = 0;
	cmd = "IF;";
	int ret = wait_char(';', 38, 100, "get mode", ASC);
	if (ret < 38) return modeA;

	int md = replybuff[ret - 38 + 29] - '1';
	if (md < 0) md = 0;
	if (md > 5) md = 5;
	modeA = md;

	return modeA;
}

// Tranceiver PTT on/off
void RIG_TS140::set_PTT_control(int val)
{
	if (val) sendCommand("TX;");
	else	 sendCommand("RX;");
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

int RIG_TS140::get_PTT()
{
	cmd = "IF;";
	int ret = wait_char(';', 38, 100, "get VFO", ASC);
	if (ret < 38) return ptt_;
	ptt_ = (replybuff[28] == '1');
	return ptt_;
}
