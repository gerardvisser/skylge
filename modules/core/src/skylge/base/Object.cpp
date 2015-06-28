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

#include <skylge/base/errors.h>
#include <skylge/base/mem.h>
#include <skylge/base/Object.h>

REFL_IMPL (Object)

Object::Object (void) {
  m_refCountInternal = 0;
  m_refCount = 1;
}

Object::~Object (void) {
}

void Object::ref (void) {
  ++m_refCount;
}

void Object::unref (void) {
  ___CBTPUSH;
#ifdef DEBUG_MODE
  if (m_refCountInternal == m_refCount) {
    /* Error */
  }
#endif

  --m_refCount;
  if (m_refCount == 0) {
    delete this;
  }

  ___CBTPOP;
}


/* ╔════════════════════════════════════════════════════════════════════════╗
   ║ Operators:                                                             ║
   ╚════════════════════════════════════════════════════════════════════════╝ */

void Object::operator delete (void* p) {
  ___CBTPUSH;

  mem_release (p);

  ___CBTPOP;
}

void Object::operator delete[] (void* p) {
  ___CBTPUSH;

  mem_release (p);

  ___CBTPOP;
}

void* Object::operator new (size_t s) {
  ___CBTPUSH;

  void* result = mem_allocate (NULL, s, PTYPE_REF_COUNT_CONTAINER);

  ___CBTPOP;
  return result;
}

void* Object::operator new[] (size_t s) {
  ___CBTPUSH;

  /* Note: Unknown type! */
  void* result = mem_allocate (NULL, s, PTYPE_ORDINARY);

  ___CBTPOP;
  return result;
}


/* ╔════════════════════════════════════════════════════════════════════════╗
   ║ Debug mode only:                                                       ║
   ╚════════════════════════════════════════════════════════════════════════╝ */

#ifdef DEBUG_MODE

void Object::externalize (void) {
  ___CBTPUSH;

  if (m_refCountInternal == 0) {
    /* Error */
  }
  --m_refCountInternal;

  ___CBTPOP;
}

void Object::internalize (void) {
  ___CBTPUSH;

  if (m_refCountInternal == m_refCount) {
    /* Error */
  }
  ++m_refCountInternal;

  ___CBTPOP;
}

unsigned long int Object::refCount (void) {
  return m_refCount;
}

unsigned long int Object::refCountInternal (void) {
  return m_refCountInternal;
}

void Object::refInternal (void) {
  ___CBTPUSH;

  ref ();
  internalize ();

  ___CBTPOP;
}

void Object::unrefInternal (void) {
  ___CBTPUSH;

  externalize ();
  unref ();

  ___CBTPOP;
}

#endif
