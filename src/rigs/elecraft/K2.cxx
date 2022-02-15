// ----------------------------------------------------------------------------
// Copyright (C) 2014
//              David Freese, W1HKJ
//
// This file is part of flrig.
//
// flrig is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// flrig is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// aunsigned long int with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------
// Additions by Kent Haase, VE4KEH
//-----------------------------------------------------------------------------
 
#include "elecraft/K2.h"
#include "support.h"
#include "status.h"

#define K2_WAIT 200

const char K2name_[] = "K2";

static const char *K2modes_[] = 
	{ "LSB",  "USB",  "CW",   "RTTY", "CW-R", "RTTY-R", NULL};

static const char K2_mode_type[] =
	{ 'L',    'U',    'L',    'L',     'U',   'U' };

static const char *K2_mode_str[] =
	{ "MD1;", "MD2;", "MD3;", "MD6;", "MD7;", "MD9;" };

static int K2_num_modes = 6;

const char *K2_widths[] = {"FL1", "FL2", "FL3", "FL4", NULL};
static int K2_bw_vals[] = { 1, 2, 3, 4, WVALS_LIMIT};

static GUI k2_widgets[]= {
	{ (Fl_Widget *)sldrPOWER, 266, 105, 156 },
	{ (Fl_Widget *)NULL, 0, 0, 0 }
};

RIG_K2::RIG_K2() {
// base class values	
	name_ = K2name_;
	modes_ = K2modes_;
	bandwidths_ = K2_widths;
	bw_vals_ = K2_bw_vals;

	widgets = k2_widgets;

	comm_baudrate = BR4800;
	stopbits = 2;
	comm_retries = 3;
	comm_wait = 50;
	comm_timeout = 200;
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = false;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	modeB = modeA = 1;
	bwB = bwA = 0;
	freqB = freqA = 14070000;

	has_split_AB =
	has_get_info =
	has_attenuator_control =
	has_preamp_control =
	has_power_control =
	has_mode_control =
	has_bandwidth_control =
	has_ptt_control =
	has_split =
	has_smeter =
	has_power_out =
	has_tune_control = true;

	K2split =
	has_micgain_control =
	has_notch_control =
	has_ifshift_control =
	has_volume_control =
	has_swr_control = false;

	hipower = false;

	precision = 10;
	ndigits = 7;

}

int  RIG_K2::adjust_bandwidth(int m)
{
	return 0;
}

void RIG_K2::initialize()
{
	progStatus.gettrace = 1;
	progStatus.settrace = 1;

	k2_widgets[0].W = sldrPOWER;

	set_trace(1, "disable auto reporting");
	sendCommand("AI0;", 0);
	sett("");

	set_trace(1,"enable extended command mode");
	sendCommand("K22;", 0);
	sett("");

//ensure K2 is in VFO A
    get_power_control();
}

bool RIG_K2::check ()
{
	rsp = cmd = "FA";
	cmd += ';';
	get_trace(1, "check");
	int ret = wait_char(';', 14, K2_WAIT, "check", ASC);
	gett("");

	if (ret < 14) return false;
	return true;
}

/*
 * IF (Transceiver Information; GET only)
 * 
 * RSP format: IF[f]*****+yyyyrx*00tmvspb01*; where the fields are defined as follows:
 * 
 * 0         1         2         3
 * 0123456789012345678901234567890123456789
 * IFfffffffffff*****+yyyyrx*00tmvspb01*;
 *   0..1   IF 
 *   2..12  [f] operating frequency, excluding any RIT/XIT offset (11 digits; see FA command)
 *   13..17 * represents a space (BLANK, or ASCII 0x20)
 *   18     + either "+" or "-" (sign of RIT/XIT offset)
 *   19..22 yyyy RIT/XIT offset in Hz (range is -9990 to +9990 Hz when computer-controlled)
 *   23     r 1 if RIT is on, 0 if off
 *   24     x 1 if XIT is on, 0 if off
 *   25..27 fill
 *   28     t 1 if the K2 is in transmit mode, 0 if receive
 *   29     m operating mode (see MD command)
 *   30     v receive-mode VFO selection, 0 for VFO A, 1 for VFO B
 *   31     s 1 if scan is in progress, 0 otherwise
 *   32     p 1 if the transeiver is in split mode, 0 otherwise
 *   33     b basic RSP format: always 0
 *            extended RSP format: 
 *                1 if the present IF response is due to a K2 band change and 
 *                0 otherwise
 *   34..35 fill
 *   37     ; terminator
 * The fixed-value fields (space, 0, and 1) are provided for syntactic compatibility with existing software.
 * 
 * received from M1CNK
 * 01234567890123457890123456789012345678
 * FA00014039310;
 * IF00014039310     +007000 0002000001 ;
 * IF00014039310     +007000 0002000001 ;
 * 
*/
// set replystr to teststr to test for various control bytes
//const char *teststr = "IFfffffffffff*****+yyyyrx*000m0s1b01*;";

//static void do_selectA(void *)
//{
//	cb_selectA();
//}

//static void do_selectB(void *)
//{
//	cb_selectB();
//}

bool RIG_K2::get_info()
{
	rsp = cmd = "IF";
	cmd += ';';
	get_trace(1, "get info");
	int ret = wait_char(';', 38, K2_WAIT,"get info", ASC);
	gett("");

	if (ret < 38) return false;
	size_t p = replystr.find(rsp);
	ptt_ = (replystr[p+28] == '1');
	inuse = (replystr[p+30] == '1') ? onB : onA;
	K2split = replystr[p+32]-'0';
	return true;
}

void RIG_K2::selectA()
{
	cmd = "FR0;FT0";
	set_trace(1, "selectA");
	sendCommand(cmd);
	sett("");

	K2split = false;
	showresp(WARN, ASC, "select A", cmd, replystr);
	inuse = onA;
}

void RIG_K2::selectB()
{
	cmd = "FR1;FT1";
	set_trace(1, "selectB");
	sendCommand(cmd);
	sett("");

	K2split = false;
	showresp(WARN, ASC, "select B", cmd, replystr);
	inuse = onB;
}

bool RIG_K2::can_split()
{
	return true;
}

void RIG_K2::set_split(bool val)
{
	if (val) {
		if (inuse == onB)
			cmd = "FR1;FT0;";
		else
			cmd = "FR0;FT1;";
		set_trace(1, "set split");
		sendCommand(cmd);
		sett("");
		showresp(WARN, ASC, "set split ON", cmd, replystr);
	} else {
		if (inuse == onB)
			cmd = "FR1;FT1;";
		else
			cmd = "FR0;FT0;";
		set_trace(1, "set split");
		sendCommand(cmd);
		sett("");
		showresp(WARN, ASC, "set split OFF", cmd, replystr);
	}
	cmd = "FA;";
	get_trace(1, "get vfoA after set split");
	wait_char(';', 14, 1000, "get vfo A", ASC);
	gett("");
}

int RIG_K2::get_split()
{
	return K2split;
}

unsigned long int RIG_K2::get_vfoA ()
{
	rsp = cmd = "FA";
	cmd += ';';
	get_trace(1, "get vfoA");
	int ret = wait_char(';', 14, K2_WAIT,"get vfo A", ASC);
	gett("");

	if (ret < 14) return freqA;
	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return freqA;
	unsigned long int f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replystr[p + n] - '0';
	freqA = f;
	return freqA;
}

void RIG_K2::set_vfoA (unsigned long int freq)
{
	freqA = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	set_trace(1, "set vfoA");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vfo A", cmd, replystr);
	sett("");
}

unsigned long int RIG_K2::get_vfoB()
{
	rsp = cmd = "FB";
	cmd += ';';
	get_trace(1, "get vfoB");
	int ret = wait_char(';', 14, K2_WAIT,"get vfo B", ASC);
	gett("");

	if (ret < 14) return freqB;
	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return freqB;
	unsigned long int f = 0;
	for (size_t n = 2; n < 13; n++)
		f = f*10 + replystr[p + n] - '0';
	freqB = f;
	return freqB;
}

void RIG_K2::set_vfoB(unsigned long int freq)
{
	freqB = freq;
	cmd = "FB00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	set_trace(1, "set vfoB");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET vfo B", cmd, replystr);
	sett("");
}

// LSB        MD1;
// LSB-rev    MD2;
// USB        MD2;
// USB-rev    MD1;
// RTTY       MD6;
// RTTY-rev   MD9;
// CW         MD3;
// CW-rev     MD7;

void RIG_K2::set_modeA(int val)
{
	modeA = val;
	cmd = K2_mode_str[val];
	set_trace(1, "set modeA");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mode A", cmd, replystr);
	sett("");
}

int RIG_K2::get_modeA()
{
	rsp = cmd = "MD";
	cmd += ';';
	get_trace(1, "get modeA");
	int ret = wait_char(';', 4, K2_WAIT,"get mode A", ASC);
	gett("");

	if (ret < 4) return 0;
	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return 0;
	for (int i = 0; i < K2_num_modes; i++) {
		if (replystr.substr(p) == K2_mode_str[i]) {
			modeA = i;
			return modeA;
		}
	}
	return modeA;
}

void RIG_K2::set_modeB(int val)
{
	modeB = val;
	cmd = K2_mode_str[val];
	set_trace(1, "set modeB");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET mode B", cmd, replystr);
	sett("");
}

int  RIG_K2::get_modeB()
{
	rsp = cmd = "MD";
	cmd += ';';
	get_trace(1, "get modeB");
	int ret = wait_char(';', 4, K2_WAIT,"get mode B", ASC);
	gett("");

	if (ret < 4) return 0;
	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return 0;
	for (int i = 0; i < K2_num_modes; i++) {
		if (replystr.substr(p) == K2_mode_str[i]) {
			modeB = i;
			return modeB;
		}
	}
	return modeB;
}

int RIG_K2::get_modetype(int n)
{
	return K2_mode_type[n];
}

void RIG_K2::set_bwA(int val)
{
	bwA = val;
	switch (val) {
		case 0 : cmd = "FW00001;"; break;
		case 1 : cmd = "FW00002;"; break;
		case 2 : cmd = "FW00003;"; break;
		case 3 : cmd = "FW00004;"; break;
		default: cmd = "FW00001;";
	}
	set_trace(1, "set bwA");
	sendCommand(cmd);
	showresp(WARN, ASC, "set bwA", cmd, replystr);
	sett("");
}

int RIG_K2::get_bwA()
{
	rsp = cmd = "FW";
	cmd += ';';
	get_trace(1, "get bwA");
	int ret = wait_char(';', 9, K2_WAIT,"get bw A", ASC);
	gett("");

	if (ret < 9) return 0;
	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return 0;
	bwA = replystr[p + 6] - '1';
	return bwA;
}

void RIG_K2::set_bwB(int val)
{
	bwB = val;
	switch (val) {
		case 0 : cmd = "FW00001;"; break;
		case 1 : cmd = "FW00002;"; break;
		case 2 : cmd = "FW00003;"; break;
		case 3 : cmd = "FW00004;"; break;
		default: cmd = "FW00001;";
	}
	set_trace(1, "set bwB");
	sendCommand(cmd);
	showresp(WARN, ASC, "set bwA", cmd, replystr);
	sett("");
}

int  RIG_K2::get_bwB()
{
	rsp = cmd = "FW";
	cmd += ';';
	get_trace(1, "get bwB");
	int ret = wait_char(';', 9, K2_WAIT,"get bw B", ASC);
	gett("");

	if (ret < 9) return 0;
	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return 0;
	bwB = replystr[p + 6] - '1';
	return bwB;
}

void RIG_K2::tune_rig(int val)
{
	cmd = "SW20;";
	sendCommand(cmd);
	showresp(WARN, ASC, "tune", cmd, replystr);
	set_trace(1, "tune rig");
}

int RIG_K2::get_smeter()
{
	rsp = cmd = "BG";
	cmd += ';';
	get_trace(1, "get smeter");
	int ret = wait_char(';', 5, K2_WAIT,"get smeter", ASC);
	gett("");

	if (ret < 5) return 0;
	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return 0;
	int mtr = 10 * (replystr[p + 2] - '0') 
			+ replystr[p + 3] - '0';   //assemble two chars into 2 digit int
	if (mtr > 10) {                    //if int greater than 10 (bar mode)
	    mtr -= 12;                     //shift down to 0 thru 10
	}
	mtr *= 10;                         //normalize to 0 thru 100
	return mtr;
}

int RIG_K2::get_power_out()
{
	rsp = cmd = "BG";
	cmd += ';';
	int ret = wait_char(';', 5, K2_WAIT,"get power", ASC);
	gett("");
	get_trace(1, "get power out");

	if (ret < 5) return 0;
	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return 0;
	int mtr = 10 * (replystr[p + 2] - '0') 
			+ replystr[p + 3] - '0';   //assemble two chars into 2 digit int
	if (mtr > 10) {                           //if int greater than 10 (bar mode)
	    mtr -= 12;                            //shift down to 0 thru 10
	}
	mtr *= 10;                                //normalize to 0 thru 100
	return mtr;
}

double RIG_K2::get_power_control()
{
	rsp = cmd = "PC";  // extended mode for get power
	cmd += ';';
	get_trace(1, "get power control");
	int ret = wait_char(';', 7, K2_WAIT,"get pwr cont", ASC);
	gett("");

	if (ret < 7) return progStatus.power_level;
	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return 0;
	hipower = (replystr[p+5] == '1');
	int mtr = fm_decimal(replystr.substr(p+4), 3);
	if (!hipower) mtr /= 10;
	return mtr;
}

void RIG_K2::set_power_control(double val)
{
    int ival = (int)val;
	cmd = "PC";                    //init the cmd std::string
	if (val > 15) {
		hipower = true;
		cmd[5] = '1';
		cmd.append(to_decimal(ival,3)).append("1;");
	} else {
		hipower = false;
		cmd.append(to_decimal((int)(val*10),3)).append("0;");
	}
	set_trace(1, "set power control");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET pwr", cmd, replystr);
	sett("");
}

void RIG_K2::get_pc_min_max_step(double &min, double &max, double &step)
{
	if (hipower) {
		min = 1.0; max_power = max = 110.0; step = 1.0;
	} else {
		min = 0.1; max_power = max = 20; step = 0.1;
	}
}

void RIG_K2::set_PTT_control(int val)
{
	if (val) cmd = "TX;";
	else	 cmd = "RX;";

	set_trace(1, "set PTT");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET ptt", cmd, replystr);
	sett("");

	ptt_ = val;
}

int RIG_K2::get_PTT()
{
	rsp = cmd = "IF";
	cmd += ';';
	get_trace(1, "get info");
	int ret = wait_char(';', 38, K2_WAIT,"get info", ASC);
	gett("");

	if (ret < 38) return false;
	size_t p = replystr.find(rsp);
	ptt_ = (replystr[p+28] == '1');
	return ptt_;
}

void RIG_K2::set_attenuator(int val)
{
	if (val) cmd = "RA01;";
	else	 cmd = "RA00;";
	set_trace(1, "set attenuator");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET att", cmd, replystr);
	sett("");
}

int RIG_K2::get_attenuator()
{
	rsp = cmd = "RA";
	cmd += ';';
	get_trace(1, "get_attenuator");
	int ret = wait_char(';', 5, K2_WAIT,"get att", ASC);
	gett("");

	if (ret < 5) return 0;
	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return 0;
	return (replystr[p + 3] == '1' ? 1 : 0);
}

void RIG_K2::set_preamp(int val)
{
	if (val) cmd = "PA1;";
	else	 cmd = "PA0;";
	set_trace(1, "set preamp");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET pre", cmd, replystr);
	sett("");
}

int RIG_K2::get_preamp()
{
	rsp = cmd = "PA";
	cmd += ';';
	get_trace(1, "get preamp");
	int ret = wait_char(';', 4, K2_WAIT,"get pre", ASC);
	gett("");

	if (ret < 4) return 0;
	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return 0;
	return (replystr[p + 2] == '1' ? 1 : 0);
}

