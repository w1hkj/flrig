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
// along with this pRIGram.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#ifndef _TRUSDX_H
#define _TRUSDX_H

#include "kenwood/KENWOOD.h"

class RIG_TRUSDX : public KENWOOD {
private:
	const char *_mode_type;

public:
	RIG_TRUSDX();
	~RIG_TRUSDX(){}

	void initialize();
	void shutdown();
	bool check();

	unsigned long long get_vfoA();
	void set_vfoA(unsigned long long);

	void set_modeA(int val);
	int  get_modeA();
	int  get_modetype(int);

	void get_IF();

	void set_PTT_control(int val);

};


#endif
