#ifndef _status_H
#define _status_H

#include <string>
#include <FL/Fl.H>
#include <FL/Enumerations.H>

#include "rig.h"

using namespace std;

struct status {
	int		mainX;
	int		mainY;
	int		rig_nbr;
	string	xcvr_serial_port;
	int		comm_baudrate;
	int		stopbits;
	int		comm_retries;
	int		comm_wait;
	int		comm_timeout;
	bool	comm_echo;
	bool	comm_catptt;
	bool	comm_rtsptt;
	bool	comm_dtrptt;
	bool	comm_rtscts;
	bool	comm_rtsplus;
	bool	comm_dtrplus;
	int		serloop_timing;

	string	aux_serial_port;
	bool	aux_rts;
	bool	aux_dtr;

	string	sep_serial_port;
	bool	sep_rtsptt;
	bool	sep_dtrptt;
	bool	sep_rtsplus;
	bool	sep_dtrplus;
	int		CIV;
	bool	USBaudio;

	int		opBW;
	int		opMODE;
	int		freq;
	int		iBW_B;
	int		imode_B;
	int		freq_B;
	bool	use_rig_data;

	bool	spkr_on;
	int		volume;
	int		power_level;
	int		mic_gain;
	bool	notch;
	int		notch_val;
	bool	shift;
	int		shift_val;
	int		rfgain;
	int		squelch;
	int		line_out;
	int		agc_level;
	int		cw_wpm;
	int		cw_vol;
	int		cw_spot;
	bool	vox_onoff;
	int		vox_gain;
	int		vox_anti;
	int		vox_hang;
	int		compression;
	bool	compON;

	bool	noise_reduction;
	int		noise_reduction_val;

	bool	noise;
	int		attenuator;
	int		preamp;
	int		auto_notch;

	bool	use_line_in;

	bool	split;

//============= transceiver specific prameters
//tt550 controls
	int		tt550_line_out;
	int		tt550_agc_level;

	int		tt550_cw_wpm;
	double	tt550_cw_weight;
	int		tt550_cw_vol;
	int		tt550_cw_spot;
	bool	tt550_spot_onoff;
	int		tt550_cw_qsk;
	bool	tt550_enable_keyer;

	bool	tt550_vox_onoff;
	int		tt550_vox_gain;
	int		tt550_vox_anti;
	int		tt550_vox_hang;

	int		tt550_mon_vol;
	int		tt550_squelch_level;
	int		tt550_compression;
	int		tt550_nb_level;
	bool	tt550_compON;
	bool	tt550_tuner_bypass;

	bool	tt550_enable_xmtr;
	bool	tt550_enable_tloop;

	int		tt550_xmt_bw;
	bool	tt550_use_xmt_bw;

	int		tt550_AM_level;

//and others
	double	vfo_adj;
	int		bfo_freq;
	int		rit_freq;
	int		xit_freq;
	int		bpf_center;
	bool	use_bpf_center;
// =========================

	int		s_red;
	int		s_green;
	int		s_blue;

	int		bg_red;
	int		bg_green;
	int		bg_blue;

	int		fg_red;
	int		fg_green;
	int		fg_blue;

	int		meter_red;
	int		meter_green;
	int		meter_blue;

	int		pwrRed;
	int		pwrGreen;
	int		pwrBlue;

	int		swrRed;
	int		swrGreen;
	int		swrBlue;

	Fl_Font	fontnbr;

	bool	tooltips;

	string server_port;

	void saveLastState();
	void loadLastState();
	bool loadXcvrState(const char *);
};

extern status progStatus;

#endif
