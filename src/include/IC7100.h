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

#ifndef _IC7100_H
#define _IC7100_H

#include "IC746.h"

class RIG_IC7100 : public RIG_ICOM {
public:
	RIG_IC7100();
	~RIG_IC7100(){}

	bool CW_sense;

	void initialize();

	void selectA();
	void selectB();

	bool check();

	long get_vfoA ();
	void set_vfoA (long freq);

	long get_vfoB(void);
	void set_vfoB(long f);

	void set_modeA(int val);
	int  get_modeA();

	void set_modeB(int val);
	int  get_modeB();

	void set_bwA(int val);
	int  get_bwA();

	void set_bwB(int val);
	int  get_bwB();

	int  adjust_bandwidth(int m);
	int  def_bandwidth(int m);

	void set_attenuator( int val );
	int  get_attenuator();

	void set_preamp(int val);
	int  next_preamp();
	int  get_preamp();

	void set_mic_gain(int v);
	int  get_mic_gain();
	void get_mic_gain_min_max_step(int &min, int &max, int &step);

	void set_noise(bool val);
	int get_noise();

	void set_noise_reduction(int val);
	int get_noise_reduction();

	void set_noise_reduction_val(int val);
	int get_noise_reduction_val();
	void get_nr_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 15; step = 1; }

	void set_compression(int, int);
	void get_comp_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 10; step = 1; }
	void set_vox_onoff();
	void set_vox_gain();
	void set_vox_anti();
	void set_vox_hang();
	void get_vox_hang_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 200; step = 10; }

	void get_cw_wpm_min_max(int &min, int &max) {
		min = 6; max = 48; }

	void get_cw_qsk_min_max_step(double &min, double &max, double &step) {
		min = 2.0; max = 13.0; step = 0.1; }

	void get_cw_spot_tone_min_max_step(int &min, int &max, int &step) {
		min = 300; max = 900; step = 5; }

	void set_cw_wpm();
	void set_cw_qsk();
	void set_cw_spot_tone();
	void set_cw_vol();

	const char **bwtable(int m);

	bool  canswap() { return true; }

	bool can_split();
	void set_split(bool val);
	int get_split();

	void set_PTT_control(int val);
	int  get_PTT();

	void set_power_control(double val);
	int  get_power_control();
	void get_pc_min_max_step(double &min, double &max, double &step);

	void set_volume_control(int val);
	int  get_volume_control();
	void get_vol_min_max_step(int &min, int &max, int &step);

	void set_rf_gain(int val);
	int  get_rf_gain();
	void get_rf_min_max_step(double &min, double &max, double &step);

	void set_squelch(int val);
	int  get_squelch();

	int  get_smeter();
	int  get_alc();
	int  get_swr();
	int  get_power_out();

	int  get_auto_notch();
	void set_auto_notch(int val);

	bool get_notch(int &val);
	void set_notch(bool on, int val);
	void get_notch_min_max_step(int &min, int &max, int &step);

	void set_if_shift(int val);
	bool get_if_shift(int &val);
	void get_if_min_max_step(int &min, int &max, int &step);

	void set_pbt_inner(int val);
	void set_pbt_outer(int val);

	int  get_pbt_inner();
	int  get_pbt_outer();

	void set_nb_level(int val);
	int  get_nb_level();

	void set_band_selection(int v);
	void get_band_selection(int v);

	void   setVfoAdj(double v);
	double getVfoAdj();

	int   get_FILT(int mode);
	void  set_FILT(int filter);
	const char *FILT(int val);
	const char *nextFILT();
	void  set_FILTERS(std::string s);
	std::string get_FILTERS();

	void set_BANDWIDTHS(std::string s);
	std::string get_BANDWIDTHS();

	void set_xcvr_auto_on();
	void set_xcvr_auto_off();

};

#endif
