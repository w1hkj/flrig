// ----------------------------------------------------------------------------
// Copyright (C) 2014-2011
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

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>

#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Enumerations.H>

#include "icons.h"
#include "support.h"
#include "debug.h"
#include "gettext.h"
#include "rig_io.h"
#include "dialogs.h"
#include "rigbase.h"
#include "ptt.h"
#include "socket_io.h"
#include "ui.h"
#include "tod_clock.h"
#include "rig.h"
#include "rigs.h"
#include "K3_ui.h"
#include "KX3_ui.h"
#include "rigpanel.h"
#include "tod_clock.h"
#include "trace.h"
#include "cwio.h"
#include "fsk.h"
#include "fskioUI.h"
#include "xml_server.h"
#include "gpio_ptt.h"
#include "cmedia.h"
#include "tmate2.h"

void initTabs();

rigbase *selrig = rigs[0];

extern bool test;
void init_notch_control();

int freqval = 0;

XCVR_STATE vfoA		= XCVR_STATE(14070000, 0, 0, UI);
XCVR_STATE vfoB		= XCVR_STATE(7070000, 0, 0, UI);
XCVR_STATE *vfo		= &vfoA;

//XCVR_STATE xmlvfo	= XCVR_STATE(14070000, 0, 0, UI);

XCVR_STATE xcvr_vfoA, xcvr_vfoB;

enum {VOL, MIC, PWR, SQL, IFSH, NOTCH, RFGAIN, NR, NB };

std::queue<VFOQUEUE> srvc_reqs;

const char **old_bws = NULL;

// Add alpha-tag to XCVR_STATE;
struct ATAG_XCVR_STATE {
	long freq;
	int  imode;
	int  iBW;
	int  src;
	char alpha_tag[ATAGSIZE];
};
ATAG_XCVR_STATE oplist[LISTSIZE];

int  numinlist = 0;
std::vector<std::string> rigmodes_;
std::vector<std::string> rigbws_;

Cserial *RigSerial;
Cserial *AuxSerial;
Cserial *SepSerial;

bool using_buttons = false;

enum { SWR_IMAGE, ALC_IMAGE };
int meter_image = SWR_IMAGE;

bool xcvr_online = false;

// meter values passed to display functions

double smtrval = 0;
double pwrval = 0;
double swrval = 0;
double alcval = 0;
double vmtrval = 0;

//======================================================================
// slider change processing
//======================================================================
int inhibit_nb_level = 0;
int inhibit_volume = 0;
int inhibit_pbt = 0;
int inhibit_shift = 0;
int inhibit_nr = 0;
int inhibit_notch = 0;
int inhibit_power = 0;
int inhibit_mic = 0;
int inhibit_rfgain = 0;
int inhibit_squelch = 0;

struct SLIDER {
enum {NOTCH, SHIFT, INNER, OUTER, LOCK, VOLUME, MIC, POWER, SQUELCH, RFGAIN, NB_LEVEL, NR, NR_VAL};
	int  fnc;
	int  val;
};

//======================================================================
// loop for serial i/o thread
// runs continuously until program is closed
// only accesses the serial port if it has been successfully opened
//======================================================================

bool bypass_serial_thread_loop = true;
bool run_serial_thread = true;

bool PTT = false;

int  powerlevel = 0;

std::string printXCVR_STATE(XCVR_STATE &data)
{
	std::stringstream str;
	const char **bwt = selrig->bwtable(data.imode);
	const char **dsplo = selrig->lotable(data.imode);
	const char **dsphi = selrig->hitable(data.imode);
	str << data.freq << ", ";
	str <<
		(selrig->modes_ ? selrig->modes_[data.imode] : "modes n/a");
	if (data.iBW > 256 && selrig->has_dsp_controls) {
		str << ", " <<
		(dsplo ? dsplo[data.iBW & 0x7F] : "??");
	} else if (bwt) {
		str << ", " << bwt[data.iBW];
	} else
		str << ", n/a";

	if (data.iBW > 256 && selrig->has_dsp_controls) {
		str << ", " <<
		(dsphi ? dsphi[(data.iBW >> 8) & 0x7F] : "??");
	}

	return str.str();
}

std::string print_ab()
{
	std::string s;
	s.assign("VFO-A: ");
	s.append(printXCVR_STATE(vfoA));
	s.append("; VFO-B: ");
	s.append(printXCVR_STATE(vfoB));
	return s;
}

const char *print(XCVR_STATE &data)
{
	static std::string prstr;
	static char str[1024];
	const char **bwt = selrig->bwtable(data.imode);
	const char **dsplo = selrig->lotable(data.imode);
	const char **dsphi = selrig->hitable(data.imode);

	str[0] = 0;
	prstr.clear();
	snprintf(
		str, sizeof(str), "\
Data Source: %s\n\
  freq ........... %ld\n\
  mode ........... %d [%s]\n",
		data.src == XML ? "XML" : data.src == UI ? "UI" :
			data.src == SRVR ? "SRVR" : "RIG",
		data.freq,
		data.imode,
		selrig->modes_ ? selrig->modes_[data.imode] : "modes n/a");
	prstr.assign(str);
	str[0] = 0;
	if (selrig->has_FILTER) {
		if (bwt && dsplo && dsphi) {
			snprintf(str, sizeof(str), "\
  filter ......... %s\n\
  bwt index ...... %2d, [%s] [%s]\n",
			selrig->FILT(selrig->get_FILT(data.imode)),
			data.iBW,
			(data.iBW > 256 && selrig->has_dsp_controls) ?
				(dsplo ? dsplo[data.iBW & 0x7F] : "??") : (bwt ? bwt[data.iBW] : "n/a"),
			(data.iBW > 256 && selrig->has_dsp_controls) ?
				(dsphi ? dsphi[(data.iBW >> 8) & 0x7F] : "??") : ""
			);
		} else if (bwt) {
			snprintf(str, sizeof(str), "\
  filter ......... %s\n\
  bwt index ...... %2d, [%s]\n",
			selrig->FILT(selrig->get_FILT(data.imode)),
			data.iBW,
			bwt[data.iBW] );
		}
		prstr.append(str);
		str[0] = 0;
	}
	snprintf( str, sizeof(str), "\
  split .......... %4d, power_control . %4.1f, volume_control  %4d\n\
  attenuator ..... %4d, preamp ........ %4d, rf gain ....... %4d\n\
  if_shift ....... %4d, shift val ..... %4d\n\
  auto notch ..... %4d, notch ......... %4d, notch value ... %4d\n\
  noise .......... %4d, nr ............ %4d, nr val ........ %4d\n\
  mic gain ....... %4d, agc level ..... %4d, squelch ....... %4d\n\
  compression .... %4d, compON ........ %4d",
		data.split,
		data.power_control,
		data.volume_control,
		data.attenuator,
		data.preamp,
		data.rf_gain,
		data.if_shift,
		data.shift_val,
		data.auto_notch,
		data.notch,
		data.notch_val,
		data.noise,
		data.nr,
		data.nr_val,
		data.mic_gain,
		data.agc_level,
		data.squelch,
		data.compression,
		data.compON
	);
	prstr.append(str);
	return prstr.c_str();
}

void read_info()
{
	trace(1,"read_info()");
	selrig->get_info();
}

void update_vfoAorB(void *d)
{
	trace(1,"update_vfoAorB()");
	if (selrig->inuse == onB) {
		vfoB.src = RIG;
		vfoB.freq = selrig->get_vfoB();
		vfoB.imode = selrig->get_modeB();
		vfoB.iBW = selrig->get_bwB();
	} else {
		vfoA.src = RIG;
		vfoA.freq = selrig->get_vfoA();
		vfoA.imode = selrig->get_modeA();
		vfoA.iBW = selrig->get_bwA();
	}
	updateUI((void*)0);
}

void read_vfo()
{
	if (xcvr_name == rig_K3.name_) {
		read_K3_vfo();
		return;
	}

//	if (xcvr_name == rig_KX3.name_) {
//		read_KX3_vfo();
//		return;
//	}

// transceiver changed ?
	trace(1,"read_vfo()");
	unsigned long int  freq;

	static int current_vfo = onNIL;
	int chkvfo = selrig->get_vfoAorB();
	if (current_vfo != chkvfo) {
		current_vfo = chkvfo;
		Fl::awake(updateUI);
	}
	if (current_vfo == onNIL)
		return;

	if (selrig->has_get_info)
		selrig->get_info();

	if (selrig->inuse == onA) { // vfo-A
		trace(2, "vfoA active", "get vfo A");
		freq = selrig->get_vfoA();
		if (freq != vfoA.freq) {
			vfoA.freq = freq;
			Fl::awake(setFreqDispA);
			vfo = &vfoA;
		}
		if ( selrig->twovfos() ) {
			trace(2, "vfoA active", "get vfo B");
			freq = selrig->get_vfoB();
			if (freq != vfoB.freq) {
				vfoB.freq = freq;
				Fl::awake(setFreqDispB);
			}
		}
	} else { // vfo-B
		trace(2, "vfoB active", "get vfo B");
		freq = selrig->get_vfoB();
		if (freq != vfoB.freq) {
			vfoB.freq = freq;
			Fl::awake(setFreqDispB);
			vfo = &vfoB;
		}
		if ( selrig->twovfos() ) {
			trace(2, "vfoB active", "get vfo A");
			freq = selrig->get_vfoA();
			if (freq != vfoA.freq) {
				vfoA.freq = freq;
				Fl::awake(setFreqDispA);
			}
		}
	}

}

void update_ifshift(void *d);

void updateUI(void *)
{
	setModeControl(NULL);
	setBWControl(NULL);
	updateBandwidthControl(NULL);
	highlight_vfo(NULL);

	int min, max, step;
	selrig->get_if_min_max_step(min, max, step);
	if (sldrIFSHIFT) sldrIFSHIFT->minimum(min);
	if (sldrIFSHIFT) sldrIFSHIFT->maximum(max);
	if (sldrIFSHIFT) sldrIFSHIFT->step(step);
	if (sldrIFSHIFT) sldrIFSHIFT->redraw();
	if (spnrIFSHIFT) spnrIFSHIFT->minimum(min);
	if (spnrIFSHIFT) spnrIFSHIFT->maximum(max);
	if (spnrIFSHIFT) spnrIFSHIFT->step(step);
	if (spnrIFSHIFT) spnrIFSHIFT->redraw();
	update_ifshift((void *)0);

}

void TRACED(setModeControl, void *)

	opMODE->index(vfo->imode);
	opMODE->redraw();

// enables/disables the IF shift control, depending on the mode.
// the IF Shift function, is ONLY valid in CW modes, with the 870S.

	if (xcvr_name == rig_TS870S.name_) {
		if (vfo->imode == RIG_TS870S::tsCW || vfo->imode == RIG_TS870S::tsCWR) {
			btnIFsh->activate();
			if (sldrIFSHIFT) sldrIFSHIFT->activate();
			if (spnrIFSHIFT) spnrIFSHIFT->activate();
		} else {
			btnIFsh->deactivate();
			if (sldrIFSHIFT) sldrIFSHIFT->deactivate();
			if (spnrIFSHIFT) spnrIFSHIFT->deactivate();
		}
	}
}

void setFILTER(void *)
{
	if (selrig->has_FILTER) {
		if (selrig->inuse == onB)
			btnFILT->label(selrig->FILT(vfoB.filter));
		else
			btnFILT->label(selrig->FILT(vfoA.filter));
		btnFILT->redraw_label();
trace(3, "Filter", (selrig->inuse == onB ? "B" : "A"), btnFILT->label());
	}
}

// mode and bandwidth
void read_mode()
{
	if (xcvr_name == rig_K3.name_) {
		read_K3_mode();
		return;
	}
//	if (xcvr_name == rig_KX3.name_) {
//		read_KX3_mode();
//		return;
//	}

	int nu_mode;
	int nu_BW;
	if (selrig->inuse == onA) {
		rig_trace(2, "read_mode", "vfoA active");
		nu_mode = selrig->get_modeA();
		if (nu_mode != opMODE->index()) { //vfoA.imode) {
			vfoA.imode = vfo->imode = nu_mode;
			selrig->adjust_bandwidth(vfo->imode);
			nu_BW = selrig->get_bwA();
			vfoA.iBW = vfo->iBW = nu_BW;

			Fl::awake(setModeControl);
			set_bandwidth_control();
			Fl::awake(updateUI);
		}
		if (selrig->twovfos()) {
			vfoB.imode = selrig->get_modeB();
			vfoB.filter = selrig->get_FILT(vfoB.imode);
		}

		vfoA.filter = selrig->get_FILT(vfoA.imode);
		Fl::awake(setFILTER);
	} else {
		rig_trace(2, "read_mode", "vfoB active");
		nu_mode = selrig->get_modeB();
		if (nu_mode != opMODE->index()) { //vfoB.imode) {
			vfoB.imode = vfo->imode = nu_mode;
			selrig->adjust_bandwidth(vfo->imode);
			nu_BW = selrig->get_bwB();
			vfoB.iBW = vfo->iBW = nu_BW;

			Fl::awake(setModeControl);
			set_bandwidth_control();
			Fl::awake(updateUI);
		}

		if (selrig->twovfos()) {
			vfoA.imode = selrig->get_modeA();
			vfoA.filter = selrig->get_FILT(vfoA.imode);
		}

		vfoB.filter = selrig->get_FILT(vfoA.imode);
		Fl::awake(setFILTER);
	}
}

void setBWControl(void *)
{
	if (selrig->has_dsp_controls) {
		if (opDSP_lo->isbusy() || opDSP_hi->isbusy())
			return;
		if (vfo->iBW > 256) {
			opBW->index(0);
			opBW->hide();
			if (opDSP_lo->visible()) {
				opDSP_hi->index((vfo->iBW >> 8) & 0x7F);
				opDSP_hi->hide();
				opDSP_lo->index(vfo->iBW & 0xFF);
				opDSP_lo->show();
			} else {
				opDSP_hi->index((vfo->iBW >> 8) & 0x7F);
				opDSP_lo->index(vfo->iBW & 0xFF);
				opDSP_lo->hide();
				opDSP_hi->show();
			}
			btnDSP->label(selrig->SL_label);
			btnDSP->redraw_label();
			btnDSP->show();
		} else {
			opDSP_lo->index(0);
			opDSP_hi->index(0);
			opDSP_lo->hide();
			opDSP_hi->hide();
			btnDSP->hide();
			opBW->index(vfo->iBW);
			opBW->show();
		}
	}
	else {
		if (opBW->isbusy())
			return;
		opDSP_lo->hide();
		opDSP_hi->hide();
		btnDSP->hide();
		opBW->index(vfo->iBW);
		opBW->show();
	}
}

void read_bandwidth()
{
	if (xcvr_name == rig_K3.name_) {
		read_K3_bw();
		return;
	}
//	if (xcvr_name == rig_KX3.name_) {
//		read_KX3_bw();
//		return;
//	}

	trace(1,"read_bandwidth()");
	int nu_BW;
	if (selrig->inuse == onA) {
		trace(2, "vfoA active", "get_bwA()");
		nu_BW = selrig->get_bwA();
		if (nu_BW != vfoA.iBW) {
			std::stringstream s;
			s << "Bandwidth A change. nu_BW=" << nu_BW << ", vfoA.iBW=" << vfoA.iBW << ", vfo->iBW=" << vfo->iBW;
			trace(1, s.str().c_str());
			vfoA.iBW = vfo->iBW = nu_BW;
		}
	} else {
		trace(2, "vfoB active", "get_bwB()");
		nu_BW = selrig->get_bwB();
		if (nu_BW != vfoB.iBW) {
			std::stringstream s;
			s << "Bandwidth B change. nu_BW=" << nu_BW << ", vfoB.iBW=" << vfoB.iBW << ", vfo->iBW=" << vfo->iBW;
			trace(1, s.str().c_str());
			vfoB.iBW = vfo->iBW = nu_BW;
		}
	}
	Fl::awake(setBWControl);
}

// read current signal level
void read_smeter()
{
	if (!selrig->has_smeter) return;
	int  sig;
	{
		trace(1,"read_smeter()");
		sig = selrig->get_smeter();
	}
	if (sig == -1) return;
	smtrval = sig;
	Fl::awake(updateSmeter);
}

void read_voltmeter()
{
	if (!selrig->has_voltmeter) return;
	double sig;
	{
		trace(1, "read voltage");
		sig = selrig->get_voltmeter();
	}
	vmtrval = sig;
	Fl::awake(updateVmeter);
}

int tunerval = 0;
void update_UI_TUNER(void *)
{
	btn_tune_on_off->value(tunerval);
	btn_tune_on_off->redraw();
}

void read_tuner()
{
	if (!selrig->has_tune_control) return;

	tunerval = selrig->get_tune();
	if (tunerval != btn_tune_on_off->value()) {
		std::stringstream s;
		s << "tuner state: " << tunerval;
		trace(1, s.str().c_str());
		Fl::awake(update_UI_TUNER);
	}
}

// read power out
void read_power_out()
{
	if (!selrig->has_power_out) return;
	int sig;
	{
		trace(1,"read_power_out()");
		sig = selrig->get_power_out();
	}
	if (sig == -1) return;
	pwrval = sig;
	Fl::awake(updateFwdPwr);
}

// read swr
void read_swr()
{
	if ((meter_image != SWR_IMAGE) ||
		!selrig->has_swr_control) return;
	int sig;
	{
		trace(1,"read_swr()");
		sig = selrig->get_swr();
	}
	if (sig == -1) return;
	swrval = sig;
	Fl::awake(updateSWR);
}

// alc
void read_alc()
{
	if ((meter_image != ALC_IMAGE) ||
		!selrig->has_alc_control) return;
	int sig;
	{
		trace(1,"read_alc()");
		sig = selrig->get_alc();
	}
	if (sig < 0) return;
	alcval = sig;
	Fl::awake(updateALC);
}

// notch
void update_auto_notch(void *d)
{
	btnAutoNotch->value(progStatus.auto_notch);
}

void read_auto_notch()
{
	int val;
	if (!selrig->has_auto_notch) return;
	{
		trace(1,"read_auto_notch()");
		val = selrig->get_auto_notch();
	}
	if (val != progStatus.auto_notch) {
		progStatus.auto_notch = vfo->auto_notch = val;
		Fl::awake(update_auto_notch, (void*)0);
	}
}

// NOISE blanker
void update_noise(void *d)
{
	btnNOISE->value(progStatus.noise);
	btnNOISE->redraw();
	sldr_nb_level->value(progStatus.nb_level);
	sldr_nb_level->redraw();
}

void read_noise()
{
	int on = 0, val = 0;
	if (inhibit_nb_level > 0) {
		inhibit_nb_level--;
		return;
	}
	{
		trace(1,"read_noise()");
		on = selrig->get_noise();
		val = selrig->get_nb_level();
	}
	if ((on != progStatus.noise) || (val != progStatus.nb_level)) {
		vfo->noise = progStatus.noise = on;
		vfo->nb_level = progStatus.nb_level = val;
		Fl::awake(update_noise, (void*)0);
	}
}

// compression
void update_compression(void *d)
{
	if (btnCompON) btnCompON->value(progStatus.compON);
	if (spnr_compression) spnr_compression->value(progStatus.compression);
}

void read_compression()
{
	int on = progStatus.compON;
	int val = progStatus.compression;
	if (selrig->has_compression || selrig->has_compON) {
		{
			trace(1,"read_compression()");
			selrig->get_compression( on, val );
		}
		if (on != progStatus.compON || val != progStatus.compression) {
			vfo->compression = progStatus.compression = val;
			vfo->compON = progStatus.compON = on;
			Fl::awake(update_compression, (void*)0);
		}
	}
}

// preamp - attenuator
void update_preamp(void *d)
{
	btnPreamp->value(progStatus.preamp);
}

void update_attenuator(void *d)
{
	btnAttenuator->value(progStatus.attenuator);
	btnAttenuator->redraw();
}

void read_preamp()
{
	int val;
	if (selrig->has_preamp_control) {
		trace(1,"read_preamp_att()  1");
		val = selrig->get_preamp();
		vfo->preamp = progStatus.preamp = val;
		Fl::awake(update_preamp, (void*)0);
	}
}

void read_att()
{
	int val;
	if (selrig->has_attenuator_control) {
		trace(1,"read_preamp_att()  2");
		val = selrig->get_attenuator();
		vfo->attenuator = progStatus.attenuator = val;
		Fl::awake(update_attenuator, (void*)0);
	}
}

// split
void update_split(void *d)
{
	if (xcvr_name == rig_FTdx1200.name_ ||
		xcvr_name == rig_TS480SAT.name_ || xcvr_name == rig_TS480HX.name_ ||
		xcvr_name == rig_TS590S.name_ || xcvr_name == rig_TS590SG.name_ ||
		xcvr_name == rig_TS890S.name_ ||
		xcvr_name == rig_TS2000.name_ || xcvr_name == rig_TS990.name_) {
		switch (progStatus.split) {
			case 0: btnSplit->value(0);
					highlight_vfo(NULL);
					break;
			case 1: btnSplit->value(1);
					highlight_vfo(NULL);
					break;
			case 2:	btnSplit->value(1);
					highlight_vfo(NULL);
					break;
			case 3: btnSplit->value(0);
					highlight_vfo(NULL);
					break;
		}
	} else
		btnSplit->value(progStatus.split);
	btnSplit->redraw();
}

void read_split()
{
	int val = progStatus.split;
	if (selrig->has_split) {
		val = selrig->get_split();
		vfo->split = progStatus.split = val;
		Fl::awake(update_split, (void*)0);
		std::ostringstream s;
		s << "read_split() " << (val ? "ON" : "OFF");
		trace(1, s.str().c_str());
	} else {
		vfo->split = progStatus.split;
	}
}

// volume
void update_volume(void *d)
{
	long *nr = (long *)d;
	if (spnrVOLUME) {
		spnrVOLUME->value(progStatus.volume);
		spnrVOLUME->activate();
		spnrVOLUME->redraw();
	}

	if (sldrVOLUME) {
		sldrVOLUME->value(progStatus.volume); // Set slider to last known value
		sldrVOLUME->activate();				  // activate it
		sldrVOLUME->redraw();
	}

	if (*nr) btnVol->value(1);			  // Button Lit
	else     btnVol->value(0);			  // Button Dark.
	btnVol->redraw();
}

long nlzero = 0L;
long nlone = 1L;

void read_volume()
{
	if (inhibit_volume > 0) {
		inhibit_volume--;
		return;
	}
	if (!selrig->has_volume_control) return;
	int vol;
	{
		trace(1,"read_volume()");
		vol = selrig->get_volume_control();
	}
	if (vol != progStatus.volume) {
		if (vol <= 1 && !btnVol->value()) return;
		progStatus.volume = vol;
		if (vol <= 1 && btnVol->value())
			Fl::awake(update_volume, (void*)&nlzero);
		else
			Fl::awake(update_volume, (void*)&nlone);
	}
}

// ifshift
void update_ifshift(void *d)
{
	btnIFsh->value(progStatus.shift);
	btnIFsh->redraw();
	if (sldrIFSHIFT) {
		sldrIFSHIFT->value(progStatus.shift_val);
		sldrIFSHIFT->redraw();
	}
	if (spnrIFSHIFT) {
		spnrIFSHIFT->value(progStatus.shift_val);
		spnrIFSHIFT->redraw();
	}
}

void update_pbt(void *)
{
	if (sldrINNER) {
		sldrINNER->value(progStatus.pbt_inner);
		sldrINNER->redraw();
	}
	if (sldrOUTER) {
		sldrOUTER->value(progStatus.pbt_outer);
		sldrOUTER->redraw();
	}
}

void read_pbt()
{
	if (inhibit_pbt > 0) {
		inhibit_pbt--;
		return;
	}
	progStatus.pbt_inner = selrig->get_pbt_inner();
	progStatus.pbt_outer = selrig->get_pbt_outer();
	Fl::awake(update_pbt, (void*)0);
}

void read_ifshift()
{
	int on = 0;
	int val = 0;
	if (selrig->has_pbt_controls)
		return read_pbt();

	if (inhibit_shift > 0) {
		inhibit_shift--;
		return;
		}
	if (!selrig->has_ifshift_control) return;

	trace(1,"read_if_shift()");
	on = selrig->get_if_shift(val);

	if (xcvr_name == rig_KX3.name_) {
		vfo->shift_val = progStatus.shift_val = val;
		Fl::awake(update_ifshift, (void*)0);
	} else if ((on != progStatus.shift) || (val != progStatus.shift_val)) {
		vfo->if_shift = progStatus.shift = on;
		vfo->shift_val = progStatus.shift_val = val;
		Fl::awake(update_ifshift, (void*)0);
	}
}

// noise reduction
void update_nr(void *d)
{
	btnNR->value(progStatus.noise_reduction);
	btnNR->redraw();
	if (sldrNR) {
		sldrNR->value(progStatus.noise_reduction_val);
		sldrNR->redraw();
	}
	if (spnrNR) {
		spnrNR->value(progStatus.noise_reduction_val);
		spnrNR->redraw();
	}
}

void read_nr()
{
	int on = 0;
	int val = 0;
	if (inhibit_nr > 0) {
		inhibit_nr--;
		return;
	}
	if (!selrig->has_noise_reduction) return;
	{
		trace(1,"read_nr()");
		on = selrig->get_noise_reduction();
		val = selrig->get_noise_reduction_val();
	}
	if ((on != progStatus.noise_reduction) || (val != progStatus.noise_reduction_val)) {
		vfo->nr = progStatus.noise_reduction = on;
		vfo->nr_val = progStatus.noise_reduction_val = val;
		Fl::awake(update_nr, (void*)0);
	}
}

// manual notch
void update_notch(void *d)
{
	btnNotch->value(progStatus.notch);
	btnNotch->redraw();
	if (sldrNOTCH) {
		sldrNOTCH->value(progStatus.notch_val);
		sldrNOTCH->redraw();
	}
	if (spnrNOTCH) {
		spnrNOTCH->value(progStatus.notch_val);
		spnrNOTCH->redraw();
	}
}

void read_notch()
{
	if (!selrig->has_notch_control) return;

	int on = progStatus.notch;
	int val = progStatus.notch_val;
	{
		trace(1,"read_notch()");
		on = selrig->get_notch(val);
	}

	if ((on != progStatus.notch) || (val != progStatus.notch_val)) {
		vfo->notch_val = progStatus.notch_val = val;
		vfo->notch = progStatus.notch = on;
		Fl::awake(update_notch, (void*)0);
	}
}

// power_control

void update_power_control(void *d)
{
	double min, max, step;

	if (xcvr_name == rig_K2.name_ || xcvr_name == rig_KX3.name_) {
		guard_lock serial(&mutex_serial);
		selrig->get_pc_min_max_step(min, max, step);

		if (sldrPOWER) {
			sldrPOWER->minimum(min);
			sldrPOWER->maximum(max);
			sldrPOWER->step(step);
		}

		if (spnrPOWER) {
			spnrPOWER->minimum(min);
			spnrPOWER->maximum(max);
			spnrPOWER->step(step);
		}
	}

	if (sldrPOWER) {
		sldrPOWER->value(progStatus.power_level);
		sldrPOWER->redraw();
	}

	if (spnrPOWER) {
		spnrPOWER->value(progStatus.power_level);
		spnrPOWER->redraw();
	}
}

void read_power_control()
{
	if (inhibit_power > 0) {
		inhibit_power--;
		return;
	}
	if (!selrig->has_power_control) return;

	trace(1,"read_power_control()");
	vfo->power_control = progStatus.power_level = selrig->get_power_control();

	Fl::awake(update_power_control, (void*)0);

}

// mic gain
void update_mic_gain(void *d)
{
	if (sldrMICGAIN) {
		sldrMICGAIN->value(progStatus.mic_gain);
		sldrMICGAIN->redraw();
	}
	if (spnrMICGAIN) {
		spnrMICGAIN->value(progStatus.mic_gain);
		spnrMICGAIN->redraw();
	}
}

void read_mic_gain()
{
	int val;
	if (inhibit_mic > 0) {
		inhibit_mic--;
		return;
	}
	if (!selrig->has_micgain_control) return;
	{
		trace(1,"read_mic_gain()");
		val = selrig->get_mic_gain();
	}
	if (val != progStatus.mic_gain || val != vfo->mic_gain || (sldrMICGAIN && val != sldrMICGAIN->value())) {
		vfo->mic_gain = progStatus.mic_gain = val;
		Fl::awake(update_mic_gain, (void*)0);
	}
}

void read_agc()
{
	int val;
	if (!selrig->has_agc_control) return;
	trace(1,"read_agc()");
	val = selrig->get_agc();
	if (val != progStatus.agc_level) {
		vfo->agc_level = progStatus.agc_level = val;
		Fl::awake(setAGC);
	}
}

// rf gain
void update_rfgain(void *d)
{
	if (sldrRFGAIN) {
		sldrRFGAIN->value(progStatus.rfgain);
		sldrRFGAIN->redraw();
	}
	if (spnrRFGAIN) {
		spnrRFGAIN->value(progStatus.rfgain);
		spnrRFGAIN->redraw();
	}
}

void read_rfgain()
{
	int val;
	if (inhibit_rfgain > 0) {
		inhibit_rfgain--;
		return;
	}
	if (selrig->has_rf_control) {
		trace(1,"read_rfgain");
		val = selrig->get_rf_gain();
		if (val != progStatus.rfgain) {
			progStatus.rfgain = val;
			Fl::awake(update_rfgain, (void*)0);
		}
	}
	read_agc();
}

// squelch
void update_squelch(void *d)
{
	if (sldrSQUELCH) {
		sldrSQUELCH->value(progStatus.squelch);
		sldrSQUELCH->redraw();
	}
	if (spnrSQUELCH) {
		spnrSQUELCH->value(progStatus.squelch);
		spnrSQUELCH->redraw();
	}
}

void read_squelch()
{
	int val;
	if (inhibit_squelch > 0) {
		inhibit_squelch--;
		return;
	}
	if (!selrig->has_sql_control) return;
	{
		trace(1,"read_squelch()");
		val = selrig->get_squelch();
	}

	if (val != progStatus.squelch || val != vfo->squelch || (sldrSQUELCH && val != sldrSQUELCH->value())) {
		vfo->squelch = progStatus.squelch = val;
		Fl::awake(update_squelch, (void*)0);
	}
}

void set_ptt(void *d)
{
	if (d == (void*)0) {
		btnPTT->value(0);
		sldrSWR->hide();
		sldrSWR->redraw();
		sldrRcvSignal->show();
		sldrRcvSignal->redraw();
		btnALC_SWR->hide();
		scaleSmeter->show();
	} else {
		btnPTT->value(1);
		sldrRcvSignal->hide();
		sldrRcvSignal->redraw();
		scaleSmeter->hide();
		sldrSWR->show();
		sldrSWR->redraw();
		btnALC_SWR->image(meter_image == SWR_IMAGE ? image_swr : image_alc);
		btnALC_SWR->redraw();
		btnALC_SWR->show();
	}
}

void check_ptt()
{
	if (selrig->name_ == rig_FT817.name_ ||
		selrig->name_ == rig_FT817BB.name_ ||
		selrig->name_ == rig_FT818ND.name_ ) {
		return;
	}
	int chk = ptt_state();
	if (chk != PTT) {
		PTT = chk;
		Fl::awake(set_ptt, (void *)PTT);
	}
}

void check_break_in()
{
	if (selrig->has_cw_break_in)
		selrig->get_break_in();
}

//static bool resetrcv = true;
//static bool resetxmt = true;

// On early model Yaesu transceivers, the mode must be set before VFO,
// since mode changes can shift frequency.
//
// For example, might set freq to 7123.000, but then change mode from USB
// to DATA-U.  This mode shift would change the VFO to  7123.700, instead
// of the desired 7123.000.
//
// Setting VFO after the mode change will prevent this type of frequency
// shifting.
void yaesu891UpdateA(XCVR_STATE * newVfo)
{
	selrig->set_modeA(newVfo->imode);
	selrig->set_vfoA(newVfo->freq);
	selrig->set_bwA(newVfo->iBW);
	selrig->get_modeA();
	selrig->get_vfoA();
	selrig->get_bwA();
}

void yaesu891UpdateB(XCVR_STATE * newVfo)
{
	selrig->set_modeB(newVfo->imode);
	selrig->set_vfoB(newVfo->freq);
	selrig->set_bwB(newVfo->iBW);
	selrig->get_modeB();
	selrig->get_vfoB();
	selrig->get_bwB();
}

void serviceQUE()
{
	guard_lock que_lock(&mutex_srvc_reqs, "serviceQUE");
	guard_lock serial(&mutex_serial);

	std::queue<VFOQUEUE> pending; // creates an empty queue

	VFOQUEUE nuvals;

	while (!srvc_reqs.empty()) {
		{
			nuvals = srvc_reqs.front();
			srvc_reqs.pop();
		}

		if (nuvals.change == ON || nuvals.change == OFF) { // PTT processing
			if (selrig->ICOMmainsub && selrig->inuse == onB) {  // disallowed operation
				Fl::awake(update_UI_PTT);
				return;
			}
			PTT = (nuvals.change == ON);
			if (nuvals.change == ON) trace(1,"ptt ON");
			else trace(1,"ptt OFF");
			rigPTT(PTT);
			{
				bool get = ptt_state();
				int cnt = 0;
				while ((get != PTT) && (cnt++ < 100)) {
					MilliSleep(10);
					get = ptt_state();
				}
				std::stringstream s;
				s << "ptt returned " << get << " in " << cnt * 10 << " msec";
				trace(1, s.str().c_str());
				Fl::awake(update_UI_PTT);
			}
			continue;
		}

		if (PTT) {
			if ((nuvals.vfo.iBW != 255) ||
				 (nuvals.vfo.imode != -1)) {
				pending.push(nuvals);
				continue; // while (!srvc_reqs.empty())
			}
		}

		switch (nuvals.change) {
			case vX:
				if (selrig->inuse == onB)
					serviceB(nuvals.vfo);
				else
					serviceA(nuvals.vfo);
				break;
			case vA:
				serviceA(nuvals.vfo);
				break;
			case vB:
				serviceB(nuvals.vfo);
				break;
			case sA: // select A
			{
				selrig->selectA();
				vfo = &vfoA;
				if (selrig->name_ == rig_FT891.name_) {
					// Restore mode, then freq and bandwidth after select
					yaesu891UpdateA(&vfoA);
				}
				trace(2, "case sA ", printXCVR_STATE(vfoA).c_str());
				rig_trace(2, "case sA ", printXCVR_STATE(vfoA).c_str());
				Fl::awake(updateUI);
			}
				break;
			case sB: // select B
			{
				selrig->selectB();
				vfo = &vfoB;
				if (selrig->name_ == rig_FT891.name_) {
					// Restore mode, then freq and bandwidth after select
					yaesu891UpdateB(&vfoB);
				}
				trace(2, "case sB ", printXCVR_STATE(vfoB).c_str());
				rig_trace(2, "case sB ", printXCVR_STATE(vfoB).c_str());
				Fl::awake(updateUI);
			}
				break;
			case sON: case sOFF:
			{
				int on = 0;
				if (nuvals.change == sON) on = 1;
				trace(1, (on ? "split ON" : "split OFF"));
				rig_trace(2, "case sB ", printXCVR_STATE(vfoB).c_str());
				if (selrig->can_split() || selrig->has_split_AB) {
					selrig->set_split(on);
					selrig->get_split();
					progStatus.split = on;
					Fl::awake(update_split, (void *)0);
					if (selrig->ICOMmainsub) {
						selrig->selectA();
						vfo = &vfoA;
					}
				}
			}
				break;
			case SWAP:
				trace(1, "execute swapab()");
				rig_trace(1, "execute swapab()");
				execute_swapAB();
				break;
			case A2B:
				trace(1, "execute A2B()");
				rig_trace(1, "execute A2B()");
				execute_A2B();
				break;
			case FA2FB:
				trace(1, "execute FA2FB");
				rig_trace(1, "execute FA2FB");
				execute_FA2FB();
				break;
			case FB2FA:
				trace(1, "execute FB2FA");
				rig_trace(1, "execute FB2FA");
				execute_FB2FA();
				break;
			default:
				trace(2, "default ", printXCVR_STATE(nuvals.vfo).c_str());
				if (selrig->inuse == onB) serviceB(nuvals.vfo);
				else serviceA(nuvals.vfo);
				break;
		}
	}

	{
		while (!srvc_reqs.empty()) {
			pending.push(srvc_reqs.front());
			srvc_reqs.pop();
		}
		while (!pending.empty()) {
			srvc_reqs.push(pending.front());
			pending.pop();
		}
	}
	Fl::awake(updateUI);
}

void find_bandwidth(XCVR_STATE &nuvals)
{
	if (nuvals.iBW == 255) return;
	if (!selrig->has_bandwidth_control) {
		nuvals.iBW = 255;
		return;
	}
	if (nuvals.iBW > 65536) {
		nuvals.iBW /= 256;
		nuvals.iBW /= 256;
		int i = 0;
		while (	selrig->bandwidths_[i] &&
				atol(selrig->bandwidths_[i]) < nuvals.iBW) {
			i++;
		}
		if (!selrig->bandwidths_[i]) i--;
		nuvals.iBW = i;
	}
}

void serviceA(XCVR_STATE nuvals)
{
	bool A_changed;
	if (nuvals.freq == 0) nuvals.freq = vfoA.freq;
	if (nuvals.imode == -1) nuvals.imode = vfoA.imode;
	if (nuvals.iBW == 255) nuvals.iBW = vfoA.iBW;

	if (selrig->inuse == onB) {
		if (selrig->can_change_alt_vfo) {
			trace(2, "B active, set alt vfo A", printXCVR_STATE(nuvals).c_str());
			rig_trace(2, "B active, set alt vfo A", printXCVR_STATE(nuvals).c_str());
			if (vfoA.imode != nuvals.imode) {
				if (selrig->name_ == rig_FT891.name_) {
					// Mode change on ft891 requires mode first, so set all values
					yaesu891UpdateA(&nuvals);
					vfoA = nuvals;
				} else {
					selrig->set_modeA(nuvals.imode);
					selrig->get_modeA();
				}
			}
			if (vfoA.iBW != nuvals.iBW) {
				selrig->set_bwA(nuvals.iBW);
				selrig->get_bwA();
				A_changed = true;
			}
			if (vfoA.freq != nuvals.freq) {
				selrig->set_vfoA(nuvals.freq);
				selrig->get_vfoA();
				A_changed = true;
			}
			vfoA = nuvals;
		} else if (xcvr_name != rig_TT550.name_) {
			trace(2, "B active, set vfo A", printXCVR_STATE(nuvals).c_str());
			rig_trace(2, "B active, set vfo A", printXCVR_STATE(nuvals).c_str());
			selrig->selectA();
			if (vfoA.imode != nuvals.imode) {
				selrig->set_modeA(nuvals.imode);
				selrig->get_modeA();
				A_changed = true;
//				Fl::awake(updateUI);
			}
			if (vfoA.iBW != nuvals.iBW) {
				selrig->set_bwA(nuvals.iBW);
				selrig->get_bwA();
				A_changed = true;
			}
			if (vfoA.freq != nuvals.freq) {
				selrig->set_vfoA(nuvals.freq);
				selrig->get_vfoA();
				A_changed = true;
			}
			selrig->selectB();
			vfoA = nuvals;
		}
		if (A_changed)
			Fl::awake(setFreqDispA);
		return;
	}

	trace(2, "service VFO A", printXCVR_STATE(nuvals).c_str());

	if ((nuvals.imode != -1) && (vfoA.imode != nuvals.imode)) {
		if (selrig->name_ == rig_FT891.name_) {
			// Mode change on ft891 can change frequency, so set all values
			yaesu891UpdateA(&nuvals);
			vfoA = nuvals;
			set_bandwidth_control();
		} else {
			std::string m1, m2;
			m1 = selrig->modes_[nuvals.imode];
			m2 = selrig->modes_[vfoA.imode];
			selrig->set_modeA(vfoA.imode = nuvals.imode);
			selrig->get_modeA();
			set_bandwidth_control();
			selrig->set_bwA(vfoA.iBW);
			selrig->get_bwA();
			if (m1.find("CW") != std::string::npos ||
				m2.find("CW") != std::string::npos)
				vfoA.freq = nuvals.freq = selrig->get_vfoA();
		}

	}
	if (vfoA.iBW != nuvals.iBW) {
		selrig->set_bwA(vfoA.iBW = nuvals.iBW);
		selrig->get_bwA();
		A_changed = true;
	}
	if (vfoA.freq != nuvals.freq) {
		trace(1, "change vfoA frequency");
		selrig->set_vfoA(vfoA.freq = nuvals.freq);
		selrig->get_vfoA();
		A_changed = true;
}
	vfo = &vfoA;

	if (A_changed)
		Fl::awake(setFreqDispA);
}

void serviceB(XCVR_STATE nuvals)
{
	bool B_changed;
	if (nuvals.freq == 0) nuvals.freq = vfoB.freq;
	if (nuvals.imode == -1) nuvals.imode = vfoB.imode;
	if (nuvals.iBW == 255) nuvals.iBW = vfoB.iBW;

	if (selrig->inuse == onA) {
		if (selrig->can_change_alt_vfo) {
			trace(2, "A active, set alt vfo B", printXCVR_STATE(nuvals).c_str());
			if (vfoB.imode != nuvals.imode) {
				if (selrig->name_ == rig_FT891.name_) {
					// Mode change on ft891 requires mode first, so set all values
					yaesu891UpdateB(&nuvals);
					vfoB = nuvals;
				} else {
					selrig->set_modeB(nuvals.imode);
					selrig->get_modeB();
				}
			}
			if (vfoB.iBW != nuvals.iBW) {
				selrig->set_bwB(nuvals.iBW);
				selrig->get_bwB();
				B_changed = true;
			}
			if (vfoB.freq != nuvals.freq) {
				selrig->set_vfoB(nuvals.freq);
				selrig->get_vfoB();
				B_changed = true;
			}
			vfoB = nuvals;
		} else if (xcvr_name != rig_TT550.name_) {
			trace(2, "A active, set vfo B", printXCVR_STATE(nuvals).c_str());
			selrig->selectB();
			if (vfoB.imode != nuvals.imode) {
				selrig->set_modeB(nuvals.imode);
				selrig->get_modeB();
				B_changed = true;
			}
			if (vfoB.iBW != nuvals.iBW) {
				selrig->set_bwB(nuvals.iBW);
				selrig->get_bwB();
				B_changed = true;
			}
			if (vfoB.freq != nuvals.freq) {
				selrig->set_vfoB(nuvals.freq);
				selrig->get_vfoB();
				B_changed = true;
			}
			selrig->selectA();
			vfoB = nuvals;
		}
		if (B_changed)
			Fl::awake(setFreqDispB);
		return;
	}

	trace(2, "service VFO B", printXCVR_STATE(nuvals).c_str());

	if ((nuvals.imode != -1) && (vfoB.imode != nuvals.imode)) {
		std::string m1, m2;
		m1 = selrig->modes_[nuvals.imode];
		m2 = selrig->modes_[vfoB.imode];
		selrig->set_modeB(vfoB.imode = nuvals.imode);
		selrig->get_modeB();
		set_bandwidth_control();
		selrig->set_bwB(vfoB.iBW);
		selrig->get_bwB();
		if (m1.find("CW") != std::string::npos ||
			m2.find("CW") != std::string::npos)
			vfoB.freq = nuvals.freq = selrig->get_vfoB();
	}
	if (vfoB.iBW != nuvals.iBW) {
		selrig->set_bwB(vfoB.iBW = nuvals.iBW);
		selrig->get_bwB();
		B_changed = true;
	}
	if (vfoB.freq != nuvals.freq) {
		selrig->set_vfoB(vfoB.freq = nuvals.freq);
		selrig->get_vfoB();
		B_changed = true;
	}

	vfo = &vfoB;

	if (B_changed)
		Fl::awake(setFreqDispB);

}

#define MAX_FAILURES 5

void serial_failed(void *)
{
	bypass_serial_thread_loop = true;
	RigSerial->ClosePort();

	box_xcvr_connect->color(FL_BACKGROUND2_COLOR);
	box_xcvr_connect->redraw();

	fl_alert2(_("\
Transceiver not responding!\n\n\
Check serial (COM) port connection\n\
Open menu Config/Setup/Transceiver\n\
Press 'Update' button, reselect port\n\
Check that Baud matches transceiver baud\n\n\
Press 'Init' button."));
}

struct POLL_PAIR {
	int *poll;
	void (*pollfunc)();
	std::string name;
};

POLL_PAIR RX_poll_group_1[] = {
	{&progStatus.poll_smeter, read_smeter, "SMETER"},
	{&progStatus.poll_frequency, read_vfo, "FREQ"},
	{NULL, NULL}
};

POLL_PAIR RX_poll_group_2[] = {
	{&progStatus.poll_mode, read_mode, "MODE"},
	{&progStatus.poll_bandwidth, read_bandwidth, "BW"},
//	{&progStatus.poll_vfoAorB, read_vfoAorB, "A/B"},
	{NULL, NULL}
};

POLL_PAIR RX_poll_group_3[] = {
	{&progStatus.poll_mode, read_voltmeter, "voltage"},
	{&progStatus.poll_tuner, read_tuner, "tuner"},
	{&progStatus.poll_volume, read_volume, "volume"},
	{&progStatus.poll_auto_notch, read_auto_notch, "auto notch"},
	{&progStatus.poll_notch, read_notch, "notch"},
	{&progStatus.poll_ifshift, read_ifshift, "if shift"},
	{&progStatus.poll_power_control, read_power_control, "power"},
	{&progStatus.poll_pre_att, read_preamp, "preamp"},
	{&progStatus.poll_pre_att, read_att, "atten"},
	{&progStatus.poll_micgain, read_mic_gain, "mic gain"},
	{&progStatus.poll_squelch, read_squelch, "squelch"},
	{&progStatus.poll_rfgain, read_rfgain, "rfgain"},
	{&progStatus.poll_split, read_split, "split"},
	{&progStatus.poll_nr, read_nr, "noise reduction"},
	{&progStatus.poll_noise, read_noise, "noise"},
	{&progStatus.poll_compression, read_compression, "compression"},
	{&progStatus.poll_break_in, check_break_in, "break-in"},
	{NULL, NULL}
};

POLL_PAIR TX_poll_pairs[] = {
	{&progStatus.poll_power_control, read_power_control, "power"},
	{&progStatus.poll_pout, read_power_out, "pout"},
	{&progStatus.poll_swr, read_swr, "swr"},
	{&progStatus.poll_alc, read_alc, "alc"},
	{&progStatus.poll_mode, read_voltmeter, "voltage"},
	{&progStatus.poll_split, read_split, "split"},
	{NULL, NULL}
};

void * serial_thread_loop(void *d)
{
	POLL_PAIR *rx_poll_group_1 = &RX_poll_group_1[0];
	POLL_PAIR *rx_poll_group_2 = &RX_poll_group_2[0];
	POLL_PAIR *rx_poll_group_3 = &RX_poll_group_3[0];
	POLL_PAIR *tx_polling = &TX_poll_pairs[0];
	bool isRX = false;

	for(;;) {

		MilliSleep(progStatus.serloop_timing);

		if (!run_serial_thread) {
			break;
		}

		if (bypass_serial_thread_loop || disable_polling->value()) {
			goto serial_bypass_loop;
		}

		if (RigSerial->failed() >= MAX_FAILURES) {
			Fl::awake(serial_failed);
		}

//send any freq/mode/bw changes in the queu
		if (!srvc_reqs.empty()) {
			serviceQUE();
		}

		if (progStatus.poll_ptt) {
			guard_lock lk(&mutex_serial);
			check_ptt();
		}

		if (progStatus.byte_interval) MilliSleep(progStatus.byte_interval);

		if (PTT || cwio_process == SEND || cwio_process == CALIBRATE) {
			if (isRX) {
				isRX = false;
				smtrval = 0;
				Fl::awake(update_UI_PTT);
				Fl::awake(updateSmeter);
			}

			if (progStatus.poll_frequency) {
				guard_lock lk(&mutex_serial);
				read_vfo();
			}

			if (progStatus.byte_interval)
				MilliSleep(progStatus.byte_interval);

			{	guard_lock lk(&mutex_serial);
				while (!bypass_serial_thread_loop) {
					if (*(tx_polling->poll)) {
						(tx_polling->pollfunc)();
						++tx_polling;
						if (tx_polling->poll == NULL)
							tx_polling = &TX_poll_pairs[0];
						break;
					}
					++tx_polling;
					if (tx_polling->poll == NULL) {
						tx_polling = &TX_poll_pairs[0];
						break;
					}
				}

			}

		} else {
			if (!isRX) {
				isRX = true;
				Fl::awake(update_UI_PTT);
				Fl::awake(zeroXmtMeters, 0);
			}

			if (!bypass_serial_thread_loop) {
				{	guard_lock lk(&mutex_serial);
					while (rx_poll_group_1->poll != NULL) {
						(rx_poll_group_1->pollfunc)();
						if (progStatus.byte_interval)
							MilliSleep(progStatus.byte_interval);
						++rx_poll_group_1;
					}
					rx_poll_group_1 = &RX_poll_group_1[0];
				}
			}

			if (!bypass_serial_thread_loop) {
				{	guard_lock lk(&mutex_serial);
					if (*(rx_poll_group_2->poll))
						(rx_poll_group_2->pollfunc)();
					++rx_poll_group_2;
					if (rx_poll_group_2->poll == NULL)
						rx_poll_group_2 = &RX_poll_group_2[0];
				}
			}

			if (progStatus.byte_interval)
				MilliSleep(progStatus.byte_interval);

			if (!bypass_serial_thread_loop) {
				{	guard_lock lk(&mutex_serial);
					if (*(rx_poll_group_3->poll)) {
						(rx_poll_group_3->pollfunc)();
					}
					++rx_poll_group_3;
					if (rx_poll_group_3->poll == NULL) {
						rx_poll_group_3 = &RX_poll_group_3[0];
					}
				}
			}
		}
serial_bypass_loop: ;
	}
	return NULL;

}

//=============================================================================

void setBW()
{
	XCVR_STATE fm = *vfo;
	fm.src = UI;
	fm.iBW = opBW->index();
	guard_lock que_lock( &mutex_srvc_reqs, "setBW" );
	srvc_reqs.push( VFOQUEUE((selrig->inuse == onB ? vB : vA), fm));
}

void setDSP()
{
	XCVR_STATE fm = *vfo;
	fm.src = UI;
	fm.iBW = ((opDSP_hi->index() << 8) | 0x8000) | (opDSP_lo->index() & 0xFF) ;
	guard_lock que_lock( & mutex_srvc_reqs, "setDSP" );
	srvc_reqs.push ( VFOQUEUE((selrig->inuse == onB ? vB : vA), fm));
}

void selectDSP()
{
	if (btnDSP->label()[0] == selrig->SL_label[0]) {
		btnDSP->label(selrig->SH_label);
		btnDSP->redraw_label();
		opDSP_hi->show();
		opDSP_lo->hide();
	} else {
		btnDSP->label(selrig->SL_label);
		btnDSP->redraw_label();
		opDSP_lo->show();
		opDSP_hi->hide();
	}
}

void selectFILT()
{
	guard_lock lock(&mutex_serial);
	btnFILT->label(selrig->nextFILT());
	btnFILT->redraw_label();
}

// set_bandwidth_control updates iBW and then posts the call for
// the UI thread to updateBandwidthControl
// changes to the UI cannot come from any thread other than the
// main FL thread!  Doing otherwise can cause what appears to be
// random program crashes.

void set_bandwidth_control()
{
	if (!selrig->has_bandwidth_control) return;

	vfo->iBW = selrig->def_bandwidth(vfo->imode);

	if (vfo->iBW < 256) {
		int i = 0;
		while (selrig->bandwidths_[i]) i++;
		i--;
		if (vfo->iBW > i) vfo->iBW = selrig->def_bandwidth(vfo->imode);
	}
	selrig->inuse == onB ? vfoB.iBW = vfo->iBW : vfoA.iBW = vfo->iBW;
	Fl::awake(updateBandwidthControl);
}

void updateBandwidthControl(void *d)
{
	if (selrig->has_bandwidth_control) {
		if (selrig->adjust_bandwidth(vfo->imode) != -1) {
			opBW->clear();
			rigbws_.clear();
			for (int i = 0; selrig->bandwidths_[i] != NULL; i++) {
				rigbws_.push_back(selrig->bandwidths_[i]);
				opBW->add(selrig->bandwidths_[i]);
			}

			if (selrig->has_dsp_controls) {
				opDSP_lo->clear();
				opDSP_hi->clear();
				for (int i = 0; selrig->dsp_SL[i] != NULL; i++)
					opDSP_lo->add(selrig->dsp_SL[i]);
				for (int i = 0; selrig->dsp_SH[i] != NULL; i++)
					opDSP_hi->add(selrig->dsp_SH[i]);
				if (vfo->iBW > 256) {
					opBW->index(0);
					opBW->hide();
					opBW->hide();
					opDSP_lo->index(vfo->iBW & 0xFF);
					opDSP_lo->hide();
					opDSP_hi->index((vfo->iBW >> 8) & 0x7F);
					btnDSP->label(selrig->SL_label);
					opDSP_lo->show();
					btnDSP->show();
					btnFILT->hide();
				} else {
					opDSP_lo->hide();
					opDSP_hi->hide();
					btnDSP->hide();
					btnFILT->hide();
					opBW->index(vfo->iBW);
					opBW->show();
				}
			} else {  // no DSP control so update BW control, hide DSP
				opDSP_lo->hide();
				opDSP_hi->hide();
				btnDSP->hide();
				opBW->index(vfo->iBW);
				if (selrig->has_FILTER) {
					btnFILT->show();
					btnFILT->label(
						selrig->FILT(
							selrig->inuse == onB ? vfoB.filter : vfoA.filter));
					btnFILT->redraw_label();
					opBW->resize(opDSP_lo->x(), opDSP_lo->y(), opDSP_lo->w(), opDSP_lo->h());
					opBW->redraw();
				}
				opBW->show();
			}
			// Allow BW to receive rig updates as value is changed there, without needing
			// to click the dropdown first
			opBW->isbusy(false);
		}
	} else { // no BW, no DSP controls
		opBW->index(0);
		opBW->hide();
		btnFILT->hide();
		opDSP_lo->index(0);
		opDSP_hi->index(0);
		btnDSP->hide();
		opDSP_lo->hide();
		opDSP_hi->hide();
	}
}

void setMode()
{
	XCVR_STATE fm = *vfo;
	fm.imode = opMODE->index();
	fm.iBW = selrig->def_bandwidth(fm.imode);
	fm.src = UI;
	if (selrig->has_FILTER) {
		fm.filter = selrig->get_FILT(fm.imode);
		if (selrig->inuse == onB) fm.filter = selrig->get_FILT(fm.imode);
	}

	guard_lock que_lock( &mutex_srvc_reqs, "setMode" );
	srvc_reqs.push(VFOQUEUE( (selrig->inuse == onB ? vB : vA), fm));
}

void sortList() {
	if (!numinlist) return;
	ATAG_XCVR_STATE temp;
	for (int i = 0; i < numinlist - 1; i++)
		for (int j = i + 1; j < numinlist; j++)
			if (oplist[i].freq > oplist[j].freq) {
					temp = oplist[i];
					oplist[i] = oplist[j];
					oplist[j] = temp;
			}
}

void clearList() {
	if (!numinlist) return;
	for (int i = 0; i < LISTSIZE; i++) {
		oplist[i].freq = 0;
		oplist[i].imode = USB;
		oplist[i].iBW = 0;
		memset(oplist[i].alpha_tag, 0, ATAGSIZE);
	}
	FreqSelect->clear();
	numinlist = 0;
	inAlphaTag->value("");
}

void updateSelect() {
	char szline[1000];
	char szatag[ATAGSIZE];
	int i;
	FreqSelect->clear();
	if (!numinlist) return;
	sortList();
// stripe lines
	int bg1, bg2, bg_clr;
	bg1 = FL_WHITE; bg2 = FL_LIGHT2;

	for (int n = 0; n < numinlist; n++) {
		memset(szline, 0, sizeof(szline));
		memset(szatag, 0, sizeof(szatag));
		for (i = 0; i < ATAGSIZE; i++) {
			szatag[i] = oplist[n].alpha_tag[i];
			if (szatag[i] == '\n') szatag[i] = ' ';
		}
		bg_clr = (n % 2) ? bg1 : bg2;
		snprintf(szline, sizeof(szline), "\
@F%d@S%d@B%d@r%.3f\t\
@F%d@S%d@B%d@.|\t\
@F%d@S%d@B%d@r%s\t\
@F%d@S%d@B%d@.|\t\
@F%d@S%d@B%d@r%s\t\
@F%d@S%d@B%d@.|\t\
@F%d@S%d@B%d@.%s",
			progStatus.memfontnbr, progStatus.memfontsize, bg_clr, oplist[n].freq / 1000.0,
			progStatus.memfontnbr, progStatus.memfontsize, bg_clr,
			progStatus.memfontnbr, progStatus.memfontsize, bg_clr, selrig->get_bwname_(oplist[n].iBW, oplist[n].imode),
			progStatus.memfontnbr, progStatus.memfontsize, bg_clr,
			progStatus.memfontnbr, progStatus.memfontsize, bg_clr, selrig->get_modename_(oplist[n].imode),
			progStatus.memfontnbr, progStatus.memfontsize, bg_clr,
			progStatus.memfontnbr, progStatus.memfontsize, bg_clr, szatag );
		FreqSelect->add (szline);
	}
	inAlphaTag->value("");
}

void addtoList(int val, int imode, int iBW) {
	if (numinlist < LISTSIZE) {
		oplist[numinlist].imode = imode;
		oplist[numinlist].freq = val;
		oplist[numinlist].iBW = iBW;
		memset(oplist[numinlist].alpha_tag, 0, ATAGSIZE);
		numinlist++;
	}
}

void readFile() {
	std::ifstream iList(defFileName.c_str());
	if (!iList) {
		fl_message ("Could not open %s", defFileName.c_str());
		return;
	}
	clearList();
	int i = 0, mode, bw;
	long freq;
	while (!iList.eof()) {
		freq = 0L; mode = -1;
		iList >> freq >> mode >> bw;
		if (freq && (mode > -1)) {
			oplist[i].freq = freq;
			oplist[i].imode = mode;
			oplist[i].iBW = (bw == -1 ? 0 : bw);
			memset(oplist[i].alpha_tag, 0, ATAGSIZE);
			i++;
		}
	}
	iList.close();
	numinlist = i;
	updateSelect();
}

void readTagFile() {
	std::ifstream iList(defFileName.c_str());
	if (!iList) {
		fl_message ("Could not open %s", defFileName.c_str());
		return;
	}
	clearList();
	int i = 0, mode, bw;
	long freq;
	std::string atag;
	char ca[ATAGSIZE + 60];
	while (!iList.eof()) {
		freq = 0L; mode = -1;
		atag.clear();
		memset(ca, 0, sizeof(ca));
		iList >> freq >> mode >> bw;
		iList.getline(ca, sizeof(ca) - 1);
		atag = ca;
		if (freq && (mode > -1)) {
			oplist[i].freq = freq;
			oplist[i].imode = mode;
			oplist[i].iBW = (bw == -1 ? 0 : bw);
// trim leading, trailing spaces and double quotes
			atag = lt_trim(atag);
			snprintf(oplist[i].alpha_tag, ATAGSIZE, "%s", atag.c_str());
			i++;
		}
	}
	iList.close();
	numinlist = i;
	updateSelect();
}

void buildlist() {
	std::string tmpFN, orgFN;
// check for new Memory-Alpha-Tag file
	defFileName = RigHomeDir;
	defFileName.append(selrig->name_);
	defFileName.append(".mat");
	FILE *fh = fopen(defFileName.c_str(), "r");
	if (fh != NULL) {
		fclose (fh);
		readTagFile();
		return;
	}
// else only read original file to make new MAT file
	orgFN = RigHomeDir;
	orgFN.append(selrig->name_);
	orgFN.append(".arv");
	fh = fopen(orgFN.c_str(), "r");
	if (fh != NULL) {
		fclose (fh);
		tmpFN = defFileName;
		defFileName = orgFN;
		readFile();
		defFileName = tmpFN;
		return;
	}
	clearList();
}

// flrig front panel changed

int movFreqA() {
	guard_lock serial(&mutex_serial);

	if (!selrig->can_change_alt_vfo  && selrig->inuse == onB) {
		selrig->selectA();
		vfoA.freq = FreqDispA->value();
		selrig->set_vfoA(vfoA.freq);
		selrig->selectB();
	} else {
		vfoA.freq = FreqDispA->value();
		selrig->set_vfoA(vfoA.freq);
	}

	return 1;
}

int movFreqB() {
//	if (xcvr_name == rig_KX3.name_ && !progStatus.split  ) {
//		FreqDispB->value(vfoB.freq);
//		FreqDispB->redraw();
//		return 1;
//	}
	guard_lock serial(&mutex_serial);
	if (!selrig->can_change_alt_vfo  && selrig->inuse == onA) {
		selrig->selectB();
		vfoB.freq = FreqDispB->value();
		selrig->set_vfoB(vfoB.freq);
		selrig->selectA();
	} else {
		vfoB.freq = FreqDispB->value();
		selrig->set_vfoB(vfoB.freq);
	}
	return 1;
}

void execute_swapAB()
{
	if (selrig->canswap()) {
		selrig->swapAB();
		if (selrig->ICOMmainsub) {
			XCVR_STATE temp = vfoA;
			vfoA = vfoB;
			vfoB = temp;
			selrig->selectA();
			vfo = &vfoA;
		} else if (selrig->ICOMrig) {
			if (selrig->inuse == onB) {
				selrig->selectA();
				vfo = &vfoA;
			} else {
				selrig->selectB();
				vfo = &vfoB;
			}
		} else if (selrig->name_ == rig_FT891.name_) {
			// No need for extra select, as swapAB accomplishes this
			if (selrig->inuse == onB) {
				selrig->selectA();
				vfo = &vfoA;
				// Restore mode, then frequency and bandwidth after swap.
				yaesu891UpdateA(&vfoA);
			}
			else {
				selrig->selectB();
				vfo = &vfoB;
				// Restore mode, then frequency and bandwidth after swap.
				yaesu891UpdateB(&vfoB);
			}
		} else if (selrig->name_ == rig_FTdx10.name_ ) {
			XCVR_STATE temp = vfoB;
			vfoB = vfoA;
			vfoA = temp;
			if (selrig->inuse == onB)
				vfo = &vfoB;
			else
				vfo = &vfoA;
		} else {
			XCVR_STATE temp = vfoB;
			vfoB = vfoA;
			vfoA = temp;
			if (selrig->inuse == onB) {
				selrig->selectB();
				vfo = &vfoB;
			} else {
				selrig->selectA();
				vfo = &vfoA;
			}
		}
	} else {
		if (selrig->inuse == onB) {
			XCVR_STATE vfotemp = vfoA;
			selrig->selectA();
			vfoA = vfoB;
			selrig->set_modeA(vfoA.imode);
			selrig->set_vfoA(vfoA.freq);
			selrig->set_bwA(vfoA.iBW);
			selrig->get_vfoA();
			selrig->get_modeA();
			selrig->get_bwA();

			selrig->selectB();
			vfoB = vfotemp;
			selrig->set_modeB(vfoB.imode);
			selrig->set_vfoB(vfoB.freq);
			selrig->set_bwB(vfoB.iBW);
			selrig->get_vfoB();
			selrig->get_modeB();
			selrig->get_bwB();
			vfo = &vfoB;
		} else {
			XCVR_STATE vfotemp = vfoB;
			selrig->selectB();
			vfoB = vfoA;
			selrig->set_modeB(vfoB.imode);
			selrig->set_vfoB(vfoB.freq);
			selrig->set_bwB(vfoB.iBW);
			selrig->get_vfoB();
			selrig->get_modeB();
			selrig->get_bwB();

			selrig->selectA();
			vfoA = vfotemp;
			selrig->set_modeA(vfoA.imode);
			selrig->set_vfoA(vfoA.freq);
			selrig->set_bwA(vfoA.iBW);
			selrig->get_vfoA();
			selrig->get_modeA();
			selrig->get_bwA();
			vfo = &vfoA;
		}
	}
	Fl::awake(updateUI);
}

void cbAswapB()
{
	guard_lock lock(&mutex_srvc_reqs, "cbAswapB");
	if ((Fl::event_state() & FL_SHIFT) == FL_SHIFT) {
		VFOQUEUE xcvr;
		xcvr.change = FA2FB;
		trace(1, "cb VfoA freq -> VfoB freq");
		srvc_reqs.push(xcvr);
	} else if ((Fl::event_state() & FL_CTRL) == FL_CTRL) {
		VFOQUEUE xcvr;
		xcvr.change = FB2FA;
		trace(1, "cb VfoB freq -> VfoA freq");
		srvc_reqs.push(xcvr);
	} else {
		if (Fl::event_button() == FL_RIGHT_MOUSE) {
			VFOQUEUE xcvr;
			xcvr.change = A2B;
			trace(1, "cb Active->Inactive vfo");
			srvc_reqs.push(xcvr);
		} else {
			VFOQUEUE xcvr;
			xcvr.change = SWAP;
			trace(1, "cb SWAP");
			srvc_reqs.push(xcvr);
		}
	}
}

void execute_A2B()
{
	if (xcvr_name == rig_K3.name_) {
		K3_A2B();
	} else if (xcvr_name == rig_KX3.name_) {
		cb_KX3_A2B();
	} else if (xcvr_name == rig_K2.name_) {
		trace(1,"execute A2B() 1");
		vfoB = vfoA;
		selrig->set_vfoB(vfoB.freq);
		selrig->get_vfoB();
		FreqDispB->value(vfoB.freq);
	}
	if (selrig->ICOMmainsub) {
		selrig->selectA();
		selrig->A2B();
		vfoB = vfoA;
		vfo = &vfoA;
	} else if (selrig->has_a2b) {
		trace(1,"execute A2B() 2");
		selrig->A2B();
		if (selrig->inuse == onB) {
			vfoA = vfoB;
			FreqDispA->value(vfoA.freq);
		} else {
			vfoB = vfoA;
			FreqDispB->value(vfoB.freq);
		}
	} else {
		if (selrig->inuse == onB) {
			vfoA = vfoB;
			if (selrig->name_ == rig_FT891.name_) {
				yaesu891UpdateA(&vfoA);
			} else {
				selrig->set_vfoA(vfoA.freq);
				selrig->set_modeA(vfoA.imode);
				selrig->set_bwA(vfoA.iBW);
				selrig->get_vfoA();
				selrig->get_modeA();
				selrig->get_bwA();
			}
			FreqDispA->value(vfoA.freq);
		} else {
			vfoB = vfoA;
			if (selrig->name_ == rig_FT891.name_) {
				yaesu891UpdateB(&vfoB);
			} else {
				selrig->set_vfoB(vfoB.freq);
				selrig->set_modeB(vfoB.imode);
				selrig->set_bwB(vfoB.iBW);
				selrig->get_vfoB();
				selrig->get_modeB();
				selrig->get_bwB();
			}
			FreqDispB->value(vfoB.freq);
		}
	}
	Fl::awake(updateUI);
}

void execute_FA2FB()
{
	vfoB.freq = vfoA.freq;
	selrig->set_vfoB(vfoB.freq);
	FreqDispB->value(vfoB.freq);
	Fl::awake(updateUI);
}

void execute_FB2FA()
{
	vfoA.freq = vfoB.freq;
	selrig->set_vfoA(vfoA.freq);
	FreqDispA->value(vfoA.freq);
	Fl::awake(updateUI);
}

void highlight_vfo(void *d)
{
	Fl_Color norm_fg = fl_rgb_color(progStatus.fg_red, progStatus.fg_green, progStatus.fg_blue);
	Fl_Color norm_bg = fl_rgb_color(progStatus.bg_red, progStatus.bg_green, progStatus.bg_blue);
	Fl_Color dim_bg = fl_color_average( norm_bg, FL_BLACK, 0.75);
	FreqDispA->value(vfoA.freq);
	FreqDispB->value(vfoB.freq);
	if (selrig->inuse == onB) {
		FreqDispA->SetONOFFCOLOR( norm_fg, dim_bg );
		FreqDispB->SetONOFFCOLOR( norm_fg, norm_bg );
		btnA->value(0);
		btnB->value(1);
		FreqDispB->activate();
		if (selrig->can_change_alt_vfo)
			FreqDispA->activate();
		else
			FreqDispA->deactivate();
	} else {
		FreqDispA->SetONOFFCOLOR( norm_fg, norm_bg );
		FreqDispB->SetONOFFCOLOR( norm_fg, dim_bg);
		btnA->value(1);
		btnB->value(0);
		FreqDispA->activate();
		if (selrig->can_change_alt_vfo)
			FreqDispB->activate();
		else
			FreqDispB->deactivate();
	}
	FreqDispA->redraw();
	FreqDispB->redraw();
	btnA->redraw();
	btnB->redraw();
	Fl::check();
}

void cb_set_split(int val)
{
	progStatus.split = val;
	VFOQUEUE xcvr_split;
	if (val) xcvr_split.change = sON;
	else       xcvr_split.change = sOFF;
	trace(1, (val ? "cb_set_split(ON)" : "cb_set_split(OFF)"));
	srvc_reqs.push(xcvr_split);
}

void cb_selectA()
{
	guard_lock que_lock( &mutex_srvc_reqs, "cb_selectA");
	srvc_reqs.push (VFOQUEUE(sA, vfoA));
	return;
}

void cb_selectB()
{
	guard_lock que_lock( &mutex_srvc_reqs, "cb_selectB");
	srvc_reqs.push (VFOQUEUE(sB, vfoB));
	return;
}

void setLower()
{
}

void setUpper()
{
}

void selectFreq() {
	long n = FreqSelect->value();
	if (!n) return;

	n--;
	XCVR_STATE fm;
	fm.freq  = oplist[n].freq;
	fm.imode = oplist[n].imode;
	fm.iBW   = oplist[n].iBW;
	fm.src   = UI;
	if (selrig->inuse == onA) {
		FreqDispA->value(fm.freq);
		guard_lock que_lock(&mutex_srvc_reqs, "selectFreq on A");
		srvc_reqs.push(VFOQUEUE(vA, fm));
	} else {
		FreqDispB->value(fm.freq);
		guard_lock que_lock(&mutex_srvc_reqs, "selectFreq on B");
		srvc_reqs.push(VFOQUEUE(vB, fm));
	}
}

#include <FL/names.h>
void select_and_close()
{
	int key = Fl::event_key();
	int btn = Fl::event_button();

	if (FreqSelect->value() <= 0) return;

	if ((btn == FL_LEFT_MOUSE && Fl::event_clicks()) ||
		 btn == FL_RIGHT_MOUSE ||
		 key == FL_Enter ||
		 key == FL_Left) {
		inAlphaTag->value(oplist[FreqSelect->value() - 1].alpha_tag);
		selectFreq();
		Fl::focus(FreqSelect);
		return;
	}

	if (btn == FL_LEFT_MOUSE || key == FL_Up || key == FL_Down) {
		inAlphaTag->value(oplist[FreqSelect->value() - 1].alpha_tag);
		Fl::focus(FreqSelect);
		return;
	}

	if (key == FL_Right) {
		addFreq();
		FreqSelect->select(1, 1);
		inAlphaTag->value(oplist[0].alpha_tag);
		Fl::focus(FreqSelect);
		return;
	}

	if (key == FL_Delete) {
		long n = FreqSelect->value();
		delFreq();
		FreqSelect->select(n, 1);
		inAlphaTag->value(oplist[n-1].alpha_tag);
		Fl::focus(FreqSelect);
		return;
	}
}

void delFreq() {
	if (FreqSelect->value()) {
		long n = FreqSelect->value() - 1;
		for (int i = n; i < numinlist; i ++)
			oplist[i] = oplist[i+1];
		if (numinlist) {
			oplist[numinlist - 1].imode = USB;
			oplist[numinlist - 1].freq = 0;
			oplist[numinlist - 1].iBW = 0;
			memset(oplist[numinlist - 1].alpha_tag, 0, ATAGSIZE);
			numinlist--;
		}
		updateSelect();
	}
}

void addFreq() {
	if (selrig->inuse == onB) {
		long freq = FreqDispB->value();
		if (!freq) return;
		int mode = opMODE->index();
		int bw;
		if (btnDSP->visible())
			bw = ((opDSP_hi->index() << 8) | 0x8000) | (opDSP_lo->index() & 0xFF) ;
		else
			bw = opBW->index();
		for (int n = 0; n < numinlist; n++)
			if (freq == oplist[n].freq && mode == oplist[n].imode) {
				oplist[n].iBW = bw;
				updateSelect();	// update list
				return;
			}
		addtoList(freq, mode, bw);
		updateSelect();
		FreqDispB->visual_beep();
	} else {
		long freq = FreqDispA->value();
		if (!freq) return;
		int mode = opMODE->index();
		int bw;
		if (btnDSP->visible())
			bw = ((opDSP_hi->index() << 8) | 0x8000) | (opDSP_lo->index() & 0xFF) ;
		else
			bw = opBW->index();
		for (int n = 0; n < numinlist; n++)
			if (freq == oplist[n].freq && mode == oplist[n].imode) {
				oplist[n].iBW = bw;
				updateSelect();	// update list
				return;
			}
		addtoList(freq, mode, bw);
		updateSelect();
		FreqDispA->visual_beep();
		}
}

void cbRIT()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cbRIT()");
	if (selrig->has_rit  && cntRIT)
		selrig->setRit((int)cntRIT->value());
}

void cbXIT()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cbXIT()");
	selrig->setXit((int)cntXIT->value());
}

void cbBFO()
{
	if (selrig->has_bfo) {
		guard_lock serial_lock(&mutex_serial);
		trace(1, "cbBFO()");
		selrig->setBfo((int)cntBFO->value());
	}
}

void cbAttenuator()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cbAttenuator()");

//	int chk = selrig->get_attenuator();
	progStatus.attenuator = selrig->next_attenuator();
	selrig->set_attenuator (progStatus.attenuator);
//	MilliSleep(50);
//	for (int n = 0; n < 100; n++) {
//		chk = selrig->get_attenuator();
//		if (chk == progStatus.attenuator) break;
//		MilliSleep(progStatus.comm_wait);
//		Fl::awake();
//	}
	return;
}

void setAttControl(void *d)
{
	size_t val = reinterpret_cast<size_t>(d);
	btnAttenuator->value(val);
}

void cbPreamp()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cbPreamp()");

//	int chk = selrig->get_preamp();
	progStatus.preamp = selrig->next_preamp();
	selrig->set_preamp (progStatus.preamp);
//	MilliSleep(5 + progStatus.comm_wait);
//	for (int n = 0; n < 100; n++) {
//		chk = selrig->get_preamp();
//		if (chk == progStatus.preamp) break;
//		MilliSleep(progStatus.comm_wait);
//		Fl::awake();
//	}
	return;
}

void setPreampControl(void *d)
{
	size_t val = reinterpret_cast<size_t>(d);
	btnPreamp->value(val);
}

void cbAN()
{
	progStatus.auto_notch = btnAutoNotch->value();
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cbAN()");
	selrig->set_auto_notch(progStatus.auto_notch);
}

void cbbtnNotch()
{
	if (!selrig->has_notch_control) return;
	guard_lock serial_lock(&mutex_serial);

//	trace(1, "cbbtnNotch()");

	int val = 0, cnt = 0;

	progStatus.notch = btnNotch->value();

	selrig->set_notch(progStatus.notch, progStatus.notch_val);

//	int on, val = progStatus.notch_val;

//	on = selrig->get_notch(val);

	MilliSleep(progStatus.comm_wait);

	while ((selrig->get_notch(val) != progStatus.notch) && (cnt++ < 10)) {
		MilliSleep(progStatus.comm_wait);
//		on = ;
		Fl::awake();
	}
}

void setNotch()
{
	if (!selrig->has_notch_control) return;

	trace(1, "setNotch()");

	int ev = Fl::event();
	if (ev == FL_LEAVE || ev == FL_ENTER) return;

	guard_lock lock( &mutex_serial);

	if (sldrNOTCH) {
		progStatus.notch_val = sldrNOTCH->value();
	} else {
		progStatus.notch_val = spnrNOTCH->value();
	}

	selrig->set_notch(progStatus.notch, progStatus.notch_val);
}

// called from xml_io thread
// xcvr updated in xml_io / xml_server code
// this only updates the dialog controls

void setNotchControl(void *d)
{
	if (sldrNOTCH) sldrNOTCH->value(progStatus.notch_val);
	if (spnrNOTCH) spnrNOTCH->value(progStatus.notch_val);
	btnNotch->value(progStatus.notch);
}

void adjust_if_shift_control(void *d)
{
	if (sldrIFSHIFT) sldrIFSHIFT->minimum(selrig->if_shift_min);
	if (sldrIFSHIFT) sldrIFSHIFT->maximum(selrig->if_shift_max);
	if (sldrIFSHIFT) sldrIFSHIFT->step(selrig->if_shift_step);
	if (sldrIFSHIFT) sldrIFSHIFT->value(selrig->if_shift_mid);
	if (sldrIFSHIFT) sldrIFSHIFT->redraw();
	if (spnrIFSHIFT) spnrIFSHIFT->minimum(selrig->if_shift_min);
	if (spnrIFSHIFT) spnrIFSHIFT->maximum(selrig->if_shift_max);
	if (spnrIFSHIFT) spnrIFSHIFT->step(selrig->if_shift_step);
	if (spnrIFSHIFT) spnrIFSHIFT->value(selrig->if_shift_mid);
	if (spnrIFSHIFT) spnrIFSHIFT->redraw();
	btnIFsh->value(0);
	btnIFsh->redraw();
}

void setIFshiftButton(void *d)
{
	bool b = (bool)d;
	if (b && !btnIFsh->value()) {
		btnIFsh->value(1);
	}
	else if (!b && btnIFsh->value()) {
		btnIFsh->value(0);
		if (sldrIFSHIFT) sldrIFSHIFT->value( selrig->if_shift_mid );
		if (spnrIFSHIFT) spnrIFSHIFT->value( selrig->if_shift_mid );
	}
}

void setIFshiftControl(void *d)
{
	int val = *(reinterpret_cast<int *>(d));
	if (sldrIFSHIFT) {
		if (sldrIFSHIFT->value() != val)
			sldrIFSHIFT->value(val);
	}
	if (spnrIFSHIFT) {
		if (spnrIFSHIFT->value() != val)
			spnrIFSHIFT->value(val);
	}
	btnIFsh->value( val != selrig->if_shift_mid );
}

void setIFshift()
{
	trace(1, "setIFshift()");

	int ev = Fl::event();
	if (ev == FL_LEAVE || ev == FL_ENTER) return;
	if (ev == FL_DRAG || ev == FL_PUSH) {
		inhibit_shift = 1;
		return;
	}

	int btn = 0, set = 0;

	btn = btnIFsh->value();
	progStatus.shift = btn;

	if (sldrIFSHIFT) {
		set = sldrIFSHIFT->value();
	} else if (spnrIFSHIFT) {
		set = spnrIFSHIFT->value();
	}
	progStatus.shift_val = set;


	guard_lock lock(&mutex_serial);
	if (xcvr_name == rig_TS990.name_) {
		if (progStatus.shift)
			selrig->set_monitor(1);
		else
			selrig->set_monitor(0);
	}
	selrig->set_if_shift(progStatus.shift_val);
}

void cbIFsh()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "setIFsh()");

	int btn, set, cnt = 0;
	if (sldrIFSHIFT) {
		set = sldrIFSHIFT->value();
		btn = btnIFsh->value();
	} else {
		set = spnrIFSHIFT->value();
		btn = btnIFsh->value();
	}
	if (btn == 0) set = 0;

	selrig->set_if_shift(set);
	MilliSleep(50);
	int val, on;
	on = selrig->get_if_shift(val);
	while ((on != btn) && (cnt++ < 10)) {
		MilliSleep(progStatus.comm_wait);
		on = selrig->get_if_shift(val);
		Fl::awake();
	}
}

void setLOCK()
{
	progStatus.pbt_lock = btnLOCK->value();
	if (progStatus.pbt_lock) {
		guard_lock serial_lock(&mutex_serial);
		progStatus.pbt_outer = progStatus.pbt_inner;
		sldrOUTER->value(progStatus.pbt_outer);
		selrig->set_pbt_outer(progStatus.pbt_outer);
		sldrOUTER->redraw();
	}
}

void setINNER()
{
	progStatus.pbt_inner = sldrINNER->value();
	if (progStatus.pbt_lock) {
		progStatus.pbt_outer = progStatus.pbt_inner;
		sldrOUTER->value(progStatus.pbt_outer);
		sldrOUTER->redraw();
	}

	int ev = Fl::event();
	if (ev == FL_LEAVE || ev == FL_ENTER) return;
	if (ev == FL_DRAG || ev == FL_PUSH) {
		inhibit_pbt = 1;
		return;
	}

	guard_lock lock(&mutex_serial);
	selrig->set_pbt_inner(progStatus.pbt_inner);
	selrig->set_pbt_outer(progStatus.pbt_outer);
	selrig->get_pbt_inner();
	selrig->get_pbt_outer();
}

void setOUTER()
{
	progStatus.pbt_outer = sldrOUTER->value();
	if (progStatus.pbt_lock) {
		progStatus.pbt_inner = progStatus.pbt_outer;
		sldrINNER->value(progStatus.pbt_inner);
		sldrINNER->redraw();
	}

	int ev = Fl::event();
	if (ev == FL_LEAVE || ev == FL_ENTER) return;
	if (ev == FL_DRAG || ev == FL_PUSH) {
		inhibit_pbt = 1;
		return;
	}

	guard_lock lock(&mutex_serial);
	selrig->set_pbt_outer(progStatus.pbt_outer);
	selrig->set_pbt_inner(progStatus.pbt_inner);
	selrig->get_pbt_outer();
	selrig->get_pbt_inner();
}

void setCLRPBT()
{
	progStatus.pbt_inner = progStatus.pbt_outer = 0;

	sldrOUTER->value(0);
	sldrOUTER->redraw();

	sldrINNER->value(0);
	sldrINNER->redraw();

	guard_lock lock(&mutex_serial);
	selrig->set_pbt_outer(progStatus.pbt_outer);
	selrig->set_pbt_inner(progStatus.pbt_inner);
}

//----------------------------------------------------------------------
// these only apply to the IC7610
//----------------------------------------------------------------------

void digi_sel_on_off()
{
	selrig->set_digi_sel(progStatus.digi_sel_on_off);
}

void set_ic7610_digi_sel_on_off(void *)
{
	ic7610digi_sel_on_off->value(progStatus.digi_sel_on_off);
}

void digi_sel_val()
{
	selrig->set_digi_val(progStatus.digi_sel_val);
}

void set_ic7610_digi_sel_val(void *)
{
	ic7610_digi_sel_val->value(progStatus.digi_sel_val);
}

void dual_watch()
{
	selrig->set_dual_watch(progStatus.dual_watch);
}

void set_ic7610_dual_watch(void *)
{
}

void index_att()
{
	selrig->set_index_att(progStatus.index_ic7610att);
}

void set_ic7610_index_att(void *)
{
	ic7610att->index(progStatus.index_ic7610att);
}

//----------------------------------------------------------------------

void cbEventLog()
{
	debug::show();
}

void setVolume() // UI call
{
	trace(1, "setVolume()");
	int ev = Fl::event();
	if (ev == FL_LEAVE || ev == FL_ENTER) return;
	int set;
	if (spnrVOLUME) set = spnrVOLUME->value();
	else set = sldrVOLUME->value();
	progStatus.volume = set;

	if (btnVol->value() == 0) return;

	guard_lock serial_lock(&mutex_serial);
	selrig->set_volume_control(progStatus.volume);
}

void setVolumeControl(void* d) // called by xml_server
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "setVolumeControl()");
	if (sldrVOLUME) sldrVOLUME->value(progStatus.volume);
	if (spnrVOLUME) spnrVOLUME->value(progStatus.volume);
	selrig->set_volume_control(progStatus.volume);
}

void cbMute()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cbMute()");

	int set = 0, get, cnt = 0;
	if (btnVol->value() == 0) {
		if (spnrVOLUME) spnrVOLUME->deactivate();
		if (sldrVOLUME) sldrVOLUME->deactivate();
	} else {
		if (spnrVOLUME) {
			spnrVOLUME->activate();
			set = spnrVOLUME->value();
		}
		if (sldrVOLUME) {
			sldrVOLUME->activate();
			set = sldrVOLUME->value();
		}
	}
	selrig->set_volume_control(set);
	MilliSleep(50);
	get = selrig->get_volume_control();
	while (get != set && cnt++ < 10) {
		MilliSleep(progStatus.comm_wait);
		get = selrig->get_volume_control();
		Fl::awake();
	}
	progStatus.volume = set;
}

void setMicGain()
{
	int ev = Fl::event();
	if (ev == FL_LEAVE || ev == FL_ENTER) return;
	if (ev == FL_DRAG || ev == FL_PUSH) {
		inhibit_mic = 1;
		return;
	}
	std::stringstream str;
	str << "setMicGain(), ev=" << ev << ", inhibit_mic=" << inhibit_mic;
	trace(1, str.str().c_str());


	int set = 0;

	if (sldrMICGAIN) set = sldrMICGAIN->value();
	if (spnrMICGAIN) set = spnrMICGAIN->value();

	progStatus.mic_gain = set;

	guard_lock lock(&mutex_serial);
	selrig->set_mic_gain(set);
}

void setMicGainControl(void* d)
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "setMicGainControl()");
	if (sldrMICGAIN) sldrMICGAIN->value(progStatus.mic_gain);
	if (spnrMICGAIN) spnrMICGAIN->value(progStatus.mic_gain);
}

static int img = -1;

void set_power_controlImage(double pwr)
{
	if ((progStatus.pwr_scale == 8 && pwr <= 5.0) || (progStatus.pwr_scale == 0)) {
		if (img != 1) {
			img = 1;
			scalePower->image(image_p5);
			sldrFwdPwr->maximum(5.0);
			sldrFwdPwr->minimum(0.0);
			scalePower->redraw();

			mtr_PWR->image(image_p5);
			mtr_PWR->redraw();
			sigbar_PWR->maximum(5.0);
			sigbar_PWR->minimum(0.0);
			sigbar_PWR->redraw();
		}
	}
	else if ((progStatus.pwr_scale == 8 && pwr <= 10.0) || (progStatus.pwr_scale == 1)) {
		if (img != 2) {
			img = 2;
			scalePower->image(image_p10);
			sldrFwdPwr->maximum(10.0);
			sldrFwdPwr->minimum(0.0);
			scalePower->redraw();

			mtr_PWR->image(image_p10);
			mtr_PWR->redraw();
			sigbar_PWR->maximum(10.0);
			sigbar_PWR->minimum(0.0);
			sigbar_PWR->redraw();
		}
	}
	else if ((progStatus.pwr_scale == 8 && pwr <= 15.0) || (progStatus.pwr_scale == 2)) {
		if (img != 3) {
			img = 3;
			scalePower->image(image_p15);
			sldrFwdPwr->maximum(16.666);
			sldrFwdPwr->minimum(0.0);
			scalePower->redraw();

			mtr_PWR->image(image_p15);
			mtr_PWR->redraw();
			sigbar_PWR->maximum(16.666);
			sigbar_PWR->minimum(0.0);
			sigbar_PWR->redraw();
		}
	}
	else if ((progStatus.pwr_scale == 8 && pwr <= 20.0) || (progStatus.pwr_scale == 3)) {
		if (img != 4) {
			img = 4;
			scalePower->image(image_p20);
			sldrFwdPwr->maximum(20.0);
			sldrFwdPwr->minimum(0.0);
			scalePower->redraw();

			mtr_PWR->image(image_p20);
			mtr_PWR->redraw();
			sigbar_PWR->maximum(20.0);
			sigbar_PWR->minimum(0.0);
			sigbar_PWR->redraw();
		}
	}
	else if ((progStatus.pwr_scale == 8 && pwr <= 25.0) || (progStatus.pwr_scale == 4)) {
		if (img != 5) {
			img = 5;
			scalePower->image(image_p25);
			sldrFwdPwr->maximum(25.0);
			sldrFwdPwr->minimum(0.0);
			scalePower->redraw();

			mtr_PWR->image(image_p25);
			mtr_PWR->redraw();
			sigbar_PWR->maximum(25.0);
			sigbar_PWR->minimum(0.0);
			sigbar_PWR->redraw();
		}
	}
	else if ((progStatus.pwr_scale == 8 && pwr <= 50.0) || (progStatus.pwr_scale == 5)) {
		if (img != 6) {
			img = 6;
			scalePower->image(image_p50);
			sldrFwdPwr->maximum(50.0);
			sldrFwdPwr->minimum(0.0);
			scalePower->redraw();

			mtr_PWR->image(image_p50);
			mtr_PWR->redraw();
			sigbar_PWR->maximum(50.0);
			sigbar_PWR->minimum(0.0);
			sigbar_PWR->redraw();
		}
	}
	else if ((progStatus.pwr_scale == 8 && pwr <= 100.0) || (progStatus.pwr_scale == 6)) {
		if (img != 6) {
			img = 6;
			scalePower->image(image_p100);
			sldrFwdPwr->maximum(100.0);
			sldrFwdPwr->minimum(0.0);
			scalePower->redraw();

			mtr_PWR->image(image_p100);
			mtr_PWR->redraw();
			sigbar_PWR->maximum(100.0);
			sigbar_PWR->minimum(0.0);
			sigbar_PWR->redraw();
		}
	}
	else if ((progStatus.pwr_scale == 8 && pwr > 100.0) || (progStatus.pwr_scale == 7)) {
		if (img != 8) {
			img = 8;
			scalePower->image(image_p200);
			sldrFwdPwr->maximum(200.0);
			sldrFwdPwr->minimum(0.0);
			scalePower->redraw();

			mtr_PWR->image(image_p200);
			mtr_PWR->redraw();
			sigbar_PWR->maximum(200.0);
			sigbar_PWR->minimum(0.0);
			sigbar_PWR->redraw();
		}
	}
	return;
}

void set_init_power_control();

void setPower()
{
	int ev = Fl::event();
	if (ev == FL_LEAVE || ev == FL_ENTER) return;
	if (ev == FL_DRAG || ev == FL_PUSH) {
		inhibit_power = 1;
		return;
	}
	std::stringstream str;
	str << "setPower(), ev=" << ev << ", inhibit_power=" << inhibit_power;
	trace(1, str.str().c_str());

	double set = 0;

	if (spnrPOWER) set = progStatus.power_level = spnrPOWER->value();
	if (sldrPOWER) set = progStatus.power_level = sldrPOWER->value();

	if (xcvr_name == rig_K2.name_) {
		double min, max, step;
		selrig->get_pc_min_max_step(min, max, step);
		if (spnrPOWER) spnrPOWER->minimum(min);
		if (spnrPOWER) spnrPOWER->maximum(max);
		if (spnrPOWER) spnrPOWER->step(step);
		if (spnrPOWER) spnrPOWER->value(progStatus.power_level);
		if (spnrPOWER) spnrPOWER->redraw();
		if (sldrPOWER) sldrPOWER->minimum(min);
		if (sldrPOWER) sldrPOWER->maximum(max);
		if (sldrPOWER) sldrPOWER->step(step);
		if (sldrPOWER) sldrPOWER->value(progStatus.power_level);
		if (sldrPOWER) sldrPOWER->redraw();
	}

	guard_lock lock(&mutex_serial);
	selrig->set_power_control(set);
	set_init_power_control();
}

void cbTune()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cbTune()");
	selrig->tune_rig(2);
}

void cb_tune_on_off()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cb_tune_on_off()");
	selrig->tune_rig(btn_tune_on_off->value());
}

int chkptt()
{
	if (progStatus.comm_catptt) {
		return selrig->get_PTT();
	} else if (progStatus.comm_dtrptt) {
		return RigSerial->getPTT();
	} else if (progStatus.comm_rtsptt) {
		return RigSerial->getPTT();
	} else if (SepSerial->IsOpen() && progStatus.sep_dtrptt) {
		return SepSerial->getPTT();
	} else if (SepSerial->IsOpen() && progStatus.sep_rtsptt) {
		return SepSerial->getPTT();
	} else if (progStatus.gpio_ptt) {
		return get_gpio();
	} else if (progStatus.cmedia_ptt) {
		return get_cmedia();
	}
	return 0;
}

void doPTT(int on)
{
	guard_lock serlck(&mutex_serial);

//	int chk = chkptt();
//	if (chk == on) return;

	PTT = on;
	rigPTT(on);
	btnPTT->value(on);

	MilliSleep(progStatus.comm_wait);
	for (int n = 0; n < 100; n++) {
		if (on == chkptt()) break;
		MilliSleep(progStatus.comm_wait);
		Fl::awake();
	}
	return;
}

void setSQUELCH()
{
	trace(1, "setSQUELCH()");

	int ev = Fl::event();
	if (ev == FL_LEAVE || ev == FL_ENTER) return;
	if (ev == FL_DRAG || ev == FL_PUSH) {
		inhibit_squelch = 1;
		return;
	}
	int set = 0;
	if (sldrSQUELCH) set = sldrSQUELCH->value();
	if (spnrSQUELCH) set = spnrSQUELCH->value();

	progStatus.squelch = set;
	guard_lock lock(&mutex_serial);
	selrig->set_squelch(set);
}

int agcwas = 0;
void redrawAGC()
{
	const char *lbl = selrig->agc_label();
	int val = progStatus.agc_level;

	if (!selrig->has_rf_control) {
		sldrSQUELCH->label("");
		sldrSQUELCH->redraw_label();
	}
	btnAGC->label(lbl);
	btnAGC->redraw_label();

	if (xcvr_name == rig_FT450D.name_) {
		switch (val) {
			case 0 : btnAGC->selection_color(FL_BACKGROUND_COLOR);  // off
					 break;
			case 1 : btnAGC->selection_color(FL_RED); // fast
					 break;
			case 2 : btnAGC->selection_color(FL_YELLOW); // slow
					 break;
			case 3 : btnAGC->selection_color(FL_GREEN); // auto
					 break;
		}
		btnAGC->redraw();
	}

	if (xcvr_name == rig_FT991A.name_) {
		switch (val) {
			case 0 : btnAGC->selection_color(FL_BACKGROUND_COLOR);  // off
					 break;
			case 1 : btnAGC->selection_color(FL_RED); // fast
					 break;
			case 2 : btnAGC->selection_color(FL_YELLOW); // medium
					 break;
			case 3 : btnAGC->selection_color(FL_GREEN); // slow
					 break;
			case 4 : btnAGC->selection_color(FL_WHITE); // auto
		}
		btnAGC->redraw();
	}

	int rignbr = 0;
	if (xcvr_name == rig_IC7200.name_) rignbr = 1;
	if (xcvr_name == rig_IC7300.name_) rignbr = 2;
	if (rignbr) {
		if (val == 0) btnAGC->selection_color(FL_BACKGROUND_COLOR);  // off
		if (val == 1) btnAGC->selection_color(
			rignbr == 1 ? FL_GREEN : FL_RED); // fast
		if (val == 2) btnAGC->selection_color(
			rignbr == 1 ? FL_RED : FL_YELLOW);  // med / slow
		if (val == 3) btnAGC->selection_color(FL_GREEN); // slow
		btnAGC->value(val > 0);
	} else {
		if (val == 0)
			btnAGC->value(0);
		else
			btnAGC->value(1);
	}

	if (agcwas != val) {
		agcwas = val;
	}

	btnAGC->redraw();
}

void setAGC(void *)
{
	if (!selrig->has_agc_control) return;
	redrawAGC();
}

void cbAGC()
{
	if (!selrig->has_agc_control) return;
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cbAGC()");
	progStatus.agc_level = selrig->incr_agc();
	redrawAGC();
}

void setRFGAIN()
{
	trace(1, "setRFGAIN()");

	int ev = Fl::event();
	if (ev == FL_LEAVE || ev == FL_ENTER) return;
	if (ev == FL_DRAG || ev == FL_PUSH) {
		inhibit_rfgain = 1;
		return;
	}

	int set = 0;

	if (spnrRFGAIN) set = spnrRFGAIN->value();
	if (sldrRFGAIN) set = sldrRFGAIN->value();

	progStatus.rfgain = set;

	guard_lock lock(&mutex_serial);
	selrig->set_rf_gain(set);
}

void setRFGAINControl(void* d)
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "setRFGAINControl()");
	if (sldrRFGAIN) sldrRFGAIN->value(progStatus.rfgain);
	if (spnrRFGAIN) spnrRFGAIN->value(progStatus.rfgain);
}

void updateALC(void *)
{
	sigbar_ALC->value(alcval);
	sigbar_ALC->redraw();

	if (meter_image != ALC_IMAGE) return;
	sldrRcvSignal->hide();
	sldrSWR->hide();
	sldrALC->show();
	sldrALC->value(alcval);
	sldrALC->redraw();
}

void updateSWR(void *)
{
	sigbar_SWR->value(swrval);
	sigbar_SWR->redraw();

	if (meter_image != SWR_IMAGE) return;
	if (selrig->has_swr_control) {
		sldrRcvSignal->hide();
		sldrALC->hide();
		sldrSWR->show();
	}
	sldrSWR->value(swrval);
	sldrSWR->redraw();
}

void updateFwdPwr(void *)
{
	double power = pwrval;

	power /= selrig->power_scale();

	if (selrig->has_power_control)
		set_power_controlImage(power);

	sldrVoltage->hide();
	scaleVoltage->hide();
	sldrFwdPwr->show();
	scalePower->show();

	sldrFwdPwr->value(power);
	sldrFwdPwr->redraw();

	sigbar_PWR->value(power);
	sigbar_PWR->redraw();

}

void updateSquelch(void *d)
{
	if (sldrSQUELCH) sldrSQUELCH->value(reinterpret_cast<size_t>(d));
	if (sldrSQUELCH) sldrSQUELCH->redraw();
	if (spnrSQUELCH) spnrSQUELCH->value(reinterpret_cast<size_t>(d));
	if (spnrSQUELCH) spnrSQUELCH->redraw();
}

void updateRFgain(void *d)
{
	if (spnrRFGAIN) {
		spnrRFGAIN->value(reinterpret_cast<size_t>(d));
		spnrRFGAIN->redraw();
	}
	if (sldrRFGAIN) {
		sldrRFGAIN->value(reinterpret_cast<size_t>(d));
		sldrRFGAIN->redraw();
	}
}

void zeroXmtMeters(void *d)
{
	pwrval = 0; updateFwdPwr();
	alcval = 0; updateALC();
	swrval = 0; updateSWR();
	sldrFwdPwr->clear();
	sldrALC->clear();
	sldrSWR->clear();
}

void setFreqDispA(void *d)
{
	FreqDispA->value(vfoA.freq);
	FreqDispA->redraw();
}

void setFreqDispB(void *d)
{
	FreqDispB->value(vfoB.freq);
	FreqDispB->redraw();
}

void updateSmeter(void *)
{
	if (!sldrRcvSignal->visible()) {
		sldrRcvSignal->show();
		sldrFwdPwr->hide();
		sldrALC->hide();
		sldrSWR->hide();
	}
	sldrRcvSignal->value(smtrval);
	sldrRcvSignal->redraw();
	sigbar_SMETER->value(smtrval);
	sigbar_SMETER->redraw();
}

void updateVmeter(void *)
{
	if (!progStatus.display_voltmeter || !selrig->has_voltmeter) {
		sldrVoltage->hide();
		scaleVoltage->hide();
		sldrFwdPwr->show();
		scalePower->show();
	} else {
		if (PTT) {
			sldrVoltage->hide();
			scaleVoltage->hide();
			sldrFwdPwr->show();
			scalePower->show();
		} else {
			sldrFwdPwr->hide();
			scalePower->hide();
			sldrVoltage->show();
			scaleVoltage->show();
		}
	}
	if (vmtrval == -1) return;

	sldrVoltage->value(vmtrval);
	sldrVoltage->redraw();

	sigbar_VOLTS->value(vmtrval);
	sigbar_VOLTS->redraw();
}

void TRACED(saveFreqList)

	std::string atag;

	if (!numinlist) return;

	rotate_log(defFileName);

	std::ofstream oList(defFileName.c_str());
	if (!oList) {
		fl_message ("Could not write to %s", defFileName.c_str());
		return;
	}
	for (int i = 0; i < numinlist; i++) {
		atag = oplist[i].alpha_tag;
		oList << oplist[i].freq << " " << oplist[i].imode << " " << oplist[i].iBW << " \"" << atag.c_str() << "\"" << std::endl;

	}
	oList.close();
}

void TRACED(setPTT, void *d)

	size_t set = reinterpret_cast<size_t>(d);
	size_t chk = 0;

	guard_lock serlck(&mutex_serial);
	chk = chkptt();
	rigPTT(set);
	MilliSleep(50);
	for (int n = 0; n < 100; n++) {
		chk = chkptt();
		if (set == chk) break;
		MilliSleep(progStatus.comm_wait);
	}
	return;
}

void update_progress(int val)
{
	progress->value(val);
	Fl::check();
}

void TRACED(send_st_ex_command, std::string command)

	std::string cmd = "";
	if (command.find("x") != std::string::npos) { // hex std::strings
		size_t p = 0;
		unsigned int val;
		while (( p = command.find("x", p)) != std::string::npos) {
			sscanf(&command[p+1], "%x", &val);
			cmd += (unsigned char) val;
			p += 3;
		}
	} else
		cmd = command;
	sendCommand(cmd, 0);
}

#include "timeops.h"
void synchronize( void *) {

	time_t now;
	time(&now);
	struct tm *tm_time;
	static char sztm[20];

	if (progStatus.sync_gmt) {
		tm_time = gmtime(&now);
		strftime(sztm, sizeof(sztm), "%H:%M:%S Z", tm_time);
	} else {
		tm_time = localtime(&now);
		strftime(sztm, sizeof(sztm), "%H:%M:%S", tm_time);
	}

	if (strncmp(&sztm[6], "00", 2) == 0) {
		guard_lock serial_lock(&mutex_serial);
		static char szdate[20];
		strftime(szdate, sizeof(szdate), "%Y%m%d", tm_time);
		selrig->sync_clock(sztm);
		selrig->sync_date(szdate);
		txt_xcvr_synch->value("--SYNC'D--");
		return;
	}
	txt_xcvr_synch->value(sztm);
	Fl::repeat_timeout(0.05, synchronize);
}

void TRACED(synchronize_now)
	Fl::remove_timeout(synchronize);
	Fl::add_timeout(0, synchronize);
}

void TRACED(start_commands)

	if (!progStatus.cmd_on_start1.empty()) send_st_ex_command(progStatus.cmd_on_start1);
	if (!progStatus.cmd_on_start2.empty()) send_st_ex_command(progStatus.cmd_on_start2);
	if (!progStatus.cmd_on_start3.empty()) send_st_ex_command(progStatus.cmd_on_start3);
	if (!progStatus.cmd_on_start4.empty()) send_st_ex_command(progStatus.cmd_on_start4);
	if (progStatus.sync_clock && selrig->can_synch_clock)
		Fl::add_timeout(0, synchronize);
}

void TRACED(exit_commands)

	if (!progStatus.cmd_on_exit1.empty()) send_st_ex_command(progStatus.cmd_on_exit1);
	if (!progStatus.cmd_on_exit2.empty()) send_st_ex_command(progStatus.cmd_on_exit2);
	if (!progStatus.cmd_on_exit3.empty()) send_st_ex_command(progStatus.cmd_on_exit3);
	if (!progStatus.cmd_on_exit4.empty()) send_st_ex_command(progStatus.cmd_on_exit4);
}

void TRACED(close_UI)

	{
		guard_lock serial_lock(&mutex_serial);
		trace(1, "close_UI()");
		run_serial_thread = false;
	}
	pthread_join(*serial_thread, NULL);

// xcvr auto off
	if (selrig->has_xcvr_auto_on_off && progStatus.xcvr_auto_off)
		selrig->set_xcvr_auto_off();

	// close down the serial port
	RigSerial->ClosePort();

	if (dlgDisplayConfig && dlgDisplayConfig->visible())
		dlgDisplayConfig->hide();
	if (dlgXcvrConfig && dlgXcvrConfig->visible())
		dlgXcvrConfig->hide();
	if (dlgMemoryDialog && dlgMemoryDialog->visible())
		dlgMemoryDialog->hide();

	if (meters_dialog && meters_dialog->visible())
		meters_dialog->hide();

	debug::stop();

	mainwindow->hide();
}

void TRACED(closeRig)

	trace(1, "closeRig()");
	if (xcvr_online) {
		restore_xcvr_vals();
		selrig->shutdown();
	}
	xcvr_online = false;
}

void TRACED(cbExit)

	main_group->hide();
	main_group->redraw();

	grpInitializing->size(mainwindow->w(), mainwindow->h() - grpInitializing->y());
	grpInitializing->show();
	grpInitializing->redraw();

	progress->label("Closing");
	progress->redraw_label();

	progress->position(grpInitializing->w()/4, grpInitializing->y() + grpInitializing->h()/2);

	update_progress(0);

	progStatus.freq_A = vfoA.freq;
	progStatus.imode_A = vfoA.imode;
	progStatus.iBW_A = vfoA.iBW;

	progStatus.freq_B = vfoB.freq;
	progStatus.imode_B = vfoB.imode;
	progStatus.iBW_B = vfoB.iBW;

	if (selrig->has_FILTER)
		progStatus.filters = selrig->get_FILTERS();

	progStatus.spkr_on = btnVol->value();

	saveFreqList();

	if (spnrPOWER) progStatus.power_level = spnrPOWER->value();
	if (spnrVOLUME) progStatus.volume = spnrVOLUME->value();
	if (spnrRFGAIN) progStatus.rfgain = spnrRFGAIN->value();
	if (spnrMICGAIN) progStatus.mic_gain = spnrMICGAIN->value();
	if (spnrNOTCH) progStatus.notch_val = spnrNOTCH->value();
	if (spnrIFSHIFT) progStatus.shift_val = spnrIFSHIFT->value();
	if (spnrNR) progStatus.noise_reduction_val = spnrNR->value();

	if (sldrPOWER) progStatus.power_level = sldrPOWER->value();
	if (sldrVOLUME) progStatus.volume = sldrVOLUME->value();
	if (sldrRFGAIN) progStatus.rfgain = sldrRFGAIN->value();
	if (sldrMICGAIN) progStatus.mic_gain = sldrMICGAIN->value();
	if (sldrNOTCH) progStatus.notch_val = sldrNOTCH->value();
	if (sldrIFSHIFT) progStatus.shift_val = sldrIFSHIFT->value();
	if (sldrNR) progStatus.noise_reduction_val = sldrNR->value();

	progStatus.notch = btnNotch->value();
	progStatus.shift = btnIFsh->value();
	progStatus.noise_reduction = btnNR->value();
	progStatus.noise = btnNOISE->value();
	progStatus.attenuator = btnAttenuator->value();
	progStatus.preamp = btnPreamp->value();
	progStatus.auto_notch = btnAutoNotch->value();

	progStatus.bandwidths = selrig->get_BANDWIDTHS();

	progStatus.saveLastState();
	closeRig();

	stop_cwio_thread();

	exit_server();

	close_UI();

	if (tracewindow) tracewindow->hide();
	if (tabs_dialog) tabs_dialog->hide();

	if (cwio_keyer_dialog) cwio_keyer_dialog->hide();
	if (cwio_editor) cwio_editor->hide();
	if (cwio_configure) cwio_configure->hide();

	if (FSK_keyer_dialog) FSK_keyer_dialog->hide();
	if (FSK_editor) FSK_editor->hide();
	if (FSK_configure) FSK_configure->hide();

	if (meter_filters) meter_filters->hide();
	if (meter_scale_dialog) meter_scale_dialog->hide();
}

void cbALC_SWR()
{
	if (!selrig->has_alc_control) return;
	if (meter_image == SWR_IMAGE) {
		btnALC_SWR->image(image_alc);
		meter_image = ALC_IMAGE;
		sldrALC->show();
		{
			guard_lock serial_lock(&mutex_serial);
			trace(1, "cbALC_SWR()  1");
			selrig->select_alc();
		}
	} else {
		btnALC_SWR->image(image_swr);
		meter_image = SWR_IMAGE;
		sldrSWR->show();
		{
			guard_lock serial_lock(&mutex_serial);
			trace(1, "cbALC_SWR()  2");
			selrig->select_swr();
		}
	}
	btnALC_SWR->redraw();
}

// trim leading and trailing whitspace and double quote
const std::string lt_trim(const std::string& pstring, const std::string& pWhitespace)
{
	size_t beginStr, endStr, range;
	beginStr = pstring.find_first_not_of(pWhitespace);
	if (beginStr == std::string::npos) return "";	// no content
	endStr = pstring.find_last_not_of(pWhitespace);
	range = endStr - beginStr + 1;

	return pstring.substr(beginStr, range);
}

void editAlphaTag()
{
	int indx;
	std::string atag;
	if (FreqSelect->value() < 1) {
		inAlphaTag->value("");
		return;	// no memory selected
	}
	indx = FreqSelect->value() - 1;
	atag = inAlphaTag->value();
// delete leading, trailing spaces
	atag = lt_trim(atag);
	memset(oplist[indx].alpha_tag, 0, ATAGSIZE);
	snprintf(oplist[indx].alpha_tag, ATAGSIZE, "%s", atag.c_str());
// update browser list
	updateSelect();
	FreqSelect->value(indx + 1);
	inAlphaTag->value(oplist[indx].alpha_tag);
}

//----------------------------------------------------------------------
// button label and label state changes
// Note that an additional level of service request is made to insure
// that the main thread is actually changing the widget

// noise reduction
static std::string nr_label_;
static bool nr_state_;

void do_nr_label(void *)
{
	btnNR->value(nr_state_ ? 1 : 0);
	btnNR->label(nr_label_.c_str());
	btnNR->redraw_label();
}

void nr_label(const char *l, int on)
{
	nr_label_ = l;
	nr_state_ = on;
	progStatus.noise_reduction = on;
	Fl::awake(do_nr_label);
}

// noise blanker
static std::string nb_label_;
static bool nb_state_;

void do_nb_label(void *)
{
	btnNOISE->value(nb_state_ ? 1 : 0);
	btnNOISE->label(nb_label_.c_str());
	btnNOISE->redraw_label();
}

void nb_label(const char * l, int on)
{
	nb_label_ = l;
	nb_state_ = on;
	progStatus.noise = on;
	Fl::awake(do_nb_label);
}

// preamp label
static std::string preamp_label_;
static bool preamp_state_;

void do_preamp_label(void *)
{
	btnPreamp->value(preamp_state_ ? 1 : 0);
	btnPreamp->label(preamp_label_.c_str());
	btnPreamp->redraw_label();
}

void preamp_label(const char * l, int on)
{
	preamp_label_ = l;
	preamp_state_ = on;
	progStatus.preamp = on;
	Fl::awake(do_preamp_label);
}

// atten label
static std::string atten_label_;
static bool atten_state_;

void do_atten_label(void *)
{
	btnAttenuator->value(atten_state_ ? 1 : 0);
	btnAttenuator->label(atten_label_.c_str());
	btnAttenuator->redraw_label();
}

void atten_label(const char * l, int on)
{
	atten_label_ = l;
	atten_state_ = on;
	progStatus.attenuator = on;
	Fl::awake(do_atten_label);
}

// break-in label
static std::string bkin_label_;

void do_bkin_label(void *)
{
	btnBreakIn->label(bkin_label_.c_str());
	btnBreakIn->redraw_label();
}

void break_in_label(const char *l)
{
	bkin_label_ = l;

	Fl::awake(do_bkin_label);
}

// autonotch label
static std::string auto_notch_label_;
static bool auto_notch_state_;

void do_auto_notch_label(void *)
{
	btnAutoNotch->value(auto_notch_state_ ? 1 : 0);
	btnAutoNotch->label(auto_notch_label_.c_str());
	btnAutoNotch->redraw_label();
}

void auto_notch_label(const char * l, int on)
{
	auto_notch_label_ = l;
	auto_notch_state_ = on;
	progStatus.auto_notch = on;
	Fl::awake(do_auto_notch_label);
}

void cbAuxPort()
{
	AuxSerial->setRTS(progStatus.aux_rts);
	AuxSerial->setDTR(progStatus.aux_dtr);
}

void cb_agc_level()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cb_agc_level()");
	selrig->set_agc_level();
}

void cb_cw_wpm()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cb_cw_wpm()");
	selrig->set_cw_wpm();
}

void cb_cw_vol()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cb_cw_vol()");
	selrig->set_cw_vol();
}

void cb_cw_spot()
{
	int ret;
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cb_cw_spot()");
	ret = selrig->set_cw_spot();
	if (!ret) btnSpot->value(0);
}

void cb_cw_spot_tone()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cb_cw_spot_tone()");
	selrig->set_cw_spot_tone();
}


void cb_vox_gain()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cb_vox_gain()");
	selrig->set_vox_gain();
}

void cb_vox_anti()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cb_vox_anti()");
	selrig->set_vox_anti();
}

void cb_vox_hang()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cb_vox_hang()");
	selrig->set_vox_hang();
}

void cb_vox_onoff()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cb_vox_onoff()");
	selrig->set_vox_onoff();
}

void cb_vox_on_dataport()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cb_dataport()");
	selrig->set_vox_on_dataport();
}

void cb_compression()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cb_compression");
	selrig->set_compression(progStatus.compON, progStatus.compression);
}

void cb_auto_notch()
{
	progStatus.auto_notch = btnAutoNotch->value();
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cb_autonotch()");
	selrig->set_auto_notch(progStatus.auto_notch);
}

void cb_vfo_adj()
{
	if (xcvr_name == rig_TT550.name_)
		progStatus.vfo_adj = spnr_tt550_vfo_adj->value();
	else
		progStatus.vfo_adj = spnr_vfo_adj->value();
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cb_vfo_adj()");
	selrig->setVfoAdj(progStatus.vfo_adj);
}

void cb_line_out()
{
}

void cb_bpf_center()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cb_bpf_center()");
	selrig->set_if_shift(selrig->pbt);
}

void cb_special()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cb_special()");
	selrig->set_special(btnSpecial->value());
}

void cbNoise()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cbNoise()");

	int btn, get, cnt = 0;

	btn = progStatus.noise = btnNOISE->value();

	selrig->set_noise(btn);
	MilliSleep(50);
	get = selrig->get_noise();
	while ((get != btn) && (cnt++ < 10)) {
		MilliSleep(progStatus.comm_wait);
		get = selrig->get_noise();
		Fl::awake();
	}
}

void cb_nb_level()
{
	if (!selrig->has_nb_level) return;
	int set = 0;

	trace(1, "cb_nb_level()");

	int ev = Fl::event();
	if (ev == FL_LEAVE || ev == FL_ENTER) return;
	if (ev == FL_DRAG || ev == FL_PUSH) {
		inhibit_nb_level = 1;
		return;
	}
	set = sldr_nb_level->value();
	guard_lock lock(&mutex_serial);
	selrig->set_nb_level(set);
}

void cbNR()
{
	if (!selrig->has_noise_reduction_control) return;
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cbNR()");

	int btn = 0, set = 0, get, cnt = 0;
	if (sldrNR) {
		set = sldrNR->value();
		btn = btnNR->value();
	}
	if (spnrNR) {
		set = spnrNR->value();
		btn = btnNR->value();
	}


	if (xcvr_name == rig_TS2000.name_) {
		if (btn != -1) { // pia
			if (selrig->noise_reduction_level() == 0) {
				selrig->set_noise_reduction(1);
				selrig->set_noise_reduction_val(selrig->nrval1());
				progStatus.noise_reduction = 1;
				progStatus.noise_reduction_val = selrig->nrval1();
			} else if (selrig->currmode() != RIG_TS2000::FM &&
					  selrig->noise_reduction_level() == 1) {
				selrig->set_noise_reduction(2);
				selrig->set_noise_reduction_val(selrig->nrval2());
				progStatus.noise_reduction = 2;
				progStatus.noise_reduction_val = selrig->nrval2();
			} else
				selrig->set_noise_reduction(0);
		} else {
			progStatus.noise_reduction_val = set;
			selrig->set_noise_reduction_val(set);
		}
	} else { // not TS2000

		progStatus.noise_reduction = btn;

		selrig->set_noise_reduction(btn);
		MilliSleep(50);

		get = selrig->get_noise_reduction();
		while ((get != btn) && (cnt++ < 10)) {
			MilliSleep(progStatus.comm_wait);
			get = selrig->get_noise_reduction();
			Fl::awake();
		}
		progStatus.noise_reduction_val = set;
		selrig->set_noise_reduction_val(set);
		MilliSleep(50);
		get = selrig->get_noise_reduction_val();
		cnt = 0;
		while ((get != set) && (cnt++ < 10)) {
			MilliSleep(progStatus.comm_wait);
			get = selrig->get_noise_reduction_val();
			Fl::awake();
		}
	}

}

void setNR()
{
	if (!selrig->has_noise_reduction_control) return;
	trace(1, "setNR()");

	int btn = 0, set = 0;

	int ev = Fl::event();
	if (ev == FL_LEAVE || ev == FL_ENTER) return;
	if (ev == FL_DRAG || ev == FL_PUSH) {
		inhibit_nr = 1;
		return;
	}

	if (xcvr_name == rig_TS2000.name_ ||
		xcvr_name == rig_TS590S.name_ ||
		xcvr_name == rig_TS590SG.name_ ||
		xcvr_name == rig_TS890S.name_ ||
		xcvr_name == rig_TS990.name_) {
		if (sldrNR) {
			set = sldrNR->value();
			btn = -1;
		}
		if (spnrNR) {
			set = spnrNR->value();
			btn = -1;
		}
	} else {
		if (sldrNR) {
			set = sldrNR->value();
			btn = btnNR->value();
		}
		if (spnrNR) {
			set = spnrNR->value();
			btn = btnNR->value();
		}
	}

	guard_lock lock(&mutex_serial);
	selrig->set_noise_reduction_val(set);
	selrig->set_noise_reduction(btn);

}

void cb_spot()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cb_spot()");
	selrig->set_cw_spot();
}

void cb_enable_keyer()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cb_enable_keyer()");
	selrig->enable_keyer();
}

void cb_set_break_in()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cb_set_break_in()");
	selrig->set_break_in();
}

void cb_cw_weight()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cb_cw_weight()");
	selrig->set_cw_weight();
}

void cb_cw_qsk()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cb_cw_qsk()");
	selrig->set_cw_qsk();
}

void cb_cw_delay()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cb_cw_delay()");
	selrig->set_cw_delay();
}

void set_band_label(int band)
{
	switch (band) {
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			break;
		case 10:
			break;
		case 11:
			break;
		case 12:
			break;
		case 13:
			break;
	}
}

void set_band(int band)
{
	switch (band) {
		case 1: progStatus.f160 = vfo->freq;
				progStatus.m160 = vfo->imode;
				progStatus.txT_160 = choice_FT8n_tTONE->value();
				progStatus.rxT_160 = choice_FT8n_rTONE->value();
				progStatus.offset_160 = FMoffset->index();
				progStatus.oF_160 = FMoff_freq->value();
				break; // 160 meters
		case 2: progStatus.f80 = vfo->freq;
				progStatus.m80 = vfo->imode;
				progStatus.txT_80 = choice_FT8n_tTONE->value();
				progStatus.rxT_80 = choice_FT8n_rTONE->value();
				progStatus.offset_80 = FMoffset->index();
				progStatus.oF_80 = FMoff_freq->value();
				break; // 80 meters
		case 3: progStatus.f40 = vfo->freq;
				progStatus.m40 = vfo->imode;
				progStatus.txT_40 = choice_FT8n_tTONE->value();
				progStatus.rxT_40 = choice_FT8n_rTONE->value();
				progStatus.offset_40 = FMoffset->index();
				progStatus.oF_40 = FMoff_freq->value();
				break; // 40 meters
		case 4: progStatus.f30 = vfo->freq;
				progStatus.m30 = vfo->imode;
				progStatus.txT_30 = choice_FT8n_tTONE->value();
				progStatus.rxT_30 = choice_FT8n_rTONE->value();
				progStatus.offset_30 = FMoffset->index();
				progStatus.oF_30 = FMoff_freq->value();
				break; // 30 meters
		case 5: progStatus.f20 = vfo->freq;
				progStatus.m20 = vfo->imode;
				progStatus.txT_20 = choice_FT8n_tTONE->value();
				progStatus.rxT_20 = choice_FT8n_rTONE->value();
				progStatus.offset_20 = FMoffset->index();
				progStatus.oF_20 = FMoff_freq->value();
				break; // 20 meters
		case 6: progStatus.f17 = vfo->freq;
				progStatus.m17 = vfo->imode;
				progStatus.txT_17 = choice_FT8n_tTONE->value();
				progStatus.rxT_17 = choice_FT8n_rTONE->value();
				progStatus.offset_17 = FMoffset->index();
				progStatus.oF_17 = FMoff_freq->value();
				break; // 17 meters
		case 7: progStatus.f15 = vfo->freq;
				progStatus.m15 = vfo->imode;
				progStatus.txT_15 = choice_FT8n_tTONE->value();
				progStatus.rxT_15 = choice_FT8n_rTONE->value();
				progStatus.offset_15 = FMoffset->index();
				progStatus.oF_15 = FMoff_freq->value();
				break; // 15 meters
		case 8: progStatus.f12 = vfo->freq;
				progStatus.m12 = vfo->imode;
				progStatus.txT_12 = choice_FT8n_tTONE->value();
				progStatus.rxT_12 = choice_FT8n_rTONE->value();
				progStatus.offset_12 = FMoffset->index();
				progStatus.oF_12 = FMoff_freq->value();
				break; // 12 meters
		case 9: progStatus.f10 = vfo->freq;
				progStatus.m10 = vfo->imode;
				progStatus.txT_10 = choice_FT8n_tTONE->value();
				progStatus.rxT_10 = choice_FT8n_rTONE->value();
				progStatus.offset_10 = FMoffset->index();
				progStatus.oF_10 = FMoff_freq->value();
				break; // 10 meters
		case 10:progStatus.f6 = vfo->freq;
				progStatus.m6 = vfo->imode;
				progStatus.txT_6 = choice_FT8n_tTONE->value();
				progStatus.rxT_6 = choice_FT8n_rTONE->value();
				progStatus.offset_6 = FMoffset->index();
				progStatus.oF_6 = FMoff_freq->value();
				break; // 6 meters
		case 11:progStatus.f2 = vfo->freq;
				progStatus.m2 = vfo->imode;
				progStatus.txT_2 = choice_FT8n_tTONE->value();
				progStatus.rxT_2 = choice_FT8n_rTONE->value();
				progStatus.offset_2 = FMoffset->index();
				progStatus.oF_2 = FMoff_freq->value();
				break; // 2 meters
		case 12:progStatus.f70 = vfo->freq;
				progStatus.m70 = vfo->imode;
				progStatus.txT_70 = choice_FT8n_tTONE->value();
				progStatus.rxT_70 = choice_FT8n_rTONE->value();
				progStatus.offset_70 = FMoffset->index();
				progStatus.oF_70 = FMoff_freq->value();
				break; // 70 cent'
		case 13:progStatus.f12G = vfo->freq;
				progStatus.m12G = vfo->imode;
				progStatus.txT_12G = choice_FT8n_tTONE->value();
				progStatus.rxT_12G = choice_FT8n_rTONE->value();
				progStatus.offset_12G = FMoffset->index();
				progStatus.oF_12G = FMoff_freq->value();
				break; // 1.2 GHz cent'
	}
}


void updateCTCSS(int band)
{
	switch (band) {
		case 1: choice_FT8n_tTONE->value(progStatus.txT_160);
				choice_FT8n_rTONE->value(progStatus.txT_160);
				FMoffset->index(progStatus.offset_160);
				FMoff_freq->value(progStatus.oF_160 );
				break; // 160 meters
		case 2: choice_FT8n_tTONE->value(progStatus.txT_80);
				choice_FT8n_rTONE->value(progStatus.txT_80);
				FMoffset->index(progStatus.offset_80);
				FMoff_freq->value(progStatus.oF_80 );
				break; // 80 meters
		case 3: choice_FT8n_tTONE->value(progStatus.txT_40);
				choice_FT8n_rTONE->value(progStatus.txT_40);
				FMoffset->index(progStatus.offset_40);
				FMoff_freq->value(progStatus.oF_40 );
				break; // 40 meters
		case 4: choice_FT8n_tTONE->value(progStatus.txT_30);
				choice_FT8n_rTONE->value(progStatus.txT_30);
				FMoffset->index(progStatus.offset_30);
				FMoff_freq->value(progStatus.oF_30 );
				break; // 30 meters
		case 5: choice_FT8n_tTONE->value(progStatus.txT_20);
				choice_FT8n_rTONE->value(progStatus.txT_20);
				FMoffset->index(progStatus.offset_20);
				FMoff_freq->value(progStatus.oF_20 );
				break; // 20 meters
		case 6: choice_FT8n_tTONE->value(progStatus.txT_17);
				choice_FT8n_rTONE->value(progStatus.txT_17);
				FMoffset->index(progStatus.offset_17);
				FMoff_freq->value(progStatus.oF_17 );
				break; // 17 meters
		case 7: choice_FT8n_tTONE->value(progStatus.txT_15);
				choice_FT8n_rTONE->value(progStatus.txT_15);
				FMoffset->index(progStatus.offset_15);
				FMoff_freq->value(progStatus.oF_15 );
				break; // 15 meters
		case 8: choice_FT8n_tTONE->value(progStatus.txT_12);
				choice_FT8n_rTONE->value(progStatus.txT_12);
				FMoffset->index(progStatus.offset_12);
				FMoff_freq->value(progStatus.oF_12 );
				break; // 12 meters
		case 9: choice_FT8n_tTONE->value(progStatus.txT_10);
				choice_FT8n_rTONE->value(progStatus.txT_10);
				FMoffset->index(progStatus.offset_10);
				FMoff_freq->value(progStatus.oF_10 );
				break; // 10 meters
		case 10:choice_FT8n_tTONE->value(progStatus.txT_6);
				choice_FT8n_rTONE->value(progStatus.txT_6);
				FMoffset->index(progStatus.offset_6);
				FMoff_freq->value(progStatus.oF_6 );
				break; // 6 meters
		case 11:choice_FT8n_tTONE->value(progStatus.txT_2);
				choice_FT8n_rTONE->value(progStatus.txT_2);
				FMoffset->index(progStatus.offset_2);
				FMoff_freq->value(progStatus.oF_2 );
				break; // 2 meters
		case 12:choice_FT8n_tTONE->value(progStatus.txT_70);
				choice_FT8n_rTONE->value(progStatus.txT_70);
				FMoffset->index(progStatus.offset_70);
				FMoff_freq->value(progStatus.oF_70 );
				progStatus.m70 = vfo->imode;
				break; // 70 cent'
	}
}

void cbBandSelect(int band)
{
	guard_lock gl_serial(&mutex_serial);

	if (xcvr_name == rig_FT857D.name_ || xcvr_name == rig_FT897D.name_ ) {
		if (Fl::event_button() == FL_LEFT_MOUSE) {
			selrig->set_band_selection(band);
			updateCTCSS(band);
			if (band > 8) { // 10, 6, 2, 70, 1.2G
				int tTONE = PL_tones[choice_FT8n_tTONE->value()];
				int rTONE = PL_tones[choice_FT8n_rTONE->value()];
				selrig->set_tones(tTONE, rTONE);
				selrig->set_offset(FMoffset->index(), FMoff_freq->value());
			}
		} else if (Fl::event_button() == FL_RIGHT_MOUSE)
			set_band(band);
		return;
	}

	if (Fl::event_button() == FL_RIGHT_MOUSE) {
		selrig->rTONE = choice_rTONE->value();
		selrig->tTONE = choice_tTONE->value();
		selrig->set_band_selection(band);
		return;
	}

	selrig->get_band_selection(band);

// get freqmdbw
	if (selrig->inuse == onA) {
		vfoA.freq = selrig->get_vfoA();
		if (selrig->has_mode_control)
			vfoA.imode = selrig->get_modeA();
		if (selrig->has_bandwidth_control) {
			selrig->adjust_bandwidth(vfoA.imode);
			vfoA.iBW = selrig->get_bwA();
		}
		vfo = &vfoA;
	} else {
		vfoB.freq = selrig->get_vfoB();
		if (selrig->has_mode_control)
			vfoB.imode = selrig->get_modeB();
		if (selrig->has_bandwidth_control) {
			selrig->adjust_bandwidth(vfoB.imode);
			vfoB.iBW = selrig->get_bwB();
		}
		vfo = &vfoB;
	}
// local display freqmdbw
	if (selrig->has_mode_control) {
		setModeControl(NULL);
	}
	if (selrig->has_bandwidth_control) {
		set_bandwidth_control();
		setBWControl(NULL);
	}
	if (selrig->inuse == onA) { FreqDispA->value(vfo->freq); FreqDispA->redraw(); }
	else       { FreqDispB->value(vfo->freq); FreqDispB->redraw(); }

	if (selrig->CIV && (selrig->name_ != rig_IC7200.name_)) {
		choice_tTONE->value(selrig->tTONE);
		choice_tTONE->redraw();
		choice_rTONE->value(selrig->rTONE);
		choice_rTONE->redraw();
	}

}

void enable_yaesu_bandselect(int btn_num, bool enable)
{
	switch (btn_num) {
		case 1:
		case 9:
			break;
		case 10:	// 6m
			if (enable) btn_yaesu_select_10->show();
			else btn_yaesu_select_10->hide();
			break;
		case 11:	// GEN
			if (enable) btn_yaesu_select_11->show();
			else btn_yaesu_select_11->hide();
			break;
		case 13:
			if (enable) op_yaesu_select60->show();
			else op_yaesu_select60->hide();
			break;
		default:
			break;
	}
}


