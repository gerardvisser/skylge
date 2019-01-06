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

#include <ieee754.h>
#include <skylge/math/Real.h>

#define DOUBLE_EXPONENT_BIAS     (IEEE754_DOUBLE_BIAS + DOUBLE_FRACTIONAL_WIDTH)
#define DOUBLE_FRACTIONAL_WIDTH  52
#define DOUBLE_SIGNIFICAND_WIDTH (DOUBLE_FRACTIONAL_WIDTH + 1)

/* TODO: Deze naar elders verplaatsen. */
static int bsf (int64_t value) {
  if (value != 0) {
    int64_t mask = 1;
    int result = 0;
    while ((value & mask) == 0) {
      mask <<= 1;
      ++result;
    }
    return result;
  } else {
    return 0;
  }
}

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
}

Real& Real::operator= (Real&& other) {
  if (this == &other)
    return *this;
  delete m_exponent;
  delete m_number;
  move (other);
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

      int bsfVal = bsf (number);
      number >>= bsfVal;
      exponent += bsfVal;

      /* TODO: Als CAL_B=6, dan moeten we wellicht nog meer wegschuiven.  */

      if (value.ieee.negative)
        number = -number;
      *m_number = number;
      *m_exponent = exponent - DOUBLE_EXPONENT_BIAS;
    } else if (value.ieee.mantissa0 == 0 && value.ieee.mantissa1 == 0) {
      makeInfinite (value.ieee.negative);
    } else {
      /* Not a Number: what TODO */
    }
  } else {
    *m_exponent = 0;
    *m_number = 0;
  }
  return *this;
}

Real::operator double () const {
  /* TODO: IMPLEMENT */
  return 0.0;
}

bool Real::operator== (const Real& other) const {
  /* TODO: IMPLEMENT */
  return false;
}

bool Real::operator!= (const Real& other) const {
  /* TODO: IMPLEMENT */
  return false;
}

bool Real::isInfinite (void) const {
  /* TODO: IMPLEMENT */
  return false;
}

void Real::makeInfinite (bool sign) {
  /* TODO: IMPLEMENT */
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


/* Temporary??? */

Integer Real::exponent (void) {
  return *m_exponent;
}

Integer Real::number (void) {
  return *m_number;
}
