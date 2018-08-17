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
 *	The Jupiter always sends a response and ends the response with a "G\r" to
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

#include "TT538.h"
#include "support.h"
#include "math.h"

static const char TT538name_[] = "TT-538";

//static const char *TT538modes_[] = { "D-USB", "USB", "LSB", "CW", "AM", "FM", NULL}
//static const char TT538mode_chr[] =  { '1', '1', '2', '3', '0', '4' };
//static const char TT538mode_type[] = { 'U', 'U', 'L', 'L', 'U', 'U' };
static const char *TT538modes_[] = {
		"AM", "USB", "LSB", "CW", "FM", NULL};
static const char TT538mode_chr[] =  { '0', '1', '2', '3', '4' };
static const char TT538mode_type[] = { 'U', 'U', 'L', 'U', 'U' };

// filter # is 38 - index
static const char *TT538_widths[] = {
"150",   "165",  "180",  "225",  "260",  "300",  "330",  "375",  "450",  "525",
"600",   "675",  "750",  "900", "1050", "1200", "1350", "1500", "1650", "1800",
"1950", "2100", "2250", "2400", "2550", "2700", "2850", "3000", "3300", "3600",
"3900", "4200", "4500", "4800", "5100", "5400", "5700", "6000", "8000", NULL};
static int TT538_bw_vals[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,
WVALS_LIMIT};

static const int TT538_numeric_widths[] = {
150,   165,  180,  225,  260,  300,  330,  375,  450,  525,
600,   675,  750,  900, 1050, 1200, 1350, 1500, 1650, 1800,
1950, 2100, 2250, 2400, 2550, 2700, 2850, 3000, 3300, 3600,
3900, 4200, 4500, 4800, 5100, 5400, 5700, 6000, 8000};

static char TT538setFREQA[]		= "*Annnn\r";
//static char TT538setFREQB[]		= "*Bnnnn\r";
//static char TT538setAGC[]		= "*Gn\r";
//static char TT538setSQLCH[]		= "*Hc\r";
static char TT538setRF[]		= "*Ic\r";
static char TT538setATT[]		= "*Jc\r";
//static char TT538setNB[]		= "*Knar\r";
static char TT538setMODE[]		= "*Mnn\r";
static char TT538setPBT[]		= "*Pxx\r";
static char TT538setVOL[]		= "*Un\r";
static char TT538setBW[]		= "*Wx\r";

static char TT538getFREQA[]		= "?A\r";
//static char TT538getFREQB[]		= "?B\r";
//static char TT538getFWDPWR[]	= "?F\r";
//static char TT538getAGC[]		= "?G\r";
//static char TT538getSQLCH[]		= "?H\r";
//static char TT538getRF[]		= "?I\r";
static char TT538getATT[]		= "?J\r";
//static char TT538getNB[]		= "?K\r";
static char TT538getMODE[]		= "?M\r";
static char TT538getPBT[]		= "?P\r";
static char TT538getSMETER[]	= "?S\r";
static char TT538getVOL[]		= "?U\r";
static char TT538getBW[]		= "?W\r";

//static char TT538getREFPWR[]	= "?R\r";
//static char TT538setXMT[]		= "#1\r";
//static char TT538setRCV[]		= "#0\r";

static char TT538setXMT[]		= "Q1\r";
static char TT538setRCV[]		= "Q0\r";

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 },
	{ (Fl_Widget *)sldrRFGAIN,  266, 125, 156 },
	{ (Fl_Widget *)btnIFsh,     214, 105,  50 },
	{ (Fl_Widget *)sldrIFSHIFT, 266, 105, 156 },
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

RIG_TT538::RIG_TT538() {
// base class values
	name_ = TT538name_;
	modes_ = TT538modes_;
	bandwidths_ = TT538_widths;
	bw_vals_ = TT538_bw_vals;

	widgets = rig_widgets;

	comm_baudrate = BR57600;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 20;
	comm_timeout = 50;
	comm_rtscts = true;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = true;// false;
	comm_rtsptt = false;
	comm_dtrptt = false;
	serloop_timing = 200;

	def_mode = modeB = modeA = 1;
	def_bw = bwB = bwA = 25;
	def_freq = freqB = freqA = 14070000;

	max_power = 100;
	pbt = 0;
	VfoAdj = progStatus.vfo_adj;

	has_power_control =
	has_micgain_control =
	has_notch_control =
	has_preamp_control =
	has_tune_control =
	has_noise_control = false;

	has_smeter =
	has_power_out =
	has_swr_control =
	has_bpf_center = 
	has_volume_control =
	has_rf_control =
	has_attenuator_control =
	has_ifshift_control =
	has_ptt_control =
	has_bandwidth_control =
	has_mode_control = 
	has_vfo_adj = true;

	precision = 1;
	ndigits = 8;

}

//static int corrA = 0, corrB = 0;

#define sTT(s) set_trace(3, s, str2hex(cmd.c_str(), cmd.length()), str2hex(replystr.c_str(), replystr.length()))
#define gTT(s) get_trace(3, s, str2hex(cmd.c_str(), cmd.length()), str2hex(replystr.c_str(), replystr.length()))

void RIG_TT538::checkresponse(string s)
{
	if (RigSerial->IsOpen() == false)
		return;
	gTT(s.c_str());
}

void RIG_TT538::initialize()
{
	rig_widgets[0].W = btnVol;
	rig_widgets[1].W = sldrVOLUME;
	rig_widgets[2].W = sldrRFGAIN;
	rig_widgets[3].W = btnIFsh;
	rig_widgets[4].W = sldrIFSHIFT;

	VfoAdj = progStatus.vfo_adj;
	fwdpwr = refpwr = 0.0;
}

void RIG_TT538::shutdown()
{
	set_if_shift(0);
}

bool RIG_TT538::check ()
{
	cmd = TT538getFREQA;
	int ret = sendCommand(cmd);
	gTT("check");

	if (ret < 6) return false;
	return true;
}

long RIG_TT538::get_vfoA ()
{
	cmd = TT538getFREQA;
	int ret = sendCommand(cmd);
	gTT("get vfoA");

	if (ret < 6) return (long)(freqA / (1 + VfoAdj/1e6) + 0.5);
	size_t p = replystr.rfind("A");
	if (p == string::npos) return (long)(freqA / (1 + VfoAdj/1e6) + 0.5);
	
	int f = 0;
	for (size_t n = 1; n < 5; n++)
		f = f*256 + (unsigned char)replystr[p + n];
	freqA = f;

	return (long)(freqA / (1 + VfoAdj/1e6) + 0.5);
}

void RIG_TT538::set_vfoA (long freq)
{
	freqA = freq;
	long xfreq = freqA * (1 + VfoAdj/1e6) + 0.5;
	cmd = TT538setFREQA;
	cmd[5] = xfreq & 0xff; xfreq = xfreq >> 8;
	cmd[4] = xfreq & 0xff; xfreq = xfreq >> 8;
	cmd[3] = xfreq & 0xff; xfreq = xfreq >> 8;
	cmd[2] = xfreq & 0xff;
	sendCommand(cmd);
	sTT("set vfoA");

	set_if_shift(pbt);
	return ;
}

long RIG_TT538::get_vfoB ()
{
//	cmd = TT538getFREQB;
	cmd = TT538getFREQA;
	int ret = sendCommand(cmd);
	gTT("get vfoB");

	if (ret < 6) return (long)(freqB / (1 + VfoAdj/1e6) + 0.5);
	size_t p = replystr.rfind("B");
	if (p == string::npos) return (long)(freqB / (1 + VfoAdj/1e6) + 0.5);

	int f = 0;
	for (size_t n = 1; n < 5; n++)
		f = f*256 + (unsigned char)replystr[p + n];
	freqB = f;

	return (long)(freqB / (1 + VfoAdj/1e6) + 0.5);
}

void RIG_TT538::set_vfoB (long freq)
{
	freqB = freq;
	long xfreq = freqB * (1 + VfoAdj/1e6) + 0.5;
//	cmd = TT538setFREQB;
	cmd = TT538setFREQA;
	cmd[5] = xfreq & 0xff; xfreq = xfreq >> 8;
	cmd[4] = xfreq & 0xff; xfreq = xfreq >> 8;
	cmd[3] = xfreq & 0xff; xfreq = xfreq >> 8;
	cmd[2] = xfreq & 0xff;
	sendCommand(cmd);
	sTT("set vfoB");

	set_if_shift(pbt);
	return ;
}

void RIG_TT538::setVfoAdj(double v)
{
	VfoAdj = v;
}

void RIG_TT538::set_modeA(int val)
{
	modeA = val;
	cmd = TT538setMODE;
	cmd[2] = TT538mode_chr[val];
	cmd[3] = cmd[2];
	sendCommand(cmd);
	sTT("set modeA");
}

int RIG_TT538::get_modeA()
{
	cmd = TT538getMODE;
	int ret = sendCommand(cmd);
	gTT("get modeA");

	if (ret < 4) return modeA;
	size_t p = replystr.rfind("M");
	if (p == string::npos) return modeA;
	modeA = replystr[p+1] - '0';
	return modeA;
}

int RIG_TT538::get_modetype(int n)
{
	return TT538mode_type[n];
}

void RIG_TT538::set_bwA(int val)
{
	bwA = val;
	cmd = TT538setBW;
	cmd[2] = 38 - val;
	sendCommand(cmd);
	sTT("set bwA");

	set_if_shift(pbt);
}

int RIG_TT538::get_bwA()
{
	cmd = TT538getBW;
	int ret = sendCommand(cmd);
	gTT("get bwA");

	if (ret < 3) return bwA;
	size_t p = replystr.rfind("W");
	if (p == string::npos) return bwA;
	bwA = 38 - (unsigned char)replystr[p + 1];
	return bwA;
}

int  RIG_TT538::adjust_bandwidth(int m)
{
	if (m == 0) return 37;
	if (m == 1 || m == 2) return 27;
	if (m == 3) return 10;
	if (m == 4) return 27;
	return 27;
}

void RIG_TT538::set_if_shift(int val)
{
	pbt = val;
	cmd = TT538setPBT;
	int bpval = progStatus.bpf_center - 200 - TT538_numeric_widths[bwA]/2;
	short int si = val;
	if ((modeA == 1 || modeA == 2) && progStatus.use_bpf_center)
		si += (bpval > 0 ? bpval : 0);
	cmd[2] = (si & 0xff00) >> 8;
	cmd[3] = (si & 0xff);
	sendCommand(cmd);
	int ret = sendCommand(TT538getPBT);
	gTT("get PBT");

	if (ret < 4) return;
	size_t p = replystr.rfind("P");
	if (p == string::npos) return;
	if (replystr[p+1] != cmd[2] || replystr[p+2] != cmd[3]) {
		sendCommand(cmd);
		sTT("set if shift");
	}
}

bool RIG_TT538::get_if_shift(int &val)
{
	val = 0;
//	cmd = TT538getPBT;
//	sendCommand(cmd, 0);
	return false;
}

void RIG_TT538::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -2000;
	max = 2000;
	step = 10;
}

void RIG_TT538::set_attenuator(int val)
{
	cmd = TT538setATT;
	if (val) cmd[2] = '1';
	else     cmd[2] = '0';
	sendCommand(cmd);
	sTT("set attenuator");
}


int RIG_TT538::get_attenuator()
{
	cmd = TT538getATT;
	int ret = sendCommand(cmd);
	gTT("get attenuator");

	if (ret < 3) return 0;
	size_t p = replystr.rfind("J");
	if (p == string::npos) return 0;
	if (replystr[p+1] == '1')
		return 1;
	return 0;
}

int RIG_TT538::get_smeter()
{
	double sig = 0.0;
	cmd = TT538getSMETER;
	int ret = sendCommand(cmd);
	gTT("get smeter");

	if (ret < 6) return 0;
	size_t p = replystr.rfind("S");
	if (p == string::npos) return 0;

	int sval;
	replystr[p+5] = 0;
	sscanf(&replystr[p+1], "%4x", &sval);
	sig = sval / 256.0;

	return (int)(sig * 50.0 / 9.0);
}

static int gaintable[] = {
0, 2, 3, 4, 6, 7, 8, 9, 11, 12,
13, 14, 16, 17, 18, 19, 21, 22, 23, 24,
26, 27, 28, 30, 31, 32, 33, 35, 36, 37,
38, 40, 41, 42, 43, 45, 46, 47, 48, 50,
51, 52, 53, 55, 56, 57, 58, 60, 61, 62,
64, 65, 66, 68, 69, 70, 71, 72, 74, 75,
76, 77, 79, 80, 81, 82, 84, 85, 86, 88,
89, 90, 91, 92, 94, 95, 96, 98, 99, 100,
101, 103, 104, 105, 107, 108, 109, 110, 111, 113,
114, 115, 117, 118, 119, 120, 121, 123, 127, 125, 127 };

static int pot2val(int n)
{
	int i = 0;
	for (i = 0; i < 100; i++)
		if (gaintable[i] >= n) break;
	return gaintable[i];
}

static int val2pot(int n)
{
	return gaintable[n];
}

int RIG_TT538::get_volume_control()
{
	cmd = TT538getVOL;
	int ret = sendCommand(cmd);
	gTT("get volume control");

	if (ret < 3) return 0;
	size_t p = replystr.rfind("U");
	if (p == string::npos) return 0;

	return  pot2val(replystr[p+1] & 0x7F);
}

void RIG_TT538::set_volume_control(int vol)
{
	cmd = TT538setVOL;
	cmd[2] = val2pot(vol);
	sendCommand(cmd);
	sTT("set volume control");
}

void RIG_TT538::set_rf_gain(int val)
{
	cmd = TT538setRF;
	cmd[2] = val2pot(val);
	sendCommand(cmd);
	sTT("set rf gain");
}

int  RIG_TT538::get_rf_gain()
{
//	cmd = TT538getRF;
//	sendCommand(cmd, 3, true);
//showresponse(cmd);
	return 100; 
// Jupiter does not reply with values as specified in the programmers manual
// Panel RF gain 0..50% replies with 80H..00H
// Panel RF gain 50..100% replies with 00H
}

// Tranceiver PTT on/off
void RIG_TT538::set_PTT_control(int val)
{
	if (val) sendCommand(TT538setXMT);
	else     sendCommand(TT538setRCV);
	sTT("set PTT");
}

int RIG_TT538::get_power_out()
{
	int ret = sendCommand("?S\r"); // same as get smeter in receive
	gTT("get power out");

	if (ret < 4) return fwdpwr;
	size_t p = replystr.rfind("T");
	if (p == string::npos) return fwdpwr;

	fwdpwr = 0.8*fwdpwr + 0.2*(unsigned char)replystr[p+1];
	refpwr = 0.8*refpwr + 0.2*(unsigned char)replystr[p+2];

	return (int)fwdpwr;
}

int RIG_TT538::get_swr()
{
	double swr, nu;
	if (fwdpwr == 0) return 0;
	if (fwdpwr == refpwr) return 100;
	nu = sqrt(refpwr / fwdpwr);
	swr = (1 + nu) / (1 - nu) - 1.0;
	swr *= 16.67;
	if (swr < 0) swr = 0;
	if (swr > 100) swr = 100;
	
//LOG_INFO("swr %4.2f", swr);
	return (int)swr;
}
