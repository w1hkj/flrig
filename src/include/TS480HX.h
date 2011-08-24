#ifndef _TS480HX_H
#define _TS480HX_H

#include "rigbase.h"

class RIG_TS480HX : public rigbase {
private:
	bool beatcancel_on;
	bool preamp_on;
	bool att_on;
	const char *_mode_type;
	bool menu_45;
public:
	RIG_TS480HX();
	~RIG_TS480HX(){}

	bool sendTScommand(string, int);

	void initialize();
	void shutdown();

	void check_menu_45();

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
	int  get_power_out();
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
	bool get_split();
	bool twovfos() {return true;}

	void set_volume_control(int val);
	int  get_volume_control();
	void set_power_control(double val);
	int  get_power_control();
	void get_pc_min_max_step(double &min, double &max, double &step) {
		min = 5; max = 200; step = 1; }
	void set_attenuator(int val);
	int  get_attenuator();

	int  adjust_bandwidth(int m);
	int  def_bandwidth(int m);
	int  set_widths(int val);

	const char **bwtable(int);

	const char * get_bwname_(int bw, int md);
};


#endif
