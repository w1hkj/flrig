#ifndef _IC728_H
#define _IC728_H

#include "ICgeneric.h"

class RIG_IC728 : public RIG_ICOM {
public:
	RIG_IC728();
	~RIG_IC728(){}

	long get_vfoA();
	void set_vfoA(long);
};

#endif
