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

#ifndef _KENWOOD_H
#define _KENWOOD_H

#include "rigbase.h"

class KENWOOD : public rigbase {
protected:
	bool notch_on;
	int  preamp_level;
	int  att_level;
	int  nb_level;
	int  noise_reduction_level;
	bool data_mode;
	int  active_mode;
	int  active_bandwidth;
	bool rxtxa;
	bool rxona;

public:
enum TS590MODES { LSB, USB, CW, FM, AM, FSK, CWR, FSKR, LSBD, USBD, FMD };
	KENWOOD() {}
	~KENWOOD(){}

	virtual void initialize() = 0;
	virtual void shutdown() = 0;

	virtual bool check();

	virtual long get_vfoA();
	virtual void set_vfoA(long);
	virtual long get_vfoB();
	virtual void set_vfoB(long);

	virtual void selectA();
	virtual void selectB();

	virtual bool can_split() { return true; }
	virtual void set_split(bool val);
	virtual int  get_split();
	virtual bool twovfos() { return true; }

	virtual void set_PTT_control(int val);
	virtual int  get_PTT();

//	virtual int  get_modetype(int n);
//	virtual void set_modeA(int val);
//	virtual int  get_modeA();
//	virtual void set_modeB(int val);
//	virtual int  get_modeB();

//	virtual int  adjust_bandwidth(int val);
//	virtual int  def_bandwidth(int val);
//	virtual int  set_widths(int val);
//	virtual void set_active_bandwidth();
//	virtual int  get_active_bandwidth();
//	virtual void set_bwA(int val);
//	virtual int  get_bwA();
//	virtual void set_bwB(int val);
//	virtual int  get_bwB();

//	virtual int  get_smeter();
//	virtual int  get_swr();
//	virtual int  get_alc();
	virtual void select_swr();
	virtual void select_alc();

//	virtual int  get_power_out();
//	virtual int  get_power_control();
	virtual void set_volume_control(int val);
	virtual int  get_volume_control();
//	virtual void set_power_control(double val);

	virtual void tune_rig(int);
	virtual int  get_tune();
	virtual bool tuning();

//	virtual void set_attenuator(int val);
//	virtual int  get_attenuator();
//	virtual void set_preamp(int val);
//	virtual int  get_preamp();

	virtual void set_if_shift(int val);
	virtual bool get_if_shift(int &val);
	virtual void get_if_min_max_step(int &min, int &max, int &step);

	virtual void set_noise(bool b);
	virtual int  get_noise();

	virtual void set_mic_gain(int val);
	virtual int  get_mic_gain();
	virtual void get_mic_min_max_step(int &min, int &max, int &step);

	virtual void set_squelch(int val);
	virtual int  get_squelch();
	virtual void get_squelch_min_max_step(int &min, int &max, int &step);

	virtual void set_rf_gain(int val);
	virtual int  get_rf_gain();
	virtual void get_rf_min_max_step(int &min, int &max, int &step);

//	virtual void set_noise_reduction_val(int val);
//	virtual int  get_noise_reduction_val();
//	virtual void set_noise_reduction(int val);
//	virtual int  get_noise_reduction();
//	virtual void get_nr_min_max_step(int &min, int &max, int &step);

//	virtual void set_notch(bool on, int val);
//	virtual bool get_notch(int &val);
//	virtual void get_notch_min_max_step(int &min, int &max, int &step);

//	virtual void set_auto_notch(int v);
//	virtual int  get_auto_notch();

//	virtual const char **bwtable(int);
//	virtual const char **lotable(int);
//	virtual const char **hitable(int);

//	virtual const char * get_bwname_(int bw, int md);

};


#endif
