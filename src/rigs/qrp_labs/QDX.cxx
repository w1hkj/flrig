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

#include "qrp_labs/QDX.h"
#include "support.h"

static const char QDXname_[] = "QDX";

static const char *QDXmodes_[] = {
		"LSB", "USB", NULL};
static const char QDX_mode_type[] = { 'L', 'U' };

// SL command is lo cut when menu 045 OFF
static const char *QDX_BW[] = {
  "3200", NULL };
//static const char *QDX_BW_tooltip = "Fixed bandwidth";

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 }, // 0
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 }, // 1
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

void RIG_QDX::initialize()
{
	rig_widgets[0].W = btnVol;
	rig_widgets[1].W = sldrVOLUME;
}

RIG_QDX::RIG_QDX() {

	name_ = QDXname_;
	modes_ = QDXmodes_;
	bandwidths_ = QDX_BW;

	widgets = rig_widgets;

	serial_baudrate = BR9600;
	stopbits = 1;
	serial_retries = 2;
	serial_rtscts  = false;
	serial_rtsplus = false;
	serial_dtrplus = false;
	serial_rtsptt  = false;
	serial_dtrptt  = false;

	serial_write_delay = 0;
	serial_post_write_delay = 0;
	serial_timeout = 50;

	serial_catptt  = true;

	B.imode = A.imode = 1;
	B.iBW = A.iBW = 0;
	A.freq = 7070000ULL;
	B.freq = 14070000ULL;

	has_mode_control =
	has_extras =
	has_volume_control =
	has_ptt_control =
	has_vfo_adj =
	has_vox_onoff =
	has_rit =
	has_split =
	can_change_alt_vfo = true;

	precision = 1;
	ndigits = 8;

}

static int ret = 0;
static int split_ = 0;
static int rit_ = 0;
static int rit_stat_ = 0;

static char cmdstr[20];

void RIG_QDX::shutdown()
{
}

unsigned long long RIG_QDX::get_vfoA ()
{
	cmd = "FA;";
	get_trace(1, "get vfoA");
	if (wait_char(';', 14, 100, "get vfo A", ASC) < 14) return A.freq;
	gett("");

	size_t p = replystr.rfind("FA");

	if (p != std::string::npos) {
		sscanf(&replystr[p+2], "%llu", &A.freq);
	}
	return A.freq;
}

void RIG_QDX::set_vfoA (unsigned long long freq)
{
	A.freq = freq;
	snprintf(cmdstr, sizeof(cmdstr), "FA%llu;", A.freq);
	cmd = cmdstr;
	set_trace(1, "set vfoA");
	sendCommand(cmd);
	sett("");
	showresp(WARN, ASC, "set vfo A", cmd, "");
}

unsigned long long RIG_QDX::get_vfoB ()
{
	cmd = "FB;";
	get_trace(1, "get vfoB");
	if (wait_char(';', 14, 100, "get vfo B", ASC) < 14) return B.freq;
	gett("");

	size_t p = replystr.rfind("FB");

	if (p != std::string::npos) {
		sscanf(&replystr[p+2], "%llu", &B.freq);
	}
	return B.freq;
}

void RIG_QDX::set_vfoB (unsigned long long freq)
{
	B.freq = freq;
	snprintf(cmdstr, sizeof(cmdstr), "FB%llu;", B.freq);
	cmd = cmdstr;
	set_trace(1, "set vfoB");
	sendCommand(cmd);
	sett("");

	showresp(WARN, ASC, "set vfo B", cmd, "");
}

void RIG_QDX::selectA()
{
	cmd = "FR0;FT0;";
	set_trace(1, "select A");
	sendCommand(cmd);
	sett("");

	showresp(WARN, ASC, "Rx on A, Tx on A", cmd, "");
	inuse = onA;
}

void RIG_QDX::selectB()
{
	cmd = "FR1;FT1;";
	set_trace(1, "select B");
	sendCommand(cmd);
	sett("");

	showresp(WARN, ASC, "Rx on B, Tx on B", cmd, "");
	inuse = onB;
}

void RIG_QDX::set_split(bool val) 
{
	if (val) cmd = "SP1;";
	else     cmd = "SP0;";
	set_trace(1, "set split on/off");
	sendCommand(cmd);
	sett("");
	showresp(WARN, ASC, "set split", cmd, "");
	split_ = val;
}

bool RIG_QDX::can_split()
{
	return true;
}

int RIG_QDX::get_split()
{
	cmd = "SP;";
	get_trace(1, "get split");
	ret = wait_char(';', 4, 100, "get split", ASC);
	gett("");
	split_ = replystr[2] == '1';
	return split_;
}

void RIG_QDX::set_bwA(int val)
{
	A.iBW = 0;
}

int RIG_QDX::get_bwA()
{
	return A.iBW = 0;
}

void RIG_QDX::set_bwB(int val)
{
	B.iBW = 0;
}

int RIG_QDX::get_bwB()
{
	return B.iBW = 0;
}

// Tranceiver PTT on/off
void RIG_QDX::set_PTT_control(int val)
{
	if (val) cmd = "TQ1;";
	else     cmd = "TQ0;";
	set_trace(1, "set PTT");
	sendCommand(cmd);
	sett("");

	showresp(WARN, ASC, "set PTT", cmd, "");
	ptt_ = val;
}

int RIG_QDX::get_PTT()
{
	cmd = "TQ;";
	get_trace(1, "get PTT");
	ret = wait_char(';', 4, 100, "get PTT", ASC);
	gett("");
	if (ret < 4) return ptt_;
	return ptt_ = replystr[2] == '1';
}

/*======================================================================

IF; response

IFaaaaaaaaaaaXXXXXbbbbbcdXeefghjklmmX;
12345678901234567890123456789012345678
01234567890123456789012345678901234567 byte #
          1         2         3
                            ^ position 28
where:
	aaaaaaaaaaa => 11 digit decimal value of vfo frequency
	XXXXX => 5 spaces
	bbbbb => 5 digit RIT frequency, -0200 ... +0200
	c => 0 = rit OFF, 1 = rit ON
	d => xit off/on; always ZERO
	X    always ZERO
	ee => memory channel; always 00
	f => tx/rx; 0 = RX, 1 = TX
	g => mode; always 3 (CW)
	h => function; receive VFO; 0 = A, 1 = B
	j => scan off/on; always ZERO
	k => split off /on; 0 = Simplex, 1 = Split
	l => tone off /on; always ZERO
	m => tone number; always ZERO
	X => unused characters; always a SPACE
======================================================================*/ 

int RIG_QDX::get_IF()
{
	cmd = "IF;";
	get_trace(1, "get_PTT");
	ret = wait_char(';', 38, 100, "get VFO", ASC);
	gett("");

	if (ret < 38) return ptt_;

	rit_ = 0;
	for (int n = 22; n > 18; n--)
		rit_ = (10 * rit_) + (replystr[n] - '0');
	if (replystr[18] == '-') rit_ *= -1;
	rit_stat_ = (replystr[23] - '0');

	ptt_ = (replystr[28] == '1');

	split_ = (replystr[32] == '1');

	return ptt_;
}

void RIG_QDX::set_modeA(int val)
{
	if (val == 0) cmd = "MD1;"; // LSB
	else          cmd = "MD3;";
	set_trace(1, "set mode A");
	sendCommand(cmd);
	sett("");

	showresp(WARN, ASC, "set mode A", cmd, "");
	A.imode = val;
}

int RIG_QDX::get_modeA()
{
	cmd = "MD;";
	get_trace(1, "get_modeA");
	ret = wait_char(';', 4, 100, "get modeA", ASC);
	gett("");
	if (ret < 4) return A.imode;

	if (replystr[2] == '1') A.imode = 0; // LSB
	else                    A.imode = 1; // USB
	return A.imode;
}

void RIG_QDX::set_modeB(int val)
{
	if (val == 0) cmd = "MD1;"; // LSB
	else          cmd = "MD3;";
	B.imode = val;
	set_trace(1, "set mode B");
	sendCommand(cmd);
	sett("");

	showresp(WARN, ASC, "set mode B", cmd, "");
}

int RIG_QDX::get_modeB()
{
	cmd = "MD;";
	get_trace(1, "get_modeB");
	ret = wait_char(';', 4, 100, "get modeB", ASC);
	gett("");
	if (ret < 4) return B.imode;

	if (replystr[2] == '1') B.imode = 0; // LSB
	else                    B.imode = 1; // USB
	return B.imode;
}

void RIG_QDX::setVfoAdj(double v)
{
	if (v > 1000) v = 1000;
	if (v < -1000) v = -1000;
	vfo_ = v;//25000000 + (int)v;
	char cmdstr[12];
	snprintf(cmdstr, sizeof(cmdstr), "Q0%08.0f;", vfo_ + 25000000);
	cmd = cmdstr;
	set_trace(1, "set TCXO ref freq");
	sendCommand(cmd);
	sett("");
}

double RIG_QDX::getVfoAdj()
{
	cmd = "Q0;";
	get_trace(1, "get TCXO ref freq");
	ret = wait_char(';', 12, 100, "get TCXO ref freq", ASC);
	if (ret < 11) return vfo_;
	int vfo;
	sscanf( (&replystr[2]), "%d", &vfo);
	return vfo_ = vfo - 25000000;
}

void RIG_QDX::get_vfoadj_min_max_step(double &min, double &max, double &step)
{
	min = -1000; 
	max = 1000; 
	step = 1;
}

void RIG_QDX::set_vox_onoff()
{
	if (progStatus.vox_onoff) cmd = "Q41;";
	else                      cmd = "Q40;";
	set_trace(1, "set vox on/off");
	sendCommand(cmd);
	sett("");
	showresp(WARN, ASC, "SET vox gain", cmd, replystr);
}

int RIG_QDX::get_vox_onoff()
{
	cmd = "Q4;";
	get_trace(1, "get vox onoff");
	ret = wait_char(';', 4, 100, "get vox on/off", ASC);
	gett("");

	if (ret < 4) return progStatus.vox_onoff;
	progStatus.vox_onoff = (replystr[2] == '1');
	return progStatus.vox_onoff;
}

void RIG_QDX::set_volume_control(int val)
{
	cmd = "AG";
	char szval[20];
	snprintf(szval, sizeof(szval), "%02d", val);
	cmd += szval;
	cmd += ';';
	set_trace(1, "set vol control");
	sendCommand(cmd);
	sett("");
}

int RIG_QDX::get_volume_control()
{
	int val = progStatus.volume;
	cmd = "AG;";
	get_trace(1, "get vol control");
	ret = wait_char(';', 5, 100, "get vol", ASC);
	gett("");

	size_t p = replystr.rfind("AG");
	if (p == std::string::npos) return val;
	val = atoi(&replystr[p + 2]);
	return val;
}

void RIG_QDX::get_vol_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 99;
	step = 1;
}

void RIG_QDX::setRit(int val)
{
	if (val >= 0) snprintf(cmdstr, sizeof(cmdstr), "RU%u;", val);
	else if (val < 0) snprintf(cmdstr, sizeof(cmdstr), "RD-%u;", abs(val));
	rit_ = val;
	cmd = cmdstr;
	set_trace(1, "set RIT");
	sendCommand(cmd);
	sett("");
}

int RIG_QDX::getRit()
{
	get_IF();
	return rit_;
}

//**********************************************************************
/*
// SM cmd 0 ... 100 (rig values 0 ... 15)
int RIG_QDX::get_smeter()
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

struct pwrpair {int mtr; float pwr;};

static pwrpair pwrtbl[] = { 
	{0, 0.0}, 
	{2, 5.0},
	{4, 10.0}, 
	{7, 25.0}, 
	{11, 50.0}, 
	{16, 100.0}, 
	{20, 200.0} };

int RIG_QDX::get_power_out()
{
	int mtr = 0;
	cmd = "SM0;";
	get_trace(1, "get_power_out");
	ret = wait_char(';', 8, 100, "get power", ASC);
	gett("");
	if (ret < 8) return mtr;

	size_t p = replystr.rfind("SM");
	if (p != std::string::npos) {
		mtr = atoi(&replystr[p + 3]);

		size_t i = 0;
		for (i = 0; i < sizeof(pwrtbl) / sizeof(pwrpair) - 1; i++)
			if (mtr >= pwrtbl[i].mtr && mtr < pwrtbl[i+1].mtr)
				break;
		if (mtr < 0) mtr = 0;
		if (mtr > 20) mtr = 20;
		mtr = (int)ceil(pwrtbl[i].pwr + 
			(pwrtbl[i+1].pwr - pwrtbl[i].pwr)*(mtr - pwrtbl[i].mtr)/(pwrtbl[i+1].mtr - pwrtbl[i].mtr));
		if (mtr > 200) mtr = 200;void RIG_TT550::setRit(int val)
{
	progStatus.rit_freq = RitFreq = val;
	if (RitFreq) RitActive = true;
	if (inuse == onB) {
		set_vfoRX(freqB);
		set_vfoTX(freqB);
	} else if (split) {
		set_vfoRX(freqA);
		set_vfoTX(freqB);
	} else {
		set_vfoRX(freqA);
		set_vfoTX(freqA);
	}
}

int RIG_TT550::getRit()
{
	return RitFreq;
}

	}
	return mtr;
}

// RM cmd 0 ... 100 (rig values 0 ... 8)
// User report of RM; command using Send Cmd tab
// RM10000;RM20000;RM30000;
// RM1nnnn; => SWR
// RM2nnnn; => COMP
// RM3nnnn; => ALC

int RIG_QDX::get_swr()
{
	int mtr = 0;
	cmd = "RM;";
	get_trace(1, "get_swr");
	ret = wait_char(';', 8, 100, "get SWR/ALC", ASC);
	gett("");
	if (ret < 8) return (int)mtr;

	size_t p = replystr.rfind("RM1");
	if (p != std::string::npos)
		mtr = 66 * atoi(&replystr[p + 3]) / 10;
	p = replystr.rfind("RM3");
	if (p != std::string::npos)
		alc = 66 * atoi(&replystr[p+3]) / 10;
	else
		alc = 0;
	swralc_polled = true;
	return mtr;
}

int  RIG_QDX::get_alc(void) 
{
	if (!swralc_polled) get_swr();
	swralc_polled = false;
	return alc;
}

int RIG_QDX::set_widths(int val)
{
	int bw;
	if (val == 0 || val == 1 || val == 3) {
		if (menu_45) {
			bw = 0x8106; // 1500 Hz 2400 wide
			dsp_SL     = QDX_dataW;
			SL_tooltip = QDX_dataW_tooltip;
			SL_label   = QDX_dataW_label;
			dsp_SH     = QDX_dataC;
			SH_tooltip = QDX_dataC_tooltip;
			SH_label   = QDX_dataC_label;
			bandwidths_ = QDX_dataW;
			bw_vals_ = QDX_data_bw_vals;
		} else {
			bw = 0x8A03; // 200 ... 3000 Hz
			dsp_SL     = QDX_SL;
			SL_tooltip = QDX_SL_tooltip;
			SL_label   = QDX_btn_SL_label;
			dsp_SH     = QDX_SH;
			SH_tooltip = QDX_SH_tooltip;
			SH_label   = QDX_btn_SH_label;
			bandwidths_ = QDX_SH;
			bw_vals_ = QDX_HI_bw_vals;
		}
	} else if (val == 2 || val == 6) {
		bandwidths_ = QDX_CWwidths;
		bw_vals_ = QDX_CW_bw_vals;
		dsp_SL = QDX_empty;
		dsp_SH = QDX_empty;
		bw = 7;
	} else if (val == 5 || val == 7) {
		bandwidths_ = QDX_FSKwidths;
		bw_vals_ = QDX_FSK_bw_vals;
		dsp_SL = QDX_empty;
		dsp_SH = QDX_empty;
		bw = 1;
	} else { // val == 4 ==> AM
		bandwidths_ = QDX_empty;
		bw_vals_ = QDX_bw_vals;
		dsp_SL = QDX_AM_SL;
		dsp_SH = QDX_AM_SH;
		bw = 0x8201;
	}
	return bw;
}

const char **RIG_QDX::bwtable(int m)
{
	if (m == 0 || m == 1 || m == 3)
		return QDX_empty;
	else if (m == 2 || m == 6)
		return QDX_CWwidths;
	else if (m == 5 || m == 7)
		return QDX_FSKwidths;
//else AM m == 4
	return QDX_empty;
}

const char **RIG_QDX::lotable(int m)
{
	if (m == 0 || m == 1 || m == 3)
		return QDX_SL;
	else if (m == 2 || m == 6)
		return NULL;
	else if (m == 5 || m == 7)
		return NULL;
	return QDX_AM_SL;
}

const char **RIG_QDX::hitable(int m)
{
	if (m == 0 || m == 1 || m == 3)
		return QDX_SH;
	else if (m == 2 || m == 6)
		return NULL;
	else if (m == 5 || m == 7)
		return NULL;
	return QDX_AM_SH;
}

int RIG_QDX::get_modetype(int n)
{
	return _mode_type[n];
}


int RIG_QDX::adjust_bandwidth(int val)
{
	int bw = 0;
	if (val == 0 || val == 1 || val == 3)
		bw = 0x8A03;
	else if (val == 4)
		bw = 0x8201;
	else if (val == 2 || val == 6)
		bw = 7;
	else if (val == 5 || val == 7)
		bw = 1;
	return bw;
}

int RIG_QDX::def_bandwidth(int val)
{
	return adjust_bandwidth(val);
}


void RIG_QDX::set_power_control(double val)
{
	cmd = "PC";
	char szval[4];
	if (modeA == 4 && val > 50) val = 50; // AM mode limitation
	snprintf(szval, sizeof(szval), "%03d", (int)val);
	cmd += szval;
	cmd += ';';
	LOG_WARN("%s", cmd.c_str());
	sendCommand(cmd);
}

double RIG_QDX::get_power_control()
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

void RIG_QDX::set_attenuator(int val)
{
	if (val)	cmd = "RA01;";
	else		cmd = "RA00;";
	LOG_WARN("%s", cmd.c_str());
	set_trace(1, "set attenuator");
	sendCommand(cmd);
	sett("");
	atten_level = val;
}

int RIG_QDX::get_attenuator()
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

void RIG_QDX::set_preamp(int val)
{
	if (val)	cmd = "PA1;";
	else		cmd = "PA0;";
	LOG_WARN("%s", cmd.c_str());
	set_trace(1, "set preamp");
	sendCommand(cmd);
	sett("");
	preamp_level = val;
}

int RIG_QDX::get_preamp()
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

void RIG_QDX::set_if_shift(int val)
{
	cmd = "IS+";
	if (val < 0) cmd[2] = '-';
	cmd.append(to_decimal(abs(val),4)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set IF shift", cmd, "");
}

bool RIG_QDX::get_if_shift(int &val)
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

void RIG_QDX::get_if_min_max_step(int &min, int &max, int &step)
{
	if_shift_min = min = -1100;
	if_shift_max = max = 1100;
	if_shift_step = step = 10;
	if_shift_mid = 0;
}

// Noise Reduction (TS2000.cxx) NR1 only works; no NR2 and don' no why
void RIG_QDX::set_noise_reduction(int val)
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

int  RIG_QDX::get_noise_reduction()
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

void RIG_QDX::set_noise_reduction_val(int val)
{
	if (_noise_reduction_level == 0) return;
	if (_noise_reduction_level == 1) _nrval1 = val;
	else _nrval2 = val;

	cmd.assign("RL").append(to_decimal(val, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET_noise_reduction_val", cmd, "");
}

int  RIG_QDX::get_noise_reduction_val()
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

int  RIG_QDX::get_agc()
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

int RIG_QDX::incr_agc()
{
	if (fm_mode) return 0;
	agcval++;
	if (agcval == 4) agcval = 1;
	cmd.assign("GT00");
	cmd += (agcval + '0' - 1);
	cmd += ";";
	sendCommand(cmd);
	showresp(WARN, ASC, "SET agc", cmd, replystr);
	return agcval;
}


static const char *agcstrs[] = {"FM", "AGC", "FST", "SLO"};
const char *RIG_QDX::agc_label()
{
	if (fm_mode) return agcstrs[0];
	return agcstrs[agcval];
}

int  RIG_QDX::agc_val()
{
	if (fm_mode) return 0;
	return agcval;
}

// Auto Notch, beat canceller (TS2000.cxx) BC1 only, not BC2
void RIG_QDX::set_auto_notch(int v)
{
	cmd = v ? "BC1;" : "BC0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set auto notch", cmd, "");

}

int  RIG_QDX::get_auto_notch()
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
void RIG_QDX::set_noise(bool b)
{
	if (b)
		cmd = "NB1;";
	else
		cmd = "NB0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set NB", cmd, "");
}

int RIG_QDX::get_noise()
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


void RIG_QDX::set_rf_gain(int val)
{
	cmd = "RG";
	cmd.append(to_decimal(val,3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set rf gain", cmd, "");
}

int  RIG_QDX::get_rf_gain()
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

void RIG_QDX::get_rf_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

// Squelch (TS990.cxx)
void RIG_QDX::set_squelch(int val)
{
		cmd = "SQ0";
		cmd.append(to_decimal(abs(val),3)).append(";");
		sendCommand(cmd);
		showresp(INFO, ASC, "set squelch", cmd, "");
}

int  RIG_QDX::get_squelch()
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

void RIG_QDX::get_squelch_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 255; step = 1;
}

void RIG_QDX::set_mic_gain(int val)
{
	cmd = "MG";
	cmd.append(to_decimal(val,3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set mic gain", cmd, "");
}

int  RIG_QDX::get_mic_gain()
{
	int val = progStatus.mic_gain;
	cmd = "MG;";
	if (wait_char(';', 6, 100, "get mic gain", ASC) < 6) return val;

	size_t p = replystr.rfind("MG");
	if (p != std::string::npos)
		val = fm_decimal(replystr.substr(p+2), 3);
	return val;
}

void RIG_QDX::get_mic_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}


void RIG_QDX::tune_rig()
{
	cmd = "AC111;";
	LOG_WARN("%s", cmd.c_str());
	sendCommand(cmd);
}

*/
