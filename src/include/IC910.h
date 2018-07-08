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

#ifndef _ICgeneric_H
#define _ICgeneric_H

#include "IC746.h"

class RIG_IC910H : public RIG_IC746 {
public:
	RIG_IC910H();
	~RIG_IC910H(){};

	void initialize();

	void set_vfoA(long);
	void set_compression(int, int);
	void set_vox_onoff();
	void set_vox_gain();
	void set_vox_anti();
	void set_vox_hang();
	
	int  get_smeter();

	int  adjust_bandwidth(int m) { return 0; }

	void set_pbt_inner(int val);
	void set_pbt_outer(int val);

	int  get_pbt_inner();
	int  get_pbt_outer();

	bool  canswap() { return true; }

private:
	bool comp_is_on;
};


#endif
