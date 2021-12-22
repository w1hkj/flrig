// ----------------------------------------------------------------------------
// Copyright (C) 2021
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

#include "config.h"

#include "rigpanel.h"
#include "gettext.h"
#include "support.h"
#include "status.h"

Fl_Box *mtr_SMETER = (Fl_Box *)0;
Fl_Box *mtr_PWR    = (Fl_Box *)0;
Fl_Box *mtr_SWR    = (Fl_Box *)0;
Fl_Box *mtr_ALC    = (Fl_Box *)0;
Fl_Box *mtr_VOLTS  = (Fl_Box *)0;

Fl_SigBar *sigbar_SMETER = (Fl_SigBar *)0;
Fl_SigBar *sigbar_PWR    = (Fl_SigBar *)0;
Fl_SigBar *sigbar_SWR    = (Fl_SigBar *)0;
Fl_SigBar *sigbar_ALC    = (Fl_SigBar *)0;
Fl_SigBar *sigbar_VOLTS  = (Fl_SigBar *)0;

Fl_Double_Window* win_meters() {
	Fl_Double_Window* w = new Fl_Double_Window(210, 150, _("Meters"));

	Fl_Color bgclr = fl_rgb_color(progStatus.bg_red, progStatus.bg_green, progStatus.bg_blue);
	Fl_Color fgclr = fl_rgb_color(progStatus.fg_red, progStatus.fg_green, progStatus.fg_blue);

	w->color(bgclr);

	mtr_SMETER = new Fl_Box(2, 2, 206, 20);
	mtr_SMETER->box(FL_FLAT_BOX);
	mtr_SMETER->image(image_smeter);
	mtr_SMETER->tooltip(_("S units meter"));
	mtr_SMETER->color(bgclr);
	mtr_SMETER->labelcolor(fgclr);

	sigbar_SMETER = new Fl_SigBar(5, mtr_SMETER->y() + mtr_SMETER->h() + 2, 200, 6);
	sigbar_SMETER->box(FL_FLAT_BOX);
	sigbar_SMETER->color(FL_BACKGROUND_COLOR);
	sigbar_SMETER->selection_color(FL_BACKGROUND_COLOR);
	sigbar_SMETER->labeltype(FL_NORMAL_LABEL);
	sigbar_SMETER->labelfont(0);
	sigbar_SMETER->labelsize(12);
	sigbar_SMETER->labelcolor(FL_FOREGROUND_COLOR);
	sigbar_SMETER->align(Fl_Align(FL_ALIGN_CENTER));
	sigbar_SMETER->when(FL_WHEN_CHANGED);
	sigbar_SMETER->hide();
	sigbar_SMETER->minimum(0);
	sigbar_SMETER->maximum(100);
	sigbar_SMETER->color(fl_rgb_color (progStatus.smeterRed, progStatus.smeterGreen, progStatus.smeterBlue), bgclr);
	sigbar_SMETER->PeakColor(fl_rgb_color(progStatus.peakRed, progStatus.peakGreen, progStatus.peakBlue));
	sigbar_SMETER->show();

	sigbar_PWR = new Fl_SigBar(5, sigbar_SMETER->y() + sigbar_SMETER->h() + 2, 200, 6);
	sigbar_PWR->box(FL_FLAT_BOX);
	sigbar_PWR->color(FL_BACKGROUND_COLOR);
	sigbar_PWR->selection_color(FL_BACKGROUND_COLOR);
	sigbar_PWR->labeltype(FL_NORMAL_LABEL);
	sigbar_PWR->labelfont(0);
	sigbar_PWR->labelsize(12);
	sigbar_PWR->labelcolor(FL_FOREGROUND_COLOR);
	sigbar_PWR->align(Fl_Align(FL_ALIGN_CENTER));
	sigbar_PWR->when(FL_WHEN_CHANGED);
	sigbar_PWR->hide();
	sigbar_PWR->minimum(0);
	sigbar_PWR->maximum(100);
	sigbar_PWR->color(fl_rgb_color (progStatus.pwrRed, progStatus.pwrGreen, progStatus.pwrBlue), bgclr);
	sigbar_PWR->PeakColor(fl_rgb_color(progStatus.peakRed, progStatus.peakGreen, progStatus.peakBlue));
	sigbar_PWR->show();

	mtr_PWR = new Fl_Box(2, sigbar_PWR->y() + sigbar_PWR->h() + 2, 206, 20);
	mtr_PWR->box(FL_FLAT_BOX);
	mtr_PWR->image(image_p100);
	mtr_PWR->color(bgclr);
	mtr_PWR->labelcolor(fgclr);

	mtr_SWR = new Fl_Box(2, mtr_PWR->y() + mtr_PWR->h() + 2, 206, 20);
	mtr_SWR->box(FL_FLAT_BOX);
	mtr_SWR->image(image_swr);
	mtr_SWR->color(bgclr);
	mtr_SWR->labelcolor(fgclr);

	sigbar_SWR = new Fl_SigBar(5, mtr_SWR->y() + mtr_SWR->h() + 2, 200, 6);
	sigbar_SWR->box(FL_FLAT_BOX);
	sigbar_SWR->color(FL_BACKGROUND_COLOR);
	sigbar_SWR->selection_color(FL_BACKGROUND_COLOR);
	sigbar_SWR->labeltype(FL_NORMAL_LABEL);
	sigbar_SWR->labelfont(0);
	sigbar_SWR->labelsize(12);
	sigbar_SWR->labelcolor(FL_FOREGROUND_COLOR);
	sigbar_SWR->align(Fl_Align(FL_ALIGN_CENTER));
	sigbar_SWR->when(FL_WHEN_CHANGED);
	sigbar_SWR->hide();
	sigbar_SWR->minimum(0);
	sigbar_SWR->maximum(100);
	sigbar_SWR->color(fl_rgb_color (progStatus.swrRed, progStatus.swrGreen, progStatus.swrBlue), bgclr);
	sigbar_SWR->PeakColor(fl_rgb_color(progStatus.peakRed, progStatus.peakGreen, progStatus.peakBlue));
	sigbar_SWR->show();

	mtr_ALC = new Fl_Box(2, sigbar_SWR->y() + sigbar_SWR->h() + 2, 206, 20);
	mtr_ALC->box(FL_FLAT_BOX);
	mtr_ALC->image(image_alc);
	mtr_ALC->color(bgclr);
	mtr_ALC->labelcolor(fgclr);

	sigbar_ALC = new Fl_SigBar(5, mtr_ALC->y() + mtr_ALC->h() + 2, 200, 6);
	sigbar_ALC->box(FL_FLAT_BOX);
	sigbar_ALC->color(FL_BACKGROUND_COLOR);
	sigbar_ALC->selection_color(FL_BACKGROUND_COLOR);
	sigbar_ALC->labeltype(FL_NORMAL_LABEL);
	sigbar_ALC->labelfont(0);
	sigbar_ALC->labelsize(12);
	sigbar_ALC->labelcolor(FL_FOREGROUND_COLOR);
	sigbar_ALC->align(Fl_Align(FL_ALIGN_CENTER));
	sigbar_ALC->when(FL_WHEN_CHANGED);
	sigbar_ALC->hide();
	sigbar_ALC->minimum(0);
	sigbar_ALC->maximum(100);
	sigbar_ALC->color(fl_rgb_color (progStatus.swrRed, progStatus.swrGreen, progStatus.swrBlue), bgclr);
	sigbar_ALC->PeakColor(fl_rgb_color(progStatus.peakRed, progStatus.peakGreen, progStatus.peakBlue));
	sigbar_ALC->show();

	sigbar_VOLTS = new Fl_SigBar(5, sigbar_ALC->y() + sigbar_ALC->h() + 2, 200, 6);
	sigbar_VOLTS->box(FL_FLAT_BOX);
	sigbar_VOLTS->color(FL_BACKGROUND_COLOR);
	sigbar_VOLTS->selection_color(FL_BACKGROUND_COLOR);
	sigbar_VOLTS->labeltype(FL_NORMAL_LABEL);
	sigbar_VOLTS->labelfont(0);
	sigbar_VOLTS->labelsize(12);
	sigbar_VOLTS->labelcolor(FL_FOREGROUND_COLOR);
	sigbar_VOLTS->align(Fl_Align(FL_ALIGN_CENTER));
	sigbar_VOLTS->when(FL_WHEN_CHANGED);
	sigbar_VOLTS->hide();
	sigbar_VOLTS->minimum(6);
	sigbar_VOLTS->maximum(16);
	sigbar_VOLTS->color(fl_rgb_color (progStatus.voltRed, progStatus.voltGreen, progStatus.voltBlue), bgclr);
	sigbar_VOLTS->PeakColor(bgclr);
	sigbar_VOLTS->show();

	mtr_VOLTS = new Fl_Box(2, sigbar_VOLTS->y() + sigbar_VOLTS->h() + 2, 206, 20);
	mtr_VOLTS->box(FL_FLAT_BOX);
	mtr_VOLTS->image(image_voltmeter);
	mtr_VOLTS->color(bgclr);
	mtr_VOLTS->labelcolor(fgclr);

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

	w->end();

	return w;
}

