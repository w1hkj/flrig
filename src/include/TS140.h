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

#ifndef TS140_H
#define TS140_H

#include "rigbase.h"

class RIG_TS140 : public rigbase {
public:
	RIG_TS140();
	~RIG_TS140(){}

	bool check();

	long get_vfoA();
	void set_vfoA(long);
	void set_modeA(int val);
	int  get_modeA();
	int  get_modetype(int);

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
//	void tune_rig();
//	void set_bwA(int val);

};

#endif
