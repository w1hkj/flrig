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

#ifndef _TT538_H
#define _TT538_H

#include "rigbase.h"

class RIG_TT538 : public rigbase {
private:
	double fwdpwr;
	double refpwr;
	double fwdv;
	double refv;
	double VfoAdj;

public:
	RIG_TT538();
	~RIG_TT538(){}

	void initialize();

	bool check();

	unsigned long long get_vfoA();
	void set_vfoA(unsigned long long);

	unsigned long long get_vfoB();
	void set_vfoB(unsigned long long);

	void set_modeA(int val);
	int  get_modeA();
//	void set_modeB(int val);
//	int  get_modeB();
	int  get_modetype(int n);

	void set_bwA(int val);
	int  get_bwA();
//	void set_bwB(int val);
//	int  get_bwB();

	bool twovfos() { return false; }
//	void set_split(bool);
	bool can_split() {return false;}

	int  get_smeter();
	void set_volume_control(int val);
	int  get_volume_control();
	void set_attenuator(int val);
	int  get_attenuator();
	void set_rf_gain(int val);
	int  get_rf_gain();
	void set_if_shift(int val);
	bool get_if_shift(int &val);
	void get_if_min_max_step(int &min, int &max, int &step);
	void setVfoAdj(double v);
	int  get_swr();
	int  get_power_out();
//	double get_power_control();
//	void set_power_control(double val);
	void set_PTT_control(int val);
//	void tune_rig();
//	void set_preamp(int val);
//	int  get_preamp();
//	void set_notch(bool on, int val);
//	bool get_notch(int &val);
//	void get_notch_min_max_step(int &min, int &max, int &step);
//	void set_noise(bool b);
//	void set_mic_gain(int val);
//	int  get_mic_gain();
//	void get_mic_min_max_step(int &min, int &max, int &step);
	void checkresponse(std::string s);
	void shutdown();

	int  adjust_bandwidth(int m);

};


#endif
