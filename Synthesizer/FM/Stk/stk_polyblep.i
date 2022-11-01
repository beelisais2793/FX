%module stk_polyblep
%{
#include "Stk.h"
#include "Generator.h"
#include "PolyBLEPStk.h"

%}
%ignore stk::Stk;
%include "stk_core.i"
%include "PolyBLEP.h"
%include "PolyBLEPStk.h"
