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

#ifndef FT1000_H
#define FT1000_H

#include "rigbase.h"


class RIG_FT1000 : public rigbase {
private:

	int afreq, amode, aBW;
	int bfreq, bmode, bBW;

	bool notch_on;
//int  m_60m_indx;

public:
	RIG_FT1000();
	~RIG_FT1000(){};

	void initialize();
// 	void post_initialize();

	void selectA();
	void selectB();

	bool get_info();

	bool check();

	long get_vfoA();
	int  get_modeA();
	int  get_bwA();
	void set_vfoA(long);
	void set_modeA(int val);
	void set_bwA(int val);

	bool canswap() {return true;}
	void swapAB();
// 	void A2B();

	bool can_split() { return true;}
	void set_split(bool val);
	int  get_split();

	long get_vfoB();
	int  get_modeB();
	int  get_bwB();
	void set_vfoB(long);
	void set_modeB(int val);
	void set_bwB(int val);

	void set_PTT_control(int val);
	void tune_rig(int);
	int  get_smeter();
	int  get_power_out();

	int  adjust_bandwidth(int);

	bool twovfos() {return true;}

// 	void set_band_selection(int v);


private:

	void init_cmd();


};


#endif
