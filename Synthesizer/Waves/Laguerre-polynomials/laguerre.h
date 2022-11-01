//==================================================================
/**
 *  laguerre.h -- C++ functions to evaluate Laguerre polynomials
 *
 *  Copyright (C) 2019 by James A. Chappell
 *
 *  Permission is hereby granted, free of charge, to any person
 *  obtaining a copy of this software and associated documentation
 *  files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use,
 *  copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following
 *  condition:
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *  OTHER DEALINGS IN THE SOFTWARE.
 */
//=================================================================
/*
 * legendre.h:  Version 0.01
 * Created by James A. Chappell <rlrrlrll@gmail.com>
 * http://www.storage-b.com/XXXXX
 * Created 22 November 2019
 *
 * History:
 * 22-nov-2019 created
 */
//==============

#pragma once
/*
 *	Function calculates Laguerre Polynomials Ln(x)
 */
namespace Storage_B
{
  namespace Laguerre
  {
    // n = 0
    template <class T> inline auto L0(const T& x)
    {
      return static_cast<T>(1);
    }

    // n = 1
    template <class T> inline auto L1(const T& x)
    {
      return static_cast<T>(1) - x;
    }

/*
 *	  Ln(x)
 */
    template <class T> inline auto Ln(unsigned int n, const T& x)
    {
      switch(n)
      {
        case 0:
          return L0<T>(x);

        case 1:
          return L1<T>(x);

        default:
          break;
      }
/*
      auto n1 = static_cast<T>(n - 1);

      return ((((static_cast<T>(2) * n1)
              + static_cast<T>(1) - x) * Ln(static_cast<unsigned int>(n1), x))
              - (n1 * Ln(n - 2, x))) / static_cast<T>(n); */

      auto lnm1(L1<T>(x));
      auto lnm2(L0<T>(x));
      T ln;

      T m1;
      for (auto m = 2u ; m <=n ; ++m)
      {
        m1 = static_cast<T>(m - 1);
        ln = ((((static_cast<T>(2) *  m1)
                + static_cast<T>(1) - x) * lnm1)
              - (m1 * lnm2)) / static_cast<T>(m);
        lnm2 = lnm1;
        lnm1 = ln;
      }

      return ln;
    }
  }
}
