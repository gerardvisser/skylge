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

static int* setValue (int* dst, int64_t value) {
  uint64_t val = value < 0 ? -value : value;
  for (int i = 0; i < 11; ++i) {
    dst[i] = val & 0x3F;
    val >>= 6;
  }
  return dst;
}

static void testAssign (void) {
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
}

static void testBsf (void) {
  /* TODO: IMPLEMENT */
}

static void testBsr (void) {
  /* TODO: IMPLEMENT */
}

static void testCopy (void) {
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
}

static void testEqual (void) {
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
}

static void testInequal (void) {
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
}

/*
NOOT: Wil ik zowel Integer::operator int64_t () als Integer::operator int () houden?
 */
static void testMove (void) {
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
}

static void testToInt (void) {
  /* TODO: IMPLEMENT */
}

static void testToInt64 (void) {
  /* TODO: IMPLEMENT */
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
  testMove
};
