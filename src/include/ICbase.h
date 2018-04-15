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

#ifndef _ICbase_H
#define _ICbase_H

#include <pthread.h>
#include "rigbase.h"
#include "debug.h"
#include "support.h"

class RIG_ICOM : public rigbase {
private:
	static bool listenThreadRunning;
	static pthread_t listenThread;
	static void *CIV_listen_thread_loop(void *p);
	static RIG_ICOM *this_rig;
protected:
	string pre_to;
	string pre_fm;
	string post;
	string ok;
	string bad;
	string scope_waveform_data;
	int    waited;
	const char *_mode_type;
	int waitICResponse(int timeout_ms);
public:
	RIG_ICOM();
	virtual ~RIG_ICOM();
	void initialize();
	void post_initialize();
	void shutdown();
	void checkresponse();
	bool sendICcommand(string str, int nbr);
	void delayCommand(string cmd, int wait);
	bool  waitFB(const char *sz);
	bool  waitFOR(size_t n, const char *sz);
	void adjustCIV(uchar adr);

	virtual void swapvfos();
	virtual void A2B();
};

#endif
