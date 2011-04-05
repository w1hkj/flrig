#ifndef _TT535_H
#define _TT535_H

#include "ICbase.h"

class RIG_TT535 : public RIG_ICOM {
public:
	RIG_TT535();
	~RIG_TT535(){}

	long get_vfoA();
	void set_vfoA(long);
	long get_vfoB();
	void set_vfoB(long);
//	void set_PTT_control(int val);
	void set_modeA(int val);
	int  get_modeA();
	void set_modeB(int val);
	int  get_modeB();
	int  get_modetype(int n);

	int  adjust_bandwidth(int m);
};

#endif
