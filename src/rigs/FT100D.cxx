/*
 * Yaesu FT-100D drivers
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */

#include "FT100D.h"
#include "rig.h"

static const char FT100Dname_[] = "FT-100D";
static const char *FT100Dmodes_[] = 
{ "LSB", "USB", "CW", "CW-R", "AM", "DIG", "FM", "W-FM", NULL};

static const int FT100D_def_bw[] = { 2, 2, 1, 1, 3, 2, 2, 3 };

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
	afreq = bfreq = A.freq = B.freq = 14070000;
	amode = bmode = A.imode = B.imode = 1;
	aBW = bBW = A.iBW = B.iBW = 2;
	precision = 10;

//	has_get_info =
	has_mode_control =
	has_bandwidth_control =
	has_ptt_control = true;

}

int  RIG_FT100D::adjust_bandwidth(int m)
{
	if (m == 0 || m == 1 || m == 5) return 2;
	if (m == 2 || m == 3 ) return 1;
	if (m == 4 || m == 6 || m == 7) return 3;
	return 2;
}

void RIG_FT100D::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

void RIG_FT100D::initialize()
{
}

void RIG_FT100D::selectA()
{
	init_cmd();
	cmd[4] = 0x05;
	sendCommand(cmd, 0);
if (RIG_DEBUG)
	LOG_INFO("%s", str2hex(cmd.c_str(), 5));
}

void RIG_FT100D::selectB()
{
	init_cmd();
	cmd[3] = 0x01;
	cmd[4] = 0x05;
	sendCommand(cmd, 0);
if (RIG_DEBUG)
	LOG_INFO("%s", str2hex(cmd.c_str(), 5));
}

void RIG_FT100D::set_split(bool val)
{
	split = val;
	init_cmd();
	cmd[3] = val ? 0x01 : 0x00;
	cmd[4] = 0x01;
	sendCommand(cmd, 0);
if (RIG_DEBUG)
	LOG_INFO("%s", str2hex(cmd.c_str(), 5));
}


bool RIG_FT100D::get_info()
{
	int ret = 0;
	
	init_cmd();
	cmd[4] = 0x10;
	ret = sendCommand(cmd, 32);
if (RIG_DEBUG)
	LOG_INFO("%s", str2hex(replybuff, ret));

	if (ret == 32) {
		afreq = 0;
		for (size_t n = 1; n < 5; n++)
			afreq = afreq * 256 + (unsigned char)replybuff[n];
		afreq = afreq * 1.25; // 100D resolution is 1.25 Hz / bit for read
		amode = replybuff[5] & 0x07;
		if (amode > 7) amode = 7;
		aBW = (replybuff[5] >> 4) & 0x03;
		aBW = 3 - aBW;
		return true;
	}
	return false;
}

long RIG_FT100D::get_vfoA ()
{
	if (get_info()) {
		A.freq = afreq;
		A.imode = amode;
		A.iBW = aBW;
	}
	return A.freq;
}

void RIG_FT100D::set_vfoA (long freq)
{
	A.freq = freq;
	freq /=10; // 100D does not support 1 Hz resolution
	cmd = to_bcd_be(freq, 8);
	cmd += 0x0A;
	sendCommand(cmd, 0);
LOG_WARN("%s", str2hex(cmd.c_str(), 5));
}

int RIG_FT100D::get_modeA()
{
	return A.imode;
}

void RIG_FT100D::set_modeA(int val)
{
	A.imode = val;
	init_cmd();
	cmd[3] = FT100D_mode_val[val];
	cmd[4] = 0x0C;
	sendCommand(cmd, 0);
LOG_WARN("%s", str2hex(cmd.c_str(), 5));
}

void RIG_FT100D::set_bwA (int val)
{
	A.iBW = val;
	init_cmd();
	cmd[3] = FT100D_bw_val[val];
	cmd[4] = 0x8C;
	sendCommand(cmd, 0);
LOG_WARN("%s", str2hex(cmd.c_str(), 5));
}

int RIG_FT100D::get_bwA()
{
	return A.iBW;
}

long RIG_FT100D::get_vfoB()
{
	if (get_info()) {
		B.freq = afreq;
		B.imode = amode;
		B.iBW = aBW;
	}
	return B.freq;
}

void RIG_FT100D::set_vfoB(long freq)
{
	B.freq = freq;
	freq /=10; // 100D does not support 1 Hz resolution
	cmd = to_bcd_be(freq, 8);
	cmd += 0x0A;
	sendCommand(cmd, 0);
LOG_WARN("%s", str2hex(cmd.c_str(), 5));
}

void RIG_FT100D::set_modeB(int val)
{
	B.imode = val;
	init_cmd();
	cmd[3] = FT100D_mode_val[val];
	cmd[4] = 0x0C;
	sendCommand(cmd, 0);
LOG_WARN("%s", str2hex(cmd.c_str(), 5));
}

int  RIG_FT100D::get_modeB()
{
	return B.imode;
}

void RIG_FT100D::set_bwB(int val)
{
	B.iBW = val;
	init_cmd();
	cmd[3] = FT100D_bw_val[val];
	cmd[4] = 0x8C;
	sendCommand(cmd, 0);
LOG_WARN("%s", str2hex(cmd.c_str(), 5));
}

int  RIG_FT100D::get_bwB()
{
	return B.iBW;
}

int  RIG_FT100D::def_bandwidth(int m)
{
	return FT100D_def_bw[m];
}

// Tranceiver PTT on/off
void RIG_FT100D::set_PTT_control(int val)
{
	init_cmd();
	if (val) cmd[3] = 1;
	cmd[4] = 0x0F;
	sendCommand(cmd, 0);
LOG_WARN("%s", str2hex(cmd.c_str(), 5));
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

