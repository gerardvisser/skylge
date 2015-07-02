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

#ifndef STRING_LIST_INCLUDED
#define STRING_LIST_INCLUDED

typedef struct stringList_struct stringList_t;

struct stringList_struct {
  const char* value;
  stringList_t* next;
  stringList_t* previous;
  int valueLength;
};

stringList_t* stringList_append (stringList_t* this, const char* value);
void          stringList_delete (stringList_t* this);
stringList_t* stringList_firstElement (stringList_t* this);
int           stringList_length (stringList_t* this);
stringList_t* stringList_new (const char* value);

#endif
