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

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "buildfile.h"
#include "errors.h"
#include "fileInfo.h"

struct buildfile_struct {
  const char* dirName;
  const char* argsBuffer;
  char** args;
  int dirNameLength;
  int argCount;
};

static void createArgsArray (int* argc, char*** args, char* argsBuffer);

int buildfile_argCount (buildfile_t* this) {
  return this->argCount;
}

char** buildfile_arguments (buildfile_t* this) {
  return this->args;
}

void buildfile_delete (buildfile_t* this) {
  free ((void*) this->argsBuffer);
  free ((void*) this->dirName);
  free (this->args);
  free (this);
}

const char* buildfile_dirName (buildfile_t* this) {
  return this->dirName;
}

int buildfile_dirNameLength (buildfile_t* this) {
  return this->dirNameLength;
}

buildfile_t* buildfile_new (const char* filename) {
  fileInfo_t fileInfo;
  if (fileInfo_read (&fileInfo, filename) == 0) {
    char* dirnameEnd;
    int len = strlen (filename);
    char* const name = malloc (len + strlen (BUILDFILE_DEFAULT_NAME) + 2);
    if (fileInfo.fileType == FILE_TYPE_DIRECTORY) {
      dirnameEnd = stpcpy (name, filename);
      if (dirnameEnd[-1] != '/') {
        dirnameEnd[0] = '/';
        ++dirnameEnd;
      }
      stpcpy (dirnameEnd, BUILDFILE_DEFAULT_NAME);
      if (fileInfo_read (&fileInfo, name) != 0) {
        errors_printMessageAndExit ("No file named '%s' exists", name);
      }
    } else {
      stpcpy (name, filename);
      dirnameEnd = strrchr (name, '/');
      if (dirnameEnd == NULL) {
        dirnameEnd = name;
      } else {
        ++dirnameEnd;
      }
    }
    if (fileInfo.fileType != FILE_TYPE_REGULAR) {
      errors_printMessageAndExit ("'%s' is not a regular file", name);
    }

    char* argsBuffer = malloc (fileInfo.size + 1);
    /* What if no permission to read this file? */
    FILE* handle = fopen (name, "rb");
    if (fread (argsBuffer, 1, fileInfo.size, handle) < fileInfo.size) {
      errors_printMessageAndExit ("An error occurred while reading file '%s'", name);
    }
    fclose (handle);
    argsBuffer[fileInfo.size] = 0;

    int argc;
    char** args;
    createArgsArray (&argc, &args, argsBuffer);

    *dirnameEnd = 0;
    buildfile_t* result = malloc (sizeof (buildfile_t));
    result->dirNameLength = dirnameEnd - name;
    result->dirName = name;
    result->argsBuffer = argsBuffer;
    result->argCount = argc;
    result->args = args;
    return result;
  } else if (strcmp (filename, BUILDFILE_DEFAULT_NAME) == 0) {
    return NULL;
  } else {
    errors_printMessageAndExit ("No file named '%s' exists", filename);
  }
}

static int getArgCount (const char* argsBuffer);
static char* nextArg (const char* argsBuffer);
static int skipComment (const char* argsBuffer, int index);

static void createArgsArray (int* argc, char*** args, char* argsBuffer) {
  *argc = getArgCount (argsBuffer);
  *args = malloc (sizeof (void*) * (*argc + 1));
  *args[*argc] = NULL;
  int i = 0;
  argsBuffer = nextArg (argsBuffer);
  while (*argsBuffer != 0) {
    (*args)[i] = argsBuffer;
    while (!(isspace (*argsBuffer) | *argsBuffer == 0)) {
      ++argsBuffer;
    }
    char* argEnd = argsBuffer;
    argsBuffer = nextArg (argsBuffer);
    *argEnd = 0;
    ++i;
  }
}

static int getArgCount (const char* argsBuffer) {
  int result = 0;
  argsBuffer = nextArg (argsBuffer);
  while (*argsBuffer != 0) {
    while (!(isspace (*argsBuffer) | *argsBuffer == 0)) {
      ++argsBuffer;
    }
    argsBuffer = nextArg (argsBuffer);
    ++result;
  }
  return result;
}

static char* nextArg (const char* argsBuffer) {
  int i = skipComment (argsBuffer, 0);
  while (isspace (argsBuffer[i])) {
    while (isspace (argsBuffer[i])) {
      ++i;
    }
    i = skipComment (argsBuffer, i);
  }
  return (char*) argsBuffer + i;
}

static int skipComment (const char* argsBuffer, int index) {
  if (argsBuffer[index] == '#') {
    while (!(argsBuffer[index] == '\n' | argsBuffer[index] == 0)) {
      ++index;
    }
  }
  return index;
}
