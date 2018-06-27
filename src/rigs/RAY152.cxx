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
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "RAY152.h"

//=============================================================================

const char RIG_RAY152name_[] = "RAY 152";

const char *RIG_RAY152modes_[] = { "USB", "LSB", "H3E", "A1A", "F1B", NULL};
static const char RIG_RAY152_mode_type[] = {'U', 'L', 'U', 'L', 'U'};

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol, 2, 145,  50 },
	{ (Fl_Widget *)sldrVOLUME, 54, 145, 368 },
	{ (Fl_Widget *)sldrRFGAIN, 54, 125, 156 },
	{ (Fl_Widget *)sldrSQUELCH, 266, 125, 156 },
	{ (Fl_Widget *)NULL, 0, 0, 0 }
};

RIG_RAY152::RIG_RAY152() {
	name_ = RIG_RAY152name_;
	modes_ = RIG_RAY152modes_;

	widgets = rig_widgets;

	comm_baudrate = BR1200;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 10;
	comm_timeout = 50;
	comm_echo = true;
	comm_rtscts = false;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;

	A.freq = 14070000;
	A.imode = 0;
	A.iBW = 0;

	B.freq = 3580000;
	B.imode = 0;
	B.iBW = 0;

	precision = 100;
	ndigits = 6;

	has_mode_control = true;
	has_ptt_control = true;
	has_rf_control = true;
	has_volume_control = true;
	has_rit = true;
	has_sql_control = true;
	has_noise_control = true;
	has_auto_notch = true;
};


static void nocr( string & s)
{
	for (size_t i = 0; i < s.length(); i++)
		if (s[i] == '\r') s[i] = ' ';
}

/*
Data string returned by the 'O' command
		3	A*\r         AGC ON/OFF
		5	C***\r       Memory channel #
		5	D+/-**\r     Clarifier frequency
		9	FT******\r  Transmit frequemcy
		9	FR******\r  Receive frequency
		6	I****\r      ITU channel #
		3	M*\r         Mode
		3	N*\r         Noise blanker status
		3	P*\r         Power reduction status
		5	Q***\r       Squelch setting 
		5	R***\r       RF gain setting
		5	V***\r       Volume setting
		3	Z*\r         Meter function
total  64
*/

bool RIG_RAY152::check()
{
	int ret = sendCommand("O\r");
	if (ret < 66) return false;
	return true;
}

void RIG_RAY152::get_data()
{
	int ret = sendCommand("O\r");
	if (ret < 66) return;
// test string
//replystr = "A1\rC000\rD-05\rFT1407000\rFR1407000\rI0000\rM1\rN1\rP0\rQ000\rR100\rV128\rZ1\r";

	if (dumpdata)
		LOG_WARN("\n%s", replystr.c_str());
	dumpdata = false;

	size_t pos;

	pos = replystr.find("FR"); // receive frequency
	if (pos != string::npos) {
		int freq;
		sscanf(&replystr[pos + 2], "%d", &freq);
		A.freq = 100 * freq;
	}

	pos = replystr.find("M"); // mode
	if (pos != string::npos)
		A.imode = replystr[pos + 1] - '1';

	pos = replystr.find("D");
	if (pos != string::npos) {
		sscanf(&replystr[pos + 1], "%d", &RitFreq);
		RitFreq *= 10;
	}

	pos = replystr.find("\rR");
	if (pos != string::npos)
		sscanf(&replystr[pos + 2], "%d", &rfg);

	pos = replystr.find("V");
	if (pos != string::npos) {
		sscanf(&replystr[pos + 1], "%d", &vol);
		vol *= 100;
		vol /= 255;
	}

	pos = replystr.find("Q");
	if (pos != string::npos) {
		sscanf(&replystr[pos + 1], "%d", &squelch);
	}

	pos = replystr.find("N");
	if (pos != string::npos) {
		if (replystr[pos + 1] > '0') {
			nb_set = replystr[pos+1];
			nb = 1;
		}
		else {
			nb = 0;
			nb_set = '2';
		}
	}

// RAY152 usurps the autonotch button for AGC control
	pos = replystr.find("A");
	if (pos != string::npos)
		agc = replystr[1] == '1' ? 1 : 0;

}

void RIG_RAY152::initialize()
{
	rig_widgets[0].W = btnVol;
	rig_widgets[1].W = sldrVOLUME;
	rig_widgets[2].W = sldrRFGAIN;
	rig_widgets[3].W = sldrSQUELCH;

	sendCommand("E1\r", 0);
	sendCommand("Z1\r", 0);
	dumpdata = true;
	get_data();
	set_auto_notch(agc);
}

void RIG_RAY152::shutdown()
{
	sendCommand("E0\r", 0);
LOG_INFO("%s", cmd.c_str());
}

long RIG_RAY152::get_vfoA ()
{
	return A.freq;
}

void RIG_RAY152::set_vfoA (long freq)
{
	A.freq = freq;
	cmd = "FT000000\r";
	freq /= 100;
	cmd[7] += freq % 10; freq /= 10;
	cmd[6] += freq % 10; freq /= 10;
	cmd[5] += freq % 10; freq /= 10;
	cmd[4] += freq % 10; freq /= 10;
	cmd[3] += freq % 10; freq /=10;
	cmd[2] += freq;
	sendCommand(cmd, 0);
LOG_INFO("%s", cmd.c_str());
	cmd[1] = 'R';
	sendCommand(cmd, 0);
LOG_INFO("%s", cmd.c_str());
}

long RIG_RAY152::get_vfoB ()
{
	return B.freq;
}

void RIG_RAY152::set_vfoB (long freq)
{
	B.freq = freq;
	cmd = "FT000000\r";
	freq /= 100;
	cmd[7] += freq % 10; freq /= 10;
	cmd[6] += freq % 10; freq /= 10;
	cmd[5] += freq % 10; freq /= 10;
	cmd[4] += freq % 10; freq /= 10;
	cmd[3] += freq % 10; freq /=10;
	cmd[2] += freq;
	sendCommand(cmd, 0);
LOG_INFO("%s", cmd.c_str());
	cmd[1] = 'R';
	sendCommand(cmd, 0);
LOG_INFO("%s", cmd.c_str());
}

void RIG_RAY152::set_PTT_control(int val)
{
	cmd = val ? "X1\r" : "X0\r";
	sendCommand(cmd,0);
LOG_INFO("%s", cmd.c_str());
	ptt_ = val;
}

void RIG_RAY152::set_modeA(int md)
{
	A.imode = md;
	cmd = "M";
	cmd += (md  + '1');
	cmd += '\r';
	sendCommand(cmd, 0);
LOG_INFO("%s", cmd.c_str());
}

int RIG_RAY152::get_modeA()
{
	return A.imode;
}

void RIG_RAY152::set_modeB(int md)
{
	B.imode = md;
	cmd = "M";
	cmd += (md  + '1');
	cmd += '\r';
	sendCommand(cmd, 0);
LOG_INFO("%s", cmd.c_str());
}

int RIG_RAY152::get_modeB()
{
	return B.imode;
}

int RIG_RAY152::get_modetype(int n)
{
	return RIG_RAY152_mode_type[n];
}

void RIG_RAY152::set_volume_control(int val)
{
	vol = val;
	string cmd = "V000\r";
	val *= 255;
	val /= 100;
	cmd[3] += val % 10; val /= 10;
	cmd[2] += val % 10; val /= 10;
	cmd[1] += val;
	sendCommand(cmd, 0);
LOG_INFO("%s", cmd.c_str());
}

void RIG_RAY152::set_rf_gain(int val)
{
	rfg = val;
	cmd = "R000\r";
	cmd[3] += val % 10; val /= 10;
	cmd[2] += val % 10; val /= 10;
	cmd[1] += val;
	sendCommand(cmd, 0);
LOG_INFO("%s", cmd.c_str());
}

int RIG_RAY152::get_smeter(void)
{
	cmd = "U\r";
	int ret = sendCommand(cmd);
	string s = replystr;
	nocr(s);
LOG_WARN("%s", s.c_str());
	if (ret < 5) return 0;
	if (replystr[ret - 5] == 'U') {
		int val;
		sscanf(&replystr[ret - 5 + 1], "%d", &val);
		val = (int)(60.0 * (256.0 / (val + 16.0) - 1.0));
		if (val > 100) val = 100;
		if (val < 0) val = 0;
		return val;
	}
	return 0;
}

int RIG_RAY152::get_power_out(void)
{
	int ret = sendCommand("U\r");
	if (ret < 5) return 0;
	if (replystr[ret - 5] == 'U') {
		int val;
		sscanf(&replystr[ret - 5 + 1], "%d", &val);
		val /= 128;
		val *= 100;
		return val;
	}
	return -1;
}


void RIG_RAY152::setRit(int v)
{
	RitFreq = v;
	cmd = "D+00\r";
	if (v < 0) cmd[1] = '-';
	v /= 10;
	v = abs(v);
	cmd[3] += v % 10; v /= 10;
	cmd[2] += v % 10;
	sendCommand(cmd, 0);
LOG_INFO("%s", cmd.c_str());
}

int  RIG_RAY152::getRit()
{
	return RitFreq;
}

void RIG_RAY152::set_squelch(int val)
{
	squelch = val;
	cmd = "Q000\r";
	cmd[3] += val % 10; val /= 10;
	cmd[2] += val % 10; val /= 10;
	cmd[1] += val;
	sendCommand(cmd, 0);
LOG_INFO("%s", cmd.c_str());
}

int RIG_RAY152::get_squelch()
{
	return squelch;
}

void RIG_RAY152::set_noise(bool on) 
{
	cmd = "Nx\r";
	cmd[1] = on ? nb_set : '0';
	sendCommand(cmd, 0);
LOG_INFO("%s", cmd.c_str());
}

int RIG_RAY152::get_noise()
{
	return nb;
}

void RIG_RAY152::set_auto_notch(int v)
{
	cmd = "Ax\r";
	cmd[1] = v ? '1' : '0';
	sendCommand(cmd, 0);
LOG_INFO("%s", cmd.c_str());
}

int RIG_RAY152::get_auto_notch()
{
	return agc;
}

