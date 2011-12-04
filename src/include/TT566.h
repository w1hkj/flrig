#ifndef _TT566_H
#define _TT566_H

#include "ICbase.h"

class RIG_TT566 : public rigbase {
private:
	int fwdpwr;
	int refpwr;
	int atten_level;

public:
	RIG_TT566();
	~RIG_TT566(){}

	void initialize();

	long get_vfoA();
	void set_vfoA(long);
	long get_vfoB();
	void set_vfoB(long);
	void set_modeA(int val);
	int  get_modeA();
	void set_modeB(int val);
	int  get_modeB();
	void set_bwA(int bw);
	int  get_bwA();
	void set_bwB(int bw);
	int  get_bwB();

	void set_PTT_control(int val);

	int  get_modetype(int n);
	void set_preamp(int);
	int  get_preamp();
	void set_attenuator(int val);
	int  get_attenuator();

	void set_noise(bool);
	int  get_noise();

	int  get_power_control(void);
	void set_power_control(double val);
	void set_auto_notch(int v);
	int  get_auto_notch();

	int  get_smeter();
	int  get_power_out();
	int  get_swr();
	int  get_volume_control();
	void get_vol_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 100; step = 1; }
	void set_volume_control(int val);

	void set_mic_gain(int val);
	int  get_mic_gain();

//	void set_if_shift(int val);
//	bool get_if_shift(int &val);
//	void get_if_min_max_step(int &min, int &max, int &step) {
//		min = -8000; max = 8000; step = 50; }

	void set_rf_gain(int val);
	int  get_rf_gain();
	void get_rf_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 100; step = 1; }

	int  adjust_bandwidth(int m);
	int  def_bandwidth(int m) {return adjust_bandwidth(m);}

//	void set_agc_level();
};

#endif
