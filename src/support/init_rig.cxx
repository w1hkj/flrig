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

void TRACED(init_xcvr)

	if (selrig->name_ == rig_TT550.name_) return;

	if (xcvr_name == rig_FT817.name_ || 
		xcvr_name == rig_FT818ND.name_ ||
		xcvr_name == rig_FT857D.name_ ||
		xcvr_name == rig_FT897D.name_ ) {

		// transceiver should be on VFO A before starting flrig
		selrig->selectB();
		vfoB.freq = selrig->get_vfoB();
		FreqDispB->value(vfoB.freq);
		vfoB.imode = selrig->get_modeB();

		selrig->selectA();
		vfoA.freq = selrig->get_vfoA();
		FreqDispA->value(vfoA.freq);
		vfoA.imode = selrig->get_modeA();

		updateBandwidthControl();
		setModeControl((void *)0);

		highlight_vfo(NULL);

		return;
	} // Yaesu FT817, FT817ND, FT818ND, FT857D, FT897D transceivers

	if (xcvr_name == rig_FT817BB.name_) {
		if (selrig->get_vfoAorB() == onA) {
			selrig->selectB();
			vfoB.freq = selrig->get_vfoB();
			FreqDispB->value(vfoB.freq);
			vfoB.imode = selrig->get_modeB();

			selrig->selectA();
			vfoA.freq = selrig->get_vfoA();
			FreqDispA->value(vfoA.freq);
			vfoA.imode = selrig->get_modeA();

			updateBandwidthControl();
		} else {
			selrig->selectA();
			vfoA.freq = selrig->get_vfoA();
			FreqDispA->value(vfoA.freq);
			vfoA.imode = selrig->get_modeA();

			selrig->selectB();
			vfoB.freq = selrig->get_vfoB();
			FreqDispB->value(vfoB.freq);
			vfoB.imode = selrig->get_modeB();

			updateBandwidthControl();
		}

		setModeControl((void *)0);

		highlight_vfo(NULL);

		return;
	} // FT817BB transceiver

	if (!progStatus.use_rig_data) {

		vfoB.freq = progStatus.freq_B;
		vfoB.imode = progStatus.imode_B;
		vfoB.iBW = progStatus.iBW_B;

		if (vfoB.iBW == -1)
			vfoB.iBW = selrig->def_bandwidth(vfoB.imode);

		selrig->selectB();

		selrig->set_modeB(vfoB.imode);
		selrig->set_bwB(vfoB.iBW);
		selrig->set_vfoB(vfoB.freq);
		FreqDispB->value(vfoB.freq);

		update_progress(progress->value() + 4);

		trace(2, "init_xcvr() vfoB ", printXCVR_STATE(vfoB).c_str());

		vfoA.freq = progStatus.freq_A;
		vfoA.imode = progStatus.imode_A;
		vfoA.iBW = progStatus.iBW_A;

		if (vfoA.iBW == -1)
			vfoA.iBW = selrig->def_bandwidth(vfoA.imode);

		selrig->selectA();

		selrig->set_modeA(vfoA.imode);
		selrig->set_bwA(vfoA.iBW);
		selrig->set_vfoA(vfoA.freq);
		FreqDispA->value( vfoA.freq );

		update_progress(progress->value() + 4);

		vfo = &vfoA;
		updateBandwidthControl();
		highlight_vfo((void *)0);

		trace(2, "init_xcvr() vfoA ", printXCVR_STATE(vfoA).c_str());

	} else {

		// Capture VFOA mode and bandwidth, since it will be lost in VFO switch
		if (selrig->name_ == rig_FT891.name_) {
			selrig->selectA();
			vfoA.freq = selrig->get_vfoA();
			update_progress(progress->value() + 4);
			vfoA.imode = selrig->get_modeA();

			update_progress(progress->value() + 4);

			vfoA.iBW = selrig->get_bwA();
			update_progress(progress->value() + 4);
			FreqDispA->value(vfoA.freq);
			trace(2, "A: ", printXCVR_STATE(vfoA).c_str());

			selrig->selectB();			// third select call
			vfoB.freq = selrig->get_vfoB();
			update_progress(progress->value() + 4);
			vfoB.imode = selrig->get_modeB();

			update_progress(progress->value() + 4);

			vfoB.iBW = selrig->get_bwB();
			update_progress(progress->value() + 4);
			FreqDispB->value(vfoB.freq);
			trace(2, "B: ", printXCVR_STATE(vfoB).c_str());

			// Restore radio VFOA mode, then freq and bandwidth
			selrig->selectA();			// fourth select call
			yaesu891UpdateA(&vfoA);
		} else {
			vfoB = xcvr_vfoB;
			vfoA = xcvr_vfoA;
			FreqDispB->value(vfoB.freq);
			FreqDispA->value(vfoA.freq);
		}


		vfo = &vfoA;
		selrig->set_modeA(vfo->imode);
		selrig->set_bwA(vfo->iBW);
		setModeControl((void *)0);

		update_progress(progress->value() + 4);

		updateBandwidthControl();

		update_progress(progress->value() + 4);

		highlight_vfo((void *)0);
	}

	selrig->set_split(0);		// initialization set split call
}

void TRACED(init_generic_rig)

	if (progStatus.CIV > 0)
		selrig->adjustCIV(progStatus.CIV);

	if (selrig->has_getvfoAorB) {

		selrig->get_vfoAorB();

//		int ret = selrig->get_vfoAorB();
//		int retry = 10;
//		while (ret == -1 && retry--) {
//			MilliSleep(50);
//			ret = selrig->get_vfoAorB();
//		}
//		if (ret == -1) ret = 0;
//		selrig->inuse = ret;

		rig_startup_data();
		if (progStatus.use_rig_data) {
			vfoA = xcvr_vfoA;
			vfoB = xcvr_vfoB;
		}

		if (selrig->inuse == onB) {
//			selrig->selectB();
			vfo = &vfoB;
		} else {
			vfo = &vfoA;
		}
	}
	else {
		rig_startup_data();
		if (progStatus.use_rig_data) {
			vfoA = xcvr_vfoA;
			vfoB = xcvr_vfoB;
		}
	}
	init_xcvr();

	progStatus.compON = xcvr_vfoA.compON;
	progStatus.compression = xcvr_vfoA.compression;

//	vfo = &vfoA;

	rigmodes_.clear();
	opMODE->clear();
	if (selrig->has_mode_control) {
		for (int i = 0; selrig->modes_[i] != NULL; i++) {
			rigmodes_.push_back(selrig->modes_[i]);
			opMODE->add(selrig->modes_[i]);
		}
		opMODE->activate();
		opMODE->index(progStatus.imode_A);
	} else {
		opMODE->add(" ");
		opMODE->index(0);
		opMODE->deactivate();
	}

	rigbws_.clear();
	opBW->show();
	opBW->clear();
	if (selrig->has_bandwidth_control) {
		old_bws = selrig->bandwidths_;
		for (int i = 0; selrig->bandwidths_[i] != NULL; i++) {
			rigbws_.push_back(selrig->bandwidths_[i]);
			opBW->add(selrig->bandwidths_[i]);
		}
		opBW->activate();
		if (progStatus.iBW_A == -1) progStatus.iBW_A = selrig->def_bandwidth(vfoA.imode);
		if (progStatus.iBW_B == -1) progStatus.iBW_B = selrig->def_bandwidth(vfoB.imode);
		opBW->index(progStatus.iBW_A);
	} else {
		opBW->add(" ");
		opBW->index(0);
		opBW->deactivate();
	}
}
