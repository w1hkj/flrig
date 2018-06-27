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

#ifndef _RAY152_H
#define _RAY152_H

#include "rigbase.h"
#include "rigpanel.h"

class RIG_RAY152 : public rigbase {
public:
	RIG_RAY152();
	~RIG_RAY152(){}

	void initialize();
	void shutdown();

	void get_data();

	bool check();

	long get_vfoA();
	void set_vfoA(long);
	long get_vfoB();
	void set_vfoB(long);
	void set_PTT_control(int val);
	void set_modeA(int val);
	int  get_modeA();
	void set_modeB(int val);
	int  get_modeB();
	int  get_modetype(int n);

	int  get_volume_control() {return vol;}
	void get_vol_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 100; step = 1; }
	void set_volume_control(int val);

	void set_rf_gain(int val);
	int  get_rf_gain() {return rfg;}
	void get_rf_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 100; step = 1; }

	void setRit(int v);
	int  getRit();
	void get_RIT_min_max_step(int &min, int &max, int &step) {
		min = -200; max = 200; step = 10; }

	int  get_smeter(void);
	int  get_power_out(void);

	void set_squelch(int val);
	int  get_squelch();

	void set_noise(bool on);
	int  get_noise();

	void set_auto_notch(int v);
	int  get_auto_notch();

	int  adjust_bandwidths(int m) { return 0; }
	
private:
	int vol;
	int rfg;
	int squelch;
	int nb;
	int nb_set;
	int agc;
	bool dumpdata;
};

#endif
