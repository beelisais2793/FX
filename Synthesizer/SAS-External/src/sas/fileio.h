/* portable file I/O
   Copyright (C) 1994-2001 Sylvain Marchand

   Changed by Anthony Beurivé, Mon Nov 26 15:25:55 CET 2001, to
   include a file called types.h in the original version.

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

#ifndef __BASE_FILEIO_H__
#define __BASE_FILEIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#ifndef bool
  #define bool int
#endif

#ifndef true
  #define true  1
#endif

#ifndef false
  #define false 0
#endif

#ifndef REAL
  #define REAL double
#endif

#ifndef BYTE
  #define BYTE char
#endif

#ifndef UBYTE
  #define UBYTE unsigned char
#endif

#ifndef WORD
  #define WORD short
#endif

#ifndef UWORD
  #define UWORD unsigned short
#endif

#ifndef LONG
  #define LONG long
#endif

#ifndef ULONG
  #define ULONG unsigned long
#endif

#define MakeID(c1,c2,c3,c4) (((c1<<24)&0xff000000)|((c2<<16)&0xff0000)|((c3<<8)&0xff00)|(c4&0xff))

extern          long IO_ConvertSigned   (int c);
extern unsigned long IO_ConvertUnsigned (int c);

/*-- Import -----------------------------------------------------------------*/

extern bool IO_skip (FILE *fp, LONG size);

extern bool IO_Read_Str (char *str, int size, FILE *fp);

extern bool IO_Read_BYTE  (BYTE  *buffer, FILE *fp);
extern bool IO_Read_UBYTE (UBYTE *buffer, FILE *fp);

extern bool IO_Read_BE_WORD  (WORD  *buffer, FILE *fp);
extern bool IO_Read_BE_UWORD (UWORD *buffer, FILE *fp);
extern bool IO_Read_LE_WORD  (WORD  *buffer, FILE *fp);
extern bool IO_Read_LE_UWORD (UWORD *buffer, FILE *fp);

extern bool IO_Read_BE_LONG  (LONG  *buffer, FILE *fp);
extern bool IO_Read_BE_ULONG (ULONG *buffer, FILE *fp);
extern bool IO_Read_LE_LONG  (LONG  *buffer, FILE *fp);
extern bool IO_Read_LE_ULONG (ULONG *buffer, FILE *fp);

extern bool IO_Read_BYTES  (BYTE  *buffer, int number, FILE *fp);
extern bool IO_Read_UBYTES (UBYTE *buffer, int number, FILE *fp);

extern bool IO_Read_BE_WORDS  (WORD  *buffer, int number, FILE *fp);
extern bool IO_Read_BE_UWORDS (UWORD *buffer, int number, FILE *fp);
extern bool IO_Read_LE_WORDS  (WORD  *buffer, int number, FILE *fp);
extern bool IO_Read_LE_UWORDS (UWORD *buffer, int number, FILE *fp);

extern bool IO_Read_BE_LONGS  (LONG  *buffer, int number, FILE *fp);
extern bool IO_Read_BE_ULONGS (ULONG *buffer, int number, FILE *fp);
extern bool IO_Read_LE_LONGS  (LONG  *buffer, int number, FILE *fp);
extern bool IO_Read_LE_ULONGS (ULONG *buffer, int number, FILE *fp);

/*-- Export -----------------------------------------------------------------*/

extern bool IO_Write_Str (char *str, int size, FILE *fp);

extern bool IO_Write_BYTE  (BYTE  value, FILE *fp);
extern bool IO_Write_UBYTE (UBYTE value, FILE *fp);

extern bool IO_Write_BE_WORD  (WORD  value, FILE *fp);
extern bool IO_Write_BE_UWORD (UWORD value, FILE *fp);
extern bool IO_Write_LE_WORD  (WORD  value, FILE *fp);
extern bool IO_Write_LE_UWORD (UWORD value, FILE *fp);

extern bool IO_Write_BE_LONG  (LONG  value, FILE *fp);
extern bool IO_Write_BE_ULONG (ULONG value, FILE *fp);
extern bool IO_Write_LE_LONG  (LONG  value, FILE *fp);
extern bool IO_Write_LE_ULONG (ULONG value, FILE *fp);

extern bool IO_Write_BYTES  (BYTE  *buffer, int number, FILE *fp);
extern bool IO_Write_UBYTES (UBYTE *buffer, int number, FILE *fp);

extern bool IO_Write_BE_WORDS  (WORD  *buffer, int number, FILE *fp);
extern bool IO_Write_BE_UWORDS (UWORD *buffer, int number, FILE *fp);
extern bool IO_Write_LE_WORDS  (WORD  *buffer, int number, FILE *fp);
extern bool IO_Write_LE_UWORDS (UWORD *buffer, int number, FILE *fp);

extern bool IO_Write_BE_LONGS  (LONG  *buffer, int number, FILE *fp);
extern bool IO_Write_BE_ULONGS (ULONG *buffer, int number, FILE *fp);
extern bool IO_Write_LE_LONGS  (LONG  *buffer, int number, FILE *fp);
extern bool IO_Write_LE_ULONGS (ULONG *buffer, int number, FILE *fp);

#ifdef __cplusplus
}
#endif

#endif /* !__BASE_FILEIO_H__ */

/* End Of File */
