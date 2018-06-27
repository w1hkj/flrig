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

// optional filter strings
// "EMPTY", "NARR", "NORM", "WIDE", "MED",
// "FL-101", "FL-232", "FL-100", "FL-223", "FL-272", "FL-103", "FL-94"

const char IC706MKIIGname_[] = "IC-706MKIIG";
const char *IC706MKIIGmodes_[] = { "LSB", "USB", "AM", "CW", "RTTY", "FM", "WFM", NULL};

enum {
	LSB706, USB706, AM706, CW706, RTTY706, FM706, WFM706 };

const char IC706MKIIG_mode_type[] =
	{ 'L', 'U', 'U', 'L', 'L', 'U', 'U'};
const char *IC706MKIIG_ssb_cw_rtty_bws[] = { "WIDE", "NORMAL", "NARROW", NULL};
static int IC706MKIIG_vals_ssb_cw_rtty_bws[] = { 1, 2, 3, WVALS_LIMIT };
const char *IC706MKIIG_am_fm_bws[] = { "NORMAL", "NARROW", NULL};
static int IC706MKIIG_vals_am_fm_bws[] = { 1, 2, WVALS_LIMIT };
const char *IC706MKIIG_wfm_bws[] = { "FIXED", NULL};
static int IC706MKIIG_vals_wfm_bws[] = { 1, WVALS_LIMIT};

RIG_IC706MKIIG::RIG_IC706MKIIG() {
	name_ = IC706MKIIGname_;
	modes_ = IC706MKIIGmodes_;
	bandwidths_ = IC706MKIIG_ssb_cw_rtty_bws;
	bw_vals_ = IC706MKIIG_vals_ssb_cw_rtty_bws;
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
	filter_nbr = 0;

	has_mode_control = true;
	has_bandwidth_control = true;

//	has_split = true;
	has_smeter = true;
	has_mode_control = true;
	has_attenuator_control = true;

	defaultCIV = 0x58;
	adjustCIV(defaultCIV);

	precision = 1;
	ndigits = 9;

};

//=============================================================================

void RIG_IC706MKIIG::initialize()
{
	if (progStatus.use706filters) {
		if (!progStatus.ssb_cw_wide.empty())   IC706MKIIG_ssb_cw_rtty_bws[0] = progStatus.ssb_cw_wide.c_str();
		if (!progStatus.ssb_cw_normal.empty()) IC706MKIIG_ssb_cw_rtty_bws[1] = progStatus.ssb_cw_normal.c_str();
		if (!progStatus.ssb_cw_narrow.empty()) IC706MKIIG_ssb_cw_rtty_bws[2] = progStatus.ssb_cw_narrow.c_str();
	} else {
		IC706MKIIG_ssb_cw_rtty_bws[0] = "FIXED";
		IC706MKIIG_ssb_cw_rtty_bws[1] = NULL;
	  IC706MKIIG_vals_ssb_cw_rtty_bws[0] = 1;
	  IC706MKIIG_vals_ssb_cw_rtty_bws[1] = WVALS_LIMIT;
	}
}

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

bool RIG_IC706MKIIG::check ()
{
	string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	bool ok = waitFOR(11, "check vfo");
	rig_trace(2, "check()", str2hex(replystr.c_str(), replystr.length()));
	return ok;
}

long RIG_IC706MKIIG::get_vfoA ()
{
	if (useB) return A.freq;
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	string resp = pre_fm;
	resp += '\x03';
	if (waitFOR(11, "get vfo A")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			A.freq = fm_bcd_be(replystr.substr(p+5), 10);
	}
	return A.freq;
}

void RIG_IC706MKIIG::set_vfoA (long freq)
{
	A.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	waitFB("set vfo A");
}

long RIG_IC706MKIIG::get_vfoB ()
{
	if (!useB) return B.freq;
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	string resp = pre_fm;
	resp += '\x03';
	if (waitFOR(11, "get vfo B")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			B.freq = fm_bcd_be(replystr.substr(p+5), 10);
	}
	return B.freq;
}

void RIG_IC706MKIIG::set_vfoB (long freq)
{
	B.freq = freq;
	cmd = pre_to;
	cmd += '\x05';
	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	waitFB("set vfo B");
}

//void RIG_IC706MKIIG::set_split(bool b)
//{
//	cmd = pre_to;
//	cmd += 0x0F;
//	cmd += b ? 0x01 : 0x00;
//	cmd.append( post );
//	waitFB("set split");
//}

// When wide or normal operation is available, 
//   add “00” for wide operation or 
//       “01” for normal operation;
// When normal or narrow operation is available,
//   add “00” for normal operation or 
//       “01” for narrow operation;
// When wide, normal and narrow operation is available,
//   add “00” for wide operation,
//       “01” for normal operation 
//   and “02” for narrow

void RIG_IC706MKIIG::set_modeA(int val)
{
	modeA = val;
	cmd = pre_to;
	cmd += 0x06;
	cmd += val;
	switch (val) {
		case AM706: case FM706:  
		   cmd += IC706MKIIG_vals_am_fm_bws[filter_nbr];
			break;
		case WFM706:  
		   cmd += IC706MKIIG_vals_wfm_bws[filter_nbr]; 
			break;
		case LSB706: case USB706: case CW706: case RTTY706: default:
		  if (progStatus.use706filters) {
			  cmd += IC706MKIIG_vals_ssb_cw_rtty_bws[filter_nbr];
		 } else { // No filters
			cmd += 0x01;
		 }
			break;
	}
	cmd.append( post );
	waitFB("set mode A");
}

int RIG_IC706MKIIG::get_modeA()
{
	cmd = pre_to;
	cmd += 0x04;
	cmd.append(post);
	string resp = pre_fm;
	resp += 0x04;
	if (waitFOR(8, "get mode A")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			modeA = replystr[p+5];
			filter_nbr = replystr[p+6] - 1;
		}
	}
	return modeA;
}

int RIG_IC706MKIIG::get_modetype(int n)
{
	return _mode_type[n];
}

void RIG_IC706MKIIG::set_bwA(int val)
{
	filter_nbr = val;
	set_modeA(modeA);
}

int RIG_IC706MKIIG::get_bwA()
{
	return filter_nbr; 
}

int RIG_IC706MKIIG::adjust_bandwidth(int m)
{  
	switch (m) {
		case LSB706: case USB706: case CW706: case RTTY706: 
			bandwidths_ = IC706MKIIG_ssb_cw_rtty_bws;
			bw_vals_ = IC706MKIIG_vals_ssb_cw_rtty_bws;
			return 1;
			break;
		case AM706: case FM706:  
			bandwidths_ = IC706MKIIG_am_fm_bws;
			bw_vals_ = IC706MKIIG_vals_am_fm_bws;
			return 0;
			break;
		case WFM706:  
			bandwidths_ = IC706MKIIG_wfm_bws;
			bw_vals_ = IC706MKIIG_vals_wfm_bws;
			return 0;
			break;
		default:
			bandwidths_ = IC706MKIIG_ssb_cw_rtty_bws;
			bw_vals_ = IC706MKIIG_vals_ssb_cw_rtty_bws;
			return 1;
			break;
	}
	return 0;
}

int RIG_IC706MKIIG::def_bandwidth(int m)
{
	switch (m) {
		default:
		case LSB706: case USB706: case CW706: case RTTY706: 
			return 1;
			break;
		case AM706: case FM706:  
			return 0;
			break;
		case WFM706:  
			return 0;
			break;
	}
	return 1;
}

const char **RIG_IC706MKIIG::bwtable(int m)
{
	switch (m) {
		case LSB706: case USB706: case CW706: case RTTY706: 
			return IC706MKIIG_ssb_cw_rtty_bws;
			break;
		case AM706: case FM706:  
			return IC706MKIIG_am_fm_bws;
			break;
		case WFM706:  
			return IC706MKIIG_wfm_bws;
			break;
		default:
			return IC706MKIIG_ssb_cw_rtty_bws;
			break;
	}
}   

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
	if (waitFOR(9, "get smeter")) { // SHOULD BE 10 bytes
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			return (int)ceil(fm_bcd(replystr.substr(p+6), 3) / 2.55);
	}
	return -1;
}
