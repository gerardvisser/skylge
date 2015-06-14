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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commandLineArgs.h"
#include "errors.h"
#include "file.h"
#include "fileInfo.h"

#define DEFAULT_CONFIG_FILE "buildfile"
#define PROGRAMME_NAME      "build"
#define PROGRAMME_VERSION   "2.0-SNAPSHOT"

typedef struct {
  stringList_t* files;
  stringList_t* macros;
  stringList_t* libraries;
  stringList_t* libSearchPath;
  stringList_t* includeSearchPath;
  const char* optimizationLevel;
  const char* objsDirectory;
  const char* libVersion;
  const char* libName;
  const char* exeName;
} buildConfig_t;

static commandLineArgs_t* getBuildFileArgs (stringList_t* buildfilenames);
static commandLineArgs_t* getCommandLineArgs (int argc, char** args);
static void printVersionOrHelpIfRequired (int argc, char** args);

int main (int argc, char** args, char** env) {
  printVersionOrHelpIfRequired (argc, args);
  commandLineArgs_t* commandLineArgs = getCommandLineArgs (argc, args);
  commandLineArgs_t* buildFileArgs = getBuildFileArgs (commandLineArgs_getStringOptionValue (commandLineArgs, 'b'));

  buildConfig_t config;

  commandLineArgs_delete (commandLineArgs);
  if (buildFileArgs != NULL) {
    commandLineArgs_delete (buildFileArgs);
  }

  return 0;
}

static commandLineArgs_t* getCommandLineArgs (int argc, char** args) {
  commandLineArgs_t* commandLineArgs = commandLineArgs_new (argc, args, commandLineArgs_option_newBoolOption ('c'),
                                                                        commandLineArgs_option_newStringOption ('a', NULL),
                                                                        commandLineArgs_option_newStringOption ('b', DEFAULT_CONFIG_FILE),
                                                                        commandLineArgs_option_newStringOption ('D', NULL),
                                                                        commandLineArgs_option_newStringOption ('I', NULL),
                                                                        commandLineArgs_option_newStringOption ('L', NULL),
                                                                        commandLineArgs_option_newStringOption ('l', NULL),
                                                                        commandLineArgs_option_newStringOption ('O', NULL),
                                                                        commandLineArgs_option_newStringOption ('o', "objs"),
                                                                        commandLineArgs_option_newStringOption ('V', NULL),
                                                                        commandLineArgs_option_newStringOption ('x', NULL),
                                                                        NULL);
  return commandLineArgs;
}

static void printHelp (void) {
  printf ("Usage: %s [options] [files]\n", PROGRAMME_NAME);
  printf ("Options:\n");
  printf ("  --help                   Display this information.\n");
  printf ("  --version                Display this program's version number.\n");
  printf ("  -a <libname>             Creates library <libname>. Two archives will be\n");
  printf ("                           created: lib<libname>[-<version>].a and\n");
  printf ("                           lib<libname>-d[-<version>].a\n");
  printf ("  -b <file>                Specifies the buildfile to read.\n");
  printf ("  -c                       Clean.\n");
  printf ("  -D <macro>[=<val>]       Define a <macro> with <val> as its value. If just\n");
  printf ("                           <macro> is given, <val> is taken to be 1.\n");
  printf ("  -I <directory>           Add <directory> to the path to search for headers to\n");
  printf ("                           include.\n");
  printf ("  -L <directory>           Add <directory> to the library search path.\n");
  printf ("  -l <libname>             Link with library <libname>.\n");
  printf ("  -O <level>               Set optimization level. Possible values are: 0, 1, 2,\n");
  printf ("                           3, fast, s.\n");
  printf ("  -o <directory>           Place the object files in <directory>.\n");
  printf ("  -V <version>             Specifies the version of the library being created:\n");
  printf ("                           used in combination with option -a.\n");
  printf ("  -x <file>                Creates an executable <file>.\n");
  printf ("\n");
  exit (EXIT_SUCCESS);
}

static void printVersion (void) {
  printf ("%s %s\n", PROGRAMME_NAME, PROGRAMME_VERSION);
  printf ("Copyright (C) 2015 Gerard Visser.\n");
  printf ("This is free software; see the source for copying conditions.  There is NO\n");
  printf ("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\n");
  exit (EXIT_SUCCESS);
}

static void printVersionOrHelpIfRequired (int argc, char** args) {
  if (argc > 1) {
    if (strcmp (args[1], "--help") == 0) {
      printHelp ();
    } else if (strcmp (args[1], "--version") == 0) {
      printVersion ();
    }
  }
}

/******************************************************************************/
/****************   B U I L D   F I L E   A R G U M E N T S   *****************/
/******************************************************************************/

static int getArgCount (const char* argsBuffer);
static char* nextArg (const char* argsBuffer);
static int skipComment (const char* argsBuffer, int index);

static void createArgsArray (int* argc, char*** args, char* argsBuffer) {
  *argc = getArgCount (argsBuffer);
  *args = malloc (sizeof (void*) * (*argc + 1));
  *args[*argc] = NULL;
  int i = 0;
  argsBuffer = nextArg (argsBuffer);
  while (*argsBuffer != '0') {
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
  while (*argsBuffer != '0') {
    while (!(isspace (*argsBuffer) | *argsBuffer == 0)) {
      ++argsBuffer;
    }
    argsBuffer = nextArg (argsBuffer);
    ++result;
  }
  return result;
}

/* Returns NULL if no extra arguments are found. */
static commandLineArgs_t* getBuildFileArgs (stringList_t* buildfilenames) {
  if (stringList_length (buildfilenames) > 1) {
    errors_printMessageAndExit ("Only one buildfile can be specified");
  }
  fileInfo_t fileInfo;
  if (fileInfo_read (&fileInfo, buildfilenames->value) == 0) {
    if (fileInfo.fileType != FILE_TYPE_REGULAR) {
      errors_printMessageAndExit ("'%s' is not a regular file", buildfilenames->value);
    }
    if (fileInfo.size == 0) {
      return NULL;
    }

    char* argsBuffer = malloc (fileInfo.size + 1);
    /* What if no permission to read this file? */
    FILE* handle = fopen (buildfilenames->value, "rb");
    fread (argsBuffer, 1, fileInfo.size, handle);
    fclose (handle);
    argsBuffer[fileInfo.size] = 0;

    int argc;
    char** args;
    createArgsArray (&argc, &args, argsBuffer);

    commandLineArgs_t* commandLineArgs = NULL;
    if (argc > 1) {
      /* Make sure first argument is dummy!!! */
    }

    free (args);
    free (argsBuffer);
    return commandLineArgs;
  } else if (strcmp (buildfilenames->value, DEFAULT_CONFIG_FILE) == 0) {
    return NULL;
  } else {
    errors_printMessageAndExit ("No file named '%s' exists", buildfilenames->value);
  }
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
