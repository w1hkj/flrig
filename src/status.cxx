/* -----------------------------------------------------------------------------
 * status structure / methods
 *
 * A part of "rig", a rig control program compatible with fldigi / xmlrpc i/o
 *
 * copyright Dave Freese 2009, w1hkj@w1hkj.com
 *
*/

#include <iostream>
#include <fstream>
#include <string>

#include <FL/Fl_Preferences.H>
#include <FL/Fl_Progress.H>

#include "status.h"
#include "util.h"
#include "rig.h"
#include "support.h"
#include "config.h"
#include "rigpanel.h"

string last_xcvr_used = "none";

status progStatus = {
	50,			// int mainX;
	50,			// int mainY;
	0,			// int rig_nbr;
	"NONE",		// string xcvr_serial_port;
	0,			// int comm_baudrate;
	2,			// int stopbits;
	2,			// int comm_retries;
	5,			// int comm_wait;
	50,			// int comm_timeout;
	false,		// bool comm_echo;
	false,		// bool comm_catptt;
	false,		// bool comm_rtsptt;
	false,		// bool comm_dtrptt;
	false,		// bool comm_rtscts;
	false,		// bool comm_rtsplus;
	false,		// bool comm_dtrplus;
	200,		// int  serloop_timing;

	"NONE",		// string control_port;
	false,		// bool aux_rts;
	false,		// bool aux_dtr;

	"NONE",		// string	sep_serial_port;
	false,		// bool	sep_rtsptt;
	false,		// bool	sep_dtrptt;
	false,		// bool	sep_rtsplus;
	false,		// bool	sep_dtrplus;
	0,			// int	CIV;
	false,		// bool	USBaudio;

	0,			// int  opBW;
	1,			// int  opMODE;
	7070000,	// long frequency;
	0,			// vfoB bw;
	0,			// vfoB mode;
	7070000,	// vfoB freq
	true,		// bool use_rig_data;
	true,		// bool restore_rig_data;

	false,		// bool spkr_on;
	20,			// int  volume;
	0,			// int  power_level;
	10,			// int  mic_gain;
	false,		// bool notch;
	0,			// int  notch_val;
	false,		// bool shift;
	0,			// int  shift_val;
	10,			// int  rfgain;
	10,			// int  squelch;
	
	0,			// int  line_out;
	1,			// int  agc_level;
	18,			// int  cw_wpm;
	0,			// int  cw_vol;
	0,			// int  cw_spot;
	false,		// int  vox_onoff;
	0,			// int  vox_gain;
	0,			// int  vox_anti;
	0,			// int  vox_hang;
	0,			// int  compression;
	false,		// bool compON;

	false,		// bool noise_reduction;
	0,			// int  noise_reduction_val;

	false,		// bool noise;
	0,			// int  attenuator
	0,			// int  preamp;
	0,			// int  auto_notch;

	false,		// bool use_line_in;
	false,		// bool split;

//tt550 controls
	100,		// tt550_line_out;
	1,			// tt550_agc_level;

	24,			// tt550_cw_wpm;
	1.0,		// tt550_cw_weight;
	0,			// tt550_cw_vol;
	0,			// tt550_cw_spot;
	false,		// tt550_cw_spot_onoff;
	0,			// tt550_cw_qsk;
	true,		// tt550_enable_keyer;

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

	14,			// tt550_xmt_bw;
	false,		// tt550_use_xmt_bw;
	
	25,			// tt550_AM_level;

	0.0,		// vfo_adj;
	600,		// bfo_freq;
	0,			// rit_freq;
	0,			// xit_freq;
	1500,		// bpf_center;
	true,		// use_bpf_center;

	232,		// int	 s_red;
	255,		// int	 s_green;
	232,		// int	 s_blue;

	232,		// int	 bg_red;
	255,		// int	 bg_green;
	232,		// int	 bg_blue;

	0,			// int	 fg_red;
	0,			// int	 fg_green;
	0,			// int	 fg_blue;

	0,			// int	 meter_red;
	180,		// int	 meter_green;
	0,			// int	 meter_blue;

	180,		// int	 pwrRed;
	0,			// int	 pwrGreen;
	0,			// int	 pwrBlue

	148,		// int	 swrRed;
	0,			// int	 swrGreen;
	180,		// int	 swrBlue;

	FL_COURIER,	// Fl_Font fontnbr;

	false,		// bool	 tooltips;

	"7362"		// string server_port

};

void status::saveLastState()
{
	last_xcvr_used = selrig->name_;
	Fl_Preferences xcvrpref(RigHomeDir.c_str(), "w1hkj.com", PACKAGE_TARNAME);
	xcvrpref.set("last_xcvr_used", last_xcvr_used.c_str());

	int mX = mainwindow->x();
	int mY = mainwindow->y();
	if (mX >= 0 && mX >= 0) {
		mainX = mX;
		mainY = mY;
	}

	Fl_Preferences spref(RigHomeDir.c_str(), "w1hkj.com", last_xcvr_used.c_str());

	spref.set("version", PACKAGE_VERSION);
	spref.set("mainx", mX);
	spref.set("mainy", mY);

	spref.set("xcvr_serial_port", xcvr_serial_port.c_str());
	spref.set("comm_baudrate", comm_baudrate);
	spref.set("comm_stopbits", stopbits);
	spref.set("comm_retries", comm_retries);
	spref.set("comm_wait", comm_wait);
	spref.set("comm_timeout", comm_timeout);
	spref.set("serloop_timing", serloop_timing);

	spref.set("comm_echo", comm_echo);
	spref.set("ptt_via_cat", comm_catptt);
	spref.set("ptt_via_rts", comm_rtsptt);
	spref.set("ptt_via_dtr", comm_dtrptt);
	spref.set("rts_cts_flow", comm_rtscts);
	spref.set("rts_plus", comm_rtsplus);
	spref.set("dtr_plus", comm_dtrplus);
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

	spref.set("bw_A", iBW_A);
	spref.set("mode_A", imode_A);
	spref.set("freq_A", freq_A);

	spref.set("bw_B", iBW_B);
	spref.set("mode_B", imode_B);
	spref.set("freq_B", freq_B);

	spref.set("use_rig_data", use_rig_data);
	spref.set("restore_rig_data", restore_rig_data);

	spref.set("bool_spkr_on", spkr_on);
	spref.set("int_volume", volume);
	spref.set("int_power", power_level);
	spref.set("int_mic", mic_gain);
	spref.set("bool_notch", notch);
	spref.set("int_notch", notch_val);
	spref.set("bool_shift", shift);
	spref.set("int_shift", shift_val);
	spref.set("rfgain", rfgain);
	spref.set("squelch", squelch);

	if (rig_nbr == TT550) {
		spref.set("tt550_line_out", tt550_line_out);
		spref.set("tt550_agc_level", tt550_agc_level);

		spref.set("tt550_cw_wpm", tt550_cw_wpm);
		spref.set("tt550_cw_weight", tt550_cw_weight);
		spref.set("tt550_cw_vol", tt550_cw_vol);
		spref.set("tt550_cw_spot", tt550_cw_spot);
		spref.set("tt550_spot_onoff", tt550_spot_onoff);
		spref.set("tt550_cw_qsk", tt550_cw_qsk);
		spref.set("tt550_enable_keyer", tt550_enable_keyer);

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

		spref.set("tt550_AM_level", tt550_AM_level);
	} else {
		spref.set("line_out", line_out);
		spref.set("agc_level", agc_level);
		spref.set("cw_wpm", cw_wpm);
		spref.set("cw_vol", cw_vol);
		spref.set("cw_spot", cw_spot);
		spref.set("vox_onoff", vox_onoff);
		spref.set("vox_gain", vox_gain);
		spref.set("vox_anti", vox_anti);
		spref.set("vox_hang", vox_hang);
		spref.set("compression", compression);
		spref.set("compON", compON);
	}

	spref.set("noise_reduction", noise_reduction);
	spref.set("noise_red_val", noise_reduction_val);

	spref.set("bool_noise", noise);
	spref.set("int_preamp", preamp);
	spref.set("int_att", attenuator);

	spref.set("use_line_in", use_line_in);

	spref.set("vfo_adj", vfo_adj);
	spref.set("bfo_freq", bfo_freq);
	spref.set("rit_freq", rit_freq);
	spref.set("xit_freq", xit_freq);
	spref.set("bpf_center", bpf_center);
	spref.set("use_bpf_center", use_bpf_center);

	spref.set("s_red", s_red);
	spref.set("s_green", s_green);
	spref.set("s_blue", s_blue);

	spref.set("fg_red", fg_red);
	spref.set("fg_green", fg_green);
	spref.set("fg_blue", fg_blue);

	spref.set("bg_red", bg_red);
	spref.set("bg_green", bg_green);
	spref.set("bg_blue", bg_blue);

	spref.set("meter_red", meter_red);
	spref.set("meter_green", meter_green);
	spref.set("meter_blue", meter_blue);

	spref.set("power_red", pwrRed);
	spref.set("power_green", pwrGreen);
	spref.set("power_blue", pwrBlue);

	spref.set("swr_red", swrRed);
	spref.set("swr_green", swrGreen);
	spref.set("swr_blue", swrBlue);

	spref.set("fontnbr", fontnbr);

	spref.set("tooltips", tooltips);

	spref.set("server_port", server_port.c_str());

}

bool status::loadXcvrState(const char *xcvr)
{
	Fl_Preferences spref(RigHomeDir.c_str(), "w1hkj.com", xcvr);

	if (spref.entryExists("version")) {

		int i;
		char defbuffer[200];

		spref.get("mainx", mainX, mainX);
		spref.get("mainy", mainY, mainY);

		spref.get("xcvr_serial_port", defbuffer, "NONE", 199);
		xcvr_serial_port = defbuffer;
		if (xcvr_serial_port.find("tty") == 0) 
			xcvr_serial_port.insert(0, "/dev/");

		rig_nbr = NONE;
		for (int i = NONE; i < LAST_RIG; i++)
			if (strcmp(rigs[i]->name_, xcvr) == 0) {
				rig_nbr = i;
				break;
			}

		spref.get("comm_baudrate", comm_baudrate, comm_baudrate);
		spref.get("comm_stopbits", stopbits, stopbits);
		spref.get("comm_retries", comm_retries, comm_retries);
		spref.get("comm_wait", comm_wait, comm_wait);
		spref.get("comm_timeout", comm_timeout, comm_timeout);
		spref.get("serloop_timing", serloop_timing, serloop_timing);

		if (spref.get("comm_echo", i, i)) comm_echo = i;
		if (spref.get("ptt_via_cat", i, i)) comm_catptt = i;
		if (spref.get("ptt_via_rts", i, i)) comm_rtsptt = i;
		if (spref.get("ptt_via_dtr", i, i)) comm_dtrptt = i;
		if (spref.get("rts_cts_flow", i, i)) comm_rtscts = i;
		if (spref.get("rts_plus", i, i)) comm_rtsplus = i;
		if (spref.get("dtr_plus", i, i)) comm_dtrplus = i;
		spref.get("civadr", CIV, CIV);
		if (spref.get("usbaudio", i, i)) USBaudio = i;

		spref.get("aux_serial_port", defbuffer, "NONE", 199);
		aux_serial_port = defbuffer;
		if (spref.get("aux_rts", i, i)) aux_rts = i;
		if (spref.get("aux_dtr", i, i)) aux_dtr = i;

		spref.get("sep_serial_port", defbuffer, "NONE", 199);
		sep_serial_port = defbuffer;
		if (spref.get("sep_rtsptt", i, i)) sep_rtsptt = i;
		if (spref.get("sep_dtrptt", i, i)) sep_dtrptt = i;
		if (spref.get("sep_rtsplus", i, i)) sep_rtsplus = i;
		if (spref.get("sep_dtrplus", i, i)) sep_dtrplus = i;

		spref.get("bw_A", iBW_A, iBW_A);
		spref.get("mode_A", imode_A, imode_A);
		spref.get("freq_A", freq_A, freq_A);

		spref.get("bw_B", iBW_B, iBW_B);
		spref.get("mode_B", imode_B, imode_B);
		spref.get("freq_B", freq_B, freq_B);
	
		if (spref.get("use_rig_data", i, i)) use_rig_data = i;
		if (spref.get("restore_rig_data", i, i)) restore_rig_data = i;

		if (spref.get("bool_spkr_on", i, i)) spkr_on = i;
		spref.get("int_volume", volume, volume);
		spref.get("int_power", power_level, power_level);
		spref.get("int_mic", mic_gain, mic_gain);
		if (spref.get("bool_notch", i, i)) notch = i;
		spref.get("int_notch", notch_val, notch_val);
		if (spref.get("bool_shift", i, i)) shift = i;
		spref.get("int_shift", shift_val, shift_val);
		spref.get("rfgain", rfgain, rfgain);
		spref.get("squelch", squelch, squelch);

		if (rig_nbr == TT550) {
			spref.get("tt550_line_out", tt550_line_out, tt550_line_out);
			spref.get("tt550_agc_level", tt550_agc_level, tt550_agc_level);

			spref.get("tt550_cw_wpm", tt550_cw_wpm, tt550_cw_wpm);
			spref.get("tt550_cw_weight", tt550_cw_weight, tt550_cw_weight);
			spref.get("tt550_cw_vol", tt550_cw_vol, tt550_cw_vol);
			spref.get("tt550_cw_spot", tt550_cw_spot, tt550_cw_spot);
			if (spref.get("tt550_spot_onoff", i, i)) tt550_spot_onoff = i;
			spref.get("tt550_cw_qsk", tt550_cw_qsk, tt550_cw_qsk);
			if (spref.get("tt550_enable_keyer", i, i)) tt550_enable_keyer = i;

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

			spref.get("tt550_AM_level", tt550_AM_level, tt550_AM_level);
		} 
		else {
			spref.get("line_out", line_out, line_out);
			spref.get("agc_level", agc_level, agc_level);
			spref.get("cw_wpm", cw_wpm, cw_wpm);
			spref.get("cw_vol", cw_vol, cw_vol);
			spref.get("cw_spot", cw_spot, cw_spot);
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
		spref.get("int_preamp", preamp, preamp);
		spref.get("int_att", attenuator, attenuator);

		if (spref.get("use_line_in", i, i)) use_line_in = i;

		spref.get("vfo_adj", vfo_adj, vfo_adj);
		spref.get("bfo_freq", bfo_freq, bfo_freq);
		spref.get("rit_freq", rit_freq, rit_freq);
		spref.get("xit_freq", xit_freq, xit_freq);
		spref.get("bpf_center", bpf_center, bpf_center);
		spref.get("use_bpf_center", i, i); use_bpf_center = i;

		spref.get("s_red", s_red, s_red);
		spref.get("s_green", s_green, s_green);
		spref.get("s_blue", s_blue, s_blue);

		spref.get("fg_red", fg_red, fg_red);
		spref.get("fg_green", fg_green, fg_green);
		spref.get("fg_blue", fg_blue, fg_blue);

		spref.get("bg_red", bg_red, bg_red);
		spref.get("bg_green", bg_green, bg_green);
		spref.get("bg_blue", bg_blue, bg_blue);

		spref.get("meter_red", meter_red, meter_red);
		spref.get("meter_green", meter_green, meter_green);
		spref.get("meter_blue", meter_blue, meter_blue);

		spref.get("power_red", pwrRed, pwrRed);
		spref.get("power_green", pwrGreen, pwrGreen);
		spref.get("power_blue", pwrBlue, pwrBlue);

		spref.get("swr_red", swrRed, swrRed);
		spref.get("swr_green", swrGreen, swrGreen);
		spref.get("swr_blue", swrBlue, swrBlue);

		i = (int)fontnbr;
		spref.get("fontnbr", i, i); fontnbr = (Fl_Font)i;
		i = 0;
		if (spref.get("tooltips", i, i)) tooltips = i;

		spref.get("server_port", defbuffer, "7362", 199);
		server_port = defbuffer;

	}

	FreqDispA->SetONOFFCOLOR(
		fl_rgb_color(fg_red, fg_green, fg_blue),
		fl_rgb_color(bg_red, bg_green, bg_blue));
	FreqDispA->font(fontnbr);
	FreqDispB->SetONOFFCOLOR(
		fl_rgb_color(fg_red, fg_green, fg_blue),
		fl_color_average(fl_rgb_color(bg_red, bg_green, bg_blue), FL_BLACK, 0.87));
	FreqDispB->font(fontnbr);

	Fl_Color clr = fl_rgb_color(s_red, s_green, s_blue);
	scaleSmeter->color(clr);
	scalePower->color(clr);
	btnALC_SWR->color(clr);
	grpMeters->color(clr);

	Fl_Color mclr = fl_rgb_color (meter_red, meter_green, meter_blue);
	Fl_Color pclr = fl_rgb_color (pwrRed, pwrGreen, pwrBlue);
	Fl_Color sclr = fl_rgb_color (swrRed, swrGreen, swrBlue);
	sldrFwdPwr->color(pclr, clr);
	sldrRcvSignal->color(mclr, clr);
	sldrALC_SWR->color(sclr, clr);
	sldrFwdPwr->redraw();
	sldrRcvSignal->redraw();
	sldrALC_SWR->redraw();

	if (use_rig_data) mnuKeepData->set();
	else mnuKeepData->clear();

	if (restore_rig_data) mnuRestoreData->set();
	else mnuRestoreData->clear();

	return true;
}

void status::loadLastState()
{
	Fl_Preferences xcvrpref(RigHomeDir.c_str(), "w1hkj.com", PACKAGE_TARNAME);
	if (xcvrpref.entryExists("last_xcvr_used")) {
		char defbuffer[200];
		xcvrpref.get("last_xcvr_used", defbuffer, "none", 199);
		last_xcvr_used = defbuffer;
	}
	loadXcvrState(last_xcvr_used.c_str());
}

