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

// TenTec Pegasus computer controlled transceiver

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>

#include <math.h>
#include <vector>
#include <queue>

#include "TT550.h"
#include "support.h"
#include "util.h"
#include "debug.h"
#include "trace.h"

#include "rigbase.h"
#include "rig.h"

static const char TT550name_[] = "TT-550";

enum TT550_MODES {
TT550_AM_MODE, TT550_USB_MODE, TT550_LSB_MODE, TT550_CW_MODE, TT550_DIGI_MODE, TT550_FM_MODE };

static const char *TT550modes_[] = {
		"AM", "USB", "LSB", "CW", "DIGI", "FM", NULL};

static const int TT550_def_bw[] = { 32, 20, 20, 10, 20, 32 };

static const char TT550mode_chr[] =  { '0', '1', '2', '3', '1', '4' };
static const char TT550mode_type[] = { 'U', 'U', 'L', 'L', 'U', 'U' };

static const char *TT550_widths[] = {
"300",  "330",  "375",  "450",  "525",   "600",  "675",  "750",  "900", "1050",
"1200", "1350", "1500", "1650", "1800", "1950", "2100", "2250", "2400", "2550",
"2700", "2850", "3000", "3300", "3600", "3900", "4200", "4500", "4800", "5100",
"5400", "5700", "6000", "8000", NULL};
static int TT550_bw_vals[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32,33,34,
WVALS_LIMIT};

static const int TT550_filter_nbr[] = {
32, 31, 30, 29, 28, 27, 26, 25, 24, 23,
22, 21, 20, 19, 18, 17, 16, 15, 14, 13,
12, 11, 10,  9,  8,  7,  6,  5,  4,  3,
 2,  1,  0, 33 };

static const int TT550_filter_width[] = {
 300,  330,  375,  450,  525,  600,  675,  750,  900, 1050,
1200, 1350, 1500, 1650, 1800, 1950, 2100, 2250, 2400, 2550,
2700, 2850, 3000, 3300, 3600, 3900, 4200, 4500, 4800, 5100,
5400, 5700, 6000, 8000 };

const char *TT550_xmt_widths[] = {
"900", "1050",  "1200", "1350", "1500", "1650", "1800", "1950", "2100", "2250",
"2400", "2550", "2700", "2850", "3000", "3300", "3600", "3900", NULL};

static const int TT550_xmt_filter_nbr[] = {
24, 23, 22, 21, 20, 19, 18, 17, 16,
15, 14, 13, 12, 11, 10,  9,  8,  7};

static const int TT550_xmt_filter_width[] = {
 900, 1050, 1200, 1350, 1500, 1650, 1800, 1950, 2100,
2250, 2400, 2550, 2700, 2850, 3000, 3300, 3600, 3900 };

static const int TT550_steps[] = { 1, 10, 100, 1000, 10000 };

static char TT550restart[]			= "XX\r";
static char TT550init[]				= "P1\r";
//static char TT550isRADIO[]		= " RADIO START";
//static char TT550isDSP[]			= " DSP START";

//static char TT550setFREQ[]		= "N123456\r";

static char TT550setMODE[]			= "Mnn\r";
static char TT550setRcvBW[]			= "Wx\r";
static char TT550setXmtBW[]			= "Cx\r";
static char TT550setVolume[]		= "Vn\r";
static char TT550setAGC[]			= "Gc\r";
static char TT550setRFGAIN[]		= "An\r";
static char TT550setATT[]			= "Bc\r";
static char TT550setCWWPM[]			= "Eabcdef\r";
static char TT550setMONVOL[]		= "Hn\r";
static char TT550setCWMONVOL[]		= "Jn\r";
static char TT550setNRNOTCH[]		= "Kna\r";
static char TT550setLINEOUT[]		= "Ln\r"; // 63 - min, 0 - max
static char TT550setMICLINE[]		= "O1cn\r"; // *******************************************
static char TT550setPOWER[]			= "Pn\r"; // ****************************************
static char TT550setXMT[]			= "Q1\r";
static char TT550setRCV[]			= "Q0\r";
static char TT550setSQUELCH[]		= "Sn\r";	// 0..19; 6db / unit
static char TT550setVOX[]			= "Uc\r";	// '0' = off; '1' = on
static char TT550setVOXGAIN[]		= "UGn\r";	// 0 <= n <= 255
static char TT550setANTIVOX[]		= "UAn\r";	// 0..255
static char TT550setVOXHANG[]		= "UHn\r";	// 0..255; n= delay*0.0214 sec
static char TT550setCWSPOTLVL[]		= "Fn\r";	// 0..255; 0 = off
static char TT550setCWQSK[]			= "UQn\r";	// 0..255; 0 = none
static char TT550setAUXHANG[]		= "UTn\r";	// 0..255; 0 = none
static char TT550setBLANKER[]		= "Dn\r";	// 0..7; 0 = off
static char TT550setSPEECH[]		= "Yn\r";	// 0..127; 0 = off

static char TT550setDISABLE[]		= "#0\r";	// disable transmitter
static char TT550setENABLE[]		= "#1\r";	// enable transmitter
static char TT550setTLOOP_OFF[]		= "#2\r";	// disable T loop
static char TT550setTLOOP_ON[]		= "#3\r";	// enable T loop
static char TT550setKEYER_OFF[]		= "#6\r";	// enable keyer
static char TT550setKEYER_ON[]		= "#7\r";	// disable keyer
static char TT550setALIVE_OFF[]		= "#8\r";	// disable keep alive
//static char TT550setALIVE_ON[]	= "#9\r";	// enable keep alive

//static char TT550getAGC[]			= "?Y\r";	// 0..255
//static char TT550getFWDPWR[]		= "?F\r";	// F<0..255>
//static char TT550getREFPWR[]		= "?R\r";	// R<0..255>
static char TT550query[]			= "?S\r";	// S<0..255><0..255>
static char TT550getFWDREF[]		= "?S\r";	// T<0..255><0..255>

static char TT550setAMCARRIER[]		= "R \r";	// enables AM mode transmit

static string xcvrstream = "";

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 },
	{ (Fl_Widget *)sldrRFGAIN,   54, 145, 156 },
	{ (Fl_Widget *)btnIFsh,     214, 125,  50 },
	{ (Fl_Widget *)sldrIFSHIFT, 266, 125, 156 },
	{ (Fl_Widget *)sldrMICGAIN, 266, 145, 156 },
	{ (Fl_Widget *)sldrPOWER,    54, 165, 368 },
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

RIG_TT550::RIG_TT550() {
// base class values
	name_ = TT550name_;
	modes_ = TT550modes_;
	bandwidths_ = TT550_widths;
	bw_vals_ = TT550_bw_vals;

	widgets = rig_widgets;

	comm_baudrate = BR57600;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_rtscts = true;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	serloop_timing = 100;

	def_mode = modeA = modeB = 1;
	def_bw = bwA = bwB = 20;
	def_freq = freqA = freqB = 14070000;
	max_power = 100;
	can_change_alt_vfo = true;

	VfoAdj = 0;
	Bfo = 600;

	ATTlevel = 0;
	RFgain = 100;

	has_notch_control =
	has_preamp_control = false;

	has_extras =
	has_bfo =
	has_smeter =
	has_power_out =
	has_split =
	has_split_AB =
	has_swr_control =
	has_micgain_control =
	has_power_control =
	has_agc_level =
	has_cw_wpm =
	has_cw_vol =
	has_cw_spot =
	has_vox_onoff =
	has_vox_gain =
	has_vox_anti =
	has_vox_hang =
	has_compression =
	has_rit =
	has_xit =
	has_rf_control =
	has_attenuator_control =
	has_volume_control =
	has_ifshift_control =
	has_ptt_control =
	has_bandwidth_control =
	has_auto_notch =
	has_tune_control =
	has_noise_control =
	has_mode_control =
	has_vfo_adj = true;

	auto_notch = noise_reduction = false;

	precision = 1;
	ndigits = 8;

}

static std::string ctlvals[] = {
"x00", "x01", "x02", "x03", "x04", "x05", "x06", "x07",
"x08", "x09", "x0a", "x0b", "x0c", "x0d", "x0e", "x0f"};

static std::string noctl(std::string cmd)
{
	stringstream s;
	unsigned int c;
	s << cmd[0];
	for (size_t n = 1; n < cmd.length(); n++) {
		c = cmd[n] & 0xFF;
		if (c < 0x10) s << " " << ctlvals[c];
		else s << " x" << hex << c;
	}
	return s.str();
}

static const char* info(string s)
{
	static string infostr;
	infostr.assign(s);
	if (infostr[infostr.length()-1] == '\r')
		infostr.replace(infostr.length()-1, 1, "<0d>");
	infostr.append("  ");
	infostr.append(str2hex(s.c_str(), s.length()));
	return infostr.c_str();
}

void RIG_TT550::showASCII(string s1, string s)
{
	while (s[0] == ' ' || s[0] == '\r' || s[0] == '\n') s.erase(0,1);
	for (size_t i = 0; i < s.length(); i++) {
		if (s[i] == '\r' || s[i] == '\n') s[i] = ' ';
	}
	LOG_INFO("%9s : %s", s1.c_str(), s.c_str());
}

void RIG_TT550::initialize()
{
	progStatus.settrace = true;

	rig_widgets[0].W = btnVol;
	rig_widgets[1].W = sldrVOLUME;
	rig_widgets[2].W = sldrRFGAIN;
	rig_widgets[3].W = btnIFsh;
	rig_widgets[4].W = sldrIFSHIFT;
	rig_widgets[5].W = sldrMICGAIN;
	rig_widgets[6].W = sldrPOWER;

	sendCommand(TT550restart, 14);

	if (replystr.find("RADIO") == string::npos) {
		showASCII("Power up", "DSP START");
		sendCommand(TT550init); // put into radio mode
	}
	showASCII("Init", replystr);

	sendCommand("?V\r", 13);
	showASCII("Version", replystr);

	sendCommand(TT550setALIVE_OFF, 0);

	set_volume_control(0);

	set_auto_notch(auto_notch);
	set_compression(0,0);

	set_vox_hang();
	set_vox_anti();
	set_vox_gain();
	set_vox_onoff();

	set_cw_spot();
	set_cw_vol();
	set_cw_wpm();
	set_cw_qsk();
	enable_keyer();

	set_agc_level();
	set_line_out();
//	use_line_in = progStatus.use_line_in;
	set_mic_gain(progStatus.mic_gain);
	set_mic_line(0);
	set_rf_gain(RFgain);

	XitFreq = progStatus.xit_freq;
	RitFreq = progStatus.rit_freq;

	split = false;

	Bfo = progStatus.bfo_freq;
//	set_vfoA(freqA);

	VfoAdj = progStatus.vfo_adj;

//	setXit(XitFreq);
//	setRit(RitFreq);
//	setBfo(Bfo);

	set_attenuator(0);
	set_mon_vol();
	set_squelch_level();
//	set_if_shift(pbt);
	set_aux_hang();

	set_volume_control(progStatus.volume);

	cmd = TT550setAMCARRIER;
	cmd[1] = 0x0F;
	sendCommand(cmd, 0);

	enable_tloop();
	enable_xmtr();

	xcvrstream.clear();
	keypad_timeout = 0;

	encoder_count = 0;

}

void RIG_TT550::enable_xmtr()
{
	if (progStatus.tt550_enable_xmtr)
		cmd = TT550setENABLE;
	else
		cmd = TT550setDISABLE;
	sendCommand(cmd, 0);
	set_trace(2, "enable_xmtr()", noctl(cmd).c_str());
}

void RIG_TT550::enable_tloop()
{
	if (progStatus.tt550_enable_tloop)
		cmd = TT550setTLOOP_ON;
	else
		cmd = TT550setTLOOP_OFF;
	sendCommand(cmd, 0);
	set_trace(2, "enable_tloop()", noctl(cmd).c_str());
}

void RIG_TT550::shutdown()
{
	cmd = "Vx\r";
	cmd[1] = 0;
	sendCommand(cmd, 0); // volume = zero
	cmd = "Lx\r";
	cmd[1] = 0x3F;
	sendCommand(cmd, 0); // line out = minimum
	set_trace(2, "shutdown()", noctl(cmd).c_str());
}

int DigiAdj = 0;

void RIG_TT550::set_vfoRX(long freq)
{
	int NVal = 0, FVal = 0;	// N value / finetune value
    int TBfo = 0;			// temporary BFO (Hz)
	int IBfo = 0;			// Intermediate BFO Freq (Hz)

	int PbtAdj = PbtActive ? pbt : 0;//PbtFreq : 0;	// passband adj (Hz)
	int	RitAdj = RitActive ? RitFreq : 0;	// RIT adj (Hz)

	int FiltAdj = (TT550_filter_width[def_bw])/2;		// filter bw (Hz)

	long lFreq = freq * (1 + VfoAdj * 1e-6);

	lFreq += RitAdj;

	if(def_mode == TT550_DIGI_MODE) {
		DigiAdj = 1500 - FiltAdj - 200;
		DigiAdj = DigiAdj < 0 ? 0 : DigiAdj;
		IBfo = FiltAdj + 200;
		lFreq += (IBfo + PbtAdj + DigiAdj);
		IBfo = IBfo + PbtAdj + DigiAdj;
	}

	if(def_mode == TT550_USB_MODE) {
		IBfo = FiltAdj + 200;
		lFreq += (IBfo + PbtAdj);
		IBfo = IBfo + PbtAdj;
	}

	if(def_mode == TT550_LSB_MODE) {
		IBfo = FiltAdj + 200;
		lFreq -= (IBfo + PbtAdj);
		IBfo = IBfo + PbtAdj;
	}

	if(def_mode == TT550_CW_MODE) {
// CW Mode uses LSB Mode
		if (( FiltAdj + 300) <= Bfo) {
			IBfo = PbtAdj + Bfo;
		} else {
		 	IBfo = FiltAdj + 300;
			lFreq += (Bfo - IBfo);
			IBfo = IBfo + PbtAdj;
		}
	}

	if(def_mode == TT550_FM_MODE) {
		lFreq += Bfo;
		IBfo = 0;
	}

	lFreq -= 1250;

	NVal = lFreq / 2500 + 18000;
	FVal = (int)((lFreq % 2500) * 5.46);

	cmd = "N";
	cmd += (NVal >> 8) & 0xff;
	cmd += NVal & 0xff;
	cmd += (FVal >> 8) & 0xff;
	cmd += FVal & 0xff;

	TBfo = (int)((IBfo + 8000)*2.73);
	cmd += (TBfo >> 8) & 0xff;
	cmd += TBfo & 0xff;
	cmd += '\r';
	sendCommand(cmd, 0);

	stringstream s;
	s << "Rx freq = " << freq << " / adjusted to " << lFreq << ", " << noctl(cmd);
	set_trace(2, "set vfoRX", s.str().c_str());

}

void RIG_TT550::set_vfoTX(long freq)
{
	int NVal = 0, FVal = 0;	// N value / finetune value
    int TBfo = 0;			// temporary BFO
	int IBfo = 1500;		// Intermediate BFO Freq
	int bwBFO = 0;			// BFO based on selected bandwidth
	int FilterBw = 0;		// Filter Bandwidth determined from table

	int XitAdj;
	long lFreq = freq * (1 + VfoAdj * 1e-6);

	lFreq += XitAdj = XitActive ? XitFreq : 0;

	if (progStatus.tt550_use_xmt_bw)
		FilterBw = TT550_xmt_filter_width[progStatus.tt550_xmt_bw];
	else
		FilterBw = TT550_filter_width[def_bw];
	if (FilterBw < 900) FilterBw = 900;
	if (FilterBw > 3900) FilterBw = 3900;
//	if (def_mode == TT550_DIGI_MODE) FilterBw = 3000;

	bwBFO = (FilterBw/2) + 200;
	IBfo = (bwBFO > IBfo) ?  bwBFO : IBfo ;

	if (def_mode == TT550_USB_MODE || def_mode == TT550_DIGI_MODE) {
		lFreq += IBfo;
		TBfo = (int)(IBfo * 2.73);
	}

	if (def_mode == TT550_LSB_MODE) {
		lFreq -= IBfo;
		TBfo = (int)(IBfo * 2.73);
	}

// CW Mode uses LSB Mode
	if(def_mode == TT550_CW_MODE) {
		IBfo = 1500; // fixed for CW
		lFreq += Bfo - IBfo;
		TBfo = (int)(Bfo * 2.73);
	}

	if(def_mode == TT550_FM_MODE) {
		IBfo = 0;
		lFreq -= IBfo;
		TBfo = 0;
	}

	lFreq -= 1250;
	NVal = lFreq / 2500 + 18000;
	FVal = (int)((lFreq % 2500) * 5.46);

	cmd = "T";
	cmd += (NVal >> 8) & 0xff;
	cmd += NVal & 0xff;
	cmd += (FVal >> 8) & 0xff;
	cmd += FVal & 0xff;
	cmd += (TBfo >> 8) & 0xff;
	cmd += TBfo & 0xff;
	cmd += '\r';
	sendCommand(cmd, 0);

	stringstream s;
	s << "Tx freq = " << freq << " / adjusted to " << lFreq << ", " << noctl(cmd);
	set_trace(2, "set vfoTX", s.str().c_str());

}

void RIG_TT550::set_split(bool val)
{
	split = val;
	if (split) {
		if (!useB)
			set_vfoTX(freqB);
		else
			set_vfoTX(freqA);
	} else {
		if (!useB)
			set_vfoTX(freqA);
		else
			set_vfoTX(freqB);
	}
	set_trace(2, "set split ", (val ? "ON" : "OFF"));
}

void RIG_TT550::set_vfo(long freq)
{
	set_vfoRX(freq);
	if (!split)
		set_vfoTX(freq);
	xcvrstream.clear();
}

void RIG_TT550::set_vfoA (long freq)
{
	freqA = freq;
	if (!useB)
		set_vfo(freq);
}

int enc_change = 0;
void RIG_TT550::process_encoder(int val)
{
}

bool RIG_TT550::check()
{
	return true;
}

long RIG_TT550::get_vfoA ()
{
	if (!useB) {
		freqA += enc_change;
		enc_change = 0;
	}
	return freqA;
}

void RIG_TT550::set_vfoB (long freq)
{
	freqB = freq;
	if (useB)
		set_vfo(freqB);
}

long RIG_TT550::get_vfoB ()
{
	if (useB) {
		freqB += enc_change;
		enc_change = 0;
	}
	return freqB;
}

// Tranceiver PTT on/off
static unsigned int fp[FPLEN];
static unsigned int rp[FPLEN];
static int iswr;

void RIG_TT550::set_PTT_control(int val)
{
	ptt_ = val;
	if (val) {
		for (int i = 0; i < FPLEN; i++)
			fp[i] = rp[i] = 0;
		iswr = 0;
	}
	if (val) cmd = TT550setXMT;
	else     cmd = TT550setRCV;
	sendCommand(cmd, 0);

	stringstream s;
	s << "set PTT " << (val ? "ON " : "OFF ") << noctl(cmd);

	set_trace(1, s.str().c_str());

}

int RIG_TT550::get_PTT()
{
	return ptt_;
}

void RIG_TT550::set_mode(int val)
{
	def_mode = val;
	if (val == TT550_AM_MODE) {

		cmd = TT550setMODE;
		cmd[1] = cmd[2] = TT550mode_chr[val];
		sendCommand(cmd, 0);

		stringstream s;
		s << "Set Mode " << noctl(cmd);
		set_trace(1, s.str().c_str());

		cmd =  TT550setPOWER;
		cmd[1] = 0xFF;
		sendCommand(cmd, 0);

		set_power_control(progStatus.tt550_AM_level);

	} else {

		cmd = TT550setMODE;
		cmd[1] = cmd[2] = TT550mode_chr[val];
		sendCommand(cmd, 0);

		stringstream s;
		s << "Set Mode " << noctl(cmd);
		set_trace(1, s.str().c_str());

		set_power_control(progStatus.power_level);
	}
	set_bw(def_bw);
}

void RIG_TT550::set_modeA(int val)
{
	modeA = val;
	set_mode(val);
}

void RIG_TT550::set_modeB(int val)
{
	modeB = val;
	if (useB)
		set_mode(val);
}

int  RIG_TT550::get_modeB()
{
	return modeB;
}

static int ret_mode = TT550_CW_MODE;

static void tt550_tune_off(void *)
{
	selrig->set_PTT_control(0);
	selrig->set_power_control(progStatus.power_level);
	if (useB)
		selrig->set_modeB(ret_mode);
	else
		selrig->set_modeA(ret_mode);
}

void RIG_TT550::tune_rig(int val)
{
	set_PTT_control(0);
	if (!useB) {
		ret_mode = modeA;
		set_modeA(TT550_CW_MODE);
	}
	else {
		ret_mode = modeB;
		set_modeB(TT550_CW_MODE);
	}
	set_power_control(10);

	set_PTT_control(1);
	set_trace(1, "tune rig");

	Fl::add_timeout(5.0, tt550_tune_off);
}

int RIG_TT550::get_modeA()
{
	return modeA;
}

int RIG_TT550::get_modetype(int n)
{
	return TT550mode_type[n];
}

void RIG_TT550::set_bw(int val)
{
	def_bw = val;
	int rxbw = TT550_filter_nbr[val];
	int txbw = rxbw;
	if (progStatus.tt550_use_xmt_bw)
		txbw = TT550_xmt_filter_nbr[progStatus.tt550_xmt_bw];
	if (txbw < 7) txbw = 7;
	if (txbw > 24) txbw = 24;
	cmd = TT550setRcvBW;
	cmd[1] = rxbw;
	sendCommand(cmd, 0);

	stringstream s;
	s << "Set RX bandwidth " << val << " " << noctl(cmd);
	set_trace(1, s.str().c_str());

	cmd = TT550setXmtBW;
	cmd[1] = txbw;
	sendCommand(cmd, 0);

	stringstream s2;
	s2 << "Set TX bandwidth " << val << " " << noctl(cmd);
	set_trace(1, s2.str().c_str());

	set_vfo(!useB ? freqA : freqB);
}

void RIG_TT550::set_bwA(int val)
{
	bwA = val;
	set_bw(bwA);
}

int RIG_TT550::get_bwA()
{
	return bwA;
}

void RIG_TT550::set_bwB(int val)
{
	bwB = val;
	if (useB)
		set_bw(val);
}

int  RIG_TT550::get_bwB()
{
	return bwB;
}

int RIG_TT550::adjust_bandwidth(int md)
{
	return bwA;
}

int RIG_TT550::def_bandwidth(int m)
{
	if (m < 0) m = 0;
	if (m > 4) m = 4;
	return TT550_def_bw[m];
}

void RIG_TT550::set_if_shift(int val)
{
	pbt = val;
	if (pbt) PbtActive = true;
	set_vfoRX(!useB ? freqA : freqB);
}

bool RIG_TT550::get_if_shift(int &val)
{
	val = pbt;
	if (!val) return false;
	return true;
}

void RIG_TT550::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -500;
	max = 500;
	step = 50;
}

void RIG_TT550::set_attenuator(int val)
{
	progStatus.attenuator = val;
	cmd = TT550setATT;
	if (val) cmd[1] = '1';
	else     cmd[1] = '0';
	sendCommand(cmd, 0);

	stringstream s;
	s << "Set attenuator " << noctl(cmd);
	set_trace(1, s.str().c_str());

}

int RIG_TT550::get_attenuator()
{
	return progStatus.attenuator;

}

void RIG_TT550::set_volume_control(int val)
{
	cmd = TT550setVolume;
	cmd[1] = 0xFF & ((val * 255) / 100);
	sendCommand(cmd, 0);

	stringstream s;
	s << "Set volume " << noctl(cmd);
	set_trace(1, s.str().c_str());
}

int RIG_TT550::get_volume_control()
{
	return progStatus.volume;
}

static void show_encA(void *)
{
	txt_encA->show();
}
static void hide_encA(void *)
{
	txt_encA->hide();
}
static void update_encA(void *d)
{
	char *str = (char *)d;
	txt_encA->value(str);
}

void RIG_TT550::selectA()
{
	Fl::awake(hide_encA, NULL);
	xcvrstream.clear();
	set_trace(1, "Select A");

	freqA = vfoA.freq;
	set_modeA (vfoA.imode);
	set_bwA (vfoA.iBW);
	set_vfoRX(freqA);
	set_vfoTX(freqA);
}

void RIG_TT550::selectB()
{
	Fl::awake(hide_encA, NULL);
	xcvrstream.clear();
	set_trace(1, "Select B");
	freqB = vfoB.freq;
	set_modeB (vfoB.imode);
	set_bwB (vfoB.iBW);
	set_vfoRX(freqB);
	set_vfoTX(freqB);
}

void RIG_TT550::process_freq_entry(char c)
{
	static bool have_decimal = false;
	float ffreq = 0.0;
	long freq = 0;
	if (xcvrstream.empty()) have_decimal = false;
	if (c != '\r') {
		if ((c >= '0' && c <= '9') || c == '.') {
			xcvrstream += c;
			if (!have_decimal && c == '.') have_decimal = true;
			else if (have_decimal && c == '.') {
				xcvrstream.clear();
				have_decimal = false;
				keypad_timeout = 0;
				Fl::awake(hide_encA, NULL);
				return;
			}
			ffreq = 0;
			sscanf(xcvrstream.c_str(), "%f", &ffreq);
			if (have_decimal) ffreq *= 1000;
			freq = (long) ffreq;
			if (!txt_encA->visible())
				Fl::awake(show_encA, NULL);
			Fl::awake(update_encA, (void*)xcvrstream.c_str());
//			LOG_INFO("%s => %ld", str2hex(xcvrstream.c_str(), xcvrstream.length()), freq);
			keypad_timeout = progStatus.tt550_keypad_timeout / progStatus.serloop_timing;
		}
	} else {
		keypad_timeout = 0;
		if (xcvrstream.empty()) return;
		ffreq = 0;
		sscanf(xcvrstream.c_str(), "%f", &ffreq);
		if (have_decimal) ffreq *= 1000;
		freq = (long) ffreq;
		if (freq < 50000) freq *= 1000;
		Fl::awake(hide_encA, NULL);
		if (!useB) {
			freqA = freq;
		} else {
			freqB = freq;
		}
		xcvrstream.clear();
		have_decimal = false;
	}
}

//static const char *tt550_fkey_strings[] = {"None","Clear","CW++","CW--","Band++","Band--","Step++","Step--"};

void RIG_TT550::fkey_clear()
{
//	LOG_INFO("%s", tt550_fkey_strings[1]);
	xcvrstream.clear();
	keypad_timeout = 0;
	Fl::awake(hide_encA, NULL);
}

void RIG_TT550::fkey_cw_plus()
{
//	LOG_INFO("%s", tt550_fkey_strings[2]);
	if (progStatus.tt550_cw_wpm >= 80) return;
	progStatus.tt550_cw_wpm++;
	spnr_tt550_cw_wpm->value(progStatus.tt550_cw_wpm);
	spnr_tt550_cw_wpm->redraw();
	selrig->set_cw_wpm();
}

void RIG_TT550::fkey_cw_minus()
{
	if (progStatus.tt550_cw_wpm <= 5) return;
	progStatus.tt550_cw_wpm--;
	spnr_tt550_cw_wpm->value(progStatus.tt550_cw_wpm);
	spnr_tt550_cw_wpm->redraw();
	selrig->set_cw_wpm();
}

struct BANDS { int lo; int hi; int digi; };

static BANDS ibands[] = {
{ 0, 1800000, 28120000 },
{ 1800000, 2000000, 1807000 },
{ 3500000, 4000000, 3580000 },
{ 7000000, 7300000, 7035000 },
{ 10100000, 10150000, 10140000 },
{ 14000000, 14350000, 14070000 },
{ 18068000, 18168000, 18100000 },
{ 21000000, 21450000, 21070000 },
{ 24890000, 24990000, 24920000 },
{ 28000000, 29700000, 28120000 },
{ 29700000, 0, 1807000 }
};

void RIG_TT550::fkey_band_plus()
{
	VFOQUEUE qvfo;
	if (useB) qvfo.vfo = vfoB;
	else      qvfo.vfo = vfoA;
	for (size_t i = 1; i < sizeof(ibands) / sizeof(BANDS); i++) {
		if (qvfo.vfo.freq < ibands[i].lo) {
			qvfo.vfo.freq = ibands[i].digi;
			break;
		}
	}
	qvfo.vfo.src = UI;
	qvfo.vfo.iBW = 255;
	qvfo.vfo.imode = -1;
	if (useB) qvfo.change = vB;
	else      qvfo.change = vA;
	srvc_reqs.push(qvfo);
}

void RIG_TT550::fkey_band_minus()
{
	VFOQUEUE qvfo;
	if (useB) qvfo.vfo = vfoB;
	else      qvfo.vfo = vfoA;
	for (size_t i = sizeof(ibands) / sizeof(BANDS) - 2; i >= 0; i--) {
		if (qvfo.vfo.freq > ibands[i].hi) {
			qvfo.vfo.freq = ibands[i].digi;
			break;
		}
	}
	qvfo.vfo.src = UI;
	qvfo.vfo.iBW = 255;
	qvfo.vfo.imode = -1;
	if (useB) qvfo.change = vB;
	else      qvfo.change = vA;
	srvc_reqs.push(qvfo);
}

void RIG_TT550::fkey_step_plus()
{
	progStatus.tt550_encoder_step++;
	if (progStatus.tt550_encoder_step > 4) progStatus.tt550_encoder_step = 0;
	sel_tt550_encoder_step->value(progStatus.tt550_encoder_step);
	sel_tt550_encoder_step->redraw();
}

void RIG_TT550::fkey_step_minus()
{
	progStatus.tt550_encoder_step--;
	if (progStatus.tt550_encoder_step < 0) progStatus.tt550_encoder_step = 4;
	sel_tt550_encoder_step->value(progStatus.tt550_encoder_step);
	sel_tt550_encoder_step->redraw();
}

void RIG_TT550::process_fkey(char c)
{
	if (c == 0x11)
		switch (progStatus.tt550_F1_func) {
			case 1 : fkey_clear(); break;
			case 2 : fkey_cw_plus(); break;
			case 3 : fkey_cw_minus(); break;
			case 4 : fkey_band_plus(); break;
			case 5 : fkey_band_minus(); break;
			case 6 : fkey_step_plus(); break;
			case 7 : fkey_step_minus(); break;
			default: ;
		}
	if (c == 0x12)
		switch (progStatus.tt550_F2_func) {
			case 1 : fkey_clear(); break;
			case 2 : fkey_cw_plus(); break;
			case 3 : fkey_cw_minus(); break;
			case 4 : fkey_band_plus(); break;
			case 5 : fkey_band_minus(); break;
			case 6 : fkey_step_plus(); break;
			case 7 : fkey_step_minus(); break;
			default: ;
		}
	if (c == 0x13)
		switch (progStatus.tt550_F3_func) {
			case 1 : fkey_clear(); break;
			case 2 : fkey_cw_plus(); break;
			case 3 : fkey_cw_minus(); break;
			case 4 : fkey_band_plus(); break;
			case 5 : fkey_band_minus(); break;
			case 6 : fkey_step_plus(); break;
			case 7 : fkey_step_minus(); break;
			default: ;
		}
}

void RIG_TT550::process_keypad(char c)
{
	if (c < 0 || c > 0x7f) return;
	if (c == 0x11 || c == 0x12 || c == 0x13)
		process_fkey(c);
	else
		process_freq_entry(c);
}

void RIG_TT550::get_302()
{
	if (keypad_timeout) {
		keypad_timeout--;
		if (keypad_timeout == 0) {
			xcvrstream.clear();
			Fl::awake(hide_encA, NULL);
		}
	}
// reading any pending encoder / keyboard strings
	size_t p = 0;
	int encval = 0;
	int encode = 0;
	size_t len;

	if (!readResponse()) return;

	pending.append(replystr);
	len = replystr.length();

	while (p < len) {
		switch (replystr[p]) {
			case 'U' :
				if (len - p < 3) {
					pending.erase(0,p);
					break;
				}
				process_keypad(replystr[p+1]);
				p += 3;
				break;
			case '!' :
				if (len - p < 5) {
					pending.erase(0,p);
					break;
				}
				encval = ((unsigned char)replystr[p+1] << 8) | (unsigned char)replystr[p+2];
				if (encval > 16383) encval -= 65536;
				encode += encval;
				p += 5;
				break;
			default :
				p++;
				break;
		}
	}
	if (encode)
		enc_change = encode * TT550_steps[progStatus.tt550_encoder_step];
}

int RIG_TT550::get_smeter()
{
	int sval = 0;
	float fval;
	int fp;
	size_t p;
	size_t len;

	get_302();

	p = 0;
	sendCommand( TT550query, 0);
	get_trace(1, "get smeter");
	len = readResponse();

	while ((p < len) && (replystr[p] != 'S') && (replystr[p] != 'T'))
		p++;
	if (p) {
		pending.append(replystr.substr(0,p));
		replystr.erase(0,p);
	}

	len = replystr.length();

	if (replystr[0] == 'S' && len > 5) {
		sscanf(&replystr[1], "%4x", &sval);
		fval = sval/256.0;
		sval = (int)(fval * 100.0 / 18.0);
		if (sval > 100) sval = 0;
		Fl::awake(updateFwdPwr, (void*)0);
	}

	else if (replystr[0] == 'T' && len > 3) {
		fp = (unsigned char)replystr[1];
		Fl::awake(updateFwdPwr, reinterpret_cast<void*>(fp));
	}

	return sval;
}

int RIG_TT550::get_swr()
{
	return iswr;
}

int RIG_TT550::get_power_out()
{
	cmd = TT550getFWDREF;
	int ret = sendCommand(cmd, 4);
	get_trace(1, "get power out");

	if (ret < 4) return fwdpwr;
	size_t p = replystr.rfind("T");
	if (p == string::npos) return fwdpwr;

    for (int i = 0; i < FPLEN - 1; i++) {
		fp[i] = fp[i+1];
		rp[i] = rp[i+1];
	}
	fp[FPLEN-1] = (unsigned int)replystr[p+1];
	rp[FPLEN-1] = (unsigned int)replystr[p+2];
	fwdpwr = refpwr = 0;
	for (int i = FPLEN - progStatus.tt550_Nsamples; i < FPLEN; i++) {
		if (fp[i] > fwdpwr) {
			fwdpwr = fp[i];
			refpwr = rp[i];
		}
	}

	double swr = 1.0;
	if (fwdpwr < 5) iswr = 0;
	else if (fwdpwr == refpwr) iswr = 100;
	else {
		swr = (fwdpwr + refpwr) / (fwdpwr - refpwr);
		iswr =  (swr-1.0) * 25;
		if (iswr < 0) iswr = 0;
		if (iswr > 100) iswr = 100;
	}

	stringstream s;
	s << "Get pwr: fwc " << fwdpwr << ", refpwr" << refpwr << ", swr " << swr;
	set_trace(1, s.str().c_str());

	return fwdpwr;
}

void RIG_TT550::setBfo(int val)
{
	progStatus.bfo_freq = Bfo = val;
	if (useB) {
		set_vfoRX(freqB);
		set_vfoTX(freqB);
	} else if (split) {
		set_vfoRX(freqA);
		set_vfoTX(freqB);
	} else {
		set_vfoRX(freqA);
		set_vfoTX(freqA);
	}
}

int RIG_TT550::getBfo()
{
	return Bfo;
}

void RIG_TT550::setVfoAdj(double v)
{
	VfoAdj = v;
	set_vfoRX(!useB ? freqA : freqB);
}

void RIG_TT550::setRit(int val)
{
	progStatus.rit_freq = RitFreq = val;
	if (RitFreq) RitActive = true;
	if (useB) {
		set_vfoRX(freqB);
		set_vfoTX(freqB);
	} else if (split) {
		set_vfoRX(freqA);
		set_vfoTX(freqB);
	} else {
		set_vfoRX(freqA);
		set_vfoTX(freqA);
	}
}

int RIG_TT550::getRit()
{
	return RitFreq;
}

void RIG_TT550::setXit(int val)
{
	progStatus.xit_freq = XitFreq = val;
	if (XitFreq) XitActive = true;
	if (useB) {
		set_vfoRX(freqB);
		set_vfoTX(freqB);
	} else if (split) {
		set_vfoRX(freqA);
		set_vfoTX(freqB);
	} else {
		set_vfoRX(freqA);
		set_vfoTX(freqA);
	}
}

int RIG_TT550::getXit()
{
	return XitFreq;
}

void RIG_TT550::set_rf_gain(int val)
{
	cmd = TT550setRFGAIN;
	cmd[1] = (unsigned char)(255 - val * 2.55);
	if (cmd[1] == 0x0D) cmd[1] = 0x0E;
	RFgain = val;
	sendCommand(cmd, 0);

	stringstream s;
	s << "Set rf gain " << val << ", " << noctl(cmd);
	set_trace(1, s.str().c_str());

}

int  RIG_TT550::get_rf_gain()
{
	return RFgain;
}

void RIG_TT550::get_rf_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void RIG_TT550::set_line_out()
{
	cmd = TT550setLINEOUT;
	cmd[1] = (0x3F) & (((100 - progStatus.tt550_line_out) * 63) / 100);
	if (cmd[1] == 0x0D) cmd[1] = 0x0E;
	sendCommand(cmd, 0);

	stringstream s;
	s << "Set line out " << progStatus.tt550_line_out << ", " << noctl(cmd);
	set_trace(1, s.str().c_str());
}

void RIG_TT550::set_agc_level()
{
	cmd = TT550setAGC;
	switch (progStatus.tt550_agc_level) {
		case 0 : cmd[1] = '1'; break;
		case 1 : cmd[1] = '2'; break;
		case 2 : cmd[1] = '3'; break;
	}
	sendCommand(cmd, 0);

	stringstream s;
	s << "Set agc level " << progStatus.tt550_agc_level << ", " << noctl(cmd);
	set_trace(1, s.str().c_str());
}

void RIG_TT550::set_cw_wpm()
{
	cmd = TT550setCWWPM;
	int duration = 7200 / progStatus.tt550_cw_wpm;
	int ditfactor = duration * progStatus.tt550_cw_weight;
	int spcfactor = duration * (2.0 - progStatus.tt550_cw_weight);
	int dahfactor = duration * 3;
	cmd[1] = 0xFF & (ditfactor >> 8);
	cmd[2] = 0xFF & ditfactor;
	cmd[3] = 0xFF & (dahfactor >> 8);
	cmd[4] = 0xFF & dahfactor;
	cmd[5] = 0xFF & (spcfactor >> 8);
	cmd[6] = 0xFF & spcfactor;
	sendCommand(cmd, 0);

	stringstream s;
	s << "Set CW wpm " << progStatus.tt550_cw_wpm << ", " << noctl(cmd);
	set_trace(1, s.str().c_str());
}

void RIG_TT550::set_cw_vol()
{
	int val = progStatus.tt550_cw_vol;
	cmd = TT550setCWMONVOL;
	cmd[1] = 0xFF & (val * 255) / 100;
	if (cmd[1] == 0x0D) cmd[1] = 0x0E;
	sendCommand(cmd, 0);

	stringstream s;
	s << "Set CW volume " << progStatus.tt550_cw_vol << ", " << noctl(cmd);
	set_trace(1, s.str().c_str());
}

bool RIG_TT550::set_cw_spot()
{
	int val = progStatus.tt550_cw_spot;
	cmd = TT550setCWSPOTLVL;
	cmd[1] = 0xFF & ((val * 255 ) / 100);
	if (!progStatus.tt550_spot_onoff) cmd[1] = 0;
	if (cmd[1] == 0x0D) cmd[1] = 0x0E;
	sendCommand(cmd, 0);

	stringstream s;
	s << "Set CW spot " << progStatus.tt550_cw_spot << ", " << noctl(cmd);
	set_trace(1, s.str().c_str());

	return true;
}

void RIG_TT550::set_spot_onoff()
{
	set_cw_spot();
}

// front panel Preamp control is hijacked for a spot control !

void RIG_TT550::set_preamp(int val)
{
	progStatus.tt550_spot_onoff = val;
	set_cw_spot();
}

int RIG_TT550::get_preamp()
{
	return progStatus.preamp;
}

void RIG_TT550::set_cw_weight()
{
	set_cw_wpm();
}

void RIG_TT550::set_cw_qsk()
{
	cmd = TT550setCWQSK;
	cmd[2] = (0xFF) & (int)(progStatus.tt550_cw_qsk * 2);
	if (cmd[2] == 0x0D) cmd[2] = 0x0E;
	sendCommand(cmd, 0);
}


void RIG_TT550::enable_keyer()
{
	if (progStatus.tt550_enable_keyer)
		cmd = TT550setKEYER_ON;
	else
		cmd = TT550setKEYER_OFF;
	sendCommand(cmd, 0);

	stringstream s;
	s << "Set enable keyer " << (progStatus.tt550_enable_keyer ? "ON" : "OFF") << ", " << noctl(cmd);
	set_trace(1, s.str().c_str());
}


void RIG_TT550::set_vox_onoff()
{
	cmd = TT550setVOX;
	cmd[1] = progStatus.vox_onoff ? '1' : '0';
	sendCommand(cmd, 0);

	stringstream s;
	s << "Set vox " << (progStatus.vox_onoff ? "ON" : "OFF") << ", " << noctl(cmd);
	set_trace(1, s.str().c_str());
}

void RIG_TT550::set_vox_gain()
{
	cmd = TT550setVOXGAIN;
	cmd[2] = (0xFF) & (int)(progStatus.tt550_vox_gain * 2.55);
	if (cmd[2] == 0x0D) cmd[2] = 0x0E;
	sendCommand(cmd, 0);

	stringstream s;
	s << "Set vox gain " << progStatus.tt550_vox_gain << ", " << noctl(cmd);
	set_trace(1, s.str().c_str());
}

void RIG_TT550::set_vox_anti()
{
	cmd = TT550setANTIVOX;
	cmd[2] = (0xFF) & (int)(progStatus.tt550_vox_anti * 2.55);
	if (cmd[2] == 0x0D) cmd[2] = 0x0E;
	sendCommand(cmd, 0);

	stringstream s;
	s << "Set vox anti " << progStatus.tt550_vox_anti << ", " << noctl(cmd);
	set_trace(1, s.str().c_str());
}

void RIG_TT550::set_vox_hang()
{
	cmd = TT550setVOXHANG;
	cmd[2] = (0xFF) & (int)(progStatus.tt550_vox_hang * 2.55);
	if (cmd[2] == 0x0D) cmd[2] = 0x0E;
	sendCommand(cmd, 0);

	stringstream s;
	s << "Set vox hang " << progStatus.tt550_vox_hang << ", " << noctl(cmd);
	set_trace(1, s.str().c_str());
}

void RIG_TT550::set_aux_hang()
{
	cmd = TT550setAUXHANG;
	cmd[2] = 0;
	sendCommand(cmd, 0);

	stringstream s;
	s << "Set aux hang " << noctl(cmd);
	set_trace(1, s.str().c_str());
}

void RIG_TT550::set_compression(int on, int val)
{
	cmd = TT550setSPEECH;
	cmd[1] = (0x7F) & (int)(progStatus.tt550_compression * 1.27);
	if (cmd[1] == 0x0D) cmd[1] = 0x0E;
	sendCommand(cmd, 0);

	stringstream s;
	s << "Set compression " << progStatus.tt550_compression << ", " << noctl(cmd);
	set_trace(1, s.str().c_str());
}

void RIG_TT550::set_auto_notch(int v)
{
	auto_notch = v;
	cmd = TT550setNRNOTCH;
		cmd[1] = '0';
		cmd[1] = noise_reduction ? '1' : '0';
	if (v)
		cmd[2] = '1';
	else
		cmd[2] = '0';
	sendCommand(cmd, 0);

	stringstream s;
	s << "Set auto notch " << noctl(cmd);
	set_trace(1, s.str().c_str());
}

void RIG_TT550::set_noise_reduction(int b)
{
	noise_reduction = b;
	cmd = TT550setNRNOTCH;
	if (b)
		cmd[1] = '1';
	else
		cmd[1] = '0';
	cmd[2] = auto_notch ? '1' : '0';
	sendCommand(cmd, 0);

	stringstream s;
	s << "Set noise reduction " << noctl(cmd);
	set_trace(1, s.str().c_str());
}

void RIG_TT550::set_mic_gain(int v)
{
	progStatus.mic_gain = v;
	if (!progStatus.tt550_use_line_in) {
		cmd = TT550setMICLINE;
		cmd[2] = 0;
		cmd[3] = (unsigned char) v;
		sendCommand(cmd, 0);

		stringstream s;
		s << "Set mic gain " << noctl(cmd);
		set_trace(1, s.str().c_str());

	}
}

int RIG_TT550::get_mic_gain()
{
	return progStatus.mic_gain;
}

void RIG_TT550::set_mic_line(int v)
{
	if (progStatus.tt550_use_line_in) {
		cmd = TT550setMICLINE;
		cmd[2] = 1;
		cmd[3] = 0;//(unsigned char) v;
		sendCommand(cmd, 0);

		stringstream s;
		s << "Set mic line " << noctl(cmd);
		set_trace(1, s.str().c_str());
	}
}

void RIG_TT550::get_mic_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 15;
	step = 1;
}

void RIG_TT550::set_power_control(double val)
{
	int ival = 0;
	if (def_mode == TT550_AM_MODE) {
		progStatus.tt550_AM_level = (int)val;
		cmd = TT550setAMCARRIER;
		ival = (val * .25 - 5) * 256 / 95;
	} else {
		progStatus.power_level = (int) val;
		cmd =  TT550setPOWER;
		ival = (val - 5) * 256 / 95;
	}
	if (ival == 0x0d) ival++;
	if (ival < 5) ival = 5;
	if (ival > 255) ival = 255;
	cmd[1] = ival;
	sendCommand(cmd, 0);

	stringstream s;
	s << "Set power control " << noctl(cmd);
	set_trace(1, s.str().c_str());
}

int RIG_TT550::get_power_control()
{
	return progStatus.power_level;
}

void RIG_TT550::set_mon_vol()
{
	cmd = TT550setMONVOL;
	cmd[1] = 0xFF & ((progStatus.tt550_mon_vol * 255) / 100);
	if (cmd[1] == 0x0D) cmd[1] = 0x0E;
	sendCommand(cmd, 0);

	stringstream s;
	s << "Set mon volume " << progStatus.tt550_mon_vol << ", " << noctl(cmd);
	set_trace(1, s.str().c_str());
}

void RIG_TT550::set_squelch_level()
{
	cmd = TT550setSQUELCH;
	cmd[1] = 0xFF & ((progStatus.tt550_squelch_level * 255) / 100);
	if (cmd[1] == 0x0D) cmd[1] = 0x0E;
	sendCommand(cmd, 0);

	stringstream s;
	s << "Set squelch level " << progStatus.tt550_squelch_level << ", " << noctl(cmd);
	set_trace(1, s.str().c_str());
}

void RIG_TT550::set_nb_level()
{
	cmd = TT550setBLANKER;
	cmd[1] = progStatus.tt550_nb_level;
	sendCommand(cmd, 0);

	stringstream s;
	s << "Set nb level " << progStatus.tt550_nb_level << ", " << noctl(cmd);
	set_trace(1, s.str().c_str());
}

void RIG_TT550::set_noise(bool b)
{
	progStatus.noise = b;
	set_noise_reduction(b);
}

void RIG_TT550::tuner_bypass()
{
}

// callbacks for tt550 transceiver
void cb_tt550_line_out()
{
	pthread_mutex_lock(&mutex_serial);
	selrig->set_line_out();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_tt550_agc_level()
{
	pthread_mutex_lock(&mutex_serial);
	selrig->set_agc_level();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_tt550_cw_wpm()
{
	pthread_mutex_lock(&mutex_serial);
	selrig->set_cw_wpm();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_tt550_cw_vol()
{
	pthread_mutex_lock(&mutex_serial);
	selrig->set_cw_vol();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_tt550_cw_spot()
{
	pthread_mutex_lock(&mutex_serial);
	selrig->set_cw_spot();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_tt550_cw_weight()
{
	pthread_mutex_lock(&mutex_serial);
	selrig->set_cw_weight();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_tt550_enable_keyer()
{
	pthread_mutex_lock(&mutex_serial);
	selrig->enable_keyer();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_tt550_spot_onoff()
{
	pthread_mutex_lock(&mutex_serial);
	selrig->set_spot_onoff();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_tt550_vox_gain()
{
	pthread_mutex_lock(&mutex_serial);
	selrig->set_vox_gain();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_tt550_vox_anti()
{
	pthread_mutex_lock(&mutex_serial);
	selrig->set_vox_anti();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_tt550_vox_hang()
{
	pthread_mutex_lock(&mutex_serial);
	selrig->set_vox_hang();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_tt550_vox_onoff()
{
	pthread_mutex_lock(&mutex_serial);
	selrig->set_vox_onoff();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_tt550_compression()
{
	pthread_mutex_lock(&mutex_serial);
	selrig->set_compression(0,0);
	pthread_mutex_unlock(&mutex_serial);
}

void cb_tt550_mon_vol()
{
	pthread_mutex_lock(&mutex_serial);
	selrig->set_mon_vol();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_tt550_tuner_bypass()
{
	pthread_mutex_lock(&mutex_serial);
	selrig->tuner_bypass();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_tt550_enable_xmtr()
{
	pthread_mutex_lock(&mutex_serial);
	selrig->enable_xmtr();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_tt550_enable_tloop()
{
	pthread_mutex_lock(&mutex_serial);
	selrig->enable_tloop();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_tt550_nb_level()
{
	pthread_mutex_lock(&mutex_serial);
//	progStatus.tt550_nb_level = cbo_tt550_nb_level->index();
	selrig->set_nb_level();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_tt550_use_line_in()
{
	pthread_mutex_lock(&mutex_serial);
	if (progStatus.tt550_use_line_in)
		selrig->set_mic_line(0);
	else
		selrig->set_mic_gain(progStatus.mic_gain);
	pthread_mutex_unlock(&mutex_serial);
}

void cb_tt550_setXmtBW()
{
	pthread_mutex_lock(&mutex_serial);
	selrig->set_bwA(selrig->bwA);
	pthread_mutex_unlock(&mutex_serial);
}

void cb_tt550_cw_qsk()
{
	pthread_mutex_lock(&mutex_serial);
	selrig->set_cw_qsk();
	pthread_mutex_unlock(&mutex_serial);
}

// TT-550 / at-11 internal tuner i/o commands
void RIG_TT550::at11_bypass()
{
	cmd = "$0\r";
	sendCommand(cmd, 0);
	LOG_INFO("%s", info(cmd));
}

void RIG_TT550::at11_autotune()
{
	cmd = "$1\r";
	sendCommand(cmd, 0);
	LOG_INFO("%s", info(cmd));
}

void RIG_TT550::at11_cap_up()
{
	cmd = "$3\r";
	sendCommand(cmd, 0);
	LOG_INFO("%s", info(cmd));
}

void RIG_TT550::at11_cap_dn()
{
	cmd = "$4\r";
	sendCommand(cmd, 0);
	LOG_INFO("%s", info(cmd));
}

void RIG_TT550::at11_ind_up()
{
	cmd = "$5\r";
	sendCommand(cmd, 0);
	LOG_INFO("%s", info(cmd));
}

void RIG_TT550::at11_ind_dn()
{
	cmd = "$6\r";
	sendCommand(cmd, 0);
	LOG_INFO("%s", info(cmd));
}

void RIG_TT550::at11_hiZ()
{
	cmd = "$7\r";
	sendCommand(cmd, 0);
	LOG_INFO("%s", info(cmd));
}

void RIG_TT550::at11_loZ()
{
	cmd = "$8\r";
	sendCommand(cmd, 0);
	LOG_INFO("%s", info(cmd));
}

//======================================================================
// data strings captured from TenTec Windows control program for Pegasus
//======================================================================

/*
       Pegasus Control Program Startup, Query and Close Sequences
       ==========================================================

========================= start program ======================================
WRITE Length 3:
58 58 0D                                  "XX"

READ  Length 2:
0D 0D
READ  Length 14:
20 20 52 41 44 49 4F 20 53 54 41 52 54 0D "  RADIO START"

WRITE Length 3:
3F 56 0D                                  "?V"  version?
READ  Length 13:
56 45 52 20 31 32 39 31 2D 35 33 38 0D    "VER 1291.538"

WRITE Length 7:
4D 31 31 0D                               "M11" mode - USB / USB
50 2B 0D                                  "P+"  power = 16.8 watts

WRITE Length 28:
47 31 0D                                  "G1" agc - slow
4E 51 5C 0A A9 67 70 0D                   "N...." Receive tuning factor
54 51 5C 0A A9 12 20 0D                   "T...." Transmit tuning factor
57 0A 0D                                  "W." Width 3000
56 3E 0D                                  "V." Volume 24
4C 00 0D                                  "L0" Line out - 0, full output

WRITE Length 3:
50 2B 0D                                  "P+" power = 16.8 watts

WRITE Length 3:
4A 29 0D                                  "J." sidetone volume = 16

WRITE Length 13:
4F 31 01 00 0D                            "O1." select line in, gain factor = 1
55 47 0F 0D                               "UG." Vox gain = 15
55 48 0F 0D                               "UH." Vox hang = 15

WRITE Length 16:
55 41 5D 0D                               "UA." Antivox = 36
55 30 0D                                  "U0" Vox OFF
48 00 0D                                  "H." Audio monitor volume = 0
23 32 0D                                  "#2" Disable 'T' loop
23 31 0D                                  "#1" Enable transmitter

WRITE Length 26:
43 0A 0D                                  "C." Transmit filter width = 3000
23 36 0D                                  "#6" Enable keyer
53 00 0D                                  "S." Squelch = 0, OFF
52 0F 0D                                  "R." UNKNOWN
45 01 1F 03 5D 01 1F 0D                   "E...." Keyer timing
44 00 0D                                  "D." Noise blanker = 0, OFF
59 00 0D                                  "Y." Speech processor = 0, OFF

WRITE Length 8:
55 51 00 0D                               "UQ." set CW QSK = 0..255
55 54 00 0D                               "UT." set AUX TX HANG = 0..255 (aux T/R delay)

============================ smeter query ======================================
WRITE Length 3:
3F 53 0D                                  "?S" read smeter
READ	Length 6:
53 30 39 31 42 0D                         "S...." smeter value

============================== close program ====================================

WRITE Length 3:
56 00 0D                                  "V0" volume = ZERO

WRITE Length 3:
4C 3F 0D                                  "L." Line out = 63, MINIMUM


unsigned char datastream[] = {
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,

0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,

0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,
0x21, 0xFF, 0xFF, 0x00, 0x0D,

0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,
0x21, 0x00, 0x01, 0x00, 0x0D,

0x80 };

*/
