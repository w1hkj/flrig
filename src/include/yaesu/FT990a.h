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

#ifndef FT990A_H
#define FT990A_H

#include "rigbase.h"

class RIG_FT990A : public rigbase {
private:
	double fwdpwr;
	double refpwr;
	double fwdv;
	double refv;
	int amode, aBW;
	int bmode, bBW;
	unsigned long long afreq;
	unsigned long long bfreq;

public:
	RIG_FT990A();
	~RIG_FT990A(){};
	
	void initialize();

	bool can_split() { return true;}
	void set_split(bool val);
	bool twovfos() {return false;}

	bool check();

	bool get_info();

	unsigned long long get_vfoA();
	void set_vfoA(unsigned long long);
	void set_modeA(int val);
	int  get_modeA();
	void set_bwA(int val);
	int  get_bwA();

	unsigned long long get_vfoB();
	void set_vfoB(unsigned long long);
	void set_modeB(int val);
	int  get_modeB();
	void set_bwB(int val);
	int  get_bwB();

	int  get_vfoAorB();

	int  def_bandwidth(int m);

	void selectA();
	void selectB();

	void set_PTT_control(int val);
	void tune_rig(int);
	int  get_smeter();
	int  get_swr();
	int  get_power_out();

	int  adjust_bandwidth(int);

private:
	void init_cmd();
};


#endif
