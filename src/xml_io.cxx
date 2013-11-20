//======================================================================
// xml_io.cxx
//
// copyright 2009, W1HKJ
//
//======================================================================

#include <cstdlib>
#include <string>
#include <vector>
#include <queue>

#include "xml_io.h"
#include "support.h"
#include "debug.h"
#include "rig.h"

#include "rigbase.h"

#include "XmlRpc.h"

using namespace std;
using XmlRpc::XmlRpcValue;

extern char *print(FREQMODE);

extern queue<FREQMODE> queA;
extern queue<FREQMODE> queB;
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
static const char* rig_set_mode         = "rig.set_mode";
static const char* rig_get_mode         = "rig.get_mode";
static const char* rig_set_bandwidth    = "rig.set_bandwidth";
static const char* rig_get_bandwidth    = "rig.get_bandwidth";
static const char* rig_set_notch        = "rig.set_notch";
static const char* rig_get_notch        = "rig.get_notch";

static XmlRpc::XmlRpcClient* client;
static XmlRpcValue* status_query;

#define RIG_UPDATE_INTERVAL  100 // milliseconds
#define CHECK_UPDATE_COUNT   (500 / RIG_UPDATE_INTERVAL)

bool run_digi_loop = true;
//bool bypass_digi_loop = true;
bool fldigi_online = false;
bool rig_reset = false;
bool ptt_on = false;

int ignore = 0; // skip next "ignore" read loops

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
	pthread_mutex_lock(&mutex_xmlrpc);

	delete client;
	client = NULL;
	delete status_query;
	status_query = NULL;

	pthread_mutex_unlock(&mutex_xmlrpc);
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
	if (!selrig->modes_) return;

	XmlRpcValue modes, res;
	int i = 0;
	for (const char** mode = selrig->modes_; *mode; mode++, i++) {
		modes[0][i] = *mode;
	}

	try {
		execute(rig_set_modes, modes, res);
		ignore = 1;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_ERROR("%s", e.getMessage().c_str());
		throw;
	}
}

void send_bandwidths()
{
	if (!selrig->bandwidths_ ) return;
	XmlRpcValue bandwidths, res;
	int i = 0;
	for (const char** bw = selrig->bandwidths_; *bw; bw++, i++) {
		bandwidths[0][i] = *bw;
	}

	try {
		execute(rig_set_bandwidths, bandwidths, res);
		ignore = 1;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_ERROR("%s", e.getMessage().c_str());
		throw;
	}
}

void send_name()
{
	try {
		XmlRpcValue res;
		execute(rig_set_name, XmlRpcValue(selrig->name_), res);
		ignore = 1;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_ERROR("%s", e.getMessage().c_str());
		throw;
	}
}

void send_ptt_changed(bool PTT)
{
	try {
		XmlRpcValue res;
		execute((PTT ? main_set_tx : main_set_rx), XmlRpcValue(), res);
		ignore = 1;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_ERROR("%s", e.getMessage().c_str());
	}
}

void send_new_freq(long freq)
{
	try {
		xmlvfo.freq = freq;
		XmlRpcValue f((double)freq), res;
		execute(rig_set_frequency, f, res);
		ignore = 1;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_ERROR("%s", e.getMessage().c_str());
	}
}

void send_new_notch(int freq)
{
	try {
		XmlRpcValue i(freq), res;
		execute(rig_set_notch, i, res);
		ignore = 1;
		if (freq == 0)
			xml_notch_on = false;
		else {
			xml_notch_on = true;
			xml_notch_val = freq;
		}
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_ERROR("%s", e.getMessage().c_str());
	}
}

void send_xml_freq(long freq)
{
	if (rig_nbr == FT950 && freq > 5300000 && freq < 5500000)
		freq -= 1500;
	qfreq.push(freq);
}

void send_queue()
{
	while (!qfreq.empty()) {
		send_new_freq(qfreq.front());
		qfreq.pop();
	}
}

void send_new_mode(int md)
{
	if (!selrig->modes_) return;
	try {
		xmlvfo.imode = md;
		XmlRpcValue mode(selrig->modes_[md]), res;
		execute(rig_set_mode, mode, res);
		ignore = 1;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_ERROR("%s", e.getMessage().c_str());
	}
}

void send_new_bandwidth(int bw)
{
	if (!selrig->bandwidths_ ) return;
	try {
		xmlvfo.iBW = bw;
		int selbw = (bw > 0x80) ? (bw >> 8 & 0x7F) : bw;
//printf("bw %X, bw# %d, bw %s\n", bw, selbw, selrig->bandwidths_[selbw]);
		XmlRpcValue bandwidth(selrig->bandwidths_[selbw]), res;
		execute(rig_set_bandwidth, bandwidth, res);
		ignore = 1;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_ERROR("%s", e.getMessage().c_str());
	}
}

void send_sideband()
{
	try {
		XmlRpcValue sideband(selrig->get_modetype(vfo.imode) == 'U' ? "USB" : "LSB"), res;
		execute(main_set_wf_sideband, sideband, res);
		ignore = 1;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_ERROR("%s", e.getMessage().c_str());
		throw;
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
		if (XML_DEBUG) {
			string txstate = trx_state;
			LOG_ERROR("%s", txstate.c_str());
		}
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
		if (XML_DEBUG)
			LOG_ERROR("default BW %d: %s", xmlvfo.iBW, print(xmlvfo));
		return;
	}

	if (!selrig->bandwidths_ || vfo.iBW == -1 )
		return;

	string sbw = new_bw;
	if (selrig->has_dsp_controls) {
		int currlo = xmlvfo.iBW & 0x7F;
		int currhi = (xmlvfo.iBW >> 8) & 0x7F;
//printf("mode %d, lo %d, hi %d\n", xmlvfo.imode, currlo, currhi);
//printf("&bwtable(xmlvfo.imode) %p\n", selrig->bwtable(xmlvfo.imode));
		string currbw = selrig->bwtable(xmlvfo.imode)[currhi];
		if (sbw != currbw) {
			int ibw = 0;
			const char **bwt = selrig->bwtable(xmlvfo.imode);
			while ((bwt[ibw] != NULL) && (sbw != bwt[ibw])) ibw++;
//printf("index %d ", ibw);
			if (bwt[ibw] != NULL) { // && ibw != (xmlvfo.iBW & 0x7F)) {
				xmlvfo.iBW = (ibw << 8) | 0x8000 | currlo;
//printf("bandwidth %s, new xmlvfo.iBW %X", bwt[ibw], xmlvfo.iBW);
				xmlvfo_changed = true;
			}
//printf("\n");
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
	if (useB) {
		if (!queB.empty()) {
			if (XML_DEBUG)
				LOG_ERROR("B not empty %s", print(xmlvfo));
			return;
		}
		if (XML_DEBUG)
			LOG_ERROR("pushed to B %s", print(xmlvfo));
//printf("pushed to B %s\n", print(xmlvfo));
		pthread_mutex_lock(&mutex_queB);
		queB.push(xmlvfo);
		pthread_mutex_unlock(&mutex_queB);
	} else {
		if (!queA.empty()) {
			if (XML_DEBUG)
				LOG_ERROR("A not empty %s", print(xmlvfo));
			return;
		}
		if (XML_DEBUG)
			LOG_ERROR("pushed to A %s", print(xmlvfo));
//printf("pushed to A %s\n", print(xmlvfo));
		pthread_mutex_lock(&mutex_queA);
		queA.push(xmlvfo);
		pthread_mutex_unlock(&mutex_queA);
	}
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
		send_name();
//printf("send name\n");
		send_modes();
//printf("send_modes\n");
		send_bandwidths();
//printf("send_bandwidths\n");

		if (!useB) xmlvfo = vfoA;
		else       xmlvfo = vfoB;

		send_new_mode(xmlvfo.imode);
//printf("send_new_mode\n");
		send_new_bandwidth(xmlvfo.iBW);
//printf("send_new_bandwidth\n");
		send_sideband();
//printf("send sideband\n");
		send_new_freq(xmlvfo.freq);
//printf("send new frequency\n");
		fldigi_online = true;
		rig_reset = false;
		Fl::awake(set_fldigi_connect, (void *)1);
	} catch (...) {
		throw;
	}
}

void send_no_rig()
{
	XmlRpcValue res;
	execute(rig_set_name, szNORIG, res);
	send_bandwidths();
	send_modes();
	execute(rig_set_mode, "USB", res);
	XmlRpcValue sideband("USB");
	execute(main_set_wf_sideband, sideband, res);
	execute(rig_release_control, XmlRpcValue(), res);
}

static void get_fldigi_status()
{
	XmlRpcValue status;
	XmlRpcValue param((int)0);
	string xmlcall;
	if (ignore) {
		--ignore;
		return;
	}
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
	} catch (...) {
		LOG_ERROR("%s", xmlcall.c_str());
		throw;
	}
	try {
		if (selrig->has_notch_control) {
			xmlcall = rig_get_notch;
			execute(xmlcall.c_str(), param, status);
			check_for_notch_change(status);
		}
	} catch (...) {
	}
}

void * digi_loop(void *d)
{
	for (;;) {
		MilliSleep(RIG_UPDATE_INTERVAL);
		if (!run_digi_loop) break;
		pthread_mutex_lock(&mutex_xmlrpc);
		try {
			if (rig_reset || (!fldigi_online && (--try_count == 0)))
				send_rig_info();
			else if (fldigi_online) {
				if (qfreq.empty())
					get_fldigi_status();
				else
					send_queue();
			}
		} catch (const XmlRpc::XmlRpcException& e) {
			if (XML_DEBUG)
				LOG_ERROR("%s", e.getMessage().c_str());
			fldigi_online = false;
			rig_reset = false;
			try_count = CHECK_UPDATE_COUNT;
			Fl::awake(set_fldigi_connect, (void *)0);
		}
		pthread_mutex_unlock(&mutex_xmlrpc);
	}
	return NULL;
}

