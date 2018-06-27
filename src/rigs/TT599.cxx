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

#include "TT599.h"

//=============================================================================
// TT-599

const char RIG_TT599name_[] = "Eagle";

const char *RIG_TT599modes_[] = {
		"USB", "LSB", "CW", "AM", "FM", NULL};
const char RIG_TT599mchar_[] = {
		'0', '1', '3', '4', '5', 0 };
static const int  RIG_TT599_def_bw[] = { 19, 19, 7, 24, 19 };
static const char RIG_TT599_mode_type[] = {'U', 'L', 'L', 'U', 'U'};

const char *RIG_TT599widths[] = { 
"100",  "200",  "300",  "400",  "500", "600",  "700",  "800",  "900",  "1000",
"1200", "1400", "1600", "1800", "2000", "2200", "2400", "2600", "2800", "3000", NULL};
static int TT599_bw_vals[] = {
 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
11,12,13,14,15,16,17,18,19,20,
WVALS_LIMIT};

const int RIG_TT599w[] = {
100, 200, 300, 400, 500,
600, 700, 800, 900, 1000,
1200, 1400, 1600, 1800, 2000, 
2200, 2400, 2600, 2800, 3000, 0 };

static GUI rig_widgets[]= {
	{ (Fl_Widget *)sldrPOWER,   266, 105, 156 },
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

RIG_TT599::RIG_TT599() {
	name_ = RIG_TT599name_;
	modes_ = RIG_TT599modes_;
	bandwidths_ = RIG_TT599widths;
	bw_vals_ = TT599_bw_vals;

	widgets = rig_widgets;

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
	can_change_alt_vfo = true;

	max_power = 100;

	has_smeter =
	has_power_out =
	has_split =
	has_split_AB =
	has_swr_control =
	has_mode_control =
	has_bandwidth_control =
	has_ptt_control =
	has_preamp_control =
	has_auto_notch =
	has_attenuator_control =
	has_noise_reduction_control =
	has_power_control = true;
//	has_agc_level = true;

	precision = 1;
	ndigits = 8;

};

static void wait(int msec)
{
	for (int i = 0; i < msec; i += 10) {
		MilliSleep(10);
		Fl::awake();
	}
}

void RIG_TT599::initialize()
{
	rig_widgets[0].W = sldrPOWER;

	cmd = "X\r";
	sendCommand(cmd, 0);
	wait(200);
	readResponse();
	showresp(WARN, ASC, "init", cmd, replystr);
	split = false;
	cmd = "*KVAAA\r";
	sendCommand(cmd, 0);
	wait(200);
	readResponse();
	showresp(WARN, ASC, "normal", cmd, replystr);
}

void RIG_TT599::shutdown()
{
	cmd = "*RMF0\r";
	sendCommand(cmd, 0);
	wait(200);
	readResponse();
	showresp(WARN, ASC, "Enable BW control", cmd, replystr);
}

bool RIG_TT599::check ()
{
	cmd = "?AF\r";
	int ret = waitCommand( cmd, 12, "check");
	if (ret < 12) return false;
	return true;
}

long RIG_TT599::get_vfoA ()
{
	size_t p;
	cmd = "?AF\r";
	if ( waitCommand( cmd, 12, "get vfoA") ) {
		if ((p = replystr.rfind("@AF")) != string::npos)
			freqA =  atol(&replystr[p+3]);
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
	get_vfoA();
}

long RIG_TT599::get_vfoB ()
{
	size_t p;
	cmd = "?BF\r";
	if ( waitCommand( cmd, 12, "get vfoB") ) {
		if ((p = replystr.rfind("@BF")) != string::npos)
			freqB =  atol(&replystr[p+3]);
	}
	return freqB;
}

void RIG_TT599::set_vfoB (long freq)
{
	freqB = freq;
	cmd = "*BF";
	cmd.append( to_decimal( freq, 8 ) );
	cmd += '\r';
	sendCommand(cmd);
	get_vfoB();
}

void RIG_TT599::set_PTT_control(int val)
{
	cmd = val ? "*TK\r" : "*TU\r";
	sendCommand(cmd);
	wait(200);
}

int RIG_TT599::get_modeA()
{
	size_t p;
	cmd = "?RMM\r";
	if ( waitCommand( cmd, 6, "get modeA") ) {
		if ((p = replystr.rfind("@RMM")) != string::npos) {
			modeA = 0;
			while (RIG_TT599mchar_[modeA] != replystr[p+4]) {
				modeA++;
				if (RIG_TT599mchar_[modeA] == 0) {
					modeA = 0;
					break;
				}
			}
		}
	}
	return modeA;
}

void RIG_TT599::set_modeA(int md)
{
	modeA = md;
	cmd = "*RMM";
	cmd += RIG_TT599mchar_[md];
	cmd += '\r';
	sendCommand(cmd);
	get_modeA();
}

int RIG_TT599::get_modeB()
{
	size_t p;
	cmd = "?RMM\r";
	if ( waitCommand( cmd, 6, "get modeB") ) {
		if ((p = replystr.rfind("@RMM")) != string::npos) {
			modeB = 0;
			while (RIG_TT599mchar_[modeB] != replystr[p+4]) {
				modeB++;
				if (RIG_TT599mchar_[modeB] == 0) {
					modeB = 0;
					break;
				}
			}
		}
	}
	return modeB;
}

void RIG_TT599::set_modeB(int md)
{
	modeB = md;
	cmd = "*RMM";
	cmd += RIG_TT599mchar_[md];
	cmd += '\r';
	sendCommand(cmd);
	get_modeB();
}

int RIG_TT599::get_bwA()
{
	int w = 0;
	size_t p;
	cmd = "?RMF\r";
	if ( waitCommand( cmd, 12, "get bwA") ) {
		if ((p = replystr.rfind("@RMF")) != string::npos) {
			if ( p != string::npos)
				w = atol(&replystr[p+4]);
			bwA = 0;
			while ( (RIG_TT599w[bwA] < w)  &&
					(RIG_TT599w[bwA + 1] != 0) ) bwA++;
		}
	}
	return bwA;
}

void RIG_TT599::set_bwA(int bw)
{
	cmd = "*RMF";
	cmd.append(RIG_TT599widths[bw]);
	cmd += '\r';
	sendCommand(cmd);
	get_bwA();
}

int RIG_TT599::get_bwB()
{
	int w = 0;
	size_t p;
	cmd = "?RMF\r";
	if ( waitCommand( cmd, 12, "get bwB") ) {
		if ((p = replystr.rfind("@RMF")) != string::npos) {
			if ( p != string::npos)
				w = atol(&replystr[p+4]);
			bwB = 0;
			while ( (RIG_TT599w[bwB] < w)  &&
					(RIG_TT599w[bwB + 1] != 0) ) bwB++;
		}
	}
	return bwB;
}

void RIG_TT599::set_bwB(int bw)
{
	cmd = "*RMF";
	cmd.append(RIG_TT599widths[bw]);
	cmd += '\r';
	sendCommand(cmd);
	get_bwB();
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

int RIG_TT599::get_preamp()
{
	size_t p;
	cmd = "?RME\r";
	if ( waitCommand( cmd, 6, "get preamp") ) {
		if ((p = replystr.rfind("@RME")) != string::npos)
			return replystr[p+4] - '0';
	}
	return 0;
}

void RIG_TT599::set_preamp(int val)
{
	cmd = val ? "*RME1\r" : "*RME0\r";
	sendCommand(cmd);
	get_preamp();
}

//void RIG_TT599::set_agc_level()
//{
// *RMAF - fast *RMAM - medium *RMAS - slow
//}

int  RIG_TT599::get_power_control(void)
{
	size_t p;
	cmd = "?TP\r";
	if ( waitCommand( cmd, 6, "get power control") ) {
		if ((p = replystr.rfind("@TP")) != string::npos) {
			int pwr = atol(&replystr[p+3]);
			return pwr;
		}
	}
	return progStatus.power_level;
}

void RIG_TT599::set_power_control(double val) 
{
	char szPwr[4];
	snprintf(szPwr, sizeof(szPwr), "%d", (int)val);
	cmd.assign("*TP");
	cmd.append(szPwr).append("\r");
	sendCommand(cmd);
	get_power_control();
}

int  RIG_TT599::get_auto_notch()
{
	cmd = "?RMNA\r";
	sendCommand(cmd);
	if ( waitCommand( cmd, 7, "get auto notch") ) {
		if (replystr.rfind("@RMNA1") == string::npos) return 0;
	}
	return 1;
}

void RIG_TT599::set_auto_notch(int v)
{
	cmd = v ? "*RMNA1\r" : "*RMNA0\r";
	sendCommand(cmd);
	get_auto_notch();
}

int  RIG_TT599::get_attenuator()
{
	size_t p;
	cmd = "?RMT\r";
	if ( waitCommand( cmd, 6, "get attenuator") ) {
		if ((p = replystr.rfind("@RMT1")) != string::npos) return 1;
	}
	return 0;
}

void RIG_TT599::set_attenuator(int val)
{
	cmd = val ? "*RMT1\r" : "*RMT0\r";
	sendCommand(cmd);
	get_attenuator();
}

int smeter_count = 10;
int  RIG_TT599::get_smeter()
{
	size_t p;
	int dbm = 0;
	cmd = "?S\r";
	if ( waitCommand( cmd, 20, "get smeter") ) {
		if ((p = replystr.rfind("@SRM")) != string::npos) dbm = atoi(&replystr[p+4]);
	}
	return 5 * dbm / 6;
}

int  RIG_TT599::get_swr()
{
	float swr = (sqrtf(fwdpwr) + sqrtf(refpwr/10.0))/(sqrt(fwdpwr) - sqrt(refpwr/10.0) + .0001);
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
	if ( waitCommand( cmd, 20, "get smeter") ) {
		if ((p = replystr.rfind ("@STF")) != string::npos) {
			fwdpwr = atol(&replystr[p+4]);
			p = replystr.find("R", p+4);
			if (p != string::npos)
				refpwr = atol(&replystr[p+1]);
		}
	}
	return fwdpwr;
}

int RIG_TT599::get_split()
{
	cmd = "?KV\r";
	if ( waitCommand( cmd, 7, "get split") ) {
		size_t p = replystr.find("@KVAA");
		if ((p != string::npos) && (replystr[p+5] == 'B')) split = 1;
		else split = 0;
	}
	return split;
}

void  RIG_TT599:: set_split(bool val)
{
	split = val;
	cmd = val ? "*KVAAB\r" : "*KVAAA\r";
	sendCommand(cmd);
	get_split();
}

int  RIG_TT599::get_noise_reduction_val()
{
	int val = 1;
	cmd.assign("?RMNN\r");

	if ( waitCommand( cmd, 7, "get noise_reduction_value") ) {
		size_t p = replystr.rfind("@RMNN");
		if (p == string::npos) return val;
		val = atol(&replystr[p+5]);
	}
	return val;
}

void RIG_TT599::set_noise_reduction_val(int val)
{
	cmd.assign("*RMNN");
	cmd += ('0' + val);
	cmd.append("\r");
	sendCommand(cmd);
	get_noise_reduction_val();
}
