#ifndef _IC735_H
#define _IC735_H

#include "ICgeneric.h"

class RIG_IC735 : public RIG_ICOM {
public:
	RIG_IC735();
	~RIG_IC735(){}

	long get_vfoA();
	void set_vfoA(long);
	void set_mode(int val);
	void set_bandwidth(int val);
};

#endif
