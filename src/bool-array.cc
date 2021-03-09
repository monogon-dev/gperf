/* Fast lookup table abstraction implemented as an Iteration Number Array
   Copyright (C) 1989-1998, 2002-2003 Free Software Foundation, Inc.
   Written by Douglas C. Schmidt <schmidt@ics.uci.edu>
   and Bruno Haible <bruno@clisp.org>.

   This file is part of GNU GPERF.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Specification. */
#include "bool-array.h"

#include <stdio.h>
#include <string.h>
#include "options.h"

/* Frees this object.  */
Bool_Array::~Bool_Array ()
{
  /* Print out debugging diagnostics. */
  if (option[DEBUG])
    fprintf (stderr, "\ndumping boolean array information\n"
             "size = %d\niteration number = %d\nend of array dump\n",
             _size, _iteration_number);
  delete[] const_cast<unsigned int *>(_storage_array);
}

/* Initializes the bit array with room for SIZE bits, numbered from
   0 to SIZE-1. */
Bool_Array::Bool_Array (unsigned int size)
  : _size (size),
    _iteration_number (1),
    _storage_array (new unsigned int [size])
{
  memset (_storage_array, 0, size * sizeof (_storage_array[0]));
  if (option[DEBUG])
    fprintf (stderr, "\nbool array size = %d, total bytes = %d\n",
             _size,
             static_cast<unsigned int> (_size * sizeof (_storage_array[0])));
}

/* Sets the specified bit to true.
   Returns its previous value (false or true).  */
bool
Bool_Array::set_bit (unsigned int index)
{
  if (_storage_array[index] == _iteration_number)
    /* The bit was set since the last clear() call.  */
    return true;
  else
    {
      /* The last operation on this bit was clear().  Set it now.  */
      _storage_array[index] = _iteration_number;
      return false;
    }
}

/* Resets all bits to zero.  */
void
Bool_Array::clear ()
{
  /* If we wrap around it's time to zero things out again!  However, this only
     occurs once about every 2^32 iterations, so it will not happen more
     frequently than once per second.  */

  if (++_iteration_number == 0)
    {
      _iteration_number = 1;
      memset (_storage_array, 0, _size * sizeof (_storage_array[0]));
      if (option[DEBUG])
        {
          fprintf (stderr, "(re-initialized bool_array)\n");
          fflush (stderr);
        }
    }
}