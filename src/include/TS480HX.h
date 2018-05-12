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

#ifndef _TS480HX_H
#define _TS480HX_H

#include "KENWOOD.h"

class RIG_TS480HX : public KENWOOD {
private:
	bool beatcancel_on;
	bool preamp_on;
	bool att_on;
	const char *_mode_type;
	bool menu_45;
	int  alc;
	bool swralc_polled;
public:
	RIG_TS480HX();
	~RIG_TS480HX(){}

	void initialize();
	void shutdown();

	void check_menu_45();

//	long get_vfoA();
//	void set_vfoA(long);
//	long get_vfoB();
//	void set_vfoB(long);

//	void selectA();
//	void selectB();

//	void set_split(bool val);
//	bool can_split();
//	int  get_split();
//	bool twovfos() {return true;}

	void set_PTT_control(int val);
	int  get_PTT();

	int  get_modetype(int n);
	void set_modeA(int val);
	int  get_modeA();
	void set_modeB(int val);
	int  get_modeB();

	int  get_smeter();
	int  get_swr();
	int  get_alc();
	int  get_power_out();
//	void tune_rig();

	void set_bwA(int val);
	int  get_bwA();
	void set_bwB(int val);
	int  get_bwB();
//	void set_volume_control(int val);
//	int  get_volume_control();
	void set_power_control(double val);
	int  get_power_control();
	void get_pc_min_max_step(double &min, double &max, double &step) {
		min = 5; max = 200; step = 1; }

	void set_if_shift(int val);
	bool get_if_shift(int &val);
	void get_if_min_max_step(int &min, int &max, int &step);

	void set_noise(bool b);
	int  get_noise();

//	void set_mic_gain(int val);
//	int  get_mic_gain();
//	void get_mic_min_max_step(int &min, int &max, int &step);

//	void set_squelch(int val);
//	int  get_squelch();
//	void get_squelch_min_max_step(int &min, int &max, int &step);

	void set_rf_gain(int val);
	int  get_rf_gain();
	void get_rf_min_max_step(int &min, int &max, int &step);

	int  get_agc();
	int  incr_agc();
	const char *agc_label();
	int  agc_val();

	void set_attenuator(int val);
	int  get_attenuator();
	void set_preamp(int val);
	int  get_preamp();

	int  adjust_bandwidth(int m);
	int  def_bandwidth(int m);
	int  set_widths(int val);

	void set_noise_reduction_val(int val);
	int  get_noise_reduction_val();
	void set_noise_reduction(int val);
	int  get_noise_reduction();
	void get_nr_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 9; step = 1; }

	void set_auto_notch(int v);
	int  get_auto_notch();

	const char **bwtable(int);
	const char **lotable(int);
	const char **hitable(int);

	const char * get_bwname_(int bw, int md);

};


#endif
