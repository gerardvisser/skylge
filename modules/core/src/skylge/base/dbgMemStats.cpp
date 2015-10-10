/*
   Author      : Gerard Visser
   e-mail      : visser.gerard(at)gmail.com

   Copyright (C) 2013 - 2015 Gerard Visser.

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

#ifdef DEBUG_MODE

#include <string.h>
#include "dbgMemStats.h"

using namespace dbgMemStats;

static const char* const html_1 = "<!DOCTYPE html>\n<html>\n  <head>\n    <script type=\"text/javascript\">\n      var avis = true;\n      var ivis = true;\n      var style;\n\n      function setButtonTextAndDisplayStyle (button, blocktypename, visible) {\n        visible = !visible;\n        style = visible ? \"inline\" : \"none\";\n        var showOrHide = visible ? \"Hide \" : \"Show \";\n        button.innerHTML = showOrHide + blocktypename + \" Blocks\";\n        return visible;\n      }\n\n      function toggleVisibility (button, selector) {\n        if (selector == \"span.gray\") {\n          avis = setButtonTextAndDisplayStyle (button, \"Administration\", avis);\n        } else {\n          ivis = setButtonTextAndDisplayStyle (button, \"Internal\", ivis);\n        }\n        var elems = document.querySelectorAll (selector);\n        for (var i = 0; i < elems.length; ++i) {\n          elems[i].style.display = style;\n        }\n      }\n    </script>\n    <style type=\"text/css\">\n      body {\n        background-color: #000000;\n        color: #A0A0A0;\n      }\n      button {\n        width: 220px;\n      }\n      pre {\n        font-size: 15px;\n      }\n      span.gray {\n        color: #A0A0A0;\n      }\n      span.green {\n        color: #00D000;\n      }\n      span.red {\n        color: #E00000;\n      }\n      span.reverse {\n        background-color: #A0A0A0;\n        color: #000000;\n      }\n      span.yellow {\n        color: #D0B000;\n      }\n    </style>\n    <title>Mem Stats";
static const char* const html_2 = "</title>\n  </head>\n  <body>\n    <pre>";
static const char* const html_3 = "\n<button type=\"button\" onclick=\"toggleVisibility(this, 'span.gray');\">Hide Administration Blocks</button> <button type=\"button\" onclick=\"toggleVisibility(this, 'span.yellow');\">Hide Internal Blocks</button>\n";
static const char* const html_4 = "\n    </pre>\n  </body>\n</html>\n";

static DbgRefInfoHashList* _list = NULL;

static DbgAllocInfo* _debugBytes = NULL;
static DbgAllocInfo* _allocatedBytes = NULL;
static DbgAllocInfo* _requestedBytes = NULL;

static char buffer[256];

static void cleanUp (void) {

#define CLEAN_UP(x) \
  if (x != NULL) { \
    delete x; \
    x = NULL; \
  }

  CLEAN_UP (_list)
  CLEAN_UP (_debugBytes)
  CLEAN_UP (_allocatedBytes)
  CLEAN_UP (_requestedBytes)
}

static void printOverview (FILE* stream, bool error) {
  const char* brightColour = stream != stdout ? error ? "<span class=\"red\">" : "<span class=\"green\">" : error ? "\x1B[1;31m" : "\x1B[1;32m";
  const char* defaultColour = stream != stdout ? "</span>" : "\x1B[0m";

  sprintf (buffer, "\n%sNumber of allocation requests:                      %ld\n%s", brightColour, requestedBytes ()->getAllocCount (), defaultColour);
  fwrite (buffer, 1, strlen (buffer), stream);
  sprintf (buffer, "Actual number of allocations on behalf of requests: %ld\n", allocatedBytes ()->getAllocCount ());
  fwrite (buffer, 1, strlen (buffer), stream);
  sprintf (buffer, "Extra allocations for debugging purposes:           %ld\n%s", debugBytes ()->getAllocCount (), brightColour);
  fwrite (buffer, 1, strlen (buffer), stream);
  sprintf (buffer, "Number of requests to free memory:                  %ld\n\n", requestedBytes ()->getFreeCount ());
  fwrite (buffer, 1, strlen (buffer), stream);

  sprintf (buffer, "Bytes requested:                                    %ld\n", requestedBytes ()->getAllocated ());
  fwrite (buffer, 1, strlen (buffer), stream);
  sprintf (buffer, "of which were freed:                                %ld\n%s", requestedBytes ()->getFreed (), defaultColour);
  fwrite (buffer, 1, strlen (buffer), stream);

  sprintf (buffer, "Bytes allocated:                                    %ld\n", allocatedBytes ()->getAllocated ());
  fwrite (buffer, 1, strlen (buffer), stream);
  sprintf (buffer, "of which were freed:                                %ld\n", allocatedBytes ()->getFreed ());
  fwrite (buffer, 1, strlen (buffer), stream);

  sprintf (buffer, "Extra bytes allocated (for debugging purposes):     %ld\n", debugBytes ()->getAllocated ());
  fwrite (buffer, 1, strlen (buffer), stream);

  sprintf (buffer, "Max. number of bytes allocated at a time:           %ld\n", allocatedBytes ()->getAllocatedMax ());
  fwrite (buffer, 1, strlen (buffer), stream);
}

DbgAllocInfo* dbgMemStats::allocatedBytes (void) {
  if (_allocatedBytes == NULL) {
    _allocatedBytes = new DbgAllocInfo ();
    debugBytes ()->add (sizeof (DbgAllocInfo));
  }
  return _allocatedBytes;
}

DbgAllocInfo* dbgMemStats::debugBytes (void) {
  if (_debugBytes == NULL) {
    _debugBytes = new DbgAllocInfo ();
    _debugBytes->add (sizeof (DbgAllocInfo));
  }
  return _debugBytes;
}

DbgRefInfoHashList* dbgMemStats::list (void) {
  if (_list == NULL) {
    _list = new DbgRefInfoHashList ();
    debugBytes ()->add (sizeof (DbgRefInfoHashList));
  }
  return _list;
}

void dbgMemStats::printExitStats (const char* fileName) {
  const bool error = requestedBytes ()->getAllocCount () != requestedBytes ()->getFreeCount ();
  FILE* stream;

  printOverview (stdout, error);
  if (fileName != NULL) {
    stream = fopen (fileName, "wb");
    if (stream != NULL) {
      fwrite (html_1, 1, strlen (html_1), stream);
      fwrite (html_2, 1, strlen (html_2), stream);
      printOverview (stream, error);
      fwrite (html_3, 1, strlen (html_3), stream);
    } else {
      stream = stdout;
    }
  } else {
    stream = stdout;
  }

  if (allocatedBytes ()->getAllocCount () != allocatedBytes ()->getFreeCount ()) {
    sprintf (buffer, "\nStill allocated at exit ((ref-count) type name):\n");
    fwrite (buffer, 1, strlen (buffer), stream);
    list ()->printContents (stream);
  }
  printf ("\x1B[0m\n");
  cleanUp ();

  if (stream != stdout) {
    fwrite (html_4, 1, strlen (html_4), stream);
    fclose (stream);
  }
}

DbgAllocInfo* dbgMemStats::requestedBytes (void) {
  if (_requestedBytes == NULL) {
    _requestedBytes = new DbgAllocInfo ();
    debugBytes ()->add (sizeof (DbgAllocInfo));
  }
  return _requestedBytes;
}

#endif
