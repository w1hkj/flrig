#ifndef FT817_H
#define FT817_H

#include "rigbase.h"

class RIG_FT817 : public rigbase {
public:
	RIG_FT817();
	~RIG_FT817(){};

	long get_vfoA();
	void set_vfoA(long);
	void set_PTT_control(int val);
	void set_modeA(int val);
	int  get_modeA();
	int  get_modetype(int n);
	int  get_power_out(void);
	int  get_smeter(void);

private:
	void init_cmd();
};

#endif
