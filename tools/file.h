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

#ifndef ___FILE_INCLUDED
#define ___FILE_INCLUDED

#include <time.h>

typedef enum {
  FILE_TYPE_DIRECTORY,
  FILE_TYPE_REGULAR,
  FILE_TYPE_SYMLINK,
  FILE_TYPE_UNKNOWN
} fileType_t;

struct file_struct;
typedef struct file_struct file_t;

void        file_delete (file_t* this);
const char* file_extension (file_t* this);
int         file_extensionLength (file_t* this);
file_t*     file_firstEntry (file_t* this);
const char* file_fullName (file_t* this);
int         file_fullNameLength (file_t* this);
time_t      file_modificationTime (file_t* this);
const char* file_name (file_t* this);
int         file_nameLength (file_t* this);
file_t*     file_new (const char* filename);
file_t*     file_next (file_t* this);
file_t*     file_previous (file_t* this);
fileType_t  file_type (file_t* this);

#endif
