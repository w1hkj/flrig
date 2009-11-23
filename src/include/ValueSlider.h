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
