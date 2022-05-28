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

#define FPLEN 20 // used in tt550 power sample

struct status {
	int		mainX;
	int		mainY;
	int		mainW;
	int		mainH;
	int		UIsize;
	bool	UIchanged;

	int		memX;
	int		memY;
	int		memW;
	int		memH;

	int		ddX;
	int		ddY;

	std::string	xcvr_serial_port;
	int		comm_baudrate;
	int		stopbits;
	int		comm_retries;
	int		comm_wait;
	int		comm_timeout;
	bool	comm_echo;

	int		comm_catptt;
	int		comm_rtsptt;
	int		comm_dtrptt;

	bool	comm_rtscts;
	bool	comm_rtsplus;
	bool	comm_dtrplus;
	int		serloop_timing;
	int		byte_interval;

	std::string	aux_serial_port;
	bool	aux_SCU_17;
	bool	aux_rts;
	bool	aux_dtr;

	std::string	xmlport;

	std::string	sep_serial_port;

	int		sep_rtsptt;
	int		sep_dtrptt;

	bool	sep_rtsplus;
	bool	sep_dtrplus;
	bool	sep_SCU_17;

	std::string	cmedia_device;
	std::string	cmedia_gpio_line;
	int		cmedia_ptt;

	std::string	tmate2_device;
	std::string	tmate2_freq_step;
	bool	tmate2_connected;

	bool	disable_CW_ptt;

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
//	int 	poll_vfoAorB;
	int		poll_meters;
	int		poll_ops;
	int		poll_compression;
	int		poll_tuner;
	int		poll_ptt;
	int		poll_break_in;
	int		poll_all;

	int		iBW_A;
	int		imode_A;
	unsigned long int	freq_A;
	int		iBW_B;
	int		imode_B;
	unsigned long int	freq_B;

	std::string filters;
	std::string bandwidths;

	bool	use_rig_data;

	bool	spkr_on;
	int		volume;
	double	power_level;
	double	power_limit;
	bool	enable_power_limit;
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
	int		embed_tabs;
	int		show_tabs;
	bool    first_use;
	std::string visible_tab;

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

	bool	sync_clock;
	bool	sync_gmt;

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
	std::string	ssb_cw_wide;		//FL-103
	std::string	ssb_cw_normal;		//FL-272
	std::string	ssb_cw_narrow;		//FL-232

// =========================
// User command buttons
   std::string	label1;
   std::string	command1;
   std::string	shftcmd1;
   std::string	label2;
   std::string	command2;
   std::string	shftcmd2;
   std::string	label3;
   std::string	command3;
   std::string	shftcmd3;
   std::string	label4;
   std::string	command4;
   std::string	shftcmd4;
   std::string	label5;
   std::string	command5;
   std::string	shftcmd5;
   std::string	label6;
   std::string	command6;
   std::string	shftcmd6;
   std::string	label7;
   std::string	command7;
   std::string	shftcmd7;
   std::string	label8;
   std::string	command8;
   std::string	shftcmd8;

   std::string	label9;
   std::string	command9;
   std::string	shftcmd9;
   std::string	label10;
   std::string	command10;
   std::string	shftcmd10;
   std::string	label11;
   std::string	command11;
   std::string	shftcmd11;
   std::string	label12;
   std::string	command12;
   std::string	shftcmd12;
   std::string	label13;
   std::string	command13;
   std::string	shftcmd13;
   std::string	label14;
   std::string	command14;
   std::string	shftcmd14;
   std::string	label15;
   std::string	command15;
   std::string	shftcmd15;
   std::string	label16;
   std::string	command16;
   std::string	shftcmd16;

   std::string	label17;
   std::string	command17;
   std::string	shftcmd17;
   std::string	label18;
   std::string	command18;
   std::string	shftcmd18;
   std::string	label19;
   std::string	command19;
   std::string	shftcmd19;
   std::string	label20;
   std::string	command20;
   std::string	shftcmd20;
   std::string	label21;
   std::string	command21;
   std::string	shftcmd21;
   std::string	label22;
   std::string	command22;
   std::string	shftcmd22;
   std::string	label23;
   std::string	command23;
   std::string	shftcmd23;
   std::string	label24;
   std::string	command24;
   std::string	shftcmd24;

   std::string	label_on_start1;
   std::string	cmd_on_start1;

   std::string	label_on_start2;
   std::string	cmd_on_start2;

   std::string	label_on_start3;
   std::string	cmd_on_start3;

   std::string	label_on_start4;
   std::string	cmd_on_start4;

   std::string	label_on_exit1;
   std::string	cmd_on_exit1;

   std::string	label_on_exit2;
   std::string	cmd_on_exit2;

   std::string	label_on_exit3;
   std::string	cmd_on_exit3;

   std::string	label_on_exit4;
   std::string	cmd_on_exit4;

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

	int		voltRed;
	int		voltGreen;
	int		voltBlue;
	int		display_voltmeter;

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

	int		tab_red;
	int		tab_green;
	int		tab_blue;

	Fl_Font	fontnbr;

	bool	tooltips;

	std::string	ui_scheme;

//	std::string	server_port;
//	std::string	server_addr;

	std::string	tcpip_port;
	std::string	tcpip_addr;
	int		tcpip_ping_delay;
	int		tcpip_reconnect_after;
	int		tcpip_drops_allowed;
	bool	use_tcpip;

	std::string	tci_port;
	std::string	tci_addr;

	int		tci_center;

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
	bool	serialtrace;
	bool	tcitrace;
	bool	start_stop_trace;
	int		rpc_level;

// bands; defaults for FT857 / FT897 / Xiegu-G90
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
	int		f12G, m12G, txT_12G, rxT_12G, offset_12G, oF_12G;
	int		fgen, mgen, txT_gen, rxT_gen, offset_gen, oF_gen;

// memory management
	Fl_Font	memfontnbr;
	int		memfontsize;

// gpio parameters
	int		gpio_ptt;
	int		enable_gpio;
	int		gpio_on;
	int		gpio_pulse_width;

// KXPA 100 presence
//	int		kxpa;

// cwio parameters
	int		cwioWPM;
	int		cwioKEYLINE;
	int		cwioSHARED;
	int		cwioPTT;
	int		cwioCONNECTED;
	int		cwioINVERTED;
	double	cwio_comp;
	double	cwio_keycorr;
	std::string	cwioPORT;

	std::string	cwio_msgs[12];
	std::string	cwio_labels[12];

	std::string	BT;
	std::string	AA;
	std::string	AS;
	std::string	AR;
	std::string	KN;
	std::string	SK;
	std::string	INT;
	std::string	HM;
	std::string	VE;

// FSK_ parameters
	int		FSK_KEYLINE;
	int		FSK_SHARED;
	int		FSK_PTT;
	int		FSK_CONNECTED;
	int		FSK_INVERTED;
	int		FSK_STOPBITS;
	std::string	FSK_PORT;
	std::string	FSK_msgs[12];
	std::string	FSK_labels[12];

// CW logbook parameters
	std::string log_name;
	int  log_cut_numbers;
	int  log_leading_zeros;
	int  log_dupcheck;
	int  qso_nbr;

	void saveLastState();
	void loadLastState();
	bool loadXcvrState(std::string);
	void UI_laststate();

	std::string info();
};

extern status progStatus;
extern std::string xcvr_name;

extern void ss_trace(bool on);

#endif
