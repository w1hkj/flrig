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

using namespace std;

rigbase *selrig = rigs[0];

extern bool test;
int freqval = 0;

FREQMODE vfoA = {14070000, 0, 0, UI};
FREQMODE vfoB = {7070000, 0, 0, UI};
FREQMODE vfo = vfoA;
FREQMODE transceiverA;
FREQMODE transceiverB;
FREQMODE xmlvfo = vfoA;

queue<FREQMODE> queA;
queue<FREQMODE> queB;
queue<bool> quePTT;

bool useB = false;

bool bws_changed = false;
const char **old_bws = NULL;

bool localptt = false;

FREQMODE oplist[LISTSIZE];
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
	snprintf(str, sizeof(str), "%3s,%10ld,%4s,%5s",
		data.src == XML ? "xml" : "ui",
		data.freq,
		selrig->modes_[data.imode],
		selrig->bandwidths_[data.iBW]);
	return str;
}

// the following functions are ONLY CALLED by the serial loop
// read any data stream sent by transceiver

// read current vfo frequency
void read_vfo()
{
// transceiver changed ?
	pthread_mutex_lock(&mutex_serial);
	if (selrig->has_get_info) selrig->get_info();
	long  freq;
	if (!useB) { // vfo-A
		freq = selrig->get_vfoA();
		if (freq != vfoA.freq) {
			vfoA.freq = freq;
			Fl::awake(setFreqDispA, (void *)vfoA.freq);
			vfo = vfoA;
			pthread_mutex_lock(&mutex_xmlrpc);
			bypass_digi_loop = true;
			pthread_mutex_unlock(&mutex_xmlrpc);
			send_new_freq(vfo.freq);
			pthread_mutex_lock(&mutex_xmlrpc);
			bypass_digi_loop = false;
			pthread_mutex_unlock(&mutex_xmlrpc);
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
			pthread_mutex_lock(&mutex_xmlrpc);
			bypass_digi_loop = true;
			pthread_mutex_unlock(&mutex_xmlrpc);
			send_new_freq(vfo.freq);
			pthread_mutex_lock(&mutex_xmlrpc);
			bypass_digi_loop = false;
			pthread_mutex_unlock(&mutex_xmlrpc);
		}
	}
	pthread_mutex_unlock(&mutex_serial);
}

void setModeControl(void *)
{
	opMODE->index(vfo.imode);
}

void read_mode()
{
	pthread_mutex_lock(&mutex_serial);
	int nu_mode;
	if (!useB) {
		nu_mode = selrig->get_modeA();
		if (nu_mode != vfoA.imode) {
			vfoA.imode = vfo.imode = nu_mode;
			selrig->set_bwA(selrig->adjust_bandwidth(nu_mode));
			Fl::awake(setModeControl);
			Fl::awake(updateBandwidthControl);
			pthread_mutex_lock(&mutex_xmlrpc);
			bypass_digi_loop = true;
			pthread_mutex_unlock(&mutex_xmlrpc);
			send_new_mode(nu_mode);
			send_sideband();
			if (bws_changed) send_bandwidths();
			send_new_bandwidth(vfo.iBW);
			pthread_mutex_lock(&mutex_xmlrpc);
			bypass_digi_loop = false;
			pthread_mutex_unlock(&mutex_xmlrpc);
		}
	} else {
		nu_mode = selrig->get_modeB();
		if (nu_mode != vfoB.imode) {
			vfoB.imode = vfo.imode = nu_mode;
			selrig->set_bwB(selrig->adjust_bandwidth(nu_mode));
			Fl::awake(setModeControl);
			Fl::awake(updateBandwidthControl);
			pthread_mutex_lock(&mutex_xmlrpc);
			bypass_digi_loop = true;
			pthread_mutex_unlock(&mutex_xmlrpc);
			send_new_mode(nu_mode);
			send_sideband();
			if (bws_changed) send_bandwidths();
			send_new_bandwidth(vfo.iBW);
			pthread_mutex_lock(&mutex_xmlrpc);
			bypass_digi_loop = false;
			pthread_mutex_unlock(&mutex_xmlrpc);
		}
	}
	pthread_mutex_unlock(&mutex_serial);
}

void setBWControl(void *)
{
	opBW->index(vfo.iBW);
}

void read_bandwidth()
{
	pthread_mutex_lock(&mutex_serial);
	int nu_BW;
	if (!useB) {
		nu_BW = selrig->get_bwA();
		if (nu_BW != vfoA.iBW) {
			vfoA.iBW = vfo.iBW = nu_BW;
			Fl::awake(setBWControl);
			pthread_mutex_lock(&mutex_xmlrpc);
			bypass_digi_loop = true;
			pthread_mutex_unlock(&mutex_xmlrpc);
			send_new_bandwidth(vfo.iBW);
			pthread_mutex_lock(&mutex_xmlrpc);
			bypass_digi_loop = false;
			pthread_mutex_unlock(&mutex_xmlrpc);
		}
	} else {
		nu_BW = selrig->get_bwB();
		if (nu_BW != vfoB.iBW) {
			vfoB.iBW = vfo.iBW = nu_BW;
			Fl::awake(setBWControl);
			pthread_mutex_lock(&mutex_xmlrpc);
			bypass_digi_loop = true;
			pthread_mutex_unlock(&mutex_xmlrpc);
			send_new_bandwidth(vfo.iBW);
			pthread_mutex_lock(&mutex_xmlrpc);
			bypass_digi_loop = false;
			pthread_mutex_unlock(&mutex_xmlrpc);
		}
	}
	pthread_mutex_unlock(&mutex_serial);
}

// read current signal level
void read_smeter()
{
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
	int sig;
	pthread_mutex_lock(&mutex_serial);
	sig = selrig->get_power_out();
	pthread_mutex_unlock(&mutex_serial);
	Fl::awake(updateFwdPwr, (void*)sig);
}

// read swr
void read_swr()
{
	int sig;
	pthread_mutex_lock(&mutex_serial);
	sig = selrig->get_swr();
	pthread_mutex_unlock(&mutex_serial);
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
	Fl::awake(updateALC, (void*)sig);
}

static bool resetrcv = true;
static bool resetxmt = true;

void serviceA()
{
	while (!queA.empty()) {
		vfoA = queA.front();
		queA.pop();
		if (!useB) {
			if (RIG_DEBUG)
				LOG_WARN("%s", print(vfoA));
			pthread_mutex_lock(&mutex_serial);

			pthread_mutex_lock(&mutex_xmlrpc);
			bypass_digi_loop = true;
			pthread_mutex_unlock(&mutex_xmlrpc);

			if (vfoA.freq != vfo.freq) {
				selrig->set_vfoA(vfoA.freq);
				vfo.freq = vfoA.freq;
				Fl::awake(setFreqDispA, (void *)vfoA.freq);
				if (vfoA.src == UI)
					send_new_freq(vfoA.freq);
			}
// adjust for change in bandwidths_
			if (vfoA.imode != vfo.imode) {
				selrig->set_modeA(vfoA.imode);
				vfo.imode = vfoA.imode;
				vfo.iBW = selrig->get_bwA();//vfoA.iBW;
				Fl::awake(setModeControl);
				Fl::awake(updateBandwidthControl);
				Fl::awake(setBWControl);
				if (vfoA.src == UI) {
					send_new_mode(vfoA.imode);
				}
				send_sideband();
//				selrig->set_bwA(vfoA.iBW);
				send_bandwidths();
				send_new_bandwidth(vfoA.iBW);
			} else if (vfoA.iBW != vfo.iBW) {
				selrig->set_bwA(vfoA.iBW);
				vfo.iBW = vfoA.iBW;
				Fl::awake(setBWControl);
				if (vfoA.src == UI)
					send_new_bandwidth(vfoA.iBW);
			}

			pthread_mutex_lock(&mutex_xmlrpc);
			bypass_digi_loop = false;
			pthread_mutex_unlock(&mutex_xmlrpc);

			pthread_mutex_unlock(&mutex_serial);
		}
	}
}

void serviceB()
{
	while (!queB.empty()) {
		vfoB = queB.front();
		queB.pop();
		if (useB ) {
			if (RIG_DEBUG)
				LOG_WARN("%s", print(vfoB));
			pthread_mutex_lock(&mutex_serial);

			pthread_mutex_lock(&mutex_xmlrpc);
			bypass_digi_loop = true;
			pthread_mutex_unlock(&mutex_xmlrpc);

			if (vfoB.freq != vfo.freq) {
				selrig->set_vfoB(vfoB.freq);
				vfo.freq = vfoB.freq;
				if (vfoB.src == XML)
					Fl::awake(setFreqDispB, (void *)vfoB.freq);
				else
					send_new_freq(vfoB.freq);
			}
			if (vfoB.imode != vfo.imode) {
				selrig->set_modeB(vfoB.imode);
				vfo.imode = vfoB.imode;
				vfo.iBW = selrig->get_bwB();//vfoB.iBW;
				Fl::awake(setModeControl);
				Fl::awake(updateBandwidthControl);
				Fl::awake(setBWControl);
				if (vfoB.src == UI) {
					send_new_mode(vfoB.imode);
				}
				send_sideband();
//				selrig->set_bwB(vfoB.iBW);
				send_bandwidths();
				send_new_bandwidth(vfoB.iBW);
			} else if (vfoB.iBW != vfo.iBW) {
				selrig->set_bwB(vfoB.iBW);
				vfo.iBW = vfoB.iBW;
				Fl::awake(setBWControl);
				if (vfoB.src == UI)
					send_new_bandwidth(vfoB.iBW);
			}

			pthread_mutex_lock(&mutex_xmlrpc);
			bypass_digi_loop = false;
			pthread_mutex_unlock(&mutex_xmlrpc);

			pthread_mutex_unlock(&mutex_serial);
		}
	}
}

void servicePTT()
{
	while (!quePTT.empty()) {
		PTT = quePTT.front();
		quePTT.pop();
		rigPTT(PTT);
		send_ptt_changed(PTT);
		Fl::awake(ALC_SWR_image);
	}
}

void * serial_thread_loop(void *d)
{
  static int  loopcount = 0;
	for(;;) {
		if (!run_serial_thread) break;

		MilliSleep(progStatus.serloop_timing);

		if (bypass_serial_thread_loop) goto serial_bypass_loop;

		//send any freq/mode/bw changes in the queu

		servicePTT();

		if (!PTT) {
			serviceA();
			serviceB();
		}
// rig specific data reads
		if (!PTT) {
			if (resetrcv) {
				Fl::awake(zeroXmtMeters, 0);
				resetrcv = false;
			}
			resetxmt = true;

			if (rig_nbr == TT550) {
				pthread_mutex_lock(&mutex_serial);
				selrig->read_stream();
				pthread_mutex_unlock(&mutex_serial);
			}
			read_smeter();

			read_vfo();
			switch (loopcount) {
				case 0: read_mode(); break;
				case 1: read_bandwidth(); break;
				default: break;
			}
			loopcount = (loopcount + 1) % 2;
/*
			switch (loopcount) {
				case 0: read_volume(); loopcount++; break;
				case 1: read_mode(); loopcount++; break;
				case 2: read_bandwidth(); loopcount++; break;
				case 3: read_notch(); loopcount++; break;
				case 4: read_ifshift(); loopcount++; break;
				case 5: read_power_control(); loopcount++; break;
				case 6: read_preamp_att(); loopcount++; break;
				case 7: read_mic_gain(); loopcount++; break;
				case 8: read_squelch(); loopcount++; break;
				case 9: read_rfgain(); loopcount = 0;
			}
*/
		} else {
			if (resetxmt) {
				Fl::awake(updateSmeter, (void *)(0));
				resetxmt = false;
			}
			resetrcv = true;

			read_power_out();
			read_swr();
			read_alc();

		}
serial_bypass_loop: ;

	}
	return NULL;
}

//=============================================================================
static bool nofocus = false;

void setFocus()
{
	if (nofocus) return;
	if (Fl::focus() != FreqDispA)
		Fl::focus(FreqDispA);
}

void setBW()
{
	FREQMODE fm = vfo;
	fm.src = UI;
	fm.iBW = opBW->index();
	useB ? queB.push(fm) : queA.push(fm);
}

void updateBandwidthControl(void *d)
{
	bws_changed = false;
	if (selrig->has_bandwidth_control) {
		if (selrig->adjust_bandwidth(vfo.imode) != -1) {
			if (old_bws != selrig->bandwidths_) {
				bws_changed = true;
				old_bws = selrig->bandwidths_;
				opBW->clear();
				rigbws_.clear();
				int i = 0;
				for (i = 0; selrig->bandwidths_[i] != NULL; i++) {
					rigbws_.push_back(selrig->bandwidths_[i]);
					opBW->add(selrig->bandwidths_[i]);
				}
				i--;
				if (vfo.iBW > i) vfo.iBW = selrig->adjust_bandwidth(vfo.imode);
			}
		}
	}
	opBW->index(vfo.iBW);
	useB ? vfoB.iBW = vfo.iBW : vfoA.iBW = vfo.iBW;
}

void setMode()
{
	FREQMODE fm = vfo;
	fm.imode = opMODE->index();
	fm.src = UI;
	useB ? queB.push(fm) : queA.push(fm);
}

void sortList() {
	if (!numinlist) return;
	FREQMODE temp;
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
	}
	FreqSelect->clear();
	numinlist = 0;
}

void updateSelect() {
	char szFREQMODE[20];
	if (!numinlist) return;
	sortList();
	FreqSelect->clear();
	for (int n = 0; n < numinlist; n++) {
		snprintf(szFREQMODE, sizeof(szFREQMODE),
			"%13.3f%7s", oplist[n].freq / 1000.0,
			selrig->get_modename_(oplist[n].imode));
		FreqSelect->add (szFREQMODE);
	}
}

void addtoList(int val, int imode, int iBW) {
	if (numinlist < LISTSIZE) {
		oplist[numinlist].imode = imode;
		oplist[numinlist].freq = val;
		oplist[numinlist++].iBW = iBW;
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
			i++;
		}
	}
	iList.close();
	numinlist = i;
	updateSelect();
}

void buildlist() {
	defFileName = RigHomeDir;
	defFileName.append(selrig->name_);
	defFileName.append(".arv");
	FILE *fh = fopen(defFileName.c_str(), "r");
	if (fh != NULL) {
		fclose (fh);
		readFile();
		return;
	}
	clearList();
}

// flrig front panel changed

int movFreqA() {
	FREQMODE vfo = vfoA;
	vfo.freq = FreqDispA->value();
	vfo.src = UI;
	queA.push(vfo);
	return 1;
}

int movFreqB() {
	FREQMODE vfo = vfoB;
	vfo.freq = FreqDispB->value();
	vfo.src = UI;
	queB.push(vfo);
	return 1;
}

void cbABactive()
{
	if (useB) cb_selectA();
	else cb_selectB();
	return;
}

void cbA2B()
{
	vfoB = vfoA;

	pthread_mutex_lock(&mutex_serial);
		selrig->set_vfoB(vfoB.freq);
		selrig->set_modeB(vfoB.imode);
		selrig->set_bwB(vfoB.iBW);
	pthread_mutex_unlock(&mutex_serial);

	FreqDispB->value(vfoB.freq);
	FreqDispB->redraw();
}

void cb_set_split(int val)
{
	progStatus.split = val;
	pthread_mutex_lock(&mutex_serial);
		selrig->set_split(val);
	pthread_mutex_unlock(&mutex_serial);
}

void set_vfo_mode_bw()
{

	opMODE->index(vfo.imode);
	updateBandwidthControl();

	send_new_freq(vfo.freq);
	send_new_mode(vfo.imode);
	send_sideband();
	send_bandwidths();
	send_new_bandwidth(vfo.iBW);
}

void cb_selectA() {
	pthread_mutex_lock(&mutex_xmlrpc);
	bypass_digi_loop = true;
	pthread_mutex_unlock(&mutex_xmlrpc);

	FreqDispA->SetONOFFCOLOR(
		fl_rgb_color(progStatus.fg_red, progStatus.fg_green, progStatus.fg_blue),
		fl_rgb_color(progStatus.bg_red, progStatus.bg_green, progStatus.bg_blue));
	FreqDispB->SetONOFFCOLOR(
		fl_rgb_color(progStatus.fg_red, progStatus.fg_green, progStatus.fg_blue),
		fl_color_average(fl_rgb_color(progStatus.bg_red, progStatus.bg_green, progStatus.bg_blue), FL_BLACK, 0.87));
	FreqDispA->redraw();
	FreqDispB->redraw();
	Fl::flush();

	pthread_mutex_lock(&mutex_serial);
	useB = false;
	while (!queA.empty()) queA.pop();
	selrig->selectA();
	vfoA.src = UI;
	queA.push(vfoA);
	pthread_mutex_unlock(&mutex_serial);

	pthread_mutex_lock(&mutex_xmlrpc);
	bypass_digi_loop = false;
	pthread_mutex_unlock(&mutex_xmlrpc);
}

void cb_selectB() {
	pthread_mutex_lock(&mutex_xmlrpc);
	bypass_digi_loop = true;
	pthread_mutex_unlock(&mutex_xmlrpc);

	FreqDispB->SetONOFFCOLOR(
		fl_rgb_color(progStatus.fg_red, progStatus.fg_green, progStatus.fg_blue),
		fl_rgb_color(progStatus.bg_red, progStatus.bg_green, progStatus.bg_blue));
	FreqDispA->SetONOFFCOLOR(
		fl_rgb_color(progStatus.fg_red, progStatus.fg_green, progStatus.fg_blue),
		fl_color_average(fl_rgb_color(progStatus.bg_red, progStatus.bg_green, progStatus.bg_blue), FL_BLACK, 0.87));
	FreqDispA->redraw();
	FreqDispB->redraw();
	Fl::flush();

	pthread_mutex_lock(&mutex_serial);
	useB = true;
	while (!queB.empty()) queB.pop();
	selrig->selectB();
	vfoB.src = UI;
	queB.push(vfoB);
	pthread_mutex_unlock(&mutex_serial);

	pthread_mutex_lock(&mutex_xmlrpc);
	bypass_digi_loop = false;
	pthread_mutex_unlock(&mutex_xmlrpc);
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
		queA.push(fm);
	} else {
		FreqDispB->value(fm.freq);
		queB.push(fm);
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
		numinlist--;
		updateSelect();
	}
}

void addFreq() {
	long freq = FreqDispA->value();
	if (!freq) return;
	int mode = opMODE->index();
	int bw = opBW->index();
	for (int n = 0; n < numinlist; n++)
		if (freq == oplist[n].freq && mode == oplist[n].imode) {
			oplist[n].iBW = bw;
			return;
		}
	addtoList(freq, mode, bw);
	updateSelect();
	FreqDispA->visual_beep();
}

void cbRIT()
{
	if (selrig->has_rit)
		selrig->setRit((int)cntRIT->value());
}

void cbXIT()
{
	selrig->setXit((int)cntXIT->value());
}

void cbBFO()
{
	if (selrig->has_bfo)
		selrig->setBfo((int)cntBFO->value());
}

void cbAttenuator()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_attenuator(btnAttenuator->value());
	pthread_mutex_unlock(&mutex_serial);
}

void setAttControl(void *d)
{
	int val = (long)d;
	btnAttenuator->value(val);
}

void cbPreamp()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_preamp(btnPreamp->value());
	pthread_mutex_unlock(&mutex_serial);
}

void setPreampControl(void *d)
{
	int val = (long)d;
	btnPreamp->value(val);
}

void cbNoise()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_noise(btnNOISE->value());
	pthread_mutex_unlock(&mutex_serial);
}

void cbNR()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_noise_reduction(btnNR->value());
	pthread_mutex_unlock(&mutex_serial);
}

void setNR()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_noise_reduction_val(sldrNR->value());
	pthread_mutex_unlock(&mutex_serial);
}

void setNoiseControl(void *d)
{
	btnNOISE->value((long)d);
}

void cbbtnNotch()
{
	pthread_mutex_lock(&mutex_serial);
	if (btnNotch->value() == 0) {
		selrig->set_notch(false, 0);
	} else {
		selrig->set_notch(true, sldrNOTCH->value());
	}
	pthread_mutex_unlock(&mutex_serial);
}

void cbAN()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_auto_notch(btnAutoNotch->value());
	pthread_mutex_unlock(&mutex_serial);
}

void setNotchButton(void *d)
{
	btnNotch->value((bool)d);
}

void setNotchControl(void *d)
{
	int val = (long)d;
	if (sldrNOTCH->value() != val) sldrNOTCH->value(val);
}

void setNotch()
{
	if (btnNotch->value()) {
		pthread_mutex_lock(&mutex_serial);
			selrig->set_notch(true, sldrNOTCH->value());
		pthread_mutex_unlock(&mutex_serial);
	}
}

void setIFshiftButton(void *d)
{
	bool b = (bool)d;
	if (b && !btnIFsh->value()) btnIFsh->value(1);
	else if (!b && btnIFsh->value()) btnIFsh->value(0);
}

void setIFshiftControl(void *d)
{
	int val = (long)d;
	if (sldrIFSHIFT->value() != val) sldrIFSHIFT->value(val);
}

void setIFshift()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_if_shift(sldrIFSHIFT->value());
	pthread_mutex_unlock(&mutex_serial);
}

void cbIFsh()
{
	if (btnIFsh->value() == 1) {
		sldrIFSHIFT->value(0);
		sldrIFSHIFT->activate();
	} else {
		sldrIFSHIFT->value(0);
		sldrIFSHIFT->deactivate();
	}
	setIFshift();
}

void cbEventLog()
{
	debug::show();
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
			selrig->set_volume_control(0.0);
		pthread_mutex_unlock(&mutex_serial);
	} else {
		pthread_mutex_lock(&mutex_serial);
			sldrVOLUME->activate();
			selrig->set_volume_control(progStatus.volume);
		pthread_mutex_unlock(&mutex_serial);
	}
}

void setMicGain()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_mic_gain(sldrMICGAIN->value());
	pthread_mutex_unlock(&mutex_serial);
}

void cbbtnMicLine()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_mic_line(btnMicLine->value());
	pthread_mutex_unlock(&mutex_serial);
}

void setMicGainControl(void* d)
{
	sldrMICGAIN->value((long)d);
}

void set_power_controlImage(double pwr)
{
	if (pwr < 26.0) {
		scalePower->image(image_p25);
		sldrFwdPwr->maximum(25.0);
		sldrFwdPwr->minimum(0.0);
	}
	else if (pwr < 51.0) {
		scalePower->image(image_p50);
		sldrFwdPwr->maximum(50.0);
		sldrFwdPwr->minimum(0.0);
	}
	else if (pwr < 101.0) {
		scalePower->image(image_p100);
		sldrFwdPwr->maximum(100.0);
		sldrFwdPwr->minimum(0.0);
	}
	else {
		scalePower->image(image_p200);
		sldrFwdPwr->maximum(200.0);
		sldrFwdPwr->minimum(0.0);
	}
	scalePower->redraw();
	return;
}

void setPower()
{
	double pwr = sldrPOWER->value();
	pthread_mutex_lock(&mutex_serial);
		powerlevel = (int)pwr;
//		if (selrig != &rig_TT550)
			progStatus.power_level = pwr;
		selrig->set_power_control(pwr);
	pthread_mutex_unlock(&mutex_serial);
}

void reset_power_controlImage( void *d )
{
	int val = (long)d;
	sldrPOWER->value(val);
}

void cbTune()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->tune_rig();
	pthread_mutex_unlock(&mutex_serial);
}

void cbPTT()
{
	quePTT.push(btnPTT->value());
	return;
}

void setSQUELCH()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_squelch((int)sldrSQUELCH->value());
	pthread_mutex_unlock(&mutex_serial);
}

void setRFGAIN()
{
	pthread_mutex_lock(&mutex_serial);
		selrig->set_rf_gain((int)sldrRFGAIN->value());
	pthread_mutex_unlock(&mutex_serial);
}


void updateALC(void * d)
{
	if (meter_image != ALC_IMAGE) return;
	double data = (long)d;
	sldrALC_SWR->value(data);
	sldrALC_SWR->redraw();
}

void updateSWR(void * d)
{
	if (meter_image != SWR_IMAGE) return;
	double data = (long)d;
	sldrALC_SWR->value(data);
	sldrALC_SWR->redraw();
}

void updateFwdPwr(void *d)
{
	double power = (long)d;
	if (!sldrFwdPwr->visible()) {
		sldrFwdPwr->show();
		if (selrig->has_swr_control) {
			sldrRcvSignal->hide();
			sldrALC_SWR->show();
		}
	}
	sldrFwdPwr->value(power);
	sldrFwdPwr->redraw();
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
	sldrFwdPwr->aging(1);
	updateFwdPwr(0);
	updateALC(0);
	updateSWR(0);
	sldrFwdPwr->aging(5);
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
	double swr = (long)d;
	if (!sldrRcvSignal->visible()) {
		sldrFwdPwr->hide();
		sldrRcvSignal->show();
		sldrALC_SWR->hide();
	}
	sldrRcvSignal->value(swr);
	sldrRcvSignal->redraw();
}

void saveFreqList()
{
	if (!numinlist) {
		remove(defFileName.c_str());
		return;
	}
	ofstream oList(defFileName.c_str());
	if (!oList) {
		fl_message ("Could not write to %s", defFileName.c_str());
		return;
	}
	for (int i = 0; i < numinlist; i++)
		oList << oplist[i].freq << " " << oplist[i].imode << " " << oplist[i].iBW << endl;
	oList.close();
}

void setPTT( void *d)
{
	int val = (long)d;
	quePTT.push(val);
}


void closeRig()
{
	// restore initial rig settings
	pthread_mutex_lock(&mutex_serial);
	selrig->setVfoAdj(0);
	selrig->set_vfoA(transceiverA.freq);
	selrig->set_modeA(transceiverA.imode);
	selrig->set_bwA(transceiverA.iBW);
	selrig->set_vfoB(transceiverB.freq);
	selrig->set_modeB(transceiverB.imode);
	selrig->set_bwB(transceiverB.iBW);
	selrig->shutdown();
	pthread_mutex_unlock(&mutex_serial);
}


void cbExit()
{
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

//	closeRig();
	// shutdown serial thread

	pthread_mutex_lock(&mutex_serial);
		run_serial_thread = false;
	pthread_mutex_unlock(&mutex_serial);
	pthread_join(*serial_thread, NULL);

	selrig->setVfoAdj(0);
	if (progStatus.restore_rig_data) {
		selrig->set_vfoA(transceiverA.freq);
		selrig->set_modeA(transceiverA.imode);
		selrig->set_bwA(transceiverA.iBW);
	}
	selrig->shutdown();

	// close down the serial port
	RigSerial.ClosePort();

	// restore fldigi to its no-rig state
	try {
		send_no_rig();
	} catch (...) { }

	// shutdown xmlrpc thread
	pthread_mutex_lock(&mutex_xmlrpc);
	run_digi_loop = false;
	pthread_mutex_unlock(&mutex_xmlrpc);
	pthread_join(*digi_thread, NULL);

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
	} else {
		btnALC_SWR->image(image_swr);
		meter_image = SWR_IMAGE;
	}
	btnALC_SWR->redraw();
}

void ALC_SWR_image(void *d)
{
	btnPTT->value(PTT);
	if (!PTT) {
		btnALC_SWR->hide();
		scaleSmeter->show();
	} else {
		btnALC_SWR->show();
		scaleSmeter->hide();
	}
}

void adjust_control_positions()
{
	int y = cntRIT->y() + 2;
	if (selrig->has_volume_control) {
		y += 20;
		sldrVOLUME->position( sldrVOLUME->x(), y );
		btnVol->position( btnVol->x(), y);
		sldrVOLUME->redraw();
		btnVol->redraw();
	}
	if (selrig->has_rf_control) {
		y += 20;
		sldrRFGAIN->position( sldrRFGAIN->x(), y );
		sldrRFGAIN->redraw();
	}
	if (selrig->has_sql_control) {
		y += 20;
		sldrSQUELCH->position( sldrSQUELCH->x(), y);
		sldrSQUELCH->redraw();
	}
	if (selrig->has_noise_reduction_control) {
		y += 20;
		sldrNR->position( sldrNR->x(), y);
		btnNR->position( btnNR->x(), y);
		sldrNR->redraw();
		btnNR->show();
	}
	if (selrig->has_ifshift_control) {
		y += 20;
		sldrIFSHIFT->position( sldrIFSHIFT->x(), y);
		btnIFsh->position( btnIFsh->x(), y);
		sldrIFSHIFT->redraw();
		btnIFsh->show();
	}
	if (selrig->has_notch_control) {
		y += 20;
		sldrNOTCH->position( sldrNOTCH->x(), y);
		btnNotch->position( btnNotch->x(), y);
		sldrNOTCH->redraw();
		btnNotch->show();
	}
	if (selrig->has_micgain_control) {
		y += 20;
		sldrMICGAIN->position( sldrMICGAIN->x(), y);
		sldrMICGAIN->redraw();
	}
	if (selrig->has_power_control) {
		y += 20;
		sldrPOWER->position( sldrPOWER->x(), y);
		sldrPOWER->redraw();
	}
	y += 20;
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

	mainwindow->size( mainwindow->w(), y + 20);
	mainwindow->redraw();

	if (progStatus.tooltips) {
		Fl_Tooltip::enable(1);
		mnuTooltips->set();
	} else {
		mnuTooltips->clear();
		Fl_Tooltip::enable(0);
	}

}

void initXcvrTab()
{
	if (rig_nbr == TT550) {
		cnt_tt550_line_out->activate(); cnt_tt550_line_out->value(progStatus.tt550_line_out);
		cbo_tt550_agc_level->activate(); cbo_tt550_agc_level->index(progStatus.tt550_agc_level);
		cnt_tt550_cw_wpm->activate(); cnt_tt550_cw_wpm->value(progStatus.tt550_cw_wpm);
		cnt_tt550_cw_vol->activate(); cnt_tt550_cw_vol->value(progStatus.tt550_cw_vol);
		cnt_tt550_cw_spot->activate(); cnt_tt550_cw_spot->value(progStatus.tt550_cw_spot);
		cnt_tt550_cw_weight->activate(); cnt_tt550_cw_weight->value(progStatus.tt550_cw_weight);
		btn_tt550_spot_onoff->activate(); btn_tt550_spot_onoff->value(progStatus.tt550_spot_onoff);
		btn_tt550_enable_keyer->activate(); btn_tt550_enable_keyer->value(progStatus.tt550_enable_keyer);
		btn_tt550_vox->activate(); btn_tt550_vox->value(progStatus.tt550_vox_onoff);
		cnt_tt550_vox_gain->activate(); cnt_tt550_vox_gain->value(progStatus.tt550_vox_gain);
		cnt_tt550_anti_vox->activate(); cnt_tt550_anti_vox->value(progStatus.tt550_vox_anti);
		cnt_tt550_vox_hang->activate(); cnt_tt550_vox_hang->value(progStatus.tt550_vox_hang);
		btn_tt550_CompON->activate(); btn_tt550_CompON->value(progStatus.tt550_compON);
		cnt_tt550_compression->activate(); cnt_tt550_compression->value(progStatus.tt550_compression);
		cnt_tt550_mon_vol->activate(); cnt_tt550_mon_vol->value(progStatus.tt550_mon_vol);
		btn_tt550_tuner_bypass->activate(); btn_tt550_tuner_bypass->value(progStatus.tt550_tuner_bypass);
		mnuKeepData->deactivate();
		op_tt550_XmtBW->clear();
		for (int i = 0; TT550_xmt_widths[i] != NULL; i++) {
			op_tt550_XmtBW->add(TT550_xmt_widths[i]);
		}
		op_tt550_XmtBW->activate();
		op_tt550_XmtBW->index(progStatus.tt550_xmt_bw);
	} else {
		if (selrig->has_agc_level) cbo_agc_level->activate(); else cbo_agc_level->deactivate();
		if (selrig->has_cw_wpm) cnt_cw_wpm->activate(); else cnt_cw_wpm->deactivate();
		if (selrig->has_cw_vol) cnt_cw_vol->activate(); else cnt_cw_vol->deactivate();
		if (selrig->has_cw_spot) {
			cnt_cw_spot->activate(); 
			btnSpot->activate();
		} else {
			cnt_cw_spot->deactivate();
			btnSpot->deactivate();
		}
		if (selrig->has_vox_onoff) btn_vox->activate(); else btn_vox->deactivate();
		if (selrig->has_vox_gain) cnt_vox_gain->activate(); else cnt_vox_gain->deactivate();
		if (selrig->has_vox_anti) cnt_anti_vox->activate(); else cnt_anti_vox->deactivate();
		if (selrig->has_vox_hang) cnt_vox_hang->activate(); else cnt_vox_hang->deactivate();
		if (selrig->has_compression)
			cnt_compression->activate();
		else
			cnt_compression->deactivate();
		if (selrig->has_compON)
			btnCompON->activate();
		else
			btnCompON->deactivate();
		cnt_line_out->deactivate();
		mnuKeepData->activate();
	}
}

void initRig()
{
//	wait_query = true;

// disable xml loop
	pthread_mutex_lock(&mutex_xmlrpc);
	bypass_digi_loop = true;
	pthread_mutex_unlock(&mutex_xmlrpc);

// disable the serial thread
	pthread_mutex_lock(&mutex_serial);

	selrig->initialize();

	FreqDispA->set_precision(selrig->precision);
	FreqDispB->set_precision(selrig->precision);

	if (progStatus.CIV > 0) selrig->adjustCIV(progStatus.CIV);

	if (selrig->has_get_info) selrig->get_info();
	transceiverA.freq = selrig->get_vfoA();
	transceiverA.imode = selrig->get_modeA();
	transceiverA.iBW = selrig->get_bwA();
	transceiverB.freq = selrig->get_vfoB();
	transceiverB.imode = selrig->get_modeB();
	transceiverB.iBW = selrig->get_bwB();

	if (selrig->restore_mbw) selrig->last_bw = transceiverA.iBW;

	if (progStatus.use_rig_data) {
		progStatus.freq_A = transceiverA.freq;
		progStatus.imode_A = transceiverA.imode;
		progStatus.iBW_A = transceiverA.iBW;
		progStatus.freq_B = transceiverB.freq;
		progStatus.imode_B = transceiverB.imode;
		progStatus.iBW_B = transceiverB.iBW;
		mnuKeepData->set();
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
		updateBandwidthControl();
		selrig->set_modeA(progStatus.imode_A);
	} else {
		opMODE->add(" ");
		opMODE->index(0);
		opMODE->deactivate();
	}

	rigbws_.clear();
	opBW->clear();
	if (selrig->has_bandwidth_control) {
//		selrig->adjust_bandwidth(vfoA.imode);
		old_bws = selrig->bandwidths_;
		for (int i = 0; selrig->bandwidths_[i] != NULL; i++) {
			rigbws_.push_back(selrig->bandwidths_[i]);
				opBW->add(selrig->bandwidths_[i]);
			}
		opBW->activate();
		opBW->index(progStatus.iBW_A);
		selrig->set_bwA(progStatus.iBW_A);
	} else {
		opBW->add(" ");
		opBW->index(0);
		opBW->deactivate();
	}

	if (selrig->has_special)
		btnSpecial->show();
	else
		btnSpecial->hide();

	if (selrig->has_bpf_center) {
		cnt_bpf_center->value(progStatus.bpf_center);
		cnt_bpf_center->activate();
		btn_use_bpf_center->activate();
	} else {
		cnt_bpf_center->deactivate();
		btn_use_bpf_center->deactivate();
	}

	if (selrig->has_vfo_adj) {
		cnt_vfo_adj->value(progStatus.vfo_adj);
		cnt_vfo_adj->activate();
	} else
		cnt_vfo_adj->deactivate();

	if (selrig->has_rit) {
		cntRIT->value(progStatus.rit_freq);
		int min, max, step;
		selrig->get_RIT_min_max_step(min, max, step);
		cntRIT->minimum(min);
		cntRIT->maximum(max);
		cntRIT->step(step);
		cntRIT->activate();
	} else
		cntRIT->deactivate();

	if (selrig->has_xit) {
		cntXIT->value(progStatus.xit_freq);
		int min, max, step;
		selrig->get_XIT_min_max_step(min, max, step);
		cntXIT->minimum(min);
		cntXIT->maximum(max);
		cntXIT->step(step);
		cntXIT->activate();
	} else
		cntXIT->deactivate();

	if (selrig->has_bfo) {
		cntBFO->value(progStatus.bfo_freq);
		int min, max, step;
		selrig->get_BFO_min_max_step(min, max, step);
		cntBFO->minimum(min);
		cntBFO->maximum(max);
		cntBFO->step(step);
		cntBFO->activate();
	} else
		cntBFO->deactivate();

	if (selrig->has_volume_control) {
		int min, max, step;
		selrig->get_vol_min_max_step(min, max, step);
		sldrVOLUME->minimum(min);
		sldrVOLUME->maximum(max);
		sldrVOLUME->step(step);
		sldrVOLUME->redraw();
		progStatus.volume = selrig->get_volume_control();
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
		btnVol->show();
		sldrVOLUME->show();
	} else {
		btnVol->hide();
		sldrVOLUME->hide();
	}

	if (selrig->has_rf_control) {
		sldrRFGAIN->value(
			progStatus.rfgain = selrig->get_rf_gain());
		sldrRFGAIN->show();
	} else {
		sldrRFGAIN->hide();
	}

	if (selrig->has_sql_control) {
		sldrSQUELCH->value(
			progStatus.squelch = selrig->get_squelch());
		sldrSQUELCH->show();
	} else {
		sldrSQUELCH->hide();
	}

	if (selrig->has_noise_reduction_control) {
		btnNR->show();
		btnNR->value(progStatus.noise_reduction);
		sldrNR->show();
		sldrNR->value(progStatus.noise_reduction_val);
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
		if (progStatus.shift) {
			btnIFsh->value(1);
			sldrIFSHIFT->value(progStatus.shift_val);
			selrig->set_if_shift(progStatus.shift_val);
		} else {
			btnIFsh->value(0);
			sldrIFSHIFT->value(0);
			sldrIFSHIFT->deactivate();
			selrig->set_if_shift(0);
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
		btnNotch->value(progStatus.notch);
		sldrNOTCH->value(progStatus.notch_val);
		selrig->set_notch(progStatus.notch, progStatus.notch_val);
		btnNotch->show();
		sldrNOTCH->show();
	} else {
		btnNotch->hide();
		sldrNOTCH->hide();
	}

	if (selrig->has_micgain_control) {
		int min, max, step;
		selrig->get_mic_min_max_step(min, max, step);
		sldrMICGAIN->minimum(min);
		sldrMICGAIN->maximum(max);
		sldrMICGAIN->step(step);
		sldrMICGAIN->redraw();
		sldrMICGAIN->value(progStatus.mic_gain);
		selrig->set_mic_gain(progStatus.mic_gain);
		sldrMICGAIN->show();
	} else {
		sldrMICGAIN->hide();
	}

	if (selrig->has_power_control) {
		int min, max, step;
		selrig->get_pc_min_max_step(min, max, step);
		sldrPOWER->minimum(min);
		sldrPOWER->maximum(max);
		sldrPOWER->step(step);
		sldrPOWER->redraw();
		if (min > progStatus.power_level)
			progStatus.power_level = min;
		if (max < progStatus.power_level)
			progStatus.power_level = max;
		sldrPOWER->value(progStatus.power_level);
		sldrPOWER->show();
		selrig->set_power_control(progStatus.power_level);
	} else {
		sldrPOWER->hide();
	}
	set_power_controlImage(selrig->max_power);

	if (selrig->has_attenuator_control) {
		btnAttenuator->label("Att");
		progStatus.attenuator = selrig->get_attenuator();
		btnAttenuator->value(progStatus.attenuator);
		btnAttenuator->show();
	} else {
		btnAttenuator->hide();
	}

	if (selrig->has_preamp_control) {
		btnPreamp->label("Pre");
		progStatus.preamp = selrig->get_preamp();
		btnPreamp->value(progStatus.preamp);
		btnPreamp->show();
	} else {
		btnPreamp->hide();
	}

	if (selrig->has_noise_control) {
		btnNOISE->value(progStatus.noise);
		selrig->set_noise(progStatus.noise);
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
		btnPTT->show();
	} else {
		btnPTT->hide();
	}

	if (selrig->has_auto_notch) {
		if (rig_nbr == RAY152)
			btnAutoNotch->label("AGC");
		else
			btnAutoNotch->label("AN");
		progStatus.auto_notch = selrig->get_auto_notch();
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
	selrig->set_bwA(vfoA.iBW);

	vfo = vfoA;
	useB = false;

	opMODE->index( vfo.imode );
	updateBandwidthControl();

	selrig->selectA();

	if (selrig->CIV) {
		char hexstr[8];
		snprintf(hexstr, sizeof(hexstr), "0x%2X", selrig->CIV);
		txtCIV->value(hexstr);
		txtCIV->activate();
		btnCIVdefault->activate();
		if (strstr(selrig->name_, "IC-7200") || strstr(selrig->name_, "IC-7600")) {
			btnUSBaudio->value(progStatus.USBaudio = false);
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

	// enable the serial thread
	queA.push(vfo);
	pthread_mutex_unlock(&mutex_serial);
//	set_vfo_mode_bw();

// enable xml loop
	pthread_mutex_lock(&mutex_xmlrpc);
	fldigi_online = false;
	rig_reset = true;
//	wait_query = false;
	bypass_digi_loop = false;
	pthread_mutex_unlock(&mutex_xmlrpc);

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
			btnUSBaudio->value(progStatus.USBaudio = false);
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
	} else {
		selectCommPort->value(progStatus.xcvr_serial_port.c_str());
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

	bypass_serial_thread_loop = false;

}

void initRigCombo()
{
	selectRig->clear();
	int i = 0;
	while (rigs[i] != NULL)
		selectRig->add(rigs[i++]->name_);

	selectRig->index(rig_nbr = 0);
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
	selrig->set_agc_level();
}

void cb_cw_wpm()
{
	selrig->set_cw_wpm();
}

void cb_cw_vol()
{
	selrig->set_cw_vol();
}

void cb_cw_spot()
{
	selrig->set_cw_spot();
}

void cb_vox_gain()
{
	selrig->set_vox_gain();
}

void cb_vox_anti()
{
	selrig->set_vox_anti();
}

void cb_vox_hang()
{
	selrig->set_vox_hang();
}

void cb_vox_onoff()
{
	selrig->set_vox_onoff();
}

void cb_compression()
{
	selrig->set_compression();
}

void cb_auto_notch()
{
	selrig->set_auto_notch(btnAutoNotch->value());
}

void cb_vfo_adj()
{
	pthread_mutex_lock(&mutex_serial);
		if (selrig->has_get_info) selrig->get_info();
		long f = selrig->get_vfoA();
		selrig->setVfoAdj(progStatus.vfo_adj);
		selrig->set_vfoA(f);
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

