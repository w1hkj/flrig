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

#ifndef FT857D_H
#define FT857D_H

#include "FT817.h"

class RIG_FT857D : public rigbase {
public:
	RIG_FT857D();
	~RIG_FT857D(){};

	bool check();

	long get_vfoA();
	void set_vfoA(long);

	long get_vfoB();
	void set_vfoB(long);

	void set_PTT_control(int val);

	void set_modeA(int val);
	int  get_modeA();
	int  get_modetype(int n);

	void set_modeB(int val);
	int  get_modeB();

	int  get_power_out(void);
	int  get_smeter(void);

	void selectA();
	void selectB();

	bool can_split();
	void set_split(bool val);

	int  adjust_bandwidth(int m) { return 0; }

	void set_band_selection(int v);

	void set_tones(int, int);
	void set_offset(int, int);

private:
	void init_cmd();

protected:
	bool onB;
};

class RIG_FT897D : public RIG_FT857D {
public :
	RIG_FT897D();
	~RIG_FT897D() {};
};

#endif
