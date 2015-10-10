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

#ifndef SKYLGE__BASE___DBG_REF_INFO_INCLUDED
#define SKYLGE__BASE___DBG_REF_INFO_INCLUDED

#define PTYPE_VIRTUAL             8 /* See also skylge/base/mem.h */

class DbgRefInfo {
private:
  DbgRefInfo* m_previous;
  DbgRefInfo* m_next;

  const void* m_pointer;
  long int m_size;
  const char* const m_typeName;
  const int m_ptype;

public:
  DbgRefInfo (const void* pointer, const char* typeName, long int size, int ptype);
  virtual ~DbgRefInfo (void);

  DbgRefInfo* getNext (void);
  const void* getPointer (void);
  DbgRefInfo* getPrevious (void);
  int         getPtype (void);
  long int    getRefCount (void);
  long int    getSize (void);
  const char* getTypeName (void);
  void        setNext (DbgRefInfo* val);
  void        setPointer (const void* val);
  void        setPrevious (DbgRefInfo* val);
  void        setSize (long int val);
};

#endif

#endif
