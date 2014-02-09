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

Fl_Button *btnAddFreq=(Fl_Button *)0;
Fl_Button *btnPickFreq=(Fl_Button *)0;
Fl_Button *btnDelFreq=(Fl_Button *)0;
Fl_Button *btnClearList=(Fl_Button *)0;
Fl_Browser *FreqSelect=(Fl_Browser *)0;
Fl_Input *inAlphaTag=(Fl_Input *)0;

static void cb_btnAddFreq(Fl_Button*, void*) {
	addFreq();
}

static void cb_btnPickFreq(Fl_Button*, void*) {
	if (FreqSelect->value())
		selectFreq();
}

static void cb_btnDelFreq(Fl_Button*, void*) {
	delFreq();
}

static void cb_btnClearList(Fl_Button*, void*) {
	clearList();
}

static void cb_Close(Fl_Button*, void*) {
	cbCloseMemory();
}

static void cb_FreqSelect(Fl_Browser* o, void*) {
	select_and_close();
	switch (Fl::event_button()) {
		case FL_LEFT_MOUSE:
			if (Fl::event_clicks()) { // double click
				if (o->value())
					selectFreq();
				o->parent()->hide();
			}
			break;
		case FL_RIGHT_MOUSE:
			if (o->value()) selectFreq();
			break;
		default:
			break;
	}
}

static void cb_inAlphaTag(Fl_Input*, void*) {
	editAlphaTag();
}

Fl_Double_Window* Memory_Dialog() {
	Fl_Double_Window* w = new Fl_Double_Window(505, 175, _("Flrig Memory"));

	Fl_Group* mm_grp1 = new Fl_Group(2, 2, 60, 133);
		mm_grp1->box(FL_ENGRAVED_FRAME);

		btnAddFreq = new Fl_Button(6, 6, 51, 22, _("Add @-1>"));
		btnAddFreq->tooltip(_("Add to list"));
		btnAddFreq->down_box(FL_DOWN_BOX);
		btnAddFreq->labelsize(12);
		btnAddFreq->callback((Fl_Callback*)cb_btnAddFreq);
		btnAddFreq->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));

		btnPickFreq = new Fl_Button(6, 31, 51, 22, _("Pick @-1<"));
		btnPickFreq->tooltip(_("Use selected data"));
		btnPickFreq->down_box(FL_DOWN_BOX);
		btnPickFreq->labelsize(12);
		btnPickFreq->callback((Fl_Callback*)cb_btnPickFreq);
		btnPickFreq->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));

		btnDelFreq = new Fl_Button(6, 57, 51, 22, _("Del @-11+"));
		btnDelFreq->tooltip(_("Delete from list"));
		btnDelFreq->down_box(FL_DOWN_BOX);
		btnDelFreq->labelsize(12);
		btnDelFreq->callback((Fl_Callback*)cb_btnDelFreq);
		btnDelFreq->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));

		btnClearList = new Fl_Button(6, 83, 51, 22, _("Clr @-2square"));
		btnClearList->tooltip(_("Clear list"));
		btnClearList->down_box(FL_DOWN_BOX);
		btnClearList->labelsize(12);
		btnClearList->callback((Fl_Callback*)cb_btnClearList);
		btnClearList->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));

		Fl_Button* close = new Fl_Button(6, 109, 51, 22, _("Close"));
		close->tooltip(_("Close Memory dialog"));
		close->labelsize(12);
		close->callback((Fl_Callback*)cb_Close);
		close->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));

	mm_grp1->end();

	FreqSelect = new Fl_Browser(66, 2, 434, 133);
	FreqSelect->tooltip(
	_("Right click => pick freq\nDouble-click ==> pick and close\nLeft click => edit Alpha Tag"));
	FreqSelect->type(2);
	FreqSelect->labelfont(4);
	FreqSelect->labelsize(12);
	FreqSelect->textfont(4);
	FreqSelect->textsize(12);
	FreqSelect->callback((Fl_Callback*)cb_FreqSelect);
	FreqSelect->has_scrollbar(Fl_Browser_::VERTICAL_ALWAYS);
	FreqSelect->column_widths(freq_sel_widths);

	inAlphaTag = new Fl_Input(290, 140, 190, 25, _("Alpha Tag:"));
	inAlphaTag->tooltip(
_("Select Tag from browser window\nLeft click => in Tag field to edit\n[Enter] => when done to update"));
	inAlphaTag->callback((Fl_Callback*)cb_inAlphaTag);
	inAlphaTag->when(FL_WHEN_ENTER_KEY_ALWAYS);

	w->end();

	return w;
}

