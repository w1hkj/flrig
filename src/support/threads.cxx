// ----------------------------------------------------------------------------
//      threads.cxx
//
// Copyright (C) 2014
//              Stelios Bounanos, M0GLD
//              David Freese, W1HKJ
//
// This file is part of fldigi.
//
// fldigi is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// fldigi is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

#include "threads.h"

/// This ensures that a mutex is always unlocked when leaving a function or block.
extern pthread_mutex_t mutex_rcv_socket;
extern pthread_mutex_t mutex_serial;
extern pthread_mutex_t mutex_xmlrpc;
extern pthread_mutex_t mutex_queA;
extern pthread_mutex_t mutex_queB;
extern pthread_mutex_t mutex_ptt;

// Change to 1 to observe guard lock/unlock processing on stdout
#define DEBUG_GUARD_LOCK 0

guard_lock::guard_lock(pthread_mutex_t* m, int h) : mutex(m), how(h) {
	pthread_mutex_lock(mutex);
	if (how != 0 && DEBUG_GUARD_LOCK)
		printf("lock %s : %d\n", name(mutex), how);
}

guard_lock::~guard_lock(void) {
	pthread_mutex_unlock(mutex);
	if (how != 0 && DEBUG_GUARD_LOCK)
		printf("unlock %s : %d\n", name(mutex), how);
}

const char * guard_lock::name(pthread_mutex_t *m) {
		if (m == &mutex_ptt) return "PTT";
		if (m == &mutex_queA) return "QueA";
		if (m == &mutex_queB) return "QueB";
		if (m == &mutex_rcv_socket) return "Rcv Socket";
		if (m == &mutex_serial) return "Serial";
		if (m == &mutex_xmlrpc) return "XmlRpc";
		return "Unknown";
	}
