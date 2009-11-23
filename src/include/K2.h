#ifndef _H
#define _H

#include "rigbase.h"

class RIG_K2 : public rigbase {
public:
	RIG_K2();
	~RIG_K2(){}

	long get_vfoA();
	void set_vfoA(long);
	void set_attenuator(int val);
	int  get_attenuator();
	void set_preamp(int val);
	int  get_preamp();
/*
	int  get_smeter();
	int  get_swr();
	int  get_power_out();
	int  get_power_control();
	void set_volume_control(double val);
	void set_power_control(double val);
	void set_PTT_control(int val);
	void tune_rig();
	void set_mode(int val);
	int  get_mode();
	int  get_modetype();
	void set_bandwidth(int val);
*/
};

#endif
