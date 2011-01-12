#ifndef _TT565_H
#define _TT565_H

#include "ICbase.h"

class RIG_TT565 : public RIG_ICOM {
public:
	RIG_TT565();
	~RIG_TT565(){}

	long get_vfoA();
	void set_vfoA(long);
	void set_PTT_control(int val);
	void set_modeA(int val);
	int  get_modeA();
	int  get_modetype(int n);
};

#endif
