#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <vector>
#include <utility>
#include <complex>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string.hpp>
#include <pwd.h>
#include <cstdlib>
#include "lua.hpp"

#include "def.hpp"
#include "Naming.hpp"
#include "RuntimeNames.hpp"
#include "StateSynchrony.hpp"
#include "interfaces/ILogger.hpp"
#include "interfaces/IModulePathProvider.hpp"
#include "LoggingSystem.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "TypeLookup.hpp"
#include "ParameterValueCollection.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ChimeraSystem.hpp"
#include "lua/lua_math.hpp"
#include "lua/types/lua_complex.hpp"
#include "lua/types/lua_vector.hpp"
#include "lua/types/lua_matrixrow.hpp"
#include "lua/types/lua_matrix.hpp"
#include "lua/types/lua_Simulation.hpp"
#include "lua/lua_global.hpp"
#include "types/lua_basetypes.hpp"
#include "ModuleLoader.hpp"
#include "ChimeraRuntime.hpp"
#include "Configuration.hpp"
#include "ModuleLoader/FilesystemLoader.hpp"

#include "StandardLogger.hpp"
#include "EntryPoints/SystemDynamicEntryPoint.hpp"
#include "EntryPoints/IntegratorEntryPoint.hpp"
#include "EntryPoints/OutputEntryPoint.hpp"


namespace fs = boost::filesystem;

const std::string version("0.1.0.0");

int main(int argc, char** argv)
{
    // --help
    // --help module
    // --help module str1 str2 ...
    // --version print version

    // -L load module, that does not belong to a EntryPoint, containing Types and EntryPoints
    // --load=path to module

    // -I include in path lookup
    // --lookup=path to lookup

    Configuration conf;

    const std::string etcPath = "/etc/chimera/solver.ini";
//#define DEBUG
#ifdef DEBUG
    fs::path modPath = fs::system_complete( fs::path( "./modules" ) );
    FilesystemLoader* dbgPath = nullptr;
    dbgPath = new FilesystemLoader(modPath.string());
    conf.addLoader("current_path", dbgPath);
#else
    conf.load(etcPath);
    passwd* pw = getpwuid(getuid());
    if(pw != nullptr)
    {
        fs::path userConf1 = fs::system_complete( fs::path( pw->pw_dir ) );
        userConf1 /= ".config/chimera/solver.ini";
        if (fs::exists(userConf1))
        {
            conf.load(userConf1.string());
        }

        fs::path userConf2 = fs::system_complete( fs::path( pw->pw_dir ) );
        userConf2 /= ".local/share/chimera/solver.ini";
        if (fs::exists(userConf2))
        {
            conf.load(userConf2.string());
        }
    }
    const char* cimeraEnv = std::getenv("CHIMERACONF");
    if (cimeraEnv != nullptr) {
        std::string envPath = std::string(cimeraEnv);
        boost::trim(envPath);
        if (envPath.size() > 0)
        {
            size_t s_start = 0;
            size_t s_end;

            do
            {
                s_end = envPath.find(':', s_start + 1);
                std::string envPathLocation = envPath.substr(s_start, s_end - s_start);
                boost::trim(envPathLocation);
                fs::path envPathItem = fs::system_complete( fs::path( envPathLocation ) );
                if (fs::exists(envPathItem))
                {
                    conf.load(envPathItem.string());
                }
                s_start = s_end + 1;
            } while (s_end != std::string::npos);
        }
    }
#endif // DEBUG


    fs::path data_dir(fs::current_path());
    FilesystemLoader* currentPath = nullptr;
    FilesystemLoader* scriptPath = nullptr;
    currentPath = new FilesystemLoader(data_dir.string());
    conf.addLoader("current_path", currentPath);

    if(argc > 1 && fs::exists(fs::path( argv[1] )))
    {
        fs::path full_path( fs::initial_path<fs::path>() );
        full_path = fs::system_complete( fs::path( argv[1] ).parent_path() );
        //full_path /= chimera::runtime::Naming::Path_modules;
        scriptPath = new FilesystemLoader(full_path.string());
    }

    //lookup.push_back(full_path.string());
    // add all paths of env NETWORK_INCLUDE_PATH

    chimera::runtime::ModuleLoader loader(*conf.getLoaders());

    //StandardLogger stdLog;
    //LoggingSystem::AddLogger(&stdLog);

    std::vector<char*> vargv;
    for(int i = 1; i < argc; i++) {
        vargv.push_back(argv[i]);
    }

    StandardLogger* stdLog = new StandardLogger();

    std::string filename(argv[1]);
    chimera::runtime::ChimeraRuntime* runtime = new chimera::runtime::ChimeraRuntime(filename, vargv, &loader);

    runtime->getLoggingSystem()->addLogger(0, stdLog);

    //if(luaL_dofile(runtime->getLuaState(), filename.c_str()))
    //{
    //    std::cout << "LUA ERROR? -.-" << std::endl;
    //}
    int status, result;
    status = luaL_loadfile(runtime->getLuaState(), filename.c_str());
    if (status) {
        /* If something went wrong, error message is at the top of */
        /* the stack */
        std::string s("Couldn't load file: ");
        s += lua_tostring(runtime->getLuaState(), -1);
        std::cout << "Lua Error: " << s << std::endl;
        return false;
    }

    result = lua_pcall(runtime->getLuaState(), 0, LUA_MULTRET, 0);

    if (result) {
        std::string s("Failed to run script: ");
        s += lua_tostring(runtime->getLuaState(), -1);
        std::cout << "Lua Error: " << s << std::endl;
        return false;
    }

    lua_settop(runtime->getLuaState(), 0);

    delete runtime;

    //LoggingSystem::ShutDown();

    return 0;
}
