#ifndef K3_H
#define K3_H

#include "rigbase.h"

class RIG_K3 : public rigbase {
public:
	RIG_K3();
	~RIG_K3(){}

	void initialize();

	void showparse(int level, string s, string rx);

	long get_vfoA();
	int  parse_vfoA(string);
	void set_vfoA(long);
	long get_vfoB();
	int  parse_vfoB(string);
	void set_vfoB(long);
	void set_modeA(int val);
	int  get_modeA();
	int  parse_modeA(string);
	void set_modeB(int val);
	int  get_modeB();
	int  parse_modeB(string);
	int  get_modetype(int n);
	void set_bwA(int val);
	int  get_bwA();
	int  parse_bwA(string);
	void set_bwB(int val);
	int  get_bwB();
	int  parse_bwB(string);

	void set_power_control(double val);
	int  get_power_control();
	int  parse_power_control(string);
	void get_pc_min_max_step(double &, double &, double &);

	void set_rf_gain(int val);
	int  get_rf_gain();
	int  parse_rf_gain(string);
	void get_rf_min_max_step(int &min, int &max, int &step);

	void set_mic_gain(int val);
	int  get_mic_gain();
	int  parse_mic_gain(string);
	void get_mic_min_max_step(int &min, int &max, int &step);

	void set_volume_control(int val);
	int  get_volume_control();
	int  parse_volume_control(string);

	void set_PTT_control(int val);
	void set_attenuator(int val);
	int  get_attenuator();
	int  parse_attenuator(string);
	void set_preamp(int val);
	int  get_preamp();
	int  parse_preamp(string);
	int  get_smeter();
	int  parse_smeter(string);
	void set_noise(bool on);
	int  get_noise();
	int  parse_noise(string);
	int  get_power_out();
	int  parse_power_out(string);

	int  adjust_bandwidth(int m);
	int  def_bandwidth(int m);

	bool can_split();
	void set_split(bool val);
	bool get_split();
	int  parse_split(string);
	bool twovfos() {return true;}

	void shutdown();

private:
	double  minpwr;
	double  maxpwr;
	double  steppwr;
/*
	int  get_swr();
	void tune_rig();
*/
};

#endif
