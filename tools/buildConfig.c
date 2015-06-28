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
#include "buildfile.h"
#include "commandLineArgs.h"
#include "errors.h"

#define BUFFER_SIZE 256

#define freeIfNecessary(pointer) \
  if (pointer != NULL) { \
    free ((void*) pointer); \
  }

static buildfile_t* getBuildFile (stringList_t* buildfilenames);
static commandLineArgs_t* getCommandLineArgs (int argc, char** args);
static stringList_t* getFiles (bool* inhibitAorXfromBuildfile, commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs, buildfile_t* buildFile);

void buildConfig_delete (buildConfig_t* this) {
  stringList_delete (this->files);
  stringList_delete (this->macros);
  stringList_delete (this->libraries);
  stringList_delete (this->libSearchPath);
  stringList_delete (this->includeSearchPath);
  freeIfNecessary (this->optimizationLevel);
  freeIfNecessary (this->objsDirectory);
  freeIfNecessary (this->libVersion);
  freeIfNecessary (this->libName);
  freeIfNecessary (this->exeName);
  free (this);
}

/*
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
*/
buildConfig_t* buildConfig_new (int argc, char** args) {
  commandLineArgs_t* commandLineArgs = getCommandLineArgs (argc - 1, args + 1);
  buildfile_t* buildFile = getBuildFile (commandLineArgs_getStringOptionValue (commandLineArgs, 'b'));
  commandLineArgs_t* buildFileArgs = NULL;
  if (buildFile != NULL) {
    buildFileArgs = getCommandLineArgs (buildfile_argCount (buildFile), buildfile_arguments (buildFile));
  }

  buildConfig_t* result = malloc (sizeof (buildConfig_t));
  memset (result, 0, sizeof (buildConfig_t));

  bool inhibitAorXfromBuildfile;
  result->files = getFiles (&inhibitAorXfromBuildfile, commandLineArgs, buildFileArgs, buildFile);




  commandLineArgs_delete (commandLineArgs);
  if (buildFile != NULL) {
    commandLineArgs_delete (buildFileArgs);
    buildfile_delete (buildFile);
  }
  return result;
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

static stringList_t* getFiles (bool* inhibitAorXfromBuildfile, commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs, buildfile_t* buildFile) {
  /* Files defined at the commandline override files defined in the buildfile. */
  int dirNameLength;
  const char* dirName;
  stringList_t* list = commandLineArgs_getMainArgs (commandLineArgs);
  *inhibitAorXfromBuildfile = list == NULL & buildFileArgs != NULL;
  if (*inhibitAorXfromBuildfile) {
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

  char buf[BUFFER_SIZE];
  stringList_t* result = NULL;

  while (list != NULL) {
    int len = strlen (list->value) + 1;
    char* name = dirNameLength + len > BUFFER_SIZE ? malloc (dirNameLength + len) : buf;
    //mempcpy (mempcpy (name, dirName, dirNameLength), list->value, len);
    memcpy (name, dirName, dirNameLength);
    memcpy (name + dirNameLength, list->value, len);
    result = stringList_append (result, name);

    if (name != buf) {
      free (name);
    }
    list = list->next;
  }

  return result;
}
