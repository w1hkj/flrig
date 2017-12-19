// =====================================================================
//
// TOD_clock.cxx
//
// Copyright (C) 2017
//		Dave Freese, W1HKJ
//
// This file is part of flrig
//
// Fldigi is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Fldigi is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with fldigi.  If not, see <http://www.gnu.org/licenses/>.
// =====================================================================

#include <config.h>

#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include <cmath>
#include <cstring>

#include <iostream>

#include <FL/Fl.H>

#include "threads.h"

#include "util.h"
#include "debug.h"
//#include "status.h"
#include "icons.h"

#include "tod_clock.h"
#include "timeops.h"

using namespace std;

static pthread_t TOD_thread;
static pthread_mutex_t TOD_mutex     = PTHREAD_MUTEX_INITIALIZER;

static unsigned long  _zmsec = 0;

static char ztbuf[20] = "20120602 123000";

unsigned long zmsec(void)
{
	guard_lock zmsec_lock(&TOD_mutex);
	return _zmsec;
}

char* zdate()
{
	guard_lock zmsec_lock(&TOD_mutex);
	return ztbuf;
}

char* ztime()
{
	guard_lock zmsec_lock(&TOD_mutex);
	return ztbuf + 9;
}

char* zshowtime() {
	static char s[5];
	strncpy(s, &ztbuf[9], 4);
	s[4] = 0;
	return s;
}

static bool TOD_exit = false;
static bool TOD_enabled = false;

void ztimer(void *)
{
	struct tm tm;
	time_t t_temp;
	struct timeval tv;

	gettimeofday(&tv, NULL);

	t_temp=(time_t)tv.tv_sec;
	gmtime_r(&t_temp, &tm);
	if (!strftime(ztbuf, sizeof(ztbuf), "%Y%m%d %H%M%S", &tm))
		memset(ztbuf, 0, sizeof(ztbuf));
	else
		ztbuf[8] = '\0';
}

//======================================================================
// TOD Thread loop
//======================================================================
static bool first_call = true;

void *TOD_loop(void *args)
{
	int count = 100;
	while(1) {

		if (TOD_exit) break;

		if (first_call) {
			guard_lock zmsec_lock(&TOD_mutex);
			struct timeval tv;
			gettimeofday(&tv, NULL);
			double st = 1000.0 - tv.tv_usec / 1e3;
			MilliSleep(st);
			first_call = false;
			_zmsec += st;
		} else {
			{
				guard_lock zmsec_lock(&TOD_mutex);
				_zmsec += 10;
			}
			if (--count == 0) {
				Fl::awake(ztimer);
				count = 100;
			}
			MilliSleep(10);
		}
	}

// exit the TOD thread
	SET_THREAD_CANCEL();
	return NULL;
}

//======================================================================
//
//======================================================================
void TOD_init()
{
	TOD_exit = false;

	if (pthread_create(&TOD_thread, NULL, TOD_loop, NULL) < 0) {
		LOG_ERROR("%s", "pthread_create failed");
		return;
	}

	LOG_INFO("%s", "Time Of Day thread started");

	TOD_enabled = true;
}

//======================================================================
//
//======================================================================
void TOD_close()
{
	if (!TOD_enabled) return;

	TOD_exit = true;
	pthread_join(TOD_thread, NULL);
	TOD_enabled = false;

	LOG_INFO("%s", "Time Of Day thread terminated. ");

}

