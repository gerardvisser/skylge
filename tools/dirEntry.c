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
#include "dirEntry.h"
#include "errors.h"

struct dirEntry_struct {
  const char* name;
  const char* extension;
  dirEntry_t* next;
  dirEntry_t* previous;
  fileType_t fileType;
  int nameLength;
};

static fileType_t getFileType (const char* filename) {
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
    return FILE_TYPE_DIRECTORY;
  } else if (S_ISREG (attrs.st_mode)) {
    return FILE_TYPE_REGULAR;
  } else if (S_ISLNK (attrs.st_mode)) {
    return FILE_TYPE_SYMLINK;
  } else {
    return FILE_TYPE_UNKNOWN;
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

static dirEntry_t* dirEntry_new (const char* name, fileType_t fileType) {
  dirEntry_t* result = malloc (sizeof (dirEntry_t));
  result->nameLength = strlen (name);
  char* nameCopy = malloc (result->nameLength + 1);
  strcpy (nameCopy, name);
  result->fileType = fileType;
  result->name = nameCopy;
  result->previous = NULL;
  result->next = NULL;
  result->extension = strrchr (nameCopy, '.');
  if (result->extension == NULL) {
    result->extension = nameCopy + result->nameLength;
  } else {
    ++result->extension;
  }
  return result;
}

static dirEntry_t* dirEntry_append (dirEntry_t* this, const char* name, fileType_t fileType) {
  if (this == NULL) {
    return dirEntry_new (name, fileType);
  }
  this->next = dirEntry_new (name, fileType);
  this->next->previous = this;
  return this->next;
}

void dirEntry_delete (dirEntry_t* this) {
  if (this == NULL) {
    return;
  }
  this = dirEntry_firstEntry (this);
  dirEntry_t* next;
  do {
    next = this->next;
    free ((void*) this->name);
    free (this);
    this = next;
  } while (this != NULL);
}

const char* dirEntry_extension (dirEntry_t* this) {
  return this->extension;
}

fileType_t dirEntry_fileType (dirEntry_t* this) {
  return this->fileType;
}

dirEntry_t* dirEntry_firstEntry (dirEntry_t* this) {
  if (this == NULL) {
    return NULL;
  }
  while (this->previous != NULL) {
    this = this->previous;
  }
  return this;
}

const char* dirEntry_name (dirEntry_t* this) {
  return this->name;
}

int dirEntry_nameLength (dirEntry_t* this) {
  return this->nameLength;
}

dirEntry_t* dirEntry_next (dirEntry_t* this) {
  return this->next;
}

dirEntry_t* dirEntry_previous (dirEntry_t* this) {
  return this->previous;
}

dirEntry_t* dirEntry_readDir (const char* dirName) {
  char* fullName = malloc (8192);
  char* dirNameEnd = stpcpy (fullName, dirName);
  if (dirNameEnd != fullName && dirNameEnd[-1] != '/') {
    dirNameEnd[0] = '/';
    dirNameEnd[1] = 0;
    ++dirNameEnd;
  }

  dirEntry_t* entryList = NULL;
  DIR* dir = openDirectory (dirName);
  struct dirent* entry = readdir (dir);
  while (entry != NULL) {
    if (!(strcmp (entry->d_name, ".") == 0 | strcmp (entry->d_name, "..") == 0)) {
      strcpy (dirNameEnd, entry->d_name);
      fileType_t fileType = getFileType (fullName);
      entryList = dirEntry_append (entryList, entry->d_name, fileType);
    }
    entry = readdir (dir);
  }
  closedir (dir);
  free (fullName);
  entryList = dirEntry_firstEntry (entryList);
  return entryList;
}
