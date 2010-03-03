#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
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
#include "rig.h"
#include "dialogs.h"
#include "rigbase.h"
#include "ptt.h"
#include "xml_io.h"

using namespace std;

rigbase *selrig = rigs[0];

extern bool test;
int freqval = 0;

FREQMODE vfoB = {7070000, 0, 0};
FREQMODE vfoA = {14070000, 0, 0};
int transceiver_bw = 0;
int transceiver_mode = 0;
long transceiver_freq = 7070000;

bool bws_changed = false;
const char **old_bws = NULL;

bool localptt = false;
bool remoteptt = false;

char szVfoB[20];

FREQMODE oplist[LISTSIZE];
int  numinlist = 0;
vector<string> rigmodes_;
vector<string> rigbws_;

Cserial RigSerial;
Cserial AuxSerial;
Cserial SepSerial;

bool using_buttons = false;

//=============================================================================
// loop for serial i/o thread
// runs continuously until program is closed
// only accesses the serial port if it has been successfully opened
//=============================================================================

bool bypass_serial_thread_loop = true;
bool run_serial_thread = true;

bool PTT = false;
int  powerlevel = 0;

// the following functions are ONLY CALLED by the serial loop

// read current vfo frequency
void read_vfo()
{
	if (vfoA.freq != FreqDisp->value()) {
		wait_query = true;
		vfoA.freq = FreqDisp->value();
		selrig->set_vfoA(vfoA.freq);
		send_new_freq();
		wait_query = false;
		return;
	}
	long  freq = selrig->get_vfoA();
	if (freq != vfoA.freq) {
		vfoA.freq = freq;
		Fl::awake(setFreqDisp);
	}
}

void setModeControl(void *)
{
	opMODE->index(vfoA.imode);
}

void read_mode()
{
	int nu_mode = opMODE->index();
	if (vfoA.imode != nu_mode) {
		wait_query = true;
		if (selrig->restore_mbw) {
			selrig->set_bandwidth(selrig->last_bw);
			selrig->last_bw = vfoA.iBW;
		}
		vfoA.imode = nu_mode;
		selrig->adjust_bandwidth(vfoA.imode);
		selrig->set_mode(vfoA.imode);
		Fl::awake(updateBandwidthControl);

		send_mode_changed();
		send_sideband();
		if (bws_changed) send_bandwidths();
		send_bandwidth_changed();
		wait_query = false;
		return;
	}
	nu_mode = selrig->get_mode();
	if (vfoA.imode != nu_mode) {
		vfoA.imode = nu_mode;
		wait_query = true;
		selrig->adjust_bandwidth(vfoA.imode);
		Fl::awake(setModeControl);
		Fl::awake(updateBandwidthControl);
		send_mode_changed();
		send_sideband();
		if (bws_changed) send_bandwidths();
		send_bandwidth_changed();
		wait_query = false;
	}
}

void setBWControl(void *)
{
	opBW->index(vfoA.iBW);
}

void read_bandwidth()
{
	int nu_BW = opBW->index();
	if (vfoA.iBW != nu_BW) {
		wait_query = true;
		vfoA.iBW = nu_BW;
		selrig->set_bandwidth(vfoA.iBW);
		send_bandwidth_changed();
		wait_query = false;
		return;
	}
	nu_BW = selrig->get_bandwidth();
	if (vfoA.iBW != nu_BW) {
		wait_query = true;
		vfoA.iBW = nu_BW;
		Fl::awake(setBWControl);
		send_bandwidth_changed();
		wait_query = false;
	}
}

// read current signal level
void read_smeter()
{
	int  sig;
		sig = selrig->get_smeter();
	if (sig == -1) return;
	Fl::awake(updateSmeter, (void*)sig);
}

// read power out
void read_power_out()
{
	int sig;
	sig = selrig->get_power_out();
	Fl::awake(updateFwdPwr, (void*)sig);
}

// read swr
void read_swr()
{
	int sig;
		sig = selrig->get_swr();
	Fl::awake(updateSWR, (void*)sig);
}

// alc
void read_alc()
{
	if (!selrig->has_alc_control) return;
	int sig;
		sig = selrig->get_alc();
	Fl::awake(updateALC, (void*)sig);
}

static bool resetrcv = true;
static bool resetxmt = true;

void * serial_thread_loop(void *d)
{
  static int  loopcount = 0;
	for(;;) {
		if (!run_serial_thread) break;

		MilliSleep(progStatus.serloop_timing);

		if (bypass_serial_thread_loop) goto serial_bypass_loop;
// rig specific data reads
		if (!PTT) {
			if (resetrcv) {
				Fl::awake(zeroXmtMeters, 0);
				resetrcv = false;
			}
			resetxmt = true;

			pthread_mutex_lock(&mutex_serial);
				read_vfo();
				read_smeter();
				switch (loopcount) {
					case 0: read_mode(); break;
					case 1: read_bandwidth(); break;
					default: break;
				}
				++loopcount; loopcount %= 2;
			pthread_mutex_unlock(&mutex_serial);

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

			pthread_mutex_lock(&mutex_serial);
				read_power_out();
				read_swr();
				read_alc();
			pthread_mutex_unlock(&mutex_serial);

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
	if (Fl::focus() != FreqDisp)
		Fl::focus(FreqDisp);
}

void setBW()
{
	wait_query = true;
	pthread_mutex_lock(&mutex_serial);
		vfoA.iBW = opBW->index();
		selrig->set_bandwidth(vfoA.iBW);
	pthread_mutex_unlock(&mutex_serial);
	send_bandwidth_changed();
	wait_query = false;
}

void updateBandwidthControl(void *d)
{
	bws_changed = false;
	if (selrig->has_bandwidth_control) {
		int newbw = selrig->adjust_bandwidth(vfoA.imode);
		if (newbw != -1) {
			if (old_bws != selrig->bandwidths_) {
				bws_changed = true;
				old_bws = selrig->bandwidths_;
				opBW->clear();
				rigbws_.clear();
				for (int i = 0; selrig->bandwidths_[i] != NULL; i++) {
					rigbws_.push_back(selrig->bandwidths_[i]);
					opBW->add(selrig->bandwidths_[i]);
				}
			}
			opBW->index(newbw);
			vfoA.iBW = newbw;
		}
	}
}

void setMode()
{
return;
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

int movFreq() {
//	pthread_mutex_lock(&mutex_serial);
//		vfoA.freq = FreqDisp->value();
//		selrig->set_vfoA(vfoA.freq);
//	pthread_mutex_unlock(&mutex_serial);
//	wait_query = true;
//	send_new_freq();
//	wait_query = false;
	return 1;
}

void cbABactive()
{
	if (!vfoB.freq) return;
	static  FREQMODE temp;

	wait_query = true;

	pthread_mutex_lock(&mutex_serial);
		temp = vfoA;
		vfoA = vfoB;
		vfoB = temp;

		FreqDisp->value(vfoA.freq);

		snprintf(szVfoB, sizeof(szVfoB), "%13.3f", vfoB.freq / 1000.0);
		txtInactive->label(szVfoB);
		txtInactive->redraw_label();

		selrig->set_vfoA(vfoA.freq);
		if (vfoA.imode != vfoB.imode) {
			opMODE->index(vfoA.imode);
			updateBandwidthControl();
			selrig->set_mode(vfoA.imode);
		}
		if (vfoA.iBW != vfoB.iBW) {
			opBW->index(vfoA.iBW);
			selrig->set_bandwidth(vfoA.iBW);
		}
	pthread_mutex_unlock(&mutex_serial);

	send_new_freq();
	if (vfoA.imode != vfoB.imode)
		send_mode_changed();
		send_sideband();
	if (bws_changed) {
		send_bandwidths();
		send_bandwidth_changed();
	}

	wait_query = false;

}

void cbA2B()
{
	if (Fl::event_button() == FL_RIGHT_MOUSE) {
		cbABactive();
		return;
	}
	vfoB.freq = FreqDisp->value();
	vfoB.imode = opMODE->index();
	vfoB.iBW = opBW->index();
	snprintf(szVfoB, sizeof(szVfoB), "%13.3f", vfoB.freq / 1000.0);
	txtInactive->label(szVfoB);
	txtInactive->redraw_label();
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
	pthread_mutex_lock(&mutex_serial);
		wait_query = true;

		vfoA.freq  = oplist[n].freq;
		vfoA.imode = oplist[n].imode;
		vfoA.iBW   = oplist[n].iBW;

		FreqDisp->value(vfoA.freq);
		selrig->set_vfoA(vfoA.freq);

		opMODE->index(vfoA.imode);
		updateBandwidthControl();
		selrig->set_mode(vfoA.imode);

		opBW->index(vfoA.iBW);
		selrig->set_bandwidth(vfoA.iBW);

		send_new_freq();
		send_mode_changed();
		if (bws_changed) send_bandwidths();
		send_bandwidth_changed();
		send_sideband();
		wait_query = false;

	pthread_mutex_unlock(&mutex_serial);
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
	long freq = FreqDisp->value();
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
	FreqDisp->visual_beep();
}

void cbRIT()
{
	if (selrig->has_rit)
		selrig->setRit((int)cntRIT->value());
}

void cbXIT()
{
	if (selrig->has_xit)
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
	if (btnPTT->value() == 1) {
		PTT = true;
		localptt = true;
	} else {
		PTT = false;
		localptt = false;
	}

	if (localptt) {
		wait_query = true;
		rigPTT(PTT);
		wait_query = false;
	} else {
		wait_query = true;
		send_ptt_changed(PTT);
		rigPTT(PTT);
		wait_query = false;
	}

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
	double data = (long)d;
	Fl_Image *img = btnALC_SWR->image();
	if (img == &image_alc) {
		sldrALC_SWR->value(data);
		sldrALC_SWR->redraw();
	}
}

void updateSWR(void * d)
{
	double data = (long)d;
	Fl_Image *img = btnALC_SWR->image();
	if (img == &image_swr) {
		sldrALC_SWR->value(data);
		sldrALC_SWR->redraw();
	}
}

float fp_ = 0.0, rp_ = 0.0;

void updateFwdPwr(void *d)
{
	double power = (long)d;
	if (!sldrFwdPwr->visible()) {
		sldrRcvSignal->hide();
		sldrFwdPwr->show();
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

void setFreqDisp(void *d)
{
	FreqDisp->value(vfoA.freq);
	FreqDisp->redraw();
	wait_query = true;
	send_new_freq();
	wait_query = false;
}

void updateSmeter(void *d) // 0 to 100;
{
	double swr = (long)d;
	if (!sldrRcvSignal->visible()) {
		sldrFwdPwr->hide();
		sldrRcvSignal->show();
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
	PTT = d;
	btnPTT->value(val);
	rigPTT(val);
}


void closeRig()
{
	// restore initial rig settings
	pthread_mutex_lock(&mutex_serial);
	selrig->setVfoAdj(0);
	selrig->set_vfoA(transceiver_freq);
	selrig->set_mode(transceiver_mode);
	selrig->set_bandwidth(transceiver_bw);
	selrig->shutdown();
	pthread_mutex_unlock(&mutex_serial);
}


void cbExit()
{
	progStatus.rig_nbr = rig_nbr;

	progStatus.freq = vfoA.freq;
	progStatus.opMODE = vfoA.imode;
	progStatus.opBW = vfoA.iBW;

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

	progStatus.saveLastState();

	saveFreqList();

//	closeRig();
	// shutdown serial thread

	pthread_mutex_lock(&mutex_serial);
		run_serial_thread = false;
	pthread_mutex_unlock(&mutex_serial);
	pthread_join(*serial_thread, NULL);

	selrig->setVfoAdj(0);
	selrig->set_vfoA(transceiver_freq);
	selrig->set_mode(transceiver_mode);
	selrig->set_bandwidth(transceiver_bw);
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
	Fl_Image *img = btnALC_SWR->image();
	if (img == &image_swr)
		btnALC_SWR->image(image_alc);
	else
		btnALC_SWR->image(image_swr);
	btnALC_SWR->redraw();
}

void about()
{
	string msg = "\
%s\n\
Version %s\n\
copyright W1HKJ, 2009\n\
w1hkj@@w1hkj.com";
	fl_message(msg.c_str(), PACKAGE_TARNAME, PACKAGE_VERSION);
}

void adjust_control_positions()
{
	int y = 118;
	if (selrig->has_rit || selrig->has_xit || selrig->has_bfo) {
		y += 20;
		cntRIT->position( cntRIT->x(), y );
		cntXIT->position( cntXIT->x(), y );
		cntBFO->position( cntBFO->x(), y );
		cntRIT->redraw();
		cntXIT->redraw();
		cntBFO->redraw();
	}
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
		if (selrig->has_compression) {
			cnt_compression->activate();
			btnCompON->activate();
		} else {
			cnt_compression->deactivate();
			btnCompON->deactivate();
		}
		cnt_line_out->deactivate();
		mnuKeepData->activate();
	}
}

void initRig()
{
	wait_query = true;

// disable the serial thread
	pthread_mutex_lock(&mutex_serial);

	selrig->initialize();
	transceiver_freq = selrig->get_vfoA();
	transceiver_mode = selrig->get_mode();
	transceiver_bw = selrig->get_bandwidth();

	if (selrig->restore_mbw) selrig->last_bw = transceiver_bw;

	if (progStatus.use_rig_data) {
		progStatus.freq = transceiver_freq;
		progStatus.opMODE = transceiver_mode;
		progStatus.opBW = transceiver_bw;
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
		opMODE->index(progStatus.opMODE);
		updateBandwidthControl();
		selrig->set_mode(progStatus.opMODE);
	} else {
		opMODE->add(" ");
		opMODE->index(0);
		opMODE->deactivate();
	}

	rigbws_.clear();
	opBW->clear();
	if (selrig->has_bandwidth_control) {
		selrig->adjust_bandwidth(vfoA.imode);
		old_bws = selrig->bandwidths_;
		for (int i = 0; selrig->bandwidths_[i] != NULL; i++) {
			rigbws_.push_back(selrig->bandwidths_[i]);
				opBW->add(selrig->bandwidths_[i]);
			}
		opBW->activate();
		opBW->index(progStatus.opBW);
		selrig->set_bandwidth(progStatus.opBW);
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
	} else {
		cnt_vfo_adj->deactivate();
	}

	if (selrig->has_rit) {
		cntRIT->value(progStatus.rit_freq);
		cntRIT->activate();
		cntRIT->show();
	} else {
		cntRIT->deactivate();
		cntRIT->hide();
	}

	if (selrig->has_xit) {
		cntXIT->value(progStatus.xit_freq);
		cntXIT->activate();
		cntXIT->show();
	} else {
		cntXIT->deactivate();
		cntXIT->hide();
	}

	if (selrig->has_bfo) {
		cntBFO->value(progStatus.bfo_freq);
		cntBFO->activate();
		cntBFO->show();
	} else {
		cntBFO->deactivate();
		cntBFO->hide();
	}

	if (selrig->has_volume_control) {
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
		sldrMICGAIN->value(progStatus.mic_gain);
		selrig->set_mic_gain(progStatus.mic_gain);
		sldrMICGAIN->show();
	} else {
		sldrMICGAIN->hide();
	}

	if (selrig->has_power_control) {
		selrig->set_power_control(progStatus.power_level);
		sldrPOWER->value(progStatus.power_level);
		sldrPOWER->show();
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

	if (selrig->has_ptt_control) {
		btnPTT->show();
	} else {
		btnPTT->hide();
	}

	if (selrig->has_auto_notch) {
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

	vfoA.freq = progStatus.freq;
	vfoA.imode = progStatus.opMODE;
	vfoA.iBW = progStatus.opBW;
	FreqDisp->value( vfoA.freq );

	vfoB.freq = progStatus.freq_B;
	vfoB.imode = progStatus.imode_B;
	vfoB.iBW = progStatus.iBW_B;
	snprintf(szVfoB, sizeof(szVfoB), "%13.3f", vfoB.freq / 1000.0);
	txtInactive->label(szVfoB);
	txtInactive->redraw();

	selrig->set_vfoA(vfoA.freq);
	opMODE->index( vfoA.imode );
	updateBandwidthControl();
	selrig->set_mode(vfoA.imode);

	opBW->index( vfoA.iBW );
	selrig->set_bandwidth(vfoA.iBW);

	// enable the serial thread
	pthread_mutex_unlock(&mutex_serial);

	fldigi_online = false;
	rig_reset = true;
	wait_query = false;
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
	selectCommPort->index(0);
	rigbase *selrig = rigs[selectRig->index()];

	progStatus.loadXcvrState(selrig->name_);

	selectCommPort->value(progStatus.xcvr_serial_port.c_str());
	btnOneStopBit->value( progStatus.stopbits == 1 );
	btnTwoStopBit->value( progStatus.stopbits == 2 );

	mnuBaudrate->index( selrig->comm_baudrate );
	btnOneStopBit->value( selrig->stopbits == 1 );
	btnTwoStopBit->value( selrig->stopbits == 2 );
	cntRigCatRetries->value( selrig->comm_retries );
	cntRigCatTimeout->value( selrig->comm_timeout );
	cntRigCatWait->value( selrig->comm_wait );
	btnRigCatEcho->value( selrig->comm_echo );
	btncatptt->value( selrig->comm_catptt );
	btnrtsptt->value( selrig->comm_rtsptt );
	btndtrptt->value( selrig->comm_dtrptt );
	chkrtscts->value( selrig->comm_rtscts );
	btnrtsplus->value( selrig->comm_rtsplus );
	btndtrplus->value( selrig->comm_dtrplus );
}

void initStatusConfigDialog()
{
	rig_nbr = progStatus.rig_nbr;
	selrig = rigs[rig_nbr];

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

//	vfoB.freq = progStatus.freq_B;
//	vfoB.imode = progStatus.imode_B;
//	vfoB.iBW = progStatus.iBW_B;

//	snprintf(szVfoB, sizeof(szVfoB), "%13.3f", vfoB.freq / 1000.0);
//	txtInactive->label(szVfoB);
//	txtInactive->redraw();

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
	selrig->set_if_shift(selrig->pbt);
}

void cb_special()
{
	selrig->set_special(btnSpecial->value());
}
