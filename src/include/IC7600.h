#ifndef _IC7600_H
#define _IC7600_H

#include "IC746.h"

class RIG_IC7600 : public RIG_IC746PRO {
public:
	RIG_IC7600();
	~RIG_IC7600(){}

	void select_vfoA();
	void select_vfoB();

	void set_modeA(int val);
	int  get_modeA();

	void set_modeB(int val);
	int  get_modeB();

	void set_bwA(int val);
	int  get_bwA();

	void set_bwB(int val);
	int  get_bwB();

	void set_attenuator( int val );	//alh
	int  get_attenuator();		//alh
	void set_mic_gain(int v);
};

#endif
