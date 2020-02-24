// ----------------------------------------------------------------------------
// Copyright (C) 2014
//              David Freese, W1HKJ
//
// This file is part of flrig.
//
// flrig is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// flrig is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#ifndef _status_H
#define _status_H

#include <string>
#include <FL/Fl.H>
#include <FL/Enumerations.H>

#include "rig.h"

using namespace std;

#define FPLEN 20 // used in tt550 power sample

struct status {
	int		mainX;
	int		mainY;
	int		mainW;
	int		mainH;
	int		UIsize;
	bool	UIchanged;
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
	int		byte_interval;

	string	aux_serial_port;
	bool	aux_SCU_17;
	bool	aux_rts;
	bool	aux_dtr;

	string	sep_serial_port;
	bool	sep_rtsptt;
	bool	sep_dtrptt;
	bool	sep_rtsplus;
	bool	sep_dtrplus;
	bool	sep_SCU_17;
	int		CIV;
	bool	USBaudio;

	int		poll_smeter;
	int		poll_frequency;
	int		poll_mode;
	int		poll_bandwidth;
	int		poll_volume;
	int		poll_auto_notch;
	int		poll_notch;
	int		poll_ifshift;
	int		poll_power_control;
	int		poll_pre_att;
	int		poll_micgain;
	int		poll_squelch;
	int		poll_rfgain;
	int		poll_pout;
	int		poll_swr;
	int		poll_alc;
	int		poll_split;
	int		poll_noise;
	int		poll_nr;
	int 	poll_vfoAorB;
	int		poll_meters;
	int		poll_ops;
	int		poll_compression;
	int		poll_tuner;
	int		poll_all;

	int		iBW_A;
	int		imode_A;
	int		freq_A;
	int		iBW_B;
	int		imode_B;
	int		freq_B;

	std::string filters;
	std::string bandwidths;

	bool	use_rig_data;

	bool	spkr_on;
	int		volume;
	double	power_level;
	int		mic_gain;
	bool	notch;
	int		notch_val;
	bool	shift;
	int		shift_val;
	bool	pbt_lock;
	int		pbt_inner;
	int		pbt_outer;
	int		rfgain;
	int		squelch;

	int		schema;
	bool	hrd_buttons;
	int		sliders_button;

	int		line_out;
	bool	data_port;
	int		agc_level;
	int		cw_wpm;
	float	cw_weight;
	int		cw_vol;
	int		cw_spot;
	bool	spot_onoff;
	int		cw_spot_tone;
	bool	enable_keyer;
	int		break_in;
	double	cw_qsk;
	double	cw_delay;
	bool	vox_onoff;
	int		vox_gain;
	int		vox_anti;
	int		vox_hang;
	bool	vox_on_dataport;
	int		compression;
	bool	compON;

	int		noise_reduction;
	int		noise_reduction_val;
	int		nb_level;

	bool	noise;
	int		attenuator;
	int		preamp;
	int		auto_notch;

	int		split;
	int		no_txqsy;

	int		rx_avg;
	int		rx_peak;
	int		pwr_avg;
	int		pwr_peak;
	int		pwr_scale;

// ic7610 special controls
	bool	digi_sel_on_off;
	int		digi_sel_val;
	int		index_ic7610att;
	bool	dual_watch;

// ft950 reverse RG0; readings
	bool	ft950_rg_reverse;

	bool	restore_frequency;
	bool	restore_mode;
	bool	restore_bandwidth;
	bool	restore_volume;
	bool	restore_mic_gain;
	bool	restore_rf_gain;
	bool	restore_power_control;
	bool	restore_if_shift;
	bool	restore_notch;
	bool	restore_auto_notch;
	bool	restore_noise;
	bool	restore_squelch;
	bool	restore_split;
	bool	restore_pre_att;
	bool	restore_nr;
	bool	restore_comp_on_off;
	bool	restore_comp_level;

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

	bool	tt550_use_line_in;

	int		tt550_xmt_bw;
	bool	tt550_use_xmt_bw;

	int		tt550_AM_level;

	int		tt550_encoder_step;
	int		tt550_encoder_sensitivity;
	int		tt550_keypad_timeout;

	int		tt550_F1_func;
	int		tt550_F2_func;
	int		tt550_F3_func;

	int		tt550_Nsamples;
	bool	tt550_at11_inline;
	bool	tt550_at11_hiZ;

// =========================
//and others
	double	vfo_adj;
	int		bfo_freq;
	int		rit_freq;
	int		xit_freq;
	int		bpf_center;
	bool	use_bpf_center;

// IC706MKIIG filters
	bool	use706filters;
	string	ssb_cw_wide;		//FL-103
	string	ssb_cw_normal;		//FL-272
	string	ssb_cw_narrow;		//FL-232

// =========================
// User command buttons
   string	label1;
   string	command1;
   string	shftcmd1;
   string	label2;
   string	command2;
   string	shftcmd2;
   string	label3;
   string	command3;
   string	shftcmd3;
   string	label4;
   string	command4;
   string	shftcmd4;
   string	label5;
   string	command5;
   string	shftcmd5;
   string	label6;
   string	command6;
   string	shftcmd6;
   string	label7;
   string	command7;
   string	shftcmd7;
   string	label8;
   string	command8;
   string	shftcmd8;

   string	label9;
   string	command9;
   string	shftcmd9;
   string	label10;
   string	command10;
   string	shftcmd10;
   string	label11;
   string	command11;
   string	shftcmd11;
   string	label12;
   string	command12;
   string	shftcmd12;
   string	label13;
   string	command13;
   string	shftcmd13;
   string	label14;
   string	command14;
   string	shftcmd14;
   string	label15;
   string	command15;
   string	shftcmd15;
   string	label16;
   string	command16;
   string	shftcmd16;
// =========================
	int		bg_red;
	int		bg_green;
	int		bg_blue;

	int		fg_red;
	int		fg_green;
	int		fg_blue;

	int		swrRed;
	int		swrGreen;
	int		swrBlue;

	int		pwrRed;
	int		pwrGreen;
	int		pwrBlue;

	int		smeterRed;
	int		smeterGreen;
	int		smeterBlue;

	int		peakRed;
	int		peakGreen;
	int		peakBlue;

	int		fg_sys_red;
	int		fg_sys_green;
	int		fg_sys_blue;

	int		bg_sys_red;
	int		bg_sys_green;
	int		bg_sys_blue;

	int		bg2_sys_red;
	int		bg2_sys_green;
	int		bg2_sys_blue;

	int		slider_red;
	int		slider_green;
	int		slider_blue;

	int		slider_btn_red;
	int		slider_btn_green;
	int		slider_btn_blue;

	int		lighted_btn_red;
	int		lighted_btn_green;
	int		lighted_btn_blue;

	Fl_Font	fontnbr;

	bool	tooltips;

	string	ui_scheme;

//	string	server_port;
//	string	server_addr;

	string	tcpip_port;
	string	tcpip_addr;
	int		tcpip_ping_delay;
	int		tcpip_reconnect_after;
	int		tcpip_drops_allowed;
	bool	use_tcpip;

	bool	xcvr_auto_on;
	bool	xcvr_auto_off;

	bool	external_tuner;

	bool	trace;
	bool	rigtrace;
	bool	settrace;
	bool	gettrace;
	bool	debugtrace;
	bool	xmltrace;
	bool	rpctrace;
	bool	start_stop_trace;
	int		rpc_level;

// bands; defaults for FT857 / FT897
// frequency, mode, txCTCSS, rxCTCSS, offset, offset_freq;
	int		f160, m160, txT_160, rxT_160, offset_160, oF_160;
	int		f80,  m80,  txT_80,  rxT_80,  offset_80,  oF_80;
	int		f40,  m40,  txT_40,  rxT_40,  offset_40,  oF_40;
	int		f30,  m30,  txT_30,  rxT_30,  offset_30,  oF_30;
	int		f20,  m20,  txT_20,  rxT_20,  offset_20,  oF_20;
	int		f17,  m17,  txT_17,  rxT_17,  offset_17,  oF_17;
	int		f15,  m15,  txT_15,  rxT_15,  offset_15,  oF_15;
	int		f12,  m12,  txT_12,  rxT_12,  offset_12,  oF_12;
	int		f10,  m10,  txT_10,  rxT_10,  offset_10,  oF_10;
	int		f6,    m6,  txT_6,   rxT_6,   offset_6,   oF_6;
	int		f2,    m2,  txT_2,   rxT_2,   offset_2,   oF_2;
	int		f70,  m70,  txT_70,  rxT_70,  offset_70,  oF_70;

	void saveLastState();
	void loadLastState();
	bool loadXcvrState(string);
	void UI_laststate();

	string info();
};

extern status progStatus;
extern string xcvr_name;

extern void ss_trace(bool on);

#endif
