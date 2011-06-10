#ifndef _H
#define _H

#include "rigbase.h"

class RIG_K2 : public rigbase {
protected:
	bool hipower;
public:
	RIG_K2();
	~RIG_K2(){}

	void initialize();

	bool twovfos() {return true;}

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

	void selectA();
	void selectB();
//	void A2B();

	int  get_modetype(int n);

	void set_attenuator(int val);
	int  get_attenuator();
	void set_preamp(int val);
	int  get_preamp();

	void tune_rig();
	void set_PTT_control(int val);
	void get_pc_min_max_step(double &min, double &max, double &step);
	int  get_power_control();
	void set_power_control(double val);
	int  get_smeter();
	int  get_power_out();

	bool can_split();
	void set_split(bool val);

	int  adjust_bandwidth(int m);

};

#endif
