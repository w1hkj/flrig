#ifndef _TS480SAT_H
#define _TS480SAT_H

#include "rigbase.h"

class RIG_TS480SAT : public rigbase {
private:
	bool beatcancel_on;
	bool preamp_on;
	bool att_on;
	const char *_mode_type;
public:
	RIG_TS480SAT();
	~RIG_TS480SAT(){}

	bool sendTScommand(string, int);

	void initialize();

	long get_vfoA();
	void set_vfoA(long);
	long get_vfoB();
	void set_vfoB(long);

	int  get_modetype(int n);
	void set_modeA(int val);
	int  get_modeA();
	void set_modeB(int val);
	int  get_modeB();

	int  get_smeter();
	int  get_swr();
	void set_PTT_control(int val);
	void tune_rig();

	void set_bwA(int val);
	int  get_bwA();
	void set_bwB(int val);
	int  get_bwB();
	void selectA();
	void selectB();

	bool can_split() { return true;}
	void set_split(bool val);
	bool twovfos() {return true;}

	void set_volume_control(int val);
	int  get_volume_control();
	void set_power_control(double val);
	int  get_power_control();
	void get_pc_min_max_step(int &min, int &max, int &step) {
		min = 5; max = 100; step = 1; }
	void set_attenuator(int val);
	int  get_attenuator();

};


#endif
