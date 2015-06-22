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

#ifndef SKYLGE__BASE__CLASS_INCLUDED
#define SKYLGE__BASE__CLASS_INCLUDED

#ifdef DEBUG_MODE

#include <stddef.h>

typedef struct {
  const char* const name;
  const size_t size;
} class_t;

#define REFL_DECL \
  public: \
    virtual class_t* getClass (void);

#define REFL_IMPL(type) \
  static const char* const ___className = #type; \
  static class_t ___classObj = {#type, sizeof (type)}; \
  class_t* type::getClass (void) { \
    return &___classObj; \
  }

#else

#define REFL_DECL
#define REFL_IMPL(type)

#endif

#endif
