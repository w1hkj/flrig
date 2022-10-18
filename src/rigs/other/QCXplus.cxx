// ----------------------------------------------------------------------------
// Copyright (C) 2022
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

#include "other/QCXplus.h"
#include "support.h"

static const char QCXPname_[] = "QCX+";

static const char *QCXPmodes_[] = {
		"CW", "CW-R", NULL};
static const char QCXP_mode_chr[] =  { '1', '2' };
static const char QCXP_mode_type[] = { 'U', 'L' };

static const char *QCXP_CWwidths[] = {
"200", NULL};
static int QCXP_CW_bw_vals[] = {1, WVALS_LIMIT};

static GUI rig_widgets[]= {
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

RIG_QCXP::RIG_QCXP() {
// base class values
	name_ = QCXPname_;
	modes_ = QCXPmodes_;
	_mode_type = QCXP_mode_type;
	bandwidths_ = QCXP_CWwidths;
	bw_vals_ = QCXP_CW_bw_vals;

	widgets = rig_widgets;

	serial_baudrate = BR38400;
	stopbits = 1;
	serial_retries = 1;

//	serial_write_delay = 0;
//	serial_post_write_delay = 0;

	serial_timeout = 50;
	serial_rtscts  = false;
	serial_rtsplus = false;
	serial_dtrplus = false;
	serial_catptt  = false;
	serial_rtsptt  = false;
	serial_dtrptt  = false;
	B.imode = A.imode = 1;
	B.iBW = A.iBW = 0x8A03;
	A.freq = 7040000ULL;
	B.freq = 7025000ULL;

	has_extras = false;

	has_noise_reduction =
	has_noise_reduction_control =
	has_auto_notch =
	has_noise_control =
	has_sql_control = false;

	has_split = true;
	has_smeter = true;

	has_mode_control = true;
	has_bandwidth_control = true;

	has_ptt_control = true;

	can_synch_clock = true;

	precision = 1;
	ndigits = 8;

}

std::string RIG_QCXP::read_menu(int m1, int m2)
{
#define WAIT 10
	char str1[20];
	int n = 4;
	std::string resp1;

	snprintf(str1, sizeof(str1), "QM%d?%02d;", m1, m2);
	while (n) {
		cmd = str1;
		replystr.clear();
		MilliSleep(WAIT);
		wait_char(';', 100, 100, "init 1", ASC);
		resp1 = replystr;
		if (resp1.find("?;") == std::string::npos &&
			resp1.find("QM") != std::string::npos) {
			return resp1;
		}
		--n;
	}
	return "";
}

void RIG_QCXP::initialize()
{
	sendCommand("QU0;");
	sendCommand("RX;");
	return;
}

static int ret = 0;

// SM cmd 0 ... 100 (rig values 0 ... 15)
int RIG_QCXP::get_smeter()
{
	int mtr = 0;
	cmd = "SM0;";
	get_trace(1, "get_smeter");
	ret = wait_char(';', 8, 100, "get Smeter", ASC);
	gett("");
	if (ret < 8) return 0;

	size_t p = replystr.rfind("SM");
	if (p != std::string::npos)
		mtr = 100 * atoi(&replystr[p + 3]) / 16383;
	return mtr;
}

void RIG_QCXP::set_modeA(int val)
{
	if (val == 0) {
		A.imode = 0;
		cmd = "QM3.070;";
	} else {
		A.imode = 1;
		cmd = "QM3.071;";
	}
	sendCommand(cmd);
	sett("");
}

// CW-R.OFF.OFF#ON ;
int RIG_QCXP::get_modeA()
{
	cmd = "QM3?07;";
	get_trace(1, "get_modeA");
	ret = wait_char(';', 17, 100, "get CW-R", ASC);
	gett("");
	if (replystr.find("CW-R") == std::string::npos) return A.imode;
	if (replystr.find("CW-R.OFF") != std::string::npos) A.imode = 0;
	if (replystr.find("CW-R.ON") != std::string::npos) A.imode = 1;
	return A.imode;
}

void RIG_QCXP::set_modeB(int val)
{
	if (val == 0) {
		B.imode = 0;
		cmd = "QM3.070;";
	} else {
		B.imode = 1;
		cmd = "QM3.071;";
	}
	sendCommand(cmd);
	sett("");
}

int RIG_QCXP::get_modeB()
{
	cmd = "QM3?07;";
	get_trace(1, "get_modeA");
	ret = wait_char(';', 17, 100, "get CW-R", ASC);
	gett("");
	if (replystr.find("CW-R") == std::string::npos) return B.imode;
	if (replystr.find("CW-R.OFF") != std::string::npos) B.imode = 0;
	if (replystr.find("CW-R.ON") != std::string::npos) B.imode = 1;
	return B.imode;
}

int RIG_QCXP::get_modetype(int n)
{
	if (A.imode == 0) return 'U';
	else return 'L';
}

void RIG_QCXP::set_bwA(int val)
{
	A.iBW = 0;
}

int RIG_QCXP::get_bwA()
{
	return A.iBW = 0;
}

void RIG_QCXP::set_bwB(int val)
{
	B.iBW = 0;
}

int RIG_QCXP::get_bwB()
{
	return B.iBW = 0;
}

// Tranceiver PTT on/off
void RIG_QCXP::set_PTT_control(int val)
{
	ptt_ = val;
	return;
//	if (val)
//		cmd = "TQ1";
//	else
//		cmd = "TQ0;";
//	sendCommand(cmd);
//	showresp(WARN, ASC, "set PTT", cmd, "");
}

int RIG_QCXP::get_PTT()
{
//	cmd = "TQ;";
	return ptt_;
}


void RIG_QCXP::set_split(bool val) 
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

bool RIG_QCXP::can_split()
{
	return true;
}

int RIG_QCXP::get_split()
{
	size_t p;
	int split = 0;
	char rx = 0, tx = 0;
// tx vfo
	cmd = rsp = "FT";
	cmd.append(";");
	if (wait_char(';', 4, 100, "get split tx vfo", ASC) == 4) {
		p = replystr.rfind(rsp);
		if (p == std::string::npos) return split;
		tx = replystr[p+2];
	}
// rx vfo
	cmd = rsp = "FR";
	cmd.append(";");
	if (wait_char(';', 4, 100, "get split rx vfo", ASC) == 4) {
		p = replystr.rfind(rsp);
		if (p == std::string::npos) return split;
		rx = replystr[p+2];
	}

	if (tx == '0' && rx == '0') split = 0;
	else if (tx == '1' && rx == '0') split = 1;
	else if (tx == '0' && rx == '1') split = 2;
	else if (tx == '1' && rx == '1') split = 3;

	return split;
}

unsigned long long RIG_QCXP::get_vfoA ()
{
	cmd = "FA;";
	get_trace(1, "get_vfoA()");
	if (wait_char(';', 14, 100, "get vfo A", ASC) < 14) return A.freq;
	gett("vfoA:");

	size_t p = replystr.rfind("FA");
	if (p != std::string::npos && (p + 12 < replystr.length())) {
		unsigned long long f = 0;
		for (size_t n = 2; n < 13; n++)
			f = f*10 + replystr[p+n] - '0';
		A.freq = f;
	}
	return A.freq;
}

void RIG_QCXP::set_vfoA (unsigned long long freq)
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

unsigned long long RIG_QCXP::get_vfoB ()
{
	cmd = "FB;";
	get_trace(1, "get_vfoB()");
	if (wait_char(';', 14, 100, "get vfo B", ASC) < 14) return B.freq;
	gett("vfoB:");

	size_t p = replystr.rfind("FB");
	if (p != std::string::npos && (p + 12 < replystr.length())) {
		unsigned long long f = 0;
		for (size_t n = 2; n < 13; n++)
			f = f*10 + replystr[p+n] - '0';
		B.freq = f;
	}
	return B.freq;
}

void RIG_QCXP::set_vfoB (unsigned long long freq)
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

void RIG_QCXP::selectA()
{
	cmd = "FR0;";
	sendCommand(cmd);
	cmd = "FT0;";
	sendCommand(cmd);
	inuse = onA;
}

void RIG_QCXP::selectB()
{
	cmd = "FR1;";
	sendCommand(cmd);
	cmd = "FT1;";
	sendCommand(cmd);
	inuse = onB;
}

void RIG_QCXP::sync_date(char *dt)
{
	return;
}

// ---------------------------------------------------------------------
// tm formated as HH:MM:SS
// QCX+ needs HH:MM
// QM6.0806:35;
// ---------------------------------------------------------------------
void RIG_QCXP::sync_clock(char *tm)
{
	cmd.assign("QM6.08");

	tm[5] = 0;
	cmd.append(tm).append(";");
	sendCommand(cmd);
}

