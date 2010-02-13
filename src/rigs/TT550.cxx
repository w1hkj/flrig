/*
 * Pegasus TT-550 drivers
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */

// TenTec Pegasus computer controlled transceiver

#include <math.h>

#include "TT550.h"
#include "support.h"
#include "util.h"
#include "debug.h"

static const char TT550name_[] = "TT-550";

enum TT550_MODES { 
TT550_AM_MODE, TT550_USB_MODE, TT550_LSB_MODE, TT550_CW_MODE, TT550_DIGI_MODE, TT550_FM_MODE };

static const char *TT550modes_[] = {
		"AM", "USB", "LSB", "CW", "DIGI", "FM", NULL};
static const char TT550mode_chr[] =  { '0', '1', '2', '3', '1', '4' };
static const char TT550mode_type[] = { 'U', 'U', 'L', 'L', 'U', 'U' };

static const char *TT550_widths[] = {
 "300",  "330",  "375",  "450",  "525",  "600",  "675",  "750",  "900", "1050", 
"1200", "1350", "1500", "1650", "1800", "1950", "2100", "2250", "2400", "2550",
"2700", "2850", "3000", "3300", "3600", "3900", "4200", "4500", "4800", "5100",
"5400", "5700", "6000", "8000", NULL};

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

static char TT550restart[]		= "XX\r";
static char TT550init[]			= "P1\r";
//static char TT550isRADIO[]		= " RADIO START";
//static char TT550isDSP[]		= " DSP START";

//static char TT550setFREQ[]		= "N123456\r";
// <1> high byte of 16 bit coarse tuning factor
// <2> low  byte of 16 bit coarse tuning factor
// <3> high byte of 16 bit fine tuning factor
// <4> low  byte of 16 bit fine tuning factor
// <5> high byte of 16 bit BFO factor
// <6> low  byte of 16 bit BFO factor
// filter, mode & bfo all effect the tuning factors


static char TT550setMODE[]		= "Mnn\r";
static char TT550setRcvBW[]		= "Wx\r";
static char TT550setXmtBW[]		= "Cx\r";
static char TT550setVolume[]	= "Vn\r";
static char TT550setAGC[]		= "Gc\r";
static char TT550setRFGAIN[]	= "An\r";
static char TT550setATT[]		= "Bc\r";
static char TT550setCWWPM[]		= "Eabcdef\r";
static char TT550setMONVOL[]	= "Hn\r";
static char TT550setCWMONVOL[]	= "Jn\r";
static char TT550setNRNOTCH[]	= "Kna\r";
static char TT550setLINEOUT[]	= "Ln\r"; // 63 - min, 0 - max
static char TT550setMICLINE[]	= "O1cn\r"; // *******************************************
static char TT550setPOWER[]		= "Pn\r"; // ****************************************
static char TT550setXMT[]		= "Q1\r";
static char TT550setRCV[]		= "Q0\r";
static char TT550setSQUELCH[]	= "Sn\r";	// 0..19; 6db / unit
static char TT550setVOX[]		= "Uc\r";	// '0' = off; '1' = on
static char TT550setVOXGAIN[]	= "UGn\r";	// 0 <= n <= 255
static char TT550setANTIVOX[]	= "UAn\r";	// 0..255
static char TT550setVOXHANG[]	= "UHn\r";	// 0..255; n= delay*0.0214 sec
static char TT550setCWSPOTLVL[]	= "Fn\r";	// 0..255; 0 = off
static char TT550setCWQSK[]		= "UQn\r";	// 0..255; 0 = none
static char TT550setAUXHANG[]	= "UTn\r";	// 0..255; 0 = none
static char TT550setBLANKER[]	= "Dn\r";	// 0..7; 0 = off
static char TT550setSPEECH[]	= "Yn\r";	// 0..127; 0 = off

static char TT550setDISABLE[]	= "#0\r";	// disable transmitter
static char TT550setENABLE[]	= "#1\r";	// enable transmitter
static char TT550setTLOOP_OFF[]	= "#2\r";	// disable T loop
static char TT550setTLOOP_ON[]	= "#3\r";	// enable T loop
static char TT550setKEYER_ON[]	= "#6\r";	// enable keyer
static char TT550setKEYER_OFF[]	= "#7\r";	// disable keyer
//static char TT550setALIVE_OFF[]	= "#8\r";	// disable keep alive
//static char TT550setALIVE_ON[]	= "#9\r";	// enable keep alive

//static char TT550getAGC[]		= "?Y\r";	// 0..255
//static char TT550getFWDPWR[]	= "?F\r";	// F<0..255>
//static char TT550getREFPWR[]	= "?R\r";	// R<0..255>
static char TT550getSIGNAL_LEVEL[]	= "?S\r";	// S<0..255><0..255>
//static char TT550getFWDREF[]	= "?S\r";	// T<0..255><0..255>


RIG_TT550::RIG_TT550() {
// base class values	
	name_ = TT550name_;
	modes_ = TT550modes_;
	bandwidths_ = TT550_widths;
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
	
	mode_ = 1;
	bw_ = 20;
	def_mode = 1;
	defbw_ = 20;
	deffreq_ = 14070000;
	max_power = 100;

	VfoAdj = -0.9;
	RitFreq = 0;
	PbtFreq = 0;
	XitFreq = 0;
	Bfo = 600;

	ATTlevel = 0;
	RFgain = 100;

	has_notch_control =
	has_preamp_control =
	has_micgain_control =
	has_swr_control = false;

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
	has_bfo =
	has_rf_control =
	has_attenuator_control =
	has_volume_control =
	has_ifshift_control =
	has_ptt_control =
	has_bandwidth_control =
	has_auto_notch = 
	has_tune_control =
	has_noise_control =
	has_mode_control = true;

	auto_notch = noise_reduction = false;

	use_line_in = true;

}

void RIG_TT550::showresponse(string s)
{
	LOG_WARN("%s: %s", s.c_str(), str2hex((char *)replybuff, strlen((char *)replybuff)));
}

void RIG_TT550::showASCII(string s)
{
	string ss = "";
	for (size_t i = 0; i < s.length(); i++)
		if (!(s[i] == '\r' || s[i] == '\n'))
			ss += replystr[i];
	LOG_WARN("%s", ss.c_str());
}

void RIG_TT550::initialize()
{
	clearSerialPort();

	cmd = TT550restart; // wake up radio
	sendCommand(cmd, 0, true);
	MilliSleep(200);
	cmd = TT550restart; // wake up radio, Jupiter seems to need 2 calls
	sendCommand(cmd, 16, true);
	showASCII(replystr);

	if (replystr.find("RADIO") == string::npos) { // not in radio mode
		cmd = TT550init; // put into radio mode
		sendCommand(cmd, 0, true);//13, true);
		MilliSleep( 1000 );
		readResponse();
		showASCII(replystr);
	}

	cmd = "?V\r";
	sendCommand(cmd, 20, true);
	showASCII(replystr);

//	set_noise_reduction(noise_reduction);
	set_auto_notch(auto_notch);
	set_compression();

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
	set_mic_gain(100);
	set_rf_gain(RFgain);
	
	XitFreq = progStatus.xit_freq;
	RitFreq = progStatus.rit_freq;
	Bfo = progStatus.bfo_freq;
	set_vfoA(freq_);

	VfoAdj = progStatus.tt550_vfo_adj;

//	setXit(XitFreq);
//	setRit(RitFreq);
//	setBfo(Bfo);

//	set_volume_control(20);
	set_attenuator(0);
	set_mon_vol();
	set_squelch_level();
	set_if_shift(PbtFreq);
	set_aux_hang();

	enable_tloop();
	enable_xmtr();
}

void RIG_TT550::enable_xmtr()
{
	if (progStatus.tt550_enable_xmtr)
		cmd = TT550setENABLE;
	else
		cmd = TT550setDISABLE;
	sendCommand(cmd, 0, true);
}

void RIG_TT550::enable_tloop()
{
	if (progStatus.tt550_enable_tloop)
		cmd = TT550setTLOOP_ON;
	else
		cmd = TT550setTLOOP_OFF;
	sendCommand(cmd, 0, true);
}

void RIG_TT550::shutdown()
{
	cmd = "Vx\r";
	cmd[1] = 0;
	sendCommand(cmd, 0, true); // volume = zero
	cmd = "Lx\r";
	cmd[1] = 0x3F;
	sendCommand(cmd, 0, true); // line out = minimum
}

int DigiAdj = 0;

void RIG_TT550::set_vfoRX(long freq)
{
	freq_ = freq;

	int NVal = 0, FVal = 0;	// N value / finetune value
    int TBfo = 0;			// temporary BFO (Hz)
	int IBfo = 0;			// Intermediate BFO Freq (Hz)

	int PbtAdj = PbtActive ? PbtFreq : 0;	// passband adj (Hz)
	int	RitAdj = RitActive ? RitFreq : 0;	// RIT adj (Hz)

	int FiltAdj = (TT550_filter_width[bw_])/2;		// filter bw (Hz)

	long lFreq = freq * (1 + VfoAdj * 1e-6) + RitAdj;

	if(mode_ == TT550_DIGI_MODE) {
		DigiAdj = 1500 - FiltAdj - 200;
		DigiAdj = DigiAdj < 0 ? 0 : DigiAdj;
		IBfo = FiltAdj + 200;
		lFreq += (IBfo + PbtAdj + DigiAdj);
		IBfo = IBfo + PbtAdj + DigiAdj;
	}

	if(mode_ == TT550_USB_MODE) {
		IBfo = FiltAdj + 200;
		lFreq += (IBfo + PbtAdj);
		IBfo = IBfo + PbtAdj;
	}

	if(mode_ == TT550_LSB_MODE) {
		IBfo = FiltAdj + 200;
		lFreq -= (IBfo + PbtAdj);
		IBfo = IBfo + PbtAdj;
	}

	if(mode_ == TT550_CW_MODE) {
// CW Mode uses LSB Mode
		if (( FiltAdj + 300) <= Bfo) {
			IBfo = PbtAdj + Bfo;
		} else {
		 	IBfo = FiltAdj + 300;
			lFreq += (Bfo - IBfo);
			IBfo = IBfo + PbtAdj;
		}
	}
	
	if(mode_ == TT550_FM_MODE) {
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
	sendCommand(cmd, 0, true);
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

	FilterBw = TT550_filter_width[bw_];
	if (FilterBw < 900) FilterBw = 900;
	if (FilterBw > 3900) FilterBw = 3900;
	if (mode_ == TT550_DIGI_MODE) FilterBw = 3000;

	bwBFO = (FilterBw/2) + 200;

	IBfo = (bwBFO > IBfo) ?  bwBFO : IBfo ;

	if (mode_ == TT550_USB_MODE || mode_ == TT550_DIGI_MODE) {
		lFreq += IBfo;
		TBfo = (int)(IBfo * 2.73);
	}

	if (mode_ == TT550_LSB_MODE) {
		lFreq -= IBfo;
		TBfo = (int)(IBfo * 2.73);
	}

// CW Mode uses LSB Mode
	if(mode_ == TT550_CW_MODE) {
		IBfo = 1500; // fixed for CW
		lFreq += Bfo - IBfo;
		TBfo = (int)(Bfo * 2.73);
	}

	if(mode_ == TT550_FM_MODE) {
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
	sendCommand(cmd, 0, true);
}

void RIG_TT550::set_vfoA (long freq)
{
	set_vfoRX(freq);
	set_vfoTX(freq);
	return ;
}

long RIG_TT550::get_vfoA ()
{
	return freq_;
}

// Tranceiver PTT on/off
void RIG_TT550::set_PTT_control(int val)
{
	if (val) sendCommand(TT550setXMT, 0, true);
	else     sendCommand(TT550setRCV, 0, true);
}

void RIG_TT550::set_mode(int val)
{
	mode_ = val;
	cmd = TT550setMODE;
	cmd[1] = cmd[2] = TT550mode_chr[val];
	sendCommand(cmd, 0, true);
	set_vfoA(freq_);
}

int RIG_TT550::get_mode()
{
	return mode_;
}

int RIG_TT550::get_modetype(int n)
{
	return TT550mode_type[n];
}

void RIG_TT550::set_bandwidth(int val)
{
	bw_ = val;
	int rxbw = TT550_filter_nbr[bw_];
	int txbw = TT550_filter_nbr[bw_];
	if (txbw < 7) txbw = 7;
	if (txbw > 24) txbw = 24;
	cmd = TT550setRcvBW;
	cmd[1] = rxbw;
	sendCommand(cmd, 0, true);
	cmd = TT550setXmtBW;
	cmd[1] = txbw;
	sendCommand(cmd, 0, true);
	set_vfoA(freq_);
}

int RIG_TT550::get_bandwidth()
{
	return bw_;
}

int RIG_TT550::adjust_bandwidth(int md)
{
	if (md == TT550_AM_MODE) bw_ = 32;
	if (md == TT550_CW_MODE) bw_ = 10;
	if (md == TT550_LSB_MODE || md == TT550_USB_MODE) bw_ = 20;
	if (md == TT550_FM_MODE) bw_ = 32;
	return bw_;
}

void RIG_TT550::set_if_shift(int val)
{
	PbtFreq = val;
	if (PbtFreq) PbtActive = true;
	set_vfoRX(freq_);
}

bool RIG_TT550::get_if_shift(int &val)
{
	val = PbtFreq;
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
	cmd = TT550setATT;
	if (val) cmd[1] = '1';
	else     cmd[1] = '0';
	sendCommand(cmd, 0, true);
}

void RIG_TT550::set_volume_control(int val)
{
// 0 <= val <= 100
	cmd = TT550setVolume;
	cmd[1] = 0xFF & (int)(127.0 * log(val ? val : 1) / log(100));
	sendCommand(cmd, 0, true);
}

int RIG_TT550::get_volume_control()
{
	return progStatus.volume;
}

int RIG_TT550::get_smeter()
{
	double sig = 0.0;
	cmd = TT550getSIGNAL_LEVEL;
	sendCommand(cmd, 6, true);
	if (replybuff[0] == 'S') {
		int sval;
		replybuff[5] = 0;
		sscanf(&replybuff[1], "%4x", &sval);
		sig = sval / 256.0;
	}
	return (int)(sig * 50.0 / 9.0);
}

int RIG_TT550::get_swr()
{
	double swr = (fwdv + refv) / (fwdv - refv + .0001);
	swr -= 1.0;
	swr *= 25.0;
	if (swr < 0) swr = 0;
	if (swr > 100) swr = 100;
	return (int)swr;
}

int RIG_TT550::get_power_out()
{
	cmd = TT550getSIGNAL_LEVEL;
	sendCommand(cmd, 4, true);
	if (replybuff[0] == 'T') {
		fwdpwr = 0.8*fwdpwr + 0.2*(unsigned char)replybuff[1];
		refpwr = 0.8*refpwr + 0.2*(unsigned char)replybuff[2];
	}
	fwdv = sqrtf(fwdpwr);
	refv = sqrtf(refpwr);
	return fwdpwr;
}

void RIG_TT550::setBfo(int val)
{
	progStatus.bfo_freq = Bfo = val;
	set_vfoA(freq_);
}

int RIG_TT550::getBfo()
{
	return Bfo;
}

void RIG_TT550::setVfoAdj(double v)
{
	VfoAdj = v;
}

void RIG_TT550::setRit(int val)
{
	progStatus.rit_freq = RitFreq = val;
	if (RitFreq) RitActive = true;
	set_vfoA(freq_);
}

int RIG_TT550::getRit()
{
	return RitFreq;
}

void RIG_TT550::setXit(int val)
{
	progStatus.xit_freq = XitFreq = val;
	if (XitFreq) XitActive = true;
	set_vfoA(freq_);
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
	sendCommand(cmd, 0, true);
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
	cmd[1] = (0x3F) & (int)((100 - progStatus.tt550_line_out) * .63);
	if (cmd[1] == 0x0D) cmd[1] = 0x0E;
	sendCommand(cmd, 0, true);
}

void RIG_TT550::set_agc_level()
{
	cmd = TT550setAGC;
	switch (progStatus.tt550_agc_level) {
		case 0 : cmd[1] = '1'; break;
		case 1 : cmd[1] = '2'; break;
		case 2 : cmd[1] = '3'; break;
	}
	sendCommand(cmd, 0, true);
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
	cmd[3] = 0xFF & (spcfactor >> 8);
	cmd[4] = 0xFF & spcfactor;
	cmd[5] = 0xFF & (dahfactor >> 8);
	cmd[6] = 0xFF & dahfactor;
	sendCommand(cmd, 0, true);
}

void RIG_TT550::set_cw_vol()
{
	int val = progStatus.tt550_cw_vol;
	cmd = TT550setCWMONVOL;
	cmd[1] = 0xFF & (int)(127.0 * log(val ? val : 1) / log(100));
	if (cmd[1] == 0x0D) cmd[1] = 0x0E;
	sendCommand(cmd, 0, true);
}

void RIG_TT550::set_cw_spot()
{
	int val = progStatus.tt550_cw_spot;
	cmd = TT550setCWSPOTLVL;
	cmd[1] = 0xFF & (int)(127.0 * log(val ? val : 1) / log(100));
	if (!progStatus.tt550_spot_onoff) cmd[1] = 0;
	if (cmd[1] == 0x0D) cmd[1] = 0x0E;
	sendCommand(cmd, 0, true);
}

void RIG_TT550::set_spot_onoff()
{
	set_cw_spot();
}

void RIG_TT550::set_cw_weight()
{
	set_cw_wpm();
}

void RIG_TT550::set_cw_qsk()
{
	cmd = TT550setCWQSK;
	cmd[2] = (0xFF) & (int)(progStatus.tt550_cw_qsk * 2.55);
	if (cmd[2] == 0x0D) cmd[2] = 0x0E;
	sendCommand(cmd, 0, true);
}

void RIG_TT550::enable_keyer()
{
	if (progStatus.tt550_enable_keyer)
		cmd = TT550setKEYER_ON;
	else
		cmd = TT550setKEYER_OFF;
	sendCommand(cmd, 0, true);
}


void RIG_TT550::set_vox_onoff()
{
	cmd = TT550setVOX;
	cmd[1] = progStatus.vox_onoff ? '1' : '0';
	sendCommand(cmd, 0, true);
}

void RIG_TT550::set_vox_gain()
{
	cmd = TT550setVOXGAIN;
	cmd[2] = (0xFF) & (int)(progStatus.tt550_vox_gain * 2.55);
	if (cmd[2] == 0x0D) cmd[2] = 0x0E;
	sendCommand(cmd, 0, true);
}

void RIG_TT550::set_vox_anti()
{
	cmd = TT550setANTIVOX;
	cmd[2] = (0xFF) & (int)(progStatus.tt550_vox_anti * 2.55);
	if (cmd[2] == 0x0D) cmd[2] = 0x0E;
	sendCommand(cmd, 0, true);
}

void RIG_TT550::set_vox_hang()
{
	cmd = TT550setVOXHANG;
	cmd[2] = (0xFF) & (int)(progStatus.tt550_vox_hang * 2.55);
	if (cmd[2] == 0x0D) cmd[2] = 0x0E;
	sendCommand(cmd, 0, true);
}

void RIG_TT550::set_aux_hang()
{
	cmd = TT550setAUXHANG;
	cmd[2] = 0;
	sendCommand(cmd, 0, true);
}

void RIG_TT550::set_compression()
{
	cmd = TT550setSPEECH;
	cmd[1] = (0x7F) & (int)(progStatus.tt550_compression * 1.27);
	if (cmd[1] == 0x0D) cmd[1] = 0x0E;
	sendCommand(cmd, 0, true);
}

void RIG_TT550::set_auto_notch(int v)
{
	auto_notch = v;
	cmd = TT550setNRNOTCH;
	cmd[1] = noise_reduction ? '1' : '0';
	cmd[2] = auto_notch ? '1' : '0';
	sendCommand(cmd, 0, true);
}

void RIG_TT550::set_noise_reduction(int b)
{
	noise_reduction = b;
	cmd = TT550setNRNOTCH;
	cmd[1] = noise_reduction ? '1' : '0';
	cmd[2] = auto_notch ? '1' : '0';
	sendCommand(cmd, 0, true);
}

void RIG_TT550::set_mic_gain(int v)
{
	cmd = TT550setMICLINE;
	cmd[2] = use_line_in ? 1 : 0;
	cmd[3] = (unsigned char)(v * 0.15);
	sendCommand(cmd, 0, true);
}

void RIG_TT550::set_power_control(double val)
{
	cmd =  TT550setPOWER;
	cmd[1] = (unsigned char)(val * 2.55);
	if (cmd[1] == 0x0D) cmd[1] = 0x0E;
	sendCommand(cmd, 0, true);

}

void RIG_TT550::set_mon_vol()
{
	cmd = TT550setMONVOL;
	cmd[1] = (unsigned char)(progStatus.tt550_mon_vol * 2.55);
	if (cmd[1] == 0x0D) cmd[1] = 0x0E;
	sendCommand(cmd, 0, true);
}

void RIG_TT550::set_squelch_level()
{
	cmd = TT550setSQUELCH;
	cmd[1] = (unsigned char)(progStatus.tt550_squelch_level * 2.55);
	if (cmd[1] == 0x0D) cmd[1] = 0x0E;
	sendCommand(cmd, 0, true);
}

void RIG_TT550::set_nb_level()
{
	cmd = TT550setBLANKER;
	cmd[1] = progStatus.tt550_nb_level;
	if (!progStatus.noise) cmd[1] = 0;
	sendCommand(cmd, 0, true);
}

void RIG_TT550::set_noise(bool b)
{
	progStatus.noise = b;
	set_nb_level();
}

void RIG_TT550::tuner_bypass()
{
}

// callbacks for tt550 transceiver
void cb_tt550_line_out()
{
	selrig->set_line_out();
}

void cb_tt550_agc_level()
{
	selrig->set_agc_level();
}

void cb_tt550_cw_wpm()
{
	selrig->set_cw_wpm();
}

void cb_tt550_cw_vol()
{
	selrig->set_cw_vol();
}

void cb_tt550_cw_spot()
{
	selrig->set_cw_spot();
}

void cb_tt550_cw_weight()
{
	selrig->set_cw_weight();
}

void cb_tt550_enable_keyer()
{
	selrig->enable_keyer();
}

void cb_tt550_spot_onoff()
{
	selrig->set_spot_onoff();
}

void cb_tt550_vox_gain()
{
	selrig->set_vox_gain();
}

void cb_tt550_vox_anti()
{
	selrig->set_vox_anti();
}

void cb_tt550_vox_hang()
{
	selrig->set_vox_hang();
}

void cb_tt550_vox_onoff()
{
	selrig->set_vox_onoff();
}

void cb_tt550_compression()
{
	selrig->set_compression();
}

void cb_tt550_mon_vol()
{
	selrig->set_mon_vol();
}

void cb_tt550_tuner_bypass()
{
	selrig->tuner_bypass();
}

void cb_tt550_enable_xmtr()
{
	selrig->enable_xmtr();
}

void cb_tt550_enable_tloop()
{
	selrig->enable_tloop();
}

void cb_tt550_nb_level()
{
	progStatus.tt550_nb_level = cbo_tt550_nb_level->index();
	selrig->set_nb_level();
}

void cb_tt550_adj_vfo()
{
	selrig->setVfoAdj(progStatus.tt550_vfo_adj);
	selrig->set_vfoA(selrig->get_vfoA());
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

*/
