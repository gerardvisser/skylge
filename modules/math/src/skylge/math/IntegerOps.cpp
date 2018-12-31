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
  VALIDATE_INTEGER ("IntegerOps::add(Integer&, const Integer&)", dst, LOC_BEFORE);
  VALIDATE_INTEGER ("IntegerOps::add(Integer&, const Integer&)", src, LOC_BEFORE);
#ifdef DEBUG_MODE
  if (!(dst.m_size == m_size && src.m_size == m_size)) {
    PRINT_MESSAGE_AND_EXIT ("[IntegerOps::add(Integer&, const Integer&)] The two arguments `dst' and `src' need to be of size %d.\n", m_size);
  }
#endif

  bool carry;
  if (src.m_max > 0) {
    if (dst.m_max > 0) {
      if (dst.m_sign ^ src.m_sign)
        carry = dst.absSub (src);
      else
        carry = dst.absAdd (src);
    } else {
      dst = src;
      carry = false;
    }
  } else {
    carry = false;
  }

  VALIDATE_INTEGER ("IntegerOps::add(Integer&, const Integer&)", dst, LOC_AFTER);
  return carry;
}

bool IntegerOps::add (Integer& dst, int value) {
  VALIDATE_INTEGER ("IntegerOps::add(Integer&, int)", dst, LOC_BEFORE);
#ifdef DEBUG_MODE
  if (dst.m_size != m_size) {
    PRINT_MESSAGE_AND_EXIT ("[IntegerOps::add(Integer&, int)] Argument `dst' needs to be of size %d.\n", m_size);
  }
#endif

  bool carry;

#if CAL_B == 6
  if (value < -63 || value > 63) {
    *m_aux = value;
    carry = add (dst, *m_aux);
  } else {
#endif

    if (value != 0) {
      if (dst.m_max > 0) {
        bool valueSign = value < 0;
        uint64_t val = valueSign ? - (int64_t) value : value;
        if (dst.m_sign ^ valueSign)
          carry = dst.absSub (val);
        else
          carry = dst.absAdd (val);
      } else {
        dst = value;
        carry = false;
      }
    } else {
      carry = false;
    }

#if CAL_B == 6
  }
#endif

  VALIDATE_INTEGER ("IntegerOps::add(Integer&, int)", dst, LOC_AFTER);
  return carry;
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
  VALIDATE_INTEGER ("IntegerOps::dec(Integer&)", dst, LOC_BEFORE);
#ifdef DEBUG_MODE
  if (dst.m_size != m_size) {
    PRINT_MESSAGE_AND_EXIT ("[IntegerOps::dec(Integer&)] Argument `dst' needs to be of size %d.\n", m_size);
  }
#endif

  bool carry;
  if (dst.m_max > 0) {
    if (dst.m_sign)
      carry = dst.absInc ();
    else
      carry = dst.absDec ();
  } else {
    dst.m_max = 1;
    dst.m_buf[0] = 1;
    dst.m_sign = true;
    carry = false;
  }

  VALIDATE_INTEGER ("IntegerOps::dec(Integer&)", dst, LOC_AFTER);
  return carry;
}

Integer& IntegerOps::div (Integer& dst, const Integer& src) {
  VALIDATE_INTEGER ("IntegerOps::div(Integer&, const Integer&)", dst, LOC_BEFORE);
  VALIDATE_INTEGER/*_LAST_BIT_0*/ ("IntegerOps::div(Integer&, const Integer&)", src, LOC_BEFORE);
  /* TODO: Laatste bit zou ook niet nul moeten kunnen zijn!!! Kijk of er een aanpassing van Integer::lshl nodig is:
           misschien moet de size van m_remainder nl. met 1 worden verhoogd. */
#ifdef DEBUG_MODE
  if (!(dst.m_size == m_size && src.m_size == m_size)) {
    PRINT_MESSAGE_AND_EXIT ("[IntegerOps::div(Integer&, const Integer&)] The two arguments `dst' and `src' need to be of size %d.\n", m_size);
  }
#endif

  /* TODO: IMPLEMENT */


  VALIDATE_INTEGER ("IntegerOps::div(Integer&, const Integer&)", *m_remainder, LOC_AFTER);
  return *m_remainder;
}

bool IntegerOps::inc (Integer& dst) {
  VALIDATE_INTEGER ("IntegerOps::inc(Integer&)", dst, LOC_BEFORE);
#ifdef DEBUG_MODE
  if (dst.m_size != m_size) {
    PRINT_MESSAGE_AND_EXIT ("[IntegerOps::inc(Integer&)] Argument `dst' needs to be of size %d.\n", m_size);
  }
#endif

  bool carry;
  if (dst.m_max > 0) {
    if (dst.m_sign)
      carry = dst.absDec ();
    else
      carry = dst.absInc ();
  } else {
    dst.m_max = 1;
    dst.m_buf[0] = 1;
    dst.m_sign = false;
    carry = false;
  }

  VALIDATE_INTEGER ("IntegerOps::inc(Integer&)", dst, LOC_AFTER);
  return carry;
}

Integer& IntegerOps::mul (const Integer& srcA, const Integer& srcB) {
  VALIDATE_INTEGER ("IntegerOps::mul(const Integer&, const Integer&)", srcA, LOC_BEFORE);
  VALIDATE_INTEGER ("IntegerOps::mul(const Integer&, const Integer&)", srcB, LOC_BEFORE);
#ifdef DEBUG_MODE
  if (!(srcA.m_size == m_size && srcB.m_size == m_size)) {
    PRINT_MESSAGE_AND_EXIT ("[IntegerOps::mul(const Integer&, const Integer&)] The two arguments `srcA' and `srcB' need to be of size %d.\n", m_size);
  }
#endif

  /* TODO: IMPLEMENT */


  VALIDATE_INTEGER ("IntegerOps::mul(const Integer&, const Integer&)", *m_mulResult, LOC_AFTER);
  return *m_mulResult;
}

bool IntegerOps::sub (Integer& dst, const Integer& src) {
  VALIDATE_INTEGER ("IntegerOps::sub(Integer&, const Integer&)", dst, LOC_BEFORE);
  VALIDATE_INTEGER ("IntegerOps::sub(Integer&, const Integer&)", src, LOC_BEFORE);
#ifdef DEBUG_MODE
  if (!(dst.m_size == m_size && src.m_size == m_size)) {
    PRINT_MESSAGE_AND_EXIT ("[IntegerOps::sub(Integer&, const Integer&)] The two arguments `dst' and `src' need to be of size %d.\n", m_size);
  }
#endif

  bool carry;
  if (src.m_max > 0) {
    if (dst.m_max > 0) {
      if (dst.m_sign ^ src.m_sign)
        carry = dst.absAdd (src);
      else
        carry = dst.absSub (src);
    } else {
      dst = src;
      dst.m_sign = !src.m_sign;
      carry = false;
    }
  } else {
    carry = false;
  }

  VALIDATE_INTEGER ("IntegerOps::sub(Integer&, const Integer&)", dst, LOC_AFTER);
  return carry;
}

bool IntegerOps::subtractFromRemainder (const Integer& denominator, int denomBsr, int remainderBsr) {
  /* TODO: IMPLEMENT */
}
