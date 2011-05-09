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

void read_K3()
{
	pthread_mutex_lock(&mutex_serial);

	if (progStatus.poll_frequency) {
		long  freq;
		freq = selrig->get_vfoA();
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
		freq = selrig->get_vfoB();
		if (freq != vfoB.freq) {
			vfoB.freq = freq;
			Fl::awake(setFreqDispB, (void *)vfoB.freq);
		}
	}

	if (progStatus.poll_mode) {
		int nu_mode;
		nu_mode = selrig->get_modeA();
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
		nu_mode = selrig->get_modeB();
		if (nu_mode != vfoB.imode) {
			vfoB.imode = nu_mode;
		}
	}

	if (progStatus.poll_bandwidth) {
		int nu_BW;
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
		nu_BW = selrig->get_bwB();
		if (nu_BW != vfoB.iBW) {
			vfoB.iBW = nu_BW;
		}
	}

	pthread_mutex_unlock(&mutex_serial);
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
	cntK3bw->value(vfoA.iBW);
	opMODE->index(vfoA.imode);
	FreqDispA->value(vfoA.freq);
	FreqDispB->value(vfoB.freq);
	pthread_mutex_unlock(&mutex_serial);
	Fl::focus(FreqDispA);
}
