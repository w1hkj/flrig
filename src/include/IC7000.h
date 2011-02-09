#ifndef _IC7000_H
#define _IC7000_H

#include "IC746.h"

class RIG_IC7000 : public RIG_IC746PRO {
public:
	RIG_IC7000();
	~RIG_IC7000(){}

	void set_modeA(int val);
	int  get_modeA();
	void set_modeB(int val);
	int  get_modeB();

	int  adjust_bandwidth(int m);
	void set_attenuator( int val );
	int  get_attenuator();
	void set_preamp(int val);
	int  get_preamp();
};


#endif
