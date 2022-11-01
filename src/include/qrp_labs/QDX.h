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

#ifndef _QDX_H
#define _QDX_H

#include "kenwood/KENWOOD.h"

class RIG_QDX : public KENWOOD {
private:
	bool beatcancel_on;
	bool preamp_on;
	bool att_on;
	const char *_mode_type;
	bool menu_45;
	int  alc;
	bool swralc_polled;
public:
	RIG_QDX();
	~RIG_QDX(){}

	void initialize();
	void shutdown();

	unsigned long long get_vfoA ();
	void set_vfoA (unsigned long long freq);
	unsigned long long get_vfoB ();
	void set_vfoB (unsigned long long freq);

	void selectA();
	void selectB();

	void set_split(bool val) ;
	bool can_split();
	int  get_split();

	void set_bwA(int val);
	int  get_bwA();
	void set_bwB(int val);
	int  get_bwB();

	void set_PTT_control(int val);
	int  get_PTT();

	int  get_IF();

	void set_modeA(int val);
	int  get_modeA();
	void set_modeB(int val);
	int  get_modeB();

	void setVfoAdj(double v);
	double getVfoAdj();
	void get_vfoadj_min_max_step(double &min, double &max, double &step);

	void set_vox_onoff();
	int  get_vox_onoff();

	void set_volume_control(int val);
	int  get_volume_control();
	void get_vol_min_max_step(int &min,  int &max, int &step);

	void setRit(int val);
	int  getRit();

};


#endif
