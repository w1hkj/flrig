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

#ifndef FT817_H
#define FT817_H

#include "rigbase.h"

class RIG_FT817 : public rigbase {
public:
	RIG_FT817();
	~RIG_FT817(){};

	bool check();

	long get_vfoA();
	void set_vfoA(long);
	long get_vfoB();
	void set_vfoB(long);
	int  get_vfoAorB();
	void selectA();
	void selectB();

	void set_PTT_control(int val);
	void set_modeA(int val);
	int  get_modeA();
	void set_modeB(int val);
	int  get_modeB();
	int  get_modetype(int n);
	int  get_power_out();
	int  get_smeter();
	int  get_swr();
	int  get_alc();

	bool can_split() { return true;}
	void set_split(bool val);
	int  get_split();

	int  adjust_bandwidth(int m) { return 0;}

private:
	void init_cmd();
	void set_getACK();
};

#endif
