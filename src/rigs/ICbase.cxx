// ----------------------------------------------------------------------------
// Copyright (C) 2014
//			  David Freese, W1HKJ
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

#include <pthread.h>
#include <chrono>	// TBD DJW - For timestamps.
#include <ctime>	// TBD DJW - For timestamps.
#include <iostream>	// TBD DJW - For timestamps.
#include <iomanip>	// TBD DJW - For timestamps.
#include "ICbase.h"
#include "debug.h"
#include "support.h"
#include "icons.h"
#include "tod_clock.h"

#include "status.h"

using namespace std::chrono;	// TBD DJW - For timestamps.

// TBD DJW - Temporary while I figure out performance. This and ic7300_jig both
// use a system_clock so timestamps can be compared while both programs are
// running.
class timestamp {
	public:
		void print(ostream &out) const
		{
			system_clock::time_point now;
			system_clock::duration   dtn;
			time_t                   now_c;
			int                      tenth_ms;

			now = system_clock::now();
			dtn = now.time_since_epoch();
			tenth_ms = (dtn.count() % system_clock::period::den) / 100000;
			now_c = system_clock::to_time_t(now);

			out << std::put_time(localtime(&now_c), "%T") << '.' << setfill('0') << setw(4) << tenth_ms;
		}
};

ostream &operator<<(ostream &out, const timestamp &obj)
{
	obj.print(out);
	return out;
}

// TBD DJW - Carefully check that replystr is always protected.
static pthread_mutex_t civ_resp_mutex   = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  civ_resp_cond    = PTHREAD_COND_INITIALIZER;
static bool            civ_resp_waiting = false;
static string          civ_resp_string;

RIG_ICOM *RIG_ICOM::this_rig = NULL;
bool RIG_ICOM::listenThreadRunning = false;
pthread_t RIG_ICOM::listenThread;

//=============================================================================
RIG_ICOM::RIG_ICOM()
{
	CIV = 0x56;
	pre_to              = "\xFE\xFE\x56\xE0";
	pre_fm              = "\xFE\xFE\xE0\x56";
	post                = "\xFD";
	ok                  = "\xFE\xFE\xE0\x56\xFB\xFD";
	bad                 = "\xFE\xFE\xE0\x56\xFA\xFD";
	scope_waveform_data = "\xFE\xFE\xE0\x56\x27\x00";
}

RIG_ICOM::~RIG_ICOM()
{
}

void RIG_ICOM::initialize()
{
	puts("RIG_ICOM::initialize()");

	flrig_abort = false;	// TBD DJW - Base setting this on failure starting the listen thread.

	if(this_rig) {
		flrig_abort = true;
		LOG_ERROR("RIG_ICOM already initialized");
	}
	this_rig = this;

	//RigSerial->printConfig();

	//RigSerial->getVminVtime(cc_t * pvmin, cc_t * pvtime)
	//RigSerial->setVminVtime(cc_t vmin, cc_t vtime)

	// Can be called more than once if someone is selecting different ICOM radios.
	printf("Starting Listen Thread - %s\n", name_.c_str());
	listenThreadRunning = true;
	// Tweak the performance up. We want fast reaction to unsolicited (remote scope) data.
	// vtime = 1
	// vmin = 0

	// Start the listen thread. TBD DJW - handle error condition.
	(void)pthread_create(&listenThread, NULL, CIV_listen_thread_loop, NULL);
}

void RIG_ICOM::post_initialize() {puts("RIG_ICOM::post_initialize()");}

void RIG_ICOM::shutdown()        {
	puts("RIG_ICOM::shutdown()");
	listenThreadRunning = false;
	puts("Joining Listen Thread");
	pthread_join(listenThread, NULL);
	puts("Joined");
}

void RIG_ICOM::adjustCIV(uchar adr)
{
	CIV = adr;
	pre_to[2] = ok[3] = bad[3] = pre_fm[3] = scope_waveform_data[3] = CIV;
}

void RIG_ICOM::checkresponse()
{
	if (!progStatus.use_tcpip && !RigSerial->IsOpen())
		return;

	if (civ_resp_string.rfind(ok) != string::npos)
		return;

	string s1 = str2hex(cmd.c_str(), cmd.length());
	string s2 = str2hex(civ_resp_string.c_str(), civ_resp_string.length());
	LOG_ERROR("\nsent  %s\nreply %s", s1.c_str(), s2.c_str());
}

bool RIG_ICOM::sendICcommand(string cmd, int nbr)
{
	std::cout << timestamp() << "sendICcommand:" << std::endl;

	int ret = sendCommand(cmd);

	if (!progStatus.use_tcpip && !RigSerial->IsOpen())
		return false;

	if (ret < nbr) {
		LOG_ERROR("Expected %d received %d", nbr, ret);
		return false;
	}

	if (ret > nbr) replystr.erase(0, ret - nbr);

// look for preamble at beginning
	if (replystr.rfind(pre_fm) == string::npos)  {
		LOG_ERROR("preamble: %s not in %s", pre_fm.c_str(), cmd.c_str());
		return false;
	}

// look for postamble
	if (replystr.rfind(post) == string::npos) {
		LOG_ERROR("postample: %s not at end of %s", post.c_str(), cmd.c_str());
		return false;
	}
	return true;
}

void RIG_ICOM::delayCommand(string cmd, int wait)
{
	std::cout << timestamp() << "delayCommand:" << std::endl;

	int oldwait = progStatus.comm_wait;
	progStatus.comm_wait += wait;
	sendCommand(cmd);
	progStatus.comm_wait = oldwait;
}

static int waitcount = 0;
static bool timeout_alert = false;
static char sztimeout_alert[200];

static void show_timeout(void *)
{
	fl_alert2("%s", sztimeout_alert);
}

#include <fstream>

bool RIG_ICOM::waitFB(const char *sz)
{
#ifdef IC_DEBUG
	ofstream civ(ICDEBUGfname.c_str(), ios::app);
#endif

	char sztemp[100];
	int repeat = 0;
	int timeout_ms;
	size_t num = ok.length();
	if (progStatus.comm_echo) num += cmd.length();

	unsigned long tod_start = zmsec();

	if (!progStatus.use_tcpip && !RigSerial->IsOpen()) {
		//snprintf(sztemp, sizeof(sztemp), "%s TEST", sz);
		//showresp(DEBUG, HEX, sztemp, cmd, returned);
		waitcount = 0;
#ifdef IC_DEBUG
civ << sz << std::endl;
civ << "    " << str2hex(cmd.c_str(), cmd.length()) << std::endl;
civ.close();
#endif
	return false;
	}

	// TBD DJW Figure out what timeout_ms should be when progStatus.use_tcpip.
	// 100 mS is how long we give the radio to respond.  1000 converts seconds
	// to milliseconds. 10 bit times is approximatly what it takes to send a
	// byte over a serial port.
	timeout_ms = 100 + ((1000 * 10 * num) / RigSerial->Baud());

	for (repeat = 0; repeat < progStatus.comm_retries; repeat++) {

		std::cout << timestamp() << ": waitFB" << std::endl;

		waitICResponse(timeout_ms);
		if (civ_resp_string.find(ok) != string::npos) {
			unsigned long int waited = zmsec() - tod_start;
			snprintf(sztemp, sizeof(sztemp), "%s ans in %ld ms, OK", sz, waited);
			if (repeat)
				showresp(WARN, HEX, sztemp, cmd, civ_resp_string);
			else
				showresp(DEBUG, HEX, sztemp, cmd, civ_resp_string);

			waitcount = 0;
#ifdef IC_DEBUG
civ << sztemp << std::endl;
civ << "    " << str2hex(cmd.c_str(), cmd.length()) << std::endl;
civ << "    " << str2hex(civ_resp_string.c_str(), civ_resp_string.length()) << std::endl;
civ.close();
#endif
			return true;
		}
		if (civ_resp_string.find(bad) != string::npos) {
			unsigned long int waited = zmsec() - tod_start;
			snprintf(sztemp, sizeof(sztemp), "%s ans in %ld ms, FAIL", sz, waited);
			showresp(ERR, HEX, sztemp, cmd, civ_resp_string);
			waitcount = 0;

#ifdef IC_DEBUG
civ << sztemp << std::endl;
civ << "    " << str2hex(cmd.c_str(), cmd.length()) << std::endl;
civ << "    " << str2hex(civ_resp_string.c_str(), civ_resp_string.length()) << std::endl;
civ.close();
#endif
			return false;
		}
		MilliSleep(10);
		Fl::awake();
	}

	waitcount++;
	unsigned long int waited = zmsec() - tod_start;
	snprintf(sztemp, sizeof(sztemp), "%s TIMED OUT in %ld ms", sz, waited);
	showresp(ERR, HEX, sztemp, cmd, civ_resp_string);

	if (waitcount > 4 && !timeout_alert) {
		timeout_alert = true;
		snprintf(sztimeout_alert, sizeof(sztimeout_alert), 
			"Serial i/o failure\n%s TIME OUT in %ld ms",
			sz, waited);
			Fl::awake(show_timeout);

#ifdef IC_DEBUG
civ << sztemp << std::endl;
civ << "    " << str2hex(cmd.c_str(), cmd.length()) << std::endl;
civ << "    " << str2hex(civ_resp_string.c_str(), civ_resp_string.length()) << std::endl;
civ << sztemp << std::endl;
#endif

	}

#ifdef IC_DEBUG
civ.close();
#endif
	return false;
}

bool RIG_ICOM::waitFOR(size_t n, const char *sz)
{
#ifdef IC_DEBUG
	ofstream civ(ICDEBUGfname.c_str(), ios::app);
#endif

	char sztemp[100];
	int repeat = 0;
	int timeout_ms;
	size_t num = n;
	if (progStatus.comm_echo) num += cmd.length();

	unsigned long int tod_start = zmsec();

	if (!progStatus.use_tcpip && !RigSerial->IsOpen()) {
		// TBD DJW - Fix this. replystr = returned;
//	  snprintf(sztemp, sizeof(sztemp), "%s TEST", sz);
//	  showresp(DEBUG, HEX, sztemp, cmd, returned);
#ifdef IC_DEBUG
civ << sz << std::endl;
civ << "    " << str2hex(cmd.c_str(), cmd.length()) << std::endl;
civ.close();
#endif
		return false;
	}

	// TBD DJW Figure out what timeout_ms should be when progStatus.use_tcpip.
	// 100 mS is how long we give the radio to respond.  1000 converts seconds
	// to milliseconds. 10 bit times is approximatly what it takes to send a
	// byte over a serial port.
	timeout_ms = 100 + ((1000 * 10 * num) / RigSerial->Baud());

	for (repeat = 0; repeat < progStatus.comm_retries; repeat++) {

		std::cout << timestamp() << ": waitFOR: " << n << " bytes" << std::endl;

		waitICResponse(timeout_ms);
		if (civ_resp_string.length() >= num) {
			unsigned long int waited = zmsec() - tod_start;
			snprintf(sztemp, sizeof(sztemp), "%s ans in %ld ms, OK  ", sz, waited);
			showresp(DEBUG, HEX, sztemp, cmd, civ_resp_string);

#ifdef IC_DEBUG
civ << sztemp << std::endl;
civ << "    " << str2hex(cmd.c_str(), cmd.length()) << std::endl;
civ << "    " << str2hex(civ_resp_string.c_str(), civ_resp_string.length()) << std::endl;
civ.close();
#endif
			return true;
		}
		MilliSleep(10);
		Fl::awake();
	}

	waitcount++;
	unsigned long int waited = zmsec() - tod_start;
	snprintf(sztemp, sizeof(sztemp), "%s TIMED OUT in %ld ms", sz, waited);
	showresp(ERR, HEX, sztemp, cmd, civ_resp_string);

	if (waitcount > 4 && !timeout_alert) {
		timeout_alert = true;
		snprintf(sztimeout_alert, sizeof(sztimeout_alert), 
			"Serial i/o failure\n%s TIME OUT in %ld ms",
			sz, waited);
			Fl::awake(show_timeout);
	}

#ifdef IC_DEBUG
civ << sztemp << std::endl;
civ << "    " << str2hex(cmd.c_str(), cmd.length()) << std::endl;
civ << "    " << str2hex(civ_resp_string.c_str(), civ_resp_string.length()) << std::endl;
civ.close();
#endif
	return false;
}

// exchange & equalize are available in these Icom xcvrs
// 718 706MKIIG 746 746PRO 756 756PRO 756PROII 756PROIII
// 910 7000 7100 7200 7300 7410 7600 7700 7800 9100

void RIG_ICOM::swapvfos()
{
	cmd = pre_to;
	cmd += 0x07; cmd += 0xB0;
	cmd.append(post);
	waitFB("Exchange vfos");
}

void RIG_ICOM::A2B()
{
	cmd = pre_to;
	cmd += 0x07; cmd += 0xA0;
	cmd.append(post);
	waitFB("Equalize vfos");
}

int RIG_ICOM::waitICResponse(int timeout_ms)
{
	pthread_mutex_lock(&civ_resp_mutex);
    civ_resp_waiting = true;
	sendCommand(cmd);
    pthread_cond_wait(&civ_resp_cond, &civ_resp_mutex);
	pthread_mutex_unlock(&civ_resp_mutex);

	std::cout << timestamp() << ": waitICResponse(timeout_ms=" << timeout_ms << "): " << 
        str2hex(civ_resp_string.c_str(), civ_resp_string.length()) << std::endl;

	return 0;
}

// TBD DJW - Add shutdown to override virtual function in rigbase.
void *RIG_ICOM::CIV_listen_thread_loop(void *p)
{
	size_t nread = 0;
	size_t start_position, end_position, length;
	static bool saved = false;

	puts("Listen Thread Starting");

	while(listenThreadRunning) {

        // readResponse() fills replystr with nread bytes. replystr.length() will equal nread.
		nread = readResponse();

		if(nread) {

			std::cout << timestamp() << ": serialListenThreadLoop: read " << nread << " bytes:" << std::endl;

			start_position = 0;
			while(string::npos != (end_position = replystr.find((char)0xFD, start_position))) {

				length = 1 + end_position - start_position;

				if(saved) {
					saved = false;
					civ_resp_string.append(replystr, start_position, length);
				} else {
					civ_resp_string.assign(replystr, start_position, length);
				}

				pthread_mutex_lock(&civ_resp_mutex);
				if(civ_resp_waiting && string::npos == civ_resp_string.find(this_rig->scope_waveform_data)) {
					// This is the response we're waiting for.
					civ_resp_waiting = false;
					pthread_cond_signal(&civ_resp_cond);
					pthread_mutex_unlock(&civ_resp_mutex);
				} else {
					// This was unsolicited.
					pthread_mutex_unlock(&civ_resp_mutex);
					// TBD - Send to scope.
				}

				start_position = end_position + 1;
			}

			if(start_position < nread) {
				// TBD DJW - Have not seen this yet. Need a way to cause it to test it.
				puts("*** SAVING ***");
				saved = true;
				civ_resp_string.assign(replystr, start_position, string::npos);
			}
		}
	}

	puts("Listen Thread Ending");

	return NULL;
}
