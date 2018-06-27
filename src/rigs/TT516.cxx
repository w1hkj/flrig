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

/*
 *	Note for anyone wishing to expand on the command set.
 *
 *	The Argo V always sends a response and ends the response with a "G\r" to
 *	indicate that the command was accepted.  A rejected command is responded to by a
 *	two character sequence "Z\r".  You should always expect a maximum response equal
 *	to the number of data bytes plus two.
 *
 *	For example:
 *		A request for the present receiver filter bandwidth is the the string:
 *			"?W\r" which is 3 bytes in length
 *		The response from the Argonaut V will be:
 *			"Wn\rG\r" which is 5 bytes in length, where n is an unsigned char (byte)
 *		If the transceiver failed to receive the command correctly it will respond:
 *			"Z\r" ----> you need to check for that condition
 *
*/

#include "TT516.h"
#include "support.h"

static const char TT516name_[] = "TT-516";

static const char *TT516modes_[] = {
		"AM", "USB", "LSB", "CW", "FM", NULL};

static int TT516_def_bw[] = { 26, 36, 36, 12, 36 };
static const char TT516mode_chr[] =  { '0', '1', '2', '3', '4' };
static const char TT516mode_type[] = { 'U', 'U', 'L', 'U', 'U' };

static const char *TT516_widths[] = {
"200", "250", "300", "350", "400", "450", "500", "550", "600", "650",
"700", "750", "800", "850", "900", "950", "1000", "1100", "1200", "1300",
"1400", "1500", "1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300",
"2400", "2500", "2600", "2700", "2800", "2900", "3000", NULL};
static int TT516_WIDTH_bw_vals[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37, WVALS_LIMIT};

static const char *TT516_AM_widths[] = {
"400", "500", "600", "700", "800", "900", "1000", "1100", "1200", "1350",
"1400", "1500", "1600", "1700", "1800", "1900", "2000", "2200", "2400", "2600",
"2800", "3000", "3200", "3400", "3600", "3800", "4000", "4200", "4400", "4600",
"4800", "5000", "5200", "5400", "5600", "5800", "6000", NULL};
static int TT516_AM_bw_vals[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37, WVALS_LIMIT};

static char TT516setBW[]		= "*Wx\r";
static char TT516setPBT[]		= "*Pxx\r";
static char TT516setMODE[]		= "*Mnn\r";
static char TT516setFREQA[]		= "*Annnn\r";
static char TT516setFREQB[]		= "*Bnnnn\r";
static char TT516setNB[]		= "*Kn\r";
static char TT516setXMT[]		= "#1\r";
static char TT516setRCV[]		= "#0\r";
static char TT516setSPLIT[]		= "*On\r";
static char TT516setATT[]		= "*Jn\r";
static char TT516getFREQA[]		= "?A\r";
static char TT516getFREQB[]		= "?B\r";
static char TT516getFWDPWR[]	= "?F\r";
static char TT516getATT[]		= "?J\r";
static char TT516getMODE[]		= "?M\r";
//static char TT516getPBT[]		= "?P\r";
static char TT516getREFPWR[]	= "?R\r";
static char TT516getSMETER[]	= "?S\r";
static char TT516getBW[]		= "?W\r";
static char TT516setVfo[]		= "*EVx\r";

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnIFsh,     214, 105,  50 },
	{ (Fl_Widget *)sldrIFSHIFT, 266, 105, 156 },
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

RIG_TT516::RIG_TT516() {
// base class values	
	name_ = TT516name_;
	modes_ = TT516modes_;
	bandwidths_ = TT516_widths;
	bw_vals_ = TT516_WIDTH_bw_vals;

	widgets = rig_widgets;

	comm_baudrate = BR1200;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 20;
	comm_timeout = 50;
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = false;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	serloop_timing = 200;

	def_mode = modeB = modeA = B.imode = A.imode = 1;
	def_bw = bwB = bwA = B.iBW = A.iBW = 34;
	def_freq = freqB = freqA = B.freq = A.freq = 14070000;

	max_power = 25;
	can_change_alt_vfo = true;

	has_power_control =
	has_volume_control =
	has_micgain_control =
	has_notch_control =
	has_preamp_control =
	has_tune_control =
	has_swr_control = false;

	has_smeter =
	has_power_out =
	has_swr_control =
	has_split =
	has_noise_control =
	has_attenuator_control =
	has_ifshift_control =
	has_ptt_control =
	has_bandwidth_control =
	has_mode_control = true;

	precision = 10;
	ndigits = 7;

}

void RIG_TT516::initialize()
{
	rig_widgets[0].W = btnIFsh;
	rig_widgets[1].W = sldrIFSHIFT;
}

bool RIG_TT516::check ()
{
	cmd = TT516getFREQA;
	int ret = waitN(8, 150, "check", HEX);
	if (ret != 8) return false;
	return true;
}

long RIG_TT516::get_vfoA ()
{
	cmd = TT516getFREQA;
	int ret = waitN(8, 150, "get vfo A", HEX);
	if (ret != 8) return A.freq;
	if (replystr[0] != 'A') return A.freq;
	if (replystr[6] != 'G') return A.freq;
	int f = 0;
	for (size_t n = 1; n < 5; n++) {
		f = f*256 + ((unsigned char)replystr[n] & 0xFF) ;
		A.freq = f;
	}
	return A.freq;
}

void RIG_TT516::set_vfoA (long freq)
{
	A.freq = freq;
	cmd = TT516setFREQA;
	cmd[5] = freq & 0xff; freq = freq >> 8;
	cmd[4] = freq & 0xff; freq = freq >> 8;
	cmd[3] = freq & 0xff; freq = freq >> 8;
	cmd[2] = freq & 0xff;
	LOG_INFO(" %c%c %02X %02X %02X %02X %02X", cmd[0], cmd[1],
		cmd[2] & 0xFF, cmd[3] & 0xFF, cmd[4] & 0xFF, cmd[5] & 0xFF, cmd[6]);
	sendCommand(cmd);
	return;
}

long RIG_TT516::get_vfoB ()
{
	cmd = TT516getFREQB;
	int ret = waitN(8, 150, "get vfo B", HEX);
	if (ret != 8) return B.freq;
	if (replystr[0] != 'B') return B.freq;
	if (replystr[6] != 'G') return B.freq;

	int f = 0;
	for (size_t n = 1; n < 5; n++) {
		f = f*256 + ((unsigned char)replystr[n] & 0xFF) ;
		B.freq = f;
	}

	return B.freq;
}

void RIG_TT516::set_vfoB (long freq)
{
	B.freq = freq;
	cmd = TT516setFREQB;
	cmd[5] = freq & 0xff; freq = freq >> 8;
	cmd[4] = freq & 0xff; freq = freq >> 8;
	cmd[3] = freq & 0xff; freq = freq >> 8;
	cmd[2] = freq & 0xff;
	LOG_INFO(" %c%c %02X %02X %02X %02X %02X", cmd[0], cmd[1],
		cmd[2] & 0xFF, cmd[3] & 0xFF, cmd[4] & 0xFF, cmd[5] & 0xFF, cmd[6]);
	sendCommand(cmd);
	return;
}

void RIG_TT516::selectA()
{
	cmd = TT516setVfo;
	cmd[3] = 'A';
	sendCommand(cmd);
	inuse = onA;
	set_bwA(A.iBW);
	return;
}

void RIG_TT516::selectB()
{
	cmd = TT516setVfo;
	cmd[3] = 'B';
	sendCommand(cmd);
	inuse = onB;
	set_bwB(B.iBW);
	return;
}

void RIG_TT516::set_split(bool val)
{
	cmd = TT516setSPLIT;
	cmd[2] = val ? '\x01' : '\x00';
	sendCommand(cmd);
	return;
}

// Tranceiver PTT on/off
void RIG_TT516::set_PTT_control(int val)
{
	if (val) sendCommand(TT516setXMT);
	else     sendCommand(TT516setRCV);
}

int RIG_TT516::get_modetype(int n)
{
	return TT516mode_type[n];
}

const char **RIG_TT516::bwtable(int m)
{
	if (m == 0) {
		bandwidths_ = TT516_AM_widths;
		bw_vals_ = TT516_AM_bw_vals;
		return TT516_AM_widths;
	}
	bandwidths_ = TT516_widths;
	bw_vals_ = TT516_WIDTH_bw_vals;
	return TT516_widths;
}

int RIG_TT516::def_bandwidth(int m)
{
	return TT516_def_bw[m];
}

int RIG_TT516::adjust_bandwidth(int m)
{
	bwtable(m);
	return def_bandwidth(m);
}

void RIG_TT516::set_modeA(int val)
{
	A.imode = val;
	cmd = TT516setMODE;
	cmd[2] = TT516mode_chr[A.imode];
	cmd[3] = TT516mode_chr[B.imode];
	sendCommand(cmd);
}

int RIG_TT516::get_modeA()
{
	cmd = TT516getMODE;
	int ret = waitN(6, 150, "get mode A", HEX);
	if (ret < 6) return A.imode;
	if (replystr[ret - 2] != 'G') return A.imode;
	size_t p = replystr.rfind("M");
	if (p == string::npos) return A.imode;

	A.imode = replystr[p+1] - '0';

	return A.imode;
}

void RIG_TT516::set_modeB(int val)
{
	B.imode = val;
	cmd = TT516setMODE;
	cmd[2] = TT516mode_chr[A.imode];
	cmd[3] = TT516mode_chr[B.imode];
	sendCommand(cmd);
}

int RIG_TT516::get_modeB()
{
	cmd = TT516getMODE;
	int ret = waitN(6, 150, "get mode B", HEX);
	if (ret < 6) return B.imode;
	if (replystr[ret - 2] != 'G') return B.imode;
	size_t p = replystr.rfind("M");
	if (p == string::npos) return B.imode;

	B.imode = replystr[p+2] - '0';

	return B.imode;
}

int RIG_TT516::get_bwA()
{
	if (inuse == onA) {
		cmd = TT516getBW;
		int ret = waitN(5, 150, "get bw A", HEX);
		if (ret < 5) return A.iBW;
		if (replystr[ret - 2] != 'G') return A.iBW;
		size_t p = replystr.rfind("W");
		if (p == string::npos) return A.iBW;
		A.iBW = (unsigned char)replystr[p+1];
	}
	return A.iBW;
}

void RIG_TT516::set_bwA(int val)
{
	A.iBW = val;
	if (inuse == onA) {
		cmd = TT516setBW;
		cmd[2] = val;
		sendCommand(cmd);
	}
}

int RIG_TT516::get_bwB()
{
	if (inuse == onB) {
		cmd = TT516getBW;
		int ret = waitN(5, 150, "get bw B", HEX);
		if (ret < 5) return B.iBW;
		if (replystr[ret - 2] != 'G') return B.iBW;
		size_t p = replystr.rfind("W");
		if (p == string::npos) return B.iBW;
		B.iBW = (unsigned char)replystr[p+1];
	}
	return B.iBW;
}

void RIG_TT516::set_bwB(int val)
{
	B.iBW = val;
	if (inuse == onB) {
		cmd = TT516setBW;
		cmd[2] = val;
		sendCommand(cmd);
	}
}

void RIG_TT516::set_if_shift(int val)
{
	cmd = TT516setPBT;
	short int si = val;
	cmd[2] = (si & 0xff00) >> 8;
	cmd[3] = (si & 0xff);
	sendCommand(cmd);
}

bool RIG_TT516::get_if_shift(int &val)
{
	val = 0;
	return false;
}

void RIG_TT516::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -2900;
	max = 2900;
	step = 100;
}

void RIG_TT516::set_attenuator(int val)
{
	cmd = TT516setATT;
	if (val) cmd[2] = '1';
	else     cmd[2] = '0';
	sendCommand(cmd);
}


int RIG_TT516::get_attenuator()
{
	cmd = TT516getATT;
	int ret = sendCommand(cmd);
	if (ret < 5) return 0;
	size_t p = replystr.rfind("J");
	if (p == string::npos) return 0;
	if (replystr[p+1] == '1')
		return 1;
	return 0;
}

void RIG_TT516::set_noise(bool b)
{
	cmd = TT516setNB;
	if (b)
		cmd[2] = '4';
	else
		cmd[2] = '0';
	sendCommand(cmd);
}

int RIG_TT516::get_smeter()
{
	double sig = 0.0;
	cmd = TT516getSMETER;
	int ret = sendCommand(cmd);
	if (ret < 6) return 0;
	size_t p = replystr.rfind("S");
	if (p == string::npos) return 0;

	sig = (50.0 / 9.0) * ((unsigned char)replystr[p+1] + (unsigned char)replystr[p+2] / 256.0);

	return (int)sig;
}

int RIG_TT516::get_swr()
{
	double swr = (fwdv + refv) / (fwdv - refv + .0001);
	swr -= 1.0;
	swr *= 25.0;
	if (swr < 0) swr = 0;
	if (swr > 100) swr = 100;
	return (int)swr;
}

int RIG_TT516::get_power_out()
{
	fwdpwr = refpwr = fwdv = refv = 0;
	cmd = TT516getFWDPWR;
	int ret = sendCommand(cmd);
	if (ret < 5) return fwdpwr;
	size_t p = replystr.rfind("F");
	if (p == string::npos) return fwdpwr;

	fwdv = 1.0 * (unsigned char)replystr[p+1];
	cmd = TT516getREFPWR;
	ret = sendCommand(cmd);
	if (ret < 5) return fwdpwr;
	p = replystr.rfind("R");
	if (p == string::npos) return fwdpwr;

	refv = 1.0 * (unsigned char)replystr[p+1];

	fwdpwr = 30.0 * (fwdv * fwdv) / (256 * 256);
	refpwr = 30.0 * (refv * refv) / (256 * 256);

	return fwdpwr;
}
