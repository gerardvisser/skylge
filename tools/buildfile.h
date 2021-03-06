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

#ifndef BUILDFILE_INCLUDED
#define BUILDFILE_INCLUDED

#define BUILDFILE_DEFAULT_NAME "buildfile"

struct buildfile_struct;
typedef struct buildfile_struct buildfile_t;

int          buildfile_argCount (buildfile_t* this);
char**       buildfile_arguments (buildfile_t* this);
void         buildfile_delete (buildfile_t* this);
const char*  buildfile_dirName (buildfile_t* this);
int          buildfile_dirNameLength (buildfile_t* this);
buildfile_t* buildfile_new (const char* filename);

#endif
