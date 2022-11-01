#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <boost/numeric/ublas/vector.hpp>
#include "lua.hpp"

#include "def.hpp"
#include "Naming.hpp"
#include "extendTypes.hpp"
#include "ExtensionNaming.hpp"
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
#include "interfaces/IEventListener.hpp"
#include "interfaces/IEventListenerProvider.hpp"
#include "event/Observer.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/StateEventListener.hpp"
#include "event/NotificationManager.hpp"
//#include "simulation/TemporalStateEventProvider.hpp"
#include "EntryPointBase/OutputMultiModule.hpp"
//#include "types/LuaFunctionWrapper.hpp"
//#include "EntryPointBase/OutputModule.hpp"



#include "file_csv.hpp"

namespace ublas = boost::numeric::ublas;

extern "C"
{
    chimera::Module* LoadModule()
    {
        return new CsvFileModule();
    }

    void UnloadModule(chimera::Module* reg)
    {
        delete (CsvFileModule*)reg;
    }
}


int file_open(lua_State* L)
{
    //CsvFileWriter* w = *((CsvFileWriter**)lua_touserdata(L, lua_upvalueindex(1)));
    //w->open();

    return 0;
}

int file_close(lua_State* L)
{
    //CsvFileWriter* w = *((CsvFileWriter**)lua_touserdata(L, lua_upvalueindex(1)));
    //w->close();

    return 0;
}

int file_path(lua_State* L)
{
    //CsvFileWriter* w = *((CsvFileWriter**)lua_touserdata(L, lua_upvalueindex(1)));
    //lua_pushstring(L, w->getPath().c_str());
    return 1;
}

CsvFileModule::CsvFileModule()
{
    //ctor
    //registerMethod("open", file_open);
    //registerMethod("close", file_close);
    //registerMethod("path", file_path);
}

CsvFileModule::~CsvFileModule()
{
    //dtor
}

void CsvFileModule::destroyInstance(chimera::EntryPoint const * const entrypoint, void * const instance) const
{
    //delete (CsvFileWriter*)instance;
    //delete (TemplateIntegrator
    //        <double, double>
    //        *)instance;
}

chimera::simulation::IEventListenerProvider* CsvFileModule::getOutputInstance(chimera::vec_t_LuaItem& parameters) const
{
    if(parameters.size() > 0)
    {
        if(parameters.size() == 1 && parameters[0].getType() == chimera::systemtypes::PID_TABLE)
        {
            chimera::map_t_LuaItem* paramMap = (chimera::map_t_LuaItem*)parameters[0].getValue();
            auto pathItem = paramMap->find("path");
            if(pathItem != paramMap->end())
            {
                std::string* path = (std::string*)pathItem->second.getValue();
                return new CsvFileWriter(getContext(), *path);
            }
        }
        else if(parameters[0].getType() == chimera::systemtypes::PID_STRING)
        {
            std::string* path = (std::string*)parameters[0].getValue();
            return new CsvFileWriter(getContext(), *path);
        }
    }
    return nullptr;
}

const std::string CsvFileModule::getGUID() const
{
    return "file:CSV";
}

CsvFileWriter::CsvFileWriter(chimera::ChimeraContext* context, const std::string& path):
    _context(context),
    _path(path)
{
    _file = nullptr;
}

CsvFileWriter::~CsvFileWriter()
{
    if(_file && _file->is_open())
        _file->close();
}

void CsvFileWriter::open()
{
    if(_file == nullptr)
        _file = new std::ofstream(_path);
}

void CsvFileWriter::close()
{
    if(_file && _file->is_open())
        _file->close();
    if(_file)
        delete _file;
    _file = nullptr;
}

const std::string CsvFileWriter::getPath() const
{
    return _path;
}

chimera::simulation::IEventListener* CsvFileWriter::provideListener(size_t id, void const * const args)
{
    static const size_t pid_vecReal = _context->getParameterID(std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER));
    static const size_t pid_vecvecReal = _context->getParameterID(std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER));
    static const size_t pid_vecComplex = _context->getParameterID(std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::simulation::Naming::Type_Complex));
    static const size_t pid_vecvecComplex = _context->getParameterID(std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::simulation::Naming::Type_Complex));

    switch(id){
    case 1: // StateEventListener
        {
            struct T_StateProviderArgs* spArgs = (struct T_StateProviderArgs*)args;
            //std::cout << "Time:  " << spArgs->time_type << " :: " << _ps->getParameterName(spArgs->time_type) << std::endl;
            //std::cout << "State: " << spArgs->state_type << " :: " << _ps->getParameterName(spArgs->state_type) << std::endl;

            if(spArgs->time_type == chimera::systemtypes::PID_NUMBER)
            {
                if(spArgs->state_type == pid_vecReal)
                {
                    open();
                    return new FileOutput_real_vecReal(_file);
                }
                if(spArgs->state_type == pid_vecvecReal)
                {
                    open();
                    return new FileOutput_real_vecvecReal(_file);
                }
                if(spArgs->state_type == pid_vecComplex)
                {
                    open();
                    return new FileOutput_real_vecComplex(_file);
                }
                if(spArgs->state_type == pid_vecvecComplex)
                {
                    open();
                    return new FileOutput_real_vecvecComplex(_file);
                }
            }
        }
    }

    return nullptr;
}

FileOutput_real_vecReal::FileOutput_real_vecReal(std::ofstream* file)
{
    _file = file;
}

FileOutput_real_vecReal::~FileOutput_real_vecReal()
{

}

//void FileOutput_real_vecReal::notify(const double& time, const boost::numeric::ublas::vector<double>& state)
void FileOutput_real_vecReal::notify(chimera::simulation::NotificationManager const * const sender, void const * const args)
{
    T_TimeStateArgs const * const c_args = (T_TimeStateArgs const * const)args;
    (*_file) << *((double*)c_args->time);
    chimera::simulation::T_VectorDef* state = (chimera::simulation::T_VectorDef*)c_args->state;
    for(auto item : *((boost::numeric::ublas::vector<double>*)state->value))
    {
        (*_file) << "," << item;
    }
    (*_file) << std::endl;
}

FileOutput_real_vecvecReal::FileOutput_real_vecvecReal(std::ofstream* file)
{
    _file = file;
}

FileOutput_real_vecvecReal::~FileOutput_real_vecvecReal()
{

}

//void FileOutput_real_vecvecReal::notify(const double& time, const boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >& state)
void FileOutput_real_vecvecReal::notify(chimera::simulation::NotificationManager const * const sender, void const * const args)
{
    T_TimeStateArgs* c_args = (T_TimeStateArgs*)args;
    (*_file) << *((double*)c_args->time);
    chimera::simulation::T_VectorDef* state = (chimera::simulation::T_VectorDef*)c_args->state;
    for(auto node : *((boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >*)state->value))
    {
        for(auto item : node)
        {
            (*_file) << "," << item;
        }
    }
    (*_file) << std::endl;
}

FileOutput_real_vecComplex::FileOutput_real_vecComplex(std::ofstream* file)
{
    _file = file;
}

FileOutput_real_vecComplex::~FileOutput_real_vecComplex()
{

}

//void FileOutput_real_vecComplex::notify(const double& time, const boost::numeric::ublas::vector<std::complex<double> >& state)
void FileOutput_real_vecComplex::notify(chimera::simulation::NotificationManager const * const sender, void const * const args)
{
    T_TimeStateArgs* c_args = (T_TimeStateArgs*)args;
    static double i;
    (*_file) << *((double*)c_args->time);
    chimera::simulation::T_VectorDef* state = (chimera::simulation::T_VectorDef*)c_args->state;
    for(auto item : *((boost::numeric::ublas::vector<std::complex<double> >*)state->value))
    {
        i = std::imag(item);
        (*_file) << "," << std::real(item);
        if (i >= 0)
        {
            (*_file) << "+";
        }
        (*_file) << i << "i";
    }
    (*_file) << std::endl;
}

FileOutput_real_vecvecComplex::FileOutput_real_vecvecComplex(std::ofstream* file)
{
    _file = file;
}

FileOutput_real_vecvecComplex::~FileOutput_real_vecvecComplex()
{

}

//void FileOutput_real_vecvecComplex::notify(const double& time, const boost::numeric::ublas::vector<boost::numeric::ublas::vector<std::complex<double> > >& state)
void FileOutput_real_vecvecComplex::notify(chimera::simulation::NotificationManager const * const sender, void const * const args)
{
    T_TimeStateArgs* c_args = (T_TimeStateArgs*)args;
    static double i;
    (*_file) << *((double*)c_args->time);
    chimera::simulation::T_VectorDef* state = (chimera::simulation::T_VectorDef*)c_args->state;
    for(auto node : *((boost::numeric::ublas::vector<boost::numeric::ublas::vector<std::complex<double> > >*)state->value))
    {
        for(auto item : node)
        {
            i = std::imag(item);
            (*_file) << "," << std::real(item);
            if (i >= 0)
            {
                (*_file) << "+";
            }
            (*_file) << i << "i";
        }
    }
    (*_file) << std::endl;
}
