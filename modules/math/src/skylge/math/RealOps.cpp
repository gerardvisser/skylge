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

#include <skylge/math/RealOps.h>
#include "errors.h"

RealOps::RealOps (int size) : IntegerOps (size) {
  m_auxI = new Integer (size);
  m_auxR = new Real (size);
}

RealOps::~RealOps (void) {
  delete m_auxI;
  delete m_auxR;
}

bool RealOps::add (Real& dst, const Real& src) {
  VALIDATE_REAL ("RealOps::add(Real&, const Real&)", dst, LOC_BEFORE);
  VALIDATE_REAL ("RealOps::add(Real&, const Real&)", src, LOC_BEFORE);
#ifdef DEBUG_MODE
  if (!(dst.m_number->sizeInBits () == m_bsize && src.m_number->sizeInBits () == m_bsize)) {
    PRINT_MESSAGE_AND_EXIT ("[RealOps::add(Real&, const Real&)] The two arguments `dst' and `src' need to be of size %d.\n", m_size);
  }
#endif

  bool error = false;

  if (!src.m_number->isZero ()) {
    if (!dst.m_number->isZero ()) {
      addFiniteNonzero (dst, src);
    } else if (!dst.isInfinite ()) {
      dst = src;
    }
  } else if (src.isInfinite ()) {
    if (dst.isInfinite ()) {
      error = dst.sign () ^ src.sign ();
    } else {
      dst = src;
    }
  }

  VALIDATE_REAL ("RealOps::add(Real&, const Real&)", dst, LOC_AFTER);
  return error;
}

void RealOps::addEqualExponents (Real& dst, const Real& src) {
  bool carry = IntegerOps::add (*dst.m_number, *src.m_number);
  if (carry) {
    carry = inc (*dst.m_exponent);
    if (carry) {
      dst.makeInfinite (dst.sign ());
      return;
    }
    bool round = dst.m_number->getBit (0);
    dst.m_number->shr (1);
    setHighestBit (*dst.m_number);
    if (round) {
      if (dst.sign ())
        dec (*dst.m_number);
      else
        inc (*dst.m_number);
    }
  }
  int bsfVal = dst.m_number->bsf ();
  carry = IntegerOps::add (*dst.m_exponent, bsfVal);
  if (carry) {
    dst.makeInfinite (dst.sign ());
    return;
  }
  dst.m_number->shr (bsfVal);
}

void RealOps::addFiniteNonzero (Real& dst, const Real& src) {
  *m_auxI = *dst.m_exponent;
  bool carry = IntegerOps::sub (*m_auxI, *src.m_exponent);
  int expDiff = *m_auxI;
  if (!(carry || expDiff == 0x80000000)) {
    if (expDiff != 0) {
      addUnequalExponents (dst, src, expDiff);
    } else {
      addEqualExponents (dst, src);
    }
  } else if (m_auxI->sign ()) {
    dst = src;
  }
}

/* Misschien hier afronden als expDiff == maxExpDiff + 1? */
void RealOps::addUnequalExponents (Real& dst, const Real& src, int expDiff) {
  int dstBsr = dst.m_number->bsr ();
  int srcBsr = src.m_number->bsr ();
  if (expDiff > 0) {
    int maxLeftShift = m_bsize - dstBsr;
    int maxRightShift = srcBsr - 1;
    int maxExpDiff = maxLeftShift + maxRightShift;
    if (expDiff <= maxExpDiff) {
      if (expDiff <= maxLeftShift) {
        dst.m_number->shl (expDiff);
        IntegerOps::add (*dst.m_exponent, -expDiff);
        addEqualExponents (dst, src);
      } else {
        dst.m_number->shl (maxLeftShift);
        IntegerOps::add (*dst.m_exponent, -maxLeftShift);
        expDiff -= maxLeftShift;
        *m_auxR = src;
        m_auxR->m_number->shr (expDiff);
        /* Intentionally not updating exponent of m_auxR, here.  */
        addEqualExponents (dst, *m_auxR);
      }
    }
  } else {
    expDiff = -expDiff;
    int maxLeftShift = m_bsize - srcBsr;
    int maxRightShift = dstBsr - 1;
    int maxExpDiff = maxLeftShift + maxRightShift;
    if (expDiff <= maxExpDiff) {
      *m_auxR = src;
      if (expDiff <= maxLeftShift) {
        m_auxR->m_number->shl (expDiff);
        /* Intentionally not updating exponent of m_auxR, here.  */
      } else {
        m_auxR->m_number->shl (maxLeftShift);
        /* Intentionally not updating exponent of m_auxR, here.  */
        expDiff -= maxLeftShift;
        dst.m_number->shr (expDiff);
        IntegerOps::add (*dst.m_exponent, expDiff);
      }
      addEqualExponents (dst, *m_auxR);
    } else {
      dst = src;
    }
  }
}

Real RealOps::createReal (double value) {
  Real result (m_size);
  result = value;
  return result;
}

bool RealOps::div (Real& dst, const Real& src) {
  /* TODO: IMPLEMENT */
  return false;
}

bool RealOps::mul (Real& dst, const Real& src) {
  /* TODO: IMPLEMENT */
  return false;
}

bool RealOps::sub (Real& dst, const Real& src) {
  /* TODO: IMPLEMENT */
  return false;
}

std::string RealOps::toString (const Real& value, int precision) {
  /* TODO: IMPLEMENT */
  return "Not implemented";
}
