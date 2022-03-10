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

void TRACED(restore_rig_vals_, XCVR_STATE &xcvrvfo)

	if (progStatus.restore_pre_att) {
		selrig->set_attenuator(xcvrvfo.attenuator);
		selrig->set_preamp(xcvrvfo.preamp);
	}
	update_progress(progress->value() + 5);

	if (progStatus.restore_auto_notch)
		selrig->set_auto_notch(xcvrvfo.auto_notch);
	if (progStatus.restore_split)
		selrig->set_split(xcvrvfo.split);
	update_progress(progress->value() + 5);

	if (progStatus.restore_power_control)
		selrig->set_power_control(xcvrvfo.power_control);
	if (progStatus.restore_volume)
		selrig->set_volume_control(xcvrvfo.volume_control);
	update_progress(progress->value() + 5);

	if (progStatus.restore_if_shift)
		selrig->set_if_shift(xcvrvfo.if_shift);
	update_progress(progress->value() + 5);

	if (progStatus.restore_notch)
		selrig->set_notch(xcvrvfo.notch, xcvrvfo.notch_val);
	if (progStatus.restore_noise)
		selrig->set_noise(xcvrvfo.noise);
	update_progress(progress->value() + 5);

	if (progStatus.restore_nr) {
		selrig->set_noise_reduction(xcvrvfo.nr);
		selrig->set_noise_reduction_val(xcvrvfo.nr_val);
	}
	update_progress(progress->value() + 5);

	if (progStatus.restore_mic_gain)
		selrig->set_mic_gain(xcvrvfo.mic_gain);
	update_progress(progress->value() + 5);

	if (progStatus.restore_squelch)
		selrig->set_squelch(xcvrvfo.squelch);
	update_progress(progress->value() + 5);

	if (progStatus.restore_rf_gain)
		selrig->set_rf_gain(xcvrvfo.rf_gain);
	update_progress(progress->value() + 5);

	if (progStatus.restore_comp_on_off && progStatus.restore_comp_level)
		selrig->set_compression(xcvrvfo.compON, xcvrvfo.compression);
	else if (progStatus.restore_comp_on_off)
		selrig->set_compression(xcvrvfo.compON, progStatus.compression);
	else if (progStatus.restore_comp_level)
		selrig->set_compression(progStatus.compON, xcvrvfo.compression);
	update_progress(progress->value() + 5);

}

void TRACED(restore_xcvr_vals)

	if (progStatus.start_stop_trace) ss_trace(true);

	update_progress(0);

	guard_lock serial_lock(&mutex_serial, "restore_xcvr_vals");
	trace(1, "restore_xcvr_vals()");

	if (selrig->inuse == onA) {
		selrig->selectB();
	}

	if (progStatus.restore_mode) {
		selrig->set_modeB(xcvr_vfoB.imode);
		selrig->set_FILT(xcvr_vfoB.filter);
	}

	if (progStatus.restore_frequency)
		selrig->set_vfoB(xcvr_vfoB.freq);

	if (progStatus.restore_bandwidth)
		selrig->set_bwB(xcvr_vfoB.iBW);

	restore_rig_vals_(xcvr_vfoB);

	trace(2, "Restored xcvr B:\n", print(xcvr_vfoB));

	selrig->selectA();

	if (progStatus.restore_mode) {
		selrig->set_modeA(xcvr_vfoA.imode);
		selrig->set_FILT(xcvr_vfoA.filter);
	}

	if (progStatus.restore_frequency)
		selrig->set_vfoA(xcvr_vfoA.freq);

	if (progStatus.restore_bandwidth)
		selrig->set_bwA(xcvr_vfoA.iBW);

	restore_rig_vals_(xcvr_vfoA);

	trace(2, "Restored xcvr A:\n", print(xcvr_vfoA));

	if (progStatus.start_stop_trace) ss_trace(false);

	exit_commands();

}

