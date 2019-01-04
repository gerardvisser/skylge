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
#include <string.h>
#include <stdexcept>
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

  const int max = 19999983;
  ErrorExamples errorExamples ("Error for: A=%ld, B=%ld.\n");
  ProgressionBar::init ("IntegerOps::add (Integer&, const Integer&)", max + 17);
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

  int fixedA[] = {0x27A1E, 0x0, -0x995, 0x272, -0x53C, 0xFFF16B, -0xFEC, 0x2A2F8,
                  -0x4DD, -0x1015, 0x80367, -0xB1E, 0x58C6, -0x2AB, 0xFEC, -0xAD2,
                  0x221};
  int fixedB[] = {0x0, 0x42A651, -0xFFF66B, 0x3FE8C, -0x3F986, 0xE9C, -0x35, 0x1F,
                  -0xB3E, 0xFE0, -0x651, 0x17, -0x1A786, 0x2AB, -0x1031, 0x980629,
                  -0x1A5A};
  for (int i = 0; i < 17; ++i) {
    bigintA = fixedA[i];
    bigintB = fixedB[i];

    bool expectedCarry;
    int64_t expectedSum = fixedA[i] + fixedB[i];
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
      errorExamples.add (fixedA[i], fixedB[i]);
    }
    ProgressionBar::update (error);
  }

  errorExamples.print ();
  return !errorExamples.empty ();
}

static bool testAddInt (void) {
  IntegerOps ops (3);
  Integer bigint = ops.createInteger ();

  const int max = (0x40000 + 0x3FFFF) * (64 + 63) + 1;
  ErrorExamples errorExamples ("Error for: bigint=%ld, val=%ld.\n");
  ProgressionBar::init ("IntegerOps::add (Integer&, int)", max);
  for (int i = -63; i < 64; ++i) {
    for (int j = -0x3FFFF; j < 0x40000; ++j) {
      bigint = j;

      bool expectedCarry;
      int64_t expectedSum = j + i;
      if (expectedSum > 0x3FFFF) {
        expectedSum &= 0x3FFFF;
        expectedCarry = true;
      } else if (expectedSum == -0x40000) {
        expectedSum = 0;
        expectedCarry = true;
      } else if (expectedSum < -0x40000) {
        expectedSum |= 0x40000;
        expectedCarry = true;
      } else {
        expectedCarry = false;
      }

      bool carry = ops.add (bigint, i);

      bool error = !(carry == expectedCarry && (int) bigint == expectedSum);
      if (error) {
        errorExamples.add (j, (int64_t) i);
      }
      ProgressionBar::update (error);
    }
  }

  int valA = 3347;
  int valB = 8219;
  int expectedSum = valA + valB;
  bigint = valA;
  bool carry = ops.add (bigint, valB);
  bool error = !(carry == false && (int) bigint == expectedSum);
  if (error) {
    errorExamples.add (valA, (int64_t) valB);
  }
  ProgressionBar::update (error);

  errorExamples.print ();
  return !errorExamples.empty ();
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
  Random random;
  IntegerOps ops (4);
  Integer bigintA = ops.createInteger ();
  Integer bigintB = ops.createInteger ();

  const int max = 19999998;
  ErrorExamples errorExamples ("Error for: A=%ld, B=%ld.\n");
  ProgressionBar::init ("IntegerOps::div (Integer&, const Integer&)", max + 2);
  for (int i = 0; i < max; ++i) {
    int64_t valA = random.nextInt (0x1FFFFFF) - 0xFFFFFF;
    int64_t valB = random.nextInt (0x1FFFFFF) - 0xFFFFFF;
    if (valB == 0)
      valB = 1;
    bigintA = valA;
    bigintB = valB;

    int expectedQuotient = valA / valB;
    int expectedRemainder = valA % valB;

    Integer& remainder = ops.div (bigintA, bigintB);

    bool error = !((int) bigintA == expectedQuotient && (int) remainder == expectedRemainder);
    if (error) {
      errorExamples.add (valA, valB);
    }
    ProgressionBar::update (error);
  }

  bigintA = 0;
  bigintB = 1000;
  Integer& remainder = ops.div (bigintA, bigintB);
  bool error = !((int) bigintA == 0 && (int) remainder == 0);
  if (error) {
    errorExamples.add (0, (int64_t) 1000);
  }
  ProgressionBar::update (error);

  bigintA = 1000;
  bigintB = 0;
  try {
    ops.div (bigintA, bigintB);
    error = true;
  } catch (std::exception& x) {
    error = strcmp (x.what (), "Division by zero.") != 0;
  }
  if (error) {
    errorExamples.add (1000, (int64_t) 0);
  }
  ProgressionBar::update (error);

  errorExamples.print ();
  return !errorExamples.empty ();
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
  Random random;
  IntegerOps ops (4);
  Integer bigintA = ops.createInteger ();
  Integer bigintB = ops.createInteger ();

  const int max = 20000000;
  ErrorExamples errorExamples ("Error for: A=%ld, B=%ld.\n");
  ProgressionBar::init ("IntegerOps::mul (const Integer&, const Integer&)", max);
  for (int i = 0; i < max; ++i) {
    int64_t valA = random.nextInt (0x1FFFFFF) - 0xFFFFFF;
    int64_t valB = random.nextInt (0x1FFFFFF) - 0xFFFFFF;
    bigintA = valA;
    bigintB = valB;

    int64_t expectedProduct = valA * valB;

    Integer& result = ops.mul (bigintA, bigintB);

    bool error = (int64_t) result != expectedProduct;
    if (error) {
      errorExamples.add (valA, valB);
    }
    ProgressionBar::update (error);
  }
  errorExamples.print ();
  return !errorExamples.empty ();
}

static bool testSub (void) {
  Random random;
  IntegerOps ops (4);
  Integer bigintA = ops.createInteger ();
  Integer bigintB = ops.createInteger ();

  const int max = 19999998;
  ErrorExamples errorExamples ("Error for: A=%ld, B=%ld.\n");
  ProgressionBar::init ("IntegerOps::sub (Integer&, const Integer&)", max + 2);
  for (int i = 0; i < max; ++i) {
    int64_t valA = random.nextInt (0x1FFFFFF) - 0xFFFFFF;
    int64_t valB = random.nextInt (0x1FFFFFF) - 0xFFFFFF;
    bigintA = valA;
    bigintB = valB;

    bool expectedCarry;
    int64_t expectedDiff = valA - valB;
    if (expectedDiff > 0xFFFFFF) {
      expectedDiff &= 0xFFFFFF;
      expectedCarry = true;
    } else if (expectedDiff == -0x1000000) {
      expectedDiff = 0;
      expectedCarry = true;
    } else if (expectedDiff < -0x1000000) {
      expectedDiff |= 0x1000000;
      expectedCarry = true;
    } else {
      expectedCarry = false;
    }

    bool carry = ops.sub (bigintA, bigintB);

    bool error = !(carry == expectedCarry && (int) bigintA == expectedDiff);
    if (error) {
      errorExamples.add (valA, valB);
    }
    ProgressionBar::update (error);
  }

  int fixedA[] = {0x27A1E, 0x0};
  int fixedB[] = {0x0, 0x42A651};
  for (int i = 0; i < 2; ++i) {
    bigintA = fixedA[i];
    bigintB = fixedB[i];

    bool expectedCarry = false;
    int64_t expectedDiff = fixedA[i] - fixedB[i];

    bool carry = ops.sub (bigintA, bigintB);

    bool error = !(carry == expectedCarry && (int) bigintA == expectedDiff);
    if (error) {
      errorExamples.add (fixedA[i], fixedB[i]);
    }
    ProgressionBar::update (error);
  }

  errorExamples.print ();
  return !errorExamples.empty ();
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
