/*
   Author      : Gerard Visser
   e-mail      : visser.gerard(at)gmail.com

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

#include "DbgRefInfoList.h"
#include <skylge/base/mem.h>
#include <string.h>

DbgRefInfoList::DbgRefInfoList (void) {
  m_head = NULL;
}

DbgRefInfoList::~DbgRefInfoList (void) {
  DbgRefInfo* elem = m_head;
  while (elem != NULL) {
    DbgRefInfo* prev = elem->getPrevious ();
    delete elem;
    elem = prev;
  }
}

/* elem->previous and elem->next are expected to be NULL. */
void DbgRefInfoList::add (DbgRefInfo* elem) {
  if (m_head != NULL) {
    m_head->setNext (elem);
  }
  elem->setPrevious (m_head);
  m_head = elem;
}

DbgRefInfo* DbgRefInfoList::get (const void* pointer) {
  DbgRefInfo* elem = m_head;
  while (!(elem == NULL || elem->getPointer () == pointer)) {
    elem = elem->getPrevious ();
  }
  return elem;
}

void DbgRefInfoList::printContents (FILE* stream) {
  const bool html = stream != stdout;
  const char* post = html ? "</span>" : "";
  const char* pre;
  char buf[256];

  DbgRefInfo* elem = m_head;
  while (elem != NULL) {
    if ((elem->getPtype () & PTYPE_ADMINISTRATION) != 0) {
      pre = html ? "<span class=\"gray\">" : "\x1B[0m";
    } else if ((elem->getPtype () & PTYPE_INTERNAL) != 0) {
      pre = html ? "<span class=\"yellow\">" : "\x1B[1;33m";
    } else {
      pre = html ? "<span class=\"red\">" : "\x1B[1;31m";
    }
    sprintf (buf, "%s  (%ld) %s\n%s", pre, elem->getRefCount (), elem->getTypeName (), post);
    int len = strlen (buf);
    fwrite (buf, 1, len, stream);
    elem = elem->getPrevious ();
  }
}

DbgRefInfo* DbgRefInfoList::remove (const void* pointer) {
  DbgRefInfo* x = get (pointer);
  if (x != NULL) {
    DbgRefInfo* a = x->getPrevious ();
    DbgRefInfo* b = x->getNext ();
    if (b != NULL) {
      b->setPrevious (a);
    } else {
      m_head = a;
    }
    if (a != NULL) {
      a->setNext (b);
    }
    x->setPrevious (NULL);
    x->setNext (NULL);
  }
  return x;
}

#endif
