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
	virtual bool ICswap() { return true; }

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

	int  get_smeter();
	void set_volume_control(int val);
	int  get_volume_control();
	void get_vol_min_max_step(int &min, int &max, int &step);
	void set_PTT_control(int val);
	void set_noise(bool val);
	int  get_noise();

	void set_mic_gain(int val);
	void get_mic_gain_min_max_step(int &min, int &max, int &step);
	void set_if_shift(int val);
	void get_if_min_max_step(int &min, int &max, int &step);
	void set_squelch(int val);
	void set_rf_gain(int val);
	void set_power_control(double val);

	void set_split(bool val);
	bool can_split() { return true;}

	int  next_attenuator();
	void set_attenuator(int val);
	int  get_attenuator();
	int  next_preamp();
	void set_preamp(int val);
	int  get_preamp();

	int  adjust_bandwidth(int);
	int  def_bandwidth(int);

	void set_auto_notch(int v);
	int  get_auto_notch();

	bool twovfos() { return false; }

	const char **bwtable(int);

protected:
	int  ICvol;

};


#endif
