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

#include "font_browser.h"

Fl_Button *btnAddFreq=(Fl_Button *)0;
Fl_Button *btnPickFreq=(Fl_Button *)0;
Fl_Button *btnDelFreq=(Fl_Button *)0;
Fl_Button *btnClearList=(Fl_Button *)0;
Fl_Browser2 *FreqSelect=(Fl_Browser2 *)0;
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
}

static void cb_inAlphaTag(Fl_Input*, void*) {
	editAlphaTag();
}

static Font_Browser  *fntSelectbrowser = 0;
static Fl_Browser *header = 0;
static int freq_sel_widths[] = {140, 15, 70, 15, 80, 15, 0};

extern void updateSelect();

void cbFreqSelectFontBrowser(Fl_Widget*, void*) {
	progStatus.memfontnbr = fntSelectbrowser->fontNumber();
	progStatus.memfontsize = fntSelectbrowser->fontSize();
	if (progStatus.memfontsize > 18) progStatus.memfontsize = 18;
	if (progStatus.memfontsize < 10) progStatus.memfontsize = 10;
	fntSelectbrowser->hide();

	char hdrline[256];
	header->clear();
	snprintf(hdrline, sizeof(hdrline),
			"\
@F%d@S%d@B%d@cFrequency\t\
@F%d@S%d@B%d|\t\
@F%d@S%d@B%d@cBW\t\
@F%d@S%d@B%d|\t\
@F%d@S%d@B%d@cMode\t\
@F%d@S%d@B%d|\t\
@F%d@S%d@B%d@cComments",
		progStatus.memfontnbr, progStatus.memfontsize, 247,
		progStatus.memfontnbr, progStatus.memfontsize, 247,
		progStatus.memfontnbr, progStatus.memfontsize, 247,
		progStatus.memfontnbr, progStatus.memfontsize, 247,
		progStatus.memfontnbr, progStatus.memfontsize, 247,
		progStatus.memfontnbr, progStatus.memfontsize, 247,
		progStatus.memfontnbr, progStatus.memfontsize, 247 );
	header->add(hdrline);
	header->redraw();

	updateSelect();

}

void cbFreqFontBrowser()
{
	fntSelectbrowser = new Font_Browser;
	fntSelectbrowser->fontNumber(progStatus.memfontnbr);
	fntSelectbrowser->fontSize(progStatus.memfontsize);
	fntSelectbrowser->callback(cbFreqSelectFontBrowser);
	fntSelectbrowser->show();
}

Fl_Double_Window* Memory_Dialog() {
	Fl_Double_Window* w = new Fl_Double_Window(600, 182, _("Flrig Memory"));

	Fl_Group* mm_grp1 = new Fl_Group(0, 0, 600, 155);
		mm_grp1->box(FL_FLAT_BOX);

		Fl_Group* mm_grp1a = new Fl_Group(0, 0, 64, 156);
			mm_grp1a->box(FL_FLAT_BOX);

			btnAddFreq = new Fl_Button(2, 1, 60, 22, _("Add @-1>"));
			btnAddFreq->tooltip(_("Add to list"));
			btnAddFreq->down_box(FL_DOWN_BOX);
			btnAddFreq->labelsize(14);
			btnAddFreq->callback((Fl_Callback*)cb_btnAddFreq);
			btnAddFreq->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));

			btnPickFreq = new Fl_Button(2, 25, 60, 22, _("Pick @-1<"));
			btnPickFreq->tooltip(_("Use selected data"));
			btnPickFreq->down_box(FL_DOWN_BOX);
			btnPickFreq->labelsize(14);
			btnPickFreq->callback((Fl_Callback*)cb_btnPickFreq);
			btnPickFreq->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));

			btnDelFreq = new Fl_Button(2, 49, 60, 22, _("Del @-11+"));
			btnDelFreq->tooltip(_("Delete from list"));
			btnDelFreq->down_box(FL_DOWN_BOX);
			btnDelFreq->labelsize(14);
			btnDelFreq->callback((Fl_Callback*)cb_btnDelFreq);
			btnDelFreq->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));

			btnClearList = new Fl_Button(2, 73, 60, 22, _("Clr @-2square"));
			btnClearList->tooltip(_("Clear list"));
			btnClearList->down_box(FL_DOWN_BOX);
			btnClearList->labelsize(14);
			btnClearList->callback((Fl_Callback*)cb_btnClearList);
			btnClearList->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));

			Fl_Button * btnFontSel = new Fl_Button(2, 97, 60, 22, _("Font"));
			btnFontSel->tooltip(_("Select Font/Size"));
			btnFontSel->down_box(FL_DOWN_BOX);
			btnFontSel->labelsize(14);
			btnFontSel->callback((Fl_Callback*)cbFreqFontBrowser);
			btnFontSel->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));

			Fl_Button* close = new Fl_Button(2, 121, 60, 22, _("Close"));
			close->tooltip(_("Close Memory dialog"));
			close->labelsize(14);
			close->callback((Fl_Callback*)cb_Close);
			close->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));

			Fl_Box *bx1a = new Fl_Box(0, 143, 66, 2, "");
			bx1a->box(FL_NO_BOX);

		mm_grp1a->end();
		mm_grp1a->resizable(bx1a);

		Fl_Group *mm_grp1b = new Fl_Group(66, 0, 534, 155);
		mm_grp1b->box(FL_FLAT_BOX);

		header = new Fl_Browser(66, 0, 532, 24, "");
		header->column_widths(freq_sel_widths);
		header->color(247);
		char hdrline[256];
		snprintf(hdrline, sizeof(hdrline),
			"\
@F%d@S%d@B%d@cFrequency\t\
@F%d@S%d@B%d|\t\
@F%d@S%d@B%d@cBW\t\
@F%d@S%d@B%d|\t\
@F%d@S%d@B%d@cMode\t\
@F%d@S%d@B%d|\t\
@F%d@S%d@B%d@cComments",
			progStatus.memfontnbr, progStatus.memfontsize, 247,
			progStatus.memfontnbr, progStatus.memfontsize, 247,
			progStatus.memfontnbr, progStatus.memfontsize, 247,
			progStatus.memfontnbr, progStatus.memfontsize, 247,
			progStatus.memfontnbr, progStatus.memfontsize, 247,
			progStatus.memfontnbr, progStatus.memfontsize, 247,
			progStatus.memfontnbr, progStatus.memfontsize, 247 );
		header->add(hdrline);

		FreqSelect = new Fl_Browser2(66, 24, 532, 131);
		FreqSelect->tooltip(
		_("\
Left Click:     high light\n\
Dbl Left Click: select\n\
Right click:    select"));
		FreqSelect->type(2);
		FreqSelect->labelfont(4);
		FreqSelect->labelsize(14);
		FreqSelect->textfont(4);
		FreqSelect->textsize(14);
		FreqSelect->column_widths(freq_sel_widths);
		FreqSelect->callback((Fl_Callback*)cb_FreqSelect);
//		FreqSelect->has_scrollbar(Fl_Browser_::BOTH_ALWAYS);

		mm_grp1b->end();
		mm_grp1b->resizable(FreqSelect);

	mm_grp1->end();
	mm_grp1->resizable(mm_grp1b);

	Fl_Group* mm_grp2 = new Fl_Group(0, 155, 600, 26);

		inAlphaTag = new Fl_Input(66, 156, 532, 24, _("Tag:"));
		inAlphaTag->tooltip(
_("Left click => in Tag field to edit\n[Enter] => when done to update"));
		inAlphaTag->callback((Fl_Callback*)cb_inAlphaTag);
		inAlphaTag->when(FL_WHEN_ENTER_KEY_ALWAYS);
		inAlphaTag->textfont(4);
		inAlphaTag->textsize(14);

	mm_grp2->end();
	mm_grp2->resizable(inAlphaTag);

	w->end();
	w->resizable(mm_grp1);

	w->size_range(w->w(), w->h(), 0, 0);
	w->resize(
		progStatus.memX, progStatus.memY,
		progStatus.memW, progStatus.memH);
	w->init_sizes();

	return w;
}

