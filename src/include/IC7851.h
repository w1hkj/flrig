// ----------------------------------------------------------------------------
// Copyright (C) 2018
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

#ifndef _IC7851_H
#define _IC7851_H

#include "IC746.h"

class RIG_IC7851 : public RIG_ICOM {
public:
	RIG_IC7851();
	~RIG_IC7851(){}

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

	bool can_split();
	void set_split(bool);
	int  get_split();

	bool canswap() { return true; }

//	bool hasA2b() { return true; }

	void set_bwA(int val);
	int  get_bwA();

	void set_bwB(int val);
	int  get_bwB();

	int  adjust_bandwidth(int m);
	int  def_bandwidth(int m);

	int  next_attenuator();
	void set_attenuator( int val );
	int  get_attenuator();

	void set_mic_gain(int v);

	void set_volume_control(int val);
	int  get_volume_control();
	void get_vol_min_max_step(int &min, int &max, int &step);

	void set_compression(int, int);
	void get_comp_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 10; step = 1; }
	void set_vox_onoff();
	void set_vox_gain();
	void set_vox_anti();
	void set_vox_hang();
	void get_vox_hang_min_max_step(int &min, int &max, int &step);

	void set_cw_wpm();
	void get_cw_wpm_min_max(int &min, int &max);

	void set_cw_qsk();
	void get_cw_qsk_min_max_step(double &min, double &max, double &step);

	void set_cw_spot_tone();
	void get_cw_spot_tone_min_max_step(int &min, int &max, int &step);

	void set_cw_vol();

	void set_pbt_inner(int val);
	void set_pbt_outer(int val);

	int  get_pbt_inner();
	int  get_pbt_outer();

	const char **bwtable(int m);

	void get_pc_min_max_step(double &min, double &max, double &step);

	void set_PTT_control(int val);
	int  get_PTT();

	int  get_smeter();
	int  get_power_out(void);
	int  get_swr(void);
	int  get_alc(void);

	void set_band_selection(int v);
	void get_band_selection(int v);

	bool get_notch(int &val);
	void set_notch(bool on, int val);
	void get_notch_min_max_step(int &min, int &max, int &step);

	void set_xcvr_auto_on();
	void set_xcvr_auto_off();
};

#endif
