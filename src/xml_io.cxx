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

static const double TIMEOUT = 0.5;

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

bool ignore = false;

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
		rig_get_mode, rig_get_bandwidth, rig_get_notch
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
		ignore = true;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_WARN("%s", e.getMessage().c_str());
		throw;
	}
}

void send_bandwidths()
{
	if (!selrig->bandwidths_ || selrig->has_dsp_controls) return;
	XmlRpcValue bandwidths, res;
	int i = 0;
	for (const char** bw = selrig->bandwidths_; *bw; bw++, i++) {
		bandwidths[0][i] = *bw;
	}

	try {
		execute(rig_set_bandwidths, bandwidths, res);
		ignore = true;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_WARN("%s", e.getMessage().c_str());
		throw;
	}
}

void send_name()
{
	try {
		XmlRpcValue res;
		execute(rig_set_name, XmlRpcValue(selrig->name_), res);
		ignore = true;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_WARN("%s", e.getMessage().c_str());
		throw;
	}
}

void send_ptt_changed(bool PTT)
{
	try {
		XmlRpcValue res;
		execute((PTT ? main_set_tx : main_set_rx), XmlRpcValue(), res);
		ignore = true;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_WARN("%s", e.getMessage().c_str());
	}
}

void send_new_freq(long freq)
{
	try {
		xmlvfo.freq = freq;
		XmlRpcValue f((double)freq), res;
		execute(rig_set_frequency, f, res);
		ignore = true;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_WARN("%s", e.getMessage().c_str());
	}
}

void send_new_notch(int freq)
{
	try {
		XmlRpcValue i(freq), res;
		execute(rig_set_notch, i, res);
		ignore = true;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_WARN("%s", e.getMessage().c_str());
	}
}

void send_xml_freq(long freq)
{
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
		ignore = true;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_WARN("%s", e.getMessage().c_str());
	}
}

void send_new_bandwidth(int bw)
{
	if (!selrig->bandwidths_ || selrig->has_dsp_controls) return;
	try {
		xmlvfo.iBW = bw;
		XmlRpcValue bandwidth(selrig->bandwidths_[bw]), res;
		execute(rig_set_bandwidth, bandwidth, res);
		ignore = true;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_WARN("%s", e.getMessage().c_str());
	}
}

void send_sideband()
{
	try {
		XmlRpcValue sideband(selrig->get_modetype(vfo.imode) == 'U' ? "USB" : "LSB"), res;
		execute(main_set_wf_sideband, sideband, res);
		ignore = true;
	} catch (const XmlRpc::XmlRpcException& e) {
		if (XML_DEBUG)
			LOG_WARN("%s", e.getMessage().c_str());
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
			LOG_WARN("%s", txstate.c_str());
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
		return;
	}

	if (!selrig->bandwidths_ || vfo.iBW == -1 || selrig->has_dsp_controls)
		return;
// out-of-bounds check for instance where bandwidths_ has changed
	int t = 0;
	while (selrig->bwtable(xmlvfo.imode)[t] != NULL)  t++;
	if (xmlvfo.iBW >= t) return;	// serivceAB will set inbounds if needed

	string sbw = new_bw;
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

static void check_for_notch_change(const XmlRpcValue& new_notch)
{
	if (!selrig->has_notch_control) return;
	int ntch = new_notch;
	int ntch_step = sldrNOTCH->step();
	if ((!ntch && progStatus.notch) || (ntch && (ntch / ntch_step != progStatus.notch_val / ntch_step)))
		Fl::awake(setNotchControl, (void*)ntch);
}

static void push2que()
{
if (XML_DEBUG)
	LOG_WARN("%s", print(xmlvfo));
	if (useB) {
		if (!queB.empty()) return;
		pthread_mutex_lock(&mutex_queA);
		queB.push(xmlvfo);
		pthread_mutex_unlock(&mutex_queA);
	} else {
		if (!queA.empty()) return;
		pthread_mutex_lock(&mutex_queB);
		queA.push(xmlvfo);
		pthread_mutex_unlock(&mutex_queB);
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
		send_modes();
		send_bandwidths();

		if (!useB) xmlvfo = vfoA;
		else       xmlvfo = vfoB;

		send_new_mode(xmlvfo.imode);

		send_new_bandwidth(xmlvfo.iBW);

		send_sideband();

		send_new_freq(xmlvfo.freq);

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
	try {
		execute("system.multicall", *status_query, status);
		if (ignore) {
			ignore = false;
			return;
		}
		check_for_ptt_change(status[0][0]);
		check_for_notch_change(status[4][0]);
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
		throw;
	}
}

void * digi_loop(void *d)
{
	int try_count = CHECK_UPDATE_COUNT;
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
				LOG_WARN("%s", e.getMessage().c_str());
			fldigi_online = false;
			rig_reset = false;
			try_count = CHECK_UPDATE_COUNT;
			Fl::awake(set_fldigi_connect, (void *)0);
		}
		pthread_mutex_unlock(&mutex_xmlrpc);
	}
	return NULL;
}

