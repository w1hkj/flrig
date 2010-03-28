#ifndef FT1000MP_H
#define FT1000MP_H

#include "rigbase.h"


class RIG_FT1000MP : public rigbase {
public :
	RIG_FT1000MP();
	~RIG_FT1000MP() {};
	long get_vfoA();
	void set_vfoA(long);
	void set_PTT_control(int val);
	void set_mode(int val);
	int  get_mode();
	int  get_modetype(int n);
	int  get_power_out(void);
	int  get_smeter(void);

private:
	void init_cmd();
};

#endif
