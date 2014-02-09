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
