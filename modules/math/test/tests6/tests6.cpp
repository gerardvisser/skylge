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

#include <stdio.h>
#include <skylge/testutils/ErrorExamples.h>
#include <skylge/testutils/Random.h>

int main (int argc, char** args, char** env) {
  Random random;
  ErrorExamples x ("Error for: bigintA = %ld (valA = 0x%07lX),  bigintB = %ld (valB = 0x%07lX)\n");
  x.add (random.bits (28), random.bits (28), random.bits (28), random.bits (28));
  x.add (random.bits (28), random.bits (28), random.bits (28), random.bits (28));
  x.print ();
  return 0;
}
