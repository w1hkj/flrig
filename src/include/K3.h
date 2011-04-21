#ifndef K3_H
#define K3_H

#include "rigbase.h"

class RIG_K3 : public rigbase {
public:
	RIG_K3();
	~RIG_K3(){}

	void initialize();
	long get_vfoA();
	void set_vfoA(long);
	long get_vfoB();
	void set_vfoB(long);
	void set_volume_control(int val);
	int  get_volume_control();
	void set_modeA(int val);
	int  get_modeA();
	void set_modeB(int val);
	int  get_modeB();
	void set_power_control(double val);
	void get_pc_min_max_step(double &, double &, double &);
	void set_PTT_control(int val);
	void set_attenuator(int val);
	int  get_attenuator();
	void set_preamp(int val);
	int  get_preamp();
	int  get_smeter();
	void set_noise(bool on);
	int  get_modetype(int n);
	void set_bwA(int val);
	int  get_bwA();
	void set_bwB(int val);
	int  get_bwB();
	int  get_power_out();

	int  adjust_bandwidth(int m);
	int  def_bandwidth(int m);

	bool can_split();
	void set_split(bool val);
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
