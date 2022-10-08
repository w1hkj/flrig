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

#include "other/sunsdr2.h"
#include "support.h"

static const char SUNSDR2name_[] = "SunSDR";

enum { SDR2_LSB, SDR2_USB, SDR2_CW, SDR2_FM, SDR2_AM, SDR2_DIGL, SDR2_CWR, SDR2_SAM, SDR2_DIGU, SDR2_DRM };

static const char *SUNSDR2modes_[] = {
"LSB", "USB", "CW", "FM", "AM", "DIGL", "CW-R", "SAM", "DIGU", "DRM", NULL};
static const char *SUNSDR2_mode_cat[] = {
"MD1;", "MD2;", "MD3;", "MD4;", "MD5;", "MD6;", "MD7;", "MD8;", "MD9;", "MD0;", NULL};
int SUNSDR2_num_modes = 10;
 
static const char *SUNSDR2_ssb_widths[] = {
"1000", "1200", "1400", "1600", "1800", 
"2000", "2200", "2400", "2600", "2800", 
"3000", "3400", "4000", "5000", NULL };
static const char *SUNSDR2_ssb_cat[] = {
"SH00;", "SH01;", "SH02;", "SH03;", "SH04;",
"SH05;", "SH06;", "SH07;", "SH08;", "SH09;",
"SH10;", "SH11;", "SH12;", "SH13;", NULL };
int SUNSDR2_num_ssb = 14;

static const char *SUNSDR2_am_widths[] = {
"5 KHz", "6 KHz", "8 KHz", "10 KHz", NULL };
static const char *SUNSDR2_am_cat[] = {
"SH00", "SH01", "SH02", "SH03", NULL };
int SUNSDR2_num_am = 4;

static const char *SUNSDR2_cw_widths[] = {
"50", "80", "100", "150", "200", 
"300", "400", "500", "600", "1000", NULL};
static const char *SUNSDR2_cw_cat[] = {
"FW0050;", "FW0080;", "FW0100;", "FW0150;", "FW0200;", 
"FW0300;", "FW0400;", "FW0500;", "FW0600;", "FW1000;", NULL };
int SUNSDR2_num_cw = 10;

static const char *SUNSDR2_digi_widths[] = {
"250", "500", "1000", "1500", NULL};
static const char *SUNSDR2_digi_cat[] = {
"FW0250;", "FW0500;", "FW1000;", "FW1500;", NULL };
int SUNSDR2_num_digi = 4;

static const char *SUNSDR2_other_widths[] = {
"FIXED", NULL };
static const char *SUNSDR2_other_cat[] = {
"FW0000;", NULL };
int SUNSDR2_num_other = 1;

static int agcval = 1;

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 }, // 0
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 }, // 1
	{ (Fl_Widget *)sldrRFGAIN,   54, 145, 156 }, // 2
	{ (Fl_Widget *)btnIFsh,     214, 105,  50 }, // 3
	{ (Fl_Widget *)sldrIFSHIFT, 266, 105, 156 }, // 4
	{ (Fl_Widget *)sldrSQUELCH, 266, 125, 156 }, // 5
	{ (Fl_Widget *)sldrMICGAIN, 266, 145, 156 }, // 6
	{ (Fl_Widget *)sldrPOWER,    54, 165, 368 }, // 7
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

void RIG_SDR2_PRO::initialize()
{
	rig_widgets[0].W = btnVol;
	rig_widgets[1].W = sldrVOLUME;
	rig_widgets[2].W = sldrRFGAIN;
	rig_widgets[3].W = btnIFsh;
	rig_widgets[4].W = sldrIFSHIFT;
	rig_widgets[5].W = sldrSQUELCH;
	rig_widgets[6].W = sldrMICGAIN;
	rig_widgets[7].W = sldrPOWER;
}

RIG_SDR2_PRO::RIG_SDR2_PRO() {
// base class values
	name_ = SUNSDR2name_;
	modes_ = SUNSDR2modes_;
	bandwidths_ = SUNSDR2_cw_widths;
	bw_vals_ = NULL;

	widgets = rig_widgets;

	serial_baudrate = BR115200;
	stopbits = 1;
	serial_retries = 2;

//	serial_write_delay = 0;
//	serial_post_write_delay = 0;

	serial_timeout = 50;
	serial_rtscts = false;
	serial_rtsplus = false;
	serial_dtrplus = false;
	serial_catptt = true;
	serial_rtsptt = false;
	serial_dtrptt = false;
	B.imode = A.imode = 1;
	B.iBW = A.iBW = 0x8A03;
	B.freq = A.freq = 14070000;

	can_change_alt_vfo = true;

	has_extras = true;

	has_noise_reduction =
	has_noise_reduction_control =
	has_auto_notch =
	has_noise_control =
	has_sql_control =

	has_split = true;
	has_split_AB = true;
	has_micgain_control = true;
	has_ifshift_control = true;
	has_rf_control = true;
	has_agc_control = true;

	has_power_out = true;
	has_smeter = true;
	has_attenuator_control = true;
	has_preamp_control = true;
	has_mode_control = true;
	has_bandwidth_control = true;
	has_volume_control = true;
	has_power_control = true;
	has_tune_control = true;
	has_ptt_control = true;

	precision = 1;
	ndigits = 8;

	_noise_reduction_level = 0;
	_nrval1 = 2;
	_nrval2 = 4;
	preamp_level = atten_level = 0;
}

static int ret = 0;

const char * RIG_SDR2_PRO::get_bwname_(int n, int md) 
{
	if (md == SDR2_LSB || md == SDR2_USB || md == SDR2_FM)
		return SUNSDR2_ssb_widths[n];
	if (md == SDR2_CW || md == SDR2_CWR)
		return SUNSDR2_cw_widths[n];
	if (md == SDR2_AM)
		return SUNSDR2_am_widths[n];
	if (md == SDR2_DIGU || md == SDR2_DIGL)
		return SUNSDR2_digi_widths[n];
	return "UNKNOWN";
}

void RIG_SDR2_PRO::shutdown()
{
}

// SM cmd 0 ... 100 (rig values 0 ... 15)
int RIG_SDR2_PRO::get_smeter()
{
	int mtr = 0;
	cmd = "SM0;";
	get_trace(1, "get_smeter");
	ret = wait_char(';', 8, 100, "get Smeter", ASC);
	gett("");
	if (ret < 8) return 0;

	size_t p = replystr.rfind("SM");
	if (p != std::string::npos)
		mtr = 5 * atoi(&replystr[p + 3]);
	return mtr;
}

int RIG_SDR2_PRO::set_widths(int val)
{
	int bw;
	if (val == SDR2_LSB || val == SDR2_USB || val == SDR2_FM) {
		bandwidths_ = SUNSDR2_ssb_widths;
		bw = 10;
	} else if (val == SDR2_CW || val == SDR2_CWR) {
		bandwidths_ = SUNSDR2_cw_widths;
		bw = 7;
	} else if (val == SDR2_DIGU || val == SDR2_DIGL) {
		bandwidths_ = SUNSDR2_digi_widths;
		bw = 0;
	} else if (val == SDR2_AM) {
		bandwidths_ = SUNSDR2_am_widths;
		bw = 2;
	} else {
		bandwidths_ = SUNSDR2_other_widths;
		bw = 0;
	}
	return bw;
}

const char **RIG_SDR2_PRO::bwtable(int m)
{
	if (m == SDR2_LSB || m == SDR2_USB || m == SDR2_FM)
		return SUNSDR2_ssb_widths;
	else if (m == SDR2_CW || m == SDR2_CWR)
		return SUNSDR2_cw_widths;
	else if (m == SDR2_DIGU || m == SDR2_DIGL)
		return SUNSDR2_digi_widths;
	else if (m == SDR2_AM)
		return SUNSDR2_am_widths;
	return SUNSDR2_other_widths;
}

void RIG_SDR2_PRO::set_modeA(int val)
{
	A.imode = val;
	cmd = SUNSDR2_mode_cat[val];
	sendCommand(cmd);
	sett("");
	showresp(WARN, ASC, "set mode", cmd, "");
	A.iBW = set_widths(val);
}

int RIG_SDR2_PRO::get_modeA()
{
	cmd = "MD;";
	get_trace(1, "get_modeA");
	ret = wait_char(';', 4, 100, "get modeA", ASC);
	gett("");

	size_t p = replystr.rfind("MD");
	if (p != std::string::npos) {
		replystr = replystr.substr(p);
		int i = 0;
		while (SUNSDR2_mode_cat[i] != NULL) {
			if (replystr == SUNSDR2_mode_cat[i]){
				A.imode = i;
				A.iBW = set_widths(i);
				return A.imode;
			}
			i++;
		}
	}
	return A.imode;
}

void RIG_SDR2_PRO::set_modeB(int val)
{
	B.imode = val;
	cmd = SUNSDR2_mode_cat[val];

	sendCommand(cmd);
	sett("");
	showresp(WARN, ASC, "set mode B", cmd, "");
	B.iBW = set_widths(val);
}

int RIG_SDR2_PRO::get_modeB()
{
	cmd = "MD;";
	get_trace(1, "get_modeB");
	ret = wait_char(';', 4, 100, "get modeB", ASC);
	gett("");

	size_t p = replystr.rfind("MD");
	if (p != std::string::npos) {
		replystr = replystr.substr(p);
		int i = 0;
		while (SUNSDR2_mode_cat[i] != NULL) {
			if (replystr == SUNSDR2_mode_cat[i]) {
				B.imode = i;
				B.iBW = set_widths(i);
				return B.imode;
			}
			i++;
		}
	}
	return B.imode;
}

int RIG_SDR2_PRO::get_modetype(int n)
{
	return _mode_type[n];
}

void RIG_SDR2_PRO::set_bwA(int val)
{
	int md = A.imode;
	if (md == SDR2_LSB || md == SDR2_USB || md == SDR2_FM)
		cmd = SUNSDR2_ssb_cat[val < SUNSDR2_num_ssb ? val : SUNSDR2_num_ssb];
	else if (md == SDR2_CW || md == SDR2_CWR)
		cmd = SUNSDR2_cw_cat[val < SUNSDR2_num_cw ? val : SUNSDR2_num_cw];
	else if (md == SDR2_DIGU || md == SDR2_DIGL)
		cmd = SUNSDR2_digi_cat[val < SUNSDR2_num_digi ? val : SUNSDR2_num_digi];
	else if (md == SDR2_AM)
		cmd = SUNSDR2_am_cat[val < SUNSDR2_num_am ? val : SUNSDR2_num_am];
	else
		cmd = SUNSDR2_other_cat[0];
	sendCommand(cmd);
	sett("");
	showresp(WARN, ASC, "set SDR2_DIGU bw", cmd, "");
}

int RIG_SDR2_PRO::get_bwA()
{
	int i = 0;
	size_t p;

	if (A.imode == SDR2_LSB || A.imode == SDR2_USB) {
		cmd = "SH;";
		int retn = wait_char(';', 5, 100, "get SH", ASC);
		gett("");
		if (retn >= 5) {
			p = replystr.rfind("SH");
			if (p != std::string::npos) {
				replystr = replystr.substr(p);
				while (SUNSDR2_ssb_cat[i] != NULL) {
					if (replystr == SUNSDR2_ssb_cat[i])
						return A.iBW = i;
					i++;
				}
			}
		}
	}
	else if (A.imode == SDR2_CW || A.imode == SDR2_CWR) {
		cmd = "FW;";
		int retn = wait_char (';', 7, 100, "get BW", ASC);
		gett("");
		if (retn >= 7) {
			p = replystr.rfind("FW");
			if (p != std::string::npos) {
				replystr = replystr.substr(p);
				while (SUNSDR2_cw_cat[i] != NULL) {
					if (replystr == SUNSDR2_cw_cat[i])
						return A.iBW = i;
					i++;
				}
			}
		}
	} else if (A.imode == SDR2_DIGU || A.imode == SDR2_DIGL) {
		cmd = "FW;";
		int retn = wait_char (';', 7, 100, "get BW", ASC);
		gett("");
		if (retn >= 7) {
			p = replystr.rfind("FW");
			if (p != std::string::npos) {
				replystr = replystr.substr(p);
				while (SUNSDR2_digi_cat[i] != NULL) {
					if (replystr == SUNSDR2_digi_cat[i])
						return A.iBW = i;
					i++;
				}
			}
		}
	}
	return A.iBW;
}

void RIG_SDR2_PRO::set_bwB(int val)
{
	int md = B.imode;
	if (md == SDR2_LSB || md == SDR2_USB || md == SDR2_FM)
		cmd = SUNSDR2_ssb_cat[val < SUNSDR2_num_ssb ? val : SUNSDR2_num_ssb];
	else if (md == SDR2_CW || md == SDR2_CWR)
		cmd = SUNSDR2_cw_cat[val < SUNSDR2_num_cw ? val : SUNSDR2_num_cw];
	else if (md == SDR2_DIGU || md == SDR2_DIGL)
		cmd = SUNSDR2_digi_cat[val < SUNSDR2_num_digi ? val : SUNSDR2_num_digi];
	else if (md == SDR2_AM)
		cmd = SUNSDR2_am_cat[val < SUNSDR2_num_am ? val : SUNSDR2_num_am];
	else
		cmd = SUNSDR2_other_cat[0];
	sendCommand(cmd);
	showresp(WARN, ASC, "set SDR2_DIGU bw", cmd, "");
	sett("");
}

int RIG_SDR2_PRO::get_bwB()
{
	int i = 0;
	size_t p;

	if (B.imode == SDR2_LSB || B.imode == SDR2_USB) {
		cmd = "SH;";
		int retn = wait_char(';', 5, 100, "get SH", ASC);
		gett("");
		if (retn >= 5) {
			p = replystr.rfind("SH");
			if (p != std::string::npos) {
				replystr = replystr.substr(p);
				while (SUNSDR2_ssb_cat[i] != NULL) {
					if (replystr == SUNSDR2_ssb_cat[i])
						return B.iBW = i;
					i++;
				}
			}
		}
	}
	else if (B.imode == SDR2_CW || B.imode == SDR2_CWR) {
		cmd = "FW;";
		int retn = wait_char (';', 7, 100, "get BW", ASC);
		gett("");
		if (retn >= 7) {
			p = replystr.rfind("FW");
			if (p != std::string::npos) {
				replystr = replystr.substr(p);
				while (SUNSDR2_cw_cat[i] != NULL) {
					if (replystr == SUNSDR2_cw_cat[i])
						return B.iBW = i;
					i++;
				}
			}
		}
	} else if (B.imode == SDR2_DIGU || B.imode == SDR2_DIGL) {
		cmd = "FW;";
		int retn = wait_char (';', 7, 100, "get BW", ASC);
		gett("");
		if (retn >= 7) {
			p = replystr.rfind("FW");
			if (p != std::string::npos) {
				replystr = replystr.substr(p);
				while (SUNSDR2_digi_cat[i] != NULL) {
					if (replystr == SUNSDR2_digi_cat[i])
						return B.iBW = i;
					i++;
				}
			}
		}
	}
	return A.iBW;
}

int RIG_SDR2_PRO::adjust_bandwidth(int val)
{
	int bw = 0;
	if (val == SDR2_LSB || val == SDR2_USB || val == SDR2_FM)
		bw = 10;
	else if (val == SDR2_AM)
		bw = 2;
	else if (val == SDR2_CW || val == SDR2_CWR)
		bw = 7;
	else if (val == SDR2_DIGU)
		bw = 1;
	else // other
		bw = 0;
	return bw;
}

int RIG_SDR2_PRO::def_bandwidth(int val)
{
	return adjust_bandwidth(val);
}


void RIG_SDR2_PRO::set_power_control(double val)
{
	cmd = "PC";
	char szval[4];
	if (modeA == 4 && val > 50) val = 50; // SDR2_AM mode limitation
	snprintf(szval, sizeof(szval), "%03d", (int)val);
	cmd += szval;
	cmd += ';';
	LOG_WARN("%s", cmd.c_str());
	sendCommand(cmd);
}

double RIG_SDR2_PRO::get_power_control()
{
	int val = progStatus.power_level;
	cmd = "PC;";
	get_trace(1, "get_power_control");
	ret = wait_char(';', 6, 100, "get Power control", ASC);
	gett("");
	if (ret < 6) return val;

	size_t p = replystr.rfind("PC");
	if (p == std::string::npos) return val;

	val = atoi(&replystr[p + 2]);

	return val;
}

void RIG_SDR2_PRO::get_pc_min_max_step(double &min, double &max, double &step)
 {
	min = 0; max = 20.0; step = 0.1;
}

void RIG_SDR2_PRO::set_attenuator(int val)
{
	if (val)	cmd = "RA01;";
	else		cmd = "RA00;";
	LOG_WARN("%s", cmd.c_str());
	set_trace(1, "set attenuator");
	sendCommand(cmd);
	sett("");
	atten_level = val;
}

int RIG_SDR2_PRO::get_attenuator()
{
	cmd = "RA;";
	get_trace(1, "get_attenuator");
	ret = wait_char(';', 7, 100, "get attenuator", ASC);
	gett("");
	if (ret < 7) return atten_level;

	size_t p = replystr.rfind("RA");
	if (p != std::string::npos)
		atten_level = (replystr[p+3] == '1');
	return atten_level;
}

void RIG_SDR2_PRO::set_preamp(int val)
{
	if (val)	cmd = "PA1;";
	else		cmd = "PA0;";
	LOG_WARN("%s", cmd.c_str());
	set_trace(1, "set preamp");
	sendCommand(cmd);
	sett("");
	preamp_level = val;
}

int RIG_SDR2_PRO::get_preamp()
{
	cmd = "PA;";
	get_trace(1, "get_preamp");
	ret = wait_char(';', 5, 100, "get preamp", ASC);
	gett("");
	if (ret < 5) return preamp_level;

	size_t p = replystr.rfind("PA");
	if (p != std::string::npos)
		preamp_level = (replystr[p+2] == '1');
	return preamp_level;
}

void RIG_SDR2_PRO::set_if_shift(int val)
{
	cmd = "IS+";
	if (val < 0) cmd[2] = '-';
	cmd.append(to_decimal(abs(val),4)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set IF shift", cmd, "");
}

bool RIG_SDR2_PRO::get_if_shift(int &val)
{
	cmd = "IS;";
	get_trace(1, "get_if_shift");
	ret = wait_char(';', 8, 100, "get IF shift", ASC);
	gett("");
	if (ret == 8) {
		size_t p = replystr.rfind("IS");
		if (p != std::string::npos) {
			val = fm_decimal(replystr.substr(p+3), 4);
			if (replystr[p+2] == '-') val *= -1;
			return (val != 0);
		}
	}
	val = progStatus.shift_val;
	return progStatus.shift;
}

void RIG_SDR2_PRO::get_if_min_max_step(int &min, int &max, int &step)
{
	if_shift_min = min = -1100;
	if_shift_max = max = 1100;
	if_shift_step = step = 10;
	if_shift_mid = 0;
}

// Noise Reduction (TS2000.cxx) NR1 only works; no NR2 and don' no why
void RIG_SDR2_PRO::set_noise_reduction(int val)
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

int  RIG_SDR2_PRO::get_noise_reduction()
{
	cmd = rsp = "NR";
	cmd.append(";");
	get_trace(1, "get_noise_reduction");
	ret = wait_char(';', 4, 100, "GET noise reduction", ASC);
	gett("");
	if (ret == 4) {
		size_t p = replystr.rfind(rsp);
		if (p == std::string::npos) return _noise_reduction_level;
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

void RIG_SDR2_PRO::set_noise_reduction_val(int val)
{
	if (_noise_reduction_level == 0) return;
	if (_noise_reduction_level == 1) _nrval1 = val;
	else _nrval2 = val;

	cmd.assign("RL").append(to_decimal(val, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET_noise_reduction_val", cmd, "");
}

int  RIG_SDR2_PRO::get_noise_reduction_val()
{
	int nrval = 0;
	if (_noise_reduction_level == 0) return 0;
	int val = progStatus.noise_reduction_val;
	cmd = rsp = "RL";
	cmd.append(";");
	get_trace(1, "get_noise_reduction_val");
	ret = wait_char(';', 5, 100, "GET noise reduction val", ASC);
	gett("");
	if (ret == 5) {
		size_t p = replystr.rfind(rsp);
		if (p == std::string::npos) {
			nrval = (_noise_reduction_level == 1 ? _nrval1 : _nrval2);
			return nrval;
		}
		val = atoi(&replystr[p+2]);
	}

	if (_noise_reduction_level == 1) _nrval1 = val;
	else _nrval2 = val;

	return val;
}

int  RIG_SDR2_PRO::get_agc()
{
	cmd = "GT;";
	get_trace(1, "get_agc");
	ret = wait_char(';', 6, 100, "GET agc val", ASC);
	gett("");
	size_t p = replystr.rfind("GT");
	if (p == std::string::npos) return agcval;
	if (replystr[4] == ' ') return 0;
	agcval = replystr[4] - '0' + 1; // '0' == off, '1' = fast, '2' = slow
	return agcval;
}

int RIG_SDR2_PRO::incr_agc()
{
	agcval++;
	if (agcval == 4) agcval = 1;
	cmd.assign("GT00");
	cmd += (agcval + '0' - 1);
	cmd += ";";
	sendCommand(cmd);
	showresp(WARN, ASC, "SET agc", cmd, replystr);
	return agcval;
}


static const char *agcstrs[] = {"SDR2_FM", "AGC", "FST", "SLO"};
const char *RIG_SDR2_PRO::agc_label()
{
	return agcstrs[agcval];
}

int  RIG_SDR2_PRO::agc_val()
{
	return agcval;
}

// Auto Notch, beat canceller (TS2000.cxx) BC1 only, not BC2
void RIG_SDR2_PRO::set_auto_notch(int v)
{
	cmd = v ? "BC1;" : "BC0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set auto notch", cmd, "");

}

int  RIG_SDR2_PRO::get_auto_notch()
{
	cmd = "BC;";
	get_trace(1, "get_auto_notch");
	ret = wait_char(';', 4, 100, "get auto notch", ASC);
	gett("");
	if (ret == 4) {
		int anotch = 0;
		size_t p = replystr.rfind("BC");
		if (p != std::string::npos) {
			anotch = (replystr[p+2] == '1');
			return anotch;
		}
	}
	return 0;
}

// Noise Blanker (TS2000.cxx)
void RIG_SDR2_PRO::set_noise(bool b)
{
	if (b)
		cmd = "NB1;";
	else
		cmd = "NB0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set NB", cmd, "");
}

int RIG_SDR2_PRO::get_noise()
{
	cmd = "NB;";
	get_trace(1, "get_noise");
	ret = wait_char(';', 4, 100, "get Noise Blanker", ASC);
	gett("");
	if (ret == 4) {
		size_t p = replystr.rfind("NB");
		if (p == std::string::npos) return 0;
		if (replystr[p+2] == '0') return 0;
	}
	return 1;
}

// Tranceiver PTT on/off
void RIG_SDR2_PRO::set_PTT_control(int val)
{
	if (val) {
		if (progStatus.data_port) cmd = "TX1;"; // DTS transmission using ANI input
		else cmd = "TX0;"; // mic input
	} else cmd = "RX;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set PTT", cmd, "");
}

int RIG_SDR2_PRO::get_PTT()
{
//	cmd = "IF;";
//	get_trace(1, "get_PTT");
//	ret = wait_char(';', 38, 100, "get VFO", ASC);
//	gett("");

	int ret = check_ifstr();
	if (ret < 38) return ptt_;
	ptt_ = (replystr[28] == '1');
	return ptt_;
}

void RIG_SDR2_PRO::set_rf_gain(int val)
{
	cmd = "RG";
	cmd.append(to_decimal(val,3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set rf gain", cmd, "");
}

int  RIG_SDR2_PRO::get_rf_gain()
{
	int val = progStatus.rfgain;
	cmd = "RG;";
	get_trace(1, "get_rf_gain");
	ret = wait_char(';', 6, 100, "get rf gain", ASC);
	gett("");
	if (ret < 6) return val;

	size_t p = replystr.rfind("RG");
	if (p != std::string::npos)
		val = fm_decimal(replystr.substr(p+2), 3);
	return val;
}

void RIG_SDR2_PRO::get_rf_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

void RIG_SDR2_PRO::selectA()
{
	cmd = "FR0;FT0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Rx on A, Tx on A", cmd, "");
	inuse = onA;
}

void RIG_SDR2_PRO::selectB()
{
	cmd = "FR1;FT1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Rx on B, Tx on B", cmd, "");
	inuse = onB;
}

void RIG_SDR2_PRO::set_split(bool val) 
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

bool RIG_SDR2_PRO::can_split()
{
	return true;
}

int RIG_SDR2_PRO::get_split()
{
	size_t p;
	int split = 0;
	char rx = 0, tx = 0;
// tx vfo
	cmd = rsp = "FT";
	cmd.append(";");
	if (wait_char(';', 4, 100, "get split tx vfo", ASC) == 4) {
		p = replystr.rfind(rsp);
		if (p == std::string::npos) return split;
		tx = replystr[p+2];
	}
// rx vfo
	cmd = rsp = "FR";
	cmd.append(";");
	if (wait_char(';', 4, 100, "get split rx vfo", ASC) == 4) {
		p = replystr.rfind(rsp);
		if (p == std::string::npos) return split;
		rx = replystr[p+2];
	}

	if (tx == '0' && rx == '0') split = 0;
	else if (tx == '1' && rx == '0') split = 1;
	else if (tx == '0' && rx == '1') split = 2;
	else if (tx == '1' && rx == '1') split = 3;

	return split;
}

unsigned long int RIG_SDR2_PRO::get_vfoA ()
{
	cmd = "FA;";
	if (wait_char(';', 14, 100, "get vfo A", ASC) < 14) return A.freq;

	size_t p = replystr.rfind("FA");
	if (p != std::string::npos && (p + 12 < replystr.length())) {
		int f = 0;
		for (size_t n = 2; n < 13; n++)
			f = f*10 + replystr[p+n] - '0';
		A.freq = f;
	}
	return A.freq;
}

void RIG_SDR2_PRO::set_vfoA (unsigned long int freq)
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

unsigned long int RIG_SDR2_PRO::get_vfoB ()
{
	cmd = "FB;";
	if (wait_char(';', 14, 100, "get vfo B", ASC) < 14) return B.freq;

	size_t p = replystr.rfind("FB");
	if (p != std::string::npos && (p + 12 < replystr.length())) {
		int f = 0;
		for (size_t n = 2; n < 13; n++)
			f = f*10 + replystr[p+n] - '0';
		B.freq = f;
	}
	return B.freq;
}

void RIG_SDR2_PRO::set_vfoB (unsigned long int freq)
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

// Squelch (TS990.cxx)
void RIG_SDR2_PRO::set_squelch(int val)
{
		cmd = "SQ0";
		cmd.append(to_decimal(abs(val),3)).append(";");
		sendCommand(cmd);
		showresp(INFO, ASC, "set squelch", cmd, "");
}

int  RIG_SDR2_PRO::get_squelch()
{
	int val = 0;
	cmd = "SQ0;";
		if (wait_char(';', 7, 20, "get squelch", ASC) >= 7) {
			size_t p = replystr.rfind("SQ0");
			if (p == std::string::npos) return val;
			replystr[p + 6] = 0;
			val = atoi(&replystr[p + 3]);
	}
	return val;
}

void RIG_SDR2_PRO::get_squelch_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 255; step = 1;
}

void RIG_SDR2_PRO::set_mic_gain(int val)
{
	cmd = "MG";
	cmd.append(to_decimal(val,3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set mic gain", cmd, "");
}

int  RIG_SDR2_PRO::get_mic_gain()
{
	int val = progStatus.mic_gain;
	cmd = "MG;";
	if (wait_char(';', 6, 100, "get mic gain", ASC) < 6) return val;

	size_t p = replystr.rfind("MG");
	if (p != std::string::npos)
		val = fm_decimal(replystr.substr(p+2), 3);
	return val;
}

void RIG_SDR2_PRO::get_mic_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}


void RIG_SDR2_PRO::set_volume_control(int val)
{
	cmd = "AG";
	char szval[20];
	snprintf(szval, sizeof(szval), "%04d", val * 255 / 100);
	cmd += szval;
	cmd += ';';
	LOG_WARN("%s", cmd.c_str());
	sendCommand(cmd);
}

int RIG_SDR2_PRO::get_volume_control()
{
	int val = progStatus.volume;
	cmd = "AG0;";
	if (wait_char(';', 7, 100, "get vol", ASC) < 7) return val;

	size_t p = replystr.rfind("AG");
	if (p == std::string::npos) return val;
	replystr[p + 6] = 0;
	val = atoi(&replystr[p + 3]);
	val = val * 100 / 255;
	return val;
}

void RIG_SDR2_PRO::tune_rig()
{
	cmd = "AC111;";
	LOG_WARN("%s", cmd.c_str());
	sendCommand(cmd);
}

