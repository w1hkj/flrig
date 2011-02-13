#ifndef _FT890_H
#define _FT890_H

#include "rigbase.h"

// UNFINISHED !!

extern RIG  RIG_FT890;

extern const char FT890name_[];
extern const char *FT890modes_[];
extern const char *FT890_widths[];

extern void FT890_init();
extern long FT890_get_vfoA();
extern void FT890_set_vfoA(long);
extern int  FT890_get_smeter();
extern int  FT890_get_power_out();
extern void FT890_set_PTT_control(int val);
extern void FT890_tune_rig();
extern void FT890_set_modeA(int val);
extern int  FT890_get_modeA();

#endif
