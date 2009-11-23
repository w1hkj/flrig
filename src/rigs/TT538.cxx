/*
 * TenTec Jupiter (TT538) drivers
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */

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

static const char *TT538modes_[] = {
		"AM", "USB", "LSB", "CW", "FM", NULL};
static const char TT538mode_chr[] =  { '0', '1', '2', '3', '4' };
static const char TT538mode_type[] = { 'U', 'U', 'L', 'U', 'U' };

// filter # is 33 - index
static const char *TT538_widths[] = {
 "300",  "330",  "375",  "450",  "525",  "600",  "675",  "750",  "900", "1050",
"1200", "1350", "1500", "1650", "1800", "1950", "2100", "2250", "2400", "2550",
"2700", "2850", "3000", "3300", "3600", "3900", "4200", "4500", "4800", "5100",
"5400", "5700", "6000", "8000", NULL};

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
//static char TT538getFWDPWR[]	= "?F\r";
//static char TT538getAGC[]		= "?G\r";
//static char TT538getSQLCH[]		= "?H\r";
//static char TT538getRF[]		= "?I\r";
static char TT538getATT[]		= "?J\r";
//static char TT538getNB[]		= "?K\r";
static char TT538getMODE[]		= "?M\r";
//static char TT538getPBT[]		= "?P\r";
static char TT538getSMETER[]	= "?S\r";
static char TT538getVOL[]		= "?U\r";
static char TT538getBW[]		= "?W\r";

//static char TT538getREFPWR[]	= "?R\r";
//static char TT538setXMT[]		= "#1\r";
//static char TT538setRCV[]		= "#0\r";

RIG_TT538::RIG_TT538() {
// base class values
	name_ = TT538name_;
	modes_ = TT538modes_;
	bandwidths_ = TT538_widths;
	comm_baudrate = BR57600;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 20;
	comm_timeout = 50;
	comm_rtscts = true;
	comm_rtsplus = false;
	comm_dtrplus = false;
	comm_catptt = false;
	comm_rtsptt = false;
	comm_dtrptt = false;
	serloop_timing = 200;

	mode_ = 1;
	bw_ = 30;
	def_mode = 3;
	defbw_ = 15;
	deffreq_ = 14070000;
	max_power = 100;

	has_power_control =
	has_micgain_control =
	has_notch_control =
	has_preamp_control =
	has_tune_control =
	has_noise_control =
	has_swr_control = false;

	has_volume_control =
	has_rf_control =
	has_attenuator_control =
	has_ifshift_control =
	has_ptt_control =
	has_bandwidth_control =
	has_mode_control = true;

}

void RIG_TT538::checkresponse()
{
	if (RigSerial.IsOpen() == false)
		return;
	if (replybuff[0] == 'G')
		return;
	LOG_ERROR("\nsent  %s\nreply %s",
		str2hex(cmd.c_str(), cmd.length()),
		str2hex((char *)replybuff, strlen((char *)replybuff)));
}

void RIG_TT538::showresponse()
{
	LOG_INFO("%s", str2hex((char *)replybuff, strlen((char *)replybuff)));
}

long RIG_TT538::get_vfoA ()
{
	cmd = TT538getFREQA;
	bool ret = sendCommand(cmd, 8, true);
	if (ret == true && replybuff[0] == 'A') {
		int f = 0;
		for (size_t n = 1; n < 5; n++) {
			f = f*256 + (unsigned char)replybuff[n];
		freq_ = f;
}
	} else
		checkresponse();
	return freq_;
}

void RIG_TT538::set_vfoA (long freq)
{
	freq_ = freq;
	cmd = TT538setFREQA;
	cmd[5] = freq & 0xff; freq = freq >> 8;
	cmd[4] = freq & 0xff; freq = freq >> 8;
	cmd[3] = freq & 0xff; freq = freq >> 8;
	cmd[2] = freq & 0xff;
	sendCommand(cmd, 2, true);
	checkresponse();
	return ;
}

void RIG_TT538::set_mode(int val)
{
	mode_ = val;
	cmd = TT538setMODE;
	cmd[2] = cmd[3] = TT538mode_chr[val];
	sendCommand(cmd, 2, true);
	checkresponse();
}

int RIG_TT538::get_mode()
{
	cmd = TT538getMODE;
	sendCommand(cmd, 6, true);
	if (replybuff[0] == 'M') {
		mode_ = replybuff[1] - '0';
	}
	return mode_;
}

int RIG_TT538::get_modetype(int n)
{
	return TT538mode_type[n];
}

void RIG_TT538::set_bandwidth(int val)
{
	bw_ = val;
	cmd = TT538setBW;
	cmd[2] = 33 - val;
	sendCommand(cmd, 2, true);
	checkresponse();
}

int RIG_TT538::get_bandwidth()
{
	cmd = TT538getBW;
	sendCommand(cmd, 5, true);
	if (replybuff[0] == 'W')
		bw_ = 33 - (unsigned char)replybuff[1];
	return bw_;
}

void RIG_TT538::set_if_shift(int val)
{
	cmd = TT538setPBT;
	short int si = val;
	cmd[2] = (si & 0xff00) >> 8;
	cmd[3] = (si & 0xff);
	sendCommand(cmd, 2, true);
	checkresponse();
}

bool RIG_TT538::get_if_shift(int &val)
{
	val = 0;
	return false;
}

void RIG_TT538::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -8000;
	max = 8000;
	step = 100;
}

void RIG_TT538::set_attenuator(int val)
{
	cmd = TT538setATT;
	if (val) cmd[2] = '1';
	else     cmd[2] = '0';
	sendCommand(cmd, 2, true);
	checkresponse();
}


int RIG_TT538::get_attenuator()
{
	cmd = TT538getATT;
	sendCommand(cmd, 5, true);
	if (replybuff[0] == 'J' && replybuff[1] == '1')
		return 1;
	return 0;
}

int RIG_TT538::get_smeter()
{
	double sig = 0.0;
	cmd = TT538getSMETER;
	sendCommand(cmd, 8, true);
	if (replybuff[0] == 'S') {
		char szval[]= "00.00";
		szval[0] = replybuff[1];
		szval[1] = replybuff[2];
		szval[3] = replybuff[3];
		szval[4] = replybuff[4];
		float sval = atof(szval);
		sig = (50.0 * sval / 9.0);
	}
	return (int)sig;
}

int RIG_TT538::get_volume_control()
{
	cmd = TT538getVOL;
	sendCommand(cmd, 5, true);
	if (replybuff[0] == 'U')
		return (int)(replybuff[1] / 1.27);
	return 0;
}

void RIG_TT538::set_volume_control(int vol)
{
	cmd = TT538setVOL;
	cmd[2] = 0x7F & (int)(vol * 1.27);
	sendCommand(cmd, 2, true);
}

void RIG_TT538::set_rf_gain(int val)
{
	cmd = TT538setRF;
	cmd[2] = 0x7F & (int)(val * 1.27);
	sendCommand(cmd, 2, true);
}

int  RIG_TT538::get_rf_gain()
{
	cmd = TT538setRF;
	sendCommand(cmd, 5, true);
	if (replybuff[0] == 'I')
		return (int)(replybuff[1] / 1.27);
	return 0;
}

////////////////////////////////////////////////////////////////////////
//               NOT IMPLEMENTED IN JUPITER ONLY MODE                 //
////////////////////////////////////////////////////////////////////////
/*
void RIG_TT538::set_noise(bool b)
{
	cmd = TT538setNB;
	if (b)
		cmd[2] = '4';
	else
		cmd[2] = '0';
	sendCommand(cmd, 2, true);
}

int RIG_TT538::get_power_out()
{
	fwdpwr = refpwr = fwdv = refv = 0;
	cmd = TT538getFWDPWR;
	sendCommand(cmd, 5, true);
	if (replybuff[0] == 'F') {
		fwdv = 1.0 * (unsigned char)replybuff[1];
		cmd = TT538getREFPWR;
		sendCommand(cmd, 5, true);
		if (replybuff[0] == 'R')
			refv = 1.0 * (unsigned char)replybuff[1];
	}
	fwdpwr = 30.0 * (fwdv * fwdv) / (256 * 256);
	refpwr = 30.0 * (refv * refv) / (256 * 256);
	return fwdpwr;
}

int RIG_TT538::get_swr()
{
	double swr = (fwdv + refv) / (fwdv - refv + .0001);
	swr -= 1.0;
	swr *= 25.0;
	if (swr < 0) swr = 0;
	if (swr > 100) swr = 100;
	return (int)swr;
}

// Tranceiver PTT on/off
void RIG_TT538::set_PTT_control(int val)
{
	if (val) sendCommand(TT538setXMT, 2, true);
	else     sendCommand(TT538setRCV, 2, true);
	checkresponse();
}

*/
