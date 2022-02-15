// ---------------------------------------------------------------------
// Copyright (C) 2021
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
// ---------------------------------------------------------------------

/* =====================================================================
TX500 CAT controls reverse engineering:

1. frequency
2. mode; set: FSK==>DIG, FSK-R==>DIG; get DIG always returns USB as mode
3. PTT
4. attenuator
5. preamp
6. identification; returns ID019;
7. power on/off status; PS; returns PS0; but transceiver is ON
8. SM0; always returns SM00002;

Factory response to request for list of supported CAT commands:

ID;		identifier, response 019;
AI;		auto information; default OFF
RX;		receiver function status; NO RESPONSE
IF;		information std::string; type response:
		IF0000703437000000+000000000020000000;
FA;		get/set frequency vfo A; FA00007034370;
FB;		get/set frequency vfo B; FB00014070000;
FR;		RX vfo: FR0; vfoA, FR1; vfoB
FT;		TX vfo: FT0; vfoA, FT1; vfoB
FN;		UNKNOWN COMMAND
MD;		mode: MD6 sets DIG, but xcvr responds with USB
PA;		PA01; preamp OFF, PA11; preamp ON
RA;		RA0000; attenuator OFF, RA0100; attenuator ON

IF; response 
0.........1.........2.........3.......
01234567890123456789012345678901234567
|         |         |         |
IF0000703501000000+000000000030000000;
  |_________||___||_____|||||||||||||______ active VFO frequency 11 digits
            ||___||_____|||||||||||||______ always zero
                 ||_____|||||||||||||______ +/- RIT/XIT frequency
                         ||||||||||||______ RIT on/off
                          |||||||||||______ XIT on/off
                           ||||||||||______ memory channel
                             ||||||||______ 0 RX, 1 TX
                              |||||||______ mode, related to MD command
                               ||||||______ related to FR/FT commands
                                |||||______ scan status
                                 ||||______ simplex/split
                                  |||______ CTSS on/off, always zero
                                  |||______ CTSS frequency, always zero
                                    |______ always zero

IF0000703437000000+000000000010000000;   LSB
IF0000703437000000+000000000020000000;   USB
IF0000703501000000+000000000030000000;   CW
IF0000703437000000+000000000040000000;   FM
IF0000703437000000+000000000050000000;   AM
IF0000703437000000+000000000020000000;   FSK (DIG)
IF0000703373000000+000000000070000000;   CW-R
IF0000703437000000+000000000020000000;   FSK-R (DIG)

======================================================================*/

#include "lab599/TX500.h"
#include "support.h"

static const char TX500name_[] = "TX500";

static const char *TX500modes_[] = {
	"LSB", "USB", "CW", "FM", "AM", "DIG", "CW-R", NULL};

static const char TX500_mode_chr[] =  { '1', '2', '3', '4', '5', '6', '7' };
static const char TX500_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'U', 'L' };

static const char *TX500_empty[] = { "N/A", NULL };
//----------------------------------------------------------------------

static GUI rig_widgets[]= {
//	{ (Fl_Widget *)btnVol,        2, 125,  50 }, // 0
//	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 }, // 1
//	{ (Fl_Widget *)sldrRFGAIN,   54, 145, 156 }, // 2
//	{ (Fl_Widget *)btnIFsh,     214, 105,  50 }, // 3
//	{ (Fl_Widget *)sldrIFSHIFT, 266, 105, 156 }, // 4
//	{ (Fl_Widget *)btnNotch,    214, 125,  50 }, // 5
//	{ (Fl_Widget *)sldrNOTCH,   266, 125, 156 }, // 6
//	{ (Fl_Widget *)sldrSQUELCH, 266, 145, 156 }, // 7
//	{ (Fl_Widget *)sldrMICGAIN, 266, 165, 156 }, // 8
//	{ (Fl_Widget *)sldrPOWER,    54, 165, 368 }, // 9
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

RIG_TX500::RIG_TX500() {
// base class values
	name_ = TX500name_;
	modes_ = TX500modes_;
	bandwidths_ = TX500_empty;

	widgets = rig_widgets;

	comm_baudrate = BR9600;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 5;
	comm_timeout = 50;
	comm_rtscts =  false;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt =  true;
	comm_rtsptt =  false;
	comm_dtrptt =  false;
	B.imode = A.imode = 1;
	B.iBW = A.iBW = 0x8803;
	B.freq = A.freq = 14070000;
	can_change_alt_vfo = true;

	has_power_out =
//	has_swr_control =
//	has_alc_control =
//	has_split =
//	has_split_AB =
//	has_dsp_controls =
//	has_rf_control =
//	has_notch_control =
//	has_auto_notch =
//	has_ifshift_control =
	has_smeter =
//	has_noise_reduction =
//	has_noise_reduction_control =
//	has_noise_control =
//	has_micgain_control =
//	has_volume_control =
//	has_power_control =
//	has_tune_control =
	has_attenuator_control =
	has_preamp_control =
	has_mode_control =
//	has_bandwidth_control =
//	has_sql_control =
	has_ptt_control = 
//	has_extras = 
		true;

	precision = 1;
	ndigits = 9;

	atten_level = 0;
	preamp_level = 0;
//	_noise_reduction_level = 0;
//	_nrval1 = 2;
//	_nrval2 = 4;

}

static int ret = 0;

void RIG_TX500::set_attenuator(int val)
{
	atten_level = val;
	if (val) cmd = "RA01;";
	else     cmd = "RA00;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set ATT", cmd, "");
	sett("attenuator");
}

int RIG_TX500::get_attenuator()
{
	cmd = "RA;";
	ret = wait_char(';', 7, 100, "get ATT", ASC);
	gett("get_attenuator");
	if (ret >= 7) {
		size_t p = replystr.rfind("RA");
		if (p != std::string::npos && (p+3 < replystr.length())) {
			if (replystr[p+2] == '0' && replystr[p+3] == '0')
				atten_level = 0;
			else
				atten_level = 1;
		}
	}
	return atten_level;
}

void RIG_TX500::set_preamp(int val)
{
	preamp_level = val;
	if (val) cmd = "PA1;";
	else     cmd = "PA0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set PRE", cmd, "");
	sett("preamp");
}

int RIG_TX500::get_preamp()
{
	cmd = "PA;";
	ret = wait_char(';', 5, 100, "get PRE", ASC);
	gett("get_preamp");
	if (ret >= 5) {
		size_t p = replystr.rfind("PA");
		if (p != std::string::npos && (p+2 < replystr.length())) {
			if (replystr[p+2] == '1')
				preamp_level = 1;
			else
				preamp_level = 0;
		}
	}
	return preamp_level;
}

void RIG_TX500::set_modeA(int val)
{
	if (val >= (int)(sizeof(TX500_mode_chr)/sizeof(*TX500_mode_chr))) return;
	_currmode = A.imode = val;
	cmd = "MD";
	cmd += TX500_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "set mode", cmd, "");
	sett("modeA");
}

int RIG_TX500::get_modeA()
{
	cmd = "MD;";
	ret = wait_char(';', 4, 100, "get mode A", ASC);
	gett("get_modeA");
	if (ret == 4) {
		size_t p = replystr.rfind("MD");
		if (p != std::string::npos) {
			int md = replystr[p+2];
			md = md - '1';
			if (md == 8) md = 7;
			A.imode = md;
		}
	}
	_currmode = A.imode;
	return A.imode;
}

void RIG_TX500::set_modeB(int val)
{
	if (val >= (int)(sizeof(TX500_mode_chr)/sizeof(*TX500_mode_chr))) return;
	_currmode = B.imode = val;
	cmd = "MD";
	cmd += TX500_mode_chr[val];
	cmd += ';';
	sendCommand(cmd);
	showresp(WARN, ASC, "set mode B", cmd, "");
	sett("modeB");
}

int RIG_TX500::get_modeB()
{
	cmd = "MD;";
	ret = wait_char(';', 4, 100, "get mode B", ASC);
	gett("get_modeB");
	if (ret == 4) {
		size_t p = replystr.rfind("MD");
		if (p != std::string::npos) {
			int md = replystr[p+2];
			md = md - '1';
			if (md == 8) md = 7;
			B.imode = md;
		}
	}
	_currmode = B.imode;
	return B.imode;
}

//====================== vfo controls

bool RIG_TX500::check()
{
	MilliSleep(200);  // needed for restart.  TX500  has mechanical relays
	cmd = "ID;";
	if (wait_char(';', 6, 500, "get ID", ASC) < 6) {
trace(1, "check ID FAILED");
		return false;
	}
	gett("check ID");
	cmd = "FA;";
	if (wait_char(';', 14, 500, "get vfo A", ASC) < 14) {
trace(1, "check FA FAILED");
		return false;
	}
	gett("check vfoA");
	return true;
}

void RIG_TX500::selectA()
{
	cmd = "FR0;FT0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Rx on A, Tx on A", cmd, "");
	inuse = onA;
}

void RIG_TX500::selectB()
{
	cmd = "FR1;FT1;";
	sendCommand(cmd);
	showresp(WARN, ASC, "Rx on B, Tx on B", cmd, "");
	inuse = onB;
}

unsigned long int RIG_TX500::get_vfoA ()
{
	cmd = "FA;";
	if (wait_char(';', 14, 100, "get vfo A", ASC) < 14) return A.freq;
	gett("get_vfoA");

	size_t p = replystr.rfind("FA");
	if (p != std::string::npos && (p + 12 < replystr.length())) {
		A.freq = atol(&replystr[ p + 2]);
	}
	return A.freq;
}

void RIG_TX500::set_vfoA (unsigned long int freq)
{
	A.freq = freq;
	cmd = "FA00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sett("set_vfoA");
	sendCommand(cmd);
	sett("");
	showresp(WARN, ASC, "set vfo A", cmd, "");
}

unsigned long int RIG_TX500::get_vfoB ()
{
	cmd = "FB;";
	if (wait_char(';', 14, 100, "get vfo B", ASC) < 14) return B.freq;
	gett("get_vfoB");

	size_t p = replystr.rfind("FB");
	if (p != std::string::npos && (p + 12 < replystr.length())) {
		B.freq = atol(&replystr[ p + 2 ]);
	}
	return B.freq;
}

void RIG_TX500::set_vfoB (unsigned long int freq)
{
	B.freq = freq;
	cmd = "FB00000000000;";
	for (int i = 12; i > 1; i--) {
		cmd[i] += freq % 10;
		freq /= 10;
	}
	sett("set_vfoB");
	sendCommand(cmd);
	sett("");
	showresp(WARN, ASC, "set vfo B", cmd, "");
}

void RIG_TX500::set_split(bool val) 
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

int RIG_TX500::get_split()
{
		cmd = "IF;";
		get_trace(1, "get split INFO");
		ret = wait_char(';', 38, 100, "get split INFO", ASC);
		gett("");
		if (ret >= 38) {
			return (split = (replystr[32] == '1'));
		}
		return 0;
}

int RIG_TX500::get_modetype(int n)
{
	if (n >= (int)(sizeof(TX500_mode_type)/sizeof(*TX500_mode_type))) return 0;
	return TX500_mode_type[n];
}

int RIG_TX500::get_smeter()
{
	int smtr = 0;
	cmd = "SM0;";

	ret = wait_char(';', 8, 100, "get smeter", ASC);
	gett("get_smeter");
	if (ret == 8) {
		size_t p = replystr.rfind("SM");
		if (p != std::string::npos) {
			smtr = atol(&replystr[p + 4]);
			smtr = (smtr * 100) / 30;
		}
	}
	return smtr;
}

void RIG_TX500::set_PTT_control(int val)
{
	ptt_ = val;
	if (val) cmd = "TX;";
	else     cmd = "RX;";
	sendCommand(cmd);
	showresp(WARN, ASC, "PTT", cmd, "");
}

//IF0000702800000000+000000000130000000;
//IF0000702800000000+000000000030000000;
//01234567890123456789012345678
int  RIG_TX500::get_PTT()
{
	cmd = "IF;";
	ret = wait_char(';', 38, 100, "get PTT", ASC);
	gett("get PTT");
	if (ret >= 38) {
		ptt_ = replystr[28] == '1';
	}
	return ptt_;
}


/*

Kenwood CAT commands NOT IMPLEMENTED IN TX500
---------------------------------------------
static const char *TX500_SL[] = {
 "0",   "50", "100", "200", "300",
"400",  "500", "600", "700", "800",
"900", "1000", NULL };
static const char *TX500_CAT_SL[] = {
"SL00;", "SL01;", "SL02;", "SL03;", "SL04;",
"SL05;", "SL06;", "SL07;", "SL08;", "SL09;",
"SL10;", "SL11;" };
static const char *TX500_SL_tooltip = "lo cut";
static const char *TX500_SSB_btn_SL_label = "L";
//----------------------------------------------------------------------
static const char *TX500_SH[] = {
"1400", "1600", "1800", "2000", "2200",
"2400", "2600", "2800", "3000", "3400",
"4000", "5000", NULL };
static const char *TX500_CAT_SH[] = {
"SH00;", "SH01;", "SH02;", "SH03;", "SH04;",
"SH05;", "SH06;", "SH07;", "SH08;", "SH09;",
"SH10;", "SH11;" };
static const char *TX500_SH_tooltip = "hi cut";
static const char *TX500_SSB_btn_SH_label = "H";
//----------------------------------------------------------------------
static const char *TX500_AM_SL[] = {
"0", "100", "200", "500", NULL };
static const char *TX500_AM_SH[] = {
"2500", "3000", "4000", "5000" };
//----------------------------------------------------------------------
static const char *TX500_CWwidths[] = {
"50", "80", "100", "150", "200",
"300", "400", "500", "600", "1000",
"2000", NULL};
static const char *TX500_CWbw[] = {
"FW0050;", "FW0080;", "FW0100;", "FW0150;", "FW0200;",
"FW0300;", "FW0400;", "FW0500;", "FW0600;", "FW1000;",
"FW2000;" };
//----------------------------------------------------------------------
static const char *TX500_FSKwidths[] = {
"250", "500", "1000", "1500", NULL};
static const char *TX500_FSKbw[] = {
"FW0250;", "FW0500;", "FW1000;", "FW1500;" };
//----------------------------------------------------------------------
*/

/*

void RIG_TX500::initialize()
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
}

void RIG_TX500::shutdown()
{
}

int RIG_TX500::set_widths(int val)
{
	int bw;
	switch (val) {
	case LSB: case USB: case FM:
		bandwidths_ = TX500_SH;
		dsp_SL = TX500_SL;
		SL_tooltip = TX500_SL_tooltip;
		SL_label   = TX500_SSB_btn_SL_label;
		dsp_SH = TX500_SH;
		SH_tooltip = TX500_SH_tooltip;
		SH_label   = TX500_SSB_btn_SH_label;
		if (val == FM) bw = 0x8A03; // 200 ... 4000 Hz
		else bw = 0x8803; // 200 ... 3000 Hz
		break;
	case CW: case CWR:
		bandwidths_ = TX500_CWwidths;
		dsp_SL = TX500_empty;
		dsp_SH = TX500_empty;
		bw = 7;
		break;
	case FSK: case FSKR:
		bandwidths_ = TX500_FSKwidths;
		dsp_SL = TX500_empty;
		dsp_SH = TX500_empty;
		bw = 1;
		break;
	case AM: default:
		bandwidths_ = TX500_AM_SH;
		dsp_SL = TX500_AM_SL;
		dsp_SH = TX500_AM_SH;
		bw = 0x8201;
		break;
	}
	return bw;
}

const char **RIG_TX500::bwtable(int val)
{
	if (val == LSB || val == USB || val == FM)
		return TX500_SH;
	else if (val == CW || val == CWR)
		return TX500_CWwidths;
	else if (val == FSK || val == FSKR)
		return TX500_FSKwidths;
//else AM m == 4
	return TX500_AM_SH;
}

const char **RIG_TX500::lotable(int val)
{
	if (val == LSB || val == USB || val == FM)
		return TX500_SL;
	if (val == AM)
		return TX500_AM_SL;
	return NULL;
}

const char **RIG_TX500::hitable(int val)
{
	if (val == LSB || val == USB || val == FM)
		return TX500_SH;
	if (val == AM)
		return TX500_AM_SH;
	return NULL;
}

const char * RIG_TX500::get_bwname_(int n, int md)
{
	static char bwname[20];
	if (n > 256) {
		int hi = (n >> 8) & 0x7F;
		int lo = n & 0xFF;
		snprintf(bwname, sizeof(bwname), "%s/%s",
			(md == LSB || md == USB || md == FM) ? TX500_SL[lo] : TX500_AM_SL[lo],
			(md == LSB || md == USB || md == FM) ? TX500_SH[hi] : TX500_AM_SH[hi] );
	} else {
		snprintf(bwname, sizeof(bwname), "%s",
			(md == CW || md == CWR) ? TX500_CWwidths[n] : TX500_FSKwidths[n]);
	}
	return bwname;
}

int RIG_TX500::adjust_bandwidth(int val)
{
	int bw = 0;
	if (val == LSB || val == USB)
		bw = 0x8803;
	else if (val == FM)
		bw = 0x8A03;
	else if (val == AM)
		bw = 0x8301;
	else if (val == CW || val == CWR)
		bw = 7;
	else if (val == FSK || val == FSKR)
		bw = 1;
	return bw;
}

int RIG_TX500::def_bandwidth(int val)
{
	return adjust_bandwidth(val);
}

void RIG_TX500::set_bwA(int val)
{
	if (A.imode == LSB || A.imode == USB || A.imode == FM || A.imode == AM) {
		if (val < 256) return;
		A.iBW = val;
		cmd = "SL";
		int index = A.iBW & 0x7F;
		if (index >= (int)(sizeof(TX500_CAT_SL)/sizeof(*TX500_CAT_SL))) return;
		cmd = TX500_CAT_SL[index];
		sendCommand(cmd);
		showresp(WARN, ASC, "set lower", cmd, "");
		sett("bwA lower");
		cmd = "SH";
		index = (A.iBW >> 8) & 0x7F;
		if (index >= (int)(sizeof(TX500_CAT_SH)/sizeof(*TX500_CAT_SH))) return;
		cmd = TX500_CAT_SH[index];
		sendCommand(cmd);
		showresp(WARN, ASC, "set upper", cmd, "");
		sett("bwA upper");
	}
	if (val > 256) return;
	else if (A.imode == CW || A.imode == CWR) {
		A.iBW = val;
		int index = A.iBW & 0x7F;
		if (index >= (int)(sizeof(TX500_CWbw)/sizeof(*TX500_CWbw))) return;
		cmd = TX500_CWbw[index];
		sendCommand(cmd);
		showresp(WARN, ASC, "set CW bw", cmd, "");
		sett("CW bw");
	}else if (A.imode == FSK || A.imode == FSKR) {
		A.iBW = val;
		int index = A.iBW & 0x7F;
		if (index >= (int)(sizeof(TX500_FSKbw)/sizeof(*TX500_FSKbw))) return;
		cmd = TX500_FSKbw[index];
		sendCommand(cmd);
		showresp(WARN, ASC, "set FSK bw", cmd, "");
		sett("FSK bw");
	}
}

int RIG_TX500::get_bwA()
{
	if (tuning()) return A.iBW;
	if (skip_get) return A.iBW;
	size_t i = 0;
	size_t p;
	if (A.imode == LSB || A.imode == USB || A.imode == FM || A.imode == AM) {
		int lo = A.iBW & 0xFF, hi = (A.iBW >> 8) & 0x7F;
		cmd = "SL;";
		get_trace(1, "get SL");
		ret = wait_char(';', 5, 100, "get SL", ASC);
		gett("");
		if (ret == 5) {
			p = replystr.rfind("SL");
			if (p != std::string::npos)
				lo = fm_decimal(replystr.substr(2), 2);
		}
		cmd = "SH;";
		get_trace(1, "get SH");
		ret = wait_char(';', 5, 100, "get SH", ASC);
		gett("");
		if (ret == 5) {
			p = replystr.rfind("SH");
			if (p != std::string::npos)
				hi = fm_decimal(replystr.substr(2), 2);
			A.iBW = ((hi << 8) | (lo & 0xFF)) | 0x8000;
		}
	} else if (A.imode == CW || A.imode == CWR) { // CW
		cmd = "FW;";
		get_trace(1, "get FW");
		ret = wait_char(';', 7, 100, "get FW", ASC);
		gett("");
		if (ret == 7) {
			p = replystr.rfind("FW");
			if (p != std::string::npos) {
				for (i = 0; i < sizeof(TX500_CWbw)/sizeof(*TX500_CWbw); i++)
					if (replystr.find(TX500_CWbw[i]) == p)
						break;
				A.iBW = i;
			}
		}
	} else if (A.imode == FSK || A.imode == FSKR) {
		cmd = "FW;";
		get_trace(1, "get FW");
		ret = wait_char(';', 7, 100, "get FW", ASC);
		gett("");
		if (ret == 7) {
			p = replystr.rfind("FW");
			if (p != std::string::npos) {
				for (i = 0; i < sizeof(TX500_FSKbw)/sizeof(*TX500_FSKbw); i++)
					if (replystr.find(TX500_FSKbw[i]) == p)
						break;
				A.iBW = i;
			}
		}
	}
	return A.iBW;
}

void RIG_TX500::set_bwB(int val)
{
	if (B.imode == LSB || B.imode == USB || B.imode == FM || B.imode == AM) {
		if (val < 256) return;
		B.iBW = val;
		cmd = "SL";
		int index = B.iBW & 0x7F;
		if (index >= (int)(sizeof(TX500_CAT_SL)/sizeof(*TX500_CAT_SL))) return;
		cmd = TX500_CAT_SL[index];
		sendCommand(cmd);
		showresp(WARN, ASC, "set lower", cmd, "");
		sett("bwB lower");
		cmd = "SH";
		index = (B.iBW >> 8) & 0x7F;
		if (index >= (int)(sizeof(TX500_CAT_SH)/sizeof(*TX500_CAT_SH))) return;
		cmd = TX500_CAT_SH[index];
		sendCommand(cmd);
		showresp(WARN, ASC, "set upper", cmd, "");
		sett("bwB upper");
	}
	if (val > 256) return;
	else if (B.imode == CW || B.imode == CWR) {
		B.iBW = val;
		int index = B.iBW & 0x7F;
		if (index >= (int)(sizeof(TX500_CWbw)/sizeof(*TX500_CWbw))) return;
		cmd = TX500_CWbw[index];
		sendCommand(cmd);
		showresp(WARN, ASC, "set CW bw", cmd, "");
		sett("bwB CW");
	}else if (B.imode == FSK || B.imode == FSKR) {
		B.iBW = val;
		int index = B.iBW & 0x7F;
		if (index >= (int)(sizeof(TX500_FSKbw)/sizeof(*TX500_FSKbw))) return;
		cmd = TX500_FSKbw[index];
		sendCommand(cmd);
		showresp(WARN, ASC, "set FSK bw", cmd, "");
		sett("bwB FSK");
	}
}

int RIG_TX500::get_bwB()
{
	if (tuning()) return B.iBW;
	if (skip_get) return B.iBW;
	size_t i = 0;
	size_t p;
	if (B.imode == LSB || B.imode == USB || B.imode == FM || B.imode == AM) {
		int lo = B.iBW & 0xFF, hi = (B.iBW >> 8) & 0x7F;
		cmd = "SL;";
		get_trace(1, "get SL");
		ret = wait_char(';', 5, 100, "get SL", ASC);
		gett("");
		if (ret == 5) {
			p = replystr.rfind("SL");
			if (p != std::string::npos)
				lo = fm_decimal(replystr.substr(2), 2);
		}
		cmd = "SH;";
		get_trace(1, "get SH");
		ret = wait_char(';', 5, 100, "get SH", ASC);
		gett("");
		if (ret == 5) {
			p = replystr.rfind("SH");
			if (p != std::string::npos)
				hi = fm_decimal(replystr.substr(2), 2);
			B.iBW = ((hi << 8) | (lo & 0xFF)) | 0x8000;
		}
	} else if (B.imode == CW || B.imode == CWR) {
		cmd = "FW;";
		get_trace(1, "get FW");
		ret = wait_char(';', 7, 100, "get FW", ASC);
		gett("");
		if (ret == 7) {
			p = replystr.rfind("FW");
			if (p != std::string::npos) {
				for (i = 0; i < sizeof(TX500_CWbw)/sizeof(*TX500_CWbw); i++)
					if (replystr.find(TX500_CWbw[i]) == p)
						break;
				B.iBW = i;
			}
		}
	} else if (B.imode == FSK || B.imode == FSKR) {
		cmd = "FW;";
		get_trace(1, "get FW");
		ret = wait_char(';', 7, 100, "get FW", ASC);
		gett("");
		if (ret == 7) {
			p = replystr.rfind("FW");
			if (p != std::string::npos) {
				for (i = 0; i < sizeof(TX500_FSKbw)/sizeof(*TX500_FSKbw); i++)
					if (replystr.find(TX500_FSKbw[i]) == p)
						break;
				B.iBW = i;
			}
		}
	}
	return B.iBW;
}

void RIG_TX500::get_if_min_max_step(int &min, int &max, int &step)
{
	if_shift_min = min = 400;
	if_shift_max = max = 1000;
	if_shift_step = step = 50;
	if_shift_mid = 700;
}

void RIG_TX500::set_notch(bool on, int val)
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

bool  RIG_TX500::get_notch(int &val)
{
	bool ison = false;
	cmd = "BC;";

	get_trace(1, "get_notch_on_off");
	ret = wait_char(';', 4, 100, "get notch on/off", ASC);
	gett("");

	if (ret == 4) {
		size_t p = replystr.rfind("BC");
		if (p != std::string::npos) {
			if (replystr[p+2] == '2') {
				ison = true;
				cmd = "BP;";

				get_trace(1, "get_notch_val");
				ret = wait_char(';', 6, 100, "get notch val", ASC);
				gett("");

				if (ret == 6) {
					gett("notch val");
					p = replystr.rfind("BP");
					if (p != std::string::npos)
						val = 200 + 50 * fm_decimal(replystr.substr(p+2),3);
				}
			}
		}
	}
	return (ison);
}

void RIG_TX500::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = 200;
	max = 3350;
	step = 50;
}

void RIG_TX500::set_auto_notch(int v)
{
	cmd = v ? "NT1;" : "NT0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set auto notch", cmd, "");
	sett("auto notch");
}

int  RIG_TX500::get_auto_notch()
{
	int anotch = 0;
	cmd = "NT;";

	get_trace(1, "get_auto_notch");
	ret = wait_char(';', 4, 100, "get auto notch", ASC);
	gett("");

	if (ret == 4) {
		size_t p = replystr.rfind("NT");
		if (p != std::string::npos) {
			anotch = (replystr[p+2] == '1');
		}
	}
	return anotch;
}

void RIG_TX500::set_noise_reduction(int val)
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
	sett("noise reduction");
}

int  RIG_TX500::get_noise_reduction()
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
	if (replystr == "?;") {
		_noise_reduction_level = 0;
		return 0;
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

void RIG_TX500::set_noise_reduction_val(int val)
{
	if (_noise_reduction_level == 0) return;
	if (_noise_reduction_level == 1) _nrval1 = val;
	else _nrval2 = val;

	cmd.assign("RL").append(to_decimal(val, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET_noise_reduction_val", cmd, "");
	sett("noise reduction val");
}

int  RIG_TX500::get_noise_reduction_val()
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

int RIG_TX500::get_power_out()
{
	int poutmtr = 0;
	cmd = "SM0;";
	get_trace(1, "get_power_out");
	ret = wait_char(';', 8, 100, "get power out", ASC);
	gett("");
	if (ret == 8) {
		size_t p = replystr.rfind("SM0");
		if (p != std::string::npos) {
			poutmtr = fm_decimal(replystr.substr(p+3),4);
			if (poutmtr <= 6) poutmtr = poutmtr * 2;
			else if (poutmtr <= 11) poutmtr = 11 + (poutmtr - 6)*(26 - 11)/(11 - 6);
			else if (poutmtr <= 18) poutmtr = 26 + (poutmtr - 11)*(50 - 26)/(18 - 11);
			else poutmtr = 50 + (poutmtr - 18)*(100 - 50)/(27 - 18);
			if (poutmtr > 100) poutmtr = 100;
		}
	}
	return poutmtr;
}

// Transceiver power level
void RIG_TX500::set_power_control(double val)
{
	int ival = (int)val;
	cmd = "PC";
	cmd.append(to_decimal(ival, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set pwr ctrl", cmd, "");
	sett("pwr control");
}

double RIG_TX500::get_power_control()
{
	int pctrl = 0;
	cmd = "PC;";
	get_trace(1, "get_power_contro");
	ret = wait_char(';', 6, 100, "get pout", ASC);
	gett("");
	if (ret >= 6) {
		size_t p = replystr.rfind("PC");
		if (p != std::string::npos) {
			pctrl = fm_decimal(replystr.substr(p+2), 3);
		}
	}
	return pctrl;
}

*/
