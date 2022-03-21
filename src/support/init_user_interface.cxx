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
#include "ptt.h"

extern bool testmode;

void TRACED(update_UI_PTT, void *d)

	btnPTT->value(PTT);
	if (!PTT) {
		btnALC_SWR->hide();
		scaleSmeter->show();
		sldrRcvSignal->clear();
	} else {
		btnALC_SWR->show();
		scaleSmeter->hide();
		sldrFwdPwr->clear();
		sldrALC->clear();
		sldrSWR->clear();
	}
}

void TRACED(adjust_small_ui)

	int y = 0;

	mainwindow->resize( mainwindow->x(), mainwindow->y(), SMALL_MAINW, SMALL_MAINH);

	btnVol->hide();
	sldrVOLUME->hide();
	sldrRFGAIN->hide();
	btnIFsh->hide();
	btn_KX3_IFsh->hide();
	sldrIFSHIFT->hide();
	btnLOCK->hide();
	btnCLRPBT->hide();
	sldrINNER->hide();
	btnCLRPBT->hide();
	sldrOUTER->hide();
	btnNotch->hide();
	sldrNOTCH->hide();
	sldrMICGAIN->hide();
	btnPOWER->hide();
	sldrPOWER->hide();
	btnPOWER->hide();
	sldrSQUELCH->hide();
	btnNR->hide();
	sldrNR->hide();
	btnNOISE->hide();
	btnAGC->hide();
	sldrRFGAIN->redraw_label();

	if (progStatus.schema == 1 && selrig->widgets[0].W != (Fl_Widget *)0) {
		int i = 0;
		while (selrig->widgets[i].W != NULL) {
			if (selrig->widgets[i].w != 0) 
				selrig->widgets[i].W->resize(
					selrig->widgets[i].x, selrig->widgets[i].y,
					selrig->widgets[i].w, selrig->widgets[i].W->h() );
			if (selrig->widgets[i].y > y) y = selrig->widgets[i].y;
			selrig->widgets[i].W->show();
			selrig->widgets[i].W->redraw();
			i++;
		}
		if (selrig->has_data_port) {
			sldrMICGAIN->label("");
			sldrMICGAIN->redraw_label();
		}
		if (selrig->has_power_control) {
			btnPOWER->resize( sldrPOWER->x() - 52, sldrPOWER->y(), 50, 18 );
			btnPOWER->redraw();
			btnPOWER->show();
		}
		if (mnuSchema) mnuSchema->set();
	} else {
		if (mnuSchema) mnuSchema->clear();
		y = cntRIT->y() + 2;
		if (selrig->has_volume_control) {
			y += 20;
			btnVol->position( 2, y);
			btnVol->show();
			btnVol->redraw();
			sldrVOLUME->resize( 54, y, 368, 18 );
			sldrVOLUME->show();
			sldrVOLUME->redraw();
		}
		if (selrig->has_rf_control) {
			y += 20;
			sldrRFGAIN->resize( 54, y, 368, 18 );
			sldrRFGAIN->show();
			sldrRFGAIN->redraw();
		}
		if (selrig->has_sql_control) {
			y += 20;
			sldrSQUELCH->resize( 54, y, 368, 18 );
			sldrSQUELCH->show();
			sldrSQUELCH->redraw();
		}
		if (selrig->has_noise_reduction_control) {
			y += 20;
			btnNR->position( 2, y);
			btnNR->show();
			btnNR->redraw();
			sldrNR->resize( 54, y, 368, 18 );
			sldrNR->show();
			sldrNR->redraw();
			if (xcvr_name == rig_TT599.name_) btnNR->deactivate();
		}
		if (selrig->has_pbt_controls) {
			y += 20;
			btnLOCK->position( 2, y);
			btnLOCK->show();
			btnLOCK->redraw();
			sldrINNER->resize( 54, y, 368, 18 );
			sldrINNER->show();
			sldrINNER->redraw();
			y += 20;
			btnCLRPBT->position( 2, y);
			btnCLRPBT->show();
			btnCLRPBT->redraw();
			sldrOUTER->resize( 54, y, 368, 18);
			sldrOUTER->show();
			sldrOUTER->redraw();
		}
		if (selrig->has_ifshift_control) {
			y += 20;
			if (xcvr_name == rig_KX3.name_) {
				btn_KX3_IFsh->position( 2, y );
				btn_KX3_IFsh->show();
				btn_KX3_IFsh->redraw();
				btnIFsh->hide();
				btnIFsh->redraw();
			} else {
				btnIFsh->position( 2, y);
				btnIFsh->show();
				btnIFsh->redraw();
				btn_KX3_IFsh->hide();
				btn_KX3_IFsh->redraw();
			}
			sldrIFSHIFT->resize( 54, y, 368, 18 );
			sldrIFSHIFT->show();
			sldrIFSHIFT->redraw();
		}
		if (selrig->has_notch_control) {
			y += 20;
			btnNotch->position( 2, y);
			btnNotch->show();
			btnNotch->redraw();
			sldrNOTCH->resize( 54, y, 368, 18 );
			sldrNOTCH->show();
			sldrNOTCH->redraw();
		}

		if (selrig->has_micgain_control) {
			y += 20;
			sldrMICGAIN->resize( 54, y, 368, 18 );
			sldrMICGAIN->show();
			sldrMICGAIN->redraw();
			if (selrig->has_data_port) {
				sldrMICGAIN->label("");
				sldrMICGAIN->redraw_label();
				btnDataPort->position( 2, y);
				btnDataPort->show();
				btnDataPort->redraw();
			}
		} else if (selrig->has_data_port) {
			btnDataPort->position( 214, 105);
			btnDataPort->show();
			btnDataPort->redraw();
		}

		if (selrig->has_power_control) {
			y += 20;
			sldrPOWER->resize( 54, y, 368, 18 );
			sldrPOWER->show();
			sldrPOWER->redraw();
			btnPOWER->resize( 2, y, 50, 18 );
			btnPOWER->show();
		}
	}
	y += 20;
	btn_show_controls->position( btn_show_controls->x(), y );
	btnAttenuator->position( btnAttenuator->x(), y);
	btnAttenuator->redraw();
	btnPreamp->position( btnPreamp->x(), y);
	btnPreamp->redraw();
	btnNOISE->position( btnNOISE->x(), y);
	btnAutoNotch->position( btnAutoNotch->x(), y);
	btnAutoNotch->redraw();
	btnTune->position( btnTune->x(), y);
	btnTune->redraw();
	btn_tune_on_off->position( btn_tune_on_off->x(), y);
	btn_tune_on_off->redraw();

	if (selrig->has_noise_reduction) {
		btnNOISE->show();
	} else {
		btnNOISE->hide();
	}
	btnNOISE->redraw();

	if (selrig->has_agc_control) {
		btnAGC->show();
		sldrRFGAIN->label("");
		sldrRFGAIN->redraw_label();
	} else {
		btnAGC->hide();
		sldrRFGAIN->label(_("RF"));
		sldrRFGAIN->redraw_label();
	}

	if (xcvr_name == rig_FT1000MP.name_) {
		y -= 20;
		btnTune->position( btnTune->x(), y);
		btnTune->redraw();
		btn_tune_on_off->position( btn_tune_on_off->x(), y);
		btn_tune_on_off->redraw();
		btnAutoNotch->position( btnAutoNotch->x(), y);
		btnAutoNotch->redraw();
		btnPTT->position( btnPTT->x(), y);
		btnPTT->redraw();
	}

	if (xcvr_name == rig_FT100D.name_ ||
		xcvr_name == rig_FT767.name_  ||
		xcvr_name == rig_FT817.name_  ||
		xcvr_name == rig_FT817BB.name_ ||
		xcvr_name == rig_FT818ND.name_ ||
		xcvr_name == rig_FT847.name_  ||
		xcvr_name == rig_FT857D.name_ ||
		xcvr_name == rig_FT890.name_  ||
		xcvr_name == rig_FT897D.name_ ||
		xcvr_name == rig_FT920.name_ ) {
		y -= 20;
		btnPTT->position( mainwindow->w() - btnPTT->w() - btn_show_controls->w() - 10, y);
		btnPTT->redraw();
		btn_show_controls->position( btnPTT->x() + btnPTT->w() + 5, y );
		btn_show_controls->redraw();
	}

	btnPTT->resize(btnPTT->x(), y, btnPTT->w(), 18);
	btnPTT->redraw();

	btn_show_controls->label("@-22->");
	btn_show_controls->redraw_label();

	y += 20;

	btn_show_controls->show();
	mainwindow->init_sizes();
	mainwindow->size( mainwindow->w(), y);

	if (progStatus.tooltips) {
		Fl_Tooltip::enable(1);
		if (mnuTooltips) mnuTooltips->set();
	} else {
		if (mnuTooltips) mnuTooltips->clear();
		Fl_Tooltip::enable(0);
	}

	if (mnuVoltmeter) {
		if (progStatus.display_voltmeter)
			mnuVoltmeter->set();
		else
			mnuVoltmeter->clear();
	}

	if (mnuEmbedTabs) {
		if (progStatus.embed_tabs) mnuEmbedTabs->set();
		else mnuEmbedTabs->clear();
	}

	mainwindow->damage();
	mainwindow->redraw();
}

void TRACED(adjust_xig_wide)

	btnPreamp->show();
	btnAttenuator->show();
	btnNOISE->show();
	btnAutoNotch->hide();

	btnAGC->resize(btnAutoNotch->x(), btnAutoNotch->y(), btnAutoNotch->w(), btnAutoNotch->h());
	btnAGC->show();
	grp_row1b1b->add(btnAGC);

	btnAswapB->show();
	btnSplit->show();
	btnPTT->show();
	btnTune->show();

	int xig_y = grp_row2->y() + grp_row2->h() / 4;
	int xig_h = 5 * btnAGC->h() / 4;

	Fl_Group *xig_group = new Fl_Group(
		2, xig_y,
		mainwindow->w() - 4, xig_h);

	Fl_Group *xig_gp1 = new Fl_Group(
		xig_group->x(), xig_group->y(),
		(xig_group->w() - 4) / 3, xig_h);

	btnVol->resize(
		xig_gp1->x(), xig_y,
		54, xig_gp1->h());
	xig_gp1->add(btnVol);

	sldrVOLUME->resize(
		xig_gp1->x() + 54, xig_y,
		xig_gp1->w() - 54, xig_h);
	xig_gp1->add(sldrVOLUME);

	btnVol->show();
	sldrVOLUME->show();

	xig_gp1->end();
	xig_gp1->resizable(sldrVOLUME);

	Fl_Group *xig_gp2 = new Fl_Group(
		xig_gp1->x() + xig_gp1->w() + 2, xig_y,
		xig_gp1->w(), xig_h);

	btnPOWER->resize(
		xig_gp2->x(), xig_y,
		54, xig_h);
	xig_gp2->add(btnPOWER);

	sldrPOWER->resize(
		xig_gp2->x() + 54, xig_y,
		xig_gp2->w() - 54, xig_h);
	xig_gp2->add(sldrPOWER);

	btnPOWER->show();
	sldrPOWER->show();

	xig_gp2->end();
	xig_gp2->resizable(sldrPOWER);

	Fl_Group *xig_gp3 = new Fl_Group(
		xig_gp2->x() + xig_gp2->w() + 2, xig_y,
		xig_group->w() - 2 * xig_gp1->w() - 4, xig_h);

	xig_gp3->add(sldrSQUELCH);
	sldrSQUELCH->resize(
		xig_gp3->x() + 54, xig_y,
		xig_gp3->w() - 54, xig_h);
	sldrSQUELCH->label("SQL");
	sldrSQUELCH->redraw_label();
	sldrSQUELCH->show();

	xig_gp3->end();
	xig_gp3->resizable(sldrSQUELCH);

	xig_group->end();

	grp_row2a->remove(sldrMICGAIN);
	grp_row2a->resize(xig_group->x(), xig_group->y(), xig_group->w(), xig_group->h());
	grp_row2a->hide();
	grp_row2b->remove(btnIFsh);
	grp_row2b->remove(btn_KX3_IFsh);
	grp_row2b->remove(sldrIFSHIFT);
	grp_row2b->resize(xig_group->x(), xig_group->y(), xig_group->w(), xig_group->h());
	grp_row2b->hide();
	grp_row2c->remove(sldrRFGAIN);
	grp_row2c->resize(xig_group->x(), xig_group->y(), xig_group->w(), xig_group->h());
	grp_row2c->hide();
	grp_row2->resize(xig_group->x(), xig_group->y(), xig_group->w(), xig_group->h());
	grp_row2->hide();
	mainwindow->remove(grp_row2);

	mainwindow->add(xig_group);

	if (progStatus.tooltips) {
		Fl_Tooltip::enable(1);
		if (mnuTooltips) mnuTooltips->set();
	} else {
		if (mnuTooltips) mnuTooltips->clear();
		Fl_Tooltip::enable(0);
	}

	if (mnuVoltmeter) {
		if (progStatus.display_voltmeter)
			mnuVoltmeter->set();
		else
			mnuVoltmeter->clear();
	}

	mainwindow->redraw();

	return;
}

void TRACED(adjust_wide_ui)

	mainwindow->resize(
		progStatus.mainX, progStatus.mainY, progStatus.mainW, progStatus.mainH);
	mainwindow->redraw();

	btnVol->show();
	sldrVOLUME->show();
	sldrRFGAIN->show();
	if (selrig->has_ifshift_control) {
		if (xcvr_name != rig_KX3.name_)
			btnIFsh->show();
		else
			btn_KX3_IFsh->show();
		sldrIFSHIFT->show();
	}
	if (selrig->has_pbt_controls) {
		btnLOCK->show();
		btnLOCK->value(progStatus.pbt_lock);
		btnCLRPBT->show();
		sldrINNER->show();
		sldrOUTER->show();
		sldrINNER->value(progStatus.pbt_inner);
		sldrOUTER->value(progStatus.pbt_outer);
	}
	btnNotch->show();
	sldrNOTCH->show();
	sldrMICGAIN->show();
	sldrPOWER->show();
	btnPOWER->hide();
	sldrSQUELCH->show();
	btnNR->show();
	sldrNR->show();
	btnAGC->hide();
	btnDataPort->hide();
	sldrRFGAIN->redraw_label();

	if (!selrig->has_micgain_control)
		sldrMICGAIN->deactivate();

	if (!selrig->has_noise_reduction)
		btnNR->deactivate();

	if (!selrig->has_noise_reduction_control)
		sldrNR->deactivate();

	if (xcvr_name == rig_TT550.name_) {
		tabs550->show();
		tabsGeneric->hide();
	} else {
		tabs550->hide();

		tabsGeneric->remove(genericAux);
		genericAux->hide();
		btnAuxDTR->hide();
		btnAuxRTS->hide();
		btnDataPort->hide();
		if (progStatus.aux_serial_port != "NONE") {
			btnAuxRTS->show();
			btnAuxDTR->show();
			tabsGeneric->add(genericAux);
			genericAux->show();
		}

		tabsGeneric->remove(genericRXB);
		if (selrig->has_rit || selrig->has_xit || selrig->has_bfo)
			tabsGeneric->add(genericRXB);

		tabsGeneric->redraw();
		tabsGeneric->show();
		if (selrig->has_agc_control) {
			btnAGC->show();
			sldrRFGAIN->label("");
			sldrRFGAIN->redraw_label();
		} else {
			btnAGC->hide();
			sldrRFGAIN->label(_("RF"));
			sldrRFGAIN->redraw_label();
		}
		if (selrig->has_power_control) {
			btnPOWER->resize(sldrPOWER->x() - 52, sldrPOWER->y(), 50, 18);
			btnPOWER->show();
		}
		else {
			sldrPOWER->deactivate();
		}
	}

	if (progStatus.tooltips) {
		Fl_Tooltip::enable(1);
		if (mnuTooltips) mnuTooltips->set();
	} else {
		if (mnuTooltips) mnuTooltips->clear();
		Fl_Tooltip::enable(0);
	}

	if (mnuVoltmeter) {
		if (progStatus.display_voltmeter)
			mnuVoltmeter->set();
		else
			mnuVoltmeter->clear();
	}

	mainwindow->redraw();
}

void TRACED(adjust_touch_ui)

	mainwindow->resize( mainwindow->x(), mainwindow->y(), mainwindow->w(), TOUCH_MAINH);
	mainwindow->redraw();

	if (spnrPOWER) spnrPOWER->show();
	if (sldrPOWER) sldrPOWER->show();
	btnVol->show();
	if (spnrVOLUME) spnrVOLUME->show();
	if (sldrVOLUME) sldrVOLUME->show();

	if (spnrRFGAIN) spnrRFGAIN->show();
	if (sldrRFGAIN) sldrRFGAIN->show();

	if (xcvr_name != rig_KX3.name_)
		btnIFsh->show();
	else
		btn_KX3_IFsh->show();
	if (spnrIFSHIFT) spnrIFSHIFT->show();
	if (sldrIFSHIFT) sldrIFSHIFT->show();

	btnNotch->show();
	if (spnrNOTCH) spnrNOTCH->show();
	if (sldrNOTCH) sldrNOTCH->show();

	if (spnrMICGAIN) spnrMICGAIN->show();
	if (sldrMICGAIN) sldrMICGAIN->show();

	if (spnrSQUELCH) spnrSQUELCH->show();
	if (sldrSQUELCH) sldrSQUELCH->show();

	if (selrig->has_agc_control) {
		btnAGC->show();
		sldrRFGAIN->label("");
		sldrRFGAIN->redraw_label();
	} else {
		btnAGC->hide();
		sldrRFGAIN->label(_("RF"));
		sldrRFGAIN->redraw_label();
	}

	btnNR->show();
	if (spnrNR) spnrNR->show();
	if (sldrNR) sldrNR->show();

	if (xcvr_name == rig_TT550.name_) {
		tabs550->show();
		tabsGeneric->hide();
	} else {
		tabs550->hide();

		tabsGeneric->remove(genericAux);
		genericAux->hide();
		btnAuxDTR->hide();
		btnAuxRTS->hide();
		btnDataPort->hide();

		if (progStatus.aux_serial_port != "NONE") {
			btnAuxRTS->show();
			btnAuxDTR->show();
			tabsGeneric->add(genericAux);
			genericAux->show();
		}

		tabsGeneric->remove(genericRXB);
		if (selrig->has_rit || selrig->has_xit || selrig->has_bfo)
			tabsGeneric->add(genericRXB);
		tabsGeneric->show();
	}

	if (progStatus.tooltips) {
		Fl_Tooltip::enable(1);
		if (mnuTooltips) mnuTooltips->set();
	} else {
		if (mnuTooltips) mnuTooltips->clear();
		Fl_Tooltip::enable(0);
	}

	if (mnuVoltmeter) {
		if (progStatus.display_voltmeter)
			mnuVoltmeter->set();
		else
			mnuVoltmeter->clear();
	}

	mainwindow->init_sizes();
	mainwindow->size_range(WIDE_MAINW, WIDE_MAINH, 0, WIDE_MAINH);
	mainwindow->redraw();
}

void TRACED(adjust_control_positions)

	if (!selrig->has_smeter)
		grpMeters->deactivate();
	else
		grpMeters->activate();

	switch (progStatus.UIsize) {
		case small_ui :
			adjust_small_ui();
			break;
		case wide_ui :
			if (xcvr_name == rig_XIG90.name_)
				adjust_xig_wide();
			else
				adjust_wide_ui();
			break;
		case touch_ui :
		default :
			adjust_touch_ui();
			break;
	}
// change control labels / tooltips if necessary
	int i = 0;
	while (selrig->widgets[i].W != NULL) {
		if (!selrig->widgets[i].label.empty()) {
			selrig->widgets[i].W->label(selrig->widgets[i].label.c_str());
			selrig->widgets[i].W->redraw_label();
		}
		if (!selrig->widgets[i].hint.empty()) {
			selrig->widgets[i].W->tooltip(selrig->widgets[i].hint.c_str());
		}
		selrig->widgets[i].W->redraw();
		i++;
	}

	FreqDispA->set_hrd(progStatus.hrd_buttons);
	FreqDispB->set_hrd(progStatus.hrd_buttons);
	if (selrig->name_ == rig_FT891.name_) {
		// Default FT891 to only send slider updates to rig once slider
		// is released. This avoids a condition where once slider is
		// released, the slider value no longer tracks changes from
		// controls on the rig.
		progStatus.sliders_button = FL_WHEN_RELEASE;
		chk_sliders_button->value(false);
	}
	set_sliders_when();
}

void TRACED(init_Generic_Tabs)

	if (hidden_tabs) {
		hidden_tabs->remove(tab_yaesu_bands);
		hidden_tabs->remove(tab_ft991_bands);
		hidden_tabs->remove(tab_FT8n_bands);
		hidden_tabs->remove(tab_FT8n_CTCSS);
		hidden_tabs->remove(tab_icom_bands);
		hidden_tabs->remove(genericCW);
		hidden_tabs->remove(genericQSK);
		hidden_tabs->remove(genericVOX);
		hidden_tabs->remove(genericSpeech);
		hidden_tabs->remove(genericRx);
		hidden_tabs->remove(genericMisc);
		hidden_tabs->remove(genericAux);
		hidden_tabs->remove(genericUser_1);
		hidden_tabs->remove(genericUser_2);
		hidden_tabs->remove(genericUser_3);
//		hidden_tabs->remove(kx3_extras);
		hidden_tabs->remove(tab7610);

		hidden_tabs->add(tab_yaesu_bands);
		hidden_tabs->add(tab_ft991_bands);
		hidden_tabs->add(tab_FT8n_bands);
		hidden_tabs->add(tab_FT8n_CTCSS);
		hidden_tabs->add(tab_icom_bands);
		hidden_tabs->add(genericCW);
		hidden_tabs->add(genericQSK);
		hidden_tabs->add(genericVOX);
		hidden_tabs->add(genericSpeech);
		hidden_tabs->add(genericRx);
		hidden_tabs->add(genericMisc);
		hidden_tabs->add(genericAux);
		hidden_tabs->add(genericUser_1);
		hidden_tabs->add(genericUser_2);
		hidden_tabs->add(genericUser_3);
//		hidden_tabs->add(kx3_extras);
		hidden_tabs->add(tab7610);
	} else {
		tabsGeneric->remove(tab_yaesu_bands);
		tabsGeneric->remove(tab_ft991_bands);
		tabsGeneric->remove(tab_FT8n_bands);
		tabsGeneric->remove(tab_FT8n_CTCSS);
		tabsGeneric->remove(tab_icom_bands);
		tabsGeneric->remove(genericCW);
		tabsGeneric->remove(genericQSK);
		tabsGeneric->remove(genericVOX);
		tabsGeneric->remove(genericSpeech);
		tabsGeneric->remove(genericRx);
		tabsGeneric->remove(genericMisc);
		tabsGeneric->remove(genericAux);
		tabsGeneric->remove(genericUser_1);
		tabsGeneric->remove(genericUser_2);
		tabsGeneric->remove(genericUser_3);
//		tabsGeneric->remove(kx3_extras);
		tabsGeneric->remove(tab7610);
	}

	if (selrig->has_band_selection) {
		if (selrig->ICOMrig ||
			selrig->name_ == rig_XIG90.name_ ||
			selrig->name_ == rig_X6100.name_ ) {
			tabsGeneric->add(tab_icom_bands);
			tab_icom_bands->redraw();
			tab_icom_bands->show();
		} else if (selrig->name_ == rig_FT857D.name_ || selrig->name_ == rig_FT897D.name_) {
			tabsGeneric->add(tab_FT8n_bands);
			tabsGeneric->add(tab_FT8n_CTCSS);
			tab_FT8n_bands->redraw();
			tab_FT8n_CTCSS->redraw();
			tab_FT8n_bands->show();
			tab_FT8n_CTCSS->show();
		} else if (selrig->name_ == rig_FT991A.name_) {
			tabsGeneric->add(tab_ft991_bands);
			tab_ft991_bands->redraw();
			tab_ft991_bands->show();
		} else {
			tabsGeneric->add(tab_yaesu_bands);
			tab_yaesu_bands->redraw();
			tab_yaesu_bands->show();
		}
	}

	if (selrig->has_cw_wpm ||
		selrig->has_cw_weight ||
		selrig->has_cw_keyer ||
		selrig->has_cw_spot ||
		selrig->has_cw_spot_tone ) {

		if (selrig->has_cw_wpm) {
			int min, max;
			selrig->get_cw_wpm_min_max(min, max);
			spnr_cw_wpm->minimum(min);
			spnr_cw_wpm->maximum(max);
			spnr_cw_wpm->value(progStatus.cw_wpm);
			spnr_cw_wpm->show();
		} else
			spnr_cw_wpm->hide();

		if (selrig->has_cw_weight) {
			double min, max, step;
			selrig->get_cw_weight_min_max_step( min, max, step );
			spnr_cw_weight->minimum(min);
			spnr_cw_weight->maximum(max);
			spnr_cw_weight->step(step);
			spnr_cw_weight->value(progStatus.cw_weight);
			spnr_cw_weight->show();
		} else
			spnr_cw_weight->hide();

		if (selrig->has_cw_keyer) {
			btn_enable_keyer->show();
			btn_enable_keyer->value(progStatus.enable_keyer);
			selrig->enable_keyer();
		}
		else
			btn_enable_keyer->hide();

		if (selrig->has_cw_spot) {
			btnSpot->value(progStatus.cw_spot);
			selrig->set_cw_spot();
			btnSpot->show();
		} else
			btnSpot->hide();

		if (selrig->has_cw_spot_tone) {
			spnr_cw_spot_tone->show();
			int min, max, step;
			selrig->get_cw_spot_tone_min_max_step(min, max, step);
			spnr_cw_spot_tone->minimum(min);
			spnr_cw_spot_tone->maximum(max);
			spnr_cw_spot_tone->step(step);
			spnr_cw_spot_tone->value(progStatus.cw_spot_tone);
			selrig->set_cw_spot_tone();
		} else
			spnr_cw_spot_tone->hide();

		tabsGeneric->add(genericCW);
		genericCW->redraw();
		genericCW->show();
	}

	if (selrig->has_cw_qsk) {

		btnBreakIn->show();
		spnr_cw_delay->show();

		if (selrig->has_cw_qsk) {
			double min, max, step;
			selrig->get_cw_qsk_min_max_step(min, max, step);
			spnr_cw_qsk->minimum(min);
			spnr_cw_qsk->maximum(max);
			spnr_cw_qsk->step(step);
			spnr_cw_qsk->value(progStatus.cw_qsk);
			spnr_cw_qsk->show();
		} else
			spnr_cw_qsk->hide();

		tabsGeneric->add(genericQSK);
		genericQSK->redraw();
		genericQSK->show();
	}

	if (selrig->has_vox_onoff ||
		selrig->has_vox_gain ||
		selrig->has_vox_hang ||
		selrig->has_vox_on_dataport) {

		if (selrig->has_vox_onoff) {
			btn_vox->value(progStatus.vox_onoff);
			btn_vox->show();
			selrig->set_vox_onoff();
		} else
			btn_vox->hide();

		if (selrig->has_vox_gain) {
			int min, max, step;
			selrig->get_vox_gain_min_max_step(min, max, step);
			spnr_vox_gain->minimum(min);
			spnr_vox_gain->maximum(max);
			spnr_vox_gain->step(step);
			spnr_vox_gain->value(progStatus.vox_gain);
			spnr_vox_gain->show();
			selrig->set_vox_gain();
		} else
			spnr_vox_gain->hide();

		if (selrig->has_vox_anti) {
			int min, max, step;
			selrig->get_vox_anti_min_max_step(min, max, step);
			spnr_anti_vox->minimum(min);
			spnr_anti_vox->maximum(max);
			spnr_anti_vox->step(step);
			spnr_anti_vox->value(progStatus.vox_anti);
			spnr_anti_vox->show();
			selrig->set_vox_anti();
		} else
			spnr_anti_vox->hide();

		if (selrig->has_vox_hang) {
			int min, max, step;
			selrig->get_vox_hang_min_max_step(min, max, step);
			spnr_vox_hang->minimum(min);
			spnr_vox_hang->maximum(max);
			spnr_vox_hang->step(step);
			spnr_vox_hang->value(progStatus.vox_hang);
			spnr_vox_hang->show();
			selrig->set_vox_hang();
		} else
			spnr_vox_hang->hide();

		if (selrig->has_vox_on_dataport) {
			btn_vox_on_dataport->value(progStatus.vox_on_dataport);
			btn_vox_on_dataport->show();
			selrig->set_vox_on_dataport();
		} else
			btn_vox_on_dataport->hide();

		tabsGeneric->add(genericVOX);
		genericVOX->redraw();
		genericVOX->show();
	}

	if (selrig->has_compON ||
		selrig->has_compression ) {

		if (selrig->has_compON) {
			btnCompON->show();
			btnCompON->value(progStatus.compON);
		} else
			btnCompON->hide();

		if (selrig->has_compression) {
			int min, max, step;
			selrig->get_comp_min_max_step(min, max, step);
			spnr_compression->minimum(min);
			spnr_compression->maximum(max);
			spnr_compression->step(step);
			spnr_compression->show();
			spnr_compression->value(progStatus.compression);
			selrig->set_compression(progStatus.compON, progStatus.compression);
		} else
			spnr_compression->hide();

		tabsGeneric->add(genericSpeech);
		genericSpeech->redraw();
		genericSpeech->show();
	}

	if (selrig->has_nb_level ||
		selrig->has_bpf_center ) {

		if (selrig->has_nb_level)
			sldr_nb_level->show();
		else
			sldr_nb_level->hide();

		spnr_bpf_center->show();
		btn_use_bpf_center->show();
		if (selrig->has_bpf_center) {
			spnr_bpf_center->value(progStatus.bpf_center);
			spnr_bpf_center->activate();
			btn_use_bpf_center->activate();
		} else {
			spnr_bpf_center->deactivate();
			btn_use_bpf_center->deactivate();
		}
		tabsGeneric->add(genericRx);
		genericRx->redraw();
		genericRx->show();
	}

	if (selrig->has_vfo_adj ||
		selrig->has_line_out ||
		selrig->has_xcvr_auto_on_off ||
		selrig->can_synch_clock ) {

		if (selrig->has_vfo_adj) {
			double min, max, step;
			selrig->get_vfoadj_min_max_step(min, max, step);
			spnr_vfo_adj->minimum(min);
			spnr_vfo_adj->maximum(max);
			spnr_vfo_adj->step(step);
			progStatus.vfo_adj = selrig->getVfoAdj();
			spnr_vfo_adj->value(progStatus.vfo_adj);
			spnr_vfo_adj->show();
		} else
			spnr_vfo_adj->hide();

		spnr_line_out->show();
		if (selrig->has_line_out)
			spnr_line_out->activate();
		else
			spnr_line_out->deactivate();

		btn_xcvr_auto_on->show();
		btn_xcvr_auto_off->show();
		if (selrig->has_xcvr_auto_on_off) {
			btn_xcvr_auto_on->value(progStatus.xcvr_auto_on);
			btn_xcvr_auto_off->value(progStatus.xcvr_auto_off);
			btn_xcvr_auto_on->activate();
			btn_xcvr_auto_off->activate();
		} else {
			btn_xcvr_auto_on->deactivate();
			btn_xcvr_auto_off->deactivate();
		}

		if (selrig->can_synch_clock) {
			btn_xcvr_synch_clock->show();
			btn_xcvr_synch_gmt->show();
			btn_xcvr_synch_now->show();
			txt_xcvr_synch->show();
		} else {
			btn_xcvr_synch_clock->hide();
			btn_xcvr_synch_gmt->hide();
			btn_xcvr_synch_now->hide();
			txt_xcvr_synch->hide();
		}
		btn_xcvr_synch_clock->redraw();
		btn_xcvr_synch_gmt->redraw();
		btn_xcvr_synch_now->redraw();
		txt_xcvr_synch->redraw();

		tabsGeneric->add(genericMisc);
		genericMisc->redraw();
		genericMisc->show();

	}


	btnAuxDTR->hide();
	btnAuxRTS->hide();
	btnDataPort->hide();

	if (progStatus.aux_serial_port != "NONE") {
		btnAuxRTS->show();
		btnAuxDTR->show();
		tabsGeneric->add(genericAux);
	}

	tabsGeneric->add(genericUser_1);
	genericUser_1->redraw();
	genericUser_1->show();

	tabsGeneric->add(genericUser_2);
	genericUser_2->redraw();
	genericUser_2->show();

	tabsGeneric->add(genericUser_3);
	genericUser_3->redraw();
	genericUser_3->show();

	if (selrig->name_ == rig_IC7610.name_) {

		tabsGeneric->add(tab7610);
		tab7610->redraw();
		tab7610->show();

		btnAttenuator->hide();
	}

	tabsGeneric->redraw();
	tabsGeneric->show();

	if (progStatus.UIsize != touch_ui)
		tabs_dialog->init_sizes();

	poll_frequency->activate(); poll_frequency->value(progStatus.poll_frequency);
	poll_mode->activate(); poll_mode->value(progStatus.poll_mode);
	poll_bandwidth->activate(); poll_bandwidth->value(progStatus.poll_bandwidth);

	poll_smeter->activate(); poll_smeter->value(progStatus.poll_smeter);
	poll_pout->activate(); poll_pout->value(progStatus.poll_pout);
	poll_swr->activate(); poll_swr->value(progStatus.poll_swr);
	poll_alc->activate(); poll_alc->value(progStatus.poll_alc);
	poll_volume->activate(); poll_volume->value(progStatus.poll_volume);
	poll_notch->activate(); poll_notch->value(progStatus.poll_notch);
	poll_auto_notch->activate(); poll_auto_notch->value(progStatus.poll_auto_notch);
	poll_ifshift->activate(); poll_ifshift->value(progStatus.poll_ifshift);
	poll_power_control->activate(); poll_power_control->value(progStatus.poll_power_control);
	poll_pre_att->activate(); poll_pre_att->value(progStatus.poll_pre_att);
	poll_squelch->activate(); poll_squelch->value(progStatus.poll_squelch);
	poll_micgain->activate(); poll_micgain->value(progStatus.poll_micgain);
	poll_rfgain->activate(); poll_rfgain->value(progStatus.poll_rfgain);
	poll_split->activate(); poll_split->value(progStatus.poll_split);
	poll_noise->activate(); poll_noise->value(progStatus.poll_noise);
	poll_nr->activate(); poll_nr->value(progStatus.poll_nr);
	poll_compression->activate(); poll_compression->value(progStatus.poll_compression);

	if (!selrig->has_bandwidth_control) { poll_bandwidth->deactivate(); poll_bandwidth->value(0); }
	if (!selrig->has_smeter) { poll_smeter->deactivate(); poll_smeter->value(0); }
	if (!selrig->has_power_out) { poll_pout->deactivate(); poll_pout->value(0); }
	if (!selrig->has_swr_control) { poll_swr->deactivate(); poll_swr->value(0); }
	if (!selrig->has_alc_control) { poll_alc->deactivate(); poll_alc->value(0); }
	if (!selrig->has_volume_control) { poll_volume->deactivate(); poll_volume->value(0); }
	if (!selrig->has_notch_control) { poll_notch->deactivate(); poll_notch->value(0); }
	if (!selrig->has_auto_notch ||
		xcvr_name == rig_FT1000MP.name_ )
		{ poll_auto_notch->deactivate(); poll_auto_notch->value(0); }
	if (!selrig->has_ifshift_control &&
		!selrig->has_pbt_controls) { poll_ifshift->deactivate(); poll_ifshift->value(0); }
	if (selrig->has_pbt_controls) {
		poll_ifshift->label("pbt");
		poll_ifshift->redraw_label();
	}
	if (!selrig->has_power_control) { poll_power_control->deactivate(); poll_power_control->value(0); }
	if (!selrig->has_preamp_control && !selrig->has_attenuator_control)
		{ poll_pre_att->deactivate(); poll_pre_att->value(0); }
	if (!selrig->has_sql_control) { poll_squelch->deactivate(); poll_squelch->value(0); }
	if (!selrig->has_micgain_control) { poll_micgain->deactivate(); poll_micgain->value(0); }
	if (!selrig->has_rf_control) { poll_rfgain->deactivate(); poll_rfgain->value(0); }
	if (!selrig->has_split) { poll_split->deactivate(); poll_split->value(0); }
	if (!selrig->has_noise_control) {poll_noise->deactivate(); poll_noise->value(0);}
	if (!selrig->has_noise_reduction) {poll_nr->deactivate(); poll_nr->value(0);}
	if (!selrig->has_compression) { poll_compression->deactivate(); poll_compression->value(0); }

}

void TRACED(initTabs)

	if (xcvr_name == rig_TT550.name_)
		init_TT550_tabs();
	else
		init_Generic_Tabs();
}

void TRACED(init_rit)

	if (!cntRIT) return;
	if (selrig->has_rit) {
		int min, max, step;
		selrig->get_RIT_min_max_step(min, max, step);
		cntRIT->minimum(min);
		cntRIT->maximum(max);
		cntRIT->step(step);
		switch (progStatus.UIsize) {
			case touch_ui :
				cntRIT->activate();
				break;
			case small_ui :
			case wide_ui :
			default :
				cntRIT->show();
		}
		cntRIT->value(progStatus.rit_freq);
	} else {
		switch (progStatus.UIsize) {
			case touch_ui :
				cntRIT->deactivate();
				break;
			case small_ui :
			case wide_ui :
			default :
				cntRIT->hide();
		}
	}
}

void TRACED(init_xit)

	if (!cntXIT) return;
	if (selrig->has_xit) {
		int min, max, step;
		selrig->get_XIT_min_max_step(min, max, step);
		cntXIT->minimum(min);
		cntXIT->maximum(max);
		cntXIT->step(step);
		cntXIT->value(progStatus.xit_freq);
		switch (progStatus.UIsize) {
			case small_ui :
				cntXIT->show();
				break;
			case wide_ui : case touch_ui : default :
				cntXIT->activate();
			}
	} else {
		switch (progStatus.UIsize) {
			case small_ui :
				cntXIT->hide();
			case wide_ui : case touch_ui : default :
				cntXIT->deactivate();
		}
	}
}

void TRACED(init_bfo)

	if (!cntBFO) return;
	if (selrig->has_bfo) {
		int min, max, step;
		selrig->get_BFO_min_max_step(min, max, step);
		cntBFO->minimum(min);
		cntBFO->maximum(max);
		cntBFO->step(step);
		cntBFO->value(progStatus.bfo_freq);
		switch (progStatus.UIsize) {
			case small_ui :
				cntBFO->show();
				break;
			case wide_ui : case touch_ui : default :
				cntBFO->activate();
		}
	} else {
		switch (progStatus.UIsize) {
			case small_ui :
				cntBFO->hide();
				break;
			case wide_ui : case touch_ui : default :
				cntBFO->deactivate();
		}
	}
}

void TRACED(init_dsp_controls)

	if (selrig->has_dsp_controls) {
		opDSP_lo->clear();
		opDSP_hi->clear();
		btnDSP->label(selrig->SL_label);
		btnDSP->redraw_label();
		for (int i = 0; selrig->dsp_SL[i] != NULL; i++)
			opDSP_lo->add(selrig->dsp_SL[i]);
		opDSP_lo->tooltip(selrig->SL_tooltip);
		for (int i = 0; selrig->dsp_SH[i] != NULL; i++)
			opDSP_hi->add(selrig->dsp_SH[i]);
		opDSP_hi->tooltip(selrig->SH_tooltip);
		if (vfo->iBW > 256) {
			opDSP_lo->index(vfo->iBW & 0xFF);
			opDSP_hi->index((vfo->iBW >> 8) & 0x7F);
			btnDSP->show();
			opDSP_hi->show();
			opDSP_lo->hide();
			opBW->hide();
			opBW->index(0);
			btnFILT->hide();
		} else {
			opDSP_lo->index(0);
			opDSP_hi->index(0);
			btnDSP->hide();
			opDSP_lo->hide();
			opDSP_hi->hide();
			btnFILT->hide();
			opBW->show();
		}
	} else if (selrig->has_FILTER) {
		btnDSP->hide();
		opDSP_lo->hide();
		opDSP_hi->hide();
		btnFILT->show();
		opBW->resize(opDSP_lo->x(), opDSP_lo->y(), opDSP_lo->w(), opDSP_lo->h());
		opBW->redraw();
		opBW->show();
	} else {
		btnDSP->hide();
		opDSP_lo->hide();
		opDSP_hi->hide();
		btnFILT->hide();
		opBW->show();
	}
}

void TRACED(init_volume_control)

	if (selrig->has_volume_control) {
		int min, max, step;
		selrig->get_vol_min_max_step(min, max, step);
		if (spnrVOLUME) {
			spnrVOLUME->minimum(min);
			spnrVOLUME->maximum(max);
			spnrVOLUME->step(step);
			spnrVOLUME->redraw();
			spnrVOLUME->activate();
		}
		if (sldrVOLUME) {
			sldrVOLUME->minimum(min);
			sldrVOLUME->maximum(max);
			sldrVOLUME->step(step);
			sldrVOLUME->redraw();
			sldrVOLUME->activate();
		}
		switch (progStatus.UIsize) {
			case small_ui :
				btnVol->show();
				if (sldrVOLUME) sldrVOLUME->show();
				if (spnrVOLUME) spnrVOLUME->show();
				break;
			case wide_ui : case touch_ui : default :
				btnVol->activate();
				if (sldrVOLUME) sldrVOLUME->activate();
				if (spnrVOLUME) spnrVOLUME->activate();
		}
	} else {
		switch (progStatus.UIsize) {
			case small_ui :
				btnVol->hide();
				if (sldrVOLUME) sldrVOLUME->hide();
				if (spnrVOLUME) spnrVOLUME->hide();
				break;
			case wide_ui : case touch_ui : default :
				btnVol->deactivate();
				if (sldrVOLUME) sldrVOLUME->deactivate();
				if (spnrVOLUME) spnrVOLUME->deactivate();
		}
	}
}

void TRACED(set_init_volume_control)

	if (!selrig->has_volume_control)
		return;

	if (progStatus.use_rig_data) {
		progStatus.volume = selrig->get_volume_control();
		if (sldrVOLUME) sldrVOLUME->value(progStatus.volume);
		if (sldrVOLUME) sldrVOLUME->activate();
		btnVol->value(1);
		if (spnrVOLUME) spnrVOLUME->value(progStatus.volume);
		if (spnrVOLUME) spnrVOLUME->activate();
		sldrVOLUME->activate();
	} else {
		if (sldrVOLUME) sldrVOLUME->value(progStatus.volume);
		if (spnrVOLUME) spnrVOLUME->value(progStatus.volume);
		if (progStatus.spkr_on == 0) {
			btnVol->value(0);
			if (sldrVOLUME) sldrVOLUME->deactivate();
			if (spnrVOLUME) spnrVOLUME->deactivate();
			selrig->set_volume_control(0);
		} else {
			btnVol->value(1);
			if (sldrVOLUME) sldrVOLUME->activate();
			if (spnrVOLUME) spnrVOLUME->activate();
			selrig->set_volume_control(progStatus.volume);
		}
	}
}

void TRACED(init_rf_control)

	if (selrig->has_rf_control) {
		int min, max, step;
		selrig->get_rf_min_max_step(min, max, step);
		if (sldrRFGAIN) sldrRFGAIN->minimum(min);
		if (sldrRFGAIN) sldrRFGAIN->maximum(max);
		if (sldrRFGAIN) sldrRFGAIN->step(step);
		if (sldrRFGAIN) sldrRFGAIN->redraw();
		if (spnrRFGAIN) spnrRFGAIN->minimum(min);
		if (spnrRFGAIN) spnrRFGAIN->maximum(max);
		if (spnrRFGAIN) spnrRFGAIN->step(step);
		if (spnrRFGAIN) spnrRFGAIN->redraw();

		switch (progStatus.UIsize) {
			case small_ui :
				if (sldrRFGAIN) sldrRFGAIN->show();
				if (spnrRFGAIN) spnrRFGAIN->show();
				break;
			case wide_ui : case touch_ui : default :
				if (sldrRFGAIN) sldrRFGAIN->activate();
				if (spnrRFGAIN) spnrRFGAIN->activate();
		}
	} else {
		switch (progStatus.UIsize) {
			case small_ui :
				if (sldrRFGAIN) sldrRFGAIN->hide();
				if (spnrRFGAIN) spnrRFGAIN->hide();
				break;
			case wide_ui : case touch_ui : default :
				if (sldrRFGAIN) sldrRFGAIN->deactivate();
				if (spnrRFGAIN) spnrRFGAIN->deactivate();
		}
	}
	if (selrig->name_ == rig_ICF8101.name_) {
		if (sldrRFGAIN) sldrRFGAIN->deactivate();
		if (spnrRFGAIN) spnrRFGAIN->deactivate();
	}
}

void TRACED(set_init_rf_gain)

	if (!selrig->has_rf_control)
		return;

	if (progStatus.use_rig_data) {
		progStatus.rfgain = selrig->get_rf_gain();
		if (sldrRFGAIN) sldrRFGAIN->value(progStatus.rfgain);
		if (spnrRFGAIN) spnrRFGAIN->value(progStatus.rfgain);
	} else {
		if (sldrRFGAIN) sldrRFGAIN->value(progStatus.rfgain);
		if (spnrRFGAIN) spnrRFGAIN->value(progStatus.rfgain);
		selrig->set_rf_gain(progStatus.rfgain);
	}
}

void TRACED(init_sql_control)

	if (selrig->has_sql_control) {
		int min, max, step;
		selrig->get_squelch_min_max_step(min, max, step);
		if (sldrSQUELCH) sldrSQUELCH->minimum(min);
		if (sldrSQUELCH) sldrSQUELCH->maximum(max);
		if (sldrSQUELCH) sldrSQUELCH->step(step);
		if (sldrSQUELCH) sldrSQUELCH->redraw();
		if (spnrSQUELCH) spnrSQUELCH->minimum(min);
		if (spnrSQUELCH) spnrSQUELCH->maximum(max);
		if (spnrSQUELCH) spnrSQUELCH->step(step);
		if (spnrSQUELCH) spnrSQUELCH->redraw();
		switch (progStatus.UIsize) {
			case small_ui :
				if (sldrSQUELCH) sldrSQUELCH->show();
				if (spnrSQUELCH) spnrSQUELCH->show();
				break;
			case wide_ui : case touch_ui : default:
				if (sldrSQUELCH) sldrSQUELCH->activate();
				if (spnrSQUELCH) spnrSQUELCH->activate();
		}

	} else {
		switch (progStatus.UIsize) {
			case small_ui :
				if (sldrSQUELCH) sldrSQUELCH->hide();
				if (spnrSQUELCH) spnrSQUELCH->hide();
				break;
			case wide_ui : case touch_ui : default :
				if (sldrSQUELCH) sldrSQUELCH->deactivate();
				if (spnrSQUELCH) spnrSQUELCH->deactivate();
		}
	}
}

void TRACED(set_init_sql_control)

	if (!selrig->has_sql_control)
		return;

	if (progStatus.use_rig_data) {
		progStatus.squelch = selrig->get_squelch();
		if (sldrSQUELCH) sldrSQUELCH->value(progStatus.squelch);
		if (spnrSQUELCH) spnrSQUELCH->value(progStatus.squelch);
	} else {
		if (sldrSQUELCH) sldrSQUELCH->value(progStatus.squelch);
		if (spnrSQUELCH) spnrSQUELCH->value(progStatus.squelch);
		selrig->set_squelch(progStatus.squelch);
	}
}

void TRACED(set_init_noise_reduction_control)

	if (!selrig->has_noise_reduction_control)
		return;

	if (progStatus.use_rig_data) {
		progStatus.noise_reduction = selrig->get_noise_reduction();
		progStatus.noise_reduction_val = selrig->get_noise_reduction_val();
		btnNR->value(progStatus.noise_reduction);
		if (sldrNR) sldrNR->value(progStatus.noise_reduction_val);
		if (spnrNR) spnrNR->value(progStatus.noise_reduction_val);

		if (selrig->name_ == rig_FT891.name_) {
			// On the FT-891, the usual definitions of NB and NR buttons
			// as defined in FLRIG are reversed. Relabel them to match
			// what the user sees in the radio screens, and handle the
			// mapping to appropriate cat controls in the FT891.xx class.
			btnNR->label("NB");
			btnNR->tooltip(_("Noise Blanker On/Off"));
		}

	} else {
		btnNR->value(progStatus.noise_reduction);
		if (sldrNR) sldrNR->value(progStatus.noise_reduction_val);
		if (spnrNR) spnrNR->value(progStatus.noise_reduction_val);
		selrig->set_noise_reduction(progStatus.noise_reduction);
		selrig->set_noise_reduction_val(progStatus.noise_reduction_val);
	}
}

void TRACED(init_noise_reduction_control)

	if (selrig->has_noise_reduction_control) {
		int min, max, step;
		selrig->get_nr_min_max_step(min, max, step);

		if (sldrNR) sldrNR->minimum(min);
		if (sldrNR) sldrNR->maximum(max);
		if (sldrNR) sldrNR->step(step);
		if (sldrNR) sldrNR->redraw();

		if (spnrNR) spnrNR->minimum(min);
		if (spnrNR) spnrNR->maximum(max);
		if (spnrNR) spnrNR->step(step);
		if (spnrNR) spnrNR->redraw();

		if (selrig->name_ == rig_FT891.name_) {
			// On the FT-891, the usual definitions of NB and NR buttons
			// as defined in FLRIG are reversed. Relabel them to match
			// what the user sees in the radio screens, and handle the
			// mapping to appropriate cat controls in the FT891.xx class.
			sldrNR->tooltip(_("Adjust noise blanker"));
		}

		switch (progStatus.UIsize) {
			case small_ui :
				btnNR->show();
				if (sldrNR) sldrNR->show();
				if (spnrNR) spnrNR->show();
				break;
			case wide_ui : case touch_ui : default:
				btnNR->show();
				if (sldrNR) sldrNR->show();//activate();
				if (spnrNR) spnrNR->show();//activate();
				break;
		}
	} else {
		switch (progStatus.UIsize) {
			case small_ui :
				btnNR->hide();
				if (sldrNR) sldrNR->hide();
				if (spnrNR) sldrNR->hide();
				break;
			case wide_ui : case touch_ui : default :
				btnNR->hide();//deactivate();
				if (sldrNR) sldrNR->hide();//deactivate();
				if (spnrNR) spnrNR->hide();//deactivate();
				break;
		}
	}
}

void TRACED(set_init_if_shift_control)

	if (!selrig->has_ifshift_control)
		return;

	if (progStatus.use_rig_data) {
		progStatus.shift = selrig->get_if_shift(progStatus.shift_val);
		btnIFsh->value(progStatus.shift);
		if (sldrIFSHIFT) sldrIFSHIFT->value(progStatus.shift_val);
		if (spnrIFSHIFT) spnrIFSHIFT->value(progStatus.shift_val);
	} else {
		if (progStatus.shift) {
			btnIFsh->value(1);
			if (sldrIFSHIFT) sldrIFSHIFT->value(progStatus.shift_val);
			if (spnrIFSHIFT) spnrIFSHIFT->value(progStatus.shift_val);
			selrig->set_if_shift(progStatus.shift_val);
		} else {
			btnIFsh->value(0);
			if (sldrIFSHIFT) sldrIFSHIFT->value(selrig->if_shift_mid);
			if (spnrIFSHIFT) spnrIFSHIFT->value(selrig->if_shift_mid);
			selrig->set_if_shift(selrig->if_shift_mid);
		}
	}
}

void TRACED(init_if_shift_control)

	if (btnLOCK) btnLOCK->hide();
	if (btnCLRPBT) btnCLRPBT->hide();
	if (sldrINNER) sldrINNER->hide();
	if (sldrOUTER) sldrOUTER->hide();
	if (btnIFsh) btnIFsh->hide();
	if (sldrIFSHIFT) sldrIFSHIFT->hide();
	if (spnrIFSHIFT) spnrIFSHIFT->hide();

	if (selrig->has_ifshift_control) {
		if (xcvr_name != rig_KX3.name_)
			btnIFsh->show();
		else
			btn_KX3_IFsh->show();
		sldrIFSHIFT->show();
		if (spnrIFSHIFT) spnrIFSHIFT->show();
		int min, max, step;
		selrig->get_if_min_max_step(min, max, step);
		if (sldrIFSHIFT) sldrIFSHIFT->minimum(min);
		if (sldrIFSHIFT) sldrIFSHIFT->maximum(max);
		if (sldrIFSHIFT) sldrIFSHIFT->step(step);
		if (sldrIFSHIFT) sldrIFSHIFT->redraw();
		if (spnrIFSHIFT) spnrIFSHIFT->minimum(min);
		if (spnrIFSHIFT) spnrIFSHIFT->maximum(max);
		if (spnrIFSHIFT) spnrIFSHIFT->step(step);
		if (spnrIFSHIFT) spnrIFSHIFT->redraw();
		switch (progStatus.UIsize) {
			case small_ui :
				if (xcvr_name != rig_KX3.name_)
					btnIFsh->show();
				else
					btn_KX3_IFsh->show();
				if (sldrIFSHIFT) sldrIFSHIFT->show();
				if (spnrIFSHIFT) spnrIFSHIFT->show();
				break;
			case wide_ui : case touch_ui : default :
				if (xcvr_name != rig_KX3.name_) btnIFsh->activate();
				if (sldrIFSHIFT) sldrIFSHIFT->activate();
				if (spnrIFSHIFT) spnrIFSHIFT->activate();
				break;
		}
	} else {
		switch (progStatus.UIsize) {
			case small_ui :
				btnIFsh->hide();
				if (sldrIFSHIFT) sldrIFSHIFT->hide();
				if (spnrIFSHIFT) spnrIFSHIFT->hide();
				break;
			case wide_ui : case touch_ui : default :
				btnIFsh->deactivate();
				if (sldrIFSHIFT) sldrIFSHIFT->deactivate();
				if (spnrIFSHIFT) spnrIFSHIFT->deactivate();
				break;
		}
	}
	if (selrig->has_pbt_controls) {
		btnLOCK->show();
		btnLOCK->value(progStatus.pbt_lock);
		btnCLRPBT->show();
		sldrINNER->show();
		sldrOUTER->show();
		sldrINNER->value(progStatus.pbt_inner);
		sldrOUTER->value(progStatus.pbt_outer);
	}

	if (xcvr_name == rig_TS870S.name_) {
		if (progStatus.imode_A == RIG_TS870S::tsCW ||
			progStatus.imode_A == RIG_TS870S::tsCWR) {
			btnIFsh->activate();
			if (sldrIFSHIFT) sldrIFSHIFT->activate();
			if (spnrIFSHIFT) spnrIFSHIFT->activate();
		} else {
			btnIFsh->deactivate();
			if (sldrIFSHIFT) sldrIFSHIFT->deactivate();
			if (spnrIFSHIFT) spnrIFSHIFT->deactivate();
		}
	}
}

void TRACED(init_notch_control)

	if (selrig->has_notch_control) {
		int min, max, step;
		selrig->get_notch_min_max_step(min, max, step);

		if (sldrNOTCH) sldrNOTCH->minimum(min);
		if (sldrNOTCH) sldrNOTCH->maximum(max);
		if (sldrNOTCH) sldrNOTCH->step(step);
		if (sldrNOTCH) sldrNOTCH->redraw();
		if (spnrNOTCH) spnrNOTCH->minimum(min);
		if (spnrNOTCH) spnrNOTCH->maximum(max);
		if (spnrNOTCH) spnrNOTCH->step(step);
		if (spnrNOTCH) spnrNOTCH->redraw();
		switch (progStatus.UIsize) {
			case small_ui :
				btnNotch->show();
				if (sldrNOTCH) sldrNOTCH->show();
				if (spnrNOTCH) spnrNOTCH->show();
				break;
			case wide_ui : case touch_ui : default :
				btnNotch->activate();
				if (sldrNOTCH) sldrNOTCH->activate();
				if (spnrNOTCH) spnrNOTCH->activate();
				break;
		}
	} else {
		switch (progStatus.UIsize) {
			case small_ui :
				btnNotch->hide();
				if (sldrNOTCH) sldrNOTCH->hide();
				if (spnrNOTCH) spnrNOTCH->hide();
				break;
			case wide_ui : case touch_ui : default :
				btnNotch->deactivate();
				if (sldrNOTCH) sldrNOTCH->deactivate();
				if (spnrNOTCH) spnrNOTCH->deactivate();
				break;
		}
	}
}

void TRACED(set_init_notch_control)

	if (selrig->has_notch_control) {
		if (progStatus.use_rig_data) {
			progStatus.notch = selrig->get_notch(progStatus.notch_val);
			btnNotch->value(progStatus.notch);
			if (sldrNOTCH) sldrNOTCH->value(progStatus.notch_val);
			if (spnrNOTCH) spnrNOTCH->value(progStatus.notch_val);
		} else {
			btnNotch->value(progStatus.notch);
			if (sldrNOTCH) sldrNOTCH->value(progStatus.notch_val);
			if (spnrNOTCH) spnrNOTCH->value(progStatus.notch_val);
			selrig->set_notch(progStatus.notch, progStatus.notch_val);
		}
	}
}

void TRACED(init_micgain_control)

	if (selrig->has_micgain_control) {
		int min = 0, max = 0, step = 0;
		selrig->get_mic_min_max_step(min, max, step);
		if (sldrMICGAIN) sldrMICGAIN->minimum(min);
		if (sldrMICGAIN) sldrMICGAIN->maximum(max);
		if (sldrMICGAIN) sldrMICGAIN->step(step);
		if (spnrMICGAIN) spnrMICGAIN->minimum(min);
		if (spnrMICGAIN) spnrMICGAIN->maximum(max);
		if (spnrMICGAIN) spnrMICGAIN->step(step);

		switch (progStatus.UIsize) {
			case small_ui :
				if (sldrMICGAIN) sldrMICGAIN->show();
				if (spnrMICGAIN) spnrMICGAIN->show();
				break;
			case wide_ui : case touch_ui : default :
				if (sldrMICGAIN) sldrMICGAIN->activate();
				if (spnrMICGAIN) spnrMICGAIN->activate();
				break;
		}
	} else {
		if (sldrMICGAIN) sldrMICGAIN->deactivate();
		if (spnrMICGAIN) spnrMICGAIN->deactivate();
	}
	switch (progStatus.UIsize) {
		case small_ui :
			if (sldrMICGAIN) sldrMICGAIN->hide();
			if (spnrMICGAIN) spnrMICGAIN->hide();
			break;
		case wide_ui : case touch_ui : default :
			if (sldrMICGAIN) sldrMICGAIN->deactivate();
			if (spnrMICGAIN) spnrMICGAIN->deactivate();
	}
}

void TRACED(set_init_micgain_control)

	int min, max, step;
	if (selrig->has_micgain_control) {
		if (progStatus.use_rig_data)
			progStatus.mic_gain = selrig->get_mic_gain();
		else
			selrig->set_mic_gain(progStatus.mic_gain);

		selrig->get_mic_min_max_step(min, max, step);
		if (sldrMICGAIN) {
			sldrMICGAIN->minimum(min);
			sldrMICGAIN->maximum(max);
			sldrMICGAIN->step(step);
			sldrMICGAIN->value(progStatus.mic_gain);
			sldrMICGAIN->activate();
		}
		if (spnrMICGAIN) {
			spnrMICGAIN->minimum(min);
			spnrMICGAIN->maximum(max);
			spnrMICGAIN->step(step);
			spnrMICGAIN->value(progStatus.mic_gain);
			spnrMICGAIN->activate();
		}
	} else {
		if (sldrMICGAIN) sldrMICGAIN->deactivate();
		if (spnrMICGAIN) sldrMICGAIN->deactivate();
	}
}

void TRACED(init_power_control)

	double min, max, step;
	if (selrig->has_power_control) {

		sldrPOWER->activate();
		selrig->get_pc_min_max_step(min, max, step);
		if (sldrPOWER) sldrPOWER->minimum(min);
		if (sldrPOWER) sldrPOWER->maximum(max);
		if (sldrPOWER) sldrPOWER->step(step);
		if (sldrPOWER) sldrPOWER->value(progStatus.power_level);
		if (sldrPOWER) sldrPOWER->show();
		if (sldrPOWER) sldrPOWER->redraw();

		if (spnrPOWER) spnrPOWER->minimum(min);
		if (spnrPOWER) spnrPOWER->maximum(max);
		if (spnrPOWER) spnrPOWER->step(step);
		if (spnrPOWER) spnrPOWER->value(progStatus.power_level);
		if (spnrPOWER) spnrPOWER->show();
		if (spnrPOWER) spnrPOWER->redraw();

	} else {
 		if (sldrPOWER) sldrPOWER->deactivate();
		if (spnrPOWER) spnrPOWER->deactivate();
	}
}

void TRACED(set_init_power_control)

	if (selrig->has_power_control) {
		if (progStatus.use_rig_data)
			progStatus.power_level = selrig->get_power_control();
		else
			selrig->set_power_control(progStatus.power_level);
	}
	set_power_controlImage(progStatus.power_level);
}

void TRACED(init_attenuator_control)

	if (selrig->has_attenuator_control) {
		if (selrig->name_ == rig_FT891.name_) {
			btnAttenuator->label("ATT");
			btnAttenuator->redraw_label();
		}
		switch (progStatus.UIsize) {
			case small_ui :
				btnAttenuator->show();
				break;
			case wide_ui : case touch_ui : default :
				btnAttenuator->activate();
		}
	} else {
		switch (progStatus.UIsize) {
			case small_ui :
				btnAttenuator->hide();
				break;
			case wide_ui : case touch_ui : default :
				btnAttenuator->deactivate();
		}
	}
}

void TRACED(set_init_attenuator_control)

	if (selrig->has_attenuator_control) {
		if (!progStatus.use_rig_data)
			selrig->set_attenuator(progStatus.attenuator);
	}
}

void TRACED(init_agc_control)

	if (selrig->has_agc_control) {
		btnAGC->show();
		sldrRFGAIN->label("");
		sldrRFGAIN->redraw_label();
	} else {
		btnAGC->hide();
		sldrRFGAIN->label(_("RF"));
		sldrRFGAIN->redraw_label();
	}
}

void TRACED(init_preamp_control)

	if (selrig->has_preamp_control) {
		if (selrig->name_ == rig_FT891.name_) {
			btnPreamp->label("IPO");
			btnPreamp->redraw_label();
		}
		switch (progStatus.UIsize) {
			case small_ui :
				btnPreamp->show();
				break;
			case wide_ui : case touch_ui : default :
			btnPreamp->activate();
		}
		if (xcvr_name == rig_IC9700.name_) {
			btnPreamp->tooltip("\
Internal/External preamp\n\
P0/E0 off/off\n\
P1/E0  on/off\n\
P0/E1 off/on\n\
P1/E1  on/on");
			btnPreamp->label("P0/E0");
			btnPreamp->redraw_label();
		} else {
			btnPreamp->tooltip("On/Off/Level");
		}
	} else {
		switch (progStatus.UIsize) {
			case small_ui :
				btnPreamp->hide();
				break;
			case wide_ui : case touch_ui : default :
				btnPreamp->deactivate();
		}
	}
}

void TRACED(set_init_preamp_control)

	if (selrig->has_preamp_control) {
		if (!progStatus.use_rig_data)
			selrig->set_preamp(progStatus.preamp);
	}
}

void TRACED(init_noise_control)

	int min, max, step;
	if (selrig->has_noise_control) {
		if (xcvr_name == rig_TS990.name_) {
			btnNOISE->label("AGC"); //Set TS990 AGC Label
			btnNOISE->tooltip("TS990 AGC control");
			btnNR->label("NR1"); //Set TS990 NR Button
			btnNR->tooltip("TS990 noise reduction control");
		}
		if (selrig->name_ == rig_FT891.name_) {
			// On the FT-891, the usual definitions of NB and NR buttons
			// as defined in FLRIG are reversed. Relabel them to match
			// what the user sees in the radio screens, and handle the
			// mapping to appropriate cat controls in the FT891.xx class.
			btnNOISE->label("DNR");
			btnNOISE->tooltip(_("DSP Noise Reduction On/Off.  See RX tab for DNR level."));
		}

		btnNOISE->show();
		btnNOISE->activate();
	}
	else {
		btnNOISE->hide();
		btnNOISE->deactivate();
	}

	if (selrig->has_nb_level) {
		selrig->get_nb_min_max_step(min, max, step);
		sldr_nb_level->minimum(min);
		sldr_nb_level->maximum(max);
		sldr_nb_level->step(step);
		sldr_nb_level->value(progStatus.nb_level);

		if (selrig->name_ == rig_FT891.name_) {
			// On the FT-891, the usual definitions of NB and NR buttons
			// as defined in FLRIG are reversed. Relabel them to match
			// what the user sees in the radio screens, and handle the
			// mapping to appropriate cat controls in the FT891.xx class.
			sldr_nb_level->label("DNR level");
			sldr_nb_level->tooltip(_("Adjust DSP Noise Reduction level"));
		}

		sldr_nb_level->activate();
		sldr_nb_level->redraw();
	} else
		sldr_nb_level->deactivate();
}

void TRACED(init_split_control)

	if (selrig->has_split || selrig->has_split_AB) {
		btnSplit->show();
		btnSplit->activate();
	} else {
		btnSplit->hide();
		btnSplit->deactivate();
	}
}

void TRACED(set_init_noise_control)

	if (selrig->has_noise_control) {
		if (progStatus.use_rig_data)
			progStatus.noise = selrig->get_noise();
		else
			selrig->set_noise(progStatus.noise);
		btnNOISE->value(progStatus.noise);
		btnNOISE->show();
		btnNOISE->activate();
	} else {
		btnNOISE->hide();
		btnNOISE->deactivate();
	}
}

void TRACED(init_tune_control)

	if (selrig->has_tune_control) {
		switch (progStatus.UIsize) {
			case small_ui :
				btnTune->show();
				btn_tune_on_off->show();
				break;
			case wide_ui : case touch_ui : default :
				btnTune->activate();
				btn_tune_on_off->activate();
		}
	} else {
		switch (progStatus.UIsize) {
			case small_ui :
				btnTune->hide();
				btn_tune_on_off->hide();
				break;
			case wide_ui : case touch_ui : default :
				btnTune->deactivate();
				btn_tune_on_off->deactivate();
		}
	}
}

void TRACED(init_ptt_control)

	if (selrig->has_ptt_control ||
		progStatus.comm_dtrptt == PTT_BOTH || progStatus.comm_dtrptt == PTT_SET ||
		progStatus.comm_rtsptt == PTT_BOTH || progStatus.comm_rtsptt == PTT_SET ||
		progStatus.sep_dtrptt  == PTT_BOTH || progStatus.sep_dtrptt  == PTT_SET ||
		progStatus.sep_rtsptt  == PTT_BOTH || progStatus.sep_rtsptt  == PTT_SET) {
		btnPTT->activate();
	} else {
		btnPTT->deactivate();
	}
}

void TRACED(init_auto_notch)

	if (selrig->has_auto_notch) {
		if (xcvr_name == rig_RAY152.name_) {
			btnAutoNotch->label("AGC");
			btnAutoNotch->tooltip("AGC on/off");
		} else if (xcvr_name == rig_FT1000MP.name_) {
			btnAutoNotch->label("Tuner");
			btnAutoNotch->tooltip("Tuner on/off");
		} else if (xcvr_name == rig_FT891.name_) {
			btnAutoNotch->label("DNF");
			btnAutoNotch->tooltip("DSP Auto notch filter on/off");
		} else {
			btnAutoNotch->label("AN");
			btnAutoNotch->tooltip("Auto notch on/off");
		}
		switch (progStatus.UIsize) {
			case small_ui :
				btnAutoNotch->show();
				break;
			case wide_ui : case touch_ui : default :
				btnAutoNotch->activate();
		}
	} else {
		switch (progStatus.UIsize) {
			case small_ui :
				btnAutoNotch->hide();
				break;
			case wide_ui : case touch_ui : default :
				btnAutoNotch->deactivate();
		}
	}
}

void TRACED(set_init_auto_notch)

	if (selrig->has_auto_notch) {
		if (progStatus.use_rig_data)
			progStatus.auto_notch = selrig->get_auto_notch();
		else
			selrig->set_auto_notch(progStatus.auto_notch);
		btnAutoNotch->value(progStatus.auto_notch);
	}
}

void TRACED(init_swr_control)

	if (selrig->has_swr_control)
		btnALC_SWR->activate();
	else {
		btnALC_SWR->deactivate();
	}
}

void TRACED(set_init_compression_control)

	if (selrig->has_compON || selrig->has_compression) {
		selrig->set_compression(progStatus.compON, progStatus.compression);

		if (selrig->name_ == rig_FT891.name_) {
			// On the FT-891, compression is called PRC, under function
			// menu FUNCTION-1.  Set the button to match for consistency.
			btnCompON->label("PRC");
			btnCompON->tooltip("Set speech processor for SSB modes on/off.");
		}
	}
}

void TRACED(set_init_break_in)

	if (!selrig->has_cw_break_in) return;
	selrig->get_break_in();
}

void TRACED(init_special_controls)

	btnSpecial->show();
	if (selrig->has_special)
		btnSpecial->activate();
	else
		btnSpecial->deactivate();
}

void TRACED(init_external_tuner)

	btn_ext_tuner->show();
	if (selrig->has_ext_tuner)
		btn_ext_tuner->activate();
	else
		btn_ext_tuner->deactivate();
}

void TRACED(init_CIV)

	if (selrig->CIV) {
		char hexstr[8];
		snprintf(hexstr, sizeof(hexstr), "0x%02X", selrig->CIV);
		txtCIV->value(hexstr);
		txtCIV->activate();
		btnCIVdefault->activate();
		if (selrig->name_ == rig_IC7200.name_ ||
			selrig->name_ == rig_IC7300.name_ ||
			selrig->name_ == rig_IC7600.name_ ||
			selrig->name_ == rig_IC7800.name_ ) {
			btnUSBaudio->value(progStatus.USBaudio = true);
			btnUSBaudio->activate();
		} else
			btnUSBaudio->deactivate();
	} else {
		txtCIV->value("");
		txtCIV->deactivate();
		btnCIVdefault->deactivate();
		btnUSBaudio->value(false);
		btnUSBaudio->deactivate();
	}
}

void TRACED(init_TS990_special)

	if (xcvr_name == rig_TS990.name_) { // Setup TS990 Mon Button
		btnIFsh->label("MON");
		btnIFsh->tooltip("\
Turn TX monitor on/off\n\
Slider controls TX monitor level");
	}
}

void TRACED(init_K3_KX3_special)

	if (xcvr_name == rig_K3.name_) {
		btnB->hide();
		btnA->hide();
		btnAswapB->hide();
		btn_KX3_swapAB->hide();
		btn_K3_swapAB->show();
		btn_K3_A2B->show();
	} else if (xcvr_name == rig_KX3.name_) {
		btnB->hide();
		btnA->hide();
		btnAswapB->hide();
		btn_K3_swapAB->hide();
		btn_K3_A2B->hide();
		btn_KX3_swapAB->show();
		btn_KX3_A2B->show();
	} else {
		btn_K3_swapAB->hide();
		btn_K3_A2B->hide();
		btn_KX3_swapAB->hide();
		btn_KX3_A2B->hide();
		labelMEMORY->hide();
		btnB->show();
		btnA->show();
		btnAswapB->show();
	}
}

void TRACED(initRig)

	xcvr_online = false;
	RigSerial->failed(0);
	if (tabs_dialog && tabs_dialog->visible()) tabs_dialog->hide();

	main_group->hide();

	grpInitializing->size(mainwindow->w(), mainwindow->h() - grpInitializing->y());
	grpInitializing->show();
	grpInitializing->redraw();
	mainwindow->damage();
	mainwindow->redraw();
	update_progress(0);
	Fl::check();

	sldrRcvSignal->aging(progStatus.rx_peak);
	sldrRcvSignal->avg(progStatus.rx_avg);
	sldrFwdPwr->aging(progStatus.pwr_peak);
	sldrFwdPwr->avg(progStatus.pwr_avg);
	sldrSWR->aging(progStatus.pwr_peak);
	sldrSWR->avg(progStatus.pwr_avg);
	sldrALC->aging(progStatus.pwr_peak);
	sldrALC->avg(progStatus.pwr_avg);

	sldrVoltage->aging(1);
	sldrVoltage->avg(1);

	sigbar_SMETER->aging(progStatus.rx_peak);
	sigbar_SMETER->avg(progStatus.rx_avg);
	sigbar_PWR->aging(progStatus.pwr_peak);
	sigbar_PWR->avg(progStatus.pwr_avg);
	sigbar_SWR->aging(progStatus.pwr_peak);
	sigbar_SWR->avg(progStatus.pwr_avg);
	sigbar_ALC->aging(progStatus.pwr_peak);
	sigbar_ALC->avg(progStatus.pwr_avg);

	sigbar_VOLTS->aging(1);
	sigbar_VOLTS->avg(1);

	if (selrig->has_voltmeter) {
		sldrVoltage->show();
		scaleVoltage->show();
		sldrFwdPwr->hide();
		scalePower->hide();
	} else {
		sldrVoltage->hide();
		scaleVoltage->hide();
		sldrFwdPwr->show();
		scalePower->show();
	}

	if (progStatus.use_tcpip) {
		try {
			connect_to_remote();
		} catch (...) {
			grpInitializing->hide();
			main_group->show();
			mainwindow->redraw();
			return;
		}
	}

// disable the serial thread
	{
		guard_lock gl_serial(&mutex_serial);
		trace(1, "init_rig()");

		init_special_controls();
		init_external_tuner();
		init_rit();
		init_xit();
		init_bfo();
		init_dsp_controls();
		init_volume_control();
		init_rf_control();
		init_sql_control();
		init_noise_reduction_control();
		init_if_shift_control();
		init_notch_control();
		init_micgain_control();
		init_power_control();
		init_attenuator_control();
		init_agc_control();
		init_preamp_control();
		init_noise_control();
		init_tune_control();
		init_ptt_control();
		init_auto_notch();
		init_swr_control();
		init_split_control();

		// Xcvr Auto Power on as soon as possible
		if (selrig->has_xcvr_auto_on_off && progStatus.xcvr_auto_on) {
			progress->label("Auto Start");
			progress->redraw_label();
			update_progress(0);
			selrig->set_xcvr_auto_on();
			progress->label("Initializing");
			progress->redraw_label();
			update_progress(0);
		}
trace(1, "selrig->initialize()");
		selrig->initialize();

if (!testmode) {
	trace(1, "selrig->check()");
		if (!selrig->check()) {
			trace(1, "FAILED");
			bypass_serial_thread_loop = true;

			xcvr_online = false;
			adjust_control_positions();
			grpInitializing->hide();
			main_group->show();
			mainwindow->redraw();

			show_controls();

			box_xcvr_connect->color(FL_BACKGROUND2_COLOR);
			box_xcvr_connect->redraw();
			fl_alert2(_("\
Transceiver not responding!\n\n\
Check serial (COM) port connection\n\
Open menu Config/Setup/Transceiver\n\
Press 'Update' button, reselect port\n\
Check that Baud matches transceiver baud\n\n\
Press 'Init' button."));
			return;
		}
	}

		FreqDispA->set_precision(selrig->precision);
		FreqDispA->set_ndigits(selrig->ndigits);
		FreqDispB->set_precision(selrig->precision);
		FreqDispB->set_ndigits(selrig->ndigits);

		if (xcvr_name == rig_TT550.name_)
			init_TT550();
		else
			init_generic_rig();

		set_init_volume_control();
		set_init_rf_gain();
		set_init_sql_control();
		set_init_noise_reduction_control();
		set_init_if_shift_control();
		set_init_micgain_control();
		set_init_power_control();
		set_init_attenuator_control();
		set_init_preamp_control();
		set_init_noise_control();
		set_init_auto_notch();
		set_init_notch_control();
		set_init_compression_control();
		set_init_break_in();

		initTabs();
		buildlist();

		init_CIV();

//		init_xcvr();

		selrig->post_initialize();

		init_TS990_special();
		init_K3_KX3_special();

		if (selrig->has_power_control) {
			if (progStatus.use_rig_data)
				progStatus.power_level = selrig->get_power_control();
			else
				selrig->set_power_control(progStatus.power_level);
		}
	}

	bypass_serial_thread_loop = false;

	grpInitializing->hide();
	grpInitializing->redraw();

	adjust_control_positions();

	if (selrig->name_ == rig_TS990.name_)  // UI tab issue pending real fix
		progStatus.show_tabs = false;

	show_controls();

	main_group->show();
	main_group->redraw();

	mainwindow->damage();
	mainwindow->show();
	mainwindow->redraw();

	Fl::flush();

	xcvr_online = true;
	box_xcvr_connect->color(FL_GREEN);
	box_xcvr_connect->redraw();

	start_commands();

	return;

}

void TRACED(init_title)

	title = PACKAGE;
	title += " ";
	title.append(selrig->name_);
	mainwindow->label(title.c_str());
}

void TRACED(initConfigDialog)

	int picked = selectRig->index();
	rigbase *srig = rigs[picked];
	xcvr_name = srig->name_;

	if (!progStatus.loadXcvrState(xcvr_name) ) {

		selectCommPort->index(0);
		mnuBaudrate->index( srig->comm_baudrate );
		btnOneStopBit->value( srig->stopbits == 1 );
		btnTwoStopBit->value( srig->stopbits == 2 );
		cntRigCatRetries->value( srig->comm_retries );
		cntRigCatTimeout->value( srig->comm_timeout );
		cntRigCatWait->value( srig->comm_wait );
		query_interval->value( srig->serloop_timing );

		btnRigCatEcho->value( srig->comm_echo );


		lbox_catptt->index( srig->comm_catptt );
		lbox_rtsptt->index( srig->comm_rtsptt );
		lbox_dtrptt->index( srig->comm_dtrptt );

		chkrtscts->value( srig->comm_rtscts );
		btnrtsplus1->value( srig->comm_rtsplus );
		btndtrplus1->value( srig->comm_dtrplus );
		btnrtsplus2->value( srig->comm_rtsplus );
		btndtrplus2->value( srig->comm_dtrplus );

		if (srig->CIV) {
			char hexstr[8];
			snprintf(hexstr, sizeof(hexstr), "0x%02X", srig->CIV);
			txtCIV->value(hexstr);
			txtCIV->activate();
			btnCIVdefault->activate();
			if (xcvr_name == rig_IC7200.name_ ||
				xcvr_name == rig_IC7300.name_ ||
				xcvr_name == rig_IC7600.name_ ||
				xcvr_name == rig_IC7800.name_) {
				btnUSBaudio->value(progStatus.USBaudio = true);
				btnUSBaudio->activate();
			} else
				btnUSBaudio->deactivate();
		} else {
			txtCIV->value("");
			txtCIV->deactivate();
			btnCIVdefault->deactivate();
			btnUSBaudio->value(false);
			btnUSBaudio->deactivate();
		}
	} else {
		initStatusConfigDialog();
		trace(1, progStatus.info().c_str());
	}
}

void TRACED(initStatusConfigDialog)

	if (progStatus.CIV) selrig->adjustCIV(progStatus.CIV);

	selectRig->value(xcvr_name.c_str());

	mnuBaudrate->index( progStatus.comm_baudrate );

	selectCommPort->value( progStatus.xcvr_serial_port.c_str() );
	selectAuxPort->value( progStatus.aux_serial_port.c_str() );
	selectSepPTTPort->value( progStatus.sep_serial_port.c_str() );
	btnOneStopBit->value( progStatus.stopbits == 1 );
	btnTwoStopBit->value( progStatus.stopbits == 2 );

	cntRigCatRetries->value( progStatus.comm_retries );
	cntRigCatTimeout->value( progStatus.comm_timeout );
	cntRigCatWait->value( progStatus.comm_wait );
	btnRigCatEcho->value( progStatus.comm_echo );
	query_interval->value( progStatus.serloop_timing );
	byte_interval->value( progStatus.byte_interval );

	lbox_catptt->index( progStatus.comm_catptt );
	lbox_rtsptt->index( progStatus.comm_rtsptt );
	lbox_dtrptt->index( progStatus.comm_dtrptt );

	chkrtscts->value( progStatus.comm_rtscts );
	btnrtsplus1->value( progStatus.comm_rtsplus );
	btndtrplus1->value( progStatus.comm_dtrplus );
	btnrtsplus2->value( progStatus.comm_rtsplus );
	btndtrplus2->value( progStatus.comm_dtrplus );

	btnSepDTRplus->value(progStatus.sep_dtrplus);
	lbox_sep_dtrptt->index(progStatus.sep_dtrptt);

	btnSepRTSplus->value(progStatus.sep_rtsplus);
	lbox_sep_rtsptt->index(progStatus.sep_rtsptt);

	if (progStatus.use_tcpip) {
		box_xcvr_connect->color(FL_BACKGROUND2_COLOR);
		box_xcvr_connect->redraw();
	} else {
		if (startXcvrSerial()) {
			selectCommPort->value(progStatus.xcvr_serial_port.c_str());
			box_xcvr_connect->color(FL_GREEN);
			box_xcvr_connect->redraw();
		} else {
			if (progStatus.xcvr_serial_port.compare("NONE") == 0) {
				LOG_WARN("No comm port ... test mode");
			} else {
				fl_alert2("\
Cannot open %s!\n\n\
Check serial (COM) port connection\n\
Open menu Config/Setup/Transceiver\n\
Press 'Update' button, reselect port\n\n\
Press 'Init' button.", progStatus.xcvr_serial_port.c_str());
				LOG_WARN("Cannot open %s", progStatus.xcvr_serial_port.c_str());
				progStatus.xcvr_serial_port = "NONE";
				selectCommPort->value(progStatus.xcvr_serial_port.c_str());
			}
			box_xcvr_connect->color(FL_BACKGROUND2_COLOR);
			box_xcvr_connect->redraw();
		}
		if ( progStatus.aux_serial_port != "NONE") {
			if (!startAuxSerial()) {
				LOG_WARN("Cannot open %s", progStatus.aux_serial_port.c_str());
				progStatus.aux_serial_port = "NONE";
				selectAuxPort->value(progStatus.aux_serial_port.c_str());
			}
		}
		if ( progStatus.sep_serial_port != "NONE") {
			if (!startSepSerial()) {
				LOG_WARN("Cannot open %s", progStatus.sep_serial_port.c_str());
				progStatus.sep_serial_port = "NONE";
				selectSepPTTPort->value(progStatus.sep_serial_port.c_str());
			}
		}
	}

	if (progStatus.cmedia_ptt)
		open_cmedia(progStatus.cmedia_device);

	init_title();

	initRig();

	if (progStatus.tmate2_connected)
		tmate2_open();
}

void TRACED(initRigCombo)

	selectRig->clear();
	int i = 0;
	while (rigs[i] != NULL)
		selectRig->add(rigs[i++]->name_.c_str());

	selectRig->index(0);
}

