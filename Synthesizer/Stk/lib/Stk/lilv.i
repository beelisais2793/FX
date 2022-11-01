%module lilv
%{
#include "lilv.h"
#include "lilvmm.hpp"
%}
%ignore lilv_plugin_get_num_ports_of_class_va;
%include "lilv.h"
%include "lilvpp.hpp"