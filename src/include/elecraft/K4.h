// ----------------------------------------------------------------------------
// Copyright (C) 2022
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

#ifndef K4_H
#define K4_H

#include "rigbase.h"

class RIG_K4 : public rigbase {
protected:
	bool K4split;
public:
	RIG_K4();
	~RIG_K4(){}

	void initialize();

	bool check();

	int power_scale();

	unsigned long long get_vfoA();
	void set_vfoA(unsigned long long);
	unsigned long long get_vfoB();
	void set_vfoB(unsigned long long);

	void set_modeA(int val);
	int  get_modeA();

	void set_modeB(int val);
	int  get_modeB();

	void set_volume_control(int val);
	int  get_volume_control();
	void get_vol_min_max_step(double &min, double &max, double &step);

	void set_power_control(double val);
	double get_power_control();
	void get_pc_min_max_step(double &min, double &max, double &step);

	void set_rf_gain(int val);
	int  get_rf_gain();
	void get_rf_min_max_step(int &min, int &max, int &step);

	void set_mic_gain(int val);
	int  get_mic_gain();
	void get_mic_min_max_step(int &min, int &max, int &step);

	void set_if_shift(int val);
	bool get_if_shift(int &val);
	void get_if_min_max_step(int &min, int &max, int &step);
	void get_if_mid();

	void set_PTT_control(int val);
        int get_PTT();

	void set_attenuator(int val);
	int  get_attenuator();
	int  next_attenuator();

	void set_preamp(int val);
	int  get_preamp();
	int  next_preamp();

	int  get_smeter();

	void set_noise(bool on);
	int  get_noise();
	int  get_modetype(int n);
	void set_bwA(int val);
	int  get_bwA();
	void set_bwB(int val);
	int  get_bwB();
	int  get_power_out();
        void selectA();
        void selectB();
	
	int  get_agc();
	int  incr_agc();
	void set_agc_level(int);
	const char *agc_label();

	int  adjust_bandwidth(int m);
	int  def_bandwidth(int m);

	bool can_split();
	void set_split(bool val);
	int  get_split();
	bool twovfos() {return true;}

	void shutdown();

	void set_pbt_values(int val);

private:
	double  minpwr;
	double  maxpwr;
	double  steppwr;

// if shift value
	int  if_mid;
	int  if_min;
	int  if_max;

	bool  split_on;
/*
	int  get_swr();
	void tune_rig();
*/
};

#endif
