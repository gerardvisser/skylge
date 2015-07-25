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

#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"
#include "file.h"
#include "fileInfo.h"

struct file_struct {
  const char* name;
  const char* fullName;
  const char* extension;
  file_t* next;
  file_t* previous;
  time_t modificationTime;
  fileType_t fileType;
  int extensionLength;
  int fullNameLength;
  int nameLength;
};

static void getFileInfo (fileInfo_t* fileInfo, const char* filename) {
  if (fileInfo_read (fileInfo, filename)) {
    errors_printMessageAndExit ("No file named '%s' exists", filename);
  }
}

static DIR* openDirectory (const char* dirName) {
  DIR* result = opendir (dirName);
  if (result == NULL) {
    switch (errno) {
    case EACCES:
      errors_printMessageAndExit ("No permission to read directory '%s'", dirName);

    case ELOOP:
      errors_printMessageAndExit ("Too many symbolic links had to be resolved while trying to look up '%s'", dirName);

    case EMFILE:
      errors_printMessageAndExit ("\x1B[7mToo many files open\x1B[27m");

    case ENOENT:
      errors_printMessageAndExit ("No directory named '%s' exists", dirName);

    case ENOMEM:
      errors_printMessageAndExit ("Not enough memory available to open directory '%s'", dirName);

    case ENOTDIR:
      errors_printMessageAndExit ("'%s' is not a directory", dirName);

    default:
      errors_printMessageAndExit ("An unknown error occurred while trying to open directory '%s'", dirName);
    }
  }
  return result;
}

static file_t* file_allocAndInit (const char* fullName) {
  fileInfo_t fileInfo;
  getFileInfo (&fileInfo, fullName);

  const int fullNameLength = strlen (fullName);
  char* fullNameCopy = malloc (fullNameLength + 1);
  strcpy (fullNameCopy, fullName);

  file_t* result = malloc (sizeof (file_t));
  result->fullName = fullNameCopy;
  result->fullNameLength = fullNameLength;
  result->fileType = fileInfo.fileType;
  result->name = strrchr (fullNameCopy, '/');
  if (result->name == NULL) {
    result->name = fullNameCopy;
  } else {
    ++result->name;
  }
  result->nameLength = strlen (result->name);
  result->previous = NULL;
  result->next = NULL;
  result->modificationTime = fileInfo.modificationTime;
  result->extension = strrchr (result->name, '.');
  if (result->extension == NULL) {
    result->extension = result->name + result->nameLength;
  } else {
    ++result->extension;
  }
  result->extensionLength = strlen (result->extension);
  return result;
}

file_t* file_append (file_t* this, file_t* file) {
  if (file == NULL) {
    return this;
  }
  if (this != NULL) {
    if (file->previous != NULL) {
      errors_printMessageAndExit ("\x1B[7m%s: The specified file already has a predecessor\x1B[27m", __func__);
    }
    if (this->next != NULL) {
      errors_printMessageAndExit ("\x1B[7m%s: Can only append to the end of a list\x1B[27m", __func__);
    }
    this->next = file;
    file->previous = this;
  }
  return file;
}

file_t* file_copy (file_t* this) {
  file_t* result = malloc (sizeof (file_t));
  char* fullNameCopy = malloc (this->fullNameLength + 1);
  strcpy (fullNameCopy, this->fullName);

  result->name = fullNameCopy + this->fullNameLength - this->nameLength;
  result->fullName = fullNameCopy;
  result->extension = fullNameCopy + this->fullNameLength - this->extensionLength;
  result->next = NULL;
  result->previous = NULL;
  result->modificationTime = this->modificationTime;
  result->fileType = this->fileType;
  result->extensionLength = this->extensionLength;
  result->fullNameLength = this->fullNameLength;
  result->nameLength = this->nameLength;
  return result;
}

void file_delete (file_t* this) {
  if (this == NULL) {
    return;
  }
  this = file_firstEntry (this);
  file_t* next;
  do {
    next = this->next;
    free ((void*) this->fullName);
    free (this);
    this = next;
  } while (this != NULL);
}

const char* file_extension (file_t* this) {
  return this->extension;
}

int file_extensionLength (file_t* this) {
  return this->extensionLength;
}

file_t* file_firstEntry (file_t* this) {
  if (this == NULL) {
    return NULL;
  }
  while (this->previous != NULL) {
    this = this->previous;
  }
  return this;
}

const char* file_fullName (file_t* this) {
  return this->fullName;
}

int file_fullNameLength (file_t* this) {
  return this->fullNameLength;
}

file_t* file_lastEntry (file_t* this) {
  if (this == NULL) {
    return NULL;
  }
  while (this->next != NULL) {
    this = this->next;
  }
  return this;
}

time_t file_modificationTime (file_t* this) {
  return this->modificationTime;
}

const char* file_name (file_t* this) {
  return this->name;
}

int file_nameLength (file_t* this) {
  return this->nameLength;
}

file_t* file_new (const char* filename, int* returnCount) {
  file_t* file = NULL;

  fileInfo_t fileInfo;
  getFileInfo (&fileInfo, filename);

  switch (fileInfo.fileType) {
  case FILE_TYPE_DIRECTORY: {
    char* fullName = malloc (8192);
    char* dirNameEnd = stpcpy (fullName, filename);
    if (dirNameEnd == fullName) {
      dirNameEnd = stpcpy (dirNameEnd, "./");
    } else if (dirNameEnd[-1] != '/') {
      dirNameEnd = stpcpy (dirNameEnd, "/");
    }

    int count = 0;
    DIR* dir = openDirectory (filename);
    struct dirent* entry = readdir (dir);
    while (entry != NULL) {
      if (!(strcmp (entry->d_name, ".") == 0 | strcmp (entry->d_name, "..") == 0)) {
        strcpy (dirNameEnd, entry->d_name);
        file = file_append (file, file_allocAndInit (fullName));
        ++count;
      }
      entry = readdir (dir);
    }
    closedir (dir);
    free (fullName);
    file = file_firstEntry (file);
    if (returnCount != NULL) {
      *returnCount = count;
    }
  } break;

  case FILE_TYPE_REGULAR:
    file = file_allocAndInit (filename);
    if (returnCount != NULL) {
      *returnCount = 1;
    }
    break;

  default:
    errors_printMessageAndExit ("File '%s' is not a regular file or a directory", filename);
    break;
  }

  return file;
}

file_t* file_next (file_t* this) {
  return this->next;
}

file_t* file_previous (file_t* this) {
  return this->previous;
}

fileType_t file_type (file_t* this) {
  return this->fileType;
}
