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

#ifndef SKYLGE__TESTUTILS___RANDOM_INCLUDED
#define SKYLGE__TESTUTILS___RANDOM_INCLUDED

#include <stdint.h>

class Random {
public:
  Random (void);
  Random (const Random&) = delete;
  Random (Random&&) = delete;

  Random& operator= (const Random&) = delete;
  Random& operator= (Random&&) = delete;

  int64_t bits (int count);
  double nextDouble (void);
  int nextInt (int bound);
  int nextInt (void);
};

#endif
