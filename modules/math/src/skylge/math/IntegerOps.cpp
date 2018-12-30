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

#include <skylge/math/IntegerOps.h>
#include "defs.h"
#include "errors.h"

#define MAX_SIZE 8192
#define MIN_SIZE 2

IntegerOps::IntegerOps (int size) : m_size (size), m_bsize (m_size * CAL_B) {
#ifdef DEBUG_MODE
  if (size < MIN_SIZE || size > MAX_SIZE) {
    PRINT_MESSAGE_AND_EXIT ("[IntegerOps::IntegerOps(int)] The specified size (%d) is out of range. It should be: %d < size < %d.\n", size, MIN_SIZE - 1, MAX_SIZE + 1);
  }
#endif

  m_mulResult = new Integer (2 * size);
  m_remainder = new Integer (size);
  m_numerator = new Integer (size);
  m_aux = new Integer (size);
}

IntegerOps::~IntegerOps (void) {
  delete m_mulResult;
  delete m_remainder;
  delete m_numerator;
  delete m_aux;
}

bool IntegerOps::add (Integer& dst, const Integer& src) {
  /* TODO: IMPLEMENT */
}

bool IntegerOps::add (Integer& dst, int value) {
  /* TODO: IMPLEMENT */
}

void IntegerOps::baseDiv (Integer& result, const Integer& denominator, int denomBsr, int total) {
  /* TODO: IMPLEMENT */
}

void IntegerOps::baseMul (const Integer& srcA, const Integer& srcB) {
  /* TODO: IMPLEMENT */
}

Integer IntegerOps::createInteger (int64_t value) {
  Integer result (m_size);
  result = value;
  return result;
}

bool IntegerOps::dec (Integer& dst) {
  /* TODO: IMPLEMENT */
}

Integer& IntegerOps::div (Integer& dst, const Integer& src) {
  /* TODO: IMPLEMENT */
  return *m_remainder;
}

bool IntegerOps::inc (Integer& dst) {
  /* TODO: IMPLEMENT */
}

Integer& IntegerOps::mul (const Integer& srcA, const Integer& srcB) {
  /* TODO: IMPLEMENT */
  return *m_mulResult;
}

bool IntegerOps::sub (Integer& dst, const Integer& src) {
  /* TODO: IMPLEMENT */
}

bool IntegerOps::subtractFromRemainder (const Integer& denominator, int denomBsr, int remainderBsr) {
  /* TODO: IMPLEMENT */
}
