#ifndef _TS450S_H
#define _TS450S_H

#include "rigbase.h"

class RIG_TS450S : public rigbase {
private:
	bool beatcancel_on;
	bool preamp_on;
	bool att_on;
	const char *_mode_type;
public:
	RIG_TS450S();
	~RIG_TS450S(){}

	void initialize();

	bool sendTScommand(string, int, bool);

	long get_vfoA();
	void set_vfoA(long);
	int  get_modeA();
	void set_modeA(int val);
	void set_bwA(int val);
	int  get_bwA();

	long get_vfoB();
	void set_vfoB(long);
	int  get_modeB();
	void set_modeB(int val);
	void set_bwB(int val);
	int  get_bwB();

	void selectA();
	void selectB();
	
	int  get_modetype(int n);
	int  def_bandwidth(int val);

	int  get_smeter();
	int  get_swr();
	void set_PTT_control(int val);

	int  adjust_bandwidth(int val) { return 0; }

	bool can_split();
	void set_split(bool val);
	int  get_split();
	bool twovfos() {return true;}

};


#endif
