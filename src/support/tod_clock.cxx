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
pthread_mutex_t TOD_mutex     = PTHREAD_MUTEX_INITIALIZER;

static long long _zmsec = 0;
static int _zsec = 0;
static int _zmin = 0;
static int _zhr = 0;

static char ztbuf[20] = "20120602 123000";

int zmsec(void)
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
	static char now[7];
	guard_lock zmsec_lock(&TOD_mutex);
	snprintf(now, sizeof(now), "%02d%02d%02d",
		_zhr, _zmin, _zsec);
	return now;
}

char exttime[9];
char *zext_time()
{
	guard_lock zmsec_lock(&TOD_mutex);
	snprintf(exttime, sizeof(exttime), "%02d:%02d:%02d",
		_zhr, _zmin, _zsec);
	return exttime;
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

	exttime[0] = ztbuf[9];  exttime[1] = ztbuf[10];  
	exttime[2] = ':';
	exttime[3] = ztbuf[11]; exttime[4] = ztbuf[12];
	exttime[5] = ':';
	exttime[6] = ztbuf[13]; exttime[7] = ztbuf[13];
	exttime[8] = 0;

	_zmsec = 0;
	_zsec = tm.tm_sec;
	_zmin = tm.tm_min;
	_zhr  = tm.tm_hour;
}

//======================================================================
// TOD Thread loop
//======================================================================
static bool first_call = true;

void *TOD_loop(void *args)
{
	while(1) {

		if (TOD_exit) break;

		if (first_call) {
			guard_lock zmsec_lock(&TOD_mutex);
			struct timeval tv;
			gettimeofday(&tv, NULL);
			double st = 1000.0 - tv.tv_usec / 1e3;
			MilliSleep(st);
			first_call = false;
			ztimer((void *)0);
		} else {
			MilliSleep(10);
			guard_lock zmsec_lock(&TOD_mutex);
			_zmsec += 10;
			if (_zmsec % 100 == 0) {
				_zsec++;
				if (_zsec >= 60) {
					_zsec = 0;
					_zmin++;
					if (_zmin >= 60) {
						_zmin = 0;
						_zhr++;
					}
				}
			}
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

