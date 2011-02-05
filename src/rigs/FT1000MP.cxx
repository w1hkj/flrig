/*
 * Yaesu FT-1000MP drivers
 *
 * a part of flrig
 *
 * Copyright 2011, Dave Freese, W1HKJ
 *
 */

#include "FT1000MP.h"

static const char FT1000MPname_[] = "FT-1000MP";

static const char *FT1000MP_modes[] = {
	"LSB",    "USB", 
	"CW-L",   "CW-U", 
	"AM",     "AM-syn", 
	"FM",     "FM-alt", 
	"RTTY-L", "RTTY-U", 
	"PKT-L",  "PKT-FM", NULL};

static const char FT1000MP_mode_type[] = {
	'L', 'U', 
	'L', 'U', 
	'U', 'U', 
	'U', 'U',
	'L', 'U', 
	'L', 'U' };

static const char *FT1000MP_widths[] = {
"---/6", "---/2.4",  "---/2", "---/500", "---/250",
"2.4/6",  "2.4/2.4", "2.4/2", "2.4/500", "2.4/250",
"2/6",    "2/2.4",   "2/2",   "2/500",   "2/250",
"500/6",  "500/2.4", "500/2", "500/500", "500/250",
"250/6",  "250/24.", "250/2", "250/500", "250/250", NULL };

RIG_FT1000MP::RIG_FT1000MP() {
// base class values
	name_ = FT1000MPname_;
	modes_ = FT1000MP_modes;
	bandwidths_ = FT1000MP_widths;
	comm_baudrate = BR4800;
	stopbits = 2;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	serloop_timing = 200;
	comm_rtscts = true;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	A.freq = 14070000;
	A.imode = 1;
	A.iBW = 1;
	B.freq = 3580000;
	B.imode = 1;
	B.iBW = 1;

	has_mode_control =
	has_bandwidth_control =
	has_ptt_control =
	has_tune_control = true;

};

void RIG_FT1000MP::init_cmd()
{
	cmd = "00000";
	for (size_t i = 0; i < 5; i++) cmd[i] = 0;
}

void RIG_FT1000MP::initialize()
{
	init_cmd();
	cmd[4] = 0x81; // switch antenna tuner on
	sendCommand(cmd,0);
LOG_INFO("%s", str2hex(cmd.c_str(), 5));
//	selectA();
}

// returns 16 bytes
// 0 - band selection
// 1,2,3,4 operating frequency
//    00 50 42 01 ==> 01425000 from page 78 is totally erroneous !!!!!!
// should be
//    01 5B E6 80
// 5,6 clarifier offset
// 7 operating mode
//    b7 - alternate user mode
//    0 = CW-U, AM-ENV, RTTY-L, PKT-L
//    1 = CW-L, AM-SYNC, RTTY-U, PKT-FM
// 8 if filter selection
// 9 vfo/mem operating flags
// a,b,c,d,e,f NOT used
//
long RIG_FT1000MP::get_vfoA ()
{
	unsigned char *p = 0;
	int ret = 0;
	int alt = 0;
	init_cmd();
	cmd[3] = 0x03;  // read both vfo's
	cmd[4] = 0x10;
	ret = sendCommand(cmd, 32);
	if (ret == 32) {
		p = (unsigned char *)(&replybuff[1]);
		A.freq = ((((((p[0]<<8) + p[1])<<8) + p[2])<<8) + p[3])*10/16;

		A.imode = (replybuff[7] >> 5) & 0x07;
		alt = replybuff[8] & 0x01;
		if (A.imode > 1)
			A.imode = 2 * A.imode + alt - 2;

		A.iBW = 5*((replybuff[8] >> 1) & 0x07) + ((replybuff[8] >> 5) & 0x07);
		if (A.iBW > 24) A.iBW = 24;

		p = (unsigned char *)(&replybuff[17]);
		B.freq = ((((((p[0]<<8) + p[1])<<8) + p[2])<<8) + p[3])*10/16;

		modeB = (replybuff[16+7] >> 5) & 0x07;
		alt = replybuff[16+8] & 0x01;
		if (modeB > 1)
			modeB = 2 * modeB + alt - 2;

		B.iBW = 5*((replybuff[16+8] >> 1) & 0x07) + ((replybuff[16+8] >> 5) & 0x07);
		if (B.iBW > 24) B.iBW = 24;
/*
LOG_INFO(
"\ndata: %s\nA: %ld\nmode: %s\nbw: %s\nB: %ld\nmode: %s\nbw: %s", 
str2hex(replybuff,32), 
A.freq,
FT1000MP_modes[A.imode],
FT1000MP_widths[A.iBW],
B.freq,
FT1000MP_modes[modeB],
FT1000MP_widths[B.iBW]
);
*/
	}
	return A.freq;
}

long RIG_FT1000MP::get_vfoB ()
{
	return B.freq;
}

void RIG_FT1000MP::set_vfoA (long freq)
{
	A.freq = freq;
	init_cmd();
	freq /=10; // 1000MP does not support 1 Hz resolution
	for (int i = 0; i < 4; i++) {
		cmd[i] = (unsigned char)(freq % 10); freq /= 10;
		cmd[i] |= (unsigned char)((freq % 10) * 16); freq /= 10;
	}
	cmd[4] = 0x0A;
LOG_INFO("%s", str2hex(cmd.c_str(), cmd.length()));
	sendCommand(cmd, 0);
}

void RIG_FT1000MP::set_vfoB (long freq)
{
	B.freq = freq;
	init_cmd();
	freq /=10; // 1000MP does not support 1 Hz resolution
	for (int i = 0; i < 4; i++) {
		cmd[i] = (unsigned char)(freq % 10); freq /= 10;
		cmd[i] |= (unsigned char)((freq % 10) * 16); freq /= 10;
	}
	cmd[4] = 0x8A;
LOG_INFO("%s", str2hex(cmd.c_str(), cmd.length()));
	sendCommand(cmd, 0);
}

int RIG_FT1000MP::get_modeA()
{
	return A.imode;
}

int RIG_FT1000MP::get_modeB()
{
	return B.imode;
}

void RIG_FT1000MP::set_modeA(int val)
{
	A.imode = val;
	init_cmd();
	cmd[3] = val;
	cmd[4] = 0x0C;
	sendCommand(cmd, 0);
LOG_INFO("%s, %s", FT1000MP_modes[A.imode], str2hex(cmd.c_str(),5));
}

void RIG_FT1000MP::set_modeB(int val)
{
	B.imode = val;
	init_cmd();
	cmd[3] = val;
	cmd[4] = 0x0C;
	sendCommand(cmd, 0);
LOG_INFO("%s, %s", FT1000MP_modes[B.imode], str2hex(cmd.c_str(),5));
}

int RIG_FT1000MP::get_modetype(int n)
{
	return FT1000MP_mode_type[n];
}

int RIG_FT1000MP::get_bwA()
{
	return A.iBW;
}

void RIG_FT1000MP::set_bwA(int val)
{
	int first_if = val / 5;
	int second_if = val % 5;
	if (!first_if) first_if += 5;
	--first_if;
	if (!second_if) second_if += 5;
	--second_if;
	second_if += 0x80;

	A.iBW = val;

	init_cmd();
	cmd[0] = 0x01;
	cmd[3] = first_if;
	cmd[4] = 0x8C;
	sendCommand(cmd, 0);
LOG_INFO("%s, %s", FT1000MP_widths[A.iBW], str2hex(cmd.c_str(), 5));
	cmd[3] = second_if;
	sendCommand(cmd, 0);
LOG_INFO("%s, %s", FT1000MP_widths[A.iBW], str2hex(cmd.c_str(), 5));
}

int RIG_FT1000MP::get_bwB()
{
	return B.iBW;
}

void RIG_FT1000MP::set_bwB(int val)
{
	B.iBW = val;
	int first_if = val / 5;
	int second_if = val % 5;
	if (!first_if) first_if += 5;
	--first_if;
	if (!second_if) second_if += 5;
	--second_if;
	second_if += 0x80;

	init_cmd();
	cmd[0] = 0x02;
	cmd[3] = first_if;
	cmd[4] = 0x8C;
	sendCommand(cmd, 0);
LOG_INFO("%s, %s", FT1000MP_widths[B.iBW], str2hex(cmd.c_str(), 5));
	cmd[3] = second_if;
	sendCommand(cmd, 0);
LOG_INFO("%s, %s", FT1000MP_widths[B.iBW], str2hex(cmd.c_str(), 5));
}

void RIG_FT1000MP::selectA()
{
	init_cmd();
	cmd[4] = 0x05;
	sendCommand(cmd, 0);
LOG_INFO("%s", str2hex(cmd.c_str(), 5));
}

void RIG_FT1000MP::selectB()
{
	init_cmd();
	cmd[3] = 0x01;
	cmd[4] = 0x05;
	sendCommand(cmd, 0);
LOG_INFO("%s", str2hex(cmd.c_str(), 5));
}

void RIG_FT1000MP::set_split(bool val)
{
	split = val;
	init_cmd();
	cmd[3] = val ? 0x01 : 0x00;
	cmd[4] = 0x01;
	sendCommand(cmd, 0);
LOG_INFO("%s", str2hex(cmd.c_str(), 5));
}


// Tranceiver PTT on/off
void RIG_FT1000MP::set_PTT_control(int val)
{
	init_cmd();
	if (val) cmd[3] = 1;
	else	 cmd[3] = 0;
	cmd[4] = 0x0F;
	sendCommand(cmd, 0);
LOG_INFO("%s", str2hex(cmd.c_str(), 5));
}

void RIG_FT1000MP::tune_rig()
{
	init_cmd();
	cmd[4] = 0x82; // start antenna tuner
	sendCommand(cmd,0);
LOG_INFO("%s", str2hex(cmd.c_str(), 5));
}

int  RIG_FT1000MP::get_power_out(void)
{
	int val = 0;
	init_cmd();
	cmd[0] = 0x80;
	cmd[4] = 0xF7;
	if (sendCommand(cmd,5)) {
LOG_INFO("%s => %d",str2hex(replybuff,1), (val = replybuff[0] && 0x0F));
	}
	return 0;
}

int  RIG_FT1000MP::get_smeter(void)
{
	int val = 0;
	init_cmd();
	cmd[4] = 0xF7;
	if (sendCommand(cmd,5)) {
LOG_INFO("%s => %d",str2hex(replybuff,1), (val = replybuff[0] && 0x0F));
	}
	return 0;
}

