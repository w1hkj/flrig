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

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <FL/Fl_Preferences.H>
#include <FL/Fl_Progress.H>
#include <FL/Enumerations.H>

#include "dialogs.h"

#include "status.h"
#include "util.h"
#include "rig.h"
#include "rigs.h"
#include "support.h"
#include "config.h"
#include "rigpanel.h"
#include "ui.h"
#include "debug.h"

std::string xcvr_name = "NONE";

int current_ui_size = -1;

status progStatus = {
	50,			// int mainX;
	50,			// int mainY;
	735,		// int mainW;
	150,		// int mainH;
	small_ui,	// UISIZE, UIsize;
	false,		// UIchanged;

	20,			// int memX;
	20,			// int memY;
	600,		// int memW;
	164,		// int memH;

	20,			// int ddX;
	20,			// int ddY;

	"NONE",		// std::string xcvr_serial_port;
	0,			// int comm_baudrate;
	2,			// int stopbits;
	2,			// int comm_retries;
	5,			// int comm_wait;
	50,			// int comm_timeout;
	false,		// bool comm_echo;

	0,			// bool comm_catptt;
	0,			// bool comm_rtsptt;
	0,			// bool comm_dtrptt;

	false,		// bool comm_rtscts;
	false,		// bool comm_rtsplus;
	false,		// bool comm_dtrplus;
	500,		// int  serloop_timing;
	1,			// int  byte_interval;

	"NONE",		// std::string aux_serial_port;
	false,		// bool aux_SCU_17;
	false,		// bool aux_rts;
	false,		// bool aux_dtr;

	"12345",	// std::string xmlport;

	"NONE",		// std::string	sep_serial_port;

	0,			// bool	sep_rtsptt;
	0,			// bool	sep_dtrptt;

	false,		// bool	sep_rtsplus;
	false,		// bool	sep_dtrplus;
	false,		// bool	sep_SCU_17;

	"NONE",		// std::string	cmedia_device
	"GPIO-3",	// std::string	cmedia_gpio_line

	0,			// bool	cmedia_ptt

	"NONE",		// std::string	tmate2_devide
	"5000",		// std::string	tmate2_freq_step
	false,		// bool		tmate2_connected

	false,		// bool	disable_CW_ptt;

	0,			// int	CIV;
	false,		// bool	USBaudio;

	1,			// bool	poll_smeter;
	1,			// bool	poll_frequency;
	1,			// bool	poll_mode;
	1,			// bool	poll_bandwidth;
	1,			// bool	poll_volume;
	1,			// bool	poll_auto_notch;
	1,			// bool poll_notch;
	1,			// bool	poll_ifshift;
	1,			// bool	poll_power_control;
	1,			// bool	poll_pre_att;
	1,			// bool	poll_micgain;
	1,			// bool	poll_squelch;
	1,			// bool	poll_rfgain;
	1,			// bool	poll_pout;
	1,			// bool	poll_swr;
	1,			// bool	poll_alc;
	1,			// bool	poll_split;
	1,			// bool	poll_noise;
	1,			// bool	poll_nr;
//	0,			// int  poll_vfoAorB;
	1,			// poll_meters;
	1,			// poll_ops;
	1,			// poll_compression;
	1,			// poll_tuner;
	1,			// poll_ptt;
	4,			// poll_break_in;
	4,			// int	poll_all;

	-1,			// int  iBW_A;
	1,			// int  imode_A;
	14070000,	// unsigned long int freq_A;
	-1,			// int  iBW_B;
	1,			// int  imode_B;
	7070000,	// unsigned long int freq_B;

	"",			// std::string filters;
	"",			// std::string bandwidths;

	true,		// bool use_rig_data;

	false,		// bool spkr_on;
	20,			// int  volume;
	0,			// double  power_level;
	100,		// double power_limit;
	false,		// bool enable_power_limit
	10,			// int  mic_gain;
	false,		// bool notch;
	0,			// int  notch_val;
	false,		// bool shift;
	0,			// int  shift_val;
	0,			// bool pbt_lock;
	0,			// int  pbt_inner;
	0,			// int  pbt_outer;
	100,		// int  rfgain;
	10,			// int  squelch;

	0,			// int  schema;
	0,			// int	embed_tabs;
	0,			// int	show_tabs;
	true,       // bool first_use;			true only during program start
	"",			// std::string visible_tab;
	true,		// bool hrd_buttons
	FL_WHEN_CHANGED,	// int sliders_button

	0,			// int  line_out;
	false,		// bool data_port;
	1,			// int  agc_level;
	18,			// int  cw_wpm;
	3.0,		// double  cw_weight;
	0,			// int  cw_vol;
	0,			// int  cw_spot;
	false,		// bool spot_onoff;
	700,		// int  cw_spot_tone;
	false,		// bool enable_keyer;
	0,			// int	break_in;
	15,			// double	cw_qsk;
	200,		// double	cw_delay;

	false,		// int  vox_onoff;
	10,			// int  vox_gain;
	10,			// int  vox_anti;
	100,		// int  vox_hang; FT950 default
	true,		// bool vox_on_dataport;
	0,			// int  compression;
	false,		// bool compON;

	0,			// bool noise_reduction;
	0,			// int  noise_reduction_val;
	0,			// int  nb_level;

	false,		// bool noise;
	0,			// int  attenuator
	0,			// int  preamp;
	0,			// int  auto_notch;

	0,			// int  split;
	0,			// int  no_txqsy

	1,			// int rx_avg;
	4,			// int rx_peak;
	1,			// int pwr_avg;
	4,			// int pwr_peak;
	4,			// int pwr_scale ==> Autoselect

	1,			// bool sync_clock
	1,			// bool sync_gmt

// ic7610 special controls
	false,		// bool	digi_sel_on_off;
	0,			// int	digi_sel_val;
	6,			// int	ic7610att;
	0,			// bool	dual_watch;

//ft950 reversed RF gain values
	false,		// bool ft950_rg_reverse

	true,		// bool	restore_frequency;
	true,		// bool	restore_mode;
	true,		// bool	restore_bandwidth;
	true,		// bool	restore_volume;
	true,		// bool	restore_mic_gain;
	true,		// bool	restore_rf_gain;
	true,		// bool	restore_power_control;
	true,		// bool	restore_if_shift;
	true,		// bool	restore_notch;
	true,		// bool	restore_auto_notch;
	true,		// bool	restore_noise;
	true,		// bool	restore_squelch;
	true,		// bool	restore_split;
	true,		// bool	restore_pre_att;
	true,		// bool	restore_nr;
	true,		// bool	restore_comp_on_off;
	true,		// bool	restore_comp_level;

//tt550 controls
	80,			// tt550_line_out;
	1,			// tt550_agc_level;

	24,			// tt550_cw_wpm;
	1.0,		// tt550_cw_weight;
	10,			// tt550_cw_vol;
	10,			// tt550_cw_spot;
	false,		// tt550_cw_spot_onoff;
	20,			// tt550_cw_qsk;
	false,		// tt550_enable_keyer;

	false,		// tt550_vox_onoff;
	0,			// tt550_vox_gain;
	0,			// tt550_vox_anti;
	0,			// tt550_vox_hang;

	0,			// tt550_mon_vol;
	0,			// tt550_squelch_level;
	0,			// tt550_compression;
	1,			// tt550_nb_level;
	false,		// tt550_bool compON;
	false,		// tt550_tuner_bypass;

	true,		// tt550_enable_xmtr;
	false,		// tt550_enable_tloop;

	true,		// tt550_use_line_in;

	14,			// tt550_xmt_bw;
	false,		// tt550_use_xmt_bw;

	25,			// tt550_AM_level;
	0,			// tt550_encoder_step;
	1,			// tt550_encoder_sensitivity;
	2000,		// tt550_keypad_timeout;

	0,			// tt550_F1_func;
	0,			// tt550_F2_func;
	0,			// tt550_F3_func;

	5,			// tt550_Nsamples;
	true,		// tt550_at11_inline;
	true,		// tt550_at11_hiZ;

// =========================
	0.0,		// vfo_adj;
	600,		// bfo_freq;
	0,			// rit_freq;
	0,			// xit_freq;
	1500,		// bpf_center;
	true,		// use_bpf_center;

// =========================
// IC706MKIIG filters
	false,		// bool		use706filters
	"EMPTY",	// std::string	ssb_cw_wide;
	"NORMAL",	// std::string	ssb_cw_normal;
	"EMPTY",	// std::string	ssb_cw_narrow;
// optional filter std::strings
// "EMPTY", "NARR", "NORM", "WIDE", "MED",
// "FL-101", "FL-232", "FL-100", "FL-223", "FL-103"

// =========================
	"cmd 1",		// std::string	label1;
	"",				// std::string	command1;
	"",				// std::string	shftcmd1;
	"cmd 2",		// std::string	label2;
	"",				// std::string	command2;
	"",				// std::string	shftcmd2;
	"cmd 3",		// std::string	label3;
	"",				// std::string	command3;
	"",				// std::string	shftcmd3;
	"cmd 4",		// std::string	label4;
	"", 			// std::string	command4;
	"",				// std::string	shftcmd4;
	"cmd 5",		// std::string	label5;
	"",				// std::string	command5;
	"",				// std::string	shftcmd5;
	"cmd 6",		// std::string	label6;
	"",				// std::string	command6;
	"",				// std::string	shftcmd6;
	"cmd 7",		// std::string	label7;
	"",				// std::string	command7;
	"",				// std::string	shftcmd7;
	"cmd 8",		// std::string	label8;
	"",				// std::string	command8;
	"",				// std::string	shftcmd8;

	"cmd 9",		// std::string	label9;
	"",				// std::string	command9;
	"",				// std::string	shftcmd9;
	"cmd 10",		// std::string	label10;
	"",				// std::string	command10;
	"",				// std::string	shftcmd10;
	"cmd 11",		// std::string	label11;
	"",				// std::string	command11;
	"",				// std::string	shftcmd11;
	"cmd 12",		// std::string	label12;
	"",				// std::string	command12;
	"",				// std::string	shftcmd12;
	"cmd 13",		// std::string	label13;
	"",				// std::string	command13;
	"",				// std::string	shftcmd13;
	"cmd 14",		// std::string	label14;
	"",				// std::string	command14;
	"",				// std::string	shftcmd14;
	"cmd 15",		// std::string	label15;
	"",				// std::string	command15;
	"",				// std::string	shftcmd15;
	"cmd 16",		// std::string	label16;
	"",				// std::string	command16;
	"",				// std::string	shftcmd16;

	"cmd 17",		// std::string	label17;
	"",				// std::string	command17;
	"",				// std::string	shftcmd17;
	"cmd 18",		// std::string	label18;
	"",				// std::string	command18;
	"",				// std::string	shftcmd18;
	"cmd 19",		// std::string	label19;
	"",				// std::string	command19;
	"",				// std::string	shftcmd19;
	"cmd 20",		// std::string	label20;
	"",				// std::string	command20;
	"",				// std::string	shftcmd20;
	"cmd 21",		// std::string	label21;
	"",				// std::string	command21;
	"",				// std::string	shftcmd21;
	"cmd 22",		// std::string	label22;
	"",				// std::string	command22;
	"",				// std::string	shftcmd22;
	"cmd 23",		// std::string	label23;
	"",				// std::string	command23;
	"",				// std::string	shftcmd23;
	"cmd 24",		// std::string	label24;
	"",				// std::string	command24;
	"",				// std::string	shftcmd24;

	"1",			// std::string	label_on_start1;
	"",				// std::string	cmd_on_start1;
	"2",			// std::string	label_on_start2;
	"",				// std::string	cmd_on_start2;
	"3",			// std::string	label_on_start3;
	"",				// std::string	cmd_on_start3;
	"4",			// std::string	label_on_start4;
	"",				// std::string	cmd_on_start4;

	"1",			// std::string	label_on_exit1;
	"",				// std::string	cmd_on_exit1;
	"2",			// std::string	label_on_exit2;
	"",				// std::string	cmd_on_exit2;
	"3",			// std::string	label_on_exit3;
	"",				// std::string	cmd_on_exit3;
	"4",			// std::string	label_on_exit4;
	"",				// std::string	cmd_on_exit4;

// =========================

	232,		// int	 bg_red;
	255,		// int	 bg_green;
	232,		// int	 bg_blue;

	0,			// int	 fg_red;
	0,			// int	 fg_green;
	0,			// int	 fg_blue;

	148,		// int	swrRed;
	0,			// int	swrGreen;
	148,		// int	swrBlue;

	180,		// int	pwrRed;
	0,			// int	pwrGreen;
	0,			// int	pwrBlue;

	0,			// int	smeterRed;
	180,		// int	smeterGreen;
	0,			//int	smeterBlue;

	255,		// int	peakRed;
	0,			// int	peakGreen;
	0,			// int	peakBlue;

	0,			// int	voltRed;
	0,			// int	voltGreen;
	255,		// int	voltBlue;
	0,			// int	display_voltmeter;

	0,			// int	fg_sys_red;
	0,			// int	fg_sys_green;
	0,			// int	fg_sys_blue;

	0xc0,		// int	bg_sys_red;
	0xc0,		// int	bg_sys_green;
	0xc0,		// int	bg_sys_blue;

	255,		// int	bg2_sys_red;
	255,		// int	bg2_sys_green;
	255,		// int	bg2_sys_blue;

	232,		// int		slider_red;
	255,		// int		slider_green;
	232,		// int		slider_blue;

	0,			// int		slider_btn_red;
	0,			// int		slider_btn_green;
	128,		// int		slider_btn_blue;

	255,		// int		lighted_btn_red;
	255,		// int		lighted_btn_green;
	0,			// int		lighted_btn_blue;

	230,		// int		tab_red;
	230,		// int		tab_green;
	230,		// int		tab_blue;

	FL_COURIER,	// Fl_Font fontnbr;

	false,		// bool	 tooltips;

	"gtk+",		// std::string ui_scheme

//	"7362",		// std::string server_port
//	"127.0.0.1",// std::string server_address

	"4001",		// std::string tcpip_port
	"127.0.0.1",// std::string tcpip_address
	50,			// int tcpip_ping_delay
	10,			// int tcpip_reconnect_after in seconds
	10,			// int tcpip_drops_allowed;
	false,		// bool use_tcpip

	"40001",	// std::string tci_port
	"127.0.0.1",// std::string tci address
	10,			// int  tci_center;

	false,		// bool xcvr auto on
	false,		// bool xcvr auto off

	false,		// bool external_tuner

	false,		// bool	trace;
	false,		// bool	rigtrace;
	false,		// bool	gettrace;
	false,		// bool	settrace;
	false,		// bool	debugtrace;
	false,		// bool	xmltrace;
	false,		// bool	rpctrace;
	false,		// bool	serialtrace;
	false,		// bool tcitrace;
	false,		// bool	start_stop_trace;
	0,			// int	rpc_level;

// bands; defaults for FT857 / FT897 / Xiegu-G90
// frequency, mode, txCTCSS, rxCTCSS, offset, offset_freq;
	1805000L, 0, 0, 0, 0, 600,		// f160
	3580000L, 0, 0, 0, 0, 600,		// f80 meters
	7070000L, 0, 0, 0, 0, 600,		// f40 meters
	10140000L, 0, 0, 0, 0, 600,		// f30 meters
	14070000L, 0, 0, 0, 0, 600,		// f20 meters
	18100000L, 0, 0, 0, 0, 600,		// f17 meters
	21070000L, 0, 0, 0, 0, 600,		// f15 meters
	24920000L, 0, 0, 0, 0, 600,		// f12 meters
	28070000L, 0, 0, 0, 0, 600,		// f10 meters
	50070000L, 0, 0, 0, 0, 600,		// f6 meters
	144070000L, 0, 0, 0, 0, 600,	// f2 meters
	432100000L, 0, 0, 0, 0, 600,	// f70 cent'
	1210000000L, 0, 0, 0, 0, 600,	// f12G
	15000000L, 0, 0, 0, 0, 600,		// general

// memory management
	4,								// Fl_Font	memfontnbr;
	14,								// int		memfontsize;

// gpio parameters
	false,		// bool	gpio_ptt;
	0,			// int	enable_gpio;
	0,			// int	gpio_on;
	0,			// int	gpio_pulse_width;

// KXPA 100 presence
//	0,			// int kxpa on line = 1

// cwio parameters
	20,			// int    cwioWPM;
	2,			// int    cwioKEYLINE; 1 == RTS, 2 == DTR
	0,			// int    cwioSHARED; 0 = NONE, 1 = CAT, 2 = AUX, 3 = SEP, 4 = CWIO
	0,			// int    cwioPTT; 1 - use PTT, 0 - use Brea-in
	0,			// int    cwioCONNECTED; 1 - connected state; 0 - unconnected state
	0,			// int    cwioINVERTED; 1 - DTR/RTS (-) keying; 
	0,			// double cwio_comp;
	0,			// double cwio_keycorr;
	"",			// std::string cwioPORT;

	"","","","","","",
	"","","","","","",					// std::string	cwio_msgs[12];
	"m2","m2","m3","m4","m4","m6",
	"m7","m8","m9","m10","m11","m12",	// std::string	cwio_labels[12];

	"=",		// std::string	BT;
	"~",		// std::string	AA;
	"<",		// std::string	AS;
	">",		// std::string	AR;
	"%",		// std::string	KN;
	"+",		// std::string	SK;
	"&",		// std::string	INT;
	"{",		// std::string	HM;
	"}",		// std::string	VE;

// FSK_ parameters
	1,			// int		FSK_KEYLINE; 1 == RTS, 2 == DTR
	0,			// int		FSK_SHARED; 0 = NONE, 1 = CAT, 2 = AUX, 3 = SEP
	0,			// int		FSK_PTT; 1 - use PTT, 0 - use Brea-in
	0,			// int		FSK_CONNECTED; 1 - connected state; 0 - unconnected state
	0,			// int		FSK_INVERTED; reverse keying; 
	1,			// int		FSK_STOPBITS; 1 - 1.5; 0 - 2
	"",			// std::string	FSK_PORT;

	"","","","","","",
	"","","","","","",					// std::string	FSK_msgs[12];
	"m2","m2","m3","m4","m4","m6",
	"m7","m8","m9","m10","m11","m12",	// std::string	FSK_labels[12];

// CW logbook parameters
	"",			// std::string log_name;
	0,			// int log_cut_numbers;
	0,			// int log_leading_zeros;
	1,			// int log_dupcheck;
	0			// int log_nbr;

};

void status::saveLastState()
{
	xcvr_name = selrig->name_;
	Fl_Preferences xcvrpref(RigHomeDir.c_str(), "w1hkj.com", PACKAGE_TARNAME);
	xcvrpref.set("xcvr_name", xcvr_name.c_str());

	int mX = mainwindow->x();
	int mY = mainwindow->y();
	int mW = mainwindow->w();
	int mH = mainwindow->h();

	if (dlgMemoryDialog) {
		memX = dlgMemoryDialog->x();
		memY = dlgMemoryDialog->y();
		memW = dlgMemoryDialog->w();
		memH = dlgMemoryDialog->h();
	}

	if (mX >= 0 && mY >= 0) {
		mainX = mX;
		mainY = mY;
		if (UIsize == wide_ui) {
			if (mW < WIDE_MAINW)
				mW = WIDE_MAINW;
			mH = WIDE_MAINH;
		}
		mainW = mW;
		mainH = mH;
	}

	if (tabsGeneric)
		visible_tab = (tabsGeneric->value())->label();

	Fl_Preferences spref(RigHomeDir.c_str(), "w1hkj.com", xcvr_name.c_str());

	spref.set("version", PACKAGE_VERSION);

	spref.set("mainx", mainX);
	spref.set("mainy", mainY);
	spref.set("mainw", mainW);
	spref.set("mainh", mainH);

	spref.set("uisize", UIsize);

	spref.set("memx", memX);
	spref.set("memy", memY);
	spref.set("memw", memW);
	spref.set("memh", memH);

	spref.set("ddx", ddX);
	spref.set("ddy", ddY);

	spref.set("xcvr_serial_port", xcvr_serial_port.c_str());
	spref.set("comm_baudrate", comm_baudrate);
	spref.set("comm_stopbits", stopbits);
	spref.set("comm_retries", comm_retries);
	spref.set("comm_wait", comm_wait);
	spref.set("comm_timeout", comm_timeout);
	spref.set("serloop_timing", serloop_timing);
	spref.set("byte_interval", byte_interval);

	spref.set("comm_echo", comm_echo);
	spref.set("ptt_via_cat", comm_catptt);
	spref.set("ptt_via_rts", comm_rtsptt);
	spref.set("ptt_via_dtr", comm_dtrptt);
	spref.set("rts_cts_flow", comm_rtscts);
	spref.set("rts_plus", comm_rtsplus);
	spref.set("dtr_plus", comm_dtrplus);

	spref.set("disable_CW_ptt", disable_CW_ptt);

	spref.set("civadr", CIV);
	spref.set("usbaudio", USBaudio);

	spref.set("aux_serial_port", aux_serial_port.c_str());
	spref.set("aux_rts", aux_rts);
	spref.set("aux_dtr", aux_dtr);

	spref.set("sep_serial_port", sep_serial_port.c_str());
	spref.set("sep_rtsptt", sep_rtsptt);
	spref.set("sep_dtrptt", sep_dtrptt);
	spref.set("sep_rtsplus", sep_rtsplus);
	spref.set("set_dtrplus", sep_dtrplus);

	spref.set("xmlport", xmlport.c_str());

	spref.set("cmedia_device", cmedia_device.c_str());
	spref.set("cmedia_gpio_line", cmedia_gpio_line.c_str());
	spref.set("cmedia_ptt", cmedia_ptt);

	spref.set("tmate2_device", tmate2_device.c_str());
	spref.set("tmate2_freq_step", tmate2_freq_step.c_str());
	spref.set("tmate2_connected", tmate2_connected);

	spref.set("poll_smeter", poll_smeter);
	spref.set("poll_frequency", poll_frequency);
	spref.set("poll_mode", poll_mode);
	spref.set("poll_bandwidth", poll_bandwidth);
	spref.set("poll_volume", poll_volume);
	spref.set("poll_auto_notch", poll_auto_notch);
	spref.set("poll_notch", poll_notch);
	spref.set("poll_ifshift", poll_ifshift);
	spref.set("poll_power_control", poll_power_control);
	spref.set("poll_pre_att", poll_pre_att);
	spref.set("poll_micgain", poll_micgain);
	spref.set("poll_squelch", poll_squelch);
	spref.set("poll_rfgain", poll_rfgain);
	spref.set("poll_pout", poll_pout);
	spref.set("poll_swr", poll_swr);
	spref.set("poll_alc", poll_alc);
	spref.set("poll_split", poll_split);
	spref.set("poll_noise", poll_noise);
	spref.set("poll_nr", poll_nr);
	spref.set("poll_compression", poll_compression);
	spref.set("poll_tuner", poll_tuner);
	spref.set("poll_ptt", poll_ptt);
	spref.set("poll_break_in", poll_break_in);
	spref.set("poll_all", poll_all);

	spref.set("bw_A", iBW_A);
	spref.set("mode_A", imode_A);

	int freq_A_lower, freq_A_upper;
	freq_A_upper = freq_A / 1000000000;
	freq_A_lower = freq_A - freq_A_upper * 1000000000;
	spref.set("freq_A_u", freq_A_upper);
	spref.set("freq_A_l", freq_A_lower);

	spref.set("bw_B", iBW_B);
	spref.set("mode_B", imode_B);

	int freq_B_lower, freq_B_upper;
	freq_B_upper = freq_B / 1000000000;
	freq_B_lower = freq_B - freq_B_upper * 1000000000;
	spref.set("freq_B_u", freq_B_upper);
	spref.set("freq_B_l", freq_B_lower);

	spref.set("filters", filters.c_str());
	spref.set("bandwidths", bandwidths.c_str());

	spref.set("use_rig_data", use_rig_data);
//	spref.set("restore_rig_data", restore_rig_data);

	spref.set("restore_frequency", restore_frequency);
	spref.set("restore_mode", restore_mode);
	spref.set("restore_bandwidth", restore_bandwidth);
	spref.set("restore_volume", restore_volume);
	spref.set("restore_mic_gain", restore_mic_gain);
	spref.set("restore_rf_gain", restore_rf_gain);
	spref.set("restore_power_control", restore_power_control);
	spref.set("restore_if_shift", restore_if_shift);
	spref.set("restore_notch", restore_notch);
	spref.set("restore_auto_notch", restore_auto_notch);
	spref.set("restore_noise", restore_noise);
	spref.set("restore_squelch", restore_squelch);
	spref.set("restore_split", restore_split);
	spref.set("restore_pre_att", restore_pre_att);
	spref.set("restore_nr", restore_nr);
	spref.set("restore_comp_on_off", restore_comp_on_off);
	spref.set("restore_comp_level", restore_comp_level);

	spref.set("bool_spkr_on", spkr_on);
	spref.set("int_volume", volume);
	spref.set("dbl_power", power_level);
	spref.set("power_limit", power_limit);
	spref.set("enable_power_limit", enable_power_limit);
	spref.set("int_mic", mic_gain);
	spref.set("bool_notch", notch);
	spref.set("int_notch", notch_val);
	spref.set("bool_shift", shift);
	spref.set("int_shift", shift_val);

	spref.set("pbt_lock", pbt_lock);
	spref.set("pbt_inner", pbt_inner);
	spref.set("pbt_outer", pbt_outer);

	spref.set("rfgain", rfgain);
	spref.set("squelch", squelch);
	spref.set("no_txqsy", no_txqsy);

	spref.set("schema", schema);
	spref.set("embed_tabs", embed_tabs);
	if (!embed_tabs && tabs_dialog)
		show_tabs = tabs_dialog->visible();
	spref.set("show_tabs", show_tabs);
	spref.set("visible_tab", visible_tab.c_str());

	spref.set("rx_avg", rx_avg);
	spref.set("rx_peak", rx_peak);
	spref.set("pwr_avg", pwr_avg);
	spref.set("pwr_peak", pwr_peak);
	spref.set("pwr_scale", pwr_scale);

	spref.set("sync_clock", sync_clock);
	spref.set("sync_gmt", sync_gmt);

	spref.set("digi_sel_on_off", digi_sel_on_off);
	spref.set("digi_sel_val", digi_sel_val);
	spref.set("dual_watch", dual_watch);
	spref.set("ic7610att", index_ic7610att);

	spref.set("ft950_rg_reverse", ft950_rg_reverse);

	if (selrig->name_ == rig_TT550.name_) {
		spref.set("tt550_line_out", tt550_line_out);
		spref.set("tt550_agc_level", tt550_agc_level);

		spref.set("tt550_cw_wpm", tt550_cw_wpm);
		spref.set("tt550_cw_weight", tt550_cw_weight);
		spref.set("tt550_cw_vol", tt550_cw_vol);
		spref.set("tt550_cw_spot", tt550_cw_spot);
		spref.set("tt550_spot_onoff", tt550_spot_onoff);
		spref.set("tt550_cw_qsk", tt550_cw_qsk);
		spref.set("tt550_enable_keyer", tt550_enable_keyer);
		spref.set("cw_delay", cw_delay);

		spref.set("tt550_vox_onoff", tt550_vox_onoff);
		spref.set("tt550_vox_gain", tt550_vox_gain);
		spref.set("tt550_vox_anti", tt550_vox_anti);
		spref.set("tt550_vox_hang", tt550_vox_hang);

		spref.set("tt550_mon_vol", tt550_mon_vol);
		spref.set("tt550_squelch_level", tt550_squelch_level);
		spref.set("tt550_compression", tt550_compression);
		spref.set("tt550_nb_level", tt550_nb_level);
		spref.set("tt550_compON", tt550_compON);
		spref.set("tt550_tuner_bypass", tt550_tuner_bypass);
		spref.set("tt550_enable_xmtr", tt550_enable_xmtr);
		spref.set("tt550_enable_tloop", tt550_enable_tloop);

		spref.set("tt550_xmt_bw", tt550_xmt_bw);
		spref.set("tt550_use_xmt_bw", tt550_use_xmt_bw);

		spref.set("tt550_AM_level", tt550_AM_level);

		spref.set("tt550_use_line_in", tt550_use_line_in);

		spref.set("tt550_encoder_step", tt550_encoder_step);
		spref.set("tt550_encoder_sensitivity", tt550_encoder_sensitivity);
		spref.set("tt550_keypad_timeout", tt550_keypad_timeout);

		spref.set("tt550_F1_func", tt550_F1_func);
		spref.set("tt550_F2_func", tt550_F2_func);
		spref.set("tt550_F3_func", tt550_F3_func);

		spref.set("tt550_Nsamples", tt550_Nsamples);
		spref.set("tt550_at11_inline", tt550_at11_inline);
		spref.set("tt550_at11_hiZ", tt550_at11_hiZ);

	} else {
		spref.set("line_out", line_out);
		spref.set("data_port", data_port);
		spref.set("vox_on_dataport", vox_on_dataport);
		spref.set("agc_level", agc_level);
		spref.set("cw_wpm", cw_wpm);
		spref.set("cw_weight", cw_weight);
		spref.set("cw_vol", cw_vol);
		spref.set("cw_spot", cw_spot);
		spref.set("spot_onoff", spot_onoff);
		spref.set("cw_spot_tone", cw_spot_tone);
		spref.set("cw_qsk", cw_qsk);
		spref.set("cw_delay", cw_delay);
		spref.set("enable_keyer", enable_keyer);
		spref.set("break_in", break_in);
		spref.set("vox_onoff", vox_onoff);
		spref.set("vox_gain", vox_gain);
		spref.set("vox_anti", vox_anti);
		spref.set("vox_hang", vox_hang);
		spref.set("compression", compression);
		spref.set("compON", compON);
	}

	spref.set("noise_reduction", noise_reduction);
	spref.set("noise_red_val", noise_reduction_val);
	spref.set("nb_level", nb_level);

	spref.set("bool_noise", noise);
	spref.set("int_preamp", preamp);
	spref.set("int_att", attenuator);

	spref.set("vfo_adj", vfo_adj);
	spref.set("bfo_freq", bfo_freq);
	spref.set("rit_freq", rit_freq);
	spref.set("xit_freq", xit_freq);
	spref.set("bpf_center", bpf_center);
	spref.set("use_bpf_center", use_bpf_center);

	if (selrig->name_ == IC706MKIIGname_) {
		spref.set("use706filters", use706filters);
		spref.set("Set_IC706MKIIG_filter_names", "");
		spref.set("IC706MKIIG_filter_names_1", \
"EMPTY,NARR,NORM,WIDE,MED" );
		spref.set("IC706MKIIG_filter_names_2", \
"FL-101,FL-232,FL-100,FL-223,FL-103" );
		spref.set("ssb_cw_wide", ssb_cw_wide.c_str());
		spref.set("ssb_cw_normal", ssb_cw_normal.c_str());
		spref.set("ssb_cw_narrow", ssb_cw_narrow.c_str());
		selrig->initialize();
	}

	spref.set("label1", label1.c_str());
	spref.set("command1", command1.c_str());
	spref.set("shftcmd1", shftcmd1.c_str());

	spref.set("label2", label2.c_str());
	spref.set("command2", command2.c_str());
	spref.set("shftcmd2", shftcmd2.c_str());

	spref.set("label3", label3.c_str());
	spref.set("command3", command3.c_str());
	spref.set("shftcmd3", shftcmd3.c_str());

	spref.set("label4", label4.c_str());
	spref.set("command4", command4.c_str());
	spref.set("shftcmd4", shftcmd4.c_str());

	spref.set("label5", label5.c_str());
	spref.set("command5", command5.c_str());
	spref.set("shftcmd5", shftcmd5.c_str());

	spref.set("label6", label6.c_str());
	spref.set("command6", command6.c_str());
	spref.set("shftcmd6", shftcmd6.c_str());

	spref.set("label7", label7.c_str());
	spref.set("command7", command7.c_str());
	spref.set("shftcmd7", shftcmd7.c_str());

	spref.set("label8", label8.c_str());
	spref.set("command8", command8.c_str());
	spref.set("shftcmd8", shftcmd8.c_str());

	spref.set("label9", label9.c_str());
	spref.set("command9", command9.c_str());
	spref.set("shftcmd9", shftcmd9.c_str());

	spref.set("label10", label10.c_str());
	spref.set("command10", command10.c_str());
	spref.set("shftcmd10", shftcmd10.c_str());

	spref.set("label11", label11.c_str());
	spref.set("command11", command11.c_str());
	spref.set("shftcmd11", shftcmd11.c_str());

	spref.set("label12", label12.c_str());
	spref.set("command12", command12.c_str());
	spref.set("shftcmd12", shftcmd12.c_str());

	spref.set("label13", label13.c_str());
	spref.set("command13", command13.c_str());
	spref.set("shftcmd13", shftcmd13.c_str());

	spref.set("label14", label14.c_str());
	spref.set("command14", command14.c_str());
	spref.set("shftcmd14", shftcmd14.c_str());

	spref.set("label15", label15.c_str());
	spref.set("command15", command15.c_str());
	spref.set("shftcmd15", shftcmd15.c_str());

	spref.set("label16", label16.c_str());
	spref.set("command16", command16.c_str());
	spref.set("shftcmd16", shftcmd16.c_str());

	spref.set("label17", label17.c_str());
	spref.set("command17", command17.c_str());
	spref.set("shftcmd17", shftcmd17.c_str());

	spref.set("label18", label18.c_str());
	spref.set("command18", command18.c_str());
	spref.set("shftcmd18", shftcmd18.c_str());

	spref.set("label19", label19.c_str());
	spref.set("command19", command19.c_str());
	spref.set("shftcmd19", shftcmd19.c_str());

	spref.set("label20", label20.c_str());
	spref.set("command20", command20.c_str());
	spref.set("shftcmd20", shftcmd20.c_str());

	spref.set("label21", label21.c_str());
	spref.set("command21", command21.c_str());
	spref.set("shftcmd21", shftcmd21.c_str());

	spref.set("label22", label22.c_str());
	spref.set("command22", command22.c_str());
	spref.set("shftcmd22", shftcmd22.c_str());

	spref.set("label23", label23.c_str());
	spref.set("command23", command23.c_str());
	spref.set("shftcmd23", shftcmd23.c_str());

	spref.set("label24", label24.c_str());
	spref.set("command24", command24.c_str());
	spref.set("shftcmd24", shftcmd24.c_str());

	spref.set("st_label1", label_on_start1.c_str());
	spref.set("st_cmd1", cmd_on_start1.c_str());

	spref.set("st_label2", label_on_start2.c_str());
	spref.set("st_cmd2", cmd_on_start2.c_str());

	spref.set("st_label3", label_on_start3.c_str());
	spref.set("st_cmd3", cmd_on_start3.c_str());

	spref.set("st_label4", label_on_start4.c_str());
	spref.set("st_cmd4", cmd_on_start4.c_str());

	spref.set("ex_label1", label_on_exit1.c_str());
	spref.set("ex_cmd1", cmd_on_exit1.c_str());

	spref.set("ex_label2", label_on_exit2.c_str());
	spref.set("ex_cmd2", cmd_on_exit2.c_str());

	spref.set("ex_label3", label_on_exit3.c_str());
	spref.set("ex_cmd3", cmd_on_exit3.c_str());

	spref.set("ex_label4", label_on_exit4.c_str());
	spref.set("ex_cmd4", cmd_on_exit4.c_str());

	spref.set("fg_red", fg_red);
	spref.set("fg_green", fg_green);
	spref.set("fg_blue", fg_blue);

	spref.set("bg_red", bg_red);
	spref.set("bg_green", bg_green);
	spref.set("bg_blue", bg_blue);

	spref.set("smeter_red", smeterRed);
	spref.set("smeter_green", smeterGreen);
	spref.set("smeter_blue", smeterBlue);

	spref.set("power_red", pwrRed);
	spref.set("power_green", pwrGreen);
	spref.set("power_blue", pwrBlue);

	spref.set("swr_red", swrRed);
	spref.set("swr_green", swrGreen);
	spref.set("swr_blue", swrBlue);

	spref.set("peak_red", peakRed);
	spref.set("peak_green", peakGreen);
	spref.set("peak_blue", peakBlue);

	spref.set("volt_red", voltRed);
	spref.set("volt_green", voltGreen);
	spref.set("volt_blue", voltBlue);
	spref.set("display_voltmeter", display_voltmeter);

	spref.set("fg_sys_red", fg_sys_red);
	spref.set("fg_sys_green", fg_sys_green);
	spref.set("fg_sys_blue", fg_sys_blue);

	spref.set("bg_sys_red", bg_sys_red);
	spref.set("bg_sys_green", bg_sys_green);
	spref.set("bg_sys_blue", bg_sys_blue);

	spref.set("bg2_sys_red", bg2_sys_red);
	spref.set("bg2_sys_green", bg2_sys_green);
	spref.set("bg2_sys_blue", bg2_sys_blue);

	spref.set("slider_red", slider_red);
	spref.set("slider_green", slider_green);
	spref.set("slider_blue", slider_blue);

	spref.set("slider_btn_red", slider_btn_red);
	spref.set("slider_btn_green", slider_btn_green);
	spref.set("slider_btn_blue", slider_btn_blue);

	spref.set("lighted_btn_red", lighted_btn_red);
	spref.set("lighted_btn_green", lighted_btn_green);
	spref.set("lighted_btn_blue", lighted_btn_blue);

	spref.set("tab_red", tab_red);
	spref.set("tab_green", tab_green);
	spref.set("tab_blue", tab_blue);

	spref.set("fontnbr", fontnbr);

	spref.set("tooltips", tooltips);

	spref.set("ui_scheme", ui_scheme.c_str());

//	spref.set("server_port", server_port.c_str());
//	spref.set("server_addr", server_addr.c_str());

	spref.set("tcpip_port", tcpip_port.c_str());
	spref.set("tcpip_addr", tcpip_addr.c_str());
	spref.set("tcpip_ping_delay", tcpip_ping_delay);
	spref.set("tcpip_tcpip_reconnect_after", tcpip_reconnect_after);
	spref.set("tcpip_drops_allowed", tcpip_drops_allowed);
	spref.set("use_tcpip", use_tcpip);

	spref.set("tci_port", tci_port.c_str());
	spref.set("tci_addr", tci_addr.c_str());
	spref.set("tci_center", tci_center);

	spref.set("xcvr_auto_on", xcvr_auto_on);
	spref.set("xcvr_auto_off", xcvr_auto_off);

	spref.set("external_tuner", external_tuner);

	spref.set("trace", trace);
	spref.set("rigtrace", rigtrace);
	spref.set("gettrace", gettrace);
	spref.set("settrace", settrace);
	spref.set("debugtrace", debugtrace);
	spref.set("xmltrace", xmltrace);
	spref.set("rpctrace", rpctrace);
	spref.set("serialtrace", serialtrace);
	spref.set("startstoptrace", start_stop_trace);
	spref.set("tcitrace", tcitrace);

	spref.set("rpc_level", rpc_level);

	spref.set("f160", f160); spref.set("m160", m160);
	spref.set("txT160", txT_160); spref.set("rxT160", rxT_160);
	spref.set("offset_160", offset_160); spref.set("oF_160", oF_160);

	spref.set("f80", f80);   spref.set("m80", m80);
	spref.set("txT80", txT_80); spref.set("rxT80", rxT_80);
	spref.set("offset_80", offset_80); spref.set("oF_80", oF_80);

	spref.set("f40", f40);   spref.set("m40", m40);
	spref.set("txT40", txT_40); spref.set("rxT40", rxT_40);
	spref.set("offset_40", offset_40); spref.set("oF_40", oF_40);

	spref.set("f30", f30);   spref.set("m30", m30);
	spref.set("txT30", txT_30); spref.set("rxT30", rxT_30);
	spref.set("offset_30", offset_30); spref.set("oF_30", oF_30);

	spref.set("f20", f20);   spref.set("m20", m20);
	spref.set("txT20", txT_20); spref.set("rxT20", rxT_20);
	spref.set("offset_20", offset_20); spref.set("oF_20", oF_20);

	spref.set("f17", f17);   spref.set("m17", m17);
	spref.set("txT17", txT_17); spref.set("rxT17", rxT_17);
	spref.set("offset_17", offset_17); spref.set("oF_17", oF_17);

	spref.set("f15", f15);   spref.set("m15", m15);
	spref.set("txT15", txT_15); spref.set("rxT15", rxT_15);
	spref.set("offset_15", offset_15); spref.set("oF_15", oF_15);

	spref.set("f12", f12);   spref.set("m12", m12);
	spref.set("txT12", txT_12); spref.set("rxT12", rxT_12);
	spref.set("offset_12", offset_12); spref.set("oF_12", oF_12);

	spref.set("f10", f10);   spref.set("m10", m10);
	spref.set("txT10", txT_10); spref.set("rxT10", rxT_10);
	spref.set("offset_10", offset_10); spref.set("oF_10", oF_10);

	spref.set("f6", f6);     spref.set("m6", m6);
	spref.set("txT6", txT_6); spref.set("rxT_6", rxT_6);
	spref.set("offset_6", offset_6); spref.set("oF_6", oF_6);

	spref.set("f2", f2);     spref.set("m2", m2);
	spref.set("txT2", txT_2); spref.set("rxT2", rxT_2);
	spref.set("offset_2", offset_2); spref.set("oF_2", oF_2);

	spref.set("f70", f70);   spref.set("m70", m70);
	spref.set("txT70", txT_70); spref.set("rxT70", rxT_70);
	spref.set("offset_70", offset_70); spref.set("oF_70", oF_70);

	spref.set("f12G", f12G);   spref.set("m12G", m12G);
	spref.set("txT12G", txT_12G); spref.set("rxT12G", rxT_12G);
	spref.set("offset_12G", offset_12G); spref.set("oF_12G", oF_12G);

	spref.set("fgen", fgen);   spref.set("mgen", mgen);
	spref.set("txTgen", txT_gen); spref.set("rxTgen", rxT_gen);
	spref.set("offset_gen", offset_gen); spref.set("oF_gen", oF_gen);

	spref.set("hrd_buttons", hrd_buttons);
	spref.set("sliders_button", sliders_button);

	spref.set("memfontnbr", memfontnbr);
	spref.set("memfontsize", memfontsize);

	spref.set("gpio_ptt", gpio_ptt);
	spref.set("enable_gpio", enable_gpio);
	spref.set("gpio_on", gpio_on);
	spref.set("gpio_pulse_width", gpio_pulse_width);

//	spref.set("kxpa", kxpa);

	spref.set("cwioWPM", cwioWPM);
	spref.set("cwio_comp", cwio_comp);
	spref.set("cwio_keycorr", cwio_keycorr);
	spref.set("cwioKEYLINE", cwioKEYLINE);
	spref.set("cwioSHARED", cwioSHARED);
	spref.set("cwioPTT", cwioPTT);
	spref.set("cwioCONNECTED", cwioCONNECTED);
	spref.set("cwioINVERTED", cwioINVERTED);
	spref.set("cwioPORT", cwioPORT.c_str());

	char setbuff[20];
	for (int n = 0; n < 12; n++) {
		snprintf(setbuff, sizeof(setbuff), "cwiolabel[%d]", n);
		spref.set(setbuff, cwio_labels[n].c_str());
		snprintf(setbuff, sizeof(setbuff), "cwiomessage[%d]", n);
		spref.set(setbuff, cwio_msgs[n].c_str());
	}

	spref.set("BT", BT.c_str());
	spref.set("AA", AA.c_str());
	spref.set("AS", AS.c_str());
	spref.set("AR", AR.c_str());
	spref.set("KN", KN.c_str());
	spref.set("SK", SK.c_str());
	spref.set("INT", INT.c_str());
	spref.set("HM", HM.c_str());
	spref.set("VE", VE.c_str());

	spref.set("FSK_KEYLINE", FSK_KEYLINE);
	spref.set("FSK_SHARED", FSK_SHARED);
	spref.set("FSK_PTT", FSK_PTT);
	spref.set("FSK_CONNECTED", FSK_CONNECTED);
	spref.set("FSK_INVERTED", FSK_INVERTED);
	spref.set("FSK_STOPBITS", FSK_STOPBITS);
	spref.set("FSK_PORT", FSK_PORT.c_str());

	for (int n = 0; n < 12; n++) {
		snprintf(setbuff, sizeof(setbuff), "fskiolabel[%d]", n);
		spref.set(setbuff, FSK_labels[n].c_str());
		snprintf(setbuff, sizeof(setbuff), "fskiomessage[%d]", n);
		spref.set(setbuff, FSK_msgs[n].c_str());
	}

// CW logbook parameters
	spref.set("LOG_NAME", log_name.c_str());
	spref.set("CUT_NUMBERS", log_cut_numbers);
	spref.set("LEADING_ZEROS", log_leading_zeros);
	spref.set("DUPCHECK", log_dupcheck);
	spref.set("QSO_NBR", qso_nbr);

}

bool status::loadXcvrState(std::string xcvr)
{
	Fl_Preferences spref(RigHomeDir.c_str(), "w1hkj.com", xcvr.c_str());

	if (spref.entryExists("version")) {

		int i;
		char defbuffer[500];

		spref.get("mainx", mainX, mainX);
		spref.get("mainy", mainY, mainY);
		spref.get("mainw", mainW, mainW);
		spref.get("mainh", mainH, mainH);
		spref.get("uisize", UIsize, UIsize);

		if (UIsize == wide_ui) {
			if (mainW < WIDE_MAINW)
				mainW = WIDE_MAINW;
			mainH = WIDE_MAINH;
		}

		spref.get("memx", memX, memX);
		spref.get("memy", memY, memY);
		spref.get("memw", memW, memW);
		spref.get("memh", memH, memH);

		spref.get("ddX", ddX, ddX);
		spref.get("ddY", ddY, ddY);

		if (current_ui_size != -1) {
			UIsize = current_ui_size;
		}
		current_ui_size = UIsize;

		if (UIsize == small_ui) {
			mainW = SMALL_MAINW;
		}
		if (UIsize == touch_ui) {
			if (mainW < TOUCH_MAINW) mainW = TOUCH_MAINW;
		}

		spref.get("xcvr_serial_port", defbuffer, "NONE", 499);
		xcvr_serial_port = defbuffer;
		if (xcvr_serial_port.find("tty") == 0)
			xcvr_serial_port.insert(0, "/dev/");

		i = 0;
		selrig = rigs[i];
		while (rigs[i] != NULL) {
			if (xcvr == rigs[i]->name_) {
				selrig = rigs[i];
				break;
			}
			i++;
		}

		spref.get("comm_baudrate", comm_baudrate, comm_baudrate);
		spref.get("comm_stopbits", stopbits, stopbits);
		spref.get("comm_retries", comm_retries, comm_retries);
		spref.get("comm_wait", comm_wait, comm_wait);
		spref.get("comm_timeout", comm_timeout, comm_timeout);
		spref.get("serloop_timing", serloop_timing, serloop_timing);
		if (serloop_timing < 10) serloop_timing = 10; // minimum loop delay of 10 msec
		spref.get("byte_interval", byte_interval, byte_interval);

		if (spref.get("comm_echo", i, i)) comm_echo = i;
		if (spref.get("ptt_via_cat", i, i)) comm_catptt = i;
		if (spref.get("ptt_via_rts", i, i)) comm_rtsptt = i;
		if (spref.get("ptt_via_dtr", i, i)) comm_dtrptt = i;
		if (spref.get("rts_cts_flow", i, i)) comm_rtscts = i;
		if (spref.get("rts_plus", i, i)) comm_rtsplus = i;
		if (spref.get("dtr_plus", i, i)) comm_dtrplus = i;

		if (spref.get("disable_CW_ptt", i, i)) disable_CW_ptt = i;

		spref.get("civadr", CIV, CIV);
		if (spref.get("usbaudio", i, i)) USBaudio = i;

		spref.get("aux_serial_port", defbuffer, "NONE", 499);
		aux_serial_port = defbuffer;

		if (spref.get("aux_rts", i, i)) aux_rts = i;
		if (spref.get("aux_dtr", i, i)) aux_dtr = i;

		spref.get("sep_serial_port", defbuffer, "NONE", 499);
		sep_serial_port = defbuffer;

		if (spref.get("sep_rtsptt", i, i)) sep_rtsptt = i;
		if (spref.get("sep_dtrptt", i, i)) sep_dtrptt = i;
		if (spref.get("sep_rtsplus", i, i)) sep_rtsplus = i;
		if (spref.get("sep_dtrplus", i, i)) sep_dtrplus = i;

		spref.get("xmlport", defbuffer, "12345", 499);
		xmlport = defbuffer;
		::xmlport = atoi(xmlport.c_str());

		spref.get("cmedia_device", defbuffer, "NONE", 499);
		cmedia_device = defbuffer;
		spref.get("cmedia_gpio_line", defbuffer, "GPIO-3", 499);
		cmedia_gpio_line = defbuffer;
		spref.get("cmedia_ptt", i, cmedia_ptt); cmedia_ptt = i;

		spref.get("tmate2_device", defbuffer, "NONE", 499);
		tmate2_device = defbuffer;
		spref.get("tmate2_freq_step", defbuffer, "5000", 499);
		tmate2_freq_step = defbuffer;
		spref.get("tmate2_connected", i, tmate2_connected); tmate2_connected = i;

		spref.get("poll_smeter", poll_smeter, poll_smeter);
		spref.get("poll_frequency", poll_frequency, poll_frequency);
		spref.get("poll_mode", poll_mode, poll_mode);
		spref.get("poll_bandwidth", poll_bandwidth, poll_bandwidth);
		spref.get("poll_volume", poll_volume, poll_volume);
		spref.get("poll_auto_notch", poll_auto_notch, poll_auto_notch);
		spref.get("poll_notch", poll_notch, poll_notch);
		spref.get("poll_ifshift", poll_ifshift, poll_ifshift);
		spref.get("poll_power_control", poll_power_control, poll_power_control);
		spref.get("poll_pre_att", poll_pre_att, poll_pre_att);
		spref.get("poll_micgain", poll_micgain, poll_micgain);
		spref.get("poll_squelch", poll_squelch, poll_squelch);
		spref.get("poll_rfgain", poll_rfgain, poll_rfgain);
		spref.get("poll_pout", poll_pout, poll_pout);
		spref.get("poll_swr", poll_swr, poll_swr);
		spref.get("poll_alc", poll_alc, poll_alc);
		spref.get("poll_split", poll_split, poll_split);
		spref.get("poll_noise", poll_noise, poll_noise);
		spref.get("poll_nr", poll_nr, poll_nr);
		spref.get("poll_compression", poll_compression, poll_compression);
		spref.get("poll_tuner", poll_tuner, poll_tuner);
		spref.get("poll_ptt", poll_ptt, poll_ptt);
		spref.get("poll_break_in", poll_break_in, poll_break_in);
		spref.get("poll_all", poll_all, poll_all);

		spref.get("bw_A", iBW_A, iBW_A);
		spref.get("mode_A", imode_A, imode_A);

		int freq_A_upper = 0;
		int freq_A_lower = 14070000;
		spref.get("freq_A_u", freq_A_upper, freq_A_upper);
		spref.get("freq_A_l", freq_A_lower, freq_A_lower);
		freq_A = (unsigned long int)freq_A_upper * 1000000000 + freq_A_lower;

		spref.get("bw_B", iBW_B, iBW_B);
		spref.get("mode_B", imode_B, imode_B);

		int freq_B_upper = 0;
		int freq_B_lower = 14070000;
		spref.get("freq_B_u", freq_B_upper, freq_B_upper);
		spref.get("freq_B_l", freq_B_lower, freq_B_lower);
		freq_B = (unsigned long int)freq_B_upper * 1000000000 + freq_B_lower;

		spref.get("filters", defbuffer, "", 499);
		filters = defbuffer;

		spref.get("bandwidths", defbuffer, "", 499);
		bandwidths = defbuffer;

		if (spref.get("use_rig_data", i, i)) use_rig_data = i;
//		if (spref.get("restore_rig_data", i, i)) restore_rig_data = i;

		if (spref.get("restore_frequency", i, i)) restore_frequency = i;
		if (spref.get("restore_mode", i, i)) restore_mode = i;
		if (spref.get("restore_bandwidth", i, i)) restore_bandwidth = i;
		if (spref.get("restore_volume", i, i)) restore_volume = i;
		if (spref.get("restore_mic_gain", i, i)) restore_mic_gain = i;
		if (spref.get("restore_rf_gain", i, i)) restore_rf_gain = i;
		if (spref.get("restore_power_control", i, i)) restore_power_control = i;
		if (spref.get("restore_if_shift", i, i)) restore_if_shift = i;
		if (spref.get("restore_notch", i, i)) restore_notch = i;
		if (spref.get("restore_auto_notch", i, i)) restore_auto_notch = i;
		if (spref.get("restore_noise", i, i)) restore_noise = i;
		if (spref.get("restore_squelch", i, i)) restore_squelch = i;
		if (spref.get("restore_split", i, i)) restore_split = i;
		if (spref.get("restore_pre_att", i, i)) restore_pre_att = i;
		if (spref.get("restore_nr", i, i)) restore_nr = i;
		if (spref.get("restore_comp_on_off", i, i)) restore_comp_on_off = i;
		if (spref.get("restore_comp_level", i, i)) restore_comp_level = i;

		if (spref.get("bool_spkr_on", i, i)) spkr_on = i;
		spref.get("int_volume", volume, volume);
		spref.get("dbl_power", power_level, power_level);
		spref.get("power_limit", power_limit, power_limit);
		if (spref.get("enable_power_limit", i, i)) enable_power_limit = i;
		spref.get("int_mic", mic_gain, mic_gain);
		if (spref.get("bool_notch", i, i)) notch = i;
		spref.get("int_notch", notch_val, notch_val);
		if (spref.get("bool_shift", i, i)) shift = i;
		spref.get("int_shift", shift_val, shift_val);

		if (spref.get("pbt_lock", i, pbt_lock)) pbt_lock = i;
		if (spref.get("pbt_inner", i, pbt_inner)) pbt_inner = i;
		if (spref.get("pbt_outer", i, pbt_outer)) pbt_outer = i;

		spref.get("rfgain", rfgain, rfgain);
		spref.get("squelch", squelch, squelch);
		spref.get("no_txqsy", no_txqsy, no_txqsy);

		spref.get("schema", schema, schema);
		spref.get("embed_tabs", embed_tabs, embed_tabs);
		spref.get("show_tabs", show_tabs, show_tabs);

		spref.get("visible_tab", defbuffer, visible_tab.c_str(), 499);
		visible_tab = defbuffer;

		spref.get("rx_avg", rx_avg, rx_avg);
		spref.get("rx_peak", rx_peak, rx_peak);
		spref.get("pwr_avg", pwr_avg, pwr_avg);
		spref.get("pwr_peak", pwr_peak, pwr_peak);
		spref.get("pwr_scale", pwr_scale, pwr_scale);

		if (spref.get("sync_clock", i, sync_clock)) sync_clock = i;
		if (spref.get("sync_gmt", i, sync_gmt)) sync_gmt = i;

		if (spref.get("digi_sel_on_off", i, i)) digi_sel_on_off = i;
		spref.get("digi_sel_val", digi_sel_val, digi_sel_val);

		if (spref.get("dual_watch", i, i)) dual_watch = i;
		spref.get("ic7610att", index_ic7610att, index_ic7610att);

		if (spref.get("ft950_rg_reverse", i, i)) ft950_rg_reverse = i;

		if (selrig->name_ == rig_TT550.name_) {
			spref.get("tt550_line_out", tt550_line_out, tt550_line_out);
			spref.get("tt550_agc_level", tt550_agc_level, tt550_agc_level);

			spref.get("tt550_cw_wpm", tt550_cw_wpm, tt550_cw_wpm);
			spref.get("tt550_cw_weight", tt550_cw_weight, tt550_cw_weight);
			spref.get("tt550_cw_vol", tt550_cw_vol, tt550_cw_vol);
			spref.get("tt550_cw_spot", tt550_cw_spot, tt550_cw_spot);
			if (spref.get("tt550_spot_onoff", i, i)) tt550_spot_onoff = i;
			spref.get("tt550_cw_qsk", tt550_cw_qsk, tt550_cw_qsk);
			if (spref.get("tt550_enable_keyer", i, i)) tt550_enable_keyer = i;
			if (spref.get("break_in", i, i)) break_in = i;
			if (spref.get("vox_onoff", i, i)) vox_onoff = i;

			if (spref.get("tt550_vox_onoff", i, i)) tt550_vox_onoff = i;
			spref.get("tt550_vox_gain", tt550_vox_gain, tt550_vox_gain);
			spref.get("tt550_vox_anti", tt550_vox_anti, tt550_vox_anti);
			spref.get("tt550_vox_hang", tt550_vox_hang, tt550_vox_hang);

			spref.get("tt550_mon_vol", tt550_mon_vol, tt550_mon_vol);
			spref.get("tt550_squelch_level", tt550_squelch_level, tt550_squelch_level);
			spref.get("tt550_compression", tt550_compression, tt550_compression);
			spref.get("tt550_nb_level", tt550_nb_level, tt550_nb_level);
			if (spref.get("tt550_compON", i, i)) tt550_compON = i;
			if (spref.get("tt550_tuner_bypass", i, i)) tt550_tuner_bypass = i;
			if (spref.get("tt550_enable_xmtr", i, i)) tt550_enable_xmtr = i;
			if (spref.get("tt550_enable_tloop", i, i)) tt550_enable_tloop = i;

			spref.get("tt550_xmt_bw", tt550_xmt_bw, tt550_xmt_bw);
			if (spref.get("tt550_use_xmt_bw", i, i)) tt550_use_xmt_bw = i;

			if (spref.get("tt550_use_line_in", i, i)) tt550_use_line_in = i;

			spref.get("tt550_AM_level", tt550_AM_level, tt550_AM_level);
			spref.get("tt550_encoder_step", tt550_encoder_step, tt550_encoder_step);
			spref.get("tt550_encoder_sensitivity", tt550_encoder_sensitivity, tt550_encoder_sensitivity);
			spref.get("tt550_keypad_timeout", tt550_keypad_timeout, tt550_keypad_timeout);

			spref.get("tt550_F1_func", tt550_F1_func, tt550_F1_func);
			spref.get("tt550_F2_func", tt550_F2_func, tt550_F2_func);
			spref.get("tt550_F3_func", tt550_F3_func, tt550_F3_func);

			spref.get("tt550_Nsamples", tt550_Nsamples, tt550_Nsamples);
			spref.get("tt550_at11_inline", i, tt550_at11_inline); tt550_at11_inline = i;
			spref.get("tt550_at11_hiZ", i, tt550_at11_hiZ); tt550_at11_hiZ = i;

		}
		else {
			spref.get("line_out", line_out, line_out);
			spref.get("data_port", i, data_port); data_port = i;
			spref.get("vox_on_dataport", i, vox_on_dataport); vox_on_dataport = i;
			spref.get("agc_level", agc_level, agc_level);
			spref.get("cw_wpm", cw_wpm, cw_wpm);
			spref.get("cw_weight", cw_weight, cw_weight);
			spref.get("cw_vol", cw_vol, cw_vol);
			spref.get("cw_spot", cw_spot, cw_spot);
			if (spref.get("spot_onoff", i, i)) spot_onoff = i;
			spref.get("cw_spot_tone", cw_spot_tone, cw_spot_tone);
			spref.get("cw_qsk", cw_qsk, cw_qsk);
			spref.get("(cw_delay", cw_delay, cw_delay);
			if (spref.get("enable_keyer", i, i)) enable_keyer = i;
			if (spref.get("break_in", break_in, break_in));
			if (spref.get("vox_onoff", i, i)) vox_onoff = i;
			spref.get("vox_gain", vox_gain, vox_gain);
			spref.get("vox_anti", vox_anti, vox_anti);
			spref.get("vox_hang", vox_hang, vox_hang);
			spref.get("compression", compression, compression);
			if (spref.get("compON", i, i)) compON = i;
		}

		if (spref.get("noise_reduction", i, i)) noise_reduction = i;
		spref.get("noise_red_val", noise_reduction_val, noise_reduction_val);

		if (spref.get("bool_noise", i, i)) noise = i;
		spref.get("nb_level", nb_level, nb_level);

		spref.get("int_preamp", preamp, preamp);
		spref.get("int_att", attenuator, attenuator);

		spref.get("compression", compression, compression);
		if (spref.get("compON", i, i)) compON = i;

		spref.get("vfo_adj", vfo_adj, vfo_adj);
		spref.get("bfo_freq", bfo_freq, bfo_freq);
		spref.get("rit_freq", rit_freq, rit_freq);
		spref.get("xit_freq", xit_freq, xit_freq);
		spref.get("bpf_center", bpf_center, bpf_center);
		spref.get("use_bpf_center", i, i); use_bpf_center = i;

		if (spref.get("use706filters", i, i)) use706filters = i;
		spref.get("ssb_cw_wide", defbuffer, ssb_cw_wide.c_str(), 499);
		ssb_cw_wide = defbuffer;
		spref.get("ssb_cw_normal", defbuffer, ssb_cw_normal.c_str(), 499);
		ssb_cw_normal = defbuffer;
		spref.get("ssb_cw_narrow", defbuffer, ssb_cw_narrow.c_str(), 499);
		ssb_cw_narrow = defbuffer;

		spref.get("label1", defbuffer,  label1.c_str(), 499);
		label1 = defbuffer;
		spref.get("command1", defbuffer, command1.c_str(), 499);
		command1 = defbuffer;
		spref.get("shftcmd1", defbuffer, shftcmd1.c_str(), 499);
		shftcmd1 = defbuffer;

		spref.get("label2", defbuffer, label2.c_str(), 499);
		label2 = defbuffer;
		spref.get("command2", defbuffer, command2.c_str(), 499);
		command2 = defbuffer;
		spref.get("shftcmd2", defbuffer, shftcmd2.c_str(), 499);
		shftcmd2 = defbuffer;

		spref.get("label3", defbuffer, label3.c_str(), 499);
		label3 = defbuffer;
		spref.get("command3", defbuffer, command3.c_str(), 499);
		command3 = defbuffer;
		spref.get("shftcmd3", defbuffer, shftcmd3.c_str(), 499);
		shftcmd3 = defbuffer;

		spref.get("label4", defbuffer, label4.c_str(), 499);
		label4 = defbuffer;
		spref.get("command4", defbuffer, command4.c_str(), 499);
		command4 = defbuffer;
		spref.get("shftcmd4", defbuffer, shftcmd4.c_str(), 499);
		shftcmd4 = defbuffer;

		spref.get("label5", defbuffer, label5.c_str(), 499);
		label5 = defbuffer;
		spref.get("command5", defbuffer, command5.c_str(), 499);
		command5 = defbuffer;
		spref.get("shftcmd5", defbuffer, shftcmd5.c_str(), 499);
		shftcmd5 = defbuffer;

		spref.get("label6", defbuffer, label6.c_str(), 499);
		label6 = defbuffer;
		spref.get("command6", defbuffer, command6.c_str(), 499);
		command6 = defbuffer;
		spref.get("shftcmd6", defbuffer, shftcmd6.c_str(), 499);
		shftcmd6 = defbuffer;

		spref.get("label7", defbuffer, label7.c_str(), 499);
		label7 = defbuffer;
		spref.get("command7", defbuffer, command7.c_str(), 499);
		command7 = defbuffer;
		spref.get("shftcmd7", defbuffer, shftcmd7.c_str(), 499);
		shftcmd7 = defbuffer;

		spref.get("label8", defbuffer, label8.c_str(), 499);
		label8 = defbuffer;
		spref.get("command8", defbuffer, command8.c_str(), 499);
		command8 = defbuffer;
		spref.get("shftcmd8", defbuffer, shftcmd8.c_str(), 499);
		shftcmd8 = defbuffer;

		spref.get("label9", defbuffer,  label9.c_str(), 499);
		label9 = defbuffer;
		spref.get("command9", defbuffer, command9.c_str(), 499);
		command9 = defbuffer;
		spref.get("shftcmd9", defbuffer, shftcmd9.c_str(), 499);
		shftcmd9 = defbuffer;

		spref.get("label10", defbuffer, label10.c_str(), 499);
		label10 = defbuffer;
		spref.get("command10", defbuffer, command10.c_str(), 499);
		command10 = defbuffer;
		spref.get("shftcmd10", defbuffer, shftcmd10.c_str(), 499);
		shftcmd10 = defbuffer;

		spref.get("label11", defbuffer, label11.c_str(), 499);
		label11 = defbuffer;
		spref.get("command11", defbuffer, command11.c_str(), 499);
		command11 = defbuffer;
		spref.get("shftcmd11", defbuffer, shftcmd11.c_str(), 499);
		shftcmd11 = defbuffer;

		spref.get("label12", defbuffer, label12.c_str(), 499);
		label12 = defbuffer;
		spref.get("command12", defbuffer, command12.c_str(), 499);
		command12 = defbuffer;
		spref.get("shftcmd12", defbuffer, shftcmd12.c_str(), 499);
		shftcmd12 = defbuffer;

		spref.get("label13", defbuffer, label13.c_str(), 499);
		label13 = defbuffer;
		spref.get("command13", defbuffer, command13.c_str(), 499);
		command13 = defbuffer;
		spref.get("shftcmd13", defbuffer, shftcmd13.c_str(), 499);
		shftcmd13 = defbuffer;

		spref.get("label14", defbuffer, label14.c_str(), 499);
		label14 = defbuffer;
		spref.get("command14", defbuffer, command14.c_str(), 499);
		command14 = defbuffer;
		spref.get("shftcmd14", defbuffer, shftcmd14.c_str(), 499);
		shftcmd14 = defbuffer;

		spref.get("label15", defbuffer, label15.c_str(), 499);
		label15 = defbuffer;
		spref.get("command15", defbuffer, command15.c_str(), 499);
		command15 = defbuffer;
		spref.get("shftcmd15", defbuffer, shftcmd15.c_str(), 499);
		shftcmd15 = defbuffer;

		spref.get("label16", defbuffer, label16.c_str(), 499);
		label16 = defbuffer;
		spref.get("command16", defbuffer, command16.c_str(), 499);
		command16 = defbuffer;
		spref.get("shftcmd16", defbuffer, shftcmd16.c_str(), 499);
		shftcmd16 = defbuffer;

		spref.get("label17", defbuffer,  label17.c_str(), 499);
		label17 = defbuffer;
		spref.get("command17", defbuffer, command17.c_str(), 499);
		command17 = defbuffer;
		spref.get("shftcmd17", defbuffer, shftcmd17.c_str(), 499);
		shftcmd17 = defbuffer;

		spref.get("label18", defbuffer, label18.c_str(), 499);
		label18 = defbuffer;
		spref.get("command18", defbuffer, command18.c_str(), 499);
		command18 = defbuffer;
		spref.get("shftcmd18", defbuffer, shftcmd18.c_str(), 499);
		shftcmd18 = defbuffer;

		spref.get("label19", defbuffer, label19.c_str(), 499);
		label19 = defbuffer;
		spref.get("command19", defbuffer, command19.c_str(), 499);
		command19 = defbuffer;
		spref.get("shftcmd19", defbuffer, shftcmd11.c_str(), 499);
		shftcmd19 = defbuffer;

		spref.get("label20", defbuffer, label20.c_str(), 499);
		label20 = defbuffer;
		spref.get("command20", defbuffer, command20.c_str(), 499);
		command20 = defbuffer;
		spref.get("shftcmd20", defbuffer, shftcmd20.c_str(), 499);
		shftcmd20 = defbuffer;

		spref.get("label21", defbuffer, label21.c_str(), 499);
		label21 = defbuffer;
		spref.get("command21", defbuffer, command21.c_str(), 499);
		command21 = defbuffer;
		spref.get("shftcmd21", defbuffer, shftcmd21.c_str(), 499);
		shftcmd21 = defbuffer;

		spref.get("label22", defbuffer, label22.c_str(), 499);
		label22 = defbuffer;
		spref.get("command22", defbuffer, command22.c_str(), 499);
		command22 = defbuffer;
		spref.get("shftcmd22", defbuffer, shftcmd22.c_str(), 499);
		shftcmd22 = defbuffer;

		spref.get("label23", defbuffer, label23.c_str(), 499);
		label23 = defbuffer;
		spref.get("command23", defbuffer, command23.c_str(), 499);
		command23 = defbuffer;
		spref.get("shftcmd23", defbuffer, shftcmd23.c_str(), 499);
		shftcmd23 = defbuffer;

		spref.get("label24", defbuffer, label24.c_str(), 499);
		label24 = defbuffer;
		spref.get("command24", defbuffer, command24.c_str(), 499);
		command24 = defbuffer;
		spref.get("shftcmd24", defbuffer, shftcmd24.c_str(), 499);
		shftcmd24 = defbuffer;

		spref.get("st_label1", defbuffer, label_on_start1.c_str(), 499);
		label_on_start1 = defbuffer;
		spref.get("st_cmd1", defbuffer, cmd_on_start1.c_str(), 499);
		cmd_on_start1 = defbuffer;

		spref.get("st_label2", defbuffer, label_on_start2.c_str(), 499);
		label_on_start2 = defbuffer;
		spref.get("st_cmd2", defbuffer, cmd_on_start2.c_str(), 499);
		cmd_on_start2 = defbuffer;

		spref.get("st_label3", defbuffer, label_on_start3.c_str(), 499);
		label_on_start3 = defbuffer;
		spref.get("st_cmd3", defbuffer, cmd_on_start3.c_str(), 499);
		cmd_on_start3 = defbuffer;

		spref.get("st_label4", defbuffer, label_on_start4.c_str(), 499);
		label_on_start4 = defbuffer;
		spref.get("st_cmd4", defbuffer, cmd_on_start4.c_str(), 499);
		cmd_on_start4 = defbuffer;

		spref.get("ex_label1", defbuffer, label_on_exit1.c_str(), 499);
		label_on_exit1 = defbuffer;
		spref.get("ex_cmd1", defbuffer, cmd_on_exit1.c_str(), 499);
		cmd_on_exit1 = defbuffer;

		spref.get("ex_label2", defbuffer, label_on_exit2.c_str(), 499);
		label_on_exit2 = defbuffer;
		spref.get("ex_cmd2", defbuffer, cmd_on_exit2.c_str(), 499);
		cmd_on_exit2 = defbuffer;

		spref.get("ex_label3", defbuffer, label_on_exit3.c_str(), 499);
		label_on_exit3 = defbuffer;
		spref.get("ex_cmd3", defbuffer, cmd_on_exit3.c_str(), 499);
		cmd_on_exit3 = defbuffer;

		spref.get("ex_label4", defbuffer, label_on_exit4.c_str(), 499);
		label_on_exit4 = defbuffer;
		spref.get("ex_cmd4", defbuffer, cmd_on_exit4.c_str(), 499);
		cmd_on_exit4 = defbuffer;

		spref.get("fg_red", fg_red, fg_red);
		spref.get("fg_green", fg_green, fg_green);
		spref.get("fg_blue", fg_blue, fg_blue);

		spref.get("bg_red", bg_red, bg_red);
		spref.get("bg_green", bg_green, bg_green);
		spref.get("bg_blue", bg_blue, bg_blue);

		spref.get("smeter_red", smeterRed, smeterRed);
		spref.get("smeter_green", smeterGreen, smeterGreen);
		spref.get("smeter_blue", smeterBlue, smeterBlue);

		spref.get("power_red", pwrRed, pwrRed);
		spref.get("power_green", pwrGreen, pwrGreen);
		spref.get("power_blue", pwrBlue, pwrBlue);

		spref.get("swr_red", swrRed, swrRed);
		spref.get("swr_green", swrGreen, swrGreen);
		spref.get("swr_blue", swrBlue, swrBlue);

		spref.get("peak_red", peakRed, peakRed);
		spref.get("peak_green", peakGreen, peakGreen);
		spref.get("peak_blue", peakBlue, peakBlue);

		spref.get("volt_red", voltRed, voltRed);
		spref.get("volt_green", voltGreen, voltGreen);
		spref.get("volt_blue", voltBlue, voltBlue);
		spref.get("display_voltmeter", display_voltmeter, display_voltmeter);

		spref.get("fg_sys_red", fg_sys_red, fg_sys_red);
		spref.get("fg_sys_green", fg_sys_green, fg_sys_green);
		spref.get("fg_sys_blue", fg_sys_blue, fg_sys_blue);

		spref.get("bg_sys_red", bg_sys_red, bg_sys_red);
		spref.get("bg_sys_green", bg_sys_green, bg_sys_green);
		spref.get("bg_sys_blue", bg_sys_blue, bg_sys_blue);

		spref.get("bg2_sys_red", bg2_sys_red, bg2_sys_red);
		spref.get("bg2_sys_green", bg2_sys_green, bg2_sys_green);
		spref.get("bg2_sys_blue", bg2_sys_blue, bg2_sys_blue);

		spref.get("slider_red", slider_red, slider_red);
		spref.get("slider_green", slider_green, slider_green);
		spref.get("slider_blue", slider_blue, slider_blue);

		spref.get("slider_btn_red", slider_btn_red, slider_btn_red);
		spref.get("slider_btn_green", slider_btn_green, slider_btn_green);
		spref.get("slider_btn_blue", slider_btn_blue, slider_btn_blue);

		spref.get("lighted_btn_red", lighted_btn_red, lighted_btn_red);
		spref.get("lighted_btn_green", lighted_btn_green, lighted_btn_green);
		spref.get("lighted_btn_blue", lighted_btn_blue, lighted_btn_blue);

		spref.get("tab_red", tab_red, tab_red);
		spref.get("tab_green", tab_green, tab_green);
		spref.get("tab_blue", tab_blue, tab_blue);

		i = (int)fontnbr;
		spref.get("fontnbr", i, i); fontnbr = (Fl_Font)i;
		i = 0;
		if (spref.get("tooltips", i, i)) tooltips = i;

		spref.get("ui_scheme", defbuffer, "gtk+", 499);
		ui_scheme = defbuffer;

//		spref.get("server_port", defbuffer, "7362", 499);
//		server_port = defbuffer;
//		spref.get("server_addr", defbuffer, "127.0.0.1", 499);
//		server_addr = defbuffer;

		spref.get("tcpip_port", defbuffer, "4001", 499);
		tcpip_port = defbuffer;
		spref.get("tcpip_addr", defbuffer, "127.0.0.1", 499);
		tcpip_addr = defbuffer;
		spref.get("tcpip_ping_delay", tcpip_ping_delay, tcpip_ping_delay);
		spref.get("tcpip_tcpip_reconnect_after", tcpip_reconnect_after, tcpip_reconnect_after);
		spref.get("tcpip_drops_allowed", tcpip_drops_allowed, tcpip_drops_allowed);
		if (spref.get("use_tcpip", i, i)) use_tcpip = i;

		spref.get("tci_port", defbuffer, "40001", 499);
		tci_port = defbuffer;
		spref.get("tci_addr", defbuffer, "127.0.0.1", 499);
		tci_addr = defbuffer;
		spref.get("tci_center", tci_center, tci_center);

		if (spref.get("xcvr_auto_on", i, i)) xcvr_auto_on = i;
		if (spref.get("xcvr_auto_off", i, i)) xcvr_auto_off = i;

		if (spref.get("external_tuner", i,i)) external_tuner = i;

		if (spref.get("trace", i, trace)) trace = i;
		if (spref.get("rigtrace", i, rigtrace)) rigtrace = i;
		if (spref.get("gettrace", i, gettrace)) gettrace = i;
		if (spref.get("settrace", i, settrace)) settrace = i;
		if (spref.get("debugtrace", i, debugtrace)) debugtrace = i;
		if (spref.get("xmltrace", i, xmltrace)) xmltrace = i;
		if (spref.get("serialtrace", i, serialtrace)) serialtrace = i;
		if (spref.get("startstoptrace", i, start_stop_trace)) start_stop_trace = i;
		if (spref.get("rpctrace", i, rpctrace)) rpctrace = i;
		if (spref.get("tcitrace", i, tcitrace)) tcitrace = i;

#ifndef NDEBUG
		trace =
		rigtrace =
		settrace =
		debugtrace =
		xmltrace =
		rpctrace =
		serialtrace =
		gettrace = 
		tcitrace = true;
#endif

		spref.get("rpc_level", rpc_level, rpc_level);

		spref.get("f160", f160, f160); spref.get("m160", m160, m160);
		spref.get("txT160", txT_160, txT_160); spref.get("rxT160", rxT_160, rxT_160);
		spref.get("offset_160", offset_160, offset_160); spref.get("oF_160", oF_160, oF_160);

		spref.get("f80", f80, f80);    spref.get("m80", m80, m80);
		spref.get("txT80", txT_80, txT_80); spref.get("rxT80", rxT_80, rxT_80);
		spref.get("offset_80", offset_80, offset_80); spref.get("oF_80", oF_80, oF_80);

		spref.get("f40", f40, f40);    spref.get("m40", m40, m40);
		spref.get("txT40", txT_40, txT_40); spref.get("rxT40", rxT_40, rxT_40);
		spref.get("offset_40", offset_40, offset_40); spref.get("oF_40", oF_40, oF_40);

		spref.get("f30", f30, f30);    spref.get("m30", m30, m30);
		spref.get("txT30", txT_30, txT_30); spref.get("rxT30", rxT_30, rxT_30);
		spref.get("offset_30", offset_30, offset_30); spref.get("oF_30", oF_30, oF_30);

		spref.get("f20", f20, f20);    spref.get("m20", m20, m20);
		spref.get("txT20", txT_20, txT_20); spref.get("rxT20", rxT_20, rxT_20);
		spref.get("offset_20", offset_20, offset_20); spref.get("oF_20", oF_20, oF_20);

		spref.get("f17", f17, f17);    spref.get("m17", m17, m17);
		spref.get("txT17", txT_17, txT_17); spref.get("rxT17", rxT_17, rxT_17);
		spref.get("offset_17", offset_17, offset_17); spref.get("oF_17", oF_17, oF_17);

		spref.get("f15", f15, f15);    spref.get("m15", m15, m15);
		spref.get("txT15", txT_15, txT_15); spref.get("rxT15", rxT_15, rxT_15);
		spref.get("offset_15", offset_15, offset_15); spref.get("oF_15", oF_15, oF_15);

		spref.get("f12", f12, f12);    spref.get("m12", m12, m12);
		spref.get("txT12", txT_12, txT_12); spref.get("rxT12", rxT_12, rxT_12);
		spref.get("offset_12", offset_12, offset_12); spref.get("oF_12", oF_12, oF_12);

		spref.get("f10", f10, f10);    spref.get("m10", m10, m10);
		spref.get("txT10", txT_10, txT_10); spref.get("rxT10", rxT_10, rxT_10);
		spref.get("offset_10", offset_10, offset_10); spref.get("oF_10", oF_10, oF_10);

		spref.get("f6", f6, f6);       spref.get("m6", m6, m6);
		spref.get("txT6", txT_6, txT_6); spref.get("rxT6", rxT_6, rxT_6);
		spref.get("offset_6", offset_6, offset_6); spref.get("oF_6", oF_6, oF_6);

		spref.get("f2", f2, f2);       spref.get("m2", m2, m2);
		spref.get("txT2", txT_2, txT_2); spref.get("rxT2", rxT_2, rxT_2);
		spref.get("offset_2", offset_2, offset_2); spref.get("oF_2", oF_2, oF_2);

		spref.get("f70", f70, f70);    spref.get("m70", m70, m70);
		spref.get("txT70", txT_70, txT_70); spref.get("rxT70", rxT_70, rxT_70);
		spref.get("offset_70", offset_70, offset_70); spref.get("oF_70", oF_70, oF_70);

		spref.get("f12G", f12G, f12G);    spref.get("m12G", m12G, m12G);
		spref.get("txT12G", txT_12G, txT_12G); spref.get("rxT12G", rxT_12G, rxT_12G);
		spref.get("offset_12G", offset_12G, offset_12G); spref.get("oF_12G", oF_12G, oF_12G);

		spref.get("fgen", fgen, fgen);    spref.get("mgen", mgen, mgen);
		spref.get("txTgen", txT_gen, txT_gen); spref.get("rxTgen", rxT_gen, rxT_gen);
		spref.get("offset_gen", offset_gen, offset_gen); spref.get("oF_gen", oF_gen, oF_gen);

		if (spref.get("hrd_buttons", i, i)) hrd_buttons = i;
		spref.get("sliders_button", sliders_button, sliders_button);

		spref.get("memfontnbr", memfontnbr, memfontnbr);
		spref.get("memfontsize", memfontsize, memfontsize);

		if (spref.get("gpio_ptt", i, i)) gpio_ptt = i;
		spref.get("enable_gpio", enable_gpio, enable_gpio);
		spref.get("gpio_on", gpio_on, gpio_on);
		spref.get("gpio_pulse_width", gpio_pulse_width, gpio_pulse_width);

//		spref.get("kxpa", kxpa, kxpa);

		spref.get("cwioWPM", cwioWPM, cwioWPM);
		spref.get("cwio_comp", cwio_comp, cwio_comp);
		spref.get("cwio_keycorr", cwio_keycorr, cwio_keycorr);
		spref.get("cwioKEYLINE", cwioKEYLINE, cwioKEYLINE);
		spref.get("cwioSHARED", cwioSHARED, cwioSHARED);
		spref.get("cwioPTT", cwioPTT, cwioPTT);
		spref.get("cwioCONNECTED", cwioCONNECTED, cwioCONNECTED);
		spref.get("cwioINVERTED", cwioINVERTED, cwioINVERTED);
		spref.get("cwioPORT", defbuffer, "NONE", 499);
		cwioPORT = defbuffer;
		char getbuff[20];
		for (int n = 0; n < 12; n++) {
			snprintf(getbuff, sizeof(getbuff), "cwiolabel[%d]", n);
			spref.get(getbuff, defbuffer, "", 499);
			cwio_labels[n] = defbuffer;
			snprintf(getbuff, sizeof(getbuff), "cwiomessage[%d]", n);
			spref.get(getbuff, defbuffer, "", 499);
			cwio_msgs[n] = defbuffer;
		}

		spref.get("BT", defbuffer, BT.c_str(), 499);
		BT = defbuffer;
		spref.get("AA", defbuffer, AA.c_str(), 499);
		AA = defbuffer;
		spref.get("AS", defbuffer, AS.c_str(), 499);
		AS = defbuffer;
		spref.get("AR", defbuffer, AR.c_str(), 499);
		AR = defbuffer;
		spref.get("KN", defbuffer, KN.c_str(), 499);
		KN = defbuffer;
		spref.get("SK", defbuffer, SK.c_str(), 499);
		SK = defbuffer;
		spref.get("INT", defbuffer, INT.c_str(), 499);
		INT = defbuffer;
		spref.get("HM", defbuffer, HM.c_str(), 499);
		HM = defbuffer;
		spref.get("VE", defbuffer, VE.c_str(), 499);
		VE = defbuffer;

		spref.get("FSK_KEYLINE", FSK_KEYLINE, FSK_KEYLINE);
		spref.get("FSK_SHARED", FSK_SHARED, FSK_SHARED);
		spref.get("FSK_PTT", FSK_PTT, FSK_PTT);
		spref.get("FSK_CONNECTED", FSK_CONNECTED, FSK_CONNECTED);
		spref.get("FSK_INVERTED", FSK_INVERTED, FSK_INVERTED);
		spref.get("FSK_STOPBITS", FSK_STOPBITS, FSK_STOPBITS);
		spref.get("FSK_PORT", defbuffer, "NONE", 499);
		FSK_PORT = defbuffer;

// CW logbook parameters
		spref.get("LOG_NAME", defbuffer, "", 499);
			log_name = defbuffer;
		spref.get("CUT_NUMBERS", log_cut_numbers, log_cut_numbers);
		spref.get("LEADING_ZEROS", log_leading_zeros, log_leading_zeros);
		spref.get("DUPCHECK", log_dupcheck, log_dupcheck);
		spref.get("QSO_NBR", qso_nbr, qso_nbr);

		for (int n = 0; n < 12; n++) {
			snprintf(getbuff, sizeof(getbuff), "fskiolabel[%d]", n);
			spref.get(getbuff, defbuffer, "", 499);
			FSK_labels[n] = defbuffer;
			snprintf(getbuff, sizeof(getbuff), "fskiomessage[%d]", n);
			spref.get(getbuff, defbuffer, "", 499);
			FSK_msgs[n] = defbuffer;
		}

		return true; 
	}
	return false;
}

void status::loadLastState()
{
	Fl_Preferences xcvrpref(RigHomeDir.c_str(), "w1hkj.com", PACKAGE_TARNAME);
	if (xcvrpref.entryExists("xcvr_name")) {
		char defbuffer[200];
		xcvrpref.get("xcvr_name", defbuffer, "NONE", 499);
		xcvr_name = defbuffer;
// for backward compatability
	} else if (xcvrpref.entryExists("last_xcvr_used")) {
		char defbuffer[200];
		xcvrpref.get("last_xcvr_used", defbuffer, "NONE", 499);
		xcvr_name = defbuffer;
	}
//	xcvrpref.get("xml_port", xmlport, xmlport);

	loadXcvrState(xcvr_name);
}

void status::UI_laststate()
{
	Fl_Color bgclr = fl_rgb_color(bg_red, bg_green, bg_blue);
	Fl_Color fgclr = fl_rgb_color(fg_red, fg_green, fg_blue);

	Fl::background( bg_sys_red, bg_sys_green, bg_sys_blue);
	Fl::background2( bg2_sys_red, bg2_sys_green, bg2_sys_blue);
	Fl::foreground( fg_sys_red, fg_sys_green, fg_sys_blue);

	FreqDispA->SetONOFFCOLOR( fgclr, bgclr );
	FreqDispA->font(fontnbr);
	FreqDispB->SetONOFFCOLOR( fgclr, fl_color_average(bgclr, FL_BLACK, 0.87));
	FreqDispB->font(fontnbr);

	scaleSmeter->color(bgclr);
	scaleSmeter->labelcolor(fgclr);
	scalePower->color(bgclr);
	scalePower->labelcolor(fgclr);
	btnALC_IDD_SWR->color(bgclr);
	btnALC_IDD_SWR->labelcolor(fgclr);

	sldrFwdPwr->color(fl_rgb_color (pwrRed, pwrGreen, pwrBlue), bgclr);
	sldrFwdPwr->PeakColor(fl_rgb_color(peakRed, peakGreen, peakBlue));

	sldrRcvSignal->color(fl_rgb_color (smeterRed, smeterGreen, smeterBlue), bgclr);
	sldrRcvSignal->PeakColor(fl_rgb_color(peakRed, peakGreen, peakBlue));

	sldrALC->color(fl_rgb_color (swrRed, swrGreen, swrBlue), bgclr);
	sldrALC->PeakColor(fl_rgb_color(peakRed, peakGreen, peakBlue));

	sldrSWR->color(fl_rgb_color (swrRed, swrGreen, swrBlue), bgclr);
	sldrSWR->PeakColor(fl_rgb_color(peakRed, peakGreen, peakBlue));

	sldrIDD->color(fl_rgb_color (swrRed, swrGreen, swrBlue), bgclr);
	sldrIDD->PeakColor(fl_rgb_color (peakRed, peakGreen, peakBlue));

	sldrVoltage->color(fl_rgb_color (voltRed, voltGreen, voltBlue), bgclr);
	sldrVoltage->PeakColor(bgclr);

	scaleVoltage->color(bgclr);
	scaleVoltage->labelcolor(fgclr);

	mtr_SMETER->color(bgclr);
	mtr_SMETER->labelcolor(fgclr);
	sigbar_SMETER->color(fl_rgb_color (smeterRed, smeterGreen, smeterBlue), bgclr);
	sigbar_SMETER->PeakColor(fl_rgb_color(peakRed, peakGreen, peakBlue));

	mtr_PWR->color(bgclr);
	mtr_PWR->labelcolor(fgclr);
	sigbar_PWR->color(fl_rgb_color (pwrRed, pwrGreen, pwrBlue), bgclr);
	sigbar_PWR->PeakColor(fl_rgb_color(peakRed, peakGreen, peakBlue));

	mtr_SWR->color(bgclr);
	mtr_SWR->labelcolor(fgclr);
	sigbar_SWR->color(fl_rgb_color (swrRed, swrGreen, swrBlue), bgclr);
	sigbar_SWR->PeakColor(fl_rgb_color(peakRed, peakGreen, peakBlue));

	mtr_ALC->color(bgclr);
	mtr_ALC->labelcolor(fgclr);
	sigbar_ALC->color(fl_rgb_color (swrRed, swrGreen, swrBlue), bgclr);
	sigbar_ALC->PeakColor(fl_rgb_color(peakRed, peakGreen, peakBlue));

	mtr_VOLTS->color(bgclr);
	mtr_VOLTS->labelcolor(fgclr);
	sigbar_VOLTS->color(fl_rgb_color (voltRed, voltGreen, voltBlue), bgclr);
	sigbar_VOLTS->PeakColor(bgclr);

// set scale for both voltage display
	sldrVoltage->minimum (6); sldrVoltage->maximum (16);
	sigbar_VOLTS->minimum (6); sigbar_VOLTS->maximum (16);

	if (UIsize != small_ui)
		meter_fill_box->color(bgclr);

	grpMeters->color(bgclr);
	grpMeters->labelcolor(fgclr);

	Fl_Color btn_lt_color = fl_rgb_color(lighted_btn_red, lighted_btn_green, lighted_btn_blue);
	if (btnVol)				btnVol->selection_color(btn_lt_color);
	if (btnNR)				btnNR->selection_color(btn_lt_color);
	if (btnIFsh)			btnIFsh->selection_color(btn_lt_color);
	if (btnNotch)			btnNotch->selection_color(btn_lt_color);
	if (btnA)				btnA->selection_color(btn_lt_color);
	if (btnB)				btnB->selection_color(btn_lt_color);
	if (btnSplit)			btnSplit->selection_color(btn_lt_color);
	if (btnAttenuator)		btnAttenuator->selection_color(btn_lt_color);
	if (btnPreamp)			btnPreamp->selection_color(btn_lt_color);
	if (btnNOISE)			btnNOISE->selection_color(btn_lt_color);
	if (btnAutoNotch)		btnAutoNotch->selection_color(btn_lt_color);
	if (btnTune)			btnTune->selection_color(btn_lt_color);
	if (btn_tune_on_off)	btn_tune_on_off->selection_color(btn_lt_color);
	if (btnPTT)				btnPTT->selection_color(btn_lt_color);
	if (btnLOCK)			btnLOCK->selection_color(btn_lt_color);
	if (btnAuxRTS)			btnAuxRTS->selection_color(btn_lt_color);
	if (btnAuxDTR)			btnAuxDTR->selection_color(btn_lt_color);
	if (btnSpot)			btnSpot->selection_color(btn_lt_color);
	if (btn_vox)			btn_vox->selection_color(btn_lt_color);
	if (btnCompON)			btnCompON->selection_color(btn_lt_color);
	if (btnSpecial)			btnSpecial->selection_color(btn_lt_color);
	if (btn_tt550_vox)		btn_tt550_vox->selection_color(btn_lt_color);
	if (btn_tt550_CompON)	btn_tt550_CompON->selection_color(btn_lt_color);
	if (btnAGC)				btnAGC->selection_color(btn_lt_color);
	if (btnPOWER)			btnPOWER->selection_color(btn_lt_color);

	Fl_Color bg_slider = fl_rgb_color(slider_red, slider_green, slider_blue);
	Fl_Color btn_slider = fl_rgb_color(slider_btn_red, slider_btn_green, slider_btn_blue);
	if (sldrVOLUME)			sldrVOLUME->color(bg_slider);
	if (sldrVOLUME)			sldrVOLUME->selection_color(btn_slider);
	if (sldrRFGAIN)			sldrRFGAIN->color(bg_slider);
	if (sldrRFGAIN)			sldrRFGAIN->selection_color(btn_slider);
	if (sldrSQUELCH)		sldrSQUELCH->color(bg_slider);
	if (sldrSQUELCH)		sldrSQUELCH->selection_color(btn_slider);
	if (sldrNR)				sldrNR->color(bg_slider);
	if (sldrNR)				sldrNR->selection_color(btn_slider);
	if (sldrIFSHIFT)		sldrIFSHIFT->color(bg_slider);
	if (sldrIFSHIFT)		sldrIFSHIFT->selection_color(btn_slider);
	if (sldrINNER)			sldrINNER->color(bg_slider);
	if (sldrINNER)			sldrINNER->selection_color(btn_slider);
	if (sldrOUTER)			sldrOUTER->color(bg_slider);
	if (sldrOUTER)			sldrOUTER->selection_color(btn_slider);
	if (sldrNOTCH)			sldrNOTCH->color(bg_slider);
	if (sldrNOTCH)			sldrNOTCH->selection_color(btn_slider);
	if (sldrMICGAIN)		sldrMICGAIN->color(bg_slider);
	if (sldrMICGAIN)		sldrMICGAIN->selection_color(btn_slider);
	if (sldrPOWER)			sldrPOWER->color(bg_slider);
	if (sldrPOWER)			sldrPOWER->selection_color(btn_slider);

	if (ic7610_digi_sel_val) {
		ic7610_digi_sel_val->color(bg_slider);
		ic7610_digi_sel_val->selection_color(btn_slider);
	}

	if (sldr_nb_level) {
		sldr_nb_level->color(bg_slider);
		sldr_nb_level->selection_color(btn_slider);
	}

	if (spnrPOWER)			spnrPOWER->color(bg_slider);
	if (spnrPOWER)			spnrPOWER->selection_color(btn_slider);

	if (spnrVOLUME)			spnrVOLUME->color(bg_slider);
	if (spnrVOLUME)			spnrVOLUME->selection_color(btn_slider);

	if (spnrRFGAIN)			spnrRFGAIN->color(bg_slider);
	if (spnrRFGAIN)			spnrRFGAIN->selection_color(btn_slider);

	if (spnrSQUELCH)		spnrSQUELCH->color(bg_slider);
	if (spnrSQUELCH)		spnrSQUELCH->selection_color(btn_slider);

	if (spnrNR)				spnrNR->color(bg_slider);
	if (spnrNR)				spnrNR->selection_color(btn_slider);

	if (spnrIFSHIFT)		spnrIFSHIFT->color(bg_slider);
	if (spnrIFSHIFT)		spnrIFSHIFT->selection_color(btn_slider);

	if (spnrNOTCH)			spnrNOTCH->color(bg_slider);
	if (spnrNOTCH)			spnrNOTCH->selection_color(btn_slider);

	if (spnrMICGAIN)		spnrMICGAIN->color(bg_slider);
	if (spnrMICGAIN)		spnrMICGAIN->selection_color(btn_slider);

	btnUser1->label(label1.c_str()); btnUser1->redraw_label();
	btnUser2->label(label2.c_str()); btnUser2->redraw_label();
	btnUser3->label(label3.c_str()); btnUser3->redraw_label();
	btnUser4->label(label4.c_str()); btnUser4->redraw_label();
	btnUser5->label(label5.c_str()); btnUser5->redraw_label();
	btnUser6->label(label6.c_str()); btnUser6->redraw_label();
	btnUser7->label(label7.c_str()); btnUser7->redraw_label();
	btnUser8->label(label8.c_str()); btnUser8->redraw_label();

	btnUser9->label(label9.c_str()); btnUser9->redraw_label();
	btnUser10->label(label10.c_str()); btnUser10->redraw_label();
	btnUser11->label(label11.c_str()); btnUser11->redraw_label();
	btnUser12->label(label12.c_str()); btnUser12->redraw_label();
	btnUser13->label(label13.c_str()); btnUser13->redraw_label();
	btnUser14->label(label14.c_str()); btnUser14->redraw_label();
	btnUser15->label(label15.c_str()); btnUser15->redraw_label();
	btnUser16->label(label16.c_str()); btnUser16->redraw_label();

	btnUser17->label(label17.c_str()); btnUser17->redraw_label();
	btnUser18->label(label18.c_str()); btnUser18->redraw_label();
	btnUser19->label(label19.c_str()); btnUser19->redraw_label();
	btnUser20->label(label20.c_str()); btnUser20->redraw_label();
	btnUser21->label(label21.c_str()); btnUser21->redraw_label();
	btnUser22->label(label22.c_str()); btnUser22->redraw_label();
	btnUser23->label(label23.c_str()); btnUser23->redraw_label();
	btnUser24->label(label24.c_str()); btnUser24->redraw_label();

	Fl::scheme(ui_scheme.c_str());
}

std::string status::info()
{
	std::stringstream info;
	static std::string retinfo;

	info << "status::info()\n============== Prefs File Contents =============\n\n";
	info << "xcvr_serial_port   : " << xcvr_serial_port << "\n";
	info << "comm_baudrate      : " << comm_baudrate << "\n";
	info << "comm_stopbits      : " << stopbits << "\n";
	info << "comm_retries       : " << comm_retries << "\n";
	info << "comm_wait          : " << comm_wait << "\n";
	info << "comm_timeout       : " << comm_timeout << "\n";
	info << "serloop_timing     : " << serloop_timing << "\n";
	info << "byte_interval      : " << byte_interval << "\n";
	info << "\n";
	info << "comm_echo          : " << comm_echo << "\n";
	info << "ptt_via_cat        : " << comm_catptt << "\n";
	info << "ptt_via_rts        : " << comm_rtsptt << "\n";
	info << "ptt_via_dtr        : " << comm_dtrptt << "\n";
	info << "rts_cts_flow       : " << comm_rtscts << "\n";
	info << "rts_plus           : " << comm_rtsplus << "\n";
	info << "dtr_plus           : " << comm_dtrplus << "\n";
	info << "civadr             : " << CIV << "\n";
	info << "usbaudio           : " << USBaudio << "\n";
	info << "\n";
	info << "aux_serial_port    : " << aux_serial_port.c_str() << "\n";
	info << "aux_rts            : " << aux_rts << "\n";
	info << "aux_dtr            : " << aux_dtr << "\n";
	info << "\n";
	info << "sep_serial_port    : " << sep_serial_port.c_str() << "\n";
	info << "sep_rtsptt         : " << sep_rtsptt << "\n";
	info << "sep_dtrptt         : " << sep_dtrptt << "\n";
	info << "sep_rtsplus        : " << sep_rtsplus << "\n";
	info << "set_dtrplus        : " << sep_dtrplus << "\n";
	info << "\n";
	info << "poll_smeter        : " << poll_smeter << "\n";
	info << "poll_frequency     : " << poll_frequency << "\n";
	info << "poll_mode          : " << poll_mode << "\n";
	info << "poll_bandwidth     : " << poll_bandwidth << "\n";
	info << "poll_volume        : " << poll_volume << "\n";
	info << "poll_auto_notch    : " << poll_auto_notch << "\n";
	info << "poll_notch         : " << poll_notch << "\n";
	info << "poll_ifshift       : " << poll_ifshift << "\n";
	info << "poll_power_control : " << poll_power_control << "\n";
	info << "poll_pre_att       : " << poll_pre_att << "\n";
	info << "poll_micgain       : " << poll_micgain << "\n";
	info << "poll_squelch       : " << poll_squelch << "\n";
	info << "poll_rfgain        : " << poll_rfgain << "\n";
	info << "poll_pout          : " << poll_pout << "\n";
	info << "poll_swr           : " << poll_swr << "\n";
	info << "poll_alc           : " << poll_alc << "\n";
	info << "poll_split         : " << poll_split << "\n";
	info << "poll_noise         : " << poll_noise << "\n";
	info << "poll_nr            : " << poll_nr << "\n";
	info << "poll_ptt           : " << poll_ptt << "\n";
	info << "poll_break_in      : " << poll_break_in << "\n";
	info << "poll_all           : " << poll_all << "\n";
	info << "\n";
	info << "freq_A             : " << freq_A << "\n";
	info << "mode_A             : " << imode_A << "\n";
	info << "bw_A               : " << iBW_A << "\n";
	info << "\n";
	info << "freq_B             : " << freq_B << "\n";
	info << "mode_B             : " << imode_B << "\n";
	info << "bw_B               : " << iBW_B << "\n";
	info << "\n";
	info << "filters            : " << filters << "\n";
	info << "\n";
	info << "use_rig_data       : " << use_rig_data << "\n";
//	info << "restore_rig_data   : " << restore_rig_data << "\n";
	info << "\n";
	info << "bool_spkr_on       : " << spkr_on << "\n";
	info << "int_volume         : " << volume << "\n";
	info << "dbl_power          : " << power_level << "\n";
	info << "dbl_power_limit %  : " << power_limit << "\n";
	info << "enable power limit:  " << enable_power_limit << "\n";
	info << "int_mic            : " << mic_gain << "\n";
	info << "bool_notch         : " << notch << "\n";
	info << "int_notch          : " << notch_val << "\n";
	info << "bool_shift         : " << shift << "\n";
	info << "int_shift          : " << shift_val << "\n";
	info << "rfgain             : " << rfgain << "\n";
	info << "squelch            : " << squelch << "\n";
	info << "\n";
	info << "schema             : " << schema << "\n";
	info << "\n";
	info << "rx_avg             : " << rx_avg << "\n";
	info << "rx_peak            : " << rx_peak << "\n";
	info << "pwr_avg            : " << pwr_avg << "\n";
	info << "pwr_peak           : " << pwr_peak << "\n";
	info << "pwr_scale          : " << pwr_scale << "\n";
	info << "\n";
	info << "line_out           : " << line_out << "\n";
	info << "data_port          : " << data_port << "\n";
	info << "vox_on_dataport    : " << vox_on_dataport << "\n";
	info << "agc_level          : " << agc_level << "\n";
	info << "cw_wpm             : " << cw_wpm << "\n";
	info << "cw_weight          : " << cw_weight << "\n";
	info << "cw_vol             : " << cw_vol << "\n";
	info << "cw_spot            : " << cw_spot << "\n";
	info << "spot_onoff         : " << spot_onoff << "\n";
	info << "cw_spot_tone       : " << cw_spot_tone << "\n";
	info << "cw_qsk             : " << cw_qsk << "\n";
	info << "(cw_delay          : " << cw_delay << "\n";
	info << "enable_keyer       : " << enable_keyer << "\n";
	info << "break_in           : " << break_in << "\n";
	info << "vox_onoff          : " << vox_onoff << "\n";
	info << "vox_gain           : " << vox_gain << "\n";
	info << "vox_anti           : " << vox_anti << "\n";
	info << "vox_hang           : " << vox_hang << "\n";
	info << "compression        : " << compression << "\n";
	info << "compON             : " << compON << "\n";
	info << "\n";
	info << "label 1            : " << label1 << "\n";
	info << "command 1          : " << command1 << "\n";
	info << "label 2            : " << label1 << "\n";
	info << "command 2          : " << command1 << "\n";
	info << "label 3            : " << label1 << "\n";
	info << "command 3          : " << command1 << "\n";
	info << "label 4            : " << label1 << "\n";
	info << "command 4          : " << command1 << "\n";
	info << "label 5            : " << label1 << "\n";
	info << "command 5          : " << command1 << "\n";
	info << "label 6            : " << label1 << "\n";
	info << "command 6          : " << command1 << "\n";
	info << "label 7            : " << label1 << "\n";
	info << "command 7          : " << command1 << "\n";
	info << "label 8            : " << label1 << "\n";
	info << "command 8          : " << command1 << "\n";
	info << "label 9            : " << label9 << "\n";
	info << "command 9          : " << command9 << "\n";
	info << "label 10           : " << label10 << "\n";
	info << "command 10         : " << command10 << "\n";
	info << "label 11           : " << label11 << "\n";
	info << "command 11         : " << command11 << "\n";
	info << "label 12           : " << label12 << "\n";
	info << "command 12         : " << command12 << "\n";
	info << "label 13           : " << label13 << "\n";
	info << "command 13         : " << command13 << "\n";
	info << "label 14           : " << label14 << "\n";
	info << "command 14         : " << command14 << "\n";
	info << "label 15           : " << label15 << "\n";
	info << "command 15         : " << command15 << "\n";
	info << "label 16           : " << label16 << "\n";
	info << "command 16         : " << command16 << "\n";
	info << "label 17           : " << label17 << "\n";
	info << "command 17         : " << command17 << "\n";
	info << "label 18           : " << label18 << "\n";
	info << "command 18         : " << command18 << "\n";
	info << "label 19           : " << label19 << "\n";
	info << "command 19         : " << command19 << "\n";
	info << "label 20           : " << label20 << "\n";
	info << "command 20         : " << command20 << "\n";
	info << "label 21           : " << label21 << "\n";
	info << "command 21         : " << command21 << "\n";
	info << "label 22           : " << label22 << "\n";
	info << "command 22         : " << command22 << "\n";
	info << "label 23           : " << label23 << "\n";
	info << "command 23         : " << command23 << "\n";
	info << "label 24           : " << label24 << "\n";
	info << "command 24         : " << command24 << "\n";

	info << "BT                 : " << BT << "\n";
	info << "AA                 : " << AA << "\n";
	info << "AS                 : " << AS << "\n";
	info << "AR                 : " << AR << "\n";
	info << "KN                 : " << KN << "\n";
	info << "SK                 : " << SK << "\n";
	info << "INT                : " << INT << "\n";
	info << "HM                 : " << HM << "\n";
	info << "VE                 : " << VE << "\n";

	return retinfo = info.str();
}

static bool strace;
static bool srigtrace;
static bool ssettrace;
static bool sgettrace;
static bool sstrace;
static bool stcitrace;

void ss_trace(bool on)
{
	if (on) {
		strace = progStatus.trace;
		srigtrace = progStatus.rigtrace;
		ssettrace = progStatus.settrace;
		sgettrace = progStatus.gettrace;
		sstrace   = progStatus.serialtrace;
		stcitrace = progStatus.tcitrace;

		progStatus.trace =
		progStatus.rigtrace =
		progStatus.settrace =
		progStatus.serialtrace =
		progStatus.gettrace = 
		progStatus.tcitrace = true;
	} else {
		progStatus.trace = strace;
		progStatus.rigtrace = srigtrace;
		progStatus.settrace = ssettrace;
		progStatus.gettrace = sgettrace;
		progStatus.serialtrace = sstrace;
		progStatus.tcitrace = stcitrace;
	}
}

