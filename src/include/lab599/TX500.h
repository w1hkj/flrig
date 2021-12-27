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
// aunsigned long int with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#ifndef _TX500_H
#define _TX500_H

#include "rigbase.h"

class RIG_TX500 : public rigbase {
public:

enum TX500MODES { LSB, USB, CW, FM, AM, DIG, CWR };

	RIG_TX500();
	~RIG_TX500(){}

	unsigned long int get_vfoA();
	void set_vfoA(unsigned long int);
	unsigned long int get_vfoB();
	void set_vfoB(unsigned long int);

	void selectA();
	void selectB();

	bool can_split() { return true;}
	void set_split(bool val);
	int  get_split();
	bool twovfos() {return true;}

	int  get_modetype(int n);

	void set_modeA(int val);
	int  get_modeA();

	void set_modeB(int val);
	int  get_modeB();

	void set_attenuator(int val);
	int  get_attenuator();
	void set_preamp(int val);
	int  get_preamp();

	bool check();

	int  get_smeter();

	void set_PTT_control(int val);
	int  get_PTT();

//	void initialize();
//	void shutdown();

//	double get_power_control();

//	void set_power_control(double val);
//	void get_pc_min_max_step(double &min, double &max, double &step) {
//		min = 5; pmax = max = 100; step = 1; }

//	int  get_power_out();

//	void set_PTT_control(int val);

//	void set_bwA(int val);
//	int  get_bwA();

//	void set_bwB(int val);
//	int  get_bwB();
//	int  def_bandwidth(int);

//	int  adjust_bandwidth(int val);
//	int  set_widths(int val);

//	int  get_swr();
//	int  get_alc();
//	void select_swr();
//	void select_alc();
//	void set_volume_control(int val);
//	int  get_volume_control();
//	void tune_rig();

//	void set_if_shift(int val);
//	bool get_if_shift(int &val);
//	void get_if_min_max_step(int &min, int &max, int &step);

//	void set_notch(bool on, int val);
//	bool get_notch(int &val);
//	void get_notch_min_max_step(int &min, int &max, int &step);
//	void set_auto_notch(int v);
//	int  get_auto_notch();

//	void set_noise_reduction_val(int val);
//	int  get_noise_reduction_val();
//	void set_noise_reduction(int val);
//	int  get_noise_reduction();
//	void get_nr_min_max_step(int &min, int &max, int &step) {
//		min = 0; max = 9; step = 1; }

//	void set_noise(bool b);
//	int  get_noise();

//	void set_mic_gain(int val);
//	int  get_mic_gain();
//	void get_mic_min_max_step(int &min, int &max, int &step);
//	void set_rf_gain(int val);
//	int  get_rf_gain();
//	void get_rf_min_max_step(int &min, int &max, int &step) {
//		min = 0; max = 100; step = 1; }

//	bool tuning();

//	bool sendTScommand(string, int, bool);

//	const char **bwtable(int);
//	const char **lotable(int);
//	const char **hitable(int);

//	const char * get_bwname_(int bw, int md);

};


#endif
