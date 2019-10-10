//======================================================================
//
// flrig user interface include file
//
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


#ifndef UI_DIALOGS
#define UI_DIALOGS

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <stdlib.h>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Tooltip.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Radio_Light_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Simple_Counter.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Input_Choice.H>

#include "flinput2.h"
#include "Fl_SigBar.h"
#include "FreqControl.h"
#include "combo.h"
#include "ValueSlider.h"
#include "hspinner.h"
#include "pl_tones.h"

extern Fl_Double_Window* XcvrDialog();
extern Fl_Double_Window* MetersDialog();
extern Fl_Double_Window* power_meter_scale_select();
extern Fl_Double_Window* DisplayDialog();
extern Fl_Double_Window* Memory_Dialog();
extern Fl_Double_Window* Wide_rig_window();
extern Fl_Double_Window* Small_rig_window();
extern Fl_Double_Window* touch_rig_window();
extern Fl_Double_Window* tabs_window();

extern void select_power_meter_scales();

extern Fl_Output *txt_encA;

extern Fl_Group *grpMeters;
extern Fl_Box *scaleSmeter;

extern Fl_Button *btnALC_SWR;

extern Fl_SigBar *sldrRcvSignal;
extern Fl_SigBar *sldrALC;
extern Fl_SigBar *sldrSWR;
extern Fl_SigBar *sldrFwdPwr;
extern Fl_Button *scalePower;
extern Fl_Box *meter_fill_box;

extern cFreqControl *FreqDispA;
extern cFreqControl *FreqDispB;
extern Fl_Group *grp_row0a;

extern Fl_Light_Button *btnA;
extern Fl_Button *btn_K3_swapAB;
extern Fl_Button *btn_KX3_swapAB;
extern Fl_Light_Button *btnB;
extern Fl_Box *labelMEMORY;
extern Fl_Button *btnTune;
extern Fl_Light_Button *btn_tune_on_off;
extern Fl_Box *bx_row0_expander;
extern Fl_Button *btn_show_controls;
extern Fl_Group *grp_row1;
extern Fl_Group *grp_row1a;

extern Fl_ComboBox *opBW;
extern Fl_Button *btnDSP;
extern Fl_ComboBox *opDSP_lo;
extern Fl_ComboBox *opDSP_hi;
extern Fl_Button *btnFILT;
extern Fl_ComboBox *opMODE;
extern Fl_Group *grp_row1b;
extern Fl_Group *grp_row1b1;
extern Fl_Light_Button *btnAttenuator;
extern Fl_Light_Button *btnPreamp;
extern Fl_Light_Button *btnNOISE;
extern Fl_Light_Button *btnAutoNotch;
extern Fl_Group *grp_row1b2;
extern Fl_Button *btnAswapB;
extern Fl_Light_Button *btnSplit;
extern Fl_Light_Button *btnPTT;
extern Fl_Box *bx_row1_expander2;
extern Fl_Group *grp_row2;
extern Fl_Group *grp_row2a;

extern Fl_Wheel_Value_Slider *sldrSQUELCH;
extern Fl_Wheel_Value_Slider *sldrMICGAIN;
extern Fl_Box *bx_sep2a;
extern Fl_Group *grp_row2b;
extern Fl_Light_Button *btnIFsh;
extern Fl_Wheel_Value_Slider *sldrIFSHIFT;
extern Fl_Light_Button *btnNR;
extern Fl_Wheel_Value_Slider *sldrNR;
extern Fl_Box *bx_sep2b;
extern Fl_Group *grp_row2c;
extern Fl_Light_Button *btnNotch;
extern Fl_Wheel_Value_Slider *sldrNOTCH;
extern Fl_Light_Button *btnAGC;
extern Fl_Wheel_Value_Slider *sldrRFGAIN;
extern Fl_Box *bx_sep2c;
extern Fl_Group *grp_row2d;

extern Fl_Light_Button *btnLOCK;
extern Fl_Button *btnCLRPBT;
extern Fl_Wheel_Value_Slider *sldrINNER;
extern Fl_Wheel_Value_Slider *sldrOUTER;

extern Fl_Wheel_Value_Slider *sldrPOWER;

extern Fl_Light_Button *btnPOWER;
extern Hspinner *spnrPOWER;
extern Hspinner *spnrVOLUME;
extern Hspinner *spnrRFGAIN;
extern Hspinner *spnrSQUELCH;
extern Hspinner *spnrNR;
extern Hspinner *spnrIFSHIFT;
extern Hspinner *spnrNOTCH;
extern Hspinner *spnrMICGAIN;

extern Fl_Light_Button *btnVol;
extern Fl_Wheel_Value_Slider *sldrVOLUME;
extern Fl_Group *grpTABS;

extern Fl_Tabs *tabsGeneric;
extern Fl_Group *hidden_tabs;
extern Fl_Group *tab_yaesu_bands;
extern Fl_Button *btn_yaesu_select_1;
extern Fl_Button *btn_yaesu_select_2;
extern Fl_Button *btn_yaesu_select_3;
extern Fl_Button *btn_yaesu_select_4;
extern Fl_Button *btn_yaesu_select_5;
extern Fl_Button *btn_yaesu_select_6;
extern Fl_Button *btn_yaesu_select_7;
extern Fl_Button *btn_yaesu_select_8;
extern Fl_Button *btn_yaesu_select_9;
extern Fl_Button *btn_yaesu_select_10;
extern Fl_Button *btn_yaesu_select_11;
extern Fl_ComboBox *op_yaesu_select60;

extern Fl_Group *tab_FT8n_bands;
extern Fl_Button *btn_FT8n_select_1;
extern Fl_Button *btn_FT8n_select_2;
extern Fl_Button *btn_FT8n_select_3;
extern Fl_Button *btn_FT8n_select_4;
extern Fl_Button *btn_FT8n_select_5;
extern Fl_Button *btn_FT8n_select_6;
extern Fl_Button *btn_FT8n_select_7;
extern Fl_Button *btn_FT8n_select_8;
extern Fl_Button *btn_FT8n_select_9;
extern Fl_Button *btn_FT8n_select_10;
extern Fl_Button *btn_FT8n_select_11;
extern Fl_Button *btn_FT8n_select_12;
extern Fl_Button *btn_FT8n_select_13;

extern Fl_Group *tab_FT8n_CTCSS;
extern Fl_PL_tone *choice_FT8n_tTONE;
extern Fl_PL_tone *choice_FT8n_rTONE;
extern Fl_Button *setTONES;
extern Fl_ComboBox *FMoffset;
extern Fl_Button *setOFFSET;
extern cFreqControl *FMoff_freq;

extern Fl_Group *tab_icom_bands;
extern Fl_Button *btn_icom_select_1;
extern Fl_Button *btn_icom_select_2;
extern Fl_Button *btn_icom_select_3;
extern Fl_Button *btn_icom_select_4;
extern Fl_Button *btn_icom_select_5;
extern Fl_Button *btn_icom_select_6;
extern Fl_Button *btn_icom_select_7;
extern Fl_Button *btn_icom_select_8;
extern Fl_Button *btn_icom_select_9;
extern Fl_Button *btn_icom_select_10;
extern Fl_Button *btn_icom_select_11;
extern Fl_Button *btn_icom_select_12;
extern Fl_Button *btn_icom_select_13;

extern Fl_PL_tone *choice_tTONE;
extern Fl_PL_tone *choice_rTONE;

extern int PL_tones[50];
extern const char *szTONES;

extern Fl_Group *genericCW;
extern Fl_Light_Button *btnSpot;

extern Fl_Group *genericQSK;
extern Fl_Button *btnBreakIn;
extern Hspinner *spnr_cw_delay;
extern Hspinner *cnt_cw_qsk;

extern Fl_Group *tab7610;
extern Fl_ComboBox *ic7610att;
extern Fl_Light_Button *ic7610dual_watch;
extern Fl_Light_Button *ic7610digi_sel_on_off;
extern Fl_Wheel_Value_Slider *ic7610_digi_sel_val;

extern Fl_Group  *genericUser_1;
extern Fl_Button *btnUser1;
extern Fl_Button *btnUser2;
extern Fl_Button *btnUser3;
extern Fl_Button *btnUser4;
extern Fl_Button *btnUser5;
extern Fl_Button *btnUser6;
extern Fl_Button *btnUser7;
extern Fl_Button *btnUser8;
extern Fl_Output *cmdResponse;

extern Fl_Group  *genericUser_2;
extern Fl_Button *btnUser9;
extern Fl_Button *btnUser10;
extern Fl_Button *btnUser11;
extern Fl_Button *btnUser12;
extern Fl_Button *btnUser13;
extern Fl_Button *btnUser14;
extern Fl_Button *btnUser15;
extern Fl_Button *btnUser16;

extern Hspinner *spnr_cw_spot_tone;
extern Hspinner *spnr_cw_qsk;
extern Hspinner *spnr_cw_weight;
extern Hspinner *spnr_cw_wpm;

extern Fl_Check_Button *btn_enable_keyer;
extern Fl_Group *genericVOX;
extern Hspinner *spnr_vox_gain;
extern Hspinner *spnr_anti_vox;
extern Hspinner *spnr_vox_hang;
extern Fl_Light_Button *btn_vox;
extern Fl_Check_Button *btn_vox_on_dataport;
extern Fl_Group *genericSpeech;
extern Fl_Light_Button *btnCompON;
extern Hspinner *spnr_compression;
extern Fl_Group *genericRx;
extern Fl_Wheel_Value_Slider *sldr_nb_level;
//extern Fl_ComboBox *cbo_agc_level;
extern Hspinner *spnr_bpf_center;
extern Fl_Check_Button *btn_use_bpf_center;
extern Fl_Group *genericMisc;
extern Hspinner *spnr_vfo_adj;
extern Hspinner *spnr_line_out;
extern Fl_Light_Button *btnSpecial;
extern Fl_Check_Button *btn_ext_tuner;
extern Fl_Check_Button *btn_xcvr_auto_on;
extern Fl_Check_Button *btn_xcvr_auto_off;
extern Fl_Group *genericRXB;
extern Hspinner *cntRIT;
extern Hspinner *cntXIT;
extern Hspinner *cntBFO;
extern Fl_Group *genericAux;
extern Fl_Box *boxControl;
extern Fl_Light_Button *btnAuxRTS;
extern Fl_Light_Button *btnAuxDTR;
extern Fl_Light_Button *btnDataPort;
extern Fl_Tabs *tabs550;
extern Fl_Group *tt550_CW;
extern Hspinner *spnr_tt550_cw_wpm;
extern Hspinner *spnr_tt550_cw_weight;
extern Hspinner *spnr_tt550_cw_qsk;
extern Hspinner *spnr_tt550_cw_vol;
extern Hspinner *spnr_tt550_cw_spot;
extern Fl_Check_Button *btn_tt550_enable_keyer;
extern Fl_Group *tt550_VOX;
extern Hspinner *spnr_tt550_vox_gain;
extern Hspinner *spnr_tt550_anti_vox;
extern Hspinner *spnr_tt550_vox_hang;
extern Fl_Light_Button *btn_tt550_vox;
extern Fl_Group *tt550_Speech;
extern Fl_Light_Button *btn_tt550_CompON;
extern Hspinner *spnr_tt550_compression;
extern Hspinner *spnr_tt550_mon_vol;
extern Fl_Group *tt550_Audio;
extern Hspinner *spnr_tt550_line_out;
extern Fl_Check_Button *btnAccInp;
extern Fl_Group *tt550_Rx;
extern Fl_ComboBox *cbo_tt550_nb_level;
extern Fl_ComboBox *cbo_tt550_agc_level;
extern Hspinner *spnr_tt550_vfo_adj;
extern Fl_Group *tt550_Tx;
extern Fl_Check_Button *btn_tt550_enable_xmtr;
extern Fl_Check_Button *btn_tt550_enable_tloop;
extern Fl_Check_Button *btn_tt550_tuner_bypass;
extern Fl_ComboBox *op_tt550_XmtBW;
extern Fl_Check_Button *btn_tt550_use_xmt_bw;
extern Fl_Group *tt550_302A;
extern Hspinner *spnr_tt550_keypad_time_out;
extern Hspinner *spnr_tt550_encoder_sensitivity;

extern Fl_Choice *sel_tt550_encoder_step;
extern Fl_Group *tt550_302B;
extern Fl_Choice *sel_tt550_F1_func;
extern Fl_Choice *sel_tt550_F2_func;
extern Fl_Choice *sel_tt550_F3_func;
extern Fl_Light_Button *tt550_AT_inline;
extern Fl_Light_Button *tt550_AT_Z;

extern Fl_Group  *main_group;
extern Fl_Group  *grpInitializing;
extern Fl_Progress *progress;

extern Fl_Group *grp_menu;

extern Fl_Menu_Item *mnuExit;
extern Fl_Menu_Item *mnuConfig;
extern Fl_Menu_Item *mnuRestoreData;
extern Fl_Menu_Item *mnuKeepData;
extern Fl_Menu_Item *mnuTooltips;
extern Fl_Menu_Item *mnuSchema;
extern Fl_Menu_Item *mnuColorConfig;
extern Fl_Menu_Item *mnu_meter_filtering;
extern Fl_Menu_Item *mnuConfigXcvr;
extern Fl_Menu_Item *save_me;
extern Fl_Menu_Item *mnuHelp;
extern Fl_Menu_Item *mnuOnLineHelp;
extern Fl_Menu_Item *mnuAbout;
extern Fl_Box *tcpip_menu_box;
extern Fl_Group *tcpip_box;
extern Fl_Check_Button *chk_use_tcpip;

extern Fl_Radio_Light_Button *btn_wide_ui;
extern Fl_Radio_Light_Button *btn_narrow_ui;
extern Fl_Radio_Light_Button *btn_touch_ui;
extern Fl_Check_Button *chk_sliders_button;

extern Fl_Box *lblTest;
extern Fl_Button *prefFont;
extern Fl_Button *prefForeground;
extern Fl_Button *btnBacklight;
extern Fl_Group *grpMeterColor;
extern Fl_Box *scaleSmeterColor;
extern Fl_SigBar *sldrRcvSignalColor;
extern Fl_SigBar *sldrSWRcolor;
extern Fl_Box *scaleSWRcolor;
extern Fl_SigBar *sldrPWRcolor;
extern Fl_Box *scalePWRcolor;
extern Fl_Button *btMeterColor;
extern Fl_Button *btnSWRColor;
extern Fl_Button *btnPwrColor;
extern Fl_Button *btnPeakColor;
extern Fl_Choice *mnuScheme;
extern Fl_Button *pref_sys_foreground;
extern Fl_Button *pref_sys_background;
extern Fl_Button *prefsys_background2;
extern Fl_Button *prefsys_defaults;
extern Fl_Wheel_Value_Slider *sldrColors;
extern Fl_Button *pref_slider_background;
extern Fl_Button *pref_slider_select;
extern Fl_Button *prefslider_defaults;
extern Fl_Button *btnReset;
extern Fl_Button *btnCancel;

extern Fl_Return_Button *btnOkDisplayDialog;
extern Fl_Light_Button *btn_lighted;
extern Fl_Button *btn_lighted_default;

extern Fl_Button *btnAddFreq;
extern Fl_Button *btnPickFreq;
extern Fl_Button *btnDelFreq;
extern Fl_Button *btnClearList;

extern Fl_Browser *FreqSelect;

extern Fl_Input *inAlphaTag;

extern Fl_Tabs *tabsConfig;
extern Fl_Group *tabPrimary;
extern Fl_ComboBox *selectRig;

extern Fl_Group *tabCommands;
extern Fl_Tabs *tabCmds;
extern Fl_Group *tabCmds1;
extern Fl_Group *tabCmds2;
extern Fl_Input2 * cmdlbl1;
extern Fl_Input2 * cmdlbl2;
extern Fl_Input2 * cmdlbl3;
extern Fl_Input2 * cmdlbl4;
extern Fl_Input2 * cmdlbl5;
extern Fl_Input2 * cmdlbl6;
extern Fl_Input2 * cmdlbl7;
extern Fl_Input2 * cmdlbl8;
extern Fl_Input2 * cmdlbl9;
extern Fl_Input2 * cmdlbl10;
extern Fl_Input2 * cmdlbl11;
extern Fl_Input2 * cmdlbl12;
extern Fl_Input2 * cmdlbl13;
extern Fl_Input2 * cmdlbl14;
extern Fl_Input2 * cmdlbl15;
extern Fl_Input2 * cmdlbl16;
extern Fl_Input2 * cmdtext1;
extern Fl_Input2 * cmdtext2;
extern Fl_Input2 * cmdtext3;
extern Fl_Input2 * cmdtext4;
extern Fl_Input2 * cmdtext5;
extern Fl_Input2 * cmdtext6;
extern Fl_Input2 * cmdtext7;
extern Fl_Input2 * cmdtext8;
extern Fl_Input2 * cmdtext9;
extern Fl_Input2 * cmdtext10;
extern Fl_Input2 * cmdtext11;
extern Fl_Input2 * cmdtext12;
extern Fl_Input2 * cmdtext13;
extern Fl_Input2 * cmdtext14;
extern Fl_Input2 * cmdtext15;
extern Fl_Input2 * cmdtext16;

extern Fl_Counter *cntRigCatRetries;
extern Fl_Counter *cntRigCatTimeout;
extern Fl_Counter *cntRigCatWait;
extern Fl_Counter *query_interval;

extern Fl_ComboBox *selectCommPort;
extern Fl_ComboBox *mnuBaudrate;
extern Fl_Check_Button *btnTwoStopBit;
extern Fl_Check_Button *btnOneStopBit;
extern Fl_Check_Button *btnRigCatEcho;
extern Fl_Input *server_addr;

extern Fl_Int_Input *server_port;

extern Fl_Round_Button *btncatptt;
extern Fl_Round_Button *btnrtsptt;
extern Fl_Round_Button *btndtrptt;
extern Fl_Check_Button *chkrtscts;
extern Fl_Check_Button *btnrtsplus;
extern Fl_Check_Button *btndtrplus;
extern Fl_Int_Input *txtCIV;
extern Fl_Button *btnCIVdefault;
extern Fl_Check_Button *btnUSBaudio;
extern Fl_Check_Button *btn_key_fldigi;
extern Fl_Group *tabPTT;
extern Fl_ComboBox *selectSepPTTPort;
extern Fl_Check_Button *btnSep_SCU_17;
extern Fl_Check_Button *btnSepRTSptt;
extern Fl_Check_Button *btnSepRTSplus;
extern Fl_Check_Button *btnSepDTRptt;
extern Fl_Check_Button *btnSepDTRplus;
extern Fl_Group *tabAux;
extern Fl_ComboBox *selectAuxPort;
extern Fl_Check_Button *btnAux_SCU_17;
extern Fl_Group *tabPolling;
extern Fl_Group *tabTRACE;
extern Fl_Group *tabRestore;
extern Fl_Group *tabTCPIP;
extern Fl_Group *tabXMLRPC;

extern Fl_Value_Input *poll_smeter;
extern Fl_Value_Input *poll_pout;
extern Fl_Value_Input *poll_swr;
extern Fl_Value_Input *poll_alc;
extern Fl_Value_Input *poll_frequency;
extern Fl_Value_Input *poll_mode;
extern Fl_Value_Input *poll_bandwidth;
extern Fl_Value_Input *poll_volume;
extern Fl_Value_Input *poll_micgain;
extern Fl_Value_Input *poll_rfgain;
extern Fl_Value_Input *poll_power_control;
extern Fl_Value_Input *poll_ifshift;
extern Fl_Value_Input *poll_notch;
extern Fl_Value_Input *poll_auto_notch;
extern Fl_Value_Input *poll_pre_att;
extern Fl_Value_Input *poll_squelch;
extern Fl_Value_Input *poll_split;
extern Fl_Value_Input *poll_noise;
extern Fl_Value_Input *poll_nr;
extern Fl_Value_Input *poll_compression;
extern Fl_Button *btnClearAddControls;
extern Fl_Value_Input *poll_all;
extern Fl_Button *btnSetAllAdd;
extern Fl_Group *tabSndCmd;

extern Fl_Input2 *txt_command;
extern Fl_Button *btn_send_command;
extern Fl_Output *txt_response;
extern Fl_Box *box_xcvr_connect;
extern Fl_Box *box_fldigi_connect;
extern Fl_Box *box_tcpip_connect;

extern Fl_Button *btnCloseCommConfig;
extern Fl_Button *btnOkXcvrDialog;
extern Fl_Button *btnOkSepSerial;
extern Fl_Button *btnOkAuxSerial;

extern Fl_Counter *inp_tcpip_ping_delay;
extern Fl_Counter *cntRetryAfter;
extern Fl_Counter *cntDropsAllowed;

extern Fl_Wheel_Value_Slider *sldr_smeter_avg;
extern Fl_Wheel_Value_Slider *sldr_smeter_peak;
extern Fl_Wheel_Value_Slider *sldr_pout_avg;
extern Fl_Wheel_Value_Slider *sldr_pout_peak;

extern Fl_Button *sel_25;
extern Fl_Button *sel_100;
extern Fl_Button *sel_auto;
extern Fl_Button *sel_50;
extern Fl_Button *sel_200;
extern Fl_Output *pwr_scale_description;

extern Fl_Group *tabRestore;
extern Fl_Check_Button *btnRestoreFrequency;
extern Fl_Check_Button *btnRestoreMode;
extern Fl_Check_Button *btnRestoreBandwidth;
extern Fl_Check_Button *btnRestoreVolume;
extern Fl_Check_Button *btnRestoreMicGain;
extern Fl_Check_Button *btnRestoreRfGain;
extern Fl_Check_Button *btnRestorePowerControl;
extern Fl_Check_Button *btnRestoreIFshift;
extern Fl_Check_Button *btnRestoreNotch;
extern Fl_Check_Button *btnRestoreAutoNotch;
extern Fl_Check_Button *btnRestoreSquelch;
extern Fl_Check_Button *btnRestoreSplit;
extern Fl_Check_Button *btnRestorePreAtt;
extern Fl_Check_Button *btnRestoreNoise;
extern Fl_Check_Button *btnRestoreNR;
extern Fl_Check_Button *btnRestoreCompOnOff;
extern Fl_Check_Button *btnRestoreCompLevel;
extern Fl_Check_Button *btnUseRigData;

#endif
