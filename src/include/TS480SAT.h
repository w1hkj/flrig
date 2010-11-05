#ifndef _TS480SAT_H
#define _TS480SAT_H

#include "rigbase.h"

class RIG_TS480SAT : public rigbase {
private:
	bool beatcancel_on;
	bool preamp_on;
	bool att_on;
	const char *_mode_type;
public:
	RIG_TS480SAT();
	~RIG_TS480SAT(){}

	bool sendTScommand(string, int, bool);

	long get_vfoA();
	void set_vfoA(long);
	int  get_smeter();
	int  get_swr();
	void set_PTT_control(int val);
	void set_modeA(int val);
	int  get_modeA();
	int  get_modetype(int n);
	void set_bwA(int val);
	int  get_bwA();

//	int  get_power_out();
	void set_volume_control(int val);
	int  get_volume_control();
	void set_power_control(double val);
	int  get_power_control();
	void get_pc_min_max_step(int &min, int &max, int &step) {
		min = 5; max = 100; step = 1; }
	void set_attenuator(int val);
	int  get_attenuator();

//	void tune_rig();
//	void set_preamp(int val);
//	int  get_preamp();
//	int  adjust_bandwidth(int val);
//	void set_widths();
	//	void set_if_shift(int val);
	//	bool get_if_shift(int &val);
	//	void get_if_min_max_step(int &min, int &max, int &step);
	//	void set_notch(bool on, int val);
	//	bool get_notch(int &val);
	//	void get_notch_min_max_step(int &min, int &max, int &step);
//	void set_beatcancel(int val);
//	int  get_beatcancel();
//	void set_mic_gain(int val);
//	int  get_mic_gain();
//	void get_mic_min_max_step(int &min, int &max, int &step);

};


#endif
