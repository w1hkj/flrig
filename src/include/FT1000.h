#ifndef FT1000_H
#define FT1000_H

#include "rigbase.h"


class RIG_FT1000 : public rigbase {
public :
	RIG_FT1000();
	~RIG_FT1000() {};
	long get_vfoA();
	void set_vfoA(long);
	void set_PTT_control(int val);
	void set_modeA(int val);
	int  get_modeA();
	int  get_modetype(int n);
	void set_bwA(int val);
	int  get_bwA();
	int  get_power_out(void);
	int  get_smeter(void);
	void tune_rig();

	int  adjust_bandwidth(int);

private:
	void init_cmd();
};

#endif
