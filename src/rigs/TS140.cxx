/*
 * TS140 drivers
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */

#include "TS140.h"

const char TS140name_[] = "TS140";

const char *TS140modes_[] = {
		"LSB", "USB", "CW", "FM", "AM", "CWN", NULL};
static const char TS140_mode_type[] =
	{'L', 'U', 'U', 'U', 'U', 'U'};

RIG_TS140::RIG_TS140() {
// base class values	
	name_ = TS140name_;
	modes_ = TS140modes_;
	comm_baudrate = BR9600;
	stopbits = 2;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_rtscts = true;
	comm_rtsplus = false;
	comm_dtrplus = false;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	mode_ = 1;
	bw_ = 2;

	has_mode_control =
	has_ptt_control = true;

	has_attenuator_control =
	has_preamp_control =
	has_power_control =
	has_volume_control =
	has_bandwidth_control =
	has_micgain_control =
	has_notch_control =
	has_ifshift_control =
	has_tune_control =
	has_swr_control = false;

}

/*
========================================================================
	frequency & mode data are contained in the IF; response
		IFaaaaaaaaaaaXXXXXbbbbbcdXeefghjklmmX;
		12345678901234567890123456789012345678
		01234567890123456789012345678901234567 byte #
		where:
			aaaaaaaaaaa => decimal value of vfo frequency
			bbbbb => rit/xit frequency
			c => rit off/on
			d => xit off/on
			e => memory channel
			f => tx/rx
			g => mode
			h => function
			j => scan off/on
			k => split off /on
			l => tone off /on
			m => tone number
			X => unused characters
		 
	Test output from Minicom to IF; command		 

	IF00014070000	   -00300	 000200;

	0001000 is vfoA in LSB
	0002000 is vfoA in USB
	0003000 CW
	0004000 FM
	0005000 AM
	0007000 CWN	(dont have narrow filter however)
	0002100 VFOB in USB
	0002001 VFOA in USB SPILT
	0012000 PTT on in USB
========================================================================
*/ 

long RIG_TS140::get_vfoA ()
{
	cmd = "IF;";
	if (sendCommand(cmd, 38, false)) {
		long f = 0;
		for (size_t n = 2; n < 13; n++)
			f = f*10 + replybuff[n] - '0';
		freq_ = f;
	}
	return freq_;
}

void RIG_TS140::set_vfoA (long freq)
{
	freq_ = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd, 0, false);
}

// Tranceiver PTT on/off
void RIG_TS140::set_PTT_control(int val)
{
	if (val) sendCommand("TX;", 0, false);
	else	 sendCommand("RX;", 0, false);
}

int RIG_TS140::get_modetype(int n)
{
	return TS140_mode_type[n];
}

void RIG_TS140::set_mode(int val)
{
	if (val == 5) val++;
	cmd = "MD0;";
	cmd[2] = '1' + (val % 10);
	sendCommand(cmd, 0, false);
}

int RIG_TS140::get_mode()
{
	mode_ = 0;
	if (sendCommand("IF;", 38, false)) {
		int md = replybuff[29] - '1';
		if (md < 0) md = 0;
		if (md > 5) md = 5;
		mode_ = md;
	}
	return mode_;
}


/*
int RIG_TS140::get_smeter()
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

int RIG_TS140::get_swr()
{
	cmd = "RM6;";
	if (sendCommand(cmd,7)) {
		replybuff[6] = 0;
		int mtr = atoi(&replybuff[3]);
		return mtr;
	}
	return 0;
}

int RIG_TS140::get_power_out()
{
	cmd = "RM5;";
	if (sendCommand(cmd,7)) {
		replybuff[6] = 0;
		int mtr = atoi(&replybuff[3]);
		return mtr;
	}
	return 0;
}

int RIG_TS140::get_power_control()
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
void RIG_TS140::set_power_control(double val)
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
void RIG_TS140::set_volume_control(double val) 
{
	int ivol = (int)(val * 255);
	cmd = "AG0000;";
	for (int i = 5; i > 2; i--) {
		cmd[i] += ivol % 10;
		ivol /= 10;
	}
	sendCommand(cmd,0);
}

void RIG_TS140::tune_rig()
{
	sendCommand("AC002;",0);
}

void RIG_TS140::set_bandwidth(int val)
{
	switch (val) {
		case 0 : sendCommand("SH000;",0); break;
		case 1 : sendCommand("SH016;",0); break;
		case 2 : sendCommand("SH031;",0); break;
		default: sendCommand("SH031;",0);
	}
}

void RIG_TS140::set_attenuator(int val)
{
	if (val) sendCommand("RA01;", 0, false);
	else	 sendCommand("RA00;", 0, false);
}

int RIG_TS140::get_attenuator()
{
	if (sendCommand("RA0", 5, false))
		return (replybuff[3] == '1' ? 1 : 0);
   return 0;
}

void RIG_TS140::set_preamp(int val)
{
	if (val) sendCommand("PA01;", 0, false);
	else	 sendCommand("PA00;", 0, false);
}

int RIG_TS140::get_preamp()
{
	if (sendCommand("PA0", 5, false))
		return (replybuff[3] == '1' ? 1 : 0);
	return 0;
}

*/
