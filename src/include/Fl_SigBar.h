//
// "$Id: Fl_SigBar.H 4288 2005-04-16 00:13:17Z mike $"
//
// SigBar bar widget definitions.
//
// Copyright 2000-2005 by Michael Sweet.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems on the following page:
//
//	 http://www.fltk.org/str.php
//

#ifndef _Fl_SigBar_H_
#  define _Fl_SigBar_H_

//
// Include necessary headers.
//

#include <FL/Fl_Widget.H>


//
// SigBar class...
//

class FL_EXPORT Fl_SigBar : public Fl_Widget
{
	float	value_,
		peakv_,
		minimum_,
		maximum_,
		peak_[11];
	int aging_;
	bool horiz;

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
	void horizontal(bool yes) { horiz = yes; };
};

#endif // !_Fl_SigBar_H_

//
// End of "$Id: Fl_SigBar.H 4288 2005-04-16 00:13:17Z mike $".
//
