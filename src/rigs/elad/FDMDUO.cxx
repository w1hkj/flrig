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

#include "elad/FDMDUO.h"
#include "support.h"

static const char FDMDUOname_[] = "FDM DUO";

enum { duoLSB, duoUSB, duoCW, duoFM, duoAM, duoCWR};

static const char *FDMDUOmodes_[] = {
		"LSB", "USB", "CW", "FM", "AM", "CW-R", NULL};
static const char FDMDUO_mode_chr[] =  { '1', '2', '3', '4', '5', '7' };
static const char FDMDUO_mode_type[] = { 'L', 'U', 'L', 'U', 'U', 'U' };

static const char *FDMDUO_empty[] = { "N/A", NULL };

static const char *FDMDUO_SSBwidths[] = {
"1600", "1700", "1800", "1900", "2000",
"2100", "2200", "2300", "2400", "2500",
"2600", "2700", "2800", "2900", "3000",
"3100", "4000", "5000", "6000", "D-300",
"D-600", "D-1000", NULL };

static std::string FDMDUO_LSBvals[] = {
"RF100;", "RF101;", "RF102;", "RF103;", "RF104;",
"RF105;", "RF106;", "RF107;", "RF108;", "RF109;",
"RF110;", "RF111;", "RF112;", "RF113;", "RF114;",
"RF115;", "RF116;", "RF117;", "RF118;", "RF119;",
"RF120;", "RF121;", "RF122;" };
static const int duoLSBvals = 23;

static std::string FDMDUO_USBvals[] = {
"RF200;", "RF201;", "RF202;", "RF203;", "RF204;",
"RF205;", "RF206;", "RF207;", "RF208;", "RF209;",
"RF210;", "RF211;", "RF212;", "RF213;", "RF214;",
"RF215;", "RF216;", "RF217;", "RF218;", "RF219;",
"RF220;", "RF221;", "RF222;" };
static const int duoUSBvals = 23;

static const char *FDMDUO_AMwidths[] = {
"2500", "3000", "3500", "4000", "4500",
"5000", "5500", "6000", NULL };

static std::string FDMDUO_AMvals[] = {
"RF500;", "RF501;", "RF502;", "RF503;", "RF504;",
"RF505;", "RF506;", "RF507;" };
static const int duoAMvals = 8;

static const char *FDMDUO_CWwidths[] = {
"100.4", "100.3", "100.2", "100.1", "100",
"300",   "500",   "1000",  "1500",  "2600", NULL};

static std::string FDMDUO_CWvals[] = {
"RF307;", "RF308;", "RF309;", "RF310;", "RF311;",
"RF312;", "RF313;", "RF314;", "RF315;", "RF316;" };

static const char *FDMDUO_CWRwidths[] = {
"100.4", "100.3", "100.2", "100.1", "100",
"300",   "500",   "1000",  "1500",  "2600", NULL};

static std::string FDMDUO_CWRvals[] = {
"RF707;", "RF708;", "RF709;", "RF710;", "RF711;",
"RF712;", "RF713;", "RF714;", "RF715;", "RF716;" };
static const int duoCWvals = 10;

static const char *FDMDUO_FMwidths[] = {
"Narrow", "Wide", "Data", NULL };

static std::string FDMDUO_FMvals[] = {
"RF400;", "RF401;", "RF402;" };
static const int duoFMvals = 3;

static int agcval = 1;
static bool fm_mode = false;

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 }, // 0
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 }, // 1
	{ (Fl_Widget *)sldrSQUELCH, 266, 125, 156 }, // 5
	{ (Fl_Widget *)sldrMICGAIN,  54, 145, 368 }, // 6
	{ (Fl_Widget *)sldrPOWER,    54, 165, 368 }, // 7
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

void RIG_FDMDUO::initialize()
{
	rig_widgets[0].W = btnVol;
	rig_widgets[1].W = sldrVOLUME;
	rig_widgets[2].W = sldrSQUELCH;
	rig_widgets[3].W = sldrMICGAIN;
	rig_widgets[4].W = sldrPOWER;
}

RIG_FDMDUO::RIG_FDMDUO() {
// base class values
	name_ = FDMDUOname_;
	modes_ = FDMDUOmodes_;
	_mode_type = FDMDUO_mode_type;
	bandwidths_ = FDMDUO_empty;

	widgets = rig_widgets;

	comm_baudrate = BR57600;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_rtscts = true;
	comm_rtsplus = false;
	comm_dtrplus = false;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
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
//	has_agc_control = true;
	has_swr_control = true;
	has_alc_control = true;
	has_power_out = true;
	has_dsp_controls = true;
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

	powerScale = 100; // displays 0 to 5 as 0 to 50 W
}

static int ret = 0;

const char * RIG_FDMDUO::get_bwname_(int n, int md) 
{
	const char *name = "NIL";
	switch (md) {
		case duoLSB:
			if (n < duoLSBvals) name = FDMDUO_SSBwidths[n];
			break;
		case duoUSB:
			if (n < duoUSBvals) name = FDMDUO_SSBwidths[n];
			break;
		case duoCW:
			if (n < duoCWvals) name = FDMDUO_CWwidths[n];
			break;
		case duoCWR:
			if (n < duoCWvals) name = FDMDUO_CWwidths[n];
			break;
		case duoAM:
			if (n < duoAMvals) name = FDMDUO_AMwidths[n];
			break;
		case duoFM:
			if (n < duoFMvals) name = FDMDUO_FMwidths[n];
			break;
	}
	return name;
}

void RIG_FDMDUO::shutdown()
{
}

// SM cmd 0 ... 100 (rig values 0 ... 22)
int RIG_FDMDUO::get_smeter()
{
	int mtr = 0;
	cmd = "SM0;";
	get_trace(1, "get_smeter");
	ret = wait_char(';', 8, 100, "get Smeter", ASC);
	gett("");
	if (ret < 8) return 0;

	size_t p = replystr.rfind("SM");
	if (p == std::string::npos)
		return mtr;
	mtr = 100 * atoi(&replystr[p + 3]) / 22;
	return mtr;
}

int RIG_FDMDUO::get_power_out()
{
	float mtr = 0;
	cmd = "FP;";
	get_trace(1, "get_power_out");
	ret = wait_char(';', 10, 100, "get power", ASC);
	gett("");
	if (ret < 10) return mtr;

	size_t p = replystr.rfind("FP");
	if (p != std::string::npos) {
		mtr = atoi(&replystr[p + 3]); // value is 0 to 6000; 0 to 6.0
	}
	return mtr;
}

int RIG_FDMDUO::power_scale()
{
	return powerScale;
}

int RIG_FDMDUO::get_swr()
{
	cmd = "WR;";
	get_trace(1, "get_swr");
	ret = wait_char(';', 10, 100, "get SWR/ALC", ASC);
	gett("");
	if (ret < 10) return 0;
	size_t p = replystr.rfind("WR");
	double swr = 0.0;
	if (p != std::string::npos)
		swr = atoi(&replystr[p+4]) + atoi(&replystr[p+7]) / 100.0;
	swr -= 1.0;
	if (swr < 0) swr = 0;
	swr *= 25;
	if (swr > 100) swr = 100;
	return int(swr);
}

int  RIG_FDMDUO::get_alc(void) 
{
	return 0;
}

int RIG_FDMDUO::set_widths(int val)
{
	int bw = 0;
	switch (val) {
		case duoLSB: 
			bandwidths_ = FDMDUO_SSBwidths;
			bw = 14;
			break;
		case duoUSB:
			bandwidths_ = FDMDUO_SSBwidths;
			bw = 14;
			break;
		case duoCW:
			bandwidths_ = FDMDUO_CWwidths;
			bw = 6;
			break;
		case duoCWR:
			bandwidths_ = FDMDUO_CWRwidths;
			bw = 6;
			break;
		case duoAM:
			bandwidths_ = FDMDUO_AMwidths;
			bw = 5;
			break;
		case duoFM:
			bandwidths_ = FDMDUO_FMwidths;
			bw = 1;
			break;
	}
	return bw;
}

const char **RIG_FDMDUO::bwtable(int m)
{
	switch (m) {
		case duoLSB: 
			return FDMDUO_SSBwidths;
		case duoUSB:
			return FDMDUO_SSBwidths;
		case duoCW:
			return FDMDUO_CWwidths;
		case duoCWR:
			return FDMDUO_CWRwidths;
		case duoAM:
			return FDMDUO_AMwidths;
		case duoFM:
			return FDMDUO_FMwidths;
	}
	return FDMDUO_SSBwidths;
}

void RIG_FDMDUO::set_modeA(int val)
{
	A.imode = val;
	cmd = "MD";
	cmd += FDMDUO_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "set mode", cmd, "");
	A.iBW = set_widths(val);
}

int RIG_FDMDUO::get_modeA()
{
	cmd = "MD;";
	get_trace(1, "get_modeA");
	ret = wait_char(';', 4, 100, "get modeA", ASC);
	gett("");
	if (ret < 4) return A.imode;

	size_t p = replystr.rfind("MD");
	if (p != std::string::npos && (p + 2 < replystr.length())) {
		int md = replystr[p+2];
		md = md - '1';
		if (md == 6) md = 5;
		A.imode = md;
		A.iBW = set_widths(A.imode);
	}
	if (A.imode == 3) fm_mode = true;
	else fm_mode = false;
	return A.imode;
}

void RIG_FDMDUO::set_modeB(int val)
{
	if (val == 3) fm_mode = true;
	else fm_mode = false;
	B.imode = val;
	cmd = "MD";
	cmd += FDMDUO_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "set mode B", cmd, "");
	B.iBW = set_widths(val);
}

int RIG_FDMDUO::get_modeB()
{
	cmd = "MD;";
	get_trace(1, "get_modeB");
	ret = wait_char(';', 4, 100, "get modeB", ASC);
	gett("");
	if (ret < 4) return B.imode;

	size_t p = replystr.rfind("MD");
	if (p != std::string::npos && (p + 2 < replystr.length())) {
		int md = replystr[p+2];
		md = md - '1';
		if (md == 6) md = 5;
		B.imode = md;
		B.iBW = set_widths(B.imode);
	}
	if (B.imode == 3) fm_mode = true;
	else fm_mode = false;
	return B.imode;
}

int RIG_FDMDUO::get_modetype(int n)
{
	return _mode_type[n];
}

void RIG_FDMDUO::set_bwA(int val)
{
	switch (A.imode) {
		case duoLSB: cmd.assign(FDMDUO_LSBvals[val > -1 ? (val < duoLSBvals ? val : 0) : 0]); break;
		case duoUSB: cmd.assign(FDMDUO_USBvals[val > -1 ? (val < duoUSBvals ? val : 0) : 0]); break;
		case duoCW:  cmd.assign(FDMDUO_CWvals[val > -1 ? (val < duoCWvals ? val : 0) : 0]); break;
		case duoFM:  cmd.assign(FDMDUO_FMvals[val > -1 ? (val < duoFMvals ? val : 0) : 0]); break;
		case duoAM:  cmd.assign(FDMDUO_AMvals[val > -1 ? (val < duoAMvals ? val : 0) : 0]); break;
		case duoCWR: cmd.assign(FDMDUO_CWRvals[val > -1 ? (val < duoCWvals ? val : 0) : 0]); break;
	}
	set_trace(1, "set bwA");
	sendCommand(cmd);
	showresp(WARN, ASC, "set bwA", cmd, "");
}

int RIG_FDMDUO::get_bwA()
{
	int i = 0;
	size_t p;

	if (A.imode == duoLSB) {
		cmd = "RF1;";
		get_trace(1, "get bwA");
		ret = wait_char(';', 6, 100, "get LSB bw", ASC);
		gett("");
		if (ret >= 6) {
			p = replystr.rfind("RF");
			for (i = 0; i < duoLSBvals; i++) {
				if (FDMDUO_LSBvals[i] == replystr.substr(p)) {
					A.iBW = i;
					break;
				}
			}
		}
	} else if (A.imode == duoUSB) {
		cmd = "RF2;";
		get_trace(1, "get bwA");
		ret = wait_char(';', 6, 100, "get USB bw", ASC);
		gett("");
		if (ret >= 6) {
			p = replystr.rfind("RF");
			for (i = 0; i < duoUSBvals; i++) {
				if (FDMDUO_USBvals[i] == replystr.substr(p)) {
					A.iBW = i;
					break;
				}
			}
		}
	} else if (A.imode == duoCW) {
		cmd = "RF3;";
		get_trace(1, "get CW bw");
		ret = wait_char(';', 6, 100, "get RF", ASC);
		gett("");
		if (ret >= 6) {
			p = replystr.rfind("RF");
			if (p != std::string::npos) {
				for (i = 0; i < duoCWvals; i++)
					if (FDMDUO_CWvals[i] == replystr.substr(p))
						break;
				A.iBW = i;
			}
		}
	} else if (A.imode == duoCWR) {
		cmd = "RF4;";
		get_trace(1, "get CWR bw");
		ret = wait_char(';', 6, 100, "get RF", ASC);
		gett("");
		if (ret >= 6) {
			p = replystr.rfind("RF");
			if (p != std::string::npos) {
				for (i = 0; i < duoCWvals; i++)
					if (FDMDUO_CWRvals[i] == replystr.substr(p))
						break;
				A.iBW = i;
			}
		}
	} else if (A.imode == duoAM) {
		cmd = "RF5;";
		get_trace(1, "get AM bw");
		ret = wait_char(';', 6, 100, "get RF", ASC);
		gett("");
		if (ret >= 6) {
			p = replystr.rfind("RF");
			if (p != std::string::npos) {
				for (i = 0; i < duoAMvals; i++)
					if (FDMDUO_AMvals[i] == replystr.substr(p))
						break;
				A.iBW = i;
			}
		}
	} else if (A.imode == duoFM) {
		cmd = "RF5;";
		get_trace(1, "get FM bw");
		ret = wait_char(';', 6, 100, "get RF", ASC);
		gett("");
		if (ret >= 6) {
			p = replystr.rfind("RF");
			if (p != std::string::npos) {
				for (i = 0; i < duoFMvals; i++)
					if (FDMDUO_FMvals[i] == replystr.substr(p))
						break;
				A.iBW = i;
			}
		}
	}

	return A.iBW;
}

void RIG_FDMDUO::set_bwB(int val)
{
	switch (B.imode) {
		case duoLSB: cmd.assign(FDMDUO_LSBvals[val > -1 ? (val < duoLSBvals ? val : 0) : 0]); break;
		case duoUSB: cmd.assign(FDMDUO_USBvals[val > -1 ? (val < duoUSBvals ? val : 0) : 0]); break;
		case duoCW:  cmd.assign(FDMDUO_CWvals[val > -1 ? (val < duoCWvals ? val : 0) : 0]); break;
		case duoFM:  cmd.assign(FDMDUO_FMvals[val > -1 ? (val < duoFMvals ? val : 0) : 0]); break;
		case duoAM:  cmd.assign(FDMDUO_AMvals[val > -1 ? (val < duoAMvals ? val : 0) : 0]); break;
		case duoCWR: cmd.assign(FDMDUO_CWRvals[val > -1 ? (val < duoCWvals ? val : 0) : 0]); break;
	}
	set_trace(1, "set bwB");
	sendCommand(cmd);
	showresp(WARN, ASC, "set bwB", cmd, "");
}

int RIG_FDMDUO::get_bwB()
{
	int i = 0;
	size_t p;

	if (B.imode == duoLSB) {
		cmd = "RF1;";
		get_trace(1, "get bwB");
		ret = wait_char(';', 6, 100, "get LSB bw", ASC);
		gett("");
		if (ret >= 6) {
			p = replystr.rfind("RF");
			for (i = 0; i < duoLSBvals; i++) {
				if (FDMDUO_LSBvals[i] == replystr.substr(p)) {
					B.iBW = i;
					break;
				}
			}
		}
	} else if (B.imode == duoUSB) {
		cmd = "RF2;";
		get_trace(1, "get bwB");
		ret = wait_char(';', 6, 100, "get USB bw", ASC);
		gett("");
		if (ret >= 6) {
			p = replystr.rfind("RF");
			for (i = 0; i < duoUSBvals; i++) {
				if (FDMDUO_USBvals[i] == replystr.substr(p)) {
					B.iBW = i;
					break;
				}
			}
		}
	} else if (B.imode == duoCW) {
		cmd = "RF3;";
		get_trace(1, "get CW bw");
		ret = wait_char(';', 6, 100, "get RF", ASC);
		gett("");
		if (ret >= 6) {
			p = replystr.rfind("RF");
			if (p != std::string::npos) {
				for (i = 0; i < duoCWvals; i++)
					if (FDMDUO_CWvals[i] == replystr.substr(p))
						break;
				B.iBW = i;
			}
		}
	} else if (B.imode == duoCWR) {
		cmd = "RF4;";
		get_trace(1, "get CWR bw");
		ret = wait_char(';', 6, 100, "get RF", ASC);
		gett("");
		if (ret >= 6) {
			p = replystr.rfind("RF");
			if (p != std::string::npos) {
				for (i = 0; i < duoCWvals; i++)
					if (FDMDUO_CWRvals[i] == replystr.substr(p))
						break;
				B.iBW = i;
			}
		}
	} else if (B.imode == duoAM) {
		cmd = "RF5;";
		get_trace(1, "get AM bw");
		ret = wait_char(';', 6, 100, "get RF", ASC);
		gett("");
		if (ret >= 6) {
			p = replystr.rfind("RF");
			if (p != std::string::npos) {
				for (i = 0; i < duoAMvals; i++)
					if (FDMDUO_AMvals[i] == replystr.substr(p))
						break;
				B.iBW = i;
			}
		}
	} else if (B.imode == duoFM) {
		cmd = "RF5;";
		get_trace(1, "get FM bw");
		ret = wait_char(';', 6, 100, "get RF", ASC);
		gett("");
		if (ret >= 6) {
			p = replystr.rfind("RF");
			if (p != std::string::npos) {
				for (i = 0; i < duoFMvals; i++)
					if (FDMDUO_FMvals[i] == replystr.substr(p))
						break;
				B.iBW = i;
			}
		}
	}

	return B.iBW;
}

int RIG_FDMDUO::adjust_bandwidth(int val)
{
	int bw = 0;
	switch (val) {
		case duoLSB: 
			bw = 14;
			break;
		case duoUSB:
			bw = 14;
			break;
		case duoCW:
			bw = 6;
			break;
		case duoCWR:
			bw = 6;
			break;
		case duoAM:
			bw = 5;
			break;
		case duoFM:
			bw = 1;
			break;
	}
	return bw;
}

int RIG_FDMDUO::def_bandwidth(int val)
{
	return adjust_bandwidth(val);
}


void RIG_FDMDUO::set_power_control(double val)
{
	char szval[8];
	snprintf(szval, sizeof(szval), "TQ%04d;", (int)(val * 1000));
	cmd = szval;
	set_trace(1, "set power control ");
	sendCommand(cmd);
	sett("");
	LOG_WARN("%s", cmd.c_str());
}

double RIG_FDMDUO::get_power_control()
{
	double val = progStatus.power_level;
	cmd = "TQ;";
	get_trace(1, "get_power_control");
	ret = wait_char(';', 7, 100, "get Power control", ASC);
	gett("");
	if (ret < 7) return val;

	size_t p = replystr.rfind("TQ");
	if (p == std::string::npos) return val;

	val = atoi(&replystr[p + 2]);

	return val / 1000.0;
}

void RIG_FDMDUO::set_attenuator(int val)
{
	if (val)	cmd = "RA01;";
	else		cmd = "RA00;";
	LOG_WARN("%s", cmd.c_str());
	set_trace(1, "set attenuator");
	sendCommand(cmd);
	sett("");
	atten_level = val;
}

int RIG_FDMDUO::get_attenuator()
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

void RIG_FDMDUO::set_preamp(int val)
{
	if (val)	cmd = "PA1;";
	else		cmd = "PA0;";
	LOG_WARN("%s", cmd.c_str());
	set_trace(1, "set preamp");
	sendCommand(cmd);
	sett("");
	preamp_level = val;
}

int RIG_FDMDUO::get_preamp()
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

// Noise Reduction (TS2000.cxx) NR1 only works; no NR2 and don' no why
void RIG_FDMDUO::set_noise_reduction(int val)
{
}

int  RIG_FDMDUO::get_noise_reduction()
{
	return 0;
}

void RIG_FDMDUO::set_noise_reduction_val(int val)
{
}

int  RIG_FDMDUO::get_noise_reduction_val()
{
	int nrval = 0;
	return nrval;
}

int  RIG_FDMDUO::get_agc()
{
	return 0;
}

int RIG_FDMDUO::incr_agc()
{
	return 0;
}


static const char *agcstrs[] = {"FM", "AGC", "FST", "SLO"};
const char *RIG_FDMDUO::agc_label()
{
	return agcstrs[1];
}

int  RIG_FDMDUO::agc_val()
{
	return agcval;
}

// Auto Notch, beat canceller (TS2000.cxx) BC1 only, not BC2
void RIG_FDMDUO::set_auto_notch(int v)
{
}

int  RIG_FDMDUO::get_auto_notch()
{
	return 0;
}

// Noise Blanker (TS2000.cxx)
void RIG_FDMDUO::set_noise(bool b)
{
}

int RIG_FDMDUO::get_noise()
{
	return 0;
}

// Tranceiver PTT on/off
void RIG_FDMDUO::set_PTT_control(int val)
{
	if (val) cmd = "TX1;";
	else cmd = "RX;";
	set_trace(1, "set PTT");
	sendCommand(cmd);
	sett("");
	showresp(WARN, ASC, "set PTT", cmd, "");
}

int RIG_FDMDUO::get_PTT()
{
	cmd = "IF;";
	get_trace(1, "get_PTT");
	ret = wait_char(';', 38, 100, "get VFO", ASC);
	gett("");
	ptt_ = (replystr[28] == '1');
	return ptt_;
}

void RIG_FDMDUO::selectA()
{
	cmd = "FR0;FT0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Rx on A, Tx on A", cmd, "");
	inuse = onA;
}

void RIG_FDMDUO::selectB()
{
	cmd = "FR1;FT1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Rx on B, Tx on B", cmd, "");
	inuse = onB;
}

void RIG_FDMDUO::set_split(bool val) 
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

bool RIG_FDMDUO::can_split()
{
	return true;
}

int RIG_FDMDUO::get_split()
{
	return 0;
}

unsigned long int RIG_FDMDUO::get_vfoA ()
{
	cmd = "FA;";

	get_trace(1, "get_vfoA");
	ret = wait_char(';', 14, 100, "get vfo A", ASC);
	gett("");

	if (ret < 14) return A.freq;

	size_t p = replystr.rfind("FA");
	if (p != std::string::npos && (p + 12 < replystr.length())) {
		int f = 0;
		for (size_t n = 2; n < 13; n++)
			f = f*10 + replystr[p+n] - '0';
		A.freq = f;
	}
	return A.freq;
}

void RIG_FDMDUO::set_vfoA (unsigned long int freq)
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

unsigned long int RIG_FDMDUO::get_vfoB ()
{
	cmd = "FB;";

	get_trace(1, "get_vfoB");
	ret = wait_char(';', 14, 100, "get vfo A", ASC);
	gett("");

	if (ret < 14) return A.freq;

	size_t p = replystr.rfind("FB");
	if (p != std::string::npos && (p + 12 < replystr.length())) {
		int f = 0;
		for (size_t n = 2; n < 13; n++)
			f = f*10 + replystr[p+n] - '0';
		B.freq = f;
	}
	return B.freq;
}

void RIG_FDMDUO::set_vfoB (unsigned long int freq)
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
void RIG_FDMDUO::set_squelch(int val)
{
	char szcmd[8];
	snprintf(szcmd, sizeof(szcmd), "SQ0%03d;", val);
	cmd = szcmd;
	sendCommand(cmd);
	showresp(INFO, ASC, "set squelch", cmd, "");
}

int  RIG_FDMDUO::get_squelch()
{
	int val = 0;
	cmd = "SQ0;";
	if (wait_char(';', 7, 20, "get squelch", ASC) >= 7) {
		size_t p = replystr.rfind("SQ0");
		if (p == std::string::npos) return val;
		sscanf(&replystr[3], "%d", &val);
	}
	return val;
}

void RIG_FDMDUO::get_squelch_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 10; step = 1;
}

void RIG_FDMDUO::set_mic_gain(int val)
{
	char szcmd[8];
	snprintf(szcmd, sizeof(szcmd), "MG%03d;", val);
	cmd = szcmd;
	sendCommand(cmd);
	showresp(WARN, ASC, "set mic gain", cmd, "");
}

int  RIG_FDMDUO::get_mic_gain()
{
	int val = progStatus.mic_gain;
	cmd = "MG;";
	if (wait_char(';', 6, 100, "get mic gain", ASC) < 6) return val;

	size_t p = replystr.rfind("MG");
	if (p != std::string::npos)
		val = fm_decimal(replystr.substr(p+2), 3);
	return val;
}

void RIG_FDMDUO::get_mic_min_max_step(int &min, int &max, int &step)
{
	min = 26; max = 74; step = 1;
}


void RIG_FDMDUO::set_volume_control(int val)
{
	char szval[20];
	if (val > 15) {
		val = 5 * roundf(val / 5.0);
	}
	snprintf(szval, sizeof(szval), "VM%03d;", val);
	cmd = szval;
	set_trace(1, "set volume control");
	sendCommand(cmd);
	sett("");
}

int RIG_FDMDUO::get_volume_control()
{
	int val = progStatus.volume;
	cmd = "VM;";
	get_trace(1, "get volume control");
	ret = wait_char(';', 6, 100, "get vol", ASC);
	gett("");
	if (ret < 6) return val;
	size_t p = replystr.rfind("VM");
	if (p == std::string::npos) return val;
	replystr[p + 5] = 0;
	val = atoi(&replystr[p + 2]);
	return val;
}

void RIG_FDMDUO::tune_rig()
{
	cmd = "TX2;";
	set_trace(1, "set TUNE");
	sendCommand(cmd);
	sett("");
	showresp(WARN, ASC, "set PTT", cmd, "");

}

