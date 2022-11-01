%module ladspa
%{
#include "ladspamm.h"
#include "dl.h"
#include "library.h"
#include "plugin.h"
#include "plugin_instance.h"
#include "world.h"
using namespace ladspamm1;
%}

%include "stdint.i"
%include "std_vector.i"
%include "std_string.i"

%include "ladspamm.h"
%include "dl.h"
%include "library.h"
%include "plugin.h"
%include "plugin_instance.h"
%include "world.h"

%extend ladspamm1::library
{
    ladspamm1::plugin& __getitem(size_t i)  {
        return *($self->plugins[i].get());
    }

    ladspamm1::plugin_ptr get_plugin_ptr(size_t i) {
        return $self->plugins[i];
    }

    size_t size() const { return $self->plugins.size(); }
}

%extend ladspamm1::world {

    ladspamm1::library& __getitem(size_t i) {
        return *($self->libraries[i].get());
    }

    size_t size() const { return $self->libraries.size(); }
}