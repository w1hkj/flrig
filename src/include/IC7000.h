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

#ifndef _IC7000_H
#define _IC7000_H

#include "IC746.h"

class RIG_IC7000 : public RIG_IC746PRO {
public:
	RIG_IC7000();
	~RIG_IC7000(){}

	void initialize();

	bool check();

	long get_vfoA(void);
	void set_vfoA(long f);
	int  get_bwA();
	void set_bwA(int);
	int  get_modeA();
	void set_modeA(int val);

	long get_vfoB(void);
	void set_vfoB(long f);
	int  get_bwB();
	void set_bwB(int);
	void set_modeB(int val);
	int  get_modeB();

	void selectA();
	void selectB();

	int  adjust_bandwidth(int m);
	int  def_bandwidth(int m);
	void set_attenuator( int val );
	int  get_attenuator();
	void set_preamp(int val);
	int  get_preamp();

	void set_volume_control(int val);
	int  get_volume_control();
	void get_vol_min_max_step(int &min, int &max, int &step);
	void set_rf_gain(int val);
	int  get_rf_gain();
	void set_squelch(int val);
	int  get_squelch();
	void set_power_control(double val);
	int  get_power_control();
	int  get_auto_notch();
	void set_auto_notch(int v);
	int  get_smeter();
	int  get_power_out();
	int  get_alc();

	void set_split(bool v);
	int  get_split();

	bool canswap() { return true; }

	int  get_mic_gain();
	void set_mic_gain(int val);
	void get_mic_gain_min_max_step(int &min, int &max, int &step);

	void set_notch(bool on, int val);
	bool get_notch(int &val);
	void get_notch_min_max_step(int &min, int &max, int &step);

	void set_noise(bool val);
	int  get_noise();

	void set_noise_reduction(int val);
	int  get_noise_reduction();

	void set_noise_reduction_val(int val);
	int  get_noise_reduction_val();
	void set_compression(int, int);
	void get_comp_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 10; step = 1; }

	void set_pbt_inner(int val);
	void set_pbt_outer(int val);

	int  get_pbt_inner();
	int  get_pbt_outer();

	void set_PTT_control(int val);
	int  get_PTT();

	void set_band_selection(int v);
	void get_band_selection(int v);

};


#endif
