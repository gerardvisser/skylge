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
static const char* getExeName (commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs, buildfile_t* buildFile, bool inhibitAorXfromBuildfile);
static stringList_t* getFiles (bool* inhibitAorXfromBuildfile, commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs, buildfile_t* buildFile);
static stringList_t* getIncludeSearchPath (commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs, buildfile_t* buildFile);
static const char* getLibName (commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs, bool inhibitAorXfromBuildfile);
static const char* getObjsDirectory (commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs, buildfile_t* buildFile);

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
  result->exeName = getExeName (commandLineArgs, buildFileArgs, buildFile, inhibitAorXfromBuildfile);
  result->libName = getLibName (commandLineArgs, buildFileArgs, inhibitAorXfromBuildfile);
  if (!(result->exeName == NULL | result->libName == NULL)) {
    errors_printMessageAndExit ("Should an archive or an executable be created (cannot create both)?");
  }
  result->objsDirectory = getObjsDirectory (commandLineArgs, buildFileArgs, buildFile);


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
                                                                        commandLineArgs_option_newStringOption ('o', NULL),
                                                                        commandLineArgs_option_newStringOption ('V', NULL),
                                                                        commandLineArgs_option_newStringOption ('x', NULL),
                                                                        NULL);
  return commandLineArgs;
}

static const char* getExeName (commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs, buildfile_t* buildFile, bool inhibitAorXfromBuildfile) {
  int dirNameLength;
  const char* dirName;
  stringList_t* list = commandLineArgs_getStringOptionValue (commandLineArgs, 'x');
  if (list == NULL & buildFileArgs != NULL & !inhibitAorXfromBuildfile) {
    list = commandLineArgs_getStringOptionValue (buildFileArgs, 'x');
    dirNameLength = buildfile_dirNameLength (buildFile);
    dirName = buildfile_dirName (buildFile);
  } else {
    dirNameLength = 0;
    dirName = "";
  }
  if (list != NULL) {
    if (stringList_length (list) > 1) {
      errors_printMessageAndExit ("Only one executable target can be specified");
    }
    int len = strlen (list->value) + 1;
    char* result = malloc (dirNameLength + len);
    memcpy (result, dirName, dirNameLength);
    memcpy (result + dirNameLength, list->value, len);
    return result;
  }
  return NULL;
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
  result = stringList_firstElement (result);
  return result;
}

static stringList_t* getIncludeSearchPath (commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs, buildfile_t* buildFile) {
  /* TODO: implementeren. Eigenlijk moet je hier alles al aan elkaar plakken en const char* teruggeven.  */

  stringList_t* result = NULL;
  stringList_t* list = commandLineArgs_getStringOptionValue (commandLineArgs, 'I');
  while (list != NULL) {
    result = stringList_append (result, list->value);
    list = list->next;
  }
  if (buildFileArgs != NULL) {
    list = commandLineArgs_getStringOptionValue (buildFileArgs, 'I');
    int dirNameLength = buildfile_dirNameLength (buildFile);
    const char* dirName = buildfile_dirName (buildFile);
    /* En nu die er nog bij... */
  }


  result = stringList_firstElement (result);
  return result;
}

static const char* getLibName (commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs, bool inhibitAorXfromBuildfile) {
  stringList_t* list = commandLineArgs_getStringOptionValue (commandLineArgs, 'a');
  if (list == NULL & buildFileArgs != NULL & !inhibitAorXfromBuildfile) {
    list = commandLineArgs_getStringOptionValue (buildFileArgs, 'a');
  }
  if (list != NULL) {
    if (stringList_length (list) > 1) {
      errors_printMessageAndExit ("Only one archive target can be specified");
    }
    int len = strlen (list->value) + 1;
    char* result = malloc (len);
    memcpy (result, list->value, len);
    return result;
  }
  return NULL;
}

static const char* getObjsDirectory (commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs, buildfile_t* buildFile) {
  int dirNameLength;
  const char* dirName;
  stringList_t* list = commandLineArgs_getStringOptionValue (commandLineArgs, 'o');
  if (list == NULL & buildFileArgs != NULL) {
    list = commandLineArgs_getStringOptionValue (buildFileArgs, 'o');
    dirNameLength = buildfile_dirNameLength (buildFile);
    dirName = buildfile_dirName (buildFile);
  } else {
    dirNameLength = 0;
    dirName = "";
  }
  const char* name;
  if (list != NULL) {
    if (stringList_length (list) > 1) {
      errors_printMessageAndExit ("Only one object file destination can be specified");
    }
    name = list->value;
  } else {
    name = "objs";
  }
  int len = strlen (name) + 1;
  char* result = malloc (dirNameLength + len);
  memcpy (result, dirName, dirNameLength);
  memcpy (result + dirNameLength, name, len);
  return result;
}