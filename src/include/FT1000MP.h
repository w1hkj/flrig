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

#ifndef FT1000MP_H
#define FT1000MP_H

#include "rigbase.h"


class RIG_FT1000MP : public rigbase {
public :
	RIG_FT1000MP();
	~RIG_FT1000MP() {};

	void initialize();

	bool can_split() { return true;}
	void set_split(bool val);
	bool twovfos() {return true;}

	bool check();

	long get_vfoA();
	void set_vfoA(long);
	long get_vfoB(void);
	void set_vfoB(long f);

	void set_modeA(int val);
	int  get_modeA();
	void set_modeB(int val);
	int  get_modeB();
	int  get_modetype(int n);

	void selectA();
	void selectB();

	int  get_bwA();
	void set_bwA(int n);
	int  get_bwB();
	void set_bwB(int n);

	void set_PTT_control(int val);
	void tune_rig();

	int  get_power_out(void);
	int  get_smeter(void);
	int  get_swr(void);
	int  get_alc(void);

	void set_auto_notch(int v); // usurped for Tuner ON/OFF control
	int  get_auto_notch();

	int  def_bandwidth(int m);
	int  adjust_bandwidth(int);

private:
	bool tune_on;
	void init_cmd();
	bool get_info(void);
	bool tuner_on;
};

#endif
