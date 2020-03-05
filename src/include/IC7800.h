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

#ifndef _IC7800_H
#define _IC7800_H

#include "IC746.h"

class RIG_IC7800 : public RIG_IC746PRO {
public:
	RIG_IC7800();
	~RIG_IC7800(){}

	void initialize();

	void selectA();
	void selectB();

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
	void set_attenuator( int val );	//alh
	int  get_attenuator();		//alh

	void set_mic_gain(int v);

	void set_pbt_inner(int val);
	void set_pbt_outer(int val);

	int  get_pbt_inner();
	int  get_pbt_outer();

	const char *FILT(int &val);
	const char *nextFILT();

	void set_compression(int, int);
	void get_comp_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 10; step = 1; }
	void set_vox_onoff();
	void set_vox_gain();
	void set_vox_anti();
	void set_vox_hang();
	void get_vox_hang_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 200; step = 10; } //alh

	void get_cw_wpm_min_max(int &min, int &max) {
		min = 6; max = 60; }

	void get_cw_qsk_min_max_step(double &min, double &max, double &step) {
		min = 2.0; max = 13.0; step = 0.1; }

	void get_cw_spot_tone_min_max_step(int &min, int &max, int &step) {
		min = 300; max = 900; step = 5; }

	void set_cw_wpm();
	void set_cw_qsk();
	void set_cw_spot_tone();
	void set_cw_vol();

	const char **bwtable(int m);

	void get_pc_min_max_step(double &min, double &max, double &step) {
		min = 5; max = 200; step = 1; }

	void set_PTT_control(int val);
	int  get_PTT();

	void set_band_selection(int v);
	void get_band_selection(int v);

	bool get_notch(int &val);
	void set_notch(bool on, int val);
	void get_notch_min_max_step(int &min, int &max, int &step);

};

#endif
