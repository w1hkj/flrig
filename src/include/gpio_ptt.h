// ----------------------------------------------------------------------------
// Copyright (C) 2020
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

#ifndef _GPIO_PTT_H
#define _GPIO_PTT_H

#include <string>
#include "status.h"

extern void gpioEXEC(std::string execstr);
extern void export_gpio(int bcm);
extern void unexport_gpio(int bcm);
extern void open_gpio(void);
extern void close_gpio(void);
extern void set_gpio(bool ptt);
extern int  get_gpio();

#endif
