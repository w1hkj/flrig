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

#ifndef FT847_H
#define FT847_H

#include "rigbase.h"

class RIG_FT847 : public rigbase {
private:
	double fwdpwr;
	int afreq, amode, aBW;

public:
	RIG_FT847();
	~RIG_FT847(){};

	void initialize();

	bool get_info();

	bool can_split() { return false;}
	bool twovfos() {return false;}

	bool check();

	long get_vfoA();
	void set_vfoA(long);
	void set_modeA(int val);
	int  get_modeA();

	long get_vfoB();
	void set_vfoB(long);
	void set_modeB(int val);
	int  get_modeB();

	void set_PTT_control(int val);
	int  get_smeter();
	int  get_power_out();

	int  adjust_bandwidth(int m) { return 0; }

private:
	void init_cmd();
};


#endif
