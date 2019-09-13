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
#include "trace.h"
#include "rig_io.h"

#include "rigpanel.h"

using namespace std;

enum {onA, onB};
enum {UI, XML, SRVR, RIG};
enum {BINARY, STRING};

struct XCVR_STATE {
	long freq;
	int  imode;
	int  iBW;
	int  filter;
	int  src;

	int		split;
	int		power_control;
	int		volume_control;
	int		attenuator;
	int		preamp;
	int		if_shift;
	int		shift_val;
	int		auto_notch;
	int		notch;
	int		notch_val;
	int		noise;
	int		nb_level;
	int		nr;
	int		nr_val;
	int		mic_gain;
	int		rf_gain;
	int		agc_level;
	int		squelch;
	int		compression;
	int		compON;

	XCVR_STATE() {
		freq = 0;
		imode = 0;
		iBW = 0;
		filter = 0;
		src = UI;

		split = 0;
		power_control = 0;
		volume_control = 0;
		attenuator = 0;
		preamp = 0;
		if_shift = 0;
		shift_val = 0;
		auto_notch = 0;
		notch = 0;
		notch_val = 0;
		noise = 0;
		nb_level = 0;
		nr = 0;
		nr_val = 0;
		mic_gain = 0;
		rf_gain = 0;
		agc_level = 0;
		squelch = 0;
		compression = 0;
		compON = 0;
	}
	XCVR_STATE(int a, int b, int c, int d) {
		XCVR_STATE();
		freq = a;
		imode = b;
		iBW = c;
		src = d;
	}
//	bool equals(XCVR_STATE alt) {
//		if (freq != alt.freq) return false;
//		if (iBW != alt.iBW) return false;
//		if (imode != alt.imode) return false;
//		return true;
//	}
};

enum {
	vA,   // apply to vfoA
	vB,   // apply to vfoB
	vX,   // apply to current vfo
	sA,  // select vfoA
	sB,  // select vfoB
	ON,  // ptt ON
	OFF, // ptt OFF
	sON, // split ON
	sOFF, // split OFF
	A2B,  // active to inactive vfo
	SWAP  // swap vfo's
};

struct VFOQUEUE {
	int change; // A B AA BB ON OFF
	XCVR_STATE vfo;
	VFOQUEUE() {}
	VFOQUEUE(int c, XCVR_STATE v) { change = c; vfo = v; }
};

struct GUI {
	Fl_Widget*	W;
	int			x;
	int			y;
	int			w;
};

class rigbase {
#define WVALS_LIMIT -1
public:
	string name_;
	const char ** modes_;
	const char ** bandwidths_;
	const char ** dsp_SL;
	const char *  SL_tooltip;
	const char *  SL_label;
	const char ** dsp_SH;
	const char *  SH_tooltip;
	const char *  SH_label;
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

	XCVR_STATE A;
	XCVR_STATE B;
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

	int  atten_level;
	int  preamp_level;

	int  rTONE;  // index into szTONES, PL_tones arrar of receive PL tones
	int  tTONE;

	bool has_xcvr_auto_on_off;
	bool has_a2b;
	bool has_b2a;
	bool has_vfo_adj;
	bool has_rit;
	bool has_xit;
	bool has_bfo;
	bool has_power_control;
	bool has_volume_control;
	bool has_mode_control;
	bool has_bandwidth_control;
	bool has_dsp_controls;
	bool has_FILTER;
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
	bool has_pbt_controls;
	bool has_ptt_control;
	bool has_tune_control;
	bool has_swr_control;
	bool has_alc_control;
	bool has_agc_control;
	bool has_rf_control;
	bool has_sql_control;
	bool has_smeter;
	bool has_power_out;
	bool has_line_out;
	bool has_split;
	bool has_split_AB;
	bool has_data_port;
	bool has_getvfoAorB;

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

	bool has_vfoAB;

	bool restore_mbw;

	bool has_get_info;

// Icom Xcvr 
	bool ICOMrig;
	bool ICOMmainsub;
	string pre_to;
	string pre_fm;
	string post;
	string ok;
	string bad;

	int filA;
	int filB;

	int  data_type;

// for pia TS2000 without a front panel
	int  _noise_reduction_level;
	int  _nrval1;
	int  _nrval2;
	int  _currmode;

	std::string replystr;

protected:
	string cmd; // command string
	string rsp; // expected response string (header etc)

	string to_bcd_be(long freq, int len);
	string to_bcd(long freq, int len);
	long fm_bcd (string bcd, int len);
	long fm_bcd_be(string bcd, int len);
	string to_binary_be(long freq, int len);
	string to_binary(long freq, int len);
	long fm_binary(string binary, int len);
	long fm_binary_be(string binary_be, int len);
	string to_decimal_be(long d, int len);
	string to_decimal(long d, int len);
	long fm_decimal(string decimal, int len);
	long fm_decimal_be(string decimal_be, int len);

public:
	rigbase();
	virtual ~rigbase(){}

	virtual void initialize() {}
	virtual void post_initialize() {}
	virtual void shutdown() {}

	virtual bool check() { return false; }

	virtual void set_data_port() {}

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
	virtual void selectA(){ inuse = onA;}
	virtual void selectB(){ inuse = onB;}
	virtual void A2B(){}
	virtual void B2A(){}
	virtual int  get_vfoAorB(){return inuse;}
	virtual bool isOnA(){return inuse == onA;}
	virtual bool isOnB(){return inuse == onB;}

	virtual void swapAB(){inuse = isOnA() ? onB : onA;}

	virtual void set_bwA(int val) {A.iBW = val;}
	virtual int  get_bwA() {return A.iBW;}
	virtual void set_bwB(int val) {B.iBW = val; set_bwA(val);}
	virtual int  get_bwB() {return B.iBW;}
	virtual int  adjust_bandwidth(int m) {return 0;}
	virtual int  def_bandwidth(int m) {return 0;}
	virtual const char **bwtable(int m) {return bandwidths_;}
	virtual const char **lotable(int m) {return NULL;}
	virtual const char **hitable(int m) {return NULL;}

	virtual const char *FILT(int val) { return "1"; }
	virtual const char *nextFILT() { return "1";}

	virtual int  get_FILT(int mode) { return 1; }
	virtual void set_FILT(int filter) {}

	virtual void set_BANDWIDTHS(std::string s) {}
	virtual std::string get_BANDWIDTHS() { return ""; }

	virtual void set_FILTERS(std::string s) {}
	virtual std::string get_FILTERS() { return ""; }

	virtual bool can_split() { return false;}
	virtual void set_split(bool val);
	virtual int  get_split();

	virtual bool twovfos() {return false;}

	virtual bool canswap() { return false; }

	virtual int  get_agc() { return 0; }
	virtual int  incr_agc() { return 0;}
	virtual const char *agc_label() { return "";}
	virtual int  agc_val() { return 0; }

	virtual int  get_smeter(void) {return -1;}
	virtual int  get_swr(void) {return -1;}
	virtual int  get_alc(void) {return -1;}
	virtual void select_swr() {}
	virtual void select_alc() {}
	virtual int  get_power_out(void) {return -1;}

int po_, po_val;
	virtual int  get_power_control(void) {return po_;}
	virtual void set_power_control(double val) {po_ = val;}
	virtual void get_pc_min_max_step(double &min, double &max, double &step) {
		min = 0; max = 100; step = 1; }

int vl_;
	virtual void set_volume_control(int val) {vl_ = val;}
	virtual int  get_volume_control() {return vl_;}
	virtual void get_vol_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 100; step = 1; }

int ptt_;
int tune_;
	virtual void set_PTT_control(int val) {ptt_ = val;}
	virtual int  get_PTT() {return ptt_;}
	virtual void tune_rig(int how) {}
	virtual int  get_tune() { return tune_; }

	virtual void set_attenuator(int val) {atten_level = val;}
	virtual int  next_attenuator() { atten_level = !atten_level; return atten_level; }
	virtual int  get_attenuator() {return atten_level;}

	virtual void set_preamp(int val) {preamp_level = val;}
	virtual int  next_preamp() { preamp_level = !preamp_level; return preamp_level; }
	virtual int  get_preamp() {return preamp_level;}

// CTCSS tones / offset
	virtual void set_tones(int tx_tone, int rx_tone) { return; }
	virtual void set_offset(int indx, int offset) { return; }

int sh_;
bool sh_on_;
	virtual void set_if_shift(int val) {sh_ = val;}
	virtual void set_if_shift_on(int on) { sh_on_ = on; }
	virtual bool get_if_shift(int &val) {val = sh_; return sh_on_;}
	virtual void get_if_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 100; step = 1; }
	virtual void get_if_mid() {};

int no_, noval_;
	virtual void set_notch(bool on, int val) { no_ = on; noval_ = val;}
	virtual bool get_notch(int &val) {val = noval_; return no_;}
	virtual void get_notch_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 100; step = 1; }

int nb_, nbval_;
	virtual void set_noise(bool on) {nb_ = on;}
	virtual int  get_noise(){return nb_;}
	virtual void set_nb_level(int val) {nbval_ = val;}
	virtual int  get_nb_level() { return nbval_; }
	virtual void get_nb_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 100; step = 1; }

int nr_, nrval_;
	virtual void set_noise_reduction(int val) {nr_ = val;}
	virtual int  get_noise_reduction() {return nr_;}
	virtual void set_noise_reduction_val(int val) {nrval_ = val;}
	virtual int  get_noise_reduction_val() {return nrval_;}
	virtual void get_nr_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 100; step = 1; }

	virtual void set_pbt_inner(int val) {}
	virtual void set_pbt_outer(int val) {}
	virtual int  get_pbt_inner() {return 0;}
	virtual int  get_pbt_outer() {return 0;}

int mcval_;
	virtual void set_mic_gain(int val) {mcval_ = val;}
	virtual int  get_mic_gain() {return mcval_;}
	virtual void get_mic_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 100; step = 1; }
	virtual void set_mic_line(int val) {}

int sqval_;
	virtual void set_squelch(int val) {sqval_ = val;}
	virtual int  get_squelch() {return sqval_;}
	virtual void get_squelch_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 100; step = 1; }

int rfg_;
	virtual void set_rf_gain(int val) {rfg_ = val;}
	virtual int  get_rf_gain() {return rfg_;}
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

int ritval_;
	virtual void setRit(int v) {ritval_ = v;}
	virtual int  getRit() {return ritval_;}
	virtual void get_RIT_min_max_step(int &min, int &max, int &step) {
		min = -100; max = 100; step = 1; }

	virtual void setXit(int v) { XitFreq = v;}
	virtual int  getXit() {return XitFreq;}
	virtual void get_XIT_min_max_step(int &min, int &max, int &step) {
		min = -100; max = 100; step = 1; }

int bfo_;
	virtual void setBfo(int v) {bfo_ = v;}
	virtual int  getBfo() {return bfo_;}
	virtual void get_BFO_min_max_step(int &min, int &max, int &step) {
		min = -100; max = 100; step = 1; }

int vfo_;
	virtual void setVfoAdj(double v) {vfo_ = v;}
	virtual double getVfoAdj() {return vfo_;}
	virtual void get_vfoadj_min_max_step(int &min, int &max, int &step) {
		min = -100; max = 100; step = 1; }

	virtual void set_line_out() {}
	virtual int  get_line_out() { return 100; }
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
	virtual void set_compression(int, int) {}
	virtual void get_compression(int &on, int &val) { on = 0; val = 0; }
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
	virtual void get_band_selection(int v) {}
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

// IC-7610
	virtual void set_digi_sel(bool) {}
	virtual void set_digi_val(int) {}
	virtual int  get_digi_sel() {return 0;}
	virtual int  get_digi_val() {return 0;}
	virtual void set_dual_watch(bool) {}
	virtual int  get_dual_watch() {return 0;}
	virtual void set_index_att(int) {}

// more pia TS2000 without front panel!
	int currmode() { return _currmode; }
	int nrval1() { return _nrval1; }
	int nrval2() { return _nrval2; }
	int noise_reduction_level() { return _noise_reduction_level; }

// TS990 monitor usurp'd use of IF shift control
	virtual void set_monitor(bool) {}

// TT-550 / at-11 internal tuner i/o commands
	virtual void at11_bypass() {}
	virtual void at11_autotune() {}
	virtual void at11_cap_up() {}
	virtual void at11_cap_dn() {}
	virtual void at11_ind_up() {}
	virtual void at11_ind_dn() {}
	virtual void at11_loZ() {}
	virtual void at11_hiZ() {}

	string bcd255(int val);
	int    num100(string bcd);

// Yaesu force ID response before sending command
	std::string IDstr;
	bool id_OK(std::string ID, int wait);
	void sendOK(std::string cmd);

// KX3 power scale
	bool power10x;
	virtual bool power_10x() { return false; }
};

extern rigbase *rigs[];

extern const char *szNORIG;
extern const char *szNOMODES[];
extern const char *szNOBWS[];

extern char bcdval[];

#endif
