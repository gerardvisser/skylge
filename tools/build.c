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
#include "buildConfig.h"
/*
#include "errors.h"
#include "file.h"
#include "fileInfo.h"
*/

#define PROGRAMME_NAME    "build"
#define PROGRAMME_VERSION "2.0-SNAPSHOT"

static void printBuildConfig (buildConfig_t* config);/* TEMP */
static void printVersionOrHelpIfRequired (int argc, char** args);

int main (int argc, char** args, char** env) {
  printVersionOrHelpIfRequired (argc, args);

  buildConfig_t* config = buildConfig_new (argc, args);

  printBuildConfig (config);/* TEMP */

  buildConfig_delete (config);

  return 0;
}

/* TEMPORARY  */
static void printList (stringList_t* list) {
  if (list != NULL) {
    printf ("%s", list->value);
    list = list->next;
    while (list != NULL) {
      printf (", %s", list->value);
      list = list->next;
    }
  }
  printf ("\n");
}

static void printBuildConfig (buildConfig_t* config) {
  if (config->exeName != NULL) {
    printf ("\x1B[1mExecutable:\x1B[22m %s\n", config->exeName);
    printf ("\x1B[1mLibraries:\x1B[22m ");
    printList (config->libraries);
    printf ("\x1B[1mLibrary search path:\x1B[22m ");
    printList (config->libSearchPath);
  } else if (config->libName != NULL) {
    printf ("\x1B[1mArchive:\x1B[22m lib%s-%s.a\n", config->libName, config->libVersion);
    printf ("\x1B[1mArchive directory:\x1B[22m %s\n", config->libDirectory);
  }
  printf ("\x1B[1mObject file directory:\x1B[22m %s\n", config->objsDirectory);
  printf ("\x1B[1mInclude search path:\x1B[22m ");
  printList (config->includeSearchPath);
  printf ("\x1B[1mMacros:\x1B[22m ");
  printList (config->macros);
  printf ("\x1B[1mFiles:\x1B[22m ");
  printList (config->files);
  printf ("\x1B[1mClean:\x1B[22m %s\n", config->clean ? "true" : "false");
}
/* END TEMPORARY  */

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
