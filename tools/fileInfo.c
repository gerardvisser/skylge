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

#include <errno.h>
#include <sys/stat.h>
#include "fileInfo.h"

#define NO_ERROR            0
#define FILE_DOES_NOT_EXIST 1

int fileInfo_read (fileInfo_t* fileInfo, const char* filename) {
  struct stat attrs;
  int errorCode = NO_ERROR;
  if (lstat (filename, &attrs)) {
    switch (errno) {
    case EACCES:
      errors_printMessageAndExit ("No permission to read file '%s'", filename);

    case ENOENT:
      errorCode = FILE_DOES_NOT_EXIST;
      break;

    case ENOMEM:
      errors_printMessageAndExit ("Not enough memory available to read the attributes of file '%s'", filename);

    default:
      errors_printMessageAndExit ("An unknown error occurred while trying to read the attributes of file '%s'", filename);
    }
  }
  if (S_ISDIR (attrs.st_mode)) {
    fileInfo->fileType = FILE_TYPE_DIRECTORY;
  } else if (S_ISREG (attrs.st_mode)) {
    fileInfo->fileType = FILE_TYPE_REGULAR;
  } else if (S_ISLNK (attrs.st_mode)) {
    fileInfo->fileType = FILE_TYPE_SYMLINK;
  } else {
    fileInfo->fileType = FILE_TYPE_UNKNOWN;
  }
  fileInfo->modificationTime = attrs.st_mtime;
  fileInfo->size = attrs.st_size;
  return errorCode;
}
