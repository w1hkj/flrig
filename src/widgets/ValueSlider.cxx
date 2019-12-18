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

#include <iostream>

#include <FL/Fl.H>
#include "ValueSlider.h"

/* events

 0  FL_NO_EVENT - No event (or an event fltk does not understand) occurred (0).
 1  FL_PUSH - A mouse button was pushed.
 2  FL_RELEASE - A mouse button was released.
 3  FL_ENTER - The mouse pointer entered a widget.
 4  FL_LEAVE - The mouse pointer left a widget.
 5  FL_DRAG - The mouse pointer was moved with a button pressed.
 6  FL_FOCUS - A widget should receive keyboard focus.
 7  FL_UNFOCUS - A widget loses keyboard focus.
 8  FL_KEYBOARD - A key was pressed.
 9  FL_CLOSE - A window was closed.
 10 FL_MOVE - The mouse pointer was moved with no buttons pressed.
 11 FL_SHORTCUT - The user pressed a shortcut key.
 12 FL_DEACTIVATE - The widget has been deactivated.
 13 FL_ACTIVATE - The widget has been activated.
 14 FL_HIDE - The widget has been hidden.
 15 FL_SHOW - The widget has been shown.
 16 FL_PASTE - The widget should paste the contents of the clipboard.
 17 FL_SELECTIONCLEAR - The widget should clear any selections made for the clipboard.
 18 FL_MOUSEWHEEL - The horizontal or vertical mousewheel was turned.
 19 FL_DND_ENTER - The mouse pointer entered a widget dragging data.
 20 FL_DND_DRAG - The mouse pointer was moved dragging data.
 21 FL_DND_LEAVE - The mouse pointer left a widget still dragging data.
 22 FL_DND_RELEASE - Dragged data is about to be dropped.
 23 FL_SCREEN_CONFIGURATION_CHANGED - The screen configuration (number, positions) was changed.
 24 FL_FULLSCREEN - The fullscreen state of the window has changed.
*/
int Fl_Wheel_Slider::handle(int event)
{
	if (event == FL_RELEASE || event == FL_LEAVE) {
		do_callback();
		return 1;
	}
	if (event == FL_MOUSEWHEEL) {
		int d;
		if ( !((d = Fl::event_dy()) || (d = Fl::event_dx())) )
			return Fl_Slider::handle(event);
		if (reverse_) d = -d;
		value(clamp(increment(value(), d)));
		do_callback();
		return 1;
	}
	return Fl_Slider::handle(event);
}

int Fl_Wheel_Value_Slider::handle(int event)
{
	if (event == FL_RELEASE || event == FL_LEAVE) {
		do_callback();
		return 1;
	}
	if (event == FL_MOUSEWHEEL) {
		int d;
		if ( !((d = Fl::event_dy()) || (d = Fl::event_dx())) )
			return Fl_Value_Slider::handle(event);
		if (reverse_) d = -d;
		value(clamp(increment(value(), d)));
		do_callback();
		return 1;
	}
	return Fl_Value_Slider::handle(event);
}
