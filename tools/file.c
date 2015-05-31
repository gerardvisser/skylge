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
#include <sys/stat.h>
#include "errors.h"
#include "file.h"

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

typedef struct {
  time_t modificationTime;
  fileType_t fileType;
} fileInfo_t;

static void getFileInfo (fileInfo_t* fileInfo, const char* filename) {
  struct stat attrs;
  if (lstat (filename, &attrs)) {
    switch (errno) {
    case EACCES:
      errors_printMessageAndExit ("No permission to read file '%s'", filename);

    case ENOENT:
      errors_printMessageAndExit ("No file named '%s' exists", filename);

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

static file_t* file_append (file_t* this, const char* fullName) {
  if (this == NULL) {
    return file_allocAndInit (fullName);
  }
  this->next = file_allocAndInit (fullName);
  this->next->previous = this;
  return this->next;
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

time_t file_modificationTime (file_t* this) {
  return this->modificationTime;
}

const char* file_name (file_t* this) {
  return this->name;
}

int file_nameLength (file_t* this) {
  return this->nameLength;
}

file_t* file_new (const char* filename) {
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

    DIR* dir = openDirectory (filename);
    struct dirent* entry = readdir (dir);
    while (entry != NULL) {
      if (!(strcmp (entry->d_name, ".") == 0 | strcmp (entry->d_name, "..") == 0)) {
        strcpy (dirNameEnd, entry->d_name);
        file = file_append (file, fullName);
      }
      entry = readdir (dir);
    }
    closedir (dir);
    free (fullName);
    file = file_firstEntry (file);
  } break;

  case FILE_TYPE_REGULAR:
    file = file_allocAndInit (filename);
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
