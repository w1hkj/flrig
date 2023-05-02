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
// aunsigned long int with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#ifndef _ICR71_H
#define _ICR71_H

#include "icom/ICbase.h"

class RIG_ICR71 : public RIG_ICOM {
public:
	RIG_ICR71();
	~RIG_ICR71(){}

	void initialize();
	bool check();

	unsigned long long get_vfoA();
	void set_vfoA(unsigned long long);

//	int  adjust_bandwidth(int m) { return 1; }

    void set_modeA(int val);
	int  get_modeA();
    int  get_smeter();

};

#endif
