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
// clase wheel_slider - based on Fl_Slider
// class wheel_value_slider - based on Fl_Value_Slider
//======================================================================

#include <FL/Fl_Slider.H>
#include <FL/Fl_Value_Slider.H>

class Fl_Wheel_Slider : public Fl_Slider
{
private:
	bool reverse_;
public:
	Fl_Wheel_Slider(int x, int y, int w, int h, const char *label = 0)
		: Fl_Slider(x, y, w, h, label), reverse_(false) { }
	int handle(int event);
	void reverse(bool rev) { reverse_ = rev;}
	bool reverse() { return reverse_;}
};

class Fl_Wheel_Value_Slider : public Fl_Value_Slider
{
private:
	bool reverse_;
public:
	Fl_Wheel_Value_Slider(int x, int y, int w, int h, const char *label = 0)
		: Fl_Value_Slider(x, y, w, h, label), reverse_(false) { }
	int handle(int event);
	void reverse(bool rev) { reverse_ = rev;}
	bool reverse() { return reverse_;}
};
