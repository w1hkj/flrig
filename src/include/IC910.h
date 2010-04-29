#ifndef _ICgeneric_H
#define _ICgeneric_H

#include "IC746.h"

class RIG_IC910H : public RIG_IC746 {
public:
	RIG_IC910H();
	~RIG_IC910H(){};
	void set_vfoA(long);
	void set_compression();
	void set_vox_onoff();
	void set_vox_gain();
	void set_vox_anti();
	void set_vox_hang();

private:
	bool comp_is_on;
};


#endif
