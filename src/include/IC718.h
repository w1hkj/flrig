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

#ifndef _IC718_H
#define _IC718_H

#include "ICbase.h"

class RIG_IC718 : public RIG_ICOM {
public:
	RIG_IC718();
	~RIG_IC718(){}
	void initialize();

	bool check();

	long get_vfoA(void);
	void set_vfoA(long f);
	int  get_bwA();
	void set_bwA(int);
	int  get_modeA();
	void set_modeA(int val);

	int  get_modetype(int n);

	long get_vfoB(void);
	void set_vfoB(long f);
	int  get_bwB();
	void set_bwB(int);
	void set_modeB(int val);
	int  get_modeB();

	void selectA();
	void selectB();

	void set_volume_control(int val);
	int  get_volume_control();
	void get_vol_min_max_step(int &min, int &max, int &step);

	void set_attenuator(int val);
	int  get_smeter();

	void set_noise(bool val);
	int  get_noise();
	void set_nb_level(int val);
	int  get_nb_level();

	void set_noise_reduction(int val);
	int  get_noise_reduction();
	void set_noise_reduction_val(int val);
	int  get_noise_reduction_val();
	void get_nr_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 15; step = 1; }

	void set_preamp(int val);
	int  get_preamp();
	void set_rf_gain(int val);
	int  get_rf_gain();
	void set_squelch(int val);
	void set_power_control(double val);
	int  get_power_control();

	void set_mic_gain(int val);
	int  get_mic_gain();
	void get_mic_gain_min_max_step(int &min, int &max, int &step);

	void set_compression(int, int);
	void set_auto_notch(int val);
	int  get_auto_notch();
	void set_vox_onoff();

	int  adjust_bandwidth(int m);

	bool can_split() { return true;}
	void set_split(bool val);
	int  get_split();

	bool  canswap() { return true; }

protected:
	int  filter_nbr;
};

#endif
//=============================================================================


