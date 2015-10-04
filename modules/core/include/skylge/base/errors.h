/*
   Author:  Gerard Visser
   e-mail:  visser.gerard(at)gmail.com

   Copyright (C) 2010 - 2015 Gerard Visser.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef SKYLGE__BASE__ERRORS_INCLUDED
# define SKYLGE__BASE__ERRORS_INCLUDED

# ifdef DEBUG_MODE
#  define ___BTPUSH errors_push (__FILE__, __func__)
#  define ___BTPOP  errors_pop ()

#  include <string.h>

#  define ___CBTPUSH \
  char ___classFunc[strlen (___className) + strlen (__func__) + 3]; \
  stpcpy (stpcpy (stpcpy (___classFunc, ___className), "::"), __func__); \
  errors_push (__FILE__, ___classFunc);

#  define ___CBTPOP  ___BTPOP

void errors_pop (void);
void errors_printBacktraceFatalSignals (void);
void errors_printMessageAndExit (const char* message, ...);
void errors_push (const char* fileName, const char* functionName);

# else
#  define ___BTPUSH
#  define ___BTPOP
#  define ___CBTPUSH
#  define ___CBTPOP
#  define errors_printBacktraceFatalSignals()
# endif

#endif
