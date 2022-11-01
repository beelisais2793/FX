#include <vector>
#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include "lua.hpp"

#include "def.hpp"
#include "Naming.hpp"
#include "ExtensionNaming.hpp"
#include "RuntimeNames.hpp"
//#include "interfaces/IConnectEventHandler.hpp"
//#include "lua/lua_static.hpp"
#include "StateSynchrony.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "TypeLookup.hpp"
//#include "lua/LuaParser.hpp"
#include "interfaces/IEventListener.hpp"
#include "interfaces/IEventListenerProvider.hpp"
#include "event/StateEventListener.hpp"
#include "event/Observer.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/DefaultEventProvider.hpp"
#include "event/NotificationManager.hpp"
#include "ParameterValueCollection.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "EntryPointBase/OutputModule.hpp"
#include "EntryPointBase/OutputMultiModule.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/AbstractIntegrator.hpp"
#include "EntryPointBase/AbstractTemporalIntegrator.hpp"
#include "simulation/TemporalStateEventProvider.hpp"
#include "simulation/AbstractSimulation.hpp"
#include "simulation/TemporalSimulation.hpp"
#include "ChimeraSystem.hpp"
#include "ChimeraRuntime.hpp"
#include "lua/types/lua_TemporalSimulation.hpp"

/*
int chimera::runtime::lua_TemporalSimulation_add_event(lua_State* L)
{
    static const size_t basetype = chimera::systemtypes::PID_INSTANCE;
    static size_t tagtype;

    if(lua_gettop(L) > 0 && luaL_getmetafield(L, 1, "__type"))
    {
        int type = lua_tointeger(L, -1);
        lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_INSTANCE);
        lua_gettable(L, LUA_REGISTRYINDEX);
        chimera::ChimeraSystem* chimeraSystem = (chimera::ChimeraSystem*)lua_touserdata(L, -1);
        lua_pop(L, 1);

        chimera::EntryPoint* ep = chimeraSystem->getEntryPointSystem()->getEntryPoint(chimera::simulation::Naming::EntryPoint_observer);
        tagtype = (size_t)ep;

        if(chimeraSystem->getTypeSystem()->getParameterBase(type) == basetype && chimeraSystem->getTypeSystem()->getParameterTag(type) == tagtype)
        {
            TemporalSimulation* sim = dynamic_cast<TemporalSimulation*>
                (*((chimera::simulation::AbstractSimulation**)lua_touserdata(L, lua_upvalueindex(1))));
            chimera::simulation::AbstractEventProvider* provider = *((chimera::simulation::AbstractEventProvider**)lua_touserdata(L, 1));
            sim->registerEvent(provider);
            lua_pushboolean(L, 1);
            return 1;
        }
    }
    lua_pushboolean(L, 0);
    return 1;
}

int chimera::runtime::lua_TemporalSimulation_register_eventlistener(lua_State* L)
{
//, (size_t)ep
    static const size_t basetype = chimera::systemtypes::PID_INSTANCE;
    static size_t tagtype;

    if(lua_gettop(L) > 0 && luaL_getmetafield(L, 1, "__type"))
    {
        int type = lua_tointeger(L, -1);
        lua_pushstring(L, chimera::registrynames::LUA_REGISTRY_CHIMERA_INSTANCE);
        lua_gettable(L, LUA_REGISTRYINDEX);
        chimera::ChimeraSystem* chimeraSystem = (chimera::ChimeraSystem*)lua_touserdata(L, -1);
        lua_pop(L, 1);

        chimera::EntryPoint* ep = chimeraSystem->getEntryPointSystem()->getEntryPoint(chimera::simulation::Naming::EntryPoint_output);
        tagtype = (size_t)ep;

        if(chimeraSystem->getTypeSystem()->getParameterBase(type) == basetype && chimeraSystem->getTypeSystem()->getParameterTag(type) == tagtype)
        {
            if(luaL_getmetafield(L, 1, "__modptr"))
            {
                const Module* mod = (const Module*)lua_touserdata(L, -1);
                lua_pop(L, 1);
                if(mod)
                {
                    TemporalSimulation* sim = dynamic_cast<TemporalSimulation*>
                        (*((chimera::simulation::AbstractSimulation**)lua_touserdata(L, lua_upvalueindex(1))));

                    const chimera::simulation::OutputModule* mod_single = dynamic_cast<const chimera::simulation::OutputModule*>(mod);
                    if(mod_single)
                    {
                        chimera::simulation::IEventListener* listen = *((chimera::simulation::IEventListener**)lua_touserdata(L, 1));
                        if(listen)
                        {
                            sim->addListener(lua_tostring(L, lua_upvalueindex(2)), listen);
                        }
                        return 0;
                    }

                    const chimera::simulation::OutputMultiModule* mod_multi = dynamic_cast<const chimera::simulation::OutputMultiModule*>(mod);
                    if(mod_multi)
                    {
                        chimera::simulation::IEventListenerProvider* provider = *((chimera::simulation::IEventListenerProvider**)lua_touserdata(L, 1));
                        if(provider)
                        {
                            sim->addListener(lua_tostring(L, lua_upvalueindex(2)), provider);
                        }
                        return 0;
                    }
                }
            }

            return 0;
        }

        //LoggingSystem::Error("Parameter `1` is not a valid event listener");
        return 0;
    }
    //LoggingSystem::Error("Expected parameter `1` to be event listener ");
    return 0;
}

int chimera::runtime::lua_TemporalSimulation_index(lua_State* L)
{
    TemporalSimulation* sim = dynamic_cast<TemporalSimulation*>
        (*((chimera::simulation::AbstractSimulation**)lua_touserdata(L, lua_upvalueindex(1))));

    std::string fname = lua_tostring(L, 2);
    if(fname.size() > 2 && fname[0] == 'o' && fname[1] == 'n')
    {
        std::string ename = fname.substr(2);

        if(sim->hasEvent(ename))
        {
            lua_pushvalue(L, lua_upvalueindex(1));
            lua_pushstring(L, ename.c_str());
            lua_pushcclosure (L, lua_TemporalSimulation_register_eventlistener, 2);
            return 1;
        }
    }


    return 0;
}
*/

int chimera::runtime::lua_TemporalSimulation_start(lua_State* L)
{
    ChimeraRuntime* chimeraSystem = (ChimeraRuntime*)(*((void**)lua_getextraspace(L)));

    lua_State* NL = (lua_State*)lua_touserdata(L, lua_upvalueindex(1));
    TemporalSimulation* sim = dynamic_cast<TemporalSimulation*>
        (*((chimera::simulation::AbstractSimulation**)lua_touserdata(L, lua_upvalueindex(2))));
    if(!(sim->isRunning()))
    {
        vec_t_LuaItem items;
        for(int i = 1; i <= lua_gettop(L); i++)
        {
            items.push_back(chimeraSystem->getTypeSystem()->getValue(L, i));
        }
        sim->getIntegrator()->start(items);

//        lp->_rL = NL;
        chimeraSystem->setCurrentState(NL);
        lua_resume(NL, L, 0);
        chimeraSystem->setCurrentState(L);
//        lp->_rL = L;

        //lua_pushnumber(L, sim->getStep());
        chimeraSystem->getTypeSystem()->pushValue(L,
            ((chimera::simulation::AbstractTemporalIntegrator*)sim->getIntegrator())->getTimeType(),
            ((chimera::simulation::AbstractTemporalIntegrator*)sim->getIntegrator())->currentTime());
        return 1;
    }
    else
    {
        //LoggingSystem::Error("Simulation is already running.");
        return 0;
    }
}

int chimera::runtime::lua_TemporalSimulation_stop(lua_State* L)
{
    TemporalSimulation* sim = dynamic_cast<TemporalSimulation*>
        (*((chimera::simulation::AbstractSimulation**)lua_touserdata(L, lua_upvalueindex(2))));
    if(sim->isRunning())
    {
        sim->setYield(true);
    }
    else
    {
        //LoggingSystem::Error("Simulation was not started yet.");
    }

    return 0;
}

int chimera::runtime::lua_TemporalSimulation_current(lua_State* L)
{
    ChimeraRuntime* chimeraSystem = (ChimeraRuntime*)(*((void**)lua_getextraspace(L)));
    TemporalSimulation* sim = dynamic_cast<TemporalSimulation*>
        (*((chimera::simulation::AbstractSimulation**)lua_touserdata(L, lua_upvalueindex(1))));
    chimeraSystem->getTypeSystem()->pushValue(L,
        ((chimera::simulation::AbstractTemporalIntegrator*)sim->getIntegrator())->getTimeType(),
        ((chimera::simulation::AbstractTemporalIntegrator*)sim->getIntegrator())->currentTime());
    chimeraSystem->getTypeSystem()->pushValue(L,
        ((chimera::simulation::AbstractTemporalIntegrator*)sim->getIntegrator())->getStateType(),
        ((chimera::simulation::AbstractTemporalIntegrator*)sim->getIntegrator())->currentState());
    return 2;
}

int chimera::runtime::lua_TemporalSimulation_step(lua_State* L)
{
    TemporalSimulation* sim = dynamic_cast<TemporalSimulation*>
        ((chimera::simulation::AbstractSimulation*)lua_touserdata(L, lua_upvalueindex(1)));
    if(!(sim->isRunning())) {
        sim->nextStep();
        ChimeraRuntime* chimeraSystem = (ChimeraRuntime*)(*((void**)lua_getextraspace(L)));
        chimeraSystem->getTypeSystem()->pushValue(L,
            ((chimera::simulation::AbstractTemporalIntegrator*)sim->getIntegrator())->getTimeType(),
            ((chimera::simulation::AbstractTemporalIntegrator*)sim->getIntegrator())->currentTime());
        return 1;
    }
    return 0;
}

int k (lua_State *L, int status, lua_KContext ctx)
{
    int result = chimera::runtime::lua_TemporalSimulation_run(L);
    return result;
}

int chimera::runtime::lua_TemporalSimulation_run(lua_State* L)
{
    TemporalSimulation* sim = dynamic_cast<TemporalSimulation*>
        ((chimera::simulation::AbstractSimulation*)lua_touserdata(L, lua_upvalueindex(1)));
    //if(lua_isfunction
    sim->setYield(false);
    while(sim->nextStep())
    {

    }

        //lua_getglobal(L, "func");
        //if(lua_isfunction(L, -1))

        if(false)
        {
            //lua_pushinteger(L, sim->getStep());
            //lua_pushinteger(L, 1);

            ChimeraRuntime* chimeraSystem = (ChimeraRuntime*)(*((void**)lua_getextraspace(L)));

            chimeraSystem->getTypeSystem()->pushValue(L,
                ((chimera::simulation::AbstractTemporalIntegrator*)sim->getIntegrator())->getTimeType(),
                ((chimera::simulation::AbstractTemporalIntegrator*)sim->getIntegrator())->currentTime());

            chimeraSystem->getTypeSystem()->pushValue(L,
                ((chimera::simulation::AbstractTemporalIntegrator*)sim->getIntegrator())->getStateType(),
                ((chimera::simulation::AbstractTemporalIntegrator*)sim->getIntegrator())->currentState());

            lua_pcall(L, 2, 0, 0);

            if(sim->getYield())
            {
                sim->setYield(false);
                lua_yieldk(L, 0, 0, k);
            }

        } else {
            lua_pop(L, 1);
        }

    return 0;
}

int chimera::runtime::lua_TemporalSimulation_on(lua_State* L)
{
    if (lua_gettop(L) > 0)
    {
        if (lua_type(L, 1) == LUA_TSTRING && lua_tostring(L, 1) == std::string("step"))
        {
            lua_remove(L, 1);
            return lua_TemporalSimulation_onStep(L);
        }
    }
    return 0;
}

int chimera::runtime::lua_TemporalSimulation_onStep(lua_State* L)
{
    ChimeraRuntime* chimeraSystem = (ChimeraRuntime*)(*((void**)lua_getextraspace(L)));
    // test for nullptr
    TemporalSimulation* sim = dynamic_cast<TemporalSimulation*>
        (*((chimera::simulation::AbstractSimulation**)lua_touserdata(L, lua_upvalueindex(1))));
    if (lua_gettop(L) > 0)
    {
        ParameterValue v(chimeraSystem->getTypeSystem()->getValue(L, 1));
        if(v.getAttribute(chimera::simulation::Naming::Attribute_MultiObserver))
        {
            chimera::simulation::IEventListenerProvider* provider = (chimera::simulation::IEventListenerProvider*)v.getValue();
            sim->addListener("step", provider);
            return 0;
        }

        if(v.getAttribute(chimera::simulation::Naming::Attribute_Observer))
        {
            chimera::simulation::IEventListener* listener = (chimera::simulation::IEventListener*)v.getValue();
            sim->addListener("step", listener);
            return 0;
        }
    }
    return 0;
}
