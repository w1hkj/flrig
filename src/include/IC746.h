#ifndef _IC746_H
#define _IC746_H

#include "ICbase.h"

class RIG_IC746 : public RIG_ICOM {
public:
	RIG_IC746();
	virtual ~RIG_IC746(){}

	long get_vfoA(void);
	void set_vfoA(long);
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
	virtual void set_attenuator(int val);
	virtual int  get_attenuator();
	virtual void set_preamp(int val);
	virtual int  get_preamp();
	virtual void set_mode(int val);
	virtual int  get_mode();
	virtual int  get_modetype(int n);
	virtual void set_bandwidth(int val);
	virtual int  get_bandwidth();
	virtual void set_mic_gain(int val);
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
protected:
	int  preamp_level;
	int  atten_level;
	int  filter_nbr;
	int  ICvol;

};

class RIG_IC746PRO : public RIG_IC746 {
public:
	RIG_IC746PRO();
	virtual ~RIG_IC746PRO(){}
	virtual void set_mode(int val);
	virtual int  get_mode();
	virtual void set_bandwidth(int val);
	virtual int  get_bandwidth();
	int  get_swr();
	int  get_alc();
	int  get_power_out();
	virtual void set_attenuator(int val);
	virtual int  get_attenuator();
	void tune_rig();
	virtual int  adjust_bandwidth(int m);
	void set_notch(bool on, int val);
	bool get_notch(int &val);
	void get_notch_min_max_step(int &min, int &max, int &step);
};

extern const char *IC746modes_[];
extern const char IC746_mode_type[];
extern const char *IC746_widths[];
extern const char *IC746PROmodes_[];
extern const char IC746PRO_mode_type[];
extern const char *IC746PRO_SSBwidths[];
extern const char *IC746PRO_RTTYwidths[];
extern const char *IC746PRO_AMFMwidths[];


#endif
