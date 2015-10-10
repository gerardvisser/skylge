/*
   Author:  Gerard Visser
   e-mail:  visser.gerard(at)gmail.com

   Copyright (C) 2008 - 2015 Gerard Visser.

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

#include <pthread.h>
#include <skylge/base/errors.h>
#include <skylge/base/mem.h>
#include "dbgMemStats.h"

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* mem_allocate (const char* typeName, size_t sizeInBytes, int ptype) {
  ___BTPUSH;
  if ((ptype & ((PTYPE_INTERNAL | PTYPE_REF_COUNT_CONTAINER | PTYPE_ADMINISTRATION) ^ -1)) != 0) {
    errors_printMessageAndExit ("Invalid ptype specified");
  }
  if (sizeInBytes < 1) {
    errors_printMessageAndExit ("The specified `sizeInBytes' needs to be a positive integer");
  }

  pthread_mutex_lock (&mutex);
  if ((ptype & PTYPE_ADMINISTRATION) == 0) {
    dbgMemStats::requestedBytes ()->add (sizeInBytes);
  }
  dbgMemStats::debugBytes ()->add (sizeof (DbgRefInfo));
  dbgMemStats::allocatedBytes ()->add (sizeInBytes);

  void* result = malloc (sizeInBytes);
  dbgMemStats::list ()->add (new DbgRefInfo (result, typeName, sizeInBytes, ptype));
  pthread_mutex_unlock (&mutex);

  ___BTPOP;
  return result;
}

long int mem_currentlyAllocatedBytes_real (void) {
  ___BTPUSH;
  pthread_mutex_lock (&mutex);
  long int result = dbgMemStats::allocatedBytes ()->getCurrentlyAllocated ();
  pthread_mutex_unlock (&mutex);
  ___BTPOP;
  return result;
}

long int mem_currentlyAllocatedBytes_virtual (void) {
  ___BTPUSH;
  pthread_mutex_lock (&mutex);
  long int result = dbgMemStats::requestedBytes ()->getCurrentlyAllocated ();
  pthread_mutex_unlock (&mutex);
  ___BTPOP;
  return result;
}

void mem_deregister (void* pointer) {
  ___BTPUSH;
  pthread_mutex_lock (&mutex);
  DbgRefInfo* refInfo = dbgMemStats::list ()->remove (pointer);
  if (refInfo == NULL) {
    errors_printMessageAndExit ("Unknown pointer specified: double free or corruption");
  }
  if ((refInfo->getPtype () & PTYPE_VIRTUAL) == 0) {
    errors_printMessageAndExit ("The specified pointer should be removed using `mem_release'");
  }

  dbgMemStats::requestedBytes ()->sub (refInfo->getSize ());
  dbgMemStats::debugBytes ()->sub (sizeof (DbgRefInfo));
  delete refInfo;
  pthread_mutex_unlock (&mutex);
  ___BTPOP;
}

void mem_printAllocationInfo (void) {
  ___BTPUSH;
  printf ("\nCurrently allocated:\n");
  pthread_mutex_lock (&mutex);
  dbgMemStats::list ()->printContents ();
  pthread_mutex_unlock (&mutex);
  printf ("\x1B[0m\n");
  ___BTPOP;
}

void mem_printExitStats (const char* fileName) {
  ___BTPUSH;
  pthread_mutex_lock (&mutex);
  dbgMemStats::printExitStats (fileName);
  pthread_mutex_unlock (&mutex);
  pthread_mutex_destroy (&mutex);
  ___BTPOP;
}

void* mem_reallocate (void* pointer, size_t sizeInBytes) {
  ___BTPUSH;
  pthread_mutex_lock (&mutex);

  DbgRefInfo* refInfo = dbgMemStats::list ()->remove (pointer);

  if (sizeInBytes < 1) {
    errors_printMessageAndExit ("The specified `sizeInBytes' needs to be a positive integer");
  }
  if (refInfo == NULL) {
    errors_printMessageAndExit ("Unknown pointer specified: reallocation not possible");
  }
  if (! (refInfo->getPtype ()==PTYPE_ORDINARY || refInfo->getPtype ()==PTYPE_INTERNAL) ) {
    errors_printMessageAndExit ("Only data blocks of type `PTYPE_ORDINARY' or `PTYPE_INTERNAL' (see mem.h)\ncan be relocated");
  }

  void* result = realloc (pointer, sizeInBytes);

  if (result == NULL) {
    errors_printMessageAndExit ("Out of memory");
  }
  if (result != pointer) {
    refInfo->setPointer (result);
    dbgMemStats::allocatedBytes ()->add (sizeInBytes);
    dbgMemStats::allocatedBytes ()->sub (refInfo->getSize ());
    dbgMemStats::requestedBytes ()->add (sizeInBytes);
    dbgMemStats::requestedBytes ()->sub (refInfo->getSize ());
  } else {
    dbgMemStats::allocatedBytes ()->update (sizeInBytes - refInfo->getSize ());
    dbgMemStats::requestedBytes ()->update (sizeInBytes - refInfo->getSize ());
  }
  dbgMemStats::list ()->add (refInfo);
  refInfo->setSize (sizeInBytes);

  pthread_mutex_unlock (&mutex);
  ___BTPOP;
  return result;
}

void mem_register (const char* typeName, size_t sizeInBytes, int ptype, const void* pointer) {
  ___BTPUSH;
  if ((ptype & ((PTYPE_INTERNAL | PTYPE_REF_COUNT_CONTAINER) ^ -1)) != 0) {
    errors_printMessageAndExit ("Invalid ptype specified");
  }
  ptype |= PTYPE_VIRTUAL;

  if (sizeInBytes < 1) {
    errors_printMessageAndExit ("The specified `sizeInBytes' needs to be a positive integer");
  }

  pthread_mutex_lock (&mutex);
  dbgMemStats::requestedBytes ()->add (sizeInBytes);
  dbgMemStats::debugBytes ()->add (sizeof (DbgRefInfo));
  dbgMemStats::list ()->add (new DbgRefInfo (pointer, typeName, sizeInBytes, ptype));
  pthread_mutex_unlock (&mutex);
  ___BTPOP;
}

void mem_release (void* pointer) {
  ___BTPUSH;
  pthread_mutex_lock (&mutex);

  DbgRefInfo* refInfo = dbgMemStats::list ()->remove (pointer);
  if (refInfo == NULL) {
    errors_printMessageAndExit ("Unknown pointer specified: double free or corruption");
  }
  if ((refInfo->getPtype () & PTYPE_VIRTUAL) != 0) {
    errors_printMessageAndExit ("The specified pointer should be removed using `mem_deregister'");
  }

  if ((refInfo->getPtype () & PTYPE_ADMINISTRATION) == 0) {
    dbgMemStats::requestedBytes ()->sub (refInfo->getSize ());
  }
  dbgMemStats::allocatedBytes ()->sub (refInfo->getSize ());
  dbgMemStats::debugBytes ()->sub (sizeof (DbgRefInfo));
  delete refInfo;

  pthread_mutex_unlock (&mutex);
  ___BTPOP;
}

#endif
