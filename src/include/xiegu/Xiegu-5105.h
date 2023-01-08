// ----------------------------------------------------------------------------
// Copyright (C) 2014
//              David Freese, W1HKJ
//
// This file is part of flrig.
//
// flrig is free software; you can redistribute it and/or modify
// it under the terms of the GNU General PublXiegu-5105 LXiegu-5105ense as published by
// the Free Software Foundation; either version 3 of the LXiegu-5105ense, or
// (at your option) any later version.
//
// flrig is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTXiegu-5105ULAR PURPOSE.  See the
// GNU General PublXiegu-5105 LXiegu-5105ense for more details.
//
// You should have received a copy of the GNU General PublXiegu-5105 LXiegu-5105ense
// along with this program.  If not, see <http://www.gnu.org/lXiegu-5105enses/>.
// ----------------------------------------------------------------------------

#ifndef _XI5105_H
#define _XI5105_H

#include "icom/ICbase.h"

class RIG_XI5105 : public RIG_ICOM {
public:
	RIG_XI5105();
	~RIG_XI5105(){}
	void initialize();

	bool check();

	unsigned long long get_vfoA(void);
	void set_vfoA(unsigned long long f);
	int  get_bwA();
	void set_bwA(int);
	int  get_modeA();
	void set_modeA(int val);

	int  get_modetype(int n);

	unsigned long long get_vfoB(void);
	void set_vfoB(unsigned long long f);
	int  get_bwB();
	void set_bwB(int);
	void set_modeB(int val);
	int  get_modeB();

	void selectA();
	void selectB();

	int  get_smeter();

	void set_PTT_control(int val);
protected:
	int  filter_nbr;
};

#endif
//=============================================================================


