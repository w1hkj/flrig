/*
 * K2 drivers
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 *
 * Additions by Kent Haase, VE4KEH
 * Ref Elecraft KIO2 Programmer's Reference (Rev E, 3 Feb, 2004)
 * K2 MCU firmware rev 2.04
 * 
 */
 
 #include "K2.h"

const char K2name_[] = "K2";

const char *K2modes_[] = {
		"LSB", "USB", "CW", "FM", "AM", "RTTY-L", "CW-R", "USER-L", "RTTY-U", NULL};

static const char K2_mode_type[] =
	{'L', 'U', 'U', 'U', 'U', 'L', 'L', 'L', 'U', 'U'};

const char *K2_widths[] = {"FL1", "FL2", "FL3", "FL4", NULL};

RIG_K2::RIG_K2() {
// base class values	
	name_ = K2name_;
	modes_ = K2modes_;
	bandwidths_ = K2_widths;
	comm_baudrate = BR4800;
	stopbits = 2;
	comm_retries = 3;
	comm_wait = 50;
	comm_timeout = 200;
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = false;
	comm_catptt = false;
	comm_rtsptt = false;
	comm_dtrptt = true;
	mode_ = 1;
	bw_ = 0;

	has_attenuator_control = true;
	has_preamp_control = true;
	has_power_control = true;
	has_volume_control = false;
	has_mode_control = true;
	has_bandwidth_control = true;
	has_micgain_control = false;
	has_notch_control = false;
	has_ifshift_control = false;
	has_ptt_control = true;
	has_tune_control = true;
	has_swr_control = false;
}

void RIG_K2::initialize()
{
//enable extended command mode	
	sendCommand("K22;", 0, false);
//ensure K2 is in VFO A
	sendCommand("FR0;",0, false);
// get power output setting
    get_power_control();
}

long RIG_K2::get_vfoA ()
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
	if (sendCommand("RA;", 5, false))
		return (replybuff[3] == '1' ? 1 : 0);
   return 0;
}

void RIG_K2::set_preamp(int val)
{
	if (val) sendCommand("PA1;", 0, false);
	else	 sendCommand("PA0;", 0, false);
}

int RIG_K2::get_preamp()
{
	if (sendCommand("PA;", 4, false))
		return (replybuff[2] == '1' ? 1 : 0);
	return 0;
}

void RIG_K2::set_mode(int val)
{
	val++;
	cmd = "MD0;";
	cmd[2] += val;
	sendCommand(cmd, 0, false);
}

int RIG_K2::get_mode()
{
	sendCommand("MD;",4);
	int md = replybuff[2] - '0';
	md--;
	return md;
}

int RIG_K2::get_modetype(int n)
{
	return K2_mode_type[n];
}

void RIG_K2::set_bandwidth(int val)
{
	switch (val) {
		case 0 : sendCommand("FW00001;",0); break;
		case 1 : sendCommand("FW00002;",0); break;
		case 2 : sendCommand("FW00003;",0); break;
		case 3 : sendCommand("FW00004;",0); break;
		default: sendCommand("FW00001;",0);
	}
}

int RIG_K2::get_bandwidth()
{
	sendCommand("FW;",9);
	int bw = replybuff[6] - '0';
	bw --;
	return bw;
}

void RIG_K2::tune_rig()
{
	sendCommand("SW20;",0);
}

int RIG_K2::get_smeter()
{
	cmd = "BG;";
	if(sendCommand(cmd, 5)) {
		int mtr = 10 * (replybuff[2] - '0') + replybuff[3] - '0';   //assemble two chars into 2 digit int
		if (mtr > 10) {                                             //if int greater than 10 (bar mode)
		    mtr -= 12;                                              //shift down to 0 thru 10
		}
		mtr *= 10;                                                  //normalize to 0 thru 100
		return mtr;
	}
	return 0;
}

int RIG_K2::get_power_out()
{
	cmd = "BG;";
	if(sendCommand(cmd, 5)) {
		int mtr = 10 * (replybuff[2] - '0') + replybuff[3] - '0';   //assemble two chars into 2 digit int
		if (mtr > 10) {                                             //if int greater than 10 (bar mode)
		    mtr -= 12;                                              //shift down to 0 thru 10
		}
		mtr *= 10;                                                  //normalize to 0 thru 100
		return mtr;
	}
	return 0;
}

int RIG_K2::get_power_control()
{
	cmd = "PC;";                        //init the get string
	if (sendCommand(cmd,7)) {           //send it, if there's a response
	    int mtr = (replybuff[4] - '0'   //pwr is the least sig digit
	    + 10 * (replybuff[3] - '0')     //plus ten times the next sig digit
	    + 100 * (replybuff[2] - '0'));   //plus one hundred times the most sig digit
		return mtr;
	}
	return 0;                           //if no response, return nothing
}

void RIG_K2::set_power_control(double val)
{
    int ival = (int)val;
	cmd = "PC0001;";                    //init the cmd string, hi pwr range
	for (int i = 4; i > 1; i--) {       //select the char in the cmd string, least sig digit first
		cmd[i] += ival % 10;            //parse the digit, add it to ascii '0', assign to char
		ival /= 10;                     //shift the pwr over
	}
	sendCommand(cmd,0);
}

void RIG_K2::set_PTT_control(int val)
{
	if (val) sendCommand("TX;",0);
	else	 sendCommand("RX;",0);
}


