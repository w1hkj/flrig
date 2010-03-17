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
	void set_mode(int val);
	int  get_mode();
	int  get_modetype(int n);
	void set_bandwidth(int val);
	int  get_bandwidth();
	int  get_power_out(void);
	int  get_smeter(void);
	void tune_rig();

private:
	void init_cmd();
};

#endif
