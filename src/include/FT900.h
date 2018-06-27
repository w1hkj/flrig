// ----------------------------------------------------------------------------
// Copyright (C) 2014
//              David Freese, W1HKJ
//
// 2015-10-04 adapted from FT890.h by Ernst F. Schroeder DJ7HS
// 2016-04-03 made getinfo() private  DJ7HS
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

#ifndef _FT900_H
#define _FT900_H

#include "rigbase.h"

extern const char FT900name_[];
extern const char *FT900modes_[];
extern const char *FT900_widths[];

class RIG_FT900 : public rigbase {
private:
	double fwdpwr;
	double refpwr;
	double fwdv;
	double refv;
	int afreq, amode, aBW;
	int bfreq, bmode, bBW;
	int vfoAorB;
	int splitison;

public:
	RIG_FT900();
	~RIG_FT900(){};
	
	void initialize();

	bool can_split() { return true;}
	void set_split(bool val);
	int  get_split();
	
	bool twovfos() {return true;}

	bool canswap() {return false;}

	bool check();

	long get_vfoA();
	void set_vfoA(long);
	void set_modeA(int val);
	int  get_modeA();

	long get_vfoB();
	void set_vfoB(long);
	void set_modeB(int val);
	int  get_modeB();

	int  get_vfoAorB();

	void selectA();
	void selectB();

	void set_PTT_control(int val);

	int  get_smeter();
	int  get_power_out();

private:
	void init_cmd();
	bool get_info();

};


#endif
