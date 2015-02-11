#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "hspinner.h"

Hspinner::Hspinner(int X, int Y, int W, int H, const char *L, int W2)
: Fl_Group(X, Y, W, H, L),
	input_(X + (W2 ? 2*W2 : 6*H/5), Y, (W2 ? W - 2*W2 : W - 6*H/5), H),
	down_button_(X, Y, (W2 ? W2 : 6*H/10), H),
	up_button_(X + (W2 ? W2 : 6*H/10), Y, (W2 ? W2 : 6*H/10), H)
{
	end();

	value_		= 1.0;
	minimum_	= 1.0;
	maximum_	= 100.0;
	step_		= 1.0;
	format_		= "%f";

	sX = X;
	sY = Y;
	sW = W;
	sH = H;
	sW2 = (W2 ? W2 : H);

	align(FL_ALIGN_LEFT);

	input_.value("1");
	input_.type(FL_INT_INPUT);
	input_.when(FL_WHEN_ENTER_KEY | FL_WHEN_RELEASE);
	input_.callback((Fl_Callback *)sb_cb, this);

	if (W2) {
		down_button_.labelsize( 3 * W2 / 4);
		up_button_.labelsize( 3 * W2 / 4);
	} else {
		down_button_.labelsize( 4*sH / 10);
		up_button_.labelsize( 4*sH / 10);
	}
	down_button_.label("@2>");
	down_button_.callback((Fl_Callback *)sb_cb, this);
	down_button_.align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);

	up_button_.label("@8>");
	up_button_.callback((Fl_Callback *)sb_cb, this);
	up_button_.align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);

}

/// Resized the widget
void Hspinner::resize(int X, int Y, int W, int H, int W2)
{
	sX = X;
	sY = Y;
	sW = W;
	sH = H;
	sW2 = (W2 ? W2 : 6*H/10);
	Fl_Group::resize(X,Y,W,H);
	input_.resize(sX + 2*sW2, sY, sW - 2*sW2, sH);
	if (W2) {
		down_button_.labelsize(W2 / 2);
		up_button_.labelsize(W2 / 2);
	} else {
		down_button_.labelsize(sH / 2);
		up_button_.labelsize(sH / 2);
	}
	down_button_.resize(sX, sY, sW2, sH);
	up_button_.resize(sX + sW2, sY, sW2, sH);
}

/// Sets or returns the amount to change the value when the user clicks a button.
/// Before setting step to a non-integer value, the spinner
/// type() should be changed to floating point.
void Hspinner::step(double s)
{
	step_ = s;
	if (step_ != (int)step_) {
		input_.type(FL_FLOAT_INPUT);
		if (step_ > 1.0) format ("%.0f");
		else if (step_ >= 0.1) format("%.1f");
		else if (step_ >= 0.01) format("%.01f");
		else if (step_ >= 0.001) format("%.001f");
		else if (step_ >= 0.0001) format("%.0001f");
		else format("%f");
	}
	else { 
		input_.type(FL_INT_INPUT);
		format("%.0f");
	}
	update();
}

/// Sets the numeric representation in the input field.
/// Valid values are FL_INT_INPUT and FL_FLOAT_INPUT.
/// Also changes the format() template.
/// Setting a new spinner type via a superclass pointer will not work.
/// note:  type is not a virtual function.
void Hspinner::type(unsigned char v)
{
	if (v==FL_FLOAT_INPUT) {
		if (step_ > 1.0) format ("%.0f");
		else if (step_ >= 0.1) format("%.1f");
		else if (step_ >= 0.01) format("%.01f");
		else if (step_ >= 0.001) format("%.001f");
		else if (step_ >= 0.0001) format("%.0001f");
		else format("%f");
	} else {
		format("%.0f");
	}
	input_.type(v);
}

int Hspinner::handle(int event)
{
	switch (event) {
		case FL_KEYDOWN :
		case FL_SHORTCUT :
			if (Fl::event_key() == FL_Up) {
				up_button_.do_callback();
				return 1;
			} else if (Fl::event_key() == FL_Down) {
				down_button_.do_callback();
				return 1;
			} else return 0;
		case FL_FOCUS :
			if (input_.take_focus()) return 1;
			else return 0;
	}
	return Fl_Group::handle(event);
}

/// Get/Set label font size
void Hspinner::labelsize(Fl_Fontsize sz)
{
	Fl_Group::labelsize(sz);
}

Fl_Fontsize Hspinner::labelsize() 
{ 
	return Fl_Group::labelsize();
}

void Hspinner::sb_cb(Fl_Widget *w, Hspinner *sb)
{
	double v;		// New value

	if (w == &(sb->input_)) {
// Something changed in the input field...
		v = atof(sb->input_.value());
		if (v < sb->minimum_) {
			sb->value_ = sb->minimum_;
			sb->update();
		} else if (v > sb->maximum_) {
			sb->value_ = sb->maximum_;
			sb->update();
		} else sb->value_ = v;
	} else if (w == &(sb->up_button_)) {
// Up button pressed...
		v = sb->value_ + sb->step_;
		if (v > sb->maximum_) sb->value_ = sb->maximum_;
		else sb->value_ = v;
		sb->update();
	} else if (w == &(sb->down_button_)) {
// Down button pressed...
		v = sb->value_ - sb->step_;
		if (v < sb->minimum_) sb->value_ = sb->minimum_;
		else sb->value_ = v;
		sb->update();
	}
	sb->do_callback();
}

void Hspinner::update() {
	char s[255]; // Value string

	if (format_[0]=='%'&&format_[1]=='.'&&format_[2]=='*') {
		int c = 0;
		char temp[64], *sp = temp;
		sprintf(temp, "%.12f", step_);
		while (*sp) sp++;
		sp--;
		while (sp>temp && *sp=='0') sp--;
		while (sp>temp && (*sp>='0' && *sp<='9')) { sp--; c++; }
		sprintf(s, format_, c, value_);
	} else {
		sprintf(s, format_, value_);
	}
	input_.value(s);
}

double Hspinner::value()
{
	return (value_);
}

void Hspinner::value(double v)
{
	value_ = v;
	update();
}


