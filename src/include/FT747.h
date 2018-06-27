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

#ifndef FT747_H
#define FT747_H

#include "rigbase.h"

class RIG_FT747 : public rigbase {
public:
	RIG_FT747();
	~RIG_FT747(){};

	bool can_split() { return true;}
	void set_split(bool val);
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

	void selectA();
	void selectB();

	bool get_info();

	bool snd_cmd(string cmd, size_t n);

	void set_PTT_control(int val);
	int  get_PTT_control();

private:
	void init_cmd();
};

#endif
