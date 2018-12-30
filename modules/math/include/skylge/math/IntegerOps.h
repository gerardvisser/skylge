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

#ifndef SKYLGE__MATH___INTEGER_OPS_INCLUDED
#define SKYLGE__MATH___INTEGER_OPS_INCLUDED

#include <skylge/math/Integer.h>

class IntegerOps {
private:
  Integer* m_mulResult;
  Integer* m_remainder;
  Integer* m_numerator;
  Integer* m_aux;
  const int m_size;
  const int m_bsize;

public:
  IntegerOps (int size);
  IntegerOps (const IntegerOps&) = delete;
  IntegerOps (IntegerOps&&) = delete;
  virtual ~IntegerOps (void);

  IntegerOps& operator= (const IntegerOps&) = delete;
  IntegerOps& operator= (IntegerOps&&) = delete;

  bool add (Integer& dst, const Integer& src);
  bool add (Integer& dst, int value);
  Integer createInteger (int64_t value = 0);
  bool dec (Integer& dst);
  Integer& div (Integer& dst, const Integer& src);
  bool inc (Integer& dst);
  Integer& mul (const Integer& srcA, const Integer& srcB);
  bool sub (Integer& dst, const Integer& src);

private:
  void baseDiv (Integer& result, const Integer& denominator, int denomBsr, int total);
  void baseMul (const Integer& srcA, const Integer& srcB);
  bool subtractFromRemainder (const Integer& denominator, int denomBsr, int remainderBsr);
};

#endif
