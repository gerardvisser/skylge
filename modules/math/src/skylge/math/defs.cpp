/*
   Author:  Gerard Visser
   e-mail:  visser.gerard(at)gmail.com

   Copyright (C) 2018 Gerard Visser.

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

#include "defs.h"

#if CAL_B == 6

const uint64_t CAL_LMASK[7] = {0x3F, 0x3E, 0x3C, 0x38, 0x30, 0x20, 0x00};
const uint64_t CAL_RMASK[7] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F};
const uint64_t CAL_SMASK[7] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40};

#else

const uint64_t CAL_LMASK[33] = {0xFFFFFFFF, 0xFFFFFFFE, 0xFFFFFFFC, 0xFFFFFFF8,
                                0xFFFFFFF0, 0xFFFFFFE0, 0xFFFFFFC0, 0xFFFFFF80,
                                0xFFFFFF00, 0xFFFFFE00, 0xFFFFFC00, 0xFFFFF800,
                                0xFFFFF000, 0xFFFFE000, 0xFFFFC000, 0xFFFF8000,
                                0xFFFF0000, 0xFFFE0000, 0xFFFC0000, 0xFFF80000,
                                0xFFF00000, 0xFFE00000, 0xFFC00000, 0xFF800000,
                                0xFF000000, 0xFE000000, 0xFC000000, 0xF8000000,
                                0xF0000000, 0xE0000000, 0xC0000000, 0x80000000,
                                0x00000000};

const uint64_t CAL_RMASK[33] = {0x00000000, 0x00000001, 0x00000003, 0x00000007,
                                0x0000000F, 0x0000001F, 0x0000003F, 0x0000007F,
                                0x000000FF, 0x000001FF, 0x000003FF, 0x000007FF,
                                0x00000FFF, 0x00001FFF, 0x00003FFF, 0x00007FFF,
                                0x0000FFFF, 0x0001FFFF, 0x0003FFFF, 0x0007FFFF,
                                0x000FFFFF, 0x001FFFFF, 0x003FFFFF, 0x007FFFFF,
                                0x00FFFFFF, 0x01FFFFFF, 0x03FFFFFF, 0x07FFFFFF,
                                0x0FFFFFFF, 0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF,
                                0xFFFFFFFF};

const uint64_t CAL_SMASK[33] = {0x00000001, 0x00000002, 0x00000004, 0x00000008,
                                0x00000010, 0x00000020, 0x00000040, 0x00000080,
                                0x00000100, 0x00000200, 0x00000400, 0x00000800,
                                0x00001000, 0x00002000, 0x00004000, 0x00008000,
                                0x00010000, 0x00020000, 0x00040000, 0x00080000,
                                0x00100000, 0x00200000, 0x00400000, 0x00800000,
                                0x01000000, 0x02000000, 0x04000000, 0x08000000,
                                0x10000000, 0x20000000, 0x40000000, 0x80000000,
                                0x100000000};

#endif
