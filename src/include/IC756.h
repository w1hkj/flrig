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

#ifndef _IC756_H
#define _IC756_H

#include "IC746.h"
//#include "ICbase.h"

class RIG_IC756 : public RIG_IC746 {
public:
	RIG_IC756();
	~RIG_IC756(){}
	void initialize();

	virtual bool canswap() { return true; }

	void set_pbt_inner(int val);
	void set_pbt_outer(int val);

};

class RIG_IC756PRO : public RIG_ICOM {
public:
	RIG_IC756PRO();
	~RIG_IC756PRO(){}

	void initialize();

	void selectA();
	void selectB();

	long get_vfoA(void);
	void set_vfoA(long f);
	int  get_modetype(int n);
	void set_modeA(int val);
	int  get_modeA();

	long get_vfoB(void);
	void set_vfoB(long f);
	void set_modeB(int val);
	int  get_modeB();

	void set_bwA(int val);
	int  get_bwA();
	void set_bwB(int val);
	int  get_bwB();

	const char *FILT(int val);
	const char *nextFILT();

	int  get_FILT(int mode);
	void set_FILT(int filter);

	void set_FILTERS(std::string s);
	std::string get_FILTERS();

	int  get_smeter();
	void set_volume_control(int val);
	int  get_volume_control();
	void get_vol_min_max_step(int &min, int &max, int &step);
	void set_PTT_control(int val);
	int  get_PTT();
	void set_noise(bool val);
	int  get_noise();

	void set_mic_gain(int val);
	void get_mic_gain_min_max_step(int &min, int &max, int &step);

	void set_if_shift(int val);
	void get_if_min_max_step(int &min, int &max, int &step);

	void set_pbt_inner(int val);
	void set_pbt_outer(int val);

	void set_squelch(int val);

	void set_notch(bool on, int val);
	bool get_notch(int &val);
	void get_notch_min_max_step(int &min, int &max, int &step);

	void set_rf_gain(int val);
	int  get_rf_gain();

	void set_power_control(double val);
	int  get_power_control();

	void set_split(bool val);
	bool can_split() { return true;}

	int  next_attenuator();
	void set_attenuator(int val);
	int  get_attenuator();
	int  next_preamp();
	void set_preamp(int val);
	int  get_preamp();

	void set_noise_reduction(int val);
	int  get_noise_reduction();
	void set_noise_reduction_val(int val);
	int  get_noise_reduction_val();
	void get_nr_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 16; step = 1; }

	int  adjust_bandwidth(int);
	int  def_bandwidth(int);

	void set_auto_notch(int v);
	int  get_auto_notch();

	bool twovfos() { return false; }

	void swapAB();
	void A2B();

	const char **bwtable(int);

protected:
	int  ICvol;

};


#endif
