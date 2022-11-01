/* portable file I/O
   Copyright (C) 1994-2001 Sylvain Marchand

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "fileio.h"


/* Following functions are very tricky...
   They convert the result of fgetc, taking care of the sign.
   They don't even suppose that host's arithmetic is 2-complement ;-)
   (but the value read by fgetc is...).	 */

long
IO_ConvertSigned (int c)
{
  return (c & 0x80) ? (((long)c) & 0x7f) - 128 : ((long)c) & 0x7f;
}

unsigned long
IO_ConvertUnsigned (int c)
{
  return ((unsigned long)c) & 0xff;
}


/*-- Import -----------------------------------------------------------------*/


bool
IO_skip (FILE *fp, LONG size)
{
  char c;

  assert ( fp && (size >= 0) );

  while (size > 0)
    {
      if (fread (&c, 1, 1, fp) != 1) { return false; }
      size--;
    }

  return true;
}


bool
IO_Read_Str (char *str, int size, FILE *fp)
{
  assert ( str && (size > 0) && fp );

#ifdef _WINDOWS
  return (fread (str, 1, size, fp) == (unsigned int) size);
#else
  return (fread (str, 1, size, fp) == size);
#endif
}


bool
IO_Read_BYTE (BYTE *buffer, FILE *fp)
{
  int c;

  assert (buffer && fp);

  if ( (c = fgetc (fp)) != EOF )
    {
      *buffer = (BYTE)(IO_ConvertSigned (c));

      return true;
    }
  else
    {
      return false;
    }
}

bool
IO_Read_UBYTE (UBYTE *buffer, FILE *fp)
{
  int c;

  assert (buffer && fp);

  if ( (c = fgetc (fp)) != EOF )
    {
      *buffer = (UBYTE)(IO_ConvertUnsigned (c));

      return true;
    }
  else
    {
      return false;
    }
}


bool
IO_Read_BE_WORD (WORD *buffer, FILE *fp)
{
  int c1, c2;

  assert (buffer && fp);

  if ( ((c1 = fgetc (fp)) == EOF) || ((c2 = fgetc (fp)) == EOF) )
    {
      return false;
    }
  else
    {
      WORD b1 = (WORD)(IO_ConvertSigned (c1));
      WORD b2 = ((WORD)(c2)) & 0xFF;

      *buffer = (b1 << 8) | b2;

      return true;
    }
}

bool
IO_Read_BE_UWORD (UWORD *buffer, FILE *fp)
{
  int c1, c2;

  assert (buffer && fp);

  if ( ((c1 = fgetc (fp)) == EOF) || ((c2 = fgetc (fp)) == EOF) )
    {
      return false;
    }
  else
    {
      UWORD b1 = (UWORD)(IO_ConvertUnsigned (c1));
      UWORD b2 = ((UWORD)c2) & 0xFF;

      *buffer = (b1 << 8) | b2;

      return true;
    }
}

bool
IO_Read_LE_WORD (WORD *buffer, FILE *fp)
{
  int c1, c2;

  assert (buffer && fp);

  if ( ((c1 = fgetc (fp)) == EOF) || ((c2 = fgetc (fp)) == EOF) )
    {
      return false;
    }
  else
    {
      WORD b1 = (WORD)(IO_ConvertSigned (c2));
      WORD b2 = ((WORD)c1) & 0xFF;

      *buffer = (b1 << 8) | b2;

      return true;
    }
}

bool
IO_Read_LE_UWORD (UWORD *buffer, FILE *fp)
{
  int c1, c2;

  assert (buffer && fp);

  if ( ((c1 = fgetc (fp)) == EOF) || ((c2 = fgetc (fp)) == EOF) )
    {
      return false;
    }
  else
    {
      UWORD b1 = (UWORD)(IO_ConvertUnsigned (c2));
      UWORD b2 = ((UWORD)c1) & 0xFF;

      *buffer = (b1 << 8) | b2;

      return true;
    }
}


bool
IO_Read_BE_LONG (LONG *buffer, FILE *fp)
{
  int c1, c2, c3, c4;

  assert (buffer && fp);

  if (((c1 = fgetc (fp)) == EOF) ||
      ((c2 = fgetc (fp)) == EOF) ||
      ((c3 = fgetc (fp)) == EOF) ||
      ((c4 = fgetc (fp)) == EOF))
    {
      return false;
    }
  else
    {
      LONG b1 = (LONG)(IO_ConvertSigned (c1));
      LONG b2 = ((LONG)c2) & 0xFF;
      LONG b3 = ((LONG)c3) & 0xFF;
      LONG b4 = ((LONG)c4) & 0xFF;

      *buffer = (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;

      return true;
    }
}

bool
IO_Read_BE_ULONG (ULONG *buffer, FILE *fp)
{
  int c1, c2, c3, c4;

  assert (buffer && fp);

  if (((c1 = fgetc (fp)) == EOF) ||
      ((c2 = fgetc (fp)) == EOF) ||
      ((c3 = fgetc (fp)) == EOF) ||
      ((c4 = fgetc (fp)) == EOF))
    {
      return false;
    }
  else
    {
      ULONG b1 = (ULONG)(IO_ConvertUnsigned (c1));
      ULONG b2 = ((ULONG)c2) & 0xFF;
      ULONG b3 = ((ULONG)c3) & 0xFF;
      ULONG b4 = ((ULONG)c4) & 0xFF;

      *buffer = (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;

      return true;
    }
}

bool
IO_Read_LE_LONG (LONG *buffer, FILE *fp)
{
  int c1, c2, c3, c4;

  assert (buffer && fp);

  if (((c1 = fgetc (fp)) == EOF) ||
      ((c2 = fgetc (fp)) == EOF) ||
      ((c3 = fgetc (fp)) == EOF) ||
      ((c4 = fgetc (fp)) == EOF))
    {
      return false;
    }
  else
    {
      LONG b1 = (LONG)(IO_ConvertSigned (c4));
      LONG b2 = ((LONG)c3) & 0xFF;
      LONG b3 = ((LONG)c2) & 0xFF;
      LONG b4 = ((LONG)c1) & 0xFF;

      *buffer = (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;

      return true;
    }
}

bool
IO_Read_LE_ULONG (ULONG *buffer, FILE *fp)
{
  int c1, c2, c3, c4;

  assert (buffer && fp);

  if (((c1 = fgetc (fp)) == EOF) ||
      ((c2 = fgetc (fp)) == EOF) ||
      ((c3 = fgetc (fp)) == EOF) ||
      ((c4 = fgetc (fp)) == EOF))
    {
      return false;
    }
  else
    {
      ULONG b1 = (ULONG)(IO_ConvertUnsigned (c4));
      ULONG b2 = ((ULONG)c3) & 0xFF;
      ULONG b3 = ((ULONG)c2) & 0xFF;
      ULONG b4 = ((ULONG)c1) & 0xFF;

      *buffer = (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
      return true;
    }
}


#define DEFINE_MULTIPLE_READ_FUNCTION(type_name, type)       \
bool                                                         \
IO_Read_##type_name##S (type *buffer, int number, FILE *fp)  \
{                                                            \
  assert ( buffer && (number > 0) && fp );                   \
                                                             \
  while (number > 0)                                         \
    {                                                        \
      if (!IO_Read_##type_name (buffer, fp)) return false;   \
      buffer++; number--;                                    \
    }                                                        \
                                                             \
  return true;                                               \
}

DEFINE_MULTIPLE_READ_FUNCTION ( BYTE,  BYTE)
DEFINE_MULTIPLE_READ_FUNCTION (UBYTE, UBYTE)

DEFINE_MULTIPLE_READ_FUNCTION (BE_WORD,   WORD)
DEFINE_MULTIPLE_READ_FUNCTION (BE_UWORD, UWORD)
DEFINE_MULTIPLE_READ_FUNCTION (LE_WORD,   WORD)
DEFINE_MULTIPLE_READ_FUNCTION (LE_UWORD, UWORD)

DEFINE_MULTIPLE_READ_FUNCTION (BE_LONG,   LONG)
DEFINE_MULTIPLE_READ_FUNCTION (BE_ULONG, ULONG)
DEFINE_MULTIPLE_READ_FUNCTION (LE_LONG,   LONG)
DEFINE_MULTIPLE_READ_FUNCTION (LE_ULONG, ULONG)


/*-- Export -----------------------------------------------------------------*/


bool
IO_Write_Str (char *str, int size, FILE *fp)
{
  assert ( str && (size > 0) && fp );

#ifdef _WINDOWS
  return (fwrite (str, 1, size, fp) == (unsigned int) size);
#else
  return (fwrite (str, 1, size, fp) == size);
#endif
}


bool
IO_Write_BYTE (BYTE value, FILE *fp)
{
  assert (fp);

  return (fputc (value, fp) != EOF);
}

bool
IO_Write_UBYTE (UBYTE value, FILE *fp)
{
  assert (fp);

  return (fputc (value, fp) != EOF);
}


bool
IO_Write_BE_WORD (WORD value, FILE *fp)
{
  int c1, c2;

  assert (fp);

  c1 = value & 0xff;
  c2 = (value >> 8) & 0xff;

  return ((fputc (c2, fp) != EOF) && (fputc (c1, fp) != EOF));
}

bool
IO_Write_BE_UWORD (UWORD value, FILE *fp)
{
  int c1, c2;

  assert (fp);

  c1 = value & 0xff;
  c2 = (value >> 8) & 0xff;

  return ((fputc (c2, fp) != EOF) && (fputc (c1, fp) != EOF));
}

bool
IO_Write_LE_WORD (WORD value, FILE *fp)
{
  int c1, c2;

  assert (fp);

  c1 = value & 0xff;
  c2 = (value >> 8) & 0xff;

  return ((fputc (c1, fp) != EOF) && (fputc (c2, fp) != EOF));
}

bool
IO_Write_LE_UWORD (UWORD value, FILE *fp)
{
  int c1, c2;

  assert (fp);

  c1 = value & 0xff;
  c2 = (value >> 8) & 0xff;

  return ((fputc (c1, fp) != EOF) && (fputc (c2, fp) != EOF));
}


bool
IO_Write_BE_LONG (LONG value, FILE *fp)
{
  int c1, c2, c3, c4;

  assert (fp);

  c1 = value & 0xff;
  c2 = (value >> 8) & 0xff;
  c3 = (value >> 16) & 0xff;
  c4 = (value >> 24) & 0xff;

  return ((fputc (c4, fp) != EOF) &&
	  (fputc (c3, fp) != EOF) &&
	  (fputc (c2, fp) != EOF) &&
	  (fputc (c1, fp) != EOF));
}

bool
IO_Write_BE_ULONG (ULONG value, FILE *fp)
{
  int c1, c2, c3, c4;

  assert (fp);

  c1 = value & 0xff;
  c2 = (value >> 8) & 0xff;
  c3 = (value >> 16) & 0xff;
  c4 = (value >> 24) & 0xff;

  return ((fputc (c4, fp) != EOF) &&
	  (fputc (c3, fp) != EOF) &&
	  (fputc (c2, fp) != EOF) &&
	  (fputc (c1, fp) != EOF));
}

bool
IO_Write_LE_LONG (LONG value, FILE *fp)
{
  int c1, c2, c3, c4;

  assert (fp);

  c1 = value & 0xff;
  c2 = (value >> 8) & 0xff;
  c3 = (value >> 16) & 0xff;
  c4 = (value >> 24) & 0xff;

  return ((fputc (c1, fp) != EOF) &&
	  (fputc (c2, fp) != EOF) &&
	  (fputc (c3, fp) != EOF) &&
	  (fputc (c4, fp) != EOF));
}

bool
IO_Write_LE_ULONG (ULONG value, FILE *fp)
{
  int c1, c2, c3, c4;

  assert (fp);

  c1 = value & 0xff;
  c2 = (value >> 8) & 0xff;
  c3 = (value >> 16) & 0xff;
  c4 = (value >> 24) & 0xff;

  return ((fputc (c1, fp) != EOF) &&
	  (fputc (c2, fp) != EOF) &&
	  (fputc (c3, fp) != EOF) &&
	  (fputc (c4, fp) != EOF));
}


#define DEFINE_MULTIPLE_WRITE_FUNCTION(type_name, type)       \
bool                                                          \
IO_Write_##type_name##S (type *buffer, int number, FILE *fp)  \
{                                                             \
  assert ( buffer && (number > 0) && fp );                    \
                                                              \
  while (number > 0)                                          \
    {                                                         \
      if (!IO_Write_##type_name (*buffer, fp)) return false;  \
      buffer++; number--;                                     \
    }                                                         \
                                                              \
  return true;                                                \
}

DEFINE_MULTIPLE_WRITE_FUNCTION ( BYTE,  BYTE)
DEFINE_MULTIPLE_WRITE_FUNCTION (UBYTE, UBYTE)

DEFINE_MULTIPLE_WRITE_FUNCTION (BE_WORD,   WORD)
DEFINE_MULTIPLE_WRITE_FUNCTION (BE_UWORD, UWORD)
DEFINE_MULTIPLE_WRITE_FUNCTION (LE_WORD,   WORD)
DEFINE_MULTIPLE_WRITE_FUNCTION (LE_UWORD, UWORD)

DEFINE_MULTIPLE_WRITE_FUNCTION (BE_LONG,   LONG)
DEFINE_MULTIPLE_WRITE_FUNCTION (BE_ULONG, ULONG)
DEFINE_MULTIPLE_WRITE_FUNCTION (LE_LONG,   LONG)
DEFINE_MULTIPLE_WRITE_FUNCTION (LE_ULONG, ULONG)

/* End Of File */
