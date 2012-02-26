#ifndef _TT588_H
#define _TT588_H

#include "rigbase.h"

class RIG_TT588 : public rigbase {
private:
	double fwdpwr;
	double refpwr;
	double fwdv;
	double refv;
	double VfoAdj;
	int    vfo_corr;
	int    atten_level;
	int    an_;
	int    nb_;

public:
	RIG_TT588();
	~RIG_TT588(){}

	void initialize();

	long get_vfoA();
	void set_vfoA(long);
	long get_vfoB();
	void set_vfoB(long);
	void setVfoAdj(double v);

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

	int  get_smeter();

	void set_volume_control(int val);
	int  get_volume_control();
	void set_attenuator(int val);
	int  get_attenuator();
	void set_rf_gain(int val);
	int  get_rf_gain();
	void set_if_shift(int val);
	bool get_if_shift(int &val);
	void get_if_min_max_step(int &min, int &max, int &step);

	void set_squelch(int val);
	int  get_squelch();
	void get_squelch_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 100; step = 1; }

//	void set_auto_notch(int);
//	int  get_auto_notch();

	void set_noise(bool);
	int  get_noise();

	void set_split(bool val);
	int  get_split();

	int  get_swr();
	int  get_power_out();

//	int  get_power_control(void);
//	void set_power_control(double val);
//	void get_pc_min_max_step(double &min, double &max, double &step) {
//		min = 0; max = 100; step = 1; }

	void set_PTT_control(int val);

//	void tune_rig();
//	void set_preamp(int val);
//	int  get_preamp();
//	void set_notch(bool on, int val);
//	bool get_notch(int &val);
//	void get_notch_min_max_step(int &min, int &max, int &step);
//	void set_mic_gain(int val);
//	int  get_mic_gain();
//	void get_mic_min_max_step(int &min, int &max, int &step);
	void checkresponse(string s);
	void shutdown();

	int  adjust_bandwidth(int m);
	int  def_bandwidth(int m);

};


#endif
