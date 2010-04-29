#ifndef _IC718_H
#define _IC718_H

#include "ICbase.h"

class RIG_IC718 : public RIG_ICOM {
public:
	RIG_IC718();
	~RIG_IC718(){}

	long get_vfoA ();
	void set_vfoA (long freq);
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
	void set_mode(int val);
	int  get_mode();
	int  get_modetype(int n);
	void set_bandwidth(int val);
	int  get_bandwidth();
	void set_compression();
	void set_auto_notch(int val);
	void set_vox_onoff();

protected:
	int  filter_nbr;
};

#endif
//=============================================================================


