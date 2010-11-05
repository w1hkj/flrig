#ifndef _IC756_H
#define _IC756_H

#include "IC746.h"

class RIG_IC756PRO2 : public RIG_IC746PRO {
public:
	RIG_IC756PRO2();
	virtual ~RIG_IC756PRO2(){}
	void set_attenuator(int val);
	int  get_attenuator();
	void set_preamp(int val);
	int  get_preamp();
};

class RIG_IC756PRO3 : public RIG_IC756PRO2 {
public:
	RIG_IC756PRO3();
	~RIG_IC756PRO3(){}
	void set_modeA(int val);
	int  get_modeA();
};

#endif
