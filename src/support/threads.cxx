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
#include "support.h"

/// This ensures that a mutex is always unlocked when leaving a function or block.

extern pthread_mutex_t mutex_replystr;
extern pthread_mutex_t command_mutex;
extern pthread_mutex_t mutex_vfoque;
extern pthread_mutex_t mutex_serial;
extern pthread_mutex_t debug_mutex;
extern pthread_mutex_t mutex_rcv_socket;
extern pthread_mutex_t mutex_ptt;
extern pthread_mutex_t mutex_srvc_reqs;
extern pthread_mutex_t mutex_trace;

// Change to 1 to observe guard lock/unlock processing on stdout
//#define DEBUG_GUARD_LOCK 0
//guard_lock::guard_lock(pthread_mutex_t* m, int h) : mutex(m), how(h) {
//char szlock[100];
//	pthread_mutex_lock(mutex);
//	snprintf(szlock, sizeof(szlock), "lock %s : %d", name(mutex), how);
//	how = h
//	if (how >= 100)
//		trace(1, szlock);
//	if (how != 0 && DEBUG_GUARD_LOCK)
//		printf("%s", szlock);
//}

//guard_lock::~guard_lock(void) {
//char szunlock[100];
//	snprintf(szunlock, sizeof(szunlock), "unlock %s : %d\n", name(mutex), how);
//	if (how >= 100)
//		trace(1, szunlock);
//	if (how != 0 && DEBUG_GUARD_LOCK)
//		printf("%s", szunlock);
//	pthread_mutex_unlock(mutex);
//}

guard_lock::guard_lock(pthread_mutex_t* m, std::string h) : mutex(m) {
	pthread_mutex_lock(mutex);
	if (!h.empty()) {
		how = h;
		std::string szlock;
		szlock.assign("lock ").append(name(mutex)).append(" : ").append(how);
		trace(1, szlock.c_str());
	}
}

guard_lock::~guard_lock(void) {
	if (!how.empty()) {
		std::string szlock;
		szlock.assign("unlock ").append(name(mutex)).append(" : ").append(how).append("\n");
		trace(1, szlock.c_str());
	}
	pthread_mutex_unlock(mutex);
}

const char * guard_lock::name(pthread_mutex_t *m) {
	if (m == &mutex_replystr) return "mutex_replystr";
	if (m == &command_mutex) return "command_mutex";
	if (m == &mutex_replystr) return "mutex_replystr";
	if (m == &mutex_vfoque) return "mutex_vfoque";
	if (m == &mutex_serial) return "mutex_serial";
	if (m == &debug_mutex) return "debug_mutex";
	if (m == &mutex_rcv_socket) return "mutex_rcv_socket";
	if (m == &mutex_ptt) return "mutex_ptt";
	if (m == &mutex_srvc_reqs) return "mutex_service_requests";
	if (m == &mutex_trace) return "mutex_trace";
	return "";
}
