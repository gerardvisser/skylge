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

#ifndef ___DIR_ENTRY_INCLUDED
#define ___DIR_ENTRY_INCLUDED

#include <time.h>

typedef enum {
  FILE_TYPE_DIRECTORY,
  FILE_TYPE_REGULAR,
  FILE_TYPE_SYMLINK,
  FILE_TYPE_UNKNOWN
} fileType_t;

struct dirEntry_struct;
typedef struct dirEntry_struct dirEntry_t;

void        dirEntry_delete (dirEntry_t* this);
const char* dirEntry_extension (dirEntry_t* this);
fileType_t  dirEntry_fileType (dirEntry_t* this);
dirEntry_t* dirEntry_firstEntry (dirEntry_t* this);
const char* dirEntry_fullName (dirEntry_t* this);
time_t      dirEntry_modificationTime (dirEntry_t* this);
const char* dirEntry_name (dirEntry_t* this);
int         dirEntry_nameLength (dirEntry_t* this);
dirEntry_t* dirEntry_next (dirEntry_t* this);
dirEntry_t* dirEntry_previous (dirEntry_t* this);
dirEntry_t* dirEntry_readDir (const char* dirName);

#endif
