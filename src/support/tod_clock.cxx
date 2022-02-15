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

size_t zmsec()
{
	static struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000L + (tv.tv_usec / 1000L));
}

char *ztime()
{
	static char exttime[13];

	static struct tm tim;
	static struct timeval tv;

	gettimeofday(&tv, NULL);
	time_t tval = tv.tv_sec;

	gmtime_r(&tval, &tim);

	snprintf(exttime, sizeof(exttime),
		"%02d:%02d:%02d.%03d",
		tim.tm_hour, tim.tm_min, tim.tm_sec, (int)(tv.tv_usec / 1000L) );

	return exttime;
}
