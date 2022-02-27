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
// aunsigned long int with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include <iostream>

#include "icom/ICbase.h"
#include "debug.h"
#include "icons.h"
#include "tod_clock.h"
#include "trace.h"
#include "status.h"
#include "threads.h"
#include "socket_io.h"
#include "serial.h"
#include "support.h"

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

	if (!RigSerial->IsOpen()) return;

	if (replystr.rfind(ok) != std::string::npos)
		return;

	std::string s1 = str2hex(cmd.c_str(), cmd.length());
	std::string s2 = str2hex(replystr.c_str(), replystr.length());
	LOG_ERROR("\nsent  %s\nreply %s", s1.c_str(), s2.c_str());
}

bool RIG_ICOM::sendICcommand(std::string cmd, int nbr)
{
	guard_lock reply_lock(&mutex_replystr);

	int ret = sendCommand(cmd);

	if (!progStatus.use_tcpip && !RigSerial->IsOpen())
		return false;

	if (!RigSerial->IsOpen()) return false;

	if (ret < nbr) {
		LOG_ERROR("Expected %d received %d", nbr, ret);
		return false;
	}

	if (ret > nbr) respstr.erase(0, ret - nbr);

// look for preamble at beginning
	if (respstr.rfind(pre_fm) == std::string::npos)  {
		LOG_ERROR("preamble: %s not in %s", pre_fm.c_str(), cmd.c_str());
		replystr.clear();
		return false;
	}

// look for postamble
	if (respstr.rfind(post) == std::string::npos) {
		LOG_ERROR("postample: %s not at end of %s", post.c_str(), cmd.c_str());
		replystr.clear();
		return false;
	}
	replystr = respstr;
	return true;
}

void RIG_ICOM::delayCommand(std::string cmd, int wait)
{
	int oldwait = progStatus.comm_wait;
	progStatus.comm_wait += wait;
	sendCommand(cmd);
	progStatus.comm_wait = oldwait;
}

#include <fstream>

void RIG_ICOM::ICtrace(std::string cmd, std::string hexstr) 
{
	std::string s1 = str2hex(hexstr.c_str(), hexstr.length());
	rig_trace(2, cmd.c_str(), s1.c_str());
}

bool RIG_ICOM::waitFOR(size_t n, const char *sz, unsigned long timeout)
{
	static char sztemp[200];
	memset(sztemp, 0, 200);
	std::string check = "1234";
	std::string eor   = "\xFD";
	std::string bad   = "\xFA\xFD";

	check[0] = cmd[0];
	check[1] = cmd[1];
	check[2] = cmd[3];
	check[3] = cmd[2];

	int delay =  n * 11000.0 / RigSerial->Baud();
	int retnbr = 0;

	replystr.clear();

	if (progStatus.use_tcpip) {
		send_to_remote(cmd, progStatus.byte_interval);
		MilliSleep(delay + progStatus.tcpip_ping_delay);
		retnbr = read_from_remote(replystr);
		LOG_DEBUG ("%s: read %d bytes, %s", sz, retnbr, replystr.c_str());
	} else {
		if(!RigSerial->IsOpen()) {
			LOG_DEBUG("TEST %s", sz);
			return false;
		}
		RigSerial->FlushBuffer();
		RigSerial->WriteBuffer(cmd.c_str(), cmd.length());

		size_t tout1 = zmsec();
		size_t tout2 = tout1;
		size_t pcheck = std::string::npos;
		size_t peor = std::string::npos;

		std::string tempstr;
		int nret;

		while ((tout2 - tout1) < timeout) {
			tempstr.clear();
			nret = RigSerial->ReadBuffer(tempstr, n + cmd.length(), check, eor);
			replystr.append(tempstr);
			retnbr += nret;

			if (replystr.rfind(bad) != std::string::npos)
				return false;
			pcheck = replystr.rfind(check);
			peor = replystr.rfind(eor);

			if ((pcheck != std::string::npos) && 
				(peor != std::string::npos) &&
				(peor > pcheck) )
				return true;

			tout2 = zmsec();
			if (tout2 < tout1) tout1 = tout2;
		}

	}

	return false;
}

bool RIG_ICOM::waitFB(const char *sz, int timeout)
{
#if SERIAL_DEBUG
fprintf(serlog, "waitFB\n");
#endif

	if (!waitFOR(6, sz, timeout))
		return false;
	if (replystr.rfind("\xFB\xFD") == std::string::npos)
		return false;
	return true;
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
	if (inuse == onB) filA = filB;
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
	std::string resp;
	std::string cstr = "\x1C\x01";
	cmd.assign(pre_to).append(cstr).append(post);
	resp.assign(pre_fm).append(cstr);
	int val = tune_;
	if (waitFOR(8, "get TUNE")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			val = replystr[p + 6];
	}
	return (tune_ = val);
}

std::string RIG_ICOM::hexstr(std::string s)
{
	return str2hex(s.c_str(), s.length());
}

