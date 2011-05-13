#ifndef _FT890_H
#define _FT890_H

#include "rigbase.h"

extern const char FT890name_[];
extern const char *FT890modes_[];
extern const char *FT890_widths[];

class RIG_FT890 : public rigbase {
private:
	double fwdpwr;
	double refpwr;
	double fwdv;
	double refv;
	int afreq, amode, aBW;
	int bfreq, bmode, bBW;

public:
	RIG_FT890();
	~RIG_FT890(){};
	
	void initialize();

	bool get_info();

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

	void set_PTT_control(int val);

	int  get_smeter();
	int  get_power_out();

private:
	void init_cmd();
};


#endif
