/* A set of byte positions.
   Copyright (C) 1989-1998, 2000, 2002-2003 Free Software Foundation, Inc.
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
#include "positions.h"

#include <stdio.h>
#include <stdlib.h> /* declares exit() */
#include <string.h>

/* ---------------------------- Class Positions ---------------------------- */

/* Constructors.  */
Positions::Positions ()
  : _useall (false),
    _size (0)
{
}

Positions::Positions (int pos1)
  : _useall (false),
    _size (1)
{
  _positions[0] = pos1;
}

Positions::Positions (int pos1, int pos2)
  : _useall (false),
    _size (2)
{
  _positions[0] = pos1;
  _positions[1] = pos2;
}

/* Copy constructor.  */
Positions::Positions (const Positions& src)
  : _useall (src._useall),
    _size (src._size)
{
  memcpy (_positions, src._positions, _size * sizeof (_positions[0]));
}

/* Assignment operator.  */
Positions&
Positions::operator= (const Positions& src)
{
  _useall = src._useall;
  _size = src._size;
  memcpy (_positions, src._positions, _size * sizeof (_positions[0]));
  return *this;
}

/* Accessors.  */
bool
Positions::is_useall () const
{
  return _useall;
}

int
Positions::operator[] (unsigned int index) const
{
  return _positions[index];
}

unsigned int
Positions::get_size () const
{
  return _size;
}

/* Write access.  */
void
Positions::set_useall (bool useall)
{
  _useall = useall;
  if (useall)
    {
      /* The positions are 0, 1, ..., MAX_KEY_POS-1, in descending order.  */
      _size = MAX_KEY_POS;
      int *ptr = _positions;
      for (int i = MAX_KEY_POS - 1; i >= 0; i--)
        *ptr++ = i;
    }
}

int *
Positions::pointer ()
{
  return _positions;
}

void
Positions::set_size (unsigned int size)
{
  _size = size;
}

/* Sorts the array in reverse order.
   Returns true if there are no duplicates, false otherwise.  */
bool
Positions::sort ()
{
  if (_useall)
    return true;

  /* Bubble sort.  */
  bool duplicate_free = true;
  int *base = _positions;
  unsigned int len = _size;

  for (unsigned int i = 1; i < len; i++)
    {
      unsigned int j;
      int tmp;

      for (j = i, tmp = base[j]; j > 0 && tmp >= base[j - 1]; j--)
        if ((base[j] = base[j - 1]) == tmp) /* oh no, a duplicate!!! */
          duplicate_free = false;

      base[j] = tmp;
    }

  return duplicate_free;
}

/* Creates an iterator, returning the positions in descending order.  */
PositionIterator
Positions::iterator () const
{
  return PositionIterator (*this);
}

/* Creates an iterator, returning the positions in descending order,
   that apply to strings of length <= maxlen.  */
PositionIterator
Positions::iterator (int maxlen) const
{
  return PositionIterator (*this, maxlen);
}

/* Initializes an iterator through POSITIONS.  */
PositionIterator::PositionIterator (Positions const& positions)
  : _set (positions),
    _index (0)
{
}

/* Initializes an iterator through POSITIONS, ignoring positions >= maxlen.  */
PositionIterator::PositionIterator (Positions const& positions, int maxlen)
  : _set (positions)
{
  if (positions._useall)
    _index = (maxlen <= Positions::MAX_KEY_POS ? Positions::MAX_KEY_POS - maxlen : 0);
  else
    {
      unsigned int index;
      for (index = 0;
           index < positions._size && positions._positions[index] >= maxlen;
           index++)
        ;
      _index = index;
    }
}

/* Retrieves the next position, or EOS past the end.  */
int
PositionIterator::next ()
{
  return (_index < _set._size ? _set._positions[_index++] : EOS);
}

/* Returns the number of remaining positions, i.e. how often next() will
   return a value != EOS.  */
unsigned int
PositionIterator::remaining () const
{
  return _set._size - _index;
}

/* Copy constructor.  */
PositionIterator::PositionIterator (const PositionIterator& src)
  : _set (src._set),
    _index (src._index)
{
}

/* Initializes an iterator through POSITIONS.  */
PositionReverseIterator::PositionReverseIterator (Positions const& positions)
  : _set (positions),
    _index (_set._size),
    _minindex (0)
{
}

/* Initializes an iterator through POSITIONS, ignoring positions >= maxlen.  */
PositionReverseIterator::PositionReverseIterator (Positions const& positions, int maxlen)
  : _set (positions),
    _index (_set._size)
{
  if (positions._useall)
    _minindex = (maxlen <= Positions::MAX_KEY_POS ? Positions::MAX_KEY_POS - maxlen : 0);
  else
    {
      unsigned int index;
      for (index = 0;
           index < positions._size && positions._positions[index] >= maxlen;
           index++)
        ;
      _minindex = index;
    }
}

/* Retrieves the next position, or EOS past the end.  */
int
PositionReverseIterator::next ()
{
  return (_index > _minindex ? _set._positions[--_index] : EOS);
}

/* Returns the number of remaining positions, i.e. how often next() will
   return a value != EOS.  */
unsigned int
PositionReverseIterator::remaining () const
{
  return _index - _minindex;
}

/* Copy constructor.  */
PositionReverseIterator::PositionReverseIterator (const PositionReverseIterator& src)
  : _set (src._set),
    _index (src._index),
    _minindex (src._minindex)
{
}


/* Creates an iterator, returning the positions in ascending order.  */
PositionReverseIterator
Positions::reviterator () const
{
  return PositionReverseIterator (*this);
}

/* Creates an iterator, returning the positions in ascending order,
   that apply to strings of length <= maxlen.  */
PositionReverseIterator
Positions::reviterator (int maxlen) const
{
  return PositionReverseIterator (*this, maxlen);
}

/* Set operations.  Assumes the array is in reverse order.  */

bool
Positions::contains (int pos) const
{
  unsigned int count = _size;
  const int *p = _positions + _size - 1;

  for (; count > 0; p--, count--)
    {
      if (*p == pos)
        return true;
      if (*p > pos)
        break;
    }
  return false;
}

void
Positions::add (int pos)
{
  set_useall (false);

  unsigned int count = _size;

  if (count == MAX_SIZE)
    {
      fprintf (stderr, "Positions::add internal error: overflow\n");
      exit (1);
    }

  int *p = _positions + _size - 1;

  for (; count > 0; p--, count--)
    {
      if (*p == pos)
        {
          fprintf (stderr, "Positions::add internal error: duplicate\n");
          exit (1);
        }
      if (*p > pos)
        break;
      p[1] = p[0];
    }
  p[1] = pos;
  _size++;
}

void
Positions::remove (int pos)
{
  set_useall (false);

  unsigned int count = _size;
  if (count > 0)
    {
      int *p = _positions + _size - 1;

      if (*p == pos)
        {
          _size--;
          return;
        }
      if (*p < pos)
        {
          int prev = *p;

          for (;;)
            {
              p--;
              count--;
              if (count == 0)
                break;
              if (*p == pos)
                {
                  *p = prev;
                  _size--;
                  return;
                }
              if (*p > pos)
                break;
              int curr = *p;
              *p = prev;
              prev = curr;
            }
        }
    }
  fprintf (stderr, "Positions::remove internal error: not found\n");
  exit (1);
}

/* Output in external syntax.  */
void
Positions::print () const
{
  if (_useall)
    printf ("*");
  else
    {
      bool first = true;
      bool seen_LASTCHAR = false;
      unsigned int count = _size;
      const int *p = _positions + _size - 1;

      for (; count > 0; p--)
        {
          count--;
          if (*p == LASTCHAR)
            seen_LASTCHAR = true;
          else
            {
              if (!first)
                printf (",");
              printf ("%d", *p + 1);
              if (count > 0 && p[-1] == *p + 1)
                {
                  printf ("-");
                  do
                    {
                      p--;
                      count--;
                    }
                  while (count > 0 && p[-1] == *p + 1);
                  printf ("%d", *p + 1);
                }
              first = false;
            }
        }
      if (seen_LASTCHAR)
        {
          if (!first)
            printf (",");
          printf ("$");
        }
    }
}
