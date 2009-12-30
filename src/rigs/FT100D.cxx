/*
 * Yaesu FT-100D drivers
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */

#include "FT100D.h"

static const char FT100Dname_[] = "FT-100D";
static const char *FT100Dmodes_[] = 
{ "LSB", "USB", "CW", "CW-R", "AM", "DIG", "FM", "W-FM", NULL};
static const int FT100D_mode_val[] =
{ 0, 1, 2, 3, 4, 5, 6, 7 };
static const char FT100D_mode_type[] =
{ 'L', 'U', 'U', 'L', 'U', 'U', 'U', 'U' };
static const char *FT100Dwidths_[] =
{ "300", "500", "2400", "6000", NULL};
static const int FT100D_bw_val[] =
{ 0, 1, 2, 3 };

RIG_FT100D::RIG_FT100D() {
	name_ = FT100Dname_;
	modes_ = FT100Dmodes_;
	bandwidths_ = FT100Dwidths_;
	comm_baudrate = BR4800;
	stopbits = 2;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	mode_ = 1;
	bw_ = 2;

	has_mode_control =
	has_bandwidth_control =
	has_ptt_control = true;

}

void RIG_FT100D::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

long RIG_FT100D::get_vfoA ()
{
	init_cmd();
	cmd[4] = 0x10;

	if (sendCommand(cmd, 32)) {
		long f = 0;
// vfo value is in bytes 1..3; binary MSB in byte 1
		for (size_t n = 1; n < 5; n++)
			f = f * 256 + (unsigned char)replybuff[n];
		freq_ = f * 1.25; // 100D resolution is 1.25 Hz / bit for read
// interpret mode bits
		mode_ = replybuff[5] & 0x0F;
// interpret bandwidth bits
		int bw =  (replybuff[5] >> 4) & 0x03;
		if (bw == 0) bw_ = 3;
		else if (bw == 1) bw_ = 2;
		else if (bw == 2) bw_ = 1;
		else if (bw == 3) bw_ = 0;
	}
	return freq_;
}

void RIG_FT100D::set_vfoA (long freq)
{
	freq_ = freq;
	freq /=10; // 100D does not support 1 Hz resolution
	cmd = to_bcd_be(freq, 8);
	cmd += 0x0A;
	sendCommand(cmd, 0);
}

int RIG_FT100D::get_mode()
{
// combined with get_vfoA
// do not need a separate read for mode
	return mode_;
}

void RIG_FT100D::set_mode(int val)
{
	mode_ = val;
	init_cmd();
	cmd[3] = FT100D_mode_val[val];
	cmd[4] = 0x0C;
	sendCommand(cmd, 0);
}

// Tranceiver PTT on/off
void RIG_FT100D::set_PTT_control(int val)
{
	init_cmd();
	if (val) cmd[3] = 1;
	cmd[4] = 0x0F;
	sendCommand(cmd, 0);
}

void RIG_FT100D::set_bandwidth (int val)
{
	init_cmd();
	cmd[3] = FT100D_bw_val[val];
	cmd[4] = 0x8C;
	sendCommand(cmd, 0);
}

int RIG_FT100D::get_bandwidth()
{
	return bw_;
}

int RIG_FT100D::get_smeter()
{
	init_cmd();
	cmd[4] = 0xF7;
	sendCommand(cmd, 9);
	int sval = (200 -  (unsigned char)replybuff[3]) / 1.1;
	if (sval < 0) sval = 0;
	if (sval > 100) sval = 100;
	return sval;
}

int RIG_FT100D::get_swr()
{
	double swr = (fwdpwr + refpwr) / (fwdpwr - refpwr + .0001);
	swr -= 1.0;
	swr *= 25.0;
	if (swr < 0) swr = 0;
	if (swr > 100) swr = 100;
	return (int) swr;
}

int RIG_FT100D::get_power_out()
{
	init_cmd();
	cmd[4] = 0xF7;
	sendCommand(cmd, 9);
	fwdpwr = replybuff[1] / 2.56;
	refpwr = replybuff[2] / 2.56;
	return (int) fwdpwr;
}
