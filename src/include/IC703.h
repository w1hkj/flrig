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

#ifndef _IC703_H
#define _IC703_H

#include "ICbase.h"

class RIG_IC703 : public RIG_ICOM {
public:
	RIG_IC703();
	~RIG_IC703(){}

	void initialize();

	bool check();

	long get_vfoA(void);
	void set_vfoA(long);

	void set_modeA(int val);
	int  get_modeA();
	void set_bwA(int val);
	int  get_modetype(int n);
	int  get_bwA();

	int  get_smeter();
	int  get_power_out();
	int  get_swr();
	int  get_alc();

	void set_attenuator(int val);
	int  get_attenuator();
	int  next_preamp();
	void set_preamp(int val);
	int  get_preamp();
	void set_compression(int, int);
	void get_comp_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 10; step = 1; }
	void set_vox_onoff();
	void set_vox_gain();
	void set_vox_anti();
	void set_vox_hang();
	void set_PTT_control(int val);

	void get_cw_wpm_min_max(int &min, int &max) {
		min = 6; max = 60; }

	void get_cw_qsk_min_max_step(double &min, double &max, double &step) {
		min = 2.0; max = 13.0; step = 0.1; }

	void get_cw_spot_tone_min_max_step(int &min, int &max, int &step) {
		min = 300; max = 900; step = 300; }

	void set_cw_wpm();
	void set_cw_qsk();
	void set_cw_spot_tone();
	void set_cw_vol();

	void set_noise(bool val);
	int  get_noise();
	void set_noise_reduction(int val);
	int  get_noise_reduction();
	void set_noise_reduction_val(int val);
	int  get_noise_reduction_val();

	void set_volume_control(int val);
	int  get_volume_control();
	void get_vol_min_max_step(int &min, int &max, int &step);

	int  get_mic_gain();
	void set_mic_gain(int val);
	void get_mic_gain_min_max_step(int &min, int &max, int &step);

	void set_if_shift(int val);
	bool get_if_shift(int &val);
	void get_if_min_max_step(int &min, int &max, int &step);

	void set_pbt_inner(int val);
	void set_pbt_outer(int val);

	int  get_pbt_inner();
	int  get_pbt_outer();

	void set_squelch(int val);
	int  get_squelch();

	void set_rf_gain(int val);
	int  get_rf_gain();

	void set_power_control(double val);
	int  get_power_control();

	void set_split(bool val);
	int  get_split();

protected:
	int  preamp_level;
	int  atten_level;
};

#endif
