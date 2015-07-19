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
#include "buildConfig.h"
#include "objectFiles.h"

typedef struct {
  const char* basename;
  time_t modificationTime;
  int basenameLength;
} objFile_t;

struct objectFiles_struct {
  objFile_t* objFiles;
  int count;
  int sourceFileExtLenPlusOne;
};

static int compareFileToObjFile (file_t* sourceFile, const objFile_t* objFile, int sourceFileExtLenPlusOne) {
  int sourceFileBasenameLen = file_nameLength (sourceFile) - sourceFileExtLenPlusOne;
  int cmpResult = memcmp (file_name (sourceFile), objFile->basename, sourceFileBasenameLen);
  if (sourceFileBasenameLen < objFile->basenameLength & cmpResult == 0) {
    cmpResult = -1;
  }
  return cmpResult;
}

static int compareObjFiles (const objFile_t* obj1, const objFile_t* obj2) {
  int len = obj1->basenameLength > obj2->basenameLength ? obj1->basenameLength : obj2->basenameLength;
  return memcmp (obj1->basename, obj2->basename, len);
}

void objectFiles_delete (objectFiles_t* this) {
  if (this->count > 0) {
    int i;
    for (i = 0; i < this->count; ++i) {
      free ((char*) this->objFiles[i].basename);
    }
    free (this->objFiles);
  }
  free (this);
}

time_t objectFiles_modificationTimeOfCorrespondingObjectFile (objectFiles_t* this, file_t* sourceFile) {
  int l = -1;
  int h = this->count;
  int i = (int) ((l + h) / 2.0 + 0.5);
  while (l < i & i < h) {
    int cmpResult = compareFileToObjFile (sourceFile, this->objFiles + i, this->sourceFileExtLenPlusOne);
    if (cmpResult == 0) {
      return this->objFiles[i].modificationTime;
    } else if (cmpResult > 0) {
      l = i;
    } else {
      h = i;
    }
    i = (int) ((l + h) / 2.0 + 0.5);
  }
  return 0;
}

objectFiles_t* objectFiles_new (const char* objsDirectory) {
  int count;
  objFile_t* objFiles;
  file_t* files = file_new (objsDirectory, &count);
  if (count > 0) {
    objFiles = malloc (count * sizeof (objFile_t));
    const char* const objFileExtension = buildConfig_objectFileExtension ();
    const int objFileExtensionLength = strlen (objFileExtension);

    count = 0;
    file_t* file = files;
    while (file != NULL) {
      if (strcmp (file_extension (file), objFileExtension) == 0) {
        int basenameLength = file_nameLength (file) - objFileExtensionLength - 1;
        char* basename = malloc (basenameLength + 1);
        memcpy (basename, file_name (file), basenameLength);
        basename[basenameLength] = 0;
        objFiles[count].basename = basename;
        objFiles[count].basenameLength = basenameLength;
        objFiles[count].modificationTime = file_modificationTime (file);
        ++count;
      }
      file = file_next (file);
    }
    file_delete (files);
    if (count > 0) {
      qsort (objFiles, count, sizeof (objFile_t), (__compar_fn_t) compareObjFiles);
    } else {
      free (objFiles);
    }
  }

  objectFiles_t* result = malloc (sizeof (objectFiles_t));
  result->sourceFileExtLenPlusOne = strlen (buildConfig_sourceFileExtension ()) + 1;
  result->objFiles = objFiles;
  result->count = count;
  return result;
}
