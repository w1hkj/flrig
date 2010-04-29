#ifndef _IC7200_H
#define _IC7200_H

#include "IC756.h"

class RIG_IC7200 : public RIG_IC756PRO3 {
public:
	RIG_IC7200();
	~RIG_IC7200(){}

	int  get_mode();
	void set_mode(int m);
	void set_mic_gain(int v);
};

#endif
