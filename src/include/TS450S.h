#ifndef _TS450S_H
#define _TS450S_H

#include "rigbase.h"

class RIG_TS450S : public rigbase {
private:
	bool beatcancel_on;
	bool preamp_on;
	bool att_on;
	const char *_mode_type;
public:
	RIG_TS450S();
	~RIG_TS450S(){}

	void initialize();

	bool sendTScommand(string, int, bool);

	long get_vfoA();
	void set_vfoA(long);
	int  get_modeA();
	void set_modeA(int val);
	void set_bwA(int val);
	int  get_bwA();

	long get_vfoB();
	void set_vfoB(long);
	int  get_modeB();
	void set_modeB(int val);
	void set_bwB(int val);
	int  get_bwB();

	int  get_modetype(int n);

	int  get_smeter();
	int  get_swr();
	void set_PTT_control(int val);

	int  adjust_bandwidth(int val) { return 0; }

	bool can_split();
	void set_split(bool val);
	bool get_split();
	bool twovfos() {return true;}

//	int  get_power_out();
//	int  get_power_control();
//	void set_volume_control(int val);
//	int  get_volume_control();
//	void set_power_control(double val);
//	void tune_rig();
//	void set_attenuator(int val);
//	int  get_attenuator();
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
