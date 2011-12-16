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

#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/fl_show_colormap.H>
#include <FL/fl_ask.H>

#include "rig.h"
#include "rigbase.h"
#include "rig_io.h"
#include "images.h"
#include "serial.h"
#include "status.h"

#define LISTSIZE 200

extern bool flrig_abort;

extern FREQMODE vfo;
extern FREQMODE vfoA;
extern FREQMODE vfoB;
extern FREQMODE xmlvfo;

extern bool useB;
extern bool PTT;
extern bool localptt;

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
extern void adjust_if_shift_control(void *);
extern void setIFshiftButton(void *);
extern void setIFshiftControl(void *);
extern void setPTT( void *);
extern void setPreampControl(void *);
extern void setAttControl(void *);
extern void setMicGainControl(void *);

extern void setMode();
extern void setBW();
extern void setDSP();
extern void selectDSP();

extern void addFreq();
extern void delFreq();
extern void buildlist();
extern void clearList();
extern void saveFreqList();
extern void readList();
extern void selectFreq();
extern int  movFreqA();
extern int  movFreqB();
extern void adjustFreqs();
extern void cbABactive();
extern void cbA2B();
extern void cb_selectA();
extern void cb_selectB();
extern void highlight_vfo(void *);
extern void cb_set_split(int);
extern void cbRIT();
extern void cbXIT();
extern void cbBFO();
extern void cbAuxPort();
extern void cb_line_out();
extern void cb_agc_level();

extern void cb_cw_wpm();
extern void cb_cw_vol();
extern void cb_cw_spot();
extern void cb_cw_spot_tone();
extern void cb_cw_qsk();
extern void cb_cw_weight();
extern void cb_enable_keyer();

extern void cb_vox_gain();
extern void cb_vox_anti();
extern void cb_vox_hang();
extern void cb_vox_onoff();
extern void cb_vox_on_dataport();
extern void cb_compression();
extern void setLower();
extern void setUpper();
extern void cb_vfo_adj();
extern void cb_line_out();
extern void cb_bpf_center();
extern void cb_special();
extern void cb_nb_level();

extern void setFocus();

// callbacks for tt550 transceiver
extern void cb_tt550_line_out();
extern void cb_tt550_agc_level();
extern void cb_tt550_cw_wpm();
extern void cb_tt550_cw_vol();
extern void cb_tt550_cw_spot();
extern void cb_tt550_cw_weight();
extern void cb_tt550_spot_onoff();
extern void cb_tt550_enable_keyer();
extern void cb_tt550_vox_gain();
extern void cb_tt550_vox_anti();
extern void cb_tt550_vox_hang();
extern void cb_tt550_vox_onoff();
extern void cb_tt550_compression();
extern void cb_tt550_mon_vol();
extern void cb_tt550_tuner_bypass();
extern void cb_tt550_enable_xmtr();
extern void cb_tt550_enable_tloop();
extern void cb_tt550_nb_level();
extern void cb_tt550_use_line_in();
extern void cb_tt550_setXmtBW();
extern void cb_tt550_cw_qsk();

extern void closeRig();
extern void cbExit();

extern void about();

extern void setNotch();
extern void visit_URL(void* arg);

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
extern void cbBandSelect(int band);

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
extern void open_poll_tab();
extern void open_send_command_tab();
extern void show_controls();
extern void cb_close_TT550_setup();
extern void cbUseRepeatButtons(bool on);
extern void nb_label(const char *, bool on);
extern void preamp_label(const char *, bool on);
extern void atten_label(const char *, bool on);
extern void auto_notch_label(const char *,bool on);

// Display Dialog
extern void cbUSBaudio();
extern void cbCIVdefault();
extern void cbCIV();
extern void cbOkXcvrDialog();
extern void cbCancelXcvrDialog();
extern void initCommPortTable ();
extern void configXcvr();
extern void setColors();
extern void setDisplayColors();
extern void openMemoryDialog();
extern void cbCloseMemory();
extern void select_and_close();

extern void cbOkDisplayDialog();
extern void cbCancelDisplayDialog();
extern void cbPrefFont();
extern void cbPrefBackground();
extern void cbPrefForeground();

extern void cbSMeterColor();
extern void cbPwrMeterColor();
extern void cbSWRMeterColor();
extern void cbPeakMeterColor();
extern void cbBacklightColor();

extern void cb_sys_defaults();
extern void cb_sys_foreground();
extern void cb_sys_background();
extern void cb_sys_background2();

extern void cb_reset_display_dialog();
extern void cb_slider_background();
extern void cb_slider_select();
extern void cb_slider_defaults();

extern void cb_lighted_button();
extern void cb_lighted_default();

extern Fl_Color flrig_def_color(int);

extern void * serial_thread_loop( void * );
extern bool bypass_serial_thread_loop;
extern bool bypass_xmlrcp_thread_loop;

extern Cserial RigSerial;
extern Cserial AuxSerial;

extern char szttyport[];
extern int  baudttyport;

extern void serial_timer(void *);

extern void init_xmlrpc();

extern void setFreqDispA(void *d);
extern void setFreqDispB(void *d);
extern void updateSmeter(void *d);
extern void updateFwdPwr(void *d);
extern void updateALC(void *d);
extern void updateSWR(void *d);
extern void updateSquelch(void *d);
extern void updateRFgain(void *d);
extern void zeroXmtMeters(void *d);
extern void set_power_controlImage(double);
extern void update_UI_PTT(void *d = NULL);

extern void cb_send_command();

extern void adjust_control_positions();

#endif
