/*
   Author:  Gerard Visser
   e-mail:  visser.gerard(at)gmail.com

   Copyright (C) 2019 Gerard Visser.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef SKYLGE__MATH__IEEE_DOUBLE_INCLUDED
#define SKYLGE__MATH__IEEE_DOUBLE_INCLUDED

/* This include is not available on Mac Os X.  */
#include <ieee754.h>

#define DOUBLE_EXPONENT_BIAS     (IEEE754_DOUBLE_BIAS + DOUBLE_FRACTIONAL_WIDTH)
#define DOUBLE_FRACTION_MASK     0xFFFFFFFFFFFFF
#define DOUBLE_FRACTIONAL_WIDTH  52
#define DOUBLE_INFINITY          (1.0 / 0.0)
#define DOUBLE_MAX_EXPONENT      1023
#define DOUBLE_MIN_EXPONENT      (DOUBLE_MIN_EXPONENT_N - DOUBLE_FRACTIONAL_WIDTH)
#define DOUBLE_MIN_EXPONENT_N    (-1022)
#define DOUBLE_SIGNIFICAND_WIDTH (DOUBLE_FRACTIONAL_WIDTH + 1)

#endif
