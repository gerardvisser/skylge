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

#if __has_include (<ieee754.h>)

# include <ieee754.h>

#elif __has_include (<machine/endian.h>)

# include <machine/endian.h>

# if BYTE_ORDER == LITTLE_ENDIAN

union ieee754_double {
  double d;
  struct {
    unsigned int mantissa1 : 32;
    unsigned int mantissa0 : 20;
    unsigned int exponent  : 11;
    unsigned int negative  : 1;
  } ieee;
};

# elif BYTE_ORDER == BIG_ENDIAN

union ieee754_double {
  double d;
  struct {
    unsigned int negative  : 1;
    unsigned int exponent  : 11;
    unsigned int mantissa0 : 20;
    unsigned int mantissa1 : 32;
  } ieee;
};

# else
#  error Cannot compile on this platform.
# endif

# define IEEE754_DOUBLE_BIAS 0x3FF

#else
# error Cannot compile on this platform.
#endif

#define DOUBLE_EXPONENT_BIAS     (IEEE754_DOUBLE_BIAS + DOUBLE_FRACTIONAL_WIDTH)
#define DOUBLE_FRACTION_MASK     0xFFFFFFFFFFFFF
#define DOUBLE_FRACTIONAL_WIDTH  52
#define DOUBLE_INFINITY          (1.0 / 0.0)
#define DOUBLE_MAX_EXPONENT      1023
#define DOUBLE_MIN_EXPONENT      (DOUBLE_MIN_EXPONENT_N - DOUBLE_FRACTIONAL_WIDTH)
#define DOUBLE_MIN_EXPONENT_N    (-1022)
#define DOUBLE_SIGNIFICAND_WIDTH (DOUBLE_FRACTIONAL_WIDTH + 1)

#endif
