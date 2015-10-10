/*
   Author      : Gerard Visser
   e-mail      : visser.gerard(at)gmail.com

   Copyright (C) 2008 - 2015 Gerard Visser.

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

#ifdef DEBUG_MODE

#include <stdlib.h>
#include "DbgRefInfo.h"
#include <skylge/base/mem.h>
#include <skylge/base/Object.h>

DbgRefInfo::DbgRefInfo (const void* pointer, const char* typeName, long int size, int ptype) : m_pointer (pointer), m_typeName (typeName), m_size (size), m_ptype (ptype) {
  m_previous = NULL;
  m_next = NULL;
}

DbgRefInfo::~DbgRefInfo (void) {
  if ((m_ptype & PTYPE_VIRTUAL) == 0) {
    free ((void*) m_pointer);
  }
}

DbgRefInfo* DbgRefInfo::getNext (void) {
  return m_next;
}

const void* DbgRefInfo::getPointer (void) {
  return m_pointer;
}

DbgRefInfo* DbgRefInfo::getPrevious (void) {
  return m_previous;
}

int DbgRefInfo::getPtype (void) {
  return m_ptype;
}

long int DbgRefInfo::getRefCount (void) {
  long int refCount;

  if ((m_ptype & PTYPE_REF_COUNT_CONTAINER) != 0) {
    refCount = ((Object*) m_pointer)->refCount ();
  } else {
    refCount = 1;
  }
  return refCount;
}

long int DbgRefInfo::getSize (void) {
  return m_size;
}

const char* DbgRefInfo::getTypeName (void) {
  const char* name;

  if (m_typeName == NULL) {
    if ((m_ptype & PTYPE_REF_COUNT_CONTAINER) != 0) {
      name = ((Object*) m_pointer)->getClass ()->name;
    } else {
      name = "(Unknown type)";
    }
  } else {
    name = m_typeName;
  }
  return name;
}

void DbgRefInfo::setNext (DbgRefInfo* val) {
  m_next = val;
}

void DbgRefInfo::setPointer (const void* val) {
  m_pointer = val;
}

void DbgRefInfo::setPrevious (DbgRefInfo* val) {
  m_previous = val;
}

void DbgRefInfo::setSize (long int val) {
  m_size = val;
}

#endif
