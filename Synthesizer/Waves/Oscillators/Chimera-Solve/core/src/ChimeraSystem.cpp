#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>
//#include "lua.hpp"

#include "def.hpp"
#include "Naming.hpp"
#include "StateSynchrony.hpp"
#include "types/lua_basetypes.hpp"
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
#include "ChimeraContext.hpp"
#include "ChimeraSystem.hpp"

chimera::ChimeraSystem::ChimeraSystem()
{
    _loggingSys = new LoggingSystem();
    _typeSys = new ParameterTypeSystem(this);
    _epSys = new EntryPointSystem(this);
    _context = createContext(this);
    _epSys->addListener(this);
    _typeSys->addListener(this);

    init();
}

chimera::ChimeraSystem::~ChimeraSystem()
{
    delete _context;
    delete _loggingSys;
    if(_typeSys != nullptr) {
        delete _typeSys;
    }
    if(_epSys != nullptr) {
        delete _epSys;
    }
}

chimera::ChimeraContext* chimera::ChimeraSystem::createContext(StateSynchrony* referrer)
{
    return new ChimeraContext(this, referrer);
}

chimera::LoggingSystem* chimera::ChimeraSystem::getLoggingSystem()
{
    return _loggingSys;
}

chimera::ParameterTypeSystem* chimera::ChimeraSystem::getTypeSystem()
{
    return _typeSys;
}

chimera::EntryPointSystem* chimera::ChimeraSystem::getEntryPointSystem()
{
    return _epSys;
}

chimera::Module* chimera::ChimeraSystem::includeModule(chimera::EntryPoint* entryPoint, const std::string& name)
{
    Module* modPtr = this->openModule(entryPoint, name);
    if(modPtr) {
        if(entryPoint->addModule(name, modPtr))
        {
            return modPtr;
        }
        else
        {
            closeModule(modPtr);
        }
    }
    return nullptr;
}

chimera::Module* chimera::ChimeraSystem::openModule(chimera::EntryPoint const * const entrypoint, const std::string& name)
{
    // virtual - do nothing
    return nullptr;
}

void chimera::ChimeraSystem::closeModule(Module* module)
{
    // virtual - do nothing
}

void chimera::ChimeraSystem::notifyItemAdded(StateSynchrony* sender, void* item, void const * const data)
{
    if(sender == _typeSys)
    {
        // ParameterType
        pushType((ParameterType*)item);
    }
    else
    {
        StateSynchrony* s = (StateSynchrony*)item;
        if(s->_context != nullptr)
        {
            delete s->_context;
        }
        s->_context = createContext(s);

        if(sender == _epSys)
        {
            // EntryPoint
            //std::string* name = (std::string*)data;
            EntryPoint* entrypoint = (EntryPoint*)item;
            entrypoint->addListener(this);

            if (entrypoint->isLoaded(this))
            {
                notifyItemLoaded(sender, item, data);
            }

            for(auto it = entrypoint->_modules->begin(); it != entrypoint->_modules->end(); it++)
            {
                notifyItemAdded(entrypoint, it->second, &it->first);
            }
        }
        else
        {
            // Module
            chimera::EntryPoint* entrypoint = dynamic_cast<chimera::EntryPoint*>(sender);
            std::string* name = (std::string*)data;
            chimera::Module* module = (chimera::Module*)item;
            module->init(entrypoint);
            if(module->isLoaded(entrypoint))
            {
                pushModule(entrypoint, *name, module);
            }
        }
    }
}

void chimera::ChimeraSystem::notifyItemRemoved(StateSynchrony* sender, void* item, void const * const data)
{
    if(sender == _typeSys)
    {
        // ParameterType
        popType(*((ParameterType*)item));
    }
    else
    {
        StateSynchrony* s = (StateSynchrony*)item;
        if(s->_context != nullptr)
        {
            delete s->_context;
            s->_context = nullptr;
        }

        if(sender == _epSys)
        {
            // EntryPoint
            EntryPoint* entrypoint = (EntryPoint*)item;
            entrypoint->removeListener(this);
        }
        else
        {
            // Module
            chimera::EntryPoint* entrypoint = dynamic_cast<chimera::EntryPoint*>(sender);
            //std::string* name = (std::string*)data;
            chimera::Module* module = (chimera::Module*)item;
            //popModule(entrypoint, module);
            closeModule(module);
        }
    }
}

void chimera::ChimeraSystem::notifyItemLoaded(StateSynchrony* sender, void* item, void const * const data)
{
    if(sender == _epSys)
    {
        // EntryPoint
        EntryPoint* entrypoint = (EntryPoint*)item;
        std::string name = getEntryPointSystem()->findEntryPoint((const EntryPoint*)item);
        pushEntryPoint(name, entrypoint);
    }
    else
    {
        // Module
        //chimera::EntryPoint* entrypoint = dynamic_cast<chimera::EntryPoint*>(sender);
        //const chimera::Module* module = (const chimera::Module*)item;
        chimera::EntryPoint* entrypoint = dynamic_cast<chimera::EntryPoint*>(sender);
        std::string* name = (std::string*)data;
        chimera::Module* module = (chimera::Module*)item;
        pushModule(entrypoint, *name, module);
    }
}

void chimera::ChimeraSystem::notifyItemUnloaded(StateSynchrony* sender, void* item, void const * const data)
{
    if(sender == _epSys)
    {
        // EntryPoint
        EntryPoint* entrypoint = (EntryPoint*)item;
        //entrypoint->removeListener(this);
        popEntryPoint(entrypoint);
    }
    else
    {
        // Module
        chimera::EntryPoint* entrypoint = dynamic_cast<chimera::EntryPoint*>(sender);
        chimera::Module* module = (chimera::Module*)item;
        popModule(entrypoint, module);
    }
}

const std::string chimera::ChimeraSystem::getGUID() const
{
    return "Chimerasystem";
}

void chimera::ChimeraSystem::setLogLevel(const unsigned int& loglevel)
{
    _loggingSys->setLogLevel(loglevel);
}
