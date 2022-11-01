#include <iostream>
#include "laguerre.h"

using namespace std;
using namespace Storage_B::Laguerre;

int main()
{
  double ln ;

  cout.precision(5) ;
  for (unsigned int n = 0 ; n <= 5 ; n++)
  {
    for (double x = -1.0 ; x <= 1.0 ; x = x + 0.1)
    { 
      ln = Ln<double>(n, x) ;
      cout << "L" << n << "(" << x << ") = " << ln << endl ;
    }
    cout << endl ;
  }

  return 0 ;
}
