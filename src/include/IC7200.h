#ifndef _IC7200_H
#define _IC7200_H

#include "ICbase.h"

class RIG_IC7200 : public RIG_ICOM {
public:
	RIG_IC7200();
	~RIG_IC7200(){}

	void selectA();
	void selectB();

	long get_vfoA ();
	void set_vfoA (long freq);

	long get_vfoB(void);
	void set_vfoB(long f);

	int  get_modetype(int n);
	void set_modeA(int val);
	int  get_modeA();
	void set_bwA(int val);
	int  get_bwA();

	void set_modeB(int val);
	int  get_modeB();
	void set_bwB(int val);
	int  get_bwB();

	void set_PTT_control(int val);
	void set_volume_control(int val);
	int  get_volume_control();
	void get_vol_min_max_step(int &min, int &max, int &step);
	void set_attenuator(int val);
	int  get_smeter();
	void set_noise(bool val);
	int  get_noise();
	void set_noise_reduction(int val);
	int  get_noise_reduction();
	void set_noise_reduction_val(int val);
	int  get_noise_reduction_val();
	void set_preamp(int val);
	int  get_preamp();
	void set_rf_gain(int val);
	int  get_rf_gain();
	void set_squelch(int val);
	void set_power_control(double val);
	void set_mic_gain(int val);
	int  adjust_bandwidth(int m);
	int  def_bandwidth(int m);
	void set_compression();
	void set_auto_notch(int val);
	int  get_auto_notch();
	void set_vox_onoff();
	void set_split(bool);
	bool can_split() { return true; }
	const char **bwtable(int);

protected:
	int  filter_nbr;
	void showresp(string);
};

#endif
