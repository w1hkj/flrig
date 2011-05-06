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
		"USB", "LSB", "CWU", "CWL", "AM", "FM", NULL};
static const int  RIG_TT599_def_bw[] = { 19, 19, 7, 7, 24, 19 };
static const char RIG_TT599_mode_type[] = {'U', 'L', 'U', 'L', 'U', 'U'};

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
	comm_echo = false;
	comm_rtscts = true;
	comm_rtsplus = false;
	comm_dtrplus = true;
	comm_catptt = true;
	comm_rtsptt = false;
	comm_dtrptt = false;
	modeA = 1;
	bwA = 16;
	
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

void RIG_TT599::initialize()
{
	cmd = "X\r";
	sendCommand(cmd, 0);
	MilliSleep(200);
	readResponse();
	showresp(WARN, ASC, "init", cmd, replystr);

	split = false;
	cmd = "*KVAAA\r";
	sendCommand(cmd, 0);
	MilliSleep(200);
	readResponse();
	showresp(WARN, ASC, "normal", cmd, replystr);
}

long RIG_TT599::get_vfoA ()
{
	size_t p;
	cmd = "?AF\r";
	sendCommand(cmd);
	showresp(WARN, ASC, "get vfoA", cmd, replystr);
	if ((p = replystr.rfind("@AF")) != string::npos) {
		freqA = fm_decimal(&replystr[p+3], 8);
	}
	return freqA;
}

void RIG_TT599::set_vfoA (long freq)
{
	freqA = freq;
	cmd = "*AF";
	cmd.append( to_decimal( freq, 8 ) );
	cmd += '\r';
	sendCommand(cmd);
	showresp(WARN, ASC, "set vfoA", cmd, replystr);
}

long RIG_TT599::get_vfoB ()
{
	size_t p;
	cmd = "?BF\r";
	sendCommand(cmd);
	showresp(WARN, ASC, "get vfoB", cmd, replystr);
	if ((p = replystr.rfind("@BF")) != string::npos) {
		freqB = fm_decimal(&replystr[p+3], 8);
	}
	return freqB;
}

void RIG_TT599::set_vfoB (long freq)
{
	freqB = freq;
	cmd = "*BF";
	cmd.append( to_decimal( freq, 8 ) );
	cmd += '\r';
	LOG_WARN("set B : %s", cmd.c_str());
	sendCommand(cmd);
	showresp(WARN, ASC, "set vfoB", cmd, replystr);
}

void RIG_TT599::set_PTT_control(int val)
{
	cmd = val ? "*TK\r" : "*TU\r";
	sendCommand(cmd);
	showresp(WARN, ASC, "set PTT", cmd, replystr);
}

void RIG_TT599::set_modeA(int md)
{
	modeA = md;
	cmd = "*RMM";
	cmd += '0' + md;
	cmd += '\r';
	sendCommand(cmd);
	showresp(WARN, ASC, "set modeA", cmd, replystr);
}

int RIG_TT599::get_modeA()
{
	size_t p;
	cmd = "?RMM\r";
	sendCommand(cmd);
	showresp(WARN, ASC, "get modeA", cmd, replystr);
	if ((p = replystr.rfind("@RMM")) != string::npos) {
		modeA = replystr[p+4] - '0';
	}
	return modeA;
}

void RIG_TT599::set_modeB(int md)
{
	modeB = md;
	cmd = "*RMM";
	cmd += '0' + md;
	cmd += '\r';
	sendCommand(cmd);
	showresp(WARN, ASC, "set modeB", cmd, replystr);
}

int RIG_TT599::get_modeB()
{
	size_t p;
	cmd = "?RMM\r";
	sendCommand(cmd);
	showresp(WARN, ASC, "get modeB", cmd, replystr);
	if ((p = replystr.rfind("@RMM")) != string::npos) {
		modeB = replystr[p+4] - '0';
	}
	return modeB;
}

void RIG_TT599::set_bwA(int bw)
{
	cmd = "*RMF";
	cmd.append(RIG_TT599widths[bw]);
	cmd += '\r';
	sendCommand(cmd);
	showresp(WARN, ASC, "set bwA", cmd, replystr);
	bwA = bw;
}

int RIG_TT599::get_bwA()
{
	size_t p;
	cmd = "?RMF\r";
	sendCommand(cmd);
	showresp(WARN, ASC, "get bwA", cmd, replystr);

	if ((p = replystr.rfind("@RMF")) != string::npos) {
		string bwstr = replystr.substr(p+4);
		p = bwstr.find("\r");
		if ( p != string::npos)
			bwstr.erase(p, string::npos);
		if (bwstr.empty()) return bwA;
		int i = 0;
		while( RIG_TT599widths[i] != NULL) {
			if (bwstr == RIG_TT599widths[i]) {
				bwA = i;
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
	sendCommand(cmd);
	showresp(WARN, ASC, "set bwB", cmd, replystr);
	bwB = bw;
}

int RIG_TT599::get_bwB()
{
	size_t p;
	cmd = "?RMF\r";
	sendCommand(cmd);
	showresp(WARN, ASC, "get bwB", cmd, replystr);

	if ((p = replystr.rfind("@RMF")) != string::npos) {
		string bwstr = replystr.substr(p+4);
		p = bwstr.find("\r");
		if ( p != string::npos)
			bwstr.erase(p, string::npos);
		if (bwstr.empty()) return bwB;
		int i = 0;
		while( RIG_TT599widths[i] != NULL) {
			if (bwstr == RIG_TT599widths[i]) {
				bwA = i;
				break;
			}
			i++;
		}
	}
	return bwB;
}

int  RIG_TT599::adjust_bandwidth(int m)
{
	return RIG_TT599_def_bw[m];
}

int  RIG_TT599::def_bandwidth(int m)
{
	return RIG_TT599_def_bw[m];
}

int RIG_TT599::get_modetype(int n)
{
	return RIG_TT599_mode_type[n];
}

void RIG_TT599::set_preamp(int val)
{
	cmd = val ? "*RME1\r" : "*RME0\r";
	sendCommand(cmd);
	showresp(WARN, ASC, "set preamp", cmd, replystr);
}

int RIG_TT599::get_preamp()
{
	size_t p;
	cmd = "?RME\r";
	sendCommand(cmd);
	showresp(WARN, ASC, "get preamp", cmd, replystr);

	if ((p = replystr.rfind("@RME")) != string::npos)
		return replystr[p+4] - '0';
	return 0;
}

//void RIG_TT599::set_agc_level()
//{
// *RMAF - fast *RMAM - medium *RMAS - slow
//}

int  RIG_TT599::get_power_control(void)
{
	size_t p;
	cmd = "?TP\r";
	sendCommand(cmd);
	showresp(WARN, ASC, "get pc", cmd, replystr);

	if ((p = replystr.rfind("@TP")) != string::npos) {
		int pwr = 0;
		sscanf(&replystr[p+3], "%d", &pwr);
		return pwr;
	}
	return progStatus.power_level;//0;
}

void RIG_TT599::set_power_control(double val) 
{
	char szCmd[12];
	snprintf(szCmd, sizeof(szCmd), "*TP%d\r", (int)val);
	sendCommand(szCmd);
	showresp(WARN, ASC, "set pc", cmd, replystr);
}

void RIG_TT599::set_auto_notch(int v)
{
	cmd = v ? "*RMNA1\r" : "*RMNA0\r";
	sendCommand(cmd);
	showresp(WARN, ASC, "set AN", cmd, replystr);
}

int  RIG_TT599::get_auto_notch()
{
	cmd = "?RMNA\r";
	sendCommand(cmd);
	showresp(WARN, ASC, "get AN", cmd, replystr);

	if (replystr.rfind("@RMNA1") == string::npos)
		return 0;
	return 1;
}

void RIG_TT599::set_attenuator(int val)
{
	cmd = val ? "*RMT1\r" : "*RMT0\r";
	sendCommand(cmd);
	showresp(WARN, ASC, "set ATT", cmd, replystr);
}

int  RIG_TT599::get_attenuator()
{
	size_t p;
	cmd = "?RMT\r";
	sendCommand(cmd);
	showresp(WARN, ASC, "get ATT", cmd, replystr);

	if ((p = replystr.rfind("@RMT1")) != string::npos)
		return 1;
	return 0;
}

int smeter_count = 10;
int  RIG_TT599::get_smeter()
{
	size_t p;
	int dbm = 0;
	cmd = "?S\r";
	sendCommand(cmd);
	showresp(WARN, ASC, "get Smeter", cmd, replystr);

	if ((p = replystr.rfind("@SRM")) != string::npos)
		sscanf(&replystr[p+4], "%d", &dbm);
	return 5 * dbm / 6;
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
	size_t p;
	fwdpwr = 0; refpwr = 0;
	cmd = "?S\r";
	sendCommand(cmd);
	showresp(WARN, ASC, "get pout", cmd, replystr);

	if ((p = replystr.rfind ("@STF")) != string::npos) {
		sscanf(&replystr[p+4], "%d", &fwdpwr);
		p = replystr.find("R", p+4);
		if (p != string::npos) {
			sscanf(&replystr[p+1], "%d", &refpwr);
			refpwr /= 10;
		}
	}
	return fwdpwr;
}

void  RIG_TT599:: set_split(bool val)
{
	split = val;
	cmd = val ? "*KVAAB\r" : "*KVAAA\r";
	sendCommand(cmd);
	showresp(WARN, ASC, "set split", cmd, replystr);
}

