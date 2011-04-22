#ifndef _IC7000_H
#define _IC7000_H

#include "IC746.h"

class RIG_IC7000 : public RIG_IC746PRO {
public:
	RIG_IC7000();
	~RIG_IC7000(){}

	void initialize();

	long get_vfoA(void);
	void set_vfoA(long f);
	int  get_bwA();
	void set_bwA(int);
	int  get_modeA();
	void set_modeA(int val);

	long get_vfoB(void);
	void set_vfoB(long f);
	int  get_bwB();
	void set_bwB(int);
	void set_modeB(int val);
	int  get_modeB();

	void selectA();
	void selectB();

	int  adjust_bandwidth(int m);
	int  def_bandwidth(int m);
	void set_attenuator( int val );
	int  get_attenuator();
	void set_preamp(int val);
	int  get_preamp();

	void set_auto_notch(int v);
	void set_split(bool v);

};


#endif
