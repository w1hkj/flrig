// ---------------------------------------------------------------------
//
// tci_io.hpp, a part of flrig
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
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the program; if not, write to the
//
//  Free Software Foundation, Inc.
//  51 Franklin Street, Fifth Floor
//  Boston, MA  02110-1301 USA.
//
// ---------------------------------------------------------------------
#ifndef TCI_IO_H
#define TCI_IO_H

#include "WSclient.h"

struct TCI_VFO {
	int freq;
	std::string bw;  // lower, upper pair
	std::string mod; // noun name
};

struct TCI_VALS {
	TCI_VFO A;
	TCI_VFO B;
	int dds;
	int vol, sql_level, pwr, smeter;
	bool ptt, tune, split, sql;
	float tx_power, tx_swr;
};

extern TCI_VALS slice_0, slice_1;

extern void print_vals(TCI_VALS &slice);
extern void parse_tci(std::string s);

extern void tci_open(std::string address, std::string port);
extern void tci_close();
extern void tci_send(std::string txt);
extern bool tci_running();

#endif
