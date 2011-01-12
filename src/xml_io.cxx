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
bool bypass_digi_loop = true;
bool wait_query = false;
bool fldigi_online = false;
bool rig_reset = false;
bool xml_query = false;
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
void snf(void *)
{
	auto_mutex lock(mutex_xmlrpc);
	try {
		XmlRpcValue freq((double)xmlvfo.freq), res;
		execute(rig_set_frequency, freq, res);
	}
	catch (...) { }
}

void send_new_freq(long freq)
{
	xml_query = false;
	if (!fldigi_online) return;
	xmlvfo.freq = freq;
	Fl::awake(snf);
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

void snm(void *)
{
	auto_mutex lock(mutex_xmlrpc); 
	try { 
		send_modes_e(); 
	} catch (...) { } 
}

void send_modes() { 
	xml_query = false;
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
	execute(rig_set_bandwidths, bandwidths, res);
}

static void send_bandwidths_e() 
{
	send_bandwidths_(selrig->bandwidths_);
}

void sbs(void *)
{
	auto_mutex lock(mutex_xmlrpc);
	try { 
		send_bandwidths_e(); 
	} catch (...) { } 
}

void send_bandwidths()
{
	xml_query = false;
	if (!fldigi_online) return;
	Fl::awake(sbs);
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
		ptt_on = PTT;
	} catch (...) { }
}

void smc(void *m)
{
	int md = (int)(reinterpret_cast<long>(m));
	auto_mutex lock(mutex_xmlrpc);
	try {
		XmlRpcValue mode(selrig->modes_[md]), res;
		execute(rig_set_mode, mode, res);
	} catch (...) { }
}

void send_new_mode(int m)
{
	xml_query = false;
	if (!fldigi_online || !selrig->modes_) return;
	Fl::awake(smc, (void *)m);
}

void sbc(void *val)
{
	int bw = (int)(reinterpret_cast<long>(val));
	auto_mutex lock(mutex_xmlrpc);
	try {
		XmlRpcValue bandwidth(selrig->bandwidths_[bw]), res;
		execute(rig_set_bandwidth, bandwidth, res);
	} catch (...) { }
}

void send_new_bandwidth(int bw)
{
	xml_query = false;
	if (!fldigi_online || !selrig->bandwidths_) return;
	Fl::awake(sbc, (void*)bw);
}

void ssb(void *)
{
	auto_mutex lock(mutex_xmlrpc);
	try {
		XmlRpcValue sideband(selrig->get_modetype(vfo.imode) == 'U' ? "USB" : "LSB"), res;
		execute(main_set_wf_sideband, sideband, res);
	} catch (...) { }
}

void send_sideband()
{
	if (!fldigi_online) return;
	Fl::awake(ssb);
}

// --------------------------------------------------------------------
// receive functions
// --------------------------------------------------------------------


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
		if (useB) {
			queB.push(xmlvfo);
			Fl::awake(setFreqDispB, (void *)xmlvfo.freq);
		} else {
			queA.push(xmlvfo);
			Fl::awake(setFreqDispA, (void *)xmlvfo.freq);
		}
	}
}

static void update_mode_change(void *d)
{
	int imode = (int)(reinterpret_cast<long>(d));
	vfo.imode = imode;
	useB ? vfoB.imode = imode : vfoA.imode = imode;

	pthread_mutex_lock(&mutex_serial);
	if (selrig->restore_mbw) {
		selrig->set_bwA(selrig->last_bw);
		selrig->last_bw = vfo.iBW;
	}
	if (selrig->adjust_bandwidth(vfo.imode) != -1) {
		if (!useB) {
			selrig->set_modeA(vfo.imode);
			selrig->set_bwA(vfo.iBW);
		} else {
			selrig->set_modeB(vfo.imode);
			selrig->set_bwB(vfo.iBW);
		}
	} else
		if (!useB)
			selrig->set_modeA(vfo.imode);
		else
			selrig->set_modeB(vfo.imode);
	pthread_mutex_unlock(&mutex_serial);

	opMODE->index(imode);
	send_bandwidths_e();
	updateBandwidthControl(NULL);
	return;
}

static void check_for_mode_change(const XmlRpcValue& new_mode)
{
	if (!selrig->modes_) 
		return;
	string smode = new_mode;
	if (smode != selrig->modes_[vfo.imode]) {
		long imode = 0;
		while (selrig->modes_[imode] != NULL && smode != selrig->modes_[imode])
			imode++;
		if (selrig->modes_[imode] != NULL && imode != vfo.imode)
			Fl::awake(update_mode_change, (void *)imode);
	}
}

static void update_bandwidth_change(void *d)
{
	int ibw = (int)(reinterpret_cast<long>(d));
	vfo.iBW = useB ? vfoB.iBW = ibw : vfoA.iBW = ibw;
	selrig->set_bwA(ibw);
	opBW->index(ibw);
	return;
}


static void check_for_bandwidth_change(const XmlRpcValue& new_bw)
{
	if (!selrig->bandwidths_)
		return;

	string sbw = new_bw;
	if (sbw != selrig->bandwidths_[vfo.iBW]) {
		int ibw = 0;
		while (selrig->bandwidths_[ibw] != NULL && sbw != selrig->bandwidths_[ibw])
			ibw++;
		if (selrig->bandwidths_[ibw] != NULL && ibw != vfo.iBW)
			Fl::awake(update_bandwidth_change, (void *)ibw);
	}
}

#define REG_UPDATE_INTERVAL  50 // milliseconds
#define CHECK_UPDATE_COUNT   (5000 / REG_UPDATE_INTERVAL)

static void send_rig_info()
{
	XmlRpcValue res;
	try {
		execute(rig_take_control, XmlRpcValue(), res);
		execute(rig_set_name, selrig->name_, res);
		if (selrig->bandwidths_) send_bandwidths_e();
		send_modes_e();
//MilliSleep(200);

//printf("fldigi init:\nvfo %ld, mode %s, bandwidth %s\n",
//vfo.freq, 
//selrig->modes_[vfo.imode],
//selrig->bandwidths_ ? selrig->bandwidths_[vfo.iBW] : "none");

		execute(rig_set_frequency, (double)vfo.freq, res);
		execute(rig_set_mode, selrig->modes_[vfo.imode], res);
		if (selrig->bandwidths_)
			execute(rig_set_bandwidth, selrig->bandwidths_[vfo.iBW], res);

		XmlRpcValue sideband(selrig->get_modetype(vfo.imode) == 'U' ? "USB" : "LSB");
		execute(main_set_wf_sideband, sideband, res);

		fldigi_online = true;
		rig_reset = false;
	} catch (...) { }
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
		if (!ptt_on && xml_query) {
			check_for_mode_change(status[2][0]);
			check_for_bandwidth_change(status[3][0]);
			check_for_frequency_change(status[1][0]);
		}
	} catch (...) {
	}
	xml_query = false;
}

void * digi_loop(void *d)
{
	int try_count = CHECK_UPDATE_COUNT;
	for (;;) {
		MilliSleep(REG_UPDATE_INTERVAL);
		if (!run_digi_loop) break;
		if (bypass_digi_loop) continue;
		pthread_mutex_lock(&mutex_xmlrpc);
		try {
			if (rig_reset || (!fldigi_online && !try_count--))
				send_rig_info();
			else get_all_status();
			if (!run_digi_loop) break;
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

