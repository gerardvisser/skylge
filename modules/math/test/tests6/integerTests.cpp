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

static void testCopy (void) {
}

static void testMove (void) {
}

const test_fn_t integerTests[] = {
  testAssign,
  testCopy,
  testMove
};
