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

#include <skylge/math/Integer.h>
#include <skylge/testutils/ErrorExamples.h>
#include <skylge/testutils/progressionBar.h>
#include <skylge/testutils/Random.h>
#include "integerTests.h"

/* TEST EXPLICIET VOOR 0x8000000000000000: KOMEN WE NIET IN EEN ONEINDIGE LUS????  */
static void testAssign (void) {
  Random random;
  Integer bigint (11);
  const int max = 0x400000;
  ErrorExamples errorExamples ("Error for: %ld\n");
  ProgressionBar::init ("Integer::operator= (int64_t)", max);
  for (int i = 0; i < max; ++i) {
    int val = random.nextInt (0x1FFFFF) - 0xFFFFF;

    bigint = val;

    bool error = false;
    if (error) {
      errorExamples.add (val);
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
