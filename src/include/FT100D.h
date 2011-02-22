#ifndef FT100D_H
#define FT100D_H

#include "rigbase.h"

class RIG_FT100D : public rigbase {
private:
	double fwdpwr;
	double refpwr;
	double fwdv;
	double refv;
	int afreq, amode, aBW;
	int bfreq, bmode, bBW;

public:
	RIG_FT100D();
	~RIG_FT100D(){};
	
	void initialize();

	bool get_info();

	bool can_split() { return true;}
	void set_split(bool val);
	bool twovfos() {return false;}

	long get_vfoA();
	void set_vfoA(long);
	void set_modeA(int val);
	int  get_modeA();
	void set_bwA(int val);
	int  get_bwA();

	long get_vfoB();
	void set_vfoB(long);
	void set_modeB(int val);
	int  get_modeB();
	void set_bwB(int val);
	int  get_bwB();

	int  def_bandwidth(int m);

	void selectA();
	void selectB();

	void set_PTT_control(int val);
	int  get_smeter();
	int  get_swr();
	int  get_power_out();

private:
	void init_cmd();
};


#endif
