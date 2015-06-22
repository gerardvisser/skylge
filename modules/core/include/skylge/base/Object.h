/*
   Author:  Gerard Visser
   e-mail:  visser.gerard(at)gmail.com

   Copyright (C) 2015 Gerard Visser.

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

#ifndef SKYLGE__BASE___OBJECT_INCLUDED
#define SKYLGE__BASE___OBJECT_INCLUDED

#include <skylge/base/class.h>

class Object {
private:
  unsigned long int m_refCount;
#ifdef DEBUG_MODE
  unsigned long int m_refCountInternal;
#endif

public:
  Object (void);
  virtual ~Object (void);

  void ref (void);
  void unref (void);

#ifdef DEBUG_MODE
  void internalize (void);
  unsigned long int refCount (void);
  unsigned long int refCountInternal (void);
  void refInternal (void);
  void unrefInternal (void);
#else
#define refInternal ref
#define unrefInternal unref
#endif

  REFL_DECL

  static void* operator new (size_t s);
  static void operator delete (void* p);
  static void* operator new[] (size_t s);
  static void operator delete[] (void* p);
};

#ifdef DEBUG_MODE
#define INTERNALIZE(obj) obj->internalize ()
#else
#define INTERNALIZE(obj)
#endif

#endif
