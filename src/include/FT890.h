#ifndef _FT890_H
#define _FT890_H

#include "rigbase.h"

extern RIG  RIG_FT890;
extern const char FT890name_[];
extern const char *FT890modes_[];
extern const char *FT890_widths[];

extern void FT890_init();
extern long FT890_get_vfoA();
extern void FT890_set_vfoA(long);
extern int  FT890_get_smeter();
//extern int  FT890_get_swr();
extern int  FT890_get_power_out();
//extern int  FT890_get_power_control();
//extern void FT890_set_volume_control(double val);
//extern void FT890_set_power_control(double val);
extern void FT890_set_PTT_control(int val);
extern void FT890_tune_rig();
//extern void FT890_set_attenuator(int val);
//extern int  FT890_get_attenuator();
//extern void FT890_set_preamp(int val);
//extern int  FT890_get_preamp();
extern void FT890_set_modeA(int val);
extern int  FT890_get_modeA();
//extern int  FT890_get_modetype(int n);
//extern void FT890_set_bwA(int val);
//extern int  FT890_get_bwA();
//extern void FT890_set_if_shift(int val);
//extern int  FT890_get_if_shift();
//extern void FT890_get_if_min_max_step(int &min, int &max, int &step);
//extern void FT890_set_notch(bool on, int val);
//extern int  FT890_get_notch();
//extern void FT890_get_notch_min_max_step(int &min, int &max, int &step);
//extern void FT890_set_mic_gain(int val);
//extern int  FT890_get_mic_gain();
//extern void FT890_get_mic_min_max_step(int &min, int &max, int &step);

#endif
