// ---------------------------------------------------------------------
// morse.cxx  --  morse code modem
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

#include <config.h>
#include <cstring>
#include <iostream>

#include "morse.h"

/*
 * Morse code characters table.  This table allows lookup of the Morse
 * shape of a given alphanumeric character.  Shapes are held as a string,
 * with "-' representing dash, and ".' representing dot.  The table ends
 * with a NULL entry.
 *
 * This is the main table from which the other tables are computed.
 *
 * The Prosigns are also defined in the configuration.h file
 * The user can specify the character which substitutes for the prosign
 */

CWstruct Cmorse::cw_table[] = {
// Prosigns
	{ "=", "-...-" },	// <BT>
	{ "~", ".-.-" },	// <AA>
	{ "<", ".-..." },	// <AS>
	{ ">", ".-.-." },	// <AR>
	{ "%", "...-.-" },	// <SK>
	{ "+", "-.--." },	// <KN>
	{ "&", "..-.-" },	// <INT>
	{ "{", "....--" },	// <HM>
	{ "}", "...-." },	// <VE>

// upper case  letters
	{ "A", ".-" },
	{ "B", "-..." },
	{ "C", "-.-." },
	{ "D", "-.." },
	{ "E", "."	 },
	{ "F", "..-." },
	{ "G", "--." },
	{ "H", "...." },
	{ "I", ".." },
	{ "J", ".---" },
	{ "K", "-.-" },
	{ "L", ".-.." },
	{ "M", "--" },
	{ "N", "-." },
	{ "O", "---" },
	{ "P", ".--." },
	{ "Q", "--.-" },
	{ "R", ".-." },
	{ "S", "..." },
	{ "T", "-"	 },
	{ "U", "..-" },
	{ "V", "...-" },
	{ "W", ".--" },
	{ "X", "-..-" },
	{ "Y", "-.--" },
	{ "Z", "--.." },

// lower case 7 bit letters
	{ "a", ".-" },
	{ "b", "-..." },
	{ "c", "-.-." },
	{ "d", "-.." },
	{ "e", "."	 },
	{ "f", "..-." },
	{ "g", "--." },
	{ "h", "...." },
	{ "i", ".." },
	{ "j", ".---" },
	{ "k", "-.-" },
	{ "l", ".-.." },
	{ "m", "--" },
	{ "n", "-." },
	{ "o", "---" },
	{ "p", ".--." },
	{ "q", "--.-" },
	{ "r", ".-." },
	{ "s", "..." },
	{ "t", "-"	 },
	{ "u", "..-" },
	{ "v", "...-" },
	{ "w", ".--" },
	{ "x", "-..-" },
	{ "y", "-.--" },
	{ "z", "--.." },

// numerals
	{ "0", "-----" },
	{ "1", ".----" },
	{ "2", "..---" },
	{ "3", "...--" },
	{ "4", "....-" },
	{ "5", "....." },
	{ "6", "-...." },
	{ "7", "--..." },
	{ "8", "---.." },
	{ "9", "----." },

// punctuation
	{ "\\", ".-..-." },
	{ "\'", ".----." },
	{ "$", "...-..-" },
	{ "(", "-.--."	 },
	{ ")", "-.--.-" },
	{ ",", "--..--" },
	{ "-", "-....-" },
	{ ".", ".-.-.-" },
	{ "/", "-..-."	 },
	{ ":", "---..." },
	{ ";", "-.-.-." },
	{ "?", "..--.." },
	{ "_", "..--.-" },
	{ "@", ".--.-." },
	{ "!", "-.-.--" },

// accented characters
	{ "Ä", ".-.-" },	// A umlaut
	{ "ä", ".-.-" },	// A umlaut
	{ "Æ", ".-.-" },	// A aelig
	{ "æ", ".-.-" },	// A aelig
	{ "Å", ".--.-" },	// A ring
	{ "å", ".--.-" },	// A ring
	{ "Ç", "-.-.." },	// C cedilla
	{ "ç", "-.-.." },	// C cedilla
	{ "È", ".-..-" },	// E grave
	{ "è", ".-..-" },	// E grave
	{ "É", "..-.." },	// E acute
	{ "é", "..-.." },	// E acute
	{ "Ó", "---." },	// O acute
	{ "ó", "---." },	// O acute
	{ "Ö", "---." },	// O umlaut
	{ "ö", "---." },	// O umlaut
	{ "Ø", "---." },	// O slash
	{ "ø", "---." },	// O slash
	{ "Ñ", "--.--" },	// N tilde
	{ "ñ", "--.--" },	// N tilde
	{ "Ü", "..--" },	// U umlaut
	{ "ü", "..--" },	// U umlaut
	{ "Û", "..--" },	// U circ
	{ "û", "..--" },	// U circ

// array termination
	{ "", ""}
};

//----------------------------------------------------------------------

std::string Cmorse::tx_lookup(int c)
{
	c &= 0xFF;
	utf8 += c;

	if (((utf8[0] & 0xFF) > 0x7F) && (utf8.length() == 1)) {
		return "";
	}

	for (int i = 0; cw_table[i].rpr.length(); i++) {
		if (utf8 == cw_table[i].chr) {
			utf8.clear();
			return cw_table[i].rpr;
		}
	}
	utf8.clear();
	return "";

}

int Cmorse::tx_length(int c)
{
	if (c == ' ') return 4;
	std::string ms = tx_lookup(c);
	if (ms.empty()) return 0;
	int len = 0;
	for (size_t i = 0; i < ms.length(); i++)
		if (ms[i] == '.') len += 2;
		else              len += 4;
	len += 2;
	return len;
}

//----------------------------------------------------------------------

