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

#ifndef _TS440_H
#define _TS440_H

#include "kenwood/KENWOOD.h"

class RIG_TS440 : public KENWOOD {
private:
	const char *_mode_type;
public:
	RIG_TS440();
	~RIG_TS440(){}

	void initialize();

	bool check();

	unsigned long int get_vfoA();
	void set_vfoA(unsigned long int);
	int  get_modeA();
	void set_modeA(int val);

	unsigned long int get_vfoB();
	void set_vfoB(unsigned long int);
	int  get_modeB();
	void set_modeB(int val);

	void selectA();
	void selectB();

	int  get_modetype(int n);

	int  get_PTT();

	int  get_IF();

	void shutdown () {}

/* not available
	void set_bwA(int val);
	int  get_bwA();

	void set_bwB(int val);
	int  get_bwB();

	int  def_bandwidth(int val);
	int  adjust_bandwidth(int val) { return 0; }

	int  get_smeter();
	int  get_swr();
	void set_PTT_control(int val);

	bool can_split();
	void set_split(bool val);
	int  get_split();
	bool twovfos() {return true;}

*/

};


#endif
