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

#ifndef SKYLGE__MATH___INTEGER_INCLUDED
#define SKYLGE__MATH___INTEGER_INCLUDED

#include <stdint.h>

class Integer {
private:
  uint64_t* m_buf;
  int m_size;
  int m_max;
  bool m_sign;

public:
  explicit Integer (int size);
  Integer (const Integer& other);
  Integer (Integer&& other);
  virtual ~Integer (void);

  Integer& operator= (const Integer& other);
  Integer& operator= (Integer&& other);
  Integer& operator= (int64_t val);
  operator int64_t ();
  operator int ();

  bool operator== (const Integer& other) const;
  bool operator!= (const Integer& other) const;

  int bsf (void) const;
  int bsr (void) const;
  bool getBit (int bitNo) const;
  void shl (int x);
  void shr (int x);
  bool sign (void) const;

#ifdef DEBUG_MODE
  const uint64_t* buf (void) const;
  int max (void) const;
  int size (void) const;
#else
private:
#endif

  bool absAdd (const Integer& other);
  bool absAdd (uint64_t value);
  bool absDec (void);
  bool absInc (void);
  bool absSub (const Integer& other);
  bool absSub (uint64_t value);
  void lshl (Integer& incomingBits, int x);
  void rcl (bool carry);
  void setMax (int fromIndex);

  void copy (const Integer& other);
  void copyUsingExistingBuffer (const Integer& other);
  void move (Integer& other);

  friend class IntegerOps;
};

#endif
