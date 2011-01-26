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

extern queue<FREQMODE> queA;
extern queue<FREQMODE> queB;
extern queue<bool> quePTT;

static const double TIMEOUT = 0.5;
static const char* tcpip_address = "127.0.0.1";
static const int tcpip_port      = 7362;

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


static XmlRpc::XmlRpcClient* client;
static XmlRpcValue* status_query;

bool run_digi_loop = true;
bool bypass_digi_loop = true;
//bool wait_query = false;
bool fldigi_online = false;
bool rig_reset = false;
bool ptt_on = false;

class auto_mutex
{
	pthread_mutex_t& mutex;
	auto_mutex(const auto_mutex& m);
	auto_mutex& operator=(const auto_mutex& m);
public:
	auto_mutex(pthread_mutex_t& m) : mutex(m) { pthread_mutex_lock(&mutex); }
	~auto_mutex(void) { pthread_mutex_unlock(&mutex); }
};

//=====================================================================
// socket ops
//=====================================================================

void open_rig_xmlrpc()
{
	client = new XmlRpc::XmlRpcClient(tcpip_address, tcpip_port);
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
	delete client;
	client = NULL;
	delete status_query;
	status_query = NULL;
}

static inline void execute(const char* name, const XmlRpcValue& param, XmlRpcValue& result)
{
	if (!client->execute(name, param, result, TIMEOUT))
		throw XmlRpc::XmlRpcException(name);
}

// --------------------------------------------------------------------
// send functions
// --------------------------------------------------------------------
void snf(void *d)
{
	long f = (reinterpret_cast<long>(d));
//	auto_mutex lock(mutex_xmlrpc);
	try {
		XmlRpcValue freq((double)f), res;
		execute(rig_set_frequency, freq, res);
		xmlvfo.freq = f;
	}
	catch (...) { 
		fldigi_online = false;
	}
}

void send_new_freq(long freq)
{
	if (!fldigi_online) return;
	Fl::awake(snf, (void*)freq);
}

static void send_modes_(const char** md_array)
{
	if (!md_array)
		return;
		XmlRpcValue modes, res;
	int i = 0;
	for (const char** mode = md_array; *mode; mode++, i++)
		modes[0][i] = *mode;
//	auto_mutex lock(mutex_xmlrpc);
	try {
		execute(rig_set_modes, modes, res);
	} catch (...) {
		fldigi_online = false;
	}
}

static void send_modes_e() 
{
	send_modes_(selrig->modes_);
}

void snm(void *)
{
	send_modes_e(); 
}

void send_modes() { 
	if (!fldigi_online) return;
	Fl::awake(snm);
}

static void send_bandwidths_(const char** bw_array)
{
	if (!bw_array)
		return;
	XmlRpcValue bandwidths, res;
	int i = 0;
	for (const char** bw = bw_array; *bw; bw++, i++)
		bandwidths[0][i] = *bw;
//	auto_mutex lock(mutex_xmlrpc);
	try {
		execute(rig_set_bandwidths, bandwidths, res);
	} catch (...) {
		fldigi_online = false;
	}
}

static void send_bandwidths_e() 
{
	send_bandwidths_(selrig->bandwidths_);
}

void sbs(void *)
{
	send_bandwidths_e(); 
}

void send_bandwidths()
{
	if (!fldigi_online) return;
	Fl::awake(sbs);
}

void send_name()
{
	if (!fldigi_online) return;
//	auto_mutex lock(mutex_xmlrpc);
	try {
		XmlRpcValue res;
		execute(rig_set_name, XmlRpcValue(selrig->name_), res);
	} catch (...) { }
}

void send_ptt_changed(bool PTT)
{
	if (!fldigi_online) return;
//	auto_mutex lock(mutex_xmlrpc);
	try {
		XmlRpcValue res;
		execute((PTT ? main_set_tx : main_set_rx), XmlRpcValue(), res);
		ptt_on = PTT;
	} catch (...) { 
		fldigi_online = false;
	}
}

void smc(void *m)
{
	int md = (int)(reinterpret_cast<long>(m));
//	auto_mutex lock(mutex_xmlrpc);
	try {
		XmlRpcValue mode(selrig->modes_[md]), res;
		execute(rig_set_mode, mode, res);
		xmlvfo.imode = md;
	} catch (...) {
		fldigi_online = false;
	}
}

void send_new_mode(int m)
{
	if (!fldigi_online || !selrig->modes_) return;
	Fl::awake(smc, (void *)m);
}

void sbc(void *val)
{
	int bw = (int)(reinterpret_cast<long>(val));
//	auto_mutex lock(mutex_xmlrpc);
	try {
		XmlRpcValue bandwidth(selrig->bandwidths_[bw]), res;
		execute(rig_set_bandwidth, bandwidth, res);
		xmlvfo.iBW = bw;
	} catch (...) {
		fldigi_online = false;
	}
}

void send_new_bandwidth(int bw)
{
	if (!fldigi_online || !selrig->bandwidths_) return;
	Fl::awake(sbc, (void*)bw);
}

void ssb(void *)
{
//	auto_mutex lock(mutex_xmlrpc);
	try {
		XmlRpcValue sideband(selrig->get_modetype(vfo.imode) == 'U' ? "USB" : "LSB"), res;
		execute(main_set_wf_sideband, sideband, res);
	} catch (...) { 
		fldigi_online = false;
	}
}

void send_sideband()
{
	if (!fldigi_online) return;
	Fl::awake(ssb);
}

// --------------------------------------------------------------------
// receive functions
// --------------------------------------------------------------------

bool xmlvfo_changed = false;

static void check_for_ptt_change(const XmlRpcValue& trx_state)
{
	bool nuptt = trx_state == "TX";
	if (nuptt != ptt_on) {
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

static void check_for_mode_change(const XmlRpcValue& new_mode)
{
	if (!selrig->modes_) 
		return;
	string smode = new_mode;
	if (smode != selrig->modes_[vfo.imode]) {
		int imode = 0;
		while (selrig->modes_[imode] != NULL && smode != selrig->modes_[imode])
			imode++;
		if (selrig->modes_[imode] != NULL && imode != xmlvfo.imode) {
			xmlvfo.imode = imode;
			xmlvfo_changed = true;
		}
	}
}

static void check_for_bandwidth_change(const XmlRpcValue& new_bw)
{
	if (!selrig->bandwidths_)
		return;

	string sbw = new_bw;
	if (sbw != selrig->bandwidths_[vfo.iBW]) {
		int ibw = 0;
		const char **bwt = selrig->bwtable(xmlvfo.imode);
		while (bwt[ibw] != NULL && sbw.compare(bwt[ibw]) != 0) {ibw++;}
		if (bwt[ibw] != NULL && ibw != xmlvfo.iBW) {
			xmlvfo.iBW = ibw;
			xmlvfo_changed = true;
		}
	}
}

static void push2que()
{
	if (xmlvfo_changed == false) return;
	if (useB)
		queB.push(xmlvfo);
	else
		queA.push(xmlvfo);
}

#define RIG_UPDATE_INTERVAL  50 // milliseconds
#define CHECK_UPDATE_COUNT   (2000 / RIG_UPDATE_INTERVAL)

static void send_rig_info()
{
	XmlRpcValue res;
	try {
		execute(rig_take_control, XmlRpcValue(), res);
		execute(rig_set_name, selrig->name_, res);
		if (selrig->bandwidths_) send_bandwidths_e();
		send_modes_e();

		if (!useB) xmlvfo = vfoA;
		else       xmlvfo = vfoB;

		XmlRpcValue mode(selrig->modes_[xmlvfo.imode]);
		execute(rig_set_mode, mode, res);

		if (selrig->bandwidths_) {
			XmlRpcValue bandwidth(selrig->bandwidths_[xmlvfo.iBW]);
			execute(rig_set_bandwidth, bandwidth, res);
		}

		XmlRpcValue sideband(selrig->get_modetype(xmlvfo.imode) == 'U' ? "USB" : "LSB");
		execute(main_set_wf_sideband, sideband, res);

		XmlRpcValue freq((double)xmlvfo.freq);
		execute(rig_set_frequency, freq, res);

extern char *print(FREQMODE);
LOG_INFO("%s", print(xmlvfo));

		fldigi_online = true;
		rig_reset = false;
	} catch (...) {
		throw;
	}
}

void send_no_rig()
{
	auto_mutex lock(mutex_xmlrpc);
	XmlRpcValue res;
	execute(rig_set_name, szNORIG, res);
	send_bandwidths_(szNOBWS);
	send_modes_(szNOMODES);
	execute(rig_set_mode, "USB", res);
	XmlRpcValue sideband("USB");
	execute(main_set_wf_sideband, sideband, res);
	execute(rig_release_control, XmlRpcValue(), res);
}

static void get_all_status()
{
	XmlRpcValue status;
	try {
		execute("system.multicall", *status_query, status);
		check_for_ptt_change(status[0][0]);
		if (!ptt_on) {
			xmlvfo.src = XML;
			xmlvfo_changed = false;
			check_for_mode_change(status[2][0]);
			check_for_bandwidth_change(status[3][0]);
			check_for_frequency_change(status[1][0]);
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
		if (bypass_digi_loop) continue;
		pthread_mutex_lock(&mutex_xmlrpc);
		try {
			if (rig_reset || (!fldigi_online && (--try_count == 0)))
				send_rig_info();
			else if (fldigi_online) get_all_status();
			if (!run_digi_loop) break;
		} catch (const XmlRpc::XmlRpcException& e) {
			if (XML_DEBUG)
				LOG_WARN("%s", e.getMessage().c_str());
			fldigi_online = false;
			rig_reset = false;
			try_count = CHECK_UPDATE_COUNT;
		}
		pthread_mutex_unlock(&mutex_xmlrpc);
	}
	return NULL;
}

