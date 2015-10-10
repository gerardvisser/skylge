/*
   Author      : Gerard Visser
   e-mail      : visser.gerard(at)gmail.com

   Copyright (C) 2013 - 2015 Gerard Visser.

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

#include "DbgAllocInfo.h"

DbgAllocInfo::DbgAllocInfo (void) {
  m_allocCount = 0;
  m_freeCount = 0;
  m_allocated = 0;
  m_allocatedMax = 0;
  m_freed = 0;
}

DbgAllocInfo::~DbgAllocInfo (void) {
}

void DbgAllocInfo::add (long int sizeInBytes) {
  ++m_allocCount;
  m_allocated += sizeInBytes;
  updateMax ();
}

long int DbgAllocInfo::getAllocated (void) {
  return m_allocated;
}

long int DbgAllocInfo::getAllocatedMax (void) {
  return m_allocatedMax;
}

long int DbgAllocInfo::getAllocCount (void) {
  return m_allocCount;
}

long int DbgAllocInfo::getCurrentlyAllocated (void) {
  return m_allocated - m_freed;
}

long int DbgAllocInfo::getFreeCount (void) {
  return m_freeCount;
}

long int DbgAllocInfo::getFreed (void) {
  return m_freed;
}

void DbgAllocInfo::sub (long int sizeInBytes) {
  ++m_freeCount;
  m_freed += sizeInBytes;
}

void DbgAllocInfo::update (long int sizeInBytes) {
  if (sizeInBytes > 0) {
    m_allocated += sizeInBytes;
    updateMax ();
  } else {
    m_freed -= sizeInBytes;
  }
}

void DbgAllocInfo::updateMax (void) {
  long int allocatedCurrently = m_allocated - m_freed;
  if (allocatedCurrently > m_allocatedMax) {
    m_allocatedMax = allocatedCurrently;
  }
}

#endif
