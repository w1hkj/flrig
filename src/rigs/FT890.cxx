/*
 * Yaesu FT-890 drivers
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */

// a work in progress

#include "FT890.h"

const char FT890name_[] = "FT-890";

const char *FT890modes_[] = {
		"LSB", "USB", "CW", "CW-N", "AM", "AM-N", "FM", NULL};
static const int FT890_mode_val[] =  { 0, 1, 2, 3, 4, 5, 6 };
static const char FT890_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'U', 'U' };

RIG RIG_FT890 = {
	FT890,
	FT890name_,
	FT890modes_,
	NULL, // FT890_widths,
	BR4800,			// comm_baudrate
	2,				// stopbits
	2,				 // comm_retries
	5,				 // comm_wait
	50,				// comm_timeout
	false,			 // comm_rtscts;
	false,			 // comm_rtsplus;
	true,			  // comm_dtrplus;
	true,			  // comm_catptt;
	false,			 // comm_rtsptt;
	false,			 // comm_dtrptt;
	1,
	0,
	
	FT890_init,
	FT890_get_vfoA, 
	FT890_set_vfoA, 
	NULL, // FT890_get_smeter, 
	NULL, // FT890_get_swr,
	NULL, // FT890_get_power_out, 
	NULL, // FT890_get_power_control,
	NULL, // FT890_set_volume_control,
	NULL, // FT890_set_power_control,
	FT890_set_PTT_control,
	FT890_tune_rig,
	NULL, // FT890_set_attenuator,
	NULL, // FT890_get_attenuator,
	NULL, // FT890_set_preamp,
	NULL, // FT890_get_preamp,
	FT890_set_modeA,
	FT890_get_modeA,
	NULL, // FT890_get_modetype,
	NULL, // FT890_set_bwA,
	NULL, // adjust_bandwidth,
	NULL, // FT890_get_bwA,
	NULL, // FT890_set_if_shift,
	NULL, // FT890_get_if_shift,
	NULL, // FT890_get_if_min_max_step,
	NULL, // FT890_set_notch,
	NULL, // get_notch
	NULL, // FT890_get_notch_min_max_step,
	NULL, // FT890_set_mic_gain,
	NULL, // FT890_get_mic_gain

	has_power_control =
	has_tune_control =
	has_attenuator_control =
	has_preamp_control = true;

	has_volume_control =
	has_mode_control =
	has_bandwidth_control =
	has_micgain_control =
	has_notch_control =
	has_ifshift_control =
	has_ptt_control =
	has_swr_control = false;

};

// use init_cmd() from the FT817 code structure

void FT890_init()
{
}

int currmode = 0;

long FT890_get_vfoA ()
{
	init_cmd();
	cmd[3] = 3;
	cmd[4] = 0x10;

	if (sendCommand(cmd, 18)) {
		long f = 0;
// vfo value is in bytes 1..3; binary MSB in byte 1
		for (size_t n = 1; n < 4; n++) {
			f = f*256 + replybuff[n];
		}
		freqA = f * 10; // 890 resolution is 10 Hz
// interpret mode byte
		int md = replybuff[6];
		int flg = replybuff[8] & 0xC0; // bits 6 & 7
		switch (md) {
			case 0: modeA = 0; break;
			case 1: modeA = 1; break;
			case 2: modeA = 2;
					if (flg && 0x80 == 0x80) modeA = 3;
					break;
			case 3: currmode = 4;
					if (flg && 0x40 == 0x40) modeA = 5;
					break;
			case 4: modeA = 6; break;
		}
	}
	return freqA;
}

void FT890_set_vfoA (long freq)
{
	freqA = freq;
	init_cmd();
	freq /=10; // 890 does not support 1 Hz resolution
	for (size_t i = 0; i < 4; i++) {
		cmd[i] = freq % 10; freq /=10;
		cmd[i] += (freq % 10) << 4; freq /= 10;
	}
	cmd[4] = 0x0A;
	sendCommand(cmd, 0);
}

int FT890_get_modeA()
{
// combined with get_vfoA
// do not need a separate read for mode
	return modeA;
}

void FT890_set_modeA(int val)
{
	modeA = val;
	init_cmd();
	cmd[3] = FT890_mode_val[val];
	cmd[4] = 0x0C;
	sendCommand(cmd, 0);
}

// Tranceiver PTT on/off
void FT890_set_PTT_control(int val)
{
	init_cmd();
	if (val) cmd[3] = 1;
	cmd[4] = 0x0F;
	sendCommand(cmd, 0);
}

void FT890_tune_rig()
{
	init_cmd();
	cmd[3] = 1;
	cmd[4] = 0x81;
	sendCommand(cmd, 0);
	cmd[3] = 0;
	cmd[4]= 0x82;
	sendCommand(cmd, 0);
}

int FT890_get_smeter()
{
	init_cmd();
	cmd[4] = 0xF7;
	if (sendCommand(cmd, 5)) {
		return (replybuff[0] * 100.0 / 256.0 - 128.0);
	}
	return 0;
}

int FT890_get_power_out()
{
	init_cmd();
	cmd[4] = 0xF7;
	if (sendCommand(cmd, 5)) {
		return (replybuff[0]);
	}
	return 0;
}

// following commands not supported by FT890
/*
int FT890_get_swr()
{
	return 0;
}

int FT890_get_power_control()
{
	return 0;
}

// Volume control
void FT890_set_volume_control(double val) 
{
}

// Transceiver power level
void FT890_set_power_control(double val)
{
}

void FT890_set_attenuator(int val)
{
}

int FT890_get_attenuator()
{
}

void FT890_set_preamp(int val)
{
}

int FT890_get_preamp()
{
}


void FT890_set_bwA(int val)
{
}

int FT890_get_bwA()
{
	return 0;
}

int FT890_get_modetype(int n)
{
	return 0;
}

void FT890_set_if_shift(int val)
{
}

int FT890_get_if_shift()
{
}

void FT890_get_if_min_max_step(int &min, int &max, int &step)
{
	min = -1000;
	max = 1000;
	step = 100;
}

static bool notch_on = false;

void FT890_set_notch(bool on, int val)
{
}

int  FT890_get_notch()
{
}

void FT890_get_notch_min_max_step(int &min, int &max, int &step)
{
	min = -1800;
	max = +1800;
	step = 10;
}

void FT890_set_mic_gain(int val)
{
}

int FT890_get_mic_gain()
{
	return 0;
}

void FT890_get_mic_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 255;
	step = 1;
}
*/
