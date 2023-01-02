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

#ifndef TCI_SDR2_H
#define TCI_SDR2_H

#include "rigbase.h"

#include "tci_io.h"

extern int tci_centers[];
extern int tci_center;
extern int tci_nbr_centers;
extern const char *TCI_centers[];
extern void tci_adjust_widths();

enum SDR2_TYPE { UNK, PRO, DX };

enum TCI_MODES {
TCI_AM, TCI_SAM, TCI_DSB, TCI_LSB, TCI_USB,
TCI_CW, TCI_NFM, TCI_DIGL, TCI_DIGU, TCI_WFM,
TCI_DRM };

// class RIG_TCI_SDR is never directly declared.  used as a base class for
// RIG_TCI_SUNDX and RIG_TCI_SUNPRO

class RIG_TCI_SDR : public rigbase {
private:
	bool preamp_on;
	bool att_on;
	const char *_mode_type;
	int FilterInner_A;
	int FilterOuter_A;
	int FilterInner_B;
	int FilterOuter_B;

public:
	RIG_TCI_SDR();
	~RIG_TCI_SDR(){}

	void initialize();
	void shutdown();

	bool  check();

	unsigned long long get_vfoA();
	void set_vfoA(unsigned long long);
	void set_modeA(int val);
	int  get_modeA();
	void set_bwA(int val);
	int  get_bwA();

	unsigned long long get_vfoB();
	void set_vfoB(unsigned long long);
	void set_modeB(int val);
	int  get_modeB();
	void set_bwB(int val);
	int  get_bwB();

	int  adjust_bandwidth(int val);

	void selectA();
	void selectB();

	void set_split(bool val);
	bool can_split();
	int  get_split();
	bool twovfos() {return true;}

	void set_PTT_control(int val);
	int  get_PTT();

	int  get_modetype(int n);

	int  get_smeter();
	int  get_swr();
	int  get_power_out();
	void tune_rig();

	void set_volume_control(int val);
	int  get_volume_control();
	void set_volume_min_max_step(double &min, double &max, double &step);

	void set_power_control(double val);
	double get_power_control();
	void get_pc_min_max_step(double &min, double &max, double &step);

//	void set_if_shift(int val);
//	bool get_if_shift(int &val);
//	void get_if_min_max_step(int &min, int &max, int &step);

	void set_noise(bool b);
	int  get_noise();

//	void set_mic_gain(int val);
//	int  get_mic_gain();
//	void get_mic_min_max_step(int &min, int &max, int &step);

	void set_squelch(int val);
	int  get_squelch();
	void get_squelch_min_max_step(int &min, int &max, int &step);

	void set_rf_gain(int val);
	int  get_rf_gain();
	void get_rf_min_max_step(int &min, int &max, int &step);

	int  get_agc();
	int  incr_agc();
	const char *agc_label();
	int  agc_val();

	void set_attenuator(int val);
	int  get_attenuator();
	void set_preamp(int val);
	int  get_preamp();

	int  def_bandwidth(int m);
	int  set_widths(int val);

	int  get_pbt_inner();
	int  get_pbt_outer();
	void set_pbt(int inner, int outer);

	void set_noise_reduction_val(int val);
	int  get_noise_reduction_val();
	void set_noise_reduction(int val);
	int  get_noise_reduction();
	void get_nr_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 9; step = 1; }

//	void set_auto_notch(int v);
//	int  get_auto_notch();

	const char **bwtable(int);
	const char *get_bwname_(int bw, int md);

	int		sdrtype;
};

class RIG_TCI_SUNPRO : public RIG_TCI_SDR {
public:
	RIG_TCI_SUNPRO();
};

class RIG_TCI_SUNDX : public RIG_TCI_SDR {
public:
	RIG_TCI_SUNDX();
};

#endif
