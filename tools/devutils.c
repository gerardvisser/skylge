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
#include "buildConfig.h"
#include "devutils.h"

#define EMPH_COLOUR   "\x1B[1m"
#define NORMAL_COLOUR "\x1B[22m"

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

void devutils_printBuildConfig (void) {
  printf ("%scompiler:%s %s\n", EMPH_COLOUR, NORMAL_COLOUR, buildConfig_compiler ());
  printf ("%sobjectFileExtension:%s %s\n", EMPH_COLOUR, NORMAL_COLOUR, buildConfig_objectFileExtension ());
  printf ("%ssourceFileExtension:%s %s\n\n", EMPH_COLOUR, NORMAL_COLOUR, buildConfig_sourceFileExtension ());
}

void devutils_printBuildOptions (buildOptions_t* options) {
  printf ("%sOptimization level:%s %d\n", EMPH_COLOUR, NORMAL_COLOUR, options->optimizationLevel);
  printf ("%sObject file directory:%s %s\n", EMPH_COLOUR, NORMAL_COLOUR, options->objsDirectory);
  printf ("%sInclude search path:%s ", EMPH_COLOUR, NORMAL_COLOUR);
  printList (options->includeSearchPath);
  printf ("%sMacros:%s ", EMPH_COLOUR, NORMAL_COLOUR);
  printList (options->macros);
  printf ("%sFiles:%s ", EMPH_COLOUR, NORMAL_COLOUR);
  printList (options->files);
  printf ("%sSnapshot:%s %s\n", EMPH_COLOUR, NORMAL_COLOUR, options->snapshot ? "true" : "false");
  printf ("%sClean:%s %s\n\n", EMPH_COLOUR, NORMAL_COLOUR, options->clean ? "true" : "false");
  if (options->exeName != NULL) {
    printf ("%sExecutable:%s %s\n", EMPH_COLOUR, NORMAL_COLOUR, options->exeName);
    printf ("%sLibraries:%s ", EMPH_COLOUR, NORMAL_COLOUR);
    printList (options->libraries);
    printf ("%sLibrary search path:%s ", EMPH_COLOUR, NORMAL_COLOUR);
    printList (options->libSearchPath);
    printf ("\n");
  } else if (options->libName != NULL) {
    printf ("%sArchives:%s lib%s-%s.a, lib%s-%s-d.a\n", EMPH_COLOUR, NORMAL_COLOUR, options->libName, options->libVersion, options->libName, options->libVersion);
    printf ("%sArchive directory:%s %s\n", EMPH_COLOUR, NORMAL_COLOUR, options->libDirectory);
    printf ("\n");
  }
}
