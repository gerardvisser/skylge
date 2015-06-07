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

#ifndef FILE_INFO_INCLUDED
#define FILE_INFO_INCLUDED

#include <time.h>
#include "fileType.h"

typedef struct {
  unsigned long int size;
  time_t modificationTime;
  fileType_t fileType;
} fileInfo_t;

int fileInfo_read (fileInfo_t* fileInfo, const char* filename);

#endif
