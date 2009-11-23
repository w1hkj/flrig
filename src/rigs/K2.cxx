/*
 * K2 drivers
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */

#include "K2.h"

const char K2name_[] = "K2";

const char *K2modes_[] = {
		"LSB", "USB", "CW", "FM", "AM", "RTTY-L", "CW-R", "USER-L", "RTTY-U", "FM-N", "USER-U", NULL};
static const char K2_mode_type[] =
	{'L', 'U', 'U', 'U', 'U', 'L', 'L', 'L', 'U', 'U', 'U'};

const char *K2_widths[] = {"NARR", "NORM", "WIDE", NULL};

RIG_K2::RIG_K2() {
// base class values	
	name_ = K2name_;
	modes_ = K2modes_;
	bandwidths_ = K2_widths;
	comm_baudrate = BR38400;
	stopbits = 2;
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

	has_attenuator_control =
	has_preamp_control = true;

	has_power_control =
	has_volume_control =
	has_mode_control =
	has_bandwidth_control =
	has_micgain_control =
	has_notch_control =
	has_ifshift_control =
	has_ptt_control =
	has_tune_control =
	has_swr_control = false;

}

long RIG_K2::get_vfoA ()
{
	cmd = "FA;";
	if (sendCommand(cmd, 11, false)) {
		long f = 0;
		for (size_t n = 2; n < 13; n++)
			f = f*10 + replybuff[n] - '0';
		freq_ = f;
	}
	return freq_;
}

void RIG_K2::set_vfoA (long freq)
{
	freq_ = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd, 0, false);
}

void RIG_K2::set_attenuator(int val)
{
	if (val) sendCommand("RA01;", 0, false);
	else	 sendCommand("RA00;", 0, false);
}

int RIG_K2::get_attenuator()
{
	if (sendCommand("RA0", 5, false))
		return (replybuff[3] == '1' ? 1 : 0);
   return 0;
}

void RIG_K2::set_preamp(int val)
{
	if (val) sendCommand("PA01;", 0, false);
	else	 sendCommand("PA00;", 0, false);
}

int RIG_K2::get_preamp()
{
	if (sendCommand("PA0;", 5, false))
		return (replybuff[3] == '1' ? 1 : 0);
	return 0;
}

/*
int RIG_K2::get_smeter()
{
	cmd = "SM0;";
	if(sendCommand(cmd, 7)) {
		replybuff[6] = 0;
		int mtr = atoi(&replybuff[3]);
		mtr = mtr * 100.0 / 256.0 - 128.0;
		return mtr;
	}
	return 0;
}

int RIG_K2::get_swr()
{
	cmd = "RM6;";
	if (sendCommand(cmd,7)) {
		replybuff[6] = 0;
		int mtr = atoi(&replybuff[3]);
		return mtr;
	}
	return 0;
}

int RIG_K2::get_power_out()
{
	cmd = "RM5;";
	if (sendCommand(cmd,7)) {
		replybuff[6] = 0;
		int mtr = atoi(&replybuff[3]);
		return mtr;
	}
	return 0;
}

int RIG_K2::get_power_control()
{
	cmd = "PC;";
	if (sendCommand(cmd,6)) {
		replybuff[5] = 0;
		int mtr = atoi(&replybuff[2]);
		return (mtr * 255 / 150);
	}
	return 0;
}

// Transceiver power level
void RIG_K2::set_power_control(double val)
{
	int ival = (int)val * 150 / 255;
	cmd = "PC000;";
	for (int i = 4; i > 1; i--) {
		cmd[i] += ival % 10;
		ival /= 10;
	}
	sendCommand(cmd,0);
}

// Volume control
void RIG_K2::set_volume_control(double val) 
{
	int ivol = (int)(val * 255);
	cmd = "AG0000;";
	for (int i = 5; i > 2; i--) {
		cmd[i] += ivol % 10;
		ivol /= 10;
	}
	sendCommand(cmd,0);
}

// Tranceiver PTT on/off
void RIG_K2::set_PTT_control(int val)
{
	if (val) sendCommand("TX;",0);
	else	 sendCommand("RX;",0);
}

void RIG_K2::tune_rig()
{
	sendCommand("AC002;",0);
}

void RIG_K2::set_mode(int val)
{
	val++;
	if (val > 9) val++;
	cmd = "MD00;";
	cmd[3] += (val % 10);
	val /= 10;
	cmd[2] += (val % 10);
	sendCommand(cmd,0);
}

int RIG_K2::get_mode()
{
	sendCommand("MD0;",5);
	int md = replybuff[3] - '0';
	if (md > 10) md--;
	md--;
	return md;
}

void RIG_K2::set_bandwidth(int val)
{
	switch (val) {
		case 0 : sendCommand("SH000;",0); break;
		case 1 : sendCommand("SH016;",0); break;
		case 2 : sendCommand("SH031;",0); break;
		default: sendCommand("SH031;",0);
	}
}

int RIG_K2::get_modetype(int n)
{
	return K2_mode_type[n];
}

*/
