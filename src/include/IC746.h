#ifndef _IC746_H
#define _IC746_H

#include "ICbase.h"

class RIG_IC746 : public RIG_ICOM {
public:
	RIG_IC746();
	virtual ~RIG_IC746(){}

	virtual void selectA();
	virtual void selectB();

	virtual long get_vfoA(void);
	virtual void set_vfoA(long f);
	virtual long get_vfoB(void);
	virtual void set_vfoB(long f);

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

	virtual int  get_modetype(int n);
	virtual void set_modeA(int val);
	virtual int  get_modeA();
	virtual void set_bwA(int val);
	virtual int  get_bwA();
	virtual void set_modeB(int val);
	virtual int  get_modeB();
	virtual void set_bwB(int val);
	virtual int  get_bwB();

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

	void set_split(bool val);
	bool can_split() { return true;}

protected:
	int  preamp_level;
	int  atten_level;
	int  ICvol;
	int  filter_nbr;

};

class RIG_IC746PRO : public RIG_IC746 {
public:
	RIG_IC746PRO();
	virtual ~RIG_IC746PRO(){}
	virtual void set_modeA(int val);
	virtual int  get_modeA();

	virtual void set_bwA(int val);
	virtual int  get_bwA();
	virtual void set_modeB(int val);
	virtual int  get_modeB();
	virtual void set_bwB(int val);
	virtual int  get_bwB();

	int  get_swr();
	int  get_alc();
	int  get_power_out();
	virtual void set_attenuator(int val);
	virtual int  get_attenuator();
	void tune_rig();
	virtual int  adjust_bandwidth(int m);
	virtual int  def_bandwidth(int m);
	void set_notch(bool on, int val);
	bool get_notch(int &val);
	void get_notch_min_max_step(int &min, int &max, int &step);

	bool twovfos() { return false; }
	const char **bwtable(int);

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
