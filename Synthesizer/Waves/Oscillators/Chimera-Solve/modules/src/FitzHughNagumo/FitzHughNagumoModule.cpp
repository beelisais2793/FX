#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <boost/numeric/ublas/vector.hpp>
#include "lua.hpp"

#include "def.hpp"
#include "StateSynchrony.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "ParameterValueCollection.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ChimeraSystem.hpp"
#include "ChimeraContext.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/SystemDynamicModule.hpp"
#include "EntryPointBase/TemplateOdeSystem.hpp"
#include "FitzHughNagumo/FitzHughNagumo.hpp"
#include "FitzHughNagumo/FitzHughNagumoModule.hpp"

namespace ublas = boost::numeric::ublas;

extern "C"
{
    chimera::Module* LoadModule()
    {
        return new FitzHughNagumoModule();
    }

    void UnloadModule(chimera::Module* module)
    {
        delete (FitzHughNagumoModule*)module;
    }
}

/*
int rk_test(lua_State* L)
{
    FitzHughNagumo* inc = *((FitzHughNagumo**)lua_touserdata(L, lua_upvalueindex(1)));
    return 0;
}
*/

FitzHughNagumoModule::FitzHughNagumoModule()
{
    //setValue("keks", {ParameterTypeSystem::getParameterID(Naming::Type_real), (void*)new double(17.349)});
    //registerMethod("test", rk_test);
}

void* FitzHughNagumoModule::getInstance(chimera::EntryPoint const * const entrypoint, chimera::vec_t_LuaItem& parameters) const
{
    bool inA = false;
    bool inEpsilon = false;
    double a = 0.0;
    double epsilon = 0.0;

    if(parameters.size() == 1 && parameters[0].getType() == chimera::systemtypes::PID_TABLE)
    {
        chimera::map_t_LuaItem* paramMap = (chimera::map_t_LuaItem*)parameters[0].getValue();
        for(auto p : *paramMap)
        {
            if(p.first == "a" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                inA = true;
                a = p.second;
            }
            if(p.first == "epsilon" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                inEpsilon = true;
                epsilon = p.second;
            }
        }
    }
    else
    {
        if(parameters.size() > 0 && chimera::systemtypes::PID_NUMBER == parameters[0].getType())
        {
            inEpsilon = true;
            epsilon = parameters[0];
        }
        if(parameters.size() > 1 && chimera::systemtypes::PID_NUMBER == parameters[1].getType())
        {
            inA = true;
            a = parameters[1];
        }
    }

    if(!(inA && inEpsilon))
    {
        return nullptr;
    }

    return new FitzHughNagumo(getContext(), epsilon, a);
}

const std::string FitzHughNagumoModule::getGUID() const
{
    return "FitzHugh-Nagumo";
}

void FitzHughNagumoModule::destroyInstance(chimera::EntryPoint const * const entrypoint, void* instance) const
{
    delete ((FitzHughNagumo*)instance);
}
