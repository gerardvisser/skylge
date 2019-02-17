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

#ifndef SKYLGE__MATH___REAL_OPS_INCLUDED
#define SKYLGE__MATH___REAL_OPS_INCLUDED

#include <skylge/math/IntegerOps.h>
#include <skylge/math/Real.h>

class RealOps : protected IntegerOps {
private:
  Real* m_auxR;
  Integer* m_auxI;

public:
  explicit RealOps (int size);
  virtual ~RealOps (void);

  bool add (Real& dst, const Real& src);
  Real createReal (double value = 0.0);
  bool div (Real& dst, const Real& src);
  bool mul (Real& dst, const Real& src);
  bool sub (Real& dst, const Real& src);
  std::string toString (const Real& value, int precision = 6);

private:
  void addEqualExponents (Real& dst, const Real& src);
  void addFiniteNonzero (Real& dst, const Real& src);
  void addUnequalExponents (Real& dst, const Real& src, int expDiff);
  void mulFiniteNonzero (Real& dst, const Real& src);
};

#endif
