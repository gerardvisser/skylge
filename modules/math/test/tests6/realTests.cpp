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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdexcept>
#include <utility>
#include <skylge/math/IntegerOps.h>
#include <skylge/math/Real.h>
#include <skylge/testutils/ErrorExamples.h>
#include <skylge/testutils/progressionBar.h>
#include <skylge/testutils/Random.h>
#include "../../src/skylge/math/ieeeDouble.h"
#include "realTests.h"

#define ASSERT_ASSIGN(real, valueToSet, expectedNumber, expectedExponent) \
  assert (errorExamples, real, valueToSet, expectedNumber, expectedExponent)

#define ASSERT_TO_DOUBLE(real, expectedValue) \
  assert (errorExamples, real, expectedValue)

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

static void assert (ErrorExamples& errorExamples, const Real& real, double expectedValue) {
  if (real.number ().sizeInBits () != 72) {
    printf ("realTests.cpp: assert: real not of expected size (72 bits).\n");
    exit (EXIT_FAILURE);
  }
  bool error = real != expectedValue;
  if (error) {
    Integer num = real.number ();
    bool sign = num.sign ();
    if (sign) {
      num.setSign (false);
    }
    num.shr (60);
    int64_t hbits = num;
    num = real.number ();
    if (sign) {
      num.setSign (false);
    }
    num.shl (12);
    num.shr (12);
    int64_t lbits = num;
    int64_t expo = real.exponent ();
    errorExamples.add (sign, expo, hbits, lbits);
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

static int64_t doubleToInt64Bits (double d) {
  union ieee754_double value;
  value.d = d;
  int64_t result = value.ieee.negative;
  result <<= 11;
  result |= value.ieee.exponent;
  result <<= 20;
  result |= value.ieee.mantissa0;
  result <<= 32;
  result |= value.ieee.mantissa1;
  return result;
}

static void setValue (Real& real, IntegerOps& ops, bool sign, int exponent, int h12bits, int64_t l60bits) {
  Integer& number = const_cast<Integer&> (real.number ());
  Integer& expo = const_cast<Integer&> (real.exponent ());
  expo = exponent;
  number = h12bits;
  number.shl (30);
  ops.add (number, (int) (l60bits >> 30));
  number.shl (30);
  ops.add (number, (int) (l60bits & 0x3FFFFFFF));
  number.setSign (sign);
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
  Real real (2);

  const int max = 63;
  ErrorExamples errorExamples ("Error for: sign=%ld, exp=%03lX, frac=%013lX\n");
  ProgressionBar::init ("Real::operator= (double val) [small reals]", max);

  ASSERT_ASSIGN (real, 0.0, 0, 0);
  ASSERT_ASSIGN (real, DOUBLE_INFINITY, 0, 1);
  ASSERT_ASSIGN (real, -DOUBLE_INFINITY, 0, 1);
  ASSERT_ASSIGN (real, 1.0, 1, 0);
  ASSERT_ASSIGN (real, -0.625, -5, -3);
  ASSERT_ASSIGN (real, 1 / 10.0, 0xCCD, -15);
  ASSERT_ASSIGN (real, -1 / 100.0, -0xA3D, -18);
  ASSERT_ASSIGN (real, 1 / 29.0, 0x235, -14);
  ASSERT_ASSIGN (real, 4097.0, 0x801, 1);
  ASSERT_ASSIGN (real, createDouble (false, 15, 0x5B0AD2B0053FC), 0x15B, 7);
  ASSERT_ASSIGN (real, createDouble (false, -1022, 0), 1, -1022);
  int64_t frac = 0x8000000000000;
  for (int i = 0; i < 52; ++i) {
    ASSERT_ASSIGN (real, createDouble (false, -1023, frac), 1, -1023 - i);
    frac >>= 1;
  }

  errorExamples.print ();
  return !errorExamples.empty ();
}

static bool testCopy (void) {
  Random random;
  Real realA (9);
  Real realB (9);

  const int max = 5;
  ErrorExamples errorExamples ("Error for: 0x%016lX\n");
  ProgressionBar::init ("Real::operator= (const Real&)", max);

  double value = random.nextDouble ();
  realB = value;
  realA = realB;
  bool error = realA != realB;
  if (error) {
    errorExamples.add (doubleToInt64Bits (value));
  }
  ProgressionBar::update (error);

  value = 34359738369.0 * random.nextDouble ();
  realB = value;
  realA = realB;
  error = realA != realB;
  if (error) {
    errorExamples.add (doubleToInt64Bits (value));
  }
  ProgressionBar::update (error);

  value = -DOUBLE_INFINITY;
  realB = value;
  realA = realB;
  error = realA != realB;
  if (error) {
    errorExamples.add (doubleToInt64Bits (value));
  }
  ProgressionBar::update (error);

  /* Test self assigment.  */
  value = -3.0 * random.nextDouble ();
  realB = value;
  realA = value;
  realA = realA;
  error = realA != realB;
  if (error) {
    errorExamples.add (doubleToInt64Bits (value));
  }
  ProgressionBar::update (error);

  /* Test assigment of a different sized real.  */
  Real realC (10);
  value = 1.0;
  realC = value;
  realA = realC;
  error = !(realA == realC && realA.number ().sizeInBits () ==  realC.number ().sizeInBits ());
  if (error) {
    errorExamples.add (doubleToInt64Bits (value));
  }
  ProgressionBar::update (error);

  errorExamples.print ();
  return !errorExamples.empty ();
}

static bool testEqual (void) {
  Random random;
  Real realA (9);
  Real realB (9);

  const int max = 4;
  ErrorExamples errorExamples ("Error for: valA=0x%016lX, valB=0x%016lX\n");
  ProgressionBar::init ("Real::operator== (const Real&)", max);

  double valA = 2147483648.0 * random.nextDouble () + 1;
  realA = valA;
  realB = valA;
  bool error = !(realA == realB);
  if (error) {
    errorExamples.add (doubleToInt64Bits (valA), doubleToInt64Bits (valA));
  }
  ProgressionBar::update (error);

  union ieee754_double valB;
  valB.d = valA;
  --valB.ieee.exponent;
  realB = valB.d;
  error = realA == realB;
  if (error) {
    errorExamples.add (doubleToInt64Bits (valA), doubleToInt64Bits (valB.d));
  }
  ProgressionBar::update (error);

  valB.d = valA;
  ++valB.ieee.mantissa0;
  realB = valB.d;
  error = realA == realB;
  if (error) {
    errorExamples.add (doubleToInt64Bits (valA), doubleToInt64Bits (valB.d));
  }
  ProgressionBar::update (error);

  valB.d = valA;
  ++valB.ieee.exponent;
  --valB.ieee.mantissa0;
  realB = valB.d;
  error = realA == realB;
  if (error) {
    errorExamples.add (doubleToInt64Bits (valA), doubleToInt64Bits (valB.d));
  }
  ProgressionBar::update (error);

  errorExamples.print ();
  return !errorExamples.empty ();
}

static bool testInequal (void) {
  Random random;
  Real realA (9);
  Real realB (9);

  const int max = 4;
  ErrorExamples errorExamples ("Error for: valA=0x%016lX, valB=0x%016lX\n");
  ProgressionBar::init ("Real::operator!= (const Real&)", max);

  double valA = 2147483648.0 * random.nextDouble () + 1;
  realA = valA;
  realB = valA;
  bool error = realA != realB;
  if (error) {
    errorExamples.add (doubleToInt64Bits (valA), doubleToInt64Bits (valA));
  }
  ProgressionBar::update (error);

  union ieee754_double valB;
  valB.d = valA;
  --valB.ieee.exponent;
  realB = valB.d;
  error = !(realA != realB);
  if (error) {
    errorExamples.add (doubleToInt64Bits (valA), doubleToInt64Bits (valB.d));
  }
  ProgressionBar::update (error);

  valB.d = valA;
  ++valB.ieee.mantissa0;
  realB = valB.d;
  error = !(realA != realB);
  if (error) {
    errorExamples.add (doubleToInt64Bits (valA), doubleToInt64Bits (valB.d));
  }
  ProgressionBar::update (error);

  valB.d = valA;
  ++valB.ieee.exponent;
  --valB.ieee.mantissa0;
  realB = valB.d;
  error = !(realA != realB);
  if (error) {
    errorExamples.add (doubleToInt64Bits (valA), doubleToInt64Bits (valB.d));
  }
  ProgressionBar::update (error);

  errorExamples.print ();
  return !errorExamples.empty ();
}

static bool testMove (void) {
  Real realA (9);

  ErrorExamples errorExamples ("Error for: 0x%016lX\n");
  ProgressionBar::init ("Real::operator= (Real&&)", 2);

  double value = 8.0 / 7;
  Real* realB = new Real (9);
  *realB = value;
  realA = std::move (*realB);
  delete realB;
  bool error = value != realA;
  if (error) {
    errorExamples.add (doubleToInt64Bits (value));
  }
  ProgressionBar::update (error);

  /* Test self assigment.  */
  value = 1.602176634e-19;
  realA = value;
  realA = std::move (realA);
  error = value != realA;
  if (error) {
    errorExamples.add (doubleToInt64Bits (value));
  }
  ProgressionBar::update (error);

  errorExamples.print ();
  return !errorExamples.empty ();
}

static bool testToDouble (void) {
  IntegerOps ops (12);
  Real real (12);

  const int max = 25;
  ErrorExamples errorExamples ("Error for: sign=%ld, exp=%ld, num=0x%03lX%015lX\n");
  ProgressionBar::init ("Real::operator double ()", max);

  real = 0.0;
  ASSERT_TO_DOUBLE (real, 0);

  real = DOUBLE_INFINITY;
  ASSERT_TO_DOUBLE (real, DOUBLE_INFINITY);

  real = -DOUBLE_INFINITY;
  ASSERT_TO_DOUBLE (real, -DOUBLE_INFINITY);

  setValue (real, ops, false, 971, 0x0, 0x1FFFFFFFFFFFFF);
  ASSERT_TO_DOUBLE (real, createDouble (false, 1023, 0xFFFFFFFFFFFFF));

  setValue (real, ops, false, 1024, 0x0, 0x1);
  ASSERT_TO_DOUBLE (real, DOUBLE_INFINITY);

  setValue (real, ops, false, 972, 0x0, 0x1FFFFFFFFFFFFF);
  ASSERT_TO_DOUBLE (real, DOUBLE_INFINITY);

  setValue (real, ops, true, 1023, 0x0, 0x3);
  ASSERT_TO_DOUBLE (real, -DOUBLE_INFINITY);

  setValue (real, ops, false, -1022, 0x0, 0x1);
  ASSERT_TO_DOUBLE (real, createDouble (false, -1022, 0x0));

  setValue (real, ops, false, -1023, 0x0, 0x1);
  ASSERT_TO_DOUBLE (real, createDouble (false, -1023, 0x8000000000000));

  setValue (real, ops, true, -1026, 0x0, 0x5);
  ASSERT_TO_DOUBLE (real, createDouble (true, -1023, 0x5000000000000));

  setValue (real, ops, false, -1074, 0x0, 0x1);
  ASSERT_TO_DOUBLE (real, createDouble (false, -1023, 0x1));

  setValue (real, ops, false, -1075, 0x0, 0x1);
  ASSERT_TO_DOUBLE (real, 0.0);

  setValue (real, ops, true, -1075, 0x0, 0x1);
  ASSERT_TO_DOUBLE (real, 0.0);

  setValue (real, ops, false, -64, 0xCFC, 0x4D00460CDD9503B);
  ASSERT_TO_DOUBLE (real, createDouble (false, 7, 0x9F89A008C19BB));

  setValue (real, ops, false, -69, 0xE08, 0x53D8963A03E58C1);
  ASSERT_TO_DOUBLE (real, createDouble (false, 2, 0xC10A7B12C7408));

  setValue (real, ops, false, -70, 0x7FF, 0xFFFFFFFFFFE5883);
  ASSERT_TO_DOUBLE (real, createDouble (false, 1, 0x0));

  setValue (real, ops, true, -60, 0x035, 0x3D887D9893E7D45);
  ASSERT_TO_DOUBLE (real, createDouble (true, 5, 0xA9EC43ECC49F4));

  setValue (real, ops, true, -65, 0x007, 0x59E2FAE79C4212F);
  ASSERT_TO_DOUBLE (real, createDouble (true, -3, 0xD678BEB9E7108));

  setValue (real, ops, false, -55, 0x001, 0x6DC2D61AC8E7F4D);
  ASSERT_TO_DOUBLE (real, createDouble (false, 5, 0x6DC2D61AC8E7F));

  setValue (real, ops, false, -59, 0x000, 0xEBBA5BB83746A51);
  ASSERT_TO_DOUBLE (real, createDouble (false, 0, 0xD774B7706E8D5));

  setValue (real, ops, true, -60, 0x000, 0x3FFFFFFFFFFFFF3);
  ASSERT_TO_DOUBLE (real, createDouble (true, -2, 0x0));

  setValue (real, ops, true, -50, 0x000, 0x067C85B28945765);
  ASSERT_TO_DOUBLE (real, createDouble (true, 4, 0x9F216CA2515D9));

  setValue (real, ops, true, -54, 0x000, 0x02978D27D676261);
  ASSERT_TO_DOUBLE (real, createDouble (true, -1, 0x4BC693EB3B131));

  real = 1 / 53.0;
  ASSERT_TO_DOUBLE (real, 1 / 53.0);

  real = -2.5;
  ASSERT_TO_DOUBLE (real, -2.5);

  /* TODO: FINISH */

  errorExamples.print ();
  return !errorExamples.empty ();
}

const test_fn_t realTests[] = {
  testAssign,
  testAssignSmallReals,
  testEqual,
  testInequal,
  testCopy,
  testToDouble,
  testMove
};
