#ifndef _TS590S_H
#define _TS590S_H

#include "rigbase.h"

class RIG_TS590S : public rigbase {

enum TS590MODES { LSB, USB, CW, FM, AM, FSK, CWR, FSKR, LSBD, USBD, FMD };

private:
	bool notch_on;
	int  preamp_level;
	int  att_level;
	int  nb_level;
	bool data_mode;
	int  active_mode;
	int  active_bandwidth;
	bool rxtxa;
public:
	RIG_TS590S();
	~RIG_TS590S(){}
	
	void initialize();

	long get_vfoA();
	void set_vfoA(long);
	long get_vfoB();
	void set_vfoB(long);

	int  get_modetype(int n);
	void set_modeA(int val);
	int  get_modeA();
	void set_modeB(int val);
	int  get_modeB();

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

	bool can_split() { return true;}
	void set_split(bool val);
	int  get_split();
	bool twovfos() {return true;}

	int  get_smeter();
	int  get_swr();
	int  get_alc();
	int  get_power_out();
	int  get_power_control();
	void set_volume_control(int val);
	int  get_volume_control();
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

	const char **bwtable(int);
	const char **lotable(int);
	const char **hitable(int);

	const char * get_bwname_(int bw, int md);

};


#endif
