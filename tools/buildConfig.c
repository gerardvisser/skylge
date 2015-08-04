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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buildConfig.h"
#include "errors.h"
#include "fileInfo.h"
#include "stringBuilder.h"

#define COMPILER_IDX              0
#define OBJECT_FILE_EXTENSION_IDX 1
#define SOURCE_FILE_EXTENSION_IDX 2
#define CONFIG_ELEM_COUNT         3

typedef struct {
  const char* const name;
  const char* value;
  const int nameLength;
} configElem_t;

static char homeDirectory[512];
static char configFilename[512];
static configElem_t configElements[CONFIG_ELEM_COUNT];

static configElem_t* findConfigElement (const char* str);
static void initializeConfigFilename (char** env);
static void printOutputErrorMessageAndExit (void);

const char* buildConfig_compiler (void) {
  return configElements[COMPILER_IDX].value;
}

const char* buildConfig_homeDirectory (void) {
  return homeDirectory;
}

void buildConfig_init (char** env) {
  initializeConfigFilename (env);

  fileInfo_t fileInfo;
  if (fileInfo_read (&fileInfo, configFilename) == 0) {
    if (fileInfo.fileType != FILE_TYPE_REGULAR) {
      errors_printMessageAndExit ("File '%s' should be a regular file (or not exist)", configFilename);
    }
    if (fileInfo.size > 0) {
      char* config = malloc (fileInfo.size); /* Not to be freed! */
      FILE* handle = fopen (configFilename, "rb");
      if (fread (config, 1, fileInfo.size, handle) < fileInfo.size) {
        errors_printMessageAndExit ("An error occurred while reading file '%s'", configFilename);
      }
      fclose (handle);

      int i = 0;
      while (i < fileInfo.size) {
        configElem_t* elem = findConfigElement (config + i);
        if (elem == NULL || config[i + elem->nameLength] != '=') {
          errors_printMessageAndExit ("File '%s' corrupt", configFilename);
        }
        i += elem->nameLength + 1;
        char* end = memchr (config + i, '\n', fileInfo.size - i);
        if (end == NULL) {
          errors_printMessageAndExit ("File '%s' corrupt", configFilename);
        }
        *end = 0;
        elem->value = config + i;
        i += end - config - i + 1;
      }
    }
  }
}

const char* buildConfig_objectFileExtension (void) {
  return configElements[OBJECT_FILE_EXTENSION_IDX].value;
}

void buildConfig_save (void) {
  int i;
  stringBuilder_t* buffer = stringBuilder_new (1024);
  for (i = 0; i < CONFIG_ELEM_COUNT; ++i) {
    stringBuilder_append (buffer, configElements[i].name);
    stringBuilder_appendChar (buffer, '=');
    stringBuilder_append (buffer, configElements[i].value);
    stringBuilder_appendChar (buffer, '\n');
  }
  const char* bytes = stringBuilder_getBuffer (buffer);
  const int len = stringBuilder_length (buffer);

  FILE* handle = fopen (configFilename, "wb");
  if (handle == NULL) {
    printOutputErrorMessageAndExit ();
  }
  if (fwrite (bytes, 1, len, handle) < len) {
    printOutputErrorMessageAndExit ();
  }
  if (fclose (handle) != 0) {
    printOutputErrorMessageAndExit ();
  }

  stringBuilder_delete (buffer);
}

void buildConfig_set (const char* name, const char* value) {
  configElem_t* elem = findConfigElement (name);
  if (elem == NULL || name[elem->nameLength] != 0) {
    errors_printMessageAndExit ("Unknown configuration item '%s'", name);
  }
  elem->value = value;
}

const char* buildConfig_sourceFileExtension (void) {
  return configElements[SOURCE_FILE_EXTENSION_IDX].value;
}


static configElem_t configElements[CONFIG_ELEM_COUNT] = {
  {"compiler", "cc", 8},
  {"objectFileExtension", "o", 19},
  {"sourceFileExtension", "c", 19}
};

static configElem_t* findConfigElement (const char* str) {
  int i = 0;
  while (i < CONFIG_ELEM_COUNT && memcmp (str, configElements[i].name, configElements[i].nameLength) != 0) {
    ++i;
  }
  return i < CONFIG_ELEM_COUNT ? configElements + i : NULL;
}

/* In windows heet HOME: USERPROFILE. */
static void initializeConfigFilename (char** env) {
  int i = 0;
  while (!(env[i] == NULL || memcmp (env[i], "HOME=", 5) == 0)) {
    ++i;
  }
  if (env[i] == NULL) {
    errors_printMessageAndExit ("Environment variable 'HOME' not found");
  }
  strcpy (homeDirectory, env[i] + 5);
  stpcpy (stpcpy (configFilename, homeDirectory), "/.buildconfig");
}

static void printOutputErrorMessageAndExit (void) {
  switch (errno) {
  case EACCES:
    errors_printMessageAndExit ("No permission to write file '%s'", configFilename);
  case EIO:
    errors_printMessageAndExit ("A hardware error occurred while trying to write file '%s'", configFilename);
  case EISDIR:
    errors_printMessageAndExit ("'%s' is a directory", configFilename);
  case EMFILE:
    errors_printMessageAndExit ("Cannot open a stream to write '%s': too many streams open", configFilename);
  case ENOSPC:
    errors_printMessageAndExit ("No disk space left to write '%s'", configFilename);
  default:
    errors_printMessageAndExit ("An unknown error occurred while trying to write file '%s'", configFilename);
  }
}
