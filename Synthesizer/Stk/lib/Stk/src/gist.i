%module luagist 
%{
#define USE_FFTW
#include <Gist.h>    
%}
#define USE_FFTW
%include <Gist.h>

%template (GistF) Gist<float>;
%template (GistD) Gist<double>;

