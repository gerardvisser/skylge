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

#ifndef SKYLGE__MATH___REAL_INCLUDED
#define SKYLGE__MATH___REAL_INCLUDED

#include <skylge/math/Integer.h>

class Real {
private:
  Integer* m_exponent;
  Integer* m_number;

public:
  explicit Real (int size);
  Real (const Real& other);
  Real (Real&& other);
  virtual ~Real (void);

  Real& operator= (const Real& other);
  Real& operator= (Real&& other);
  Real& operator= (double val);
  operator double () const;

  bool operator== (const Real& other) const;
  bool operator!= (const Real& other) const;

  bool isInfinite (void) const;
  bool sign (void) const;

#ifdef DEBUG_MODE
  const Integer& exponent (void) const;
  const Integer& number (void) const;
#endif

private:
  void makeInfinite (bool sign);

  void copy (const Real& other);
  void move (Real& other);
};

#endif
