// ----------------------------------------------------------------------------
// Copyright (C) 2022
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

#ifndef _QCXP_H
#define _QCXP_H

#include "kenwood/KENWOOD.h"

class RIG_QCXP : public KENWOOD {
private:
	bool preamp_on;
	bool att_on;
	const char *_mode_type;

public:
	RIG_QCXP();
	~RIG_QCXP(){}

	void initialize();
	void shutdown() {}

	unsigned long int get_vfoA();
	void set_vfoA(unsigned long int);
	unsigned long int get_vfoB();
	void set_vfoB(unsigned long int);

	void selectA();
	void selectB();

	void set_split(bool val);
	bool can_split();
	int  get_split();

	void set_PTT_control(int val);
	int  get_PTT();

	int  get_modetype(int n);
	void set_modeA(int val);
	int  get_modeA();
	void set_modeB(int val);
	int  get_modeB();

	int  get_smeter();

	void set_bwA(int val);
	int  get_bwA();
	void set_bwB(int val);
	int  get_bwB();

	void sync_date(char *dt);
	void sync_clock(char *tm);

	std::string read_menu(int m1, int m2);
};


#endif
