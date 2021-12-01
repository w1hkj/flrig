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

#define SIGBAR_ARRAY_SIZE 20

class Fl_SigBar : public Fl_Widget
{
protected:
	double value_, peakv_, minimum_, maximum_;
	double vals_[SIGBAR_ARRAY_SIZE];
	int aging_, avg_;
	bool horiz;
	Fl_Color pkcolor;

protected:

	virtual void draw();

public:

	Fl_SigBar(int x, int y, int w, int h, const char *l = 0);

	void	maximum(double v) { maximum_ = v; redraw(); }
	double	maximum() const { return (maximum_); }

	void	minimum(double v) { minimum_ = v; redraw(); }
	double	minimum() const { return (minimum_); }

	void	value(double v);
	double	value() const { return (value_); }
	double	peak() const { return peakv_;};
  
	void aging (int n) { 
		if (n <= SIGBAR_ARRAY_SIZE && n > 0) aging_ = n;
		else aging_ = SIGBAR_ARRAY_SIZE / 2;
	}

	void avg (int n) {
		if (n <= SIGBAR_ARRAY_SIZE && n > 0) avg_ = n;
		else avg_ = SIGBAR_ARRAY_SIZE / 2;
	}

	void clear () {
		for (int i = 0; i < SIGBAR_ARRAY_SIZE; i++) 
			vals_[i] = 0;
		peakv_ = value_ = 0;
	}

	void PeakColor(Fl_Color c) { pkcolor = c; };
	Fl_Color PeakColor() { return pkcolor; }
	void horizontal(bool yes) { horiz = yes; };
};

#endif
