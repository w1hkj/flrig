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

//======================================================================
// class wheel_slider - based on Fl_Slider
// class wheel_value_slider - based on Fl_Value_Slider
//======================================================================

#include <FL/Fl.H>
#include "ValueSlider.h"

int Fl_Wheel_Slider::handle(int event)
{
	if (event != FL_MOUSEWHEEL || !Fl::event_inside(this))
		return Fl_Slider::handle(event);
	int d;
	if ( !((d = Fl::event_dy()) || (d = Fl::event_dx())) )
		return Fl_Slider::handle(event);
    if (reverse_) d = -d;
	value(clamp(increment(value(), d)));
	do_callback();
	return 1;
}

int Fl_Wheel_Value_Slider::handle(int event)
{
	if (event != FL_MOUSEWHEEL || !Fl::event_inside(this))
		return Fl_Value_Slider::handle(event);
	int d;
	if ( !((d = Fl::event_dy()) || (d = Fl::event_dx())) )
		return Fl_Value_Slider::handle(event);
    if (reverse_) d = -d;
	value(clamp(increment(value(), d)));
	do_callback();
	return 1;
}
