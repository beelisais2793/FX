

double DSF (double x,  // input
            double a,  // a<1.0
            double N,  // N<SmplFQ/2,
            double fi) // phase
{
  double s1 = pow(a,N-1.0)*sin((N-1.0)*x+fi);
  double s2 = pow(a,N)*sin(N*x+fi);
  double s3 = a*sin(x+fi);
  double s4 =1.0 - (2*a*cos(x)) +(a*a);
  if (s4==0)
     return 0;
  else
     return (sin(fi) - s3 - s2 +s1)/s4;
}
