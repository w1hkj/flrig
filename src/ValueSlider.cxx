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
