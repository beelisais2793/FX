#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/tokenizer.hpp>

#include "def.hpp"
#include "Naming.hpp"
#include "ExtensionNaming.hpp"
#include "extendTypes.hpp"
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
#include "CsvTools.hpp"

namespace ublas = boost::numeric::ublas;
namespace fs = boost::filesystem;

extern "C"
{
    chimera::Module* LoadModule()
    {
        return new CsvToolsModule();
    }

    void UnloadModule(chimera::Module* module)
    {
        delete (CsvToolsModule*)module;
    }
}

CsvToolsModule::CsvToolsModule()
{
    //setValue("keks", {ParameterTypeSystem::getParameterID(Naming::Type_real), (void*)new double(17.349)});
    //registerMethod("test", rk_test);
    registerMethod("load", fn_CsvTools_load);
    registerMethod("save", fn_CsvTools_save);
    registerMethod("read", fn_CsvTools_read);
    registerMethod("write", fn_CsvTools_write);
}

void* CsvToolsModule::getInstance(chimera::EntryPoint const * const entrypoint, chimera::vec_t_LuaItem& parameters) const
{
    bool inFile = false;
    std::string file;

    if(parameters.size() == 1 && parameters[0].getType() == chimera::systemtypes::PID_TABLE)
    {
        chimera::map_t_LuaItem* paramMap = (chimera::map_t_LuaItem*)parameters[0].getValue();
        for(auto p : *paramMap)
        {
            if(p.first == "file" && p.second.getType() == chimera::systemtypes::PID_STRING)
            {
                inFile = true;
                file = (std::string)p.second;
            }
        }
    }
    else
    {
        if(parameters.size() > 0 && chimera::systemtypes::PID_STRING == parameters[0].getType())
        {
            inFile = true;
            file = (std::string)parameters[0];
        }
    }

    if(!inFile)
    {
        return nullptr;
    }

    return new CsvTools(file, ",");
}

const std::string CsvToolsModule::getGUID() const
{
    return "CsvTools";
}

void CsvToolsModule::destroyInstance(chimera::EntryPoint const * const entrypoint, void* instance) const
{
    delete ((CsvTools*)instance);
}

CsvTools::CsvTools(std::string filename, std::string delimiter)
{
    _filename = filename;
    _delimiter = delimiter;
}

CsvTools::~CsvTools()
{

}

std::string CsvTools::getFilename() const
{
    return _filename;
}

chimera::vec_t_LuaItem fn_CsvTools_load(chimera::EntryPoint const * const entrypoint, chimera::Module const * const module, void* instance, const chimera::vec_t_LuaItem& params)
{
    const int INIT_ROWS = 10;
    const int MAX_GROW = 1000;

    CsvTools* ct = (CsvTools*)instance;
    chimera::vec_t_LuaItem result;

    if(fs::exists(ct->getFilename())) {
        std::ifstream csvfile(ct->getFilename());
        std::string line;
        char esc = '\\';
        char quote = '\"';
        char sep = ',';
        double number;
        int lineNum = -1, num;
        int lineCount = 0;

        std::vector<double> first;
        boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >* data = new boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >(INIT_ROWS);

        while(std::getline(csvfile, line))
        {
            boost::escaped_list_separator<char> els(esc, sep, quote);
            boost::tokenizer<boost::escaped_list_separator<char> > tok(line, els);
            num = 0;

            for(boost::tokenizer<boost::escaped_list_separator<char> >::iterator beg=tok.begin(); beg!=tok.end();++beg){
                try
                {
                    number = std::stod(*beg);
//                    std::cout << number << std::endl;
                    if(lineNum == -1)
                    {
                        first.push_back(number);
                    }
                    else
                    {
                        if(num == 0)
                        {
                            if(lineCount >= data->size())
                            {
                                if(data->size() <= MAX_GROW)
                                {
                                    data->resize(data->size() * 2, true);
                                }
                                else
                                {
                                    data->resize(data->size() + MAX_GROW, true);
                                }
                            }

                            (*data)[lineCount].resize(lineNum, false);
                        }
                        if(num >= lineNum)
                        {
                            delete data;
                            return chimera::vec_t_LuaItem();
                        }
                        (*data)[lineCount][num] = number;
                    }
                }
                catch(std::invalid_argument)
                {
                    delete data;
                    return chimera::vec_t_LuaItem();
                }
                num++;
            }
            lineCount++;

            if(lineNum == -1)
            {
                lineNum = num;
                (*data)[0].resize(lineNum, false);
                for (int i = 0; i < lineNum; i++)
                {
                    (*data)[0][i] = first[i];
                }
            }
            else
            {
                if(lineNum != num)
                {
                    delete data;
                    return chimera::vec_t_LuaItem();
                }
            }
        }

        data->resize(lineCount, true);
        struct chimera::simulation::T_VectorDef* vd = new struct chimera::simulation::T_VectorDef({data->size(), false, false, data});
        const std::string vectorVectorRealMetaName = std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER);
        size_t type = module->getContext()->getParameterID(vectorVectorRealMetaName);

        chimera::ParameterValue p = module->getContext()->createValue(type, vd);
        result.push_back(p);
        //std::cout << "Load file " << ct->getFilename() << "\n" << std::endl;
    }
    return result;
}

chimera::vec_t_LuaItem fn_CsvTools_save(chimera::EntryPoint const * const entrypoint, chimera::Module const * const, void* instance, const chimera::vec_t_LuaItem& params)
{
    CsvTools* ct = (CsvTools*)instance;
    std::cout << "Load file " << ct->getFilename() << "\n" << std::endl;
    return chimera::vec_t_LuaItem();
}

chimera::vec_t_LuaItem fn_CsvTools_read(chimera::EntryPoint const * const entrypoint, chimera::Module const * const, void* instance, const chimera::vec_t_LuaItem& params)
{
    CsvTools* ct = (CsvTools*)instance;
    std::cout << "Load file " << ct->getFilename() << "\n" << std::endl;
    return chimera::vec_t_LuaItem();
}

chimera::vec_t_LuaItem fn_CsvTools_write(chimera::EntryPoint const * const entrypoint, chimera::Module const * const, void* instance, const chimera::vec_t_LuaItem& params)
{
    CsvTools* ct = (CsvTools*)instance;
    std::cout << "Load file " << ct->getFilename() << "\n" << std::endl;
    return chimera::vec_t_LuaItem();
}
