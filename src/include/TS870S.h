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

#ifndef _TS870S_H
#define _TS870S_H

#include "KENWOOD.h"

class RIG_TS870S : public KENWOOD {
public:
enum TS870MODES { tsLSB, tsUSB, tsCW, tsFM, tsAM, tsFSK, tsCWR, tsFSKR };

private:
	bool notch_on;

	int  nb_level;
//	bool data_mode;
	int  active_bandwidth;
	int  active_mode;

	bool att_on;
	bool is_TS870S;

public:
	RIG_TS870S();
	~RIG_TS870S(){}

	void initialize();
	virtual void shutdown();

	bool get_TS870Sid();

	bool check();

	long get_vfoA();
	void set_vfoA(long);
	long get_vfoB();
	void set_vfoB(long);

	int  vfo;  // 0, 1 or 2.  = A, B or Memory                 wbx2

	int  get_modetype(int n);
	void set_modeA(int val);
	int  get_modeA();
	void set_modeB(int val);
	int  get_modeB();

	int  adjust_bandwidth(int val);
	int  def_bandwidth(int val);
	int  set_widths(int val);
	void set_active_bandwidth();
	int  get_active_bandwidth();
	void set_bwA(int val);
	int  get_bwA();
	void set_bwB(int val);
	int  get_bwB();

	void selectA();
	void selectB();

	bool can_split() { return true;}
	bool twovfos() {return true;}

	int  get_smeter();
//	int  get_swr();
	int  get_power_out();
	int  get_power_control();
	void set_volume_control(int val);
	int  get_volume_control();
	void set_power_control(double val);
	void set_PTT_control(int val);
	void tune_rig();

	int  next_attenuator();
	void set_attenuator(int val);
	int  get_attenuator();
//	void set_preamp(int val);
//	int  get_preamp();

	void set_if_shift(int val);
	bool get_if_shift(int &val);
	void get_if_min_max_step(int &min, int &max, int &step);
	void set_notch(bool on, int val);
	bool get_notch(int &val);
	void get_notch_min_max_step(int &min, int &max, int &step);

	void set_noise(bool val);
	int  get_noise();

	void set_mic_gain(int val);
	int  get_mic_gain();
	void get_mic_min_max_step(int &min, int &max, int &step);

	bool sendTScommand(string, int, bool);

	const char **bwtable(int);
	const char **lotable(int);
	const char **hitable(int);

	const char * get_bwname_(int bw, int md);


};


#endif
