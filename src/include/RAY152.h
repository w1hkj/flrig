#ifndef _RAY152_H
#define _RAY152_H

#include "rigbase.h"

class RIG_RAY152 : public rigbase {
public:
	RIG_RAY152();
	~RIG_RAY152(){}
	
	void initialize();
	void shutdown();

	void get_data();

	long get_vfoA();
	void set_vfoA(long);
	long get_vfoB();
	void set_vfoB(long);
	void set_PTT_control(int val);
	void set_modeA(int val);
	int  get_modeA();
	void set_modeB(int val);
	int  get_modeB();
	int  get_modetype(int n);

	int  get_volume_control() {return vol;}
	void get_vol_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 100; step = 1; }
	void set_volume_control(int val);

	void set_rf_gain(int val);
	int  get_rf_gain() {return rfg;}
	void get_rf_min_max_step(int &min, int &max, int &step) {
		min = 0; max = 100; step = 1; }

	void setRit(int v);
	int  getRit();
	void get_RIT_min_max_step(int &min, int &max, int &step) {
		min = -200; max = 200; step = 10; }

	int  get_smeter(void);
	int  get_power_out(void);

	void set_squelch(int val);
	int  get_squelch();

	void set_noise(bool on);
	int  get_noise();

	void set_auto_notch(int v);
	int  get_auto_notch();

	int  adjust_bandwidths(int m) { return 0; }
	
private:
	int vol;
	int rfg;
	int squelch;
	int nb;
	int nb_set;
	int agc;
	bool dumpdata;
};

#endif
