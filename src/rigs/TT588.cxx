// ----------------------------------------------------------------------------
// Copyright (C) 2014
//              David Freese, W1HKJ
//              Fernando M. Maresca, 
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
 * Note for anyone wishing to expand on the command set.
 *
 * A rejected command is responded to by a three character sequence "Zy\r".
 * where the y is the letter of the command in error
 * you need to check for that response
 *
*/

#include "TT588.h"
#include "support.h"
#include "math.h"

#include "debug.h"
// fer, debugging purpose
#include <iostream>
#include <typeinfo>

static const char TT588name_[] = "Omni-VII";

//static const char *TT588modes_[] = { "D-USB", "USB", "LSB", "CW", "AM", "FM", NULL}
//static const char TT588mode_chr[] =  { '1', '1', '2', '3', '0', '4' };
//static const char TT588mode_type[] = { 'U', 'U', 'L', 'L', 'U', 'U' };
static const char *TT588modes_[] = {
		"AM", "USB", "LSB", "CWU", "FM", "CWL", "FSK", NULL};
static const char TT588mode_chr[] =  { '0', '1', '2', '3', '4', '5', '6' };
static const char TT588mode_type[] = { 'U', 'U', 'L', 'U', 'U', 'L', 'L' };

// filter # is 37 - index
static const char *TT588_widths[] = {
"200",   "250",  "300",  "350",  "400",  "450",  "500",  "600",  "700",  "800",
"900",  "1000", "1200", "1400", "1600", "1800", "2000", "2200", "2400", "2500",
"2600", "2800", "3000", "3200", "3400", "3600", "3800", "4000", "4500", "5000",
"5500", "6000", "6500", "7000", "7500", "8000", "9000", "12000", NULL};
static int TT588_bw_vals[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,
WVALS_LIMIT};

static const int TT588_numeric_widths[] = {
200,   250,  300,  350,  400,  450,  500,  600,  700,  800,
900,  1000, 1200, 1400, 1600, 1800, 2000, 2200, 2400, 2500,
2600, 2800, 3000, 3200, 3400, 3600, 3800, 4000, 4500, 5000,
5500, 6000, 6500, 7000, 7500, 8000, 9000, 12000, 0};


static char TT588setFREQA[]		= "*Annnn\r";
static char TT588setFREQB[]		= "*Bnnnn\r";
//static char TT588setAGC[]		= "*Gn\r";
static char TT588setSQLCH[]		= "*Hc\r";
static char TT588setRF[]		= "*Ic\r";
static char TT588setATT[]		= "*Jc\r";
static char TT588setNB[]		= "*Knar\r";
static char TT588setMODE[]		= "*Mnn\r";
static char TT588setSPLIT[]		= "*Nn\r";
static char TT588setPBT[]		= "*Pxx\r";
static char TT588setVOL[]		= "*Un\r";
static char TT588setBW[]		= "*Wx\r";

// these C1* and C2* commands are for ethernet only
//static char TT588setPOWER[]	= "*C1Xn\r";
//static char TT588setPREAMP[]	= "*C1Zn\r";
//static char TT588getPOWER[]		= "?C1X\r";
//static char TT588getPREAMP[]	= "?C1Z\r";
// set pout is not available under serial interface
//static char TT588setPOWER[]	= "*C1Xn\r";
// F command gets fwd and ref power  
//static char TT588getFWDPWR[]	= "?F\r";
static char TT588getPOWER[]		= "?F\r";

static char TT588getFREQA[]		= "?A\r";
static char TT588getFREQB[]		= "?B\r";
//static char TT588getAGC[]		= "?G\r";
static char TT588getSQLCH[]		= "?H\r";
static char TT588getRF[]		= "?I\r";
static char TT588getATT[]		= "?J\r";
static char TT588getNB[]		= "?K\r";
static char TT588getMODE[]		= "?M\r";
static char TT588getSPLIT[]		= "?N\r";
static char TT588getPBT[]		= "?P\r";
static char TT588getSMETER[]	= "?S\r";
static char TT588getVOL[]		= "?U\r";
static char TT588getBW[]		= "?W\r";

//static char TT588getFWDPWR[]	= "?F\r";
//static char TT588getREFPWR[]	= "?R\r";

static char TT588setXMT[]		= "*Tnn\r";

static GUI tt588_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 },
	{ (Fl_Widget *)sldrRFGAIN,   54, 145, 156 },
	{ (Fl_Widget *)sldrMICGAIN, 266, 125, 156 },
	{ (Fl_Widget *)sldrSQUELCH, 266, 145, 156 },
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

RIG_TT588::RIG_TT588() {
// base class values
	name_ = TT588name_;
	modes_ = TT588modes_;
	bandwidths_ = TT588_widths;
	bw_vals_ = TT588_bw_vals;

	widgets = tt588_widgets;

	comm_baudrate = BR57600;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 20;
	comm_timeout = 50;
	comm_echo = false;
	comm_rtscts = true;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = true;// false;
	comm_rtsptt = false;
	comm_dtrptt = false;
	serloop_timing = 200;

	def_mode = modeB = modeA = A.imode = B.iBW = 1;
	def_bw = bwB = bwA = A.iBW = B.iBW = 15;
	def_freq = freqB = freqA = A.freq = B.freq = 14070000;
	max_power = 100;
	pbt = 0;
	VfoAdj = progStatus.vfo_adj;
	vfo_corr = 0;

	atten_level = 0;
	nb_ = 0;
	an_ = 0;

	has_preamp_control =
	has_bpf_center =
	has_power_control = // must be in REMOTE mode
	has_micgain_control = // must be in REMOTE mode
	has_tune_control =
	has_vfo_adj = false;

	can_change_alt_vfo =

//	has_auto_notch =
//	has_notch_control = 

	has_split =
	has_split_AB =
	has_smeter =
	has_power_out =
	has_swr_control =
	has_volume_control =
	has_rf_control =
	has_attenuator_control =
	has_ifshift_control =
	has_sql_control =
	has_ptt_control =
	has_bandwidth_control =
	has_noise_control =
	has_mode_control = true;

	precision = 1;
	ndigits = 8;

}

void RIG_TT588::initialize()
{
	tt588_widgets[0].W = btnVol;
	tt588_widgets[1].W = sldrVOLUME;
	tt588_widgets[2].W = sldrRFGAIN;
	tt588_widgets[3].W = sldrMICGAIN;
	tt588_widgets[4].W = sldrSQUELCH;

	VfoAdj = progStatus.vfo_adj;
}

void RIG_TT588::shutdown()
{
	set_if_shift(0);
}

bool RIG_TT588::check ()
{
	cmd = TT588getFREQA;
	int ret = waitN(6, 100, "check");
	if (ret < 6) return false;
	return true;
}

long RIG_TT588::get_vfoA ()
{
	cmd = TT588getFREQA;
	int ret = waitN(6, 100, "get vfo A");
	if (ret >= 6) {
        /*
         whenever 41 is in the freq. value answered, we don't want to use that index
         for an offset. (reproduce: freq. set around 21 MHz, then drag 
         the wf to change freq. up and down.)
        */
		size_t p = replystr.rfind("A");
		if (p != string::npos) {
			int f = 0;
			for (size_t n = 1; n < 5; n++)
				f = f*256 + (unsigned char)replystr[n];
			freqA = f;
		}
	}
	return (long)(freqA - vfo_corr);
}

void RIG_TT588::set_vfoA (long freq)
{
	freqA = freq;
	vfo_corr = (freq / 1e6) * VfoAdj + 0.5;
	long xfreq = freqA + vfo_corr;
	cmd = TT588setFREQA;
	cmd[5] = xfreq & 0xff; xfreq = xfreq >> 8;
	cmd[4] = xfreq & 0xff; xfreq = xfreq >> 8;
	cmd[3] = xfreq & 0xff; xfreq = xfreq >> 8;
	cmd[2] = xfreq & 0xff;
	sendCommand(cmd);
	showresp(WARN, HEX, "set vfo A", cmd, replystr);
	return ;
}

long RIG_TT588::get_vfoB()
{
	cmd = TT588getFREQB;
	int ret = waitN(6, 100, "get vfo B");
	if (ret >= 6) {
		size_t p = replystr.rfind("B");
		if (p != string::npos) {
			int f = 0;
			for (size_t n = 1; n < 5; n++)
				f = f*256 + (unsigned char)replystr[n];
			freqB = f;
		}
	}
	return (long)(freqB - vfo_corr);
}

void RIG_TT588::set_vfoB (long freq)
{
	freqB = freq;
	vfo_corr = (freq / 1e6) * VfoAdj + 0.5;
	long xfreq = freqB + vfo_corr;
	cmd = TT588setFREQB;
	cmd[5] = xfreq & 0xff; xfreq = xfreq >> 8;
	cmd[4] = xfreq & 0xff; xfreq = xfreq >> 8;
	cmd[3] = xfreq & 0xff; xfreq = xfreq >> 8;
	cmd[2] = xfreq & 0xff;
	sendCommand(cmd);
	showresp(WARN, HEX, "set vfo B", cmd, replystr);
	return ;
}

void RIG_TT588::setVfoAdj(double v)
{
	VfoAdj = v;
}

void RIG_TT588::set_modeA(int val)
{
	modeA = val;
	cmd = TT588setMODE;
	cmd[2] = TT588mode_chr[modeA];
	cmd[3] = TT588mode_chr[modeB];
	sendCommand(cmd);
	showresp(WARN, HEX, "set mode A", cmd, replystr);
}

void RIG_TT588::set_modeB(int val)
{
	modeB = val;
	cmd = TT588setMODE;
	cmd[2] = TT588mode_chr[modeA];
	cmd[3] = TT588mode_chr[modeB];
	sendCommand(cmd);
	showresp(WARN, HEX, "set mode B", cmd, replystr);
}

int RIG_TT588::get_modeA()
{
	cmd = TT588getMODE;
	int ret = waitN(4, 100, "get mode A");
	if (ret < 4) return modeA;
	size_t p = replystr.rfind("M");
	if (p == string::npos) return modeA;
	modeA = replystr[p + 1] - '0';
	return modeA;
}

int RIG_TT588::get_modeB()
{
	cmd = TT588getMODE;
	int ret = waitN(4, 100, "get mode B");
	if (ret < 4) return modeB;
	size_t p = replystr.rfind("M");
	if (p == string::npos) return modeB;
	modeB = replystr[p + 2] - '0';
	return modeB;
}

int RIG_TT588::get_modetype(int n)
{
	return TT588mode_type[n];
}

void RIG_TT588::set_bwA(int val)
{
	bwA = val;
	cmd = TT588setBW;
	cmd[2] = 37 - val;
	sendCommand(cmd);
	showresp(WARN, HEX, "set BW A", cmd, replystr);
    set_if_shift(pbt);
}

void RIG_TT588::set_bwB(int val)
{
	bwB = val;
	cmd = TT588setBW;
	cmd[2] = 37 - val;
	sendCommand(cmd);
	showresp(WARN, HEX, "set BW B", cmd, replystr);
    set_if_shift(pbt);
}

int RIG_TT588::get_bwA()
{
	cmd = TT588getBW;
	int ret = waitN(3, 100, "get BW A");
	if (ret < 3) return 37 - bwA;
	size_t p = replystr.rfind("W");
	if (p == string::npos) return 37 - bwA;
	bwA = (int)(replystr[p + 1] & 0x7F);
	return 37 - bwA;
}

int RIG_TT588::get_bwB()
{
	cmd = TT588getBW;
	int ret = waitN(3, 100, "get BW B");
	if (ret < 3) return 37 - bwB;
	size_t p = replystr.rfind("W");
	if (p == string::npos) return 37 - bwB;
	bwB = (int)(replystr[p + 1] & 0x7F);
	return 37 - bwB;
}

int  RIG_TT588::adjust_bandwidth(int m)
{
	if (m == 0) return 35; // AM
	if (m == 1 || m == 2) return 22; // LSB-USB
	if (m == 3 || m == 5) return 9;
	if (m == 6) return 0;
	if (m == 4) return 25;
	return 22;
}

int  RIG_TT588::def_bandwidth(int m)
{
	return adjust_bandwidth(m);
}

void RIG_TT588::set_if_shift(int val)
{
	pbt = val;
	cmd = TT588setPBT;
	short int si = val;
	int bpval = progStatus.bpf_center - 200 - TT588_numeric_widths[bwA]/2;
	if ((modeA == 1 || modeA == 2) && progStatus.use_bpf_center)
		si += (bpval > 0 ? bpval : 0);
	cmd[2] = (si & 0xff00) >> 8;
	cmd[3] = (si & 0xff);
	sendCommand(cmd);
	showresp(WARN, HEX, "set pbt", cmd, replystr);
}

bool RIG_TT588::get_if_shift(int &val)
{
	cmd = TT588getPBT;
	int ret = waitN(4, 100, "get pbt");
	val = pbt;
	if (ret >= 4) {
		size_t p = replystr.rfind("P");
		if (p != string::npos) {
			val = ((replystr[p + 1] & 0xFF) << 8) | (replystr[p+2] & 0xFF);
			if (val > 0x7FFF) val -= 0x10000;
		}
	}
	return (val == 0 ? false : true);
}

void RIG_TT588::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -2500;
	max = 2500;
	step = 10;
}

int  RIG_TT588::next_attenuator()
{
	switch (atten_level) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 3;
		case 3: return 0;
	}
	return 0;
}

void RIG_TT588::set_attenuator(int val)
{
	atten_level = val;
	cmd = TT588setATT;

	cmd[2] = '0' + atten_level;
	switch (atten_level) {
		case 0: atten_label("0 dB", false); break;
		case 1: atten_label("6 dB", true); break;
		case 2: atten_label("12 dB", true); break;
		case 3: atten_label("18 dB", true); break;
	}
	sendCommand(cmd);
	showresp(WARN, HEX, "set att", cmd, replystr);
}


int RIG_TT588::get_attenuator()
{
	cmd = TT588getATT;
	int ret = waitN(3, 100, "get att");
	int val = atten_level;
	if (ret >= 3) {
		size_t p = replystr.rfind("J");
		if (p != string::npos) val = (replystr[p + 1] - '0');
	}
	if (atten_level != val) atten_level = val;
	switch (atten_level) {
		case 0: atten_label("0 dB", false); break;
		case 1: atten_label("6 dB", true); break;
		case 2: atten_label("12 dB", true); break;
		case 3: atten_label("18 dB", true); break;
	}
	return atten_level;
}

int RIG_TT588::get_smeter()
{
	int sval = 0;
	float fval = 0;
	cmd = TT588getSMETER;
	int ret = waitN(6, 100, "get smeter");
	if (ret < 6) return sval;
	size_t p = replystr.rfind("S");
	if (p == string::npos) return sval;

	sscanf(&replystr[p + 1], "%4x", &sval);
	fval = sval/256.0;
	sval = (int)(fval * 100.0 / 18.0);
	if (sval > 100) sval = 100;
	return sval;
}

int RIG_TT588::get_volume_control()
{
	cmd = TT588getVOL;
	int ret = waitN(3, 100, "get vol");
	if (ret < 3) return progStatus.volume;
	size_t p = replystr.rfind("U");
	if (p == string::npos) return progStatus.volume;

	return (int)((replystr[p + 1] & 0x7F) / 1.27);
}

void RIG_TT588::set_volume_control(int vol)
{
	cmd = TT588setVOL;
	cmd[2] = 0x7F & (int)(vol * 1.27);
	sendCommand(cmd);
	showresp(WARN, HEX, "set vol", cmd, replystr);
}

void RIG_TT588::set_rf_gain(int val)
{
	cmd = TT588setRF;
	cmd[2] = 0x7F & (int)(val * 1.27);
	sendCommand(cmd);
	showresp(WARN, HEX, "set rfgain", cmd, replystr);
}

int  RIG_TT588::get_rf_gain()
{
	cmd = TT588getRF;
	int ret = waitN(3, 100, "get rfgain");
	if (ret < 3) return 100;
	size_t p = replystr.rfind("I");
	if (p == string::npos) return 100;
	return (int)((replystr[p+1] & 0x7F) / 1.27);
}

// Tranceiver PTT on/off

void RIG_TT588::set_PTT_control(int val)
{
	cmd = TT588setXMT;
	if (val) {
		cmd[2] = 0x04;
		cmd[3] = 0;
	} else {
		cmd[2] = 0;
		cmd[3] = 0;
	}
	sendCommand(cmd);
	showresp(WARN, HEX, "set PTT", cmd, replystr);
}

int RIG_TT588::get_power_out()
{
	cmd = TT588getPOWER;
	int ret = waitN(4, 100, "get pout");
	if (ret < 4) return 0;
	size_t p = replystr.rfind("F");
	if (p == string::npos) return 0;
	fwdpwr = replystr[p + 1] & 0x7F;
	refpwr = replystr[p + 2] & 0x7F;
    // it looks like it never returns reflected power < 1
    refpwr -= 1;
	fwdv = sqrtf(fwdpwr);
	refv = sqrtf(refpwr);
	return fwdpwr;
}
int RIG_TT588::get_swr()
{
    float swr = (fwdv + refv)/(fwdv - refv) ;
    swr -= 1; 
	swr *= 25.0;
	if (swr < 0) swr = 0;
	if (swr > 100) swr = 100;
	return (int)swr;
}
void RIG_TT588::set_squelch(int val)
{
	cmd = TT588setSQLCH;
	cmd[2] = (unsigned char)(val * 1.27);
	sendCommand(cmd);
	showresp(WARN, HEX, "set sql", cmd, replystr);
}

int  RIG_TT588::get_squelch()
{
	cmd = TT588getSQLCH;
	int ret = waitN(3, 100, "get sql");
	if (ret < 3) return 0;
	size_t p = replystr.rfind("H");
	if (p == string::npos) return 0;
	return (int)((replystr[p+1] & 0x7F) / 1.27);
}

void RIG_TT588::set_noise(bool val)
{
	static char nblabel[] = "NB ";
	nb_++;
	if (nb_ == 8) nb_ = 0;
	nblabel[2] = '0' + nb_;
	nb_label(nblabel, nb_ ? true : false);
	cmd = TT588setNB;
	cmd[2] = (unsigned char)nb_;
	cmd[3] = 0;
	cmd[4] = (unsigned char)an_;
	sendCommand(cmd);
	showresp(WARN, HEX, "set NB", cmd, replystr);
}

int  RIG_TT588::get_noise()
{
	cmd = TT588getNB;
	int ret = waitN(5, 100, "get NB");
	if (ret < 5) return nb_;
	size_t p = replystr.rfind("K");
	if (p == string::npos) return nb_;
	int val = replystr[p+1];
	if (nb_ != val) nb_ = val;
	static char nblabel[] = "NB ";
	nblabel[2] = '0' + nb_;
	nb_label(nblabel, nb_ ? true : false);
	return nb_;
}

/*
void RIG_TT588::set_auto_notch(int val)
{
	static char anlabel[] = "AN ";
	an_++;
	if (an_ == 10) an_ = 0;
	anlabel[2] = '0' + an_;
	auto_notch_label(anlabel, an_ > 0 ? true : false);
	cmd = TT588setNB;
	cmd[2] = (unsigned char)nb_;
	cmd[3] = 0;
	cmd[4] = (unsigned char)an_;
	sendCommand(cmd);
}

int  RIG_TT588::get_auto_notch()
{
	cmd = TT588getNB;
	int ret = sendCommand(cmd);
	showresp(WARN, HEX, "get AN", cmd, replystr);
	int val = an_;
	if (ret >= 5) {
		size_t p = replystr.rfind("K");
		if (p != string::npos) val = replystr[p+3];
	}
	if (an_ != val) an_ = val;
	static char anlabel[] = "AN ";
	anlabel[2] = '0' + an_;
	auto_notch_label(anlabel, an_ ? true : false);
	return an_;
}
*/

void RIG_TT588::set_split(bool val)
{
	cmd = TT588setSPLIT;
	cmd[2] = val == true ? 1 : 0;
	sendCommand(cmd);
	showresp(WARN, HEX, "set split", cmd, replystr);
}

int RIG_TT588::get_split()
{
	cmd = TT588getSPLIT;
	int ret = waitN(3, 100, "get split");
	if (ret == 3)
		return (replystr[1] == 0x01);
	return false;
}

/*
int  RIG_TT588::get_power_control(void)
{
	cmd = TT588getPOWER;
	int ret = waitN(7, 100, "get pc");
	if (ret == 7) {
		int pc = replystr[3] & 0x7F;
		return (int)ceil(pc / 1.27);
	}
	return 0;
}

void RIG_TT588::set_power_control(double val)
{
	cmd = TT588setPOWER;
	cmd[4] = ((int)(val * 1.27) & 0x7f);
	sendCommand(cmd);
	showresp(WARN, HEX, "set pc", cmd, replystr);
}

int  RIG_TT588::get_preamp()
{
	cmd = TT588getPREAMP;
	int ret = waitN(5, 100, "get preamp");
	if (ret == 5) 
		return replystr[3];
	return 0;
}

void RIG_TT588::set_preamp(int val)
{
	cmd = TT588setPREAMP;
	cmd[4] = (val == 0 ? 0 : 1);
	sendCommand(cmd);
	showresp(WARN, HEX, "set preamp", cmd, replystr);
}
*/
