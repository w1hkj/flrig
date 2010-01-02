#ifndef FT767_H
#define FT767_H

#include "rigbase.h"

class RIG_FT767 : public rigbase {
public:
	RIG_FT767();
	~RIG_FT767(){};

	long get_vfoA();
	void set_vfoA(long);
	void set_mode(int val);
	int  get_mode();
	bool snd_cmd(string cmd, size_t n);

private:
	void init_cmd();
};

#endif
