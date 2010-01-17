// generated by Fast Light User Interface Designer (fluid) version 1.0110

#ifndef rigpanel_h
#define rigpanel_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <stdlib.h>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Tooltip.H>
#include "FreqControl.h"
extern cFreqControl *FreqDisp;
#include <FL/Fl_Light_Button.H>
extern Fl_Light_Button *btnVol;
#include "ValueSlider.h"
extern Fl_Wheel_Value_Slider *sldrVOLUME;
extern Fl_Wheel_Value_Slider *sldrRFGAIN;
extern Fl_Wheel_Value_Slider *sldrSQUELCH;
extern Fl_Light_Button *btnNR;
extern Fl_Wheel_Value_Slider *sldrNR;
extern Fl_Light_Button *btnIFsh;
extern Fl_Wheel_Value_Slider *sldrIFSHIFT;
extern Fl_Light_Button *btnNotch;
extern Fl_Wheel_Value_Slider *sldrNOTCH;
extern Fl_Wheel_Value_Slider *sldrMICGAIN;
extern Fl_Wheel_Value_Slider *sldrPOWER;
#include <FL/Fl_Box.H>
extern Fl_Box *txtInactive;
#include <FL/Fl_Button.H>
extern Fl_Button *btnA2B;
extern Fl_Button *btnABactive;
#include "combo.h"
extern Fl_ComboBox *opMODE;
extern Fl_ComboBox *opBW;
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
extern Fl_SigBar *sldrALC_SWR;
extern Fl_SigBar *sldrFwdPwr;
extern Fl_Box *scalePower;
extern Fl_Box *boxControl;
extern Fl_Light_Button *btnAuxRTS;
extern Fl_Light_Button *btnAuxDTR;
extern Fl_Light_Button *btnMicLine;
#include <FL/Fl_Counter.H>
extern Fl_Counter *cntRIT;
extern Fl_Counter *cntXIT;
extern Fl_Counter *cntBFO;
extern Fl_Button *btnInitializing;
Fl_Double_Window* Rig_window();
extern Fl_Menu_Item menu_[];
#define mnuExit (menu_+1)
#define mnuConfig (menu_+3)
#define mnuControls (menu_+4)
#define mnuKeepData (menu_+5)
#define mnuColorConfig (menu_+6)
#define mnuTooltips (menu_+7)
#define mnuConfigXcvr (menu_+8)
#define mnuStore (menu_+10)
#define mnuStored (menu_+11)
#define mnuHelp (menu_+14)
#define mnuAbout (menu_+15)
#define mnuEventLog (menu_+16)
extern Fl_Group *grpMeterColor;
extern Fl_Box *scaleSmeterColor;
extern Fl_SigBar *sldrRcvSignalColor;
extern Fl_Button *btnBacklight;
extern Fl_Button *btMeterColor;
extern Fl_Button *btnSWRColor;
extern Fl_Button *btnPwrColor;
extern Fl_Button *btnCancel;
#include <FL/Fl_Return_Button.H>
extern Fl_Return_Button *btnOkDisplayDialog;
extern Fl_Box *lblTest;
extern Fl_Button *prefForeground;
extern Fl_Button *prefBackground;
Fl_Double_Window* DisplayDialog();
extern Fl_Button *btnAddFreq;
extern Fl_Button *btnDelFreq;
extern Fl_Button *btnClearList;
#include <FL/Fl_Browser.H>
extern Fl_Browser *FreqSelect;
Fl_Double_Window* Memory_Dialog();
#include <FL/Fl_Tabs.H>
extern Fl_ComboBox *selectRig;
extern Fl_Counter *cntRigCatRetries;
extern Fl_Counter *cntRigCatTimeout;
extern Fl_Counter *cntRigCatWait;
extern Fl_Counter *query_interval;
extern Fl_ComboBox *selectCommPort;
extern Fl_ComboBox *mnuBaudrate;
#include <FL/Fl_Check_Button.H>
extern Fl_Check_Button *btnTwoStopBit;
extern Fl_Check_Button *btnOneStopBit;
extern Fl_Check_Button *btnRigCatEcho;
#include <FL/Fl_Input.H>
extern Fl_Input *server_port;
#include <FL/Fl_Round_Button.H>
extern Fl_Round_Button *btncatptt;
extern Fl_Round_Button *btnrtsptt;
extern Fl_Round_Button *btndtrptt;
extern Fl_Check_Button *chkrtscts;
extern Fl_Check_Button *btnrtsplus;
extern Fl_Check_Button *btndtrplus;
extern Fl_ComboBox *selectSepPTTPort;
extern Fl_Round_Button *btnSepRTSptt;
extern Fl_Check_Button *btnSepRTSplus;
extern Fl_Round_Button *btnSepDTRptt;
extern Fl_Check_Button *btnSepDTRplus;
extern Fl_ComboBox *selectAuxPort;
extern Fl_Button *btnCancelCommConfig;
extern Fl_Return_Button *btnOkXcvrDialog;
Fl_Double_Window* XcvrDialog();
extern Fl_Counter *cnt_cw_wpm;
extern Fl_Counter *cnt_cw_vol;
extern Fl_Counter *cnt_cw_spot;
extern Fl_Light_Button *btnSpot;
extern Fl_Counter *cnt_vox_gain;
extern Fl_Counter *cnt_anti_vox;
extern Fl_Counter *cnt_vox_hang;
extern Fl_Light_Button *btn_vox;
extern Fl_Light_Button *btnCompON;
extern Fl_Counter *cnt_compression;
extern Fl_ComboBox *cbo_agc_level;
extern Fl_Counter *cnt_line_out;
Fl_Double_Window* make_XcvrXtra();
extern Fl_Counter *cnt_tt550_cw_wpm;
extern Fl_Counter *cnt_tt550_cw_vol;
extern Fl_Counter *cnt_tt550_cw_spot;
extern Fl_Light_Button *btn_tt550_spot_onoff;
extern Fl_Counter *cnt_tt550_cw_weight;
extern Fl_Check_Button *btn_tt550_enable_keyer;
extern Fl_Counter *cnt_tt550_vox_gain;
extern Fl_Counter *cnt_tt550_anti_vox;
extern Fl_Counter *cnt_tt550_vox_hang;
extern Fl_Light_Button *btn_tt550_vox;
extern Fl_Light_Button *btn_tt550_CompON;
extern Fl_Counter *cnt_tt550_compression;
extern Fl_Counter *cnt_tt550_mon_vol;
extern Fl_Counter *cnt_tt550_line_out;
extern Fl_ComboBox *cbo_tt550_nb_level;
extern Fl_ComboBox *cbo_tt550_agc_level;
extern Fl_Check_Button *btn_tt550_enable_xmtr;
extern Fl_Check_Button *btn_tt550_enable_tloop;
extern Fl_Check_Button *btn_tt550_tuner_bypass;
Fl_Double_Window* make_TT550();
#endif
