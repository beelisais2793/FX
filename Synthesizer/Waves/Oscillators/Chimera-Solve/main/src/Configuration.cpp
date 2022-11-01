#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>
#include <complex>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/foreach.hpp>
#include <string>

#include "interfaces/IModulePathProvider.hpp"
#include "ModuleLoader/FilesystemLoader.hpp"
#include "Configuration.hpp"

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

Configuration::Configuration()
{
    _loglevel = 0;
    _pathLookup = new std::vector<std::pair<std::string, chimera::runtime::IModulePathProvider*> >();
}

Configuration::~Configuration()
{
    for(auto it = _pathLookup->begin(); it != _pathLookup->end(); it++)
    {
        delete it->second;
    }
    delete _pathLookup;
}

void Configuration::load(const std::string& confPath)
{
    pt::ptree root;
    try
    {
        pt::read_ini(confPath, root);
    }
    catch(...)
    {
        return;
    }

    std::string strLogLevel = root.get<std::string>("LogLevel", "");
    if(strLogLevel == "")
    {
        // do nothing
    }
    else if(strLogLevel == "0" || strLogLevel == "debug" || strLogLevel == "Debug" || strLogLevel == "DEBUG")
    {
        _loglevel = 0;
    }
    else if(strLogLevel == "notice" || strLogLevel == "Notice" || strLogLevel == "NOTICE")
    {
        _loglevel = 100;
    }
    else if(strLogLevel == "warning" || strLogLevel == "Warning" || strLogLevel == "WARNING")
    {
        _loglevel = 200;
    }
    else if(strLogLevel == "error" || strLogLevel == "Error" || strLogLevel == "ERROR")
    {
        _loglevel = 500;
    }
    else if(strLogLevel == "critical" || strLogLevel == "Critical" || strLogLevel == "CRITICAL")
    {
        _loglevel = 1000;
    }
    else
    {
        try
        {
            _loglevel = std::stoi(strLogLevel);
        }
        catch(...)
        {}
    }

    std::vector<std::pair<std::string, chimera::runtime::IModulePathProvider*> >* lastLoaders = nullptr;
    if(_pathLookup->size() > 0)
    {
        lastLoaders = _pathLookup;
        _pathLookup = new std::vector<std::pair<std::string, chimera::runtime::IModulePathProvider*> >();
    }
    for (pt::ptree::value_type &row : root)
    {
        if(!row.second.empty())
        {
            std::string type = row.second.get<std::string>("type", "");
            if(type == "filesystem")
            {
                std::string path = row.second.get<std::string>("path", "");
                fs::path full_path = fs::system_complete( fs::path( path ) );
                if(fs::exists(full_path) && fs::is_directory(full_path))
                {
                    addLoader(row.first, new FilesystemLoader(full_path.string()));
                }
            }
        }
    }
    if(lastLoaders != nullptr)
    {
        for (auto loader = lastLoaders->begin(); loader != lastLoaders->end(); loader++)
        {
            _pathLookup->push_back(*loader);
        }
        delete lastLoaders;
    }
}

void Configuration::addLoader(std::string name, chimera::runtime::IModulePathProvider* item)
{
    _pathLookup->push_back(std::make_pair(name, item));
}

std::vector<std::pair<std::string, chimera::runtime::IModulePathProvider*> >* Configuration::getLoaders()
{
    return _pathLookup;
}

void Configuration::setLogLevel(const unsigned int& loglevel)
{
    _loglevel = loglevel;
}

unsigned int Configuration::getLogLevel()
{
    return _loglevel;
}
