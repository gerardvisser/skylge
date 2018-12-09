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
#include <string.h>
#include <skylge/math/Integer.h>
#include "defs.h"
#include "errors.h"

#define MAX_SIZE 16384
#define MIN_SIZE 2

/* Do we need:
bool Integer::operator== (const Integer& other) const;
bool Integer::operator!= (const Integer& other) const;
bool Integer::operator< (const Integer& other) const;
bool Integer::operator<= (const Integer& other) const;
bool Integer::operator> (const Integer& other) const;
bool Integer::operator>= (const Integer& other) const;
*/

Integer::Integer (int size) : m_size (size), m_max (0), m_sign (false) {
#ifdef DEBUG_MODE
  if (size < MIN_SIZE || size > MAX_SIZE) {
    PRINT_MESSAGE_AND_EXIT ("[Integer::Integer(int)] The specified size (%d) is out of range. It should be: %d < size < %d.\n", size, MIN_SIZE - 1, MAX_SIZE + 1);
  }
#endif

  size_t bsize = size << 3;
  m_buf = (uint64_t*) malloc (bsize);
  memset (m_buf, 0, bsize);
}

Integer::Integer (const Integer& other) {
  copy (other);
}

Integer::Integer (Integer&& other) {
  move (other);
}

Integer::~Integer (void) {
  if (m_buf != NULL)
    free (m_buf);
}

Integer& Integer::operator= (const Integer& other) {
  if (this == &other)
    return *this;
  free (m_buf); /* Niet altijd nodig... */
  copy (other);
  return *this;
}

Integer& Integer::operator= (Integer&& other) {
  if (this == &other)
    return *this;
  free (m_buf);
  move (other);
  return *this;
}

Integer& Integer::operator= (int64_t val) {
  VALIDATE_INTEGER ("Integer::operator=(int64_t)", *this, LOC_BEFORE);

  m_sign = val < 0;
  if (m_sign)
    val = -val;

#ifdef DEBUG_MODE
  if (val >> CAL_B * m_size > 0) {
    PRINT_MESSAGE_AND_EXIT ("[Integer::operator=(int64_t)] The magnitude of argument `val' is too large.\n");
  }
#endif

  int i = 0;
  while (val != 0) {
    m_buf[i] = val & CAL_LMASK[0];
    val >>= CAL_B;
    ++i;
  }
  if (i < m_max) {
    memset (m_buf + i, 0, m_max - i << 3);
  }
  m_max = i;

  VALIDATE_INTEGER ("Integer::operator=(int64_t)", *this, LOC_AFTER);
  return *this;
}

Integer::operator int64_t () {
  VALIDATE_INTEGER ("Integer::operator int64_t()", *this, LOC_BEFORE);

  int64_t result;
  if (bsr () < 64) {
    result = 0;
    for (int i = m_max - 1; i > -1; --i) {
      result <<= CAL_B;
      result |= (int64_t) m_buf[i];
    }
    if (m_sign)
      result = -result;
  } else {
    result = 0x8000000000000000;
  }
  return result;
}

Integer::operator int () {
  VALIDATE_INTEGER ("Integer::operator int()", *this, LOC_BEFORE);

  int result;

#if CAL_B == 32

  if (m_max < 2 && (m_buf[0] & CAL_SMASK[31]) == 0) {
    result = (int) m_buf[0];
    if (m_sign)
      result = -result;
  } else {
    result = 0x80000000;
  }

#else

  if (bsr () < 32) {
    result = 0;
    for (int i = m_max - 1; i > -1; --i) {
      result <<= CAL_B;
      result |= (int) m_buf[i];
    }
    if (m_sign)
      result = -result;
  } else {
    result = 0x80000000;
  }

#endif

  return result;
}


bool Integer::absAdd (const Integer& other) {
  /* TODO: IMPLEMENT */
}

bool Integer::absDec (void) {
  /* TODO: IMPLEMENT */
}

bool Integer::absInc (void) {
  /* TODO: IMPLEMENT */
}

bool Integer::absSub (const Integer& other) {
  VALIDATE_INTEGER ("Integer::absSub(const Integer&)", *this, LOC_BEFORE);
  VALIDATE_INTEGER ("Integer::absSub(const Integer&)", other, LOC_BEFORE);


  /* TODO: IMPLEMENT */


  VALIDATE_INTEGER ("Integer::absSub(const Integer&)", *this, LOC_AFTER);
  return false;
}

int Integer::bsf (void) const {
  VALIDATE_INTEGER ("Integer::bsf(void)", *this, LOC_BEFORE);

  int result = 0;
  if (m_max > 0) {
    int i = 0;
    uint64_t mask = 1;
    while (m_buf[i] == 0) {
      ++i;
    }
    while ((m_buf[i] & mask) == 0) {
      mask <<= 1;
      ++result;
    }
    result += CAL_B * i;
  }
  return result;
}

int Integer::bsr (void) const {
  VALIDATE_INTEGER ("Integer::bsr(void)", *this, LOC_BEFORE);

  int result;
  if (m_max > 0) {
    int m = m_max - 1;
    uint64_t mask = CAL_SMASK[CAL_B - 1];
    result = CAL_B * m_max;
    while ((m_buf[m] & mask) == 0) {
      mask >>= 1;
      --result;
    }
  } else {
    result = 0;
  }
  return result;
}

bool Integer::getBit (int bitNo) const {
  /* TODO: IMPLEMENT */
}

void Integer::lshl (Integer& incomingBits, int x) {
  /* TODO: IMPLEMENT */
}

void Integer::rcl (bool carry) {
  /* TODO: IMPLEMENT */
}

void Integer::setMax (int fromIndex) {
  while (fromIndex > -1 && m_buf[fromIndex] == 0) {
    --fromIndex;
  }
  m_max = fromIndex + 1;

  VALIDATE_INTEGER ("Integer::setMax(int)", *this, LOC_AFTER);
}

void Integer::shl (int x) {
  /* TODO: IMPLEMENT */
}

void Integer::shr (int x) {
  /* TODO: IMPLEMENT */
}

bool Integer::sign (void) const {
  return m_sign;
}


void Integer::copy (const Integer& other) {
  m_size = other.m_size;
  m_sign = other.m_sign;
  m_max = other.m_max;
  size_t bsize = m_size << 3;
  m_buf = (uint64_t*) malloc (bsize);
  memcpy (m_buf, other.m_buf, bsize);
}

void Integer::move (Integer& other) {
  m_size = other.m_size;
  m_sign = other.m_sign;
  m_max = other.m_max;
  m_buf = other.m_buf;

  other.m_buf = NULL;
}


#ifdef DEBUG_MODE

const uint64_t* Integer::buf (void) const {
  return m_buf;
}

int Integer::max (void) const {
  return m_max;
}

int Integer::size (void) const {
  return m_size;
}

#endif
