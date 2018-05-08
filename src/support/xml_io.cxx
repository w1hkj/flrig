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

#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>
#include <queue>

#include "xml_io.h"
#include "support.h"
#include "debug.h"
#include "rig.h"
#include "rigs.h"
#include "rigbase.h"

#include "threads.h"
#include "XmlRpc.h"
#include "xml_server.h"

using namespace std;
using XmlRpc::XmlRpcValue;

extern char *print(XCVR_STATE &);

extern queue<XCVR_STATE> vfoque;
extern queue<bool> quePTT;

queue<long> qfreq;

static const double TIMEOUT = 1.0;

// these are get only
static const char* main_get_trx_state   = "main.get_trx_state";

// these are set only
static const char* main_set_tx          = "main.tx";
static const char* main_set_rx          = "main.rx";
static const char* rig_set_name         = "rig.set_name";
static const char* rig_set_modes        = "rig.set_modes";
static const char* rig_set_bandwidths   = "rig.set_bandwidths";
static const char* rig_take_control     = "rig.take_control";
static const char* rig_release_control  = "rig.release_control";

// these are get/set
static const char* main_get_frequency   = "main.get_frequency";
static const char* main_set_wf_sideband = "main.set_wf_sideband";
static const char* rig_set_frequency    = "rig.set_frequency";
static const char *rig_set_smeter       = "rig.set_smeter";
static const char *rig_set_pwrmeter     = "rig.set_pwrmeter";
static const char* rig_set_mode         = "rig.set_mode";
static const char* rig_get_mode         = "rig.get_mode";
static const char* rig_set_bandwidth    = "rig.set_bandwidth";
static const char* rig_get_bandwidth    = "rig.get_bandwidth";
static const char* rig_set_notch        = "rig.set_notch";
static const char* rig_get_notch        = "rig.get_notch";

static XmlRpc::XmlRpcClient* client;
static XmlRpcValue* status_query;

#define RIG_UPDATE_INTERVAL  100 // milliseconds
#define CHECK_UPDATE_COUNT   (2000 / RIG_UPDATE_INTERVAL)

bool run_digi_loop = true;
bool fldigi_online = false;

bool rig_reset = false;
int  err_count = 5;

bool ptt_on = false;

int xml_ignore = 0; // skip next "xml_ignore" read loops

int try_count = CHECK_UPDATE_COUNT;

int xml_notch_val = 0;
bool xml_notch_on = false;

//=====================================================================
// socket ops
//=====================================================================

void open_rig_xmlrpc()
{
	int server_port = atoi(progStatus.server_port.c_str());
	client = new XmlRpc::XmlRpcClient(
				progStatus.server_addr.c_str(),
				server_port );
	// XmlRpc::setVerbosity(5);

	// Create a value suitable for the system.multicall method, which
	// accepts a single argument: an array of struct { methodName, params }
	// where methodName is a string and params is an array of method
	// arguments.  The params member is mandatory so we use a single-element
	// array containing nil.

	XmlRpcValue nil(XmlRpc::nil);
	status_query = new XmlRpcValue;
	const char* status_methods[] = {
		main_get_trx_state, main_get_frequency,
		rig_get_mode, rig_get_bandwidth
	};
	for (size_t i = 0; i < sizeof(status_methods)/sizeof(*status_methods); i++) {
		(*status_query)[0][i]["methodName"] = status_methods[i];
		(*status_query)[0][i]["params"][0] = nil;
	};
}

void close_rig_xmlrpc()
{
	guard_lock gl_xmlclose(&mutex_xmlrpc, 100);

	delete client;
	client = NULL;
	delete status_query;
	status_query = NULL;

}

static inline void execute(const char* name, const XmlRpcValue& param, XmlRpcValue& result)
{
	if (client)
		if (!client->execute(name, param, result, TIMEOUT))
			throw XmlRpc::XmlRpcException(name);
}

// --------------------------------------------------------------------
// send functions
// --------------------------------------------------------------------

void send_modes() { 
	if (!selrig->modes_ || !fldigi_online) return;

	XmlRpcValue modes, res;
	int i = 0;
	for (const char** mode = selrig->modes_; *mode; mode++, i++) {
		modes[0][i] = *mode;
	}

	try {
		execute(rig_set_modes, modes, res);
		xml_ignore = 2;
		err_count = 5;
	} catch (const XmlRpc::XmlRpcException& e) {
		err_count--;
//		LOG_ERROR("err count %d: %s", err_count, e.getMessage().c_str());
	}
}

void send_bandwidths()
{
	if (!selrig->bandwidths_ || !fldigi_online) {
		return;
	}
	XmlRpcValue bandwidths, res;
	int i = 0;
	for (const char** bw = selrig->bandwidths_; *bw; bw++, i++) {
		bandwidths[0][i] = *bw;
	}

	try {
		execute(rig_set_bandwidths, bandwidths, res);
		xml_ignore = 2;
		err_count = 5;
	} catch (const XmlRpc::XmlRpcException& e) {
		err_count--;
//		LOG_ERROR("err count %d: %s", err_count, e.getMessage().c_str());
	}
}

void send_name()
{
	if (!fldigi_online) return;
	try {
		XmlRpcValue res;
		execute(rig_set_name, XmlRpcValue(selrig->name_), res);
		xml_ignore = 2;
		err_count = 5;
	} catch (const XmlRpc::XmlRpcException& e) {
		err_count--;
//		LOG_ERROR("err count %d: %s", err_count, e.getMessage().c_str());
	}
}

void send_ptt_changed(bool PTT)
{
	if (!fldigi_online) return;
	try {
		XmlRpcValue res;
		execute((PTT ? main_set_tx : main_set_rx), XmlRpcValue(), res);
		xml_ignore = 2;
		err_count = 5;
	} catch (const XmlRpc::XmlRpcException& e) {
		err_count--;
//		LOG_ERROR("err count %d: %s", err_count, e.getMessage().c_str());
	}
}

void send_new_freq(long freq)
{
	if (!fldigi_online) return;
	try {
		xmlvfo.freq = freq;
		XmlRpcValue f((double)freq), res;
		execute(rig_set_frequency, f, res);
		xml_ignore = 2;
		err_count = 5;
	} catch (const XmlRpc::XmlRpcException& e) {
		err_count--;
//		LOG_ERROR("err count %d: %s", err_count, e.getMessage().c_str());
	}
}

void send_smeter_val(int val)
{
	if (!fldigi_online) return;
	try {
		XmlRpcValue mval((int)val), res;
		execute(rig_set_smeter, mval, res);
		xml_ignore = 2;
		err_count = 5;
	} catch (const XmlRpc::XmlRpcException& e) {
		err_count--;
//		LOG_ERROR("err count %d: %s", err_count, e.getMessage().c_str());
	}
}

void send_pwrmeter_val(int val)
{
	if (!fldigi_online) return;
	try {
		XmlRpcValue mval((int)val), res;
		execute(rig_set_pwrmeter, mval, res);
		xml_ignore = 2;
		err_count = 5;
	} catch (const XmlRpc::XmlRpcException& e) {
		err_count--;
//		LOG_ERROR("err count %d: %s", err_count, e.getMessage().c_str());
	}
}

void send_new_notch(int freq)
{
	if (!fldigi_online) return;
	try {
		XmlRpcValue i(freq), res;
		execute(rig_set_notch, i, res);
		xml_ignore = 2;
		err_count = 5;
		if (freq == 0)
			xml_notch_on = false;
		else {
			xml_notch_on = true;
			xml_notch_val = freq;
		}
	} catch (const XmlRpc::XmlRpcException& e) {
		err_count--;
//		LOG_ERROR("err count %d: %s", err_count, e.getMessage().c_str());
	}
}

void send_xml_freq(long freq)
{
	if (!fldigi_online) return;
	if (xcvr_name == rig_FT950.name_ && freq > 5300000 && freq < 5500000)
		freq -= 1500;
	qfreq.push(freq);
}

void send_queue()
{
	if (!fldigi_online) return;
	while (!qfreq.empty()) {
		send_new_freq(qfreq.front());
		qfreq.pop();
	}
}

void send_new_mode(int md)
{
	if (!selrig->modes_ || !fldigi_online) return;
	try {
		xmlvfo.imode = md;
		XmlRpcValue mode(selrig->modes_[md]), res;
		execute(rig_set_mode, mode, res);
		xml_ignore = 2;
		err_count = 5;
	} catch (const XmlRpc::XmlRpcException& e) {
		err_count--;
//		LOG_ERROR("err count %d: %s", err_count, e.getMessage().c_str());
	}
}

void send_new_bandwidth(int bw)
{
	if (!selrig->bandwidths_ || !fldigi_online) return;
	try {
		xmlvfo.iBW = bw;
		int selbw = (bw > 0x80) ? (bw >> 8 & 0x7F) : bw;
		XmlRpcValue bandwidth(selrig->bandwidths_[selbw]), res;
		execute(rig_set_bandwidth, bandwidth, res);
		xml_ignore = 2;
		err_count = 5;
	} catch (const XmlRpc::XmlRpcException& e) {
		err_count--;
//		LOG_ERROR("err count %d: %s", err_count, e.getMessage().c_str());
	}
}

void send_sideband()
{
	if (!fldigi_online) return;
	try {
		XmlRpcValue sideband(selrig->get_modetype(vfo->imode) == 'U' ? "USB" : "LSB"), res;
		execute(main_set_wf_sideband, sideband, res);
		xml_ignore = 2;
		err_count = 5;
	} catch (const XmlRpc::XmlRpcException& e) {
		err_count--;
//		LOG_ERROR("err count %d: %s", err_count, e.getMessage().c_str());
	}
}

// --------------------------------------------------------------------
// receive functions
// --------------------------------------------------------------------

bool xmlvfo_changed = false;

static void check_for_ptt_change(const XmlRpcValue& trx_state)
{
	bool nuptt = (trx_state == "TX");
	if (nuptt != ptt_on) {

		string txstate = trx_state;
		xml_trace(1, txstate.c_str());

		ptt_on = nuptt;
		Fl::awake(setPTT, (void*)ptt_on);
	}
}

static void check_for_frequency_change(const XmlRpcValue& freq)
{
	double f = freq;
	long newfreq = (long)f;
	if (newfreq != xmlvfo.freq) {
		xmlvfo.freq = newfreq;
		xmlvfo_changed = true;
	}
}

static bool xmlmode_changed = false;

static void check_for_mode_change(const XmlRpcValue& new_mode)
{
	if (!selrig->modes_) 
		return;
	string smode = new_mode;
	xmlmode_changed = false;
	if (smode != selrig->modes_[xmlvfo.imode]) {
		int imode = 0;
		while (selrig->modes_[imode] != NULL && smode != selrig->modes_[imode])
			imode++;
		if (selrig->modes_[imode] != NULL && imode != xmlvfo.imode) {
			xmlvfo.imode = imode;
			xmlvfo_changed = true;
			xmlmode_changed = true;
		}
	}
}

static void check_for_bandwidth_change(const XmlRpcValue& new_bw)
{
	if (xmlmode_changed) {
		xmlvfo.iBW = selrig->def_bandwidth(xmlvfo.imode);
		ostringstream s;
		s << "default BW " << xmlvfo.iBW << " : " << print(xmlvfo);
		xml_trace(1, s.str().c_str());
		return;
	}

	if (!selrig->bandwidths_ || vfo->iBW == -1 )
		return;

	string sbw = new_bw;
	if (selrig->has_dsp_controls) {
		int currlo = xmlvfo.iBW & 0x7F;
		int currhi = (xmlvfo.iBW >> 8) & 0x7F;
		string currbw = selrig->bwtable(xmlvfo.imode)[currhi];
		if (sbw != currbw) {
			int ibw = 0;
			const char **bwt = selrig->bwtable(xmlvfo.imode);
			while ((bwt[ibw] != NULL) && (sbw != bwt[ibw])) ibw++;
			if (bwt[ibw] != NULL) { // && ibw != (xmlvfo.iBW & 0x7F)) {
				xmlvfo.iBW = (ibw << 8) | 0x8000 | currlo;
				xmlvfo_changed = true;
			}
		}
	} else {
		if (sbw != selrig->bwtable(xmlvfo.imode)[xmlvfo.iBW]) {
			int ibw = 0;
			const char **bwt = &selrig->bwtable(xmlvfo.imode)[0];
			while ((bwt[ibw] != NULL) && (sbw != bwt[ibw])) ibw++;
			if (bwt[ibw] != NULL && ibw != xmlvfo.iBW) {
				xmlvfo.iBW = ibw;
				xmlvfo_changed = true;
			}
		}
	}
}

static void check_for_notch_change(const XmlRpcValue& new_notch)
{
	int ntch = new_notch;
	if (ntch == 0 && xml_notch_on)
		xml_notch_on = false;
	else
	if (ntch != xml_notch_val ) {
		xml_notch_on = true;
		xml_notch_val = ntch;
		Fl::awake(setNotchControl, (void *)ntch);
	}
}

static void push2que()
{
	guard_lock que_lock( &mutex_vfoque );
	vfoque.push( (useB ? B : A), xmlvfo);
	xmlvfo_changed = false;
}

void set_fldigi_connect (void *d)
{
	bool b = (long *)d;
	box_fldigi_connect->color(b ? FL_RED : FL_BACKGROUND2_COLOR);
	box_fldigi_connect->redraw();
}

static void send_rig_info()
{
	XmlRpcValue res;
	try {
		execute(rig_take_control, XmlRpcValue(), res);
	} catch (const XmlRpc::XmlRpcException& e) {
		throw e;
	}
	try {
		fldigi_online = true;

		if (!useB) xmlvfo = vfoA;
		else       xmlvfo = vfoB;

		send_name();
		send_modes();
		send_new_mode(xmlvfo.imode);
		send_sideband();

MilliSleep(500);

		send_bandwidths();

		send_new_bandwidth(xmlvfo.iBW);

		send_new_freq(xmlvfo.freq);

		rig_reset = false;
		LOG_ERROR("Connected");
		Fl::awake(set_fldigi_connect, (void *)1);
		err_count = 5;
		xml_ignore = 2;
	} catch (const XmlRpc::XmlRpcException& e) {
		throw e;
	}
}

void send_no_rig()
{
	if (!fldigi_online) return;
	try {
		XmlRpcValue res;
		execute(rig_set_name, szNORIG, res);
		send_bandwidths();
		send_modes();
		execute(rig_set_mode, "USB", res);
		XmlRpcValue sideband("USB");
		execute(main_set_wf_sideband, sideband, res);
		execute(rig_release_control, XmlRpcValue(), res);
	} catch (const XmlRpc::XmlRpcException& e) {
		throw e;
	}
}

static void get_fldigi_status()
{
	if (xml_ignore) {
		--xml_ignore;
		return;
	}
	XmlRpcValue status;
	XmlRpcValue param((int)0);
	string xmlcall;
	try {
		xmlcall = "system.multicall";
		execute(xmlcall.c_str(), *status_query, status);
		check_for_ptt_change(status[0][0]);
		if (!ptt_on) {
			xmlvfo.src = XML;
			xmlvfo_changed = false;
			check_for_frequency_change(status[1][0]);
			check_for_mode_change(status[2][0]);
			check_for_bandwidth_change(status[3][0]);
			if (xmlvfo_changed)
				push2que();
		}
		err_count = 5;
	} catch (const XmlRpc::XmlRpcException& e) {
		err_count--;
//		LOG_ERROR("err count %d: %s", err_count, e.getMessage().c_str());
		if (err_count <= 0)
			throw e;
	}
	if (selrig->has_notch_control) {
		try {
			xmlcall = rig_get_notch;
			execute(xmlcall.c_str(), param, status);
			check_for_notch_change(status);
			err_count = 5;
		} catch (const XmlRpc::XmlRpcException& e) {
			err_count--;
//			LOG_ERROR("err count %d: %s", err_count, e.getMessage().c_str());
			if (err_count <= 0)
				throw e;
		}
	}
}

extern bool xcvr_initialized;

void * digi_loop(void *d)
{
	for (;;) {
		MilliSleep(RIG_UPDATE_INTERVAL);

		if (!run_digi_loop) {
			LOG_ERROR("Exiting digi loop");
			try {
				send_no_rig();
			} catch (const XmlRpc::XmlRpcException& e) {
				break;
			}
			break;
		}
		if (!xcvr_initialized) continue;

		if (rig_reset || (!fldigi_online && (--try_count == 0))) {
			try {
				guard_lock gl_xmloop(&mutex_xmlrpc, 103);
				send_rig_info();
			} catch (const XmlRpc::XmlRpcException& e) {
				fldigi_online = false;
				rig_reset = false;
				err_count = 5;
				try_count = CHECK_UPDATE_COUNT;
				Fl::awake(set_fldigi_connect, (void *)0);
			}
		}
		if (fldigi_online) {
			try {
				if (qfreq.empty()) {
					guard_lock gl_xmloop(&mutex_xmlrpc, 104);
					get_fldigi_status();
				} else {
					guard_lock gl_xmloop(&mutex_xmlrpc, 105);
					send_queue();
				}
			} catch (const XmlRpc::XmlRpcException& e) {
				LOG_ERROR("%s", e.getMessage().c_str());
				fldigi_online = false;
				rig_reset = false;
				err_count = 5;
				try_count = CHECK_UPDATE_COUNT;
				Fl::awake(set_fldigi_connect, (void *)0);
			}
		}
	}
	return NULL;
}

