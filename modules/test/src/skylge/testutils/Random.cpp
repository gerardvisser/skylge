/*
   Author:  Gerard Visser
   e-mail:  visser.gerard(at)gmail.com

   Copyright (C) 2018 Gerard Visser.

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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <skylge/testutils/Random.h>

static const int max[] = {
  0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
  0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000, 0x10000
};

Random::Random (void) {
  srand (time (nullptr));
}

int64_t Random::bits (int count) {
  if (count < 0 || count > 64) {
    printf ("Random::bits: count should be in the range [0, 64].");
    exit (EXIT_FAILURE);
  }
  int64_t result = 0;
  int q = count >> 4;
  int r = count & 0xF;
  int i = 0;
  while (i < q) {
    result <<= 16;
    result |= (int64_t) (max[16] * nextDouble ());
    ++i;
  }
  if (r > 0) {
    result <<= r;
    result |= (int64_t) (max[r] * nextDouble ());
  }
  return result;
}

double Random::nextDouble (void) {
  return rand () / (RAND_MAX + 1.0);
}

int Random::nextInt (void) {
  return rand ();
}
