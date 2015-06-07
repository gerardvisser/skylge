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

#ifndef COMMAND_LINE_ARGS_INCLUDED
#define COMMAND_LINE_ARGS_INCLUDED

#include <stdbool.h>
#include "stringList.h"

struct commandLineArgs_struct;
typedef struct commandLineArgs_struct commandLineArgs_t;

struct commandLineArgs_option_struct;
typedef struct commandLineArgs_option_struct commandLineArgs_option_t;

void                      commandLineArgs_delete (commandLineArgs_t* this);
bool                      commandLineArgs_getBoolOptionValue (commandLineArgs_t* this, char optKey);
stringList_t*             commandLineArgs_getMainArgs (commandLineArgs_t* this);
stringList_t*             commandLineArgs_getStringOptionValue (commandLineArgs_t* this, char optKey);
commandLineArgs_t*        commandLineArgs_new (int argc, char** args, ...);

void                      commandLineArgs_option_delete (commandLineArgs_option_t* this);
commandLineArgs_option_t* commandLineArgs_option_newBoolOption (char key);
commandLineArgs_option_t* commandLineArgs_option_newStringOption (char key, const char* defaultValue);

#endif
