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

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
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

void initTabs();

using namespace std;

rigbase *selrig = rigs[0];

extern bool test;
void init_notch_control();

bool flrig_abort = false;

int freqval = 0;

XCVR_STATE vfoA		= XCVR_STATE(14070000, 0, 0, UI);
XCVR_STATE vfoB		= XCVR_STATE(7070000, 0, 0, UI);
XCVR_STATE *vfo		= &vfoA;

//XCVR_STATE xmlvfo	= XCVR_STATE(14070000, 0, 0, UI);

XCVR_STATE xcvr_vfoA, xcvr_vfoB;

enum {VOL, MIC, PWR, SQL, IFSH, NOTCH, RFGAIN, NR, NB };

queue<VFOQUEUE> srvc_reqs;

bool useB = false;
bool changed_vfo = false;

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
vector<string> rigmodes_;
vector<string> rigbws_;

Cserial *RigSerial;
Cserial *AuxSerial;
Cserial *SepSerial;

bool using_buttons = false;

enum { SWR_IMAGE, ALC_IMAGE };
int meter_image = SWR_IMAGE;

bool xcvr_initialized = false;

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

string printXCVR_STATE(XCVR_STATE &data)
{
	stringstream str;
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

string print_ab()
{
	std::string s;
	s.assign("VFO-A: ");
	s.append(printXCVR_STATE(vfoA));
	s.append("; VFO-B: ");
	s.append(printXCVR_STATE(vfoB));
	return s;
}

char *print(XCVR_STATE &data)
{
	static char str[1024];
	const char **bwt = selrig->bwtable(data.imode);
	const char **dsplo = selrig->lotable(data.imode);
	const char **dsphi = selrig->hitable(data.imode);
	snprintf(
		str, sizeof(str),
		"\
Data Source: %s\n\
  freq ........... %ld\n\
  mode ........... %d [%s]\n\
  filter ......... %s\n\
  bwt index ...... %2d, [%s] [%s]\n\
  split .......... %4d, power_control . %4d, volume_control  %4d\n\
  attenuator ..... %4d, preamp ........ %4d, rf gain ....... %4d\n\
  if_shift ....... %4d, shift val ..... %4d\n\
  auto notch ..... %4d, notch ......... %4d, notch value ... %4d\n\
  noise .......... %4d, nr ............ %4d, nr val ........ %4d\n\
  mic gain ....... %4d, agc level ..... %4d, squelch ....... %4d\n\
  compression .... %4d, compON ........ %4d",
		data.src == XML ? "XML" : data.src == UI ? "UI" :
			data.src == SRVR ? "SRVR" : "RIG",
		data.freq,
		data.imode,
		selrig->modes_ ? selrig->modes_[data.imode] : "modes n/a",
		selrig->has_FILTER ? selrig->FILT(selrig->get_FILT(data.imode)) : "n/a",
		data.iBW,
		(data.iBW > 256 && selrig->has_dsp_controls) ?
			(dsplo ? dsplo[data.iBW & 0x7F] : "??") : (bwt ? bwt[data.iBW] : "n/a"),
		(data.iBW > 256 && selrig->has_dsp_controls) ?
			(dsphi ? dsphi[(data.iBW >> 8) & 0x7F] : "??") : "",
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
	return str;
}

// the following functions are ONLY CALLED by the serial loop
// read any data stream sent by transceiver
// support for the K3 and KX3 read of VFO, MODE and BW are
// in the K3_ui and KX3_ui source files

void read_info()
{
	trace(1,"read_info()");
	selrig->get_info();
}

// read current vfo frequency

void read_vfo()
{
// transceiver changed ?
	trace(1,"read_vfo()");
	long  freq;
	if (!useB) { // vfo-A
		trace(2, "vfoA active", "get vfo A");
		freq = selrig->get_vfoA();
		if (freq != vfoA.freq) {
			vfoA.freq = freq;
			Fl::awake(setFreqDispA, (void *)vfoA.freq);
			vfo = &vfoA;
		}
		if ( selrig->twovfos() ) {
			trace(2, "vfoA active", "get vfo B");
			freq = selrig->get_vfoB();
			if (freq != vfoB.freq) {
				vfoB.freq = freq;
				Fl::awake(setFreqDispB, (void *)vfoB.freq);
			}
		}
	} else { // vfo-B
		trace(2, "vfoB active", "get vfo B");
		freq = selrig->get_vfoB();
		if (freq != vfoB.freq) {
			vfoB.freq = freq;
			Fl::awake(setFreqDispB, (void *)vfoB.freq);
			vfo = &vfoB;
		}
		if ( selrig->twovfos() ) {
			trace(2, "vfoB active", "get vfo A");
			freq = selrig->get_vfoA();
			if (freq != vfoA.freq) {
				vfoA.freq = freq;
				Fl::awake(setFreqDispA, (void *)vfoA.freq);
			}
		}
	}
}

void updateUI(void *)
{
	setModeControl(NULL);
	setBWControl(NULL);
	updateBandwidthControl(NULL);
	highlight_vfo(NULL);
}

void update_vfoAorB(void *d)
{
	if (xcvr_name == rig_FT817.name_) {
		trace(1,"FT817, update_vfoAorB()");
		if (useB) {
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
		return;
	}
	updateUI((void*)0);
}

void read_vfoAorB()
{
	int val;
	if (selrig->has_getvfoAorB) {
		{
			trace(1,"read_vfoAorB()");
			val = selrig->get_vfoAorB();
			if (val == -1) val = 0;
		}
		if (val != useB) {
			useB = val;
			Fl::awake(update_vfoAorB, reinterpret_cast<void*>(val));
			MilliSleep(50);
		}
	}
}

void setModeControl(void *)
{
	opMODE->index(vfo->imode);

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
	init_notch_control();
}

void setFILTER(void *)
{
	if (selrig->has_FILTER) {
		if (useB)
			btnFILT->label(selrig->FILT(vfoB.filter));
		else
			btnFILT->label(selrig->FILT(vfoA.filter));
		btnFILT->redraw_label();
trace(3, "Filter", (useB ? "B" : "A"), btnFILT->label());
	}
}

// mode and bandwidth
void read_mode()
{
	int nu_mode;
	int nu_BW;
	if (!useB) {
		trace(2, "read_mode", "vfoA active");
		nu_mode = selrig->get_modeA();
		vfoA.filter = selrig->get_FILT(nu_mode);
		if (nu_mode != vfoA.imode) {
			{
				vfoA.imode = vfo->imode = nu_mode;
				selrig->adjust_bandwidth(vfo->imode);
				nu_BW = selrig->get_bwA();
				vfoA.iBW = vfo->iBW = nu_BW;
			}
			Fl::awake(setModeControl);
			set_bandwidth_control();
		}
		Fl::awake(setFILTER);
		if (selrig->can_change_alt_vfo) {
			vfoB.imode = selrig->get_modeB();
			vfoB.filter = selrig->get_FILT(vfoB.imode);
		}
	} else {
		trace(2, "read_mode", "vfoB active");
		nu_mode = selrig->get_modeB();
		vfoB.filter = selrig->get_FILT(nu_mode);
		if (nu_mode != vfoB.imode) {
			{
				vfoB.imode = vfo->imode = nu_mode;
				selrig->adjust_bandwidth(vfo->imode);
				nu_BW = selrig->get_bwB();
				vfoB.iBW = vfo->iBW = nu_BW;
			}
			Fl::awake(setModeControl);
			set_bandwidth_control();
		}
		Fl::awake(setFILTER);
		if (selrig->can_change_alt_vfo) {
			vfoA.imode = selrig->get_modeA();
			vfoA.filter = selrig->get_FILT(vfoA.imode);
		}
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
	trace(1,"read_bandwidth()");
	int nu_BW;
	if (!useB) {
		trace(2, "vfoA active", "get_bwA()");
		nu_BW = selrig->get_bwA();
		if (nu_BW != vfoA.iBW) {
			stringstream s;
			s << "Bandwidth A change. nu_BW=" << nu_BW << ", vfoA.iBW=" << vfoA.iBW << ", vfo->iBW=" << vfo->iBW;
			trace(1, s.str().c_str());
			
			vfoA.iBW = vfo->iBW = nu_BW;
//			Fl::awake(setBWControl);
		}
	} else {
		trace(2, "vfoB active", "get_bwB()");
		nu_BW = selrig->get_bwB();
		if (nu_BW != vfoB.iBW) {
			stringstream s;
			s << "Bandwidth B change. nu_BW=" << nu_BW << ", vfoB.iBW=" << vfoB.iBW << ", vfo->iBW=" << vfo->iBW;
			trace(1, s.str().c_str());
			
			vfoB.iBW = vfo->iBW = nu_BW;
//			Fl::awake(setBWControl);
		}
	}
	Fl::awake(setBWControl);
}

// read current signal level
int mval = 0;
void read_smeter()
{
	if (!selrig->has_smeter) return;
	int  sig;
	{
		trace(1,"read_smeter()");
		sig = selrig->get_smeter();
	}
	if (sig == -1) return;
	mval = sig;
	Fl::awake(updateSmeter, reinterpret_cast<void*>(sig));
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
		stringstream s;
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
	mval = sig;
	Fl::awake(updateFwdPwr, reinterpret_cast<void*>(sig));
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
	if (sig > -1)
		Fl::awake(updateSWR, reinterpret_cast<void*>(sig));
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
	if (sig > -1)
		Fl::awake(updateALC, reinterpret_cast<void*>(sig));
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

void read_preamp_att()
{
	int val;
	if (selrig->has_preamp_control) {
		{
			trace(1,"read_preamp_att()  1");
			val = selrig->get_preamp();
		}
		if (val != progStatus.preamp || val != vfo->preamp || (btnPreamp && val != btnPreamp->value())) {
			vfo->preamp = progStatus.preamp = val;
			Fl::awake(update_preamp, (void*)0);
		}
	}
	if (selrig->has_attenuator_control) {
		{
			trace(1,"read_preamp_att()  2");
			val = selrig->get_attenuator();
		}
		if (val != progStatus.attenuator || val != vfo->attenuator || (btnAttenuator && val != btnAttenuator->value())) {
			vfo->attenuator = progStatus.attenuator = val;
			Fl::awake(update_attenuator, (void*)0);
		}
	}
}

// split
void update_split(void *d)
{
	/*
	if (xcvr_name == rig_yaesu.name_ || xcvr_name == rig_FTdx1200.name_ ||
		xcvr_name == rig_TS480SAT.name_ || xcvr_name == rig_TS480HX.name_ ||
		xcvr_name == rig_TS590S.name_ || xcvr_name == rig_TS590SG.name_ ||
		xcvr_name == rig_TS890S.name_ ||
		xcvr_name == rig_TS2000.name_ || xcvr_name == rig_TS990.name_) {
		switch (progStatus.split) {
			case 0: btnSplit->value(0);
					useB = false;
					highlight_vfo(NULL);
					break;
			case 1: btnSplit->value(1);
					useB = true;
					highlight_vfo(NULL);
					break;
			case 2:	btnSplit->value(1);
					useB = false;
					highlight_vfo(NULL);
					break;
			case 3: btnSplit->value(0);
					useB = true;
					highlight_vfo(NULL);
					break;
		}
	} else
*/
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
		ostringstream s;
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
	{
		trace(1,"read_if_shift()");
		on = selrig->get_if_shift(val);
	}
	if ((on != progStatus.shift) || (val != progStatus.shift_val)) {
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
	int on = progStatus.notch;
	int val = progStatus.notch_val;
	if (inhibit_notch > 0) {
		inhibit_notch--;
		return;
	}
	if (!selrig->has_notch_control) return;
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
	set_power_controlImage(progStatus.power_level);
	if (sldrPOWER) {
		sldrPOWER->value(progStatus.power_level);
		sldrPOWER->redraw();
	}
	if (spnrPOWER) {
		spnrPOWER->value(progStatus.power_level);
		spnrPOWER->redraw();
	}
	if (xcvr_name == rig_K2.name_) {
		double min, max, step;
		selrig->get_pc_min_max_step(min, max, step);
		if (sldrPOWER) sldrPOWER->minimum(min);
		if (sldrPOWER) sldrPOWER->maximum(max);
		if (sldrPOWER) sldrPOWER->step(step);
		if (sldrPOWER) sldrPOWER->redraw();
		if (spnrPOWER) spnrPOWER->minimum(min);
		if (spnrPOWER) spnrPOWER->maximum(max);
		if (spnrPOWER) spnrPOWER->step(step);
		if (spnrPOWER) spnrPOWER->redraw();
	}
}

void read_power_control()
{
	int val;
	if (inhibit_power > 0) {
		inhibit_power--;
		return;
	}
	if (!selrig->has_power_control) return;
	{
		trace(1,"read_power_control()");
		val = selrig->get_power_control();
	}
	if (val != progStatus.power_level  ||  val != vfo->power_control  ||  (sldrPOWER && val != sldrPOWER->value()) ) {
		stringstream s;
		s << "read_power_control(), UPDATE progStatus.power_level=" << progStatus.power_level << ", vfo->power_control=" << vfo->power_control << ", radio power=" << val << ", sldrPOWER->value()=" << (sldrPOWER ? sldrPOWER->value() : -1);
		trace(1, s.str().c_str());

		vfo->power_control = progStatus.power_level = val;
		Fl::awake(update_power_control, (void*)0);
	}
	//else
	//{
	//	stringstream s;
	//	s << "read_power_control(), CURRENT progStatus.power_level=" << progStatus.power_level << ", vfo->power_control=" << vfo->power_control << ", read power=" << val << ", sldrPOWER->value()=" << (sldrPOWER ? sldrPOWER->value() : -1);
	//	trace(1, s.str().c_str());
	//}
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

struct POLL_PAIR {
	int *poll;
	void (*pollfunc)();
};

POLL_PAIR RX_poll_pairs[] = {
	{&progStatus.poll_vfoAorB, read_vfoAorB},
	{&progStatus.poll_frequency, read_vfo},
	{&progStatus.poll_mode, read_mode},
	{&progStatus.poll_bandwidth, read_bandwidth},
	{&progStatus.poll_smeter, read_smeter},
	{&progStatus.poll_tuner, read_tuner},
	{&progStatus.poll_volume, read_volume},
	{&progStatus.poll_auto_notch, read_auto_notch},
	{&progStatus.poll_notch, read_notch},
	{&progStatus.poll_ifshift, read_ifshift},
	{&progStatus.poll_power_control, read_power_control},
	{&progStatus.poll_pre_att, read_preamp_att},
	{&progStatus.poll_micgain, read_mic_gain},
	{&progStatus.poll_squelch, read_squelch},
	{&progStatus.poll_rfgain, read_rfgain},
	{&progStatus.poll_split, read_split},
	{&progStatus.poll_nr, read_nr},
	{&progStatus.poll_noise, read_noise},
	{&progStatus.poll_compression, read_compression},
	{NULL, NULL}
};

POLL_PAIR TX_poll_pairs[] = {
	{&progStatus.poll_pout, read_power_out},
	{&progStatus.poll_swr, read_swr},
	{&progStatus.poll_alc, read_alc},
	{&progStatus.poll_split, read_split},
	{NULL, NULL}
};

POLL_PAIR *poll_parameters;

static bool resetrcv = true;
static bool resetxmt = true;

// On the Yaesu FT-891, the mode must be set before VFO, since mode 
// changes can shift frequency.
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
}
void yaesu891UpdateB(XCVR_STATE * newVfo)
{
	selrig->set_modeB(newVfo->imode);
	selrig->set_vfoB(newVfo->freq);
	selrig->set_bwB(newVfo->iBW);
}

void serviceQUE()
{
	guard_lock que_lock(&mutex_srvc_reqs, "serviceQUE");
	guard_lock serial(&mutex_serial);

	queue<VFOQUEUE> pending; // creates an empty queue

	VFOQUEUE nuvals;

	while (!srvc_reqs.empty()) {
		{
			nuvals = srvc_reqs.front();
			srvc_reqs.pop();
		}

		if (nuvals.change == ON || nuvals.change == OFF) { // PTT processing

			if (selrig->ICOMmainsub && useB) {  // disallowed operation
				Fl::awake(update_UI_PTT);
				return;
			}

			PTT = (nuvals.change == ON);
			if (nuvals.change == ON) trace(1,"ptt ON");
			else trace(1,"ptt OFF");
			rigPTT(PTT);
			int get = selrig->get_PTT();
			int cnt = 0;
			while ((get != PTT) && (cnt++ < 100)) {
				MilliSleep(10);
				get = selrig->get_PTT();
			}
			{
				stringstream s;
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
				if (useB)
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
				useB = false;
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
				useB = true;
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
					progStatus.split = on;
					Fl::awake(update_split, (void *)0);
					if (selrig->ICOMmainsub) {
						useB = false;
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
			default:
				trace(2, "default ", printXCVR_STATE(nuvals.vfo).c_str());
				if (useB) serviceB(nuvals.vfo);
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
	if (nuvals.freq == 0) nuvals.freq = vfoA.freq;
	if (nuvals.imode == -1) nuvals.imode = vfoA.imode;
//	find_bandwidth(nuvals);
	if (nuvals.iBW == 255) nuvals.iBW = vfoA.iBW;

	if (useB) {
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
				}
			}
			if (vfoA.iBW != nuvals.iBW)
				selrig->set_bwA(nuvals.iBW);
			if (vfoA.freq != nuvals.freq)
				selrig->set_vfoA(nuvals.freq);
			vfoA = nuvals;
		} else if (xcvr_name != rig_TT550.name_) {
			trace(2, "B active, set vfo A", printXCVR_STATE(nuvals).c_str());
			rig_trace(2, "B active, set vfo A", printXCVR_STATE(nuvals).c_str());
			useB = false;
			selrig->selectA();
			if (vfoA.imode != nuvals.imode)
				selrig->set_modeA(nuvals.imode);
			if (vfoA.iBW != nuvals.iBW)
				selrig->set_bwA(nuvals.iBW);
			if (vfoA.freq != nuvals.freq)
				selrig->set_vfoA(nuvals.freq);
			useB = true;
			selrig->selectB();
			vfoA = nuvals;
		}
		Fl::awake(setFreqDispA, (void *)nuvals.freq);
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
			set_bandwidth_control();
			selrig->set_bwA(vfoA.iBW);
			if (m1.find("CW") != std::string::npos ||
				m2.find("CW") != std::string::npos)
				vfoA.freq = nuvals.freq = selrig->get_vfoA();
		}
		
	}
	if (vfoA.iBW != nuvals.iBW) {
		selrig->set_bwA(vfoA.iBW = nuvals.iBW);
	}
	if (vfoA.freq != nuvals.freq) {
		trace(1, "change vfoA frequency");
		selrig->set_vfoA(vfoA.freq = nuvals.freq);
}
	vfo = &vfoA;

	Fl::awake(setFreqDispA, (void *)vfoA.freq);
//	Fl::awake(updateUI);  // may be redundant
}

void serviceB(XCVR_STATE nuvals)
{
	if (nuvals.freq == 0) nuvals.freq = vfoB.freq;
	if (nuvals.imode == -1) nuvals.imode = vfoB.imode;
	if (nuvals.iBW == 255) nuvals.iBW = vfoB.iBW;

	if (!useB) {
		if (selrig->can_change_alt_vfo) {
			trace(2, "A active, set alt vfo B", printXCVR_STATE(nuvals).c_str());
			if (vfoB.imode != nuvals.imode) {
				if (selrig->name_ == rig_FT891.name_) {
					// Mode change on ft891 requires mode first, so set all values
					yaesu891UpdateB(&nuvals);
					vfoB = nuvals;
				} else {
					selrig->set_modeB(nuvals.imode);
				}
			}
			if (vfoB.iBW != nuvals.iBW)
				selrig->set_bwB(nuvals.iBW);
			if (vfoB.freq != nuvals.freq)
				selrig->set_vfoB(nuvals.freq);
			vfoB = nuvals;
		} else if (xcvr_name != rig_TT550.name_) {
			trace(2, "A active, set vfo B", printXCVR_STATE(nuvals).c_str());
			useB = true;
			selrig->selectB();
			if (vfoB.imode != nuvals.imode)
				selrig->set_modeB(nuvals.imode);
			if (vfoB.iBW != nuvals.iBW)
				selrig->set_bwB(nuvals.iBW);
			if (vfoB.freq != nuvals.freq)
				selrig->set_vfoB(nuvals.freq);
			useB = false;
			selrig->selectA();
			vfoB = nuvals;
		}
		Fl::awake(setFreqDispB, (void *)nuvals.freq);
		return;
	}

	trace(2, "service VFO B", printXCVR_STATE(nuvals).c_str());

	if ((nuvals.imode != -1) && (vfoB.imode != nuvals.imode)) {
		std::string m1, m2;
		m1 = selrig->modes_[nuvals.imode];
		m2 = selrig->modes_[vfoB.imode];
		selrig->set_modeB(vfoB.imode = nuvals.imode);
		set_bandwidth_control();
		selrig->set_bwB(vfoB.iBW);
		if (m1.find("CW") != std::string::npos ||
			m2.find("CW") != std::string::npos)
			vfoB.freq = nuvals.freq = selrig->get_vfoB();
	}
	if (vfoB.iBW != nuvals.iBW) {
		selrig->set_bwB(vfoB.iBW = nuvals.iBW);
	}
	if (vfoB.freq != nuvals.freq)
		selrig->set_vfoB(vfoB.freq = nuvals.freq);

	vfo = &vfoB;

	Fl::awake(setFreqDispB, (void *)vfoB.freq);

}

bool close_rig = false;

void * serial_thread_loop(void *d)
{
  static int  loopcount = progStatus.serloop_timing / 10;
  static int  poll_nbr = 0;

	for(;;) {
		if (!run_serial_thread) break;

		MilliSleep(10);

		if (bypass_serial_thread_loop) {
			goto serial_bypass_loop;
		}

		if (close_rig) {
			trace(1, "serial_thread_loop: close_rig");
			return NULL;
		}

//send any freq/mode/bw changes in the queu

		if (!srvc_reqs.empty())
			serviceQUE();

		if (!PTT) {
			if (resetrcv) {
				Fl::awake(zeroXmtMeters, 0);
				resetrcv = false;
				loopcount = progStatus.serloop_timing / 10;
				poll_nbr = 0;
			}
			resetxmt = true;

			if (--loopcount <= 0) {
				loopcount = progStatus.serloop_timing / 10;
				poll_nbr++;

				if (xcvr_name == rig_K3.name_) {
					read_K3();
				}
				else if (xcvr_name == rig_KX3.name_) {
					read_KX3();
				}
				if ((xcvr_name == rig_K2.name_) ||
					(selrig->has_get_info &&
						(progStatus.poll_frequency ||
						 progStatus.poll_mode ||
						 progStatus.poll_bandwidth) ) )
					read_info();
				if (bypass_serial_thread_loop) {
					trace(1, "bypass serial thread loop");
					goto serial_bypass_loop;// continue;
				}
				poll_parameters = &RX_poll_pairs[0];
				while (poll_parameters->poll) {
					// need to put thread asleep to allow other threads
					// access to serial mutex
					MilliSleep(1);

					if (!srvc_reqs.empty()) goto serial_bypass_loop;//break;
					if (PTT) {
						trace(1, "PTT detected");
						goto serial_bypass_loop;//break;
					}
					if (bypass_serial_thread_loop) {
						trace(1, "bypass_serial_thread_loop");
						goto serial_bypass_loop;//break;
					}
					if (*(poll_parameters->poll) && !(poll_nbr % *(poll_parameters->poll))) {
						guard_lock serial_lock(&mutex_serial);
						(poll_parameters->pollfunc)();
					}
					poll_parameters++;
				}
			}
		} else {
			if (resetxmt) {
				Fl::awake(updateSmeter, (void *)(0));
				resetxmt = false;
				loopcount = progStatus.serloop_timing / 10;
				poll_nbr = 0;
			}
			resetrcv = true;
			if (!srvc_reqs.empty()) goto serial_bypass_loop;
			if (--loopcount <= 0) {
				loopcount = progStatus.serloop_timing / 10;
				poll_nbr++;
				poll_parameters = &TX_poll_pairs[0];
				while (poll_parameters->poll) {
					MilliSleep(1);
					if (!srvc_reqs.empty()) goto serial_bypass_loop;
					if (!PTT) {
						trace(1, "!PTT detected");
						goto serial_bypass_loop;
					}
					if (*(poll_parameters->poll) && !(poll_nbr % *(poll_parameters->poll))) {
						guard_lock serial_lock(&mutex_serial);
						(poll_parameters->pollfunc)();
					}
					poll_parameters++;
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
	srvc_reqs.push( VFOQUEUE((useB ? vB : vA), fm));
}

void setDSP()
{
	XCVR_STATE fm = *vfo;
	fm.src = UI;
	fm.iBW = ((opDSP_hi->index() << 8) | 0x8000) | (opDSP_lo->index() & 0xFF) ;
	guard_lock que_lock( & mutex_srvc_reqs, "setDSP" );
	srvc_reqs.push ( VFOQUEUE((useB ? vB : vA), fm));
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
	useB ? vfoB.iBW = vfo->iBW : vfoA.iBW = vfo->iBW;
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
							useB ? vfoB.filter : vfoA.filter));
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
		if (useB) fm.filter = selrig->get_FILT(fm.imode);
	}

	guard_lock que_lock( &mutex_srvc_reqs, "setMode" );
	srvc_reqs.push(VFOQUEUE( (useB ? vB : vA), fm));
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
	char szline[80 + ATAGSIZE];
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
		for (i = 0; i < ATAGSIZE - 1; i++) {
			szatag[i] = oplist[n].alpha_tag[i];
			if (szatag[i] == 0) szatag[i] = ' ';
		}
		bg_clr = (n % 2) ? bg1 : bg2;
		snprintf(szline, sizeof(szline),
			"@B%d@r%.3f\t@B%d@r%s\t@B%d@r%s\t@B%d@r%s", bg_clr,
			oplist[n].freq / 1000.0, bg_clr,
			selrig->get_bwname_(oplist[n].iBW, oplist[n].imode), bg_clr,
			selrig->get_modename_(oplist[n].imode), bg_clr,
			szatag );
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
	ifstream iList(defFileName.c_str());
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
	ifstream iList(defFileName.c_str());
	if (!iList) {
		fl_message ("Could not open %s", defFileName.c_str());
		return;
	}
	clearList();
	int i = 0, mode, bw;
	long freq;
	string atag;
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
	string tmpFN, orgFN;
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
	XCVR_STATE nuvfo = vfoA;
	nuvfo.freq = FreqDispA->value();
	nuvfo.src = UI;
	guard_lock que_lock(&mutex_srvc_reqs, "movFreqA");
	srvc_reqs.push(VFOQUEUE(vA, nuvfo));
	return 1;
}

int movFreqB() {
	XCVR_STATE nuvfo = vfoB;
	nuvfo.freq = FreqDispB->value();
	nuvfo.src = UI;
	guard_lock que_lock(&mutex_srvc_reqs, "movFreqB");
	srvc_reqs.push(VFOQUEUE(vB, nuvfo));
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
			useB = false;
		} else if (selrig->ICOMrig) {
			if (useB) {
				useB = false;
				selrig->selectA();
				vfo = &vfoA;
			} else {
				useB = true;
				selrig->selectB();
				vfo = &vfoB;
			}
		} else if (selrig->name_ == rig_FT891.name_) {
			// No need for extra select, as swapAB accomplishes this
			if (useB) {
				useB = false;
				vfo = &vfoA;
				// Restore mode, then frequency and bandwidth after swap.
				yaesu891UpdateA(&vfoA);
			}
			else {
				useB = true;
				vfo = &vfoB;
				// Restore mode, then frequency and bandwidth after swap.
				yaesu891UpdateB(&vfoB);
			}
		} else {
			XCVR_STATE temp = vfoB;
			vfoB = vfoA;
			vfoA = temp;
			if (useB) {
				selrig->selectB();
				vfo = &vfoB;
			} else {
				selrig->selectA();
				vfo = &vfoA;
			}
		}
	} else {
		if (useB) {
			XCVR_STATE vfotemp = vfoA;
			selrig->selectA();
			vfoA = vfoB;
			selrig->set_vfoA(vfoA.freq);
			selrig->set_modeA(vfoA.imode);
			selrig->set_bwA(vfoA.iBW);

			selrig->selectB();
			vfoB = vfotemp;
			selrig->set_vfoB(vfoB.freq);
			selrig->set_modeB(vfoB.imode);
			selrig->set_bwB(vfoB.iBW);
			vfo = &vfoB;
		} else {
			XCVR_STATE vfotemp = vfoB;
			selrig->selectB();
			vfoB = vfoA;
			selrig->set_vfoB(vfoB.freq);
			selrig->set_modeB(vfoB.imode);
			selrig->set_bwB(vfoB.iBW);

			selrig->selectA();
			vfoA = vfotemp;
			selrig->set_vfoA(vfoA.freq);
			selrig->set_modeA(vfoA.imode);
			selrig->set_bwA(vfoA.iBW);
			vfo = &vfoA;
		}
	}
	Fl::awake(updateUI);
}

void cbAswapB()
{
	guard_lock lock(&mutex_srvc_reqs, "cbAswapB");
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

void execute_A2B()
{
	if (xcvr_name == rig_K3.name_) {
		K3_A2B();
	} else if (xcvr_name == rig_KX3.name_) {
		KX3_A2B();
	} else if (xcvr_name == rig_K2.name_) {
		trace(1,"cbA2B() 1");
		vfoB = vfoA;
		selrig->set_vfoB(vfoB.freq);
		FreqDispB->value(vfoB.freq);
	}
	if (selrig->ICOMmainsub) {
		useB = false;
		selrig->selectA();
		selrig->A2B();
		vfoB = vfoA;
		vfo = &vfoA;
	} else if (selrig->has_a2b) {
		trace(1,"cbA2B() 2");
		selrig->A2B();
		if (useB) {
			vfoA = vfoB;
			FreqDispA->value(vfoA.freq);
		} else {
			vfoB = vfoA;
			FreqDispB->value(vfoB.freq);
		}
	} else {
		if (useB) {
			vfoA = vfoB;
			if (selrig->name_ == rig_FT891.name_) {
				yaesu891UpdateA(&vfoA);
			} else {
				selrig->set_vfoA(vfoA.freq);
				selrig->set_modeA(vfoA.imode);
				selrig->set_bwA(vfoA.iBW);
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
			}
			FreqDispB->value(vfoB.freq);
		}
	}
	Fl::awake(updateUI);
}

void highlight_vfo(void *d)
{
	Fl_Color norm_fg = fl_rgb_color(progStatus.fg_red, progStatus.fg_green, progStatus.fg_blue);
	Fl_Color norm_bg = fl_rgb_color(progStatus.bg_red, progStatus.bg_green, progStatus.bg_blue);
	Fl_Color dim_bg = fl_color_average( norm_bg, FL_BLACK, 0.75);
	FreqDispA->value(vfoA.freq);
	FreqDispB->value(vfoB.freq);
	if (useB) {
		FreqDispA->SetONOFFCOLOR( norm_fg, dim_bg );
		FreqDispB->SetONOFFCOLOR( norm_fg, norm_bg );
		btnA->value(0);
		btnB->value(1);
	} else {
		FreqDispA->SetONOFFCOLOR( norm_fg, norm_bg );
		FreqDispB->SetONOFFCOLOR( norm_fg, dim_bg);
		btnA->value(1);
		btnB->value(0);
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
	if (!useB) {
		FreqDispA->value(fm.freq);
		guard_lock que_lock(&mutex_srvc_reqs, "selectFreq on A");
		srvc_reqs.push(VFOQUEUE(vA, fm));
	} else {
		FreqDispB->value(fm.freq);
		guard_lock que_lock(&mutex_srvc_reqs, "selectFreq on B");
		srvc_reqs.push(VFOQUEUE(vB, fm));
	}
}

void select_and_close()
{
	switch (Fl::event_button()) {
		case FL_LEFT_MOUSE:
			if (FreqSelect->value() > 0)
				inAlphaTag->value(oplist[FreqSelect->value() - 1].alpha_tag);
			if (Fl::event_clicks()) { // double click
				selectFreq();
				cbCloseMemory();
			}
			break;
		case FL_RIGHT_MOUSE:
			if (FreqSelect->value() > 0)
				inAlphaTag->value(oplist[FreqSelect->value() - 1].alpha_tag);
			selectFreq();
			break;
		default:
			break;
	}

// update Alpha Tag field when keyboard scrolling
	switch (Fl::event_key()) {
		case FL_Up:
		case FL_Down:
			if (FreqSelect->value() > 0)
				inAlphaTag->value(oplist[FreqSelect->value() - 1].alpha_tag);
			break;
		default:
			break;
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
	if (useB) {
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
	if (selrig->has_rit)
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
	progStatus.attenuator = selrig->next_attenuator();
	selrig->set_attenuator(progStatus.attenuator);
}

void setAttControl(void *d)
{
	int val = (long)d;
	btnAttenuator->value(val);
}

void cbPreamp()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cbPreamp()");
	progStatus.preamp = selrig->next_preamp();
	selrig->set_preamp(progStatus.preamp);
}

void setPreampControl(void *d)
{
	int val = (long)d;
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
	trace(1, "cbbtnNotch()");

	int btn, cnt = 0;

	btn = btnNotch->value();
	progStatus.notch = btn;

	selrig->set_notch(btn, progStatus.notch_val);

	MilliSleep(progStatus.comm_wait);
	int on, val = progStatus.notch_val;
	on = selrig->get_notch(val);
	while ((on != btn) && (cnt++ < 10)) {
		MilliSleep(progStatus.comm_wait);
		on = selrig->get_notch(val);
		Fl::awake();
	}
}

void setNotch()
{
	if (!selrig->has_notch_control) return;

	trace(1, "setNotch()");

	int ev = Fl::event();
	if (ev == FL_LEAVE || ev == FL_ENTER) return;
	if (ev == FL_DRAG || ev == FL_PUSH) {
		inhibit_notch = 1;
		return;
	}

	int set = 0;
	if (sldrNOTCH) {
		set = sldrNOTCH->value();
	} else {
		set = spnrNOTCH->value();
	}

	progStatus.notch_val = set;
	guard_lock lock( &mutex_serial);
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
	int val = (long)d;
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
	MilliSleep(progStatus.comm_wait);
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
	if (progStatus.pbt_lock) {
		sldrOUTER->value(sldrINNER->value());
		sldrOUTER->redraw();
	}

	int ev = Fl::event();
	if (ev == FL_LEAVE || ev == FL_ENTER) return;
	if (ev == FL_DRAG || ev == FL_PUSH) {
		inhibit_pbt = 1;
		return;
	}
	progStatus.pbt_inner = sldrINNER->value();
	guard_lock lock(&mutex_serial);
	selrig->set_pbt_inner(progStatus.pbt_inner);
	selrig->set_pbt_outer(progStatus.pbt_outer);
}

void setOUTER()
{
	if (progStatus.pbt_lock) {
		sldrINNER->value(sldrOUTER->value());
		sldrINNER->redraw();
	}

	int ev = Fl::event();
	if (ev == FL_LEAVE || ev == FL_ENTER) return;
	if (ev == FL_DRAG || ev == FL_PUSH) {
		inhibit_pbt = 1;
		return;
	}

	progStatus.pbt_outer = sldrOUTER->value();
	if (progStatus.pbt_lock) {
		progStatus.pbt_inner = progStatus.pbt_outer;
		sldrINNER->value(progStatus.pbt_outer);
		sldrINNER->redraw();
	}
	guard_lock lock(&mutex_serial);
	selrig->set_pbt_outer(progStatus.pbt_outer);
	selrig->set_pbt_inner(progStatus.pbt_inner);
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
	if (ev == FL_DRAG || ev == FL_PUSH) {
		inhibit_volume = 1;
		return;
	}
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
	MilliSleep(progStatus.comm_wait);
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
	stringstream str;
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
	if (progStatus.pwr_scale == 0 || (progStatus.pwr_scale == 5 && pwr <= 5.0)) {
		if (img != 1) {
			img = 1;
			scalePower->image(image_p5);
			sldrFwdPwr->maximum(5.0);
			sldrFwdPwr->minimum(0.0);
			scalePower->redraw();
		}
	}
	else if (progStatus.pwr_scale == 1 || (progStatus.pwr_scale == 5 && pwr <= 25.0)) {
		if (img != 2) {
			img = 2;
			scalePower->image(image_p25);
			sldrFwdPwr->maximum(25.0);
			sldrFwdPwr->minimum(0.0);
			scalePower->redraw();
		}
	}
	else if (progStatus.pwr_scale == 2 || (progStatus.pwr_scale == 5 && pwr <= 50.0)) {
		if (img != 3) {
			img = 3;
			scalePower->image(image_p50);
			sldrFwdPwr->maximum(50.0);
			sldrFwdPwr->minimum(0.0);
			scalePower->redraw();
		}
	}
	else if (progStatus.pwr_scale == 3 || (progStatus.pwr_scale == 5 && pwr <= 100.0)) {
		if (img != 4) {
			img = 4;
			scalePower->image(image_p100);
			sldrFwdPwr->maximum(100.0);
			sldrFwdPwr->minimum(0.0);
			scalePower->redraw();
		}
	}
	else if (progStatus.pwr_scale == 4 || (pwr > 100.0)) {
		if (img != 5) {
			img = 5;
			scalePower->image(image_p200);
			sldrFwdPwr->maximum(200.0);
			sldrFwdPwr->minimum(0.0);
			scalePower->redraw();
		}
	}
	return;
}

void setPower()
{
	int ev = Fl::event();
	if (ev == FL_LEAVE || ev == FL_ENTER) return;
	if (ev == FL_DRAG || ev == FL_PUSH) {
		inhibit_power = 1;
		return;
	}
	stringstream str;
	str << "setPower(), ev=" << ev << ", inhibit_power=" << inhibit_power;
	trace(1, str.str().c_str());

	int set = 0;

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


void cbPTT()
{
	setPTT(reinterpret_cast<void *>(btnPTT->value()));
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

	btnAGC->label(lbl);
	btnAGC->redraw_label();

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

void updateALC(void * d)
{
	if (meter_image != ALC_IMAGE) return;
	double data = (long)d;
	sldrRcvSignal->hide();
	sldrSWR->hide();
	sldrALC->show();
	sldrALC->value(data);
	sldrALC->redraw();
}

void updateSWR(void * d)
{
	if (meter_image != SWR_IMAGE) return;
	double data = (long)d;
	if (selrig->has_swr_control) {
		sldrRcvSignal->hide();
		sldrALC->hide();
		sldrSWR->show();
	}
	sldrSWR->value(data);
	sldrSWR->redraw();
}

void updateFwdPwr(void *d)
{
	double power = (long)d;
	if (!sldrFwdPwr->visible()) {
		sldrFwdPwr->show();
	}
	if (xcvr_name == rig_FT817.name_) power /= 10;
	if (xcvr_name == rig_KX3.name_ && selrig->power_10x()) power /= 10;
	sldrFwdPwr->value(power);

	sldrFwdPwr->redraw();
	if (selrig->has_power_control)
		set_power_controlImage(power);
}

void updateSquelch(void *d)
{
	if (sldrSQUELCH) sldrSQUELCH->value((long)d);
	if (sldrSQUELCH) sldrSQUELCH->redraw();
	if (spnrSQUELCH) spnrSQUELCH->value((long)d);
	if (spnrSQUELCH) spnrSQUELCH->redraw();
}

void updateRFgain(void *d)
{
	if (spnrRFGAIN) {
		spnrRFGAIN->value((long)d);
		spnrRFGAIN->redraw();
	}
	if (sldrRFGAIN) {
		sldrRFGAIN->value((long)d);
		sldrRFGAIN->redraw();
	}
}

void zeroXmtMeters(void *d)
{
	sldrFwdPwr->clear();
	sldrALC->clear();
	sldrSWR->clear();
	updateFwdPwr(0);
	updateALC(0);
	updateSWR(0);
}

void setFreqDispA(void *d)
{
	long f = (long)d;
	FreqDispA->value(f);
	FreqDispA->redraw();
}

void setFreqDispB(void *d)
{
	long f = (long)d;
	FreqDispB->value(f);
	FreqDispB->redraw();
}

void updateSmeter(void *d) // 0 to 100;
{
	double smeter = (long)d;
	if (!sldrRcvSignal->visible()) {
		sldrRcvSignal->show();
		sldrFwdPwr->hide();
		sldrALC->hide();
		sldrSWR->hide();
	}
	sldrRcvSignal->value(smeter);
	sldrRcvSignal->redraw();
}

void saveFreqList()
{
	string atag;

	if (!numinlist) return;

	rotate_log(defFileName);

	ofstream oList(defFileName.c_str());
	if (!oList) {
		fl_message ("Could not write to %s", defFileName.c_str());
		return;
	}
	for (int i = 0; i < numinlist; i++) {
		atag = oplist[i].alpha_tag;
		oList << oplist[i].freq << " " << oplist[i].imode << " " << oplist[i].iBW << " \"" << atag.c_str() << "\"" << endl;

	}
	oList.close();
}

void setPTT( void *d)
{
	guard_lock que_lock(&mutex_srvc_reqs, "setPTT");

	int set = (long)d;

	VFOQUEUE xcvrptt;
	if (set) xcvrptt.change = ON;
	else     xcvrptt.change = OFF;
	srvc_reqs.push(xcvrptt);
}

void update_progress(int val)
{
	progress->value(val);
	Fl::check();
}

void restore_rig_vals_(XCVR_STATE &xcvrvfo)
{
	if (progStatus.restore_pre_att) {
		selrig->set_attenuator(xcvrvfo.attenuator);
		selrig->set_preamp(xcvrvfo.preamp);
	}
	update_progress(progress->value() + 5);

	if (progStatus.restore_auto_notch)
		selrig->set_auto_notch(xcvrvfo.auto_notch);
	if (progStatus.restore_split)
		selrig->set_split(xcvrvfo.split);
	update_progress(progress->value() + 5);

	if (progStatus.restore_power_control)
		selrig->set_power_control(xcvrvfo.power_control);
	if (progStatus.restore_volume)
		selrig->set_volume_control(xcvrvfo.volume_control);
	update_progress(progress->value() + 5);

	if (progStatus.restore_if_shift)
		selrig->set_if_shift(xcvrvfo.if_shift);
	update_progress(progress->value() + 5);

	if (progStatus.restore_notch)
		selrig->set_notch(xcvrvfo.notch, xcvrvfo.notch_val);
	if (progStatus.restore_noise)
		selrig->set_noise(xcvrvfo.noise);
	update_progress(progress->value() + 5);

	if (progStatus.restore_nr) {
		selrig->set_noise_reduction(xcvrvfo.nr);
		selrig->set_noise_reduction_val(xcvrvfo.nr_val);
	}
	update_progress(progress->value() + 5);

	if (progStatus.restore_mic_gain)
		selrig->set_mic_gain(xcvrvfo.mic_gain);
	update_progress(progress->value() + 5);

	if (progStatus.restore_squelch)
		selrig->set_squelch(xcvrvfo.squelch);
	update_progress(progress->value() + 5);

	if (progStatus.restore_rf_gain)
		selrig->set_rf_gain(xcvrvfo.rf_gain);
	update_progress(progress->value() + 5);

	if (progStatus.restore_comp_on_off && progStatus.restore_comp_level)
		selrig->set_compression(xcvrvfo.compON, xcvrvfo.compression);
	else if (progStatus.restore_comp_on_off)
		selrig->set_compression(xcvrvfo.compON, progStatus.compression);
	else if (progStatus.restore_comp_level)
		selrig->set_compression(progStatus.compON, xcvrvfo.compression);
	update_progress(progress->value() + 5);

}

void restore_rig_vals()
{
	if (progStatus.start_stop_trace) ss_trace(true);

	update_progress(0);

	guard_lock serial_lock(&mutex_serial);
	trace(1, "restore_rig_vals()");

	if (!useB) {
		useB = true;
		selrig->selectB();
	}

	if (progStatus.restore_mode) {
		selrig->set_modeB(xcvr_vfoB.imode);
		selrig->set_FILT(xcvr_vfoB.filter);
	}

	if (progStatus.restore_frequency)
		selrig->set_vfoB(xcvr_vfoB.freq);

	if (progStatus.restore_bandwidth)
		selrig->set_bwB(xcvr_vfoB.iBW);

	restore_rig_vals_(xcvr_vfoB);

	trace(2, "Restored xcvr B:\n", print(xcvr_vfoB));

	useB = false;
	selrig->selectA();

	if (progStatus.restore_mode) {
		selrig->set_modeA(xcvr_vfoA.imode);
		selrig->set_FILT(xcvr_vfoA.filter);
	}

	if (progStatus.restore_frequency)
		selrig->set_vfoA(xcvr_vfoA.freq);

	if (progStatus.restore_bandwidth)
		selrig->set_bwA(xcvr_vfoA.iBW);

	restore_rig_vals_(xcvr_vfoA);

	trace(2, "Restored xcvr A:\n", print(xcvr_vfoA));

	if (progStatus.start_stop_trace) ss_trace(false);

}

void read_rig_vals_(XCVR_STATE &xcvrvfo)
{
	if (selrig->has_preamp_control)
		xcvrvfo.preamp = selrig->get_preamp();
	if (selrig->has_attenuator_control)
		xcvrvfo.attenuator = selrig->get_attenuator();

	if (selrig->has_preamp_control || selrig->has_attenuator_control)
		btnRestorePreAtt->activate();
	else
		btnRestorePreAtt->deactivate();

	if (selrig->has_auto_notch) {
		btnRestoreAutoNotch->activate();
		if (progStatus.restore_auto_notch && selrig->has_auto_notch)
			xcvrvfo.auto_notch = selrig->get_auto_notch();
	} else btnRestoreAutoNotch->deactivate();

	if (selrig->has_split) {
		btnRestoreSplit->activate();
		if (progStatus.restore_split && selrig->has_split)
			xcvrvfo.split = selrig->get_split();
	} else
		btnRestoreSplit->deactivate();

	update_progress(progress->value() + 4);

	if (selrig->has_power_control) {
		btnRestorePowerControl->activate();
		if (progStatus.restore_power_control)
			xcvrvfo.power_control = selrig->get_power_control();
	} else
		btnRestorePowerControl->deactivate();

	if (selrig->has_volume_control) {
		btnRestoreVolume->activate();
		if (progStatus.restore_volume)
			xcvrvfo.volume_control = selrig->get_volume_control();
	} else
		btnRestoreVolume->deactivate();

	if (selrig->has_ifshift_control) {
		btnRestoreIFshift->activate();
		if (progStatus.restore_if_shift)
			selrig->get_if_shift(xcvrvfo.if_shift);
	} else
		btnRestoreIFshift->deactivate();

	update_progress(progress->value() + 4);

	if (selrig->has_notch_control) {
		btnRestoreNotch->activate();
		if (progStatus.restore_notch)
			xcvrvfo.notch = selrig->get_notch(xcvrvfo.notch_val);
	} else
		btnRestoreNotch->deactivate();

	if (selrig->has_noise_control) {
		btnRestoreNoise->activate();
		if (progStatus.restore_noise)
			xcvrvfo.noise = selrig->get_noise();
	} else
		btnRestoreNoise->deactivate();


	update_progress(progress->value() + 4);

	if (selrig->has_noise_reduction_control) {
		btnRestoreNR->activate();
		if (progStatus.restore_nr) {
			xcvrvfo.nr = selrig->get_noise_reduction();
			xcvrvfo.nr_val = selrig->get_noise_reduction_val();
		}
	} else
		btnRestoreNR->deactivate();

	if (selrig->has_micgain_control) {
		btnRestoreMicGain->activate();
		if (progStatus.restore_mic_gain)
			xcvrvfo.mic_gain = selrig->get_mic_gain();
	} else
		btnRestoreMicGain->deactivate();

	if (selrig->has_sql_control) {
		btnRestoreSquelch->activate();
		if (progStatus.restore_squelch)
			xcvrvfo.squelch = selrig->get_squelch();
	} else
		btnRestoreSquelch->deactivate();


	update_progress(progress->value() + 4);

	if (selrig->has_rf_control) {
		btnRestoreRfGain->activate();
		if (progStatus.restore_rf_gain)
			xcvrvfo.rf_gain = selrig->get_rf_gain();
	} else
		btnRestoreRfGain->deactivate();

	if (selrig->has_compression || selrig->has_compON) {

		selrig->get_compression( xcvrvfo.compON, xcvrvfo.compression );

		if (selrig->has_compON)
			btnRestoreCompOnOff->activate();
		else
			btnRestoreCompOnOff->deactivate();

		if (selrig->has_compression)
			btnRestoreCompLevel->activate();
		else
			btnRestoreCompLevel->deactivate();

	} else {
		btnRestoreCompOnOff->deactivate();
		btnRestoreCompLevel->deactivate();
	}

	update_progress(progress->value() + 4);

}

void read_vfoA_vals()
{
trace(1, "read_vfoA_vals()");
	update_progress(progress->value() + 4);

	if (selrig->has_get_info)
		selrig->get_info();

trace(1, "read vfoA()");
	xcvr_vfoA.freq = selrig->get_vfoA();
	update_progress(progress->value() + 4);

trace(1, "read modeA()");
	xcvr_vfoA.imode = selrig->get_modeA();
	update_progress(progress->value() + 4);

trace(1, "get bwA()");
	xcvr_vfoA.iBW = selrig->get_bwA();
	update_progress(progress->value() + 4);

trace(1, "get FILT(A)");
	xcvr_vfoA.filter = selrig->get_FILT(xcvr_vfoA.imode);

trace(1, "read_rig_vals()");
	read_rig_vals_(xcvr_vfoA);

	trace(2, "Read xcvr A:\n", print(xcvr_vfoA));
}

void read_vfoB_vals()
{
trace(1, "read_vfoB_vals()");
	update_progress(progress->value() + 4);

	if (selrig->has_get_info)
		selrig->get_info();

trace(1, "read vfoB()");
	xcvr_vfoB.freq = selrig->get_vfoB();
	update_progress(progress->value() + 4);

trace(1, "read modeB()");
	xcvr_vfoB.imode = selrig->get_modeB();
	update_progress(progress->value() + 4);

trace(1, "get bwB()");
	xcvr_vfoB.iBW = selrig->get_bwB();
	update_progress(progress->value() + 4);

trace(1, "get FILT(B)");
	xcvr_vfoB.filter = selrig->get_FILT(xcvr_vfoB.imode);

trace(1, "read_rig_vals()");
	read_rig_vals_(xcvr_vfoB);

	trace(2, "Read xcvr B:\n", print(xcvr_vfoB));
}

void read_rig_vals()
{
	if (progStatus.start_stop_trace) ss_trace(true);

	if (selrig->has_mode_control)
		btnRestoreMode->activate();
	else
		btnRestoreMode->deactivate();

	if (selrig->has_bandwidth_control)
		btnRestoreBandwidth->activate();
	else
		btnRestoreBandwidth->deactivate();

// no guard_lock ... this function called from within a guard_lock block
	trace(1, "read_rig_vals()");
	update_progress(0);
	
	if (selrig->name_ == rig_FT891.name_) {
		// The FT-891 loses width WDH on A/B changes.  It also starts 
		// with VFOA active, so no selectA() before reading VFOA values.
		useB = false;
		read_vfoA_vals();
		useB = true;
		selrig->selectB();		// first select call
		read_vfoB_vals();

		// Restore VFOA mode, then freq and bandwidth
		useB = false;
		selrig->selectA();		// second select call
		yaesu891UpdateA(&xcvr_vfoA);
	} else {
		useB = true;
		selrig->selectB();		// first select call to FT897D
		read_vfoB_vals();
		useB = false;
		selrig->selectA();		// second select call
		read_vfoA_vals();
	}
//std::cout << "xcvr_vfoA.freq " << xcvr_vfoA.freq << std::endl;
//std::cout << "xcvr_vfoB.freq " << xcvr_vfoB.freq << std::endl;
	if (selrig->has_agc_control) {
		progStatus.agc_level = selrig->get_agc();
		redrawAGC();
	}

	if (selrig->has_FILTER)
		selrig->set_FILTERS(progStatus.filters);

	selrig->set_BANDWIDTHS(progStatus.bandwidths);

	if (progStatus.start_stop_trace) ss_trace(false);
}

void close_UI()
{
	{
		guard_lock serial_lock(&mutex_serial);
		trace(1, "close_UI()");
		run_serial_thread = false;
	}
	pthread_join(*serial_thread, NULL);

// xcvr auto off
//	if (selrig->has_xcvr_auto_on_off && progStatus.xcvr_auto_off)
//		selrig->set_xcvr_auto_off();

	// close down the serial port
	RigSerial->ClosePort();

	if (dlgDisplayConfig && dlgDisplayConfig->visible())
		dlgDisplayConfig->hide();
	if (dlgXcvrConfig && dlgXcvrConfig->visible())
		dlgXcvrConfig->hide();
	if (dlgMemoryDialog && dlgMemoryDialog->visible())
		dlgMemoryDialog->hide();

	debug::stop();

	mainwindow->hide();
}

void closeRig()
{
	trace(1, "closeRig()");
	if (xcvr_initialized) {
		restore_rig_vals();
		selrig->shutdown();
	// xcvr auto off
		if (selrig->has_xcvr_auto_on_off && progStatus.xcvr_auto_off)
			selrig->set_xcvr_auto_off();
	}
	close_rig = false;
}

void cbExit()
{
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

	{
		guard_lock serial_lock(&mutex_serial);
		trace(1, "shutdown serial thread");
		close_rig = true;
	}

	close_UI();

	if (tracewindow) tracewindow->hide();
	if (tabs_dialog) tabs_dialog->hide();
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

void update_UI_PTT(void *d)
{
	btnPTT->value(PTT);
	if (!PTT) {
		btnALC_SWR->hide();
		scaleSmeter->show();
		sldrRcvSignal->clear();
	} else {
		btnALC_SWR->show();
		scaleSmeter->hide();
		sldrFwdPwr->clear();
		sldrALC->clear();
		sldrSWR->clear();
	}
}

void adjust_small_ui()
{
	int y = 0;

	mainwindow->resize( mainwindow->x(), mainwindow->y(), SMALL_MAINW, SMALL_MAINH);

	btnVol->hide();
	sldrVOLUME->hide();
	sldrRFGAIN->hide();
	btnIFsh->hide();
	sldrIFSHIFT->hide();
	btnLOCK->hide();
	btnCLRPBT->hide();
	sldrINNER->hide();
	btnCLRPBT->hide();
	sldrOUTER->hide();
	btnNotch->hide();
	sldrNOTCH->hide();
	sldrMICGAIN->hide();
	btnPOWER->hide();
	sldrPOWER->hide();
	btnPOWER->hide();
	sldrSQUELCH->hide();
	btnNR->hide();
	sldrNR->hide();
	btnNOISE->hide();
	btnAGC->hide();
	sldrRFGAIN->redraw_label();

	if (progStatus.schema == 1 && selrig->widgets[0].W != NULL) {
		int i = 0;
		while (selrig->widgets[i].W != NULL) {
			selrig->widgets[i].W->resize(
				selrig->widgets[i].x, selrig->widgets[i].y,
				selrig->widgets[i].w, selrig->widgets[i].W->h() );
			selrig->widgets[i].W->show();
			selrig->widgets[i].W->redraw();
			if (selrig->widgets[i].y > y) y = selrig->widgets[i].y;
			i++;
		}
		if (selrig->has_data_port) {
			sldrMICGAIN->label("");
			sldrMICGAIN->redraw_label();
		}
		if (selrig->has_power_control) {
			btnPOWER->resize( sldrPOWER->x() - 52, sldrPOWER->y(), 50, 18 );
			btnPOWER->redraw();
			btnPOWER->show();
		}
		if (mnuSchema) mnuSchema->set();
	} else {
		if (mnuSchema) mnuSchema->clear();
		y = cntRIT->y() + 2;
		if (selrig->has_volume_control) {
			y += 20;
			btnVol->position( 2, y);
			btnVol->show();
			btnVol->redraw();
			sldrVOLUME->resize( 54, y, 368, 18 );
			sldrVOLUME->show();
			sldrVOLUME->redraw();
		}
		if (selrig->has_rf_control) {
			y += 20;
			sldrRFGAIN->resize( 54, y, 368, 18 );
			sldrRFGAIN->show();
			sldrRFGAIN->redraw();
		}
		if (selrig->has_sql_control) {
			y += 20;
			sldrSQUELCH->resize( 54, y, 368, 18 );
			sldrSQUELCH->show();
			sldrSQUELCH->redraw();
		}
		if (selrig->has_noise_reduction_control) {
			y += 20;
			btnNR->position( 2, y);
			btnNR->show();
			btnNR->redraw();
			sldrNR->resize( 54, y, 368, 18 );
			sldrNR->show();
			sldrNR->redraw();
			if (xcvr_name == rig_TT599.name_) btnNR->deactivate();
		}
		if (selrig->has_pbt_controls) {
			y += 20;
			btnLOCK->position( 2, y);
			btnLOCK->show();
			btnLOCK->redraw();
			sldrINNER->resize( 54, y, 368, 18 );
			sldrINNER->show();
			sldrINNER->redraw();
			y += 20;
			btnCLRPBT->position( 2, y);
			btnCLRPBT->show();
			btnCLRPBT->redraw();
			sldrOUTER->resize( 54, y, 368, 18);
			sldrOUTER->show();
			sldrOUTER->redraw();
		}
		if (selrig->has_ifshift_control) {
			y += 20;
			btnIFsh->position( 2, y);
			btnIFsh->show();
			btnIFsh->redraw();
			sldrIFSHIFT->resize( 54, y, 368, 18 );
			sldrIFSHIFT->show();
			sldrIFSHIFT->redraw();
		}
		if (selrig->has_notch_control) {
			y += 20;
			btnNotch->position( 2, y);
			btnNotch->show();
			btnNotch->redraw();
			sldrNOTCH->resize( 54, y, 368, 18 );
			sldrNOTCH->show();
			sldrNOTCH->redraw();
		}
		if (selrig->has_micgain_control || selrig->has_data_port) {
			if (selrig->has_micgain_control) {
				y += 20;
				sldrMICGAIN->resize( 54, y, 368, 18 );
				sldrMICGAIN->show();
				sldrMICGAIN->redraw();
				if (selrig->has_data_port) {
					sldrMICGAIN->label("");
					sldrMICGAIN->redraw_label();
					btnDataPort->position( 2, y);
					btnDataPort->show();
					btnDataPort->redraw();
				}
			} else if (selrig->has_data_port) {
				btnDataPort->position( 214, 105);
				btnDataPort->show();
				btnDataPort->redraw();
			}
		}
		if (selrig->has_power_control) {
			y += 20;
			sldrPOWER->resize( 54, y, 368, 18 );
			sldrPOWER->show();
			sldrPOWER->redraw();
			btnPOWER->resize( 2, y, 50, 18 );
			btnPOWER->show();
		}
	}
	y += 20;
	btn_show_controls->position( btn_show_controls->x(), y );
	btnAttenuator->position( btnAttenuator->x(), y);
	btnAttenuator->redraw();
	btnPreamp->position( btnPreamp->x(), y);
	btnPreamp->redraw();
	btnNOISE->position( btnNOISE->x(), y);
	btnNOISE->show();
	btnNOISE->redraw();
	btnAutoNotch->position( btnAutoNotch->x(), y);
	btnAutoNotch->redraw();
	btnTune->position( btnTune->x(), y);
	btnTune->redraw();
	btn_tune_on_off->position( btn_tune_on_off->x(), y);
	btn_tune_on_off->redraw();

	if (selrig->has_agc_control) {
		btnAGC->show();
		sldrRFGAIN->label("");
		sldrRFGAIN->redraw_label();
	} else {
		btnAGC->hide();
		sldrRFGAIN->label(_("RF"));
		sldrRFGAIN->redraw_label();
	}

	if (xcvr_name == rig_FT1000MP.name_) {
		y -= 20;
		btnTune->position( btnTune->x(), y);
		btnTune->redraw();
		btn_tune_on_off->position( btn_tune_on_off->x(), y);
		btn_tune_on_off->redraw();
		btnAutoNotch->position( btnAutoNotch->x(), y);
		btnAutoNotch->redraw();
		btnPTT->position( btnPTT->x(), y);
		btnPTT->redraw();
	}

	if (xcvr_name == rig_FT100D.name_ ||
		xcvr_name == rig_FT767.name_  ||
		xcvr_name == rig_FT817.name_  ||
		xcvr_name == rig_FT847.name_  ||
		xcvr_name == rig_FT857D.name_ ||
		xcvr_name == rig_FT890.name_  ||
		xcvr_name == rig_FT897D.name_ ||
		xcvr_name == rig_FT920.name_ ) {
		y -= 20;
		btnPTT->position( mainwindow->w() - btnPTT->w() - btn_show_controls->w() - 10, y);
		btnPTT->redraw();
		btn_show_controls->position( btnPTT->x() + btnPTT->w() + 5, y );
		btn_show_controls->redraw();
	}

	int use_AuxPort = (progStatus.aux_serial_port != "NONE");
	if (use_AuxPort) {
		btnPTT->resize(btnPTT->x(), y, btnPTT->w(), 38);
		btnPTT->redraw();
		y += 20;
		boxControl->position(boxControl->x(), y);
		btnAuxRTS->position(btnAuxRTS->x(), y);
		btnAuxDTR->position(btnAuxDTR->x(), y);
		btnAuxRTS->value(progStatus.aux_rts);
		btnAuxDTR->value(progStatus.aux_dtr);
		boxControl->show();
		btnAuxRTS->show();
		btnAuxDTR->show();
	} else {
		boxControl->hide();
		btnAuxRTS->hide();
		btnAuxDTR->hide();
		btnPTT->resize(btnPTT->x(), y, btnPTT->w(), 18);
		btnPTT->redraw();
	}

	btn_show_controls->label("@-22->");
	btn_show_controls->redraw_label();

	y += 20;

	btn_show_controls->show();
	mainwindow->init_sizes();
	mainwindow->size( mainwindow->w(), y);

	if (progStatus.tooltips) {
		Fl_Tooltip::enable(1);
		if (mnuTooltips) mnuTooltips->set();
	} else {
		if (mnuTooltips) mnuTooltips->clear();
		Fl_Tooltip::enable(0);
	}

	mainwindow->redraw();

	if (tabs_dialog && tabs_dialog->visible())
		tabs_dialog->position(mainwindow->x(), mainwindow->y() + mainwindow->h() + 26);

}

void adjust_wide_ui()
{
	mainwindow->resize( mainwindow->x(), mainwindow->y(), mainwindow->w(), WIDE_MAINH);
	mainwindow->redraw();

	btnVol->show();
	sldrVOLUME->show();
	sldrRFGAIN->show();
	if (selrig->has_ifshift_control) {
		btnIFsh->show();
		sldrIFSHIFT->show();
	}
	if (selrig->has_pbt_controls) {
		btnLOCK->show();
		btnLOCK->value(progStatus.pbt_lock);
		btnCLRPBT->show();
		sldrINNER->show();
		sldrOUTER->show();
		sldrINNER->value(progStatus.pbt_inner);
		sldrOUTER->value(progStatus.pbt_outer);
	}
	btnNotch->show();
	sldrNOTCH->show();
	sldrMICGAIN->show();
	sldrPOWER->show();
	btnPOWER->hide();
	sldrSQUELCH->show();
	btnNR->show();
	sldrNR->show();
	btnAGC->hide();
	btnDataPort->hide();
	sldrRFGAIN->redraw_label();

	if (!selrig->has_micgain_control)
		sldrMICGAIN->deactivate();

	if (!selrig->has_noise_reduction)
		btnNR->deactivate();

	if (!selrig->has_noise_reduction_control)
		sldrNR->deactivate();

	if (xcvr_name == rig_TT550.name_) {
		tabs550->show();
		tabsGeneric->hide();
	} else {
		tabs550->hide();

		tabsGeneric->remove(genericAux);
		if (progStatus.aux_serial_port != "NONE" || selrig->has_data_port) {
			if (progStatus.aux_serial_port != "NONE") {
				btnAuxRTS->activate();
				btnAuxDTR->activate();
			} else {
				btnAuxRTS->deactivate();
				btnAuxDTR->deactivate();
			}
			if (selrig->has_data_port)
				btnDataPort->show();
			else
				btnDataPort->hide();
			tabsGeneric->add(genericAux);
		}
		tabsGeneric->remove(genericRXB);
		if (selrig->has_rit || selrig->has_xit || selrig->has_bfo)
			tabsGeneric->add(genericRXB);
		tabsGeneric->show();
		if (selrig->has_agc_control) {
			btnAGC->show();
			sldrRFGAIN->label("");
			sldrRFGAIN->redraw_label();
		} else {
			btnAGC->hide();
			sldrRFGAIN->label(_("RF"));
			sldrRFGAIN->redraw_label();
		}
		if (selrig->has_power_control) {
			btnPOWER->resize(sldrPOWER->x() - 52, sldrPOWER->y(), 50, 18);
			btnPOWER->show();
		}
		else {
			sldrPOWER->deactivate();
		}
	}

	if (progStatus.tooltips) {
		Fl_Tooltip::enable(1);
		if (mnuTooltips) mnuTooltips->set();
	} else {
		if (mnuTooltips) mnuTooltips->clear();
		Fl_Tooltip::enable(0);
	}

	mainwindow->redraw();
}

void adjust_touch_ui()
{
	mainwindow->resize( mainwindow->x(), mainwindow->y(), mainwindow->w(), TOUCH_MAINH);
	mainwindow->redraw();

	if (spnrPOWER) spnrPOWER->show();
	if (sldrPOWER) sldrPOWER->show();
	btnVol->show();
	if (spnrVOLUME) spnrVOLUME->show();
	if (sldrVOLUME) sldrVOLUME->show();

	if (spnrRFGAIN) spnrRFGAIN->show();
	if (sldrRFGAIN) sldrRFGAIN->show();

	btnIFsh->show();
	if (spnrIFSHIFT) spnrIFSHIFT->show();
	if (sldrIFSHIFT) sldrIFSHIFT->show();

	btnNotch->show();
	if (spnrNOTCH) spnrNOTCH->show();
	if (sldrNOTCH) sldrNOTCH->show();

	if (spnrMICGAIN) spnrMICGAIN->show();
	if (sldrMICGAIN) sldrMICGAIN->show();

	if (spnrSQUELCH) spnrSQUELCH->show();
	if (sldrSQUELCH) sldrSQUELCH->show();

	if (selrig->has_agc_control) {
		btnAGC->show();
		sldrRFGAIN->label("");
		sldrRFGAIN->redraw_label();
	} else {
		btnAGC->hide();
		sldrRFGAIN->label(_("RF"));
		sldrRFGAIN->redraw_label();
	}

	btnNR->show();
	if (spnrNR) spnrNR->show();
	if (sldrNR) sldrNR->show();

	if (xcvr_name == rig_TT550.name_) {
		tabs550->show();
		tabsGeneric->hide();
	} else {
		tabs550->hide();
		tabsGeneric->remove(genericAux);
		if (progStatus.aux_serial_port != "NONE" || selrig->has_data_port) {
//std::cout << "has data port\n";
			if (progStatus.aux_serial_port != "NONE") {
				btnAuxRTS->activate();
				btnAuxDTR->activate();
			} else {
				btnAuxRTS->deactivate();
				btnAuxDTR->deactivate();
			}
			if (selrig->has_data_port)
				btnDataPort->activate();
			else
				btnDataPort->deactivate();
			tabsGeneric->add(genericAux);
		}
		tabsGeneric->remove(genericRXB);
		if (selrig->has_rit || selrig->has_xit || selrig->has_bfo)
			tabsGeneric->add(genericRXB);
		tabsGeneric->show();
	}

	if (progStatus.tooltips) {
		Fl_Tooltip::enable(1);
		if (mnuTooltips) mnuTooltips->set();
	} else {
		if (mnuTooltips) mnuTooltips->clear();
		Fl_Tooltip::enable(0);
	}

	mainwindow->init_sizes();
	mainwindow->size_range(WIDE_MAINW, WIDE_MAINH, 0, WIDE_MAINH);
	mainwindow->redraw();
}

void adjust_control_positions()
{
	switch (progStatus.UIsize) {
		case small_ui :
			adjust_small_ui();
			break;
		case wide_ui :
			adjust_wide_ui();
			break;
		case touch_ui :
		default :
			adjust_touch_ui();
			break;
	}
	FreqDispA->set_hrd(progStatus.hrd_buttons);
	FreqDispB->set_hrd(progStatus.hrd_buttons);
	if (selrig->name_ == rig_FT891.name_) {
		// Default FT891 to only send slider updates to rig once slider 
		// is released. This avoids a condition where once slider is 
		// released, the slider value no longer tracks changes from 
		// controls on the rig.
		progStatus.sliders_button = FL_WHEN_RELEASE;
		chk_sliders_button->value(false);
	}
	set_sliders_when();
}

void initTabs()
{
	if (xcvr_name == rig_TT550.name_) {
		spnr_tt550_line_out->value(progStatus.tt550_line_out);
		cbo_tt550_agc_level->index(progStatus.tt550_agc_level);
		spnr_tt550_cw_wpm->value(progStatus.tt550_cw_wpm);
		spnr_tt550_cw_vol->value(progStatus.tt550_cw_vol);
		spnr_tt550_cw_spot->value(progStatus.tt550_cw_spot);
		spnr_tt550_cw_weight->value(progStatus.tt550_cw_weight);
		spnr_tt550_cw_qsk->value(progStatus.tt550_cw_qsk);
		btn_tt550_enable_keyer->value(progStatus.tt550_enable_keyer);
		btn_tt550_vox->value(progStatus.tt550_vox_onoff);
		spnr_tt550_vox_gain->value(progStatus.tt550_vox_gain);
		spnr_tt550_anti_vox->value(progStatus.tt550_vox_anti);
		spnr_tt550_vox_hang->value(progStatus.tt550_vox_hang);
		btn_tt550_CompON->value(progStatus.tt550_compON);
		spnr_tt550_compression->value(progStatus.tt550_compression);
		spnr_tt550_mon_vol->value(progStatus.tt550_mon_vol);
		btn_tt550_enable_xmtr->value(progStatus.tt550_enable_xmtr);
		btn_tt550_enable_tloop->value(progStatus.tt550_enable_tloop);
		btn_tt550_tuner_bypass->value(progStatus.tt550_tuner_bypass);
		btn_tt550_use_xmt_bw->value(progStatus.tt550_use_xmt_bw);
		sel_tt550_encoder_step->value(progStatus.tt550_encoder_step);
		spnr_tt550_encoder_sensitivity->value(progStatus.tt550_encoder_sensitivity);
		sel_tt550_F1_func->value(progStatus.tt550_F1_func);
		sel_tt550_F2_func->value(progStatus.tt550_F2_func);
		sel_tt550_F3_func->value(progStatus.tt550_F3_func);
		progStatus.use_rig_data = false;
		op_tt550_XmtBW->clear();
		for (int i = 0; TT550_xmt_widths[i] != NULL; i++) {
			op_tt550_XmtBW->add(TT550_xmt_widths[i]);
		}
		op_tt550_XmtBW->activate();
		op_tt550_XmtBW->index(progStatus.tt550_xmt_bw);

		poll_smeter->activate(); poll_smeter->value(progStatus.poll_smeter);
		poll_pout->activate(); poll_pout->value(progStatus.poll_pout);
		poll_swr->activate(); poll_swr->value(progStatus.poll_swr);
		poll_alc->activate(); poll_alc->value(progStatus.poll_alc);
		poll_frequency->deactivate(); poll_frequency->value(0);
		poll_mode->deactivate(); poll_mode->value(0);
		poll_bandwidth->deactivate(); poll_bandwidth->value(0);
		poll_volume->deactivate(); poll_volume->value(0);
		poll_notch->deactivate(); poll_notch->value(0);
		poll_auto_notch->deactivate(); poll_auto_notch->value(0);
		poll_ifshift->deactivate(); poll_ifshift->value(0);
		poll_power_control->deactivate(); poll_power_control->value(0);
		poll_pre_att->deactivate(); poll_pre_att->value(0);
		poll_squelch->deactivate(); poll_squelch->value(0);
		poll_micgain->deactivate(); poll_micgain->value(0);
		poll_rfgain->deactivate(); poll_rfgain->value(0);
		poll_split->deactivate(); poll_split->value(0);
		poll_nr->deactivate(); poll_nr->value(0);
		poll_noise->deactivate(); poll_noise->value(0);
		poll_all->deactivate(); poll_all->value(0);

		if (progStatus.tt550_at11_inline) {
			tt550_AT_inline->value(1);
			tt550_AT_inline->label("Inline");
			tt550_AT_inline->redraw_label();
			selrig->at11_autotune();
		} else {
			tt550_AT_inline->value(0);
			tt550_AT_inline->label("Bypassed");
			tt550_AT_inline->redraw_label();
			selrig->at11_bypass();
		}
		if (progStatus.tt550_at11_hiZ) {
			selrig->at11_hiZ();
			tt550_AT_Z->value(1);
		} else{
			selrig->at11_loZ();
			tt550_AT_Z->value(0);
		}

	} else {

		hidden_tabs->add(tab_yaesu_bands);
		hidden_tabs->add(tab_FT8n_bands);
		hidden_tabs->add(tab_FT8n_CTCSS);
		hidden_tabs->add(tab_icom_bands);
		hidden_tabs->add(genericCW);
		hidden_tabs->add(genericQSK);
		hidden_tabs->add(genericVOX);
		hidden_tabs->add(genericSpeech);
		hidden_tabs->add(genericRx);
		hidden_tabs->add(genericMisc);
		hidden_tabs->add(genericUser_1);
		hidden_tabs->add(genericUser_2);
		hidden_tabs->add(tab7610);

		if (selrig->has_band_selection) {
			if (selrig->ICOMrig) {
				tabsGeneric->add(tab_icom_bands);
				tab_icom_bands->redraw();
			} else if (selrig->name_ == rig_FT857D.name_ || selrig->name_ == rig_FT897D.name_) {
				tabsGeneric->add(tab_FT8n_bands);
				tabsGeneric->add(tab_FT8n_CTCSS);
				tab_FT8n_bands->redraw();
				tab_FT8n_CTCSS->redraw();
			} else {
				tabsGeneric->add(tab_yaesu_bands);
				tab_yaesu_bands->redraw();
			}
		}

		if (selrig->has_cw_wpm ||
			selrig->has_cw_weight ||
			selrig->has_cw_keyer ||
			selrig->has_cw_spot ||
			selrig->has_cw_spot_tone ) {

			if (selrig->has_cw_wpm) {
				int min, max;
				selrig->get_cw_wpm_min_max(min, max);
				spnr_cw_wpm->minimum(min);
				spnr_cw_wpm->maximum(max);
				spnr_cw_wpm->value(progStatus.cw_wpm);
				spnr_cw_wpm->show();
			} else
				spnr_cw_wpm->hide();

			if (selrig->has_cw_weight) {
				double min, max, step;
				selrig->get_cw_weight_min_max_step( min, max, step );
				spnr_cw_weight->minimum(min);
				spnr_cw_weight->maximum(max);
				spnr_cw_weight->step(step);
				spnr_cw_weight->value(progStatus.cw_weight);
				spnr_cw_weight->show();
			} else
				spnr_cw_weight->hide();

			if (selrig->has_cw_keyer) {
				btn_enable_keyer->show();
				btn_enable_keyer->value(progStatus.enable_keyer);
				selrig->enable_keyer();
			}
			else
				btn_enable_keyer->hide();

			if (selrig->has_cw_spot) {
				btnSpot->value(progStatus.cw_spot);
				selrig->set_cw_spot();
				btnSpot->show();
			} else
				btnSpot->hide();

			if (selrig->has_cw_spot_tone) {
				spnr_cw_spot_tone->show();
				int min, max, step;
				selrig->get_cw_spot_tone_min_max_step(min, max, step);
				spnr_cw_spot_tone->minimum(min);
				spnr_cw_spot_tone->maximum(max);
				spnr_cw_spot_tone->step(step);
				spnr_cw_spot_tone->value(progStatus.cw_spot_tone);
				selrig->set_cw_spot_tone();
			} else
				spnr_cw_spot_tone->hide();

			tabsGeneric->add(genericCW);
			genericCW->redraw();
		}

		if (selrig->has_cw_qsk) {

			tabsGeneric->add(genericQSK);

			btnBreakIn->show();
			spnr_cw_delay->show();

			if (selrig->has_cw_qsk) {
				double min, max, step;
				selrig->get_cw_qsk_min_max_step(min, max, step);
				spnr_cw_qsk->minimum(min);
				spnr_cw_qsk->maximum(max);
				spnr_cw_qsk->step(step);
				spnr_cw_qsk->value(progStatus.cw_qsk);
				spnr_cw_qsk->show();
			} else
				spnr_cw_qsk->hide();
		}

		if (selrig->has_vox_onoff ||
			selrig->has_vox_gain ||
			selrig->has_vox_hang ||
			selrig->has_vox_on_dataport) {

			if (selrig->has_vox_onoff) {
				btn_vox->value(progStatus.vox_onoff);
				btn_vox->show();
				selrig->set_vox_onoff();
			} else btn_vox->hide();

			if (selrig->has_vox_gain) {
				int min, max, step;
				selrig->get_vox_gain_min_max_step(min, max, step);
				spnr_vox_gain->minimum(min);
				spnr_vox_gain->maximum(max);
				spnr_vox_gain->step(step);
				spnr_vox_gain->value(progStatus.vox_gain);
				spnr_vox_gain->show();
				selrig->set_vox_gain();
			} else spnr_vox_gain->hide();

			if (selrig->has_vox_anti) {
				int min, max, step;
				selrig->get_vox_anti_min_max_step(min, max, step);
				spnr_anti_vox->minimum(min);
				spnr_anti_vox->maximum(max);
				spnr_anti_vox->step(step);
				spnr_anti_vox->value(progStatus.vox_anti);
				spnr_anti_vox->show();
				selrig->set_vox_anti();
			} else spnr_anti_vox->hide();

			if (selrig->has_vox_hang) {
				int min, max, step;
				selrig->get_vox_hang_min_max_step(min, max, step);
				spnr_vox_hang->minimum(min);
				spnr_vox_hang->maximum(max);
				spnr_vox_hang->step(step);
				spnr_vox_hang->value(progStatus.vox_hang);
				spnr_vox_hang->show();
				selrig->set_vox_hang();
			} else spnr_vox_hang->hide();

			if (selrig->has_vox_on_dataport) {
				btn_vox_on_dataport->value(progStatus.vox_on_dataport);
				btn_vox_on_dataport->show();
				selrig->set_vox_on_dataport();
			} else btn_vox_on_dataport->hide();

			tabsGeneric->add(genericVOX);
			genericVOX->redraw();
		}

		if (selrig->has_compON ||
			selrig->has_compression ) {

			if (selrig->has_compON) {
				btnCompON->show();
				btnCompON->value(progStatus.compON);
			} else
				btnCompON->hide();

			if (selrig->has_compression) {
				int min, max, step;
				selrig->get_comp_min_max_step(min, max, step);
				spnr_compression->minimum(min);
				spnr_compression->maximum(max);
				spnr_compression->step(step);
				spnr_compression->show();
				spnr_compression->value(progStatus.compression);
				selrig->set_compression(progStatus.compON, progStatus.compression);
			} else
				spnr_compression->hide();

			tabsGeneric->add(genericSpeech);
			genericSpeech->redraw();
		}

		if (selrig->has_nb_level ||
			selrig->has_bpf_center ) {

			if (selrig->has_nb_level)
				sldr_nb_level->show();
			else
				sldr_nb_level->hide();

			if (selrig->has_bpf_center) {
				spnr_bpf_center->value(progStatus.bpf_center);
				spnr_bpf_center->show();
				btn_use_bpf_center->show();
			} else {
				spnr_bpf_center->hide();
				btn_use_bpf_center->hide();
			}
			tabsGeneric->add(genericRx);
			genericRx->redraw();
		}

		if (selrig->has_vfo_adj ||
			selrig->has_line_out ||
			selrig->has_xcvr_auto_on_off ) {

			if (selrig->has_vfo_adj) {
				int min, max, step;
				selrig->get_vfoadj_min_max_step(min, max, step);
				spnr_vfo_adj->minimum(min);
				spnr_vfo_adj->maximum(max);
				spnr_vfo_adj->step(step);
				progStatus.vfo_adj = selrig->getVfoAdj();
				spnr_vfo_adj->value(progStatus.vfo_adj);
				spnr_vfo_adj->show();
			} else
				spnr_vfo_adj->hide();

			if (selrig->has_line_out)
				spnr_line_out->show();
			else
				spnr_line_out->hide();

			if (selrig->has_xcvr_auto_on_off) {
				btn_xcvr_auto_on->value(progStatus.xcvr_auto_on);
				btn_xcvr_auto_off->value(progStatus.xcvr_auto_off);
				btn_xcvr_auto_on->show();
				btn_xcvr_auto_off->show();
			} else {
				btn_xcvr_auto_on->hide();
				btn_xcvr_auto_off->hide();
			}
			tabsGeneric->add(genericMisc);
			genericMisc->redraw();
		}

		tabsGeneric->add(genericUser_1);
		tabsGeneric->add(genericUser_2);
		genericUser_1->redraw();
		genericUser_2->redraw();

		if (selrig->name_ == rig_IC7610.name_) {
			tabsGeneric->add(tab7610);
			tab7610->redraw();
			btnAttenuator->hide();
		}

		tabsGeneric->redraw();

		poll_frequency->activate(); poll_frequency->value(progStatus.poll_frequency);
		poll_mode->activate(); poll_mode->value(progStatus.poll_mode);
		poll_bandwidth->activate(); poll_bandwidth->value(progStatus.poll_bandwidth);

		poll_smeter->activate(); poll_smeter->value(progStatus.poll_smeter);
		poll_pout->activate(); poll_pout->value(progStatus.poll_pout);
		poll_swr->activate(); poll_swr->value(progStatus.poll_swr);
		poll_alc->activate(); poll_alc->value(progStatus.poll_alc);
		poll_volume->activate(); poll_volume->value(progStatus.poll_volume);
		poll_notch->activate(); poll_notch->value(progStatus.poll_notch);
		poll_auto_notch->activate(); poll_auto_notch->value(progStatus.poll_auto_notch);
		poll_ifshift->activate(); poll_ifshift->value(progStatus.poll_ifshift);
		poll_power_control->activate(); poll_power_control->value(progStatus.poll_power_control);
		poll_pre_att->activate(); poll_pre_att->value(progStatus.poll_pre_att);
		poll_squelch->activate(); poll_squelch->value(progStatus.poll_squelch);
		poll_micgain->activate(); poll_micgain->value(progStatus.poll_micgain);
		poll_rfgain->activate(); poll_rfgain->value(progStatus.poll_rfgain);
		poll_split->activate(); poll_split->value(progStatus.poll_split);
		poll_noise->activate(); poll_noise->value(progStatus.poll_noise);
		poll_nr->activate(); poll_nr->value(progStatus.poll_nr);
		poll_compression->activate(); poll_compression->value(progStatus.poll_compression);

		if (!selrig->has_bandwidth_control) { poll_bandwidth->deactivate(); poll_bandwidth->value(0); }
		if (!selrig->has_smeter) { poll_smeter->deactivate(); poll_smeter->value(0); }
		if (!selrig->has_power_out) { poll_pout->deactivate(); poll_pout->value(0); }
		if (!selrig->has_swr_control) { poll_swr->deactivate(); poll_swr->value(0); }
		if (!selrig->has_alc_control) { poll_alc->deactivate(); poll_alc->value(0); }
		if (!selrig->has_volume_control) { poll_volume->deactivate(); poll_volume->value(0); }
		if (!selrig->has_notch_control) { poll_notch->deactivate(); poll_notch->value(0); }
		if (!selrig->has_auto_notch ||
			xcvr_name == rig_FT1000MP.name_ )
			{ poll_auto_notch->deactivate(); poll_auto_notch->value(0); }
		if (!selrig->has_ifshift_control &&
			!selrig->has_pbt_controls) { poll_ifshift->deactivate(); poll_ifshift->value(0); }
		if (selrig->has_pbt_controls) {
			poll_ifshift->label("pbt");
			poll_ifshift->redraw_label();
		}
		if (!selrig->has_power_control) { poll_power_control->deactivate(); poll_power_control->value(0); }
		if (!selrig->has_preamp_control && !selrig->has_attenuator_control)
			{ poll_pre_att->deactivate(); poll_pre_att->value(0); }
		if (!selrig->has_sql_control) { poll_squelch->deactivate(); poll_squelch->value(0); }
		if (!selrig->has_micgain_control) { poll_micgain->deactivate(); poll_micgain->value(0); }
		if (!selrig->has_rf_control) { poll_rfgain->deactivate(); poll_rfgain->value(0); }
		if (!selrig->has_split) { poll_split->deactivate(); poll_split->value(0); }
		if (!selrig->has_noise_control) {poll_noise->deactivate(); poll_noise->value(0);}
		if (!selrig->has_noise_reduction) {poll_nr->deactivate(); poll_nr->value(0);}
		if (!selrig->has_compression) { poll_compression->deactivate(); poll_compression->value(0); }

	}

}

void init_TT550()
{
	useB = false;
	selrig->selectA();

	vfoB.freq = progStatus.freq_B;
	vfoB.imode = progStatus.imode_B;
	vfoB.iBW = progStatus.iBW_B;
	FreqDispB->value(vfoB.freq);
	selrig->set_vfoB(vfoB.freq);
	selrig->set_modeB(vfoB.imode);
	selrig->set_bwB(vfoB.iBW);

	vfoA.freq = progStatus.freq_A;
	vfoA.imode = progStatus.imode_A;
	vfoA.iBW = progStatus.iBW_A;
	FreqDispA->value( vfoA.freq );
	selrig->set_vfoA(vfoA.freq);
	selrig->set_modeA(vfoA.imode);

	vfo = &vfoA;

	if (vfoA.iBW == -1) vfoA.iBW = selrig->def_bandwidth(vfoA.imode);
		selrig->set_bwA(vfoA.iBW);

	rigmodes_.clear();
	opMODE->clear();
	for (int i = 0; selrig->modes_[i] != NULL; i++) {
		rigmodes_.push_back(selrig->modes_[i]);
		opMODE->add(selrig->modes_[i]);
	}
	opMODE->activate();
	opMODE->index(vfoA.imode);

	rigbws_.clear();
	opBW->show();
	opBW->clear();
	old_bws = selrig->bandwidths_;
	for (int i = 0; selrig->bandwidths_[i] != NULL; i++) {
		rigbws_.push_back(selrig->bandwidths_[i]);
			opBW->add(selrig->bandwidths_[i]);
		}
	opBW->activate();
	opBW->index(vfoA.iBW);

	spnr_tt550_vfo_adj->value(progStatus.vfo_adj);

	btnPreamp->label("Spot");
	btnPreamp->value(progStatus.tt550_spot_onoff);
	switch (progStatus.UIsize) {
		case small_ui :
			btnPreamp->show();
			break;
		case wide_ui : case touch_ui : default :
			btnPreamp->activate();
	}
}

void init_generic_rig()
{
	if (progStatus.CIV > 0)
		selrig->adjustCIV(progStatus.CIV);

	if (selrig->has_getvfoAorB) {

		int ret = selrig->get_vfoAorB();
		int retry = 10;
		while (ret == -1 && retry--) {
			MilliSleep(50);
			ret = selrig->get_vfoAorB();
		}
		if (ret == -1) ret = 0;

		useB = ret;

		read_rig_vals();
		if (progStatus.use_rig_data) {
			vfoA = xcvr_vfoA;
			vfoB = xcvr_vfoB;
		}

		if (useB) {
			selrig->selectB();
			vfo = &vfoB;
		} else {
			vfo = &vfoA;
		}
	}
	else {
		read_rig_vals();
		if (progStatus.use_rig_data) {
			vfoA = xcvr_vfoA;
			vfoB = xcvr_vfoB;
		}
	}
	progStatus.compON = xcvr_vfoA.compON;
	progStatus.compression = xcvr_vfoA.compression;

	vfo = &vfoA;

	rigmodes_.clear();
	opMODE->clear();
	if (selrig->has_mode_control) {
		for (int i = 0; selrig->modes_[i] != NULL; i++) {
			rigmodes_.push_back(selrig->modes_[i]);
			opMODE->add(selrig->modes_[i]);
		}
		opMODE->activate();
		opMODE->index(progStatus.imode_A);
	} else {
		opMODE->add(" ");
		opMODE->index(0);
		opMODE->deactivate();
	}

	rigbws_.clear();
	opBW->show();
	opBW->clear();
	if (selrig->has_bandwidth_control) {
		old_bws = selrig->bandwidths_;
		for (int i = 0; selrig->bandwidths_[i] != NULL; i++) {
			rigbws_.push_back(selrig->bandwidths_[i]);
			opBW->add(selrig->bandwidths_[i]);
		}
		opBW->activate();
		if (progStatus.iBW_A == -1) progStatus.iBW_A = selrig->def_bandwidth(vfoA.imode);
		if (progStatus.iBW_B == -1) progStatus.iBW_B = selrig->def_bandwidth(vfoB.imode);
		opBW->index(progStatus.iBW_A);
	} else {
		opBW->add(" ");
		opBW->index(0);
		opBW->deactivate();
	}
}

void init_rit()
{
	if (selrig->has_rit) {
		int min, max, step;
		selrig->get_RIT_min_max_step(min, max, step);
		cntRIT->minimum(min);
		cntRIT->maximum(max);
		cntRIT->step(step);
		switch (progStatus.UIsize) {
			case small_ui :
				cntRIT->show();
				break;
			case wide_ui : case touch_ui : default :
				cntRIT->activate();
		}
		cntRIT->value(progStatus.rit_freq);
	} else {
		switch (progStatus.UIsize) {
			case small_ui :
				cntRIT->hide();
				break;
			case wide_ui: case touch_ui : default :
				cntRIT->deactivate();
		}
	}
}

void init_xit()
{
	if (selrig->has_xit) {
		int min, max, step;
		selrig->get_XIT_min_max_step(min, max, step);
		cntXIT->minimum(min);
		cntXIT->maximum(max);
		cntXIT->step(step);
		cntXIT->value(progStatus.xit_freq);
		switch (progStatus.UIsize) {
			case small_ui :
				cntXIT->show();
				break;
			case wide_ui : case touch_ui : default :
				cntXIT->activate();
			}
	} else {
		switch (progStatus.UIsize) {
			case small_ui :
				cntXIT->hide();
			case wide_ui : case touch_ui : default :
				cntXIT->deactivate();
		}
	}
}

void init_bfo()
{
	if (selrig->has_bfo) {
		int min, max, step;
		selrig->get_BFO_min_max_step(min, max, step);
		cntBFO->minimum(min);
		cntBFO->maximum(max);
		cntBFO->step(step);
		cntBFO->value(progStatus.bfo_freq);
		switch (progStatus.UIsize) {
			case small_ui :
				cntBFO->show();
				break;
			case wide_ui : case touch_ui : default :
				cntBFO->activate();
		}
	} else {
		switch (progStatus.UIsize) {
			case small_ui :
				cntBFO->hide();
				break;
			case wide_ui : case touch_ui : default :
				cntBFO->deactivate();
		}
	}
}

void init_dsp_controls()
{
	if (selrig->has_dsp_controls) {
		opDSP_lo->clear();
		opDSP_hi->clear();
		btnDSP->label(selrig->SL_label);
		btnDSP->redraw_label();
		for (int i = 0; selrig->dsp_SL[i] != NULL; i++)
			opDSP_lo->add(selrig->dsp_SL[i]);
		opDSP_lo->tooltip(selrig->SL_tooltip);
		for (int i = 0; selrig->dsp_SH[i] != NULL; i++)
			opDSP_hi->add(selrig->dsp_SH[i]);
		opDSP_hi->tooltip(selrig->SH_tooltip);
		if (vfo->iBW > 256) {
			opDSP_lo->index(vfo->iBW & 0xFF);
			opDSP_hi->index((vfo->iBW >> 8) & 0x7F);
			btnDSP->show();
			opDSP_hi->show();
			opDSP_lo->hide();
			opBW->hide();
			opBW->index(0);
			btnFILT->hide();
		} else {
			opDSP_lo->index(0);
			opDSP_hi->index(0);
			btnDSP->hide();
			opDSP_lo->hide();
			opDSP_hi->hide();
			btnFILT->hide();
			opBW->show();
		}
	} else if (selrig->has_FILTER) {
		btnDSP->hide();
		opDSP_lo->hide();
		opDSP_hi->hide();
		btnFILT->show();
		opBW->resize(opDSP_lo->x(), opDSP_lo->y(), opDSP_lo->w(), opDSP_lo->h());
		opBW->redraw();
		opBW->show();
	} else {
		btnDSP->hide();
		opDSP_lo->hide();
		opDSP_hi->hide();
		btnFILT->hide();
		opBW->show();
	}
}

void init_volume_control()
{
	if (selrig->has_volume_control) {
		int min, max, step;
		selrig->get_vol_min_max_step(min, max, step);
		if (spnrVOLUME) {
			spnrVOLUME->minimum(min);
			spnrVOLUME->maximum(max);
			spnrVOLUME->step(step);
			spnrVOLUME->redraw();
			spnrVOLUME->activate();
		}
		if (sldrVOLUME) {
			sldrVOLUME->minimum(min);
			sldrVOLUME->maximum(max);
			sldrVOLUME->step(step);
			sldrVOLUME->redraw();
			sldrVOLUME->activate();
		}
		switch (progStatus.UIsize) {
			case small_ui :
				btnVol->show();
				if (sldrVOLUME) sldrVOLUME->show();
				if (spnrVOLUME) spnrVOLUME->show();
				break;
			case wide_ui : case touch_ui : default :
				btnVol->activate();
				if (sldrVOLUME) sldrVOLUME->activate();
				if (spnrVOLUME) spnrVOLUME->activate();
		}
	} else {
		switch (progStatus.UIsize) {
			case small_ui :
				btnVol->hide();
				if (sldrVOLUME) sldrVOLUME->hide();
				if (spnrVOLUME) spnrVOLUME->hide();
				break;
			case wide_ui : case touch_ui : default :
				btnVol->deactivate();
				if (sldrVOLUME) sldrVOLUME->deactivate();
				if (spnrVOLUME) spnrVOLUME->deactivate();
		}
	}
}

void set_init_volume_control()
{
	if (!selrig->has_volume_control)
		return;

	if (progStatus.use_rig_data) {
		progStatus.volume = selrig->get_volume_control();
		if (sldrVOLUME) sldrVOLUME->value(progStatus.volume);
		if (sldrVOLUME) sldrVOLUME->activate();
		btnVol->value(1);
		if (spnrVOLUME) spnrVOLUME->value(progStatus.volume);
		if (spnrVOLUME) spnrVOLUME->activate();
		sldrVOLUME->activate();
	} else {
		if (sldrVOLUME) sldrVOLUME->value(progStatus.volume);
		if (spnrVOLUME) spnrVOLUME->value(progStatus.volume);
		if (progStatus.spkr_on == 0) {
			btnVol->value(0);
			if (sldrVOLUME) sldrVOLUME->deactivate();
			if (spnrVOLUME) spnrVOLUME->deactivate();
			selrig->set_volume_control(0);
		} else {
			btnVol->value(1);
			if (sldrVOLUME) sldrVOLUME->activate();
			if (spnrVOLUME) spnrVOLUME->activate();
			selrig->set_volume_control(progStatus.volume);
		}
	}
}

void init_rf_control()
{
	if (selrig->has_rf_control) {
		int min, max, step;
		selrig->get_rf_min_max_step(min, max, step);
		if (sldrRFGAIN) sldrRFGAIN->minimum(min);
		if (sldrRFGAIN) sldrRFGAIN->maximum(max);
		if (sldrRFGAIN) sldrRFGAIN->step(step);
		if (sldrRFGAIN) sldrRFGAIN->redraw();
		if (spnrRFGAIN) spnrRFGAIN->minimum(min);
		if (spnrRFGAIN) spnrRFGAIN->maximum(max);
		if (spnrRFGAIN) spnrRFGAIN->step(step);
		if (spnrRFGAIN) spnrRFGAIN->redraw();

		switch (progStatus.UIsize) {
			case small_ui :
				if (sldrRFGAIN) sldrRFGAIN->show();
				if (spnrRFGAIN) spnrRFGAIN->show();
				break;
			case wide_ui : case touch_ui : default :
				if (sldrRFGAIN) sldrRFGAIN->activate();
				if (spnrRFGAIN) spnrRFGAIN->activate();
		}
	} else {
		switch (progStatus.UIsize) {
			case small_ui :
				if (sldrRFGAIN) sldrRFGAIN->hide();
				if (spnrRFGAIN) spnrRFGAIN->hide();
				break;
			case wide_ui : case touch_ui : default :
				if (sldrRFGAIN) sldrRFGAIN->deactivate();
				if (spnrRFGAIN) spnrRFGAIN->deactivate();
		}
	}
	if (selrig->name_ == rig_ICF8101.name_) {
		if (sldrRFGAIN) sldrRFGAIN->deactivate();
		if (spnrRFGAIN) spnrRFGAIN->deactivate();
	}
}

void set_init_rf_gain()
{
	if (!selrig->has_rf_control)
		return;

	if (progStatus.use_rig_data) {
		progStatus.rfgain = selrig->get_rf_gain();
		if (sldrRFGAIN) sldrRFGAIN->value(progStatus.rfgain);
		if (spnrRFGAIN) spnrRFGAIN->value(progStatus.rfgain);
	} else {
		if (sldrRFGAIN) sldrRFGAIN->value(progStatus.rfgain);
		if (spnrRFGAIN) spnrRFGAIN->value(progStatus.rfgain);
		selrig->set_rf_gain(progStatus.rfgain);
	}
}

void init_sql_control()
{
	if (selrig->has_sql_control) {
		int min, max, step;
		selrig->get_squelch_min_max_step(min, max, step);
		if (sldrSQUELCH) sldrSQUELCH->minimum(min);
		if (sldrSQUELCH) sldrSQUELCH->maximum(max);
		if (sldrSQUELCH) sldrSQUELCH->step(step);
		if (sldrSQUELCH) sldrSQUELCH->redraw();
		if (spnrSQUELCH) spnrSQUELCH->minimum(min);
		if (spnrSQUELCH) spnrSQUELCH->maximum(max);
		if (spnrSQUELCH) spnrSQUELCH->step(step);
		if (spnrSQUELCH) spnrSQUELCH->redraw();
		switch (progStatus.UIsize) {
			case small_ui :
				if (sldrSQUELCH) sldrSQUELCH->show();
				if (spnrSQUELCH) spnrSQUELCH->show();
				break;
			case wide_ui : case touch_ui : default:
				if (sldrSQUELCH) sldrSQUELCH->activate();
				if (spnrSQUELCH) spnrSQUELCH->activate();
		}

	} else {
		switch (progStatus.UIsize) {
			case small_ui :
				if (sldrSQUELCH) sldrSQUELCH->hide();
				if (spnrSQUELCH) spnrSQUELCH->hide();
				break;
			case wide_ui : case touch_ui : default :
				if (sldrSQUELCH) sldrSQUELCH->deactivate();
				if (spnrSQUELCH) spnrSQUELCH->deactivate();
		}
	}
}

void set_init_sql_control()
{
	if (!selrig->has_sql_control)
		return;

	if (progStatus.use_rig_data) {
		progStatus.squelch = selrig->get_squelch();
		if (sldrSQUELCH) sldrSQUELCH->value(progStatus.squelch);
		if (spnrSQUELCH) spnrSQUELCH->value(progStatus.squelch);
	} else {
		if (sldrSQUELCH) sldrSQUELCH->value(progStatus.squelch);
		if (spnrSQUELCH) spnrSQUELCH->value(progStatus.squelch);
		selrig->set_squelch(progStatus.squelch);
	}
}

void set_init_noise_reduction_control()
{
	if (!selrig->has_noise_reduction_control)
		return;

	if (progStatus.use_rig_data) {
		progStatus.noise_reduction = selrig->get_noise_reduction();
		progStatus.noise_reduction_val = selrig->get_noise_reduction_val();
		btnNR->value(progStatus.noise_reduction);
		if (sldrNR) sldrNR->value(progStatus.noise_reduction_val);
		if (spnrNR) spnrNR->value(progStatus.noise_reduction_val);
		
		if (selrig->name_ == rig_FT891.name_) {
			// On the FT-891, the usual definitions of NB and NR buttons
			// as defined in FLRIG are reversed. Relabel them to match
			// what the user sees in the radio screens, and handle the
			// mapping to appropriate cat controls in the FT891.xx class.
			btnNR->label("NB");
			btnNR->tooltip(_("Noise Blanker On/Off"));
		}
		
	} else {
		btnNR->value(progStatus.noise_reduction);
		if (sldrNR) sldrNR->value(progStatus.noise_reduction_val);
		if (spnrNR) spnrNR->value(progStatus.noise_reduction_val);
		selrig->set_noise_reduction(progStatus.noise_reduction);
		selrig->set_noise_reduction_val(progStatus.noise_reduction_val);
	}
}

void init_noise_reduction_control()
{
	if (selrig->has_noise_reduction_control) {
		int min, max, step;
		selrig->get_nr_min_max_step(min, max, step);

		if (sldrNR) sldrNR->minimum(min);
		if (sldrNR) sldrNR->maximum(max);
		if (sldrNR) sldrNR->step(step);
		if (sldrNR) sldrNR->redraw();

		if (spnrNR) spnrNR->minimum(min);
		if (spnrNR) spnrNR->maximum(max);
		if (spnrNR) spnrNR->step(step);
		if (spnrNR) spnrNR->redraw();

		if (selrig->name_ == rig_FT891.name_) {
			// On the FT-891, the usual definitions of NB and NR buttons
			// as defined in FLRIG are reversed. Relabel them to match
			// what the user sees in the radio screens, and handle the
			// mapping to appropriate cat controls in the FT891.xx class.
			sldrNR->tooltip(_("Adjust noise blanker"));
		}
		
		switch (progStatus.UIsize) {
			case small_ui :
				btnNR->show();
				if (sldrNR) sldrNR->show();
				if (spnrNR) spnrNR->show();
				break;
			case wide_ui : case touch_ui : default:
				btnNR->show();
				if (sldrNR) sldrNR->show();//activate();
				if (spnrNR) spnrNR->show();//activate();
				break;
		}
	} else {
		switch (progStatus.UIsize) {
			case small_ui :
				btnNR->hide();
				if (sldrNR) sldrNR->hide();
				if (spnrNR) sldrNR->hide();
				break;
			case wide_ui : case touch_ui : default :
				btnNR->hide();//deactivate();
				if (sldrNR) sldrNR->hide();//deactivate();
				if (spnrNR) spnrNR->hide();//deactivate();
				break;
		}
	}
}

void set_init_if_shift_control()
{
	if (!selrig->has_ifshift_control)
		return;
	if (progStatus.use_rig_data) {
		progStatus.shift = selrig->get_if_shift(progStatus.shift_val);
		btnIFsh->value(progStatus.shift);
		if (sldrIFSHIFT) sldrIFSHIFT->value(progStatus.shift_val);
		if (spnrIFSHIFT) spnrIFSHIFT->value(progStatus.shift_val);
	} else {
		if (progStatus.shift) {
			btnIFsh->value(1);
			if (sldrIFSHIFT) sldrIFSHIFT->value(progStatus.shift_val);
			if (spnrIFSHIFT) spnrIFSHIFT->value(progStatus.shift_val);
			selrig->set_if_shift(progStatus.shift_val);
		} else {
			btnIFsh->value(0);
			if (sldrIFSHIFT) sldrIFSHIFT->value(selrig->if_shift_mid);
			if (spnrIFSHIFT) spnrIFSHIFT->value(selrig->if_shift_mid);
			selrig->set_if_shift(selrig->if_shift_mid);
		}
	}
}

void init_if_shift_control()
{
	if (btnLOCK) btnLOCK->hide();
	if (btnCLRPBT) btnCLRPBT->hide();
	if (sldrINNER) sldrINNER->hide();
	if (sldrOUTER) sldrOUTER->hide();
	if (btnIFsh) btnIFsh->hide();
	if (sldrIFSHIFT) sldrIFSHIFT->hide();
	if (spnrIFSHIFT) spnrIFSHIFT->hide();

	if (selrig->has_ifshift_control) {
		btnIFsh->show();
		sldrIFSHIFT->show();
		if (spnrIFSHIFT) spnrIFSHIFT->show();
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
		switch (progStatus.UIsize) {
			case small_ui :
				btnIFsh->show();
				if (sldrIFSHIFT) sldrIFSHIFT->show();
				if (spnrIFSHIFT) spnrIFSHIFT->show();
				break;
			case wide_ui : case touch_ui : default :
				btnIFsh->activate();
				if (sldrIFSHIFT) sldrIFSHIFT->activate();
				if (spnrIFSHIFT) spnrIFSHIFT->activate();
				break;
		}
	} else {
		switch (progStatus.UIsize) {
			case small_ui :
				btnIFsh->hide();
				if (sldrIFSHIFT) sldrIFSHIFT->hide();
				if (spnrIFSHIFT) spnrIFSHIFT->hide();
				break;
			case wide_ui : case touch_ui : default :
				btnIFsh->deactivate();
				if (sldrIFSHIFT) sldrIFSHIFT->deactivate();
				if (spnrIFSHIFT) spnrIFSHIFT->deactivate();
				break;
		}
	}
	if (selrig->has_pbt_controls) {
		btnLOCK->show();
		btnLOCK->value(progStatus.pbt_lock);
		btnCLRPBT->show();
		sldrINNER->show();
		sldrOUTER->show();
		sldrINNER->value(progStatus.pbt_inner);
		sldrOUTER->value(progStatus.pbt_outer);
	}

	if (xcvr_name == rig_TS870S.name_) {
		if (progStatus.imode_A == RIG_TS870S::tsCW ||
			progStatus.imode_A == RIG_TS870S::tsCWR) {
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

void init_notch_control()
{
	if (selrig->has_notch_control) {
		int min, max, step;
		selrig->get_notch_min_max_step(min, max, step);
		if (sldrNOTCH) sldrNOTCH->minimum(min);
		if (sldrNOTCH) sldrNOTCH->maximum(max);
		if (sldrNOTCH) sldrNOTCH->step(step);
		if (sldrNOTCH) sldrNOTCH->redraw();
		if (spnrNOTCH) spnrNOTCH->minimum(min);
		if (spnrNOTCH) spnrNOTCH->maximum(max);
		if (spnrNOTCH) spnrNOTCH->step(step);
		if (spnrNOTCH) spnrNOTCH->redraw();
		switch (progStatus.UIsize) {
			case small_ui :
				btnNotch->show();
				if (sldrNOTCH) sldrNOTCH->show();
				if (spnrNOTCH) spnrNOTCH->show();
				break;
			case wide_ui : case touch_ui : default :
				btnNotch->activate();
				if (sldrNOTCH) sldrNOTCH->activate();
				if (spnrNOTCH) spnrNOTCH->activate();
				break;
		}
	} else {
		switch (progStatus.UIsize) {
			case small_ui :
				btnNotch->hide();
				if (sldrNOTCH) sldrNOTCH->hide();
				if (spnrNOTCH) spnrNOTCH->hide();
				break;
			case wide_ui : case touch_ui : default :
				btnNotch->deactivate();
				if (sldrNOTCH) sldrNOTCH->deactivate();
				if (spnrNOTCH) spnrNOTCH->deactivate();
				break;
		}
	}
}

void set_init_notch_control()
{
	if (selrig->has_notch_control) {
		if (progStatus.use_rig_data) {
			progStatus.notch = selrig->get_notch(progStatus.notch_val);
			btnNotch->value(progStatus.notch);
			if (sldrNOTCH) sldrNOTCH->value(progStatus.notch_val);
			if (spnrNOTCH) spnrNOTCH->value(progStatus.notch_val);
		} else {
			btnNotch->value(progStatus.notch);
			if (sldrNOTCH) sldrNOTCH->value(progStatus.notch_val);
			if (spnrNOTCH) spnrNOTCH->value(progStatus.notch_val);
			selrig->set_notch(progStatus.notch, progStatus.notch_val);
		}
	}
}

void init_micgain_control()
{
	if (selrig->has_micgain_control || selrig->has_data_port) {
		if (selrig->has_micgain_control) {
			int min = 0, max = 0, step = 0;
			selrig->get_mic_min_max_step(min, max, step);
			if (sldrMICGAIN) sldrMICGAIN->minimum(min);
			if (sldrMICGAIN) sldrMICGAIN->maximum(max);
			if (sldrMICGAIN) sldrMICGAIN->step(step);
			if (spnrMICGAIN) spnrMICGAIN->minimum(min);
			if (spnrMICGAIN) spnrMICGAIN->maximum(max);
			if (spnrMICGAIN) spnrMICGAIN->step(step);

			switch (progStatus.UIsize) {
				case small_ui :
					if (sldrMICGAIN) sldrMICGAIN->show();
					if (spnrMICGAIN) spnrMICGAIN->show();
					break;
				case wide_ui : case touch_ui : default :
					if (sldrMICGAIN) sldrMICGAIN->activate();
					if (spnrMICGAIN) spnrMICGAIN->activate();
					break;
			}
		} else {
			if (sldrMICGAIN) sldrMICGAIN->deactivate();
			if (spnrMICGAIN) spnrMICGAIN->deactivate();
		}
	} else {
		switch (progStatus.UIsize) {
			case small_ui :
				if (sldrMICGAIN) sldrMICGAIN->hide();
				if (spnrMICGAIN) spnrMICGAIN->hide();
				break;
			case wide_ui : case touch_ui : default :
				if (sldrMICGAIN) sldrMICGAIN->deactivate();
				if (spnrMICGAIN) spnrMICGAIN->deactivate();
		}
	}
}

void set_init_micgain_control()
{
	int min, max, step;
	if (selrig->has_micgain_control) {
		if (progStatus.use_rig_data)
			progStatus.mic_gain = selrig->get_mic_gain();
		else
			selrig->set_mic_gain(progStatus.mic_gain);

		selrig->get_mic_min_max_step(min, max, step);
		if (sldrMICGAIN) {
			sldrMICGAIN->minimum(min);
			sldrMICGAIN->maximum(max);
			sldrMICGAIN->step(step);
			sldrMICGAIN->value(progStatus.mic_gain);
			sldrMICGAIN->activate();
		}
		if (spnrMICGAIN) {
			spnrMICGAIN->minimum(min);
			spnrMICGAIN->maximum(max);
			spnrMICGAIN->step(step);
			spnrMICGAIN->value(progStatus.mic_gain);
			spnrMICGAIN->activate();
		}
	} else {
		if (sldrMICGAIN) sldrMICGAIN->deactivate();
		if (spnrMICGAIN) sldrMICGAIN->deactivate();
	}
}

void init_power_control()
{
	double min, max, step;
	if (selrig->has_power_control) {
		if (progStatus.use_rig_data)
			progStatus.power_level = selrig->get_power_control();
		else
			selrig->set_power_control(progStatus.power_level);

		sldrPOWER->activate();
		selrig->get_pc_min_max_step(min, max, step);
		if (sldrPOWER) sldrPOWER->minimum(min);
		if (sldrPOWER) sldrPOWER->maximum(max);
		if (sldrPOWER) sldrPOWER->step(step);
		if (sldrPOWER) sldrPOWER->value(progStatus.power_level);
		if (sldrPOWER) sldrPOWER->show();
		if (sldrPOWER) sldrPOWER->redraw();

		if (spnrPOWER) spnrPOWER->minimum(min);
		if (spnrPOWER) spnrPOWER->maximum(max);
		if (spnrPOWER) spnrPOWER->step(step);
		if (spnrPOWER) spnrPOWER->value(progStatus.power_level);
		if (spnrPOWER) spnrPOWER->show();
		if (spnrPOWER) spnrPOWER->redraw();

	} else {
 		if (sldrPOWER) sldrPOWER->deactivate();
		if (spnrPOWER) spnrPOWER->deactivate();
	}
}

void set_init_power_control()
{
	if (selrig->has_power_control) {
		if (progStatus.use_rig_data)
			progStatus.power_level = selrig->get_power_control();
		else
			selrig->set_power_control(progStatus.power_level);
	}
	set_power_controlImage(progStatus.power_level);
}

void init_attenuator_control()
{
	if (selrig->has_attenuator_control) {
		if (selrig->name_ == rig_FT891.name_) {
			btnAttenuator->label("ATT");
			btnAttenuator->redraw_label();
		}		
		switch (progStatus.UIsize) {
			case small_ui :
				btnAttenuator->show();
				break;
			case wide_ui : case touch_ui : default :
				btnAttenuator->activate();
		}
	} else {
		switch (progStatus.UIsize) {
			case small_ui :
				btnAttenuator->hide();
				break;
			case wide_ui : case touch_ui : default :
				btnAttenuator->deactivate();
		}
	}
}

void set_init_attenuator_control()
{
	if (selrig->has_attenuator_control) {
		if (!progStatus.use_rig_data)
			selrig->set_attenuator(progStatus.attenuator);
	}
}

void init_agc_control()
{
	if (selrig->has_agc_control) {
		btnAGC->show();
		sldrRFGAIN->label("");
		sldrRFGAIN->redraw_label();
	} else {
		btnAGC->hide();
		sldrRFGAIN->label(_("RF"));
		sldrRFGAIN->redraw_label();
	}
}

void init_preamp_control()
{
	if (selrig->has_preamp_control) {
		if (selrig->name_ == rig_FT891.name_) {
			btnPreamp->label("IPO");
			btnPreamp->redraw_label();
		}
		switch (progStatus.UIsize) {
			case small_ui :
				btnPreamp->show();
				break;
			case wide_ui : case touch_ui : default :
			btnPreamp->activate();
		}
	} else {
		switch (progStatus.UIsize) {
			case small_ui :
				btnPreamp->hide();
				break;
			case wide_ui : case touch_ui : default :
				btnPreamp->deactivate();
		}
	}
}

void set_init_preamp_control()
{
	if (selrig->has_preamp_control) {
		if (!progStatus.use_rig_data)
			selrig->set_preamp(progStatus.preamp);
	}
}

void init_noise_control()
{
	int min, max, step;
	if (selrig->has_noise_control) {
		if (xcvr_name == rig_TS990.name_) {
			btnNOISE->label("AGC"); //Set TS990 AGC Label
			btnNR->label("NR1"); //Set TS990 NR Button
		}
		if (selrig->name_ == rig_FT891.name_) {
			// On the FT-891, the usual definitions of NB and NR buttons
			// as defined in FLRIG are reversed. Relabel them to match
			// what the user sees in the radio screens, and handle the
			// mapping to appropriate cat controls in the FT891.xx class.
			btnNOISE->label("DNR");
			btnNOISE->tooltip(_("DSP Noise Reduction On/Off.  See RX tab for DNR level."));
		}

		btnNOISE->show();
		btnNOISE->activate();
	}
	else {
		btnNOISE->hide();
		btnNOISE->deactivate();
	}

	if (selrig->has_nb_level) {
		selrig->get_nb_min_max_step(min, max, step);
		sldr_nb_level->minimum(min);
		sldr_nb_level->maximum(max);
		sldr_nb_level->step(step);
		sldr_nb_level->value(progStatus.nb_level);
		
		if (selrig->name_ == rig_FT891.name_) {
			// On the FT-891, the usual definitions of NB and NR buttons
			// as defined in FLRIG are reversed. Relabel them to match
			// what the user sees in the radio screens, and handle the
			// mapping to appropriate cat controls in the FT891.xx class.
			sldr_nb_level->label("DNR level");
			sldr_nb_level->tooltip(_("Adjust DSP Noise Reduction level"));
		}
		
		sldr_nb_level->activate();
		sldr_nb_level->redraw();
	} else
		sldr_nb_level->deactivate();
}

void set_init_noise_control()
{
	if (selrig->has_noise_control) {
		if (progStatus.use_rig_data)
			progStatus.noise = selrig->get_noise();
		else
			selrig->set_noise(progStatus.noise);
		btnNOISE->value(progStatus.noise);
		btnNOISE->show();
		btnNOISE->activate();
	}
}

void init_tune_control()
{
	if (selrig->has_tune_control) {
		switch (progStatus.UIsize) {
			case small_ui :
				btnTune->show();
				btn_tune_on_off->show();
				break;
			case wide_ui : case touch_ui : default :
				btnTune->activate();
				btn_tune_on_off->activate();
		}
	} else {
		switch (progStatus.UIsize) {
			case small_ui :
				btnTune->hide();
				btn_tune_on_off->hide();
				break;
			case wide_ui : case touch_ui : default :
				btnTune->deactivate();
				btn_tune_on_off->deactivate();
		}
	}
}

void init_ptt_control()
{
	if (selrig->has_ptt_control ||
		progStatus.comm_dtrptt || progStatus.comm_rtsptt ||
		progStatus.sep_dtrptt || progStatus.sep_rtsptt) {
		btnPTT->activate();
	} else {
		btnPTT->deactivate();
	}
}

void init_auto_notch()
{
	if (selrig->has_auto_notch) {
		if (xcvr_name == rig_RAY152.name_) {
			btnAutoNotch->label("AGC");
			btnAutoNotch->tooltip("AGC on/off");
		} else if (xcvr_name == rig_FT1000MP.name_) {
			btnAutoNotch->label("Tuner");
			btnAutoNotch->tooltip("Tuner on/off");
		} else if (xcvr_name == rig_FT891.name_) {
			btnAutoNotch->label("DNF");
			btnAutoNotch->tooltip("DSP Auto notch filter on/off");
		} else {
			btnAutoNotch->label("AN");
			btnAutoNotch->tooltip("Auto notch on/off");
		}
		switch (progStatus.UIsize) {
			case small_ui :
				btnAutoNotch->show();
				break;
			case wide_ui : case touch_ui : default :
				btnAutoNotch->activate();
		}
	} else {
		switch (progStatus.UIsize) {
			case small_ui :
				btnAutoNotch->hide();
				break;
			case wide_ui : case touch_ui : default :
				btnAutoNotch->deactivate();
		}
	}
}

void set_init_auto_notch()
{
	if (selrig->has_auto_notch) {
		if (progStatus.use_rig_data)
			progStatus.auto_notch = selrig->get_auto_notch();
		else
			selrig->set_auto_notch(progStatus.auto_notch);
		btnAutoNotch->value(progStatus.auto_notch);
	}
}

void init_swr_control()
{
	if (selrig->has_swr_control)
		btnALC_SWR->activate();
	else {
		btnALC_SWR->deactivate();
	}
}

void set_init_compression_control()
{
	if (selrig->has_compON || selrig->has_compression) {
		selrig->set_compression(progStatus.compON, progStatus.compression);
		
		if (selrig->name_ == rig_FT891.name_) {
			// On the FT-891, compression is called PRC, under function 
			// menu FUNCTION-1.  Set the button to match for consistency.
			btnCompON->label("PRC");
			btnCompON->tooltip("Set speech processor for SSB modes on/off.");
		}
	}
}

void init_special_controls()
{
	if (selrig->has_special)
		btnSpecial->show();
	else
		btnSpecial->hide();
}

void init_external_tuner()
{
	if (selrig->has_ext_tuner)
		btn_ext_tuner->show();
	else
		btn_ext_tuner->hide();
}

void init_CIV()
{
	if (selrig->CIV) {
		char hexstr[8];
		snprintf(hexstr, sizeof(hexstr), "0x%02X", selrig->CIV);
		txtCIV->value(hexstr);
		txtCIV->activate();
		btnCIVdefault->activate();
		if (selrig->name_ == rig_IC7200.name_ ||
			selrig->name_ == rig_IC7300.name_ ||
			selrig->name_ == rig_IC7600.name_ ||
			selrig->name_ == rig_IC7800.name_ ) {
			btnUSBaudio->value(progStatus.USBaudio = true);
			btnUSBaudio->activate();
		} else
			btnUSBaudio->deactivate();
	} else {
		txtCIV->value("");
		txtCIV->deactivate();
		btnCIVdefault->deactivate();
		btnUSBaudio->value(false);
		btnUSBaudio->deactivate();
	}
}

void init_VFOs()
{
	if (selrig->name_ == rig_TT550.name_) return;
	if (selrig->name_ == rig_FT817.name_) {
		FreqDispA->value(vfoA.freq);
		FreqDispB->value(vfoB.freq);
		updateBandwidthControl();
		highlight_vfo(NULL);
		useB = false;
		return;
	}

	if (!progStatus.use_rig_data) {

		vfoB.freq = progStatus.freq_B;
		vfoB.imode = progStatus.imode_B;
		vfoB.iBW = progStatus.iBW_B;
//std::cout << "use Status data, vfoB.freq " << vfoB.freq << std::endl;

		if (vfoB.iBW == -1)
			vfoB.iBW = selrig->def_bandwidth(vfoB.imode);

		useB = true;
		selrig->selectB();

		selrig->set_modeB(vfoB.imode);
		selrig->set_bwB(vfoB.iBW);
		selrig->set_vfoB(vfoB.freq);
		FreqDispB->value(vfoB.freq);

		update_progress(progress->value() + 4);

		trace(2, "init_VFOs() vfoB ", printXCVR_STATE(vfoB).c_str());

		vfoA.freq = progStatus.freq_A;
		vfoA.imode = progStatus.imode_A;
		vfoA.iBW = progStatus.iBW_A;

		if (vfoA.iBW == -1)
			vfoA.iBW = selrig->def_bandwidth(vfoA.imode);

		useB = false;
		selrig->selectA();

		selrig->set_modeA(vfoA.imode);
		selrig->set_bwA(vfoA.iBW);
		selrig->set_vfoA(vfoA.freq);
		FreqDispA->value( vfoA.freq );
//std::cout << "use Status data, vfoA.freq " << vfoA.freq << std::endl;

		update_progress(progress->value() + 4);

		vfo = &vfoA;
		updateBandwidthControl();
		highlight_vfo((void *)0);

		trace(2, "init_VFOs() vfoA ", printXCVR_STATE(vfoA).c_str());

	} 
	else {
		// Capture VFOA mode and bandwidth, since it will be lost in VFO switch
		if (selrig->name_ == rig_FT891.name_) {
			useB = false;
			vfoA.freq = selrig->get_vfoA();
			update_progress(progress->value() + 4);
			vfoA.imode = selrig->get_modeA();
			update_progress(progress->value() + 4);
			vfoA.iBW = selrig->get_bwA();
			update_progress(progress->value() + 4);
			FreqDispA->value(vfoA.freq);
			trace(2, "A: ", printXCVR_STATE(vfoA).c_str());

			
			useB = true;
			selrig->selectB();			// third select call
			vfoB.freq = selrig->get_vfoB();
			update_progress(progress->value() + 4);
			vfoB.imode = selrig->get_modeB();
			update_progress(progress->value() + 4);
			vfoB.iBW = selrig->get_bwB();
			update_progress(progress->value() + 4);
			FreqDispB->value(vfoB.freq);
			trace(2, "B: ", printXCVR_STATE(vfoB).c_str());

			// Restore radio VFOA mode, then freq and bandwidth
			useB = false;
			selrig->selectA();			// fourth select call
			yaesu891UpdateA(&vfoA);
		} else {
			vfoB = xcvr_vfoB;
			vfoA = xcvr_vfoA;
//std::cout << "use rig data, vfoA.freq " << vfoA.freq << std::endl;
//std::cout << "use rig data, vfoB.freq " << vfoB.freq << std::endl;
			FreqDispB->value(vfoB.freq);
			FreqDispA->value(vfoA.freq);
		}


		vfo = &vfoA;
		setModeControl((void *)0);
		update_progress(progress->value() + 4);
		updateBandwidthControl();
		update_progress(progress->value() + 4);
		highlight_vfo((void *)0);
	}

	selrig->set_split(0);		// initialization set split call
}

void init_IC7300_special()
{
	if (selrig->name_ ==  rig_IC7300.name_) {
		selrig->enable_break_in();
		redrawAGC();
	}
}

void init_TS990_special()
{
	if (xcvr_name == rig_TS990.name_) { // Setup TS990 Mon Button
		btnIFsh->label("MON");
	}
}

void init_K3_KX3_special()
{
	if (xcvr_name == rig_K3.name_) {
		btnB->hide();
		btnA->hide();
		btn_KX3_swapAB->hide();
		btn_K3_swapAB->show();
	} else if (xcvr_name == rig_KX3.name_) {
		btnB->hide();
		btnA->hide();
		btn_K3_swapAB->hide();
		btn_KX3_swapAB->show();
	} else {
		btn_K3_swapAB->hide();
		btn_KX3_swapAB->hide();
		btnB->show();
		btnA->show();
	}
}

void initRig()
{
	xcvr_initialized = false;
	if (tabs_dialog && tabs_dialog->visible()) tabs_dialog->hide();

	grpInitializing->show();
	main_group->hide();
	mainwindow->redraw();

	flrig_abort = false;

	sldrRcvSignal->aging(progStatus.rx_peak);
	sldrRcvSignal->avg(progStatus.rx_avg);
	sldrFwdPwr->aging(progStatus.pwr_peak);
	sldrFwdPwr->avg(progStatus.pwr_avg);

	if (progStatus.use_tcpip) {
		try {
			connect_to_remote();
		} catch (...) {
			grpInitializing->hide();
			main_group->show();
			mainwindow->redraw();
			return;
		}
	}

// disable the serial thread
	{
		guard_lock gl_serial(&mutex_serial);
		trace(1, "init_rig()");

		// Xcvr Auto Power on as soon as possible
		if (selrig->has_xcvr_auto_on_off && progStatus.xcvr_auto_on)
			selrig->set_xcvr_auto_on();

		bool check_ok = false;
		if (progStatus.xcvr_serial_port != "NONE") check_ok = selrig->check();

		init_special_controls();
		init_external_tuner();
		init_rit();
		init_xit();
		init_bfo();
		init_dsp_controls();
		init_volume_control();
		init_rf_control();
		init_sql_control();
		init_noise_reduction_control();
		init_if_shift_control();
		init_notch_control();
		init_micgain_control();
		init_power_control();
		init_attenuator_control();
		init_agc_control();
		init_preamp_control();
		init_noise_control();
		init_tune_control();
		init_ptt_control();
		init_auto_notch();
		init_swr_control();

		selrig->initialize();

		if (progStatus.xcvr_serial_port != "NONE") {
			if (!check_ok) goto failed;
			if (flrig_abort) goto failed;
		}

		FreqDispA->set_precision(selrig->precision);
		FreqDispA->set_ndigits(selrig->ndigits);
		FreqDispB->set_precision(selrig->precision);
		FreqDispB->set_ndigits(selrig->ndigits);

		FreqDispB->set_precision(selrig->precision);
		FreqDispB->set_ndigits(selrig->ndigits);
		FreqDispB->set_precision(selrig->precision);
		FreqDispB->set_ndigits(selrig->ndigits);

		if (xcvr_name == rig_TT550.name_)
			init_TT550();
		else
			init_generic_rig();

		set_init_volume_control();
		set_init_rf_gain();
		set_init_sql_control();
		set_init_noise_reduction_control();
		set_init_if_shift_control();
		set_init_micgain_control();
		set_init_power_control();
		set_init_attenuator_control();
		set_init_preamp_control();
		set_init_noise_control();
		set_init_auto_notch();
		set_init_notch_control();
		set_init_compression_control();

		initTabs();
		buildlist();

		init_CIV();

		init_VFOs();

		selrig->post_initialize();

		init_IC7300_special();
		init_TS990_special();
		init_K3_KX3_special();

		btnAswapB->show();

	}  // enable the serial thread


	bypass_serial_thread_loop = false;

	grpInitializing->hide();

	adjust_control_positions();

	main_group->show();
	mainwindow->redraw();
	mainwindow->damage();
	Fl::flush();
//	Fl::check();

	xcvr_initialized = true;
	return;

failed:
	xcvr_initialized = false;
	adjust_control_positions();
	grpInitializing->hide();
	main_group->show();
	mainwindow->redraw();

	bypass_serial_thread_loop = true;

	fl_alert2(_("\
Transceiver not responding!\n\n\
Check serial (COM) port connection\n\
Open menu Config/Setup/Transceiver\n\
Press 'Ser Port' button, reselect port\n\
Press 'Init' button."));

	return;
}

void init_title()
{
	title = PACKAGE;
	title += " ";
	title.append(selrig->name_);
	mainwindow->label(title.c_str());
}

void initConfigDialog()
{
	int picked = selectRig->index();
	rigbase *srig = rigs[picked];
	xcvr_name = srig->name_;

	if (!progStatus.loadXcvrState(xcvr_name) ) {

		selectCommPort->index(0);
		mnuBaudrate->index( srig->comm_baudrate );
		btnOneStopBit->value( srig->stopbits == 1 );
		btnTwoStopBit->value( srig->stopbits == 2 );
		cntRigCatRetries->value( srig->comm_retries );
		cntRigCatTimeout->value( srig->comm_timeout );
		cntRigCatWait->value( srig->comm_wait );
		btnRigCatEcho->value( srig->comm_echo );
		btncatptt->value( srig->comm_catptt );
		btnrtsptt->value( srig->comm_rtsptt );
		btndtrptt->value( srig->comm_dtrptt );
		chkrtscts->value( srig->comm_rtscts );
		btnrtsplus->value( srig->comm_rtsplus );
		btndtrplus->value( srig->comm_dtrplus );

		if (selrig->CIV) {
			char hexstr[8];
			snprintf(hexstr, sizeof(hexstr), "0x%02X", srig->CIV);
			txtCIV->value(hexstr);
			txtCIV->activate();
			btnCIVdefault->activate();
			if (xcvr_name == rig_IC7200.name_ ||
				xcvr_name == rig_IC7300.name_ ||
				xcvr_name == rig_IC7600.name_ ||
				xcvr_name == rig_IC7800.name_) {
				btnUSBaudio->value(progStatus.USBaudio = true);
				btnUSBaudio->activate();
			} else
				btnUSBaudio->deactivate();
		} else {
			txtCIV->value("");
			txtCIV->deactivate();
			btnCIVdefault->deactivate();
			btnUSBaudio->value(false);
			btnUSBaudio->deactivate();
		}
	} else {
		initStatusConfigDialog();
		trace(1, progStatus.info().c_str());
	}
}

void initStatusConfigDialog()
{
	if (progStatus.CIV) selrig->adjustCIV(progStatus.CIV);

	selectRig->value(xcvr_name.c_str());

	mnuBaudrate->index( progStatus.comm_baudrate );

	selectCommPort->value( progStatus.xcvr_serial_port.c_str() );
	selectAuxPort->value( progStatus.aux_serial_port.c_str() );
	selectSepPTTPort->value( progStatus.sep_serial_port.c_str() );
	btnOneStopBit->value( progStatus.stopbits == 1 );
	btnTwoStopBit->value( progStatus.stopbits == 2 );

	cntRigCatRetries->value( progStatus.comm_retries );
	cntRigCatTimeout->value( progStatus.comm_timeout );
	cntRigCatWait->value( progStatus.comm_wait );
	btnRigCatEcho->value( progStatus.comm_echo );

	btncatptt->value( progStatus.comm_catptt );
	btnrtsptt->value( progStatus.comm_rtsptt );
	btndtrptt->value( progStatus.comm_dtrptt );
	chkrtscts->value( progStatus.comm_rtscts );
	btnrtsplus->value( progStatus.comm_rtsplus );
	btndtrplus->value( progStatus.comm_dtrplus );

	btnSepDTRplus->value(progStatus.sep_dtrplus);
	btnSepDTRptt->value(progStatus.sep_dtrptt);
	btnSepRTSplus->value(progStatus.sep_rtsplus);
	btnSepRTSptt->value(progStatus.sep_rtsptt);

	if (progStatus.use_tcpip) {
		box_xcvr_connect->color(FL_BACKGROUND2_COLOR);
		box_xcvr_connect->redraw();
	} else {
		if (!startXcvrSerial()) {
			if (progStatus.xcvr_serial_port.compare("NONE") == 0) {
				LOG_WARN("No comm port ... test mode");
			} else {
				fl_alert2("\
Cannot open %s!\n\n\
Check serial (COM) port connection\n\
Open menu Config/Setup/Transceiver\n\
Press 'Ser Port' button, reselect port\n\
Press 'Init' button.", progStatus.xcvr_serial_port.c_str());
				LOG_WARN("Cannot open %s", progStatus.xcvr_serial_port.c_str());
				progStatus.xcvr_serial_port = "NONE";
				selectCommPort->value(progStatus.xcvr_serial_port.c_str());
			}
			box_xcvr_connect->color(FL_BACKGROUND2_COLOR);
			box_xcvr_connect->redraw();
		} else {
			selectCommPort->value(progStatus.xcvr_serial_port.c_str());
			box_xcvr_connect->color(FL_GREEN);
			box_xcvr_connect->redraw();
		}
		if (!startAuxSerial()) {
			if (progStatus.aux_serial_port.compare("NONE") != 0) {
				LOG_WARN("Cannot open %s", progStatus.aux_serial_port.c_str());
				progStatus.aux_serial_port = "NONE";
				selectAuxPort->value(progStatus.aux_serial_port.c_str());
			}
		}
		if (!startSepSerial()) {
			if (progStatus.sep_serial_port.compare("NONE") != 0) {
				LOG_WARN("Cannot open %s", progStatus.sep_serial_port.c_str());
				progStatus.sep_serial_port = "NONE";
				selectSepPTTPort->value(progStatus.sep_serial_port.c_str());
			}
		}
	}

	init_title();

	initRig();

}

void initRigCombo()
{
	selectRig->clear();
	int i = 0;
	while (rigs[i] != NULL)
		selectRig->add(rigs[i++]->name_.c_str());

	selectRig->index(0);
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

void nr_label(const char *l, bool on = false)
{
	nr_label_ = l;
	nr_state_ = on;
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

void nb_label(const char * l, bool on = false)
{
	nb_label_ = l;
	nb_state_ = on;
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

void preamp_label(const char * l, bool on = false)
{
	preamp_label_ = l;
	preamp_state_ = on;
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

void atten_label(const char * l, bool on = false)
{
	atten_label_ = l;
	atten_state_ = on;
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

void auto_notch_label(const char * l, bool on = false)
{
	auto_notch_label_ = l;
	auto_notch_state_ = on;
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
	MilliSleep(progStatus.comm_wait);
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
		MilliSleep(progStatus.comm_wait);

		get = selrig->get_noise_reduction();
		while ((get != btn) && (cnt++ < 10)) {
			MilliSleep(progStatus.comm_wait);
			get = selrig->get_noise_reduction();
			Fl::awake();
		}
		progStatus.noise_reduction_val = set;
		selrig->set_noise_reduction_val(set);
		MilliSleep(progStatus.comm_wait);
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

void cb_enable_break_in()
{
	guard_lock serial_lock(&mutex_serial);
	trace(1, "cb_enable_break_in()");
	selrig->enable_break_in();
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
			if (band > 8) { // 10, 6, 2, 70
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
	if (!useB) {
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
	if (!useB) { FreqDispA->value(vfo->freq); FreqDispA->redraw(); }
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

// trim leading and trailing whitspace and double quote
const string lt_trim(const string& pString, const string& pWhitespace)
{
	size_t beginStr, endStr, range;
	beginStr = pString.find_first_not_of(pWhitespace);
	if (beginStr == string::npos) return "";	// no content
	endStr = pString.find_last_not_of(pWhitespace);
	range = endStr - beginStr + 1;

	return pString.substr(beginStr, range);
}

void editAlphaTag()
{
	int indx;
	string atag;
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

