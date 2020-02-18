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

#ifndef SUPPORT_H
#define SUPPORT_H

#include <fstream>
#include <vector>
#include <queue>
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
#include <FL/Fl_Tree.H>

#include "rig.h"
#include "rigbase.h"
#include "rig_io.h"
#include "images.h"
#include "serial.h"
#include "status.h"

#define LISTSIZE 200
#define ATAGSIZE 21

extern bool flrig_abort;

extern XCVR_STATE *vfo;
extern XCVR_STATE vfoA;
extern XCVR_STATE vfoB;
//extern XCVR_STATE xmlvfo;

extern bool useB;
extern bool PTT;
extern bool localptt;

extern Cserial *RigSerial;
extern Cserial *AuxSerial;
extern Cserial *SepSerial;

extern bool run_xmlrcp_thread;
extern bool bandwidth_changed;
extern bool modes_changed;
extern bool bandwidths_changed;

extern std::vector<std::string> rigmodes_;
extern std::vector<std::string> rigbws_;

extern rigbase *selrig;

extern void serviceA(XCVR_STATE nuvals);
extern void serviceB(XCVR_STATE nuvals);

extern void cbExit();

extern void setVolumeControl(void *);
extern void setModeControl(void *);
extern void set_bandwidth_control();
extern void updateBandwidthControl(void *d = NULL);
extern void updateBW(void *);
extern void update_split(void *);
extern void setBWControl(void *);
extern void setNotchControl(void *);
extern void adjust_if_shift_control(void *);
extern void setIFshiftButton(void *);
extern void setIFshiftControl(void *);
extern void setPTT( void *);
extern void setPreampControl(void *);
extern void setAttControl(void *);
extern void setMicGainControl(void *);
extern void setAGC(void *);
extern void setRFGAINControl(void *);

extern void setMode();
extern void setBW();
extern void setDSP();
extern void selectDSP();
extern void selectFILT();

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
extern void execute_swapAB();
extern void execute_A2B();
extern void cbAswapB();
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
extern void cb_cw_delay();
extern void cb_cw_weight();
extern void cb_enable_keyer();
extern void cb_enable_break_in();

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
extern void  rotate_log(std::string);

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
extern void setLOCK();
extern void setINNER();
extern void setOUTER();
extern void setCLRPBT();
extern void setVolume();
extern void setMicGain();
extern void cbbtnMicLine();
extern void setPower();
extern void setRFGAIN();
extern void setSQUELCH();

extern void cbTune();
extern void cb_tune_on_off();
extern void cbPTT();
//extern void cbSmeter();
extern void cbALC_SWR();
//extern void cbPWR();
extern void cbMute();
extern void cbAGC();

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
extern void open_trace_tab();
extern void open_commands_tab();
extern void open_restore_tab();
extern void open_send_command_tab();
extern void show_controls();

extern void open_xmlrpc_tab();
extern void open_tcpip_tab();
extern void open_ptt_tab();
extern void open_aux_tab();

extern void cb_close_TT550_setup();
extern void cbUseRepeatButtons(bool on);
extern void nr_label(const char *, bool on);
extern void nb_label(const char *, bool on);
extern void preamp_label(const char *, bool on);
extern void atten_label(const char *, bool on);
extern void auto_notch_label(const char *,bool on);
extern void enable_yaesu_bandselect(int btn_num, bool enable = true);

extern void break_in_label(const char *);

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
extern void cb_change_hrd_button();
extern void cb_change_sliders_button();
extern void set_sliders_when();

// ic7610 functions

extern void digi_sel_on_off();
extern void set_ic7610_digi_sel_on_off(void *);
extern void digi_sel_val();
extern void set_ic7610_digi_sel_val(void *);
extern void dual_watch();
extern void set_ic7610_dual_watch(void *);
extern void index_att();
extern void set_ic7610_index_att(void *);

extern Fl_Color flrig_def_color(int);

extern void * serial_thread_loop( void * );
extern bool bypass_serial_thread_loop;
extern bool bypass_xmlrcp_thread_loop;

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

extern void cb_send_command(string cmd, Fl_Output *resp = 0);
extern const string lt_trim(const string& pString, const string& pWhitespace = " \"\t");
extern void editAlphaTag();

extern void adjust_control_positions();

extern bool rig_notch;
extern int rig_notch_val;
extern void update_notch(void *d);

extern int mval;

extern queue<VFOQUEUE> srvc_reqs;

extern void trace(int n, ...); // all args of type const char *
extern void trace1(int n, ...); // all args of type const char *

extern string printXCVR_STATE(XCVR_STATE &data);

extern Fl_Group *tabXCVR;
extern Fl_Group *tabTRACE;
extern Fl_Group *tabTCPIP;
extern Fl_Group *tabPTT;
extern Fl_Group *tabAUX;
extern Fl_Group *tabPOLLING;
extern Fl_Group *tabSNDCMD;
extern Fl_Group *tabCOMMANDS;
extern Fl_Group *tabRESTORE;

extern Fl_Tree *tab_tree;

extern void select_tab(const char *);

#endif
