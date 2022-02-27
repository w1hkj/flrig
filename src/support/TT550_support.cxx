// ----------------------------------------------------------------------------
// Copyright (C) 2014-2011
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
#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>

#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Enumerations.H>

#include "icons.h"
#include "support.h"
#include "debug.h"
#include "gettext.h"
#include "rig_io.h"
#include "dialogs.h"
#include "rigbase.h"
#include "ptt.h"
#include "socket_io.h"
#include "ui.h"
#include "tod_clock.h"
#include "rig.h"
#include "rigs.h"
#include "K3_ui.h"
#include "KX3_ui.h"
#include "rigpanel.h"
#include "tod_clock.h"
#include "trace.h"
#include "cwio.h"
#include "fsk.h"
#include "fskioUI.h"
#include "xml_server.h"
#include "gpio_ptt.h"
#include "cmedia.h"
#include "tmate2.h"

void init_TT550_tabs()
{
	spnr_tt550_line_out->value(progStatus.tt550_line_out);
	cbo_tt550_agc_level->index(progStatus.tt550_agc_level);
	spnr_tt550_cw_wpm->value(progStatus.tt550_cw_wpm);
	spnr_tt550_cw_vol->value(progStatus.tt550_cw_vol);
	spnr_tt550_cw_spot->value(progStatus.tt550_cw_spot);
	spnr_tt550_cw_weight->value(progStatus.tt550_cw_weight);
	spnr_tt550_cw_qsk->value(progStatus.tt550_cw_qsk);
	btn_tt550_enable_keyer->value(progStatus.tt550_enable_keyer);
	btn_tt550_vox->value(progStatus.tt550_vox_onoff);
	spnr_tt550_vox_gain->value(progStatus.tt550_vox_gain);
	spnr_tt550_anti_vox->value(progStatus.tt550_vox_anti);
	spnr_tt550_vox_hang->value(progStatus.tt550_vox_hang);
	btn_tt550_CompON->value(progStatus.tt550_compON);
	spnr_tt550_compression->value(progStatus.tt550_compression);
	spnr_tt550_mon_vol->value(progStatus.tt550_mon_vol);
	btn_tt550_enable_xmtr->value(progStatus.tt550_enable_xmtr);
	btn_tt550_enable_tloop->value(progStatus.tt550_enable_tloop);
	btn_tt550_tuner_bypass->value(progStatus.tt550_tuner_bypass);
	btn_tt550_use_xmt_bw->value(progStatus.tt550_use_xmt_bw);
	sel_tt550_encoder_step->value(progStatus.tt550_encoder_step);
	spnr_tt550_encoder_sensitivity->value(progStatus.tt550_encoder_sensitivity);
	sel_tt550_F1_func->value(progStatus.tt550_F1_func);
	sel_tt550_F2_func->value(progStatus.tt550_F2_func);
	sel_tt550_F3_func->value(progStatus.tt550_F3_func);
	progStatus.use_rig_data = false;
	op_tt550_XmtBW->clear();
	for (int i = 0; TT550_xmt_widths[i] != NULL; i++) {
		op_tt550_XmtBW->add(TT550_xmt_widths[i]);
	}
	op_tt550_XmtBW->activate();
	op_tt550_XmtBW->index(progStatus.tt550_xmt_bw);
		poll_smeter->activate(); poll_smeter->value(progStatus.poll_smeter);
	poll_pout->activate(); poll_pout->value(progStatus.poll_pout);
	poll_swr->activate(); poll_swr->value(progStatus.poll_swr);
	poll_alc->activate(); poll_alc->value(progStatus.poll_alc);
	poll_frequency->deactivate(); poll_frequency->value(0);
	poll_mode->deactivate(); poll_mode->value(0);
	poll_bandwidth->deactivate(); poll_bandwidth->value(0);
	poll_volume->deactivate(); poll_volume->value(0);
	poll_notch->deactivate(); poll_notch->value(0);
	poll_auto_notch->deactivate(); poll_auto_notch->value(0);
	poll_ifshift->deactivate(); poll_ifshift->value(0);
	poll_power_control->deactivate(); poll_power_control->value(0);
	poll_pre_att->deactivate(); poll_pre_att->value(0);
	poll_squelch->deactivate(); poll_squelch->value(0);
	poll_micgain->deactivate(); poll_micgain->value(0);
	poll_rfgain->deactivate(); poll_rfgain->value(0);
	poll_split->deactivate(); poll_split->value(0);
	poll_nr->deactivate(); poll_nr->value(0);
	poll_noise->deactivate(); poll_noise->value(0);
	poll_all->deactivate(); poll_all->value(0);

	if (progStatus.tt550_at11_inline) {
		tt550_AT_inline->value(1);
		tt550_AT_inline->label("Inline");
		tt550_AT_inline->redraw_label();
		selrig->at11_autotune();
	} else {
		tt550_AT_inline->value(0);
		tt550_AT_inline->label("Bypassed");
		tt550_AT_inline->redraw_label();
		selrig->at11_bypass();
	}
	if (progStatus.tt550_at11_hiZ) {
		selrig->at11_hiZ();
		tt550_AT_Z->value(1);
	} else{
		selrig->at11_loZ();
		tt550_AT_Z->value(0);
	}
}

void init_TT550()
{
	selrig->selectA();

	vfoB.freq = progStatus.freq_B;
	vfoB.imode = progStatus.imode_B;
	vfoB.iBW = progStatus.iBW_B;
	FreqDispB->value(vfoB.freq);
	selrig->set_vfoB(vfoB.freq);
	selrig->set_modeB(vfoB.imode);
	selrig->set_bwB(vfoB.iBW);

	vfoA.freq = progStatus.freq_A;
	vfoA.imode = progStatus.imode_A;
	vfoA.iBW = progStatus.iBW_A;
	FreqDispA->value( vfoA.freq );
	selrig->set_vfoA(vfoA.freq);
	selrig->set_modeA(vfoA.imode);

	vfo = &vfoA;

	if (vfoA.iBW == -1) vfoA.iBW = selrig->def_bandwidth(vfoA.imode);
		selrig->set_bwA(vfoA.iBW);

	rigmodes_.clear();
	opMODE->clear();
	for (int i = 0; selrig->modes_[i] != NULL; i++) {
		rigmodes_.push_back(selrig->modes_[i]);
		opMODE->add(selrig->modes_[i]);
	}
	opMODE->activate();
	opMODE->index(vfoA.imode);

	rigbws_.clear();
	opBW->show();
	opBW->clear();
	old_bws = selrig->bandwidths_;
	for (int i = 0; selrig->bandwidths_[i] != NULL; i++) {
		rigbws_.push_back(selrig->bandwidths_[i]);
			opBW->add(selrig->bandwidths_[i]);
		}
	opBW->activate();
	opBW->index(vfoA.iBW);

	spnr_tt550_vfo_adj->value(progStatus.vfo_adj);

	btnPreamp->label("Spot");
	btnPreamp->value(progStatus.tt550_spot_onoff);
	switch (progStatus.UIsize) {
		case small_ui :
			btnPreamp->show();
			break;
		case wide_ui : case touch_ui : default :
			btnPreamp->activate();
	}
}


