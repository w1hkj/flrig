// =====================================================================
// KX3 user interface support
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
// aunsigned long int with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#ifndef KX3_UI_H
#define KX3_UI_H

extern void read_KX3_vfo();
extern void read_KX3_mode();
extern void read_KX3_bw();

extern void KX3_set_split(int val);
extern void cb_KX3_A2B();
extern void cb_KX3_swapAB();
extern void cb_KX3_IFsh();

#endif
