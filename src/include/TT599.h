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

#ifndef _TT599_H
#define _TT599_H

#include "ICbase.h"

class RIG_TT599 : public rigbase {
private:
	int fwdpwr;
	int refpwr;

public:
	RIG_TT599();
	~RIG_TT599(){}

	void initialize();
	void shutdown();

	bool check();

	long get_vfoA();
	void set_vfoA(long);
	long get_vfoB();
	void set_vfoB(long);
	void set_modeA(int val);
	int  get_modeA();
	void set_modeB(int val);
	int  get_modeB();
	void set_bwA(int bw);
	int  get_bwA();
	void set_bwB(int bw);
	int  get_bwB();

	bool can_split() { return true;}
	void set_split(bool val);
	int  get_split();
	bool twovfos() { return false;}//true;}

	void set_PTT_control(int val);
	int  get_modetype(int n);
	void set_preamp(int);
	int  get_preamp();
	int  get_power_control(void);
	void set_power_control(double val);
	void set_auto_notch(int v);
	int  get_auto_notch();
	void set_attenuator(int val);
	int  get_attenuator();
	int  get_smeter();
	int  get_power_out();
	int  get_swr();

	int  adjust_bandwidth(int);
	int  def_bandwidth(int);

	void get_nr_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 9; step = 1; }
	int  get_noise_reduction_val();
	void set_noise_reduction_val(int val);


//	void set_agc_level();
};

#endif
