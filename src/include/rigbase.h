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

#ifndef _RIG_BASE_H
#define _RIG_BASE_H

#include <string>
#include <cmath>
#include <cstdlib>

#include <FL/Fl.H>
#include "util.h"
#include "debug.h"
#include "rig_io.h"

using namespace std;

struct FREQMODE {
	long freq;
	int  imode;
	int  iBW;
	int  src;
};

struct GUI {
	Fl_Widget*	W;
	int			x;
	int			y;
	int			w;
};

enum {onA, onB};
enum {UI, XML};

enum {BINARY, STRING};

enum {
	NONE,

	FT100D, FT450,  FT450D, FT747,    FT767,  FT817,
	FT847,  FT857D, FT890,    FT897D, FT920,
	FT950,  FT990,  FT1000MP, FT2000, FTdx1200, FTdx3000,
	FT5000,

	IC703, IC706MKIIG, IC718, IC728, IC735, IC746,
	IC746PRO, IC756, IC756PRO, IC756PRO2, IC756PRO3,
	IC7000, IC7100, IC7200, IC7410, IC7600, IC7700,
	IC9100, IC910H,

	K2, K3,

	PCR1000,

	RAY152,

	TS140, TS450S, TS480HX, TS480SAT, TS570, TS590S, TS870S, TS990, TS2000,

	TT516, TT535, TT538, TT550, TT563, TT566, TT588, TT599, // 48

	LAST_RIG
};

class rigbase {
public:
	const char * name_;
	const char ** modes_;
	const char ** bandwidths_;
	const char ** dsp_lo;
	const char *  lo_tooltip;
	const char *  lo_label;
	const char ** dsp_hi;
	const char *  hi_tooltip;
	const char *  hi_label;
	const int  * bw_vals_;

	GUI *widgets;

	int  comm_baudrate;
	int  stopbits;
	int  comm_retries;
	int  comm_wait;
	int  comm_timeout;
	bool comm_echo;
	bool comm_rtscts;
	bool comm_rtsplus;
	bool comm_dtrplus;
	bool comm_catptt;
	bool comm_rtsptt;
	bool comm_dtrptt;
	int  serloop_timing;
	int  CIV;
	int  defaultCIV;
	bool USBaudio;

	FREQMODE A;
	FREQMODE B;
	int  inuse;

	int  modeA;
	int  bwA;
	long freqA;
	int  modeB;
	int  bwB;
	long freqB;
	int  precision;
	int  ndigits;
	bool can_change_alt_vfo;
	int  active_mode;		// wbx

	int  def_mode;
	int  def_bw;
	long def_freq;
	int  max_power;
	int  last_bw;
	int  bpf_center;
	int  pbt;

	bool PbtActive;
	int  RitFreq;
	bool RitActive;
	int  XitFreq;
	bool XitActive;
	int  split;

	int  if_shift_min;
	int  if_shift_max;
	int  if_shift_step;
	int  if_shift_mid;

	bool has_xcvr_auto_on_off;
	bool has_a2b;
	bool has_vfo_adj;
	bool has_rit;
	bool has_xit;
	bool has_bfo;
	bool has_power_control;
	bool has_volume_control;
	bool has_mode_control;
	bool has_bandwidth_control;
	bool has_dsp_controls;
	bool has_micgain_control;
	bool has_mic_line_control;
	bool has_notch_control;
	bool has_auto_notch;
	bool has_noise_control;
	bool has_noise_reduction_control;
	bool has_noise_reduction;
	bool has_attenuator_control;
	bool has_preamp_control;
	bool has_ifshift_control;
	bool has_ptt_control;
	bool has_tune_control;
	bool has_swr_control;
	bool has_alc_control;
	bool has_rf_control;
	bool has_sql_control;
	bool has_smeter;
	bool has_power_out;
	bool has_split;
	bool has_split_AB;
	bool has_data_port;

	bool has_extras;
	bool has_nb_level;
	bool has_agc_level;
	bool has_cw_wpm;
	bool has_cw_vol;
	bool has_cw_spot;
	bool has_cw_spot_tone;
	bool has_cw_qsk;
	bool has_cw_break_in;
	bool has_cw_delay;
	bool has_cw_weight;
	bool has_cw_keyer;
	bool has_vox_onoff;
	bool has_vox_gain;
	bool has_vox_anti;
	bool has_vox_hang;
	bool has_vox_on_dataport;
	bool has_compression;
	bool has_compON;
	bool use_line_in;
	bool has_bpf_center;
	bool has_special;
	bool has_ext_tuner;
	bool has_band_selection;

	bool restore_mbw;

	bool has_get_info;

	int  data_type;

// for pia TS2000 without a front panel
	int  _noise_reduction_level;
	int  _nrval1;
	int  _nrval2;
	int  _currmode;

protected:
	string cmd; // command string
	string rsp; // expected response string (header etc)

	string to_bcd_be(long freq, int len);
	string to_bcd(long freq, int len);
	long fm_bcd (char *bcd, int len);
	long fm_bcd_be(char *bcd, int len);
	string to_binary_be(long freq, int len);
	string to_binary(long freq, int len);
	long fm_binary(char *binary, int len);
	long fm_binary_be(char *binary_be, int len);
	string to_decimal_be(long d, int len);
	string to_decimal(long d, int len);
	long fm_decimal(char *decimal, int len);
	long fm_decimal_be(char *decimal_be, int len);

public:
	rigbase();
	virtual ~rigbase(){}

	virtual void initialize() {}
	virtual void post_initialize() {}
	virtual void shutdown() {}

	virtual bool get_info(void) {return false;}
	virtual long get_vfoA(void) {return A.freq;}
	virtual void set_vfoA(long f) {A.freq = f;}
	virtual long get_vfoB(void) {return B.freq;}
	virtual void set_vfoB(long f) {B.freq = f; set_vfoA(f);}

	virtual void set_modeA(int val) {A.imode = val;}
	virtual int  get_modeA() {return A.imode;}
	virtual int  get_modetype(int n) {return 'U';}
	virtual void set_modeB(int val) {B.imode = val; set_modeA(val);}
	virtual int  get_modeB() {return B.imode;}
	virtual void selectA(){}
	virtual void selectB(){}
	virtual void A2B(){}

	virtual void swapAB(){}

	virtual void set_bwA(int val) {A.iBW = val;}
	virtual int  get_bwA() {return A.iBW;}
	virtual void set_bwB(int val) {B.iBW = val; set_bwA(val);}
	virtual int  get_bwB() {return B.iBW;}
	virtual int  adjust_bandwidth(int m) {return 0;}
	virtual int  def_bandwidth(int m) {return 0;}
	virtual const char **bwtable(int m) {return bandwidths_;}
	virtual const char **lotable(int m) {return dsp_lo;}
	virtual const char **hitable(int m) {return dsp_hi;}

	virtual bool can_split() { return false;}
	virtual void set_split(bool val) {split = val;}
	virtual int  get_split() {return split;}
	virtual bool twovfos() {return false;}
	virtual void swapvfos() {}
	virtual bool  canswap() { return false;}

	virtual int  get_smeter(void) {return -1;}
	virtual int  get_swr(void) {return -1;}
	virtual int  get_alc(void) {return -1;}
	virtual void select_swr() {}
	virtual void select_alc() {}
	virtual int  get_power_out(void) {return -1;}
	virtual int  get_power_control(void) {return 0;}
	virtual void set_power_control(double val) {}
	virtual void get_pc_min_max_step(double &min, double &max, double &step) {
		min = 0; max = 100; step = 1; }
	virtual int  get_volume_control() {return 0;}
	virtual void get_vol_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 100; step = 1; }
	virtual void set_volume_control(int val) {}
	virtual void set_PTT_control(int val) {}
	virtual void tune_rig() {}
	virtual void set_attenuator(int val) {}
	virtual int  get_attenuator() {return 0;}
	virtual void set_preamp(int val) {}
	virtual int  get_preamp() {return 0;}
	virtual void set_if_shift(int val) {}
	virtual bool get_if_shift(int &val) {val = 0; return false;}
	virtual void get_if_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 100; step = 1; }
	virtual void get_if_mid() {};
	virtual void set_notch(bool on, int val) {}
	virtual bool get_notch(int &val) {val = 0; return false;}
	virtual void get_notch_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 100; step = 1; }
	virtual void set_noise(bool on) {}
	virtual int  get_noise(){return 0;}
	virtual void set_noise_reduction_val(int val) {}
	virtual int  get_noise_reduction_val() {return 0;}
	virtual void get_nr_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 100; step = 1; }
	virtual void set_noise_reduction(int val) {}
	virtual int  get_noise_reduction() {return 0;}
	virtual void set_mic_gain(int val) {}
	virtual int  get_mic_gain() {return 0;}
	virtual void get_mic_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 100; step = 1; }
	virtual void set_mic_line(int val) {}
	virtual void set_squelch(int val) {}
	virtual int  get_squelch() {return 0;}
	virtual void get_squelch_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 100; step = 1; }
	virtual void set_rf_gain(int val) {}
	virtual int  get_rf_gain() {return 0;}
	virtual void get_rf_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 100; step = 1; }

	virtual const char * get_modename_(int n){
		if (modes_ == NULL) return "";
		int nmodes = 0;
		while (modes_[nmodes] != NULL) nmodes++;
		if (n < nmodes && n > -1)
			return modes_[n];
		else
			return modes_[0];
	}

	virtual const char * get_bwname_(int bw, int md) {
// read bw based on mode
		const char ** pbwt;
		pbwt = bwtable(md);
		if (pbwt == NULL) return "";
		int nbw = 0;
		while (pbwt[nbw] != NULL) nbw++;
		if (bw < nbw && bw > -1)
			return pbwt[bw];
		else
			return pbwt[0];
	}

	virtual void setRit(int v) {}
	virtual int  getRit() {return 0;}
	virtual void get_RIT_min_max_step(int &min, int &max, int &step) {
		min = -100; max = 100; step = 1; }

	virtual void setXit(int v) { XitFreq = v;}
	virtual int  getXit() {return XitFreq;}
	virtual void get_XIT_min_max_step(int &min, int &max, int &step) {
		min = -100; max = 100; step = 1; }

	virtual void setBfo(int v) {}
	virtual int  getBfo() {return 0;}
	virtual void get_BFO_min_max_step(int &min, int &max, int &step) {
		min = -100; max = 100; step = 1; }

	virtual void setVfoAdj(double v) {}
	virtual double getVfoAdj() {return 0;}
	virtual void get_vfoadj_min_max_step(int &min, int &max, int &step) {
		min = -100; max = 100; step = 1; }

	virtual void set_line_out() {}
	virtual void set_agc_level() {}
	virtual void set_cw_weight() {}
	virtual void get_cw_weight_min_max_step(double &min, double &max, double &step) {
		min = 2.5; max = 4.5; step = 0.1; } // default for FT950/FT450D
	virtual void set_cw_wpm() {}
	virtual void get_cw_wpm_min_max(int &min, int &max) {
		min = 5; max = 50; } // default for FT950/FT450D
	virtual void enable_keyer() {}
	virtual void enable_break_in() {}

	virtual void set_cw_qsk() {}
	virtual void get_cw_qsk_min_max_step(double &min, double &max, double &step) {
		min = 15; max = 30; step = 5; } // default for FT950/FT450D

	virtual void set_cw_delay() {}
	virtual void get_cw_delay_min_max_step(double &min, double &max, double &step) {
		min = 30; max = 3000; step = 10; } // default for FT950/FT450D

	virtual void set_cw_vol() {}
	virtual bool set_cw_spot() {return false;}
	virtual void set_cw_spot_tone() {}
	virtual void get_cw_spot_tone_min_max_step(int &min, int &max, int &step) {
		min = 300; max = 1050; step = 50; } // default for FT950/FT450D
	virtual void set_spot_onoff() {}
	virtual void set_vox_onoff() {}
	virtual void set_vox_gain() {}
	virtual void get_vox_gain_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 100; step = 1; } // default for FT950/FT450D
	virtual void set_vox_anti() {}
	virtual void get_vox_anti_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 100; step = 1; } // default for FT950/FT450D
	virtual void set_vox_hang() {}
	virtual void get_vox_hang_min_max_step(int &min, int &max, int &step) {
		min = 30; max = 3000; step = 10; } // default for FT950/FT450D
	virtual void set_vox_on_dataport() {}
	virtual void set_mon_vol() {}
	virtual void set_squelch_level() {}
	virtual void set_compression() {}
	virtual void get_comp_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 100; step = 1; }
	virtual void set_auto_notch(int v) {}
	virtual int  get_auto_notch() {return 0;}
	virtual void tuner_bypass() {}
	virtual void enable_xmtr() {}
	virtual void enable_tloop() {}
	virtual void set_nb_level() {}
	virtual void set_special(int v) {}
	virtual void set_band_selection(int v) {}
	virtual void set_xcvr_auto_on() {}
	virtual void set_xcvr_auto_off() {}

	virtual void adjustCIV(uchar adr) {}

	virtual void read_stream() {}

	virtual int  parse_vfoA(string){return 0;}
	virtual int  parse_vfoB(string){return 0;}
	virtual int  parse_modeA(string){return 0;}
	virtual int  parse_modeB(string){return 0;}
	virtual int  parse_bwA(string){return 0;}
	virtual int  parse_bwB(string){return 0;}
	virtual int  parse_volume_control(string){return 0;}
	virtual int  parse_power_control(string){return 0;}
	virtual int  parse_rf_gain(string){return 0;}
	virtual int  parse_mic_gain(string){return 0;}
	virtual int  parse_attenuator(string){return 0;}
	virtual int  parse_preamp(string){return 0;}
	virtual int  parse_smeter(string){return 0;}
	virtual int  parse_(string){return 0;}
	virtual int  parse_noise(string){return 0;}
	virtual int  parse_power_out(string){return 0;}
	virtual int  parse_swr(string){return 0;}
	virtual int  parse_alc(string){return 0;}
	virtual int  parse_split(string){return 0;}

	int waitN(size_t n, int timeout, const char *, int pr = HEX);
	int wait_char(int ch, size_t n, int timeout, const char *, int pr = HEX);

// more pia TS2000 without front panel!
	int currmode() { return _currmode; }
	int nrval1() { return _nrval1; }
	int nrval2() { return _nrval2; }
	int noise_reduction_level() { return _noise_reduction_level; }

};

extern rigbase *rigs[];
extern int rig_nbr;

extern const char *szNORIG;
extern const char *szNOMODES[];
extern const char *szNOBWS[];

#endif
