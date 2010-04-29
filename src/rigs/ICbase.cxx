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

void RIG_ICOM::checkresponse(int n)
{
	if (RigSerial.IsOpen() == false)
		return;

	if (replystr.find(ok) != string::npos)
//	if (strcmp( replybuff, ok.c_str()) == 0)
		return;

	LOG_ERROR("\nsent  %s\nreply %s",
		str2hex(cmd.c_str(), cmd.length()),
		str2hex(replybuff, n));
}

bool RIG_ICOM::sendICcommand(string cmd, int nbr)
{
	if (RigSerial.IsOpen()) {
		for (int i = 0; i < progStatus.comm_retries; i++) {
			if (i) clearSerialPort();
			if (sendCommand(cmd, nbr, true) != nbr) {
				LOG_ERROR("sendCommand() failed");
				continue;
			}

// look for preamble at beginning
			if (replystr.find(pre_fm) == string::npos)  {
				LOG_ERROR("preamble");
				continue;
			}

// look for postamble
			if (replystr.find(post) == string::npos) {
				LOG_ERROR("postample 0x%X", (unsigned char)post[0]);
				continue;
			}
			return true;
		}
		LOG_ERROR("Exceeded retry count");
		return false;
	} else {
		sendCommand(cmd, nbr, true);
		return false;
	}
	return false;
}

