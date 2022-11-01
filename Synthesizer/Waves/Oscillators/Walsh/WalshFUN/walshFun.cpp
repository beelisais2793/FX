/*
* File: walshFun.cpp
* Purpose: Walsh functions for Arduino
* Version: 1.0.0
* Release date: 28-01-2021
* Last edit:
*
* Version history:
*
*
* URL: https://github.com/MartinStokroos/WalshFUN
* License: MIT License
*
*
* Copyright (c) M.Stokroos 2021
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation 
* files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, 
* modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the 
* Software is furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
* WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*/

#include "walshFun.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


bool walshFun::wal8(unsigned char _n, unsigned char _t) {
  unsigned char n, t, i;

  n = _n & 7;     // truncate input range of n and t to three bits.
  t = _t & 7;

  if (n == 0) {
        return true;
  }
  else {
    i = pgm_read_word(gray_lut8 + n);
    return !parityTable256[i & t]; //XOR (determine even/uneven parity)
  }
}


bool walshFun::wal16(unsigned char _n, unsigned char _t) {
  unsigned char n, t, i;

  n = _n & 15;     // truncate input range of n and t to four bits.
  t = _t & 15;

  if (n == 0) {
        return true;
  }
  else {
    i = pgm_read_word(gray_lut16 + n);
    return !parityTable256[i & t]; //XOR (determine even/uneven parity)
  }
}


bool walshFun::wal32(unsigned char _n, unsigned char _t) {
  unsigned char n, t, i;

  n = _n & 31;     // truncate input range of n and t to five bits.
  t = _t & 31;

  if (n == 0) {
        return true;
  }
  else {
    i = pgm_read_word(gray_lut32 + n);
    return !parityTable256[i & t]; //XOR (determine even/uneven parity)
  }
}


bool walshFun::wal64(unsigned char _n, unsigned char _t) {
  unsigned char n, t, i;

  n = _n & 63;     // truncate input range of n and t to six bits.
  t = _t & 63;

  if (n == 0) {
        return true;
  }
  else {
    i = pgm_read_word(gray_lut64 + n);
    return !parityTable256[i & t]; //XOR (determine even/uneven parity)
  }
}


bool walshFun::wal128(unsigned char _n, unsigned char _t) {
  unsigned char n, t, i;

  n = _n & 127;     // truncate input range of n and t to seven bits.
  t = _t & 127;

  if (n == 0) {
        return true;
  }
  else {
    i = pgm_read_word(gray_lut128 + n);
    return !parityTable256[i & t]; //XOR (determine even/uneven parity)
  }
}


bool walshFun::wal256(unsigned char _n, unsigned char _t) {
  unsigned char n, t, i;

  n = _n;
  t = _t;

  if (n == 0) {
        return true;
  }
  else {
    i = pgm_read_word(gray_lut256 + n);
    return !parityTable256[i & t]; //XOR (determine even/uneven parity)
  }
}


// end of walshFun.cpp
