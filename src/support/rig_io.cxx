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

#include <math.h>
#include <string>

#include <fstream>

#include <FL/Fl.H>

#include "rig.h"
#include "support.h"
#include "util.h"
#include "debug.h"
#include "status.h"
#include "rigbase.h"
#include "rig_io.h"
#include "icons.h"
#include "tod_clock.h"

#include "socket_io.h"

using namespace std;

extern bool test;

const char *nuline = "\n";

static int iBaudRates[] = { 300, 600, 1200, 2400, 4800, 9600,
	19200, 38400, 57600, 115200, 230400, 460800 };
const char *szBaudRates[] = { "300", "600", "1200", "2400", "4800", "9600",
	"19200", "38400", "57600", "115200", "230400", "460800", NULL };

int BaudRate(int n)
{
	if (n > (int)sizeof(iBaudRates)) return 1200;
	return (iBaudRates[n]);
}


bool startXcvrSerial()
{
	debug::level_e level = debug::level;
	debug::level = debug::DEBUG_LEVEL;

	bypass_serial_thread_loop = true;
// setup commands for serial port
	if (progStatus.xcvr_serial_port == "NONE") {
		bypass_serial_thread_loop = false;
		return false;
	}

	RigSerial->Device(progStatus.xcvr_serial_port);
	RigSerial->Baud(BaudRate(progStatus.comm_baudrate));
	RigSerial->Stopbits(progStatus.stopbits);
	RigSerial->Retries(progStatus.comm_retries);
	RigSerial->Timeout(progStatus.comm_timeout);
	RigSerial->RTSptt(progStatus.comm_rtsptt);
	RigSerial->DTRptt(progStatus.comm_dtrptt);
	RigSerial->RTSCTS(progStatus.comm_rtscts);
	RigSerial->RTS(progStatus.comm_rtsplus);
	RigSerial->DTR(progStatus.comm_dtrplus);

	if (!RigSerial->OpenPort()) {
		LOG_ERROR("Cannot access %s", progStatus.xcvr_serial_port.c_str());
		return false;
	} else {
		LOG_DEBUG("\n\
Serial port:\n\
    Port     : %s\n\
    Baud     : %d\n\
    Stopbits : %d\n\
    Retries  : %d\n\
    Timeout  : %d\n\
    Loop     : %d\n\
    RTSCTS   : %d\n\
    CATptt   : %d\n\
    RTSptt   : %d\n\
    DTRptt   : %d\n\
    RTS+     : %d\n\
    DTR+     : %d\n",
			progStatus.xcvr_serial_port.c_str(),
			progStatus.comm_baudrate,
			progStatus.stopbits,
			progStatus.comm_retries,
			progStatus.comm_timeout,
			progStatus.serloop_timing,
			progStatus.comm_rtscts,
			progStatus.comm_catptt,
			progStatus.comm_rtsptt,
			progStatus.comm_dtrptt,
			progStatus.comm_rtsplus,
			progStatus.comm_dtrplus );
	}

	RigSerial->FlushBuffer();

	debug::level = level;

	return true;
}

bool startAuxSerial()
{
	if (progStatus.aux_serial_port == "NONE") return false;

	AuxSerial->Device(progStatus.aux_serial_port);
	AuxSerial->Baud(BaudRate(progStatus.comm_baudrate));
	AuxSerial->Stopbits(progStatus.stopbits);
	AuxSerial->Retries(progStatus.comm_retries);
	AuxSerial->Timeout(progStatus.comm_timeout);

	if (!AuxSerial->OpenPort()) {
		LOG_ERROR("Cannot access %s", progStatus.aux_serial_port.c_str());
		return false;
	}
	return true;
}

bool startSepSerial()
{
	if (progStatus.sep_serial_port == "NONE") return false;

	SepSerial->Device(progStatus.sep_serial_port);
	SepSerial->Baud(BaudRate(progStatus.comm_baudrate));

	SepSerial->RTSCTS(false);
	SepSerial->RTS(progStatus.sep_rtsplus);
	SepSerial->RTSptt(progStatus.sep_rtsptt);

	SepSerial->DTR(progStatus.sep_dtrplus);
	SepSerial->DTRptt(progStatus.sep_dtrptt);

	if (!SepSerial->OpenPort()) {
		LOG_ERROR("Cannot access %s", progStatus.sep_serial_port.c_str());
		return false;
	}
	return true;
}

// TODO: Review for thread safety.  
//  Tried adding mutex, but deadlocks startup
// progress dialog:
// guard_lock reply_lock(&mutex_replystr); 
//
void assignReplyStr(string val)
{
	selrig->replystr = val;
}

char replybuff[RXBUFFSIZE+1];
string respstr;

int readResponse()
{
	int numread = 0;
	respstr.clear();
	memset(replybuff, 0, RXBUFFSIZE + 1);
	if (progStatus.use_tcpip)
		numread = read_from_remote(respstr);
	else {
		numread = RigSerial->ReadBuffer(replybuff, RXBUFFSIZE);
		for (int i = 0; i < numread; respstr += replybuff[i++]);
	}
	if (numread)
		LOG_DEBUG("rsp:%3d, %s", numread, str2hex(respstr.c_str(), respstr.length()));
	return numread;
}

int sendCommand (string s, int nread, int wait)
{
	int numwrite = (int)s.size();

	// TODO: Review for thread safety
	//
	// Clear command before sending, to keep the logs sensical.  Otherwise it looks like 
	// reply was from this command, when it really was from a previous command.
	assignReplyStr("");

	if (progStatus.use_tcpip) {
		readResponse();
		send_to_remote(s, progStatus.byte_interval);
		int timeout = 
			progStatus.comm_wait + progStatus.tcpip_ping_delay +
			(int)((nread + progStatus.comm_echo ? numwrite : 0)*11000.0/RigSerial->Baud() );
		while (timeout > 0) {
			if (timeout > 10) MilliSleep(10);
			else MilliSleep(timeout);
			timeout -= 10;
			Fl::awake();
		}
		if (nread == 0) return 0;
		return readResponse();
	}

	if (RigSerial->IsOpen() == false) {
		return 0;
	}

	LOG_DEBUG("cmd:%3d, %s", (int)s.length(), str2hex(s.data(), s.length()));

	RigSerial->FlushBuffer();
	RigSerial->WriteBuffer(s.c_str(), numwrite);

	int timeout = progStatus.comm_wait + 
		(int)((nread + progStatus.comm_echo ? numwrite : 0)*11000.0/RigSerial->Baud());
	timeout += wait;
	while (timeout > 0) {
		if (timeout > 10) MilliSleep(10);
		else MilliSleep(timeout);
		timeout -= 10;
		Fl::awake();
	}
	if (nread == 0) return 0;
	return readResponse();
}

static int waitcount = 0;
static bool timeout_alert = false;
static char sztimeout_alert[200];

static void show_timeout(void *)
{
	fl_alert2("%s",sztimeout_alert);
}

bool waitCommand(
				string command,
				int nread,
				string info,
				int msec,
				char term,
				int how,
				int level )
{
	guard_lock reply_lock(&mutex_replystr);

	int numwrite = (int)command.length();
	if (nread == 0)
		LOG_DEBUG("cmd:%3d, %s", numwrite, how == ASC ? command.c_str() : str2hex(command.data(), numwrite));

	if (progStatus.use_tcpip) {
		send_to_remote(command, progStatus.byte_interval);
		if (nread == 0) return 0;
	} else {
		if (RigSerial->IsOpen() == false) {
			LOG_DEBUG("cmd: %s", how == ASC ? command.c_str() : str2hex(command.data(), command.length()));
			waitcount = 0;
			return 0;
		}

		RigSerial->FlushBuffer();
//		replystr.clear();

		RigSerial->WriteBuffer(command.c_str(), numwrite);
		if (nread == 0) {
			waitcount = 0;
			return 0;
		}
	}

	int tod_start = zmsec();

// minimimum time to wait for a response
	int timeout = (int)((nread + progStatus.comm_echo ? numwrite : 0)*11000.0/RigSerial->Baud()
		+ progStatus.use_tcpip ? progStatus.tcpip_ping_delay : 0);
	while (timeout > 0) {
		if (timeout > 10) MilliSleep(10);
		else MilliSleep(timeout);
		timeout -= 10;
		Fl::awake();
	}
// additional wait for xcvr processing
	string returned = "";
	static char sztemp[100];
	int waited = 0;
	while (waited < msec) {
		if (readResponse())
			returned.append(respstr);
		if (	((int)returned.length() >= nread) || 
				(returned.find(term) != string::npos) ) {
			assignReplyStr(returned);
			waited = zmsec() - tod_start;
			snprintf(sztemp, sizeof(sztemp), "%s rcvd in %d msec", info.c_str(), waited);
			showresp(level, how, sztemp, command, returned);
			waitcount = 0;
			return true;
		}
		waited += 10;
		MilliSleep(10);
		Fl::awake();
	}
	waitcount++;
	assignReplyStr(returned);
	waited = zmsec() - tod_start;
	snprintf(sztemp, sizeof(sztemp), "%s TIMED OUT in %d ms", command.c_str(), waited);
	showresp(ERR, HEX, sztemp, command, returned);
	if (waitcount > 4 && !timeout_alert) {
		timeout_alert = true;
		snprintf(sztimeout_alert, sizeof(sztimeout_alert), 
			"Serial i/o failure\n%s TIME OUT in %d ms",
			command.c_str(), waited);
			Fl::awake(show_timeout);
	}
	return false;
}

int waitResponse(int timeout)
{
	int n = 0;
	if (!progStatus.use_tcpip && RigSerial->IsOpen() == false)
		return 0;

	MilliSleep(10);
	if (!(n = readResponse()))
		while (timeout > 0) {
			if (timeout > 10) MilliSleep(10);
			else MilliSleep(timeout);
			timeout -= 10;
			Fl::awake();
		}
	return n;
}

void showresp(int level, int how, string s, string tx, string rx) 
{
	time_t now;
	time(&now);
	struct tm *local = localtime(&now);
	char sztm[20];
	strftime(sztm, sizeof(sztm), "%H:%M:%S", local);

	string s1 = how == HEX ? str2hex(tx.c_str(), tx.length()) : tx;
	string s2 = how == HEX ? str2hex(rx.c_str(), rx.length()) : rx;
	if (how == ASC) {
		size_t p;
		while((p = s1.find('\r')) != string::npos)
			s1.replace(p, 1, "<cr>");
		while((p = s1.find('\n')) != string::npos)
			s1.replace(p, 1, "<lf>");
		while((p = s2.find('\r')) != string::npos)
			s2.replace(p, 1, "<cr>");
		while((p = s2.find('\n')) != string::npos)
			s2.replace(p, 1, "<lf>");
	}

	switch (level) {
	case QUIET:
		SLOG_QUIET("%s: %10s\ncmd %s\nans %s", sztm, s.c_str(), s1.c_str(), s2.c_str());
		break;
	case ERR:
		SLOG_ERROR("%s: %10s\ncmd %s\nans %s", sztm, s.c_str(), s1.c_str(), s2.c_str());
		break;
	case INFO:
		SLOG_INFO("%s: %10s\ncmd %s\nans %s", sztm, s.c_str(), s1.c_str(), s2.c_str());
		break;
	case DEBUG:
		SLOG_DEBUG("%s: %10s\ncmd %s\nans %s", sztm, s.c_str(), s1.c_str(), s2.c_str());
		break;
	case WARN:
	default:
		SLOG_WARN("%s: %10s\ncmd %s\nans %s", sztm, s.c_str(), s1.c_str(), s2.c_str());
		break;
	}
}
