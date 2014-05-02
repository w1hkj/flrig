// ---------------------------------------------------------------------
//
//      xml_server.cxx, a part of flrig
//
// Copyright (C) 2014
//               Dave Freese, W1HKJ
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the program; if not, write to the Free Software
// Foundation, Inc.
// 59 Temple Place, Suite 330
// Boston, MA  02111-1307 USA
//
// ---------------------------------------------------------------------

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Enumerations.H>

#include "support.h"
#include "debug.h"

#include "XmlRpc.h"

using namespace XmlRpc;

// The server
XmlRpcServer s;

/*
// Request record if it exists else return "NO_RECORD"
// Returns ADIF record
class log_get_record : public XmlRpcServerMethod
{
public:
  log_get_record(XmlRpcServer* s) : XmlRpcServerMethod("log.get_record", s) {}

  void execute(XmlRpcValue& params, XmlRpcValue& result)
  {
	std::string callsign = std::string(params[0]);
    std::string resultString = fetch_record(callsign.c_str());
	result = resultString;
  }

  std::string help() { return std::string("log.get_record CALL"); }

} log_get_record(&s);    // This constructor registers the method with the server

// Arguments: CALLSIGN MODE TIME_SPAN FREQ
class log_check_dup : public XmlRpcServerMethod
{
public:
  log_check_dup(XmlRpcServer* s) : XmlRpcServerMethod("log.check_dup", s) {}

  void execute(XmlRpcValue& params, XmlRpcValue& result)
  {
	if (params.size() != 6) {
		result = "Wrong # parameters";
		return;
	}
	std::string callsign = std::string(params[0]);
	std::string mode = std::string(params[1]);
	std::string spn = std::string(params[2]);
	std::string freq = std::string(params[3]);
	std::string state = std::string(params[4]);
	std::string xchg_in = std::string(params[5]);
	int ispn = atoi(spn.c_str());
	int ifreq = atoi(freq.c_str());
	bool bspn = (ispn > 0);
	bool bfreq = (ifreq > 0);
	bool bmode = (mode != "0");
	bool bstate = (state != "0");
	bool bxchg = (xchg_in != "0");
	bool res = qsodb.duplicate(
			callsign.c_str(),
			(const char *)szDate(6), (const char *)szTime(0), (unsigned int)ispn, bspn,
			freq.c_str(), bfreq,
			state.c_str(), bstate,
			mode.c_str(), bmode,
			xchg_in.c_str(), bxchg);
	result = (res ? "true" : "false");
	}

	std::string help() { 
		return std::string("log.check_dup CALL, MODE(0), TIME_SPAN(0), FREQ_HZ(0), STATE(0), XCHG_IN(0)"); 
	}

} log_check_dup(&s);

void updateBrowser(void *)
{
	loadBrowser(false);
}
*/

class rig_get_vfoA : public XmlRpcServerMethod
{
public:
  rig_get_vfoA(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_vfoA") {}

  void execute(XmlRpcValue& params, XmlRpcValue& result)
  {
//    std::string adif_record = std::string(params[0]);
//	xml_adif.add_record(adif_record.c_str(), qsodb);
//	Fl::awake(updateBrowser);
  }
	std::string help() { return std::string("rig.get_vfoA(Hz)"); }

} rig_get_vfoA(&s);

class rig_set_vfoA : public XmlRpcServerMethod
{
public:
  rig_set_vfoA(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_vfoA", s) {}

  void execute(XmlRpcValue& params, XmlRpcValue& result)
  {
//    std::string adif_record = std::string(params[0]);
//	xml_adif.add_record(adif_record.c_str(), qsodb);
//	Fl::awake(updateBrowser);
  }
	std::string help() { return std::string("rig.set_vfoA NNNNNNNN (Hz)"); }

} rig_set_vfoA(&s);

pthread_t *xml_thread = 0;

void * xml_thread_loop(void *d)
{
	for(;;) {
		s.work(-1.0);
	}
	return NULL;
}

void start_server(int port)
{
	XmlRpc::setVerbosity(0);

// Create the server socket on the specified port
	s.bindAndListen(port);

// Enable introspection
	s.enableIntrospection(true);

	xml_thread = new pthread_t;
	if (pthread_create(xml_thread, NULL, xml_thread_loop, NULL)) {
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}
}

void exit_server()
{
	s.exit();
}


