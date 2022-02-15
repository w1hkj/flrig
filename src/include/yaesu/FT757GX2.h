// ----------------------------------------------------------------------------
// Copyright (C) 2020
//              David Freese, W1HKJ
//              Michael Keitz, KD4QDM
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
// aunsigned long int with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#ifndef FT757GX2_H
#define FT757GX2_H

#include "rigbase.h"

class RIG_FT757GX2 : public rigbase {
public:
	RIG_FT757GX2();
	~RIG_FT757GX2(){};

	bool can_split() { return true;}
	void set_split(bool val);
	int  get_split();
	bool twovfos() {return false;}

	bool check();
	
	int  get_smeter();
	int  get_vfoAorB();

	unsigned long int get_vfoA();
	void set_vfoA(unsigned long int);
	void set_modeA(int val);
	int  get_modeA();

	unsigned long int get_vfoB();
	void set_vfoB(unsigned long int);
	void set_modeB(int val);
	int  get_modeB();

	void selectA();
	void selectB();

	bool get_info();

	bool snd_cmd(std::string cmd, size_t n);

// FT757GX2 does not support PTT via CAT.
//	void set_PTT_control(int val);
	int  get_PTT_control();

private:
	void init_cmd();
};

#endif
