#ifndef FT847_H
#define FT847_H

#include "rigbase.h"

class RIG_FT847 : public rigbase {
private:
	double fwdpwr;
	int afreq, amode, aBW;

public:
	RIG_FT847();
	~RIG_FT847(){};

	void initialize();

	bool get_info();

	bool can_split() { return false;}
	bool twovfos() {return false;}

	long get_vfoA();
	void set_vfoA(long);
	void set_modeA(int val);
	int  get_modeA();

	long get_vfoB();
	void set_vfoB(long);
	void set_modeB(int val);
	int  get_modeB();

	void set_PTT_control(int val);
	int  get_smeter();
	int  get_power_out();

private:
	void init_cmd();
};


#endif
