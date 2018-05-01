// ---------------------------------------------------------------------
//
// xml_server.cxx, a part of flrig
//
// Copyright (C) 2014
// Dave Freese, W1HKJ
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
#include <sstream>
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

#include "tod_clock.h"

// The server
XmlRpcServer rig_server;

extern bool xcvr_initialized;

//------------------------------------------------------------------------------
// Request for transceiver name
//------------------------------------------------------------------------------
class rig_get_xcvr : public XmlRpcServerMethod {
public:
	rig_get_xcvr(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_xcvr", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_initialized)
			result = "";
		else
			result = selrig->name_;
	}

	std::string help() { return std::string("returns noun name of transceiver"); }

} rig_get_xcvr(&rig_server);

//------------------------------------------------------------------------------
// Request for info
//------------------------------------------------------------------------------
string uname;
string ufreq;
string umode;
string unotch;
string ubw;
string utx;

static string sname()
{
	string temp;
	temp.assign("R:").append(selrig->name_).append("\n");
	return temp;
}

static string stx()
{
	string temp;
	temp.assign("T:").append(btnPTT->value() ? "X" : "R").append("\n");
	return temp;
}

static string tempfreq;
static string tempmode;
static string tempbw;

static void freq_mode_bw()
{
	string temp;
	int freq = 0;
	int mode = 0;
	int BW = 0;
	static char szval[20];

	snprintf(szval, sizeof(szval), "%d", freq);
	tempfreq.assign("F").append(useB ? "B:" : "A:").append(szval).append("\n");

	BW = useB ? vfoB.iBW : vfoA.iBW;
	mode = useB ? vfoB.imode : vfoA.imode;
	const char **bwt = selrig->bwtable(mode);
	const char **dsplo = selrig->lotable(mode);
	const char **dsphi = selrig->hitable(mode);

	tempmode.assign("M:").append(selrig->modes_ ? selrig->modes_[mode] : "none").append("\n");

	tempbw.assign("L:").append((BW > 256 && selrig->has_dsp_controls) ?
						(dsplo ? dsplo[BW & 0x7F] : "") : 
						(bwt ? bwt[BW] : "")).append("\n");
	tempbw.append("U:").append((BW > 256 && selrig->has_dsp_controls) ?
						(dsphi ? dsphi[(BW >> 8) & 0x7F] : "") : 
						"").append("\n");
}

static string snotch()
{
	string temp;
	static char szval[20];
	snprintf(szval, sizeof(szval), "%d", (int)(progStatus.notch_val));
	temp.assign("N:").append(szval).append("\n");
	return temp;
}

class rig_get_info : public XmlRpcServerMethod {
public:
	rig_get_info(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_info", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		string info = "";

		if (!xcvr_initialized) {
			result = info;
			return;
		}

		uname = sname();   info.assign(uname);
		utx = stx();       info.append(utx);
		freq_mode_bw();
		ufreq = tempfreq;  info.append(ufreq);
		umode = tempmode;  info.append(umode);
		ubw = tempbw;      info.append(ubw);
		unotch = snotch(); info.append(unotch);

		result = info;

	}

	std::string help() { return std::string("returns all info in single string"); }

} rig_get_info(&rig_server);

class rig_get_update : public XmlRpcServerMethod {
public:
	rig_get_update(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_update", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		string info;  info.clear();
		string temp;  temp.clear();

		if (!xcvr_initialized) {
			result = info;
			return;
		}

		if (selrig->has_smeter && !btnPTT->value()) {
			static char szval[10];
			snprintf(szval, sizeof(szval), "S:%d\n", (int)mval);
			info.append(szval);
		}
		if (selrig->has_power_out && btnPTT->value()) {
			static char szval[10];
			snprintf(szval, sizeof(szval), "P:%d\n", (int)mval);
			info.append(szval);
		}

		if ((temp = sname()) != uname) { uname = temp;     info.append(uname); }
		if ((temp = stx()) != utx)     { utx = temp;       info.append(utx);}

		freq_mode_bw();
		if (tempfreq != ufreq) { ufreq = tempfreq; info.append(ufreq); }
		if (tempmode != umode) { umode = tempmode; info.append(umode); }
		if (tempbw != ubw)     { ubw = tempbw;     info.append(ubw); }
		if ((temp = snotch()) != unotch) { unotch = temp; info.append(unotch); }

		if (info.empty()) info.assign("NIL");

		result = info;

	}

	std::string help() { return std::string("returns all updates in single string"); }

} rig_get_update(&rig_server);


//------------------------------------------------------------------------------
// Request for PTT state
//------------------------------------------------------------------------------
class rig_get_ptt : public XmlRpcServerMethod {
public:
	rig_get_ptt(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_ptt", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		result = int(PTT);
	}

	std::string help() { return std::string("returns state of PTT"); }

} rig_get_ptt(&rig_server);

//------------------------------------------------------------------------------
// Request for split state
//------------------------------------------------------------------------------
inline void wait()
{
	int n = 0;
	while (!srvc_reqs.empty()) {
		MilliSleep(10);
		if (++n == 1000) break;
	}
}

class rig_get_split : public XmlRpcServerMethod {
public:
	rig_get_split(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_split", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {

		wait();
		guard_lock service_lock(&mutex_srvc_reqs, 155);
		result = progStatus.split;
	}

	std::string help() { return std::string("returns state of split"); }

} rig_get_split(&rig_server);

//------------------------------------------------------------------------------
// Request for active vfo frequency
//------------------------------------------------------------------------------
class rig_get_vfo : public XmlRpcServerMethod {
public:
	rig_get_vfo(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_vfo", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_initialized) {
			result = "14070000";
			return;
		}

		static char szfreq[20];
		int freq;

		wait();
		guard_lock service_lock(&mutex_srvc_reqs);//, 156);

		if (useB)
			freq = vfoB.freq;
		else
			freq = vfoA.freq;

		snprintf(szfreq, sizeof(szfreq), "%d", freq);
		std::string result_string = szfreq;
trace(3, "freq on vfo ", (useB ? "B " : "A "), szfreq);
		result = result_string;
	}

	std::string help() { return std::string("returns active vfo in Hertz"); }

} rig_get_vfo(&rig_server);

//------------------------------------------------------------------------------
// Request for vfo A frequency
//------------------------------------------------------------------------------
class rig_get_vfoA : public XmlRpcServerMethod {
public:
	rig_get_vfoA(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_vfoA", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_initialized) {
			result = "14070000";
			return;
		}
		int freq;

		wait();
		guard_lock service_lock(&mutex_srvc_reqs);//, 157);

		freq = vfoA.freq;

		static char szfreq[20];
		snprintf(szfreq, sizeof(szfreq), "%d", freq);
		std::string result_string = szfreq;
trace(2, "freq on vfo A", szfreq);
		result = result_string;
	}

	std::string help() { return std::string("returns active vfo in Hertz"); }

} rig_get_vfoA(&rig_server);

//------------------------------------------------------------------------------
// Request for vfo A frequency
//------------------------------------------------------------------------------
class rig_get_vfoB : public XmlRpcServerMethod {
public:
	rig_get_vfoB(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_vfoB", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_initialized) {
			result = "14070000";
			return;
		}
		int freq;

		wait();
		guard_lock service_lock(&mutex_srvc_reqs);//, 158);

		freq = vfoB.freq;

		static char szfreq[20];
		snprintf(szfreq, sizeof(szfreq), "%d", freq);
		std::string result_string = szfreq;
trace(2, "freq on vfo B", szfreq);
		result = result_string;
	}

	std::string help() { return std::string("returns active vfo in Hertz"); }

} rig_get_vfoB(&rig_server);

//------------------------------------------------------------------------------
// Request for vfo in use
//------------------------------------------------------------------------------

class rig_get_AB : public XmlRpcServerMethod {
public:
	rig_get_AB(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_AB", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_initialized) {
			result = "A";
			return;
		}

		wait();
		guard_lock service_lock(&mutex_srvc_reqs);//, 159);

		result = useB ? "B" : "A";
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
		if (!xcvr_initialized) {
			result = 0;
			return;
		}

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
		if (!xcvr_initialized) {
			result = 0;
			return;
		}

		static int ntch;
		ntch = static_cast<int>((double)(params[0]));
		if (ntch) progStatus.notch_val = ntch;
		if (ntch)
			progStatus.notch = true;
		else
			progStatus.notch = false;

		guard_lock serial_lock(&mutex_serial, 103);
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
		XmlRpcValue modes;

		if (!xcvr_initialized || !selrig->modes_) {
			modes[0] = "CW";
			modes[1] = "LSB";
			modes[2] = "USB";
			return;
		}

		wait();
		guard_lock service_lock(&mutex_srvc_reqs);//, 150);

		trace(1, "rig_get_modes");

		int i = 0;
		for (const char** mode = selrig->modes_; *mode; mode++, i++)
			modes[i] = *mode;
		result = modes;

		}

	std::string help() { return std::string("returns list of modes"); }

} rig_get_modes(&rig_server);

//------------------------------------------------------------------------------
// Request for active vfo sideband
//------------------------------------------------------------------------------

class rig_get_sideband : public XmlRpcServerMethod {
public:
	rig_get_sideband(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_sideband", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {

		if (!xcvr_initialized) {
			result = "U";
			return;
		}
		int mode;

		wait();
		guard_lock service_lock(&mutex_srvc_reqs);//, 151);

		mode = vfo->imode;

		std::string result_string = "";
		result_string += selrig->get_modetype(mode);
trace(2, "rig_get_sideband ", result_string.c_str());
		result = result_string;

	}

	std::string help() { return std::string("returns current xcvr sideband (U/L)"); }

} rig_get_sideband(&rig_server);

//------------------------------------------------------------------------------
// Request for active vfo mode
//------------------------------------------------------------------------------

class rig_get_mode : public XmlRpcServerMethod {
public:
	rig_get_mode(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_mode", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_initialized) {
			result = "USB";
			return;
		}

		int mode;
		wait();
		guard_lock service_lock(&mutex_srvc_reqs);//, 152);

		mode = vfo->imode;

		std::string result_string = "none";
		if (selrig->modes_) result_string = selrig->modes_[mode];
trace(3, "mode on ", (useB ? "B " : "A "), result_string.c_str());
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

		if (!xcvr_initialized || !selrig->bandwidths_) {
			XmlRpcValue bws;
			bws[0][0] = "Bandwidth";
			bws[0][1] = "NONE";
			result = bws;
			return;
		}
		XmlRpcValue bws;

		wait();
		guard_lock service_lock(&mutex_srvc_reqs);//, 153);

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

		result[0] = "NONE";
		result[1] = "";

		if (!xcvr_initialized) return;

		wait();
		guard_lock service_lock(&mutex_srvc_reqs);//, 154);

		int BW = useB ? vfoB.iBW : vfoA.iBW;
		int mode = useB ? vfoB.imode : vfoA.imode;

		const char **bwt = selrig->bwtable(mode);
		const char **dsplo = selrig->lotable(mode);
		const char **dsphi = selrig->hitable(mode);

		int max_bwt = 0,
			max_lotable = 0,
			max_hitable = 0;
		if (bwt)   while (bwt[max_bwt] != NULL) max_bwt++;
		if (dsplo) while (dsplo[max_lotable] != NULL) max_lotable++;
		if (dsphi) while (dsphi[max_hitable] != NULL) max_hitable++;

		result[0] = result[1] = "";
		if (BW < 256 && bwt) {
			int SB = BW & 0x7F;
			if (SB < 0) SB = 0;
			if (SB >= max_bwt) SB = max_bwt-1;
			result[0] = bwt[SB];
		}
		else if (dsplo && dsphi) {
			int SL = BW & 0x7F;
			if (SL >= max_lotable) SL = max_lotable - 1;
			if (SL < 0) SL = 0;

			int SH = (BW >> 8) & 0x7F;
			if (SH >= max_hitable) SH = max_hitable - 1;
			if (SH < 0) SH = 0;

			if (dsplo) result[0] = dsplo[SL];
			if (dsphi) result[1] = dsphi[SH];
		}
		std::string s1 = result[0], s2 = result[1];
trace( 5, "bandwidth on ", (useB ? "B " : "A "), s1.c_str(), " | ", s2.c_str());
	}

	std::string help() { return std::string("returns current bw L/U value"); }

} rig_get_bw(&rig_server);


class rig_get_smeter : public XmlRpcServerMethod {
public:
	rig_get_smeter(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_smeter", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_initialized || !selrig->has_smeter) 
			result = (int)(0);
		else
			result = (int)(mval);
	}
} rig_get_smeter(&rig_server);


class rig_get_pwrmeter : public XmlRpcServerMethod {
public:
	rig_get_pwrmeter(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_pwrmeter", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_initialized || !selrig->has_power_out) 
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

extern queue<VFOQUEUE> srvc_reqs;
extern queue<bool> quePTT;

extern XCVR_STATE vfoA;
extern XCVR_STATE vfoB;

XCVR_STATE srvr_vfo;

std::string print(int f, int m, int b)
{
	std::ostringstream p;
//	p.seekp(ios::beg);
	if (b > 65536) b /= 65536;
	p << "freq: " << f << ", imode: " << m << ", bw " << b;
	return p.str();
}

static void push_xml()
{
	srvr_vfo.src = SRVR;
	guard_lock service_lock(&mutex_srvc_reqs, 108);
trace(2, "push_xml()", print(srvr_vfo.freq, srvr_vfo.imode, srvr_vfo.iBW).c_str());
	srvc_reqs.push(VFOQUEUE(vX, srvr_vfo));//(useB ? vB : vA), srvr_vfo ));
}

static void push_xmlA()
{
	srvr_vfo.src = SRVR;
	guard_lock service_lock(&mutex_srvc_reqs, 109);
trace(2, "push_xmlA()", print(srvr_vfo.freq, srvr_vfo.imode, srvr_vfo.iBW).c_str());
	srvc_reqs.push(VFOQUEUE( vA, srvr_vfo));
}

static void push_xmlB()
{
	srvr_vfo.src = SRVR;
	guard_lock service_lock(&mutex_srvc_reqs, 110);
trace(2, "push_xmlB()", print(srvr_vfo.freq, srvr_vfo.imode, srvr_vfo.iBW).c_str());
	srvc_reqs.push(VFOQUEUE(vB, srvr_vfo));
}


//------------------------------------------------------------------------------
// Set PTT on (1) or off (0)
//------------------------------------------------------------------------------
class rig_set_ptt : public XmlRpcServerMethod {
public:
	rig_set_ptt(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_ptt", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_initialized) {
			result = 0;
			return;
		}
		int state = int(params[0]);

		guard_lock que_lock(&mutex_srvc_reqs, 180);

		VFOQUEUE xcvrptt;
		if (state) xcvrptt.change = ON;
		else       xcvrptt.change = OFF;
		trace(1, (state ? "rig_set_ptt ON" : "rig_set_ptt OFF"));
		srvc_reqs.push(xcvrptt);
	}

	std::string help() { return std::string("sets PTT on (1) or off (0)"); }

} rig_set_ptt(&rig_server);

//------------------------------------------------------------------------------
// Execute vfo Swap
//------------------------------------------------------------------------------
//void do_swap(void *)
//{
//	cbAswapB();
//}

class rig_swap : public XmlRpcServerMethod {
public:
	rig_swap(XmlRpcServer* s) : XmlRpcServerMethod("rig.swap", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_initialized) {
			result = 0;
			return;
		}

	guard_lock que_lock( &mutex_srvc_reqs, 160);
	if (useB) {
		srvc_reqs.push(VFOQUEUE(vB, vfoA));
		srvc_reqs.push(VFOQUEUE(sA, vfoA));
		srvc_reqs.push(VFOQUEUE(vA, vfoB));
		srvc_reqs.push(VFOQUEUE(sB, vfoB));
	} else {
		srvc_reqs.push(VFOQUEUE(vA, vfoB));
		srvc_reqs.push(VFOQUEUE(sB, vfoB));
		srvc_reqs.push(VFOQUEUE(vB, vfoA));
		srvc_reqs.push(VFOQUEUE(sA, vfoA));
	}

	}

	std::string help() { return std::string("executes vfo swap"); }

} rig_swap(&rig_server);

// deprecated method - retain for backward compatibility

class rig_set_swap : public XmlRpcServerMethod {
public:
	rig_set_swap(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_swap", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_initialized) {
			result = 0;
			return;
		}

	guard_lock que_lock( &mutex_srvc_reqs, 161);
	if (useB) {
		srvc_reqs.push(VFOQUEUE(vB, vfoA));
		srvc_reqs.push(VFOQUEUE(sA, vfoA));
		srvc_reqs.push(VFOQUEUE(vA, vfoB));
		srvc_reqs.push(VFOQUEUE(sB, vfoB));
	} else {
		srvc_reqs.push(VFOQUEUE(vA, vfoB));
		srvc_reqs.push(VFOQUEUE(sB, vfoB));
		srvc_reqs.push(VFOQUEUE(vB, vfoA));
		srvc_reqs.push(VFOQUEUE(sA, vfoA));
	}

	}

	std::string help() { return std::string("executes vfo swap"); }

} rig_set_swap(&rig_server);

//------------------------------------------------------------------------------
// Execute vfo split operation
//------------------------------------------------------------------------------

class rig_set_split : public XmlRpcServerMethod {
public:
	rig_set_split(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_split", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_initialized) {
			result = 0;
			return;
		}
		int state = int(params[0]);

		if (selrig->has_split_AB) {
			guard_lock serial_lock(&mutex_serial);
			ostringstream s;
			s << "set split(" << (state ? "ON" : "OFF") << ")";
			trace(1, s.str().c_str());
			progStatus.split = state;
			selrig->set_split(state);

			Fl::awake(update_split);
		}
	}

	std::string help() { return std::string("executes vfo split"); }

} rig_set_split(&rig_server);

//------------------------------------------------------------------------------
// Set vfo in use A or B
//------------------------------------------------------------------------------
inline bool ptt_off()
{
	int n = 0;
	if (!PTT) return true;
	while (1) {
		if (n++ == 1000) break;
		MilliSleep(10);
		if (!PTT) return true;
	}
	return false;
}

class rig_set_AB : public XmlRpcServerMethod {
public:
	rig_set_AB(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_AB", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_initialized) {
			result = 0;
			return;
		}
		std::string ans = std::string(params[0]);
		if (!(ans == "A" || ans == "B")) {
			return;
		}

		if (!ptt_off()) {
std::cout << "!ptt_off()\n";
			return;
		}

		guard_lock service_lock(&mutex_srvc_reqs, 111);
		XCVR_STATE vfo = vfoA;

		vfo.src = SRVR;


trace(4, "set_AB ", ans.c_str(), " ", printXCVR_STATE(vfo).c_str());
		srvc_reqs.push (VFOQUEUE((ans == "A" ? sA : sB), vfo));
	}

	std::string help() { return std::string("sets vfo in use A or B"); }

} rig_set_AB(&rig_server);

//------------------------------------------------------------------------------
// Set vfoA frequency
//------------------------------------------------------------------------------

class rig_set_vfoA : public XmlRpcServerMethod {
public:
	rig_set_vfoA(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_vfoA", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_initialized) {
			result = 0;
			return;
		}
		long freq = static_cast<long>(double(params[0]));
		srvr_vfo = vfoA;
		srvr_vfo.freq = freq;
		srvr_vfo.imode = -1;
		srvr_vfo.iBW = 255;

		push_xmlA();
	}
	std::string help() { return std::string("rig.set_vfo NNNNNNNN (Hz)"); }

} rig_set_vfoA(&rig_server);

//------------------------------------------------------------------------------
// Set vfo B frequency
//------------------------------------------------------------------------------

class rig_set_vfoB : public XmlRpcServerMethod {
public:
	rig_set_vfoB(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_vfoB", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_initialized) {
			result = 0;
			return;
		}
		long freq = static_cast<long>(double(params[0]));
		srvr_vfo = vfoB;
		srvr_vfo.freq = freq;
		srvr_vfo.imode = -1;
		srvr_vfo.iBW = 255;

		push_xmlB();
	}
	std::string help() { return std::string("rig.set_vfo NNNNNNNN (Hz)"); }

} rig_set_vfoB(&rig_server);

//------------------------------------------------------------------------------
// Set active vfo frequency
//------------------------------------------------------------------------------

class rig_set_vfo : public XmlRpcServerMethod {
public:
	rig_set_vfo(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_vfo", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_initialized) {
			result = 0;
			return;
		}
		long freq = static_cast<long>(double(params[0]));
// set the frequency in vfoA or vfoB
		if (useB) srvr_vfo = vfoB;
		else       srvr_vfo = vfoA;
		srvr_vfo.freq = freq;
		srvr_vfo.imode = -1;
		srvr_vfo.iBW = 255;

		push_xml();
/*
		int ifreq = 0;
		int n = 0;
		while (ifreq != srvr_vfo.freq) {
			MilliSleep(10);
			ifreq = vfo->freq;
			n++;
		}
*/
	}
	std::string help() { return std::string("rig.set_vfo NNNNNNNN (Hz)"); }

} rig_set_vfo(&rig_server);


class main_set_frequency : public XmlRpcServerMethod {
public:
	main_set_frequency(XmlRpcServer* s) : XmlRpcServerMethod("main.set_frequency", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_initialized) {
			result = 0;
			return;
		}
		long freq = static_cast<long>(double(params[0]));
// set the frequency in vfoA or vfoB
		if (useB) srvr_vfo = vfoB;
		else       srvr_vfo = vfoA;
		srvr_vfo.freq = freq;
		srvr_vfo.imode = -1;
		srvr_vfo.iBW = 255;

		push_xml();
/*
		int ifreq = 0;
		int n = 0;
		while (ifreq != srvr_vfo.freq) {
			MilliSleep(10);
			ifreq = vfo->freq;
			n++;
		}
*/
	}
	std::string help() { return std::string("main.set_frequency NNNNNNNN (Hz)"); }

} main_set_frequency(&rig_server);

class rig_set_frequency : public XmlRpcServerMethod {
public:
	rig_set_frequency(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_frequency", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_initialized) {
			result = 0;
			return;
		}
		long freq = static_cast<long>(double(params[0]));
// set the frequency in vfoA or vfoB
		if (useB) srvr_vfo = vfoB;
		else       srvr_vfo = vfoA;
		srvr_vfo.freq = freq;
		srvr_vfo.imode = -1;
		srvr_vfo.iBW = 255;

		push_xml();

		int ifreq = 0;
		int n = 0;
		while (ifreq != srvr_vfo.freq) {
			MilliSleep(10);
			ifreq = vfo->freq;
			n++;
		}
	}
	std::string help() { return std::string("rig.set_frequency NNNNNNNN (Hz)"); }

} rig_set_frequency(&rig_server);

//------------------------------------------------------------------------------
// Set mode
//------------------------------------------------------------------------------

class rig_set_mode : public XmlRpcServerMethod {
public:
	rig_set_mode(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_mode", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue &result) {
		if (!xcvr_initialized) {
			result = 0;
			return;
		}

		if (useB)
			srvr_vfo = vfoB;
		else
			srvr_vfo = vfoA;

		std::string numode = string(params[0]);
		int i = 0;

		if (!selrig->modes_) {
			return;
		}
		if (numode == selrig->modes_[srvr_vfo.imode]) return;
		while (selrig->modes_[i] != NULL) {
			if (numode == selrig->modes_[i]) {
				srvr_vfo.imode = i;

				srvr_vfo.freq = 0;
				srvr_vfo.iBW = 255;

				push_xml();
				break;
			}
			i++;
		}
		int imode = -1;
		int n = 0;
		while (imode != srvr_vfo.imode) {
			MilliSleep(10);
			imode = vfo->imode;
			n++;
		}
	}
	std::string help() { return std::string("set_mode MODE_NAME"); }

} rig_set_mode(&rig_server);


//------------------------------------------------------------------------------
// Set bandwidth
//------------------------------------------------------------------------------

class rig_set_bandwidth : public XmlRpcServerMethod {
public:
	rig_set_bandwidth(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_bandwidth", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_initialized) {
			result = 0;
			return;
		}
		int bw = int(params[0]);

		{
			guard_lock lock(&mutex_srvc_reqs);
			if (useB) srvr_vfo = vfoB;
			else      srvr_vfo = vfoA;

			int i = 0;
			while (	selrig->bandwidths_[i] && 
				atol(selrig->bandwidths_[i]) < bw) {
				i++;
			}
			if (!selrig->bandwidths_[i]) i--;
			bw = atol(selrig->bandwidths_[i]);
ostringstream s;
s << "nearest bandwidth " << selrig->bandwidths_[i];
trace(2,"Set to ", s.str().c_str());
			srvr_vfo.iBW = i;//bw * 256 * 256;
			srvr_vfo.freq = 0;
			srvr_vfo.imode = -1;
		}

		push_xml();

	}
	std::string help() { return std::string("set_bw to nearest requested"); }

} rig_set_bandwidth(&rig_server);


class rig_set_bw : public XmlRpcServerMethod {
public:
	rig_set_bw(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_bw", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_initialized) {
			result = 0;
			return;
		}
		int bw = int(params[0]);

		if (useB)
			srvr_vfo = vfoB;
		else
			srvr_vfo = vfoA;

		srvr_vfo.iBW = bw;
		srvr_vfo.freq = 0;
		srvr_vfo.imode = -1;

		push_xml();

		int iBW = -1;
		int n = 0;
		while (iBW != srvr_vfo.iBW) {
			MilliSleep(10);
			iBW = vfo->iBW;
			n++;
		}
	}
	std::string help() { return std::string("set_bw to VAL"); }

} rig_set_bw(&rig_server);

class rig_set_BW : public XmlRpcServerMethod {
public:
	rig_set_BW(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_BW", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_initialized) {
			result = 0;
			return;
		}
		string bwstr = params;

		int bw = int(params[0]);
		guard_lock que_lock ( &mutex_srvc_reqs, 115 );
		if (useB)
			srvr_vfo = vfoB;
		else
			srvr_vfo = vfoA;

		srvr_vfo.iBW = bw;

		srvr_vfo.freq = 0;
		srvr_vfo.imode = -1;

		push_xml();

		int iBW = -1;
		int n = 0;
		while (iBW != srvr_vfo.iBW) {
			MilliSleep(10);
			iBW = vfo->iBW;
			n++;
		}

	}
	std::string help() { return std::string("set_bw to VAL"); }

} rig_set_BW(&rig_server);

struct MLIST {
	string name; string signature; string help;
} mlist[] = {
	{ "main.set_frequency", "d:d", "set current VFO in Hz" },
	{ "rig.get_AB",       "s:n", "returns vfo in use A or B" },
	{ "rig.get_bw",       "s:n", "return BW of current VFO" },
	{ "rig.get_bws",      "s:n", "return table of BW values" },
	{ "rig.get_info",     "s:n", "return an info string" },
	{ "rig.get_mode",     "s:n", "return MODE of current VFO" },
	{ "rig.get_modes",    "s:n", "return table of MODE values" },
	{ "rig.get_sideband", "s:n", "return sideband (U/L)" },
	{ "rig.get_notch",    "s:n", "return notch value" },
	{ "rig.get_ptt",      "s:n", "return PTT state" },
	{ "rig.get_pwrmeter", "s:n", "return PWR out" },
	{ "rig.get_smeter",   "s:n", "return Smeter" },
	{ "rig.get_split",    "s:n", "return split state" },
	{ "rig.get_update",   "s:n", "return update to info" },
	{ "rig.get_vfo",      "s:n", "return current VFO in Hz" },
	{ "rig.get_vfoA",     "s:n", "return vfo A in Hz" },
	{ "rig.get_vfoB",     "s:n", "return vfo B in Hz" },
	{ "rig.get_xcvr",     "s:n", "returns name of transceiver" },
	{ "rig.set_AB",       "s:s", "set VFO A/B" },
	{ "rig.set_bw",       "i:i", "set BW iaw BW table" },
	{ "rig.set_bandwidth","i:i", "set bandwidth to nearest requested value" },
	{ "rig.set_BW",       "i:i", "set L/U pair" },
	{ "rig.set_frequency","d:d", "set current VFO in Hz" },
	{ "rig.set_mode",     "i:i", "set MODE iaw MODE table" },
	{ "rig.set_notch",    "d:d", "set NOTCH value in Hz" },
	{ "rig.set_ptt",      "i:i", "set PTT 1/0 (on/off)" },
	{ "rig.set_vfo",      "d:d", "set current VFO in Hz" },
	{ "rig.set_vfoA",     "d:d", "set vfo A in Hz" },
	{ "rig.set_vfoB",     "d:d", "set vfo B in Hz" },
	{ "rig.set_split",    "i:i", "set split 1/0 (on/off)" },
	{ "rig.swap",         "i:i", "execute vfo swap" }
};

class rig_list_methods : public XmlRpcServerMethod {
public:
	rig_list_methods(XmlRpcServer *s) : XmlRpcServerMethod("rig.list_methods", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {

		vector<XmlRpcValue> methods;
		for (size_t n = 0; n < sizeof(mlist) / sizeof(*mlist); ++n) {
			XmlRpcValue::ValueStruct item;
			item["name"]      = mlist[n].name;
			item["signature"] = mlist[n].signature;
			item["help"]      = mlist[n].help;
			methods.push_back(item);
		}

		result = methods;
	}
	std::string help() { return std::string("get flrig methods"); }
} rig_list_methods(&rig_server);

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

std::string print_xmlhelp()
{
	string pstr;
	string line;
	for (size_t n = 0; n < sizeof(mlist) / sizeof(*mlist); ++n) {
		line.clear();
		line.assign(mlist[n].name);
		line.append(20 - line.length(), ' ');
		line.append(mlist[n].signature);
		line.append("  ");
		line.append(mlist[n].help);
		line.append("\n");
		pstr.append(line);
	}
	return pstr;
}

