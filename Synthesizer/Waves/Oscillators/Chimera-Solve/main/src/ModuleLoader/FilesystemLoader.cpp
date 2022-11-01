#include <iostream>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include "RuntimeNames.hpp"
#include "interfaces/IModulePathProvider.hpp"
#include "ModuleLoader/FilesystemLoader.hpp"

namespace fs = boost::filesystem;

FilesystemLoader::FilesystemLoader(const std::string& directory)
{
    _directory = directory;
}

FilesystemLoader::~FilesystemLoader()
{
    //dtor
}

const std::string FilesystemLoader::getFileName(const std::string& moduleName)
{
    fs::path loadDir(_directory);
    if(fs::exists(loadDir) && fs::is_directory(loadDir))
    {
        fs::path modulePath(loadDir);
        std::string modName(chimera::runtime::Naming::Module_prefix);
        modName += moduleName;
        modName += chimera::runtime::Naming::Module_extension;
        modulePath /= modName;

        if(fs::exists(modulePath) && !fs::is_directory(modulePath))
        {
            return modulePath.string();
        }
    }
    return "";
}
