#ifndef _IC735_H
#define _IC735_H

#include "ICbase.h"

class RIG_IC735 : public RIG_ICOM {
public:
	RIG_IC735();
	~RIG_IC735(){}

	void selectA();
	void selectB();

	long get_vfoA();
	void set_vfoA(long);

	long get_vfoB(void);
	void set_vfoB(long f);

	void set_modeA(int val);
	int  get_modeA();

};

#endif
