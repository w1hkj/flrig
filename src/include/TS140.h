#ifndef TS140_H
#define TS140_H

#include "rigbase.h"

class RIG_TS140 : public rigbase {
public:
	RIG_TS140();
	~RIG_TS140(){}

	long get_vfoA();
	void set_vfoA(long);
	void set_modeA(int val);
	int  get_modeA();
	int  get_modetype(int);

	int  adjust_bandwidth(int val) { return 0; }

//	void set_attenuator(int val);
//	int  get_attenuator();
//	void set_preamp(int val);
//	int  get_preamp();
//	int  get_smeter();
//	int  get_swr();
//	int  get_power_out();
//	int  get_power_control();
//	void set_volume_control(double val);
//	void set_power_control(double val);
	void set_PTT_control(int val);
//	void tune_rig();
//	void set_bwA(int val);

};

#endif
