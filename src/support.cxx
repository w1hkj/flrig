#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>

#include "icons.h"
#include "support.h"
#include "debug.h"
#include "gettext.h"
#include "rig_io.h"
#include "dialogs.h"
#include "rigbase.h"
#include "ptt.h"
#include "xml_io.h"

#include "rigs.h"
#include "K3_ui.h"

using namespace std;

rigbase *selrig = rigs[0];

extern bool test;

bool flrig_abort = false;

int freqval = 0;

FREQMODE vfoA = {14070000, 0, 0, UI};
FREQMODE vfoB = {7070000, 0, 0, UI};
FREQMODE vfo = {0, 0, 0, UI};
FREQMODE transceiverA;
FREQMODE transceiverB;
FREQMODE xmlvfo = vfoA;

queue<FREQMODE> queA;
queue<FREQMODE> queB;
queue<bool> quePTT;

bool useB = false;
bool changed_vfo = false;
bool pushedB = false;

const char **old_bws = NULL;

// Add alpha-tag to FREQMODE;
struct ATAG_FREQMODE {
	long freq;
	int  imode;
	int  iBW;
	int  src;
	char alpha_tag[ATAGSIZE];
};
ATAG_FREQMODE oplist[LISTSIZE];

int  numinlist = 0;
vector<string> rigmodes_;
vector<string> rigbws_;

Cserial RigSerial;
Cserial AuxSerial;
Cserial SepSerial;

bool using_buttons = false;

enum { SWR_IMAGE, ALC_IMAGE };
int meter_image = SWR_IMAGE;

//=============================================================================
// loop for serial i/o thread
// runs continuously until program is closed
// only accesses the serial port if it has been successfully opened
//=============================================================================

bool bypass_serial_thread_loop = true;
bool run_serial_thread = true;

bool PTT = false;
int  powerlevel = 0;

char *print(FREQMODE data)
{
	static char str[100];
	snprintf(str, sizeof(str), "%3s,%10ld, %4s, %5s %5s",
		data.src == XML ? "xml" : "ui",
		data.freq,
		selrig->modes_ ? selrig->modes_[data.imode] : "modes n/a",
		(data.iBW > 256 && selrig->has_dsp_controls) ?
			selrig->dsp_lo[data.iBW & 0x7F] : selrig->bandwidths_ ? selrig->bandwidths_[data.iBW] : "bw n/a",
		(data.iBW > 256 && selrig->has_dsp_controls) ?
			selrig->dsp_hi[(data.iBW >> 8) & 0x7F] : "" 
		);
	return str;
}

// the following functions are ONLY CALLED by the serial loop
// read any data stream sent by transceiver

// read current vfo frequency

void read_info()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->get_info();
	pthread_mutex_unlock(&mutex_serial);
}

void read_vfo()
{
// transceiver changed ?
	pthread_mutex_lock(&mutex_serial);
	long  freq;
	if (!useB) { // vfo-A
		freq = selrig->get_vfoA();
		if (freq != vfoA.freq) {
			vfoA.freq = freq;
			Fl::awake(setFreqDispA, (void *)vfoA.freq);
			vfo = vfoA;
			send_xml_freq(vfo.freq);
		}
		if ( selrig->twovfos() ) {
			freq = selrig->get_vfoB();
			if (freq != vfoB.freq) {
				vfoB.freq = freq;
				Fl::awake(setFreqDispB, (void *)vfoB.freq);
			}
		}
	} else { // vfo-B
		freq = selrig->get_vfoB();
		if (freq != vfoB.freq) {
			vfoB.freq = freq;
			Fl::awake(setFreqDispB, (void *)vfoB.freq);
			vfo = vfoB;
			send_xml_freq(vfo.freq);
		}
		if ( selrig->twovfos() ) {
			freq = selrig->get_vfoA();
			if (freq != vfoA.freq) {
				vfoA.freq = freq;
				Fl::awake(setFreqDispA, (void *)vfoA.freq);
			}
		}
	}
	pthread_mutex_unlock(&mutex_serial);
}

void setModeControl(void *)
{
	opMODE->index(vfo.imode);
}

// mode and bandwidth
void read_mode()
{
	pthread_mutex_lock(&mutex_serial);
	int nu_mode;
	int nu_BW;
	if (!useB) {
		nu_mode = selrig->get_modeA();
		if (nu_mode != vfoA.imode) {
			pthread_mutex_lock(&mutex_xmlrpc);
			vfoA.imode = vfo.imode = nu_mode;
			selrig->adjust_bandwidth(vfo.imode);
			nu_BW = selrig->get_bwA();
			vfoA.iBW = vfo.iBW = nu_BW;
			try {
				send_bandwidths();
				send_new_mode(nu_mode);
				send_sideband();
				send_new_bandwidth(vfo.iBW);
			} catch (...) {}
			pthread_mutex_unlock(&mutex_xmlrpc);
			Fl::awake(setModeControl);
			set_bandwidth_control();
		} else {	// mode and bwtable are in sync so read vfoA bw
			nu_BW = selrig->get_bwA();
			if (nu_BW != vfoA.iBW) {
				pthread_mutex_lock(&mutex_xmlrpc);
				vfoA.iBW = vfo.iBW = nu_BW;
				Fl::awake(setBWControl);
				try {
					send_new_bandwidth(vfo.iBW);
				} catch (...) {}
				pthread_mutex_unlock(&mutex_xmlrpc);
			}
		}
	} else {
		nu_mode = selrig->get_modeB();
		if (nu_mode != vfoB.imode) {
			pthread_mutex_lock(&mutex_xmlrpc);
			vfoB.imode = vfo.imode = nu_mode;
			selrig->adjust_bandwidth(vfo.imode);
			nu_BW = selrig->get_bwB();
			vfoB.iBW = vfo.iBW = nu_BW;
			try {
				send_bandwidths();
				send_new_mode(nu_mode);
				send_sideband();
				send_new_bandwidth(vfo.iBW);
			} catch (...) {}
			pthread_mutex_unlock(&mutex_xmlrpc);
			Fl::awake(setModeControl);
			set_bandwidth_control();
		} else {	// mode and bwtable are in sync so read vfoB bw
			nu_BW = selrig->get_bwB();
			if (nu_BW != vfoB.iBW) {
				pthread_mutex_lock(&mutex_xmlrpc);
				vfoB.iBW = vfo.iBW = nu_BW;
				Fl::awake(setBWControl);
				try {
					send_new_bandwidth(vfo.iBW);
				} catch (...) {}
				pthread_mutex_unlock(&mutex_xmlrpc);
			}
		}
	}
	pthread_mutex_unlock(&mutex_serial);
}

void setBWControl(void *)
{
	if (selrig->has_dsp_controls) {
		if (vfo.iBW < 256) {
			opBW->index(vfo.iBW);
			opDSP_lo->hide();
			opDSP_hi->hide();
			btnDSP->hide();
			opDSP_lo->index(0);
			opDSP_hi->index(0);
			opBW->show();
		} else {
			opDSP_lo->index(vfo.iBW & 0xFF);
			opDSP_hi->index((vfo.iBW >> 8) & 0x7F);
			if (!btnDSP->visible()) {
				opBW->index(0);
				opBW->hide();
				opDSP_hi->show();
				opDSP_lo->hide();
				btnDSP->label(selrig->hi_label);
				btnDSP->redraw_label();
				btnDSP->show();
			}
		}
	} else {
		opBW->index(vfo.iBW);
		opDSP_lo->hide();
		opDSP_hi->hide();
		btnDSP->hide();
		opBW->show();
	}
}

void read_bandwidth()
{
	return;
	
	pthread_mutex_lock(&mutex_serial);
	int nu_BW;
	if (!useB) {
		nu_BW = selrig->get_bwA();
		if (nu_BW != vfoA.iBW) {
			pthread_mutex_lock(&mutex_xmlrpc);
			vfoA.iBW = vfo.iBW = nu_BW;
			Fl::awake(setBWControl);
			try {
				send_new_bandwidth(vfo.iBW);
			} catch (...) {}
			pthread_mutex_unlock(&mutex_xmlrpc);
		}
	} else {
		nu_BW = selrig->get_bwB();
		if (nu_BW != vfoB.iBW) {
			vfoB.iBW = vfo.iBW = nu_BW;
			pthread_mutex_lock(&mutex_xmlrpc);
			Fl::awake(setBWControl);
			try {
				send_new_bandwidth(vfo.iBW);
			} catch (...) {}
			pthread_mutex_unlock(&mutex_xmlrpc);
		}
	}
	pthread_mutex_unlock(&mutex_serial);
}

// read current signal level
void read_smeter()
{
	if (!selrig->has_smeter) return;
	int  sig;
	pthread_mutex_lock(&mutex_serial);
	sig = selrig->get_smeter();
	pthread_mutex_unlock(&mutex_serial);
	if (sig == -1) return;
	Fl::awake(updateSmeter, (void*)sig);
}

// read power out
void read_power_out()
{
	if (!selrig->has_power_out) return;
	int sig;
	pthread_mutex_lock(&mutex_serial);
	sig = selrig->get_power_out();
	pthread_mutex_unlock(&mutex_serial);
	if (sig > -1)
		Fl::awake(updateFwdPwr, (void*)sig);
}

// read swr
void read_swr()
{
	if (!selrig->has_swr_control) return;
	int sig;
	pthread_mutex_lock(&mutex_serial);
	sig = selrig->get_swr();
	pthread_mutex_unlock(&mutex_serial);
	if (sig > -1)
		Fl::awake(updateSWR, (void*)sig);
}

// alc
void read_alc()
{
	if (!selrig->has_alc_control) return;
	int sig;
	pthread_mutex_lock(&mutex_serial);
		sig = selrig->get_alc();
	pthread_mutex_unlock(&mutex_serial);
	if (sig > -1)
		Fl::awake(updateALC, (void*)sig);
}

// notch
void update_auto_notch(void *d)
{
	btnAutoNotch->value(progStatus.auto_notch);
}

void read_auto_notch()
{
	if (!selrig->has_auto_notch) return;
	pthread_mutex_lock(&mutex_serial);
		progStatus.auto_notch = selrig->get_auto_notch();
	pthread_mutex_unlock(&mutex_serial);
	Fl::awake(update_auto_notch, (void*)0);
}

// NOISE blanker
void update_noise(void *d)
{
	btnNOISE->value(progStatus.noise);
}

void read_noise()
{
	pthread_mutex_lock(&mutex_serial);
		progStatus.noise = selrig->get_noise();
	pthread_mutex_unlock(&mutex_serial);
	Fl::awake(update_noise, (void*)0);
}

// preamp - attenuator
void update_preamp(void *d)
{
	btnPreamp->value(progStatus.preamp);
}

void update_attenuator(void *d)
{
	btnAttenuator->value(progStatus.attenuator);
}

void read_preamp_att()
{
	if (selrig->has_preamp_control) {
		pthread_mutex_lock(&mutex_serial);
			progStatus.preamp = selrig->get_preamp();
		pthread_mutex_unlock(&mutex_serial);
		Fl::awake(update_preamp, (void*)0);
	}
	if (selrig->has_attenuator_control) {
		pthread_mutex_lock(&mutex_serial);
			progStatus.attenuator = selrig->get_attenuator();
		pthread_mutex_unlock(&mutex_serial);
		Fl::awake(update_attenuator, (void*)0);
	}
}

// split
void update_split(void *d)
{
	if (rig_nbr == FT450 || rig_nbr == FT950) {
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
		btnSplit->value(progStatus.split);
}

void read_split()
{
	int val;
	if (selrig->can_split()) {
		pthread_mutex_lock(&mutex_serial);
			val = selrig->get_split();
		pthread_mutex_unlock(&mutex_serial);
		if (val != progStatus.split) {
			progStatus.split = val;
			Fl::awake(update_split, (void*)0);
		}
	}
}

// volume
void update_volume(void *d)
{
	long *nr = (long *)d;
	sldrVOLUME->value(progStatus.volume);
	sldrVOLUME->activate();
	if (*nr) btnVol->value(1);
	else     btnVol->value(0);
}

long nlzero = 0L;
long nlone = 1L;

void read_volume()
{
	if (!selrig->has_volume_control) return;
//	if (rig_nbr == FT950) {
		int vol;
		pthread_mutex_lock(&mutex_serial);
			vol = selrig->get_volume_control();
		pthread_mutex_unlock(&mutex_serial);
		if (vol != progStatus.volume) {
			if (vol <= 1 && !btnVol->value()) return;
			progStatus.volume = vol;
			if (vol <= 1 && btnVol->value())
				Fl::awake(update_volume, (void*)&nlzero);
			else
				Fl::awake(update_volume, (void*)&nlone);
		}
//	} else {
//		if (btnVol->value() == 0) return; // muted
//		if (!sldrVOLUME->active()) return; // cbMute() not done still un-muting
//		pthread_mutex_lock(&mutex_serial);
//			progStatus.volume = selrig->get_volume_control();
//		pthread_mutex_unlock(&mutex_serial);
//		Fl::awake(update_volume, (void*)&nlzero);
//	}
}

// ifshift
void update_ifshift(void *d)
{
	btnIFsh->value(progStatus.shift);
	sldrIFSHIFT->value(progStatus.shift_val);
}

void read_ifshift()
{
	if (!selrig->has_ifshift_control) return;
	pthread_mutex_lock(&mutex_serial);
		progStatus.shift = selrig->get_if_shift(progStatus.shift_val);
	pthread_mutex_unlock(&mutex_serial);
	Fl::awake(update_ifshift, (void*)0);
}

// noise reduction
void update_nr(void *d)
{
	btnNR->value(progStatus.noise_reduction);
	sldrNR->value(progStatus.noise_reduction_val);
}

void read_nr()
{
	if (!selrig->has_noise_reduction) return;
	pthread_mutex_lock(&mutex_serial);
		progStatus.noise_reduction = selrig->get_noise_reduction();
		progStatus.noise_reduction_val = selrig->get_noise_reduction_val();
	pthread_mutex_unlock(&mutex_serial);
	Fl::awake(update_nr, (void*)0);
}

// manual notch
void update_notch(void *d)
{
	btnNotch->value(progStatus.notch);
	sldrNOTCH->value(progStatus.notch_val);
	send_new_notch(progStatus.notch ? progStatus.notch_val : 0);
}

void read_notch()
{
	if (!selrig->has_notch_control) return;
	pthread_mutex_lock(&mutex_serial);
		progStatus.notch = selrig->get_notch(progStatus.notch_val);
	pthread_mutex_unlock(&mutex_serial);
	Fl::awake(update_notch, (void*)0);
}

// power_control
void update_power_control(void *d)
{
	set_power_controlImage(progStatus.power_level);
	sldrPOWER->value(progStatus.power_level);
	if (rig_nbr == K2) {
		double min, max, step;
		selrig->get_pc_min_max_step(min, max, step);
		sldrPOWER->minimum(min);
		sldrPOWER->maximum(max);
		sldrPOWER->step(step);
		sldrPOWER->redraw();
	}
}

void read_power_control()
{
	if (!selrig->has_power_control) return;
	pthread_mutex_lock(&mutex_serial);
		progStatus.power_level = selrig->get_power_control();
	pthread_mutex_unlock(&mutex_serial);
	Fl::awake(update_power_control, (void*)0);
}

// mic gain
void update_mic_gain(void *d)
{
	sldrMICGAIN->value(progStatus.mic_gain);
}

void read_mic_gain()
{
	if (!selrig->has_micgain_control) return;
	pthread_mutex_lock(&mutex_serial);
		progStatus.mic_gain = selrig->get_mic_gain();
	pthread_mutex_unlock(&mutex_serial);
	Fl::awake(update_mic_gain, (void*)0);
}

// rf gain
void update_rfgain(void *d)
{
	sldrRFGAIN->value(progStatus.rfgain);
}

void read_rfgain()
{
	if (!selrig->has_rf_control) return;
	pthread_mutex_lock(&mutex_serial);
		progStatus.rfgain = selrig->get_rf_gain();
	pthread_mutex_unlock(&mutex_serial);
	Fl::awake(update_rfgain, (void*)0);
}

// squelch
void update_squelch(void *d)
{
	sldrSQUELCH->value(progStatus.squelch);
}

void read_squelch()
{
	if (!selrig->has_sql_control) return;
	pthread_mutex_lock(&mutex_serial);
		progStatus.squelch = selrig->get_squelch();
	pthread_mutex_unlock(&mutex_serial);
	Fl::awake(update_squelch, (void*)0);
}

static bool resetrcv = true;
static bool resetxmt = true;

void serviceA()
{
	if (!selrig->can_change_alt_vfo && useB) return;
	if (queA.empty()) return;

	pthread_mutex_lock(&mutex_serial);
	pthread_mutex_lock(&mutex_xmlrpc);
	pthread_mutex_lock(&mutex_queA);
	while (!queA.empty()) {
		vfoA = queA.front();
		queA.pop();
	}
	pthread_mutex_unlock(&mutex_queA);

	if (RIG_DEBUG)
		LOG_INFO("%s", print(vfoA));

	if (changed_vfo && !useB) {
		selrig->selectA();
	}

// if TT550 etal and on the B vfo
	if (selrig->can_change_alt_vfo && useB) {
		selrig->set_vfoA(vfoA.freq);
		goto end_serviceA;
	}

	if (vfoA.freq != vfo.freq || changed_vfo) {
		selrig->set_vfoA(vfoA.freq);
		Fl::awake(setFreqDispA, (void *)vfoA.freq);
		vfo.freq = vfoA.freq;
		if (vfoA.src == UI) send_xml_freq(vfoA.freq);
	}
// adjust for change in bandwidths_
	if (vfoA.imode != vfo.imode || changed_vfo) {
		selrig->set_modeA(vfoA.imode);
		vfo.imode = vfoA.imode;
		Fl::awake(setModeControl);
		vfo.iBW = vfoA.iBW;
		set_bandwidth_control();
		Fl::awake(setBWControl);
		selrig->set_bwA(vfo.iBW);
		try {
			if (vfoA.src == UI)
				send_new_mode(vfoA.imode);
			send_sideband();
			send_bandwidths();
			send_new_bandwidth(vfo.iBW);
		} catch (...) {}
	} else if (vfoA.iBW != vfo.iBW) {
		selrig->set_bwA(vfoA.iBW);
		vfo.iBW = vfoA.iBW;
		Fl::awake(setBWControl);
		if (vfoA.src == UI) {
			try {
				send_new_bandwidth(vfo.iBW);
			} catch (...) {}
		}
	}

end_serviceA:
	changed_vfo = false;

	pthread_mutex_unlock(&mutex_xmlrpc);
	pthread_mutex_unlock(&mutex_serial);
}

void serviceB()
{
	if (!selrig->can_change_alt_vfo && !useB) return;

	if (queB.empty())
		return;

	pthread_mutex_lock(&mutex_serial);
	pthread_mutex_lock(&mutex_xmlrpc);
	pthread_mutex_lock(&mutex_queB);
	while (!queB.empty()) {
		vfoB = queB.front();
		queB.pop();
	}
	pthread_mutex_unlock(&mutex_queB);

	if (RIG_DEBUG)
		LOG_INFO("%s", print(vfoB));

	if (changed_vfo && useB) {
		selrig->selectB();
	}

// if TT550 or K3 and split or on vfoA just update the B vfo
	if (selrig->can_change_alt_vfo && !useB) {
		selrig->set_vfoB(vfoB.freq);
		goto end_serviceB;
	}

	if (vfoB.freq != vfo.freq || pushedB || changed_vfo) {
		selrig->set_vfoB(vfoB.freq);
		vfo.freq = vfoB.freq;
		Fl::awake(setFreqDispB, (void *)vfoB.freq);
		if (vfoB.src == UI) send_xml_freq(vfoB.freq);
	}
	if (vfoB.imode != vfo.imode || pushedB || changed_vfo) {
		selrig->set_modeB(vfoB.imode);
		vfo.imode = vfoB.imode;
		Fl::awake(setModeControl);
		vfo.iBW = vfoB.iBW;
		set_bandwidth_control();
		Fl::awake(setBWControl);
		selrig->set_bwB(vfo.iBW);
		try {
			if (vfoB.src == UI)
				send_new_mode(vfoB.imode);
			send_sideband();
			send_bandwidths();
			send_new_bandwidth(vfo.iBW);
		} catch (...) {}
	} else if (vfoB.iBW != vfo.iBW || pushedB) {
		selrig->set_bwB(vfoB.iBW);
		vfo.iBW = vfoB.iBW;
		Fl::awake(setBWControl);
		if (vfoB.src == UI) {
			try {
				send_new_bandwidth(vfo.iBW);
			} catch (...) {}
		}
	}
	pushedB = false;

end_serviceB:
	changed_vfo = false;
	pthread_mutex_unlock(&mutex_xmlrpc);
	pthread_mutex_unlock(&mutex_serial);
}

void servicePTT()
{
	pthread_mutex_lock(&mutex_ptt);
	while (!quePTT.empty()) {
		PTT = quePTT.front();
		quePTT.pop();
		rigPTT(PTT);
		Fl::awake(update_UI_PTT);
	}
	pthread_mutex_unlock(&mutex_ptt);
}

void * serial_thread_loop(void *d)
{
  static int  loopcount = progStatus.serloop_timing / 10;
  static int  poll_nbr = 0;

	for(;;) {
		if (!run_serial_thread) break;

		MilliSleep(10);

		if (bypass_serial_thread_loop) goto serial_bypass_loop;

//send any freq/mode/bw changes in the queu

		servicePTT();

		if (!PTT) {
			serviceA();
			if (!quePTT.empty()) continue;
			serviceB();
			if (!quePTT.empty()) continue;

			if (resetrcv) {
				Fl::awake(zeroXmtMeters, 0);
				resetrcv = false;
				loopcount = progStatus.serloop_timing / 10;
				poll_nbr = 0;
			}
			resetxmt = true;

			if (!loopcount--) {
				poll_nbr++;

				if (rig_nbr == K3) read_K3();
				else if ((rig_nbr == K2) ||
						 (selrig->has_get_info &&
						 (progStatus.poll_frequency || progStatus.poll_mode || progStatus.poll_bandwidth)))
					read_info();

				if (progStatus.poll_frequency)
					if (!(poll_nbr % progStatus.poll_frequency)) read_vfo();
				if (!quePTT.empty()) continue;
				if (progStatus.poll_mode)
					if (!(poll_nbr % progStatus.poll_mode)) read_mode();
				if (!quePTT.empty()) continue;
				if (progStatus.poll_bandwidth)
					if (!(poll_nbr % progStatus.poll_bandwidth)) read_bandwidth();
				if (!quePTT.empty()) continue;

				if (progStatus.poll_smeter)
					if (!(poll_nbr % progStatus.poll_smeter)) read_smeter();
				if (!quePTT.empty()) continue;

				if (progStatus.poll_volume)
					if (!(poll_nbr % progStatus.poll_volume)) read_volume();
				if (!quePTT.empty()) continue;
				if (progStatus.poll_auto_notch)
					if (!(poll_nbr % progStatus.poll_auto_notch)) read_auto_notch();
				if (!quePTT.empty()) continue;
				if (progStatus.poll_notch)
					if (!(poll_nbr % progStatus.poll_notch)) read_notch();
				if (!quePTT.empty()) continue;
				if (progStatus.poll_ifshift)
					if (!(poll_nbr % progStatus.poll_ifshift)) read_ifshift();
				if (!quePTT.empty()) continue;
				if (progStatus.poll_power_control)
					if (!(poll_nbr % progStatus.poll_power_control)) read_power_control();
				if (!quePTT.empty()) continue;
				if (progStatus.poll_pre_att)
					if (!(poll_nbr % progStatus.poll_pre_att)) read_preamp_att();
				if (!quePTT.empty()) continue;
				if (progStatus.poll_micgain)
					if (!(poll_nbr % progStatus.poll_micgain)) read_mic_gain();
				if (!quePTT.empty()) continue;
				if (progStatus.poll_squelch)
					if (!(poll_nbr % progStatus.poll_squelch)) read_squelch();
				if (!quePTT.empty()) continue;
				if (progStatus.poll_rfgain)
					if (!(poll_nbr % progStatus.poll_rfgain)) read_rfgain();
				if (!quePTT.empty()) continue;
				if (progStatus.poll_split)
					if (!(poll_nbr % progStatus.poll_split)) read_split();
				if (!quePTT.empty()) continue;
				if (progStatus.poll_nr)
					if (!(poll_nbr % progStatus.poll_nr)) read_nr();
				if (!quePTT.empty()) continue;
				if (progStatus.poll_noise)
					if (!(poll_nbr % progStatus.poll_noise)) read_noise();

				loopcount = progStatus.serloop_timing / 10;
			}
		} else {
			if (resetxmt) {
				Fl::awake(updateSmeter, (void *)(0));
				resetxmt = false;
				loopcount = progStatus.serloop_timing / 10;
				poll_nbr = 0;
			}
			resetrcv = true;
			if (!loopcount--) {
				poll_nbr++;
				if (progStatus.poll_pout)
					if (!(poll_nbr % progStatus.poll_pout)) read_power_out();
				if (!quePTT.empty()) continue;
				if (progStatus.poll_swr)
					if (!(poll_nbr % progStatus.poll_swr)) read_swr();
				if (!quePTT.empty()) continue;
				if (progStatus.poll_alc)
					if (!(poll_nbr % progStatus.poll_alc)) read_alc();
				loopcount = progStatus.serloop_timing / 10;
			}

		}
serial_bypass_loop: ;

	}
	return NULL;
}

//=============================================================================

void setFocus()
{
	if (useB) Fl::focus(FreqDispB);
	else Fl::focus(FreqDispA);
}

void setBW()
{
	FREQMODE fm = vfo;
	fm.src = UI;
	fm.iBW = opBW->index();
	if (useB) {
		pthread_mutex_lock(&mutex_queB);
		queB.push(fm);
		pthread_mutex_unlock(&mutex_queB);
	} else {
		pthread_mutex_lock(&mutex_queA);
		queA.push(fm);
		pthread_mutex_unlock(&mutex_queA);
	}
	setFocus();
}

void setDSP()
{
	FREQMODE fm = vfo;
	fm.src = UI;
	fm.iBW = ((opDSP_hi->index() << 8) | 0x8000) | (opDSP_lo->index() & 0xFF) ;
	if (useB) {
		pthread_mutex_lock(&mutex_queB);
		queB.push(fm);
		pthread_mutex_unlock(&mutex_queB);
	} else {
		pthread_mutex_lock(&mutex_queA);
		queA.push(fm);
		pthread_mutex_unlock(&mutex_queA);
	}
	setFocus();
}

void selectDSP()
{
	if (btnDSP->label()[0] == selrig->lo_label[0]) {
		btnDSP->label(selrig->hi_label);
		btnDSP->redraw_label();
		opDSP_hi->show();
		opDSP_lo->hide();
	} else {
		btnDSP->label(selrig->lo_label);
		btnDSP->redraw_label();
		opDSP_lo->show();
		opDSP_hi->hide();
	}
}

// set_bandwidth_control updates iBW and then posts the call for
// the UI thread to updateBandwidthControl
// changes to the UI cannot come from any thread other than the
// main FL thread!  Doing otherwise can cause what appears to be
// random program crashes.

void set_bandwidth_control()
{
	if (!selrig->has_bandwidth_control) return;

	selrig->adjust_bandwidth(vfo.imode);

	if (vfo.iBW < 256) {
		int i = 0;
		while (selrig->bandwidths_[i]) i++;
		i--;
		if (vfo.iBW > i) vfo.iBW = selrig->def_bandwidth(vfo.imode);
	}
	useB ? vfoB.iBW = vfo.iBW : vfoA.iBW = vfo.iBW;
	Fl::awake(updateBandwidthControl);
}

void updateBandwidthControl(void *d)
{
	if (selrig->has_bandwidth_control) {
		if (selrig->adjust_bandwidth(vfo.imode) != -1) {
			opBW->clear();
			rigbws_.clear();
			for (int i = 0; selrig->bandwidths_[i] != NULL; i++) {
				rigbws_.push_back(selrig->bandwidths_[i]);
				opBW->add(selrig->bandwidths_[i]);
			}

			if (selrig->has_dsp_controls) {
				opDSP_lo->clear();
				opDSP_hi->clear();
				for (int i = 0; selrig->dsp_lo[i] != NULL; i++)
					opDSP_lo->add(selrig->dsp_lo[i]);
				for (int i = 0; selrig->dsp_hi[i] != NULL; i++)
					opDSP_hi->add(selrig->dsp_hi[i]);
				if (vfo.iBW > 256) {
					opDSP_lo->index(vfo.iBW & 0xFF);
					opDSP_hi->index((vfo.iBW >> 8) & 0x7F);
					opBW->index(0);
					opBW->hide();
					btnDSP->label(selrig->hi_label);
					btnDSP->show();
					opDSP_hi->show();
					opDSP_lo->hide();
					opBW->hide();
				} else {
					opBW->index(vfo.iBW);
					opBW->show();
					opDSP_lo->hide();
					opDSP_hi->hide();
					btnDSP->hide();
				}
			} else {  // no DSP control so update BW control, hide DSP
				opBW->index(vfo.iBW);
				opBW->show();
				opDSP_lo->hide();
				opDSP_hi->hide();
				btnDSP->hide();
			}
		}
	} else { // no BW, no DSP controls
		opBW->index(0);
		opBW->hide();
		opDSP_lo->index(0);
		opDSP_hi->index(0);
		btnDSP->hide();
		opDSP_lo->hide();
		opDSP_hi->hide();
	}
}

void setMode()
{
	FREQMODE fm = vfo;
	fm.imode = opMODE->index();
	fm.iBW = selrig->def_bandwidth(fm.imode);
	fm.src = UI;
	if (useB) {
		pthread_mutex_lock(&mutex_queB);
		queB.push(fm);
		pthread_mutex_unlock(&mutex_queB);
	} else {
		pthread_mutex_lock(&mutex_queA);
		queA.push(fm);
		pthread_mutex_unlock(&mutex_queA);
	}
	setFocus();
}

void sortList() {
	if (!numinlist) return;
	ATAG_FREQMODE temp;
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
	if (!numinlist) return;
	sortList();
	FreqSelect->clear();
// stripe lines
	int bg1, bg2, bg_clr;
	// bg1 = FL_WHITE; bg2 = FL_LIGHT3;
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
	FREQMODE vfo = vfoA;
	vfo.freq = FreqDispA->value();
	vfo.src = UI;
	pthread_mutex_lock(&mutex_queA);
	queA.push(vfo);
	pthread_mutex_unlock(&mutex_queA);
	return 1;
}

int movFreqB() {
	if (progStatus.split && (rig_nbr >= IC703 && rig_nbr <= IC910H))
		return 0; // disallow for ICOM transceivers
	FREQMODE vfo = vfoB;
	vfo.freq = FreqDispB->value();
	vfo.src = UI;
	pthread_mutex_lock(&mutex_queB);
	queB.push(vfo);
	pthread_mutex_unlock(&mutex_queB);
	return 1;
}

void cbA2B()
{
	if (rig_nbr == K3) {
		K3_A2B();
		return;
	}
	if (rig_nbr == K2) {
		pthread_mutex_lock(&mutex_serial);
		vfoB = vfoA;
		selrig->set_vfoB(vfoB.freq);
		FreqDispB->value(vfoB.freq);
		pthread_mutex_unlock(&mutex_serial);
		setFocus();
		return;
	}
	if (selrig->has_a2b) {
		pthread_mutex_lock(&mutex_serial);
			selrig->A2B();
		pthread_mutex_unlock(&mutex_serial);
	}
	vfoB = vfoA;
	pthread_mutex_lock(&mutex_queB);
	queB.push(vfoB);
	pthread_mutex_unlock(&mutex_queB);
	FreqDispB->value(vfoB.freq);
	FreqDispB->redraw();
	pushedB = true;
	setFocus();
}

void cb_set_split(int val)
{
	progStatus.split = val;

	if (selrig->has_split_AB) {
		pthread_mutex_lock(&mutex_serial);
			selrig->set_split(val);
		pthread_mutex_unlock(&mutex_serial);
	} else if (val) {
		if (useB) {
			if (vfoB.freq != FreqDispB->value()) {
				vfoB.freq = FreqDispB->value();
				pthread_mutex_lock(&mutex_serial);
					selrig->selectB();
					selrig->set_vfoB(vfoB.freq);
					selrig->selectA();
				pthread_mutex_unlock(&mutex_serial);
			}
			btnA->value(1);
			btnB->value(0);
			cb_selectA();
		}
	} else
		cb_selectA();

	setFocus();
}

void highlight_vfo(void *d)
{
	Fl_Color norm_fg = fl_rgb_color(progStatus.fg_red, progStatus.fg_green, progStatus.fg_blue);
	Fl_Color norm_bg = fl_rgb_color(progStatus.bg_red, progStatus.bg_green, progStatus.bg_blue);
	Fl_Color dim_bg = fl_color_average( norm_bg, FL_BLACK, 0.87);
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
	Fl::flush();
	setFocus();
}

void cb_selectA() {
	if (progStatus.split) {
		btnSplit->value(0);
		if (!selrig->has_split_AB)
			cb_set_split(0);
	}
	pthread_mutex_lock(&mutex_serial);
	changed_vfo = true;
	vfoA.src = UI;
	vfoA.freq = FreqDispA->value();
	pthread_mutex_lock(&mutex_queA);
	queA.push(vfoA);
	useB = false;
	pthread_mutex_unlock(&mutex_queA);
	pthread_mutex_unlock(&mutex_serial);
	highlight_vfo((void *)0);
}

void cb_selectB() {
	if (progStatus.split) {
		btnSplit->value(0);
		if (!selrig->has_split_AB)
			cb_set_split(0);
	}
	pthread_mutex_lock(&mutex_serial);
	changed_vfo = true;
	vfoB.src = UI;
	vfoB.freq = FreqDispB->value();
	pthread_mutex_lock(&mutex_queB);
	queB.push(vfoB);
	useB = true;
	pthread_mutex_unlock(&mutex_queB);
	pthread_mutex_unlock(&mutex_serial);
	highlight_vfo((void *)0);
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
	FREQMODE fm;
	fm.freq  = oplist[n].freq;
	fm.imode = oplist[n].imode;
	fm.iBW   = oplist[n].iBW;
	fm.src   = UI;
	if (!useB) {
		FreqDispA->value(fm.freq);
		pthread_mutex_lock(&mutex_queA);
		queA.push(fm);
		pthread_mutex_unlock(&mutex_queA);
	} else {
		FreqDispB->value(fm.freq);
		pthread_mutex_lock(&mutex_queB);
		queB.push(fm);
		pthread_mutex_unlock(&mutex_queB);
	}
	setFocus();
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
		oplist[numinlist - 1].imode = USB;
		oplist[numinlist - 1].freq = 0;
		oplist[numinlist - 1].iBW = 0;
		memset(oplist[numinlist - 1].alpha_tag, 0, ATAGSIZE);
		numinlist--;
		updateSelect();
	}
	setFocus();
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
		setFocus();
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
		setFocus();
	}
}

void cbRIT()
{
	pthread_mutex_lock(&mutex_serial);
	if (selrig->has_rit)
		selrig->setRit((int)cntRIT->value());
	pthread_mutex_unlock(&mutex_serial);
	setFocus();
}

void cbXIT()
{
	pthread_mutex_lock(&mutex_serial);
	selrig->setXit((int)cntXIT->value());
	pthread_mutex_unlock(&mutex_serial);
	setFocus();
}

void cbBFO()
{
	if (selrig->has_bfo)
	pthread_mutex_lock(&mutex_serial);
		selrig->setBfo((int)cntBFO->value());
	pthread_mutex_unlock(&mutex_serial);
	setFocus();
}

void cbAttenuator()
{
	pthread_mutex_lock(&mutex_serial);
		progStatus.attenuator = btnAttenuator->value();
		selrig->set_attenuator(progStatus.attenuator);
	pthread_mutex_unlock(&mutex_serial);
	setFocus();
}

void setAttControl(void *d)
{
	int val = (long)d;
	btnAttenuator->value(val);
}

void cbPreamp()
{
	pthread_mutex_lock(&mutex_serial);
		progStatus.preamp = btnPreamp->value();
		selrig->set_preamp(progStatus.preamp);
	pthread_mutex_unlock(&mutex_serial);
	setFocus();
}

void setPreampControl(void *d)
{
	int val = (long)d;
	btnPreamp->value(val);
}

void cbNoise()
{
	progStatus.noise = btnNOISE->value();
	pthread_mutex_lock(&mutex_serial);
		selrig->set_noise(progStatus.noise);
	pthread_mutex_unlock(&mutex_serial);
	setFocus();
}

void cbNR()
{
	progStatus.noise_reduction = btnNR->value();
	pthread_mutex_lock(&mutex_serial);
		selrig->set_noise_reduction(progStatus.noise_reduction);
	pthread_mutex_unlock(&mutex_serial);
	setFocus();
}

void setNR()
{
	progStatus.noise_reduction_val = sldrNR->value();
	pthread_mutex_lock(&mutex_serial);
		selrig->set_noise_reduction_val(progStatus.noise_reduction_val);
	pthread_mutex_unlock(&mutex_serial);
	setFocus();
}

void cbbtnNotch()
{
	pthread_mutex_lock(&mutex_serial);
	if (btnNotch->value() == 0) {
		selrig->set_notch(false, 0);
	} else {
		selrig->set_notch(true, (int)floor(sldrNOTCH->value()));
	}
	pthread_mutex_unlock(&mutex_serial);
	try {
		send_new_notch(progStatus.notch ? progStatus.notch_val : 0);
	} catch (...) {}
	setFocus();
}

void cbAN()
{
	progStatus.auto_notch = btnAutoNotch->value();
	pthread_mutex_lock(&mutex_serial);
		selrig->set_auto_notch(progStatus.auto_notch);
	pthread_mutex_unlock(&mutex_serial);
	setFocus();
}

void setNotchControl(void *d)
{
	int val = (long)d;
	if (val) {
		progStatus.notch_val = val;
		progStatus.notch = true;
	} else
		progStatus.notch = false;

	pthread_mutex_lock(&mutex_serial);
		selrig->set_notch(progStatus.notch, progStatus.notch_val);
	pthread_mutex_unlock(&mutex_serial);

	sldrNOTCH->value(progStatus.notch_val);
	btnNotch->value(progStatus.notch);
}

void setNotch()
{
	if (btnNotch->value() || selrig->allow_notch_changes) {
		progStatus.notch = btnNotch->value();
		progStatus.notch_val = sldrNOTCH->value();
		pthread_mutex_lock(&mutex_serial);
			selrig->set_notch(progStatus.notch, progStatus.notch_val);
		pthread_mutex_unlock(&mutex_serial);
		try {
			send_new_notch(progStatus.notch ? progStatus.notch_val : 0);
		} catch (...) {}
	}
}

void adjust_if_shift_control(void *d)
{
	sldrIFSHIFT->minimum(selrig->if_shift_min);
	sldrIFSHIFT->maximum(selrig->if_shift_max);
	sldrIFSHIFT->step(selrig->if_shift_step);
	sldrIFSHIFT->value(selrig->if_shift_mid);
	sldrIFSHIFT->redraw();
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
		sldrIFSHIFT->value( selrig->if_shift_mid );
	}
}

void setIFshiftControl(void *d)
{
	int val = (long)d;
	if (sldrIFSHIFT->value() != val)
		sldrIFSHIFT->value(val);
	btnIFsh->value( val != selrig->if_shift_mid );
}

void setIFshift()
{
	int val = sldrIFSHIFT->value();
	btnIFsh->value( val != selrig->if_shift_mid );
	progStatus.shift_val = sldrIFSHIFT->value();
	progStatus.shift = btnIFsh->value();
	pthread_mutex_lock(&mutex_serial);
		selrig->set_if_shift(val);
	pthread_mutex_unlock(&mutex_serial);
}

void cbIFsh()
{
	if (sldrIFSHIFT->value() == selrig->if_shift_mid)
		btnIFsh->value(0);
	if (!btnIFsh->value()) {
		if (rig_nbr == K3) selrig->get_if_mid();
		sldrIFSHIFT->value(selrig->if_shift_mid);
	}
	progStatus.shift_val = sldrIFSHIFT->value();
	progStatus.shift = btnIFsh->value();
	pthread_mutex_lock(&mutex_serial);
		selrig->set_if_shift(sldrIFSHIFT->value());
	pthread_mutex_unlock(&mutex_serial);
	setFocus();
}

void cbEventLog()
{
	debug::show();
	setFocus();
}

void setVolume()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_volume_control(sldrVOLUME->value());
		progStatus.volume = sldrVOLUME->value();
	pthread_mutex_unlock(&mutex_serial);
}

void cbMute()
{
	if (btnVol->value() == 0) {
		pthread_mutex_lock(&mutex_serial);
			sldrVOLUME->deactivate();
			selrig->set_volume_control(0);
		pthread_mutex_unlock(&mutex_serial);
	} else {
		pthread_mutex_lock(&mutex_serial);
			selrig->set_volume_control(progStatus.volume);
			sldrVOLUME->activate();
		pthread_mutex_unlock(&mutex_serial);
	}
	setFocus();
}

void setMicGain()
{
	pthread_mutex_lock(&mutex_serial);
		progStatus.mic_gain = sldrMICGAIN->value();
		selrig->set_mic_gain(sldrMICGAIN->value());
	pthread_mutex_unlock(&mutex_serial);
}

void cbbtnMicLine()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_mic_line(btnMicLine->value());
	pthread_mutex_unlock(&mutex_serial);
	setFocus();
}

void setMicGainControl(void* d)
{
	int val = (long)d;
	sldrMICGAIN->value(val);
}

static int img = -1;

void set_power_controlImage(double pwr)
{
	if (progStatus.pwr_scale == 0 || (progStatus.pwr_scale == 4 && pwr < 26.0)) {
		if (img != 1) {
			img = 1;
			scalePower->image(image_p25);
			sldrFwdPwr->maximum(25.0);
			sldrFwdPwr->minimum(0.0);
			scalePower->redraw();
		}
	}
	else if (progStatus.pwr_scale == 1 || (progStatus.pwr_scale == 4 && pwr < 51.0)) {
		if (img != 2) {
			img = 2;
			scalePower->image(image_p50);
			sldrFwdPwr->maximum(50.0);
			sldrFwdPwr->minimum(0.0);
			scalePower->redraw();
		}
	}
	else if (progStatus.pwr_scale == 2 || (progStatus.pwr_scale == 4 && pwr < 101.0)) {
		if (img != 3) {
			img = 3;
			scalePower->image(image_p100);
			sldrFwdPwr->maximum(100.0);
			sldrFwdPwr->minimum(0.0);
			scalePower->redraw();
		}
	}
	else if (progStatus.pwr_scale >= 3) {
		if (img != 4) {
			img = 4;
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
	double pwr = sldrPOWER->value();
	pthread_mutex_lock(&mutex_serial);
		powerlevel = (int)pwr;
		progStatus.power_level = pwr;
		selrig->set_power_control(pwr);
	pthread_mutex_unlock(&mutex_serial);
	if (rig_nbr == K2) {
		double min, max, step;
		selrig->get_pc_min_max_step(min, max, step);
		sldrPOWER->minimum(min);
		sldrPOWER->maximum(max);
		sldrPOWER->step(step);
		sldrPOWER->value(progStatus.power_level);
		sldrPOWER->redraw();
	}
	set_power_controlImage(pwr);
	setFocus();
}

void cbTune()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->tune_rig();
	pthread_mutex_unlock(&mutex_serial);
	setFocus();
}

void cbPTT()
{
	if (fldigi_online && progStatus.key_fldigi)
		send_ptt_changed(btnPTT->value());
	else {
		pthread_mutex_lock(&mutex_ptt);
		quePTT.push(btnPTT->value());
		pthread_mutex_unlock(&mutex_ptt);
	}
	setFocus();
}

void setSQUELCH()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_squelch((int)sldrSQUELCH->value());
	pthread_mutex_unlock(&mutex_serial);
	setFocus();
}

void setRFGAIN()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_rf_gain((int)sldrRFGAIN->value());
	pthread_mutex_unlock(&mutex_serial);
	setFocus();
}


void updateALC(void * d)
{
	if (meter_image != ALC_IMAGE) return;
	double data = (long)d;
	sldrRcvSignal->hide();
	sldrSWR->hide();
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
	sldrFwdPwr->value(power);
	sldrFwdPwr->redraw();
	if (!selrig->has_power_control)
		set_power_controlImage(sldrFwdPwr->peak());
}

void updateSquelch(void *d)
{
	sldrSQUELCH->value((long)d);
	sldrSQUELCH->redraw();
}

void updateRFgain(void *d)
{
	sldrRFGAIN->value((long)d);
	sldrRFGAIN->redraw();
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
	if (!numinlist) {
		remove(defFileName.c_str());
		return;
	}
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
	int val = (long)d;
	pthread_mutex_lock(&mutex_ptt);
	quePTT.push(val);
	pthread_mutex_unlock(&mutex_ptt);
}


void closeRig()
{
	// restore initial rig settings
	pthread_mutex_lock(&mutex_serial);
	selrig->set_vfoA(transceiverA.freq);
	selrig->set_modeA(transceiverA.imode);
	selrig->set_bwA(transceiverA.iBW);
	selrig->set_vfoB(transceiverB.freq);
	selrig->set_modeB(transceiverB.imode);
	selrig->set_bwB(transceiverB.iBW);
	pthread_mutex_unlock(&mutex_serial);
}


void cbExit()
{
	// shutdown xmlrpc thread
	close_rig_xmlrpc();

	pthread_mutex_lock(&mutex_xmlrpc);
	run_digi_loop = false;
	pthread_mutex_unlock(&mutex_xmlrpc);
	pthread_join(*digi_thread, NULL);

	progStatus.rig_nbr = rig_nbr;

	progStatus.freq_A = vfoA.freq;
	progStatus.imode_A = vfoA.imode;
	progStatus.iBW_A = vfoA.iBW;

	progStatus.freq_B = vfoB.freq;
	progStatus.imode_B = vfoB.imode;
	progStatus.iBW_B = vfoB.iBW;

	progStatus.spkr_on = btnVol->value();
	progStatus.volume = sldrVOLUME->value();
	progStatus.power_level = sldrPOWER->value();
	progStatus.rfgain = sldrRFGAIN->value();
	progStatus.mic_gain = sldrMICGAIN->value();
	progStatus.notch = btnNotch->value();
	progStatus.notch_val = sldrNOTCH->value();
	progStatus.shift = btnIFsh->value();
	progStatus.shift_val = sldrIFSHIFT->value();
	progStatus.noise_reduction = btnNR->value();
	progStatus.noise_reduction_val = sldrNR->value();
	progStatus.noise = btnNOISE->value();
	progStatus.attenuator = btnAttenuator->value();
	progStatus.preamp = btnPreamp->value();
	progStatus.auto_notch = btnAutoNotch->value();
	progStatus.saveLastState();

	saveFreqList();

// shutdown serial thread

	pthread_mutex_lock(&mutex_serial);
		run_serial_thread = false;
	pthread_mutex_unlock(&mutex_serial);
	pthread_join(*serial_thread, NULL);

	selrig->setVfoAdj(0);
	if (progStatus.restore_rig_data){
		selrig->set_vfoA(transceiverA.freq);
		selrig->set_modeA(transceiverA.imode);
		selrig->set_bwA(transceiverA.iBW);
		selrig->set_vfoB(transceiverB.freq);
		selrig->set_modeB(transceiverB.imode);
		selrig->set_bwB(transceiverB.iBW);
	}

	selrig->shutdown();

	// xcvr auto off
	if (selrig->has_xcvr_auto_on_off)
		selrig->set_xcvr_auto_off();

	// close down the serial port
	RigSerial.ClosePort();

	// restore fldigi to its no-rig state
	try {
		send_no_rig();
	} catch (...) { }

	if (dlgDisplayConfig && dlgDisplayConfig->visible())
		dlgDisplayConfig->hide();
	if (dlgXcvrConfig && dlgXcvrConfig->visible())
		dlgXcvrConfig->hide();
	if (dlgMemoryDialog && dlgMemoryDialog->visible())
		dlgMemoryDialog->hide();
	debug::stop();

	exit(0);
}

void cbALC_SWR()
{
	if (!selrig->has_alc_control) return;
	if (meter_image == SWR_IMAGE) {
		btnALC_SWR->image(image_alc);
		meter_image = ALC_IMAGE;
		sldrALC->show();
	} else {
		btnALC_SWR->image(image_swr);
		meter_image = SWR_IMAGE;
		sldrSWR->show();
	}
	btnALC_SWR->redraw();
	setFocus();
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

void adjust_control_positions()
{
	int y = 0;

	btnVol->hide();
	sldrVOLUME->hide();
	sldrRFGAIN->hide();
	btnIFsh->hide();
	sldrIFSHIFT->hide();
	btnNotch->hide();
	sldrNOTCH->hide();
	sldrMICGAIN->hide();
	sldrPOWER->hide();
	sldrSQUELCH->hide();
	btnNR->hide();
	sldrNR->hide();

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
	} else {
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
		if (selrig->has_micgain_control) {
			y += 20;
			if (selrig->has_data_port) {
				sldrMICGAIN->label("");
				sldrMICGAIN->redraw_label();
				btnDataPort->position( 2, y);
				btnDataPort->show();
				btnDataPort->redraw();
			}
			sldrMICGAIN->resize( 54, y, 368, 18 );
			sldrMICGAIN->show();
			sldrMICGAIN->redraw();
		}
		if (selrig->has_power_control) {
			y += 20;
			sldrPOWER->resize( 54, y, 368, 18 );
			sldrPOWER->show();
			sldrPOWER->redraw();
		}
	}
	y += 20;
	btn_show_controls->position( btn_show_controls->x(), y );
	btnAttenuator->position( btnAttenuator->x(), y);
	btnAttenuator->redraw();
	btnPreamp->position( btnPreamp->x(), y);
	btnPreamp->redraw();
	btnNOISE->position( btnNOISE->x(), y);
	btnNOISE->redraw();
	btnAutoNotch->position( btnAutoNotch->x(), y);
	btnAutoNotch->redraw();
	btnTune->position( btnTune->x(), y);
	btnTune->redraw();

	if (rig_nbr == FT1000MP) {
		y -= 20;
		btnTune->position( btnTune->x(), y);
		btnTune->redraw();
		btnAutoNotch->position( btnAutoNotch->x(), y);
		btnAutoNotch->redraw();
		btnPTT->position( btnPTT->x(), y);
		btnPTT->redraw();
	}

	if (	rig_nbr == FT100D || 
			rig_nbr == FT767  ||
			rig_nbr == FT817  ||
			rig_nbr == FT847  || 
			rig_nbr == FT857D ||
			rig_nbr == FT890  ||
			rig_nbr == FT897D ||
			rig_nbr == FT920 ) {
		y -= 20;
		btnPTT->position( btnPTT->x(), y);
		btnPTT->redraw();
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

	btnInitializing->hide();

	tabs550->resize(tabs550->x(), y + 20, tabs550->w(), tabs550->h());
	tabs550->hide();

	tabsGeneric->resize(tabsGeneric->x(), y + 20, tabsGeneric->w(), tabsGeneric->h());
	tabsGeneric->hide();

	btn_show_controls->label("@-22->");
	btn_show_controls->redraw_label();

	if (selrig->has_extras)
		btn_show_controls->show();
	else
		btn_show_controls->hide();

	y += 20;

	mainwindow->size( mainwindow->w(), y);
	mainwindow->init_sizes();
	mainwindow->redraw();

	if (progStatus.tooltips) {
		Fl_Tooltip::enable(1);
		mnuTooltips->set();
	} else {
		mnuTooltips->clear();
		Fl_Tooltip::enable(0);
	}
	if (progStatus.schema)
		mnuSchema->set(); 
	else
		mnuSchema->clear();

}

void initXcvrTab()
{
	if (rig_nbr == TT550) {
		cnt_tt550_line_out->value(progStatus.tt550_line_out);
		cbo_tt550_agc_level->index(progStatus.tt550_agc_level);
		cnt_tt550_cw_wpm->value(progStatus.tt550_cw_wpm);
		cnt_tt550_cw_vol->value(progStatus.tt550_cw_vol);
		cnt_tt550_cw_spot->value(progStatus.tt550_cw_spot);
		cnt_tt550_cw_weight->value(progStatus.tt550_cw_weight);
		cnt_tt550_cw_qsk->value(progStatus.tt550_cw_qsk);
		btn_tt550_enable_keyer->value(progStatus.tt550_enable_keyer);
		btn_tt550_vox->value(progStatus.tt550_vox_onoff);
		cnt_tt550_vox_gain->value(progStatus.tt550_vox_gain);
		cnt_tt550_anti_vox->value(progStatus.tt550_vox_anti);
		cnt_tt550_vox_hang->value(progStatus.tt550_vox_hang);
		btn_tt550_CompON->value(progStatus.tt550_compON);
		cnt_tt550_compression->value(progStatus.tt550_compression);
		cnt_tt550_mon_vol->value(progStatus.tt550_mon_vol);
		btn_tt550_enable_xmtr->value(progStatus.tt550_enable_xmtr);
		btn_tt550_enable_tloop->value(progStatus.tt550_enable_tloop);
		btn_tt550_tuner_bypass->value(progStatus.tt550_tuner_bypass);
		btn_tt550_use_xmt_bw->value(progStatus.tt550_use_xmt_bw);
		sel_tt550_encoder_step->value(progStatus.tt550_encoder_step);
		cnt_tt550_encoder_sensitivity->value(progStatus.tt550_encoder_sensitivity);
		sel_tt550_F1_func->value(progStatus.tt550_F1_func);
		sel_tt550_F2_func->value(progStatus.tt550_F2_func);
		sel_tt550_F3_func->value(progStatus.tt550_F3_func);
		mnuRestoreData->clear();
		mnuRestoreData->hide();
		mnuKeepData->clear();
		mnuKeepData->hide();
		progStatus.restore_rig_data = false;
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

	} else {

		tabsGeneric->remove(*genericBands);
		tabsGeneric->remove(*genericCW);
		tabsGeneric->remove(*genericVOX);
		tabsGeneric->remove(*genericSpeech);
		tabsGeneric->remove(*genericRx);
		tabsGeneric->remove(*genericMisc);

		if (selrig->has_band_selection) {
			genericBands->resize(
				tabsGeneric->x() + 2,
				tabsGeneric->y() + 19,
				tabsGeneric->w() - 4,
				tabsGeneric->h() - 21);
			tabsGeneric->insert(*genericBands, 0);
			btnBandSelect_12->hide();
		}

		poll_all->activate();
		poll_all->value(progStatus.poll_all);

		if (selrig->has_cw_wpm || selrig->has_cw_qsk || selrig->has_cw_weight ||
			selrig->has_cw_keyer || selrig->has_cw_spot || selrig->has_cw_spot_tone) {
			genericCW->resize(
				tabsGeneric->x() + 2,
				tabsGeneric->y() + 19,
				tabsGeneric->w() - 4,
				tabsGeneric->h() - 21);
			tabsGeneric->insert(*genericCW, 6);
			if (selrig->has_cw_wpm) {
				int min, max;
				selrig->get_cw_wpm_min_max(min, max);
				cnt_cw_wpm->minimum(min);
				cnt_cw_wpm->maximum(max);
				cnt_cw_wpm->value(progStatus.cw_wpm);
				cnt_cw_wpm->show();
				selrig->set_cw_wpm();
			} else cnt_cw_wpm->hide();
			if (selrig->has_cw_qsk) {
				double min, max, step;
				selrig->get_cw_qsk_min_max_step(min, max, step);
				cnt_cw_qsk->minimum(min);
				cnt_cw_qsk->maximum(max);
				cnt_cw_qsk->step(step);
				cnt_cw_qsk->value(progStatus.cw_qsk);
				cnt_cw_qsk->show();
				selrig->set_cw_qsk();
			} else cnt_cw_qsk->hide();
			if (selrig->has_cw_weight) {
				double min, max, step;
				selrig->get_cw_weight_min_max_step( min, max, step );
				cnt_cw_weight->minimum(min);
				cnt_cw_weight->maximum(max);
				cnt_cw_weight->step(step);
				cnt_cw_weight->value(progStatus.cw_weight);
				cnt_cw_weight->show();
				selrig->set_cw_weight();
			} else cnt_cw_weight->hide();
			if (selrig->has_cw_keyer) {
				btn_enable_keyer->show();
				btn_enable_keyer->value(progStatus.enable_keyer);
				selrig->enable_keyer();
			} else btn_enable_keyer->hide();
			if (selrig->has_cw_spot) {
				btnSpot->value(progStatus.cw_spot);
				selrig->set_cw_spot();
				btnSpot->show();
			} else btnSpot->hide();
			if (selrig->has_cw_spot_tone) {
				cnt_cw_spot_tone->show();
				int min, max, step;
				selrig->get_cw_spot_tone_min_max_step(min, max, step);
				cnt_cw_spot_tone->minimum(min);
				cnt_cw_spot_tone->maximum(max);
				cnt_cw_spot_tone->step(step);
				cnt_cw_spot_tone->value(progStatus.cw_spot_tone);
				selrig->set_cw_spot_tone();
			} else cnt_cw_spot_tone->hide();
		}

		if (selrig->has_vox_onoff || selrig->has_vox_gain || selrig->has_vox_anti ||
			selrig->has_vox_hang || selrig->has_vox_on_dataport || selrig->has_cw_spot_tone) {
			genericVOX->resize(
				tabsGeneric->x() + 2,
				tabsGeneric->y() + 19,
				tabsGeneric->w() - 4,
				tabsGeneric->h() - 21);
			tabsGeneric->insert(*genericVOX, 6);
			if (selrig->has_vox_onoff) {
				btn_vox->show();
				btn_vox->value(progStatus.vox_onoff);
				selrig->set_vox_onoff();
			} else btn_vox->hide();
			if (selrig->has_vox_gain) {
				cnt_vox_gain->show(); 
				int min, max, step;
				selrig->get_vox_gain_min_max_step(min, max, step);
				cnt_vox_gain->minimum(min);
				cnt_vox_gain->maximum(max);
				cnt_vox_gain->step(step);
				cnt_vox_gain->value(progStatus.vox_gain);
				selrig->set_vox_gain();
			} else cnt_vox_gain->hide();
			if (selrig->has_vox_anti) {
				cnt_anti_vox->show();
				int min, max, step;
				selrig->get_vox_anti_min_max_step(min, max, step);
				cnt_anti_vox->minimum(min);
				cnt_anti_vox->maximum(max);
				cnt_anti_vox->step(step);
				cnt_anti_vox->value(progStatus.vox_anti);
				selrig->set_vox_anti();
			} else cnt_anti_vox->hide();
			if (selrig->has_vox_hang) {
				cnt_vox_hang->show();
				int min, max, step;
				selrig->get_vox_hang_min_max_step(min, max, step);
				cnt_vox_hang->minimum(min);
				cnt_vox_hang->maximum(max);
				cnt_vox_hang->step(step);
				cnt_vox_hang->value(progStatus.vox_hang);
				selrig->set_vox_hang();
			} else cnt_vox_hang->hide();
			if (selrig->has_vox_on_dataport) {
				btn_vox_on_dataport->show();
				btn_vox_on_dataport->value(progStatus.vox_on_dataport);
				selrig->set_vox_on_dataport();
			} else btn_vox_on_dataport->hide();
		}

		if (selrig->has_compON || selrig->has_compression) {
			genericSpeech->resize(
				tabsGeneric->x() + 2,
				tabsGeneric->y() + 19,
				tabsGeneric->w() - 4,
				tabsGeneric->h() - 21);
			tabsGeneric->insert(*genericSpeech, 6);
			if (selrig->has_compON) {
				btnCompON->show();
				btnCompON->value(progStatus.compON);
			} else
				btnCompON->hide();
			if (selrig->has_compression) {
				int min, max, step;
				selrig->get_comp_min_max_step(min, max, step);
				cnt_compression->minimum(min);
				cnt_compression->maximum(max);
				cnt_compression->step(step);
				cnt_compression->show();
				cnt_compression->value(progStatus.compression);
				selrig->set_compression();
			} else
				cnt_compression->hide();
		}
		if (selrig->has_agc_level || selrig->has_nb_level || selrig->has_bpf_center) {
			genericRx->resize(
				tabsGeneric->x() + 2,
				tabsGeneric->y() + 19,
				tabsGeneric->w() - 4,
				tabsGeneric->h() - 21);
			tabsGeneric->insert(*genericRx, 6);
			if (selrig->has_agc_level) cbo_agc_level->activate(); else cbo_agc_level->deactivate();
			if (selrig->has_nb_level) cbo_nb_level->activate(); else cbo_nb_level->deactivate();
			if (selrig->has_bpf_center) {
				cnt_bpf_center->value(progStatus.bpf_center);
				cnt_bpf_center->show();
				btn_use_bpf_center->show();
			} else {
				cnt_bpf_center->hide();
				btn_use_bpf_center->hide();
			}
		}

		if (selrig->has_vfo_adj || selrig->has_xcvr_auto_on_off) {
			genericMisc->resize(
				tabsGeneric->x() + 2,
				tabsGeneric->y() + 19,
				tabsGeneric->w() - 4,
				tabsGeneric->h() - 21);
			tabsGeneric->insert(*genericMisc, 6);
			if (selrig->has_vfo_adj) {
				cnt_vfo_adj->value(progStatus.vfo_adj);
				selrig->setVfoAdj(progStatus.vfo_adj);
				cnt_vfo_adj->show();
			} else
				cnt_vfo_adj->hide();
			cnt_line_out->hide(); // enable if a lineout control is used by any transceiver

			if (selrig->has_xcvr_auto_on_off) {
				btn_xcvr_auto_on->value(progStatus.xcvr_auto_on);
				btn_xcvr_auto_off->value(progStatus.xcvr_auto_off);
				btn_xcvr_auto_on->show();
				btn_xcvr_auto_off->show();
			} else {
				btn_xcvr_auto_on->hide();
				btn_xcvr_auto_off->hide();
			}

		}

		tabsGeneric->redraw();

		mnuRestoreData->show();
		mnuKeepData->show();

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

		if (!selrig->has_bandwidth_control) { poll_bandwidth->deactivate(); poll_bandwidth->value(0); }
		if (!selrig->has_smeter) { poll_smeter->deactivate(); poll_smeter->value(0); }
		if (!selrig->has_power_out) { poll_pout->deactivate(); poll_pout->value(0); }
		if (!selrig->has_swr_control) { poll_swr->deactivate(); poll_swr->value(0); }
		if (!selrig->has_alc_control) { poll_alc->deactivate(); poll_alc->value(0); }
		if (!selrig->has_volume_control) { poll_volume->deactivate(); poll_volume->value(0); }
		if (!selrig->has_notch_control) { poll_notch->deactivate(); poll_notch->value(0); }
		if (!selrig->has_auto_notch || rig_nbr == FT1000MP) 
			{ poll_auto_notch->deactivate(); poll_auto_notch->value(0); }
		if (!selrig->has_ifshift_control) { poll_ifshift->deactivate(); poll_ifshift->value(0); }
		if (!selrig->has_power_control) { poll_power_control->deactivate(); poll_power_control->value(0); }
		if (!selrig->has_preamp_control && !selrig->has_attenuator_control) 
			{ poll_pre_att->deactivate(); poll_pre_att->value(0); }
		if (!selrig->has_sql_control) { poll_squelch->deactivate(); poll_squelch->value(0); }
		if (!selrig->has_micgain_control) { poll_micgain->deactivate(); poll_micgain->value(0); }
		if (!selrig->has_rf_control) { poll_rfgain->deactivate(); poll_rfgain->value(0); }
		if (!selrig->has_split) { poll_split->deactivate(); poll_split->value(0); }
		if (!selrig->has_noise_control) {poll_noise->deactivate(); poll_noise->value(0);}
		if (!selrig->has_noise_reduction) {poll_nr->deactivate(); poll_nr->value(0);}

	}
}

void initRig()
{
	flrig_abort = false;

	sldrRcvSignal->aging(progStatus.rx_peak);
	sldrRcvSignal->avg(progStatus.rx_avg);
	sldrFwdPwr->aging(progStatus.pwr_peak);
	sldrFwdPwr->avg(progStatus.pwr_avg);

//	wait_query = true;

// disable xml loop
	pthread_mutex_lock(&mutex_xmlrpc);

// disable the serial thread
	pthread_mutex_lock(&mutex_serial);

// Xcvr Auto Power on as soon as possible
	if (selrig->has_xcvr_auto_on_off)
		selrig->set_xcvr_auto_on();

	selrig->initialize();
	if (flrig_abort) goto failed;

	FreqDispA->set_precision(selrig->precision);
	FreqDispB->set_precision(selrig->precision);

	if (rig_nbr == TT550) {
//		selrig->selectB();  // not necessary for 550 as there are no xcvr memories
		selrig->selectA();

		vfoB.freq = progStatus.freq_B;
		vfoB.imode = progStatus.imode_B;
		vfoB.iBW = progStatus.iBW_B;
		FreqDispB->value(vfoB.freq);
		selrig->set_vfoB(vfoB.freq);
		selrig->set_modeB(vfoB.imode);
		selrig->set_bwB(vfoB.iBW);

		vfo.freq = vfoA.freq = progStatus.freq_A;
		vfo.imode = vfoA.imode = progStatus.imode_A;
		vfo.iBW = vfoA.iBW = progStatus.iBW_A;
		FreqDispA->value( vfoA.freq );
		selrig->set_vfoA(vfoA.freq);
		selrig->set_modeA(vfoA.imode);

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

	} else { // !TT550
		if (progStatus.CIV > 0)
			selrig->adjustCIV(progStatus.CIV);
		if (progStatus.use_rig_data) {
			selrig->selectA();
			if (selrig->has_get_info)
				selrig->get_info();
			transceiverA.freq = selrig->get_vfoA();
			transceiverA.imode = selrig->get_modeA();
			selrig->adjust_bandwidth(transceiverA.imode);
			transceiverA.iBW = selrig->get_bwA();
			selrig->selectB();
			if (selrig->has_get_info)
				selrig->get_info();
			transceiverB.freq = selrig->get_vfoB();
			transceiverB.imode = selrig->get_modeB();
			selrig->adjust_bandwidth(transceiverB.imode);
			transceiverB.iBW = selrig->get_bwB();
			progStatus.freq_A = transceiverA.freq;
			progStatus.imode_A = transceiverA.imode;
			progStatus.iBW_A = transceiverA.iBW;
			progStatus.freq_B = transceiverB.freq;
			progStatus.imode_B = transceiverB.imode;
			progStatus.iBW_B = transceiverB.iBW;
			mnuKeepData->set();
			if (selrig->restore_mbw) selrig->last_bw = transceiverA.iBW;
		} else
			mnuKeepData->clear();

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
	} // !TT550

	if (selrig->has_special)
		btnSpecial->show();
	else
		btnSpecial->hide();

	if (selrig->has_rit) {
		int min, max, step;
		selrig->get_RIT_min_max_step(min, max, step);
		cntRIT->minimum(min);
		cntRIT->maximum(max);
		cntRIT->step(step);
		cntRIT->show();
		cntRIT->value(progStatus.rit_freq);
	} else {
		cntRIT->hide();
	}

	if (selrig->has_xit) {
		int min, max, step;
		selrig->get_XIT_min_max_step(min, max, step);
		cntXIT->minimum(min);
		cntXIT->maximum(max);
		cntXIT->step(step);
		cntXIT->value(progStatus.xit_freq);
		cntXIT->show();
	} else {
		cntXIT->hide();
	}

	if (selrig->has_bfo) {
		int min, max, step;
		selrig->get_BFO_min_max_step(min, max, step);
		cntBFO->minimum(min);
		cntBFO->maximum(max);
		cntBFO->step(step);
		cntBFO->value(progStatus.bfo_freq);
		cntBFO->show();
	} else {
		cntBFO->hide();
	}

	if (selrig->has_dsp_controls) {
		opDSP_lo->clear();
		opDSP_hi->clear();
		btnDSP->label(selrig->lo_label);
		btnDSP->redraw_label();
		for (int i = 0; selrig->dsp_lo[i] != NULL; i++)
			opDSP_lo->add(selrig->dsp_lo[i]);
		opDSP_lo->tooltip(selrig->lo_tooltip);
		for (int i = 0; selrig->dsp_hi[i] != NULL; i++)
			opDSP_hi->add(selrig->dsp_hi[i]);
		opDSP_hi->tooltip(selrig->hi_tooltip);
		if (vfo.iBW > 256) {
			opDSP_lo->index(vfo.iBW & 0xFF);
			opDSP_hi->index((vfo.iBW >> 8) & 0x7F);
			btnDSP->show();
			opDSP_hi->show();
			opDSP_lo->hide();
			opBW->hide();
			opBW->index(0);
		} else {
			opDSP_lo->index(0);
			opDSP_hi->index(0);
			btnDSP->hide();
			opDSP_lo->hide();
			opDSP_hi->hide();
			opBW->show();
		}
	} else {
		btnDSP->hide();
		opDSP_lo->hide();
		opDSP_hi->hide();
		opBW->show();
	}

	if (selrig->has_volume_control) {
		int min, max, step;
		selrig->get_vol_min_max_step(min, max, step);
		sldrVOLUME->minimum(min);
		sldrVOLUME->maximum(max);
		sldrVOLUME->step(step);
		sldrVOLUME->redraw();
		if (progStatus.use_rig_data) {
			progStatus.volume = selrig->get_volume_control();
			sldrVOLUME->value(progStatus.volume);
			btnVol->value(1);
			sldrVOLUME->activate();
		} else {
			sldrVOLUME->value(progStatus.volume);
			if (progStatus.spkr_on == 0) {
				btnVol->value(0);
				sldrVOLUME->deactivate();
				selrig->set_volume_control(0);
			} else {
				btnVol->value(1);
				sldrVOLUME->activate();
				selrig->set_volume_control(progStatus.volume);
			}
		}
		btnVol->show();
		sldrVOLUME->show();
	} else {
		btnVol->hide();
		sldrVOLUME->hide();
	}

	if (selrig->has_rf_control) {
		int min, max, step;
		selrig->get_rf_min_max_step(min, max, step);
		sldrRFGAIN->minimum(min);
		sldrRFGAIN->maximum(max);
		sldrRFGAIN->step(step);
		sldrRFGAIN->redraw();
		if (progStatus.use_rig_data) {
			progStatus.rfgain = selrig->get_rf_gain();
			sldrRFGAIN->value(progStatus.rfgain);
			sldrRFGAIN->show();
		} else {
			sldrRFGAIN->value(progStatus.rfgain);
			selrig->set_rf_gain(progStatus.rfgain);
			sldrRFGAIN->show();
		}
	} else {
		sldrRFGAIN->hide();
	}

	if (selrig->has_sql_control) {
		int min, max, step;
		selrig->get_squelch_min_max_step(min, max, step);
		sldrSQUELCH->minimum(min);
		sldrSQUELCH->maximum(max);
		sldrSQUELCH->step(step);
		sldrSQUELCH->redraw();
		if (progStatus.use_rig_data) {
			progStatus.squelch = selrig->get_squelch();
			sldrSQUELCH->value(progStatus.squelch);
			sldrSQUELCH->show();
		} else {
			sldrSQUELCH->value(progStatus.squelch);
			selrig->set_squelch(progStatus.squelch);
			sldrSQUELCH->show();
		}
	} else {
		sldrSQUELCH->hide();
	}

	if (selrig->has_noise_reduction_control) {
		int min, max, step;
		selrig->get_nr_min_max_step(min, max, step);
		sldrNR->minimum(min);
		sldrNR->maximum(max);
		sldrNR->step(step);
		sldrNR->redraw();
		btnNR->show();
		sldrNR->show();
		if (progStatus.use_rig_data) {
			progStatus.noise_reduction = selrig->get_noise_reduction();
			progStatus.noise_reduction_val = selrig->get_noise_reduction_val();
			btnNR->value(progStatus.noise_reduction);
			sldrNR->value(progStatus.noise_reduction_val);
		} else {
			btnNR->value(progStatus.noise_reduction);
			sldrNR->value(progStatus.noise_reduction_val);
			selrig->set_noise_reduction(progStatus.noise_reduction);
			selrig->set_noise_reduction_val(progStatus.noise_reduction_val);
		}
	} else {
		btnNR->hide();
		sldrNR->hide();
	}

	if (selrig->has_ifshift_control) {
		int min, max, step;
		selrig->get_if_min_max_step(min, max, step);
		sldrIFSHIFT->minimum(min);
		sldrIFSHIFT->maximum(max);
		sldrIFSHIFT->step(step);
		sldrIFSHIFT->redraw();
		if (progStatus.use_rig_data) {
			progStatus.shift = selrig->get_if_shift(progStatus.shift_val);
			btnIFsh->value(progStatus.shift);
			sldrIFSHIFT->value(progStatus.shift_val);
		} else {
			if (progStatus.shift) {
				btnIFsh->value(1);
				sldrIFSHIFT->value(progStatus.shift_val);
				selrig->set_if_shift(progStatus.shift_val);
			} else {
				btnIFsh->value(0);
				sldrIFSHIFT->value(selrig->if_shift_mid);
				selrig->set_if_shift(selrig->if_shift_mid);
			}
		}
		btnIFsh->show();
		sldrIFSHIFT->show();
	} else {
		btnIFsh->hide();
		sldrIFSHIFT->hide();
	}

	if (selrig->has_notch_control) {
		int min, max, step;
		selrig->get_notch_min_max_step(min, max, step);
		sldrNOTCH->minimum(min);
		sldrNOTCH->maximum(max);
		sldrNOTCH->step(step);
		sldrNOTCH->redraw();
		if (progStatus.use_rig_data) {
			progStatus.notch = selrig->get_notch(progStatus.notch_val);
			btnNotch->value(progStatus.notch);
			sldrNOTCH->value(progStatus.notch_val);
		} else {
			btnNotch->value(progStatus.notch);
			sldrNOTCH->value(progStatus.notch_val);
			selrig->set_notch(progStatus.notch, progStatus.notch_val);
		}
		btnNotch->show();
		sldrNOTCH->show();
	} else {
		btnNotch->hide();
		sldrNOTCH->hide();
	}

	if (selrig->has_micgain_control) {
		int min = 0, max = 0, step = 0;
		selrig->get_mic_min_max_step(min, max, step);
		sldrMICGAIN->minimum(min);
		sldrMICGAIN->maximum(max);
		sldrMICGAIN->step(step);
		if (progStatus.use_rig_data)
			progStatus.mic_gain = selrig->get_mic_gain();
		else
			selrig->set_mic_gain(progStatus.mic_gain);
		sldrMICGAIN->value(progStatus.mic_gain);
		sldrMICGAIN->show();
		if (selrig->has_data_port) { 
			btnDataPort->show();
			btnDataPort->value(progStatus.data_port);
		} else btnDataPort->hide();
	} else {
		sldrMICGAIN->hide();
	}

	if (selrig->has_power_control) {
		double min, max, step;
		if (progStatus.use_rig_data)
			progStatus.power_level = selrig->get_power_control();
		else
			selrig->set_power_control(progStatus.power_level);
		selrig->get_pc_min_max_step(min, max, step);
		sldrPOWER->minimum(min);
		sldrPOWER->maximum(max);
		sldrPOWER->step(step);
		sldrPOWER->value(progStatus.power_level);
		sldrPOWER->redraw();
		sldrPOWER->show();
	} else {
		sldrPOWER->hide();
	}
	set_power_controlImage(progStatus.power_level);

	if (selrig->has_attenuator_control) {
		if (progStatus.use_rig_data)
			progStatus.attenuator = selrig->get_attenuator();
		else
			selrig->set_attenuator(progStatus.attenuator);
		btnAttenuator->show();
	} else {
		btnAttenuator->hide();
	}

// hijack the preamp control for a SPOT button on the TT550 Pegasus
	if (rig_nbr == TT550) {
		btnPreamp->label("Spot");
		btnPreamp->value(progStatus.tt550_spot_onoff);
		btnPreamp->show();
	} else {
		if (selrig->has_preamp_control) {
			if (progStatus.use_rig_data)
				progStatus.preamp = selrig->get_preamp();
			else
				selrig->set_preamp(progStatus.preamp);
			btnPreamp->show();
		} else {
			btnPreamp->hide();
		}
	}

	if (selrig->has_noise_control) {
		if (progStatus.use_rig_data)
			progStatus.noise = selrig->get_noise();
		else
			selrig->set_noise(progStatus.noise);
		btnNOISE->value(progStatus.noise);
		btnNOISE->show();
	}
	else {
		btnNOISE->hide();
	}

	if (selrig->has_tune_control) {
		btnTune->show();
	} else {
		btnTune->hide();
	}

	if (selrig->has_ptt_control ||
		progStatus.comm_dtrptt || progStatus.comm_rtsptt ||
		progStatus.sep_dtrptt || progStatus.sep_rtsptt) {
		btnPTT->activate();
	} else {
		btnPTT->deactivate();
	}

	if (selrig->has_auto_notch) {
		if (rig_nbr == RAY152) {
			btnAutoNotch->label("AGC");
			btnAutoNotch->tooltip("AGC on/off");
		} else if (rig_nbr == FT1000MP) {
			btnAutoNotch->label("Tuner");
			btnAutoNotch->tooltip("Tuner on/off");
		} else {
			btnAutoNotch->label("AN");
			btnAutoNotch->tooltip("Auto notch on/off");
		}
		if (progStatus.use_rig_data)
			progStatus.auto_notch = selrig->get_auto_notch();
		else
			selrig->set_auto_notch(progStatus.auto_notch);
		btnAutoNotch->value(progStatus.auto_notch);
		btnAutoNotch->show();
	} else {
		btnAutoNotch->hide();
	}

	if (selrig->has_swr_control)
		btnALC_SWR->activate();
	else {
		btnALC_SWR->deactivate();
	}

	if (selrig->has_compON || selrig->has_compression)
		selrig->set_compression();

	adjust_control_positions();
	initXcvrTab();

	buildlist();

	if (selrig->CIV) {
		char hexstr[8];
		snprintf(hexstr, sizeof(hexstr), "0x%2X", selrig->CIV);
		txtCIV->value(hexstr);
		txtCIV->activate();
		btnCIVdefault->activate();
		if (strstr(selrig->name_, "IC-7200") || strstr(selrig->name_, "IC-7600")) {
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

	if (rig_nbr != TT550) {
		vfoA.freq = progStatus.freq_A;
		vfoA.imode = progStatus.imode_A;
		vfoA.iBW = progStatus.iBW_A;

		if (vfoA.iBW == -1) vfoA.iBW = selrig->def_bandwidth(vfoA.imode);
		FreqDispA->value( vfoA.freq );

		vfoB.freq = progStatus.freq_B;
		vfoB.imode = progStatus.imode_B;
		vfoB.iBW = progStatus.iBW_B;

		if (vfoB.iBW == -1) vfoB.iBW = selrig->def_bandwidth(vfoB.imode);
		FreqDispB->value(vfoB.freq);

		selrig->selectB();
		selrig->set_vfoB(vfoB.freq);
		selrig->set_modeB(vfoB.imode);
		selrig->set_bwB(vfoB.iBW);

		selrig->selectA();
		selrig->set_vfoA(vfoA.freq);
		selrig->set_modeA(vfoA.imode);
		selrig->set_bwA(vfoA.iBW);

		vfo = vfoA;
		updateBandwidthControl();

		useB = false;
		highlight_vfo((void *)0);

	}

// enable buttons, change labels
	selrig->post_initialize();

	// enable the serial thread

	pthread_mutex_unlock(&mutex_serial);

	fldigi_online = false;
	rig_reset = true;

// initialize fldigi
	try {
		send_modes();
		send_bandwidths();
		send_new_freq(vfoA.freq);
		send_new_mode(vfoA.imode);
		send_sideband();
		send_new_bandwidth(vfoA.iBW);
	} catch (...) {}

// enable xml loop
	pthread_mutex_unlock(&mutex_xmlrpc);

	if (rig_nbr == K3) {
		btnB->hide();
		btnA->hide();
		btn_K3_swapAB->show();
	} else {
		btn_K3_swapAB->hide();
		btnB->show();
		btnA->show();
	}

	setFocus();
	bypass_serial_thread_loop = false;
	return;

failed:
	bypass_serial_thread_loop = true;
	pthread_mutex_unlock(&mutex_serial);
	pthread_mutex_unlock(&mutex_xmlrpc);
	btnInitializing->hide();
	fl_alert2(_("Transceiver not responding"));
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
	selectCommPort->index(0);

	progStatus.loadXcvrState(selrig->name_);

	selectCommPort->value(progStatus.xcvr_serial_port.c_str());
	btnOneStopBit->value( progStatus.stopbits == 1 );
	btnTwoStopBit->value( progStatus.stopbits == 2 );

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

	if (picked >= IC703 && picked <= IC910H) {
		char hexstr[8];
		snprintf(hexstr, sizeof(hexstr), "0x%2X", srig->CIV);
		txtCIV->value(hexstr);
		txtCIV->activate();
		btnCIVdefault->activate();
		if (picked == IC7200 || picked == IC7600) {
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

void initStatusConfigDialog()
{
	rig_nbr = progStatus.rig_nbr;
	selrig = rigs[rig_nbr];

	if (rig_nbr >= IC703 && rig_nbr <= IC910H)
		if (progStatus.CIV) selrig->adjustCIV(progStatus.CIV);

	selectRig->index(rig_nbr);
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

	init_title();

	if (!startXcvrSerial()) {
		if (progStatus.xcvr_serial_port.compare("NONE") == 0) {
			LOG_WARN("No comm port ... test mode");
		} else {
			LOG_WARN("%s cannot be accessed", progStatus.xcvr_serial_port.c_str());
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
			LOG_WARN("%s cannot be accessed", progStatus.aux_serial_port.c_str());
			progStatus.aux_serial_port = "NONE";
			selectAuxPort->value(progStatus.aux_serial_port.c_str());
		}
	}
	if (!startSepSerial()) {
		if (progStatus.sep_serial_port.compare("NONE") != 0) {
			LOG_WARN("%s cannot be accessed", progStatus.sep_serial_port.c_str());
			progStatus.sep_serial_port = "NONE";
			selectSepPTTPort->value(progStatus.sep_serial_port.c_str());
		}
	}

	initRig();

}

void initRigCombo()
{
	selectRig->clear();
	int i = 0;
	while (rigs[i] != NULL)
		selectRig->add(rigs[i++]->name_);

	selectRig->index(rig_nbr = 0);
}

void nb_label(const char * l, bool on = false)
{
	btnNOISE->value(on);
	btnNOISE->label(l);
	btnNOISE->redraw_label();
}

void preamp_label(const char * l, bool on = false)
{
	btnPreamp->value(on);
	btnPreamp->label(l);
	btnPreamp->redraw_label();
}

void atten_label(const char * l, bool on = false)
{
	btnAttenuator->value(on);
	btnAttenuator->label(l);
	btnAttenuator->redraw_label();
}

void auto_notch_label(const char * l, bool on = false)
{
	btnAutoNotch->value(on);
	btnAutoNotch->label(l);
	btnAutoNotch->redraw_label();
}

void cbAuxPort()
{
	AuxSerial.setRTS(progStatus.aux_rts);
	AuxSerial.setDTR(progStatus.aux_dtr);
}

void cb_agc_level()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_agc_level();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_cw_wpm()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_cw_wpm();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_cw_vol()
{
//	pthread_mutex_lock(&mutex_serial);
//		selrig->set_cw_vol();
//	pthread_mutex_unlock(&mutex_serial);
}

void cb_cw_spot()
{
	int ret;
	pthread_mutex_lock(&mutex_serial);
		ret = selrig->set_cw_spot();
	pthread_mutex_unlock(&mutex_serial);
	if (!ret) btnSpot->value(0);
}

void cb_cw_spot_tone()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_cw_spot_tone();
	pthread_mutex_unlock(&mutex_serial);
}


void cb_vox_gain()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_vox_gain();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_vox_anti()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_vox_anti();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_vox_hang()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_vox_hang();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_vox_onoff()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_vox_onoff();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_vox_on_dataport()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_vox_on_dataport();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_compression()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_compression();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_auto_notch()
{
	progStatus.auto_notch = btnAutoNotch->value();
	pthread_mutex_lock(&mutex_serial);
		selrig->set_auto_notch(progStatus.auto_notch);
	pthread_mutex_unlock(&mutex_serial);
}

void cb_vfo_adj()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->setVfoAdj(progStatus.vfo_adj);
	pthread_mutex_unlock(&mutex_serial);
}

void cb_line_out()
{
}

void cb_bpf_center()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_if_shift(selrig->pbt);
	pthread_mutex_unlock(&mutex_serial);
}

void cb_special()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_special(btnSpecial->value());
	pthread_mutex_unlock(&mutex_serial);
}

void cb_nb_level()
{
	pthread_mutex_lock(&mutex_serial);
	progStatus.nb_level = cbo_nb_level->index();
	selrig->set_nb_level();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_spot()
{
	pthread_mutex_lock(&mutex_serial);
	selrig->set_cw_spot();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_enable_keyer()
{
	pthread_mutex_lock(&mutex_serial);
	selrig->enable_keyer();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_cw_weight()
{
	pthread_mutex_lock(&mutex_serial);
	selrig->set_cw_weight();
	pthread_mutex_unlock(&mutex_serial);
}

void cb_cw_qsk()
{
	pthread_mutex_lock(&mutex_serial);
	selrig->set_cw_qsk();
	pthread_mutex_unlock(&mutex_serial);
}

void cbBandSelect(int band)
{
	pthread_mutex_lock(&mutex_serial);
	pthread_mutex_lock(&mutex_xmlrpc);
	selrig->set_band_selection(band);
// get freqmdbw
	if (!useB) {
		vfoA.freq = selrig->get_vfoA();
		if (selrig->has_mode_control)
			vfoA.imode = selrig->get_modeA();
		if (selrig->has_bandwidth_control) {
			selrig->adjust_bandwidth(vfoA.imode);
			vfoA.iBW = selrig->get_bwA();
		}
		vfo = vfoA;
	} else {
		vfoB.freq = selrig->get_vfoB();
		if (selrig->has_mode_control)
			vfoB.imode = selrig->get_modeB();
		if (selrig->has_bandwidth_control) {
			selrig->adjust_bandwidth(vfoB.imode);
			vfoB.iBW = selrig->get_bwB();
		}
		vfo = vfoB;
	}
// local display freqmdbw
	if (selrig->has_mode_control)
		Fl::awake(setModeControl);
	if (selrig->has_bandwidth_control) {
		set_bandwidth_control();
		Fl::awake(setBWControl);
	}
	if (!useB) Fl::awake(setFreqDispA, (void *)vfo.freq);
	else Fl::awake(setFreqDispB, (void *)vfo.freq);
// remote send freqmdbw
	if (fldigi_online) {
		send_xml_freq(vfo.freq);
		if (selrig->has_mode_control) {
			try {
				send_new_mode(vfo.imode);
				send_sideband();
			} catch (...) {}
		}
		if (selrig->has_bandwidth_control) {
			try {
				send_bandwidths();
				send_new_bandwidth(vfo.iBW);
			} catch (...) {}
		}
	}
	pthread_mutex_unlock(&mutex_xmlrpc);
	pthread_mutex_unlock(&mutex_serial);
}

void enable_bandselect_btn(int btn_num, bool enable)
{
	switch (btn_num) {
		case 1:
		case 9:
			break;
		case 10:	// 6m
			if (enable) btnBandSelect_10->show();
			else btnBandSelect_10->hide();
			break;
		case 11:	// GEN
			if (enable) btnBandSelect_11->show();
			else btnBandSelect_11->hide();
			break;
		case 12:	// 60m
			if (enable) btnBandSelect_12->show();
			else btnBandSelect_12->hide();
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
