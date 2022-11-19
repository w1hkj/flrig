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

#ifndef COMPAT_H
#define COMPAT_H

#include "config.h"


/* adapted from git-compat-util.h in git-1.6.1.2 */


#ifdef __WIN32__
#  include <fcntl.h>
#  include <stdio.h>
#  include <unistd.h>
#  include <sys/stat.h>
#  include <sys/types.h>
#  undef _WINSOCKAPI_		// Required only if windows.h has already been included - it shouldn't have been.
#  include "compat-mingw.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Prior to the UCRT in Visual Studio 2015 and Windows 10, snprintf was not C99
// standard conformant; hence this work-around.
#if defined(__WIN32__) && (!defined(__GNUC__) || __GNUC__ < 4)
#  define SNPRINTF_RETURNS_BOGUS 1
#else
#  define SNPRINTF_RETURNS_BOGUS 0
#endif

#if SNPRINTF_RETURNS_BOGUS
#define snprintf git_snprintf
extern int git_snprintf(char *str, size_t maxsize,
			const char *format, ...);
#define vsnprintf git_vsnprintf
extern int git_vsnprintf(char *str, size_t maxsize,
			 const char *format, va_list ap);
#endif

#ifdef __cplusplus
}
#endif

#endif // COMPAT_H
