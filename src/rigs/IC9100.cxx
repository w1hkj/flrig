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

#include "IC9100.h"

bool IC9100_DEBUG = true;

//=============================================================================
// IC-9100

const char IC9100name_[] = "IC-9100";

const char *IC9100modes_[] = {
	"LSB", "USB", "AM", "CW", "RTTY", "FM", "CW-R", "RTTY-R", "DV", NULL};

const char IC9100_mode_type[] = {
	'L', 'U', 'U', 'U', 'L', 'U', 'L', 'U', 'U' };

const char IC9100_mode_nbr[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x07, 0x08, 0x17 };

const char *IC9100_bws[] = { "FIL1", "FIL2", "FIL3", NULL };
const int IC9100_bw_vals[] = { 0x01, 0x02, 0x03, WVALS_LIMIT};

static GUI IC9100_widgets[]= {
	{ (Fl_Widget *)btnVol, 2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME, 54, 125, 156 },
	{ (Fl_Widget *)sldrRFGAIN, 54, 145, 156 },
	{ (Fl_Widget *)sldrSQUELCH, 54, 165, 156 },
	{ (Fl_Widget *)btnNR, 2, 185,  50 },
	{ (Fl_Widget *)sldrNR, 54, 185, 156 },
	{ (Fl_Widget *)btnIFsh, 214, 125,  50 },
	{ (Fl_Widget *)sldrIFSHIFT, 266, 125, 156 },
	{ (Fl_Widget *)btnNotch, 214, 145,  50 },
	{ (Fl_Widget *)sldrNOTCH, 266, 145, 156 },
	{ (Fl_Widget *)sldrMICGAIN, 266, 165, 156 },
	{ (Fl_Widget *)sldrPOWER, 266, 185, 156 },
	{ (Fl_Widget *)NULL, 0, 0, 0 }
};

RIG_IC9100::RIG_IC9100() {
	defaultCIV = 0x7C;
	name_ = IC9100name_;
	modes_ = IC9100modes_;
	bandwidths_ = IC9100_bws;
	bw_vals_ = IC9100_bw_vals;

	_mode_type = IC9100_mode_type;
	adjustCIV(defaultCIV);

	widgets = IC9100_widgets;

	has_extras =

	has_cw_wpm =
	has_cw_spot_tone =
	has_cw_qsk =

	has_vox_onoff =
	has_vox_gain =
	has_vox_anti =
	has_vox_hang =

	has_smeter =
	has_power_out =
	has_alc_control =
	has_swr_control =

	has_compON =
	has_compression =

	has_auto_notch = 
	has_notch_control = 

	has_split =

	has_micgain_control =
	has_bandwidth_control = true;

	precision = 1;
	ndigits = 9;

};

//======================================================================
// IC9100 unique commands
//======================================================================

void RIG_IC9100::initialize()
{
	IC9100_widgets[0].W = btnVol;
	IC9100_widgets[1].W = sldrVOLUME;
	IC9100_widgets[2].W = sldrRFGAIN;
	IC9100_widgets[3].W = sldrSQUELCH;
	IC9100_widgets[4].W = btnNR;
	IC9100_widgets[5].W = sldrNR;
	IC9100_widgets[6].W = btnIFsh;
	IC9100_widgets[7].W = sldrIFSHIFT;
	IC9100_widgets[8].W = btnNotch;
	IC9100_widgets[9].W = sldrNOTCH;
	IC9100_widgets[10].W = sldrMICGAIN;
	IC9100_widgets[11].W = sldrPOWER;
	cmd = pre_to;
	cmd += '\x1A'; cmd += '\x05';
	cmd += '\x00'; cmd += '\x58'; cmd += '\x00';
	cmd.append(post);
	sendICcommand(cmd,6);
	checkresponse();
}

void RIG_IC9100::selectA()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\xD0';
	cmd.append(post);
	waitFB("select A");
}

void RIG_IC9100::selectB()
{
	cmd = pre_to;
	cmd += '\x07';
	cmd += '\xD1';
	cmd.append(post);
	waitFB("select B");
}

void RIG_IC9100::set_modeA(int val)
{
	A.imode = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += IC9100_mode_nbr[val];
	cmd.append( post );
	if (IC9100_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	waitFB("set mode A");
}

int RIG_IC9100::get_modeA()
{
	int md = 0;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	string resp = pre_fm;
	resp += '\x04';
	if (waitFOR(8, "get mode/bw A")) {
		for (md = 0; md < 9; md++) if (replystr[5] == IC9100_mode_nbr[md]) break;
	} else {
		checkresponse();
		return A.imode;
	}
	if (md > 8) md = 1; // force USB
	A.imode = md;
	return A.imode;
}

void RIG_IC9100::set_modeB(int val)
{
	B.imode = val;
	cmd = pre_to;
	cmd += '\x06';
	cmd += IC9100_mode_nbr[val];
	cmd.append( post );
	if (IC9100_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	waitFB("set mode B");
}

int RIG_IC9100::get_modeB()
{
	int md = 0;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	string resp = pre_fm;
	resp += '\x04';
	if (waitFOR(8, "get mode A")) {
		for (md = 0; md < 9; md++) if (replystr[5] == IC9100_mode_nbr[md]) break;
	} else {
		checkresponse();
		return B.imode;
	}
	if (md > 8) md = 1; // force USB
	B.imode = md;
	return B.imode;
}

int RIG_IC9100::get_bwA()
{
	int bw = 0;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	string resp = pre_fm;
	resp += '\x04';
	if (waitFOR(8, "get mode/bw A")) {
		for (bw = 0; bw < 3; bw++) if (replystr[6] == IC9100_bw_vals[bw]) break;
	} else {
		return A.iBW;
	}
	if (bw > 2) bw = 0;
	A.iBW = bw;
	return A.iBW;
}

void RIG_IC9100::set_bwA(int val)
{
	A.iBW = val;
	cmd = pre_to;
	cmd += '\x01';
	cmd += IC9100_mode_nbr[A.imode];
	cmd += IC9100_bw_vals[A.iBW];
	cmd.append( post );
	if (IC9100_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	waitFB("set bandwidth A");
}

int RIG_IC9100::get_bwB()
{
	int bw = 0;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	string resp = pre_fm;
	resp += '\x04';
	if (waitFOR(8, "get mode/bw B")) {
		for (bw = 0; bw < 3; bw++) if (replystr[6] == IC9100_bw_vals[bw]) break;
	} else {
		return B.iBW;
	}
	if (bw > 2) bw = 0;
	B.iBW = bw;
	return B.iBW;
}

void RIG_IC9100::set_bwB(int val)
{
	B.iBW = val;
	cmd = pre_to;
	cmd += '\x01';
	cmd += IC9100_mode_nbr[B.imode];
	cmd += IC9100_bw_vals[B.iBW];
	cmd.append( post );
	if (IC9100_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
	waitFB("set bandwidth B");
}

int RIG_IC9100::adjust_bandwidth(int m)
{
	return 0;
}

int RIG_IC9100::def_bandwidth(int m)
{
	return 0;
}

void RIG_IC9100::set_mic_gain(int v)
{
	ICvol = (int)(v * 255 / 100);
	if (!progStatus.USBaudio) {
		cmd = pre_to;
		cmd.append("\x14\x0B");
		cmd.append(to_bcd(ICvol, 3));
		cmd.append( post );
	} else {
		cmd = pre_to;
		cmd += '\x1A'; cmd += '\x05';
		cmd += '\x00'; cmd += '\x29';
		cmd.append(to_bcd(ICvol, 3));
		cmd.append( post );
	}
	waitFB("set mic gain");
	if (IC9100_DEBUG)
		LOG_WARN("%s", str2hex(cmd.data(), cmd.length()));
}

void RIG_IC9100::set_attenuator(int val)
{
	int cmdval = 0;
	if (atten_level == 0) {
		atten_level = 1;
		atten_label("20 dB", true);
		cmdval = 0x20;
	} else {
		atten_level = 0;
		atten_label("Att", false);
		cmdval = 0x00;
	}
	cmd = pre_to;
	cmd += '\x11';
	cmd += cmdval;
	cmd.append( post );
	waitFB("set attenuator");
	if (IC9100_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

int RIG_IC9100::get_attenuator()
{
	cmd = pre_to;
	cmd += '\x11';
	cmd.append( post );
	string resp = pre_fm;
	resp += '\x11';
	if (waitFOR(7, "get attenuator")) {
		if (replystr[5] == 0x20) {
			atten_level = 1;
			atten_label("20 dB", true);
		} else {
			atten_level = 0;
			atten_label("Att", false);
		}
	}
	return atten_level;
}

void RIG_IC9100::set_compression()
{
	if (progStatus.compON) {
		cmd.assign(pre_to).append("\x14\x0E");
		cmd.append(to_bcd(progStatus.compression * 255 / 100, 3));
		cmd.append( post );
		waitFB("set comp");

		cmd = pre_to;
		cmd.append("\x16\x44");
		cmd += '\x01';
		cmd.append(post);
		waitFB("set Comp ON");

	} else{
		cmd.assign(pre_to).append("\x16\x44");
		cmd += '\x00';
		cmd.append(post);
		waitFB("set Comp OFF");
	}
}

void RIG_IC9100::set_vox_onoff()
{
	cmd.assign(pre_to).append("\x16\x46");
	if (progStatus.vox_onoff) {
		cmd += '\x01';
		cmd.append( post );
		waitFB("set vox ON");
	} else {
		cmd += '\x00';
		cmd.append( post );
		waitFB("set vox OFF");
	}
}

void RIG_IC9100::set_vox_gain()
{
	cmd.assign(pre_to).append("\x1A\x05");
	cmd +='\x01';
	cmd +='\x25';
	cmd.append(to_bcd((int)(progStatus.vox_gain * 2.55), 3));
	cmd.append( post );
	waitFB("SET vox gain");
}

void RIG_IC9100::set_vox_anti()
{
	cmd.assign(pre_to).append("\x1A\x05");
	cmd +='\x01';
	cmd +='\x26';
	cmd.append(to_bcd((int)(progStatus.vox_anti * 2.55), 3));
	cmd.append( post );
	waitFB("SET anti-vox");
}

void RIG_IC9100::set_vox_hang()
{
	cmd.assign(pre_to).append("\x1A\x05");	//values 00-20 = 0.0 - 2.0 sec
	cmd +='\x01';
	cmd +='\x27';
	cmd.append(to_bcd((int)(progStatus.vox_hang / 10 ), 2));
	cmd.append( post );
	waitFB("SET vox hang");
}

// these need to be written and tested
void  RIG_IC9100::get_vox_onoff()
{
}

void  RIG_IC9100::get_vox_gain()
{
}

void  RIG_IC9100::get_vox_anti()
{
}

void  RIG_IC9100::get_vox_hang()
{
}

// CW controls

void RIG_IC9100::set_cw_wpm()
{
	cmd.assign(pre_to).append("\x14\x0C"); // values 0-255
	cmd.append(to_bcd(round((progStatus.cw_wpm - 6) * 255 / (60 - 6)), 3));
	cmd.append( post );
	waitFB("SET cw wpm");
}

void RIG_IC9100::set_cw_qsk()
{
	int n = round(progStatus.cw_qsk * 10); // values 0-255
	cmd.assign(pre_to).append("\x14\x0F");
	cmd.append(to_bcd(n, 3));
	cmd.append(post);
	waitFB("Set cw qsk delay");
}

void RIG_IC9100::set_cw_spot_tone()
{
	cmd.assign(pre_to).append("\x14\x09"); // values 0=300Hz 255=900Hz
	int n = round((progStatus.cw_spot_tone - 300) * 255.0 / 600.0);
	if (n > 255) n = 255;
	if (n < 0) n = 0;
	cmd.append(to_bcd(n, 3));
	cmd.append( post );
	waitFB("SET cw spot tone");
}

void RIG_IC9100::set_cw_vol()
{
	cmd.assign(pre_to);
	cmd.append("\x1A\x05");
	cmd += '\x00';
	cmd += '\x24';
	cmd.append(to_bcd((int)(progStatus.cw_vol * 2.55), 3));
	cmd.append( post );
	waitFB("SET cw sidetone volume");
}

int RIG_IC9100::get_smeter()
{
	string cstr = "\x15\x02";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;
	if (waitFOR(9, "get smeter")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			mtr = fm_bcd(&replystr[p+6], 3);
			mtr = (int)ceil(mtr /2.40);
			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

int RIG_IC9100::get_power_out()
{
	string cstr = "\x15\x11";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;
	if (waitFOR(9, "get pout")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			mtr = fm_bcd(&replystr[p+6], 3);
			mtr = (int)ceil(mtr /2.15);
			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

int RIG_IC9100::get_swr()
{
	string cstr = "\x15\x12";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;
	if (waitFOR(9, "get SWR")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			mtr = fm_bcd(&replystr[p+6], 3);
			mtr = (int)ceil(mtr /2.40);
			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

int RIG_IC9100::get_alc()
{
	string cstr = "\x15\x13";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;
	if (waitFOR(9, "get alc")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			mtr = fm_bcd(&replystr[p+6], 3);
			mtr = (int)ceil(mtr /2.55);
			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

void RIG_IC9100::set_notch(bool on, int val)
{
	int notch = (int)(val * 256.0 / 3000.0);

	cmd = pre_to;
	cmd.append("\x16\x48");
	cmd += on ? '\x01' : '\x00';
	cmd.append(post);
	waitFB("set notch");

	cmd = pre_to;
	cmd.append("\x14\x0D");
	cmd.append(to_bcd(notch,3));
	cmd.append(post);
	waitFB("set notch val");

}

bool RIG_IC9100::get_notch(int &val)
{
	bool on = false;
	val = 0;

	string cstr = "\x16\x48";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get notch")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			on = replystr[p + 6];
		cmd = pre_to;
		resp = pre_fm;
		cstr = "\x14\x0D";
		cmd.append(cstr);
		resp.append(cstr);
		cmd.append(post);
		if (waitFOR(9, "notch val")) {
			size_t p = replystr.rfind(resp);
			if (p != string::npos)
				val = (int)ceil(fm_bcd(&replystr[p+6],3) * 3000.0 / 255.0);
		}
	}
	return on;
}

void RIG_IC9100::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 3000;
	step = 20;
}

void RIG_IC9100::set_auto_notch(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x41';
	cmd += val ? 0x01 : 0x00;
	cmd.append( post );
	waitFB("set AN");
}

int RIG_IC9100::get_auto_notch()
{
	string cstr = "\x16\x41";
	string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get AN")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos) {
			if (replystr[p+6] == 0x01) {
				auto_notch_label("AN", true);
				return true;
			} else {
				auto_notch_label("AN", false);
				return false;
			}
		}
	}
	return progStatus.auto_notch;
}

void RIG_IC9100::set_split(bool val)
{
	cmd = pre_to;
	cmd += 0x0F;
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	waitFB("set split");
}

int  RIG_IC9100::get_split()
{
	LOG_WARN("%s", "get split - not implemented");
	return progStatus.split;
}

