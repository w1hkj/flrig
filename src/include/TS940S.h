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

#ifndef TS940S_H
#define TS940S_H

#include "rigbase.h"

class RIG_TS940S : public rigbase {
public:
	RIG_TS940S();
	~RIG_TS940S(){}

	long get_vfoA();
	void set_vfoA(long);
	long get_vfoB();
	void set_vfoB(long);

	bool check();

	int  getvfoAorB();
	int  get_modetype(int n);
	
	void set_modeA(int val);
	int  get_modeA();
	void set_modeB(int val);
        int  get_modeB();

	void selectA();
	void selectB();
	
	int  adjust_bandwidth(int val) { return 0; }

//	void set_attenuator(int val);
//	int  get_attenuator();
//	void set_preamp(int val);
//	int  get_preamp();
//	int  get_smeter();
//	int  get_swr();
//	int  get_power_out();
//	int  get_power_control();
//	void set_volume_control(double val);
//	void set_power_control(double val);
	void set_PTT_control(int val);
	int  get_PTT();

	void tune_rig();
	
	int get_split();
	void set_split(bool val);
	bool can_split();
	
//	void set_bwA(int val);

};

#endif
