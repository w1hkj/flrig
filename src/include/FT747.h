#ifndef FT747_H
#define FT747_H

#include "rigbase.h"

class RIG_FT747 : public rigbase {
public:
	RIG_FT747();
	~RIG_FT747(){};

	bool can_split() { return true;}
	void set_split(bool val);
	bool twovfos() {return false;}

	long get_vfoA();
	void set_vfoA(long);
	void set_modeA(int val);
	int  get_modeA();

	long get_vfoB();
	void set_vfoB(long);
	void set_modeB(int val);
	int  get_modeB();

	void selectA();
	void selectB();

	bool get_info();

	bool snd_cmd(string cmd, size_t n);

	void set_PTT_control(int val);
	int  get_PTT_control();

private:
	void init_cmd();
};

#endif
