/*
   Author:  Gerard Visser
   e-mail:  visser.gerard(at)gmail.com

   Copyright (C) 2018 Gerard Visser.

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

#ifndef SKYLGE__TESTUTILS__TEST_RUNNER_INCLUDED
#define SKYLGE__TESTUTILS__TEST_RUNNER_INCLUDED

#define RUN_TESTS(x) TestRunner::run (x, sizeof (x) / sizeof (void*))

/* A test function is expected to return false on success.  */
typedef bool (* test_fn_t) (void);

namespace TestRunner {
  void run (const test_fn_t* tests, int testCount);
}

#endif
