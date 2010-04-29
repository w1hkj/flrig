#ifndef _IC703_H
#define _IC703_H

#include "ICbase.h"

class RIG_IC703 : public RIG_ICOM {
public:
	RIG_IC703();
	~RIG_IC703(){}

	long get_vfoA(void);
	void set_vfoA(long);

	void set_mode(int val);
	int  get_mode();
	void set_bandwidth(int val);
	int  get_modetype(int n);
	int  get_bandwidth();

	int  get_smeter();
	int  get_power_out();
	int  get_swr();

	void set_attenuator(int val);
	int  get_attenuator();
	void set_preamp(int val);
	int  get_preamp();
	void set_compression();
	void set_vox_onoff();

protected:
	int  preamp_level;
	int  atten_level;
};

#endif
