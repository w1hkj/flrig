/*
 * TenTec 566 (Orion II) driver
 *
 * a part of flrig
 * 
 * Copyright 2011, Dave Freese, W1HKJ
 * 
 */

#include "TT566.h"

//=============================================================================
// TT-566

const char RIG_TT566name_[] = "Orion-II";

const char *RIG_TT566modes_[] = {
		"USB", "LSB", "UCW", "LCW", "AM", "FM", "FSK", NULL};
static const char RIG_TT566_mode_type[] = {'U', 'L', 'U', 'L', 'U', 'U', 'L'};

const char *RIG_TT566widths[] = { 
"100",  "200",  "300",  "400",  "500",
"600",  "700",  "800",  "900",  "1000",
"1200", "1400", "1600", "1800", "2000",
"2200", "2400", "2600", "2800", "3000",
"3200", "3400", "3600", "3800", "4000",
"4500", "5000", "5500", "6000",
NULL};

static char TT566setFREQa[]		= "*A"; // binary data
static char TT566getFREQa[]		= "?A\r";
static char TT566rspFREQa[]		= "@A";

static char TT566setFREQb[]		= "*B";
static char TT566getFREQb[]		= "?B\r";
static char TT566rspFREQb[]		= "@B";

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

static char TT566setVOLb[]		= "*US";
static char TT566getVOLb[]		= "?US\r";
static char TT566rspVOLb[]		= "@US";

static char TT566setPBTa[]		= "*RMP";
static char TT566getPBTa[]		= "?RMP\r";
static char TT566rspPBTa[]		= "@RMP";

static char TT566setPBTb[]		= "*RSP";
static char TT566getPBTb[]		= "?RSP\r";
static char TT566rspPBTb[]		= "@RSP";

static char TT566setAGCa[]		= "*RMA";
static char TT566getAGCa[]		= "?RMA\r";
static char TT566rspAGCa[]		= "@RMA";

static char TT566setAGCb[]		= "*RSA";
static char TT566getAGCb[]		= "?RSA\r";
static char TT566rspAGCb[]		= "@RSA";

static char TT566setRFGa[]		= "*RMG";
static char TT566getRFGa[]		= "?RMG\r";
static char TT566rspRFGa[]		= "@RMG";

static char TT566setRFGb[]		= "*RSG";
static char TT566getRFGb[]		= "?RSG\r";
static char TT566rspRFGb[]		= "@RSG";

static char TT566setATTa[]		= "*RMT";
static char TT566getATTa[]		= "?RMT\r";
static char TT566rspATTa[]		= "@RMT";

static char TT566setATTb[]		= "*RST";
static char TT566getATTb[]		= "?RST\r";
static char TT566rspATTb[]		= "@RST";

static char TT566setSQLa[]		= "*RMS";
static char TT566getSQLa[]		= "?RMS\r";
static char TT566rspSQLa[]		= "@RMS";

static char TT566setSQLb[]		= "*RSS";
static char TT566getSQLb[]		= "?RSS\r";
static char TT566rspSQLb[]		= "@RSS";

static char TT566setANa[]		= "*RMNA";
static char TT566getANa[]		= "?RMNA\r";
static char TT566rspANa[]		= "@RMNA";

static char TT566setANb[]		= "*RSNA";
static char TT566getANb[]		= "?RSNA\r";
static char TT566rspANb[]		= "@RSNA";

static char TT566setNBa[]		= "*RMNB";
static char TT566getNBa[]		= "?RMNB\r";
static char TT566rspNBa[]		= "@RMNB";

static char TT566setNBb[]		= "*RSNB";
static char TT566getNBb[]		= "?RSNB\r";
static char TT566rspNBb[]		= "@RSNB";

static char TT566setMIC[]		= "*TM";
static char TT566getMIC[]		= "?TM\r";
static char TT566rspMIC[]		= "@TM";

static char TT566setPWR[]		= "*TP";
static char TT566getPWR[]		= "?TP\r";
static char TT566rspPWR[]		= "@TP";

static char TT566setTBW[]		= "*TF";
static char TT566getTBW[]		= "?TF\r";
static char TT566rspTBW[]		= "@TF";

static char TT566setTUNE[]		= "*TT";
static char TT566getTUNE[]		= "?TT\r";
static char TT566rspTUNE[]		= "@TT";

static char TT566getSMETER[]	= "?S\r";
static char TT566rspSMETER[]	= "@SRM";

static char TT566getPOUT[]		= "?S\r";
static char TT566rspPOUT[]		= "@STF";

static char TT566setPREAMP[]	= "*RME"; // ONLY AVAILABLE ON PRIMARY RX
static char TT566getPREAMP[]	= "?RME\r";
static char TT566rspPREAMP[]	= "@RME";

static char TT566setRITa[]		= "*RMR";
static char TT566getRITa[]		= "?RMR\r";
static char TT566rspRITa[]		= "@RMR";

static char TT566setRITb[]		= "*RSR";
static char TT566getRITb[]		= "?RSR\r";
static char TT566rspRITb[]		= "@RSR";

static char TT566setXIT[]		= "*RMX"; // ONLY AVAILABLE ON PRIMARY RX
static char TT566getXIT[]		= "?RMX\r";
static char TT566rspXIT[]		= "@RMX";

static char TT566setPTT[]		= "*T";

RIG_TT566::RIG_TT566() {
	name_ = RIG_TT566name_;
	modes_ = RIG_TT566modes_;
	bandwidths_ = RIG_TT566widths;
	comm_baudrate = BR57600;
	stopbits = 1;
	comm_retries = 2;
	comm_wait = 10;
	comm_timeout = 50;
	comm_echo = true;
	comm_rtscts = false;
	comm_rtsplus = true;
	comm_dtrplus = true;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	def_mode = modeB = modeA = A.imode = B.imode = 1;
	def_bw = bwB = bwA = A.iBW = B.iBW = 19;
	def_freq = freqB = freqA = A.freq = B.freq = 14070000;
	can_change_alt_vfo = true;

	max_power = 100;

//	has_ifshift_control =
//	has_agc_level =
	has_smeter =
	has_power_out =
	has_swr_control =
	has_rf_control =
	has_volume_control =
	has_swr_control =
	has_mode_control =
	has_bandwidth_control =
	has_ptt_control =
	has_preamp_control =
	has_auto_notch =
	has_attenuator_control =
	has_power_control = true;

};

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


long RIG_TT566::get_vfoA ()
{
	cmd = TT566getFREQa;
	int ret = sendCommand(cmd);
	if (ret < 8) return A.freq;
	size_t p = replystr.rfind("A");
	if (p == string::npos) return A.freq;

	int f = 0;
	for (size_t n = 1; n < 5; n++) {
		f = f*256 + (unsigned char)replystr[p+n];
		A.freq = f;
	}

	return A.freq;
}

void RIG_TT566::set_vfoA (long freq)
{
	A.freq = freq;
	cmd = TT566setFREQa;
	cmd[5] = freq & 0xff; freq = freq >> 8;
	cmd[4] = freq & 0xff; freq = freq >> 8;
	cmd[3] = freq & 0xff; freq = freq >> 8;
	cmd[2] = freq & 0xff;
	sendCommand(cmd);
	return;
}

long RIG_TT566::get_vfoB ()
{
	cmd = TT566getFREQb;
	int ret = sendCommand(cmd);
	if (ret < 8) return B.freq;
	size_t p = replystr.rfind("B");
	if (p == string::npos) return B.freq;

	int f = 0;
	for (size_t n = 1; n < 5; n++) {
		f = f*256 + (unsigned char)replystr[p+n];
		B.freq = f;
	}

	return B.freq;
}

void RIG_TT566::set_vfoB (long freq)
{
	B.freq = freq;
	cmd = TT566setFREQb;
	cmd[5] = freq & 0xff; freq = freq >> 8;
	cmd[4] = freq & 0xff; freq = freq >> 8;
	cmd[3] = freq & 0xff; freq = freq >> 8;
	cmd[2] = freq & 0xff;
	sendCommand(cmd);
	return;
}

void RIG_TT566::set_PTT_control(int val)
{
	cmd = TT566setPTT;
	cmd += val ? "K\r" : "U\r";
	sendCommand(cmd);
}

void RIG_TT566::set_modeA(int md)
{
	A.imode = md;
	cmd = TT566setMODEa;
	cmd += '0' + md;
	cmd += '\r';
	sendCommand(cmd);
}

int RIG_TT566::get_modeA()
{
	cmd = TT566getMODEa;
	int ret = sendCommand (cmd);
	if (ret >= 6) {
		A.imode = replystr[ret - 6 + 4] - '0';
	}
	return A.imode;
}

void RIG_TT566::set_modeB(int md)
{
	B.imode = md;
	cmd = TT566setMODEb;
	cmd += '0' + md;
	cmd += '\r';
	sendCommand(cmd);
}

int RIG_TT566::get_modeB()
{
	cmd = TT566getMODEb;
	int ret = sendCommand (cmd);
	if (ret >= 6) {
		B.imode = replystr[ret - 6 + 4] - '0';
	}
	return B.imode;
}

void RIG_TT566::set_bwA(int bw)
{
	A.iBW = bw;
	cmd = TT566setBWa;
	cmd.append(RIG_TT566widths[bw]);
	cmd += '\r';
	sendCommand(cmd);
}

int RIG_TT566::get_bwA()
{
	cmd = TT566getBWa;
	int ret = sendCommand(cmd);
	if (ret >= 8) {
		string bwstr = "";
		if (replystr.length() == 9) bwstr = replystr.substr(ret - 9 + 4, 4);
		if (replystr.length() == 8) bwstr = replystr.substr(ret - 8 + 4, 3);
		if (replystr.empty()) return A.iBW;
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
}

int RIG_TT566::get_bwB()
{
	cmd = TT566getBWb;
	int ret = sendCommand(cmd);
	if (ret >= 8) {
		string bwstr = "";
		if (replystr.length() == 9) bwstr = replystr.substr(ret - 9 + 4, 4);
		if (replystr.length() == 8) bwstr = replystr.substr(ret - 8 + 4, 3);
		if (replystr.empty()) return B.iBW;
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

void RIG_TT566::set_preamp(int val)
{
	cmd = TT566getPREAMP;
	cmd += val ? "1\r" : "0\r";
	sendCommand(cmd);
}

int RIG_TT566::get_preamp()
{
	cmd = TT566getPREAMP;
	int ret = sendCommand(cmd);
	if (ret >= 6)
		return replystr[ret - 6 + 4] - '0';
	return 0;
}

int  RIG_TT566::get_power_control(void)
{
	cmd = TT566getPWR;
	sendCommand(cmd);
	size_t p = replystr.rfind(TT566rspPREAMP);
	if (p == string::npos) return 0;
	int pwr;
	sscanf(&replystr[p + strlen(TT566rspPREAMP)], "%d", &pwr);
	return pwr;
}

void RIG_TT566::set_power_control(double val) 
{
	cmd = TT566setPWR;
	char szCmd[5];
	snprintf(szCmd, sizeof(szCmd), "%d\r", (int)val);
	cmd.append(szCmd);
	sendCommand(cmd);
}

/*
void RIG_TT566::set_auto_notch(int v)
{
	cmd = v ? "*RMNA1\r" : "*RMNA0\r";
	sendCommand(cmd, 0);
}

int  RIG_TT566::get_auto_notch()
{
	cmd = "?RMNA\r";
	int ret = sendCommand(cmd);
	if (ret >= 7)
		return replystr[ret - 7 + 5] - '0';
	return 0;
}

void RIG_TT566::set_attenuator(int val)
{
	cmd = val ? "*RMT1\r" : "*RMT0\r";
	sendCommand(cmd, 0);
}

int  RIG_TT566::get_attenuator()
{
	cmd = "?RMT\r";
	int ret = sendCommand(cmd);
	if (ret >= 6)
		return replystr[ret - 6 + 4] - '0';
	return 0;
}
*/

int  RIG_TT566::get_smeter()
{
	int dbm = 0;
	cmd = TT566getSMETER;
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get smeter", cmd, replystr);
	if (ret < 12) return 0;
	size_t p = replystr.rfind(TT566rspSMETER);
	if (p == string::npos) return 0;
	sscanf(&replystr[p + strlen(TT566rspSMETER)], "%d", &dbm);
	return 5 * dbm / 6; // 60 = S9 --> 50% of full scale of flrig display
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
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get pout", cmd, replystr);
	if (ret < 12) return 0;
	size_t p = replystr.rfind(TT566rspPOUT);
	if (p == string::npos) return 0;
	sscanf(&replystr[p + strlen(TT566rspPOUT)], "%d", &fwdpwr);
	size_t n = 4;
	while ( replystr[p + n] != 'R' && n < replystr.length()) n++;
	if (n < replystr.length()) n++;
	sscanf(&replystr[p + n], "%d", &refpwr);
	return fwdpwr;
}

int RIG_TT566::get_volume_control()
{
	cmd = TT566getVOLa;
	int ret = sendCommand(cmd);
	showresp(WARN, ASC, "get vol", cmd, replystr);
	if (ret < 6) return 0;
	int val = 0;
	size_t p = replystr.rfind(TT566rspVOLa);
	sscanf(&replystr[p + strlen(TT566rspVOLa)], "%d", &val);
	return val;
}

void RIG_TT566::set_volume_control(int val)
{
	cmd = TT566setVOLa;
	cmd.append(to_decimal(val,2));
	cmd += '\r';
	sendCommand(cmd,0);
}

/*
void RIG_TT566::set_if_shift(int val)
{
	char sznum[6];
	snprintf(sznum, sizeof(sznum), "%d", val);
	cmd = "*RMP";
	cmd.append(sznum);
	cmd += '\r';
	sendCommand(cmd,0);
}

bool RIG_TT566::get_if_shift(int &val)
{
	int retval = 0;
	cmd = "?RMP\r";
	sendCommand(cmd);
	size_t p = replystr.rfind("@RMP");
	if (p == string::npos) return false;
	sscanf(&replystr[p + 4], "%d", &retval);
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
}

int  RIG_TT566::get_rf_gain()
{
	cmd = TT566getRFGa;
	int retval = 0;
	sendCommand(cmd, 8);
	showresp(WARN, ASC, "get rfgain", cmd, replystr);
	size_t p = replystr.rfind(TT566rspRFGa);
	if (p == string::npos) return retval;
	sscanf(&replystr[p + strlen(TT566getRFGa)], "%d", &retval);
	return retval;
}

