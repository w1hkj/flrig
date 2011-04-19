#ifndef _IC706MKIIG_H
#define _IC706MKIIG_H

#include "ICbase.h"

class RIG_IC706MKIIG : public RIG_ICOM {
public:
	RIG_IC706MKIIG();
	~RIG_IC706MKIIG(){}

	void select_A();
	void select_B();

	long get_vfoA ();
	void set_vfoA (long freq);

	long get_vfoB(void);
	void set_vfoB(long f);

	void set_modeA(int val);
	int  get_modeA();
	int  get_modetype(int n);
	void set_bwA(int val);
	int  get_bwA();

	int  get_smeter();
//	int  get_swr();
	void set_attenuator(int val);

	void set_split(bool);
	bool can_split() { return true; }
};

#endif
