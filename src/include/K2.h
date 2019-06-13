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

#ifndef _H
#define _H

#include "rigbase.h"

class RIG_K2 : public rigbase {
protected:
	bool hipower;
	bool K2split;
public:
	RIG_K2();
	~RIG_K2(){}

	void initialize();

	bool get_info();
	bool twovfos() {return true;}

	bool check();

	long get_vfoA();
	void set_vfoA(long);
	void set_modeA(int val);
	int  get_modeA();
	void set_bwA(int val);
	int  get_bwA();

	long get_vfoB();
	void set_vfoB(long);
	void set_modeB(int val);
	int  get_modeB();
	void set_bwB(int val);
	int  get_bwB();

	void selectA();
	void selectB();
//	void A2B();

	int  get_modetype(int n);

	void set_attenuator(int val);
	int  get_attenuator();
	void set_preamp(int val);
	int  get_preamp();

	void tune_rig(int);
	void set_PTT_control(int val);
	void get_pc_min_max_step(double &min, double &max, double &step);
	int  get_power_control();
	void set_power_control(double val);
	int  get_smeter();
	int  get_power_out();

	bool can_split();
	void set_split(bool val);
	int  get_split();

	int  adjust_bandwidth(int m);

};

#endif
