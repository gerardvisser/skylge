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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buildfile.h"
#include "commandLineArgs.h"
#include "errors.h"
#include "file.h"
#include "fileInfo.h"

#define PROGRAMME_NAME    "build"
#define PROGRAMME_VERSION "2.0-SNAPSHOT"

#define freeIfNecessary(pointer) \
  if (pointer != NULL) { \
    free ((void*) pointer); \
  }

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

static void deleteBuildConfig (buildConfig_t* config);
static void initBuildConfig (buildConfig_t* config, int argc, char** args);
static void printVersionOrHelpIfRequired (int argc, char** args);

int main (int argc, char** args, char** env) {
  printVersionOrHelpIfRequired (argc, args);

  buildConfig_t config;
  initBuildConfig (&config, argc, args);


  deleteBuildConfig (&config);

  return 0;
}

static void deleteBuildConfig (buildConfig_t* config) {
  stringList_delete (config->files);
  stringList_delete (config->macros);
  stringList_delete (config->libraries);
  stringList_delete (config->libSearchPath);
  stringList_delete (config->includeSearchPath);
  freeIfNecessary (config->optimizationLevel);
  freeIfNecessary (config->objsDirectory);
  freeIfNecessary (config->libVersion);
  freeIfNecessary (config->libName);
  freeIfNecessary (config->exeName);
}

static buildfile_t* getBuildFile (stringList_t* buildfilenames) {
  if (stringList_length (buildfilenames) > 1) {
    errors_printMessageAndExit ("Only one buildfile can be specified");
  }
  return buildfile_new (buildfilenames->value);
}

static commandLineArgs_t* getCommandLineArgs (int argc, char** args) {
  commandLineArgs_t* commandLineArgs = commandLineArgs_new (argc, args, commandLineArgs_option_newBoolOption ('c'),
                                                                        commandLineArgs_option_newStringOption ('a', NULL),
                                                                        commandLineArgs_option_newStringOption ('b', BUILDFILE_DEFAULT_NAME),
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

static void initBuildConfig (buildConfig_t* config, int argc, char** args) {
  commandLineArgs_t* commandLineArgs = getCommandLineArgs (argc - 1, args + 1);
  buildfile_t* buildFile = getBuildFile (commandLineArgs_getStringOptionValue (commandLineArgs, 'b'));
  commandLineArgs_t* buildFileArgs = NULL;
  if (buildFile != NULL) {
    buildFileArgs = getCommandLineArgs (buildfile_argCount (buildFile), buildfile_arguments (buildFile));
  }
  memset (config, 0, sizeof (buildConfig_t));

  int dirNameLength;
  const char* dirName;

  /* Files defined at the commandline override files defined in the buildfile. */
  stringList_t* list = commandLineArgs_getMainArgs (commandLineArgs);
  if (list == NULL & buildFileArgs != NULL) {
    list = commandLineArgs_getMainArgs (buildFileArgs);
    dirName = buildfile_dirName (buildFile);
    dirNameLength = buildfile_dirNameLength (buildFile);
    /* Here, inhibit -a or -x from buildfile. */
  } else {
    dirName = "";
    dirNameLength = 0;
  }
  if (list == NULL) {
    errors_printMessageAndExit ("No input files");
  }



  commandLineArgs_delete (commandLineArgs);
  if (buildFile != NULL) {
    commandLineArgs_delete (buildFileArgs);
    buildfile_delete (buildFile);
  }
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
