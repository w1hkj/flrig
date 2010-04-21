/*
 * K3 drivers
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */

#include "K3.h"

const char K3name_[] = "K3";

const char *K3modes_[] = 
	{ "LSB", "USB", "CW", "FM", "AM", "DATA", "CW-R", "DATA-R", NULL};
const char modenbr[] = 
	{ '1', '2', '3', '4', '5', '6', '7', '9' };
static const char K3_mode_type[] =
	{ 'L', 'U', 'U', 'U', 'U', 'L', 'L', 'L' };

const char *K3_widths[] = {"FL-1", "FL-2", "FL-3", "FL-4", NULL};

RIG_K3::RIG_K3() {
// base class values	
	name_ = K3name_;
	modes_ = K3modes_;
	bandwidths_ = K3_widths;
	comm_baudrate = BR38400;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = false;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	mode_ = 1;
	bw_ = 2;

	has_power_control =
	has_volume_control =
	has_mode_control =
	has_ptt_control =
	has_attenuator_control =
	has_preamp_control = true;

	has_micgain_control =
	has_bandwidth_control =
	has_notch_control =
	has_ifshift_control =
	has_tune_control =
	has_swr_control = false;

}

void RIG_K3::initialize()
{
	cmd = "K31;";
	sendCommand(cmd, 0, false);
}

long RIG_K3::get_vfoA ()
{
	cmd = "FA;";
	if (sendCommand(cmd, 14, false)) {
		long f = 0;
		for (size_t n = 2; n < 13; n++)
			f = f*10 + replybuff[n] - '0';
		freq_ = f;
	}
	return freq_;
}

void RIG_K3::set_vfoA (long freq)
{
	freq_ = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd, 0, false);
}

// Volume control
void RIG_K3::set_volume_control(int val) 
{
	int ivol = (int)(val * 2.55);
	cmd = "AG000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ivol % 10;
		ivol /= 10;
	}
	sendCommand(cmd, 0, false);
}

int RIG_K3::get_volume_control()
{
	cmd = "AG;";
	sendCommand(cmd, 6, false);
	replybuff[5] = 0;
	int v = atoi(&replybuff[2]);
	return (int)(v / 2.55);
}

void RIG_K3::set_mode(int val)
{
	cmd = "MD0;";
	cmd[2] = modenbr[val];
	sendCommand(cmd, 0, false);
}

int RIG_K3::get_mode()
{
	sendCommand("MD;", 4, false);
	int md = replybuff[2] - '1';
	if (md == 8) md--;
	return md;
}

int RIG_K3::get_modetype(int n)
{
	return K3_mode_type[n];
}

void RIG_K3::set_preamp(int val)
{
	if (val) sendCommand("PA1;", 0, false);
	else	 sendCommand("PA0;", 0, false);
}

int RIG_K3::get_preamp()
{
	if (sendCommand("PA;", 4, false))
		return (replybuff[2] == '1' ? 1 : 0);
	return 0;
}

//
void RIG_K3::set_attenuator(int val)
{
	if (val) sendCommand("RA01;", 0, false);
	else	 sendCommand("RA00;", 0, false);
}

int RIG_K3::get_attenuator()
{
	if (sendCommand("RA;", 5, false))
		return (replybuff[3] == '1' ? 1 : 0);
   return 0;
}

// Transceiver power level
void RIG_K3::set_power_control(double val)
{
	int ival = (int)val;
	cmd = "PC000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ival % 10;
		ival /= 10;
	}
	sendCommand(cmd, 0, false);
}

void RIG_K3::get_pc_min_max_step(int &min, int &max, int &step)
{
   min = 0; max = 120; step = 1; 
}

// Tranceiver PTT on/off
void RIG_K3::set_PTT_control(int val)
{
	if (val) sendCommand("TX;", 0, false);
	else	 sendCommand("RX;", 0, false);
}

//BG (Bargraph Read; GET only)
//RSP format: BGnn; where <nn> is 00 (no bars) through 10 (bar 10) if the bargraph is in DOT mode, and 12 (no
//bars) through 22 (all 10 bars) if the bargraph is in BAR mode. Reads the S-meter level on receive. Reads the
//power output level or ALC level on transmit, depending on the RF/ALC selection. Also see SM/SM$ command,
//which can read either main or sub RX S-meter level.

//SM $ (S-meter Read; GET only)
//Basic RSP format: SMnnnn; where nnnn is 0000-0015. S9=6; S9+20=9; S9+40=12; S9+60=15.
//K3 Extended RSP format (K31): nnnn is 0000-0021. S9=9; S9+20=13; S9+40=17; S9+60=21.
//This command can be used to obtain either the main (SM) or sub (SM$) S-meter readings. Returns 0000 in transmit
//mode. BG can be used to simply emulate the bar graph level, and applies to either RX or TX mode.

int RIG_K3::get_smeter()
{
	cmd = "SM;";
	if(sendCommand(cmd, 7, false)) {
		replybuff[6] = 0;
		int mtr = atoi(&replybuff[3]);
		if (mtr <= 6) mtr = (int) (50.0 * mtr / 6.0);
		else mtr = (int)(50 + (mtr - 6.0) * 50.0 / 9.0);
		return mtr;
	}
	return 0;
}

void RIG_K3::set_noise(bool on)
{
	if (on) sendCommand("NB1;", 0, false);
	else	sendCommand("NB0;", 0, false);
}

//FW $ (Filter Bandwidth and Number; GET/SET)
//K3 Extended SET/RSP format (K31): FWxxxx; where xxxx is 0-9999, the bandwidth in 10-Hz units. May be
//quantized and/or range limited based on the present operating mode.
//Basic and K2 Extended formats: See KIO2 Programmer’s Reference (K2). In K22 mode, direct selection of
//crystal filters is possible by adding a 5th digit. However, K31 must not be in effect, or it will override the legacy K2
//behavior and only allow direct bandwidth selection. For example, you could send K30;K22;FW00003;K20;K31;
//to select filter #3 and then restore the original K2 and K3 command modes (yours may be different).
//Notes: (1) In AI2/3 modes, moving the physical WIDTH control results in both FW and IS responses (width and
//shift). (2) In diversity mode, FW matches the sub receiver’s filter bandwidth to the main receiver’s, which may
//result in the generation of FA/FB/FR/FT responses. (3) Both FW and FW$ can be used in BSET mode (one
//exception: at present, FW/FW$ SET can’t be used in BSET mode with diversity receive in effect). (4) In K22
//mode, a legacy 6th digit is added to the response. It is always 0. In the K2, it indicated audio filter on/off status.

void RIG_K3::set_bandwidth(int val)
{
	bw_ = val;
	cmd = "K30;K22;FW0000x;K20;K31;";
	cmd[14] = '0' + val;
	sendCommand(cmd, 0, false);
}

int RIG_K3::get_bandwidth()
{
	cmd = "K30;K22;FW;K20;K31;";
	sendCommand(cmd, 9, false);
	bw_ = replybuff[5] - '0';
	return bw_;
}

//int RIG_K3::get_swr()
//{
//}

int RIG_K3::get_power_out()
{
	cmd = "PC;";
	if (sendCommand(cmd, 6, false)) {
		replybuff[5] = 0;
		int mtr = atoi(&replybuff[2]);
		return mtr;
	}
	return 0;
}

//void RIG_K3::tune_rig()
//{
//}


