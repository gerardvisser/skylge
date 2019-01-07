/*
   Author:  Gerard Visser
   e-mail:  visser.gerard(at)gmail.com

   Copyright (C) 2019 Gerard Visser.

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

#include "utils.h"

int Utils::bsf (int64_t val) {
  if (val != 0) {
    int result = 0;
    int64_t mask = 1;
    while ((val & mask) == 0) {
      mask <<= 1;
      ++result;
    }
    return result;
  } else {
    return 0;
  }
}

int Utils::bsr (int64_t val) {
  if (val != 0) {
    int result = 64;
    int64_t mask = 0x8000000000000000;
    while ((val & mask) == 0) {
      mask >>= 1;
      --result;
    }
    return result;
  } else {
    return 0;
  }
}

bool Utils::getBit (int64_t val, int bitNo) {
  int64_t mask = 1;
  mask <<= bitNo;
  return (val & mask) != 0;
}
