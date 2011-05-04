#ifndef FT920_H
#define FT920_H

#include "rigbase.h"

class RIG_FT920 : public rigbase {
private:
	double fwdpwr;
	double refpwr;
	double fwdv;
	double refv;
	int afreq, amode, aBW;
	int bfreq, bmode, bBW;

public:
	RIG_FT920();
	~RIG_FT920(){};
	
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
	int  adjust_bandwidth(int);

	void selectA();
	void selectB();

//	int  get_smeter();
//	int  get_swr();
//	int  get_power_out();

private:
	void init_cmd();
};


#endif
