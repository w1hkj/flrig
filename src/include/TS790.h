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

#ifndef _TS790_H
#define _TS790_H

#include "rigbase.h"

class RIG_TS790 : public rigbase {
private:
	bool beatcancel_on;
	bool preamp_on;
	bool att_on;
	const char *_mode_type;
public:
	RIG_TS790();
	~RIG_TS790(){}

	void initialize();

	bool sendTScommand(string, int, bool);

	bool check();

	void set_PTT_control(int val);
	int  get_PTT();

	long get_vfoA();
	void set_vfoA(long);

	long get_vfoB();
	void set_vfoB(long);

	void selectA();
	void selectB();

	int  get_modeA();
	void set_modeA(int val);

	int  get_modeB();
	void set_modeB(int val);

	int  get_modetype(int n);

	int  adjust_bandwidth(int val) { return 0; }

	bool can_split();
	void set_split(bool val);
	int  get_split();

	int  get_smeter();

//	void set_bwA(int val);
//	int  get_bwA();
//	void set_bwB(int val);
//	int  get_bwB();
//	int  def_bandwidth(int val);

//	bool twovfos() {return true;}

};


#endif
