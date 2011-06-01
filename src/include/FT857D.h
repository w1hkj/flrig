#ifndef FT857D_H
#define FT857D_H

#include "FT817.h"

class RIG_FT857D : public RIG_FT817 {
public:
	RIG_FT857D();
	~RIG_FT857D(){};

	long get_vfoA();
	void set_vfoA(long);

	long get_vfoB();
	void set_vfoB(long);

	void set_PTT_control(int val);

	void set_modeA(int val);
	int  get_modeA();
	int  get_modetype(int n);

	void set_modeB(int val);
	int  get_modeB();

	int  get_power_out(void);
	int  get_smeter(void);

	void selectA();
	void selectB();

	bool can_split();
	void set_split(bool val);

	int  adjust_bandwidth(int m) { return 0; }

private:
	void init_cmd();
protected:
	bool onB;
};

class RIG_FT897D : public RIG_FT857D {
public :
	RIG_FT897D();
	~RIG_FT897D() {};
};

#endif
