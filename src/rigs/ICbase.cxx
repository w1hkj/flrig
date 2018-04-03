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

#include "ICbase.h"
#include "debug.h"
#include "support.h"
#include "icons.h"
#include "tod_clock.h"

#include "status.h"

bool RIG_ICOM::listenThreadRunning = false;
pthread_t RIG_ICOM::listenThread;

//=============================================================================

RIG_ICOM::RIG_ICOM()
{
    CIV = 0x56;
    pre_to = "\xFE\xFE\x56\xE0";
    pre_fm = "\xFE\xFE\xE0\x56";
    post = "\xFD";
    ok = "\xFE\xFE\xE0\x56\xFB\xFD";
    bad = "\xFE\xFE\xE0\x56\xFA\xFD";

    // There are many RIG_ICOM objects that get created during startup. There
    // needs to be only one static listen thread for all of them.
    if(!listenThreadRunning) {
        listenThreadRunning = true;
        // Start the listen thread. TBD DJW - handle error condition.
        (void)pthread_create(&listenThread, NULL, serialListenThreadLoop, (void *)name_.c_str());
    }
}

RIG_ICOM::~RIG_ICOM()
{
    // There are many RIG_ICOM objects that get created during startup. There
    // needs to be only one static listen thread for all of them.
    if(listenThreadRunning) {
        listenThreadRunning = false;
        puts("Joining Listen Thread");
        pthread_join(listenThread, NULL);
        puts("Joined");
    }
}

void RIG_ICOM::adjustCIV(uchar adr)
{
	CIV = adr;
	pre_to[2] = ok[3] = bad[3] = pre_fm[3] = CIV;
}

void RIG_ICOM::checkresponse()
{
	if (!progStatus.use_tcpip && !RigSerial->IsOpen())
		return;

	if (replystr.rfind(ok) != string::npos)
		return;

	string s1 = str2hex(cmd.c_str(), cmd.length());
	string s2 = str2hex(replystr.c_str(), replystr.length());
	LOG_ERROR("\nsent  %s\nreply %s", s1.c_str(), s2.c_str());
}

bool RIG_ICOM::sendICcommand(string cmd, int nbr)
{
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
	string returned = "";
	string tosend = cmd;
	unsigned long tod_start = zmsec();

	if (!progStatus.use_tcpip && !RigSerial->IsOpen()) {
		replystr = returned;
//		snprintf(sztemp, sizeof(sztemp), "%s TEST", sz);
//		showresp(DEBUG, HEX, sztemp, tosend, returned);
		waitcount = 0;
#ifdef IC_DEBUG
civ << sz << std::endl;
civ << "    " << str2hex(cmd.c_str(), cmd.length()) << std::endl;
civ.close();
#endif
		return false;
	}
	int cnt = 0, repeat = 0, num = cmd.length() + ok.length();

	int wait_msec = (int)(num*11000.0/RigSerial->Baud() +
			progStatus.use_tcpip ? progStatus.tcpip_ping_delay : 0) / 10;

	for (repeat = 0; repeat < progStatus.comm_retries; repeat++) {
		sendCommand(cmd, 0);
		returned = "";
		for ( cnt = 0; cnt < wait_msec; cnt++) {
			readResponse();
			returned.append(replystr);
			if (returned.find(ok) != string::npos) {
				replystr = returned;
				unsigned long int waited = zmsec() - tod_start;
				snprintf(sztemp, sizeof(sztemp), "%s ans in %ld ms, OK", sz, waited);
				if (repeat)
					showresp(WARN, HEX, sztemp, tosend, returned);
				else
					showresp(DEBUG, HEX, sztemp, tosend, returned);

				waitcount = 0;
#ifdef IC_DEBUG
civ << sztemp << std::endl;
civ << "    " << str2hex(cmd.c_str(), cmd.length()) << std::endl;
civ << "    " << str2hex(returned.c_str(), returned.length()) << std::endl;
civ.close();
#endif
				return true;
			}
			if (returned.find(bad) != string::npos) {
				replystr = returned;
				unsigned long int waited = zmsec() - tod_start;
				snprintf(sztemp, sizeof(sztemp), "%s ans in %ld ms, FAIL", sz, waited);
				showresp(ERR, HEX, sztemp, tosend, returned);
				waitcount = 0;

#ifdef IC_DEBUG
civ << sztemp << std::endl;
civ << "    " << str2hex(cmd.c_str(), cmd.length()) << std::endl;
civ << "    " << str2hex(returned.c_str(), returned.length()) << std::endl;
civ.close();
#endif
				return false;
			}
			MilliSleep(10);
			Fl::awake();
		}
	}
	waitcount++;
	replystr = returned;
	unsigned long int waited = zmsec() - tod_start;
	snprintf(sztemp, sizeof(sztemp), "%s TIMED OUT in %ld ms", sz, waited);
	showresp(ERR, HEX, sztemp, tosend, returned);

#ifdef IC_DEBUG
civ << sztemp << std::endl;
civ << "    " << str2hex(cmd.c_str(), cmd.length()) << std::endl;
civ << "    " << str2hex(returned.c_str(), returned.length()) << std::endl;
#endif

	if (waitcount > 4 && !timeout_alert) {
		timeout_alert = true;
		snprintf(sztimeout_alert, sizeof(sztimeout_alert), 
			"Serial i/o failure\n%s TIME OUT in %ld ms",
			sz, waited);
			Fl::awake(show_timeout);
#ifdef IC_DEBUG
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
	string returned = "";
	string tosend = cmd;
	int cnt = 0, repeat = 0;
	size_t num = n;
	if (progStatus.comm_echo) num += cmd.length();

	unsigned long int tod_start = zmsec();

	int delay =  (int)(num * 11000.0 / RigSerial->Baud() + 
		progStatus.use_tcpip ? progStatus.tcpip_ping_delay : 0) / 10;

	if (!progStatus.use_tcpip && !RigSerial->IsOpen()) {
		replystr = returned;
//		snprintf(sztemp, sizeof(sztemp), "%s TEST", sz);
//		showresp(DEBUG, HEX, sztemp, tosend, returned);
#ifdef IC_DEBUG
civ << sz << std::endl;
civ << "    " << str2hex(cmd.c_str(), cmd.length()) << std::endl;
civ.close();
#endif
		return false;
	}
	for (repeat = 0; repeat < progStatus.comm_retries; repeat++) {
		sendCommand(tosend, 0);
		returned = "";
		for ( cnt = 0; cnt < delay; cnt++) {
			readResponse();
			returned.append(replystr);
			if (returned.length() >= num) {
				replystr = returned;
				unsigned long int waited = zmsec() - tod_start;
				snprintf(sztemp, sizeof(sztemp), "%s ans in %ld ms, OK  ", sz, waited);
				showresp(DEBUG, HEX, sztemp, tosend, returned);

#ifdef IC_DEBUG
civ << sztemp << std::endl;
civ << "    " << str2hex(cmd.c_str(), cmd.length()) << std::endl;
civ << "    " << str2hex(returned.c_str(), returned.length()) << std::endl;
civ.close();
#endif
				return true;
			}
			MilliSleep(10);
			Fl::awake();
		}
	}

	waitcount++;
	replystr = returned;
	unsigned long int waited = zmsec() - tod_start;
	snprintf(sztemp, sizeof(sztemp), "%s TIMED OUT in %ld ms", sz, waited);
	showresp(ERR, HEX, sztemp, tosend, returned);

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
civ << "    " << str2hex(returned.c_str(), returned.length()) << std::endl;
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

void *RIG_ICOM::serialListenThreadLoop(void *p)
{
    //int n, nread;
    printf("Listen Thread Starting %s\n", (char *)p);

    while(listenThreadRunning) {
        //nread = readResponse();
        sleep(1);
        putchar('.');
        //if(nread) {
        //    printf("serialListenThreadLoop: read %d bytes:", nread);
        //    for(n = 0; n < nread; n++) {
        //        printf(" %02X", replystr[n]);
        //    }
        //    putchar('\n');
        //}
    }

    puts("Listen Thread Ending");

    return NULL;
}
