#ifndef _IC756PRO2_H
#define _IC756PRO2_H

#include "ICbase.h"

class RIG_IC756PRO2 : public RIG_ICOM {
public:
	RIG_IC756PRO2();
	~RIG_IC756PRO2(){}

	void selectA();
	void selectB();

	long get_vfoA(void);
	void set_vfoA(long f);
	int  get_modetype(int n);
	void set_modeA(int val);
	int  get_modeA();
	void set_bwA(int val);
	int  get_bwA();

	long get_vfoB(void);
	void set_vfoB(long f);
	void set_modeB(int val);
	int  get_modeB();
	void set_bwB(int val);
	int  get_bwB();

	int  get_smeter();
	void set_volume_control(int val);
	int  get_volume_control();
	void get_vol_min_max_step(int &min, int &max, int &step);
	void set_PTT_control(int val);
	void set_noise(bool val);
	int  get_noise();
	void set_noise_reduction(int val);
	int  get_noise_reduction();
	void set_noise_reduction_val(int val);
	int  get_noise_reduction_val();

	void set_mic_gain(int val);
//	int  get_mic_gain();
	void get_mic_gain_min_max_step(int &min, int &max, int &step);
	void set_if_shift(int val);
//	bool get_if_shift(int val);
	void get_if_min_max_step(int &min, int &max, int &step);
	void set_squelch(int val);
//	int  get_squelch();
	void set_rf_gain(int val);
//	int  get_rf_gain();
	void set_power_control(double val);

	void set_split(bool val);
	bool can_split() { return true;}

	void set_attenuator(int val);
	int  get_attenuator();
	void set_preamp(int val);
	int  get_preamp();

	int  adjust_bandwidth(int);
	int  def_bandwidth(int);
	int  get_swr();
	int  get_alc();
	int  get_power_out();
	void tune_rig();
	void set_notch(bool, int);
	bool get_notch(int&);
	void get_notch_min_max_step(int&, int&, int&);

	bool twovfos() { return false; }

	const char **bwtable(int);

protected:
	int  preamp_level;
	int  atten_level;
	int  ICvol;

};


#endif
