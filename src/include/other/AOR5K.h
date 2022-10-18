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

#ifndef AOR5K_H
#define AOR5K_H

#include "rigbase.h"

class RIG_AOR5K : public rigbase {
public:
	RIG_AOR5K();
	~RIG_AOR5K(){}

	void initialize();

	bool check();

	unsigned long long get_vfoA();
	void set_vfoA(unsigned long long);
	unsigned long long get_vfoB();
	void set_vfoB(unsigned long long);
	void set_volume_control(int val);
	int  get_volume_control();
	void get_vol_min_max_step(int &min, int &max, int &step);

	void set_modeA(int val);
	int  get_modeA();
	void set_modeB(int val);
	int  get_modeB();
	void selectA();
	void selectB();

	int  next_attenuator();
	void set_attenuator(int val);
	int  get_attenuator();

	int  get_smeter();
	void set_noise(bool on);
	int  get_noise();
	int  get_modetype(int n);
	void set_bwA(int val);
	int  get_bwA();
	void set_bwB(int val);
	int  get_bwB();

	int  adjust_bandwidth(int m);
	int  def_bandwidth(int m);

	bool twovfos() {return true;}

	void shutdown();

	int  get_agc();
        int  incr_agc();
        const char *agc_label();
        int  agc_val();




private:
	int agcval;

/*
	int  get_swr();
	void tune_rig();
*/
};

#endif
