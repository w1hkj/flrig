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

#ifndef _TS990_H
#define _TS990_H

#include "rigbase.h"

class RIG_TS990 : public rigbase {

private:
enum TS990MODES {
LSB,   USB,   CW,   FM,   AM,
FSK,   PSK,   CWR,  FSKR, PSKR,
LSBD1, USBD1, FMD1, AMD1,
LSBD2, USBD2, FMD2, AMD2,
LSBD3, USBD3, FMD3, AMD3 };

private:
	bool menu_0607;
	bool menu_0608;
	bool save_menu_0607;
	bool save_menu_0608;

	bool notch_on;

	int  nb_level;
	bool data_mode;
	int  active_mode;
	int  active_bandwidth;
	bool rxtxa;

	bool att_on;

// default mode values
	int A_default_SH_WI;
	int A_default_SH_WI_D1;
	int A_default_SH_WI_D2;
	int A_default_SH_WI_D3;
	int A_default_HI_LO;
	int A_default_FM;
	int A_default_FM_D1;
	int A_default_FM_D2;
	int A_default_FM_D3;
	int A_default_CW;
	int A_default_AM;
	int A_default_AM_D1;
	int A_default_AM_D2;
	int A_default_AM_D3;
	int A_default_FSK;
	int A_default_PSK;

	int B_default_SH_WI;
	int B_default_SH_WI_D1;
	int B_default_SH_WI_D2;
	int B_default_SH_WI_D3;
	int B_default_HI_LO;
	int B_default_FM;
	int B_default_FM_D1;
	int B_default_FM_D2;
	int B_default_FM_D3;
	int B_default_CW;
	int B_default_AM;
	int B_default_AM_D1;
	int B_default_AM_D2;
	int B_default_AM_D3;
	int B_default_FSK;
	int B_default_PSK;

public:
	RIG_TS990();
	~RIG_TS990(){}

	void initialize();
	void shutdown();

	bool check();

	long get_vfoA();
	void set_vfoA(long);
	long get_vfoB();
	void set_vfoB(long);

	int  get_modetype(int n);
	void set_modeA(int val);
	int  get_modeA();
	void set_modeB(int val);
	int  get_modeB();

	void read_menu_0607();
	void read_menu_0608();
	void set_menu_0607(int);
	void set_menu_0608(int);

	int  adjust_bandwidth(int val);
	int  def_bandwidth(int val);
	int  set_widths(int val);
	void set_active_bandwidth();
	int  get_active_bandwidth();
	void set_bwA(int val);
	int  get_bwA();
	void set_bwB(int val);
	int  get_bwB();

	void selectA();
	void selectB();

	bool can_split() { return false; }//return true;}
	void set_split(bool val);
	int  get_split();
	bool twovfos() {return true;}

	int  get_smeter();
	int  get_swr();
	int  get_alc();
 	int  get_agc();

	void set_volume_control(int val);
	int  get_volume_control();

	void set_power_control(double val);
	int  get_power_control(void);
	void get_pc_min_max_step(double &min, double &max, double &step) {
		min = 0; max = 200; step = 1; }
	int  get_power_out();

	void set_PTT_control(int val);
	int  get_PTT();
	void tune_rig();

	int  next_attenuator();
	void set_attenuator(int val);
	int  get_attenuator();

	int  next_preamp();
	void set_preamp(int val);
	int  get_preamp();

	void set_ssbfilter(int val);
	int  get_ssbfilter();

	void set_if_shift(int val);
	bool get_if_shift(int &val);
	void get_if_min_max_step(int &min, int &max, int &step);

	void set_noise(bool b);
	void set_mic_gain(int val);
	int  get_mic_gain();
	void get_mic_min_max_step(int &min, int &max, int &step);

	void set_squelch(int val);
	int  get_squelch();
	void get_squelch_min_max_step(int &min, int &max, int &step);

	void set_rf_gain(int val);
	int  get_rf_gain();
	void get_rf_min_max_step(int &min, int &max, int &step);

	void set_noise_reduction_val(int val);
	int  get_noise_reduction_val();
	void set_noise_reduction(int val);
	int  get_noise_reduction();
	void get_nr_min_max_step(int &min, int &max, int &step) {
		min = 1; max = 10; step = 1; }

	void set_notch(bool on, int val);
	bool get_notch(int &val);
	void get_notch_min_max_step(int &min, int &max, int &step);

	void set_auto_notch(int v);
	int  get_auto_notch();

	bool sendTScommand(string, int, bool);

	void set_monitor(bool);

	const char **bwtable(int);
	const char **lotable(int);
	const char **hitable(int);

	const char * get_bwname_(int bw, int md);

};

#endif
