/*
 * Yaesu FT-920 drivers
 * 
 * a part of flrig
 * 
 * Copyright 2011, Dave Freese, W1HKJ
 * 
 */

#include "FT920.h"
#include "rig.h"

static const char FT920name_[] = "FT-920";
static const char *FT920modes_[] = 
{ "LSB", "USB", "CW-USB", "CW-LSB", "AM", "AM-2", "FM", "FM-N",
"DATA-LSB", "DATA2-LSB", "DATA-USB", "DATA-FM", NULL};

static const int FT920_def_bw[] = { 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };

static const int FT920_mode_val[] =
{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

static const char FT920_mode_type[] =
{ 'L', 'U', 'U', 'L', 'U', 'U', 'U', 'U', 'L', 'L', 'U', 'U' };

static const char *FT920widths_[] =
{ "wide", "narr", NULL};

static const int FT920_bw_val[] =
{ 0, 1 };

RIG_FT920::RIG_FT920() {
	name_ = FT920name_;
	modes_ = FT920modes_;
	bandwidths_ = FT920widths_;
	comm_baudrate = BR4800;
	stopbits = 2;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = false;
	comm_rtsptt = false;
	comm_dtrptt = false;
	afreq = bfreq = A.freq = B.freq = 14070000;
	amode = bmode = A.imode = B.imode = 1;
	aBW = bBW = A.iBW = B.iBW = 0;
	precision = 10;

	has_get_info =
	has_mode_control =
	has_bandwidth_control = true;

}

int  RIG_FT920::adjust_bandwidth(int m)
{
	return FT920_def_bw[m];
}

int  RIG_FT920::def_bandwidth(int m)
{
	return FT920_def_bw[m];
}

void RIG_FT920::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

void RIG_FT920::initialize()
{
}

void RIG_FT920::selectA()
{
	init_cmd();
	cmd[4] = 0x05;
	sendCommand(cmd);
	showresp(WARN, HEX, "select A", cmd, replystr);
}

void RIG_FT920::selectB()
{
	init_cmd();
	cmd[3] = 0x01;
	cmd[4] = 0x05;
	sendCommand(cmd);
	showresp(WARN, HEX, "select B", cmd, replystr);
}

void RIG_FT920::set_split(bool val)
{
	split = val;
	init_cmd();
	cmd[3] = val ? 0x01 : 0x00;
	cmd[4] = 0x01;
	sendCommand(cmd);
	if (val)
		showresp(INFO, HEX, "set split ON", cmd, replystr);
	else
		showresp(INFO, HEX, "set split OFF", cmd, replystr);
}


bool RIG_FT920::get_info()
{
	init_cmd();
	cmd[3] = 0x03;
	cmd[4] = 0x10;
	int ret = sendCommand(cmd);
	showresp(WARN, HEX, "get info", cmd, replystr);

	if (ret >= 28) {
		size_t p = ret - 28;
		afreq = 0;
		bfreq = 0;
		for (size_t n = 1; n < 5; n++) {
			afreq = afreq * 256 + (unsigned char)replybuff[p + n];
			bfreq = bfreq * 256 + (unsigned char)replybuff[p + 14 + n];
		}
		afreq = afreq * 1.25;
		bfreq = bfreq * 1.25;
		int md = replybuff[p+5];
		int mode = md & 0x07;
		switch (md) {
			case 0 : 
				amode = (md & 0x40) ? 1 : 0;
				break;
			case 1 :
				amode = (md & 0x40) ? 2 : 3;
				break;
			case 2 :
				amode = 4;
				break;
			case 3 :
				amode = (md & 0x80) ? 7 : 6;
				break;
			case 4 :
				amode = (md & 0x40) ? 8 : 10;
				break;
			case 6 :
				amode = 11;
				break;
			default :
				amode = 1;
		}
		aBW = (md & 0x80) ? 0 : 1;

		md = replybuff[p + 19];
		mode = md & 0x07;
		switch (md) {
			case 0 : 
				bmode = (md & 0x40) ? 1 : 0;
				break;
			case 1 :
				bmode = (md & 0x40) ? 2 : 3;
				break;
			case 2 :
				bmode = 4;
				break;
			case 3 :
				bmode = (md & 0x80) ? 7 : 6;
				break;
			case 4 :
				bmode = (md & 0x40) ? 8 : 10;
				break;
			case 6 :
				bmode = 11;
				break;
			default :
				bmode = 1;
		}
		bBW = (md & 0x80) ? 0 : 1;

		A.freq = afreq;
		A.imode = amode;
		A.iBW = aBW;

		B.freq = bfreq;
		B.imode = bmode;
		B.iBW = bBW;

		return true;
	}
	return false;
}

long RIG_FT920::get_vfoA ()
{
	return A.freq;
}

void RIG_FT920::set_vfoA (long freq)
{
	A.freq = freq;
	freq /=10; // 920 does not support 1 Hz resolution
	cmd = to_bcd_be(freq, 8);
	cmd += 0x0A;
	sendCommand(cmd);
	showresp(WARN, HEX, "set vfo A", cmd, replystr);
}

int RIG_FT920::get_modeA()
{
	return A.imode;
}

void RIG_FT920::set_modeA(int val)
{
	A.imode = val;
	init_cmd();
	cmd[3] = FT920_mode_val[val];
	cmd[4] = 0x0C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set mode A", cmd, replystr);
}

void RIG_FT920::set_bwA (int val)
{
	A.iBW = val;
	init_cmd();
	cmd[3] = FT920_bw_val[val];
	cmd[4] = 0x8C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set bw A", cmd, replystr);
}

int RIG_FT920::get_bwA()
{
	return A.iBW;
}

long RIG_FT920::get_vfoB()
{
	return B.freq;
}

void RIG_FT920::set_vfoB(long freq)
{
	B.freq = freq;
	freq /=10; // 920 does not support 1 Hz resolution
	cmd = to_bcd_be(freq, 8);
	cmd += 0x8A;
	sendCommand(cmd);
	showresp(WARN, HEX, "set vfo B", cmd, replystr);
}

void RIG_FT920::set_modeB(int val)
{
	B.imode = val;
	init_cmd();
	cmd[3] = FT920_mode_val[val] | 0x80;
	cmd[4] = 0x0C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set mode B", cmd, replystr);
}

int  RIG_FT920::get_modeB()
{
	return B.imode;
}

void RIG_FT920::set_bwB(int val)
{
	B.iBW = val;
	init_cmd();
	cmd[3] = FT920_bw_val[val] | 0x80;
	cmd[4] = 0x8C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set bw B", cmd, replystr);
}

int  RIG_FT920::get_bwB()
{
	return B.iBW;
}

/*
int RIG_FT920::get_smeter()
{
	init_cmd();
	cmd[4] = 0xF7;
	int ret = sendCommand(cmd, 9);
	if (ret < 9) return 0;
	int sval = (200 -  (unsigned char)replybuff[ret - 9 + 3]) / 1.1;
	if (sval < 0) sval = 0;
	if (sval > 100) sval = 100;
	return sval;
}

int RIG_FT920::get_swr()
{
	double swr = (fwdpwr + refpwr) / (fwdpwr - refpwr + .0001);
	swr -= 1.0;
	swr *= 25.0;
	if (swr < 0) swr = 0;
	if (swr > 100) swr = 100;
	return (int) swr;
}

int RIG_FT920::get_power_out()
{
	init_cmd();
	cmd[4] = 0xF7;
	int ret = sendCommand(cmd);
	if (ret < 9) return 0;
	fwdpwr = replybuff[ret - 9 + 1] / 2.56;
	refpwr = replybuff[ret - 9 + 2] / 2.56;
	return (int) fwdpwr;
}
*/
