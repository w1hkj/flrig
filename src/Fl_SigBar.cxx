// "$Id: Fl_SigBar.H
//
// Signal Bar widget definitions.
//
// Copyright 2011 Dave Freese, W1HKJ
//
// This source code is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This source code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to: w1hkj@w1hkj.com
//
// Contents:
//
//   Fl_SigBar::draw()		- Draw the check button.
//   Fl_SigBar::Fl_SigBar() - Construct a Fl_SigBar widget.
//

//
// Include necessary header files...
//

#include <stdio.h>
#include <math.h>

#include <FL/Fl.H>
#include <FL/fl_draw.H>

#include "Fl_SigBar.h"

//
// Fl_SigBar is a SigBar bar widget based off Fl_Widget that shows a
// standard Signal Bar with a peak reading indicator ...
//

//
// 'Fl_SigBar::draw()' - Draw the check button.
//

void Fl_SigBar::draw()
{
// Get the box borders...
	int bx = Fl::box_dx(box());
	int by = Fl::box_dy(box());
	int bw = Fl::box_dw(box());
	int bh = Fl::box_dh(box());
// Size of SigBar bar...
	int SigBar; 
	int PeakPos;


// Draw the SigBar bar...
// Draw the box and label...
	if (horiz == true) {
		int tx, tw;	 // Temporary X + width
		tx = x() + bx;
		tw = w() - bw;
		SigBar = (int)(tw * (value_ - minimum_) / (maximum_ - minimum_) + 0.5f);
		PeakPos = (int)(tw * (peakv_ - minimum_) / (maximum_ - minimum_) + 0.5f);
		if (SigBar > 0 ) { //|| PeakPos > 0) {

			fl_clip(x(), y(), SigBar + bx, h());
			draw_box(box(), x(), y(), w(), h(), active_r() ? color() : fl_inactive(color()));
			fl_pop_clip();

			fl_clip(tx + SigBar, y(), w() - SigBar, h());
			draw_box(box(), x(), y(), w(), h(), active_r() ? color2() : fl_inactive(color2()));
			fl_pop_clip();

			fl_clip(tx + PeakPos, y(), 2, h());
			draw_box(box(), x(), y(), w(), h(), pkcolor);
			fl_pop_clip();

		} else
			draw_box(box(), x(), y(), w(), h(), color2());
	} else {
		int ty, th;	 // Temporary Y + height
		ty = y() + by;
		th = h() - bh;
		SigBar = (int)(th * (value_ - minimum_) / (maximum_ - minimum_) + 0.5f);
		PeakPos = (int)(th * (peakv_ - minimum_) / (maximum_ - minimum_) + 0.5f);
		if (SigBar > 0 ) { //|| PeakPos > 0) {

			fl_clip(x(), y(), w(), SigBar + by);
			draw_box(box(), x(), y(), w(), h(), FL_BLACK);
			fl_pop_clip();
			

			fl_clip(x(), ty + SigBar, w(), h() - SigBar);
			draw_box(box(), x(), y(), w(), h(), color());
			fl_pop_clip();
			
			fl_clip(x(), ty + PeakPos, w(), 2);
			draw_box(box(), x(), y(), w(), h(), pkcolor);
			fl_pop_clip();

		} else
			draw_box(box(), x(), y(), w(), h(), color2());
	}
}


//
// 'Fl_SigBar::Fl_SigBar()' - Construct a Fl_SigBar widget.
//

Fl_SigBar::Fl_SigBar(int X, int Y, int W, int H, const char* l)
: Fl_Widget(X, Y, W, H, l)
{
	align(FL_ALIGN_INSIDE);
	aging_ = 4;
	peakv_ = 0.0f;
	value_ = 0.0f;
	horiz = true;
	pkcolor = FL_RED;
	for (int i = 0; i < 11; i++) peak_[i] = 0.0f;
}

void Fl_SigBar::peak( float v)
{
	peakv_ = v;
	if (aging_ > 1) {
		for (int i = 1; i <= aging_; i++)
			if (peakv_ < (peak_[i-1] = peak_[i])) 
				peakv_ = peak_[i-1];
		peak_[aging_] = v;
		if (peakv_ < v) peakv_ = v;
	}
}

//
// End of "$Id: Fl_SigBar.cxx 4288 2005-04-16 00:13:17Z mike $".
//
