#ifndef SUPPORT_H
#define SUPPORT_H

#include <fstream>
#include <vector>
#include <string>

#include <math.h>
#ifndef WIN32
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#endif

#include "rig.h"
#include "rigbase.h"
#include "rig_io.h"
#include "images.h"
#include "serial.h"
#include "status.h"

#include <FL/fl_show_colormap.H>
#include <FL/fl_ask.H>

#define LISTSIZE 200

struct FREQMODE {
	long freq;
	int imode;
	int iBW;
};

extern FREQMODE vfoA;
extern FREQMODE vfoB;
extern bool PTT;
extern bool localptt;
extern bool remoteptt;

extern Cserial RigSerial;
extern Cserial AuxSerial;
extern Cserial SepSerial;

extern bool run_xmlrcp_thread;
extern bool bandwidth_changed;
extern bool modes_changed;
extern bool bandwidths_changed;

extern std::vector<std::string> rigmodes_;
extern std::vector<std::string> rigbws_;

extern rigbase *selrig;

extern void cbExit();

extern void setVolumeControl(void *);
extern void setModeControl(void *);
extern void updateBandwidthControl(void *d = NULL);
extern void updateBW(void *);
extern void setBWControl(void *);
extern void setNotchControl(void *);
extern void setNotchButton(void *);
extern void setIFshiftButton(void *);
extern void setIFshiftControl(void *);
extern void setPTT( void *);
extern void setPreampControl(void *);
extern void setAttControl(void *);
extern void setNoiseControl(void *);
extern void setMicGainControl(void *);

extern void setMode();
extern void setBW();

extern void addFreq();
extern void delFreq();
extern void buildlist();
extern void clearList();
extern void saveFreqList();
extern void readList();
extern void selectFreq();
extern int  movFreq();
extern void adjustFreqs();
extern void cbABactive();
extern void cbA2B();
extern void cbRIT();
extern void cbXIT();
extern void cbBFO();
extern void cbAuxPort();
extern void cb_line_out();
extern void cb_agc_level();
extern void cb_cw_wpm();
extern void cb_cw_vol();
extern void cb_cw_spot();
extern void cb_vox_gain();
extern void cb_vox_anti();
extern void cb_vox_hang();
extern void cb_vox_onoff();
extern void cb_compression();
extern void setLower();
extern void setUpper();

// callbacks for tt550 transceiver
extern void cb_tt550_line_out();
extern void cb_tt550_agc_level();
extern void cb_tt550_cw_wpm();
extern void cb_tt550_cw_vol();
extern void cb_tt550_cw_spot();
extern void cb_tt550_vox_gain();
extern void cb_tt550_vox_anti();
extern void cb_tt550_vox_hang();
extern void cb_tt550_vox_onoff();
extern void cb_tt550_compression();

extern void closeRig();
extern void cbExit();

extern void about();

extern void setNotch();

extern void cbAttenuator();
extern void cbPreamp();
extern void cbNR();
extern void setNR();
extern void cbNoise();
extern void cbbtnNotch();
extern void setNotch();
extern void cbAN();
extern void setIFshift();
extern void cbIFsh();
extern void setVolume();
extern void setMicGain();
extern void cbbtnMicLine();
extern void setPower();
extern void setRFGAIN();
extern void setSQUELCH();

extern void cbTune();
extern void cbPTT();
//extern void cbSmeter();
extern void cbALC_SWR();
//extern void cbPWR();
extern void cbMute();
extern void cbEventLog();

extern void loadConfig();
extern void saveConfig();
extern void loadState();
extern void saveState();
extern void initRig();
extern void init_title();
extern void initConfigDialog();
extern void initStatusConfigDialog();
extern void initRigCombo();
extern void createXcvrDialog();
extern void show_controls();
extern void cbUseRepeatButtons(bool on);
extern void preamp_label(const char *, bool on);
extern void atten_label(const char *, bool on);

// Display Dialog
extern void cbOkXcvrDialog();
extern void cbCancelXcvrDialog();
extern void initCommPortTable ();
extern void configXcvr();
extern void setDisplayColors();
extern void openMemoryDialog();
extern void cbCloseMemory();

extern void cbOkDisplayDialog();
extern void cbCancelDisplayDialog();
extern void cbPrefBackground();
extern void cbPrefForeground();

extern void cbSMeterColor();
extern void cbPwrMeterColor();
extern void cbSWRMeterColor();
extern void cbBacklightColor();

extern void * serial_thread_loop( void * );
extern bool bypass_serial_thread_loop;
extern bool bypass_xmlrcp_thread_loop;

extern Cserial RigSerial;
extern Cserial AuxSerial;

extern char szttyport[];
extern int  baudttyport;

extern void serial_timer(void *);

extern void init_xmlrpc();

extern void setFreqDisp(void *d);
extern void updateSmeter(void *d);
extern void updateFwdPwr(void *d);
extern void updateALC(void *d);
extern void updateSWR(void *d);
extern void updateSquelch(void *d);
extern void updateRFgain(void *d);
extern void zeroXmtMeters(void *d);
extern void reset_power_controlImage( void *d );
#endif
