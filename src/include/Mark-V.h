#ifndef MARK_V_H
#define MARK_V_H

#include "rigbase.h"


class RIG_MARK_V : public rigbase {
public :
	RIG_MARK_V();
	~RIG_MARK_V() {};
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
