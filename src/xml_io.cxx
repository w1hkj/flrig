//======================================================================
// xml_io.cxx
//
// copyright 2009, W1HKJ
//
//======================================================================

#include <cstdlib>
#include <string>

#include "support.h"
#include "debug.h"
#include "rig.h"

#include "rigbase.h"

#include "XmlRpc.h"

using namespace std;
using XmlRpc::XmlRpcValue;

static const double TIMEOUT = 0.5;//2.0;
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
bool wait_query = false;
bool fldigi_online = false;
bool rig_reset = false;

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
void send_new_freq()
{
	if (!fldigi_online) return;
	auto_mutex lock(mutex_xmlrpc);
	try {
		XmlRpcValue freq((double)vfoA.freq), res;
		execute(rig_set_frequency, freq, res);
	}
	catch (...) { }
}

static void send_modes_(const char** md_array)
{
	if (!md_array)
		return;
		XmlRpcValue modes, res;
	int i = 0;
	for (const char** mode = md_array; *mode; mode++, i++)
		modes[0][i] = *mode;
	execute(rig_set_modes, modes, res);
}

static void send_modes_e() 
{
	send_modes_(selrig->modes_);
}

void send_modes() { 
	if (!fldigi_online) return;
	auto_mutex lock(mutex_xmlrpc); 
	try { 
		send_modes_e(); 
	} catch (...) { } 
}

static void send_bandwidths_(const char** bw_array)
{
	if (!bw_array)
		return;
	XmlRpcValue bandwidths, res;
	int i = 0;
	for (const char** bw = bw_array; *bw; bw++, i++)
		bandwidths[0][i] = *bw;
	execute(rig_set_bandwidths, bandwidths, res);
}

static void send_bandwidths_e() 
{
	send_bandwidths_(selrig->bandwidths_);
}

void send_bandwidths()
{
	if (!fldigi_online) return;
	auto_mutex lock(mutex_xmlrpc);
	try { 
		send_bandwidths_e(); 
	} catch (...) { } 
}

void send_name()
{
	if (!fldigi_online) return;
	auto_mutex lock(mutex_xmlrpc);
	try {
		XmlRpcValue res;
		execute(rig_set_name, XmlRpcValue(selrig->name_), res);
	} catch (...) { }
}

void send_ptt_changed(bool PTT)
{
	if (!fldigi_online) return;
	auto_mutex lock(mutex_xmlrpc);
	try {
		XmlRpcValue res;
		execute((PTT ? main_set_tx : main_set_rx), XmlRpcValue(), res);
	} catch (...) { }
}

void send_mode_changed()
{
	if (!fldigi_online) return;
	if (!selrig->modes_)
		return;
	auto_mutex lock(mutex_xmlrpc);
	try {
		XmlRpcValue mode(selrig->modes_[vfoA.imode]), res;
		execute(rig_set_mode, mode, res);
	} catch (...) { }
}

void send_bandwidth_changed()
{
	if (!fldigi_online) return;
	if (!selrig->bandwidths_)
		return;
	auto_mutex lock(mutex_xmlrpc);
	try {
		XmlRpcValue bandwidth(selrig->bandwidths_[vfoA.iBW]), res;
		execute(rig_set_bandwidth, bandwidth, res);
	} catch (...) { }
}

void send_sideband()
{
	if (!fldigi_online) return;
	auto_mutex lock(mutex_xmlrpc);
	try {
		XmlRpcValue sideband(selrig->get_modetype(vfoA.imode) == 'U' ? "USB" : "LSB"), res;
		execute(main_set_wf_sideband, sideband, res);
	} catch (...) { }
}

// --------------------------------------------------------------------
// receive functions
// --------------------------------------------------------------------

static bool ptt_now_is = false;
static void check_for_ptt_change(const XmlRpcValue& trx_state)
{
	bool nuptt = trx_state == "TX";
	if (nuptt != ptt_now_is) {
		ptt_now_is = nuptt;
		Fl::awake(setPTT, (void*)nuptt);
	}
}

bool updatingVFO = false;

void setvfo(void *d)
{
	long newfreq = (long)d;
	if (XML_DEBUG)
		LOG_INFO("%ld", newfreq);
	pthread_mutex_lock(&mutex_serial);
		selrig->set_vfoA(newfreq);
	pthread_mutex_unlock(&mutex_serial);
	vfoA.freq = newfreq;
	FreqDisp->value(newfreq);
	updatingVFO = false;
}

static void check_for_frequency_change(const XmlRpcValue& freq)
{
	if (updatingVFO) return;
	double f = freq;
	long newfreq = (long)f;
	if (newfreq == vfoA.freq || updatingVFO == true || !f) return;
	updatingVFO = true;
	Fl::awake(setvfo, (void *)newfreq);
}

bool updatingMode = false;

static void updateModeControl(void *d)
{
	if (!run_digi_loop) return;
	int md = (long)d;
	if (XML_DEBUG)
		LOG_INFO("%d", md);
	pthread_mutex_lock(&mutex_serial);
		vfoA.imode = md;
		opMODE->index(vfoA.imode);
		selrig->set_modeA(vfoA.imode);
		updateBandwidthControl();
		selrig->set_bwA(vfoA.iBW);
	pthread_mutex_unlock(&mutex_serial);
	send_sideband();
	send_bandwidths();
	send_bandwidth_changed();
	updatingMode = false;
}

static void check_for_mode_change(const XmlRpcValue& new_mode)
{
	if (!selrig->modes_ || updatingMode) 
		return;

	if (new_mode != selrig->modes_[vfoA.imode]) {
		long imode = 0;
		while (selrig->modes_[imode] != NULL && new_mode != selrig->modes_[imode])
			imode++;
		if (selrig->modes_[imode] != NULL) {
			updatingMode = true;
			Fl::awake(updateModeControl, (void*)imode);
			return;
		}
	}
}

bool updatingBW = false;

void updateBW(void *d)
{
	if (!run_digi_loop) return;
	pthread_mutex_lock(&mutex_serial);
		vfoA.iBW = (long)d;
		opBW->index(vfoA.iBW);
		selrig->set_bwA(vfoA.iBW);
	pthread_mutex_unlock(&mutex_serial);
	updatingBW = false;
	if (XML_DEBUG)
		LOG_INFO("%d", vfoA.iBW);
}

static void check_for_bandwidth_change(const XmlRpcValue& new_bw)
{
	if (!selrig->bandwidths_ || updatingBW || updatingMode)
		return;

	if (new_bw != selrig->bandwidths_[vfoA.iBW]) {
		long ibw = 0;
		while (selrig->bandwidths_[ibw] != NULL && new_bw != selrig->bandwidths_[ibw])
			ibw++;
		if (selrig->bandwidths_[ibw] != NULL) {
			updatingBW = true;
			Fl::awake(updateBW, (void*)ibw);
		}
	}
}

#define REG_UPDATE_INTERVAL  50 // milliseconds
#define CHECK_UPDATE_COUNT   (1000 / REG_UPDATE_INTERVAL)

static void send_rig_info()
{
	XmlRpcValue res;
	rig_reset = false;
	execute(rig_take_control, XmlRpcValue(), res);
	execute(rig_set_name, selrig->name_, res);
	send_bandwidths_e();
	send_modes_e();
MilliSleep(200);

	execute(rig_set_frequency, (double)vfoA.freq, res);
	execute(rig_set_mode, selrig->modes_[vfoA.imode], res);
	execute(rig_set_bandwidth, selrig->bandwidths_[vfoA.iBW], res);

	XmlRpcValue sideband(selrig->get_modetype(vfoA.imode) == 'U' ? "USB" : "LSB");
	execute(main_set_wf_sideband, sideband, res);

	fldigi_online = true;
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
	execute("system.multicall", *status_query, status);
	check_for_ptt_change(status[0][0]);
	check_for_frequency_change(status[1][0]);
	check_for_mode_change(status[2][0]);
	check_for_bandwidth_change(status[3][0]);
}

void * digi_loop(void *d)
{
	int try_count = CHECK_UPDATE_COUNT;
	for (;;) {
		MilliSleep(REG_UPDATE_INTERVAL);
		if (!run_digi_loop) break;
		pthread_mutex_lock(&mutex_xmlrpc);
		try {
			if (rig_reset) send_rig_info();
			else if (!fldigi_online) {
				if (!wait_query && !try_count--) send_rig_info();
			} else {
 				if (!wait_query) get_all_status();
				if (!run_digi_loop) break;
			}
		} catch (const XmlRpc::XmlRpcException& e) {
			if (XML_DEBUG)
				LOG_INFO("%s", e.getMessage().c_str());
			try_count = CHECK_UPDATE_COUNT;
			fldigi_online = false;
		}
		pthread_mutex_unlock(&mutex_xmlrpc);

	}
	return NULL;
}

