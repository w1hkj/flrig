// generated by Fast Light User Interface Designer (fluid) version 1.0110

#ifndef rigpanel_h
#define rigpanel_h
#include <FL/Fl.H>
void select_power_meter_scales();
#include <FL/Fl_Double_Window.H>
#include <stdlib.h>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Tooltip.H>
#include <FL/Fl_Output.H>
extern Fl_Output *txt_encA;
#include "FreqControl.h"
extern cFreqControl *FreqDispA;
extern cFreqControl *FreqDispB;
#include <FL/Fl_Light_Button.H>
extern Fl_Light_Button *btnVol;
#include "ValueSlider.h"
extern Fl_Wheel_Value_Slider *sldrVOLUME;
extern Fl_Wheel_Value_Slider *sldrRFGAIN;
extern Fl_Wheel_Value_Slider *sldrSQUELCH;
extern Fl_Light_Button *btnIFsh;
extern Fl_Wheel_Value_Slider *sldrIFSHIFT;
extern Fl_Light_Button *btnNotch;
extern Fl_Wheel_Value_Slider *sldrNOTCH;
extern Fl_Light_Button *btnNR;
extern Fl_Wheel_Value_Slider *sldrNR;
extern Fl_Light_Button *btnDataPort;
extern Fl_Wheel_Value_Slider *sldrMICGAIN;
extern Fl_Wheel_Value_Slider *sldrPOWER;
#include <FL/Fl_Box.H>
extern Fl_Box *txtInactive;
extern Fl_Light_Button *btnA;
extern Fl_Light_Button *btnB;
#include <FL/Fl_Button.H>
extern Fl_Button *btn_K3_swapAB;
extern Fl_Button *btnA2B;
extern Fl_Light_Button *btnSplit;
#include "combo.h"
extern Fl_ComboBox *opBW;
extern Fl_Button *btnDSP;
extern Fl_ComboBox *opDSP_lo;
extern Fl_ComboBox *opDSP_hi;
extern Fl_ComboBox *opMODE;
extern Fl_Button *btn_show_controls;
extern Fl_Light_Button *btnAttenuator;
extern Fl_Light_Button *btnPreamp;
extern Fl_Light_Button *btnNOISE;
extern Fl_Light_Button *btnAutoNotch;
extern Fl_Button *btnTune;
extern Fl_Light_Button *btnPTT;
#include <FL/Fl_Group.H>
extern Fl_Group *grpMeters;
extern Fl_Box *scaleSmeter;
extern Fl_Button *btnALC_SWR;
#include "Fl_SigBar.h"
extern Fl_SigBar *sldrRcvSignal;
extern Fl_SigBar *sldrALC;
extern Fl_SigBar *sldrSWR;
extern Fl_SigBar *sldrFwdPwr;
extern Fl_Button *scalePower;
extern Fl_Box *boxControl;
extern Fl_Light_Button *btnAuxRTS;
extern Fl_Light_Button *btnAuxDTR;
extern Fl_Light_Button *btnMicLine;
#include <FL/Fl_Counter.H>
extern Fl_Counter *cntRIT;
extern Fl_Counter *cntXIT;
extern Fl_Counter *cntBFO;
extern Fl_Button *btnInitializing;
#include <FL/Fl_Tabs.H>
extern Fl_Tabs *tabs550;
extern Fl_Group *tt550_CW;
extern Fl_Counter *cnt_tt550_cw_wpm;
extern Fl_Counter *cnt_tt550_cw_weight;
extern Fl_Counter *cnt_tt550_cw_qsk;
extern Fl_Counter *cnt_tt550_cw_vol;
extern Fl_Counter *cnt_tt550_cw_spot;
#include <FL/Fl_Check_Button.H>
extern Fl_Check_Button *btn_tt550_enable_keyer;
extern Fl_Group *tt550_VOX;
extern Fl_Counter *cnt_tt550_vox_gain;
extern Fl_Counter *cnt_tt550_anti_vox;
extern Fl_Counter *cnt_tt550_vox_hang;
extern Fl_Light_Button *btn_tt550_vox;
extern Fl_Group *tt550_Speech;
extern Fl_Light_Button *btn_tt550_CompON;
extern Fl_Counter *cnt_tt550_compression;
extern Fl_Counter *cnt_tt550_mon_vol;
extern Fl_Group *tt550_Audio;
extern Fl_Counter *cnt_tt550_line_out;
extern Fl_Check_Button *btnAccInp;
extern Fl_Group *tt550_Rx;
extern Fl_ComboBox *cbo_tt550_nb_level;
extern Fl_ComboBox *cbo_tt550_agc_level;
extern Fl_Counter *cnt_tt550_vfo_adj;
extern Fl_Group *tt550_Tx;
extern Fl_Check_Button *btn_tt550_enable_xmtr;
extern Fl_Check_Button *btn_tt550_enable_tloop;
extern Fl_Check_Button *btn_tt550_tuner_bypass;
extern Fl_ComboBox *op_tt550_XmtBW;
extern Fl_Check_Button *btn_tt550_use_xmt_bw;
extern Fl_Group *tt550_302A;
extern Fl_Counter *cnt_tt550_keypad_time_out;
extern Fl_Counter *cnt_tt550_encoder_sensitivity;
#include <FL/Fl_Choice.H>
extern Fl_Choice *sel_tt550_encoder_step;
extern Fl_Group *tt550_302B;
extern Fl_Choice *sel_tt550_F1_func;
extern Fl_Choice *sel_tt550_F2_func;
extern Fl_Choice *sel_tt550_F3_func;
extern Fl_Tabs *tabsGeneric;
extern Fl_Group *genericBands;
extern Fl_Button *btnBandSelect_1;
extern Fl_Button *btnBandSelect_2;
extern Fl_Button *btnBandSelect_3;
extern Fl_Button *btnBandSelect_4;
extern Fl_Button *btnBandSelect_5;
extern Fl_Button *btnBandSelect_6;
extern Fl_Button *btnBandSelect_7;
extern Fl_Button *btnBandSelect_8;
extern Fl_Button *btnBandSelect_9;
extern Fl_Button *btnBandSelect_10;
extern Fl_Button *btnBandSelect_11;
extern Fl_Button *btnBandSelect_12;
extern Fl_Group *genericCW;
extern Fl_Light_Button *btnSpot;
extern Fl_Counter *cnt_cw_spot_tone;
extern Fl_Counter *cnt_cw_qsk;
extern Fl_Counter *cnt_cw_weight;
extern Fl_Counter *cnt_cw_wpm;
extern Fl_Check_Button *btn_enable_keyer;
extern Fl_Group *genericVOX;
extern Fl_Counter *cnt_vox_gain;
extern Fl_Counter *cnt_anti_vox;
extern Fl_Counter *cnt_vox_hang;
extern Fl_Light_Button *btn_vox;
extern Fl_Check_Button *btn_vox_on_dataport;
extern Fl_Group *genericSpeech;
extern Fl_Light_Button *btnCompON;
extern Fl_Counter *cnt_compression;
extern Fl_Group *genericRx;
extern Fl_ComboBox *cbo_nb_level;
extern Fl_ComboBox *cbo_agc_level;
extern Fl_Counter *cnt_bpf_center;
extern Fl_Check_Button *btn_use_bpf_center;
extern Fl_Group *genericMisc;
extern Fl_Counter *cnt_vfo_adj;
extern Fl_Counter *cnt_line_out;
extern Fl_Light_Button *btnSpecial;
Fl_Double_Window* Rig_window();
extern Fl_Menu_Item menu_[];
#define mnuExit (menu_+1)
#define mnuConfig (menu_+3)
#define mnuRestoreData (menu_+4)
#define mnuKeepData (menu_+5)
#define mnuTooltips (menu_+6)
#define mnuSchema (menu_+7)
#define mnuColorConfig (menu_+8)
#define mnu_meter_filtering (menu_+9)
#define mnuConfigXcvr (menu_+10)
#define save_me (menu_+13)
#define mnuHelp (menu_+19)
#define mnuOnLineHelp (menu_+20)
#define mnuAbout (menu_+21)
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
#include <FL/Fl_Return_Button.H>
extern Fl_Return_Button *btnOkDisplayDialog;
extern Fl_Light_Button *btn_lighted;
extern Fl_Button *btn_lighted_default;
Fl_Double_Window* DisplayDialog();
extern Fl_Button *btnAddFreq;
extern Fl_Button *btnPickFreq;
extern Fl_Button *btnDelFreq;
extern Fl_Button *btnClearList;
#include <FL/Fl_Browser.H>
extern Fl_Browser *FreqSelect;
Fl_Double_Window* Memory_Dialog();
extern Fl_Tabs *tabsConfig;
extern Fl_Group *tabPrimary;
extern Fl_ComboBox *selectRig;
extern Fl_Counter *cntRigCatRetries;
extern Fl_Counter *cntRigCatTimeout;
extern Fl_Counter *cntRigCatWait;
extern Fl_Counter *query_interval;
extern Fl_ComboBox *selectCommPort;
extern Fl_ComboBox *mnuBaudrate;
extern Fl_Check_Button *btnTwoStopBit;
extern Fl_Check_Button *btnOneStopBit;
extern Fl_Check_Button *btnRigCatEcho;
#include <FL/Fl_Input.H>
extern Fl_Input *server_addr;
extern Fl_Input *server_port;
#include <FL/Fl_Round_Button.H>
extern Fl_Round_Button *btncatptt;
extern Fl_Round_Button *btnrtsptt;
extern Fl_Round_Button *btndtrptt;
extern Fl_Check_Button *chkrtscts;
extern Fl_Check_Button *btnrtsplus;
extern Fl_Check_Button *btndtrplus;
extern Fl_Input *txtCIV;
extern Fl_Button *btnCIVdefault;
extern Fl_Check_Button *btnUSBaudio;
extern Fl_Check_Button *btn_key_fldigi;
extern Fl_Group *tabPTT;
extern Fl_ComboBox *selectSepPTTPort;
extern Fl_Round_Button *btnSepRTSptt;
extern Fl_Check_Button *btnSepRTSplus;
extern Fl_Round_Button *btnSepDTRptt;
extern Fl_Check_Button *btnSepDTRplus;
extern Fl_Group *tabAux;
extern Fl_ComboBox *selectAuxPort;
extern Fl_Group *tabPolling;
#include <FL/Fl_Value_Input.H>
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
extern Fl_Button *btnClearAddControls;
extern Fl_Value_Input *poll_all;
extern Fl_Button *btnSetAllAdd;
extern Fl_Group *tabSndCmd;
#include "flinput2.h"
extern Fl_Input2 *txt_command;
extern Fl_Button *btn_send_command;
extern Fl_Output *txt_response;
extern Fl_Box *box_xcvr_connect;
extern Fl_Box *box_fldigi_connect;
extern Fl_Button *btnCancelCommConfig;
extern Fl_Return_Button *btnOkXcvrDialog;
Fl_Double_Window* XcvrDialog();
extern Fl_Wheel_Value_Slider *sldr_smeter_avg;
extern Fl_Wheel_Value_Slider *sldr_smeter_peak;
extern Fl_Wheel_Value_Slider *sldr_pout_avg;
extern Fl_Wheel_Value_Slider *sldr_pout_peak;
Fl_Double_Window* MetersDialog();
extern Fl_Button *sel_25;
extern Fl_Button *sel_100;
extern Fl_Button *sel_auto;
extern Fl_Button *sel_50;
extern Fl_Button *sel_200;
extern Fl_Output *pwr_scale_description;
Fl_Double_Window* power_meter_scale_select();
#endif
