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

#include <stdio.h>
#include <string>
#include <skylge/math/IntegerOps.h>
#include <skylge/testutils/ErrorExamples.h>
#include <skylge/testutils/progressionBar.h>
#include <skylge/testutils/Random.h>
#include "integerOpsTests.h"

static bool testAdd (void) {
  Random random;
  IntegerOps ops (4);
  Integer bigintA = ops.createInteger ();
  Integer bigintB = ops.createInteger ();

  const int max = 20000000;
  ErrorExamples errorExamples ("Error for: A=%ld, B=%ld.\n");
  ProgressionBar::init ("IntegerOps::add (Integer&, const Integer&)", max);
  for (int i = 0; i < max; ++i) {
    int64_t valA = random.nextInt (0x1FFFFFF) - 0xFFFFFF;
    int64_t valB = random.nextInt (0x1FFFFFF) - 0xFFFFFF;
    bigintA = valA;
    bigintB = valB;

    bool expectedCarry;
    int64_t expectedSum = valA + valB;
    if (expectedSum > 0xFFFFFF) {
      expectedSum &= 0xFFFFFF;
      expectedCarry = true;
    } else if (expectedSum == -0x1000000) {
      expectedSum = 0;
      expectedCarry = true;
    } else if (expectedSum < -0x1000000) {
      expectedSum |= 0x1000000;
      expectedCarry = true;
    } else {
      expectedCarry = false;
    }

    bool carry = ops.add (bigintA, bigintB);

    bool error = !(carry == expectedCarry && (int) bigintA == expectedSum);
    if (error) {
      errorExamples.add (valA, valB);
    }
    ProgressionBar::update (error);
  }
  errorExamples.print ();
  return !errorExamples.empty ();
}

static bool testAddInt (void) {
  /* TODO: IMPLEMENT */
  return false;
}

static bool testCreateInteger (void) {
  IntegerOps ops (11);
  std::string errors = "";
  ProgressionBar::init ("IntegerOps::createInteger (int64_t)", 4);

  Integer bigint = ops.createInteger ();
  bool error = bigint.size () != 11;
  if (error) {
    errors += "Error: size of created integer not 11, but " + std::to_string (bigint.size ()) + ".\n";
  }
  ProgressionBar::update (error);

  error = (int) bigint != 0;
  if (error) {
    errors += "Error: value not 0, but " + std::to_string ((int) bigint) + ".\n";
  }
  ProgressionBar::update (error);

  int64_t val = 3141592653589793238;
  bigint = ops.createInteger (val);
  error = bigint.size () != 11;
  if (error) {
    errors += "Error: size of created integer not 11, but " + std::to_string (bigint.size ()) + ".\n";
  }
  ProgressionBar::update (error);

  error = (int64_t) bigint != val;
  if (error) {
    errors += "Error: value not " + std::to_string (val) + ", but " + std::to_string ((int64_t) bigint) + ".\n";
  }
  ProgressionBar::update (error);

  if (errors.length () > 0)
    printf ("%s\n", errors.c_str ());
  return errors.length () > 0;
}

static bool testDec (void) {
  IntegerOps ops (3);
  Integer bigint = ops.createInteger ();

  const int max = 0x40000;
  ErrorExamples errorExamples ("Error for: %ld\n");
  ProgressionBar::init ("IntegerOps::dec (Integer&)", 2 * max - 1);
  for (int i = max - 1; i > -max; --i) {
    bigint = i;

    bool carry = ops.dec (bigint);

    int expectedResult = i != -0x3FFFF ? i - 1 : 0;
    bool error = !(carry == (i == -0x3FFFF) && (int) bigint == expectedResult);
    if (error) {
      errorExamples.add (i);
    }
    ProgressionBar::update (error);
  }
  errorExamples.print ();
  return !errorExamples.empty ();
}

static bool testDiv (void) {
  /* TODO: IMPLEMENT */
  return false;
}

static bool testInc (void) {
  IntegerOps ops (3);
  Integer bigint = ops.createInteger ();

  const int max = 0x40000;
  ErrorExamples errorExamples ("Error for: %ld\n");
  ProgressionBar::init ("IntegerOps::inc (Integer&)", 2 * max - 1);
  for (int i = -(max - 1); i < max; ++i) {
    bigint = i;

    bool carry = ops.inc (bigint);

    int expectedResult = i != 0x3FFFF ? i + 1 : 0;
    bool error = !(carry == (i == 0x3FFFF) && (int) bigint == expectedResult);
    if (error) {
      errorExamples.add (i);
    }
    ProgressionBar::update (error);
  }
  errorExamples.print ();
  return !errorExamples.empty ();
}

static bool testMul (void) {
  /* TODO: IMPLEMENT */
  return false;
}

static bool testSub (void) {
  /* TODO: IMPLEMENT */
  return false;
}

const test_fn_t integerOpsTests[] = {
  testCreateInteger,
  testInc,
  testDec,
  testAdd,
  testAddInt,
  testSub,
  testMul,
  testDiv
};
