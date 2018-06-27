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

#ifndef _IC706MKIIG_H
#define _IC706MKIIG_H

#include "ICbase.h"

class RIG_IC706MKIIG : public RIG_ICOM {
public:
	RIG_IC706MKIIG();
	~RIG_IC706MKIIG(){}

	void initialize();
	void selectA();
	void selectB();

	bool check();

	long get_vfoA ();
	void set_vfoA (long freq);

	long get_vfoB(void);
	void set_vfoB(long f);

	void set_modeA(int val);
	int  get_modeA();
	int  get_modetype(int n);
	void set_bwA(int val);
	int  get_bwA();

   int adjust_bandwidth(int val);
   int def_bandwidth(int m);

	int  get_smeter();
//	int  get_swr();
	void set_attenuator(int val);

	const char **bwtable(int);

//	void set_split(bool val);
//	bool can_split() { return true; }

	bool canswap() { return true; }

protected:
   int filter_nbr;

};

extern const char IC706MKIIGname_[];

#endif
