// ---------------------------------------------------------------------
//
//			xml_server.cxx, a part of flrig
//
// Copyright (C) 2014
//							 Dave Freese, W1HKJ
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the program; if not, write to the
//
//  Free Software Foundation, Inc.
//  51 Franklin Street, Fifth Floor
//  Boston, MA  02110-1301 USA.
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

#include "xml_server.h"

#include "XmlRpc.h"

using namespace XmlRpc;

// The server
XmlRpcServer rig_server;

//------------------------------------------------------------------------------
// Request for transceiver name
//------------------------------------------------------------------------------
class rig_get_xcvr : public XmlRpcServerMethod {
public:
	rig_get_xcvr(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_xcvr", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		result = selrig->name_;;
	}

	std::string help() { return std::string("returns noun name of transceiver"); }

} rig_get_xcvr(&rig_server);

//------------------------------------------------------------------------------
// Request for PTT state
//------------------------------------------------------------------------------
class rig_get_ptt : public XmlRpcServerMethod {
public:
	rig_get_ptt(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_ptt", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		result = btnPTT->value();
	}

	std::string help() { return std::string("returns state of PTT"); }

} rig_get_ptt(&rig_server);


//------------------------------------------------------------------------------
// Request for active vfo frequency
//------------------------------------------------------------------------------
class rig_get_vfo : public XmlRpcServerMethod {
public:
	rig_get_vfo(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_vfo", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		static char szfreq[20];
		int freq;
		if (useB) {
			guard_lock queB_lock(&mutex_queB);
			if (!queB.empty()) freq = queB.back().freq;
			else freq = vfoB.freq;
		} else {
			guard_lock queA_lock(&mutex_queA);
			if (!queA.empty()) freq = queA.back().freq;
			else freq = vfoA.freq;
		}
		snprintf(szfreq, sizeof(szfreq), "%d", freq);
		std::string result_string = szfreq;
		result = result_string;
	}

	std::string help() { return std::string("returns active vfo in Hertz"); }

} rig_get_vfo(&rig_server);

//------------------------------------------------------------------------------
// Request for vfo in use
//------------------------------------------------------------------------------

class rig_get_AB : public XmlRpcServerMethod {
public:
	rig_get_AB(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_AB", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		result[0] = useB ? "B" : "A";
	}

	std::string help() { return std::string("returns vfo in use A or B"); }

} rig_get_AB(&rig_server);

//------------------------------------------------------------------------------
// Get notch value
//------------------------------------------------------------------------------
int  rig_xml_notch_val = 0;

class rig_get_notch : public XmlRpcServerMethod {
public:
	rig_get_notch(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_notch", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {

	if (progStatus.notch)
		result = (int)(progStatus.notch_val);
	else
		result = (int)0;
	}

	std::string help() { return std::string("returns notch value"); }

} rig_get_notch(&rig_server);

//------------------------------------------------------------------------------
// Set notch value
//------------------------------------------------------------------------------
class rig_set_notch : public XmlRpcServerMethod {
public:
	rig_set_notch(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_notch", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		static int ntch;
		ntch = static_cast<int>((double)(params[0]));
		progStatus.notch_val = ntch;
		if (ntch)
			progStatus.notch = true;
		else
			progStatus.notch = false;

		guard_lock serial_lock(&mutex_serial, 61);
		selrig->set_notch(progStatus.notch, progStatus.notch_val);
		Fl::awake(setNotchControl, static_cast<void *>(&ntch));
	}

	std::string help() { return std::string("sets notch value"); }

} rig_set_notch(&rig_server);

//------------------------------------------------------------------------------
// Request list of modes
//------------------------------------------------------------------------------

class rig_get_modes : public XmlRpcServerMethod {
public :
	rig_get_modes(XmlRpcServer *s) : XmlRpcServerMethod("rig.get_modes", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {

		if (!selrig->modes_) {
			result = "none";
			return;
		}
		XmlRpcValue modes;
		int i = 0;
		for (const char** mode = selrig->modes_; *mode; mode++, i++)
			modes[i] = *mode;
		result = modes;
		}

	std::string help() { return std::string("returns list of modes"); }

} rig_get_modes(&rig_server);

//------------------------------------------------------------------------------
// Request for active vfo mode
//------------------------------------------------------------------------------

class rig_get_mode : public XmlRpcServerMethod {
public:
	rig_get_mode(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_mode", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		int mode;
		if (useB) {
			guard_lock queB_lock(&mutex_queB);
			if (!queB.empty()) mode = queB.back().imode;
			else mode = vfoB.imode;
		} else {
			guard_lock queA_lock(&mutex_queA);
			if (!queA.empty()) mode = queA.back().imode;
			else mode = vfoA.imode;
		}
		std::string result_string = selrig->modes_ ? selrig->modes_[mode] : "none";
		result = result_string;
	}

	std::string help() { return std::string("returns current xcvr mode"); }

} rig_get_mode(&rig_server);

//------------------------------------------------------------------------------
// Request array of bandwidths
//------------------------------------------------------------------------------

class rig_get_bws : public XmlRpcServerMethod {
public :
	rig_get_bws(XmlRpcServer *s) : XmlRpcServerMethod("rig.get_bws", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {

		if (!selrig->bandwidths_) {
			result = "none";
			return;
		}
		XmlRpcValue bws;

		int mode = useB ? vfoB.imode : vfoA.imode;
		const char **bwt = selrig->bwtable(mode);
		const char **dsplo = selrig->lotable(mode);
		const char **dsphi = selrig->hitable(mode);

// single bandwidth table
		if (!selrig->has_dsp_controls || !dsplo || !dsphi) {
			int i = 0;
			bws[0][0] = "Bandwidth";
			while (*bwt) {
				bws[0][i+1] = *bwt;
				bwt++;
				i++;
			}
		}
// double table either lo/hi or center/width
		if (selrig->has_dsp_controls && dsplo && dsphi) {
			int i = 0;
			int n = 1;
			std::string control_label = selrig->SL_label;
			control_label.append("|").append(selrig->SL_tooltip);
			bws[0][0] = control_label.c_str();
			while (dsplo[i]) {
				bws[0][n] = dsplo[i];
				n++; i++;
			}
			i = 0;
			control_label.assign(selrig->SH_label);
			control_label.append("|").append(selrig->SH_tooltip);
			bws[1][0] = control_label.c_str();
			n = 1;
			while (dsphi[i]) {
				bws[1][n] = dsphi[i];
				n++; i++;
			}
		}
		result = bws;
	}

	std::string help() { return std::string("returns array of bandwidths"); }

} rig_get_bws(&rig_server);

//------------------------------------------------------------------------------
// Request for active vfo bandwidth
//------------------------------------------------------------------------------

class rig_get_bw : public XmlRpcServerMethod {
public:
	rig_get_bw(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_bw", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		{
			guard_lock queA_lock(&mutex_queA);
			guard_lock queB_lock(&mutex_queB);
			if (! queA.empty() || !queB.empty()) {
				result[0] = "";
				result[1] = "";
				return;
			}
		}

		int BW = useB ? vfoB.iBW : vfoA.iBW;
		int mode = useB ? vfoB.imode : vfoA.imode;
		const char **bwt = selrig->bwtable(mode);
		const char **dsplo = selrig->lotable(mode);
		const char **dsphi = selrig->hitable(mode);
//std::cout << "BW " << std::hex << BW << std::dec << "\n";
		result[0] = (BW > 256 && selrig->has_dsp_controls) ?
					(dsplo ? dsplo[BW & 0x7F] : "") : 
					(bwt ? bwt[BW] : "");
		result[1] = (BW > 256 && selrig->has_dsp_controls) ?
					(dsphi ? dsphi[(BW >> 8) & 0x7F] : "") : 
					"";
	}

	std::string help() { return std::string("returns current bw L/U value"); }

} rig_get_bw(&rig_server);


class rig_get_smeter : public XmlRpcServerMethod {
public:
	rig_get_smeter(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_smeter", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!selrig->has_smeter) 
			result = (int)(0);
		else
			result = (int)(mval);
	}
} rig_get_smeter(&rig_server);


class rig_get_pwrmeter : public XmlRpcServerMethod {
public:
	rig_get_pwrmeter(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_pwrmeter", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!selrig->has_power_out) 
			result = (int)(0);
		else
			result = (int)(mval);
	}
} rig_get_pwrmeter(&rig_server);


//==============================================================================
// set interface
//==============================================================================
#include <queue>
#include "rigbase.h"

extern char *print(FREQMODE);
extern queue<FREQMODE> queA;
extern queue<FREQMODE> queB;
extern queue<bool> quePTT;

extern FREQMODE vfoA;
extern FREQMODE vfoB;

FREQMODE srvr_vfo;

static void push_xml()
{
	srvr_vfo.src = SRVR;
	char pr[200];
	snprintf(pr, sizeof(pr), "srvr: %ld, mode: %i, bw-HI: %i | bw-LO %i",
		srvr_vfo.freq,
		srvr_vfo.imode,
		(srvr_vfo.iBW / 256),
		(srvr_vfo.iBW & 0xFF));
//std::cout << pr << "\n";
	LOG_DEBUG("%s", pr);
	if (useB) {
		guard_lock gl_xmlqueB(&mutex_queB, 101);
		queB.push(srvr_vfo);
	} else {
		guard_lock gl_xmlqueA(&mutex_queA, 102);
		queA.push(srvr_vfo);
	}
}

//------------------------------------------------------------------------------
// Set PTT on (1) or off (0)
//------------------------------------------------------------------------------
class rig_set_ptt : public XmlRpcServerMethod {
public:
	rig_set_ptt(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_ptt", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		int state = int(params[0]);
		if (state) Fl::awake(setPTT, (void *)1);
		else Fl::awake(setPTT, (void *)0);
	}

	std::string help() { return std::string("sets PTT on (1) or off (0)"); }

} rig_set_ptt(&rig_server);

//------------------------------------------------------------------------------
// Set vfo in use A or B
//------------------------------------------------------------------------------
static void selectA(void *)
{
	cb_selectA();
}

static void selectB(void *)
{
	cb_selectB();
}

class rig_set_AB : public XmlRpcServerMethod {
public:
	rig_set_AB(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_AB", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		std::string ans = std::string(params[0]);
		if (ans == "A" && useB) Fl::awake(selectA);
		if (ans == "B" && !useB) Fl::awake(selectB);
	}

	std::string help() { return std::string("sets vfo in use A or B"); }

} rig_set_AB(&rig_server);

//------------------------------------------------------------------------------
// Set active vfo frequency
//------------------------------------------------------------------------------

class rig_set_vfo : public XmlRpcServerMethod {
public:
	rig_set_vfo(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_vfo", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		long freq = static_cast<long>(double(params[0]));
// set the frequency in vfoA or vfoB
		if (useB) srvr_vfo = vfoB;
		else       srvr_vfo = vfoA;
		srvr_vfo.freq = freq;
		push_xml();
	}
	std::string help() { return std::string("rig.set_vfo NNNNNNNN (Hz)"); }

} rig_set_vfo(&rig_server);


//------------------------------------------------------------------------------
// Set mode
//------------------------------------------------------------------------------

class rig_set_mode : public XmlRpcServerMethod {
public:
	rig_set_mode(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_mode", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue &result) {
		if (useB) {
			guard_lock queB_lock(&mutex_queB);
			if (!queB.empty()) srvr_vfo = queB.back();
			else srvr_vfo = vfoB;
		} else {
			guard_lock queA_lock(&mutex_queA);
			if (!queA.empty()) srvr_vfo = queA.back();
			else srvr_vfo = vfoA;
		}
		std::string numode = string(params[0]);
		int i = 0;
		if (!selrig->modes_) return;
		if (numode == selrig->modes_[srvr_vfo.imode]) return;
		while (selrig->modes_[i] != NULL) {
			if (numode == selrig->modes_[i]) {
				srvr_vfo.imode = i;
				srvr_vfo.iBW = 0;
				push_xml();
				break;
			}
			i++;
		}
	}
	std::string help() { return std::string("set_mode MODE_NAME"); }

} rig_set_mode(&rig_server);


//------------------------------------------------------------------------------
// Set bandwidth
//------------------------------------------------------------------------------

class rig_set_bw : public XmlRpcServerMethod {
public:
	rig_set_bw(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_bw", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		int bw = int(params[0]);
		if (useB) {
			guard_lock queB_lock(&mutex_queB);
			if (!queB.empty()) srvr_vfo = queB.back();
			else srvr_vfo = vfoB;
		} else {
			guard_lock queA_lock(&mutex_queA);
			if (!queA.empty()) srvr_vfo = queA.back();
			else srvr_vfo = vfoA;
		}
		srvr_vfo.iBW = bw;
		push_xml();
	}
	std::string help() { return std::string("set_bw to VAL"); }

} rig_set_bw(&rig_server);


//------------------------------------------------------------------------------
// support thread xmlrpc clients
//------------------------------------------------------------------------------

pthread_t *xml_thread = 0;

void * xml_thread_loop(void *d)
{
	for(;;) {
		rig_server.work(-1.0);
	}
	return NULL;
}

void start_server(int port)
{
	XmlRpc::setVerbosity(0);

// Create the server socket on the specified port
	rig_server.bindAndListen(port);

// Enable introspection
	rig_server.enableIntrospection(true);

	xml_thread = new pthread_t;
	if (pthread_create(xml_thread, NULL, xml_thread_loop, NULL)) {
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}
}

void exit_server()
{
	rig_server.exit();
}


