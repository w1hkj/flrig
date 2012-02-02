#ifndef _FT450_H
#define _FT450_H

#include "rigbase.h"

class RIG_FT450 : public rigbase {
private:
	bool notch_on;
	int  m_60m_indx;
public:
	RIG_FT450();
	~RIG_FT450(){}
	
	void initialize();

	long get_vfoA();
	void set_vfoA(long);
	long get_vfoB();
	void set_vfoB(long);

	void selectA();
	void selectB();
	void A2B();
	void set_modeA(int val);
	int  get_modeA();
	int  get_modetype(int n);
	void set_bwA(int val);
	int  get_bwA();

	void set_modeB(int val);
	int  get_modeB();
	void set_bwB(int val);
	int  get_bwB();

	bool twovfos() {return true;}
	bool can_split() {return true;}
	void set_split(bool);
	bool get_split();

	int  get_smeter();
	int  get_swr();
	int  get_power_out();
	int  get_power_control();
	void get_pc_min_max_step(double &min, double &max, double &step) {
		min = 5; max = 100; step = 1; }
	void set_volume_control(int val);
	int  get_volume_control();
	void get_vol_min_max_step(int &min, int &max, int &step);
	void set_power_control(double val);
	void set_PTT_control(int val);
	void tune_rig();
	void set_attenuator(int val);
	int  get_attenuator();
	void set_preamp(int val);
	int  get_preamp();
	void set_if_shift(int val);
	bool get_if_shift(int &val);
	void get_if_min_max_step(int &min, int &max, int &step);
	void set_notch(bool on, int val);
	bool get_notch(int &val);
	void get_notch_min_max_step(int &min, int &max, int &step);
	void set_noise(bool b);
	void set_mic_gain(int val);
	int  get_mic_gain();
	void get_mic_min_max_step(int &min, int &max, int &step);
	
	void set_special(int v);

	int  adjust_bandwidth(int m);

	void set_vox_onoff();
	void set_vox_gain();
	void get_vox_gain_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 255; step = 1; }
	void set_vox_hang();
	void get_vox_hang_min_max_step(int &min, int &max, int &step) {
		min = 100; max = 3000; step = 100; }

	void get_cw_wpm_min_max(int &min, int &max) {
		min = 4; max = 60; }

	void set_cw_weight();
	void set_cw_wpm();
	void enable_keyer();
	void set_cw_qsk();
	bool set_cw_spot();
	void set_cw_spot_tone();
	void get_cw_spot_tone_min_max_step(int &min, int &max, int &step) {
		min = 400; max = 800; step = 100; }

	void set_band_selection(int v);

	void get_nr_min_max_step(int &min, int &max, int &step) {
		min = 1; max = 11; step = 1; }

	void set_noise_reduction_val(int val);
	int  get_noise_reduction_val();
	void set_noise_reduction(int val);
	int  get_noise_reduction();

	void set_xcvr_auto_on();
	void set_xcvr_auto_off();

};


#endif
