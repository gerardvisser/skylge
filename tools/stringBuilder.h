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

#ifndef STRING_BUILDER_INCLUDED
#define STRING_BUILDER_INCLUDED

struct stringBuilder_struct;
typedef struct stringBuilder_struct stringBuilder_t;

void stringBuilder_append (stringBuilder_t* this, const char* str);
void stringBuilder_appendChar (stringBuilder_t* this, char c);
void stringBuilder_appendChars (stringBuilder_t* this, const char* str, int len);
void stringBuilder_clear (stringBuilder_t* this);
void stringBuilder_delete (stringBuilder_t* this);
const char* stringBuilder_getBuffer (stringBuilder_t* this);
int stringBuilder_length (stringBuilder_t* this);
stringBuilder_t* stringBuilder_new (int initialCapacity);
char* stringBuilder_toString (stringBuilder_t* this);

#endif
