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

#include "util.h"
#include "debug.h"
//#include "status.h"
#include "icons.h"

#include "tod_clock.h"
#include "timeops.h"

using namespace std;

static int _zmsec = 0;
static int _zsec = 0;
static int _zmin = 0;
static int _zhr = 0;

void ztimer()
{
	struct tm tm;
	time_t t_temp;
	struct timeval tv;

	gettimeofday(&tv, NULL);

	t_temp=(time_t)tv.tv_sec;
	gmtime_r(&t_temp, &tm);

	_zmsec = (tv.tv_usec/1000) % 1000 + tv.tv_sec * 1000;
	_zsec = tm.tm_sec;
	_zmin = tm.tm_min;
	_zhr  = tm.tm_hour;
}

int zmsec(void)
{
	ztimer();
	return _zmsec;
}

char exttime[13];
char *ztime()
{
	ztimer();
	snprintf(exttime, sizeof(exttime),
		"%02d:%02d:%02d.%03d",
		_zhr, _zmin, _zsec, _zmsec);
	return exttime;
}
