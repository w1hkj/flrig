#ifndef _HSPINNER_H
#define _HSPINNER_H

//
// Include necessary headers...
//

#include <FL/Enumerations.H>
#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Repeat_Button.H>
#include <FL/Fl_Input_Choice.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

#include <stdio.h>
#include <stdlib.h>


/// This widget is a combination of the input
/// widget and repeat buttons. The user can either type into the
/// input area or use the buttons to change the value.

class Hspinner : public Fl_Group {

  double	value_;			// Current value
  double	minimum_;		// Minimum value
  double	maximum_;		// Maximum value
  double	step_;			// Amount to add/subtract for up/down
  const char	*format_;		// Format string

protected:
	Fl_Input			input_;			// Input field for the value
	Fl_Repeat_Button	down_button_;	// Up button
	Fl_Repeat_Button	up_button_;		// Down button
	int sH;
	int sW;
	int sX;
	int sY;
	int sW2;

private:
	static void sb_cb(Fl_Widget *w, Hspinner *sb);
	void update();

public:
/// Creates a new Fl_Spinner widget using the given position, size,
/// and label string.
/// Inherited destructor Destroys the widget and any value associated with it.

Hspinner(int X, int Y, int W, int H, const char *L = 0, int W2 = 0);

/// Sets or returns the format string for the value.
	const char	*format() { return (format_); }
	void format(const char *f) { format_ = f; update(); }

	int handle(int event);

/// Gets/Sets the maximum value of the widget.
	double	maximum() const { return (maximum_); }
	void	maximum(double m) { maximum_ = m; }

/// Gets/Sets the minimum value of the widget.
	double	mininum() const { return (minimum_); }
	void	minimum(double m) { minimum_ = m; }

/// Sets the minimum and maximum values for the widget.
	void	range(double a, double b) { minimum_ = a; maximum_ = b; }

/// Resized the widget
	void	resize(int X, int Y, int W, int H, int W2 = 0);

/// Sets or returns the amount to change the value when the user clicks a button.
/// Before setting step to a non-integer value, the spinner
/// type() should be changed to floating point.

	double	step() const { return (step_); }
	void	step(double s);

/// Gets the color of the text in the input field.
	Fl_Color	textcolor() const {
		return (input_.textcolor());
	}
/// Sets the color of the text in the input field.
	void		textcolor(Fl_Color c) {
		input_.textcolor(c);
	}

/// Gets the font of the text in the input field.
	Fl_Font		textfont() const {
		return (input_.textfont());
	}
/// Sets the font of the text in the input field.
	void		textfont(Fl_Font f) {
		input_.textfont(f);
	}

/// Gets the size of the text in the input field.
	Fl_Fontsize		textsize() const {
		return (input_.textsize());
	}
/// Sets the size of the text in the input field.
	void		textsize(Fl_Fontsize s) {
		input_.textsize(s);
	}

/// Gets/Sets the numeric representation in the input field.
/// Valid values are FL_INT_INPUT and FL_FLOAT_INPUT.
/// Also changes the format() template.
/// Setting a new spinner type via a superclass pointer will not work.
/// note:  type is not a virtual function.
	unsigned char	type() const { return (input_.type()); }
	void	type(unsigned char v);

/// Gets/Sets the current value of the widget.
/// Before setting value to a non-integer value, the spinner
/// type() should be changed to floating point.
	double		value();
	void		value(double v);

/// Get/Set the background color of the spinner widget's input field.
	void		color(Fl_Color v) { input_.color(v); }
	Fl_Color	color() const { return(input_.color()); }

/// Get/Set label font size
	void		labelsize(Fl_Fontsize sz);
	Fl_Fontsize labelsize();

};

#endif // !_HSPINNER_H

