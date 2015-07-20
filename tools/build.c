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
#include "buildConfig.h"
#include "buildOptions.h"
#include "commandGenerator.h"
#include "errors.h"
#include "file.h"
#include "objectFiles.h"

#define PROGRAMME_NAME    "build"
#define PROGRAMME_VERSION "2.0-SNAPSHOT"

static void compileFiles (stringList_t* files, const int optimizationLevel, commandGenerator_t* commandGenerator, objectFiles_t* objectFiles);
static void createExecutable (buildOptions_t* options, commandGenerator_t* commandGenerator);
static void createLibrary (buildOptions_t* options, commandGenerator_t* commandGenerator);
static void executeCommand (const char* command);
static void handleSpecialOptions (int argc, char** args);
static void printHelp (void);
static void printVersion (void);

static void printBuildConfig (void);/* TEMP */
static void printBuildOptions (buildOptions_t* options);/* TEMP */

int main (int argc, char** args, char** env) {
  if (system (NULL) == 0) {
    errors_printMessageAndExit ("No command processor available");
  }
  buildConfig_init (env);
  handleSpecialOptions (argc, args);

  buildOptions_t* options = buildOptions_new (argc, args);
  commandGenerator_t* commandGenerator = commandGenerator_new (options->objsDirectory, options->includeSearchPath, options->macros);
  executeCommand (commandGenerator_makeDirCommand (commandGenerator, options->objsDirectory));

  printBuildConfig ();/* TEMP */
  printBuildOptions (options);/* TEMP */

  /*
  if (options->libName != NULL) {
    createLibrary (options, commandGenerator);
  } else if (options->exeName != NULL) {
    createExecutable (options, commandGenerator);
  } else {
    if (options->clean) {
      executeCommand (commandGenerator_cleanCommand (commandGenerator));
    }
    objectFiles_t* objectFiles = objectFiles_new (options->objsDirectory);
    compileFiles (options->files, options->optimizationLevel, commandGenerator, objectFiles);
    objectFiles_delete (objectFiles);
  }
  */

  commandGenerator_delete (commandGenerator);
  buildOptions_delete (options);

  return 0;
}

static void compileFile (file_t* file, const int filenameMacroValueStartIndex, const int optimizationLevel, commandGenerator_t* commandGenerator, objectFiles_t* objectFiles) {
  const char* sourceFileExtension = buildConfig_sourceFileExtension ();
  while (file != NULL) {
    switch (file_type (file)) {
    case FILE_TYPE_DIRECTORY: {
      file_t* dirEntries = file_new (file_fullName (file), NULL);
      compileFile (dirEntries, filenameMacroValueStartIndex, optimizationLevel, commandGenerator, objectFiles);
      file_delete (dirEntries);
    } break;

    case FILE_TYPE_REGULAR:
      if (strcmp (file_extension (file), sourceFileExtension) == 0) {
        if (file_modificationTime (file) > objectFiles_modificationTimeOfCorrespondingObjectFile (objectFiles, file)) {
          executeCommand (commandGenerator_compileCommand (commandGenerator, file, filenameMacroValueStartIndex, optimizationLevel));
        }
      }
      break;
    }
    file = file_next (file);
  }
}

static void compileFiles (stringList_t* files, const int optimizationLevel, commandGenerator_t* commandGenerator, objectFiles_t* objectFiles) {
  while (files != NULL) {
    file_t* file = file_new (files->value, NULL);
    if (file != NULL) {
      int filenameMacroValueStartIndex = file_fullNameLength (file) - file_nameLength (file);
      compileFile (file, filenameMacroValueStartIndex, optimizationLevel, commandGenerator, objectFiles);
      file_delete (file);
    }
    files = files->next;
  }
}

static void createExecutable (buildOptions_t* options, commandGenerator_t* commandGenerator) {
    if (options->clean) {
      executeCommand (commandGenerator_cleanCommand (commandGenerator));
    }
    objectFiles_t* objectFiles = objectFiles_new (options->objsDirectory);
    compileFiles (options->files, options->optimizationLevel, commandGenerator, objectFiles);

    bool strip = options->optimizationLevel > 0;
    if (options->optimizationLevel < 0) {
      /* DEBUG_MODE:
         Voordat we gaan linken moet er eerst gekeken worden of voor de opgegeven bibliotheken ook een
         debugversie bestaat. Als dat 't geval is, dan moet de naam van de bibiotheek worden gewijzigd
         in de naam van de desbetreffende debugversie. */
    } else {
    }
    //executeCommand (commandGenerator_createExeCommand (commandGenerator, options->exeName, options->libSearchPath, libraries, strip));

    objectFiles_delete (objectFiles);
}

static void createLibrary (buildOptions_t* options, commandGenerator_t* commandGenerator) {
}

static void executeCommand (const char* command) {
  printf ("%s\n", command);
  /*
  if (system (command) != 0) {
    //Zie: 26.1 Running a Command
  }
  */
}

static void handleSpecialOptions (int argc, char** args) {
  if (argc > 1) {
    if (strcmp (args[1], "--help") == 0) {
      printHelp ();
    } else if (strcmp (args[1], "--version") == 0) {
      printVersion ();
    } else if (strcmp (args[1], "--config") == 0) {
      if (argc < 4) {
        errors_printMessageAndExit ("Two arguments expected after --config");
      }
      buildConfig_set (args[2], args[3]);
      buildConfig_save ();
      exit (EXIT_SUCCESS);
    }
  }
}

static void printHelp (void) {
  printf ("Usage: %s [options] [files]\n", PROGRAMME_NAME);
  printf ("Options:\n");
  printf ("  --config                 Specify a value for a global variable. Two more argu-\n");
  printf ("                           ments expected after this option: the variable and\n");
  printf ("                           its value. Variables that can be set, are: compiler,\n");
  printf ("                           objectFileExtension, sourceFileExtension. By default\n");
  printf ("                           their values are: cc, o, c.\n");
  printf ("  --help                   Display this information.\n");
  printf ("  --version                Display this program's version number.\n");
  printf ("  -a <libname>             Creates library <libname>. Two archives will be\n");
  printf ("                           created: lib<libname>[-<version>].a and\n");
  printf ("                           lib<libname>[-<version>]-d.a\n");
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

static void printBuildConfig (void) {
  printf ("\x1B[1mcompiler:\x1B[22m %s\n", buildConfig_compiler ());
  printf ("\x1B[1mobjectFileExtension:\x1B[22m %s\n", buildConfig_objectFileExtension ());
  printf ("\x1B[1msourceFileExtension:\x1B[22m %s\n\n", buildConfig_sourceFileExtension ());
}

static void printBuildOptions (buildOptions_t* options) {
  if (options->exeName != NULL) {
    printf ("\x1B[1mExecutable:\x1B[22m %s\n", options->exeName);
    printf ("\x1B[1mLibraries:\x1B[22m ");
    printList (options->libraries);
    printf ("\x1B[1mLibrary search path:\x1B[22m ");
    printList (options->libSearchPath);
  } else if (options->libName != NULL) {
    printf ("\x1B[1mArchives:\x1B[22m lib%s-%s.a, lib%s-%s-d.a\n", options->libName, options->libVersion, options->libName, options->libVersion);
    printf ("\x1B[1mArchive directory:\x1B[22m %s\n", options->libDirectory);
  }
  printf ("\x1B[1mOptimization level:\x1B[22m %d\n", options->optimizationLevel);
  printf ("\x1B[1mObject file directory:\x1B[22m %s\n", options->objsDirectory);
  printf ("\x1B[1mInclude search path:\x1B[22m ");
  printList (options->includeSearchPath);
  printf ("\x1B[1mMacros:\x1B[22m ");
  printList (options->macros);
  printf ("\x1B[1mFiles:\x1B[22m ");
  printList (options->files);
  printf ("\x1B[1mClean:\x1B[22m %s\n", options->clean ? "true" : "false");
}
/* END TEMPORARY  */
