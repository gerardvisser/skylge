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

#ifndef SKYLGE__BASE__MEM_INCLUDED
# define SKYLGE__BASE__MEM_INCLUDED

# include <stdlib.h>

# ifdef DEBUG_MODE

#  define PTYPE_ORDINARY            0
#  define PTYPE_ADMINISTRATION      1
#  define PTYPE_INTERNAL            2
#  define PTYPE_REF_COUNT_CONTAINER 4

#  define mem_currentlyAllocatedBytes() mem_currentlyAllocatedBytes_virtual ()

void*    mem_allocate (const char* typeName, size_t sizeInBytes, int ptype);
long int mem_currentlyAllocatedBytes_real (void); /* Does not include memory allocated for debugging purposes. */
long int mem_currentlyAllocatedBytes_virtual (void);
void     mem_deregister (void* pointer);
void     mem_printAllocationInfo (void);
void     mem_printExitStats (const char* fileName = NULL);
void*    mem_reallocate (void* pointer, size_t sizeInBytes);
void     mem_register (const char* typeName, size_t sizeInBytes, int ptype, const void* pointer);
void     mem_release (void* pointer);

# else

#  define mem_allocate(typeName, sizeInBytes, ptype)      malloc (sizeInBytes)
#  define mem_deregister(ptr)
#  define mem_printExitStats(...)
#  define mem_reallocate(ptr, sizeInBytes)                realloc (ptr, sizeInBytes)
#  define mem_register(typeName, sizeInBytes, ptype, ptr)
#  define mem_release(ptr)                                free (ptr)

# endif

#endif
