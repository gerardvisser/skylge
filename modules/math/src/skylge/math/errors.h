/*
   Author:  Gerard Visser
   e-mail:  visser.gerard(at)gmail.com

   Copyright (C) 2018, 2019 Gerard Visser.

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

#ifndef SKYLGE__MATH__ERRORS_INCLUDED
#define SKYLGE__MATH__ERRORS_INCLUDED

#ifdef DEBUG_MODE

# include <stdio.h>
# include <stdlib.h>
# include "defs.h"

# define LOC_AFTER  "after"
# define LOC_BEFORE "before"

# define PRINT_MESSAGE_AND_EXIT(...) \
  printf (__VA_ARGS__); \
  exit (EXIT_FAILURE)

# define VALIDATE_INTEGER(funcName, integerObj, beforeOrAfter) \
  { \
    const uint64_t* dbg_buf = (integerObj).buf (); \
    if ((integerObj).max () > (integerObj).size ()) { \
      PRINT_MESSAGE_AND_EXIT ("[%s][%s][tested=%s] integer max greater than size (max: %d, size: %d)\n", \
                              beforeOrAfter, funcName, #integerObj, (integerObj).max (), (integerObj).size ()); \
    } \
    int dbg_i = (integerObj).size () - 1; \
    while (dbg_i > -1 && dbg_buf[dbg_i] == 0) { \
      --dbg_i; \
    } \
    if ((integerObj).max () != dbg_i + 1) { \
      PRINT_MESSAGE_AND_EXIT ("[%s][%s][tested=%s] integer max corrupt (was: %d, should've been: %d)\n", \
                              beforeOrAfter, funcName, #integerObj, (integerObj).max (), dbg_i + 1); \
    } \
    while (dbg_i > -1 && (dbg_buf[dbg_i] & CAL_LMASK[0]) == dbg_buf[dbg_i]) { \
      --dbg_i; \
    } \
    if (dbg_i > -1) { \
      PRINT_MESSAGE_AND_EXIT ("[%s][%s][tested=%s] one or more integer cells corrupt\n", \
                              beforeOrAfter, funcName, #integerObj); \
    } \
  }

# define VALIDATE_INTEGER_LAST_BIT_0(funcName, integerObj, beforeOrAfter) \
  VALIDATE_INTEGER (funcName, integerObj, beforeOrAfter) \
  { \
    int dbg_lastBit = CAL_B * (integerObj).size () - 1; \
    if ((integerObj).getBit (dbg_lastBit)) { \
      PRINT_MESSAGE_AND_EXIT ("[%s][%s][tested=%s] integer's most significant bit not zero\n", \
                              beforeOrAfter, funcName, #integerObj); \
    } \
  }

# define VALIDATE_REAL(funcName, realObj, beforeOrAfter) \
  { \
    const Integer& dbg_numb = (realObj).number (); \
    const Integer& dbg_expo = (realObj).exponent (); \
    if (dbg_numb.sizeInBits () != dbg_expo.sizeInBits ()) { \
      PRINT_MESSAGE_AND_EXIT ("[%s][%s][tested=%s] m_exponent not of same size as m_number.\n", \
                              beforeOrAfter, funcName, #realObj); \
    } \
    if (dbg_numb.isZero ()) { \
      if (!(realObj).isInfinite () && (realObj).sign ()) { \
        PRINT_MESSAGE_AND_EXIT ("[%s][%s][tested=%s] sign set of a real number that represents zero.\n", \
                                beforeOrAfter, funcName, #realObj); \
      } \
    } else { \
      if (!dbg_numb.getBit (0)) { \
        PRINT_MESSAGE_AND_EXIT ("[%s][%s][tested=%s] least significant bit not set even though the number does not represent zero or infinity.\n", \
                                beforeOrAfter, funcName, #realObj); \
      } \
    } \
  }


#else

# define VALIDATE_INTEGER(funcName, integerObj, beforeOrAfter)
# define VALIDATE_INTEGER_LAST_BIT_0(funcName, integerObj, beforeOrAfter)
# define VALIDATE_REAL(funcName, realObj, beforeOrAfter)

#endif


#endif
