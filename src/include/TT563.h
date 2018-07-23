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

#ifndef _TT563_H
#define _TT563_H

#include "ICbase.h"

class RIG_TT563 : public RIG_ICOM {
public:
	RIG_TT563();
	~RIG_TT563(){}

	bool check();

	void selectA();
	void selectB();

	long get_vfoA();
	void set_vfoA(long);

	long get_vfoB();
	void set_vfoB(long);

	bool  canswap() { return true; }

	void swapAB();
	void A2B();

	int  get_vfoAorB();

	void set_split(bool);
	int  get_split();
	bool can_split();

	void set_PTT_control(int val);
	int  get_PTT();

	void set_modeA(int val);
	int  get_modeA();

	void set_modeB(int val);
	int  get_modeB();

	int  get_modetype(int n);

	int  adjust_bandwidth(int m) { return 1; }

};

#endif
