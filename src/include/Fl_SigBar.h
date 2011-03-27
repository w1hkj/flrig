//
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

#ifndef _Fl_SigBar_H_
#define _Fl_SigBar_H_

#include <FL/Fl_Widget.H>

//
// SigBar class...
//

class Fl_SigBar : public Fl_Widget
{
	float	value_,
		peakv_,
		minimum_,
		maximum_,
		peak_[11];
	int aging_;
	bool horiz;
	Fl_Color pkcolor;

protected:

	virtual void draw();

public:

	Fl_SigBar(int x, int y, int w, int h, const char *l = 0);

	void	maximum(float v) { maximum_ = v; redraw(); }
	float	maximum() const { return (maximum_); }

	void	minimum(float v) { minimum_ = v; redraw(); }
	float	minimum() const { return (minimum_); }

	void	value(float v) { value_ = v; peak(v); redraw(); Fl::flush(); };
	float	value() const { return (value_); }
  
	void aging (int n) { 
		if (n <= 10 && n > 0) aging_ = n - 1;
		else aging_ = 10;
	}
	void peak(float);
	float peak() { return peakv_;};
	void PeakColor(Fl_Color c) { pkcolor = c; };
	Fl_Color PeakColor() { return pkcolor; }
	void horizontal(bool yes) { horiz = yes; };
};

#endif
