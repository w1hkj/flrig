#ifndef FT1000MP_H
#define FT1000MP_H

#include "rigbase.h"


class RIG_FT1000MP : public rigbase {
public :
	RIG_FT1000MP();
	~RIG_FT1000MP() {};

	void initialize();

	bool can_split() { return true;}
	void set_split(bool val);
	bool twovfos() {return true;}

	long get_vfoA();
	void set_vfoA(long);
	long get_vfoB(void);
	void set_vfoB(long f);

	void set_modeA(int val);
	int  get_modeA();
	void set_modeB(int val);
	int  get_modeB();
	int  get_modetype(int n);

	void selectA();
	void selectB();

	int  get_bwA();
	void set_bwA(int n);
	int  get_bwB();
	void set_bwB(int n);

	void set_PTT_control(int val);
	void tune_rig();

	int  get_power_out(void);
	int  get_smeter(void);
	int  get_swr(void);
	int  get_alc(void);

	int  def_bandwidth(int m);

private:
	bool tune_on;
	void init_cmd();
	void get_info(void);
};

#endif
