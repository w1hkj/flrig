#ifndef _H
#define _H

#include "rigbase.h"

class RIG_K2 : public rigbase {
public:
	RIG_K2();
	~RIG_K2(){}

	void initialize();
	long get_vfoA();
	void set_vfoA(long);
	void set_attenuator(int val);
	int  get_attenuator();
	void set_preamp(int val);
	int  get_preamp();
	void set_modeA(int val);
	int  get_modeA();
	int  get_modetype(int n);
	void set_bwA(int val);
	int get_bwA();
	void tune_rig();
	void set_PTT_control(int val);
    virtual void get_pc_min_max_step(double &min, double &max, double &step){
        min = 0; max = 110; step = 1; }
	int  get_power_control();
	void set_power_control(double val);
	int  get_smeter();
	int  get_power_out();

	int  adjust_bandwidth(int m);

};

#endif
