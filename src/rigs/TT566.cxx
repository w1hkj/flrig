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
NULL};


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
	modeA = 1;
	bwA = 19;
	
	max_power = 100;

	has_rf_control =
	has_volume_control =
	has_ifshift_control =
	has_swr_control =
	has_mode_control =
	has_bandwidth_control =
	has_ptt_control =
	has_preamp_control =
	has_auto_notch =
	has_attenuator_control =
	has_power_control = true;
//	has_agc_level = true;

};

long RIG_TT566::get_vfoA ()
{
	cmd = "?AF\r";
	if (sendCommand(cmd, 12) == 12) {
		freqA = fm_decimal(&replystr[3], 8);
	}
	return freqA;
}

void RIG_TT566::set_vfoA (long freq)
{
	freqA = freq;
	cmd = "*AF";
	cmd.append( to_decimal( freq, 8 ) );
	cmd += '\r';
	sendCommand(cmd, 0);
}

long RIG_TT566::get_vfoB ()
{
	cmd = "?BF\r";
	if (sendCommand(cmd, 12) == 12) {
		freqB = fm_decimal(&replystr[3], 8);
	}
	return freqB;
}

void RIG_TT566::set_vfoB (long freq)
{
	freqB = freq;
	cmd = "*BF";
	cmd.append( to_decimal( freq, 8 ) );
	cmd += '\r';
	sendCommand(cmd, 0);
}

void RIG_TT566::set_PTT_control(int val)
{
	cmd = val ? "*TK\r" : "*TU\r";
	sendCommand(cmd,0);
}

void RIG_TT566::set_modeA(int md)
{
	modeA = md;
	cmd = "*RMM";
	cmd += '0' + md;
	cmd += '\r';
	sendCommand(cmd, 6);
}

int RIG_TT566::get_modeA()
{
	cmd = "?RMM\r";
	if( sendCommand (cmd, 6 ) == 6) {
		modeA = replystr[4] - '0';
	}
	return modeA;
}

void RIG_TT566::set_modeB(int md)
{
	modeB = md;
	cmd = "*RMM";
	cmd += '0' + md;
	cmd += '\r';
	sendCommand(cmd, 6);
}

int RIG_TT566::get_modeB()
{
	cmd = "?RMM\r";
	if( sendCommand (cmd, 6 ) == 6) {
		modeB = replystr[4] - '0';
	}
	return modeB;
}

void RIG_TT566::set_bwA(int bw)
{
	cmd = "*RMF";
	cmd.append(RIG_TT566widths[bw]);
	cmd += '\r';
	sendCommand(cmd, 0);
	bwA = bw;
}

int RIG_TT566::get_bwA()
{
	cmd = "?RMF\r";
	if (sendCommand(cmd, 9) == 9) {
		string bwstr = "";
		if (replystr.length() == 9) bwstr = replystr.substr(4, 4);
		if (replystr.length() == 8) bwstr = replystr.substr(4, 3);
		if (replystr.empty()) return bwA;
		int i = 0;
		while( RIG_TT566widths[i] != NULL) {
			if (bwstr == RIG_TT566widths[i]) {
				bwB = i;
				break;
			}
			i++;
		}
	}
	return bwA;
}

void RIG_TT566::set_bwB(int bw)
{
	cmd = "*RMF";
	cmd.append(RIG_TT566widths[bw]);
	cmd += '\r';
	sendCommand(cmd, 0);
	bwB = bw;
}

int RIG_TT566::get_bwB()
{
	cmd = "?RMF\r";
	if (sendCommand(cmd, 9) == 9) {
		string bwstr = "";
		if (replystr.length() == 9) bwstr = replystr.substr(4, 4);
		if (replystr.length() == 8) bwstr = replystr.substr(4, 3);
		if (replystr.empty()) return bwB;
		int i = 0;
		while( RIG_TT566widths[i] != NULL) {
			if (bwstr == RIG_TT566widths[i]) {
				bwB = i;
				break;
			}
			i++;
		}
	}
	return bwB;
}


int RIG_TT566::get_modetype(int n)
{
	return RIG_TT566_mode_type[n];
}

void RIG_TT566::set_preamp(int val)
{
	cmd = val ? "*RME1\r" : "*RME0\r";
	sendCommand(cmd,0);
}

int RIG_TT566::get_preamp()
{
	cmd = "?RME\r";
	if (sendCommand(cmd, 6) == 6)
		return replystr[4] - '0';
	return 0;
}

//void RIG_TT566::set_agc_level()
//{
// *RMAF - fast *RMAM - medium *RMAS - slow
//}

int  RIG_TT566::get_power_control(void)
{
	cmd = "?TP\r";
	sendCommand(cmd, 0);
	if (replystr.find("@TP") == 0) {
		int pwr;
		sscanf("%d", &replystr[3], &pwr);
		return pwr;
	}
	return 0;
}

void RIG_TT566::set_power_control(double val) 
{
	char szCmd[12];
	snprintf(szCmd, sizeof(szCmd), "*TP%d\r", (int)val);
	sendCommand(szCmd, 0);
}

void RIG_TT566::set_auto_notch(int v)
{
	cmd = v ? "*RMNA1\r" : "*RMNA0\r";
	sendCommand(cmd, 0);
}

int  RIG_TT566::get_auto_notch()
{
	cmd = "?RMNA\r";
	if (sendCommand(cmd, 7) == 7)
		return replystr[5] - '0';
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
	if (sendCommand(cmd, 6) == 6)
		return replystr[4] - '0';
	return 0;
}

int  RIG_TT566::get_smeter()
{
	int dbm = 0;
	cmd = "?S\r";
	sendCommand(cmd, 12);
	if (replystr.find("@SRM") == 0)
		sscanf(&replystr[4], "%d", &dbm);
	LOG_INFO("smeter: %s = %d", str2hex(replystr.c_str(), replystr.length()), dbm);
	return dbm;
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
	fwdpwr = 0; refpwr = 0;
	cmd = "?S\r";
	sendCommand(cmd, 12);
	if (replystr.find("@SRF") == 0) {
		sscanf(&replystr[4], "%d", &fwdpwr);
		size_t n = 4;
		while ( replystr[n] != 'R' && n < replystr.length()) n++;
		if (n < replystr.length()) n++;
		sscanf(&replystr[n], "%d", &refpwr);
	}
	LOG_INFO("pwr out: %s = %d, %d", str2hex(replystr.c_str(), replystr.length()), fwdpwr, refpwr);
	return fwdpwr;
}

int RIG_TT566::get_volume_control()
{
	cmd = "?UM\r";
	sendCommand(cmd, 6);
	int val = 0;
	if (replystr.length() > 4)
		sscanf(&replystr[3], "%d", &val);
	return val;
}

void RIG_TT566::set_volume_control(int val)
{
	cmd = "*UM";
	cmd.append(to_decimal(val,2));
	cmd += '\r';
	sendCommand(cmd,0);
}

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
	sendCommand(cmd, 9);
	if (replystr.find("@RMP") == 0)
		sscanf(&replystr[4], "%d", &retval);
	val = retval;
	if (val) return true;
	return false;
}

void RIG_TT566::set_rf_gain(int val)
{
	char sznum[4];
	snprintf(sznum, sizeof(sznum), "%d", val);
	cmd = "*RMG";
	cmd.append(sznum);
	cmd += '\r';
	sendCommand(cmd,0);
}

int  RIG_TT566::get_rf_gain()
{
	int retval = 0;
	cmd = "?RMG\r";
	sendCommand(cmd, 8);
	if (replystr.find("@RMG") == 0)
		sscanf(&replystr[4], "%d", &retval);
	return retval;
}

