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

#ifndef FT736R_H
#define FT736R_H

#include "rigbase.h"

class RIG_FT736R : public rigbase {
public:
	RIG_FT736R();
	~RIG_FT736R(){};

	bool check();
	void initialize();
	void shutdown();

	unsigned long long get_vfoA();
	unsigned long long get_vfoB();

	void set_vfoA(unsigned long long);
	void set_vfoB(unsigned long long);

	int  get_modeA();
	int  get_modeB();

	int  get_modetype(int n);

	void set_modeA(int);
	void set_modeB(int);

	void selectA();
	void selectB();

	void set_PTT_control(int val);
	int  get_PTT();

	int  get_smeter();

private:
	void init_cmd();
	void set_vfo(unsigned long long);
	void set_mode(int);
	void send();

	// S Meter data minimum & maximum values from rig.  wbx
	int rig_smin;
	int rig_smax;

};

#endif
