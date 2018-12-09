/*
   Author:  Gerard Visser
   e-mail:  visser.gerard(at)gmail.com

   Copyright (C) 2015, 2018 Gerard Visser.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <stdio.h>
#include <sys/time.h>
#include <skylge/testutils/Stopwatch.h>

static int64_t timeOfDay (void) {
  struct timeval x;
  gettimeofday (&x, NULL);
  return (int64_t) x.tv_sec * 100 + x.tv_usec / 10000;
}

void Stopwatch::print (char* buf) {
  int64_t diff = m_endTime - m_startTime;
  int centis = diff % 100;
  diff /= 100;
  int seconds = diff % 60;
  diff /= 60;
  sprintf (buf, "%ld:%02d.%02d", diff, seconds, centis);
}

void Stopwatch::start (void) {
  m_startTime = timeOfDay ();
}

void Stopwatch::stop (void) {
  m_endTime = timeOfDay ();
}
