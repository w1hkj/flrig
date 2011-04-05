#ifndef FT857D_H
#define FT857D_H

#include "FT817.h"

class RIG_FT857D : public RIG_FT817 {
public:
	RIG_FT857D();
	~RIG_FT857D(){};

	long get_vfoA();
	void set_vfoA(long);
	void set_PTT_control(int val);
	void set_modeA(int val);
	int  get_modeA();
	int  get_modetype(int n);
	int  get_power_out(void);
	int  get_smeter(void);

	int  adjust_bandwidth(int m) { return 0; }

private:
	void init_cmd();
};

class RIG_FT897D : public RIG_FT857D {
public :
	RIG_FT897D();
	~RIG_FT897D() {};
};

#endif
