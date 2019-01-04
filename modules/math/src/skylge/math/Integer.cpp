/*
   Author:  Gerard Visser
   e-mail:  visser.gerard(at)gmail.com

   Copyright (C) 2018, 2019 Gerard Visser.

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
  VALIDATE_INTEGER ("Integer::operator=(const Integer&)", *this, LOC_BEFORE);
  VALIDATE_INTEGER ("Integer::operator=(const Integer&)", other, LOC_BEFORE);

  if (this == &other)
    return *this;
  if (other.m_max <= m_size) {
    copyUsingExistingBuffer (other);
  } else {
    free (m_buf);
    copy (other);
  }

  VALIDATE_INTEGER ("Integer::operator=(const Integer&)", *this, LOC_AFTER);
  return *this;
}

Integer& Integer::operator= (Integer&& other) {
  VALIDATE_INTEGER ("Integer::operator=(Integer&&)", *this, LOC_BEFORE);
  VALIDATE_INTEGER ("Integer::operator=(Integer&&)", other, LOC_BEFORE);

  if (this == &other)
    return *this;
  if (other.m_max <= m_size) {
    copyUsingExistingBuffer (other);
  } else {
    free (m_buf);
    move (other);
  }

  VALIDATE_INTEGER ("Integer::operator=(Integer&&)", *this, LOC_AFTER);
  return *this;
}

Integer& Integer::operator= (int64_t value) {
  VALIDATE_INTEGER ("Integer::operator=(int64_t)", *this, LOC_BEFORE);

  m_sign = value < 0;
  uint64_t val = m_sign ? -value : value;

#ifdef DEBUG_MODE
  if (CAL_B * m_size < 64 && val >> CAL_B * m_size != 0) {
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

Integer::operator int64_t () const {
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

Integer::operator int () const {
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

bool Integer::operator== (const Integer& other) const {
  VALIDATE_INTEGER ("Integer::operator==(const Integer&)", *this, LOC_BEFORE);
  VALIDATE_INTEGER ("Integer::operator==(const Integer&)", other, LOC_BEFORE);

  if (!(m_sign == other.m_sign && m_max == other.m_max))
    return false;
  for (int i = 0; i < m_max; ++i) {
    if (m_buf[i] != other.m_buf[i])
      return false;
  }
  return true;
}

bool Integer::operator!= (const Integer& other) const {
  return !(*this == other);
}


bool Integer::absAdd (const Integer& other) {
  VALIDATE_INTEGER ("Integer::absAdd(const Integer&)", *this, LOC_BEFORE);
  VALIDATE_INTEGER ("Integer::absAdd(const Integer&)", other, LOC_BEFORE);
#ifdef DEBUG_MODE
  if (m_size != other.m_size) {
    PRINT_MESSAGE_AND_EXIT ("[Integer::absAdd(const Integer&)] m_size should be equal to other.m_size.\n");
  }
#endif

  bool carry = false;
  bool otherMaxGreaterThanThisMax = other.m_max > m_max;
  int max = otherMaxGreaterThanThisMax ? m_max : other.m_max;
  int i;

  for (i = 0; i < max; ++i) {
    if (carry) {
      CAL_CLEAR_CARRY (m_buf[i - 1]);
      ++m_buf[i];
    }
    m_buf[i] += other.m_buf[i];
    carry = CAL_CARRY (m_buf[i]);
  }

  if (otherMaxGreaterThanThisMax) {

    if (carry) {
      CAL_CLEAR_CARRY (m_buf[i - 1]);
      while (i < other.m_max && other.m_buf[i] == CAL_LMASK[0]) {
        ++i;
      }
      carry = i == m_size;
      if (carry) {
        setMax (m_max - 1);
      } else {
        m_buf[i] = other.m_buf[i] + 1;
        ++i;
        while (i < other.m_max) {
          m_buf[i] = other.m_buf[i];
          ++i;
        }
        m_max = i;
      }
    } else {
      while (i < other.m_max) {
        m_buf[i] = other.m_buf[i];
        ++i;
      }
      m_max = other.m_max;
    }

  } else if (carry) {

    CAL_CLEAR_CARRY (m_buf[i - 1]);
    while (i < m_max && m_buf[i] == CAL_LMASK[0]) {
      m_buf[i] = 0;
      ++i;
    }
    carry = i == m_size;
    if (carry) {
      setMax (other.m_max - 1);
    } else {
      ++m_buf[i];
      if (i == m_max)
        ++m_max;
    }

  }

  VALIDATE_INTEGER ("Integer::absAdd(const Integer&)", *this, LOC_AFTER);
  return carry;
}

bool Integer::absAdd (uint64_t value) {
  VALIDATE_INTEGER ("Integer::absAdd(uint64_t)", *this, LOC_BEFORE);
#ifdef DEBUG_MODE
  if (value > CAL_LMASK[0]) {
    PRINT_MESSAGE_AND_EXIT ("[Integer::absAdd(uint64_t)] The specified value should be less than or equal to %ld (0x%lX).\n", CAL_LMASK[0], CAL_LMASK[0]);
  }
  if (m_max == 0) {
    PRINT_MESSAGE_AND_EXIT ("[Integer::absAdd(uint64_t)] *this should not represent 0.\n");
  }
#endif

  m_buf[0] += value;
  bool carry = CAL_CARRY (m_buf[0]);
  if (carry) {
    int i = 1;
    CAL_CLEAR_CARRY (m_buf[0]);
    while (i < m_max && m_buf[i] == CAL_LMASK[0]) {
      m_buf[i] = 0;
      ++i;
    }
    carry = i == m_size;
    if (carry) {
      m_max = m_buf[0] == 0 ? 0 : 1;
    } else {
      ++m_buf[i];
      if (i == m_max)
        ++m_max;
    }
  }

  VALIDATE_INTEGER ("Integer::absAdd(uint64_t)", *this, LOC_AFTER);
  return carry;
}

bool Integer::absDec (void) {
  VALIDATE_INTEGER ("Integer::absDec(void)", *this, LOC_BEFORE);
#ifdef DEBUG_MODE
  if (m_max == 0) {
    PRINT_MESSAGE_AND_EXIT ("[Integer::absDec(void)] This function may never be called when m_max is 0.\n");
  }
#endif

  int i = 0;
  while (m_buf[i] == 0) {
    m_buf[i] = CAL_LMASK[0];
    ++i;
  }
  --m_buf[i];
  if (m_buf[i] == 0 && i == m_max - 1) {
    --m_max;
    if (m_max == 0)
      m_sign = false;
  }

  VALIDATE_INTEGER ("Integer::absDec(void)", *this, LOC_AFTER);
  return false;
}

bool Integer::absInc (void) {
  VALIDATE_INTEGER ("Integer::absInc(void)", *this, LOC_BEFORE);

  int i = 0;
  while (i < m_size && m_buf[i] == CAL_LMASK[0]) {
    m_buf[i] = 0;
    ++i;
  }
  bool carry = i == m_size;
  if (carry) {
    m_max = 0;
  } else {
    ++m_buf[i];
    if (i == m_max)
      ++m_max;
  }

  VALIDATE_INTEGER ("Integer::absInc(void)", *this, LOC_AFTER);
  return carry;
}

bool Integer::absSub (const Integer& other) {
  VALIDATE_INTEGER ("Integer::absSub(const Integer&)", *this, LOC_BEFORE);
  VALIDATE_INTEGER ("Integer::absSub(const Integer&)", other, LOC_BEFORE);
#ifdef DEBUG_MODE
  if (m_size != other.m_size) {
    PRINT_MESSAGE_AND_EXIT ("[Integer::absSub(const Integer&)] m_size should be equal to other.m_size.\n");
  }
#endif

  int i;
  bool carry = false;

  if (m_max > other.m_max) {

    for (i = 0; i < other.m_max; ++i) {
      if (carry) {
        CAL_CLEAR_CARRY (m_buf[i - 1]);
        --m_buf[i];
      }
      m_buf[i] -= other.m_buf[i];
      carry = CAL_CARRY (m_buf[i]);
    }
    if (carry) {
      CAL_CLEAR_CARRY (m_buf[i - 1]);
      while (m_buf[i] == 0) {
        m_buf[i] = CAL_LMASK[0];
        ++i;
      }
      --m_buf[i];
      if (i == m_max - 1 && m_buf[i] == 0)
        setMax (i - 1);
    }

  } else if (m_max == other.m_max) {

    for (i = 0; i < m_max; ++i) {
      if (carry) {
        CAL_CLEAR_CARRY (m_buf[i - 1]);
        --m_buf[i];
      }
      m_buf[i] -= other.m_buf[i];
      carry = CAL_CARRY (m_buf[i]);
    }
    if (carry) {
      i = 0;
      m_sign = !m_sign;
      while (m_buf[i] == 0) {
        ++i;
      }
      m_buf[i] ^= CAL_LMASK[0];
      CAL_CLEAR_CARRY (m_buf[i]);
      ++m_buf[i];
      ++i;
      while (i < m_max) {
        m_buf[i] ^= CAL_LMASK[0];
        CAL_CLEAR_CARRY (m_buf[i]);
        ++i;
      }
    }
    setMax (m_max - 1);
    if (m_max == 0)
      m_sign = false;

  } else { /* m_max < other.m_max */

    m_sign = !m_sign;
    for (i = 0; i < m_max; ++i) {
      if (carry) {
        CAL_CLEAR_CARRY (m_buf[i - 1]);
        ++m_buf[i];
      }
      m_buf[i] = other.m_buf[i] - m_buf[i];
      carry = CAL_CARRY (m_buf[i]);
    }
    m_max = other.m_max;
    if (carry) {
      CAL_CLEAR_CARRY (m_buf[i - 1]);
      while (other.m_buf[i] == 0) {
        m_buf[i] = CAL_LMASK[0];
        ++i;
      }
      m_buf[i] = other.m_buf[i] - 1;
      if (i == other.m_max - 1) {
        if (m_buf[i] == 0)
          setMax (i - 1);
      } else {
        ++i;
        while (i < other.m_max) {
          m_buf[i] = other.m_buf[i];
          ++i;
        }
      }
    } else {
      while (i < other.m_max) {
        m_buf[i] = other.m_buf[i];
        ++i;
      }
    }

  }

  VALIDATE_INTEGER ("Integer::absSub(const Integer&)", *this, LOC_AFTER);
  return false;
}

bool Integer::absSub (uint64_t value) {
  VALIDATE_INTEGER ("Integer::absSub(uint64_t)", *this, LOC_BEFORE);
#ifdef DEBUG_MODE
  if (value > CAL_LMASK[0]) {
    PRINT_MESSAGE_AND_EXIT ("[Integer::absSub(uint64_t)] The specified value should be less than or equal to %ld (0x%lX).\n", CAL_LMASK[0], CAL_LMASK[0]);
  }
  if (m_max == 0) {
    PRINT_MESSAGE_AND_EXIT ("[Integer::absSub(uint64_t)] *this should not represent 0.\n");
  }
#endif

  m_buf[0] -= value;
  bool carry = CAL_CARRY (m_buf[0]);
  if (m_max > 1) {

    if (carry) {
      int i = 1;
      CAL_CLEAR_CARRY (m_buf[0]);
      while (m_buf[i] == 0) {
        m_buf[i] = CAL_LMASK[0];
        ++i;
      }
      --m_buf[i];
      if (i == m_max - 1 && m_buf[i] == 0)
        --m_max;
    }

  } else { /* m_max == 1 */

    if (carry) {
      m_sign = !m_sign;
      m_buf[0] ^= CAL_LMASK[0];
      CAL_CLEAR_CARRY (m_buf[0]);
      ++m_buf[0];
    } else if (m_buf[0] == 0) {
      m_max = 0;
      m_sign = false;
    }

  }

  VALIDATE_INTEGER ("Integer::absSub(uint64_t)", *this, LOC_AFTER);
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
  VALIDATE_INTEGER ("Integer::getBit(int)", *this, LOC_BEFORE);

  int r = CAL_R (bitNo);
  int q = CAL_Q (bitNo);

#ifdef DEBUG_MODE
  if (bitNo < 0 || q >= m_size) {
    PRINT_MESSAGE_AND_EXIT ("[Integer::getBit(int)] Argument 'bitNo' out of bounds.\n");
  }
#endif

  return (m_buf[q] & CAL_SMASK[r]) != 0;
}

void Integer::lshl (Integer& incomingBits, int x) {
  VALIDATE_INTEGER ("Integer::lshl(Integer&, int)", *this, LOC_BEFORE);
  VALIDATE_INTEGER ("Integer::lshl(Integer&, int)", incomingBits, LOC_BEFORE);
#ifdef DEBUG_MODE
  if (m_size != incomingBits.m_size) {
    PRINT_MESSAGE_AND_EXIT ("[Integer::lshl(Integer&, int)] m_size should be equal to incomingBits.m_size.\n");
  }
#endif

  if (x > 0) {
    const int n = m_size - 1;
    const int q = CAL_Q (x);
    const int r = CAL_R (x);
    int zh;

    if (r > 0) {

      if (m_max > n - q)
        zh = n;
      else
        zh = q + m_max;
      const int k = CAL_B - r;
      int ih;

      if (m_max > 0) {
        for (ih = zh; ih > q; --ih) {
          m_buf[ih] = m_buf[ih - q] << r;
          m_buf[ih] |= m_buf[ih - q - 1] >> k;
          CAL_CLEAR_CARRY (m_buf[ih]);
        }
        m_buf[q] = m_buf[0] << r;
        CAL_CLEAR_CARRY (m_buf[q]);
        --ih;
      } else {
        ih = -1;
      }

      if (incomingBits.m_max > 0) {

        int il = n;
        m_buf[q] |= incomingBits.m_buf[n] >> k;
        for (ih = q - 1; ih > -1; --ih) {
          m_buf[ih] = incomingBits.m_buf[il] << r;
          --il;
          m_buf[ih] |= incomingBits.m_buf[il] >> k;
          CAL_CLEAR_CARRY (m_buf[ih]);
        }

        int zl;
        if (incomingBits.m_max > n - q)
          zl = n;
        else
          zl = q + incomingBits.m_max;
        for (il = zl; il > q; --il) {
          incomingBits.m_buf[il] = incomingBits.m_buf[il - q] << r;
          incomingBits.m_buf[il] |= incomingBits.m_buf[il - q - 1] >> k;
          CAL_CLEAR_CARRY (incomingBits.m_buf[il]);
        }
        incomingBits.m_buf[q] = incomingBits.m_buf[0] << r;
        CAL_CLEAR_CARRY (incomingBits.m_buf[q]);
        --il;

        while (il > -1) {
          incomingBits.m_buf[il] = 0;
          --il;
        }
        incomingBits.setMax (zl);

      } else { /* incomingBits.m_max == 0 */

        while (ih > -1) {
          m_buf[ih] = 0;
          --ih;
        }

      }

    } else { /* r == 0 */

      int ih;
      if (m_max > 0) {
        if (m_max > n - q)
          zh = n;
        else
          zh = q + m_max - 1;
        for (ih = zh; ih >= q; --ih) {
          m_buf[ih] = m_buf[ih - q];
        }
      } else {
        ih = -1;
        zh = q - 1;
      }

      if (incomingBits.m_max > 0) {

        int il = n;
        for (ih = q - 1; ih > -1; --ih) {
          m_buf[ih] = incomingBits.m_buf[il];
          --il;
        }

        int zl;
        if (incomingBits.m_max > n - q)
          zl = n;
        else
          zl = q + incomingBits.m_max - 1;
        for (il = zl; il >= q; --il) {
          incomingBits.m_buf[il] = incomingBits.m_buf[il - q];
        }

        while (il > -1) {
          incomingBits.m_buf[il] = 0;
          --il;
        }
        incomingBits.setMax (zl);

      } else { /* incomingBits.m_max == 0 */

        while (ih > -1) {
          m_buf[ih] = 0;
          --ih;
        }

      }

    }
    setMax (zh);
  }

  VALIDATE_INTEGER ("Integer::lshl(Integer&, int)", incomingBits, LOC_AFTER);
  VALIDATE_INTEGER ("Integer::lshl(Integer&, int)", *this, LOC_AFTER);
}

void Integer::rcl (bool carry) {
  VALIDATE_INTEGER_LAST_BIT_0 ("Integer::rcl(bool)", *this, LOC_BEFORE);

  if (m_max > 0) {

    const int m = m_max - 1;
    if ((m_buf[m] & CAL_SMASK[CAL_B - 1]) != 0) {
      m_buf[m_max] = 1;
      ++m_max;
    }
    for (int i = m; i > 0; --i) {
      m_buf[i] <<= 1;
      CAL_CLEAR_CARRY (m_buf[i]);
      m_buf[i] |= (m_buf[i - 1] & CAL_SMASK[CAL_B - 1]) != 0;
    }
    m_buf[0] <<= 1;
    CAL_CLEAR_CARRY (m_buf[0]);
    m_buf[0] |= carry;

  } else {
    if (carry) {
      m_buf[0] = 1;
      m_max = 1;
    }
  }

  VALIDATE_INTEGER ("Integer::rcl(bool)", *this, LOC_AFTER);
}

void Integer::setMax (int fromIndex) {
  while (fromIndex > -1 && m_buf[fromIndex] == 0) {
    --fromIndex;
  }
  m_max = fromIndex + 1;

  VALIDATE_INTEGER ("Integer::setMax(int)", *this, LOC_AFTER);
}

void Integer::shl (int x) {
  VALIDATE_INTEGER ("Integer::shl(int)", *this, LOC_BEFORE);

  if (m_max > 0 && x > 0) {
    const int n = m_size - 1;
    const int q = CAL_Q (x);
    const int r = CAL_R (x);
    int i, z;

    if (r > 0) {

      if (m_max > n - q)
        z = n;
      else
        z = q + m_max;
      const int k = CAL_B - r;
      for (i = z; i > q; --i) {
        m_buf[i] = m_buf[i - q] << r;
        m_buf[i] |= m_buf[i - q - 1] >> k;
        CAL_CLEAR_CARRY (m_buf[i]);
      }
      m_buf[q] = m_buf[0] << r;
      CAL_CLEAR_CARRY (m_buf[q]);
      --i;

    } else {

      if (m_max > n - q)
        z = n;
      else
        z = q + m_max - 1;
      for (i = z; i >= q; --i) {
        m_buf[i] = m_buf[i - q];
      }

    }

    while (i > -1) {
      m_buf[i] = 0;
      --i;
    }
    setMax (z);
  }

  VALIDATE_INTEGER ("Integer::shl(int)", *this, LOC_AFTER);
}

void Integer::shr (int x) {
  VALIDATE_INTEGER ("Integer::shr(int)", *this, LOC_BEFORE);

  if (m_max > 0 && x > 0) {
    const int q = CAL_Q (x);
    const int r = CAL_R (x);
    int z = m_max - q;

    if (z > 0) {

      int i;
      const int oldMax = m_max;
      if (r > 0) {

        --z;
        const int k = CAL_B - r;
        for (i = 0; i < z; ++i) {
          m_buf[i] = m_buf[i + q] >> r;
          m_buf[i] |= m_buf[i + q + 1] << k;
          CAL_CLEAR_CARRY (m_buf[i]);
        }
        m_buf[z] = m_buf[z + q] >> r;
        m_max = m_buf[z] > 0 ? z + 1 : z;
        ++i;

      } else {

        for (i = 0; i < z; ++i) {
          m_buf[i] = m_buf[i + q];
        }
        m_max = z;

      }
      while (i < oldMax) {
        m_buf[i] = 0;
        ++i;
      }

    } else {
      memset (m_buf, 0, m_max << 3);
      m_max = 0;
    }
  }

  VALIDATE_INTEGER ("Integer::shr(int)", *this, LOC_AFTER);
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

void Integer::copyUsingExistingBuffer (const Integer& other) {
  memcpy (m_buf, other.m_buf, other.m_max << 3);
  if (other.m_max < m_max) {
    memset (m_buf + other.m_max, 0, m_max - other.m_max << 3);
  }
  m_sign = other.m_sign;
  m_max = other.m_max;
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
