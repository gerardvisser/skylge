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

#ifndef BUILD_OPTIONS_INCLUDED
#define BUILD_OPTIONS_INCLUDED

#include <stdbool.h>
#include "stringList.h"

typedef struct {
  stringList_t* files;
  stringList_t* macros;
  stringList_t* libraries;
  stringList_t* libSearchPath;
  stringList_t* includeSearchPath;
  const char* objsDirectory;
  const char* libDirectory;
  const char* libVersion;
  const char* libName;
  const char* exeName;
  int optimizationLevel;
  bool clean;
} buildOptions_t;

void buildOptions_delete (buildOptions_t* this);
buildOptions_t* buildOptions_new (int argc, char** args);

#endif
