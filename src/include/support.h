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

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>
#include <math.h>

#ifndef WIN32
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#endif

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include <vector>
#include <queue>

#include <FL/Fl.H>
#include <FL/Enumerations.H>
#include <FL/Fl_Tree.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/fl_draw.H>
#include <FL/fl_show_colormap.H>
#include <FL/fl_ask.H>

#include "rig.h"
#include "rigbase.h"
#include "rig_io.h"
#include "images.h"
#include "serial.h"
#include "status.h"

#include "icons.h"
#include "debug.h"
#include "gettext.h"
#include "dialogs.h"
#include "rigbase.h"
#include "ptt.h"
#include "ui.h"
#include "tod_clock.h"
#include "rig.h"
#include "rigs.h"
#include "K3_ui.h"
#include "KX3_ui.h"
#include "rigpanel.h"
#include "trace.h"
#include "cwio.h"
#include "fsk.h"
#include "fskioUI.h"
#include "rig_io.h"
#include "socket_io.h"
#include "xml_server.h"
#include "gpio_ptt.h"
#include "cmedia.h"
#include "tmate2.h"

#define LISTSIZE 200
#define ATAGSIZE 128 //21

extern bool flrig_abort;

extern XCVR_STATE *vfo, vfoA, vfoB, xcvr_vfoA, xcvr_vfoB;

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

extern const char **old_bws;

extern std::string printXCVR_STATE(XCVR_STATE &data);
extern std::string print_ab();
extern const char *print(XCVR_STATE &data);

extern void yaesu891UpdateA(XCVR_STATE * newVfo);
extern void yaesu891UpdateB(XCVR_STATE * newVfo);
extern void redrawAGC();

extern void serviceA(XCVR_STATE nuvals);
extern void serviceB(XCVR_STATE nuvals);

extern void cbExit();

extern void setVolumeControl(void *);
extern void setModeControl(void *);
extern void set_bandwidth_control();
extern void updateBandwidthControl(void *d = NULL);
extern void updateUI(void *);
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
extern void update_power_control(void *);

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
extern void execute_FA2FB();
extern void execute_FB2FA();
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
extern void cb_set_break_in();

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
extern void doPTT(int);
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

extern void createXcvrDialog();
extern void open_poll_tab();
extern void open_trace_tab();
extern void open_commands_tab();
extern void open_restore_tab();
extern void open_send_command_tab();
extern void show_controls();

extern void open_xmlrpc_tab();
extern void open_tcpip_tab();
extern void open_cmedia_tab();
extern void open_ptt_tab();
extern void open_gpio_tab();
extern void open_aux_tab();
extern void open_server_tab();
extern void open_tmate2_tab();

extern void cb_close_TT550_setup();
extern void cbUseRepeatButtons(bool on);
extern void nr_label(const char *, int on = 0);
extern void nb_label(const char *, int on = 0);
extern void preamp_label(const char *, int on = 0);
extern void atten_label(const char *, int on = 0);
extern void auto_notch_label(const char *, int on = 0);
extern void enable_yaesu_bandselect(int btn_num, bool enable = true);

extern void break_in_label(const char *);

extern void synchronize_now();

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
extern void cbVoltMeterColor();

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

extern void cb_tab_defaults();
extern void cb_tab_colors();

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

extern void setFreqDispA(void *d = 0);
extern void setFreqDispB(void *d = 0);
extern void updateSmeter(void *d = 0);
extern void updateFwdPwr(void *d = 0);
extern void updateALC(void *d = 0);
extern void updateSWR(void *d = 0);
extern void updateVmeter(void *d = 0);
extern void updateSquelch(void *d = 0);
extern void updateRFgain(void *d = 0);
extern void zeroXmtMeters(void *d = 0);
extern void set_power_controlImage(double);

extern void cb_send_command(std::string cmd, Fl_Output *resp = 0);
extern const std::string lt_trim(const std::string& pString, const std::string& pWhitespace = " \"\t");
extern void editAlphaTag();

extern bool rig_notch;
extern int rig_notch_val;
extern void update_notch(void *d);

extern double smtrval;
extern double pwrval;
extern double swrval;
extern double alcval;

extern std::queue<VFOQUEUE> srvc_reqs;

extern void trace(int n, ...); // all args of type const char *
extern void trace1(int n, ...); // all args of type const char *

extern std::string printXCVR_STATE(XCVR_STATE &data);

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

extern void update_progress(int);

extern bool xcvr_online;

extern Fl_Box *mtr_SMETER;
extern Fl_Box *mtr_PWR;
extern Fl_Box *mtr_SWR;
extern Fl_Box *mtr_ALC;
extern Fl_Box *mtr_VOLTS;

extern Fl_SigBar *sigbar_SMETER;
extern Fl_SigBar *sigbar_PWR;
extern Fl_SigBar *sigbar_SWR;
extern Fl_SigBar *sigbar_ALC;
extern Fl_SigBar *sigbar_VOLTS;

// user interface support functions
extern void adjust_small_ui();
extern void adjust_xig_wide();
extern void adjust_wide_ui();
extern void adjust_touch_ui();
extern void update_UI_PTT(void *d = 0);
extern void adjust_control_positions();
extern void init_Generic_Tabs();
extern void initTabs();
extern void init_rit();
extern void init_xit();
extern void init_bfo();
extern void init_dsp_controls();
extern void init_volume_control();
extern void set_init_volume_control();
extern void init_rf_control();
extern void set_init_rf_gain();
extern void init_sql_control();
extern void set_init_sql_control();
extern void set_init_noise_reduction_control();
extern void set_init_if_shift_control();
extern void init_if_shift_control();
extern void init_notch_control();
extern void set_init_notch_control();
extern void init_micgain_control();
extern void set_init_micgain_control();
extern void init_power_control();
extern void set_init_power_control();
extern void init_attenuator_control();
extern void set_init_attenuator_control();
extern void init_agc_control();
extern void init_preamp_control();
extern void set_init_preamp_control();
extern void init_noise_control();
extern void init_split_control();
extern void set_init_noise_control();
extern void init_tune_control();
extern void init_ptt_control();
extern void init_auto_notch();
extern void set_init_auto_notch();
extern void init_swr_control();
extern void set_init_compression_control();
extern void set_init_break_in();
extern void init_special_controls();
extern void init_external_tuner();
extern void init_CIV();
extern void init_TS990_special();
extern void init_K3_KX3_special();

extern void initRig();
extern void initRigCombo();
extern void init_title();
extern void initConfigDialog();
extern void initStatusConfigDialog();

extern void start_commands();
extern void exit_commands();

// Xcvr initialization functions
extern void init_xcvr();
extern void init_generic_rig();
extern void enable_xcvr_ui();
extern void vfo_startup_data(XCVR_STATE &xcvrvfo);
extern void vfoA_startup_data();
extern void vfoB_startup_data();
extern void rig_startup_data();

// Xcvr restore functions
extern void restore_rig_vals_(XCVR_STATE &xcvrvfo);
extern void restore_xcvr_vals();

// TT550 initialization functions
extern void init_TT550_tabs();
extern void init_TT550();

#endif
