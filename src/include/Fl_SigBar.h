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

#ifndef _Fl_SigBar_H_
#define _Fl_SigBar_H_

#include <FL/Fl_Widget.H>

//
// SigBar class...
//

class Fl_SigBar : public Fl_Widget
{
	float value_, peakv_, minimum_, maximum_,
		peak_[10], vals_[10];
	int aging_, avg_;
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

	void	value(float v);
	float	value() const { return (value_); }
  
	void aging (int n) { 
		if (n <= 10 && n > 0) aging_ = n;
		else aging_ = 5;
		for (int i = 0; i < aging_; i++) peak_[i] = peakv_;
	}

	void avg (int n) {
		if (n <= 10 && n > 0) avg_ = n;
		else avg_ = 5;
		for (int i = 0; i < avg_; i++) vals_[i] = value_ / avg_;
	}

	void clear () {
		for (int i = 0; i < 10; i++) vals_[i] = peak_[i] = 0;
		peakv_ = value_ = 0;
	}

	void peak(float);
	float peak() { return peakv_;};

	void PeakColor(Fl_Color c) { pkcolor = c; };
	Fl_Color PeakColor() { return pkcolor; }
	void horizontal(bool yes) { horiz = yes; };
};

#endif
