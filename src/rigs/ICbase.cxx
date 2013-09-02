/*
 * Icom IC-746, 746PRO, 756, 756PRO
 * 7000, 7200, 7700 ... drivers
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */

#include "ICbase.h"
#include "debug.h"
#include "support.h"

//=============================================================================

void RIG_ICOM::adjustCIV(uchar adr)
{
	CIV = adr;
	pre_to[2] = ok[3] = bad[3] = pre_fm[3] = CIV;
}

void RIG_ICOM::checkresponse()
{
	if (RigSerial.IsOpen() == false)
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

bool RIG_ICOM::waitFB(const char *sz)
{
	char sztemp[50];
	string returned = "";
	string tosend = cmd;

	if (!RigSerial.IsOpen()) {
		replystr = returned;
		snprintf(sztemp, sizeof(sztemp), "%s TEST", sz);
		showresp(INFO, HEX, sztemp, tosend, returned);
		return false;
	}
	int cnt = 0, repeat = 0, num = cmd.length() + ok.length();
	for (repeat = 0; repeat < progStatus.comm_retries; repeat++) {
		sendCommand(cmd, 0);
		MilliSleep( (int)((num*11000.0/RigSerial.Baud())));
		returned = "";
		for ( cnt = 0; cnt < 20; cnt++) {
			readResponse();
			returned.append(replystr);
			if (returned.find(ok) != string::npos) {
				replystr = returned;
				waited = cnt * 10 * repeat;
				snprintf(sztemp, sizeof(sztemp), "%s ans in %d ms, OK", sz, waited);
				showresp(INFO, HEX, sztemp, tosend, returned);
				return true;
			}
			if (returned.find(bad) != string::npos) {
				replystr = returned;
				waited = cnt * 10 * repeat;
				snprintf(sztemp, sizeof(sztemp), "%s ans in %d ms, FAIL", sz, waited);
				showresp(ERR, HEX, sztemp, tosend, returned);
				return false;
			}
			MilliSleep(10);
			Fl::awake();
		}
	}
	replystr = returned;
	waited = cnt * 10 * repeat;
	snprintf(sztemp, sizeof(sztemp), "%s TIMED OUT in %d ms", sz, waited);
	showresp(INFO, HEX, sztemp, tosend, returned);
	return false;
}

bool RIG_ICOM::waitFOR(size_t n, const char *sz)
{
	char sztemp[50];
	string returned = "";
	string tosend = cmd;
	int cnt = 0, repeat = 0;
	size_t num = n + cmd.length();
	int delay =  num * 11000.0 / RigSerial.Baud();
	if (!RigSerial.IsOpen()) {
		replystr = returned;
		snprintf(sztemp, sizeof(sztemp), "%s TEST", sz);
		showresp(INFO, HEX, sztemp, tosend, returned);
		return false;
	}
	for (repeat = 0; repeat < progStatus.comm_retries; repeat++) {
		sendCommand(tosend, 0);
		MilliSleep(delay);
		returned = "";
		for ( cnt = 0; cnt < 20; cnt++) {
			readResponse();
			returned.append(replystr);
			if (returned.length() >= num) {
				replystr = returned;
				waited = cnt * 10 * repeat + delay;
				snprintf(sztemp, sizeof(sztemp), "%s ans in %d ms, OK  ", sz, waited);
				showresp(INFO, HEX, sztemp, tosend, returned);
				return true;
			}
			MilliSleep(10);
			Fl::awake();
		}
	}
	replystr = returned;
	waited = cnt * 10 * repeat + delay;
	snprintf(sztemp, sizeof(sztemp), "%s TIMED OUT in %d ms", sz, waited);
	showresp(INFO, HEX, sztemp, tosend, returned);
	return false;
}

