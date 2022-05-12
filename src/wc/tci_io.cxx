// ---------------------------------------------------------------------
//
// tci_io.cxx, a part of flrig
//
// Copyright (C) 2022
// Dave Freese, W1HKJ
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
// GNU General Public License for more detailrx.
//
// You should have received a copy of the GNU General Public License
// along with the program; if not, write to the
//
//  Free Software Foundation, Inc.
//  51 Franklin Street, Fifth Floor
//  Boston, MA  02110-1301 USA.
//
// ---------------------------------------------------------------------

#include "tci_io.h"

#include <list>
#include <iostream>
#include <string>

#include <assert.h>
#include <stdio.h>

#define TCI_DEBUG

#if 0
#include <time.h>
#include <pthread.h>

//----------------------------------------------------------------------
// utility functions & classes
//----------------------------------------------------------------------

void MilliSleep(long msecs)
{
	struct timespec tv;
	tv.tv_sec = msecs / 1000;
	tv.tv_nsec = (msecs - tv.tv_sec * 1000) * 1000000L;
	nanosleep(&tv, NULL);
}

class guard_lock
{
public:
	guard_lock(pthread_mutex_t* m) {
		mutex = m;
		pthread_mutex_lock(mutex);
	}
	~guard_lock(void) {
		pthread_mutex_unlock(mutex);
	}
private:
	pthread_mutex_t* mutex;
};

//----------------------------------------------------------------------

#else

#include "util.h"
#include "trace.h"
#include "threads.h"

#endif

TCI_VALS slice_0, slice_1;

void print_vals(TCI_VALS &slice)
{
	std::cout << "A: " << slice.A.freq << ", " << slice.A.bw << ", " << slice.A.mod << std::endl;
	std::cout << "B: " << slice.B.freq << ", " << slice.B.bw << ", " << slice.B.mod << std::endl;
	std::cout << "Volume: " << slice.vol << std::endl;
	std::cout << "PTT: " << slice.ptt << std::endl;
	std::cout << "Power: " << slice.pwr << std::endl;
}

using WSclient::WebSocket;

void handle_message(const std::string & message)
{
	int rxnbr, vfo, ival;
	float fval;
	char szstr[50];
	std::string str;
	std::string rx = message;

	for (size_t n = 0; n < rx.length(); n++)
		rx[n] = toupper(rx[n] & 0xFF);

	if (rx.find("RX_SMETER:") == 0) { // smeter reading
		sscanf(rx.c_str(), "RX_SMETER:%d,%d,%d;", &rxnbr, &vfo, &ival);
		if (rxnbr == 0) {
			if (vfo == 0) slice_0.smeter = ival;
		}
	}
	else {
		tci_trace(2, "PARSE:", rx.c_str());
		if (rx.find("VFO:") == 0) { // vfo:0,0,7032050;
			sscanf(rx.c_str(), "VFO:%d,%d,%d", &rxnbr, &vfo, &ival);
			if (rxnbr == 0) {
				if (vfo == 0) slice_0.A.freq = ival;
				else          slice_0.B.freq = ival;
			} else {
				if (vfo == 0) slice_1.A.freq = ival;
				else          slice_1.B.freq = ival;
			}
		}
		else if (rx.find("DDS:") == 0) { // dds:1,14070000;
			sscanf(rx.c_str(), "DDS:%d,%d", &rxnbr, &ival);
			if (rxnbr == 0) {
				slice_0.dds = ival;
			} else {
				slice_1.dds = ival;
			}
		}
		else if (rx.find("RX_FILTER_BAND:") == 0) { // rx_filter_band:0,-600,600;
			sscanf(rx.c_str(), "RX_FILTER_BAND:%d,%s", &vfo, szstr);
			if (vfo == 0) {slice_0.A.bw = szstr; }
			else          {slice_0.B.bw = szstr; }
		}
		else if (rx.find("MODULATION:") == 0) { // modulation:0,cw;
			sscanf(rx.c_str(), "MODULATION:%d,%s", &vfo, szstr);
			if (vfo == 0) { slice_0.A.mod = szstr; }
			else          { slice_0.B.mod = szstr; }
		}
		else if (rx.find("TRX:") == 0) {
			sscanf(rx.c_str(), "TRX:%d,%s", &rxnbr, szstr);
			str = szstr;
			if (rxnbr == 0) {
				slice_0.ptt = (str == "TRUE;");
			} else {
				slice_1.ptt = (str == "TRUE;");
			}
		}
		else if (rx.find("SPLIT_ENABLE:") == 0) { // split_enable:0,false;
			sscanf(rx.c_str(), "SPLIT_ENABLE:%d,%s", &rxnbr, szstr);
			str = szstr;
			if (rxnbr == 0) {
				slice_0.split = (str == "TRUE;");
			} else {
				slice_1.split = (str == "TRUE;");
			}
		}
		else if (rx.find("VOLUME:") == 0) { // volume:-16;
			sscanf(rx.c_str(), "VOLUME:%d", &ival);
			slice_0.vol = ival;
		}
		else if (rx.find("SQL_ENABLE:") == 0) { // sql_enable:1,false;
			sscanf(rx.c_str(), "SQL_ENABLE:%d,%s", &rxnbr, szstr);
			str = szstr;
			if (rxnbr == 0) {
				slice_0.sql = (str == "TRUE;");
			} else
				slice_1.sql = (str == "TRUE;");
		}
		else if (rx.find("SQL_LEVEL:") == 0) { // sql_level:0,-79;
			sscanf(rx.c_str(), "SQL_LEVEL:%d,%d", &rxnbr, &ival);
			if (rxnbr == 0) {
				slice_0.sql_level = ival;
			} else {
				slice_1.sql_level = ival;
			}
		}
		else if (rx.find("DRIVE:") == 0) { // drive:100;
			sscanf(rx.c_str(), "DRIVE:%d", &ival);
			slice_0.pwr = ival;
		}
		else if (rx.find("TUNE:") == 0) { // tune:0,false;
			sscanf(rx.c_str(), "TUNE:%d,%s", &rxnbr, szstr);
			str = szstr;
			if (rxnbr == 0) {
				slice_0.tune = (str == "TRUE;");
			} else {
				slice_1.tune = (str == "TRUE;");
			}
		}
		else if (rx.find("TX_POWER:") == 0) { // tx_power:4.3;
			sscanf(rx.c_str(), "TX_POWER:%f", &fval);
			slice_0.tx_power = fval;
		}
		else if (rx.find("TX_SWR:") == 0) { // tx_swr:1.3;
			sscanf(rx.c_str(), "TX_SWR:%f", &fval);
			slice_0.tx_swr = fval;
		}
	}
}

static WebSocket::pointer ws = (WebSocket::pointer)0;

static bool tci_run = true;
static std::string  send_txt = "";

static pthread_mutex_t send_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t run_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t *receiver = (pthread_t *)0;

static std::list<std::string> *send_list = (std::list<std::string> *)0;

void *tci_loop(void *)
{
	while (tci_run && tci_running()) {
		{
			guard_lock S(&send_mutex);
			while (!send_list->empty()) {
				send_txt = send_list->front();
				send_list->pop_front();
				if (send_txt.find("rx_smeter") == std::string::npos)
					tci_trace(2, "SEND:", send_txt.c_str());
				ws->send(send_txt);
				MilliSleep(50);
			}
		}
		ws->poll();
		ws->dispatch(handle_message);
		MilliSleep(10);
	}
	return NULL;
}

void tci_open(std::string address, std::string port)
{
	std::string url;
	url.assign("ws://").append(address).append(":").append(port);

	if (ws) tci_close();

	ws = WebSocket::from_url(url);

	if (ws && (ws->getReadyState() != WebSocket::CLOSED)) {

		if (!send_list)
			send_list = new std::list<std::string>;
		send_list->clear();

		tci_run = true;

		receiver = new pthread_t;
		if (pthread_create(receiver, NULL, tci_loop, NULL) < 0) {
			perror("pthread_create");
			exit(EXIT_FAILURE);
		}
	} else
		delete ws;
}

void tci_close()
{
	guard_lock R(&run_mutex);

	if (ws) {
		tci_run = false;

		pthread_join(*receiver, NULL);
		delete receiver;
		receiver = (pthread_t *)0;

		delete ws;
		ws = (WebSocket::pointer)0;
	}

	delete send_list;
	send_list = (std::list<std::string> *)0;
}

void tci_send(std::string txt)
{
	guard_lock R(&run_mutex);
	if (!send_list) return;
	{
		guard_lock S(&send_mutex);
		send_list->push_back(txt);
		if (txt.find("rx_smeter") == std::string::npos)
			tci_trace(2, "PUSH:", txt.c_str());
	}
}

bool tci_running()
{
	if (!ws) return false;
	return (ws->getReadyState() != WebSocket::CLOSED);
}
