#ifndef _IC706MKIIG_H
#define _IC706MKIIG_H

#include "ICgeneric.h"

class RIG_IC706MKIIG : public RIG_ICOM {
public:
	RIG_IC706MKIIG();
	~RIG_IC706MKIIG(){}

	long get_vfoA(void);
	void set_vfoA(long);
	int  get_smeter();
//	int  get_swr();
	void set_attenuator(int val);
};

#endif
