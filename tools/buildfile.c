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
#include <string.h>
#include "buildfile.h"
#include "errors.h"
#include "fileInfo.h"

struct buildfile_struct {
  const char* filename;
};

void buildfile_delete (buildfile_t* this) {
}

buildfile_t* buildfile_new (const char* filename) {
  fileInfo_t fileInfo;
  if (fileInfo_read (&fileInfo, filename) == 0) {
    char* dirnameEnd;
    int len = strlen (filename);
    char* name = malloc (len + strlen (BUILDFILE_DEFAULT_NAME) + 2); /* NAME ALLOCATED */
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


  } else if (strcmp (filename, BUILDFILE_DEFAULT_NAME) == 0) {
    return NULL;
  } else {
    errors_printMessageAndExit ("No file named '%s' exists", filename);
  }
}
