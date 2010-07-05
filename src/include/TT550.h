#ifndef _TT550_H
#define _TT550_H
// TenTec Pegasus computer controlled transceiver

#include "rigbase.h"

class RIG_TT550 : public rigbase {
private:
	double fwdpwr;
	double refpwr;
	double fwdv;
	double refv;

	int NVal;
	int FVal;
	int IVal;
	int TBfo;
	int IBfo;
	int TFreq;
	int Bfo;

	int ATTlevel;
	int RFgain;

	double VfoAdj;

	bool auto_notch;
	bool noise_reduction;

	void showresponse(string s);
	void showASCII(string s);

	void set_vfoRX(long freq);
	void set_vfoTX(long freq);

public:
	RIG_TT550();
	~RIG_TT550(){}
	
	void initialize();
	void shutdown();

	long get_vfoA();
	void set_vfoA(long);
	int  get_smeter();
	int  get_swr();
	int  get_power_out();
//	int  get_power_control();
	void set_volume_control(int val);
	int  get_volume_control();
	void set_power_control(double val);
	void get_pc_min_max_step(int &min, int &max, int &step) {
		min = 5; max = 100; step = 1; }
	void set_PTT_control(int val);
	void tune_rig();
	void set_attenuator(int val);
//	int  get_attenuator() {return 0}
//	void set_preamp(int val);
//	int  get_preamp();
	void set_mode(int val);
	int  get_mode();
	int  get_modetype(int n);
	void set_bandwidth(int val);
	int  get_bandwidth();
	void set_if_shift(int val);
	bool get_if_shift(int &val);
	void get_if_min_max_step(int &min, int &max, int &step);

//	void set_notch(bool on, int val);
//	bool get_notch(int &val);
//	void get_notch_min_max_step(int &min, int &max, int &step);
	void set_noise(bool b);
	void set_mic_gain(int val);
//	int  get_mic_gain();
	void get_mic_min_max_step(int &min, int &max, int &step);
	void set_rf_gain(int val);
	int  get_rf_gain();
	void get_rf_min_max_step(int &min, int &max, int &step);
	int  adjust_bandwidth(int m);

	void setRit(int v);
	int  getRit();
	void setXit(int v);
	int  getXit();
	void setBfo(int v);
	int  getBfo();

	void setVfoAdj(double v);

	void set_auto_notch(int v);
	void set_noise_reduction(int b);

	void set_line_out();
	void set_agc_level();
	void set_cw_wpm();
	void set_cw_weight();
	void enable_keyer();
	void set_cw_qsk();
	void set_cw_vol();
	void set_cw_spot();
	void set_spot_onoff();
	void set_vox_onoff();
	void set_vox_gain();
	void set_vox_anti();
	void set_vox_hang();
	void set_aux_hang();
	void set_mon_vol();
	void set_compression();
	void set_squelch_level();
	void tuner_bypass();
	void enable_xmtr();
	void enable_tloop();
	void set_nb_level();

	void read_stream();
};

extern const char *TT550_xmt_widths[];

#endif
