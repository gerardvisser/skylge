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
#include <stdexcept>
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

/* Modifies: m_aux.  */
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

void IntegerOps::baseDiv (Integer& result, const Integer& denominator, const int denomBsr, int total) {
  while (total > 0) {
    const int remainderBsr = m_remainder->bsr ();
    if (remainderBsr == 0) {
      int i = m_bsize - m_numerator->bsr ();
      if (i > total)
        i = total;
      result.lshl (*m_numerator, i);
      total -= i;
      /* m_numerator's most significant bit set or no more bits left (total=0).  */
    }
    if (total > 0) {
      int i = remainderBsr < denomBsr ? denomBsr - remainderBsr : 1;
      if (i <= total) {
        result.shl (i - 1);
        m_remainder->lshl (*m_numerator, i);
        bool bit = subtractFromRemainder (denominator, denomBsr, remainderBsr + i);
        result.rcl (bit);
        total -= i;
      } else {
        result.shl (total);
        m_remainder->lshl (*m_numerator, total);
        total = 0;
      }
    }
  }
}

/* TODO: Ook testen in CAL_B=32 conditie.  */
void IntegerOps::baseMul (const Integer& srcA, const Integer& srcB) {
  for (int i = 0; i < srcA.m_max; ++i) {
    m_mulResult->m_buf[i] += srcA.m_buf[i] * srcB.m_buf[0];
    m_mulResult->m_buf[i + 1] = m_mulResult->m_buf[i] >> CAL_B;
    CAL_CLEAR_CARRY (m_mulResult->m_buf[i]);
  }

  int k = srcA.m_max;
  for (int i = 1; i < srcB.m_max; ++i) {
    if (srcB.m_buf[i] > 0) {
      k = i;
      uint64_t high, low = 0;
      for (int j = 0; j < srcA.m_max; ++j) {
        low += srcA.m_buf[j] * srcB.m_buf[i];
        high = low >> CAL_B;
        CAL_CLEAR_CARRY (low);
        m_mulResult->m_buf[k] += low;
        if (CAL_CARRY (m_mulResult->m_buf[k])) {
          CAL_CLEAR_CARRY (m_mulResult->m_buf[k]);
          ++m_mulResult->m_buf[k + 1];
        }
        low = high;
        ++k;
      }
      m_mulResult->m_buf[k] += low;
    }
  }
  m_mulResult->m_sign = srcA.m_sign ^ srcB.m_sign;
  m_mulResult->setMax (k);
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

/*
TODO:
Er is er een probleem als we gaan delen met reële getallen. Als de meest
significante bit van de noemer is gezet en de teller heeft ook de meest signifi-
cante bit gezet, maar is toch kleiner, dan verdwijnt hier dat getal in de rest
en is de berekening klaar. Bij reële getallen, echter, delen we dan door, d.w.z.
er schuift een nul het resultaat in en de remainder wordt één naar links gescho-
ven: de meest significante bit valt er in de huidige situatie dus vanaf en we
verliezen informatie als dat een 1 was....
Daarvoor moet de grootte van m_remainder met 1 worden verhoogd, maar Integer:lshl
accepteert dat nu niet: dat moet dus worden aangepast.
 */
/* Modifies: m_aux, m_remainder, m_numerator.  */
Integer& IntegerOps::div (Integer& dst, const Integer& src) {
  VALIDATE_INTEGER ("IntegerOps::div(Integer&, const Integer&)", dst, LOC_BEFORE);
  VALIDATE_INTEGER ("IntegerOps::div(Integer&, const Integer&)", src, LOC_BEFORE);
#ifdef DEBUG_MODE
  if (!(dst.m_size == m_size && src.m_size == m_size)) {
    PRINT_MESSAGE_AND_EXIT ("[IntegerOps::div(Integer&, const Integer&)] The two arguments `dst' and `src' need to be of size %d.\n", m_size);
  }
#endif

  if (dst.m_max > 0 && src.m_max > 0) {

    const int numeratorBsr = dst.bsr ();
    const int denominatorBsr = src.bsr ();

    if (denominatorBsr <= numeratorBsr) {
      *m_numerator = dst;
      m_numerator->shl (m_bsize - numeratorBsr);
      *m_remainder = 0;
      dst = 0;

      m_remainder->lshl (*m_numerator, denominatorBsr);
      bool bit = subtractFromRemainder (src, denominatorBsr, denominatorBsr);
      dst.rcl (bit);
      baseDiv (dst, src, denominatorBsr, numeratorBsr - denominatorBsr);

      if (dst.m_max > 0)
        dst.m_sign = m_numerator->m_sign ^ src.m_sign;
      if (m_remainder->m_max > 0)
        m_remainder->m_sign = m_numerator->m_sign;
    } else {
      *m_remainder = dst;
      dst = 0;
    }

  } else if (src.m_max == 0) {

    throw std::runtime_error ("Division by zero.");

  } else {
    *m_remainder = 0;
  }

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

/* Modifies: m_mulResult.  */
Integer& IntegerOps::mul (const Integer& srcA, const Integer& srcB) {
  VALIDATE_INTEGER ("IntegerOps::mul(const Integer&, const Integer&)", srcA, LOC_BEFORE);
  VALIDATE_INTEGER ("IntegerOps::mul(const Integer&, const Integer&)", srcB, LOC_BEFORE);
#ifdef DEBUG_MODE
  if (!(srcA.m_size == m_size && srcB.m_size == m_size)) {
    PRINT_MESSAGE_AND_EXIT ("[IntegerOps::mul(const Integer&, const Integer&)] The two arguments `srcA' and `srcB' need to be of size %d.\n", m_size);
  }
#endif

  *m_mulResult = 0;
  if (srcA.m_max > 0 && srcB.m_max > 0) {
    baseMul (srcA, srcB);
  }

  VALIDATE_INTEGER ("IntegerOps::mul(const Integer&, const Integer&)", *m_mulResult, LOC_AFTER);
  return *m_mulResult;
}

void IntegerOps::setHighestBit (Integer& dst) {
  VALIDATE_INTEGER ("IntegerOps::setHighestBit(Integer&)", dst, LOC_BEFORE);
#ifdef DEBUG_MODE
  if (dst.m_size != m_size) {
    PRINT_MESSAGE_AND_EXIT ("[IntegerOps::setHighestBit(Integer&)] The argument `dst' needs to be of size %d.\n", m_size);
  }
#endif

  dst.m_max = m_size;
  dst.m_buf[m_size - 1] |= CAL_SMASK[CAL_B - 1];

  VALIDATE_INTEGER ("IntegerOps::setHighestBit(Integer&)", dst, LOC_AFTER);
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
#ifdef DEBUG_MODE
  if (denominator.bsr () != denomBsr) {
    PRINT_MESSAGE_AND_EXIT ("[IntegerOps::subtractFromRemainder(const Integer&, int, int)] denominator.bsr () != denomBsr.\n");
  }
  if (m_remainder->bsr () != remainderBsr) {
    PRINT_MESSAGE_AND_EXIT ("[IntegerOps::subtractFromRemainder(const Integer&, int, int)] m_remainder->bsr () != remainderBsr.\n");
  }
  if (!(remainderBsr == denomBsr || remainderBsr == denomBsr + 1)) {
    PRINT_MESSAGE_AND_EXIT ("[IntegerOps::subtractFromRemainder(const Integer&, int, int)] !(remainderBsr == denomBsr || remainderBsr == denomBsr + 1).\n");
  }
#endif

  int i;
  bool carry = false;
  if (remainderBsr == denomBsr) {

    for (i = 0; i < denominator.m_max; ++i) {
      m_aux->m_buf[i] = m_remainder->m_buf[i];
      if (carry) {
        CAL_CLEAR_CARRY (m_remainder->m_buf[i - 1]);
        --m_remainder->m_buf[i];
      }
      m_remainder->m_buf[i] -= denominator.m_buf[i];
      carry = CAL_CARRY (m_remainder->m_buf[i]);
    }
    if (m_aux->m_max < denominator.m_max)
      m_aux->m_max = denominator.m_max;
    if (carry) {
      memcpy (m_remainder->m_buf, m_aux->m_buf, denominator.m_max << 3);
    } else {
      m_remainder->setMax (i - 1);
    }

  } else {

    for (i = 0; i < denominator.m_max; ++i) {
      if (carry) {
        CAL_CLEAR_CARRY (m_remainder->m_buf[i - 1]);
        --m_remainder->m_buf[i];
      }
      m_remainder->m_buf[i] -= denominator.m_buf[i];
      carry = CAL_CARRY (m_remainder->m_buf[i]);
    }
    if (carry) {
      CAL_CLEAR_CARRY (m_remainder->m_buf[i - 1]);
      m_remainder->m_buf[i] = 0;
      carry = false;
    }
    m_remainder->setMax (i - 1);

  }

  VALIDATE_INTEGER ("IntegerOps::subtractFromRemainder(const Integer&, int, int)", *m_remainder, LOC_AFTER);
  VALIDATE_INTEGER ("IntegerOps::subtractFromRemainder(const Integer&, int, int)", *m_aux, LOC_AFTER);
  return !carry;
}

static void appendChars (std::string& str, int64_t val, bool withLeadingZeros) {
  int i;
  char buf[18];

  if (withLeadingZeros) {
    for (i = 0; i < 18; ++i) {
      buf[i] = val % 10 + '0';
      val /= 10;
    }
  } else {
    i = 0;
    while (val != 0) {
      buf[i++] = val % 10 + '0';
      val /= 10;
    }
  }
  --i;
  while (i > -1) {
    str += buf[i];
    --i;
  }
}

int IntegerOps::splitUp (int64_t* parts, Integer& value) {
  int index = 0;
  Integer exa = createInteger (1000000000000000000);
  while (value.m_max > 0) {
    Integer& remainder = div (value, exa);
    parts[index++] = (int64_t) remainder;
  }
  return index - 1;
}

void IntegerOps::toString (std::string& dst, int64_t* parts, Integer& value) {
  int i = splitUp (parts, value);
  appendChars (dst, parts[i--], false);
  while (i > -1) {
    appendChars (dst, parts[i--], true);
  }
}

/* Modifies: m_aux, m_remainder, m_numerator.  */
std::string IntegerOps::toString (const Integer& value) {
  VALIDATE_INTEGER ("IntegerOps::toString(const Integer&)", value, LOC_BEFORE);
#ifdef DEBUG_MODE
  if (value.m_size != m_size) {
    PRINT_MESSAGE_AND_EXIT ("[IntegerOps::toString(const Integer&)] Argument `value' needs to be of size %d.\n", m_size);
  }
#endif

  std::string result;
  if (value.m_max == 0) {
    result = "0";
    return result;
  }

  /* Estimate number of digits needed.  */
  const int digitsNeeded = (int) (0.30103 * value.bsr ()) + 1;
  const int eighteenDigitBlocks = digitsNeeded / 18 + 1;
  if (digitsNeeded > 14)
    result.reserve (digitsNeeded + 1);

  Integer temp = value;
  if (temp.m_sign) {
    temp.m_sign = false;
    result += '-';
  }

  if (eighteenDigitBlocks > 64) {
    int64_t* parts = (int64_t*) malloc (sizeof (int64_t) * eighteenDigitBlocks);
    toString (result, parts, temp);
    free (parts);
  } else {
    int64_t parts[64];
    toString (result, parts, temp);
  }
  return result;
}
