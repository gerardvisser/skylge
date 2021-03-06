/*
   Author:  Gerard Visser
   e-mail:  visser.gerard(at)gmail.com

   Copyright (C) 2015, 2016 Gerard Visser.

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
#include "devutils.h"
#include "errors.h"
#include "file.h"
#include "libraries.h"
#include "objectFiles.h"
#include "programmeInfo.h"

static bool dryRun;

static void compileFiles (stringList_t* files, const int optimizationLevel, commandGenerator_t* commandGenerator, objectFiles_t* objectFiles);
static void createExecutable (buildOptions_t* options, commandGenerator_t* commandGenerator);
static void createLibrary (buildOptions_t* options, commandGenerator_t* commandGenerator);
static void executeCommand (const char* command, bool exitOnFailure);
static void handleSpecialOptions (int argc, char** args);
static void printHelp (void);
static void printVersion (void);

int main (int argc, char** args, char** env) {
  if (system (NULL) == 0) {
    errors_printMessageAndExit ("No command processor available");
  }
  buildConfig_init (env);
  handleSpecialOptions (argc, args);

  buildOptions_t* options = buildOptions_new (argc, args);
  commandGenerator_t* commandGenerator = commandGenerator_new (options->objsDirectory, options->includeSearchPath, options->macros);

  dryRun = options->dryRun;
  if (options->showConfig) {
    devutils_printBuildConfig ();
  }
  if (options->showJobDescription) {
    devutils_printBuildOptions (options);
  }

  executeCommand (commandGenerator_makeDirCommand (commandGenerator, options->objsDirectory), true);
  if (options->libName != NULL) {
    createLibrary (options, commandGenerator);
  } else if (options->exeName != NULL) {
    createExecutable (options, commandGenerator);
  } else {
    if (options->clean) {
      executeCommand (commandGenerator_cleanCommand (commandGenerator), false);
    }
    objectFiles_t* objectFiles = objectFiles_new (options->objsDirectory);
    compileFiles (options->files, options->optimizationLevel, commandGenerator, objectFiles);
    objectFiles_delete (objectFiles);
  }

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
          executeCommand (commandGenerator_compileCommand (commandGenerator, file, filenameMacroValueStartIndex, optimizationLevel), true);
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
    executeCommand (commandGenerator_cleanCommand (commandGenerator), false);
  }
  objectFiles_t* objectFiles = objectFiles_new (options->objsDirectory);
  compileFiles (options->files, options->optimizationLevel, commandGenerator, objectFiles);

  stringList_t* libs = NULL;
  bool strip = options->optimizationLevel > 0;
  if (options->optimizationLevel < 0 & !(options->libSearchPath == NULL | options->libraries == NULL)) {
    /* DEBUG_MODE: change all library names for which a debug version can be found. */
    stringList_t* specifiedLibs = options->libraries;
    stringBuilder_t* debugLibName = stringBuilder_new (256);
    libraries_t* libraries = libraries_new (options->libSearchPath);
    while (specifiedLibs != NULL) {
      stringBuilder_appendChars (debugLibName, specifiedLibs->value, specifiedLibs->valueLength);
      stringBuilder_appendChars (debugLibName, "-d", 2);
      const char* debugName = stringBuilder_getBuffer (debugLibName);
      if (libraries_exists (libraries, debugName)) {
        libs = stringList_append (libs, debugName);
      } else {
        libs = stringList_append (libs, specifiedLibs->value);
      }
      stringBuilder_clear (debugLibName);
      specifiedLibs = specifiedLibs->next;
    }
    libs = stringList_firstElement (libs);
    stringBuilder_delete (debugLibName);
    libraries_delete (libraries);
  } else {
    libs = options->libraries;
  }
  executeCommand (commandGenerator_createExeCommand (commandGenerator, options->exeName, options->libSearchPath, libs, strip), true);

  if (libs != options->libraries) {
    stringList_delete (libs);
  }
  objectFiles_delete (objectFiles);
}

static void createLibrary (buildOptions_t* options, commandGenerator_t* commandGenerator) {
  executeCommand (commandGenerator_makeDirCommand (commandGenerator, options->libDirectory), true);

  stringBuilder_t* name = stringBuilder_new (64);
  stringBuilder_append (name, options->libName);
  stringBuilder_appendChar (name, '-');
  stringBuilder_append (name, options->libVersion);
  if (options->snapshot) {

    if (options->optimizationLevel < 0) {
      stringBuilder_appendChars (name, "-d", 2);
    }
    if (options->clean) {
      executeCommand (commandGenerator_cleanCommand (commandGenerator), false);
    }
    objectFiles_t* objectFiles = objectFiles_new (options->objsDirectory);
    compileFiles (options->files, options->optimizationLevel, commandGenerator, objectFiles);
    executeCommand (commandGenerator_createArchiveCommand (commandGenerator, options->libDirectory, stringBuilder_getBuffer (name)), true);
    objectFiles_delete (objectFiles);

  } else {

    int optimizationLevel = options->optimizationLevel > -1 ? options->optimizationLevel : 0;
    executeCommand (commandGenerator_cleanCommand (commandGenerator), false);
    objectFiles_t* objectFiles = objectFiles_new (options->objsDirectory);
    compileFiles (options->files, optimizationLevel, commandGenerator, objectFiles);
    executeCommand (commandGenerator_createArchiveCommand (commandGenerator, options->libDirectory, stringBuilder_getBuffer (name)), true);
    stringBuilder_appendChars (name, "-d", 2);
    executeCommand (commandGenerator_cleanCommand (commandGenerator), false);
    compileFiles (options->files, -1, commandGenerator, objectFiles);
    executeCommand (commandGenerator_createArchiveCommand (commandGenerator, options->libDirectory, stringBuilder_getBuffer (name)), true);
    objectFiles_delete (objectFiles);

  }
  stringBuilder_delete (name);
}

static void executeCommand (const char* command, bool exitOnFailure) {
  printf ("%s\n", command);
  if (!dryRun) {
    int status = system (command);
    if (status != 0) {
      if (status == -1) {
        errors_printMessageAndExit ("\nCould not create a shell process to execute the command");
      } else if (exitOnFailure) {
        exit (EXIT_FAILURE);
      }
    }
  }
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
  printf ("                           its value. Variables that can be set, are:\n");
  printf ("                            compiler, objectFileExtension, sourceFileExtension\n");
  printf ("                           Their default values are:\n");
  printf ("                            cc, o, c\n");
  printf ("  --help                   Display this information.\n");
  printf ("  --version                Display this programme's version number.\n");
  printf ("  -a <libname>             Creates library <libname>.\n");
  printf ("  -b <file>                Specifies the buildfile to read.\n");
  printf ("  -c                       Clean.\n");
  printf ("  -D <macro>[=<val>]       Define a <macro> with <val> as its value. If just\n");
  printf ("                           <macro> is given, <val> is taken to be 1.\n");
  printf ("  -I <directory>           Add <directory> to the path to search for headers to\n");
  printf ("                           include.\n");
  printf ("  -L <directory>           In combination with option -x, <directory> is added\n");
  printf ("                           to the library search path. In combination with -a\n");
  printf ("                           it is the location where to put the created library.\n");
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
  printf ("Copyright (C) %s Gerard Visser.\n", PROGRAMME_CRYEARS);
  printf ("This is free software; see the source for copying conditions.  There is NO\n");
  printf ("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\n");
  exit (EXIT_SUCCESS);
}
