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
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>
#include <vector>
#include <pthread.h>

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Enumerations.H>

#include "support.h"
#include "debug.h"
#include "trace.h"

#include "xml_server.h"
#include "XmlRpc.h"
#include "tod_clock.h"
#include "cwioUI.h"
#include "ptt.h"

#include "rigpanel.h"

// The server
using namespace XmlRpc;

XmlRpcServer rig_server;

//------------------------------------------------------------------------------
// Request for program version
//------------------------------------------------------------------------------
class main_get_version : public XmlRpcServerMethod {
public:
	main_get_version(XmlRpcServer* s) : XmlRpcServerMethod("main.get_version", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		result = FLRIG_VERSION;
	}

	std::string help() { return std::string("returns program version std::string"); }

} main_get_version(&rig_server);

//------------------------------------------------------------------------------
// Request for transceiver name
//------------------------------------------------------------------------------
class rig_get_xcvr : public XmlRpcServerMethod {
public:
	rig_get_xcvr(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_xcvr", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value())
			result = "";
		else
			result = selrig->name_;
	}

	std::string help() { return std::string("returns noun name of transceiver"); }

} rig_get_xcvr(&rig_server);

//------------------------------------------------------------------------------
// Request for info
//------------------------------------------------------------------------------
std::string uname;
std::string ufreq;
std::string umode;
std::string unotch;
std::string ubw;
std::string utx;
std::string urfg;
std::string uvol;
std::string umic;

static std::string sname()
{
	std::string temp;
	temp.assign("R:").append(selrig->name_).append("\n");
	return temp;
}

static std::string stx()
{
	std::string temp;
	temp.assign("T:").append(btnPTT->value() ? "X" : "R").append("\n");
	return temp;
}

static std::string tempfreq;
static std::string tempmode;
static std::string tempbw;

static void freq_mode_bw()
{
	std::string temp;
	int freq = 0;
	int mode = 0;
	int BW = 0;
	static char szval[20];

	try {
		snprintf(szval, sizeof(szval), "%d", freq);
		tempfreq.assign("F").append((selrig->inuse == onB) ? "B:" : "A:").append(szval).append("\n");

		BW = (selrig->inuse == onB) ? vfoB.iBW : vfoA.iBW;
		mode = (selrig->inuse == onB) ? vfoB.imode : vfoA.imode;
		std::vector<std::string>& bwt = selrig->bwtable(mode);
		std::vector<std::string>& dsplo = selrig->lotable(mode);
		std::vector<std::string>& dsphi = selrig->hitable(mode);

		tempmode.assign("M:").append(selrig->modes_.at(mode)).append("\n");

		tempbw.assign("L:").append((BW > 256) ?
							dsplo.at(BW & 0x7F) :
							bwt.at(BW & 0x7F)).append("\n");
		tempbw.append("U:").append((BW > 256) ?
							dsphi.at((BW >> 8) & 0x7F) : "n/a").append("\n");
	} catch (const std::exception& e) {
		LOG_ERROR("%s", e.what());
	}
}

static std::string snotch()
{
	std::string temp;
	static char szval[20];
	snprintf(szval, sizeof(szval), "%d", (int)(progStatus.notch_val));
	temp.assign("N:").append(szval).append("\n");
	return temp;
}

static std::string svol()
{
	std::string temp;
	static char szval[20];
	int volval = 0;
	if (spnrVOLUME) volval = spnrVOLUME->value();
	else if (sldrVOLUME) volval = sldrVOLUME->value();
	snprintf(szval, sizeof(szval), "%d", volval);
	temp.assign("Vol:").append(szval).append("\n");
	return temp;
}

static std::string srfg()
{
	std::string temp;
	static char szval[20];
	int rfgval = 0;
	if (spnrRFGAIN) rfgval = spnrRFGAIN->value();
	else if (sldrRFGAIN) rfgval = sldrRFGAIN->value();
	snprintf(szval, sizeof(szval), "%d", rfgval);
	temp.assign("Rfg:").append(szval).append("\n");
	return temp;
}

static std::string smic()
{
	std::string temp;
	static char szval[20];
	int micval = 0;
	if (sldrMICGAIN) micval = sldrMICGAIN->value();
	if (spnrMICGAIN) micval = spnrMICGAIN->value();
	snprintf(szval, sizeof(szval), "%d", micval);
	temp.assign("Mic:").append(szval).append("\n");
	return temp;
}

class rig_get_info : public XmlRpcServerMethod {
public:
	rig_get_info(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_info", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		std::string info = "";

		if (!xcvr_online || disable_xmlrpc->value()) {
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
		uvol = svol();     info.append(uvol);
		umic = smic();     info.append(umic);
		urfg = srfg();     info.append(urfg);

		result = info;

	}

	std::string help() { return std::string("returns all info in single std::string"); }

} rig_get_info(&rig_server);

class rig_get_update : public XmlRpcServerMethod {
public:
	rig_get_update(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_update", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		std::string info;  info.clear();
		std::string temp;  temp.clear();

		if (!xcvr_online || disable_xmlrpc->value()) {
			result = info;
			return;
		}

		if (selrig->has_smeter && !btnPTT->value()) {
			static char szval[10];
			snprintf(szval, sizeof(szval), "S:%5.0f\n", smtrval);
			info.append(szval);
		}
		if (selrig->has_power_out && btnPTT->value()) {
			static char szval[10];
			snprintf(szval, sizeof(szval), "P:%5.0f\n", pwrval);
			info.append(szval);
		}

		if ((temp = sname()) != uname) { uname = temp;     info.append(uname); }
		if ((temp = stx()) != utx)     { utx = temp;       info.append(utx);}

		freq_mode_bw();
		if (tempfreq != ufreq) { ufreq = tempfreq; info.append(ufreq); }
		if (tempmode != umode) { umode = tempmode; info.append(umode); }
		if (tempbw != ubw)     { ubw = tempbw;     info.append(ubw); }
		if ((temp = snotch()) != unotch) { unotch = temp; info.append(unotch); }
		uvol = svol();     info.append(uvol);
		umic = smic();     info.append(umic);
		urfg = srfg();     info.append(urfg);

		if (info.empty()) info.assign("NIL");

		result = info;

	}

	std::string help() { return std::string("returns all updates in single std::string"); }

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
	return;

	int n = 0;
	while (!srvc_reqs.empty()) {
		MilliSleep(10);
		if (++n == 500) break;
	}
	static char s[50];

	if (n == 500)
		snprintf(s, sizeof(s), "wait for srvc_reqs timed out");
	else
		snprintf(s, sizeof(s), "wait for srvc reqs %d msec", 10 * n);

	xml_trace(1, s);
}

class rig_get_split : public XmlRpcServerMethod {
public:
	rig_get_split(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_split", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {

		guard_lock serial(&mutex_serial);

		int split_state = selrig->get_split();
		progStatus.split = split_state;
		result = split_state;
xml_trace(2, "rig_get_split ", (split_state ? "ON" : "OFF"));
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
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = "14070000";
			return;
		}

		static char szfreq[20];
		unsigned long long freq;

		wait();
		guard_lock service_lock(&mutex_srvc_reqs, "xml rig_get_vfo");
		guard_lock serial_lock(&mutex_serial, "xml rig_get_vfo");

		if (selrig->ICOMmainsub) {
			if (progStatus.split && PTT) freq = vfoB.freq;
			else freq = vfoA.freq;
		} else {
			if (selrig->inuse == onB)
				freq = vfoB.freq;
			else
				freq = vfoA.freq;
		}

		snprintf(szfreq, sizeof(szfreq), "%llu", freq);
		std::string result_string = szfreq;
xml_trace(2, "rig_get_vfo ", szfreq);
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
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = "14070000";
			return;
		}
//		int freq;

		guard_lock serial(&mutex_serial);

		vfoA.freq = selrig->get_vfoA();
		Fl::awake(setFreqDispA);

		static char szfreq[20];
		snprintf(szfreq, sizeof(szfreq), "%llu", vfoA.freq);
		std::string result_string = szfreq;
xml_trace(2, "rig_get_vfoA", szfreq);

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
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = "14070000";
			return;
		}
		guard_lock serial(&mutex_serial);

		vfoB.freq = selrig->get_vfoB();
		Fl::awake(setFreqDispB);

		static char szfreq[20];
		snprintf(szfreq, sizeof(szfreq), "%llu", vfoB.freq);
		std::string result_string = szfreq;
xml_trace(2, "rig_get_vfoB", szfreq);

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
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = "A";
			return;
		}

		wait();
		guard_lock service_lock(&mutex_srvc_reqs, "xml rig_get_AB");
		guard_lock serial_lock(&mutex_serial, "xml rig_get_AB");

		xml_trace(2, "rig_get_AB: " , ((selrig->inuse == onB) ? "B" : "A"));
		result = (selrig->inuse == onB) ? "B" : "A";
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
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}

		if (progStatus.notch)
			result = (int)(progStatus.notch_val);
		else
			result = (int)0;
		xml_trace(1, "rig_get_notch");
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
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}

		static int ntch;
		ntch = (int)(params[0]);
		progStatus.notch_val = ntch;
		if (ntch)
			progStatus.notch = true;
		else
			progStatus.notch = false;

		guard_lock serial_lock(&mutex_serial, "xml rig_set_notch");
		selrig->set_notch(progStatus.notch, progStatus.notch_val);
		xml_trace(1, "rig_set_notch");
		Fl::awake(setNotchControl, static_cast<void *>(&ntch));
	}

	std::string help() { return std::string("sets notch value"); }

} rig_set_notch(&rig_server);

class rig_set_verify_notch : public XmlRpcServerMethod {
public:
	rig_set_verify_notch(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_verify_notch", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}

		static int ntch;
		ntch = (int)(params[0]);
		progStatus.notch_val = ntch;
		if (ntch)
			progStatus.notch = true;
		else
			progStatus.notch = false;

		guard_lock serial_lock(&mutex_serial, "xml rig_set_verify_notch");
		selrig->set_notch(progStatus.notch, progStatus.notch_val);
		xml_trace(1, "rig_set_verify_notch");
		Fl::awake(setNotchControl, static_cast<void *>(&ntch));
	}

	std::string help() { return std::string("sets & verifies notch value"); }

} rig_set_verify_notch(&rig_server);

//------------------------------------------------------------------------------
// Get rfgain value
//------------------------------------------------------------------------------
int  rig_xml_rfgain_val = 0;

class rig_get_rfgain : public XmlRpcServerMethod {
public:
	rig_get_rfgain(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_rfgain", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}

		if (progStatus.rfgain)
			result = (int)(progStatus.rfgain);
		else
			result = (int)0;
		xml_trace(1, "rig_get_rfgain");
	}

	std::string help() { return std::string("returns rfgain value"); }

} rig_get_rfgain(&rig_server);

//------------------------------------------------------------------------------
// Set rfgain value
//------------------------------------------------------------------------------

class rig_set_rfgain : public XmlRpcServerMethod {
public:
	rig_set_rfgain(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_rfgain", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}

		static int rfg;
		rfg = static_cast<int>((double)((params[0])));
		progStatus.rfgain = rfg;

		guard_lock serial_lock(&mutex_serial, "xml rig_set_rfgain");
		selrig->set_rf_gain(progStatus.rfgain);
		xml_trace(1, "rig_set_rfgain");
		Fl::awake(setRFGAINControl, static_cast<void *>(0));
	}

	std::string help() { return std::string("sets rfgain value"); }

} rig_set_rfgain(&rig_server);

class rig_set_verify_rfgain : public XmlRpcServerMethod {
public:
	rig_set_verify_rfgain(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_verify_rfgain", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}

		static int rfg;
		rfg = static_cast<int>((double)((params[0])));
		progStatus.rfgain = rfg;

		guard_lock serial_lock(&mutex_serial, "xml rig_set _verify_rfgain");
		selrig->set_rf_gain(progStatus.rfgain);

		progStatus.rfgain = selrig->get_rf_gain();

		xml_trace(1, "rig_set_verify_rfgain");
		Fl::awake(setRFGAINControl, static_cast<void *>(0));
	}

	std::string help() { return std::string("sets & verifies rfgain value"); }

} rig_set_verify_rfgain(&rig_server);

class rig_mod_rfg : public XmlRpcServerMethod {
public:
	rig_mod_rfg(XmlRpcServer* s) : XmlRpcServerMethod("rig.mod_rfg", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}

		int min, max, step;
		selrig->get_rf_min_max_step(min, max, step);

		int change = (int)(params[0]);
		progStatus.rfgain += change;
		if (progStatus.rfgain > max) progStatus.rfgain = max;
		if (progStatus.rfgain < min) progStatus.rfgain = min;

		guard_lock serial_lock(&mutex_serial, "xml rig_mod_rfgain");
		selrig->set_rf_gain(progStatus.rfgain);
		xml_trace(1, "rig_mod_rfgain");
		Fl::awake(setRFGAINControl, static_cast<void *>(0));
	}

	std::string help() { return std::string("modify rfgain +/- NNN units"); }

} rig_mod_rfg(&rig_server);

//------------------------------------------------------------------------------
// Get micgain value
//------------------------------------------------------------------------------
int  rig_xml_micgain_val = 0;

class rig_get_micgain : public XmlRpcServerMethod {
public:
	rig_get_micgain(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_micgain", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}

		if (progStatus.mic_gain)
			result = (int)(progStatus.mic_gain);
		else
			result = (int)0;
		xml_trace(1, "rig_get_micgain");
	}

	std::string help() { return std::string("returns micgain value"); }

} rig_get_micgain(&rig_server);

//------------------------------------------------------------------------------
// Set micgain value
//------------------------------------------------------------------------------

class rig_set_micgain : public XmlRpcServerMethod {
public:
	rig_set_micgain(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_micgain", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}

		static int micg;
		micg = (int)(params[0]);
		progStatus.mic_gain = micg;

		guard_lock serial_lock(&mutex_serial, "xml rig_set_micgain");
		xml_trace(1, "rig_set_micgain");
		selrig->set_mic_gain(progStatus.mic_gain);
		Fl::awake(setMicGainControl, static_cast<void *>(0));
	}

	std::string help() { return std::string("sets micgain value"); }

} rig_set_micgain(&rig_server);

class rig_set_verify_micgain : public XmlRpcServerMethod {
public:
	rig_set_verify_micgain(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_verify_micgain", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}

		static int micg;
		micg = (int)(params[0]);
		progStatus.mic_gain = micg;

		guard_lock serial_lock(&mutex_serial, "xml rig_set_verify_micgain");
		xml_trace(1, "rig_set_verify_micgain");
		selrig->set_mic_gain(progStatus.mic_gain);

		progStatus.mic_gain = selrig->get_mic_gain();

		Fl::awake(setMicGainControl, static_cast<void *>(0));
	}

	std::string help() { return std::string("sets & verifies micgain value"); }

} rig_set_verify_micgain(&rig_server);

//------------------------------------------------------------------------------
// Get volume value
//------------------------------------------------------------------------------
int  rig_xml_volume_val = 0;

class rig_get_volume : public XmlRpcServerMethod {
public:
	rig_get_volume(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_volume", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}

		if (progStatus.volume)
			result = (int)(progStatus.volume);
		else
			result = (int)0;
		xml_trace(1, "rig_get_volume");
	}

	std::string help() { return std::string("returns volume value"); }

} rig_get_volume(&rig_server);

//------------------------------------------------------------------------------
// Set volume value
//------------------------------------------------------------------------------

class rig_set_volume : public XmlRpcServerMethod {
public:
	rig_set_volume(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_volume", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}

		static int volume;
		volume = (int)(params[0]);
		progStatus.volume = volume;

		guard_lock serial_lock(&mutex_serial, "xml rig_set_volume");
		selrig->set_volume_control(progStatus.volume);
		xml_trace(1, "rig_set_volume");
		Fl::awake(setVolumeControl, static_cast<void *>(0));
	}

	std::string help() { return std::string("sets volume value"); }

} rig_set_volume(&rig_server);

class rig_set_verify_volume : public XmlRpcServerMethod {
public:
	rig_set_verify_volume(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_verify_volume", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}

		static int volume;
		volume = (int)(params[0]);
		progStatus.volume = volume;

		guard_lock serial_lock(&mutex_serial, "xml rig_set_verify_volume");
		selrig->set_volume_control(progStatus.volume);

		progStatus.volume = selrig->get_volume_control();

		xml_trace(1, "rig_set_verify_volume");
		Fl::awake(setVolumeControl, static_cast<void *>(0));
	}

	std::string help() { return std::string("sets & verifies volume value"); }

} rig_set_verify_volume(&rig_server);

class rig_mod_vol : public XmlRpcServerMethod {
public:
	rig_mod_vol(XmlRpcServer* s) : XmlRpcServerMethod("rig.mod_vol", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		int min, max, step;
		selrig->get_vol_min_max_step(min, max, step);

		int change = (int)(params[0]);
		progStatus.volume += change;
		if (progStatus.volume > max) progStatus.volume = max;
		if (progStatus.volume < min) progStatus.volume = min;

		guard_lock serial_lock(&mutex_serial, "xml rig_mod_volume");
		selrig->set_volume_control(progStatus.volume);
		xml_trace(1, "rig_mod_volume");
		Fl::awake(setVolumeControl, static_cast<void *>(0));
	}

	std::string help() { return std::string("modify volume +/- NNN"); }

} rig_mod_vol(&rig_server);

//------------------------------------------------------------------------------
// Request list of modes
//------------------------------------------------------------------------------

class rig_get_modes : public XmlRpcServerMethod {
public :
	rig_get_modes(XmlRpcServer *s) : XmlRpcServerMethod("rig.get_modes", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		XmlRpcValue modes;

		if (!xcvr_online) {
			modes[0] = "CW";
			modes[1] = "LSB";
			modes[2] = "USB";
			return;
		}

		wait();
		guard_lock service_lock(&mutex_srvc_reqs, "xml rig_get_modes");
		guard_lock serial_lock(&mutex_serial, "xml rig_get_modes");

		xml_trace(1, "rig_get_modes");
		try {
			int n = 0;
			for (size_t i = 0; i < selrig->modes_.size(); i++)
				if (!selrig->modes_.at(i).empty())
					modes[n++] = selrig->modes_.at(i);
			result = modes;
		} catch (const std::exception& e) {
			LOG_ERROR("%s", e.what());
			modes[0] = "CW";
			modes[1] = "LSB";
			modes[2] = "USB";
			return;
		}

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

		if (!xcvr_online || disable_xmlrpc->value()) {
			result = "U";
			return;
		}
		int mode;

		wait();
		guard_lock service_lock(&mutex_srvc_reqs, "xml rig_get_sideband");
		guard_lock serial_lock(&mutex_serial, "xml rig_get_sideband");

		mode = vfo->imode;

		std::string result_string = "";
		result_string += selrig->get_modetype(mode);
		xml_trace(2, "rig_get_sideband ", result_string.c_str());
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
		if (!xcvr_online) {
			result = "USB";
			return;
		}

		int mode;
		wait();
		try {
			guard_lock serial_lock(&mutex_serial, "xml rig_get_mode");

			mode = vfo->imode;

			std::string result_string = "none";
			result_string = selrig->modes_.at(mode);
			xml_trace(2, "mode on ", ((selrig->inuse == onB) ? "B " : "A "), result_string.c_str());
			result = result_string;
		} catch (const std::exception& e) {
			LOG_ERROR("%s", e.what());
			result = "USB";
		}
	}

	std::string help() { return std::string("returns current xcvr mode"); }

} rig_get_mode(&rig_server);

//------------------------------------------------------------------------------
// Request for vfo  A mode
//------------------------------------------------------------------------------

class rig_get_modeA : public XmlRpcServerMethod {
public:
	rig_get_modeA(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_modeA", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online) {
			result = "USB";
			return;
		}

		int mode;
		wait();
		try {
			guard_lock serial_lock(&mutex_serial, "xml rig_get_mode");

			mode = vfoA.imode;

			std::string result_string = "none";
			result_string = selrig->modes_.at(mode);
			xml_trace(2, "mode A ", result_string.c_str());
			result = result_string;
		} catch (const std::exception& e) {
			LOG_ERROR("%s", e.what());
			result = "USB";
		}

	}

	std::string help() { return std::string("returns vfo A mode"); }

} rig_get_modeA(&rig_server);

//------------------------------------------------------------------------------
// Request for vfo  B mode
//------------------------------------------------------------------------------

class rig_get_modeB : public XmlRpcServerMethod {
public:
	rig_get_modeB(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_modeB", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online) {
			result = "USB";
			return;
		}

		int mode;
		wait();
		try {
			guard_lock serial_lock(&mutex_serial, "xml rig_get_mode");

			mode = vfoB.imode;

			std::string result_string = "none";
			result_string = selrig->modes_.at(mode);
			xml_trace(2, "mode B ", result_string.c_str());
			result = result_string;
		} catch (const std::exception& e) {
			LOG_ERROR("%s", e.what());
			result = "USB";
		}

	}

	std::string help() { return std::string("returns vfo A mode"); }

} rig_get_modeB(&rig_server);

//------------------------------------------------------------------------------
// Request array of bandwidths
//------------------------------------------------------------------------------

class rig_get_bws : public XmlRpcServerMethod {
public :
	rig_get_bws(XmlRpcServer *s) : XmlRpcServerMethod("rig.get_bws", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {

		if (!xcvr_online) {
			XmlRpcValue bws;
			bws[0][0] = "Bandwidth";
			bws[0][1] = "NONE";
			result = bws;
			return;
		}
		XmlRpcValue bws;

		wait();
		guard_lock service_lock(&mutex_srvc_reqs, "xml rig_get_bws");
		guard_lock serial_lock(&mutex_serial, "xml rig_get_bws");

		try {
			int mode = (selrig->inuse == onB) ? vfoB.imode : vfoA.imode;
			std::vector<std::string>& bwt = selrig->bwtable(mode);
			std::vector<std::string>& dsplo = selrig->lotable(mode);
			std::vector<std::string>& dsphi = selrig->hitable(mode);

	// single bandwidth table
			if (!selrig->has_dsp_controls) {
				bws[0][0] = "Bandwidth";
				int n = 1;
				for (size_t i = 0; i < bwt.size(); i++) {
					if (!bwt.at(i).empty())
						bws[0][n++] = bwt.at(i);
				}
			}
	// double table either lo/hi or center/width
			if (selrig->has_dsp_controls) {
				int n = 1;
				std::string control_label = selrig->SL_label;
				control_label.append("|").append(selrig->SL_tooltip);
				bws[0][0] = control_label.c_str();
				for (size_t i = 0; i < dsplo.size(); i++) {
					if (!dsplo.at(i).empty())
						bws[0][n++] = dsplo[i];
				}
				control_label.assign(selrig->SH_label);
				control_label.append("|").append(selrig->SH_tooltip);
				bws[1][0] = control_label.c_str();
				n = 1;
				for (size_t i = 0; i < dsphi.size(); i++) {
					if (!dsphi.at(i).empty())
						bws[1][n++] = dsphi[i];
				}
			}
		} catch (const std::exception& e) {
			LOG_ERROR("%s", e.what());
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

		if (!xcvr_online || disable_xmlrpc->value()) return;

		wait();
		guard_lock service_lock(&mutex_srvc_reqs, "xml rig_get_bw");
		guard_lock serial_lock(&mutex_serial, "xml rig_get_bw");

		int BW = (selrig->inuse == onB) ? vfoB.iBW : vfoA.iBW;
		int mode = (selrig->inuse == onB) ? vfoB.imode : vfoA.imode;

		try {
			if (selrig->has_int_bandwidth_control) {
				char reply[10];
				snprintf(reply, sizeof(reply), "%d", BW);
				result[0] = reply;
				std::string s1 = result[0], s2 = result[1];
				xml_trace( 5, "bandwidth on ", ((selrig->inuse == onB) ? "B " : "A "), s1.c_str(), " | ", s2.c_str());
				return;
			}

			std::vector<std::string>& bwt = selrig->bwtable(mode);
			std::vector<std::string>& dsplo = selrig->lotable(mode);
			std::vector<std::string>& dsphi = selrig->hitable(mode);

			result[0] = result[1] = "";
			if (BW < 256) {
				result[0] = bwt[BW & 0x7F];
			}
			else {
				result[0] = dsplo.at(BW & 0x7F);
				result[1] = dsphi.at((BW >> 8) & 0x7F);
			}
		} catch (const std::exception& e) {
			LOG_ERROR("%s", e.what());
			result[0] = result[1] = "";
		}
		std::string s1 = result[0], s2 = result[1];
		xml_trace( 5, "bandwidth on ", ((selrig->inuse == onB) ? "B " : "A "), s1.c_str(), " | ", s2.c_str());
	}

	std::string help() { return std::string("returns current bw L/U value"); }

} rig_get_bw(&rig_server);

//------------------------------------------------------------------------------
// Request for vfoA bandwidth
//------------------------------------------------------------------------------

class rig_get_bwA : public XmlRpcServerMethod {
public:
	rig_get_bwA(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_bwA", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {

		result[0] = "NONE";
		result[1] = "";

		if (!xcvr_online || disable_xmlrpc->value()) return;

		wait();
		guard_lock service_lock(&mutex_srvc_reqs, "xml rig_get_bwA");
		guard_lock serial_lock(&mutex_serial, "xml rig_get_bwA");

		int BW = vfoA.iBW;
		int mode = vfoA.imode;

		try {
			if (selrig->has_int_bandwidth_control) {
				char reply[10];
				snprintf(reply, sizeof(reply), "%d", BW);
				result[0] = reply;
				std::string s1 = result[0], s2 = result[1];
				xml_trace( 4, "bandwidth on A", s1.c_str(), " | ", s2.c_str());
				return;
			}

			std::vector<std::string>& bwt = selrig->bwtable(mode);
			std::vector<std::string>& dsplo = selrig->lotable(mode);
			std::vector<std::string>& dsphi = selrig->hitable(mode);

			result[0] = result[1] = "";
			if (BW < 256) {
				result[0] = bwt.at(BW & 0x7F);
			}
			else {
				result[0] = dsplo.at(BW & 0x7F);
				result[1] = dsphi.at((BW >> 8) & 0x7F);
			}
		} catch (const std::exception& e) {
			LOG_ERROR("%s", e.what());
			result[0] = result[1] = "";
		}
		std::string s1 = result[0], s2 = result[1];
		xml_trace( 4, "bandwidth on A", s1.c_str(), " | ", s2.c_str());
	}

	std::string help() { return std::string("returns current bw L/U value"); }

} rig_get_bwA(&rig_server);


//------------------------------------------------------------------------------
// Request for vfoB bandwidth
//------------------------------------------------------------------------------

class rig_get_bwB : public XmlRpcServerMethod {
public:
	rig_get_bwB(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_bwB", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {

		result[0] = "NONE";
		result[1] = "";

		if (!xcvr_online || disable_xmlrpc->value()) return;

		wait();
		guard_lock service_lock(&mutex_srvc_reqs, "xml rig_get_bwB");
		guard_lock serial_lock(&mutex_serial, "xml rig_get_bwB");

		int BW = vfoB.iBW;
		int mode = vfoB.imode;
		result[0] = result[1] = "";

		try {
			
			if (selrig->has_int_bandwidth_control) {
					char reply[10];
					snprintf(reply, sizeof(reply), "%d", BW);
					result[0] = reply;
					std::string s1 = result[0], s2 = result[1];
					xml_trace( 4, "bandwidth on B", s1.c_str(), " | ", s2.c_str());
					return;
				}

			std::vector<std::string>& bwt = selrig->bwtable(mode);
			std::vector<std::string>& dsplo = selrig->lotable(mode);
			std::vector<std::string>& dsphi = selrig->hitable(mode);

			if (BW < 256) {
				result[0] = bwt.at(BW & 0x7F);
			}
			else {
				result[0] = dsplo.at(BW & 0x7F);
				result[1] = dsphi.at((BW >> 8) & 0x7F);
			}
		} catch (const std::exception& e) {
			LOG_ERROR("%s", e.what());
			result[0] = result[1] = "";
		}
		std::string s1 = result[0], s2 = result[1];
		xml_trace( 4, "bandwidth on B", s1.c_str(), " | ", s2.c_str());
	}

	std::string help() { return std::string("returns current bw L/U value"); }

} rig_get_bwB(&rig_server);

class rig_get_smeter : public XmlRpcServerMethod {
public:
	rig_get_smeter(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_smeter", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value() || !selrig->has_smeter)
			result = "0";
		else {
			guard_lock serial_lock(&mutex_serial);
			int val = selrig->get_smeter();

			char szMeter[20];
			snprintf(szMeter, sizeof(szMeter), "%d", val);
			std::string result_string = szMeter;
			result = result_string;
		}
	}

	std::string help() { return std::string("returns S-meter reading"); }

} rig_get_smeter(&rig_server);

class rig_get_DBM : public XmlRpcServerMethod {
public:
	rig_get_DBM(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_DBM", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value() || !selrig->has_smeter)
			result = "0";
		else {
			guard_lock serial_lock(&mutex_serial);
			int val = selrig->get_smeter();
			if (val > 50) val = round(-73.0 + (val - 50.0) * 6.0 / 5.0);
			else          val = round(-127.0 + val * 54.0 / 50.0);

			char szMeter[20];
			snprintf(szMeter, sizeof(szMeter), "%d", val);
			std::string result_string = szMeter;
			result = result_string;
		}
	}

	std::string help() { return std::string("returns S-meter in dBm"); }

} rig_get_DBM(&rig_server);

class rig_get_Sunits : public XmlRpcServerMethod {
public:
	rig_get_Sunits(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_Sunits", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value() || !selrig->has_smeter)
			result = "0";
		else {
			guard_lock serial_lock(&mutex_serial);
			int val = selrig->get_smeter();

			char szMeter[20];
			if (val > 50) {
				val = round((val - 50.0) * 6.0 / 5.0);
				snprintf(szMeter, sizeof(szMeter), "S9 + %ddB", val);
			} else {
				val = round(val * 9.0 / 50.0);
				snprintf(szMeter, sizeof(szMeter), "S %d", val);
			}
			std::string result_string = szMeter;
			result = result_string;
		}
	}

	std::string help() { return std::string("returns S-meter in S units 0...9...+60"); }

} rig_get_Sunits(&rig_server);

class rig_get_pwrmeter_scale : public XmlRpcServerMethod {
public:
	rig_get_pwrmeter_scale(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_pwrmeter_scale", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value() || !selrig->has_power_out)
			result = (int)(0);
		else {
			guard_lock serial_lock(&mutex_serial);
			result = (int)(selrig->power_scale());
		}
	}

	std::string help() { return std::string("returns power scale X factor"); }

} rig_get_pwrmeter_scale(&rig_server);

class rig_get_maxpwr : public XmlRpcServerMethod {
public:
	rig_get_maxpwr(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_maxpwr", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		result = (int)(selrig->power_max());
	}
} rig_get_maxpwr(&rig_server);


class rig_get_pwrmeter : public XmlRpcServerMethod {
public:
	rig_get_pwrmeter(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_pwrmeter", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value() || !selrig->has_power_out)
			result = "0";
		else {
			guard_lock serial_lock(&mutex_serial);
			int val = selrig->get_power_out();
			char szmeter[20];
			snprintf(szmeter, sizeof(szmeter), "%d", val);
			std::string result_string = szmeter;
			result = result_string;

		}
	}

	std::string help() { return std::string("returns power meter reading"); }

} rig_get_pwrmeter(&rig_server);

class rig_get_swrmeter : public XmlRpcServerMethod {
public:
	rig_get_swrmeter(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_swrmeter", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value() || !selrig->has_swr_control)
			result = "0";
		else {
			guard_lock serial_lock(&mutex_serial);
			int val = selrig->get_swr();
			char szmeter[20];
			snprintf(szmeter, sizeof(szmeter), "%d", val);
			std::string result_string = szmeter;
			result = result_string;
		}
	}

	std::string help() { return std::string("returns SWR meter reading"); }

} rig_get_swrmeter(&rig_server);

//==============================================================================
// set interface
//==============================================================================
#include <queue>
#include "rigbase.h"

extern std::queue<VFOQUEUE> srvc_reqs;
extern std::queue<bool> quePTT;

extern XCVR_STATE vfoA;
extern XCVR_STATE vfoB;

std::string print(unsigned long long f, int m, int b)
{
	static std::ostringstream p;
	p.seekp(std::ios::beg);
	if (b > 65536) b /= 65536;
	p << "freq: " << f << ", imode: " << m << ", bw " << b;
	return p.str();
}

//------------------------------------------------------------------------------
// Set Power in watts
//------------------------------------------------------------------------------

class rig_set_power : public XmlRpcServerMethod {
public:
	rig_set_power(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_power", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		int power_level = (int)(params[0]);

		guard_lock lock(&mutex_serial);
		selrig->set_power_control(power_level);
		progStatus.power_level = power_level;
		Fl::awake(update_power_control, (void*)0);
	}

	std::string help() { return std::string("sets power level in watts"); }

} rig_set_power(&rig_server);

class rig_set_verify_power : public XmlRpcServerMethod {
public:
	rig_set_verify_power(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_verify_power", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		int power_level = (int)(params[0]);

		guard_lock lock(&mutex_serial);
		selrig->set_power_control(power_level);

		progStatus.power_level = selrig->get_power_control();

		Fl::awake(update_power_control, (void*)0);
	}

	std::string help() { return std::string("sets & verifies power level in watts"); }

} rig_set_verify_power(&rig_server);

class rig_get_power : public XmlRpcServerMethod {
public:
	rig_get_power(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_power", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		result = (int)(progStatus.power_level);
	}

	std::string help() { return std::string("returns power level setting in watts"); }

} rig_get_power(&rig_server);

class rig_mod_power : public XmlRpcServerMethod {
public:
	rig_mod_power(XmlRpcServer* s) : XmlRpcServerMethod("rig.mod_pwr", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		double min, max, step;
		selrig->get_pc_min_max_step(min, max, step);

		int power_change = (int)(params[0]);

		guard_lock lock(&mutex_serial);
		progStatus.power_level += power_change;
		if (progStatus.power_level > max) progStatus.power_level = max;
		if (progStatus.power_level < min) progStatus.power_level = min;

		selrig->set_power_control(progStatus.power_level);
		Fl::awake(update_power_control, (void*)0);
	}

	std::string help() { return std::string("mods power level in watts"); }

} rig_mod_power(&rig_server);

//------------------------------------------------------------------------------
// Enable tune
//------------------------------------------------------------------------------
class rig_tune : public XmlRpcServerMethod {
public:
	rig_tune(XmlRpcServer* s) : XmlRpcServerMethod("rig.tune", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		guard_lock que_lock(&mutex_srvc_reqs, "xml rig_tune");
		guard_lock serial_lock(&mutex_serial);
		selrig->tune_rig(2);
	}

	std::string help() { return std::string("enable transceiver tune function"); }

} rig_tune(&rig_server);

//------------------------------------------------------------------------------
// Set PTT on (1) or off (0)
//------------------------------------------------------------------------------

extern bool PTT;

class rig_set_verify_ptt : public XmlRpcServerMethod {
public:
	rig_set_verify_ptt(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_verify_ptt", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}

		guard_lock ser_lock (&mutex_serial);

		PTT = int(params[0]);
		xml_trace(1, (PTT ? "rig_set_verify_ptt ON" : "rig_set_verify_ptt OFF"));
		rigPTT(PTT);
		{
			bool get = ptt_state();
			int cnt = 0;
			while ((get != PTT) && (cnt++ < 100)) {
				MilliSleep(10);
				get = ptt_state();
			}
			PTT = get;
			std::stringstream s;
			s << "ptt returned " << get << " in " << cnt * 10 << " msec";
			xml_trace(1, s.str().c_str());
			Fl::awake(update_UI_PTT);
		}
	}

	std::string help() { return std::string("sets & verifies PTT on (1) or off (0)"); }

} rig_set_verify_ptt(&rig_server);

bool ptt_pending = false;

class rig_set_ptt : public XmlRpcServerMethod {
public:
	rig_set_ptt(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_ptt", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}

		guard_lock ser_lock (&mutex_serial);

		PTT = int(params[0]);
		xml_trace(1, (PTT ? "rig_ptt ON" : "rig_ptt OFF"));

		rigPTT(PTT);
		{
			bool get = ptt_state();
			int cnt = 0;
			while ((get != PTT) && (cnt++ < 100)) {
				MilliSleep(10);
				get = ptt_state();
			}
			PTT = get;
			std::stringstream s;
			s << "ptt returned " << get << " in " << cnt * 10 << " msec";
			xml_trace(1, s.str().c_str());
			Fl::awake(update_UI_PTT);
		}
	}

	std::string help() { return std::string("sets PTT on (1) or off (0)"); }

} rig_set_ptt(&rig_server);

class rig_set_ptt_fast : public XmlRpcServerMethod {
public:
	rig_set_ptt_fast(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_ptt_fast", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}

		guard_lock ser_lock (&mutex_serial);

		PTT = int(params[0]);
		xml_trace(1, (PTT ? "rig_ptt ON" : "rig_ptt OFF"));
		rigPTT(PTT);
		Fl::awake(update_UI_PTT);
	}

	std::string help() { return std::string("deprecated; use rig.set_ptt"); }

} rig_set_ptt_fast(&rig_server);

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
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		guard_lock lock(&mutex_srvc_reqs, "xml rig_swap");
		guard_lock serial_lock(&mutex_serial, "xml rig_swap");

		VFOQUEUE xcvr;
		xcvr.change = SWAP;
		xml_trace(1, "xmlrpc SWAP");
		srvc_reqs.push(xcvr);
	}

	std::string help() { return std::string("executes vfo swap"); }

} rig_swap(&rig_server);

// deprecated method - retain for backward compatibility

class rig_set_swap : public XmlRpcServerMethod {
public:
	rig_set_swap(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_swap", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}

		guard_lock lock(&mutex_srvc_reqs, "xml rig_set_swap");
		guard_lock serial_lock(&mutex_serial, "xml rig_set_swap");

		VFOQUEUE xcvr;
		xcvr.change = SWAP;
		xml_trace(1, "xmlrpc SWAP");
		srvc_reqs.push(xcvr);

	}

	std::string help() { return std::string("executes vfo swap"); }

} rig_set_swap(&rig_server);

class rig_set_verify_swap : public XmlRpcServerMethod {
public:
	rig_set_verify_swap(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_verify_swap", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}

		guard_lock lock(&mutex_srvc_reqs, "xml rig_set_verify_swap");
		guard_lock serial_lock(&mutex_serial, "xml rig_set_verify_swap");

		VFOQUEUE xcvr;
		xcvr.change = SWAP;
		xml_trace(1, "xmlrpc SWAP");
		srvc_reqs.push(xcvr);

	}

	std::string help() { return std::string("executes vfo swap"); }

} rig_set_verify_swap(&rig_server);

//------------------------------------------------------------------------------
// Execute vfo split operation
//------------------------------------------------------------------------------

class rig_set_split : public XmlRpcServerMethod {
public:
	rig_set_split(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_split", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		int state = int(params[0]);
		{
			guard_lock lock(&mutex_srvc_reqs, "xml rig_set_verify_split");
			guard_lock serial_lock(&mutex_serial, "xml rig_set_verify_split");

			VFOQUEUE xcvr_split;
			if (state) xcvr_split.change = sON;
			else       xcvr_split.change = sOFF;
			xml_trace(1, (state ? "rig_set_verify_split ON" : "rig_set_verify_split OFF"));
			srvc_reqs.push(xcvr_split);
		}
		for (int i = 0; i < 50; i++) {
			if (progStatus.split == state) return;
			MilliSleep(10);
		}
	}

	std::string help() { return std::string("executes vfo split"); }

} rig_set_split(&rig_server);

class rig_set_verify_split : public XmlRpcServerMethod {
public:
	rig_set_verify_split(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_verify_split", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		int state = int(params[0]);
		{
			guard_lock lock(&mutex_srvc_reqs, "xml rig_set_verify_split");
			guard_lock serial_lock(&mutex_serial, "xml rig_set_verify_split");

			VFOQUEUE xcvr_split;
			if (state) xcvr_split.change = sON;
			else       xcvr_split.change = sOFF;
			xml_trace(1, (state ? "rig_set_verify_split ON" : "rig_set_verify_split OFF"));
			srvc_reqs.push(xcvr_split);
		}
		for (int i = 0; i < 50; i++) {
			if (progStatus.split == state) return;
			MilliSleep(10);
		}
	}

	std::string help() { return std::string("sets & verifies vfo split"); }

} rig_set_verify_split(&rig_server);

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
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		std::string ans = std::string(params[0]);
		if (!(ans == "A" || ans == "B")) {
			return;
		}

		if (!ptt_off()) {
			trace(1, "!ptt_off()");
			return;
		}

		guard_lock service_lock(&mutex_srvc_reqs, "xml rig_set_AB");
		guard_lock serial_lock(&mutex_serial, "xml rig_set_AB");

		XCVR_STATE vfo = vfoA;

		vfo.src = SRVR;


xml_trace(4, "set_AB ", ans.c_str(), " ", printXCVR_STATE(vfo).c_str());
		srvc_reqs.push (VFOQUEUE((ans == "A" ? sA : sB), vfo));
	}

	std::string help() { return std::string("sets & verifies vfo in use A or B"); }

} rig_set_AB(&rig_server);

class rig_set_verify_AB : public XmlRpcServerMethod {
public:
	rig_set_verify_AB(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_verify_AB", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		std::string ans = std::string(params[0]);
		if (!(ans == "A" || ans == "B")) {
			return;
		}

		if (!ptt_off()) {
			trace(1, "!ptt_off()");
			return;
		}

		guard_lock service_lock(&mutex_srvc_reqs, "xml rig_set_verify_AB");
		guard_lock serial_lock(&mutex_serial, "xml rig_set_verify_AB");

		XCVR_STATE vfo = vfoA;

		vfo.src = SRVR;


xml_trace(4, "set verify AB ", ans.c_str(), " ", printXCVR_STATE(vfo).c_str());
		srvc_reqs.push (VFOQUEUE((ans == "A" ? sA : sB), vfo));
	}

	std::string help() { return std::string("sets & verifies vfo in use A or B"); }

} rig_set_verify_AB(&rig_server);

//------------------------------------------------------------------------------
// Set vfoA frequency
//------------------------------------------------------------------------------

class rig_set_vfoA : public XmlRpcServerMethod {
public:
	rig_set_vfoA(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_vfoA", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		unsigned long long freq = static_cast<unsigned long long>((double)params[0]);

		guard_lock serial(&mutex_serial);

		if (!selrig->can_change_alt_vfo  && (selrig->inuse == onB)) {
			selrig->selectA();
			vfoA.freq = freq;
			selrig->set_vfoA(vfoA.freq);
			selrig->selectB();
		} else {
			vfoA.freq = freq;
			selrig->set_vfoA(vfoA.freq);
		}

		Fl::awake(setFreqDispA);

	}
	std::string help() { return std::string("rig.set_vfo NNNNNNNN (Hz)"); }

} rig_set_vfoA(&rig_server);

class rig_set_verify_vfoA : public XmlRpcServerMethod {
public:
	rig_set_verify_vfoA(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_verify_vfoA", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		unsigned long long freq = static_cast<unsigned long long>(double(params[0]));

		guard_lock serial(&mutex_serial);

		if (!selrig->can_change_alt_vfo  && (selrig->inuse == onB)) {
			selrig->selectA();
			selrig->set_vfoA(freq);
			vfoA.freq = selrig->get_vfoA();
			selrig->selectB();
		} else {
			selrig->set_vfoA(freq);
			vfoA.freq = selrig->get_vfoA();
		}

		Fl::awake(setFreqDispA);
	}
	std::string help() { return std::string("rig.set_verify_vfo NNNNNNNN (Hz)"); }

} rig_set_verify_vfoA(&rig_server);

class rig_set_vfoA_fast : public XmlRpcServerMethod {
public:
	rig_set_vfoA_fast(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_vfoA_fast", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		unsigned long long freq = static_cast<unsigned long long>(double(params[0]));

		guard_lock serial(&mutex_serial);

		if (!selrig->can_change_alt_vfo  && (selrig->inuse == onB)) {
			selrig->selectA();
			vfoA.freq = freq;
			selrig->set_vfoA(vfoA.freq);
			selrig->selectB();
		} else {
			vfoA.freq = freq;
			selrig->set_vfoA(vfoA.freq);
		}

		Fl::awake(setFreqDispA);
	}
	std::string help() { return std::string("deprecated; use rig.set_vfoA"); }

} rig_set_vfoA_fast(&rig_server);

class rig_mod_vfoA : public XmlRpcServerMethod {
public:
	rig_mod_vfoA(XmlRpcServer* s) : XmlRpcServerMethod("rig.mod_vfoA", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		signed long int freq = static_cast<signed long int>((double)params[0]);

		guard_lock serial(&mutex_serial);

		if (!selrig->can_change_alt_vfo  && (selrig->inuse == onB)) {
			selrig->selectA();
			vfoA.freq += freq;
			selrig->set_vfoA(vfoA.freq);
			selrig->selectB();
		} else {
			vfoA.freq += freq;
			selrig->set_vfoA(vfoA.freq);
		}

		Fl::awake(setFreqDispA);

	}
	std::string help() { return std::string("rig.mod_vfo +/- NNN (Hz)"); }

} rig_mod_vfoA(&rig_server);

//------------------------------------------------------------------------------
// Set vfo B frequency
//------------------------------------------------------------------------------

class rig_set_vfoB : public XmlRpcServerMethod {
public:
	rig_set_vfoB(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_vfoB", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		unsigned long long freq = static_cast<unsigned long long>(double(params[0]));

		guard_lock serial(&mutex_serial);

		if (!selrig->can_change_alt_vfo  && (selrig->inuse == onA)) {
			selrig->selectB();
			selrig->set_vfoB(freq);
			vfoB.freq = freq;
			selrig->selectA();
		} else {
			selrig->set_vfoB(freq);
			vfoB.freq = freq;
		}

		Fl::awake(setFreqDispB);
	}
	std::string help() { return std::string("rig.set_vfo NNNNNNNN (Hz)"); }

} rig_set_vfoB(&rig_server);

class rig_set_verify_vfoB : public XmlRpcServerMethod {
public:
	rig_set_verify_vfoB(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_verify_vfoB", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		unsigned long long freq = static_cast<unsigned long long>(double(params[0]));

		guard_lock serial(&mutex_serial);

		if (!selrig->can_change_alt_vfo  && (selrig->inuse == onA)) {
			selrig->selectB();
			selrig->set_vfoB(freq);
			vfoB.freq = selrig->get_vfoB();
			selrig->selectA();
		} else {
			selrig->set_vfoB(freq);
			vfoB.freq = selrig->get_vfoB();
		}

		Fl::awake(setFreqDispB);
	}
	std::string help() { return std::string("rig.set_verify_vfo NNNNNNNN (Hz)"); }

} rig_set_verify_vfoB(&rig_server);

class rig_set_vfoB_fast : public XmlRpcServerMethod {
public:
	rig_set_vfoB_fast(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_vfoB_fast", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		unsigned long long freq = static_cast<unsigned long long>(double(params[0]));

		guard_lock serial(&mutex_serial);

		if (!selrig->can_change_alt_vfo  && (selrig->inuse == onA)) {
			selrig->selectB();
			selrig->set_vfoB(freq);
			vfoB.freq = freq;
			selrig->selectA();
		} else {
			selrig->set_vfoB(freq);
			vfoB.freq = freq;
		}

		Fl::awake(setFreqDispB);
	}
	std::string help() { return std::string("deprecated; use rig.set_vfoB"); }

} rig_set_vfoB_fast(&rig_server);

class rig_mod_vfoB : public XmlRpcServerMethod {
public:
	rig_mod_vfoB(XmlRpcServer* s) : XmlRpcServerMethod("rig.mod_vfoB", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		signed long int freq = static_cast<signed long int>((double)params[0]);

		guard_lock serial(&mutex_serial);

		if (!selrig->can_change_alt_vfo  && (selrig->inuse == onA)) {
			selrig->selectB();
			selrig->set_vfoB(vfoB.freq);
			vfoB.freq += freq;
			selrig->selectA();
		} else {
			selrig->set_vfoB(vfoB.freq);
			vfoB.freq += freq;
		}

		Fl::awake(setFreqDispB);

	}
	std::string help() { return std::string("rig.mod_vfoB +/- NNN (Hz)"); }

} rig_mod_vfoB(&rig_server);

//------------------------------------------------------------------------------
// Set vfoB to vfoA freq/mode
//------------------------------------------------------------------------------

class rig_vfoA2B : public XmlRpcServerMethod {
public:
	rig_vfoA2B (XmlRpcServer* s) : XmlRpcServerMethod("rig.vfoA2B", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}

		guard_lock serial(&mutex_serial);

		vfoB.freq = vfoA.freq;
		vfoB.imode = vfoA.imode;

		selrig->set_vfoB(vfoB.freq);
		selrig->set_modeB(vfoB.imode);

		Fl::awake(setFreqDispB);

	}
	std::string help() { return std::string("sets vfoB to vfoA freq/mode"); }

} rig_vfoA2B(&rig_server);

//------------------------------------------------------------------------------
// Set freq B to freq A
//------------------------------------------------------------------------------

class rig_freqA2B : public XmlRpcServerMethod {
public:
	rig_freqA2B (XmlRpcServer* s) : XmlRpcServerMethod("rig.freqA2B", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}

		guard_lock serial(&mutex_serial);

		vfoB.freq = vfoA.freq;

		selrig->set_vfoB(vfoB.freq);

		Fl::awake(setFreqDispB);

	}
	std::string help() { return std::string("sets freqB to freqA"); }

} rig_freqA2B(&rig_server);

//------------------------------------------------------------------------------
// Set mode A to mode B
//------------------------------------------------------------------------------

class rig_modeA2B : public XmlRpcServerMethod {
public:
	rig_modeA2B (XmlRpcServer* s) : XmlRpcServerMethod("rig.modeA2B", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}

		guard_lock serial(&mutex_serial);

		vfoB.imode = vfoA.imode;

		selrig->set_modeB(vfoB.imode);

	}
	std::string help() { return std::string("sets modeA to modeB"); }

} rig_modeA2B(&rig_server);

//------------------------------------------------------------------------------
// Set active vfo frequency
//------------------------------------------------------------------------------

class rig_set_vfo : public XmlRpcServerMethod {
public:
	rig_set_vfo(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_vfo", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		unsigned long long freq = static_cast<unsigned long long>(double(params[0]));

		guard_lock serial(&mutex_serial);
		if (selrig->inuse == onB) {
			selrig->set_vfoB(freq);
			vfoB.freq = freq;
			Fl::awake(setFreqDispB);
		}else {
			selrig->set_vfoA(freq);
			vfoA.freq = freq;
			Fl::awake(setFreqDispA);
		}

	}
	std::string help() { return std::string("rig.set_vfo NNNNNNNN (Hz)"); }

} rig_set_vfo(&rig_server);

class rig_set_verify_vfo : public XmlRpcServerMethod {
public:
	rig_set_verify_vfo(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_verify_vfo", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		unsigned long long freq = static_cast<unsigned long long>(double(params[0]));

		guard_lock serial(&mutex_serial);
		if (selrig->inuse == onB) {
			selrig->set_vfoB(freq);
			vfoB.freq = selrig->get_vfoB();
			Fl::awake(setFreqDispB);
		}else {
			selrig->set_vfoA(freq);
			vfoA.freq = selrig->get_vfoA();
			Fl::awake(setFreqDispA);
		}

	}
	std::string help() { return std::string("rig.set_verify_vfo NNNNNNNN (Hz)"); }

} rig_set_verify_vfo(&rig_server);


class main_set_frequency : public XmlRpcServerMethod {
public:
	main_set_frequency(XmlRpcServer* s) : XmlRpcServerMethod("main.set_frequency", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		unsigned long long freq = static_cast<unsigned long long>(double(params[0]));

		guard_lock serial(&mutex_serial);
		if (selrig->inuse == onB) {
			selrig->set_vfoB(freq);
			vfoB.freq = freq;
			Fl::awake(setFreqDispB);
		}else {
			selrig->set_vfoA(freq);
			vfoA.freq = freq;
			Fl::awake(setFreqDispA);
		}

	}
	std::string help() { return std::string("main.set_frequency NNNNNNNN (Hz)"); }

} main_set_frequency(&rig_server);

class rig_set_frequency : public XmlRpcServerMethod {
public:
	rig_set_frequency(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_frequency", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		unsigned long long freq = static_cast<unsigned long long>(double(params[0]));

		guard_lock serial(&mutex_serial);
		if (selrig->inuse == onB) {
			selrig->set_vfoB(freq);
			vfoB.freq = freq;
			Fl::awake(setFreqDispB);
		}else {
			selrig->set_vfoA(freq);
			vfoA.freq = freq;
			Fl::awake(setFreqDispA);
		}

		result = 1;
	}
	std::string help() { return std::string("rig.set_frequency NNNNNNNN (Hz)"); }

} rig_set_frequency(&rig_server);

class rig_set_verify_frequency : public XmlRpcServerMethod {
public:
	rig_set_verify_frequency(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_verify_frequency", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		unsigned long long freq = static_cast<unsigned long long>(double(params[0]));

		guard_lock serial(&mutex_serial);
		if (selrig->inuse == onB) {
			selrig->set_vfoB(freq);
			vfoB.freq = selrig->get_vfoB();
			Fl::awake(setFreqDispB);
		}else {
			selrig->set_vfoA(freq);
			vfoA.freq = selrig->get_vfoA();
			Fl::awake(setFreqDispA);
		}

		result = 1;
	}
	std::string help() { return std::string("rig.set_verify_frequency NNNNNNNN (Hz)"); }

} rig_set_verify_frequency(&rig_server);

//------------------------------------------------------------------------------
// Set mode on current vfo
//------------------------------------------------------------------------------

class rig_set_mode : public XmlRpcServerMethod {
public:
	rig_set_mode(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_mode", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue &result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}

		XCVR_STATE nuvals;
		nuvals.freq = 0;
		nuvals.imode = -1;
		nuvals.iBW = 255;

		std::string numode = (std::string)params[0];

		try {
			for (size_t i = 0; i < selrig->modes_.size(); i++) {
				if (numode == selrig->modes_.at(i))  {
					nuvals.imode = i;
					nuvals.iBW = selrig->def_bandwidth(i);
					guard_lock serial_lock(&mutex_serial);
					if (selrig->inuse == onB) {
						serviceB(nuvals);
					} else {
						serviceA(nuvals);
					}
					Fl::awake(updateUI);
					result = 1;
					break;
				}
			}
		} catch (const std::exception& e) {
			LOG_ERROR("%s", e.what());
		}

	}

	std::string help() { return std::string("set_mode MODE_NAME"); }

} rig_set_mode(&rig_server);

class rig_set_verify_mode : public XmlRpcServerMethod {
public:
	rig_set_verify_mode(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_verify_mode", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue &result) {
		result = 0;
		if (!xcvr_online || disable_xmlrpc->value()) {
			return;
		}

		XCVR_STATE nuvals;
		nuvals.freq = 0;
		nuvals.imode = -1;
		nuvals.iBW = 255;

		std::string numode = (std::string)params[0];

		try {
			for (size_t i = 0; i < selrig->modes_.size(); i++) {
				if (numode == selrig->modes_.at(i))  {
					nuvals.imode = i;
					nuvals.iBW = selrig->def_bandwidth(i);
					guard_lock serial_lock(&mutex_serial);
					if (selrig->inuse == onB) {
						serviceB(nuvals);
						result = (i == (size_t)selrig->get_modeB());
					} else {
						serviceA(nuvals);
						result = (i == (size_t)selrig->get_modeA());
					}
					Fl::awake(updateUI);
					break;
				}
			}
		} catch (const std::exception& e) {
			LOG_ERROR("%s", e.what());
		}

		return;

	}

	std::string help() { return std::string("set_mode MODE_NAME"); }

} rig_set_verify_mode(&rig_server);


//------------------------------------------------------------------------------
// Set mode on vfo A
//------------------------------------------------------------------------------

class rig_set_modeA : public XmlRpcServerMethod {
public:
	rig_set_modeA(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_modeA", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue &result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}

		XCVR_STATE nuvals;
		nuvals.freq = 0;
		nuvals.imode = -1;
		nuvals.iBW = 255;

		std::string numode = (std::string)params[0];

		try {
			for (size_t i = 0; i < selrig->modes_.size(); i++) {
				if (numode == selrig->modes_.at(i))  {
					nuvals.imode = i;
					nuvals.iBW = selrig->def_bandwidth(i);
					guard_lock serial_lock(&mutex_serial);
					serviceB(nuvals);
					Fl::awake(updateUI);
					result = 1;
					break;
				}
			}
		} catch (const std::exception& e) {
			LOG_ERROR("%s", e.what());
		}

	}

	std::string help() { return std::string("set_mode on vfo A"); }

} rig_set_modeA(&rig_server);

class rig_set_verify_modeA : public XmlRpcServerMethod {
public:
	rig_set_verify_modeA(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_verify_modeA", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue &result) {
		result = 0;
		if (!xcvr_online || disable_xmlrpc->value()) {
			return;
		}

		XCVR_STATE nuvals;
		nuvals.freq = 0;
		nuvals.imode = -1;
		nuvals.iBW = 255;

		std::string numode = (std::string)params[0];

		try {
			for (size_t i = 0; i < selrig->modes_.size(); i++) {
				if (numode == selrig->modes_.at(i))  {
					nuvals.imode = i;
					nuvals.iBW = selrig->def_bandwidth(i);
					guard_lock serial_lock(&mutex_serial);
					serviceB(nuvals);
					Fl::awake(updateUI);
					result = (nuvals.imode == selrig->get_modeA());
					break;
				}
			}
		} catch (const std::exception& e) {
			LOG_ERROR("%s", e.what());
		}

	}
	std::string help() { return std::string("set_mode on vfo A"); }

} rig_set_verify_modeA(&rig_server);


//------------------------------------------------------------------------------
// Set mode on vfo B
//------------------------------------------------------------------------------

class rig_set_modeB : public XmlRpcServerMethod {
public:
	rig_set_modeB(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_modeB", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue &result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}

		XCVR_STATE nuvals;
		nuvals.freq = 0;
		nuvals.imode = -1;
		nuvals.iBW = 255;

		std::string numode = (std::string)params[0];

		try {
			for (size_t i = 0; i < selrig->modes_.size(); i++) {
				if (numode == selrig->modes_.at(i))  {
					nuvals.imode = i;
					nuvals.iBW = selrig->def_bandwidth(i);
					guard_lock serial_lock(&mutex_serial);
					serviceB(nuvals);
					Fl::awake(updateUI);
					result = 1;
					break;
				}
			}
		} catch (const std::exception& e) {
			LOG_ERROR("%s", e.what());
		}

	}
	std::string help() { return std::string("set_mode on vfo B"); }

} rig_set_modeB(&rig_server);

class rig_set_verify_modeB : public XmlRpcServerMethod {
public:
	rig_set_verify_modeB(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_verify_modeB", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue &result) {
		result= 0;
		if (!xcvr_online || disable_xmlrpc->value()) {
			return;
		}

		XCVR_STATE nuvals;
		nuvals.freq = 0;
		nuvals.imode = -1;
		nuvals.iBW = 255;

		std::string numode = (std::string)params[0];

		try {
			for (size_t i = 0; i < selrig->modes_.size(); i++) {
				if (numode == selrig->modes_.at(i))  {
					nuvals.imode = i;
					nuvals.iBW = selrig->def_bandwidth(i);
					guard_lock serial_lock(&mutex_serial);
					serviceB(nuvals);
					Fl::awake(updateUI);
					result = (nuvals.imode == selrig->get_modeB());
					break;
				}
			}
		} catch (const std::exception& e) {
			LOG_ERROR("%s", e.what());
		}

	}
	std::string help() { return std::string("set_mode on vfo B"); }

} rig_set_verify_modeB(&rig_server);

//------------------------------------------------------------------------------
// Set bandwidth
//------------------------------------------------------------------------------

class rig_set_bandwidth : public XmlRpcServerMethod {
public:
	rig_set_bandwidth(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_bandwidth", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		int bw = (int)params[0];

		guard_lock lock(&mutex_srvc_reqs, "xml rig_set_bandwidth");
		guard_lock serial_lock(&mutex_serial, "xml rig_set_bandwidth");

		XCVR_STATE nuvals;

		if (selrig->has_int_bandwidth_control) {

			int bwmod = bw % 10;
			bw /= 10;
			if (bwmod >= 5) ++bw;
			bw *= 10;
			if (bw < 50) bw = 50;
			if (bw > 4000) bw = 4000;
			nuvals.iBW = bw;

		} else {
			try {
				size_t i = 0;
				for (i = 0; i < selrig->bandwidths_.size(); i++)
					if (atol (selrig->bandwidths_.at(i).c_str()) > bw) break;
				if (--i < 0) i = 0;
				bw = atol(selrig->bandwidths_.at(i).c_str());
				nuvals.iBW = i;
				std::ostringstream s;
				s << "nearest bandwidth " << selrig->bandwidths_[i];
				xml_trace(2,"Set to ", s.str().c_str());
			} catch (const std::exception& e) {
				LOG_ERROR("%s", e.what());
			}
		}
		if (selrig->inuse == onB) {
			nuvals.freq = vfoB.freq;
			nuvals.imode = vfoB.imode;
			serviceB(nuvals);
		} else {
			nuvals.freq = vfoA.freq;
			nuvals.imode = vfoA.imode;
			serviceA(nuvals);
		}

	}
	std::string help() { return std::string("set_bw to nearest requested"); }

} rig_set_bandwidth(&rig_server);

class rig_set_verify_bandwidth : public XmlRpcServerMethod {
public:
	rig_set_verify_bandwidth(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_verify_bandwidth", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		int bw = int(params[0]);

		guard_lock lock(&mutex_srvc_reqs, "xml rig_set_verify_bandwidth");
		guard_lock serial_lock(&mutex_serial, "xml rig_set_verify_bandwidth");

		XCVR_STATE nuvals;

		if (selrig->has_int_bandwidth_control) {

			int bwmod = bw % 10;
			bw /= 10;
			if (bwmod >= 5) ++bw;
			bw *= 10;
			if (bw < 50) bw = 50;
			if (bw > 4000) bw = 4000;
			nuvals.iBW = bw;

		} else {
			try {
				size_t i = 0;
				for (i = 0; i < selrig->bandwidths_.size(); i++)
					if (atol (selrig->bandwidths_.at(i).c_str()) > bw) break;
				if (--i < 0) i = 0;
				bw = atol(selrig->bandwidths_.at(i).c_str());
				nuvals.iBW = i;
				std::ostringstream s;
				s << "nearest bandwidth " << selrig->bandwidths_.at(i);
				xml_trace(2,"Set to ", s.str().c_str());
			} catch (const std::exception& e) {
				LOG_ERROR("%s", e.what());
			}

		}
		int retbw;
		if (selrig->inuse == onB) {
			nuvals.freq = vfoB.freq;
			nuvals.imode = vfoB.imode;
			serviceB(nuvals);
			retbw = selrig->get_bwB();
		} else {
			nuvals.freq = vfoA.freq;
			nuvals.imode = vfoA.imode;
			serviceA(nuvals);
			retbw = selrig->get_bwA();
		}
		result = retbw;
	}

	std::string help() { return std::string("set_bw to nearest requested"); }

} rig_set_verify_bandwidth(&rig_server);


class rig_set_bw : public XmlRpcServerMethod {
public:
	rig_set_bw(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_bw", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		int bw = (int)params[0];
		std::vector<std::string>& widths = selrig->bandwidths_;
		if (bw < 0 || bw >= (int) widths.size()) {
			result = 0;
			return;
		}

		guard_lock que_lock ( &mutex_srvc_reqs, "xml rig_set_bw" );
		guard_lock serial_lock(&mutex_serial, "xml rig_get_bw");

		XCVR_STATE nuvals;
		nuvals.iBW = bw;
		if (selrig->inuse == onB) {
			nuvals.freq = vfoB.freq;
			nuvals.imode = vfoB.imode;
			serviceB(nuvals);
		} else {
			nuvals.freq = vfoA.freq;
			nuvals.imode = vfoA.imode;
			serviceA(nuvals);
		}

	}
	std::string help() { return std::string("set_bw to VAL"); }

} rig_set_bw(&rig_server);

class rig_set_verify_bw : public XmlRpcServerMethod {
public:
	rig_set_verify_bw(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_verify_bw", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		std::string bwstr = params;

		int bw = (int)params[0];
		std::vector<std::string>& widths = selrig->bandwidths_;
		if (bw < 0 || bw >= (int) widths.size()) {
			result = 0;
			return;
		}

		guard_lock que_lock ( &mutex_srvc_reqs, "xml rig_set_verify_bw" );
		guard_lock serial_lock(&mutex_serial, "xml rig_get_bw");

		XCVR_STATE nuvals;
		int retbw;
		nuvals.iBW = bw;
		if (selrig->inuse == onB) {
			nuvals.freq = vfoB.freq;
			nuvals.imode = vfoB.imode;
			serviceB(nuvals);
			retbw = selrig->get_bwB();
		} else {
			nuvals.freq = vfoA.freq;
			nuvals.imode = vfoA.imode;
			serviceA(nuvals);
			retbw = selrig->get_bwA();
		}
		result = (retbw == bw);
	}
	std::string help() { return std::string("set_bw to VAL"); }

} rig_set_verify_bw(&rig_server);

class rig_set_BW : public XmlRpcServerMethod {
public:
	rig_set_BW(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_BW", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		std::string bwstr = params;

		int bw = (int)params[0];
		std::vector<std::string>& widths = selrig->bandwidths_;
		if (bw < 0 || bw >= (int) widths.size()) {
			result = 0;
			return;
		}

		guard_lock que_lock ( &mutex_srvc_reqs, "xml rig_set_BW" );
		guard_lock serial_lock(&mutex_serial, "xml rig_get_BW");

		XCVR_STATE nuvals;
		if (selrig->inuse == onB) {
			nuvals.freq = vfoB.freq;
			nuvals.imode = vfoB.imode;
			nuvals.iBW = bw;
			serviceB(nuvals);
		} else {
			nuvals.freq = vfoA.freq;
			nuvals.imode = vfoA.imode;
			nuvals.iBW = bw;
			serviceA(nuvals);
		}

		result = 1;
	}
	std::string help() { return std::string("set_bw to VAL"); }

} rig_set_BW(&rig_server);

class rig_set_verify_BW : public XmlRpcServerMethod {
public:
	rig_set_verify_BW(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_verify_BW", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		std::string bwstr = params;

		int bw = (int)params[0];
		std::vector<std::string>& widths = selrig->bandwidths_;
		if (bw < 0 || bw >= (int) widths.size()) {
			result = 0;
			return;
		}

		guard_lock que_lock ( &mutex_srvc_reqs, "xml rig_set_BW" );
		guard_lock serial_lock(&mutex_serial, "xml rig_get_BW");

		XCVR_STATE nuvals;
		int retbw;
		nuvals.iBW = bw;
		if (selrig->inuse == onB) {
			nuvals.freq = vfoB.freq;
			nuvals.imode = vfoB.imode;
			serviceB(nuvals);
			retbw = selrig->get_bwB();
		} else {
			nuvals.freq = vfoA.freq;
			nuvals.imode = vfoA.imode;
			retbw = selrig->get_bwA();
			serviceA(nuvals);
		}
		result = (retbw == bw);

	}
	std::string help() { return std::string("set & verify BW to U/L values"); }

} rig_set_verify_BW(&rig_server);

static std::string retstr = "";

class rig_mod_bw: public XmlRpcServerMethod {
public:
	rig_mod_bw(XmlRpcServer* s) : XmlRpcServerMethod("rig.mod_bw", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		XCVR_STATE nuvals;
		int bwch = (int)params[0];

		guard_lock lock(&mutex_srvc_reqs, "xml rig_mod_bandwidth");
		guard_lock serial_lock(&mutex_serial, "xml rig_mod_bandwidth");

		if (selrig->inuse == onB) {
			nuvals.freq  = vfoB.freq;
			nuvals.imode = vfoB.imode;
			nuvals.iBW   = vfoB.iBW;
		} else {
			nuvals.freq  = vfoA.freq;
			nuvals.imode = vfoA.imode;
			nuvals.iBW   = vfoA.iBW;
		}

		std::vector<std::string>& widths = selrig->bandwidths_;
		int bw = atol(widths.at(0).c_str());
		try {
			bw = atol(widths.at(nuvals.iBW).c_str()) + bwch;

			size_t i = 0;
			int bwval;
			for (i = 0; i < widths.size(); i++) {
				if ((bwval = atol(widths.at(i).c_str())) >= bw)
					break;
			}
			if (bwval > bw && i > 0) bw = atol(widths.at(--i).c_str());

			std::ostringstream s;
			s << "nearest bandwidth " << widths[i];
			xml_trace(2,"Set to ", s.str().c_str());

			nuvals.iBW = i;
		} catch (const std::exception& e) {
			std::cout << e.what() << '\n';
		}

		if (selrig->inuse == onB) {
			serviceB(nuvals);
		} else {
			serviceA(nuvals);
		}

	}
	std::string help() { return std::string("modify bw to nearest requested"); }

} rig_mod_bw(&rig_server);

//------------------------------------------------------------------------------
// Get active vfo inner/outer setting
// returns NONE if not available
// returns INNER/OUTTER otherwise
//------------------------------------------------------------------------------

class rig_get_pbt : public XmlRpcServerMethod {
public:
	rig_get_pbt(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_pbt", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {

		result[0] = "NONE";
		result[1] = "";

		if (!xcvr_online || disable_xmlrpc->value() || !selrig->has_pbt_controls) return;

		wait();
		guard_lock service_lock(&mutex_srvc_reqs, "xml rig_get_");
		guard_lock serial_lock(&mutex_serial, "xml rig_get_");

		char inner[10];
		char outer[10];
		snprintf(inner, sizeof(inner), "%d", progStatus.pbt_inner);
		snprintf(outer, sizeof(outer), "%d", progStatus.pbt_outer);

		result[0] = inner;
		result[1] = outer;

		std::string s1 = result[0], s2 = result[1];
		xml_trace( 5, "pbt ", ((selrig->inuse == onB) ? "B " : "A "), s1.c_str(), " | ", s2.c_str());
	}

	std::string help() { return std::string("returns current passband inner/outer values"); }

} rig_get_pbt(&rig_server);

class rig_get_pbt_inner : public XmlRpcServerMethod {
public:
	rig_get_pbt_inner(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_pbt_inner", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {

		result[0] = "NONE";
		result[1] = "";

		if (!xcvr_online || disable_xmlrpc->value() || !selrig->has_pbt_controls) return;

		wait();
		guard_lock service_lock(&mutex_srvc_reqs, "xml rig_get_");
		guard_lock serial_lock(&mutex_serial, "xml rig_get_");

		char inner[10];
		snprintf(inner, sizeof(inner), "%d", progStatus.pbt_inner);

		result[0] = inner;

		std::string s1 = result[0], s2 = result[1];
		xml_trace( 5, "pbt inner", ((selrig->inuse == onB) ? "B " : "A "), s1.c_str(), " | ", s2.c_str());
	}

	std::string help() { return std::string("returns current passband inner values"); }

} rig_get_pbt_inner(&rig_server);

class rig_get_pbt_outer : public XmlRpcServerMethod {
public:
	rig_get_pbt_outer(XmlRpcServer* s) : XmlRpcServerMethod("rig.get_pbt_outer", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {

		result[0] = "NONE";
		result[1] = "";

		if (!xcvr_online || disable_xmlrpc->value() || !selrig->has_pbt_controls) return;

		wait();
		guard_lock service_lock(&mutex_srvc_reqs, "xml rig_get_");
		guard_lock serial_lock(&mutex_serial, "xml rig_get_");

		char outer[10];
		snprintf(outer, sizeof(outer), "%d", progStatus.pbt_outer);

		result[0] = outer;

		std::string s1 = result[0], s2 = result[1];
		xml_trace( 5, "pbt outer", ((selrig->inuse == onB) ? "B " : "A "), s1.c_str(), " | ", s2.c_str());
	}

	std::string help() { return std::string("returns current passband outer values"); }

} rig_get_pbt_outer(&rig_server);

class rig_set_pbt : public XmlRpcServerMethod {
public:
	rig_set_pbt(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_pbt", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()  || !selrig->has_pbt_controls) {
			result = 0;
			return;
		}

		guard_lock serial_lock(&mutex_serial, "xml rig_set_pbt");

		progStatus.pbt_inner = (int)(params[0]);
		progStatus.pbt_outer = (int)(params[1]);

		selrig->set_pbt_inner(progStatus.pbt_inner);
		selrig->set_pbt_outer(progStatus.pbt_outer);

		xml_trace(1, "rig_set_pbt");
	}

	std::string help() { return std::string("sets pbt value"); }

} rig_set_pbt(&rig_server);

class rig_set_pbt_inner : public XmlRpcServerMethod {
public:
	rig_set_pbt_inner(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_pbt_inner", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()  || !selrig->has_pbt_controls) {
			result = 0;
			return;
		}

		guard_lock serial_lock(&mutex_serial, "xml rig_set_pbt_inner");

		progStatus.pbt_inner = (int)(params[0]);

		selrig->set_pbt_inner(progStatus.pbt_inner);

		xml_trace(1, "rig_set_pbt_inner");
	}

	std::string help() { return std::string("sets pbt inner value"); }

} rig_set_pbt_inner(&rig_server);

class rig_set_pbt_outer : public XmlRpcServerMethod {
public:
	rig_set_pbt_outer(XmlRpcServer* s) : XmlRpcServerMethod("rig.set_pbt_outer", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()  || !selrig->has_pbt_controls) {
			result = 0;
			return;
		}

		guard_lock serial_lock(&mutex_serial, "xml rig_set_pbt_outer");

		progStatus.pbt_outer = (int)(params[0]);

		selrig->set_pbt_outer(progStatus.pbt_outer);

		xml_trace(1, "rig_set_pbt_outer");
	}

	std::string help() { return std::string("sets pbt outer value"); }

} rig_set_pbt_outer(&rig_server);


//----------------------------------------------------------------------

class rig_cat_string : public XmlRpcServerMethod {
public:
	rig_cat_string(XmlRpcServer* s) : XmlRpcServerMethod("rig.cat_string", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		result = 0;
		if (!xcvr_online || disable_xmlrpc->value()) {
			return;
		}
		std::string command = std::string(params[0]);
		bool usehex = false;
		if (command.empty()) return;

		std::string cmd = "";
		if (command.find("x") != std::string::npos) { // hex std::strings
			size_t p = 0;
			unsigned int val;
			usehex = true;
			while (( p = command.find("x", p)) != std::string::npos) {
				sscanf(&command[p+1], "%x", &val);
				cmd += (unsigned char) val;
				p += 3;
			}
		} else
			cmd = command;
// lock out polling loops until done
		{
			guard_lock lock1(&mutex_srvc_reqs);
			guard_lock lock2(&mutex_serial);

			RigSerial->WriteBuffer(cmd.c_str(), cmd.length());

			retstr.clear();
			waitResponse(10);//(100);
			if (!respstr.empty()) {
				retstr = usehex ?
					str2hex(respstr.c_str(), respstr.length()) :
					respstr;
				result = retstr;
			} else
				result = std::string("No response: ").append(selrig->name_);
		}

		xml_trace(2, "xmlrpc command:", command.c_str());
	}

	std::string help() { return std::string("sends xmlrpc CAT std::string to xcvr"); }

} rig_cat_string(&rig_server);

class rig_cat_priority : public XmlRpcServerMethod {
public:
	rig_cat_priority(XmlRpcServer* s) : XmlRpcServerMethod("rig.cat_priority", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		result = 0;
		if (!xcvr_online || disable_xmlrpc->value()) {
			return;
		}
		std::string command = std::string(params[0]);
		if (command.empty()) return;

		std::string cmd = "";
		if (command.find("x") != std::string::npos) { // hex std::strings
			size_t p = 0;
			unsigned int val;
			while (( p = command.find("x", p)) != std::string::npos) {
				sscanf(&command[p+1], "%x", &val);
				cmd += (unsigned char) val;
				p += 3;
			}
		} else
			cmd = command;

		guard_lock lock1(&mutex_srvc_reqs);
		guard_lock lock2(&mutex_serial);

		RigSerial->WriteBuffer(cmd.c_str(), cmd.length());
		result = std::string("OK");

		return;
	}

	std::string help() { return std::string("sends xmlrpc priority CAT std::string to xcvr"); }

} rig_cat_priority(&rig_server);


//------------------------------------------------------------------------------
// Set cwio words per minute
//------------------------------------------------------------------------------

static int cwio_wpm;
static void set_cwio_wpm(void *)
{
	if (sldr_cwioWPM) {
		sldr_cwioWPM->value(cwio_wpm);
		progStatus.cwioWPM = cwio_wpm;
	}
}

class rig_cwio_set_wpm : public XmlRpcServerMethod {
public:
	rig_cwio_set_wpm(XmlRpcServer* s) : XmlRpcServerMethod("rig.cwio_set_wpm", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		cwio_wpm = int(params[0]);
		Fl::awake(set_cwio_wpm);
	}

	std::string help() { return std::string("sets DTR/RTS WPM rate"); }

} rig_cwio_set_wpm(&rig_server);

class rig_cwio_get_wpm : public XmlRpcServerMethod {
public:
	rig_cwio_get_wpm(XmlRpcServer* s) : XmlRpcServerMethod("rig.cwio_get_wpm", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		result = int(progStatus.cwioWPM);
	}

	std::string help() { return std::string("sets DTR/RTS WPM rate"); }

} rig_cwio_get_wpm(&rig_server);

class rig_cwio_mod_wpm : public XmlRpcServerMethod {
public:
	rig_cwio_mod_wpm(XmlRpcServer* s) : XmlRpcServerMethod("rig.mod_cwio_wpm", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		int change = int(params[0]);
		cwio_wpm += change;
		Fl::awake(set_cwio_wpm);
	}

	std::string help() { return std::string("modify DTR/RTS WPM rate +/- NNN wpm"); }

} rig_cwio_mod_wpm(&rig_server);

//------------------------------------------------------------------------------
// Set cwio transmit std::string
//------------------------------------------------------------------------------

class rig_cwio_text : public XmlRpcServerMethod {
public:
	rig_cwio_text(XmlRpcServer* s) : XmlRpcServerMethod("rig.cwio_text", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		std::string s = (std::string)params[0];
		guard_lock lck(&cwio_text_mutex);
		add_cwio(s);
	}

	std::string help() { return std::string("sends text using cwio DTR/RTS keying"); }

} rig_cwio_text(&rig_server);

//------------------------------------------------------------------------------
// Set fskio transmit std::string
//------------------------------------------------------------------------------

extern void FSK_add(std::string);
static std::string fskio_text;

class rig_fskio_text : public XmlRpcServerMethod {
public:
	rig_fskio_text(XmlRpcServer* s) : XmlRpcServerMethod("rig.fskio_text", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		std::string s = (std::string)params[0];
		if (s.find(']') != std::string::npos) ptt_pending = true;
		FSK_add(s);
	}

	std::string help() { return std::string("sends text using fskio DTR/RTS keying"); }

} rig_fskio_text(&rig_server);

// Set cwio transmit on (1) or off (0)
//------------------------------------------------------------------------------

static void set_cwio_send_button(void *val)
{
	if (val == (void *)1) {
		btn_cwioSEND->value(1);
	} else {
		btn_cwioSEND->value(0);
		guard_lock lck(&cwio_text_mutex);
//		cwio_text.clear();
		cwio_clear_text();
	}
	btn_cwioSEND->redraw();
}

class rig_set_verify_cwio_send : public XmlRpcServerMethod {
public:
	rig_set_verify_cwio_send(XmlRpcServer* s) : XmlRpcServerMethod("rig.cwio_send", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		int send_state = int(params[0]);
		send_text(send_state);
		Fl::awake(set_cwio_send_button, reinterpret_cast<void *>(send_state));
	}

	std::string help() { return std::string("sets cwio transmit on (1) or off (0)"); }

} rig_set_verify_cwio_send(&rig_server);

//------------------------------------------------------------------------------


static void shutdown(void *)
{
	cbExit();
}

class rig_shutdown : public XmlRpcServerMethod {
public:
	rig_shutdown(XmlRpcServer* s) : XmlRpcServerMethod("rig.shutdown", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}
		Fl::awake(shutdown, static_cast<void *>(0));
	}
	std::string help() { return std::string("turn off rig / close flrig"); }

} rig_shutdown(&rig_server);

//------------------------------------------------------------------------------
// Execute command button, 1..24
// Execute command button SHIFT, 25..48
//------------------------------------------------------------------------------

static int exec_btn = 0;
static bool exec_shift = false;

void btn_user(void *)
{
	exec_btnUser(exec_btn, exec_shift);
}

class rig_cmd : public XmlRpcServerMethod {
public:
	rig_cmd (XmlRpcServer* s) : XmlRpcServerMethod("rig.cmd", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		if (!xcvr_online || disable_xmlrpc->value()) {
			result = 0;
			return;
		}

		exec_btn = int(params[0]);
		exec_shift = false;
		if (exec_btn >= 25) {
			exec_btn -= 24;
			exec_shift = true;
			
			}
		if (exec_btn < 0 || exec_btn > 24) return;
		Fl::awake(btn_user, (void *)0);

	}
	std::string help() { return std::string("execute command button 1..24; (shift)25..48"); }

} rig_cmd(&rig_server);


struct MLIST {
	std::string name; std::string signature; std::string help;
} mlist[] = {
	{ "main.set_frequency", "d:d", "set current VFO in Hz" },
	{ "main.get_version", "s:n", "returns version std::string" },
	{ "rig.get_AB",       "s:n", "returns vfo in use A or B" },
	{ "rig.get_bw",       "A:n", "return BW of current VFO" },
	{ "rig.get_bws",      "A:n", "return table of BW values" },
	{ "rig.get_bwA",      "A:n", "return BW of vfo A" },
	{ "rig.get_bwB",      "A:n", "return BW of vfo B" },
	{ "rig.get_pbt",      "A:n", "return passband tuning"},
	{ "rig.get_pbt_inner","i:i", "return passband inner"},
	{ "rig.get_pbt_outer","i:i", "return passband outer"},
	{ "rig.get_info",     "s:n", "return an info std::string" },
	{ "rig.get_mode",     "s:n", "return MODE of current VFO" },
	{ "rig.get_modeA",    "s:n", "return MODE of current VFO A" },
	{ "rig.get_modeB",    "s:n", "return MODE of current VFO B" },
	{ "rig.get_modes",    "A:n", "return table of MODE values" },
	{ "rig.get_sideband", "s:n", "return sideband (U/L)" },
	{ "rig.get_notch",    "i:n", "return notch value" },
	{ "rig.get_ptt",      "i:n", "return PTT state" },
	{ "rig.get_power",    "i:n", "return power level control value" },
	{ "rig.get_pwrmeter", "s:n", "return PWR out" },
	{ "rig.get_pwrmeter_scale", "s:n", "return scale for power meter" },
	{ "rig.get_pwrmax",   "s:n", "return maximum power available" },
	{ "rig.get_swrmeter", "s:n", "return SWR out" },
	{ "rig.get_smeter",   "s:n", "return Smeter" },
	{ "rig.get_DBM",      "s:n", "return Smeter in dBm" },
	{ "rig.get_Sunits",   "s:n", "return Smeter in S units" },
	{ "rig.get_split",    "i:n", "return split state" },
	{ "rig.get_update",   "s:n", "return update to info" },
	{ "rig.get_vfo",      "s:n", "return current VFO in Hz" },
	{ "rig.get_vfoA",     "s:n", "return vfo A in Hz" },
	{ "rig.get_vfoB",     "s:n", "return vfo B in Hz" },
	{ "rig.get_xcvr",     "s:n", "returns name of transceiver" },
	{ "rig.get_volume",   "i:n", "returns volume control value" },
	{ "rig.get_rfgain",   "i:n", "returns rf gain control value" },
	{ "rig.get_micgain",  "i:n", "returns mic gain control value" },

	{ "rig.set_AB",       "n:s", "set VFO A/B" },
	{ "rig.set_bw",       "i:i", "set BW iaw BW table" },
	{ "rig.set_bandwidth","i:i", "set bandwidth to nearest requested value" },
	{ "rig.set_BW",       "i:i", "set L/U pair" },
	{ "rig.set_pbt",      "i:A", "set pbt inner/outer" },
	{ "rig.set_pbt_inner","i:i", "set pbt inner"},
	{ "rig.set_pbt_outer","i:i", "set pbt outer"},
	{ "rig.set_frequency","d:d", "set current VFO in Hz" },
	{ "rig.set_mode",     "i:s", "set MODE iaw MODE table" },
	{ "rig.set_modeA",    "i:s", "set MODE A iaw MODE table" },
	{ "rig.set_modeB",    "i:s", "set MODE B iaw MODE table" },
	{ "rig.set_notch",    "n:i", "set NOTCH value in Hz" },
	{ "rig.set_power",    "n:i", "set power control level, watts" },
	{ "rig.set_ptt",      "n:i", "set PTT 1/0 (on/off)" },
	{ "rig.set_vfo",      "d:d", "set current VFO in Hz" },
	{ "rig.set_vfoA",     "d:d", "set vfo A in Hz" },
	{ "rig.set_vfoB",     "d:d", "set vfo B in Hz" },
	{ "rig.set_split",    "n:i", "set split 1/0 (on/off)" },
	{ "rig.set_volume",   "n:i", "set volume control" },
	{ "rig.set_rfgain",   "n:i", "set rf gain control" },
	{ "rig.set_micgain",  "n:i", "set mic gain control" },

	{ "rig.set_ptt_fast",        "n:i", "deprecated; use set_ptt" },
	{ "rig.set_vfoA_fast",       "d:d", "deprecated; use set_vfoA" },
	{ "rig.set_vfoB_fast",       "d:d", "deprecated; use set_vfoB" },

	{ "rig.set_verify_AB",       "n:s", "set & verify VFO A/B" },
	{ "rig.set_verify_bw",       "i:i", "set & verify BW iaw BW table" },
	{ "rig.set_verify_bandwidth","i:i", "set & verify bandwidth to nearest requested value" },
	{ "rig.set_verify_BW",       "i:i", "set & verify L/U pair" },
	{ "rig.set_verify_frequency","d:d", "set & verify current VFO in Hz" },
	{ "rig.set_verify_mode",     "i:s", "set & verify MODE iaw MODE table" },
	{ "rig.set_verify_modeA",    "i:s", "set & verify MODE A iaw MODE table" },
	{ "rig.set_verify_modeB",    "i:s", "set & verify MODE B iaw MODE table" },
	{ "rig.set_verify_notch",    "n:i", "set & verify NOTCH value in Hz" },
	{ "rig.set_verify_power",    "n:i", "set & verify power control level, watts" },
	{ "rig.set_verify_ptt",      "n:i", "set & verify PTT 1/0 (on/off)" },
	{ "rig.set_verify_vfoA",     "d:d", "set & verify vfo A in Hz" },
	{ "rig.set_verify_vfoB",     "d:d", "set & verify vfo B in Hz" },
	{ "rig.set_verify_split",    "n:i", "set & verify split 1/0 (on/off)" },
	{ "rig.set_verify_volume",   "n:i", "set & verify volume control" },
	{ "rig.set_verify_rfgain",   "n:i", "set & verify rf gain control" },
	{ "rig.set_verify_micgain",  "n:i", "set & verify mic gain control" },

	{ "rig.swap",         "n:n", "execute vfo swap" },
	{ "rig.tune",         "n:n", "enable transceiver tune function"},
	{ "rig.cat_string",   "s:s", "execute CAT std::string" },
	{ "rig.cat_priority", "s:s", "priority CAT std::string" },
	{ "rig.shutdown",     "i:n", "shutdown xcvr & flrig" },
	{ "rig.cwio_set_wpm", "n:i", "set cwio WPM" },
	{ "rig.cwio_text",    "i:s", "send text via cwio interface" },
	{ "rig.cwio_send",    "n:i", "cwio transmit 1/0 (on/off)"},
	{ "rig.fskio_text",   "i:s", "send text via fskio interface" },

	{ "rig.mod_vfoA",     "d:d", "modify vfo A +/- NNN Hz" },
	{ "rig.mod_vfoB",     "d:d", "modify vfo B +/- NNN Hz" },
	{ "rig.mod_vol",      "n:i", "modify volume control +/- NNN %" },
	{ "rig.mod_pwr",      "n:i", "modify power control level +/- NNN watts" },
	{ "rig.mod_rfg",      "n:i", "modify rf gain by +/- NNN units" },
	{ "rig.mod_cwio_wpm", "n:i", "modify cwio WPM by +/- NNN wpm" },
	{ "rig.mod_bw",       "i:i", "modify bandwidth +- to nearest new value" },
	{ "rig.vfoA2B",       "n:n", "set vfo B to vfo A freq/mode" },
	{ "rig.freqA2B",      "n:n", "set freq B to freq A" },
	{ "rig.modeA2B",      "n:n", "set mode B to mode A" },

	{ "rig.cmd",          "n:i", "execute command button 1..24; 25..48(shift)"}
};

class rig_list_methods : public XmlRpcServerMethod {
public:
	rig_list_methods(XmlRpcServer *s) : XmlRpcServerMethod("rig.list_methods", s) {}

	void execute(XmlRpcValue& params, XmlRpcValue& result) {
		std::vector<XmlRpcValue> methods;
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

static bool run_server = false;

void * xml_thread_loop(void *d)
{
	run_server = true;
	for(;;) {
		if (!run_server) break;
		rig_server.work(-1.0);
	}
	return NULL;
}

void start_server(int port)
{
//	XmlRpc::setVerbosity(progStatus.rpc_level);

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
	run_server = false;
}

void set_server_port(int port)
{
	rig_server.bindAndListen(port);
}

std::string print_xmlhelp()
{
	static std::string pstr;
	pstr.clear();
	std::string line;
	size_t f1_len = 0;
	for (size_t n = 0; n < sizeof(mlist) / sizeof(*mlist); ++n) {
		if (mlist[n].name.length() > f1_len) f1_len = mlist[n].name.length();
	}
	for (size_t n = 0; n < sizeof(mlist) / sizeof(*mlist); ++n) {
		line.clear();
		line.assign(mlist[n].name);
		line.append(f1_len + 2 - line.length(), ' ');
		line.append(mlist[n].signature);
		line.append("  ");
		line.append(mlist[n].help);
		line.append("\n");
		pstr.append(line);
	}
	return pstr;
}

