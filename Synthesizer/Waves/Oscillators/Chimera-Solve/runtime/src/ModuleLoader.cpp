#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <dlfcn.h>

#include "def.hpp"
#include "RuntimeNames.hpp"
#include "StateSynchrony.hpp"
#include "interfaces/ILogger.hpp"
#include "interfaces/IModulePathProvider.hpp"
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
#include "ModuleLoader.hpp"

namespace fs = boost::filesystem;

chimera::runtime::ModuleLoader::ModuleLoader(const vec_t_pathList& importLookup)
{
    _importLookup = new vec_t_pathList(importLookup);
    _modulePtr = new map_t_ModuleItem();
    _moduleReg = new map_t_ModuleRegistry();
}

chimera::runtime::ModuleLoader::~ModuleLoader()
{
    delete _importLookup;
    unloadAll();
    delete _modulePtr;
    delete _moduleReg;
}

chimera::Module* const chimera::runtime::ModuleLoader::load(const std::string& name)
{
    void* lib_handle = nullptr;
    char* error = nullptr;

    // test if module with `name` was already loaded

    map_t_ModuleItem::const_iterator available = _modulePtr->find (name);
    if ( available == _modulePtr->end() ) // if not loaded
    {
        // check all lookup directories for module
        for (auto it = _importLookup->begin() ; it != _importLookup->end(); ++it) {
            std::string pathName = it->second->getFileName(name);
            if(pathName.empty())
            {
                continue;
            }

            fs::path modulePath(pathName);
            if(fs::exists(modulePath) && !fs::is_directory(modulePath))
            {
                // if matching module file name was found, try opening
                // the library
                //lib_handle = dlopen(modulePath.string().c_str(), RTLD_LAZY);
                lib_handle = dlopen(modulePath.string().c_str(), RTLD_LAZY|RTLD_GLOBAL);
                // library was opened successfully:
                // add to open libraries
                if (lib_handle)
                {
                    break;
                }
            }
        }
    } else {
        // library was already open, return
        return (*_moduleReg)[name];
    }

    // if library was not found, return nothing
    if(lib_handle == nullptr) {
        return nullptr;
    }

    fn_create_mod fn_load;
    fn_delete_mod fn_unload;
    // load function symbol `LoadModule` which has to return
    // a Registry pointer
    fn_load = (fn_create_mod)dlsym(lib_handle, chimera::runtime::Naming::Module_createFunction);
    fn_unload = (fn_delete_mod)dlsym(lib_handle, chimera::runtime::Naming::Module_deleteFunction);

    if ((error = dlerror()) != NULL || !fn_load || !fn_unload)
    {
        // on error remove library again and return nothing
        //_modulePtr->erase(name);
        return nullptr;
    }

    struct T_ModuleSymbols symbols = { .lib=lib_handle, .load=fn_load, .unload=fn_unload };

    //(*_modulePtr)[name] = symbols;
    _modulePtr->insert(std::make_pair(name, symbols));
    // set the registry for the loaded module
    chimera::Module* mMod = (*fn_load)();
    //mMod->load(EntryPoint const * const entryPoint);
    (*_moduleReg)[name] = mMod;
    return mMod;
}

void chimera::runtime::ModuleLoader::unload(const std::string& name)
{
    void *lib_handle;

    map_t_ModuleItem::const_iterator available = _modulePtr->find (name);

    // if module was found, remove it
    // if not, do nothing
    if ( available != _modulePtr->end() )
    {
        map_t_ModuleRegistry::const_iterator it2 = _moduleReg->find (name);
        //auto item = _modulePtr->find(name)->second;
        auto item = available->second;
        // free Registry space
        //delete ((*_moduleReg)[name]);
        //(*_moduleReg)[name]->unload();
        item.unload((*_moduleReg)[name]);
        // close library file
        //lib_handle = ((*_modulePtr)[name]).lib;
        lib_handle = item.lib;
        dlclose(lib_handle);
        // remove pointers
        _modulePtr->erase(available);
        _moduleReg->erase(it2);
    }
}

void chimera::runtime::ModuleLoader::unload(const chimera::Module* module)
{
    for(auto it = _moduleReg->begin(); it != _moduleReg->end(); it++)
    {
        if(it->second == module)
        {
            unload(it->first);
        }
    }
}

void chimera::runtime::ModuleLoader::unloadAll()
{
    auto mIt = _modulePtr->begin();
    while ( mIt != _modulePtr->end() ) {
        this->unload(mIt->first);
        mIt = _modulePtr->begin();
    }
}
