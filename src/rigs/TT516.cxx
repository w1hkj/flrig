/*
 * TenTec ArgoV (TT516) drivers
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */

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
static const char TT516mode_chr[] =  { '0', '1', '2', '3', '4' };
static const char TT516mode_type[] = { 'U', 'U', 'L', 'U', 'U' };

static const char *TT516_widths[] = {
"200", "250", "300", "350", "400", "450", "500", "550", "600", "650",
"700", "750", "800", "850", "900", "950", "1000", "1100", "1200", "1300",
"1400", "1500", "1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300",
"2400", "2500", "2600", "2700", "2800", "2900", "3000", NULL};

static const char *TT516_AM_widths[] = {
"400", "500", "600", "700", "800", "900", "1000", "1100", "1200", "1350",
"1400", "1500", "1600", "1700", "1800", "1900", "2000", "2200", "2400", "2600",
"2800", "3000", "3200", "3400", "3600", "3800", "4000", "4200", "4400", "4600",
"4800", "5000", "5200", "5400", "5600", "5800", "6000", NULL};

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

RIG_TT516::RIG_TT516() {
// base class values	
	name_ = TT516name_;
	modes_ = TT516modes_;
	bandwidths_ = TT516_widths;
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
	
	modeA = 3;
	A.iBW = 36;
	def_mode = 3;
	defbw_ = 36;
	deffreq_ = 14070000;
	max_power = 25;

	has_power_control =
	has_volume_control =
	has_micgain_control =
	has_notch_control =
	has_preamp_control =
	has_tune_control =
	has_swr_control = false;

	has_noise_control =
	has_attenuator_control =
	has_ifshift_control =
	has_ptt_control =
	has_bandwidth_control =
	has_mode_control = true;

}

void RIG_TT516::checkresponse()
{
	if (RigSerial.IsOpen() == false)
		return;
	if (replybuff[0] == 'G')
		return;
	LOG_ERROR("\nsent  %s\nreply %s",
		str2hex(cmd.c_str(), cmd.length()),
		str2hex((char *)replybuff, strlen((char *)replybuff)));
}

void RIG_TT516::showresponse()
{
	LOG_INFO("%s", str2hex((char *)replybuff, strlen((char *)replybuff)));
}

long RIG_TT516::get_vfoA ()
{
	cmd = TT516getFREQA;
	bool ret = sendCommand(cmd, 8, true);
	if (ret == true && replybuff[0] == 'A') {
		int f = 0;
		for (size_t n = 1; n < 5; n++) {
			f = f*256 + (unsigned char)replybuff[n];
			A.freq = f;
		}
	} else
		checkresponse();
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
	sendCommand(cmd, 2, true);
	checkresponse();
	return;
}

long RIG_TT516::get_vfoB ()
{
	cmd = TT516getFREQB;
	bool ret = sendCommand(cmd, 8, true);
	if (ret == true && replybuff[0] == 'B') {
		int f = 0;
		for (size_t n = 1; n < 5; n++) {
			f = f*256 + (unsigned char)replybuff[n];
			B.freq = f;
		}
	} else
		checkresponse();
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
	sendCommand(cmd, 2, true);
	checkresponse();
	return;
}

void RIG_TT516::selectA()
{
	cmd = TT516setVfo;
	cmd[3] = 'A';
	sendCommand(cmd, 2, true);
	checkresponse();
	inuse = onA;
	set_bwA(A.iBW);
	return;
}

void RIG_TT516::selectB()
{
	cmd = TT516setVfo;
	cmd[3] = 'B';
	sendCommand(cmd, 2, true);
	checkresponse();
	inuse = onB;
	set_bwB(B.iBW);
	return;
}

void RIG_TT516::set_split(bool val)
{
	cmd = TT516setSPLIT;
	cmd[2] = val ? '\x01' : '\x00';
	sendCommand(cmd, 2, true);
	checkresponse();
	return;
}

// Tranceiver PTT on/off
void RIG_TT516::set_PTT_control(int val)
{
	if (val) sendCommand(TT516setXMT, 2, true);
	else     sendCommand(TT516setRCV, 2, true);
	checkresponse();
}

int RIG_TT516::get_modetype(int n)
{
	return TT516mode_type[n];
}

int RIG_TT516::adjust_bandwidth(int m)
{
	if (m == 0) { // AM
		bandwidths_ = TT516_AM_widths;
		inuse == onA ? A.iBW = 36 : B.iBW = 36;
	} else {
		bandwidths_ = TT516_widths;
		if (m == 3) inuse == onA ? A.iBW = 12 : B.iBW = 12;
		else inuse == onA ? A.iBW = 36 : B.iBW = 36;
	}
	return inuse == onA ? A.iBW : B.iBW;
}

const char **RIG_TT516::bwtable(int m)
{
	if (m == 0) return TT516_AM_widths;
	return TT516_widths;
}

void RIG_TT516::set_modeA(int val)
{
	A.imode = val;
	cmd = TT516setMODE;
	cmd[2] = TT516mode_chr[A.imode];
	cmd[3] = TT516mode_chr[B.imode];
	sendCommand(cmd, 2, true);
	checkresponse();
}

int RIG_TT516::get_modeA()
{
	cmd = TT516getMODE;
	sendCommand(cmd, 6, true);
	if (replybuff[0] == 'M') {
		A.imode = replybuff[1] - '0';
	}
	return A.imode;
}

void RIG_TT516::set_modeB(int val)
{
	B.imode = val;
	cmd = TT516setMODE;
	cmd[2] = TT516mode_chr[A.imode];
	cmd[3] = TT516mode_chr[B.imode];
	sendCommand(cmd, 2, true);
	checkresponse();
}

int RIG_TT516::get_modeB()
{
	cmd = TT516getMODE;
	sendCommand(cmd, 6, true);
	if (replybuff[0] == 'M') {
		B.imode = replybuff[2] - '0';
	}
	return B.imode;
}

int RIG_TT516::get_bwA()
{
	if (inuse == onA) {
		cmd = TT516getBW;
		sendCommand(cmd, 5, true);
		if (replybuff[0] == 'W')
			A.iBW = (unsigned char)replybuff[1];
	}
	return A.iBW;
}

void RIG_TT516::set_bwA(int val)
{
	A.iBW = val;
	if (inuse == onA) {
		cmd = TT516setBW;
		cmd[2] = val;
		sendCommand(cmd, 2, true);
		checkresponse();
	}
}

int RIG_TT516::get_bwB()
{
	if (inuse == onB) {
		cmd = TT516getBW;
		sendCommand(cmd, 5, true);
		if (replybuff[0] == 'W')
			B.iBW = (unsigned char)replybuff[1];
	}
	return B.iBW;
}

void RIG_TT516::set_bwB(int val)
{
	B.iBW = val;
	if (inuse == onB) {
		cmd = TT516setBW;
		cmd[2] = val;
		sendCommand(cmd, 2, true);
		checkresponse();
	}
}

void RIG_TT516::set_if_shift(int val)
{
	cmd = TT516setPBT;
	short int si = val;
	cmd[2] = (si & 0xff00) >> 8;
	cmd[3] = (si & 0xff);
	sendCommand(cmd, 2, true);
	checkresponse();
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
	sendCommand(cmd, 2, true);
	checkresponse();
}


int RIG_TT516::get_attenuator()
{
	cmd = TT516getATT;
	sendCommand(cmd, 5, true);
	if (replybuff[0] == 'J' && replybuff[1] == '1')
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
	sendCommand(cmd, 2, true);
}

int RIG_TT516::get_smeter()
{
	double sig = 0.0;
	cmd = TT516getSMETER;
	sendCommand(cmd, 6, true);
	if (replybuff[0] == 'S') {
		sig = (50.0 / 9.0) * ((unsigned char)replybuff[1] + (unsigned char)replybuff[2] / 256.0);
	}
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
	sendCommand(cmd, 5, true);
	if (replybuff[0] == 'F') {
		fwdv = 1.0 * (unsigned char)replybuff[1];
		cmd = TT516getREFPWR;
		sendCommand(cmd, 5, true);
		if (replybuff[0] == 'R')
			refv = 1.0 * (unsigned char)replybuff[1];
	}
	fwdpwr = 30.0 * (fwdv * fwdv) / (256 * 256);
	refpwr = 30.0 * (refv * refv) / (256 * 256);
	return fwdpwr;
}
