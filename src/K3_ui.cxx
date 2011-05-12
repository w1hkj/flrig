// =====================================================================
// K3 user interface support
//
// a part of flrig
//
// =====================================================================

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

extern void update_preamp(void *d);
extern void update_attenuator(void *d);
extern void update_power_control(void *d);
extern void update_rfgain(void *d);
extern void update_mic_gain(void *d);
extern void update_volume(void *d);
extern void update_split(void *d);

static string K3string = "";
void parse_K3()
{
	string s = "";
	size_t p = 0;
	readResponse();
	K3string.append(replystr);
	while ((p = K3string.find(";")) != string::npos) {
		s = K3string.substr(0, p+1);
		if (s.find("FA") == 0) {
			long  freq = selrig->parse_vfoA(s);
			if (freq != vfoA.freq) {
				pthread_mutex_lock(&mutex_xmlrpc);
				vfoA.freq = freq;
				Fl::awake(setFreqDispA, (void *)vfoA.freq);
				vfo = vfoA;
				try {
					send_new_freq(vfo.freq);
				} catch (...) {}
				pthread_mutex_unlock(&mutex_xmlrpc);
			}
		} else if (s.find("FB") == 0) {
			long freq = selrig->parse_vfoB(s);
			if (freq != vfoB.freq) {
				vfoB.freq = freq;
				Fl::awake(setFreqDispB, (void *)vfoB.freq);
			}
		} else if (s.find("MD$") == 0) {
			int nu_mode = selrig->parse_modeB(s);
			if (nu_mode != vfoB.imode)
				vfoB.imode = nu_mode;
		} else if (s.find("MD") == 0) {
			int nu_mode = selrig->parse_modeA(s);
			if (nu_mode != vfoA.imode) {
				pthread_mutex_lock(&mutex_xmlrpc);
				vfoA.imode = vfo.imode = nu_mode;
				selrig->set_bwA(vfo.iBW = selrig->adjust_bandwidth(nu_mode));
				try {
					send_bandwidths();
					send_new_mode(nu_mode);
					send_sideband();
					send_new_bandwidth(vfo.iBW);
				} catch (...) {}
				pthread_mutex_unlock(&mutex_xmlrpc);
				Fl::awake(setModeControl);
				Fl::awake(updateBandwidthControl);
			}
		} else if (s.find("FW$") == 0) {
			int nu_BW = selrig->parse_bwB(s);
			if (nu_BW != vfoB.iBW)
				vfoB.iBW = nu_BW;
		} else if (s.find("FW") == 0) {
			int nu_BW = selrig->parse_bwA(s);
			if (nu_BW != vfoA.iBW) {
				pthread_mutex_lock(&mutex_xmlrpc);
				vfoA.iBW = vfo.iBW = nu_BW;
				Fl::awake(setBWControl);
				try {
					send_new_bandwidth(vfo.iBW);
				} catch (...) {}
				pthread_mutex_unlock(&mutex_xmlrpc);
			}
		} else if (s.find("PA") == 0) {
			progStatus.preamp = selrig->parse_preamp(s);
			Fl::awake(update_preamp, (void*)0);
		} else if (s.find("RA") == 0) {
			progStatus.attenuator = selrig->parse_attenuator(s);
			Fl::awake(update_attenuator, (void*)0);
		} else if (s.find("PC") == 0) {
			progStatus.power_level = selrig->parse_power_control(s);
			Fl::awake(update_power_control, (void*)0);
		} else if (s.find("RG") == 0) {
			progStatus.rfgain = selrig->parse_rf_gain(s);
			Fl::awake(update_rfgain, (void*)0);
		} else if (s.find("MG") == 0) {
			progStatus.mic_gain = selrig->parse_mic_gain(s);
			Fl::awake(update_mic_gain, (void*)0);
		} else if (s.find("NB") == 0) {
			selrig->parse_noise(s);
		} else if (s.find("AG") == 0) {
			progStatus.volume = selrig->parse_volume_control(s);
			Fl::awake(update_volume, (void*)0);
		} else if (s.find("IF") == 0) {
			progStatus.split = selrig->parse_split(s);
			Fl::awake(update_split, (void*)0);
		} else if (s.find("BG") == 0) {
			int sig = selrig->parse_power_out(s);
			if (sig > -1)
				Fl::awake(updateFwdPwr, (void*)sig);
		}
		K3string.erase(0, p+1);
	}
}

void read_K3_rx()
{
	pthread_mutex_lock(&mutex_serial);
	if (progStatus.poll_frequency) {
		selrig->get_vfoA();
		selrig->get_vfoB();
	}
	pthread_mutex_unlock(&mutex_serial);
	MilliSleep(1);
	Fl::awake();

	pthread_mutex_lock(&mutex_serial);
	if (progStatus.poll_mode) {
		selrig->get_modeA();
		selrig->get_modeB();
	}
	pthread_mutex_unlock(&mutex_serial);
	MilliSleep(1);
	Fl::awake();

	pthread_mutex_lock(&mutex_serial);
	if (progStatus.poll_bandwidth) {
		selrig->get_bwA();
		selrig->get_bwB();
	}
	pthread_mutex_unlock(&mutex_serial);
	MilliSleep(1);
	Fl::awake();

	pthread_mutex_lock(&mutex_serial);
	if (progStatus.poll_smeter) selrig->get_smeter();
	pthread_mutex_unlock(&mutex_serial);
	MilliSleep(1);
	Fl::awake();

	pthread_mutex_lock(&mutex_serial);
	if (progStatus.poll_volume) selrig->get_volume_control();
	pthread_mutex_unlock(&mutex_serial);
	MilliSleep(1);
	Fl::awake();

//	if (progStatus.poll_auto_notch) get_auto_notch();
//	if (progStatus.poll_notch) get_notch();
//	if (progStatus.poll_ifshift) get_ifshift();
	pthread_mutex_lock(&mutex_serial);
	if (progStatus.poll_power_control) selrig->get_power_control();
	pthread_mutex_unlock(&mutex_serial);
	MilliSleep(1);
	Fl::awake();

	pthread_mutex_lock(&mutex_serial);
	if (progStatus.poll_pre_att) {
		selrig->get_preamp();
		selrig->get_attenuator();
	}
	pthread_mutex_unlock(&mutex_serial);
	MilliSleep(1);
	Fl::awake();

	pthread_mutex_lock(&mutex_serial);
	if (progStatus.poll_micgain) selrig->get_mic_gain();
	pthread_mutex_unlock(&mutex_serial);
	MilliSleep(1);
	Fl::awake();

//	if (progStatus.poll_squelch) get_squelch();
	pthread_mutex_lock(&mutex_serial);
	if (progStatus.poll_rfgain) selrig->get_rf_gain();
	pthread_mutex_unlock(&mutex_serial);
	MilliSleep(1);
	Fl::awake();

	pthread_mutex_lock(&mutex_serial);
	if (progStatus.poll_split) selrig->get_split();
	pthread_mutex_unlock(&mutex_serial);
	MilliSleep(1);
	Fl::awake();

	parse_K3();
}

void read_K3_tx()
{
	pthread_mutex_lock(&mutex_serial);
	if (progStatus.poll_pout) selrig->get_power_out();
	pthread_mutex_unlock(&mutex_serial);
//	if (progStatus.poll_swr) selrig->get_swr();
//	if (progStatus.poll_alc) selrig->get_alc();
	parse_K3();
}


void K3_set_split(int val)
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_split(val);
	pthread_mutex_unlock(&mutex_serial);

}

void K3_A2B()
{
	pthread_mutex_lock(&mutex_serial);
	vfoB = vfoA;
	selrig->set_vfoB(vfoB.freq);
	selrig->set_bwB(vfoB.iBW);
	selrig->set_modeB(vfoB.imode);
	FreqDispB->value(vfoB.freq);
	pthread_mutex_unlock(&mutex_serial);
	Fl::focus(FreqDispA);
}

void cb_K3_swapAB()
{
	FREQMODE temp = vfoA;
	pthread_mutex_lock(&mutex_serial);
	vfoA = vfoB;
	vfoB = temp;
	vfo = vfoA;
	selrig->set_vfoA(vfoA.freq);
	selrig->set_bwA(vfoA.iBW);
	selrig->set_modeA(vfoA.imode);
	selrig->set_vfoB(vfoB.freq);
	selrig->set_bwB(vfoB.iBW);
	selrig->set_modeB(vfoB.imode);
	opBW->index(vfoA.iBW);
	opMODE->index(vfoA.imode);
	FreqDispA->value(vfoA.freq);
	FreqDispB->value(vfoB.freq);

	try {
		send_new_freq(vfoA.freq);
		send_bandwidths();
		send_new_mode(vfoA.imode);
		send_sideband();
		send_new_bandwidth(vfoA.iBW);
	} catch (...) {}

	pthread_mutex_unlock(&mutex_serial);
	Fl::focus(FreqDispA);
}
