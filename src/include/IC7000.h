#ifndef _IC7000_H
#define _IC7000_H

#include "IC746.h"

class RIG_IC7000 : public RIG_IC746PRO {
public:
	RIG_IC7000();
	~RIG_IC7000(){}

	void initialize();

	long get_vfoA(void);
	void set_vfoA(long f);
	int  get_bwA();
	void set_bwA(int);
	int  get_modeA();
	void set_modeA(int val);

	long get_vfoB(void);
	void set_vfoB(long f);
	int  get_bwB();
	void set_bwB(int);
	void set_modeB(int val);
	int  get_modeB();

	void selectA();
	void selectB();

	int  adjust_bandwidth(int m);
	int  def_bandwidth(int m);
	void set_attenuator( int val );
	int  get_attenuator();
	void set_preamp(int val);
	int  get_preamp();

	void set_volume_control(int val);
	int  get_volume_control();
	void get_vol_min_max_step(int &min, int &max, int &step);
	void set_rf_gain(int val);
	int  get_rf_gain();
	void set_squelch(int val);
	int  get_squelch();
	void set_power_control(double val);
	int  get_power_control();
	int  get_auto_notch();
	void set_auto_notch(int v);
	int  get_smeter();

	void set_split(bool v);

	int  get_mic_gain();
	void set_mic_gain(int val);
	void get_mic_gain_min_max_step(int &min, int &max, int &step);

};


#endif
