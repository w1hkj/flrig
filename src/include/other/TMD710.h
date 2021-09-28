// 20201224

#ifndef TMD710_H
#define TMD710_H

#include "rigbase.h"

class RIG_TMD710 : public rigbase {
public:
	RIG_TMD710();
	~RIG_TMD710(){}

	bool check();

	unsigned long int get_vfoA();
	unsigned long int get_vfoB();
	void set_vfoA(unsigned long int);
	void set_vfoB(unsigned long int);
	void set_modeA(int val);
	void set_modeB(int val);
	int  get_modeA();
	int  get_modeB();
	int  get_modetype(int);

	int  adjust_bandwidth(int val) { return 0; }

	void selectA();
	void selectB();
//	void set_attenuator(int val);
//	int  get_attenuator();
//	void set_preamp(int val);
//	int  get_preamp();
//	int  get_smeter();
//	int  get_swr();
//	int  get_power_out();
//	double get_power_control();
//	void set_volume_control(double val);
//	void set_power_control(double val);
	void set_PTT_control(int val);
	int  get_PTT();
//	void tune_rig();
//	void set_bwA(int val);

	double get_power_control();
	void set_power_control(double val);
	void get_pc_min_max_step(double &min, double &max, double &step) {
		min = 5; max = 50; step = 5; }

	void set_squelch(int val);
	int  get_squelch();
	void get_squelch_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 31; step = 1; }

};

#endif
