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

#ifndef SKYLGE__BASE___DBG_ALLOC_INFO_INCLUDED
#define SKYLGE__BASE___DBG_ALLOC_INFO_INCLUDED

class DbgAllocInfo {
private:
  long int m_allocCount;
  long int m_freeCount;
  long int m_allocated;
  long int m_allocatedMax;
  long int m_freed;

public:
  DbgAllocInfo (void);
  virtual ~DbgAllocInfo (void);

  void     add (long int sizeInBytes);
  long int getAllocated (void);
  long int getAllocatedMax (void);
  long int getAllocCount (void);
  long int getCurrentlyAllocated (void);
  long int getFreeCount (void);
  long int getFreed (void);
  void     sub (long int sizeInBytes);
  void     update (long int sizeInBytes);

private:
  void     updateMax (void);
};

#endif

#endif
