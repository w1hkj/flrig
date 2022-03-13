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

#include "gpio_ptt.h"
#include "cmedia.h"

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
	if (!on && progStatus.split && !selrig->can_split())
		fake_split(on);

	std::string smode = "";
	if (selrig->modes_) smode = selrig->modes_[vfo->imode];
	if ((smode.find("CW") != std::string::npos) && progStatus.disable_CW_ptt)
		return;

	if (progStatus.comm_catptt == PTT_BOTH || progStatus.comm_catptt == PTT_SET)		selrig->set_PTT_control(on);
	else if (progStatus.comm_dtrptt == PTT_BOTH || progStatus.comm_dtrptt == PTT_SET)	RigSerial->SetPTT(on);
	else if (progStatus.comm_rtsptt == PTT_BOTH || progStatus.comm_rtsptt == PTT_SET)	RigSerial->SetPTT(on);

	else if (SepSerial->IsOpen() && 
		(progStatus.sep_dtrptt == PTT_BOTH || progStatus.sep_dtrptt == PTT_SET))		SepSerial->SetPTT(on);
	else if (SepSerial->IsOpen() && 
		(progStatus.sep_rtsptt == PTT_BOTH || progStatus.sep_rtsptt == PTT_SET))		SepSerial->SetPTT(on);
	else if (progStatus.gpio_ptt == PTT_BOTH || progStatus.gpio_ptt == PTT_SET)			set_gpio(on);
	else if (progStatus.cmedia_ptt == PTT_BOTH || progStatus.cmedia_ptt == PTT_SET)		set_cmedia(on);
	else
		LOG_DEBUG("No PTT i/o connected");
}

bool ptt_state()
{
	if (progStatus.comm_catptt == PTT_BOTH || progStatus.comm_catptt == PTT_GET)		return selrig->get_PTT();
	else if (progStatus.comm_dtrptt == PTT_BOTH || progStatus.comm_dtrptt == PTT_GET)	return selrig->get_PTT();
	else if (progStatus.comm_rtsptt == PTT_BOTH || progStatus.comm_rtsptt == PTT_GET)	return selrig->get_PTT();

	else if (SepSerial->IsOpen() && 
		(progStatus.sep_dtrptt == PTT_BOTH || progStatus.sep_dtrptt == PTT_GET))		return SepSerial->getPTT();
	else if (SepSerial->IsOpen() && 
		(progStatus.sep_rtsptt == PTT_BOTH || progStatus.sep_rtsptt == PTT_GET))		return SepSerial->getPTT();

	else if (progStatus.gpio_ptt == PTT_BOTH || progStatus.gpio_ptt == PTT_GET)			return get_gpio();
	else if (progStatus.cmedia_ptt == PTT_BOTH || progStatus.cmedia_ptt == PTT_GET)		return get_cmedia();

	LOG_DEBUG("No PTT i/o connected");
	return false;
}

