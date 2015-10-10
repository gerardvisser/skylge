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

#include <stdint.h>
#include <skylge/base/mem.h>
#include "DbgRefInfoHashList.h"

static const uint64_t M = 1073741827;
static const uint64_t A = 1048583;
static const uint64_t B = 440234149;

static int hashCode (const void* pointer) {
  uint64_t val = (uint64_t) (long) pointer;
  val &= 0x3FFFFFFF;
  val *= A;
  val += B;
  val %= M;
  return (int) val;
}

DbgRefInfoHashList::DbgRefInfoHashList (void) {
}

DbgRefInfoHashList::~DbgRefInfoHashList (void) {
}

void DbgRefInfoHashList::add (DbgRefInfo* elem) {
  int index = hashCode (elem->getPointer ()) % BUCKET_COUNT;
  m_bucket[index].add (elem);
}

void DbgRefInfoHashList::printContents (FILE* stream) {
  for (int i = 0; i < BUCKET_COUNT; ++i) {
    m_bucket[i].printContents (stream);
  }
}

DbgRefInfo* DbgRefInfoHashList::remove (const void* pointer) {
  int index = hashCode (pointer) % BUCKET_COUNT;
  m_bucket[index].remove (pointer);
}

#endif
