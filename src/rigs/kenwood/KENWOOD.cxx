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

#include "kenwood/KENWOOD.h"
#include "support.h"
#include "tod_clock.h"

static int ret = 0;

void KENWOOD::selectA()
{
	cmd = "FR0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Rx on A", cmd, "");
	if (!split) {
		cmd = "FT0;";
		sendCommand(cmd);
		showresp(WARN, ASC, "Tx on A", cmd, "");
	} else {
		cmd = "FT1;";
		sendCommand(cmd);
		showresp(WARN, ASC, "Tx on B", cmd, "");
	}
	inuse = onA;
}

void KENWOOD::selectB()
{
	cmd = "FR1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Rx on B", cmd, "");
	if (!split) {
		cmd = "FT1;";
		sendCommand(cmd);
		showresp(WARN, ASC, "Tx on B", cmd, "");
	} else {
		cmd = "FT0;";
		sendCommand(cmd);
		showresp(WARN, ASC, "Tx on A", cmd, "");
	}
	inuse = onB;
}

void KENWOOD::set_split(bool val) 
{
	split = val;

	if (inuse == onB) {
		if (val) {
			cmd = "FR1;FT0;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on B, Tx on A", cmd, "");
		} else {
			cmd = "FR1;FT1;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on B, Tx on B", cmd, "");
		}
	} else {
		if (val) {
			cmd = "FR0;FT1;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on A, Tx on B", cmd, "");
		} else {
			cmd = "FR0;FT0;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on A, Tx on A", cmd, "");
		}
	}
}

/*
========================================================================
	frequency & mode data are contained in the IF; response
		IFaaaaaaaaaaaXXXXXbbbbbcdXeefghjklmmX;
		12345678901234567890123456789012345678
		01234567890123456789012345678901234567 byte #
		          1         2         3
		                            ^ position 28
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
		 
========================================================================
*/ 

int KENWOOD::get_split()
{
	check_ifstr();
//	cmd = "IF;";
//	get_trace(1, "get_split");
//	ret = wait_char(';', 38, 100, "get split", ASC);
//	gett("");
	if (ret < 38) return 0;
	return (replystr[32] == '1');
}

bool KENWOOD::check()
{
	cmd = "FA;";
	get_trace(1, "check()");
	ret = wait_char(';', 14, 100, "check", ASC);
	gett("");
//	if (ret < 14) return false;
	return true;
}

unsigned long int KENWOOD::get_vfoA ()
{
	cmd = "FA;";
	get_trace(1, "get_vfoA");
	ret = wait_char(';', 14, 100, "get vfo A", ASC);
	gett("");
	if (ret == 14) {
		size_t p = replystr.rfind("FA");
		if (p != std::string::npos) {
			int f = 0;
			for (size_t n = 2; n < 13; n++)
				f = f*10 + replystr[p+n] - '0';
			A.freq = f;
		}
	}
	return A.freq;
}

void KENWOOD::set_vfoA (unsigned long int freq)
{
	A.freq = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set vfo A", cmd, "");
	sett("vfoA");
}

unsigned long int KENWOOD::get_vfoB ()
{
	cmd = "FB;";
	get_trace(1, "get_vfoB");
	ret = wait_char(';', 14, 100, "get vfo B", ASC);
	gett("");
	if (ret == 14) {
		size_t p = replystr.rfind("FB");
		if (p != std::string::npos) {
			int f = 0;
			for (size_t n = 2; n < 13; n++)
				f = f*10 + replystr[p+n] - '0';
			B.freq = f;
		}
	}
	return B.freq;
}

void KENWOOD::set_vfoB (unsigned long int freq)
{
	B.freq = freq;
	cmd = "FB00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set vfo B", cmd, "");
	sett("vfoB");
}

/*
========================================================================
	frequency & mode data are contained in the IF; response
		IFaaaaaaaaaaaXXXXXbbbbbcdXeefghjklmmX;
		12345678901234567890123456789012345678
		01234567890123456789012345678901234567 byte #
		          1         2         3
		                            ^ position 28
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
		 
========================================================================
*/ 

size_t KENWOOD::check_ifstr()
{
	size_t now = zmsec();
	if ((lastmsec == 0) || now < lastmsec || (now - lastmsec) > 200) {
		lastmsec = now;
		cmd = "IF;";
		get_trace(1, "get_PTT");
		wait_char(';', 38, 100, "get VFO", ASC);
		gett("");
		ifstring = replystr;
	} else
		replystr = ifstring;
	return replystr.length();
}

int KENWOOD::get_PTT()
{
	check_ifstr();
//	cmd = "IF;";
//	get_trace(1, "get_PTT");
//	ret = wait_char(';', 38, 100, "get VFO", ASC);
//	gett("");
	if (ret < 38) return ptt_;
	ptt_ = (replystr[28] == '1');
	return ptt_;
}

// Tranceiver PTT on/off
void KENWOOD::set_PTT_control(int val)
{
	if (val) {
		cmd = "TX;";
		ptt_ = 1;
	} else {
		cmd = "RX;";
		ptt_ = 0;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set PTT", cmd, "");
	sett("PTT");
}

void KENWOOD::tune_rig(int val)
{
//	cmd = "AC111;";
//	       | |||______ start tuner = 1
//	       | ||_______ set TX hold = 1
//	       | |________ set RX hold = 1
//	       |__________ tune transceiver command prefix
	if (tuning()) return;
	switch (val) {
		case 0:
			cmd = "AC000;"; break;
		case 1:
			cmd = "AC110;"; break;
		case 2: default:
			cmd = "AC111;"; break;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "tune_rig", cmd, "");
	sett("tune_run");
}

bool KENWOOD::tuning()
{
	cmd = "AC;";
	if (wait_char(';', 6, 100, "tuning?", ASC) == 6) {
		if (replystr[4] == '1') return true;
	}
	return false;
}

int KENWOOD::get_tune()
{
	cmd = "AC;";
	get_trace(1, "get_tune");
	ret = wait_char(';', 6, 100, "tuning?", ASC);
	gett("");
	if (ret == 6) {
		size_t p = replystr.rfind("AC");
		if (p != std::string::npos) {
			return (replystr[p+4] - '0');
		}
	}
	return 0;
}

// Volume control return 0 ... 100
int KENWOOD::get_volume_control()
{
	int volctrl = 0;
	cmd = "AG0;";
	get_trace(1, "get_volume_control");
	ret = wait_char(';', 7, 100, "get vol", ASC);
	gett("");
	if (ret == 7) {
		size_t p = replystr.rfind("AG");
		if (p != std::string::npos) {
			volctrl = fm_decimal(replystr.substr(p+3),3);
			volctrl = (int)(volctrl / 2.55);
		}
	}
	return volctrl;
}

void KENWOOD::set_volume_control(int val) 
{
	int ivol = (int)(val * 2.55);
	cmd = "AG0";
	cmd.append(to_decimal(ivol, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set vol", cmd, "");
	sett("volume");
}

void  KENWOOD::select_swr()
{
	cmd = "RM1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "select SWR", cmd, "");
	sett("select SWR");
}

void  KENWOOD::select_alc()
{
	cmd = "RM3;";
	sendCommand(cmd);
	showresp(WARN, ASC, "select ALC", cmd, "");
	sett("select ALC");
}

void KENWOOD::set_rf_gain(int val)
{
	cmd = "RG";
	cmd.append(to_decimal(val * 255 / 100, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set rf gain", cmd, "");
	sett("RFgain");
}

int  KENWOOD::get_rf_gain()
{
	cmd = "RG;";
	int rfg = 100;
	get_trace(1, "get_rf_gain");
	ret = wait_char(';', 6, 100, "get rf gain", ASC);
	gett("");
	if (ret == 6) {
		size_t p = replystr.rfind("RG");
		if (p != std::string::npos)
			rfg = fm_decimal(replystr.substr(p+2) ,3) * 100 / 255;
	}
	return rfg;
}

void KENWOOD::get_rf_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

// val 0 .. 100
void KENWOOD::set_mic_gain(int val)
{
	cmd = "MG";
	cmd.append(to_decimal(val,3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set mic", cmd, "");
	sett("MICgain");
}

int KENWOOD::get_mic_gain()
{
	int mgain = 0;
	cmd = "MG;";
	get_trace(1, "get_mic_gain");
	ret = wait_char(';', 6, 100, "get mic", ASC);
	gett("");
	if (ret == 6) {
		size_t p = replystr.rfind("MG");
		if (p != std::string::npos) {
			mgain = fm_decimal(replystr.substr(p+2), 3);
		}
	}
	return mgain;
}

void KENWOOD::get_mic_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

void KENWOOD::set_noise(bool b)
{
	if (b)
		cmd = "NB1;";
	else
		cmd = "NB0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set NB", cmd, "");
	sett("setNB");
}

int KENWOOD::get_noise()
{
	int response = 1;
	cmd = "NB;";
	get_trace(1, "get_noise");
	ret = wait_char(';', 4, 100, "get Noise Blanker", ASC);
	gett("");
	if (ret == 4) {
		size_t p = replystr.rfind("NB");
		if (p == std::string::npos) response = 0;
		if (replystr[p+2] == '0') response = 0;
	}
	return response;
}

void KENWOOD::set_squelch(int val)
{
	cmd = "SQ0";
	cmd.append(to_decimal(abs(val),3)).append(";");
	sendCommand(cmd,0);
	showresp(WARN, ASC, "set squelch", cmd, "");
	sett("Squelch");
}

int  KENWOOD::get_squelch()
{
	int val = 0;
	cmd = "SQ0;";
	get_trace(1, "get_squelch");
	ret = wait_char(';', 7, 100, "get squelch", ASC);
	gett("");
	if (ret >= 7) {
		size_t p = replystr.rfind("SQ0");
		if (p == std::string::npos) return val;
		replystr[p + 6] = 0;
		val = atoi(&replystr[p + 3]);
	}
	return val;
}

void KENWOOD::get_squelch_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 255; step = 1;
}

//======================================================================
// IF shift only available if the transceiver is in the CW mode
// step size is 50 Hz
//======================================================================

void KENWOOD::set_if_shift(int val)
{
	if (active_mode == CW || active_mode == CWR) { // cw modes
		progStatus.shift_val = val;
		cmd = "IS ";
		cmd.append(to_decimal(abs(val),4)).append(";");
		sendCommand(cmd,0);
		showresp(WARN, ASC, "set IF shift", cmd, "");
		sett("IF shift");
	}
}

bool KENWOOD::get_if_shift(int &val)
{
	bool response = false;
	if (active_mode == CW || active_mode == CWR) { // cw modes
		cmd = "IS;";
		get_trace(1, "get_IF_shift");
		ret = wait_char(';', 8, 100, "get IF shift", ASC);
		gett("");
		if (ret == 8) {
			size_t p = replystr.rfind("IS");
			if (p != std::string::npos) {
				val = fm_decimal(replystr.substr(p+3), 4);
			} else
				val = progStatus.shift_val;
			response = true;
		}
	}
	val = progStatus.shift_val;
	return response;
}

void KENWOOD::get_if_min_max_step(int &min, int &max, int &step)
{
}
