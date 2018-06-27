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

#ifndef _TT516_H
#define _TT516_H

#include "rigbase.h"

class RIG_TT516 : public rigbase {
private:
	double fwdpwr;
	double refpwr;
	double fwdv;
	double refv;

public:
	RIG_TT516();
	~RIG_TT516(){}

	void initialize();

	bool check();

	long get_vfoA();
	void set_vfoA(long);
	long get_vfoB();
	void set_vfoB(long);
	void selectA();
	void selectB();

	int  get_modetype(int n);

	void set_modeA(int val);
	int  get_modeA();
	void set_bwA(int val);
	int  get_bwA();

	void set_modeB(int val);
	int  get_modeB();
	void set_bwB(int val);
	int  get_bwB();

	bool can_split() { return true;}
	void set_split(bool val);
	bool twovfos() {return true;}

	int  get_smeter();
	int  get_swr();
	int  get_power_out();
//	int  get_power_control();
//	void set_volume_control(int val);
//	int  get_volume_control();
//	void set_power_control(double val);
	void set_PTT_control(int val);
//	void tune_rig();
	void set_attenuator(int val);
	int  get_attenuator();
//	void set_preamp(int val);
//	int  get_preamp();
	void set_if_shift(int val);
	bool get_if_shift(int &val);
	void get_if_min_max_step(int &min, int &max, int &step);
//	void set_notch(bool on, int val);
//	bool get_notch(int &val);
//	void get_notch_min_max_step(int &min, int &max, int &step);
	void set_noise(bool b);
//	void set_mic_gain(int val);
//	int  get_mic_gain();
//	void get_mic_min_max_step(int &min, int &max, int &step);
	void checkresponse();
	int  adjust_bandwidth(int);
	int  def_bandwidth(int);
	const char **bwtable(int);
};


#endif
