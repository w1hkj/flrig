#ifndef FT100D_H
#define FT100D_H

#include "rigbase.h"

class RIG_FT100D : public rigbase {
private:
	double fwdpwr;
	double refpwr;
	double fwdv;
	double refv;
public:
	RIG_FT100D();
	~RIG_FT100D(){};

	long get_vfoA();
	void set_vfoA(long);
	void set_PTT_control(int val);
	void set_modeA(int val);
	int  get_modeA();
	void set_bwA(int val);
	int  get_bwA();
	int  get_smeter();
	int  get_swr();
	int  get_power_out();

private:
	void init_cmd();
};


#endif
