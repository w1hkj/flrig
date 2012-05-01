#ifndef _TS2000_H
#define _TS2000_H

#include "rigbase.h"

class RIG_TS2000 : public rigbase {

enum TS2000MODES { LSB, USB, CW, FM, AM, FSK, CWR, FSKR };

private:
	bool notch_on;
	int  preamp_level;
	int  att_level;
	bool rxtxa;
public:
	RIG_TS2000();
	~RIG_TS2000(){}
	
	void initialize();

	long get_vfoA();
	void set_vfoA(long);
	void set_modeA(int val);
	int  get_modeA();
	void set_bwA(int val);
	int  get_bwA();

	long get_vfoB();
	void set_vfoB(long);
	void set_modeB(int val);
	int  get_modeB();
	void set_bwB(int val);
	int  get_bwB();
	int  def_bandwidth(int);

	int  get_modetype(int n);
	int  adjust_bandwidth(int val);
	int  set_widths(int val);

	bool can_split() { return true;}
	void set_split(bool val);
	int  get_split();
	void selectA();
	void selectB();
	bool twovfos() {return true;}

	int  get_smeter();
	int  get_swr();
	int  get_power_out();
	int  get_power_control();
	void set_volume_control(int val);
	int  get_volume_control();
	void set_power_control(double val);
	void get_pc_min_max_step(double &min, double &max, double &step) {
		min = 5; max = 100; step = 1; }
	void set_PTT_control(int val);
	void tune_rig();
	void set_attenuator(int val);
	int  get_attenuator();
	void set_preamp(int val);
	int  get_preamp();
	void set_if_shift(int val);
	bool get_if_shift(int &val);
	void get_if_min_max_step(int &min, int &max, int &step);
	void set_notch(bool on, int val);
	bool get_notch(int &val);
	void get_notch_min_max_step(int &min, int &max, int &step);
	void set_auto_notch(int v);
	int  get_auto_notch();
	void set_noise(bool b);
	int  get_noise();
	void set_mic_gain(int val);
	int  get_mic_gain();
	void get_mic_min_max_step(int &min, int &max, int &step);
	void set_rf_gain(int val);
	int  get_rf_gain();
	void get_rf_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 100; step = 1; }


	bool sendTScommand(string, int, bool);

	const char **bwtable(int);
	const char **lotable(int);
	const char **hitable(int);

	const char * get_bwname_(int bw, int md);

};


#endif
