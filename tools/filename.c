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
#include "errors.h"
#include "filename.h"

#define DOT    1
#define DOTDOT 2
#define EMPTY  3

static void normalizeAbsFile (char* destination, const char* filename, int filenameLen);
static void normalizeRelFile (char* destination, const char* filename, int filenameLen);
static int slashCount (const char* filename, int filenameLen);

bool filename_isAbsolute (const char* filename) {
  return filename[0] == '/' | filename[0] == '~';
}

char* filename_normalize (char* destination, const char* filename, int filenameLen) {
  if (filename == NULL) {
    errors_printMessageAndExit ("\x1B[7mNull pointer exception (%s:%d)\x1B[27m", __FILE__, __LINE__);
  }
  if (destination == NULL) {
    destination = malloc (filenameLen + 2);
  }
  if (filename[0] == '/') {
    normalizeAbsFile (destination, filename, filenameLen);
  } else {
    normalizeRelFile (destination, filename, filenameLen);
  }
  return destination;
}

static void copyPart (char* dst, int* di, const char* src, int* si) {
  while (!(src[*si] == '/' | src[*si] == 0)) {
    dst[(*di)++] = src[(*si)++];
  }
  ++*si;
}

static int identifyNamePart (const char* partStart) {
  if (partStart[0] == '.') {
    if (partStart[1] == '.') {
      if (partStart[2] == '/' | partStart[2] == 0) {
        return DOTDOT;
      }
    } else if (partStart[1] == '/' | partStart[1] == 0) {
      return DOT;
    }
  } else if (partStart[0] == '/') {
    return EMPTY;
  }
  return 0;
}

static void normalizeAbsFile (char* destination, const char* filename, int filenameLen) {
  int di = 1;
  int si = 1;
  int slashes = 0;
  int slash[slashCount (filename, filenameLen) + 1];
  while (si < filenameLen) {
    switch (identifyNamePart (filename + si)) {
    case DOT:
      si += 2;
      break;

    case DOTDOT:
      if (slashes > 0) {
        --slashes;
        di = slashes > 0 ? slash[slashes - 1] : 1;
      }
      si += 3;
      break;

    case EMPTY:
      ++si;
      break;

    default:
      if (slashes > 0) {
        destination[di++] = '/';
      }
      copyPart (destination, &di, filename, &si);
      slash[slashes++] = di;
    }
  }
  destination[0] = '/';
  destination[di] = 0;
}

static void normalizeRelFile (char* destination, const char* filename, int filenameLen) {
  int di = 0;
  int si = 0;
  int parents = 0;
  int slashes = 0;
  int slash[slashCount (filename, filenameLen) + 1];
  while (si < filenameLen) {
    switch (identifyNamePart (filename + si)) {
    case DOT:
      si += 2;
      break;

    case DOTDOT:
      if (slashes - parents > 0) {
        --slashes;
        di = slashes > 0 ? slash[slashes - 1] : 0;
      } else {
        if (slashes > 0) {
          destination[di++] = '/';
        }
        destination[di++] = '.';
        destination[di++] = '.';
        slash[slashes++] = di;
        ++parents;
      }
      si += 3;
      break;

    case EMPTY:
      ++si;
      break;

    default:
      if (slashes > 0) {
        destination[di++] = '/';
      }
      copyPart (destination, &di, filename, &si);
      slash[slashes++] = di;
    }
  }
  if (di == 0) {
    destination[di++] = '.';
  }
  destination[di] = 0;
}

static int slashCount (const char* filename, int filenameLen) {
  int i = 0;
  int result = 0;
  while (i < filenameLen) {
    result += filename[i] == '/';
    ++i;
  }
  return result;
}
