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

#ifndef LIBRARIES_INCLUDED
#define LIBRARIES_INCLUDED

#include <stdbool.h>
#include "stringBuilder.h"
#include "stringList.h"

struct libraries_struct;
typedef struct libraries_struct libraries_t;

void libraries_delete (libraries_t* this);
bool libraries_exists (libraries_t* this, const char* libName);
void libraries_filenameForName (stringBuilder_t* filename, const char* libName);
libraries_t* libraries_new (stringList_t* libSearchPath);

#endif
