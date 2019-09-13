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

#ifndef _RIG_IO_H
#define _RIG_IO_H

#include <cstring>
#include <cmath>

#include <FL/Fl.H>

enum { QUIET, ERR, WARN, INFO, DEBUG };
enum { ASC, HEX };

using namespace std;

enum MODES {LSB, USB, CW};

enum BAUDS {
	BR300, BR600, BR1200, BR2400, BR4800, BR9600, BR19200,
	BR38400, BR57600, BR115200, BR230400, BR460800 };

#define RXBUFFSIZE 16384

extern const char *szBaudRates[];

extern bool startXcvrSerial();
extern bool startAuxSerial();
extern bool startSepSerial();

extern int readResponse();
extern int sendCommand(string s, int nread = 0, int wait = 0);
extern int waitResponse(int);
extern bool waitCommand(
				string command,
				int nread,
				string info = "",
				int msec = 200,
				char term = 0x0d, // carriage return
				int how = ASC,
				int level = INFO );

extern char replybuff[];
//extern string replystr;
extern string respstr;

extern void showresp(int level, int how, string s, string tx, string rx);

#endif
