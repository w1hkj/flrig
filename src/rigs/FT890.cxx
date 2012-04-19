/*
 *
 * Yaesu FT-890 transceiver class
 * 
 * a part of flrig
 * 
 * Copyright 2009, Dave Freese, W1HKJ
 * 
 */

#include "FT890.h"

const char FT890name_[] = "FT-890";

const char *FT890modes_[] = {
		"LSB", "USB", "CW", "CW-N", "AM", "AM-N", "FM", NULL};
static const int FT890_mode_val[] =  { 0, 1, 2, 3, 4, 5, 6 };

static const char FT890_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'U', 'U' };

static const char *FT890widths_[] =
{ "wide", "narr", NULL};

static const int FT890_bw_val[] =
{ 0, 1 };

RIG_FT890::RIG_FT890() {
	name_ = FT890name_;
	modes_ = FT890modes_;
	bandwidths_ = FT890widths_;
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
	aBW = bBW = A.iBW = B.iBW = 0;
	precision = 10;

	has_smeter =
	has_power_out =
	has_get_info =
	has_ptt_control =
	has_mode_control = true;

	precision = 10;
	ndigits = 7;

}

void RIG_FT890::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

void RIG_FT890::initialize()
{
}

void RIG_FT890::selectA()
{
	init_cmd();
	cmd[4] = 0x05;
	sendCommand(cmd);
	showresp(WARN, HEX, "select A", cmd, replystr);
}

void RIG_FT890::selectB()
{
	init_cmd();
	cmd[3] = 0x01;
	cmd[4] = 0x05;
	sendCommand(cmd);
	showresp(WARN, HEX, "select B", cmd, replystr);
}

void RIG_FT890::set_split(bool val)
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


bool RIG_FT890::get_info()
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

long RIG_FT890::get_vfoA ()
{
	return A.freq;
}

void RIG_FT890::set_vfoA (long freq)
{
	A.freq = freq;
	freq /=10; // 890 does not support 1 Hz resolution
	cmd = to_bcd_be(freq, 8);
	cmd += 0x0A;
	sendCommand(cmd);
	showresp(WARN, HEX, "set vfo A", cmd, replystr);
}

int RIG_FT890::get_modeA()
{
	return A.imode;
}

void RIG_FT890::set_modeA(int val)
{
	A.imode = val;
	init_cmd();
	cmd[3] = FT890_mode_val[val];
	cmd[4] = 0x0C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set mode A", cmd, replystr);
}

long RIG_FT890::get_vfoB()
{
	return B.freq;
}

void RIG_FT890::set_vfoB(long freq)
{
	B.freq = freq;
	freq /=10; // 890 does not support 1 Hz resolution
	cmd = to_bcd_be(freq, 8);
	cmd += 0x8A;
	sendCommand(cmd);
	showresp(WARN, HEX, "set vfo B", cmd, replystr);
}

void RIG_FT890::set_modeB(int val)
{
	B.imode = val;
	init_cmd();
	cmd[3] = FT890_mode_val[val] | 0x80;
	cmd[4] = 0x0C;
	sendCommand(cmd);
	showresp(WARN, HEX, "set mode B", cmd, replystr);
}

int  RIG_FT890::get_modeB()
{
	return B.imode;
}

// Tranceiver PTT on/off
void RIG_FT890::set_PTT_control(int val)
{
	init_cmd();
	if (val) cmd[3] = 1;
	else	 cmd[3] = 0;
	cmd[4] = 0x0F;
	sendCommand(cmd, 0);
LOG_INFO("%s", str2hex(cmd.c_str(), 5));
}

int RIG_FT890::get_smeter()
{
	init_cmd();
	cmd[4] = 0xF7;
	int ret = sendCommand(cmd);
	if (ret < 5) return 0;
	int sval = (unsigned char)(replybuff[ret - 2]);
	sval = sval * 100 / 255;
	return sval;
}

int RIG_FT890::get_power_out()
{
	init_cmd();
	cmd[4] = 0xF7;
	int ret = sendCommand(cmd);
	if (ret < 5) return 0;
	int sval = (unsigned char)(replybuff[ret - 2]);
	sval = sval * 100 / 255;
	return sval;
}

/*
int RIG_FT890::get_swr()
{
	double swr = (fwdpwr + refpwr) / (fwdpwr - refpwr + .0001);
	swr -= 1.0;
	swr *= 25.0;
	if (swr < 0) swr = 0;
	if (swr > 100) swr = 100;
	return (int) swr;
}

int RIG_FT890::get_power_out()
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
