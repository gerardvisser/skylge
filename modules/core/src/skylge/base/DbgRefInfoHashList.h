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

#ifndef SKYLGE__BASE___DBG_REF_INFO_HASH_LIST_INCLUDED
#define SKYLGE__BASE___DBG_REF_INFO_HASH_LIST_INCLUDED

#include "DbgRefInfoList.h"

#define BUCKET_COUNT 0x800

class DbgRefInfoHashList {
private:
  DbgRefInfoList m_bucket[BUCKET_COUNT];

public:
  DbgRefInfoHashList (void);
  virtual ~DbgRefInfoHashList (void);

  void        add (DbgRefInfo* elem);
  void        printContents (FILE* stream = stdout);
  DbgRefInfo* remove (const void* pointer);
};

# endif

#endif
