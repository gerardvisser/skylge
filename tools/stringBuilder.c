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
#include "stringBuilder.h"

struct stringBuilder_struct {
  char* buffer;
  int capacityQuantum;
  int capacity;
  int length;
};

static void ensureCapacity (stringBuilder_t* this, int additionalLength) {
  int delta = this->length + additionalLength - this->capacity;
  if (delta > 0) {
    delta = 1 + (delta / this->capacityQuantum);
    delta *= this->capacityQuantum;
    this->capacity += delta;
    this->buffer = realloc (this->buffer, this->capacity);
  }
}

void stringBuilder_append (stringBuilder_t* this, const char* str) {
  int len = strlen (str);
  stringBuilder_appendChars (this, str, len);
}

void stringBuilder_appendChar (stringBuilder_t* this, char c) {
  ensureCapacity (this, 1);
  this->buffer[this->length] = c;
  ++this->length;
}

void stringBuilder_appendChars (stringBuilder_t* this, const char* str, int len) {
  if (len > 0) {
    ensureCapacity (this, len);
    memcpy (this->buffer + this->length, str, len);
    this->length += len;
  }
}

void stringBuilder_clear (stringBuilder_t* this) {
  this->length = 0;
}

void stringBuilder_delete (stringBuilder_t* this) {
  free (this->buffer);
  free (this);
}

int stringBuilder_length (stringBuilder_t* this) {
  return this->length;
}

stringBuilder_t* stringBuilder_new (int initialCapacity) {
  if (initialCapacity < 8) {
    initialCapacity = 8;
  }
  stringBuilder_t* result = malloc (sizeof (stringBuilder_t));
  result->buffer = malloc (initialCapacity);
  result->capacityQuantum = initialCapacity;
  result->capacity = initialCapacity;
  result->length = 0;
  return result;
}

char* stringBuilder_toString (stringBuilder_t* this) {
  char* result = malloc (this->length + 1);
  memcpy (result, this->buffer, this->length);
  result[this->length] = 0;
  return result;
}
