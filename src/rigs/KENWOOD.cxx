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

#include "KENWOOD.h"
#include "support.h"

static bool is_tuning = false;
static int  skip_get = 2;

bool KENWOOD::tuning()
{
	if (!is_tuning) return false;
	cmd = "AC;";
	if (wait_char(';', 6, 100, "tuning?", ASC) == 6) {
		if (replystr[4] == '1') return true;
	}
	is_tuning = false;
	skip_get = 2;
	return is_tuning;
}

static int txvfo = 0;
static int rxvfo = 0;

void KENWOOD::selectA()
{
	cmd = "FR0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Rx on A", cmd, "");
	cmd = "FT0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Tx on A", cmd, "");
	rxona = true;
	txvfo = rxvfo = 0;
}

void KENWOOD::selectB()
{
	cmd = "FR1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Rx on B", cmd, "");
	cmd = "FT1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Tx on B", cmd, "");
	rxona = false;
	txvfo = rxvfo = 1;
}

void KENWOOD::set_split(bool val) 
{
	split = val;

	if (useB) {
		if (val) {
			cmd = "FR1;FT0;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on B, Tx on A", cmd, "");
			rxvfo = 1; txvfo = 0;
		} else {
			cmd = "FR1;FT1;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on B, Tx on B", cmd, "");
			rxvfo = txvfo = 1;
		}
		rxona = false;
	} else {
		if (val) {
			cmd = "FR0;FT1;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on A, Tx on B", cmd, "");
			rxvfo = 0; txvfo = 1;
		} else {
			cmd = "FR0;FT0;";
			sendCommand(cmd);
			showresp(WARN, ASC, "Rx on A, Tx on A", cmd, "");
			rxvfo = txvfo = 0;
		}
		rxona = true;
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
	cmd = "IF;";
	int ret = wait_char(';', 38, 100, "get split", ASC);
	if (ret < 38) return 0;
	return (replybuff[32] == '1');
}

long KENWOOD::get_vfoA ()
{
	cmd = "FA;";
	if (wait_char(';', 14, 100, "get vfo A", ASC) == 14) {
		size_t p = replystr.rfind("FA");
		if (p != string::npos) {
			int f = 0;
			for (size_t n = 2; n < 13; n++)
				f = f*10 + replystr[p+n] - '0';
			A.freq = f;
		}
	}
	return A.freq;
}

void KENWOOD::set_vfoA (long freq)
{
	A.freq = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set vfo A", cmd, "");
}

long KENWOOD::get_vfoB ()
{
	cmd = "FB;";
	if (wait_char(';', 14, 100, "get vfo B", ASC) == 14) {
		size_t p = replystr.rfind("FB");
		if (p != string::npos) {
			int f = 0;
			for (size_t n = 2; n < 13; n++)
				f = f*10 + replystr[p+n] - '0';
			B.freq = f;
		}
	}
	return B.freq;
}

void KENWOOD::set_vfoB (long freq)
{
	B.freq = freq;
	cmd = "FB00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "set vfo B", cmd, "");
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

int KENWOOD::get_PTT()
{
	cmd = "IF;";
	int ret = wait_char(';', 38, 100, "get VFO", ASC);
	if (ret < 38) return ptt_;
	ptt_ = (replybuff[28] == '1');
	return ptt_;
}

// Tranceiver PTT on/off
void KENWOOD::set_PTT_control(int val)
{
	if (val) cmd = "TX;";
	else	 cmd = "RX;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set PTT", cmd, "");
}

void KENWOOD::tune_rig()
{
//	cmd = "AC111;";
//	       | |||______ start tuner = 1
//	       | ||_______ set TX hold = 1
//	       | |________ set RX hold = 1
//	       |__________ tune transceiver command prefix
	if (tuning()) return;
	cmd = "AC111;";
	sendCommand(cmd);
	showresp(WARN, ASC, "send tune command", cmd, "");
	is_tuning = true;
}

// Volume control return 0 ... 100
int KENWOOD::get_volume_control()
{
	cmd = "AG0;";
	if (wait_char(';', 7, 100, "get vol", ASC) == 7) {
		int volctrl = 0;
		size_t p = replystr.rfind("AG");
		if (p != string::npos) {
			volctrl = fm_decimal(replystr.substr(p+3),3);
			return (int)(volctrl / 2.55);
		}
	}
	return 0;
}

void KENWOOD::set_volume_control(int val) 
{
	int ivol = (int)(val * 2.55);
	cmd = "AG0";
	cmd.append(to_decimal(ivol, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set vol", cmd, "");
}

void  KENWOOD::select_swr()
{
	cmd = "RM1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "select SWR", cmd, "");
}

void  KENWOOD::select_alc()
{
	cmd = "RM3;";
	sendCommand(cmd);
	showresp(WARN, ASC, "select ALC", cmd, "");
}

void KENWOOD::set_rf_gain(int val)
{
	cmd = "RG";
	cmd.append(to_decimal(val * 255 / 100, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set rf gain", cmd, "");
}

int  KENWOOD::get_rf_gain()
{
	cmd = "RG;";
	int rfg = 100;
	if (wait_char(';', 6, 100, "get rf gain", ASC) == 6) {
		size_t p = replystr.rfind("RG");
		if (p != string::npos)
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
}

int KENWOOD::get_mic_gain()
{
	cmd = "MG;";
	if (wait_char(';', 6, 100, "get mic", ASC) == 6) {
		int mgain = 0;
		size_t p = replystr.rfind("MG");
		if (p != string::npos) {
			mgain = fm_decimal(replystr.substr(p+2), 3);
			return mgain;
		}
	}
	return 0;
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
}

int KENWOOD::get_noise()
{
	cmd = "NB;";
	if (wait_char(';', 4, 100, "get Noise Blanker", ASC) == 4) {
		size_t p = replystr.rfind("NB");
		if (p == string::npos) return 0;
		if (replystr[p+2] == '0') return 0;
	}
	return 1;
}

void KENWOOD::set_squelch(int val)
{
	cmd = "SQ0";
	cmd.append(to_decimal(abs(val),3)).append(";");
	sendCommand(cmd,0);
	showresp(WARN, ASC, "set squelch", cmd, "");
}

int  KENWOOD::get_squelch()
{
	int val = 0;
	cmd = "SQ0;";
	if (wait_char(';', 7, 100, "get squelch", ASC) >= 7) {
		size_t p = replystr.rfind("SQ0");
		if (p == string::npos) return val;
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
	}
}

bool KENWOOD::get_if_shift(int &val)
{
	if (active_mode == CW || active_mode == CWR) { // cw modes
		cmd = "IS;";
		if (wait_char(';', 8, 100, "get IF shift", ASC) == 8) {
			size_t p = replystr.rfind("IS");
			if (p != string::npos) {
				val = fm_decimal(replystr.substr(p+3), 4);
			} else
				val = progStatus.shift_val;
			return true;
		}
	}
	val = progStatus.shift_val;
	return false;
}

void KENWOOD::get_if_min_max_step(int &min, int &max, int &step)
{
/*
// 2000
	if_shift_min = min = 400;
	if_shift_max = max = 1000;
	if_shift_step = step = 50;
	if_shift_mid = 700;
// 590, 590SG
	if_shift_min = min = 300;
	if_shift_max = max = 1000;
	if_shift_step = step = 50;
	if_shift_mid = 800;
*/
}


/*

// Transceiver power level
void KENWOOD::set_power_control(double val)
{
	int ival = (int)val;
	cmd = "PC";
	cmd.append(to_decimal(ival, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set pwr ctrl", cmd, "");
}

int KENWOOD::get_power_control()
{
	cmd = "PC;";
	if (wait_char(';', 6, 100, "get pout", ASC) == 6) {
		int pctrl = 0;
		size_t p = replystr.rfind("PC");
		if (p != string::npos) {
			pctrl = fm_decimal(replystr.substr(p+2), 3);
			return pctrl;
		}
	}
	return 0;
}

void KENWOOD::set_notch(bool on, int val)
{
	if (on) {
		cmd = "BC2;"; // set manual notch
		sendCommand(cmd);
		showresp(WARN, ASC, "set notch on", cmd, "");
		cmd = "BP";
//		val = round((val - 220) / 50);
		val = round((val - 200) / 50);
		cmd.append(to_decimal(val, 3)).append(";");
		sendCommand(cmd);
		showresp(WARN, ASC, "set notch val", cmd, "");
	} else {
		cmd = "BC0;"; // no notch action
		sendCommand(cmd);
		showresp(WARN, ASC, "set notch off", cmd, "");
	}
}

bool  KENWOOD::get_notch(int &val)
{
	bool ison = false;
	cmd = "BC;";
	if (wait_char(';', 4, 100, "get notch on/off", ASC) == 4) {
		size_t p = replystr.rfind("BC");
		if (p != string::npos) {
			if (replystr[p+2] == '2') {
				ison = true;
				cmd = "BP;";
				if (wait_char(';', 6, 100, "get notch val", ASC) == 6) {
					p = replystr.rfind("BP");
					if (p != string::npos)
						val = 200 + 50 * fm_decimal(replystr.substr(p+2),3);
				}
			}
		}
	}
	return (ison);
}

void KENWOOD::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = 200;
	max = 3350;
	step = 50;
}

void KENWOOD::set_auto_notch(int v)
{
	cmd = v ? "NT1;" : "NT0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set auto notch", cmd, "");
}

int  KENWOOD::get_auto_notch()
{
	cmd = "NT;";
	if (wait_char(';', 4, 100, "get auto notch", ASC) == 4) {
		int anotch = 0;
		size_t p = replystr.rfind("NT");
		if (p != string::npos) {
			anotch = (replystr[p+2] == '1');
			return anotch;
		}
	}
	return 0;
}

void KENWOOD::set_noise_reduction(int val)
{
	if (val == -1) {
		return;
	}
	_noise_reduction_level = val;
	if (_noise_reduction_level == 0) {
		nr_label("NR", false);
	} else if (_noise_reduction_level == 1) {
		nr_label("NR1", true);
	} else if (_noise_reduction_level == 2) {
		nr_label("NR2", true);
	}
	cmd.assign("NR");
	cmd += '0' + _noise_reduction_level;
	cmd += ';';
	sendCommand (cmd);
	showresp(WARN, ASC, "SET noise reduction", cmd, "");
}

int  KENWOOD::get_noise_reduction()
{
	cmd = rsp = "NR";
	cmd.append(";");
	if (wait_char(';', 4, 100, "GET noise reduction", ASC) == 4) {
		size_t p = replystr.rfind(rsp);
		if (p == string::npos) return _noise_reduction_level;
		_noise_reduction_level = replystr[p+2] - '0';
	}

	if (_noise_reduction_level == 1) {
		nr_label("NR1", true);
	} else if (_noise_reduction_level == 2) {
		nr_label("NR2", true);
	} else {
		nr_label("NR", false);
	}

	return _noise_reduction_level;
}

void KENWOOD::set_noise_reduction_val(int val)
{
	if (_noise_reduction_level == 0) return;
	if (_noise_reduction_level == 1) _nrval1 = val;
	else _nrval2 = val;

	cmd.assign("RL").append(to_decimal(val, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET_noise_reduction_val", cmd, "");
}

int  KENWOOD::get_noise_reduction_val()
{
	int nrval = 0;
	if (_noise_reduction_level == 0) return 0;
	int val = progStatus.noise_reduction_val;
	cmd = rsp = "RL";
	cmd.append(";");
	if (wait_char(';', 5, 100, "GET noise reduction val", ASC) == 5) {
		size_t p = replystr.rfind(rsp);
		if (p == string::npos) {
			nrval = (_noise_reduction_level == 1 ? _nrval1 : _nrval2);
			return nrval;
		}
		val = atoi(&replystr[p+2]);
	}

	if (_noise_reduction_level == 1) _nrval1 = val;
	else _nrval2 = val;

	return val;
}

int KENWOOD::get_alc(void)
{
	cmd = "RM3;";
	sendCommand(cmd, 0);
	showresp(WARN, ASC, "ALC meter", cmd, "");

	cmd = "RM;";
	if (wait_char(';', 8, 100, "get alc", ASC) < 8) return 0;

	size_t p = replystr.find("RM3");
	if (p == string::npos) return 0;

	replystr[p + 7] = 0;
	int alc_val = atoi(&replystr[p + 3]);
	alc_val *= 100;
	alc_val /= 30;
	if (alc_val > 100) alc_val = 100;
	return alc_val;
}

*/
