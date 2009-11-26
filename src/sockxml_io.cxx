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

string cmdbuffer;
string querybuffer;
string inpbuffer;
string replybuffer;

string extract(string buf, string what);
double val_double(string buf);
int    val_int(string buf);
string val_string(string buf);
bool   val_bool(string buf);

string xml_rigbws;
string xml_rigmodes;
char xml_nufreq[20];

Address *adr = NULL;
bool run_digi_loop = true;
bool wait_query = false;
bool fldigi_online = false;

void sendcmd(const char *, const char *, const char *);
void sendquery(const char *, const char *, const char *);

//=====================================================================
// socket ops
//=====================================================================

void open_rig_socket()
{
	try {
		adr = new Address(tcpip_address.c_str(), tcpip_port.c_str());
	} catch (const SocketException& e) {
		LOG_WARN("%s", e.what());
		delete adr;
		adr = NULL;
	}
}


// --------------------------------------------------------------------
// send functions
// --------------------------------------------------------------------
#define XO(p)  append(XML_OPEN).append(p)
#define XC(p)  append(XML_CLOSE).append(p)

string array(const char **data)
{
	string arry("");
	arry.append("<param><value><array><data>").append(XML_EOL);
	while (*data) {
		arry.append("<value><string>").append(*data);
		arry.append("</string></value>").append(XML_EOL);
		data++;
	}
	arry.append("</data></array></value></param>").append(XML_EOL);
	return arry;
}

string payload(const char *methodname, const char *dtype, const char *data)
{
	string built;
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
	return built;
}

string header(int len)
{
	string hdr;
	stringstream slen;
	slen << len;
	hdr.clear();
	hdr.append(XML_HEADER).append(slen.str().c_str());
	hdr.append(XML_EOL).append(XML_EOL);
	return hdr;
}

void sendcmd(const char *methodname, const char *dtype, const char *data )
{
	if (!adr || !fldigi_online) return;

	string content = payload(methodname, dtype, data);
	cmdbuffer.clear();
	cmdbuffer.append(header(content.length()));
	cmdbuffer.append(content);

	try {
		pthread_mutex_lock(&mutex_xmlrpc);
			Socket sckt(*adr);
			sckt.set_timeout(0.1);
			sckt.connect();
			sckt.send(cmdbuffer);
			sckt.recv(inpbuffer);
		pthread_mutex_unlock(&mutex_xmlrpc);
		LOG_INFO("\n%s\n%s",
			cmdbuffer.c_str(),
			inpbuffer.c_str());
	} catch (const SocketException& e) {
		LOG_INFO("%s failed\n%s", methodname, e.what());
		pthread_mutex_unlock(&mutex_xmlrpc);
	}
}

void send_new_freq()
{
	stringstream xml_freq;
	xml_freq << vfoA.freq;

	sendcmd(main_set_frequency, XML_DOUBLE, xml_freq.str().c_str());

	inpbuffer.clear();
}

void send_modes()
{
	if (!selrig->modes_) return;
	xml_rigmodes = array(selrig->modes_);
	sendcmd(main_set_modes, XML_ARRAY, xml_rigmodes.c_str());
}

void send_bandwidths()
{
	if (!selrig->bandwidths_) return;
	xml_rigbws = array(selrig->bandwidths_);
	sendcmd(main_set_bandwidths, XML_ARRAY, xml_rigbws.c_str());
}

void send_name()
{
	sendcmd(main_set_name, XML_STRING, selrig->name_);
}

void send_ptt_changed(bool PTT)
{
	if (PTT)
		sendcmd(main_set_tx, XML_NIL, "");
	else
		sendcmd(main_set_rx, XML_NIL, "");
}

void send_mode_changed()
{
	if (!selrig->modes_) return;
	sendcmd(main_set_mode, XML_STRING, selrig->modes_[vfoA.imode]);
}

void send_bandwidth_changed()
{
	if (!selrig->bandwidths_) return;
	sendcmd(main_set_bandwidth, XML_STRING, selrig->bandwidths_[vfoA.iBW]);
}

void send_sideband()
{
	sendcmd(main_set_sideband, XML_STRING,
			selrig->get_modetype(vfoA.imode) == 'U' ? "USB" : "LSB" );
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

void sendquery(const char *methodname, const char *dtype, const char *data)
{
	if (!adr) return;

	string content = payload(methodname, dtype, data);
	querybuffer.clear();
	querybuffer.append(header(content.length()));
	querybuffer.append(content);
	replybuffer.clear();
	try {
		pthread_mutex_lock(&mutex_xmlrpc);
			Socket sckt(*adr);
			sckt.set_timeout(0.1);
			sckt.connect();
			sckt.send(querybuffer);
			sckt.recv(replybuffer);
		pthread_mutex_unlock(&mutex_xmlrpc);
		LOG_INFO("\n%s\n%s",
			querybuffer.c_str(),
			replybuffer.c_str());
	} catch (const SocketException& e) {
		LOG_INFO("%s %s", methodname, e.what());
		pthread_mutex_unlock(&mutex_xmlrpc);
		throw;
	}
}

static bool ptt_now_is = false;
void check_for_ptt_change()
{
	try {
		sendquery(main_get_trx_state, XML_NIL, "");
		bool nuptt = (val_string(replybuffer) == "TX");
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
	pthread_mutex_lock(&mutex_serial);
		selrig->set_vfoA(newfreq);
	pthread_mutex_unlock(&mutex_serial);
	vfoA.freq = newfreq;
	FreqDisp->value(newfreq);
}

void check_for_frequency_change()
{
	try {
		sendquery(main_get_frequency, XML_NIL, "");

		long newfreq = (long)val_double(replybuffer);
		if (newfreq && newfreq != vfoA.freq)
			Fl::awake(setvfo, (void *)newfreq);
		replybuffer.clear();
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
		sendquery(main_get_mode, XML_NIL, "");

		string new_mode = val_string(replybuffer);
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
		sendquery(main_get_bandwidth, XML_NIL, "");

		string new_bw = val_string(replybuffer);
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

#define REG_UPDATE_INTERVAL 200 // milliseconds
#define WAIT_UPDATE_INTERVAL 2000 // milliseconds

int check_xml_interval = REG_UPDATE_INTERVAL;

void send_rig_info()
{
	stringstream xml_freq;
	xml_freq << vfoA.freq;

	xml_rigbws = array(selrig->bandwidths_);
	xml_rigmodes = array(selrig->modes_);
	try {
		sendquery(main_set_name, XML_STRING, selrig->name_);
		sendquery(main_set_bandwidths, XML_ARRAY, xml_rigbws.c_str());
		sendquery(main_set_modes, XML_ARRAY, xml_rigmodes.c_str());
		sendquery(main_set_frequency, XML_DOUBLE, xml_freq.str().c_str());
		sendquery(main_set_mode, XML_STRING, selrig->modes_[vfoA.imode]);
		sendquery(main_set_bandwidth, XML_STRING, selrig->bandwidths_[vfoA.iBW]);
		check_xml_interval = REG_UPDATE_INTERVAL;
		fldigi_online = true;
	} catch (...) {
		throw;
	}
	return;
}

void send_no_rig()
{
	xml_rigbws = array(szNOBWS);
	xml_rigmodes = array(szNOMODES);
	try {
		sendquery(main_set_name, XML_STRING, szNORIG);
		sendquery(main_set_bandwidths, XML_ARRAY, xml_rigbws.c_str());
		sendquery(main_set_modes, XML_ARRAY, xml_rigmodes.c_str());
		sendquery(main_set_mode, XML_STRING, "USB");
	} catch (...) {
	}
}

void * digi_loop(void *d)
{
	for (;;) {
		MilliSleep(check_xml_interval);
		if (!run_digi_loop) break;
		try {
			if (!fldigi_online && !wait_query) send_rig_info();
			if (!wait_query) check_for_ptt_change();
			if (!run_digi_loop) break;
			if (!wait_query) check_for_frequency_change();
			if (!run_digi_loop) break;
			if (!wait_query) check_for_mode_change();
			if (!run_digi_loop) break;
			if (!wait_query) check_for_bandwidth_change();
		} catch (...) {
			check_xml_interval = WAIT_UPDATE_INTERVAL;
			fldigi_online = false;
		}
	}
	return NULL;
}

