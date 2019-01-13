/*
   Author:  Gerard Visser
   e-mail:  visser.gerard(at)gmail.com

   Copyright (C) 2019 Gerard Visser.

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

#include <string.h>
#include <stdexcept>
#include <skylge/math/Real.h>
#include <skylge/testutils/ErrorExamples.h>
#include <skylge/testutils/progressionBar.h>
#include <skylge/testutils/Random.h>
#include "../../src/skylge/math/ieeeDouble.h"
#include "realTests.h"

#define ASSERT_ASSIGN(real, valueToSet, expectedNumber, expectedExponent) \
  assert (errorExamples, real, valueToSet, expectedNumber, expectedExponent)

static void assert (ErrorExamples& errorExamples, Real& real, double valueToSet, int64_t expectedNumber, int expectedExponent) {
  real = valueToSet;
  bool error = (int64_t) real.number () != expectedNumber;
  error |= (int64_t) real.exponent () != expectedExponent;
  if (valueToSet == DOUBLE_INFINITY) {
    error |= !real.isInfinite () || real.sign ();
  } else if (valueToSet == -DOUBLE_INFINITY) {
    error |= !real.isInfinite () || !real.sign ();
  } else {
    error |= real.isInfinite () || real.sign () != valueToSet < 0;
  }
  if (error) {
    union ieee754_double value;
    value.d = valueToSet;
    int64_t sign = value.ieee.negative;
    int64_t exponent = value.ieee.exponent;
    int64_t frac = value.ieee.mantissa0;
    frac <<= 32;
    frac |= value.ieee.mantissa1;
    errorExamples.add (sign, exponent, frac);
  }
  ProgressionBar::update (error);
}

static double createDouble (bool sign, int exponent, int64_t fraction) {
  union ieee754_double value;
  value.ieee.negative = sign;
  value.ieee.exponent = exponent + IEEE754_DOUBLE_BIAS;
  value.ieee.mantissa0 = fraction >> 32;
  value.ieee.mantissa1 = fraction & 0xFFFFFFFF;
  return value.d;
}

static bool testAssign (void) {
  Real real (9);

  const int max = 63;
  ErrorExamples errorExamples ("Error for: sign=%ld, exp=%03lX, frac=%013lX\n");
  ProgressionBar::init ("Real::operator= (double val)", max);

  ASSERT_ASSIGN (real, 0.0, 0, 0);
  ASSERT_ASSIGN (real, DOUBLE_INFINITY, 0, 1);
  ASSERT_ASSIGN (real, -DOUBLE_INFINITY, 0, 1);
  ASSERT_ASSIGN (real, 1.0, 1, 0);
  ASSERT_ASSIGN (real, -0.625, -5, -3);
  ASSERT_ASSIGN (real, 1 / 10.0, 0xCCCCCCCCCCCCD, -55);
  ASSERT_ASSIGN (real, -1 / 100.0, -0x147AE147AE147B, -59);
  ASSERT_ASSIGN (real, createDouble (false, 15, 0x5B9AD2B0053FC), 0x56E6B4AC014FF, -35);
  ASSERT_ASSIGN (real, createDouble (true, 263, 0xA2F93D87809), -0x100A2F93D87809, 211);
  ASSERT_ASSIGN (real, createDouble (false, -1022, 0), 1, -1022);
  int64_t frac = 0x8000000000000;
  for (int i = 0; i < 52; ++i) {
    ASSERT_ASSIGN (real, createDouble (false, -1023, frac), 1, -1023 - i);
    frac >>= 1;
  }

  bool error;
  try {
    real = 0.0 / 0.0;
    error = true;
  } catch (std::exception& x) {
    error = strcmp (x.what (), "Cannot set NaN.") != 0;
  }
  if (error) {
    union ieee754_double value;
    value.d = 0.0 / 0.0;
    int64_t sign = value.ieee.negative;
    int64_t exponent = value.ieee.exponent;
    int64_t frac = value.ieee.mantissa0;
    frac <<= 32;
    frac |= value.ieee.mantissa1;
    errorExamples.add (sign, exponent, frac);
  }
  ProgressionBar::update (error);

  errorExamples.print ();
  return !errorExamples.empty ();
}

static bool testAssignSmallReals (void) {
  /* TODO: IMPLEMENT */
}

static bool testCopy (void) {
  /* TODO: IMPLEMENT */
}

static bool testEqual (void) {
  /* TODO: IMPLEMENT */
}

static bool testInequal (void) {
  /* TODO: IMPLEMENT */
}

static bool testMove (void) {
  /* TODO: IMPLEMENT */
}

static bool testToDouble (void) {
  /* TODO: IMPLEMENT */
}

const test_fn_t realTests[] = {
  testAssign,
  testAssignSmallReals,
  testEqual,
  testInequal,
  testCopy,
  testMove,
  testToDouble
};
