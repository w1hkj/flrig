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
	int imode;
	int iBW;
};

enum {onA, onB};

enum {
	NONE,
	FT100D, FT450, FT767, FT817, FT857D, // 5
	FT897D, FT950, FT1000MP, FT2000,
	
	IC703,								// 10
	IC706MKIIG, IC718, IC728, IC735, IC746, // 15
	IC746PRO, IC756PRO2, IC756PRO3, IC7000, IC7200, // 20
	IC7600, IC7700, IC910H,
	
	K2, K3, // 25
	
	TS140, TS450S, TS480HX, TS480SAT, TS570, // 30
	TS2000,
	 
	TT516, TT535, TT538, TT550, TT563, // 36
	TT588, LAST_RIG
};

class rigbase {
public:
	const char * name_;
	const char ** modes_;
	const char ** bandwidths_;
	const int  * bw_vals_;
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


	int  def_mode;
	int  defbw_;
	long deffreq_;
	int  max_power;
	int  last_bw;
	int  bpf_center;
	int  pbt;

	bool PbtActive;
	int RitFreq;
	bool RitActive;
	int XitFreq;
	bool XitActive;
	bool split;

	bool has_vfo_adj;
	bool has_rit;
	bool has_xit;
	bool has_bfo;
	bool has_power_control;
	bool has_volume_control;
	bool has_mode_control;
	bool has_bandwidth_control;
	bool has_micgain_control;
	bool has_mic_line_control;
	bool has_notch_control;
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

	bool has_agc_level;
	bool has_cw_wpm;
	bool has_cw_vol;
	bool has_cw_spot;
	bool has_vox_onoff;
	bool has_vox_gain;
	bool has_vox_anti;
	bool has_vox_hang;
	bool has_compression;
	bool has_compON;
	bool has_auto_notch;
	bool use_line_in;
	bool has_bpf_center;
	bool has_special;

	bool restore_mbw;


protected:
	string cmd;

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
	virtual void shutdown() {}

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

	virtual void set_bwA(int val) {A.iBW = val;}
	virtual int  get_bwA() {return A.iBW;}
	virtual void set_bwB(int val) {B.iBW = val; set_bwA(val);}
	virtual int  get_bwB() {return B.iBW;}
	virtual int  adjust_bandwidth(int m) {return -1;}

	virtual bool can_split() { return false;}
	virtual void set_split(bool val) {split = val;}
	virtual bool twovfos() {return false;}

	virtual int  get_smeter(void) {return -1;}
	virtual int  get_swr(void) {return 0;}
	virtual int  get_alc(void) {return 0;}
	virtual int  get_power_out(void) {return -1;}
	virtual int  get_power_control(void) {return 0;}
	virtual void set_power_control(double val) {}
	virtual void get_pc_min_max_step(int &min, int &max, int &step) {
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
	virtual void set_notch(bool on, int val) {}
	virtual bool get_notch(int &val) {val = 0; return false;}
	virtual void get_notch_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 100; step = 1; }
	virtual void set_noise(bool on) {}
	virtual int  get_noise(){return 0;}
	virtual void set_noise_reduction_val(int val) {}
	virtual int  get_noise_reduction_val() {return 0;}
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
			return modes_[0];}

	virtual void setRit(int v) {}
	virtual int  getRit() {return 0;}
	virtual void setXit(int v) { XitFreq = v;}
	virtual int  getXit() {return XitFreq;}
	virtual void setBfo(int v) {}
	virtual int  getBfo() {return 0;}
	virtual void setVfoAdj(double v) {}

	virtual void set_line_out() {}
	virtual void set_agc_level() {}
	virtual void set_cw_weight() {}
	virtual void set_cw_wpm() {}
	virtual void enable_keyer() {}
	virtual void set_cw_qsk() {}
	virtual void set_cw_vol() {}
	virtual void set_cw_spot() {}
	virtual void set_spot_onoff() {}
	virtual void set_vox_onoff() {}
	virtual void set_vox_gain() {}
	virtual void set_vox_anti() {}
	virtual void set_vox_hang() {}
	virtual void set_mon_vol() {}
	virtual void set_squelch_level() {}
	virtual void set_compression() {}
	virtual void set_auto_notch(int v) {}
	virtual int  get_auto_notch() {return 0;}
	virtual void tuner_bypass() {}
	virtual void enable_xmtr() {}
	virtual void enable_tloop() {}
	virtual void set_nb_level() {}
	virtual void set_special(int v) {}

	virtual void adjustCIV(uchar adr) {}

	virtual void read_stream() {}
};

extern rigbase *rigs[];
extern int rig_nbr;

extern const char *szNORIG;
extern const char *szNOMODES[];
extern const char *szNOBWS[];

#endif
