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

#include "support.h"

void TRACED(enable_xcvr_ui)

	if (selrig->has_mode_control)
		btnRestoreMode->activate();
	else
		btnRestoreMode->deactivate();

	if (selrig->has_bandwidth_control)
		btnRestoreBandwidth->activate();
	else
		btnRestoreBandwidth->deactivate();

	if (selrig->has_preamp_control || selrig->has_attenuator_control)
		btnRestorePreAtt->show();
	else
		btnRestorePreAtt->hide();

	if (selrig->has_auto_notch)
		btnRestoreAutoNotch->show();
	else
		btnRestoreAutoNotch->hide();

	if (selrig->has_split)
		btnRestoreSplit->show();
	else
		btnRestoreSplit->hide();

	if (selrig->has_power_control)
		btnRestorePowerControl->show();
	else
		btnRestorePowerControl->hide();

	if (selrig->has_volume_control)
		btnRestoreVolume->show();
	else
		btnRestoreVolume->hide();

	if (selrig->has_ifshift_control)
		btnRestoreIFshift->show();
	else
		btnRestoreIFshift->hide();

	if (selrig->has_notch_control)
		btnRestoreNotch->show();
	else
		btnRestoreNotch->hide();

	if (selrig->has_noise_control)
		btnRestoreNoise->show();
	else
		btnRestoreNoise->hide();

	if (selrig->has_noise_reduction_control)
		btnRestoreNR->show();
	else
		btnRestoreNR->hide();

	if (selrig->has_micgain_control)
		btnRestoreMicGain->show();
	else
		btnRestoreMicGain->hide();

	if (selrig->has_sql_control)
		btnRestoreSquelch->show();
	else
		btnRestoreSquelch->hide();

	if (selrig->has_rf_control)
		btnRestoreRfGain->show();
	else
		btnRestoreRfGain->hide();

	if (selrig->has_compON)
		btnRestoreCompOnOff->show();
	else
		btnRestoreCompOnOff->hide();

	if (selrig->has_compression)
		btnRestoreCompLevel->show();
	else
		btnRestoreCompLevel->hide();
}

void TRACED(vfo_startup_data, XCVR_STATE &xcvrvfo)

	if (selrig->has_preamp_control)
		xcvrvfo.preamp = selrig->get_preamp();
	if (selrig->has_attenuator_control)
		xcvrvfo.attenuator = selrig->get_attenuator();
	if (selrig->has_auto_notch)
		xcvrvfo.auto_notch = selrig->get_auto_notch();
	if (selrig->has_split)
		xcvrvfo.split = selrig->get_split();

	update_progress(progress->value() + 4);

	if (selrig->has_power_control)
		xcvrvfo.power_control = selrig->get_power_control();
	if (selrig->has_volume_control)
		xcvrvfo.volume_control = selrig->get_volume_control();
	if (selrig->has_ifshift_control)
		selrig->get_if_shift(xcvrvfo.if_shift);
	if (selrig->has_notch_control)
		xcvrvfo.notch = selrig->get_notch(xcvrvfo.notch_val);

	update_progress(progress->value() + 4);

	if (selrig->has_noise_control)
		xcvrvfo.noise = selrig->get_noise();
	if (selrig->has_noise_reduction_control) {
		xcvrvfo.nr = selrig->get_noise_reduction();
		xcvrvfo.nr_val = selrig->get_noise_reduction_val();
	}
	if (selrig->has_micgain_control)
		xcvrvfo.mic_gain = selrig->get_mic_gain();
	if (selrig->has_sql_control)
		xcvrvfo.squelch = selrig->get_squelch();

	if (selrig->has_rf_control)
		xcvrvfo.rf_gain = selrig->get_rf_gain();

	update_progress(progress->value() + 4);

}

void TRACED(vfoA_startup_data)

	update_progress(progress->value() + 4);

	if (selrig->has_get_info)
		selrig->get_info();

	xcvr_vfoA.freq = selrig->get_vfoA();
	xcvr_vfoA.imode = selrig->get_modeA();
	xcvr_vfoA.iBW = selrig->get_bwA();
	xcvr_vfoA.filter = selrig->get_FILT(xcvr_vfoA.imode);

	update_progress(progress->value() + 10);

	vfo_startup_data(xcvr_vfoA);

	rig_trace(2, "Xcvr vfo A:\n", print(xcvr_vfoA));
}

void TRACED(vfoB_startup_data) 
	update_progress(progress->value() + 4);

	if (selrig->has_get_info)
		selrig->get_info();

	xcvr_vfoB.freq = selrig->get_vfoB();
	xcvr_vfoB.imode = selrig->get_modeB();
	xcvr_vfoB.iBW = selrig->get_bwB();
	xcvr_vfoB.filter = selrig->get_FILT(xcvr_vfoB.imode);

	update_progress(progress->value() + 4);

	vfo_startup_data(xcvr_vfoB);

	rig_trace(2, "Xcvr vfo B:\n", print(xcvr_vfoB));
}

void TRACED(rig_startup_data)

	update_progress(0);

	enable_xcvr_ui();

// The FT-891 loses width WDH on A/B changes.  It also starts
// with VFOA active, so no selectA() before reading VFOA values.

	if (selrig->name_ == rig_FT891.name_) {
		vfoA_startup_data();

		selrig->selectB();		// first select call
		vfoB_startup_data();

// Restore VFOA mode, then freq and bandwidth
		selrig->selectA();		// second select call
		yaesu891UpdateA(&xcvr_vfoA);
	} else {
		selrig->selectB();		// first select call to FT897D
		vfoB_startup_data();
		selrig->selectA();		// second select call
		vfoA_startup_data();
	}

	if (selrig->has_agc_control) {
		progStatus.agc_level = selrig->get_agc();
		redrawAGC();
	}

	if (selrig->has_FILTER)
		selrig->set_FILTERS(progStatus.filters);

	selrig->set_BANDWIDTHS(progStatus.bandwidths);

}
