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

#include <iostream>

#include "ICbase.h"
#include "debug.h"
#include "support.h"
#include "icons.h"
#include "tod_clock.h"
#include "trace.h"
#include "status.h"
#include "threads.h"

pthread_mutex_t command_mutex = PTHREAD_MUTEX_INITIALIZER;

//=============================================================================

void RIG_ICOM::adjustCIV(uchar adr)
{
	CIV = adr;
	pre_to[2] = ok[3] = bad[3] = pre_fm[3] = CIV;
}

void RIG_ICOM::checkresponse()
{
	if (!progStatus.use_tcpip && !RigSerial->IsOpen())
		return;

	if (replystr.rfind(ok) != string::npos)
		return;

	string s1 = str2hex(cmd.c_str(), cmd.length());
	string s2 = str2hex(replystr.c_str(), replystr.length());
	LOG_ERROR("\nsent  %s\nreply %s", s1.c_str(), s2.c_str());
}

bool RIG_ICOM::sendICcommand(string cmd, int nbr)
{
	guard_lock reply_lock(&mutex_replystr);

	int ret = sendCommand(cmd);

	if (!progStatus.use_tcpip && !RigSerial->IsOpen())
		return false;

	if (ret < nbr) {
		LOG_ERROR("Expected %d received %d", nbr, ret);
		return false;
	}

	if (ret > nbr) respstr.erase(0, ret - nbr);

// look for preamble at beginning
	if (respstr.rfind(pre_fm) == string::npos)  {
		LOG_ERROR("preamble: %s not in %s", pre_fm.c_str(), cmd.c_str());
		replystr.clear();
		return false;
	}

// look for postamble
	if (respstr.rfind(post) == string::npos) {
		LOG_ERROR("postample: %s not at end of %s", post.c_str(), cmd.c_str());
		replystr.clear();
		return false;
	}
	replystr = respstr;
	return true;
}

void RIG_ICOM::delayCommand(string cmd, int wait)
{
	int oldwait = progStatus.comm_wait;
	progStatus.comm_wait += wait;
	sendCommand(cmd);
	progStatus.comm_wait = oldwait;
}

#include <fstream>

void RIG_ICOM::ICtrace(string cmd, string hexstr) 
{
	string s1 = str2hex(hexstr.c_str(), hexstr.length());
	rig_trace(2, cmd.c_str(), s1.c_str());
}

bool RIG_ICOM::waitFB(const char *sz)
{
	guard_lock cmd_lock(&command_mutex);

	guard_lock reply_lock(&mutex_replystr);

	char sztemp[100];
	string returned = "";
	string tosend = cmd;
	unsigned long tod_start = zmsec();
	unsigned long msec_start = 0;

	if (!progStatus.use_tcpip && !RigSerial->IsOpen()) {
		replystr = cmd;//returned;
		return false;
	}
	int cnt = 0, repeat = 0, num = cmd.length() + ok.length();

	int wait_msec = (int)(num*11000.0/RigSerial->Baud() +
			progStatus.use_tcpip ? progStatus.tcpip_ping_delay : 50) / 10;

	for (repeat = 0; repeat < progStatus.comm_retries; repeat++) {
		msec_start = zmsec();
		sendCommand(cmd, 0);
		returned = "";
		for ( cnt = 0; cnt < wait_msec; cnt++) {
			if (readResponse())
				returned.append(respstr);
			if (returned.find(ok) != string::npos) {
				replystr = returned;
				unsigned long int waited = zmsec() - msec_start;
				snprintf(sztemp, sizeof(sztemp), "%s ans in %ld ms, OK", sz, waited);
				if (repeat)
					showresp(WARN, HEX, sztemp, tosend, returned);
				else
					showresp(DEBUG, HEX, sztemp, tosend, returned);
				return true;
			}
			if (returned.find(bad) != string::npos) {
				replystr = returned;
				unsigned long int waited = zmsec() - msec_start;
				snprintf(sztemp, sizeof(sztemp), "%s ans in %ld ms, FAIL", sz, waited);
				showresp(ERR, HEX, sztemp, tosend, returned);
				return false;
			}
			MilliSleep(10);
			Fl::awake();
		}
	}
	replystr = returned;
	unsigned long int waited = zmsec() - tod_start;
	snprintf(sztemp, sizeof(sztemp), "%s TIMED OUT in %ld ms", sz, waited);
	showresp(ERR, HEX, sztemp, tosend, replystr);

	return false;
}

bool RIG_ICOM::waitFOR(size_t n, const char *sz)
{
	guard_lock cmd_lock(&command_mutex);

	guard_lock reply_lock(&mutex_replystr);

	char sztemp[100];
	string returned = "";
	string tosend = cmd;
	int cnt = 0, repeat = 0;
	size_t num = n;
	if (progStatus.comm_echo) num += cmd.length();

	unsigned long int tod_start = zmsec();
	unsigned long int msec_start = 0;

	int delay =  (int)(num * 11000.0 / RigSerial->Baud() + 
		progStatus.use_tcpip ? progStatus.tcpip_ping_delay : 50) / 10;

	if (!progStatus.use_tcpip && !RigSerial->IsOpen()) {
		replystr = cmd;//returned;
		return false;
	}
	for (repeat = 0; repeat < progStatus.comm_retries; repeat++) {
		msec_start = zmsec();
		sendCommand(tosend, 0);
		returned = "";
		for ( cnt = 0; cnt < delay; cnt++) {
			if (readResponse())
				returned.append(respstr);
			if (returned.length() >= num) {
				replystr = returned;
				unsigned long int waited = zmsec() - msec_start;
				snprintf(sztemp, sizeof(sztemp), "%s ans in %ld ms, OK  ", sz, waited);
				showresp(DEBUG, HEX, sztemp, tosend, returned);
				return true;
			}
			MilliSleep(10);
			Fl::awake();
		}
	}

	replystr = returned;
	unsigned long int waited = zmsec() - tod_start;
	snprintf(sztemp, sizeof(sztemp), "%s TIMED OUT in %ld ms", sz, waited);
	showresp(ERR, HEX, sztemp, tosend, returned);

	return false;
}

// exchange & equalize are available in these Icom xcvrs
// 718 706MKIIG 746 746PRO 756 756PRO 756PROII 756PROIII
// 910 7000 7100 7200 7300 7410 7600 7700 7800 9100

// 5/04/18 dhf

void RIG_ICOM::swapAB()
{
	cmd = pre_to;
	cmd += 0x07; cmd += 0xB0;
	ICtrace("A<>B", cmd);
	cmd.append(post);
	int fil = filB;
	filB = filA;
	filA = fil;
	waitFB("A<>B");
	ICtrace("A<>B", replystr);
}

void RIG_ICOM::A2B()
{
	cmd = pre_to;
	cmd += 0x07; cmd += 0xA0;
	ICtrace("A2B", cmd);
	cmd.append(post);
	if (useB) filA = filB;
	else      filB = filA;
	waitFB("Equalize vfos");
	ICtrace("A2B", replystr);
}

void RIG_ICOM::tune_rig(int how)
{
	cmd = pre_to;
	cmd.append("\x1c\x01");
	switch (how) {
		default:
		case 0:
			cmd += '\x00';
			break;
		case 1:
			cmd += '\x01';
			break;
		case 2:
			cmd += '\x02';
			break;
	}
	cmd.append( post );
	waitFB("tune rig");
	ICtrace("tune rig", replystr);
}

int RIG_ICOM::get_tune()
{
	string resp;
	string cstr = "\x1C\x01";
	cmd.assign(pre_to).append(cstr).append(post);
	resp.assign(pre_fm).append(cstr);
	int val = tune_;
	if (waitFOR(8, "get TUNE")) {
		size_t p = replystr.rfind(resp);
		if (p != string::npos)
			val = replystr[p + 6];
	}
	return (tune_ = val);
}

