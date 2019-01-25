/*
   Author:  Gerard Visser
   e-mail:  visser.gerard(at)gmail.com

   Copyright (C) 2019 Gerard Visser.

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

#include <stdexcept>
#include <skylge/math/Real.h>
#include "defs.h"
#include "ieeeDouble.h"
#include "utils.h"

Real::Real (int size) {
  m_exponent = new Integer (size);
  m_number = new Integer (size);
}

Real::Real (const Real& other) {
  copy (other);
}

Real::Real (Real&& other) {
  move (other);
}

Real::~Real (void) {
  if (m_exponent != nullptr)
    delete m_exponent;
  if (m_number != nullptr)
    delete m_number;
}

Real& Real::operator= (const Real& other) {
  if (this == &other)
    return *this;
  if (m_exponent->sizeInBits () == other.m_exponent->sizeInBits ()) {
    *m_exponent = *other.m_exponent;
    *m_number = *other.m_number;
  } else {
    delete m_exponent;
    delete m_number;
    copy (other);
  }
  return *this;
}

Real& Real::operator= (Real&& other) {
  if (this == &other)
    return *this;
  delete m_exponent;
  delete m_number;
  move (other);
  return *this;
}

Real& Real::operator= (double val) {
  if (val != 0) {
    union ieee754_double value;
    value.d = val;
    int exponent = value.ieee.exponent;
    if (exponent != 0x7FF) {
      int64_t number;
      if (exponent != 0) {
        number = 1 << 20 | value.ieee.mantissa0;
      } else {
        number = value.ieee.mantissa0;
        ++exponent;
      }
      number <<= 32;
      number |= value.ieee.mantissa1;

      int bsfVal = Utils::bsf (number);
      number >>= bsfVal;
      exponent += bsfVal;

#if CAL_B == 6
      int excess = Utils::bsr (number) - m_number->sizeInBits ();
      if (excess > 0) {
        bool round = Utils::getBit (number, excess - 1);
        number >>= excess;
        exponent += excess;
        if (round)
          ++number;
        bsfVal = Utils::bsf (number);
        number >>= bsfVal;
        exponent += bsfVal;
      }
#endif

      if (value.ieee.negative)
        number = -number;
      *m_number = number;
      *m_exponent = exponent - DOUBLE_EXPONENT_BIAS;
    } else if (value.ieee.mantissa0 == 0 && value.ieee.mantissa1 == 0) {
      makeInfinite (value.ieee.negative);
    } else {
      throw std::invalid_argument ("Cannot set NaN.");
    }
  } else {
    *m_exponent = 0;
    *m_number = 0;
  }
  return *this;
}

Real::operator double () const {
  if (m_number->isZero ()) {
    if (isInfinite ()) {
      if (sign ())
        return -DOUBLE_INFINITY;
      return DOUBLE_INFINITY;
    }
    return 0;
  }

  const int bsrVal = m_number->bsr ();
  int expDelta = bsrVal - DOUBLE_SIGNIFICAND_WIDTH;
  int exponent = (int) *m_exponent + expDelta + DOUBLE_FRACTIONAL_WIDTH;

  if (exponent < DOUBLE_MIN_EXPONENT || exponent > DOUBLE_MAX_EXPONENT) {
    if (m_exponent->sign ())
      return 0;
    if (sign ())
      return -DOUBLE_INFINITY;
    return DOUBLE_INFINITY;
  }

  int64_t significand;
  if (bsrVal > 62) {
    Integer temp (CAL_Q (m_number->sizeInBits ()));
    temp = *m_number;
    bool round = temp.getBit (expDelta - 1);
    temp.shr (expDelta);
    significand = temp;
    if (significand < 0)
      significand = -significand;
    if (round) {
      ++significand;
      if (Utils::bsr (significand) == DOUBLE_SIGNIFICAND_WIDTH + 1) {
        significand >>= 1;
        ++exponent;
      }
    }
  } else {
    significand = *m_number;
    if (significand < 0)
      significand = -significand;
    if (expDelta > 0) {
      bool round = Utils::getBit (significand, expDelta - 1);
      significand >>= expDelta;
      if (round) {
        ++significand;
        if (Utils::bsr (significand) == DOUBLE_SIGNIFICAND_WIDTH + 1) {
          significand >>= 1;
          ++exponent;
        }
      }
    } else {
      significand <<= -expDelta;
    }
  }

  if (exponent < DOUBLE_MIN_EXPONENT_N) {
    expDelta = DOUBLE_MIN_EXPONENT_N - exponent;
    significand >>= expDelta;
    exponent = -IEEE754_DOUBLE_BIAS;
  } else {
    significand &= DOUBLE_FRACTION_MASK;
  }

  union ieee754_double result;

  result.ieee.mantissa0 = significand >> 32;
  result.ieee.mantissa1 = significand & 0xFFFFFFFF;
  result.ieee.exponent = exponent + IEEE754_DOUBLE_BIAS;
  result.ieee.negative = sign ();

  return result.d;
}

bool Real::operator== (const Real& other) const {
  return *this->m_number == *other.m_number && *this->m_exponent == *other.m_exponent;
}

bool Real::operator!= (const Real& other) const {
  return !(*this == other);
}

bool Real::isInfinite (void) const {
  return m_number->isZero () && !m_exponent->isZero ();
}

void Real::makeInfinite (bool sign) {
  *m_number = 0;
  *m_exponent = 1;
  m_number->setSign (sign);
}

bool Real::sign (void) const {
  return m_number->sign ();
}

void Real::copy (const Real& other) {
  m_exponent = new Integer (*other.m_exponent);
  m_number = new Integer (*other.m_number);
}

void Real::move (Real& other) {
  m_exponent = other.m_exponent;
  m_number = other.m_number;
  other.m_exponent = nullptr;
  other.m_number = nullptr;
}


#ifdef DEBUG_MODE

Integer Real::exponent (void) const {
  return *m_exponent;
}

Integer Real::number (void) const {
  return *m_number;
}

#endif
