// ----------------------------------------------------------------------------
// Copyright (C) 2014
//              David Freese, W1HKJ
//
// This file is part of flrig
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

#ifndef WRAP_H
#define WRAP_H

#include <string.h>
#include <string>

#include "threads.h"
#include "socket.h"

#ifdef WIN32
#  include <winsock2.h>
#else
#  include <arpa/inet.h>
#endif

using namespace std;

extern Socket *tcpip;
extern Address *remote_addr;

void connect_to_remote();
void disconnect_from_remote();
void send_to_remote(string cmd_string, int pace = 0);
int  read_from_remote(string &str);

#endif
