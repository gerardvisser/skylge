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
#include "stringList.h"

stringList_t* stringList_append (stringList_t* this, const char* value) {
  if (this == NULL) {
    return stringList_new (value);
  }
  while (this->next != NULL) {
    this = this->next;
  }
  this->next = stringList_new (value);
  this->next->previous = this;
  return this->next;
}

void stringList_delete (stringList_t* this) {
  if (this == NULL) {
    return;
  }
  if (this->previous != NULL) {
    this->previous->next = NULL;
  }
  stringList_t* next;
  do {
    next = this->next;
    free ((void*) this->value);
    free (this);
    this = next;
  } while (this != NULL);
}

stringList_t* stringList_firstElement (stringList_t* this) {
  if (this == NULL) {
    return NULL;
  }
  while (this->previous != NULL) {
    this = this->previous;
  }
  return this;
}

int stringList_length (stringList_t* this) {
  if (this == NULL) {
    return 0;
  }
  int result = 0;
  do {
    ++result;
    this = this->next;
  } while (this != NULL);
  return result;
}

stringList_t* stringList_new (const char* value) {
  const int len = strlen (value);
  stringList_t* result = malloc (sizeof (stringList_t));
  char* valueCopy = malloc (len + 1);
  strcpy (valueCopy, value);
  result->value = valueCopy;
  result->previous = NULL;
  result->next = NULL;
  return result;
}
