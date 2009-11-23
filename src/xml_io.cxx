//======================================================================
// xml_io.cxx
//
// copyright 2009, W1HKJ
//
//======================================================================

#include <cstdlib>
#include <string>
#include <iostream>

#include <FL/Fl.H>
#include <FL/fl_ask.H>

#include "xml_io.h"
#include "support.h"
#include "debug.h"
#include "dialogs.h"

using namespace std;

bool xmlrpc_query = false;
bool wait_query = false;

// these are get only
string const fldigi_get_trx_status("main.get_trx_state");

// these are set only
string const fldigi_set_rig_name("main.set_rig_name");
string const fldigi_set_rig_modes("main.set_rig_modes");
string const fldigi_set_rig_bandwidths("main.set_rig_bandwidths");
string const fldigi_set_tx("main.tx");
string const fldigi_set_rx("main.rx");

// these are get/set
string const fldigi_get_frequency("main.get_frequency");
string const fldigi_set_frequency("main.set_rig_frequency");
string const fldigi_get_sideband("main.get_sideband");
string const fldigi_set_sideband("main.set_sideband");
string const fldigi_set_rig_mode("main.set_rig_mode");
string const fldigi_get_rig_mode("main.get_rig_mode");
string const fldigi_set_rig_bandwidth("main.set_rig_bandwidth");
string const fldigi_get_rig_bandwidth("main.get_rig_bandwidth");

xmlrpc_c::clientXmlTransport_curl   xml_transport;
xmlrpc_c::client_xml				xml_client(&xml_transport);
xmlrpc_c::carriageParm_curl0 *xml_cp;
string xmlrpc_server_addr;

bool post_freq_			= false;
bool post_modes_		= false;
bool post_bandwidths_   = false;
bool post_name_			= false;
bool post_ptt_			= false;
bool txrx				= false;
bool post_mode_			= false;
bool post_sideband_		= false;
bool post_bandwidth_	= false;

void post_freq();
void post_bandwidth();
void post_bandwidths();
void post_mode();
void post_modes();
void post_name();
void post_ptt();
void post_sideband();

bool wait_mode = false;
bool wait_BW = false;

void send_new_freq()
{
	post_freq_ = true;
}

void send_modes()
{
	post_modes_ = true;
}

void send_bandwidths()
{
	post_bandwidths_ = true;
}

void send_name()
{
	post_name_ = true;
}

void send_ptt_changed(bool on)
{
	post_ptt_ = true;
	txrx = on;
}

void send_mode_changed()
{
	wait_mode = true;
	post_mode_ = true;
}

void send_sideband()
{
	post_sideband_ = true;
}

void send_bandwidth_changed()
{
	post_bandwidth_ = true;
}

void send_rig_info()
{
	send_name();
	send_modes();
	send_bandwidths();
}

//----------------------------------------------------------------------
// functions called by the xmlprc thread
//----------------------------------------------------------------------

void post_freq()
{
	post_freq_ = false;
	double oldfreq, newfreq = vfoA.freq;
	try {
		xmlrpc_c::paramList params;
		params.add(xmlrpc_c::value_double(newfreq));
		xmlrpc_c::rpcPtr rpc(new xmlrpc_c::rpc(fldigi_set_frequency, params));

		rpc->call(&xml_client, xml_cp);
		if (rpc->isSuccessful())
			oldfreq = xmlrpc_c::value_double(rpc->getResult());
	} catch (...) {
		throw;
	}
}

void post_modes()
{
	post_modes_ = false;
	vector<xmlrpc_c::value> modes;
	modes.reserve(rigmodes_.size());
	for (vector<string>::const_iterator i = rigmodes_.begin(); i != rigmodes_.end(); ++i)
	modes.push_back(xmlrpc_c::value_string(*i));

	try {
		xmlrpc_c::paramList params(1);
		params.add(xmlrpc_c::value_array(modes));
		xmlrpc_c::rpcPtr rpc(new xmlrpc_c::rpc(fldigi_set_rig_modes, params));

		rpc->call(&xml_client, xml_cp);
	} catch (...) {
		throw;
	}
}

void post_bandwidths()
{
	post_bandwidths_ = false;
	wait_BW = true;
	vector<xmlrpc_c::value> bws;
	bws.reserve(rigbws_.size());
	for (vector<string>::const_iterator i = rigbws_.begin(); i != rigbws_.end(); ++i)
	bws.push_back(xmlrpc_c::value_string(*i));

	try {
		xmlrpc_c::paramList params(1);
		params.add(xmlrpc_c::value_array(bws));
		xmlrpc_c::rpcPtr rpc(new xmlrpc_c::rpc(fldigi_set_rig_bandwidths, params));

		rpc->call(&xml_client, xml_cp);
	} catch (...) {
		throw;
	}
}

void post_name()
{
	post_name_ = false;
	try {
		xmlrpc_c::paramList params;
		params.add(xmlrpc_c::value_string(selrig->name_));
		xmlrpc_c::rpcPtr rpc(new xmlrpc_c::rpc(fldigi_set_rig_name, params));

		rpc->call(&xml_client, xml_cp);
	} catch (...) {
		throw;
	}
}

void post_ptt()
{
	post_ptt_ = false;
	bool on = txrx;
	try {
		xmlrpc_c::paramList params;
		if (on) {
			xmlrpc_c::rpcPtr rpc(new xmlrpc_c::rpc(fldigi_set_tx, params));

			rpc->call(&xml_client, xml_cp);
		} else {
			xmlrpc_c::rpcPtr rpc(new xmlrpc_c::rpc(fldigi_set_rx, params));

			rpc->call(&xml_client, xml_cp);
		}
	} catch (...) {
		throw;
	}
}

void post_mode()
{
	post_mode_ = false;
	try {
		xmlrpc_c::paramList params;
		params.add(xmlrpc_c::value_string(selrig->modes_[vfoA.imode]));
		xmlrpc_c::rpcPtr rpc(new xmlrpc_c::rpc(fldigi_set_rig_mode, params));

		rpc->call(&xml_client, xml_cp);
		wait_mode = false;
	} catch (...) {
		throw;
	}
}

void post_sideband()
{
	post_sideband_ = false;
	try {
		xmlrpc_c::paramList params;
		params.add(xmlrpc_c::value_string(
			selrig->get_modetype(vfoA.imode) == 'U' ? "USB" : "LSB" ) );
		xmlrpc_c::rpcPtr rpc(new xmlrpc_c::rpc(fldigi_set_sideband, params));

		rpc->call(&xml_client, xml_cp);
	} catch (...) {
		throw;
	}
}

void post_bandwidth()
{
	post_bandwidth_ = false;
	wait_BW = false;
	if (!selrig->bandwidths_)
		return;
	try {
		xmlrpc_c::paramList params;
		params.add(xmlrpc_c::value_string(selrig->bandwidths_[vfoA.iBW]));
		xmlrpc_c::rpcPtr rpc(new xmlrpc_c::rpc(fldigi_set_rig_bandwidth, params));

		rpc->call(&xml_client, xml_cp);
	} catch (...) {
		throw;
	}
}

void setvfo(void *d)
{
	int newfreq = (long)d;
	pthread_mutex_lock(&mutex_serial);
		vfoA.freq = newfreq;
		FreqDisp->value(newfreq);
		selrig->set_vfoA(newfreq);
	pthread_mutex_unlock(&mutex_serial);
}

void check_for_frequency_change()
{
	int newfreq = 5000000;
	try {
		xmlrpc_c::paramList params;
		xmlrpc_c::rpcPtr rpc(new xmlrpc_c::rpc(fldigi_get_frequency, params));

		rpc->call(&xml_client, xml_cp);
		if (rpc->isSuccessful()) {
			newfreq = (int)xmlrpc_c::value_double(rpc->getResult());
			if (newfreq != vfoA.freq) {
				Fl::awake(setvfo, (void *)newfreq);
			}
		}
	} catch (...) {
		throw;
	}
}

static bool ptt_now_is = false;
void check_for_ptt_change()
{
	string ptt_state;
	try {
		xmlrpc_c::paramList params;
		xmlrpc_c::rpcPtr rpc(new xmlrpc_c::rpc(fldigi_get_trx_status, params));

		rpc->call(&xml_client, xml_cp);
		if (rpc->isSuccessful()) {
			ptt_state = xmlrpc_c::value_string(rpc->getResult());
			bool nuptt = false;
			if (ptt_state.compare("TX") == 0) nuptt = true;
			if (nuptt != ptt_now_is) {
				ptt_now_is = nuptt;
				Fl::awake( setPTT, (void*)nuptt);
			}
		}
	} catch (...) {
		throw;
	}
}

static void updateModeControl(void *d)
{
	wait_mode = true;
	pthread_mutex_lock(&mutex_serial);
		opMODE->index(vfoA.imode);
		selrig->set_mode(vfoA.imode);
		updateBandwidthControl();
		send_bandwidths();
		send_bandwidth_changed();
		send_sideband();
	pthread_mutex_unlock(&mutex_serial);
	wait_mode = false;
	wait_BW = false;
}

void check_for_mode_change()
{
	if (wait_mode) return;
	string new_mode;
	try {
		xmlrpc_c::paramList params;
		xmlrpc_c::rpcPtr rpc(new xmlrpc_c::rpc(fldigi_get_rig_mode, params));

		rpc->call(&xml_client, xml_cp);
		if (rpc->isSuccessful()) {
			new_mode = xmlrpc_c::value_string(rpc->getResult());
			if (new_mode != selrig->modes_[vfoA.imode]) {
				int imode = 0;
				while (selrig->modes_[imode] != NULL &&
					new_mode != selrig->modes_[imode]) imode++;
				if (selrig->modes_[imode] != NULL) {
					vfoA.imode = imode;
					wait_BW = true;
					Fl::awake(updateModeControl, (void*)imode);
					return;
				}
			}
		}
	} catch (...) {
		throw;
	}
}

void updateBW(void *d)
{
	vfoA.iBW = (long)d;
	wait_BW = true;
	pthread_mutex_lock(&mutex_serial);
		opBW->index(vfoA.iBW);
		selrig->set_bandwidth(vfoA.iBW);
	pthread_mutex_unlock(&mutex_serial);
	wait_BW = false;
}

void check_for_bandwidth_change()
{
	if (wait_BW) return;
	if (!selrig->bandwidths_) return;

	string new_bw;
	try {
		xmlrpc_c::paramList params;
		xmlrpc_c::rpcPtr rpc(new xmlrpc_c::rpc(fldigi_get_rig_bandwidth, params));

		rpc->call(&xml_client, xml_cp);
		if (rpc->isSuccessful()) {
			new_bw = xmlrpc_c::value_string(rpc->getResult());
			if (new_bw != selrig->bandwidths_[vfoA.iBW]) {
				int ibw = 0;
				while (selrig->bandwidths_[ibw] != NULL &&
					new_bw != selrig->bandwidths_[ibw]) ibw++;
				if (selrig->bandwidths_[ibw] != NULL)
					Fl::awake(updateBW, (void*)ibw);
			}
		}
	} catch (...) {
		throw;
	}
}

//----------------------------------------------------------------------
// xmlrpc idle function
// this function is called by the main fltk processing loop
//----------------------------------------------------------------------

bool xmlrpc_disabled = false;
int loop_count = 0;

void xmlrpc_loop(void *d)
{
	if (!xmlrpc_query) return;
	if (xmlrpc_disabled) return;

	try {
		if (post_ptt_) post_ptt();
		if (post_freq_) post_freq();
		if (post_modes_) post_modes();
		if (post_bandwidths_) post_bandwidths();
		if (post_mode_) post_mode();
		if (post_bandwidth_) post_bandwidth();
		if (post_sideband_) post_sideband();
		if (post_name_) post_name();
	} catch (const girerr::error &e) {
#ifdef __WIN32__
		cbExit();
#else
		fl_alert("fldigi connection failed");
		LOG_ERROR("xmlrpc post : %s", e.what());
#endif
	} catch (...) {
#ifdef __WIN32__
		cbExit();
#else
		fl_alert("fldigi connection failed during post");
		LOG_ERROR("fldigi connection failed during post");
#endif
	}

	try {
		if (!wait_query) {
			if (!localptt)
				check_for_ptt_change();
			if (loop_count == 0) check_for_frequency_change();
			if (loop_count == 1) check_for_mode_change();
			if (loop_count == 2) check_for_bandwidth_change();
			loop_count++;
			if (loop_count == 3) loop_count = 0;
		}
	} catch (const girerr::error &e) {
#ifdef __WIN32__
		cbExit();
#else
		fl_alert("fldigi connection failed");
		cbExit();
#endif
	} catch (...) {
#ifdef __WIN32__
		cbExit();
#else
		fl_alert("fldigi connection failed during query");
		cbExit();
#endif
	}

	Fl::repeat_timeout(0.10, xmlrpc_loop);
}

void init_xmlrpc()
{
xmlrpc_server_addr = "http://localhost:";
xmlrpc_server_addr.append(progStatus.server_port);
xmlrpc_server_addr.append("/RPC2");
static ::xmlrpc_c::carriageParm_curl0 addr_xml_cp(xmlrpc_server_addr);
xml_cp = &addr_xml_cp;

	try {
		xmlrpc_c::paramList params;
		params.add(xmlrpc_c::value_string(selrig->name_));
		xmlrpc_c::rpcPtr rpc(new xmlrpc_c::rpc(fldigi_set_rig_name, params));
		rpc->call(&xml_client, xml_cp);
		xmlrpc_query = true;

		Fl::add_timeout(0.05, xmlrpc_loop);

	} catch (const girerr::error& e) {
		xmlrpc_disabled = true;
		LOG_ERROR("xmlrpc queries disabled\n%s", e.what());
		fl_alert("fldigi not available\nusing stand-a-lone operation");
	}
}
