#ifndef _TT563_H
#define _TT563_H

#include "ICbase.h"

class RIG_TT563 : public RIG_ICOM {
public:
	RIG_TT563();
	~RIG_TT563(){}

	long get_vfoA();
	void set_vfoA(long);
	void set_PTT_control(int val);
	void set_modeA(int val);
	int  get_modeA();
	int  get_modetype(int n);
};

#endif
