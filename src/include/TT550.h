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

#ifndef _TT550_H
#define _TT550_H
// TenTec Pegasus computer controlled transceiver

#include <string>
#include "rigbase.h"

class RIG_TT550 : public rigbase {
private:
	double fwdpwr;
	double refpwr;
	double fwdv;
	double refv;

	int NVal;
	int FVal;
	int IVal;
	int TBfo;
	int IBfo;
	int TFreq;
	int Bfo;

	int encoder_count;
	int keypad_timeout;

	bool onA;

	int ATTlevel;
	int RFgain;

	double VfoAdj;

	bool auto_notch;
	bool noise_reduction;

	std::string pending;

	void showASCII(string, string);

	void set_vfoRX(long freq);
	void set_vfoTX(long freq);

	void get_302();

	void process_encoder( int v );
	void process_keypad(char c);
	void process_freq_entry(char c);
	void process_fkey(char c);
	void fkey_clear();
	void fkey_cw_plus();
	void fkey_cw_minus();
	void fkey_band_plus();
	void fkey_band_minus();
	void fkey_step_plus();
	void fkey_step_minus();

public:
	RIG_TT550();
	~RIG_TT550(){}
	
	void initialize();
	void shutdown();

	bool check();

	void set_vfo(long);
	long get_vfoA();
	void set_vfoA(long);
	long get_vfoB();
	void set_vfoB(long);

	void selectA();
	void selectB();

	void set_mode(int val);
	void set_modeA(int val);
	int  get_modeA();
	void set_modeB(int val);
	int  get_modeB();
	int  get_modetype(int n);

	void set_bw(int val);
	void set_bwA(int val);
	int  get_bwA();
	void set_bwB(int val);
	int  get_bwB();

	bool twovfos() { return true; }
	void set_split(bool);
	bool can_split() { return true; }

	int  get_smeter();
	int  get_swr();

	int  get_power_out();

	int  get_power_control();
	void set_power_control(double val);
	void get_pc_min_max_step(double &min, double &max, double &step) {
		min = 5; max = 100; step = 1; }

	void set_volume_control(int val);
	int  get_volume_control();

	void set_PTT_control(int val);
	int  get_PTT();

	void tune_rig(int);

	void set_attenuator(int val);
	int  get_attenuator();

	void set_preamp(int val);
	int  get_preamp();

	void set_if_shift(int val);
	bool get_if_shift(int &val);
	void get_if_min_max_step(int &min, int &max, int &step);

//	void set_notch(bool on, int val);
//	bool get_notch(int &val);
//	void get_notch_min_max_step(int &min, int &max, int &step);

	void set_noise(bool b);
	void set_mic_gain(int val);
	int  get_mic_gain();

	void set_mic_line(int val);
	void get_mic_min_max_step(int &min, int &max, int &step);

	void set_rf_gain(int val);
	int  get_rf_gain();
	void get_rf_min_max_step(int &min, int &max, int &step);

	int  adjust_bandwidth(int m);
	int  def_bandwidth(int m);

	void setRit(int v);
	int  getRit();
	void get_RIT_min_max_step(int &min, int &max, int &step) {
		min = -500; max = 500; step = 1; }

	void setXit(int v);
	int  getXit();
	void get_XIT_min_max_step(int &min, int &max, int &step) {
		min = -500; max = 500; step = 1; }

	void setBfo(int v);
	int  getBfo();
	void get_BFO_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 1000; step = 10; }

	void setVfoAdj(double v);

	void set_auto_notch(int v);
	void set_noise_reduction(int b);

	void set_line_out();
	void set_agc_level();
	void set_cw_wpm();
	void set_cw_weight();
	void enable_keyer();
	void set_cw_qsk();
	void set_cw_vol();
	bool set_cw_spot();
	void set_spot_onoff();
	void set_vox_onoff();
	void set_vox_gain();
	void set_vox_anti();
	void set_vox_hang();
	void set_aux_hang();
	void set_mon_vol();
	void set_compression(int, int);
	void set_squelch_level();
	void tuner_bypass();
	void enable_xmtr();
	void enable_tloop();
	void set_nb_level();

// TT-550 / at-11 internal tuner i/o commands
	void at11_bypass();
	void at11_autotune();
	void at11_cap_up();
	void at11_cap_dn();
	void at11_ind_up();
	void at11_ind_dn();
	void at11_loZ();
	void at11_hiZ();

};

extern const char *TT550_xmt_widths[];

#endif
