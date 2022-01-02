// ----------------------------------------------------------------------------
// Copyright (C) 2021
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

#ifndef _FTdx10_H
#define _FTdx10_H

#include "rigbase.h"

class RIG_FTdx10 : public rigbase {
private:
	bool notch_on;
	int  m_60m_indx;
public:
	RIG_FTdx10();
	~RIG_FTdx10() {}

	virtual void initialize();

	virtual bool check();

	virtual unsigned long int get_vfoA();
	virtual void set_vfoA(unsigned long int);

	virtual unsigned long int get_vfoB();
	virtual void set_vfoB(unsigned long int);

	virtual int get_vfoAorB();

	virtual bool twovfos();
	virtual void selectA();
	virtual void selectB();
	virtual void A2B();
	virtual bool can_split();
	virtual void set_split(bool val);
	virtual int  get_split();

	virtual void swapAB();
	virtual bool canswap() { return true; }

	virtual void set_modeA(int val);
	virtual int  get_modeA();
	virtual int  get_modetype(int n);

	virtual void set_modeB(int val);
	virtual int  get_modeB();

	virtual void set_bwA(int val);
	virtual int  get_bwA();

	virtual void set_bwB(int val);
	virtual int  get_bwB();

	virtual int  adjust_bandwidth(int val);
	virtual int  def_bandwidth(int val);

	virtual void set_BANDWIDTHS(std::string s);
	virtual std::string get_BANDWIDTHS();

	virtual int  get_smeter();
	virtual int  get_swr();
	virtual int  get_alc();
	virtual int  get_power_out();
	virtual double get_power_control();
	virtual void set_power_control(double val);
	virtual void get_pc_min_max_step(double &min, double &max, double &step) {
		min = 5; pmax = max = 100; step = 1; }

	virtual void set_volume_control(int val);
	virtual int  get_volume_control();
	virtual void set_PTT_control(int val);
	virtual int  get_PTT();
	virtual void tune_rig(int);
	virtual int  get_tune();
	virtual int  next_attenuator();
	virtual void set_attenuator(int val);
	virtual int  get_attenuator();
	virtual int  next_preamp();
	virtual void set_preamp(int val);
	virtual int  get_preamp();

	virtual void set_if_shift(int val);
	virtual bool get_if_shift(int &val);
	virtual void get_if_min_max_step(int &min, int &max, int &step);

	virtual void set_notch(bool on, int val);
	virtual bool get_notch(int &val);
	virtual void get_notch_min_max_step(int &min, int &max, int &step);

	virtual void set_auto_notch(int v);
	virtual int  get_auto_notch();

	virtual void set_noise(bool b);
	virtual int  get_noise();

	virtual void set_mic_gain(int val);
	virtual int  get_mic_gain();
	virtual void get_mic_min_max_step(int &min, int &max, int &step);

	virtual void set_rf_gain(int val);
	virtual int  get_rf_gain();
	virtual void get_rf_min_max_step(int &min, int &max, int &step);
	virtual const char **bwtable(int);

	virtual void set_vox_onoff();
	virtual void set_vox_gain();
	virtual void set_vox_anti();
	virtual void set_vox_hang();
	virtual void set_vox_on_dataport();

	virtual void get_cw_wpm_min_max(int &min, int &max) {
		min = 4; max = 60; }

	virtual void set_cw_weight();
	virtual void set_cw_wpm();
	virtual void enable_keyer();
	virtual void set_cw_qsk();
//	virtual void set_cw_vol();
	virtual bool set_cw_spot();
//	virtual void set_cw_spot_tone();
	void set_break_in();
	int  get_break_in();

	virtual void get_band_selection(int v);

	void get_nr_min_max_step(int &min, int &max, int &step) {
		min = 1; max = 15; step = 1; }	
	void set_noise_reduction_val(int val);
	int  get_noise_reduction_val();
	void set_noise_reduction(int val);
	int  get_noise_reduction();

	void set_xcvr_auto_on();
	void set_xcvr_auto_off();

	void sync_date(char *dt);
	void sync_clock(char *tm);

};

#endif
