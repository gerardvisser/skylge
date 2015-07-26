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
#include "file.h"
#include "libraries.h"

typedef struct {
  char* name;
  int nameLength;
} archive_t;

struct libraries_struct {
  stringBuilder_t* archiveName;
  archive_t* archives;
  int archiveCount;
};

static int compareArchives (const archive_t* archive1, const archive_t* archive2) {
  return memcmp (archive1->name, archive2->name, archive1->nameLength + 1);
}

static int compareStringToArchive (const char* name, const archive_t* archive) {
  return memcmp (name, archive->name, archive->nameLength + 1);
}

void libraries_delete (libraries_t* this) {
  if (this->archiveCount > 0) {
    int i;
    for (i = 0; i < this->archiveCount; ++i) {
      free (this->archives[i].name);
    }
    free (this->archives);
  }
  stringBuilder_delete (this->archiveName);
  free (this);
}

bool libraries_exists (libraries_t* this, const char* libName) {
  stringBuilder_clear (this->archiveName);
  libraries_filenameForName (this->archiveName, libName);
  const char* archiveName = stringBuilder_getBuffer (this->archiveName);

  int l = -1;
  int h = this->archiveCount;
  int i = (int) ((l + h) / 2.0 + 0.5);
  while (l < i & i < h) {
    int cmpResult = compareStringToArchive (archiveName, this->archives + i);
    if (cmpResult == 0) {
      return true;
    } else if (cmpResult > 0) {
      l = i;
    } else {
      h = i;
    }
    i = (int) ((l + h) / 2.0 + 0.5);
  }
  return false;
}

void libraries_filenameForName (stringBuilder_t* filename, const char* libName) {
  stringBuilder_appendChars (filename, "lib", 3);
  stringBuilder_append (filename, libName);
  stringBuilder_appendChars (filename, ".a", 2);
}

libraries_t* libraries_new (stringList_t* libSearchPath) {
  int count = 0;
  file_t* files = NULL;
  file_t* filesTail = NULL;
  while (libSearchPath != NULL) {
    int deltaCount;
    file_t* subset = file_new (libSearchPath->value, &deltaCount);
    filesTail = file_append (filesTail, subset);
    filesTail = file_lastEntry (filesTail);
    if (files == NULL) {
      files = subset;
    }
    count += deltaCount;
    libSearchPath = libSearchPath->next;
  }

  libraries_t* result = malloc (sizeof (libraries_t));
  if (count > 0) {
    result->archives = malloc (sizeof (archive_t) * count);

    count = 0;
    file_t* file = files;
    while (file != NULL) {
      if (strcmp (file_extension (file), "a") == 0) {
        int nameLen = file_nameLength (file);
        result->archives[count].nameLength = nameLen;
        result->archives[count].name = malloc (nameLen + 1);
        memcpy (result->archives[count].name, file_name (file), nameLen + 1);
        ++count;
      }
      file = file_next (file);
    }
    file_delete (files);

    if (count > 0) {
      qsort (result->archives, count, sizeof (void*), (__compar_fn_t) compareArchives);
    } else {
      free (result->archives);
    }
  }

  result->archiveName = stringBuilder_new (256);
  result->archiveCount = count;
  return result;
}
