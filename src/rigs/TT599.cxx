/*
 * TenTec 599 (Eagle) driver
 *
 * a part of flrig
 * 
 * Copyright 2011, Dave Freese, W1HKJ
 * 
 */

#include "TT599.h"

//=============================================================================
// TT-599

const char RIG_TT599name_[] = "Eagle";

const char *RIG_TT599modes_[] = {
		"USB", "LSB", "CWL", "AM", "FM", NULL};
static const char RIG_TT599_mode_type[] = {'U', 'L', 'L', 'U', 'U'};

const char *RIG_TT599widths[] = { 
"100",  "200",  "300",  "400",  "500",
"600",  "700",  "800",  "900",  "1000",
"1200", "1400", "1600", "1800", "2000",
"2200", "2400", "2600", "2800", "3000",
"3200", "3400", "3600", "3800", "4000",
NULL};


RIG_TT599::RIG_TT599() {
	name_ = RIG_TT599name_;
	modes_ = RIG_TT599modes_;
	bandwidths_ = RIG_TT599widths;
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

long RIG_TT599::get_vfoA ()
{
	cmd = "?AF\r";
	if (sendCommand(cmd, 12) == 12) {
		freqA = fm_decimal(&replystr[3], 8);
	}
	LOG_INFO("vfo A (%d)\n%s\n%s", 
		replystr.length(), replystr.c_str(), str2hex(replystr.c_str(), replystr.length()));
	return freqA;
}

void RIG_TT599::set_vfoA (long freq)
{
	freqA = freq;
	cmd = "*AF";
	cmd.append( to_decimal( freq, 8 ) );
	cmd += '\r';
	LOG_INFO("set A : %s", cmd.c_str());
	sendCommand(cmd, 0);
}

long RIG_TT599::get_vfoB ()
{
	cmd = "?BF\r";
	if (sendCommand(cmd, 12) == 12) {
		freqB = fm_decimal(&replystr[3], 8);
	}
	LOG_INFO("vfo B (%d) (%d)\n%s\n%s", 
		replystr.length(), replystr.c_str(), str2hex(replystr.c_str(), replystr.length()));
	return freqB;
}

void RIG_TT599::set_vfoB (long freq)
{
	freqB = freq;
	cmd = "*BF";
	cmd.append( to_decimal( freq, 8 ) );
	cmd += '\r';
	LOG_INFO("set B : %s", cmd.c_str());
	sendCommand(cmd, 0);
}

void RIG_TT599::set_PTT_control(int val)
{
	cmd = val ? "*TK\r" : "*TU\r";
	sendCommand(cmd,0);
}

void RIG_TT599::set_modeA(int md)
{
	modeA = md;
	cmd = "*RMM";
	cmd += '0' + md;
	cmd += '\r';
	sendCommand(cmd, 6);
}

int RIG_TT599::get_modeA()
{
	cmd = "?RMM\r";
	if( sendCommand (cmd, 6 ) == 6) {
		modeA = replystr[4] - '0';
	}
	return modeA;
}

void RIG_TT599::set_modeB(int md)
{
	modeB = md;
	cmd = "*RMM";
	cmd += '0' + md;
	cmd += '\r';
	sendCommand(cmd, 6);
}

int RIG_TT599::get_modeB()
{
	cmd = "?RMM\r";
	if( sendCommand (cmd, 6 ) == 6) {
		modeB = replystr[4] - '0';
	}
	return modeB;
}

void RIG_TT599::set_bwA(int bw)
{
	cmd = "*RMF";
	cmd.append(RIG_TT599widths[bw]);
	cmd += '\r';
	sendCommand(cmd, 0);
	bwA = bw;
}

int RIG_TT599::get_bwA()
{
	cmd = "?RMF\r";
	if (sendCommand(cmd, 9) == 9) {
		string bwstr = "";
		if (replystr.length() == 9) bwstr = replystr.substr(4, 4);
		if (replystr.length() == 8) bwstr = replystr.substr(4, 3);
		if (replystr.empty()) return bwA;
		int i = 0;
		while( RIG_TT599widths[i] != NULL) {
			if (bwstr == RIG_TT599widths[i]) {
				bwB = i;
				break;
			}
			i++;
		}
	}
	return bwA;
}

void RIG_TT599::set_bwB(int bw)
{
	cmd = "*RMF";
	cmd.append(RIG_TT599widths[bw]);
	cmd += '\r';
	sendCommand(cmd, 0);
	bwB = bw;
}

int RIG_TT599::get_bwB()
{
	cmd = "?RMF\r";
	if (sendCommand(cmd, 9) == 9) {
		string bwstr = "";
		if (replystr.length() == 9) bwstr = replystr.substr(4, 4);
		if (replystr.length() == 8) bwstr = replystr.substr(4, 3);
		if (replystr.empty()) return bwB;
		int i = 0;
		while( RIG_TT599widths[i] != NULL) {
			if (bwstr == RIG_TT599widths[i]) {
				bwB = i;
				break;
			}
			i++;
		}
	}
	return bwB;
}


int RIG_TT599::get_modetype(int n)
{
	return RIG_TT599_mode_type[n];
}

void RIG_TT599::set_preamp(int val)
{
	cmd = val ? "*RME1\r" : "*RME0\r";
	sendCommand(cmd,0);
}

int RIG_TT599::get_preamp()
{
	cmd = "?RME\r";
	if (sendCommand(cmd, 6) == 6)
		return replystr[4] - '0';
	return 0;
}

//void RIG_TT599::set_agc_level()
//{
// *RMAF - fast *RMAM - medium *RMAS - slow
//}

int  RIG_TT599::get_power_control(void)
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

void RIG_TT599::set_power_control(double val) 
{
	char szCmd[12];
	snprintf(szCmd, sizeof(szCmd), "*TP%d\r", (int)val);
	sendCommand(szCmd, 0);
}

void RIG_TT599::set_auto_notch(int v)
{
	cmd = v ? "*RMNA1\r" : "*RMNA0\r";
	sendCommand(cmd, 0);
}

int  RIG_TT599::get_auto_notch()
{
	cmd = "?RMNA\r";
	if (sendCommand(cmd, 7) == 7)
		return replystr[5] - '0';
	return 0;
}

void RIG_TT599::set_attenuator(int val)
{
	cmd = val ? "*RMT1\r" : "*RMT0\r";
	sendCommand(cmd, 0);
}

int  RIG_TT599::get_attenuator()
{
	cmd = "?RMT\r";
	if (sendCommand(cmd, 6) == 6)
		return replystr[4] - '0';
	return 0;
}

int  RIG_TT599::get_smeter()
{
	int dbm = 0;
	cmd = "?S\r";
	if (sendCommand(cmd, 9) == 9) {
		if (replystr.find("@SRM") == 0)
			sscanf(&replystr[4], "%d", &dbm);
		LOG_INFO("smeter: %s", str2hex(replystr.c_str(), replystr.length()));
	}
	return dbm;
}

int  RIG_TT599::get_swr()
{
	float swr = (sqrtf(fwdpwr) + sqrtf(refpwr))/(sqrt(fwdpwr) - sqrt(refpwr) + .0001);
	swr -= 1.0;
	swr *= 25.0;
	if (swr < 0) swr = 0;
	if (swr > 100) swr = 100;
	return (int)swr;

}

int  RIG_TT599::get_power_out()
{
	fwdpwr = 0; refpwr = 0;
	cmd = "?S\r";
	if (sendCommand(cmd, 12) == 12) {
		if (replystr.find("@SRF") == 0) {
			sscanf(&replystr[4], "%d", &fwdpwr);
			size_t n = 4;
			while ( replystr[n] != 'R' && n < replystr.length()) n++;
			if (n < replystr.length()) n++;
			sscanf(&replystr[n], "%d", &refpwr);
		}
	}
	return fwdpwr;
}
