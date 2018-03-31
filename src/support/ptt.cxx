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
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "threads.h"
#include "ptt.h"
#include "debug.h"
#include "rig_io.h"
#include "rig.h"
#include "support.h"

using namespace std;

// used for transceivers with a single vfo, called only by rigPTT
static XCVR_STATE fake_vfo;

static void showfreq(void *)
{
	FreqDispA->value(vfoA.freq);
}

static void fake_split(int on)
{
	if (on) {
		fake_vfo = vfoA;
		vfoA.freq = vfoB.freq;
		selrig->set_vfoA(vfoA.freq);
		Fl::awake(showfreq);
	} else {
		vfoA = fake_vfo;
		selrig->set_vfoA(vfoA.freq);
		Fl::awake(showfreq);
	}
}

// add fake rit to this function and to set_vfoA ??

void rigPTT(bool on)
{
	if (on && progStatus.split && !selrig->can_split())
		fake_split(on);

	if (progStatus.comm_catptt) {
		selrig->set_PTT_control(on);
	} else if (progStatus.comm_dtrptt || progStatus.comm_rtsptt)
		RigSerial->SetPTT(on);
	else if (SepSerial->IsOpen() && (progStatus.sep_dtrptt || progStatus.sep_rtsptt) )
		SepSerial->SetPTT(on);
	else
		LOG_ERROR("No PTT i/o connected");

	if (!on && progStatus.split && !selrig->can_split())
		fake_split(on);
}
