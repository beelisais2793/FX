%module asymp
%{
#include "Asymp.h"

using namespace stk;
%}

%ignore stk::Stk;
%include "stk_core.i"
%include "asymp.i"
