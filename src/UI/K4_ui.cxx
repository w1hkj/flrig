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

#include <stdlib.h>
#include <iostream>
#include <fstream>

#include <vector>
#include <queue>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>

#include "support.h"
#include "debug.h"
#include "gettext.h"
#include "rig_io.h"
#include "dialogs.h"
#include "rigbase.h"
#include "ptt.h"

#include "rigs.h"
#include "K4_ui.h"

extern std::queue<XCVR_STATE> queA;
extern std::queue<XCVR_STATE> queB;

void read_K4_vfo()
{
	unsigned long int freq;
	freq = selrig->get_vfoA();
	if (freq != vfoA.freq) {
		vfoA.freq = freq;
		Fl::awake(setFreqDispA);
		vfo = &vfoA;
	}
	freq = selrig->get_vfoB();
	if (freq != vfoB.freq) {
		vfoB.freq = freq;
		Fl::awake(setFreqDispB);
	}
}

void read_K4_mode()
{
	int nu_mode;
	nu_mode = selrig->get_modeA();
	if (nu_mode != vfoA.imode) {
		vfoA.imode = vfo->imode = nu_mode;
		selrig->set_bwA(vfo->iBW = selrig->adjust_bandwidth(nu_mode));
		Fl::awake(setModeControl);
		Fl::awake(updateBandwidthControl);
	}
	nu_mode = selrig->get_modeB();
	if (nu_mode != vfoB.imode) {
		vfoB.imode = nu_mode;
	}
}

void read_K4_bw()
{
	int nu_BW;
	nu_BW = selrig->get_bwA();
	if (nu_BW != vfoA.iBW) {
		vfoA.iBW = vfo->iBW = nu_BW;
		Fl::awake(setBWControl);
	}
	nu_BW = selrig->get_bwB();
	if (nu_BW != vfoB.iBW) {
		vfoB.iBW = nu_BW;
	}
}


void K4_set_split(int val)
{
	guard_lock serial_lock(&mutex_serial);
	selrig->set_split(val);
}

void K4_A2B()
{
	guard_lock serial_lock(&mutex_serial);
	vfoB = vfoA;
	selrig->set_vfoB(vfoB.freq);
	selrig->set_bwB(vfoB.iBW);
	selrig->set_modeB(vfoB.imode);
	FreqDispB->value(vfoB.freq);
	Fl::focus(FreqDispA);
}

void cb_K4_swapAB()
{
	guard_lock serial_lock(&mutex_serial);

	XCVR_STATE temp = vfoA;
	vfoA = vfoB;
	vfoB = temp;

	selrig->set_vfoB(vfoB.freq);
	selrig->set_bwB(vfoB.iBW);
	selrig->set_modeB(vfoB.imode);
	FreqDispB->value(vfoB.freq);

	selrig->set_vfoA(vfoA.freq);
	selrig->set_bwA(vfoA.iBW);
	selrig->set_modeA(vfoA.imode);

	opBW->index(vfoA.iBW);
	opMODE->index(vfoA.imode);
	FreqDispA->value(vfoA.freq);

	vfo = &vfoA;
	Fl::focus(FreqDispA);
}
