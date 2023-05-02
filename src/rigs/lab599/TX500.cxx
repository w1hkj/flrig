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

static std::vector<std::string>TX500modes_;
static const char *vTX500modes_[] = {
	"LSB", "USB", "CW", "FM", "AM", "DIG", "CW-R"};

static const char TX500_mode_chr[] =  { '1', '2', '3', '4', '5', '6', '7' };
static const char TX500_mode_type[] = { 'L', 'U', 'U', 'U', 'U', 'U', 'L' };

static std::vector<std::string>TX500_empty;
static const char *vTX500_empty[] = { "N/A" };
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

	serial_baudrate = BR9600;
	stopbits = 1;
	serial_retries = 2;

//	serial_write_delay = 0;
//	serial_post_write_delay = 0;

	serial_timeout = 50;
	serial_rtscts =  false;
	serial_rtsplus = false;
	serial_dtrplus = true;
	serial_catptt =  true;
	serial_rtsptt =  false;
	serial_dtrptt =  false;
	B.imode = A.imode = 1;
	B.iBW = A.iBW = 0x8803;
	B.freq = A.freq = 14070000ULL;
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

void RIG_TX500::initialize()
{
	VECTOR (TX500modes_, vTX500modes_);
	VECTOR (TX500_empty, vTX500_empty);

	modes_ = TX500modes_;
	bandwidths_ = TX500_empty;
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

unsigned long long RIG_TX500::get_vfoA ()
{
	cmd = "FA;";
	if (wait_char(';', 14, 100, "get vfo A", ASC) < 14) return A.freq;
	gett("get_vfoA");

	size_t p = replystr.rfind("FA");
	if (p != std::string::npos && (p + 12 < replystr.length())) {
		sscanf(replystr.c_str(), "FA%lld", &A.freq);
	}
	return A.freq;
}

void RIG_TX500::set_vfoA (unsigned long long freq)
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

unsigned long long RIG_TX500::get_vfoB ()
{
	cmd = "FB;";
	if (wait_char(';', 14, 100, "get vfo B", ASC) < 14) return B.freq;
	gett("get_vfoB");

	size_t p = replystr.rfind("FB");
	if (p != std::string::npos && (p + 12 < replystr.length())) {
		sscanf(replystr.c_str(), "FB%lld", &B.freq);
	}
	return B.freq;
}

void RIG_TX500::set_vfoB (unsigned long long freq)
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
