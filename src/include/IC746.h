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

#ifndef _IC746_H
#define _IC746_H

#include "ICbase.h"

class RIG_IC746 : public RIG_ICOM {
public:
	RIG_IC746();
	virtual ~RIG_IC746(){}
	virtual void initialize();

	virtual void selectA();
	virtual void selectB();

	bool check();

	virtual long get_vfoA(void);
	virtual void set_vfoA(long f);
	virtual long get_vfoB(void);
	virtual void set_vfoB(long f);

	virtual int  get_smeter();
	virtual void set_volume_control(int val);
	virtual int  get_volume_control();
	virtual void get_vol_min_max_step(int &min, int &max, int &step);
	virtual void set_PTT_control(int val);
	virtual int  get_PTT();
	virtual void set_noise(bool val);
	virtual int  get_noise();
	virtual void set_noise_reduction(int val);
	virtual int  get_noise_reduction();
	virtual void set_noise_reduction_val(int val);
	virtual int  get_noise_reduction_val();
	virtual void set_attenuator(int val);
	virtual int  get_attenuator();
	virtual int  next_preamp();
	virtual void set_preamp(int val);
	virtual int  get_preamp();

	virtual int  get_modetype(int n);
	virtual void set_modeA(int val);
	virtual int  get_modeA();
	virtual void set_bwA(int val);
	virtual int  get_bwA();
	virtual void set_modeB(int val);
	virtual int  get_modeB();
	virtual void set_bwB(int val);
	virtual int  get_bwB();

	virtual void set_mic_gain(int val);
	virtual void get_mic_gain_min_max_step(int &min, int &max, int &step);

	virtual void set_if_shift(int val);
	virtual void get_if_min_max_step(int &min, int &max, int &step);

	void set_pbt_inner(int val);
	void set_pbt_outer(int val);

	int  get_pbt_inner();
	int  get_pbt_outer();

	virtual void set_squelch(int val);
	virtual void set_rf_gain(int val);
	virtual void set_power_control(double val);

	virtual void set_split(bool val);
	virtual int  get_split();
	virtual bool can_split() { return true;}

	virtual bool canswap() { return true; }

protected:
	int  preamp_level;
	int  atten_level;
	int  ICvol;
	int  filter_nbr;

};

class RIG_IC746PRO : public RIG_IC746 {
public:
	RIG_IC746PRO();
	virtual ~RIG_IC746PRO(){}

	virtual void initialize();

	virtual void set_modeA(int val);
	virtual int  get_modeA();

	virtual void set_bwA(int val);
	virtual int  get_bwA();
	virtual void set_modeB(int val);
	virtual int  get_modeB();
	virtual void set_bwB(int val);
	virtual int  get_bwB();

	virtual int  get_swr();
	virtual int  get_alc();
	virtual int  get_power_out();

	virtual int  adjust_bandwidth(int m);
	virtual int  def_bandwidth(int m);
	
	virtual void set_attenuator(int val);
	virtual int  get_attenuator();
	virtual void set_preamp(int val);
	virtual int  get_preamp();

	virtual void set_mic_gain(int val);
	virtual int  get_mic_gain();
	virtual void get_mic_gain_min_max_step(int &min, int &max, int &step);

	virtual void set_notch(bool on, int val);
	virtual bool get_notch(int &val);
	virtual void get_notch_min_max_step(int &min, int &max, int &step);

	virtual void set_rf_gain(int val);
	virtual int  get_rf_gain();

	virtual void set_squelch(int val);
	virtual int  get_squelch();

	virtual void set_if_shift(int val);
	virtual void get_if_min_max_step(int &min, int &max, int &step);

	void set_pbt_inner(int val);
	void set_pbt_outer(int val);

	int  get_pbt_inner();
	int  get_pbt_outer();

	virtual void set_power_control(double val);
	virtual int  get_power_control();

	virtual void set_split(bool val);
	virtual int  get_split();
	bool can_split() { return true;}

	bool twovfos() { return false; }

	const char **bwtable(int);

	void set_band_selection(int v);
	void get_band_selection(int v);

};

extern const char *IC746modes_[];
extern const char IC746_mode_type[];
extern const char *IC746_widths[];
extern const char *IC746PROmodes_[];
extern const char IC746PRO_mode_type[];
extern const char *IC746PRO_SSBwidths[];
extern const char *IC746PRO_RTTYwidths[];
extern const char *IC746PRO_AMFMwidths[];


#endif
