//======================================================================
// socket_io.cxx
//
// copyright 2009, W1HKJ
//
//======================================================================

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>

#include "support.h"
#include "debug.h"
#include "rig.h"
#include "socket.h"

#include "rigbase.h"

#ifdef __WIN32__
#  include "compat.h"
#endif

using namespace std;

string tcpip_address = "127.0.0.1";
string tcpip_port    = "7362";

// these are get only
const char * main_get_trx_state   = "main.get_trx_state";
// these are set only
const char * main_set_name        = "main.set_rig_name";
const char * main_set_modes       = "main.set_rig_modes";
const char * main_set_bandwidths  = "main.set_rig_bandwidths";
const char * main_set_tx          = "main.tx";
const char * main_set_rx          = "main.rx";
// these are get/set
const char * main_get_frequency   = "main.get_frequency";
const char * main_set_frequency   = "main.set_rig_frequency";
const char * main_get_sideband    = "main.get_sideband";
const char * main_set_sideband    = "main.set_sideband";
const char * main_set_mode        = "main.set_rig_mode";
const char * main_get_mode        = "main.get_rig_mode";
const char * main_set_bandwidth   = "main.set_rig_bandwidth";
const char * main_get_bandwidth   = "main.get_rig_bandwidth";

const char * XML_EOL    = "\r\n";
const char * XML_HEADER = "\
POST /RPC2 HTTP/1.1\r\n\
Host: localhost:7362\r\n\
Accept: */*\r\n\
Content-Type: text/xml\r\n\
Content-Length: ";

const char * XML_PROLOGUE     = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n";
const char * XML_METHODCALL   = "methodCall>";
const char * XML_METHODNAME   = "methodName>";
const char * XML_PARAM        = "param>";
const char * XML_PARAMS       = "params>";
const char * XML_INTEGER      = "i4>";
const char * XML_DOUBLE       = "double>";
const char * XML_BOOL         = "boolean>";
const char * XML_STRING       = "string>";
const char * XML_STRUCT       = "struct>";
const char * XML_NAME         = "name>";
const char * XML_MEMBER       = "member>";
const char * XML_VALUE        = "value>";
const char * XML_ARRAY        = "array>";
const char * XML_DATA         = "data>";
const char * XML_BASE64       = "base64>";
const char * XML_FAULT        = "fault>";
const char * XML_FAULT_CODE   = "faultCode>";
const char * XML_FAULT_STRING = "faultString>";
const char * XML_NIL          = "XML_NIL";

const char * XML_RESPONE      = "methodResponse>";
const char * XML_OPEN         = "<";
const char * XML_CLOSE        = "</";

string xmlout_buffer;
string xmlrep_buffer;

string extract(string buf, string what);
double val_double(string buf);
int    val_int(string buf);
string val_string(string buf);
bool   val_bool(string buf);
void   array(const char **data, string &arry);
void   payload(const char *methodname, const char *dtype, const char *data, string &content);
void   header(int len, string &hdr);

string xml_rigbws;
string xml_rigmodes;
char xml_nufreq[20];

Address *adr = NULL;
Socket  *sck = NULL;

bool run_digi_loop = true;
bool wait_query = false;
bool fldigi_online = false;

void send_xml(const char *, const char *, const char *);
void send_xml(const char *, const char *, const char *);

int  Nconnects = 0;

//=====================================================================
// socket ops
//=====================================================================

void open_rig_socket()
{
	try {
		if (!adr)
			adr = new Address(tcpip_address.c_str(), tcpip_port.c_str());
	} catch (const SocketException& e) {
		LOG_ERROR("%s", e.what());
		delete adr;
		adr = NULL;
		throw;
	}
}

void close_rig_socket()
{
	delete adr;
	adr = NULL;
}

void send_xml(const char *methodname, const char *dtype, const char *data)
{
	if (!adr) 
		try {
			open_rig_socket();
		} catch (const SocketException& e) {
			throw;
		}

	string content;
	string hdr;
	payload(methodname, dtype, data, content);
	header(content.length(), hdr);
	xmlout_buffer.clear();
	xmlout_buffer.append(hdr);
	xmlout_buffer.append(content);
	xmlrep_buffer.clear();
	try {
		sck = new Socket(*adr);
		sck->set_timeout(0.1);
		sck->connect();
		sck->send(xmlout_buffer);
		sck->recv(xmlrep_buffer);
		delete sck;
		sck = NULL;
		Nconnects++;
//		if (Nconnects % 1000 == 0)
//			LOG_WARN("Connects: %d", Nconnects);
		LOG_INFO("\n%s\n%s",
			xmlout_buffer.c_str(),
			xmlrep_buffer.c_str());
	} catch (const SocketException& e) {
//		LOG_ERROR("Connects : %d", Nconnects);
		LOG_ERROR("%s %s", methodname, e.what());
		delete sck;
		sck = NULL;
		throw;
	}
}

// --------------------------------------------------------------------
// send functions
// --------------------------------------------------------------------
#define XO(p)  append(XML_OPEN).append(p)
#define XC(p)  append(XML_CLOSE).append(p)

void array(const char **data, string &arry)
{
	arry.clear();
	arry.append("<param><value><array><data>").append(XML_EOL);
	while (*data) {
		arry.append("<value><string>").append(*data);
		arry.append("</string></value>").append(XML_EOL);
		data++;
	}
	arry.append("</data></array></value></param>").append(XML_EOL);
}

void payload(const char *methodname, const char *dtype, const char *data, string &built)
{
	built.clear();
	built.append(XML_PROLOGUE);
	built.append("<methodCall>\r\n");
	built.append("<methodName>").append(methodname).append("</methodName>\r\n");
	built.append("<params>\r\n");
	if (dtype == XML_ARRAY) built.append(data);
	else if (dtype != XML_NIL) {
		built.append("<param>\r\n");
		built.append("<value>");
		built.append("<").append(dtype).append(data).append("</").append(dtype);
		built.append("</value>\r\n");
		built.append("</param>\r\n");
	}
	built.append("</params>\r\n");
	built.append("</methodCall>\r\n");
}

void header(int len, string &hdr)
{
	stringstream slen;
	slen << len;
	hdr.clear();
	hdr.append(XML_HEADER).append(slen.str().c_str());
	hdr.append(XML_EOL).append(XML_EOL);
}

void send_new_freq()
{
	stringstream xml_freq;
	xml_freq << vfoA.freq;

	pthread_mutex_lock(&mutex_xmlrpc);
	try {
		send_xml(main_set_frequency, XML_DOUBLE, xml_freq.str().c_str());
	} catch (...) {}
	pthread_mutex_unlock(&mutex_xmlrpc);

}

void send_modes()
{
	if (!selrig->modes_) return;
	pthread_mutex_lock(&mutex_xmlrpc);
	try {
		array(selrig->modes_, xml_rigmodes);
		send_xml(main_set_modes, XML_ARRAY, xml_rigmodes.c_str());
	} catch (...) {}
	pthread_mutex_unlock(&mutex_xmlrpc);
}

void send_bandwidths()
{
	if (!selrig->bandwidths_) return;
	pthread_mutex_lock(&mutex_xmlrpc);
	try {
		array(selrig->bandwidths_, xml_rigbws);
		send_xml(main_set_bandwidths, XML_ARRAY, xml_rigbws.c_str());
	} catch (...) {}
	pthread_mutex_unlock(&mutex_xmlrpc);
}

void send_name()
{
	pthread_mutex_lock(&mutex_xmlrpc);
	try {
			send_xml(main_set_name, XML_STRING, selrig->name_);
	} catch (...) {}
	pthread_mutex_unlock(&mutex_xmlrpc);
}

void send_ptt_changed(bool PTT)
{
	pthread_mutex_lock(&mutex_xmlrpc);
	try {
		if (PTT)
			send_xml(main_set_tx, XML_NIL, "");
		else
			send_xml(main_set_rx, XML_NIL, "");
	} catch (...) {}
	pthread_mutex_unlock(&mutex_xmlrpc);
}

void send_mode_changed()
{
	if (!selrig->modes_) return;
	pthread_mutex_lock(&mutex_xmlrpc);
	try {
		send_xml(main_set_mode, XML_STRING, selrig->modes_[vfoA.imode]);
	} catch (...) {}
	pthread_mutex_unlock(&mutex_xmlrpc);
}

void send_bandwidth_changed()
{
	if (!selrig->bandwidths_) return;
	pthread_mutex_lock(&mutex_xmlrpc);
	try {
		send_xml(main_set_bandwidth, XML_STRING, selrig->bandwidths_[vfoA.iBW]);
	} catch (...) {}
	pthread_mutex_unlock(&mutex_xmlrpc);
}

void send_sideband()
{
	pthread_mutex_lock(&mutex_xmlrpc);
	try {
		send_xml(main_set_sideband, XML_STRING,
			selrig->get_modetype(vfoA.imode) == 'U' ? "USB" : "LSB" );
	} catch (...) {}
	pthread_mutex_unlock(&mutex_xmlrpc);
}

// --------------------------------------------------------------------
// receive functions
// --------------------------------------------------------------------

string extract(string buf, string what)
{
	size_t pos = buf.find(what);
	if (pos != string::npos) {
		pos += what.length();
		size_t pos2 = buf.find(what, pos);
		if (pos2 != string::npos) {
			pos2 -= 2;
			return buf.substr(pos, pos2-pos);
		}
	}
	return "";
}

string val_string(string buf)
{
	return extract(buf, XML_STRING);
}

int val_int(string buf)
{
	string val = extract(buf, XML_INTEGER);
	if (!val.length()) return 0;
	int ival = atoi(val.c_str());
	return ival;
}

double val_double(string buf)
{
	string val = extract(buf, XML_DOUBLE);
	if (!val.length()) return 0;
	double dval = atof(val.c_str());
	return dval;
}

bool val_bool(string buf)
{
	string val = extract(buf, XML_BOOL);
	if (!val.length()) return false;
	bool bval = atoi(val.c_str());
	return bval;
}

static bool ptt_now_is = false;
void check_for_ptt_change()
{
	try {
		send_xml(main_get_trx_state, XML_NIL, "");
		bool nuptt = (val_string(xmlrep_buffer) == "TX");
		if (nuptt != ptt_now_is) {
			ptt_now_is = nuptt;
			Fl::awake( setPTT, (void*)nuptt);
		}
	} catch (...) {
		throw;
	}
}

void setvfo(void *d)
{
	long newfreq = (long)d;
	if (newfreq == vfoA.freq) return;
	if (!newfreq) return;

	pthread_mutex_lock(&mutex_serial);
		selrig->set_vfoA(newfreq);
	pthread_mutex_unlock(&mutex_serial);
	vfoA.freq = newfreq;
	FreqDisp->value(newfreq);
}

void check_for_frequency_change()
{
	try {
		send_xml(main_get_frequency, XML_NIL, "");

		long newfreq = (long)val_double(xmlrep_buffer);
		Fl::awake(setvfo, (void *)newfreq);
	} catch (...) {
		throw;
	}
}

static void updateModeControl(void *d)
{
	if (!run_digi_loop) return;
	int md = (long)d;
	pthread_mutex_lock(&mutex_serial);
		vfoA.imode = md;
		opMODE->index(vfoA.imode);
		selrig->set_mode(vfoA.imode);
		updateBandwidthControl();
		send_sideband();
		send_bandwidths();
		send_bandwidth_changed();
	pthread_mutex_unlock(&mutex_serial);
}

void check_for_mode_change()
{
	if (!selrig->modes_) {
		LOG_INFO("%s", "no mode change");
		return;
	}

	try {
		send_xml(main_get_mode, XML_NIL, "");

		string new_mode = val_string(xmlrep_buffer);
		if (new_mode != selrig->modes_[vfoA.imode]) {
			int imode = 0;
			while (selrig->modes_[imode] != NULL &&
				new_mode != selrig->modes_[imode]) imode++;
			if (selrig->modes_[imode] != NULL) {
				Fl::awake(updateModeControl, (void*)imode);
				return;
			}
		}
	} catch (...) {
		throw;
	}
}

void updateBW(void *d)
{
	if (!run_digi_loop) return;
	vfoA.iBW = (long)d;
	pthread_mutex_lock(&mutex_serial);
		opBW->index(vfoA.iBW);
		selrig->set_bandwidth(vfoA.iBW);
	pthread_mutex_unlock(&mutex_serial);
}

void check_for_bandwidth_change()
{
	if (!selrig->bandwidths_) return;

	try {
		send_xml(main_get_bandwidth, XML_NIL, "");

		string new_bw = val_string(xmlrep_buffer);
		if (new_bw != selrig->bandwidths_[vfoA.iBW]) {
			int ibw = 0;
			while (selrig->bandwidths_[ibw] != NULL &&
				new_bw != selrig->bandwidths_[ibw]) ibw++;
			if (selrig->bandwidths_[ibw] != NULL)
				Fl::awake(updateBW, (void*)ibw);
		}
	} catch (...) {
		throw;
	}
}

#define REG_UPDATE_INTERVAL  50 // milliseconds
#define CHECK_UPDATE_COUNT   (2000 / REG_UPDATE_INTERVAL)

void send_rig_info()
{
	stringstream xml_freq;
	xml_freq << vfoA.freq;

	array(selrig->bandwidths_, xml_rigbws);
	array(selrig->modes_, xml_rigmodes);
	try {
		send_xml(main_set_name, XML_STRING, selrig->name_);
		send_xml(main_set_bandwidths, XML_ARRAY, xml_rigbws.c_str());
		send_xml(main_set_modes, XML_ARRAY, xml_rigmodes.c_str());
		send_xml(main_set_frequency, XML_DOUBLE, xml_freq.str().c_str());
		send_xml(main_set_mode, XML_STRING, selrig->modes_[vfoA.imode]);
		send_xml(main_set_bandwidth, XML_STRING, selrig->bandwidths_[vfoA.iBW]);
		fldigi_online = true;
	} catch (...) {
		throw;
	}
	return;
}

void send_no_rig()
{
	array(szNOBWS, xml_rigbws);
	array(szNOMODES, xml_rigmodes);
	try {
		send_xml(main_set_name, XML_STRING, szNORIG);
		send_xml(main_set_bandwidths, XML_ARRAY, xml_rigbws.c_str());
		send_xml(main_set_modes, XML_ARRAY, xml_rigmodes.c_str());
		send_xml(main_set_mode, XML_STRING, "USB");
	} catch (...) {
	}
}

void * digi_loop(void *d)
{
	int try_count = CHECK_UPDATE_COUNT;
	for (;;) {
		MilliSleep(REG_UPDATE_INTERVAL);
		if (!run_digi_loop) break;
		pthread_mutex_lock(&mutex_xmlrpc);
		try {
			if (!fldigi_online && !wait_query) {
				if (!try_count--) send_rig_info();
			} else {
				if (!wait_query) check_for_ptt_change();
				if (!run_digi_loop) break;
				if (!wait_query) check_for_frequency_change();
				if (!run_digi_loop) break;
				if (!wait_query) check_for_mode_change();
				if (!run_digi_loop) break;
				if (!wait_query) check_for_bandwidth_change();
			}
		} catch (...) {
			try_count = CHECK_UPDATE_COUNT;
//			close_rig_socket();
			fldigi_online = false;
		}
		pthread_mutex_unlock(&mutex_xmlrpc);

	}
	return NULL;
}

