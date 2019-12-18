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

#include "TT566.h"
#include "rigbase.h"

//=============================================================================
// TT-566

#define LCW 3
#define UCW 2

extern bool PTT;

static int cur_modeA = 0;
static int cur_modeB = 0;

const char RIG_TT566name_[] = "Orion-II";

const char *RIG_TT566modes_[] = {
		"USB", "LSB", "UCW", "LCW", "AM", "FM", "FSK", NULL};
static const char RIG_TT566_mode_type[] = {'U', 'L', 'U', 'L', 'U', 'U', 'L'};

const char *RIG_TT566widths[] = { 
"100",  "200",  "300",  "400",  "500", "600",  "700",  "800",  "900",  "1000",
"1200", "1400", "1600", "1800", "2000", "2200", "2400", "2600", "2800", "3000",
"3200", "3400", "3600", "3800", "4000", "4500", "5000", "5500", "6000",
NULL};
static int RIG_TT566_bw_vals[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
21,22,23,24,25,26,27,28,29,
WVALS_LIMIT};

static char TT566setFREQa[]		= "*AF"; // binary data
static char TT566getFREQa[]		= "?AF\r";
static char TT566rspFREQa[]		= "@AF";

static char TT566setFREQb[]		= "*BF";
static char TT566getFREQb[]		= "?BF\r";
static char TT566rspFREQb[]		= "@BF";

static char TT566setMODEa[]		= "*RMM";
static char TT566getMODEa[]		= "?RMM\r";
static char TT566rspMODEa[]		= "@RMM";

static char TT566setMODEb[]		= "*RSM";
static char TT566getMODEb[]		= "?RSM\r";
static char TT566rspMODEb[]		= "@RSM";

static char TT566setBWa[]		= "*RMF";
static char TT566getBWa[]		= "?RMF\r";
static char TT566rspBWa[]		= "@RMF";

static char TT566setBWb[]		= "*RSF";
static char TT566getBWb[]		= "?RSF\r";
static char TT566rspBWb[]		= "@RSF";

static char TT566setVOLa[]		= "*UM";
static char TT566getVOLa[]		= "?UM\r";
static char TT566rspVOLa[]		= "@UM";

//static char TT566setVOLb[]		= "*US";
//static char TT566getVOLb[]		= "?US\r";
//static char TT566rspVOLb[]		= "@US";

//static char TT566setPBTa[]		= "*RMP";
//static char TT566getPBTa[]		= "?RMP\r";
//static char TT566rspPBTa[]		= "@RMP";

//static char TT566setPBTb[]		= "*RSP";
//static char TT566getPBTb[]		= "?RSP\r";
//static char TT566rspPBTb[]		= "@RSP";

//static char TT566setAGCa[]		= "*RMA";
//static char TT566getAGCa[]		= "?RMA\r";
//static char TT566rspAGCa[]		= "@RMA";

//static char TT566setAGCb[]		= "*RSA";
//static char TT566getAGCb[]		= "?RSA\r";
//static char TT566rspAGCb[]		= "@RSA";

static char TT566setRFGa[]		= "*RMG";
static char TT566getRFGa[]		= "?RMG\r";
static char TT566rspRFGa[]		= "@RMG";

//static char TT566setRFGb[]		= "*RSG";
//static char TT566getRFGb[]		= "?RSG\r";
//static char TT566rspRFGb[]		= "@RSG";

static char TT566setATTa[]		= "*RMT";
static char TT566getATTa[]		= "?RMT\r";
static char TT566rspATTa[]		= "@RMT";

//static char TT566setATTb[]		= "*RST";
//static char TT566getATTb[]		= "?RST\r";
//static char TT566rspATTb[]		= "@RST";

//static char TT566setSQLa[]		= "*RMS";
//static char TT566getSQLa[]		= "?RMS\r";
//static char TT566rspSQLa[]		= "@RMS";

//static char TT566setSQLb[]		= "*RSS";
//static char TT566getSQLb[]		= "?RSS\r";
//static char TT566rspSQLb[]		= "@RSS";

static char TT566setANa[]		= "*RMNA";
static char TT566getANa[]		= "?RMNA\r";
static char TT566rspANa[]		= "@RMNA";

//static char TT566setANb[]		= "*RSNA";
//static char TT566getANb[]		= "?RSNA\r";
//static char TT566rspANb[]		= "@RSNA";

static char TT566setNBa[]		= "*RMNB";
static char TT566getNBa[]		= "?RMNB\r";
static char TT566rspNBa[]		= "@RMNB";

//static char TT566setNBb[]		= "*RSNB";
//static char TT566getNBb[]		= "?RSNB\r";
//static char TT566rspNBb[]		= "@RSNB";

static char TT566setMIC[]		= "*TM";
static char TT566getMIC[]		= "?TM\r";
static char TT566rspMIC[]		= "@TM";

static char TT566setPWR[]		= "*TP";
static char TT566getPWR[]		= "?TP\r";
static char TT566rspPWR[]		= "@TP";

//static char TT566setTBW[]		= "*TF";
//static char TT566getTBW[]		= "?TF\r";
//static char TT566rspTBW[]		= "@TF";

//static char TT566setTUNE[]		= "*TT";
//static char TT566getTUNE[]		= "?TT\r";
//static char TT566rspTUNE[]		= "@TT";

static char TT566getSMETER[]	= "?S\r";
static char TT566rspSMETER[]	= "@SRM";

static char TT566getPOUT[]		= "?S\r";
static char TT566rspPOUT[]		= "@STF";

static char TT566setPREAMP[]	= "*RME"; // ONLY AVAILABLE ON PRIMARY RX
static char TT566getPREAMP[]	= "?RME\r";
static char TT566rspPREAMP[]	= "@RME";

//static char TT566setRITa[]		= "*RMR";
//static char TT566getRITa[]		= "?RMR\r";
//static char TT566rspRITa[]		= "@RMR";

//static char TT566setRITb[]		= "*RSR";
//static char TT566getRITb[]		= "?RSR\r";
//static char TT566rspRITb[]		= "@RSR";

//static char TT566setXIT[]		= "*RMX"; // ONLY AVAILABLE ON PRIMARY RX
//static char TT566getXIT[]		= "?RMX\r";
//static char TT566rspXIT[]		= "@RMX";

static char TT566setPTT[]		= "*T";

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 },
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 },
	{ (Fl_Widget *)sldrRFGAIN,   54, 145, 156 },
	{ (Fl_Widget *)sldrMICGAIN, 266, 125, 156 },
	{ (Fl_Widget *)sldrPOWER,   266, 145, 156 },
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

RIG_TT566::RIG_TT566() {
	name_ = RIG_TT566name_;
	modes_ = RIG_TT566modes_;
	bandwidths_ = RIG_TT566widths;
	bw_vals_ = RIG_TT566_bw_vals;

	widgets = rig_widgets;

	comm_baudrate = BR57600;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 10;
	comm_timeout = 50;
	comm_echo = true;
	comm_rtscts = true;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = false;
	comm_rtsptt = false;
	comm_dtrptt = false;
	def_mode = modeB = modeA = A.imode = B.imode = 1;
	def_bw = bwB = bwA = A.iBW = B.iBW = 19;
	def_freq = freqB = freqA = A.freq = B.freq = 14070000;

	max_power = 100;
	atten_level = 0;

//	has_ifshift_control =
//	has_agc_level =
//	has_attenuator_control =

	can_change_alt_vfo =
	has_smeter =
	has_power_out =
	has_rf_control =
	has_volume_control =
	has_micgain_control =
	has_swr_control =
	has_mode_control =
	has_bandwidth_control =
	has_ptt_control =
	has_preamp_control =
	has_noise_control =
	has_auto_notch = 
	has_attenuator_control =
	has_power_control = true;

	precision = 1;
	ndigits = 8;

};

void RIG_TT566::initialize()
{
	rig_widgets[0].W = btnVol;
	rig_widgets[1].W = sldrVOLUME;
	rig_widgets[2].W = sldrRFGAIN;
	rig_widgets[3].W = sldrMICGAIN;
	rig_widgets[4].W = sldrPOWER;
}

int  RIG_TT566::adjust_bandwidth(int m)
{
	switch (m) {
		case 0 : case 1 : return 19;
		case 2 : case 3 : return 4;
		case 4 : case 5 : return 24;
		case 6 : default : return 7;
	}
	return 19;
}

bool RIG_TT566::check ()
{
	cmd = TT566getFREQa;
	int ret = waitN(8, 100, "check");
	if (ret < 8) return false;
	return true;
}

long RIG_TT566::get_vfoA ()
{
	cmd = TT566getFREQa; 
	int ret = sendCommand(cmd, strlen(TT566rspFREQa)+9);
	showresp(WARN, ASC, "get vfo A", cmd, respstr);
	if (ret < (int)strlen(TT566rspFREQa)+9) return A.freq;
	size_t p = respstr.rfind(TT566rspFREQa);
	if (p == string::npos) return A.freq;
	int f = 0;
	sscanf(&respstr[p + strlen(TT566rspFREQa)], "%d", &f);
	A.freq = f;
	return A.freq;
}

void RIG_TT566::set_vfoA (long freq)
{
	A.freq = freq;
	cmd = TT566setFREQa;
	cmd.append(to_decimal(freq, 8));
	cmd += '\r';
	sendCommand(cmd);
	showresp(WARN, ASC, "set vfo A", cmd, respstr);
	return;
}

long RIG_TT566::get_vfoB ()
{
	cmd = TT566getFREQb;
	int ret = sendCommand(cmd, strlen(TT566rspFREQb)+9);
	showresp(WARN, ASC, "get vfo A", cmd, respstr);
	if (ret < (int)strlen(TT566rspFREQb)+9) return B.freq;
	size_t p = respstr.rfind(TT566rspFREQb);
	if (p == string::npos) return B.freq;
	int f = 0;
	sscanf(&respstr[p + strlen(TT566rspFREQb)], "%d", &f);
	B.freq = f;
	return B.freq;
}

void RIG_TT566::set_vfoB (long freq)
{
	B.freq = freq;
	cmd = TT566setFREQb;
	cmd.append(to_decimal(freq, 8));
	cmd += '\r';
	sendCommand(cmd);
	showresp(WARN, ASC, "set vfo B", cmd, respstr);
	return;
}

void RIG_TT566::set_PTT_control(int val)
{
        ptt_ = val;
	cmd = TT566setPTT;
	cmd += val ? "K\r" : "U\r";
	sendCommand(cmd);
	showresp(WARN, ASC, "set ptt", cmd, respstr);
}

void RIG_TT566::set_modeA(int md)
{
	A.imode = md;
        cur_modeA = A.imode;
	cmd = TT566setMODEa;
	cmd += '0' + md;
	cmd += '\r';
	sendCommand(cmd);
	showresp(WARN, ASC, "set mode A", cmd, respstr);
}

int RIG_TT566::get_modeA()
{
	cmd = TT566getMODEa;
	int ret = sendCommand (cmd, strlen(TT566rspMODEa)+2);
	if (ret >= 6) {
		A.imode = respstr[ret - 6 + 4] - '0';
	}
        cur_modeA = A.imode;
	return A.imode;
}

void RIG_TT566::set_modeB(int md)
{
	B.imode = md;
        cur_modeB = B.imode;
	cmd = TT566setMODEb;
	cmd += '0' + md;
	cmd += '\r';
	sendCommand(cmd);
	showresp(WARN, ASC, "set mode B", cmd, respstr);
}

int RIG_TT566::get_modeB()
{
	cmd = TT566getMODEb;
	int ret = sendCommand (cmd, strlen(TT566rspMODEb)+2);
	if (ret >= 6) {
		B.imode = respstr[ret - 6 + 4] - '0';
	}
        cur_modeB = B.imode;
	return B.imode;
}

void RIG_TT566::set_bwA(int bw)
{
	A.iBW = bw;
	cmd = TT566setBWa;
	cmd.append(RIG_TT566widths[bw]);
	cmd += '\r';
	sendCommand(cmd);
	showresp(WARN, ASC, "set bw A", cmd, respstr);
}

int RIG_TT566::get_bwA()
{
	cmd = TT566getBWa;
	int ret = sendCommand(cmd, strlen(TT566rspBWa)+5);
	if (ret >= 8) {
		string bwstr = "";
		if (respstr.length() == 9) bwstr = respstr.substr(ret - 9 + 4, 4);
		if (respstr.length() == 8) bwstr = respstr.substr(ret - 8 + 4, 3);
		if (respstr.empty()) return A.iBW;
		int i = 0;
		while( RIG_TT566widths[i] != NULL) {
			if (bwstr == RIG_TT566widths[i]) {
				A.iBW = i;
				break;
			}
			i++;
		}
	}
	return A.iBW;
}

void RIG_TT566::set_bwB(int bw)
{
	B.iBW = bw;
	cmd = TT566setBWb;
	cmd.append(RIG_TT566widths[bw]);
	cmd += '\r';
	sendCommand(cmd);
	showresp(WARN, ASC, "set bw B", cmd, respstr);
}

int RIG_TT566::get_bwB()
{
	cmd = TT566getBWb;
	int ret = sendCommand(cmd, strlen(TT566rspBWb)+5);
	if (ret >= 8) {
		string bwstr = "";
		if (respstr.length() == 9) bwstr = respstr.substr(ret - 9 + 4, 4);
		if (respstr.length() == 8) bwstr = respstr.substr(ret - 8 + 4, 3);
		if (respstr.empty()) return B.iBW;
		int i = 0;
		while( RIG_TT566widths[i] != NULL) {
			if (bwstr == RIG_TT566widths[i]) {
				B.iBW = i;
				break;
			}
			i++;
		}
	}
	return B.iBW;
}


int RIG_TT566::get_modetype(int n)
{
	return RIG_TT566_mode_type[n];
}

int  RIG_TT566::get_power_control(void)
{
	cmd = TT566getPWR;
	sendCommand(cmd, strlen(TT566rspPWR)+4);
	size_t p = respstr.rfind(TT566rspPWR);
	if (p == string::npos) return 0;
	int pwr;
	sscanf(&respstr[p + strlen(TT566rspPWR)], "%d", &pwr);
	return pwr;
}

void RIG_TT566::set_power_control(double val) 
{
	cmd = TT566setPWR;
	cmd.append(to_decimal(val,3));
	cmd += '\r';
	sendCommand(cmd);
	showresp(WARN, ASC, "set power", cmd, respstr);
}

void RIG_TT566::set_auto_notch(int v)
{
	cmd = TT566setANa;
	cmd += v ? "1\r" : "0\r";
	sendCommand(cmd);
	showresp(WARN, ASC, "set AN", cmd, respstr);
}

int  RIG_TT566::get_auto_notch()
{
	cmd = TT566getANa;
	int ret = sendCommand(cmd, strlen(TT566rspANa)+2);
	if (ret >= 7)
		return respstr[ret - 7 + 5] - '0';
	return 0;
}


int  RIG_TT566::get_smeter()
{
	int dbm = 0;
	cmd = TT566getSMETER;
	int ret = sendCommand(cmd, strlen(TT566rspSMETER)+12);
	showresp(WARN, ASC, "get smeter", cmd, respstr);
        if ((cur_modeA != LCW) && (cur_modeA != UCW)) {
	    if (ret != (int)strlen(TT566rspSMETER)+8) {
                if (respstr.rfind(TT566rspPOUT) != string::npos) {
                    ptt_ = 1;
                    PTT = true;
                    Fl::awake(update_UI_PTT);
                 }
                 return 0;
            }
        }
	size_t p = respstr.rfind(TT566rspSMETER);
	if (p == string::npos) return 0;
	sscanf(&respstr[p + strlen(TT566rspSMETER)], "%d", &dbm);
	return 5 * dbm / 9; // 90 = S9 --> 50% of full scale of flrig display
}

int  RIG_TT566::get_swr()
{
	float swr = (sqrtf(fwdpwr) + sqrtf(refpwr))/(sqrt(fwdpwr) - sqrt(refpwr) + .0001);
	swr -= 1.0;
	swr *= 25.0;
	if (swr < 0) swr = 0;
	if (swr > 100) swr = 100;
	return (int)swr;

}

int  RIG_TT566::get_power_out()
{
	cmd = TT566getPOUT;
	fwdpwr = 0; refpwr = 0;
	int ret = sendCommand(cmd, strlen(TT566rspPOUT)+12);
	showresp(WARN, ASC, "get pout", cmd, respstr);
        if ((cur_modeA != LCW) && (cur_modeA != UCW)) {
	    if (ret != (int)strlen(TT566rspPOUT)+12) {
                 if (respstr.rfind(TT566rspSMETER) != string::npos) {
                         ptt_ = 0;
                         PTT = false;
                         Fl::awake(update_UI_PTT);
                 }
                 return 0;
            }
        } 
	size_t p = respstr.rfind(TT566rspPOUT);
	if (p == string::npos) return 0;
	sscanf(&respstr[p + strlen(TT566rspPOUT)], "%d", &fwdpwr);
	size_t n = 4;
	while ( respstr[p + n] != 'R' && n < respstr.length()) n++;
	if (n < respstr.length()) n++;
	sscanf(&respstr[p + n], "%d", &refpwr);
	return fwdpwr;
}

int RIG_TT566::get_PTT()
{
        return ptt_;
}

int RIG_TT566::get_volume_control()
{
	cmd = TT566getVOLa;
	int ret = sendCommand(cmd, strlen(TT566rspVOLa)+4);
	showresp(WARN, ASC, "get vol", cmd, respstr);
	if (ret < (int)strlen(TT566rspVOLa)+4) return 0;
	int val = 0;
	size_t p = respstr.rfind(TT566rspVOLa);
	sscanf(&respstr[p + strlen(TT566rspVOLa)], "%d", &val);
        val = (100*val)/255;
	return val;
}

void RIG_TT566::set_volume_control(int val)
{
        val = (255*val)/100;
	cmd = TT566setVOLa;
	cmd.append(to_decimal(val,3));
	cmd += '\r';
	sendCommand(cmd);
	showresp(WARN, ASC, "set vol", cmd, respstr);
}

void RIG_TT566::set_mic_gain(int val)
{
	cmd = TT566setMIC;
	cmd.append(to_decimal(val,3));
	cmd += '\r';
	sendCommand(cmd);
	showresp(WARN, ASC, "set mic", cmd, respstr);
}

int  RIG_TT566::get_mic_gain()
{
	cmd = TT566getMIC;
	int ret = sendCommand(cmd, strlen(TT566rspMIC)+4);
	showresp(WARN, ASC, "get mic", cmd, respstr);
	if (ret < (int)strlen(TT566rspMIC)+4) return 0;
	int val = 0;
	size_t p = respstr.rfind(TT566rspMIC);
	sscanf(&respstr[p + strlen(TT566rspMIC)], "%d", &val);
	return val;
}



/*
void RIG_TT566::set_if_shift(int val)
{
	char sznum[6];
	snprintf(sznum, sizeof(sznum), "%d", val);
	cmd = TT566getPBTa;
	cmd.append(sznum);
	cmd += '\r';
	sendCommand(cmd);
}

bool RIG_TT566::get_if_shift(int &val)
{
	int retval = 0;
	cmd = TT566getPBTa;
	sendCommand(cmd, strlen(TT566rspPBTa)+6);
	size_t p = respstr.rfind(TT566rspPBTa);
	if (p == string::npos) return false;
	sscanf(&respstr[p + 4], "%d", &retval);
	val = retval;
	if (val) return true;
	return false;
}
*/

void RIG_TT566::set_rf_gain(int val)
{
	cmd = TT566setRFGa;
	char sznum[5];
	snprintf(sznum, sizeof(sznum), "%d\r", val);
	cmd.append(sznum);
	sendCommand(cmd);
	showresp(WARN, ASC, "set rfgain", cmd, respstr);
}

int  RIG_TT566::get_rf_gain()
{
	cmd = TT566getRFGa;
	int retval = 0;
	sendCommand(cmd, strlen(TT566rspRFGa)+4);
	showresp(WARN, ASC, "get rfgain", cmd, respstr);
	size_t p = respstr.rfind(TT566rspRFGa);
	if (p == string::npos) return retval;
	sscanf(&respstr[p + strlen(TT566rspRFGa)], "%d", &retval);
	return retval;
}

int  RIG_TT566::next_attenuator()
{
	switch (atten_level) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 3;
		case 3: return 0;
	}
	return 0;
}

void RIG_TT566::set_attenuator(int val)
{
	atten_level = val;
	cmd = TT566setATTa;

	switch (atten_level) {
		case 0: atten_label("0 dB", false); cmd += '0'; break;
		case 1: atten_label("6 dB", true); 	cmd += '1'; break;
		case 2: atten_label("12 dB", true); cmd += '2'; break;
		case 3: atten_label("18 dB", true); cmd += '3'; break;
	}
	cmd += '\r';
	sendCommand(cmd);
	showresp(WARN, ASC, "set att A", cmd, respstr);
}


int RIG_TT566::get_attenuator()
{
	cmd = TT566getATTa;
	int ret = sendCommand(cmd, strlen(TT566rspATTa)+2);
	showresp(WARN, ASC, "get att A", cmd, respstr);
	int val = atten_level;
	if (ret >= 3) {
		size_t p = respstr.rfind(TT566rspATTa);
		if (p != string::npos) val = (respstr[p + strlen(TT566rspATTa)] - '0');
	}
	if (atten_level != val) atten_level = val;
	switch (atten_level) {
		case 0: atten_label("0 dB", false); break;
		case 1: atten_label("6 dB", true); break;
		case 2: atten_label("12 dB", true); break;
		case 3: atten_label("18 dB", true); break;
	}
	return atten_level;
}

void RIG_TT566::set_preamp(int val)
{
	cmd = TT566setPREAMP;
	cmd += val ? "1\r" : "0\r";
	sendCommand(cmd);
	showresp(WARN, ASC, "set pre", cmd, respstr);
}

int RIG_TT566::get_preamp()
{
	cmd = TT566getPREAMP;
	int ret = sendCommand(cmd, strlen(TT566rspPREAMP)+2);
	if (ret >= 6)
		return respstr[ret - 6 + 4] - '0';
	return 0;
}

void RIG_TT566::set_noise(bool val)
{
	cmd = TT566setNBa;
	cmd += val ? "1\r" : "0\r";
	sendCommand(cmd);
	showresp(WARN, ASC, "set NB", cmd, respstr);
}

int  RIG_TT566::get_noise()
{
	cmd = TT566getNBa;
	int ret = sendCommand(cmd, strlen(TT566rspNBa)+2);
	if (ret >= 6)
		return respstr[ret - 6 + 4] - '0';
	return 0;
}

