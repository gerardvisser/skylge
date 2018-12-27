/*
   Author:  Gerard Visser
   e-mail:  visser.gerard(at)gmail.com

   Copyright (C) 2018 Gerard Visser.

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

#ifndef SKYLGE__MATH__DEFS_INCLUDED
#define SKYLGE__MATH__DEFS_INCLUDED

#ifndef CAL_B
# define CAL_B 32
#endif

#include <stdint.h>

#if CAL_B == 6

# define CAL_Q(x) (x / 6)
# define CAL_R(x) (x % 6)

#elif CAL_B == 32

# define CAL_Q(x) (x >> 5)
# define CAL_R(x) (x & 0x1F)

#else
# error CAL_B should be 6 or 32
#endif

#define CAL_CARRY(x)       ((x & CAL_SMASK[CAL_B]) != 0)
#define CAL_CLEAR_CARRY(x) x &= CAL_LMASK[0]

extern const uint64_t CAL_LMASK[CAL_B + 1];
extern const uint64_t CAL_RMASK[CAL_B + 1];
extern const uint64_t CAL_SMASK[CAL_B + 1];

#endif
