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

#include "IC706MKIIG.h"

//=============================================================================
// IC-706MKIIG
//
const char IC706MKIIGname_[] = "IC-706MKIIG";
const char *IC706MKIIGmodes_[] = { "LSB", "USB", "AM", "CW", "RTTY", "FM", "WFM", NULL};
const char IC706MKIIG_mode_type[] =
	{ 'L', 'U', 'U', 'L', 'L', 'U', 'U'};
//const char *IC706MKIIG_widths[] = { "WIDE", "NARR", NULL};
const char *IC706MKIIG_widths[] = { "n/a", NULL};

RIG_IC706MKIIG::RIG_IC706MKIIG() {
	name_ = IC706MKIIGname_;
	modes_ = IC706MKIIGmodes_;
	bandwidths_ = IC706MKIIG_widths;
	_mode_type = IC706MKIIG_mode_type;
	comm_baudrate = BR19200;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_echo = true;
	comm_rtscts = false;
	comm_rtsplus = true;
	comm_dtrplus = true;
	comm_catptt = false;
	comm_rtsptt = false;
	comm_dtrptt = false;
	def_freq = freqA = freqB = A.freq = B.imode = 14070000;
	def_mode = modeA = modeB = A.imode = B.imode = 1;
	def_bw = bwA = bwB = A.iBW = B.iBW = 0;

	has_smeter =
	has_mode_control =
	has_attenuator_control = true;

	defaultCIV = 0x58;
	adjustCIV(defaultCIV);

	precision = 1;
	ndigits = 9;

};

//=============================================================================

void RIG_IC706MKIIG::selectA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x00';
	cmd.append(post);
	waitFB("select A");
}

void RIG_IC706MKIIG::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\x01';
	cmd.append(post);
	waitFB("select B");
}

long RIG_IC706MKIIG::get_vfoA ()
{
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	string resp = pre_fm;
	resp += '\x03';
	if (waitFOR(11, "get vfo A")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			freqA = fm_bcd_be(&replystr[p+5], 10);
	}
	return freqA;
}

void RIG_IC706MKIIG::set_vfoA (long freq)
{
	freqA = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	waitFB("set vfo A");
}

long RIG_IC706MKIIG::get_vfoB ()
{
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	string resp = pre_fm;
	resp += '\x03';
	if (waitFOR(11, "get vfo B")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			freqB = fm_bcd_be(&replystr[p+5], 10);
	}
	return freqB;
}

void RIG_IC706MKIIG::set_vfoB (long freq)
{
	freqB = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	waitFB("set vfo B");
}

void RIG_IC706MKIIG::set_split(bool b)
{
	cmd = pre_to;
	cmd += '\x0F';
	cmd += b ? '\x01' : '\x00';
	cmd.append( post );
	waitFB("set split");
}

void RIG_IC706MKIIG::set_modeA(int val)
{
	modeA = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += val > 5 ? val + 2 : val;
	cmd += bwA;
	cmd.append( post );
	waitFB("set mode A");
}

int RIG_IC706MKIIG::get_modeA()
{
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	string resp = pre_fm;
	resp += '\x04';
	if (waitFOR(8, "get mode A")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			modeA = replystr[p+5];
			if (modeA > 6) modeA -= 2;
//			bwA = replystr[p+6];
		}
	}
	return modeA;
}

int RIG_IC706MKIIG::get_modetype(int n)
{
	return _mode_type[n];
}

/*
void RIG_IC706MKIIG::set_bwA(int val)
{
	bwA = val;
	set_modeA(modeA);
}

int  RIG_IC706MKIIG::get_bwA()
{
	return bwA;
}
*/

void RIG_IC706MKIIG::set_attenuator(int val)
{
	cmd = pre_to;
	cmd += '\x11';
	cmd += val ? '\x20' : '\x00';
	cmd.append( post );
	waitFB("set att");
}

int RIG_IC706MKIIG::get_smeter()
{
	cmd = pre_to;
	cmd.append("\x15\x02");
	cmd.append( post );
	string resp = pre_fm;
	resp.append("\x15\x02");
	if (waitFOR(9, "get smeter")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (int)ceil(fm_bcd(&replystr[p+6], 3) / 2.55);
	}
	return -1;
}
