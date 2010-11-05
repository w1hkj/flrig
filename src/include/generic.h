#ifndef _GENERIC_H
#define _GENERIC_H

#include "rig_io.h"

extern RIG  RIG_GENERIC;
extern const char GENERIC_name[];
extern const char *GENERICmodes_[];
extern const char *GENERIC_widths[];

extern long GENERIC_get_vfoA();
extern void GENERIC_set_vfoA(long);
extern int  GENERIC_get_smeter();
extern int  GENERIC_get_swr();
extern int  GENERIC_get_power_out();
extern int  GENERIC_get_power_control();
extern void GENERIC_set_volume_control(double val);
extern void GENERIC_set_power_control(double val);
extern void GENERIC_set_PTT_control(int val);
extern void GENERIC_tune_rig();
extern void GENERIC_set_attenuator(int val);
extern void GENERIC_set_preamp(int val);
extern void GENERIC_set_modeA(int val);
extern void GENERIC_set_modeA(int val);
extern void GENERIC_set_bwA(int val);

#endif
