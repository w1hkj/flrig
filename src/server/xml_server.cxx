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
	int freq;
	int mode;
	int BW;
	static char szval[20];
	if (useB) {
		guard_lock queB_lock(&mutex_queB);
		if (!queB.empty()) {
			freq = queB.back().freq;
			mode = queB.back().imode;
		} else {
			freq = vfoB.freq;
			mode = vfoB.imode;
		}
	} else {
		guard_lock queA_lock(&mutex_queA);
		if (!queA.empty()) {
			freq = queA.back().freq;
			mode = queA.back().imode;
		} else {
			freq = vfoA.freq;
			mode = vfoB.imode;
		}
	}

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
		if (!xcvr_initialized) {
			result = "A";
			return;
		}
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
		XmlRpcValue modes;

		if (!xcvr_initialized || !selrig->modes_) {
			modes[0] = "CW";
			modes[1] = "LSB";
			modes[2] = "USB";
			return;
		}

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
		if (useB) {
			guard_lock queB_lock(&mutex_queB);
			if (!queB.empty()) mode = queB.back().imode;
			else mode = vfoB.imode;
		} else {
			guard_lock queA_lock(&mutex_queA);
			if (!queA.empty()) mode = queA.back().imode;
			else mode = vfoA.imode;
		}
		std::string result_string = "";
		result_string += selrig->get_modetype(mode);
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
		if (useB) {
			guard_lock queB_lock(&mutex_queB);
			if (!queB.empty()) mode = queB.back().imode;
			else mode = vfoB.imode;
		} else {
			guard_lock queA_lock(&mutex_queA);
			if (!queA.empty()) mode = queA.back().imode;
			else mode = vfoA.imode;
		}
		std::string result_string = "none";
		if (selrig->modes_) result_string = selrig->modes_[mode];
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

		{
			guard_lock queA_lock(&mutex_queA);
			guard_lock queB_lock(&mutex_queB);
			if (! queA.empty() || !queB.empty()) return;
		}

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

		int SB = BW & 0x7F;
		if (SB < 0) SB = 0;
		if (SB > max_bwt) SB = max_bwt;

		int SL = BW & 0x7F;
		if (SL >= max_lotable) SL = max_lotable - 1;
		if (SL < 0) SL = 0;

		int SH = (BW >> 8) & 0x7F;
		if (SH >= max_hitable) SH = max_hitable - 1;
		if (SH < 0) SH = 0;

		result[0] = result[1] = "";
		if (BW > 256 && selrig->has_dsp_controls) {
			if (dsplo) result[0] = dsplo[SL];
			if (dsphi) result[1] = dsphi[SH];
		} else
			if (bwt) result[0] = bwt[SB];

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

extern queue<XCVR_STATE> queA;
extern queue<XCVR_STATE> queB;
extern queue<bool> quePTT;

extern XCVR_STATE vfoA;
extern XCVR_STATE vfoB;

XCVR_STATE srvr_vfo;

static void push_xml()
{
	srvr_vfo.src = SRVR;
	char pr[200];
	snprintf(pr, sizeof(pr), "srvr: %ld, mode: %i, bw-HI: %i | bw-LO %i",
		srvr_vfo.freq,
		srvr_vfo.imode,
		(srvr_vfo.iBW / 256),
		(srvr_vfo.iBW & 0xFF));
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
		if (!xcvr_initialized) {
			result = 0;
			return;
		}
		int state = int(params[0]);

//std::cout << "set ptt" << state << std::endl;
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
		if (!xcvr_initialized) {
			result = 0;
			return;
		}
		std::string ans = std::string(params[0]);

//std::cout << "select vfo " << ans << std::endl;

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
		if (!xcvr_initialized) {
			result = 0;
			return;
		}
		long freq = static_cast<long>(double(params[0]));
// set the frequency in vfoA or vfoB
		if (useB) srvr_vfo = vfoB;
		else       srvr_vfo = vfoA;
		srvr_vfo.freq = freq;
//std::cout << "set vfo " << freq << std::endl;
		push_xml();
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
//std::cout << "set freq " << freq << std::endl;

		push_xml();
	}
	std::string help() { return std::string("main.set_frequency NNNNNNNN (Hz)"); }

} main_set_frequency(&rig_server);

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

		if (!selrig->modes_) {
			return;
		}
		if (numode == selrig->modes_[srvr_vfo.imode]) return;
		while (selrig->modes_[i] != NULL) {
			if (numode == selrig->modes_[i]) {
				srvr_vfo.imode = i;
				srvr_vfo.iBW = selrig->def_bandwidth(i);
				push_xml();
				break;
			}
			i++;
		}
//std::cout << "set mode\n";
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
		if (!xcvr_initialized) {
			result = 0;
			return;
		}
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
//std::cout << "set bw\n";
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

} rig_set_BW(&rig_server);

struct MLIST {
	string name; string signature; string help;
} mlist[] = {
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
	{ "rig.get_update",   "s:n", "return update to info" },
	{ "rig.get_vfo",      "s:n", "return current VFO in Hz" },
	{ "rig.get_xcvr",     "s:n", "returns name of transceiver" },
	{ "rig.set_AB",       "s:s", "set VFO A/B" },
	{ "rig.set_bw",       "i:i", "set BW iaw BW table" },
	{ "rig.set_BW",       "i:i", "set L/U pair" },
	{ "rig.set_mode",     "i:i", "set MODE iaw MODE table" },
	{ "rig.set_notch",    "d:d", "set NOTCH value in Hz" },
	{ "rig.set_ptt",      "i:i", "set PTT 1/0 (on/off)" },
	{ "rig.set_vfo",      "d:d", "set current VFO in Hz" },
	{ "main.set_frequency",      "d:d", "set current VFO in Hz" }
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


