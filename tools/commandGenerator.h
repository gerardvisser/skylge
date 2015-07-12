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

#ifndef COMMAND_GENERATOR_INCLUDED
#define COMMAND_GENERATOR_INCLUDED

#include <stdbool.h>
#include "file.h"
#include "stringList.h"

struct commandGenerator_struct;
typedef struct commandGenerator_struct commandGenerator_t;

const char* commandGenerator_cleanCommand (commandGenerator_t* this);
const char* commandGenerator_compileCommand (commandGenerator_t* this, file_t* sourceFile, int filenameMacroValueStartIndex, int optimizationLevel);
const char* commandGenerator_createArchiveCommand (commandGenerator_t* this, const char* libDirectory, const char* libName, const char* libVersion, bool debugModeArchive);
const char* commandGenerator_createExeCommand (commandGenerator_t* this, const char* name, stringList_t* libSearchPath, stringList_t* libraries, bool strip);
void commandGenerator_delete (commandGenerator_t* this);
const char* commandGenerator_makeDirCommand (commandGenerator_t* this, const char* dirName);
commandGenerator_t* commandGenerator_new (const char* objsDirectory, stringList_t* includeSearchPath, stringList_t* macros);

#endif
