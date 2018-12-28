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

#include <stdlib.h>
#include <stdio.h>
#include <utility>
#include <skylge/math/Integer.h>
#include <skylge/testutils/ErrorExamples.h>
#include <skylge/testutils/progressionBar.h>
#include <skylge/testutils/Random.h>
#include "integerTests.h"

/* bigint.m_size should be 11.  */
static bool areEqual (const Integer& bigint, const int* array, bool sign) {
  if (bigint.size () != 11) {
    printf ("integerTests.cpp: areEqual: bigint not of expected size.\n");
    exit (EXIT_FAILURE);
  }

  const uint64_t* buffer = bigint.buf ();
  if (bigint.sign () != sign)
    return false;
  for (int i = 0; i < 11; ++i) {
    if (buffer[i] != array[i])
      return false;
  }
  return true;
}

static int bsf (int value) {
  if (value != 0) {
    int mask = 1;
    int result = 0;
    while ((value & mask) == 0) {
      mask <<= 1;
      ++result;
    }
    return result;
  } else {
    return 0;
  }
}

static int bsr (int value) {
  if (value != 0) {
    int result = 32;
    int mask = 0x80000000;
    while ((value & mask) == 0) {
      mask >>= 1;
      --result;
    }
    return result;
  } else {
    return 0;
  }
}

static int* setValue (int* dst, int64_t value) {
  uint64_t val = value < 0 ? -value : value;
  for (int i = 0; i < 11; ++i) {
    dst[i] = val & 0x3F;
    val >>= 6;
  }
  return dst;
}

static bool testAbsAdd (void) {
  /* TODO: IMPLEMENT */
  return false;
}

static bool testAbsDec (void) {
  /* TODO: IMPLEMENT */
  return false;
}

static bool testAbsInc (void) {
  /* TODO: IMPLEMENT */
  return false;
}

static bool testAbsSub (void) {
  /* TODO: IMPLEMENT */
  return false;
}

static bool testAssign (void) {
  Random random;
  Integer bigint (11);
  int array[11];

  const int max = 0x3FFFFC;
  ErrorExamples errorExamples ("Error for: %ld\n");
  ProgressionBar::init ("Integer::operator= (int64_t)", max + 4);
  for (int i = 0; i < max; ++i) {
    int val = random.nextInt (0x1FFFFF) - 0xFFFFF;
    setValue (array, val);

    bigint = val;

    bool error = !areEqual (bigint, array, val < 0);
    if (error) {
      errorExamples.add (val);
    }
    ProgressionBar::update (error);
  }

  int64_t fixedValues[] = {0x7FFFFFFFFFFFFFFF, (int64_t) 0x8000000000000000, -0x7FFFFFFFFFFFFFFF, 0};
  for (int i = 0; i < 4; ++i) {
    setValue (array, fixedValues[i]);

    bigint = fixedValues[i];

    bool error = !areEqual (bigint, array, fixedValues[i] < 0);
    if (error) {
      errorExamples.add (fixedValues[i]);
    }
    ProgressionBar::update (error);
  }

  errorExamples.print ();
  return !errorExamples.empty ();
}

static bool testBsf (void) {
  Integer bigint (4);

  const int max = 0x200000;
  ErrorExamples errorExamples ("Error for: %ld\n");
  ProgressionBar::init ("Integer::bsf (void)", max);
  for (int i = 0; i < max; ++i) {
    bigint = i;
    bool error = bigint.bsf () != bsf (i);
    if (error) {
      errorExamples.add (i);
    }
    ProgressionBar::update (error);
  }
  errorExamples.print ();
  return !errorExamples.empty ();
}

static bool testBsr (void) {
  Integer bigint (4);

  const int max = 0x200000;
  ErrorExamples errorExamples ("Error for: %ld\n");
  ProgressionBar::init ("Integer::bsr (void)", max);
  for (int i = 0; i < max; ++i) {
    bigint = i;
    bool error = bigint.bsr () != bsr (i);
    if (error) {
      errorExamples.add (i);
    }
    ProgressionBar::update (error);
  }
  errorExamples.print ();
  return !errorExamples.empty ();
}

static bool testCopy (void) {
  Random random;
  Integer bigintA (4);
  Integer bigintB (11);
  const uint64_t* originalBuffer = bigintA.buf ();

  const int max = 0x1FFFFE;
  ErrorExamples errorExamples ("Error for: %ld\n");
  ProgressionBar::init ("Integer::operator= (const Integer&)", max + 2);
  for (int i = 0; i < max; ++i) {
    int val = random.nextInt (0x1FFFFF) - 0xFFFFF;
    bigintB = val;

    bigintA = bigintB;

    bool error = !(bigintA == bigintB && bigintA.size () == 4 && bigintA.buf () == originalBuffer);
    if (error) {
      errorExamples.add (val);
    }
    ProgressionBar::update (error);
  }

  /* Test self assigment.  */
  int64_t val = 1000;
  bigintB = val;
  bigintA = val;
  bigintA = bigintA;
  bool error = !(bigintA == bigintB && bigintA.size () == 4 && bigintA.buf () == originalBuffer);
  if (error) {
    errorExamples.add (val);
  }
  ProgressionBar::update (error);

  /* Test assigment of a number too large for the original buffer.  */
  val = 1000000000000000000;
  bigintB = val;
  bigintA = bigintB;
  error = !(bigintA == bigintB && bigintA.size () == 11 && bigintA.buf () != originalBuffer && bigintA.buf () != bigintB.buf ());
  if (error) {
    errorExamples.add (val);
  }
  ProgressionBar::update (error);

  errorExamples.print ();
  return !errorExamples.empty ();
}

static bool testEqual (void) {
  Random random;
  Integer bigintA (4);
  Integer bigintB (11);

  const int max = 0x200000;
  ErrorExamples errorExamples ("Error for: valA=%ld, valB=%ld\n");
  ProgressionBar::init ("Integer::operator== (const Integer&)", max);
  for (int i = 0; i < max; ++i) {
    int64_t valA = random.nextInt (0x1FFFFF) - 0xFFFFF;
    int64_t valB;
    switch (i % 4) {
    case 0:
      valB = valA;
      break;

    case 1:
      valB = -valA;
      break;

    case 2:
      valB = random.nextInt (0x1FFFFF) - 0xFFFFF;
      break;

    case 3:
      valB = random.nextInt ();
      if (valA < 0)
        valB = -valB;
      break;
    }

    bigintA = valA;
    bigintB = valB;

    bool error = valA == valB ^ bigintA == bigintB;
    if (error) {
      errorExamples.add (valA, valB);
    }
    ProgressionBar::update (error);
  }
  errorExamples.print ();
  return !errorExamples.empty ();
}

static bool testGetBit (void) {
  Integer bigint (3);

  const int max = 0x40000;
  ErrorExamples errorExamples ("Error for: %05lX, bitNo=%ld\n");
  ProgressionBar::init ("Integer::getBit (int)", 18 * max);
  for (int i = 0; i < max; ++i) {
    bigint = i;

    int mask = 1;
    for (int j = 0; j < 18; ++j) {
      bool expectedBit = (i & mask) != 0;
      bool error = bigint.getBit (j) != expectedBit;
      if (error) {
        errorExamples.add (i, (int64_t) j);
      }
      ProgressionBar::update (error);
      mask <<= 1;
    }
  }
  errorExamples.print ();
  return !errorExamples.empty ();
}

static bool testInequal (void) {
  Random random;
  Integer bigintA (4);
  Integer bigintB (11);

  const int max = 0x200000;
  ErrorExamples errorExamples ("Error for: valA=%ld, valB=%ld\n");
  ProgressionBar::init ("Integer::operator!= (const Integer&)", max);
  for (int i = 0; i < max; ++i) {
    int64_t valA = random.nextInt (0x1FFFFF) - 0xFFFFF;
    int64_t valB;
    switch (i % 4) {
    case 0:
      valB = valA;
      break;

    case 1:
      valB = -valA;
      break;

    case 2:
      valB = random.nextInt (0x1FFFFF) - 0xFFFFF;
      break;

    case 3:
      valB = random.nextInt ();
      if (valA < 0)
        valB = -valB;
      break;
    }

    bigintA = valA;
    bigintB = valB;

    bool error = valA != valB ^ bigintA != bigintB;
    if (error) {
      errorExamples.add (valA, valB);
    }
    ProgressionBar::update (error);
  }
  errorExamples.print ();
  return !errorExamples.empty ();
}

static bool testLshl (void) {
  /* TODO: IMPLEMENT */
  return false;
}

/*
NOOT: Wil ik zowel Integer::operator int64_t () als Integer::operator int () houden?
 */
static bool testMove (void) {
  Integer bigintA (4);
  Integer* bigintB;
  const uint64_t* originalBuffer = bigintA.buf ();

  ErrorExamples errorExamples ("Error for: %ld\n");
  ProgressionBar::init ("Integer::operator= (Integer&&)", 3);

  int64_t val = 1000;
  bigintB = new Integer (11);
  *bigintB = val;
  bigintA = std::move (*bigintB);
  delete bigintB;
  bool error = !(val == (int64_t) bigintA && bigintA.size () == 4 && bigintA.buf () == originalBuffer);
  if (error) {
    errorExamples.add (val);
  }
  ProgressionBar::update (error);

  /* Test self assigment.  */
  val = 2000;
  bigintA = val;
  bigintA = std::move (bigintA);
  error = !(val == (int64_t) bigintA && bigintA.size () == 4 && bigintA.buf () == originalBuffer);
  if (error) {
    errorExamples.add (val);
  }
  ProgressionBar::update (error);

  /* Test assigment of a number too large for the original buffer.  */
  val = 1000000000000000000;
  bigintB = new Integer (11);
  const uint64_t* bufB = bigintB->buf ();
  *bigintB = val;
  bigintA = std::move (*bigintB);
  delete bigintB;
  error = !(val == (int64_t) bigintA && bigintA.size () == 11 && bigintA.buf () != originalBuffer && bigintA.buf () == bufB);
  if (error) {
    errorExamples.add (val);
  }
  ProgressionBar::update (error);

  errorExamples.print ();
  return !errorExamples.empty ();
}

static bool testRcl (void) {
  /* TODO: IMPLEMENT */
  return false;
}

static bool testShl (void) {
  Random random;
  Integer bigint (5);

  const int max = 0x200000;
  ErrorExamples errorExamples ("Error for: %08lX, %ld bits to shift.\n");
  ProgressionBar::init ("Integer::shl (int)", max);
  for (int i = 0; i < max; ++i) {
    int x = random.nextInt (31);
    int val = random.bits (30);
    bigint = val;

    bigint.shl (x);
    int expected = val << x & 0x3FFFFFFF;

    bool error = (int) bigint != expected;
    if (error) {
      errorExamples.add (val, (int64_t) x);
    }
    ProgressionBar::update (error);
  }
  errorExamples.print ();
  return !errorExamples.empty ();
}

static bool testShr (void) {
  Random random;
  Integer bigint (5);

  const int max = 0x200000;
  ErrorExamples errorExamples ("Error for: %08lX, %ld bits to shift.\n");
  ProgressionBar::init ("Integer::shr (int)", max);
  for (int i = 0; i < max; ++i) {
    int x = random.nextInt (31);
    int val = random.bits (30);
    bigint = val;

    bigint.shr (x);
    int expected = val >> x;

    bool error = (int) bigint != expected;
    if (error) {
      errorExamples.add (val, (int64_t) x);
    }
    ProgressionBar::update (error);
  }
  errorExamples.print ();
  return !errorExamples.empty ();
}

static bool testToInt (void) {
  Integer bigint (11);
  bool error;

  const int max = 0x1FFFFF;
  ErrorExamples errorExamples ("Error for: %ld\n");
  ProgressionBar::init ("Integer::operator int ()", max + 6);
  for (int i = 0; i < max; ++i) {
    int val = i - 0xFFFFF;
    bigint = val;

    error = (int) bigint != val;
    if (error) {
      errorExamples.add (val);
    }
    ProgressionBar::update (error);
  }

  int64_t values[] = {0x7FFFFFFFL, 0x80000000L, 0x100000000L, -0x7FFFFFFFL, -0x80000001L, -0x100000000L};
  for (int i = 0; i < 6; ++i) {
    bigint = values[i];
    if (values[i] < -0x7FFFFFFFL || values[i] > 0x7FFFFFFFL)
      error = (int) bigint != 0x80000000;
    else
      error = (int) bigint != values[i];
    if (error) {
      errorExamples.add (values[i]);
    }
    ProgressionBar::update (error);
  }

  errorExamples.print ();
  return !errorExamples.empty ();
}

static bool testToInt64 (void) {
  Integer bigint (11);
  bool error;

  const int max = 0x1FFFFF;
  ErrorExamples errorExamples ("Error for: %ld\n");
  ProgressionBar::init ("Integer::operator int64_t ()", max + 8);
  for (int i = 0; i < max; ++i) {
    int val = i - 0xFFFFF;
    bigint = val;

    error = (int64_t) bigint != val;
    if (error) {
      errorExamples.add (val);
    }
    ProgressionBar::update (error);
  }

  int64_t values[] = {0x6518800000000000, 0x7FFFFFFFFFFFFFFF, -0x6518800000000000, -0x7FFFFFFFFFFFFFFF};
  for (int i = 0; i < 4; ++i) {
    bigint = values[i];
    error = (int64_t) bigint != values[i];
    if (error) {
      errorExamples.add (values[i]);
    }
    ProgressionBar::update (error);
  }

  uint64_t* buf = (uint64_t*) bigint.buf ();

  bigint = 1;
  buf[10] = 0x08;
  bigint.setMax (10);
  error = (int64_t) bigint != 0x8000000000000000;
  if (error) {
    errorExamples.add (0x8000000000000000);
  }
  ProgressionBar::update (error);

  buf[10] = 0x1C;
  error = (int64_t) bigint != 0x8000000000000000;
  if (error) {
    errorExamples.add (0x8000000000000000);
  }
  ProgressionBar::update (error);

  bigint = -1;
  buf[10] = 0x08;
  bigint.setMax (10);
  error = (int64_t) bigint != 0x8000000000000000;
  if (error) {
    errorExamples.add (0x8000000000000000);
  }
  ProgressionBar::update (error);

  buf[10] = 0x1C;
  error = (int64_t) bigint != 0x8000000000000000;
  if (error) {
    errorExamples.add (0x8000000000000000);
  }
  ProgressionBar::update (error);

  errorExamples.print ();
  return !errorExamples.empty ();
}

const test_fn_t integerTests[] = {
  testAssign,
  testEqual,
  testInequal,
  testCopy,
  testBsf,
  testBsr,
  testToInt,
  testToInt64,
  testMove,
  testGetBit,
  testShl,
  testShr,
  testRcl,
  testLshl,
  testAbsAdd,
  testAbsDec,
  testAbsInc,
  testAbsSub
};
