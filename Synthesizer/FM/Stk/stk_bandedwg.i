%module bandedwg
%{

#include "ADSR.h"
#include "Effect.h"
#include "Filter.h"
#include "Generator.h"
#include "Instrmnt.h"
#include "Filter.h"
#include "Function.h"
#include "FM.h"

using namespace stk;

%}

%ignore stk::Stk;
%include "stk_core.i"
%include "bandedwg.i"
