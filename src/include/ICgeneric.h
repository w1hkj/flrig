#ifndef _ICgeneric_H
#define _ICgeneric_H

#include "rigbase.h"

class RIG_ICOM : public rigbase {
protected:
	string pre_to;
	string pre_fm;
	string post;
	string ok;
	string bad;
	const char *_mode_type;
public:
	RIG_ICOM() {
		pre_to = "\xFE\xFE\x56\xE0";
		pre_fm = "\xFE\xFE\xE0\x56";
		post = "\xFD";
		ok = "\xFE\xFE\xE0\x56\xFB\xFD";
		bad = "\xFE\xFE\xE0\x56\xFA\xFD";
	}
	virtual ~RIG_ICOM() {}
	void checkresponse(int n);
	bool sendICcommand(string str, int nbr);

};

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
protected:
	int  preamp_level;
	int  atten_level;
	int  filter_nbr;

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

class RIG_IC756PRO2 : public RIG_IC746PRO {
public:
	RIG_IC756PRO2();
	virtual ~RIG_IC756PRO2(){}
	void set_attenuator(int val);
	int  get_attenuator();
	void set_preamp(int val);
	int  get_preamp();
};

class RIG_IC756PRO3 : public RIG_IC756PRO2 {
public:
	RIG_IC756PRO3();
	~RIG_IC756PRO3(){}
	void set_mode(int val);
	int  get_mode();
};

class RIG_IC7000 : public RIG_IC746PRO {
public:
	RIG_IC7000();
	~RIG_IC7000(){}
	void set_mode(int val);
	int  get_mode();
	int  adjust_bandwidth(int m);
	void set_attenuator( int val );
	int  get_attenuator();
	void set_preamp(int val);
	int  get_preamp();
};

class RIG_IC7200 : public RIG_IC746PRO {
public:
	RIG_IC7200();
	~RIG_IC7200(){}
};

class RIG_IC7600 : public RIG_IC746PRO {
public:
	RIG_IC7600();
	~RIG_IC7600(){}
	void set_mode(int val);
	int  get_mode();

	void set_attenuator( int val );	//alh
	int  get_attenuator();		//alh

};

class RIG_IC7700 : public RIG_IC746PRO {
public:
	RIG_IC7700();
	~RIG_IC7700(){}
};

class RIG_IC910H : public RIG_IC746 {
public:
	RIG_IC910H();
	~RIG_IC910H(){};
	void set_vfoA(long);
	void set_compression();
	void set_vox_onoff();
	void set_vox_gain();
	void set_vox_anti();
	void set_vox_hang();
private:
	bool comp_is_on;
};


#endif
