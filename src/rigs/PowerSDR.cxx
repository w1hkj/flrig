// ----------------------------------------------------------------------------
// Copyright (C) 2014, 2020
//              Michael Black W9MDB
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

#include "PowerSDR.h"
#include "support.h"
#include <sstream>

static const char PowerSDRname_[] = "PowerSDR";

static const char *PowerSDRmodes_[] = {
	"LSB", "USB", "DSB", "CWL", "CWU", "FM", "AM", "DIGU", "SPEC", "DIGL", "SAM", "DRM", NULL};

static const char *PowerSDR_mode_chr[] =  { "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", NULL };
static const char PowerSDR_mode_type[] = { 'L', 'U', 'U', 'L', 'U', 'U', 'U', 'U', 'U', 'L', 'U', 'U' };

static const char *PowerSDR_empty[] = { NULL, NULL };
//------------------------------------------------------------------------
static const char *PowerSDR_USBwidths[] = {
"1000", "1800", "2100", "2400", "2700",
"2900", "3300", "3800", "4400", "5000",
"Var1", "Var2", "Wideband", NULL };
static const char *PowerSDR_CAT_USB[] = {
"ZZFI09;", "ZZFI08;", "ZZFI07;", "ZZFI06;", "ZZFI05;",
"ZZFI04;", "ZZFI03;", "ZZFI02;", "ZZFI01;", "ZZFI00;",
"ZZFI10;", "ZZFI11;", "ZZFI12;", NULL };
//static const char *PowerSDR_SH_tooltip = "hi cut";
//static const char *PowerSDR_SSB_btn_SH_label = "H";
//------------------------------------------------------------------------
static const char *PowerSDR_WIDEwidths[] = {
"Wideband" };
//static const char *PowerSDR_CAT_WIDE[] = {
//"ZZFI12;" };
//------------------------------------------------------------------------
static const char *PowerSDR_DIGwidths[] = {
"75", "150", "300", "600", "800",
"1000", "1500", "2000", "2500", "3000",
"Var1", "Var2", NULL };
static const char *PowerSDR_CAT_DIG[] = {
"ZZFI09;", "ZZFI08;", "ZZFI07;", "ZZFI06;", "ZZFI05;",
"ZZFI04;", "ZZFI03;", "ZZFI02;", "ZZFI01;", "ZZFI00;",
"ZZFI10;", "ZZFI11;", NULL };
//------------------------------------------------------------------------------
static const char *PowerSDR_AMwidths[] = {
"2400", "2900", "3100", "4000" "5200",
"6600", "8000", "10000", "12000", "16000",
"Var1", "Var2", NULL };
static const char *PowerSDR_CAT_AM[] = {
"ZZFI09;", "ZZFI08;", "ZZFI07;", "ZZFI06;", "ZZFI05;",
"ZZFI04;", "ZZFI03;", "ZZFI02", "ZZFI01;", "ZZFI00;",
"ZZFI10;", "ZZFI11;", NULL };
//------------------------------------------------------------------------------
static const char *PowerSDR_CWwidths[] = {
"25", "50", "100", "150", "250",
"400", "500", "600", "800", "1000",
"Var1", "Var2", NULL};
static const char *PowerSDR_CAT_CW[] = {
"ZZFI09;", "ZZFI08;", "ZZFI07;", "ZZFI06;", "ZZFI05;",
"ZZFI04;", "ZZFI03;", "ZZFI02;", "ZZFI01;", "ZZFI00;",
"ZZFI10;", "ZZFI11", NULL };
//------------------------------------------------------------------------------

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 }, // 0
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 }, // 1
	{ (Fl_Widget *)sldrRFGAIN,   54, 145, 156 }, // 2
	{ (Fl_Widget *)btnIFsh,     214, 105,  50 }, // 3
	{ (Fl_Widget *)sldrIFSHIFT, 266, 105, 156 }, // 4
	{ (Fl_Widget *)btnNotch,    214, 125,  50 }, // 5
	{ (Fl_Widget *)sldrNOTCH,   266, 125, 156 }, // 6
	{ (Fl_Widget *)sldrSQUELCH, 266, 145, 156 }, // 7
	{ (Fl_Widget *)sldrMICGAIN, 266, 165, 156 }, // 8
	{ (Fl_Widget *)sldrPOWER,    54, 165, 368 }, // 9
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

// mid range on loudness
static string menu012 = "EX01200004";

void RIG_PowerSDR::initialize()
{
	rig_widgets[0].W = btnVol;
	rig_widgets[1].W = sldrVOLUME;
	rig_widgets[2].W = sldrRFGAIN;
	rig_widgets[3].W = btnIFsh;
	rig_widgets[4].W = sldrIFSHIFT;
	rig_widgets[5].W = btnNotch;
	rig_widgets[6].W = sldrNOTCH;
	rig_widgets[7].W = sldrSQUELCH;
	rig_widgets[8].W = sldrMICGAIN;
	rig_widgets[9].W = sldrPOWER;

	menu012.clear();
	cmd = "EX0120000;"; // read menu 012 state
//might return something like EX01200004;

	if (wait_char(';', 11, 100, "read ex 012", ASC) == 11)
		menu012 = replystr;

// disable beeps before resetting front panel display to SWR
	cmd = "EX01200000;";
	sendCommand(cmd);
	sett("No beeps");
	select_swr();

// restore state of xcvr beeps
//	cmd = menu012;
//	sendCommand(cmd);

// get current noise reduction values for NR1 and NR2
	string current_nr;
	cmd = "NR;";
	if (wait_char(';', 4, 100, "read current NR", ASC) == 4)
		current_nr = replystr;
	gett("get NR");
	cmd = "RL;";
	if (wait_char(';', 5, 100, "GET noise reduction val", ASC) == 5) {
		size_t p = replystr.rfind("RL");
		if (p != string::npos)
			_nrval1 = atoi(&replystr[p+2]);
	}

// restore xcvr setting for NR
	cmd = current_nr;
	sendCommand(cmd);
}

void RIG_PowerSDR::shutdown()
{
// restore state of xcvr beeps
	if (menu012.empty()) return;
	cmd = menu012;
	sendCommand(cmd);
	sett("restore beeps");
}

static bool is_tuning = false;

RIG_PowerSDR::RIG_PowerSDR() {
// base class values
	name_ = PowerSDRname_;
	modes_ = PowerSDRmodes_;
	bandwidths_ = PowerSDR_empty;

	//dsp_SL     = PowerSDR_SL;
	//SL_tooltip = PowerSDR_SL_tooltip;
	//SL_label   = PowerSDR_SSB_btn_SL_label;

	//dsp_SH     = PowerSDR_SH;
	//SH_tooltip = PowerSDR_SH_tooltip;
	//SH_label   = PowerSDR_SSB_btn_SH_label;

	widgets = rig_widgets;

	comm_baudrate = BR4800;
	stopbits = 2;
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
	B.iBW = A.iBW = 0x8803;
	B.freq = A.freq = 14070000;
	can_change_alt_vfo = true;

	//has_dsp_controls = true;
	has_power_out = true;
	has_swr_control = true;
	has_alc_control =
	has_split =
	has_split_AB =
	has_rf_control =
	has_notch_control =
	has_auto_notch =
	has_ifshift_control =
	has_smeter =
	has_noise_reduction = true;
	has_noise_reduction_control =
	has_noise_control =
	has_micgain_control =
	has_volume_control =
	has_power_control =
	has_tune_control =
	has_preamp_control =
	has_mode_control =
	has_bandwidth_control =
	has_sql_control =
	has_ptt_control = 
	has_extras = true;

	rxona = true;

	precision = 1;
	ndigits = 9;

	att_level = 0;
	preamp_level = 0;
	_noise_reduction_level = 0;
	_nrval1 = 2;
	_nrval2 = 4;

	is_tuning = false;
}


const char * RIG_PowerSDR::get_bwname_(int n, int md)
{
	static char bwname[20];
	stringstream str;
	str << "n=" << n << ", md=" << md;
	trace(2, __func__, str.str().c_str());
	if (md == USB || md == LSB) {
		snprintf(bwname, sizeof(bwname), "%s", PowerSDR_USBwidths[n]);
	}
	else if (md == FM || md == DRM || md == SPEC) {
		snprintf(bwname, sizeof(bwname), "%s", PowerSDR_WIDEwidths[n]);
	}
	else if (md == DIGU || md == DIGL) {
		snprintf(bwname, sizeof(bwname), "%s", PowerSDR_DIGwidths[n]);
	}
	else if (md == CWU || md == CWL) {
		snprintf(bwname, sizeof(bwname), "%s", PowerSDR_CWwidths[n]);
	} else {
		snprintf(bwname, sizeof(bwname), "%s", PowerSDR_AMwidths[n]);
	}
	return bwname;
}

int RIG_PowerSDR::get_smeter()
{
	int smtr = 0;
	if (rxona)
		cmd = "ZZSM0;";
	else
		cmd = "ZZSM1;";
	int w = wait_char(';', 9, 100, "get smeter", ASC);
	if (w == 9) {
		size_t p = replystr.rfind("SM");
		if (p != string::npos) {
			smtr = fm_decimal(replystr.substr(p+3),3);
			smtr = -54 + smtr/(256.0/100.0); // in S-Units
			smtr = (smtr + 54);
		}
	}
	gett("smeter");
	return smtr;
}

// Transceiver power level
void RIG_PowerSDR::set_power_control(double val)
{
	int ival = (int)val;
	cmd = "PC";
	cmd.append(to_decimal(ival, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set pwr ctrl", cmd, "");
	sett("pwr control");
}

int RIG_PowerSDR::get_power_out()
{
	int mtr = 0;

	cmd = "ZZRM5;";
	if (wait_char(';', 8, 100, "get power", ASC) < 8) return mtr;
	sscanf(&replystr[0],"ZZRM5%d", &mtr);

	return mtr;
}


int RIG_PowerSDR::get_power_control()
{
	int pctrl = 0;
	cmd = "PC;";
	if (wait_char(';', 6, 100, "get pout", ASC) == 6) {
		size_t p = replystr.rfind("PC");
		if (p != string::npos) {
			pctrl = fm_decimal(replystr.substr(p+2), 3);
		}
	}
	gett("power control");
	return pctrl;
}

struct meterpair {float mtr; float val;};

static meterpair swr_tbl[] = {
	{ 1,   0  },
	{ 1.5,  12.5  },
	{ 2,  25 },
	{ 3,  50 },
	{ 20, 100 }
};

int RIG_PowerSDR::get_swr()
{
	double mtr = 0;

	cmd = "ZZRM8;";
	if (wait_char(';', 8, 100, "get SWR", ASC) < 8) return (int)mtr;
	if(sscanf(&replystr[0], "ZZRM8%lf", &mtr)!=1)
	{
		return 0;
	}
	size_t i = 0;
	for (i = 0; i < sizeof(swr_tbl) / sizeof(meterpair) - 1; i++)
		if (mtr >= swr_tbl[i].mtr && mtr < swr_tbl[i+1].mtr)
			break;
	if (mtr > 19) mtr = 19;
	mtr = (int)round(swr_tbl[i].val +
		(swr_tbl[i+1].val - swr_tbl[i].val)*(mtr - swr_tbl[i].mtr)/(swr_tbl[i+1].mtr - swr_tbl[i].mtr));
	if (mtr > 100) mtr = 100;

	return mtr;
}

int RIG_PowerSDR::get_alc()
{
	double alc = 0;
	cmd = "ZZRM4;";
	if (wait_char(';', 8, 100, "get ALC", ASC) < 8) return (int)alc;
	if (sscanf(&replystr[0], "ZZRM4%lf", &alc) != 1) alc=0;
	return alc;
}

void RIG_PowerSDR::set_preamp(int val)
{
	preamp_level = val;
	if (val) cmd = "ZZPA1;";
	else     cmd = "ZZPA0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set PRE", cmd, "");
	sett("preamp");
}

int RIG_PowerSDR::get_preamp()
{
	cmd = "ZZPA;";
	stringstream str;
	str << "ZZPA #1";
	trace(2, __func__, str.str().c_str());
	sendCommand(cmd);
	if (wait_char(';', 6, 100, "get PRE", ASC) != 6) {
		size_t p = replystr.rfind("PA");
	str << "ZZPA #2 replystr=" << replystr << ", p=" << p;
	trace(2, "get_preamp", replystr.c_str());
		if (p != string::npos && (p+2 < replystr.length())) {
			if (replystr[p+2] == '1')
				preamp_level = 1;
			else
				preamp_level = 0;
		}
	}
	else preamp_level = 0;
	gett("preamp");
	return preamp_level;
}

int RIG_PowerSDR::set_widths(int val)
{
	int bw = get_bwA();
	stringstream str;
	str << bw;
	trace(2, "set_widths bw=", str.str().c_str());
	switch (val) {
	case LSB: case USB:
		bandwidths_ = PowerSDR_USBwidths;
		dsp_SL = PowerSDR_empty;
		dsp_SH = PowerSDR_empty;
		//dsp_SL = PowerSDR_SL;
		//SL_tooltip = PowerSDR_SL_tooltip;
		//SL_label   = PowerSDR_SSB_btn_SL_label;
		//dsp_SH = PowerSDR_SH;
		//SH_tooltip = PowerSDR_SH_tooltip;
		//SH_label   = PowerSDR_SSB_btn_SH_label;
		//bw = 0;
		break;
	case DIGU: case DIGL:
		bandwidths_ = PowerSDR_DIGwidths;
		dsp_SL = PowerSDR_empty;
		dsp_SH = PowerSDR_empty;
		//bw = 9;
		break;
	case FM: case SPEC: case DRM:
		bandwidths_ = PowerSDR_WIDEwidths;
		dsp_SL = PowerSDR_empty;
		dsp_SH = PowerSDR_empty;
		//bw = 0;
		break;
	case CWL: case CWU:
		bandwidths_ = PowerSDR_CWwidths;
		dsp_SL = PowerSDR_empty;
		dsp_SH = PowerSDR_empty;
		//bw = 7;
		break;
	case AM: case SAM: case DSB: default:
		bandwidths_ = PowerSDR_AMwidths;
		dsp_SL = PowerSDR_empty;
		dsp_SH = PowerSDR_empty;
		//bw = 12;
		break;
	}
	return bw;
}

const char **RIG_PowerSDR::bwtable(int val)
{
	if (val == LSB || val == USB)
		return PowerSDR_USBwidths;
	else if (val == FM || val == DRM || val == SPEC)
		return PowerSDR_WIDEwidths;
	else if (val == DIGU || val == DIGL)
		return PowerSDR_DIGwidths;
	else if (val == CWU || val == CWL)
		return PowerSDR_CWwidths;
	else if (val == AM || val == SAM || val == DSB)
		return PowerSDR_AMwidths;
	return NULL;
}

#if 0
const char **RIG_PowerSDR::lotable(int val)
{
	if (val == LSB || val == USB || val == FM || val == DRM || val == SPEC)
		return PowerSDR_USBwidths;
	else if (val == DIGU || val == DIGL)
		return PowerSDR_DIGwidths;
	else if (val == AM || val == SAM || val == DSB)
		return PowerSDR_AMwidths;
	return NULL;
}

const char **RIG_PowerSDR::hitable(int val)
{
	if (val == LSB || val == USB || val == FM || val == DRM || val == SPEC)
		return PowerSDR_USBwidths;
	else if (val == DIGU || val == DIGL)
		return PowerSDR_DIGwidths;
	else if (val == CWU || val == CWL)
		return PowerSDR_CWwidths;
	else if (val == AM || val == SAM || val == DSB)
		return PowerSDR_AMwidths;
	return NULL;
}
#endif

void RIG_PowerSDR::set_modeA(int val)
{
	if (val >= (int)(sizeof(PowerSDR_mode_chr)/sizeof(*PowerSDR_mode_chr))) return;
	_currmode = A.imode = val;
	cmd = "ZZMD";
	cmd += PowerSDR_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "set mode", cmd, "");
	sett("modeA");
	A.iBW = set_widths(val);
	vfoA.iBW = A.iBW;
}

int RIG_PowerSDR::get_modeA()
{
	if (tuning()) return A.imode;
	cmd = "ZZMD;";
	int ww = wait_char(';', 7, 100, "get mode A", 7);
	if (ww == 7) {
		size_t p = replystr.rfind("MD");
		if (p != string::npos) {
			int md;
			sscanf(&replystr[p+2],"%d",&md);

			A.imode = md;
			A.iBW = set_widths(A.imode);
		}
	}
	_currmode = A.imode;
	gett("modeA");
	return A.imode;
}


void RIG_PowerSDR::set_modeB(int val)
{
	if (val >= (int)(sizeof(PowerSDR_mode_chr)/sizeof(*PowerSDR_mode_chr))) return;
	_currmode = B.imode = val;
	cmd = "MD";
	cmd += PowerSDR_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "set mode B", cmd, "");
	sett("modeB");
	MilliSleep(100); // give rig a chance to change width
	B.iBW = set_widths(val);
	vfoB.iBW = B.iBW;
}

int RIG_PowerSDR::get_modeB()
{
	if (tuning()) return B.imode;
	cmd = "ZZMD;";
	if (wait_char(';', 4, 100, "get mode B", ASC) == 7) {
		size_t p = replystr.rfind("MD");
		if (p != string::npos) {
			int md=0;
			sscanf(&replystr[p+2],"%d",&md);
			B.imode = md;
			B.iBW = set_widths(B.imode);
		}
	}
	_currmode = B.imode;
	gett("modeB");
	return B.imode;
}

int RIG_PowerSDR::adjust_bandwidth(int val)
{
	int bw = 0;
	bw = 0;
	if (val == CWU || val == CWL)
		bw = 7;
	return bw;
}

int RIG_PowerSDR::def_bandwidth(int val)
{
	return adjust_bandwidth(val);
}

void RIG_PowerSDR::set_bwA(int val)
{
	stringstream str;
	if (A.imode == FM || A.imode == DRM || A.imode == SPEC) return; // mode is fixed
	else if (A.imode == LSB || A.imode == USB) {
		A.iBW = val;
		if (val >= (int)(sizeof(PowerSDR_CAT_USB)/sizeof(*PowerSDR_CAT_USB))) return;
		cmd = PowerSDR_CAT_USB[val];
		sendCommand(cmd);
		showresp(WARN, ASC, "set_bwA USB", cmd, "");
		sett("bwA USB");
	}
	else if (A.imode == DIGU || A.imode == DIGL) {
		A.iBW = val;
	stringstream str;
	str << "val =" << val ;
	trace(2, __func__, str.str().c_str());
		if (val >= (int)(sizeof(PowerSDR_CAT_DIG)/sizeof(*PowerSDR_CAT_DIG))) return;
		cmd = PowerSDR_CAT_DIG[val];
		sendCommand(cmd);
		showresp(WARN, ASC, "set_bwA DIG", cmd, "");
		sett("bwA DIG");
	}
	else if (A.imode == CWU || A.imode == CWL) {
		A.iBW = val;
		if (val >= (int)(sizeof(PowerSDR_CAT_CW)/sizeof(*PowerSDR_CAT_CW))) return;
		cmd = PowerSDR_CAT_CW[val];
		sendCommand(cmd);
		showresp(WARN, ASC, "set_bwA CW", cmd, "");
		sett("bwA CW");
	}
	else if (A.imode == AM || A.imode == SAM || A.imode == DSB) {
		A.iBW = val;
		if (val >= (int)(sizeof(PowerSDR_CAT_AM)/sizeof(*PowerSDR_CAT_AM))) return;
		cmd = PowerSDR_CAT_AM[val];
		sendCommand(cmd);
		showresp(WARN, ASC, "set_bwA AM", cmd, "");
		sett("bwA AM");
	}
}

int RIG_PowerSDR::get_bwA()
{
	size_t i = 0;
	size_t p;
	stringstream str;
	str << "get_bwA" ;
	A.iBW = 0;
	trace(2, __func__, str.str().c_str());
	if (A.imode == FM || A.imode == DRM || A.imode == SPEC) {
		A.iBW = 0;
		gett("get_bwA Wideband");
	}
	else if (A.imode == LSB || A.imode == USB) {  
		cmd = "ZZFI;";
		if (wait_char(';', 7, 100, "get ZZFI", ASC) == 7) {
			p = replystr.rfind("ZZFI");
			if (p != string::npos) {
				for (i = 0; PowerSDR_CAT_USB[i] != NULL; i++)
				{
					if (replystr.find(PowerSDR_CAT_USB[i]) == p)
						break;
				}
				A.iBW = i;
			}
		}
		gett("get_bwA USB");
	} 
	else if (A.imode == CWL || A.imode == CWU) {
		cmd = "ZZFI;";
		if (wait_char(';', 7, 100, "get ZZFI", ASC) == 7) {
			p = replystr.rfind("ZZFI");
			if (p != string::npos) {
				for (i = 0; PowerSDR_CAT_CW[i] != NULL; i++)
					if (replystr.find(PowerSDR_CAT_CW[i]) == p)
						break;
				A.iBW = i;
			}
		}
		gett("get_bwA CW");
	} 
	else if (A.imode == DIGU || A.imode == DIGL) {
		cmd = "ZZFI;";
		if (wait_char(';', 7, 100, "get ZZFI", ASC) == 7) {
				for (i = 0; PowerSDR_CAT_DIG[i] != NULL; i++) {
					if (replystr.compare(PowerSDR_CAT_DIG[i]) == 0) {
						break;
					}
				}
				A.iBW = i;
		}
		gett("get_bwA DIG");
	}
	else if (A.imode == AM || A.imode == SAM || A.imode == DSB) {
		cmd = "ZZFI;";
		if (wait_char(';', 7, 100, "get ZZFI", ASC) == 7) {
			p = replystr.rfind("ZZFI");
			if (p != string::npos) {
				for (i = 0; PowerSDR_CAT_AM[i] != NULL; i++)
					if (replystr.find(PowerSDR_CAT_AM[i]) == p)
						break;
				A.iBW = i;
			}
		}
		gett("get_bwA AM");
	}
	vfoA.iBW = A.iBW;
	progStatus.iBW_A = A.iBW;
	return A.iBW;
}

void RIG_PowerSDR::set_bwB(int val)
{
	set_bwA(val);
	B.iBW = val;
	stringstream str;
	str << "B.iBW = " << B.iBW;
	trace(2, __func__, str.str().c_str());
	return;
}

int RIG_PowerSDR::get_bwB() // same as A
{
	B.iBW =  get_bwA();
	stringstream str;
	str << "B.iBW = " << B.iBW;
	trace(2, __func__, str.str().c_str());
	progStatus.iBW_B = B.iBW;
	return B.iBW;
}

int RIG_PowerSDR::get_modetype(int n)
{
	if (n >= (int)(sizeof(PowerSDR_mode_type)/sizeof(*PowerSDR_mode_type))) return 0;
	return PowerSDR_mode_type[n];
}

void RIG_PowerSDR::get_if_min_max_step(int &min, int &max, int &step)
{
	if_shift_min = min = 400;
	if_shift_max = max = 1000;
	if_shift_step = step = 50;
	if_shift_mid = 700;
}

void RIG_PowerSDR::set_notch(bool on, int val)
{
	if (on) {
		cmd = "BC2;"; // set manual notch
		sendCommand(cmd);
		showresp(WARN, ASC, "set notch on", cmd, "");
		sett("notch ON");
		cmd = "BP";
//		val = round((val - 220) / 50);
		val = round((val - 200) / 50);
		cmd.append(to_decimal(val, 3)).append(";");
		sendCommand(cmd);
		showresp(WARN, ASC, "set notch val", cmd, "");
		sett("notch val");
	} else {
		cmd = "BC0;"; // no notch action
		sendCommand(cmd);
		showresp(WARN, ASC, "set notch off", cmd, "");
		sett("notch OFF");
	}
}

bool  RIG_PowerSDR::get_notch(int &val)
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
					gett("notch val");
					p = replystr.rfind("BP");
					if (p != string::npos)
						val = 200 + 50 * fm_decimal(replystr.substr(p+2),3);
				}
			}
		}
	}
	gett("notch on/off");
	return (ison);
}

void RIG_PowerSDR::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = 200;
	max = 3350;
	step = 50;
}

void RIG_PowerSDR::set_auto_notch(int v)
{
	cmd = v ? "NT1;" : "NT0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set auto notch", cmd, "");
	sett("auto notch");
}

int  RIG_PowerSDR::get_auto_notch()
{
	int anotch = 0;
	cmd = "NT;";
	if (wait_char(';', 4, 100, "get auto notch", ASC) == 4) {
		size_t p = replystr.rfind("NT");
		if (p != string::npos) {
			anotch = (replystr[p+2] == '1');
		}
	}
	gett("auto notch");
	return anotch;
}

void RIG_PowerSDR::set_noise_reduction(int val)
{
	if (val == -1) {
		return;
	}
	_noise_reduction_level = val;
	if (_noise_reduction_level == 0) {
		nr_label("ZZNR0", false);
	} else if (_noise_reduction_level == 1) {
		nr_label("ZZNR1", true);
	} else {
		nr_label("???", true);
		return;
	}
	cmd.assign("ZZNR");
	cmd += '0' + _noise_reduction_level;
	cmd += ';';
	sendCommand (cmd);
	showresp(WARN, ASC, "SET noise reduction", cmd, "");
	sett("noise reduction");
}

int  RIG_PowerSDR::get_noise_reduction()
{
	cmd = rsp = "ZZNR";
	cmd.append(";");
	if (wait_char(';', 4, 100, "GET noise reduction", ASC) == 6) {
		size_t p = replystr.rfind(rsp);
		if (p == string::npos) return _noise_reduction_level;
		_noise_reduction_level = replystr[p+4] - '0';
	}

	if (_noise_reduction_level == 1) {
		nr_label("NR", true);
	} else if (_noise_reduction_level == 2) {
		nr_label("NR2", true);
	} else {
		nr_label("NR", false);
	}
	gett("nr level");
	return _noise_reduction_level;
}

void RIG_PowerSDR::set_noise_reduction_val(int val)
{
	if (_noise_reduction_level == 0) return;
	if (_noise_reduction_level == 1) _nrval1 = val;
	else _nrval2 = val;

	cmd.assign("NR").append(to_decimal(val, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET_noise_reduction_val", cmd, "");
	sett("noise reduction val");
}

int  RIG_PowerSDR::get_noise_reduction_val()
{
	int nrval = 0;
	if (_noise_reduction_level == 0) return 0;
	int val = progStatus.noise_reduction_val;
	cmd = rsp = "ZZNR";
	cmd.append(";");
	if (wait_char(';', 5, 100, "GET noise reduction val", ASC) == 5) {
		size_t p = replystr.rfind(rsp);
		if (p == string::npos) {
			nrval = (_noise_reduction_level == 1 ? _nrval1 : _nrval2);
			return nrval;
		}
		val = atoi(&replystr[p+2]);
	}
	gett("noise reduction val");

	if (_noise_reduction_level == 1) _nrval1 = val;
	else _nrval2 = val;

	return val;
}

void RIG_PowerSDR::tune_rig(int val)
{

	stringstream str;
	str << "val=" << val;
	trace(2, __func__, str.str().c_str());
	switch (val) {
		case 0:
			cmd = "ZZTU0;";
			break;
		case 1:
		case 2:
			cmd = "ZZTU1;";
			break;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "tune rig", cmd, replystr);
	sett("tune_rig");
}

int RIG_PowerSDR::get_tune()
{
	cmd = rsp = "ZZTU";
	cmd += ';';
	waitN(5, 100, "get tune", ASC);

	rig_trace(2, "get_tuner status()", replystr.c_str());

	size_t p = replystr.rfind(rsp);
	if (p == string::npos) return 0;
	int val = replystr[p+4] - '0';
	return val;
}

