/*
 * Pegasus TT-550 drivers
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */

// TenTec Pegasus computer controlled transceiver

#include "TT550.h"
#include "support.h"

static const char TT550name_[] = "TT-550";

enum TT550_MODES { 
TT550_AM_MODE, TT550_USB_MODE, TT550_LSB_MODE, TT550_CW_MODE, TT550_FM_MODE };

static const char *TT550modes_[] = {
		"AM", "USB", "LSB", "CW", "FM", NULL};
static const char TT550mode_chr[] =  { '0', '1', '2', '3', '4' };
static const char TT550mode_type[] = { 'U', 'U', 'L', 'L', 'U' };

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
//static char TT550setMONVOL[]	= "Hn\r";
static char TT550setCWMONVOL[]	= "Jn\r";
//static char TT550setNRNOTCH[]	= "Kna\r";
static char TT550setLINEOUT[]	= "Ln\r";
//static char TT550setMICLINE[]	= "Ocn\r";
//static char TT550setPOWER[]		= "Pn\r";
static char TT550setXMT[]		= "Q1\r";
static char TT550setRCV[]		= "Q0\r";
//static char TT550setSQUELCH[]	= "Sn\r";	// 0..19; 6db / unit
static char TT550setVOX[]		= "Uc\r";	// '0' = off; '1' = on
static char TT550setVOXGAIN[]	= "UGn\r";	// 0 <= n <= 255
static char TT550setANTIVOX[]	= "UAn\r";	// 0..255
static char TT550setVOXHANG[]	= "UHn\r";	// 0..255; n= delay*0.0214 sec
static char TT550setCWSPOTLVL[]	= "Fn\r";	// 0..255; 0 = off
//static char TT550setBLANKER[]	= "Dn\r";	// 0..7; 0 = off
static char TT550setSPEECH[]	= "Yn\r";	// 0..127; 0 = off

//static char TT550setDISABLE[]	= "#0\r";	// disable transmitter
//static char TT550setENABLE[]	= "#1\r";	// enable transmitter
//static char TT550setTLOOP_OFF[]	= "#2\r";	// disable T loop
//static char TT550setTLOOP_ON[]	= "#3\r";	// enable T loop
//static char TT550setKEYER_ON[]	= "#6\r";	// enable keyer
//static char TT550setKEYER_OFF[]	= "#7\r";	// disable keyer
//static char TT550setALIVE_OFF[]	= "#8\r";	// disable keep alive
//static char TT550setALIVE_ON[]	= "#9\r";	// enable keep alive

//static char TT550getAGC[]		= "?Y\r";	// 0..255
//static char TT550getFWDPWR[]	= "?F\r";	// F<0..255>
//static char TT550getREFPWR[]	= "?R\r";	// R<0..255>
static char TT550getSMETER[]	= "?S\r";	// S<0..255><0..255>
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
	
	mode_ = 3;
	bw_ = 30;
	def_mode = 1;
	defbw_ = 20;
	deffreq_ = 14070000;
	max_power = 100;

	VfoAdj = 0;
	RitFreq = 0;
	PbtFreq = 0;
	XitFreq = 0;
	Bfo = 700;

	ATTlevel = 0;
	RFgain = 100;

	has_power_control =
	has_micgain_control =
	has_notch_control =
	has_tune_control =
	has_noise_control =
	has_preamp_control =
	has_swr_control = false;

	has_line_out =
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
	has_mode_control = true;

}

void RIG_TT550::initialize()
{
	sendCommand(TT550restart, 20, true);
	cmd = TT550init;
	cmd[1] = 1;
	sendCommand(cmd, 20, true);
}

void RIG_TT550::checkresponse()
{
	if (RigSerial.IsOpen() == false)
		return;
	if (replybuff[0] == 'G')
		return;
	LOG_ERROR("\nsent  %s\nreply %s",
		str2hex(cmd.c_str(), cmd.length()),
		str2hex((char *)replybuff, strlen((char *)replybuff)));
}

void RIG_TT550::showresponse()
{
	LOG_INFO("%s", str2hex((char *)replybuff, strlen((char *)replybuff)));
}

void RIG_TT550::set_vfoRX(long freq)
{
	freq_ = freq;

	int NVal = 0, FVal = 0;	// N value / finetune value
    int TBfo = 0;			// temporary BFO (Hz)
	int IBfo = 0;			// Intermediate BFO Freq (Hz)

	int PbtAdj = PbtActive ? PbtFreq : 0;	// passband adj (Hz)
	int	RitAdj = RitActive ? RitFreq : 0;	// RIT adj (Hz)

	int FiltAdj = (TT550_filter_width[bw_])/2;		// filter bw (Hz)

	long lFreq = freq + VfoAdj + RitAdj;

	lFreq += (mode_ == TT550_USB_MODE) ? PbtAdj : -PbtAdj;

	if(mode_ == TT550_USB_MODE) {
		IBfo = FiltAdj + 200;
		lFreq += IBfo;
		IBfo = IBfo + PbtAdj;
	}

	if(mode_ == TT550_LSB_MODE) {
		IBfo = FiltAdj + 200;
		lFreq -= IBfo;
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
	long lFreq = freq;

	lFreq += XitAdj = XitActive ? XitFreq : 0;

	FilterBw = TT550_filter_width[bw_];
	if (FilterBw < 900) FilterBw = 900;
	if (FilterBw > 3900) FilterBw = 3900;

	bwBFO = (FilterBw/2) + 200;

	IBfo = (bwBFO > IBfo) ?  bwBFO : IBfo ;

	if (mode_ == TT550_USB_MODE) {
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
	cmd[2] = cmd[3] = TT550mode_chr[val];
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
	min = -2900;
	max = 2900;
	step = 100;
}

void RIG_TT550::set_attenuator(int val)
{
	cmd = TT550setATT;
	if (val) cmd[2] = '1';
	else     cmd[2] = '0';
	sendCommand(cmd, 0, true);
}

void RIG_TT550::set_noise(bool b)
{
//	cmd = TT550setNB;
//	if (b)
//		cmd[2] = '4';
//	else
//		cmd[2] = '0';
//	sendCommand(cmd, 2, true);
}

void RIG_TT550::set_volume_control(int val)
{
	cmd = TT550setVolume;
	cmd[1] = val * 2.55;
	sendCommand(cmd, 0, true);
}


int RIG_TT550::get_smeter()
{
	double sig = 0.0;
	cmd = TT550getSMETER;
	sendCommand(cmd, 3, true);
	if (replybuff[0] == 'S') {
		sig = (50.0 / 9.0) * (replybuff[1] + replybuff[2] / 256.0);
	}
	return (int)sig;
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
//	fwdpwr = refpwr = fwdv = refv = 0;
	cmd = TT550getSMETER;
	sendCommand(cmd, 3, true);
	if (replybuff[0] == 'T') {
		fwdpwr = (unsigned char)replybuff[1] / 2.56;
		refpwr = (unsigned char)replybuff[2] / 2.56;
	}
	fwdv = sqrtf(fwdpwr);
	refv = sqrtf(refpwr);
	return fwdpwr;
}

void RIG_TT550::setBfo(int val)
{
	Bfo = val;
	set_vfoA(freq_);
}

int RIG_TT550::getBfo()
{
	return Bfo;
}

void RIG_TT550::setRit(int val)
{
	RitFreq = val;
	set_vfoA(freq_);
}

int RIG_TT550::getRit()
{
	return RitFreq;
}

void RIG_TT550::setXit(int val)
{
	XitFreq = val;
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
	cmd[1] = (0x3F) & (int)((100 - progStatus.line_out) * .63);
	sendCommand(cmd, 0, true);
}

void RIG_TT550::set_agc_level()
{
	cmd = TT550setAGC;
	switch (progStatus.agc_level) {
		case 0 : cmd[1] = '1'; break;
		case 1 : cmd[1] = '2'; break;
		case 2 : cmd[1] = '3'; break;
	}
	sendCommand(cmd, 0, true);
}

void RIG_TT550::set_cw_wpm()
{
	cmd = TT550setCWWPM;
	int ditfactor = (int)(0.50/progStatus.cw_wpm*0.4166*0.0001667);
	int spcfactor = ditfactor;
	int dahfactor = ditfactor * 3;
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
	cmd = TT550setCWMONVOL;
	cmd[1] = (0x3F) & (int)(progStatus.cw_vol * 0.63);
	sendCommand(cmd, 0, true);
}

void RIG_TT550::set_cw_spot()
{
	cmd = TT550setCWSPOTLVL;
	cmd[1] = (0xFF) & (int)(progStatus.cw_spot * 2.55);
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
	cmd[2] = (0xFF) & (int)(progStatus.vox_gain * 2.55);
	sendCommand(cmd, 0, true);
}

void RIG_TT550::set_vox_anti()
{
	cmd = TT550setANTIVOX;
	cmd[2] = (0xFF) & (int)(progStatus.vox_anti * 2.55);
	sendCommand(cmd, 0, true);
}

void RIG_TT550::set_vox_hang()
{
	cmd = TT550setVOXHANG;
	cmd[2] = (0xFF) & (int)(progStatus.vox_hang * 2.55);
	sendCommand(cmd, 0, true);
}

void RIG_TT550::set_compression()
{
	cmd = TT550setSPEECH;
	cmd[1] = (0x7F) & (int)(progStatus.compression * 1.27);
	sendCommand(cmd, 0, true);
}
