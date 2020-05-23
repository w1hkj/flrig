// ---------------------------------------------------------------------
// morse.h --  morse code modem
//
// Copyright (C) 2020
//		Dave Freese, W1HKJ
//
// This file is part of flrig
//
// flrig is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// flrig is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with fldigi.  If not, see <http://www.gnu.org/licenses/>.
// ---------------------------------------------------------------------

#ifndef _MORSE_H
#define _MORSE_H

#include <string>

#define	DOT   '.'
#define	DASH  '-'

struct CWstruct {
	std::string		chr;		// utf-8 string representation of character
	std::string		rpr;		// Dot-dash code representation
};

class Cmorse {
private:
	static CWstruct	cw_table[];
	std::string utf8;

public:
	Cmorse() {}
	~Cmorse() {}

	std::string tx_lookup(int);
	int tx_length(int);
};

#endif
