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

#include <stdlib.h>
#include "commandGenerator.h"
#include "stringBuilder.h"

struct commandGenerator_struct {
  stringBuilder_t* buffer;
  const char* compileCommand;
  const char* debugMacros;
  int compileCommandLength;
  int debugMacrosLength;
  int objsDirectoryIndex;
};

const char* commandGenerator_cleanCommand (commandGenerator_t* this) {
  stringBuilder_clear (this->buffer);
  stringBuilder_appendChars (this->buffer, "rm ", 3);
  stringBuilder_appendChars (this->buffer, this->compileCommand + this->objsDirectoryIndex, this->compileCommandLength - this->objsDirectoryIndex);
  stringBuilder_appendChar (this->buffer, '*');
  return stringBuilder_getBuffer (this->buffer);
}

/* filenameMacroValueStartIndex < 0 ? optimize : debug_mode */
const char* commandGenerator_compileCommand (commandGenerator_t* this, file_t* sourceFile, int filenameMacroValueStartIndex) {
  stringBuilder_clear (this->buffer);
  stringBuilder_appendChars (this->buffer, this->compileCommand, this->compileCommandLength);
  stringBuilder_appendChars (this->buffer, file_name (sourceFile), file_nameLength (sourceFile) - file_extensionLength (sourceFile));
  stringBuilder_appendChars (this->buffer, "o ", 2);
  if (filenameMacroValueStartIndex >= 0) {
    stringBuilder_appendChars (this->buffer, this->debugMacros, this->debugMacrosLength);
    stringBuilder_appendChars (this->buffer, file_fullName (sourceFile) + filenameMacroValueStartIndex, file_fullNameLength (sourceFile) - filenameMacroValueStartIndex);
    stringBuilder_appendChars (this->buffer, "\\\" ", 3);
  } else {
    stringBuilder_appendChars (this->buffer, "-O", 2);
    /* Opt. level nog zetten. */
  }
  stringBuilder_appendChars (this->buffer, file_fullName (sourceFile), file_fullNameLength (sourceFile));
  return stringBuilder_getBuffer (this->buffer);
}

const char* commandGenerator_createArchiveCommand (commandGenerator_t* this, const char* libDirectory, const char* libName, const char* libVersion, bool debugModeArchive) {
  stringBuilder_clear (this->buffer);
  stringBuilder_appendChars (this->buffer, "ar rcs ", 7);
  stringBuilder_append (this->buffer, libDirectory);
  stringBuilder_appendChar (this->buffer, '/');
  stringBuilder_appendChars (this->buffer, "lib", 3);
  stringBuilder_append (this->buffer, libName);
  stringBuilder_appendChar (this->buffer, '-');
  stringBuilder_append (this->buffer, libVersion);
  if (debugModeArchive) {
    stringBuilder_appendChars (this->buffer, "-d", 2);
  }
  stringBuilder_appendChars (this->buffer, ".a ", 3);
  stringBuilder_appendChars (this->buffer, this->compileCommand + this->objsDirectoryIndex, this->compileCommandLength - this->objsDirectoryIndex);
  stringBuilder_appendChar (this->buffer, '*');
  return stringBuilder_getBuffer (this->buffer);
}

const char* commandGenerator_createExeCommand (commandGenerator_t* this, const char* name, stringList_t* libSearchPath, stringList_t* libraries, bool strip) {
  stringBuilder_clear (this->buffer);
  stringBuilder_appendChars (this->buffer, "c++ ", 4);
  while (libSearchPath != NULL) {
    stringBuilder_appendChars (this->buffer, "-L", 2);
    stringBuilder_appendChars (this->buffer, libSearchPath->value, libSearchPath->valueLength);
    stringBuilder_appendChar (this->buffer, ' ');
    libSearchPath = libSearchPath->next;
  }
  if (strip) {
    stringBuilder_appendChars (this->buffer, "-s ", 3);
  }
  stringBuilder_appendChars (this->buffer, "-o", 2);
  stringBuilder_append (this->buffer, name);
  stringBuilder_appendChar (this->buffer, ' ');
  stringBuilder_appendChars (this->buffer, this->compileCommand + this->objsDirectoryIndex, this->compileCommandLength - this->objsDirectoryIndex);
  stringBuilder_append (this->buffer, "* -pthread");
  while (libraries != NULL) {
    stringBuilder_appendChars (this->buffer, " -l", 3);
    stringBuilder_appendChars (this->buffer, libraries->value, libraries->valueLength);
    libraries = libraries->next;
  }
  return stringBuilder_getBuffer (this->buffer);
}

void commandGenerator_delete (commandGenerator_t* this) {
  stringBuilder_delete (this->buffer);
  free ((void*) this->compileCommand);
  free ((void*) this->debugMacros);
  free (this);
}

const char* commandGenerator_makeDirCommand (commandGenerator_t* this, const char* dirName) {
  stringBuilder_clear (this->buffer);
  stringBuilder_appendChars (this->buffer, "mkdir -p ", 9);
  stringBuilder_append (this->buffer, dirName);
  return stringBuilder_getBuffer (this->buffer);
}

commandGenerator_t* commandGenerator_new (const char* objsDirectory, stringList_t* includeSearchPath, stringList_t* macros) {
  commandGenerator_t* result = malloc (sizeof (commandGenerator_t));
  result->buffer = stringBuilder_new (4096);

  stringBuilder_append (result->buffer, "c++ -pthread -c ");
  while (includeSearchPath != NULL) {
    stringBuilder_appendChars (result->buffer, "-I", 2);
    stringBuilder_appendChars (result->buffer, includeSearchPath->value, includeSearchPath->valueLength);
    stringBuilder_appendChar (result->buffer, ' ');
    includeSearchPath = includeSearchPath->next;
  }
  stringBuilder_appendChars (result->buffer, "-o", 2);
  result->objsDirectoryIndex = stringBuilder_length (result->buffer);
  stringBuilder_append (result->buffer, objsDirectory);
  //stringBuilder_appendChar (result->buffer, '/');
  result->compileCommand = stringBuilder_toString (result->buffer);
  result->compileCommandLength = stringBuilder_length (result->buffer);

  stringBuilder_clear (result->buffer);
  stringBuilder_append (result->buffer, "-DDEBUG_MODE ");
  while (macros != NULL) {
    stringBuilder_appendChars (result->buffer, "-D", 2);
    stringBuilder_appendChars (result->buffer, macros->value, macros->valueLength);
    stringBuilder_appendChar (result->buffer, ' ');
    macros = macros->next;
  }
  stringBuilder_append (result->buffer, "-D__BESTAND__=\\\"");
  result->debugMacros = stringBuilder_toString (result->buffer);
  result->debugMacrosLength = stringBuilder_length (result->buffer);

  return result;
}
