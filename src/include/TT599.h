#ifndef _TT599_H
#define _TT599_H

#include "ICbase.h"

class RIG_TT599 : public rigbase {
private:
	int fwdpwr;
	int refpwr;

public:
	RIG_TT599();
	~RIG_TT599(){}

	void initialize();

	long get_vfoA();
	void set_vfoA(long);
	long get_vfoB();
	void set_vfoB(long);
	void set_modeA(int val);
	int  get_modeA();
	void set_modeB(int val);
	int  get_modeB();
	void set_bwA(int bw);
	int  get_bwA();
	void set_bwB(int bw);
	int  get_bwB();

	bool can_split() { return true;}
	void set_split(bool val);
	bool twovfos() { return false;}//true;}

	void set_PTT_control(int val);
	int  get_modetype(int n);
	void set_preamp(int);
	int  get_preamp();
	int  get_power_control(void);
	void set_power_control(double val);
	void set_auto_notch(int v);
	int  get_auto_notch();
	void set_attenuator(int val);
	int  get_attenuator();
	int  get_smeter();
	int  get_power_out();
	int  get_swr();

	int  adjust_bandwidth(int);
	int  def_bandwidth(int);

//	void set_agc_level();
};

#endif
