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
#include "buildOptions.h"
#include "buildfile.h"
#include "commandLineArgs.h"
#include "errors.h"
#include "filename.h"
#include "programmeInfo.h"

#define BUFFER_SIZE 256

#define freeIfNecessary(pointer) \
  if (pointer != NULL) { \
    free ((void*) pointer); \
  }

static bool getBoolOption (commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs, char option);
static buildfile_t* getBuildFile (stringList_t* buildfilenames);
static commandLineArgs_t* getCommandLineArgs (int argc, char** args);
static const char* getDestinationDirectory (commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs, buildfile_t* buildFile, char option);
static const char* getExeName (commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs, buildfile_t* buildFile, bool inhibitAorXfromBuildfile);
static stringList_t* getFiles (bool* inhibitAorXfromBuildfile, commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs, buildfile_t* buildFile);
static const char* getLibName (commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs, bool inhibitAorXfromBuildfile);
static const char* getLibVersion (commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs);
static stringList_t* getMacrosOrLibraries (commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs, char option);
static int getOptimizationLevel (commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs, bool snapshotVersion);
static stringList_t* getSearchPath (commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs, buildfile_t* buildFile, char inclOrLib);
static bool isSnapshotVersion (const char* version);
static void normalizeFilenames (buildOptions_t* this);
static void setDevelopmentOptions (buildOptions_t* this, commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs);

void buildOptions_delete (buildOptions_t* this) {
  stringList_delete (this->files);
  stringList_delete (this->macros);
  stringList_delete (this->libraries);
  stringList_delete (this->libSearchPath);
  stringList_delete (this->includeSearchPath);
  freeIfNecessary (this->objsDirectory);
  freeIfNecessary (this->libDirectory);
  freeIfNecessary (this->libVersion);
  freeIfNecessary (this->libName);
  freeIfNecessary (this->exeName);
  free (this);
}

buildOptions_t* buildOptions_new (int argc, char** args) {
  commandLineArgs_t* commandLineArgs = getCommandLineArgs (argc - 1, args + 1);
  buildfile_t* buildFile = getBuildFile (commandLineArgs_getStringOptionValue (commandLineArgs, 'b'));
  commandLineArgs_t* buildFileArgs = NULL;
  if (buildFile != NULL) {
    buildFileArgs = getCommandLineArgs (buildfile_argCount (buildFile), buildfile_arguments (buildFile));
  }

  buildOptions_t* result = malloc (sizeof (buildOptions_t));
  memset (result, 0, sizeof (buildOptions_t));

  bool inhibitAorXfromBuildfile;
  result->files = getFiles (&inhibitAorXfromBuildfile, commandLineArgs, buildFileArgs, buildFile);
  result->exeName = getExeName (commandLineArgs, buildFileArgs, buildFile, inhibitAorXfromBuildfile);
  result->libName = getLibName (commandLineArgs, buildFileArgs, inhibitAorXfromBuildfile);
  if (!(result->exeName == NULL | result->libName == NULL)) {
    errors_printMessageAndExit ("Should an archive or an executable be created (cannot create both)?");
  }
  if (result->exeName != NULL) {
    result->libSearchPath = getSearchPath (commandLineArgs, buildFileArgs, buildFile, 'L');
    result->libraries = getMacrosOrLibraries (commandLineArgs, buildFileArgs, 'l');
  } else if (result->libName != NULL) {
    result->libDirectory = getDestinationDirectory (commandLineArgs, buildFileArgs, buildFile, 'L');
    result->libVersion = getLibVersion (commandLineArgs, buildFileArgs);
    result->snapshot = isSnapshotVersion (result->libVersion);
  }
  result->clean = getBoolOption (commandLineArgs, buildFileArgs, 'c');
  result->macros = getMacrosOrLibraries (commandLineArgs, buildFileArgs, 'D');
  result->objsDirectory = getDestinationDirectory (commandLineArgs, buildFileArgs, buildFile, 'o');
  result->includeSearchPath = getSearchPath (commandLineArgs, buildFileArgs, buildFile, 'I');
  result->optimizationLevel = getOptimizationLevel (commandLineArgs, buildFileArgs, result->snapshot);
  setDevelopmentOptions (result, commandLineArgs, buildFileArgs);

  commandLineArgs_delete (commandLineArgs);
  if (buildFile != NULL) {
    commandLineArgs_delete (buildFileArgs);
    buildfile_delete (buildFile);
  }
  normalizeFilenames (result);
  return result;
}

static bool getBoolOption (commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs, char option) {
  bool result = commandLineArgs_getBoolOptionValue (commandLineArgs, option);
  if (!(result | buildFileArgs == NULL)) {
    result = commandLineArgs_getBoolOptionValue (buildFileArgs, option);
  }
  return result;
}

static buildfile_t* getBuildFile (stringList_t* buildfilenames) {
  if (stringList_length (buildfilenames) > 1) {
    errors_printMessageAndExit ("Only one buildfile can be specified");
  }
  char* buildfileLocation = filename_normalize (NULL, buildfilenames->value, buildfilenames->valueLength);
  buildfile_t* result = buildfile_new (buildfileLocation);
  free (buildfileLocation);
  return result;
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
                                                                        commandLineArgs_option_newStringOption ('-', NULL),
                                                                        NULL);
  return commandLineArgs;
}

static const char* getDestinationDirectory (commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs, buildfile_t* buildFile, char option) {
  int dirNameLength;
  const char* dirName;
  stringList_t* list = commandLineArgs_getStringOptionValue (commandLineArgs, option);
  if (list == NULL & buildFileArgs != NULL) {
    list = commandLineArgs_getStringOptionValue (buildFileArgs, option);
    if (list != NULL && filename_isAbsolute (list->value)) {
      dirNameLength = 0;
      dirName = "";
    } else {
      dirNameLength = buildfile_dirNameLength (buildFile);
      dirName = buildfile_dirName (buildFile);
    }
  } else {
    dirNameLength = 0;
    dirName = "";
  }
  int len;
  const char* name;
  if (list != NULL) {
    if (stringList_length (list) > 1) {
      if (option == 'o') {
        errors_printMessageAndExit ("Only one object file destination can be specified");
      } else {
        errors_printMessageAndExit ("Only one archive destination can be specified");
      }
    }
    name = list->value;
    len = list->valueLength + 1;
  } else if (option == 'o') {
    name = "objs";
    len = 5;
  } else {
    name = "lib";
    len = 4;
  }
  char* result = malloc (dirNameLength + len);
  memcpy (result, dirName, dirNameLength);
  memcpy (result + dirNameLength, name, len);
  return result;
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
    if (filename_isAbsolute (list->value)) {
      dirNameLength = 0;
      dirName = "";
    }
    int len = list->valueLength + 1;
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
  *inhibitAorXfromBuildfile = list != NULL;
  if (list == NULL & buildFileArgs != NULL) {
    list = commandLineArgs_getMainArgs (buildFileArgs);
    dirName = buildfile_dirName (buildFile);
    dirNameLength = buildfile_dirNameLength (buildFile);
  } else {
    dirName = "";
    dirNameLength = 0;
  }
  if (list == NULL) {
    errors_printMessageAndExit ("No input files.\nTry '%s --help' for more information", PROGRAMME_NAME);
  }

  char buf[BUFFER_SIZE];
  stringList_t* result = NULL;

  while (list != NULL) {
    if (!filename_isAbsolute (list->value)) {
      int len = list->valueLength + 1;
      char* name = dirNameLength + len > BUFFER_SIZE ? malloc (dirNameLength + len) : buf;
      //mempcpy (mempcpy (name, dirName, dirNameLength), list->value, len);
      memcpy (name, dirName, dirNameLength);
      memcpy (name + dirNameLength, list->value, len);
      result = stringList_append (result, name);
      if (name != buf) {
        free (name);
      }
    } else {
      result = stringList_append (result, list->value);
    }
    list = list->next;
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
    int len = list->valueLength + 1;
    char* result = malloc (len);
    memcpy (result, list->value, len);
    return result;
  }
  return NULL;
}

static const char* getLibVersion (commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs) {
  stringList_t* list = commandLineArgs_getStringOptionValue (commandLineArgs, 'V');
  if (list == NULL & buildFileArgs != NULL) {
    list = commandLineArgs_getStringOptionValue (buildFileArgs, 'V');
  }
  int len;
  const char* version;
  if (list != NULL) {
    if (stringList_length (list) > 1) {
      errors_printMessageAndExit ("Only one version can be specified");
    }
    version = list->value;
    len = list->valueLength + 1;
  } else {
    version = "SNAPSHOT";
    len = 9;
  }
  char* result = malloc (len);
  memcpy (result, version, len);
  return result;
}

static stringList_t* getMacrosOrLibraries (commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs, char option) {
  stringList_t* result = NULL;
  stringList_t* list = commandLineArgs_getStringOptionValue (commandLineArgs, option);
  while (list != NULL) {
    result = stringList_append (result, list->value);
    list = list->next;
  }
  if (buildFileArgs != NULL) {
    list = commandLineArgs_getStringOptionValue (buildFileArgs, option);
    while (list != NULL) {
      result = stringList_append (result, list->value);
      list = list->next;
    }
  }
  result = stringList_firstElement (result);
  return result;
}

static int getOptimizationLevel (commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs, bool snapshotVersion) {
  stringList_t* list = commandLineArgs_getStringOptionValue (commandLineArgs, 'O');
  if (list == NULL & buildFileArgs != NULL & !snapshotVersion) {
    list = commandLineArgs_getStringOptionValue (buildFileArgs, 'O');
  }
  if (list == NULL) {
    return -1;
  }
  if (stringList_length (list) > 1) {
    errors_printMessageAndExit ("Only one optimization level can be specified");
  }
  if (list->valueLength == 1) {
    switch (list->value[0]) {
    case 0x30: return 0;
    case 0x31: return 1;
    case 0x32: return 2;
    case 0x33: return 3;
    case 0x73: return 5;
    default:
      errors_printMessageAndExit ("Unknown optimization level. Possible values are: 0, 1, 2, 3, fast, s");
    }
  } else if (strcmp (list->value, "fast") != 0) {
    errors_printMessageAndExit ("Unknown optimization level. Possible values are: 0, 1, 2, 3, fast, s");
  } else {
    return 4;
  }
}

static stringList_t* getSearchPath (commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs, buildfile_t* buildFile, char inclOrLib) {
  stringList_t* result = NULL;
  stringList_t* list = commandLineArgs_getStringOptionValue (commandLineArgs, inclOrLib);
  while (list != NULL) {
    result = stringList_append (result, list->value);
    list = list->next;
  }
  if (buildFileArgs != NULL) {
    char buf[BUFFER_SIZE];
    list = commandLineArgs_getStringOptionValue (buildFileArgs, inclOrLib);
    int dirNameLength = buildfile_dirNameLength (buildFile);
    const char* dirName = buildfile_dirName (buildFile);
    while (list != NULL) {
      if (!filename_isAbsolute (list->value)) {
        int len = list->valueLength + 1;
        char* name = dirNameLength + len > BUFFER_SIZE ? malloc (dirNameLength + len) : buf;
        memcpy (name, dirName, dirNameLength);
        memcpy (name + dirNameLength, list->value, len);
        result = stringList_append (result, name);
        if (name != buf) {
          free (name);
        }
      } else {
        result = stringList_append (result, list->value);
      }
      list = list->next;
    }
  }
  result = stringList_firstElement (result);
  return result;
}

static bool isSnapshotVersion (const char* version) {
  int len = strlen (version);
  return len > 7 && memcmp (version + len - 8, "SNAPSHOT", 9) == 0;
}

static stringList_t* normalizeFilenameList (stringList_t* names, int homeDirNameLength) {
  char buf[BUFFER_SIZE];
  stringList_t* normalizedNames = NULL;
  while (names != NULL) {
    char* newName = names->valueLength > BUFFER_SIZE - homeDirNameLength ? malloc (names->valueLength + homeDirNameLength) : buf;
    filename_normalize (newName, names->value, names->valueLength);
    normalizedNames = stringList_append (normalizedNames, newName);
    if (newName != buf) {
      free (newName);
    }
    names = names->next;
  }
  normalizedNames = stringList_firstElement (normalizedNames);
  return normalizedNames;
}

static void normalizeFilenames (buildOptions_t* this) {
  char* oldName = (char*) this->objsDirectory;
  int oldNameLen = strlen (oldName);
  this->objsDirectory = filename_normalize (NULL, oldName, oldNameLen);
  free (oldName);

  oldName = (char*) this->libDirectory;
  if (oldName != NULL) {
    oldNameLen = strlen (oldName);
    this->libDirectory = filename_normalize (NULL, oldName, oldNameLen);
    free (oldName);
  }

  oldName = (char*) this->exeName;
  if (oldName != NULL) {
    oldNameLen = strlen (oldName);
    this->exeName = filename_normalize (NULL, oldName, oldNameLen);
    free (oldName);
  }

  int homeDirNameLength = strlen (buildConfig_homeDirectory ());

  stringList_t* oldNames = this->files;
  this->files = normalizeFilenameList (oldNames, homeDirNameLength);
  stringList_delete (oldNames);

  oldNames = this->libSearchPath;
  this->libSearchPath = normalizeFilenameList (oldNames, homeDirNameLength);
  stringList_delete (oldNames);

  oldNames = this->includeSearchPath;
  this->includeSearchPath = normalizeFilenameList (oldNames, homeDirNameLength);
  stringList_delete (oldNames);
}

static void setDevelopmentoptions (buildOptions_t* this, stringList_t* options) {
  while (options != NULL) {
    if (strcmp (options->value, "dry-run") == 0) {
      this->dryRun = true;
    } else if (strcmp (options->value, "show-config") == 0) {
      this->showConfig = true;
    } else if (strcmp (options->value, "show-job-description") == 0) {
      this->showJobDescription = true;
    } else if (!(strcmp (options->value, "config") == 0 || strcmp (options->value, "help") == 0 || strcmp (options->value, "version") == 0)) {
      errors_printMessageAndExit ("Unknown option '--%s'", options->value);
    }
    options = options->next;
  }
}

static void setDevelopmentOptions (buildOptions_t* this, commandLineArgs_t* commandLineArgs, commandLineArgs_t* buildFileArgs) {
  setDevelopmentoptions (this, commandLineArgs_getStringOptionValue (commandLineArgs, '-'));
  if (buildFileArgs != NULL) {
    setDevelopmentoptions (this, commandLineArgs_getStringOptionValue (buildFileArgs, '-'));
  }
}
