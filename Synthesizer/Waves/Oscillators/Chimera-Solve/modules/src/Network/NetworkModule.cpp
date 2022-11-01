#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>
#include <boost/numeric/ublas/vector.hpp>
#include <complex>

#include "def.hpp"
#include "Naming.hpp"
#include "ExtensionNaming.hpp"
#include "StateSynchrony.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "extendTypes.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "ParameterValueCollection.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ChimeraSystem.hpp"
#include "ChimeraContext.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/TemplateOdeSystem.hpp"
#include "Network/INetwork.hpp"
#include "Network/AbstractCoupling.hpp"
#include "Network/TemplateCoupling.hpp"
#include "Network/CouplingEntryPoint.hpp"
#include "EntryPointBase/AbstractRandom.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/TemplateOdeSystem.hpp"
#include "EntryPointBase/SystemDynamicModule.hpp"
#include "Network/INetwork.hpp"
#include "Network/Network_double_vecDouble.hpp"
#include "Network/Network_double_vecComplex.hpp"
#include "Network/MultiNetwork_double_vecDouble.hpp"
#include "Network/MultiNetwork_double_vecComplex.hpp"
#include "Network/NetworkModule.hpp"


extern "C"
{
    chimera::Module* LoadModule()
    {
        return new NetworkModule();
    }

    void UnloadModule(chimera::Module* reg)
    {
        delete (NetworkModule*)reg;
    }
}

NetworkModule::NetworkModule()
{
    _ep = nullptr;
}

NetworkModule::~NetworkModule()
{
    if(_ep != nullptr)
    {
        delete _ep;
    }
}

const std::string NetworkModule::getGUID() const
{
    return "Network";
}

chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >* NetworkModule::getSystem1(const chimera::ParameterValue& param) const
{
    static const size_t basetype = chimera::systemtypes::PID_INSTANCE;
    size_t tagtype;
    size_t type;

    type = param.getType();
    tagtype = (size_t)_callingEP;

    if(getContext()->getParameterBase(type) == basetype && getContext()->getParameterTag(type) == tagtype)
    {
        chimera::simulation::AbstractSystemDynamic* sd = (chimera::simulation::AbstractSystemDynamic*)param.getValue();
        return dynamic_cast<chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >*>(sd);
    }
    return nullptr;
}

TemplateCoupling<double, boost::numeric::ublas::vector<double> >* NetworkModule::getCoupling1(const chimera::ParameterValue& param) const
{
    static const size_t basetype = chimera::systemtypes::PID_INSTANCE;
    size_t tagtype;
    size_t type;

    type = param.getType();
    tagtype = (size_t)_ep;

    if(getContext()->getParameterBase(type) == basetype && getContext()->getParameterTag(type) == tagtype)
    {
        AbstractCoupling* c = (AbstractCoupling*)param.getValue();
        return dynamic_cast<TemplateCoupling<double, boost::numeric::ublas::vector<double> >*>(c);
    }
    return nullptr;
}

chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<std::complex<double> > >* NetworkModule::getSystem2(const chimera::ParameterValue& param) const
{
    static const size_t basetype = chimera::systemtypes::PID_INSTANCE;
    size_t tagtype;
    size_t type;

    type = param.getType();
    tagtype = (size_t)_callingEP;

    if(getContext()->getParameterBase(type) == basetype && getContext()->getParameterTag(type) == tagtype)
    {
        chimera::simulation::AbstractSystemDynamic* sd = (chimera::simulation::AbstractSystemDynamic*)param.getValue();
        return dynamic_cast<chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<std::complex<double> > >*>(sd);
    }
    return nullptr;
}

TemplateCoupling<double, boost::numeric::ublas::vector<std::complex<double> > >* NetworkModule::getCoupling2(const chimera::ParameterValue& param) const
{
    static const size_t basetype = chimera::systemtypes::PID_INSTANCE;
    size_t tagtype;
    size_t type;

    type = param.getType();
    tagtype = (size_t)_ep;

    if(getContext()->getParameterBase(type) == basetype && getContext()->getParameterTag(type) == tagtype)
    {
        AbstractCoupling* c = (AbstractCoupling*)param.getValue();
        return dynamic_cast<TemplateCoupling<double, boost::numeric::ublas::vector<std::complex<double> > >*>(c);
    }
    return nullptr;
}

void* NetworkModule::getInstance(chimera::EntryPoint const * const entrypoint, chimera::vec_t_LuaItem& parameters) const
{
    chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >* odeSys1;
    chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<std::complex<double> > >* odeSys2;
    int number;
    int switchNumber = 0;
    int sysType = 0, coupType = 0;

    TemplateCoupling<double, boost::numeric::ublas::vector<double> >* coupling1;
    TemplateCoupling<double, boost::numeric::ublas::vector<std::complex<double> > >* coupling2;

    if(parameters.size() == 1 && parameters[0].getType() == chimera::systemtypes::PID_TABLE)
    {
        bool inSystem = false, inNumber = false, inCoupling = false, inInit = false;
        chimera::map_t_LuaItem* initMap;
        chimera::simulation::SystemDynamicModule* sysMod;

        chimera::map_t_LuaItem* paramMap = (chimera::map_t_LuaItem*)parameters[0].getValue();
        for(auto p : *paramMap)
        {
            if(p.first == "oscillator")
            {
                odeSys1 = getSystem1(p.second);
                if(odeSys1 != nullptr) {
                    inSystem = true;
                    sysType = 1;
                } else {
                    odeSys2 = getSystem2(p.second);
                    if(odeSys2 != nullptr) {
                        inSystem = true;
                        sysType = 2;
                    }
                }
                if(inSystem)
                {
                    switchNumber = 0;
                    continue;
                }
                else
                {
                    size_t randomEntryPoint = (size_t)getContext()->getEntryPoint(chimera::simulation::Naming::EntryPoint_random);
                    if(randomEntryPoint != 0)
                    {
                        if(p.second.getType() == chimera::systemtypes::PID_MODULE)
                        {
                            chimera::Module* module = (chimera::Module*)p.second.getValue();
                            sysMod = dynamic_cast<chimera::simulation::SystemDynamicModule*>(module);
                            if(sysMod != nullptr)
                            {
                                inSystem = true;
                                switchNumber = 1;
                                continue;
                            }
                        }
                    }
                    break;
                }
            }
            if(p.first == "N" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                number = p.second;
                inNumber = true;
            }
            if(p.first == "coupling")
            {
                coupling1 = getCoupling1(p.second);
                if(coupling1 != nullptr) {
                    inCoupling = true;
                    coupType = 1;
                } else {
                    coupling2 = getCoupling2(p.second);
                    if(coupling2 != nullptr) {
                        inCoupling = true;
                        coupType = 2;
                    }
                }
                if(inCoupling)
                {
                    continue;
                }
            }
            if(p.first == "init")
            {
                if(p.second.getType() == chimera::systemtypes::PID_TABLE)
                {
                    initMap = (chimera::map_t_LuaItem*)p.second.getValue();
                    inInit = true;
                }
                else
                {
                    break;
                }
            }
        }

        if(!(inSystem && inNumber && inCoupling))
        {
            return nullptr;
        }

        switch(switchNumber)
        {
            case 0:
            {
                if (sysType == coupType) {
                    switch(sysType)
                    {
                        case 1:
                            return new Network_double_vecDouble(getContext(), odeSys1, number, coupling1);
                        case 2:
                            return new Network_double_vecComplex(getContext(), odeSys2, number, coupling2);
                    }
                }
            }
            case 1:
            {
                size_t randomEntryPoint = (size_t)getContext()->getEntryPoint(chimera::simulation::Naming::EntryPoint_random);

                std::vector<chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >*> oscillators1;
                std::vector<chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<std::complex<double> > >*> oscillators2;
                chimera::simulation::AbstractSystemDynamic* oscItem;
                chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >* odeItem1;
                chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<std::complex<double> > >* odeItem2;
                int oscType = 0;

                for(int i = 0; i < number; i++)
                {
                    chimera::vec_t_LuaItem initParams;
                    if(inInit)
                    {
                        chimera::map_t_LuaItem* paramValue1 = new chimera::map_t_LuaItem();
                        chimera::ParameterValue p1 = getContext()->createValue(chimera::systemtypes::PID_TABLE, paramValue1);
                        initParams.push_back(p1);
                        for(auto p : *initMap)
                        {
                            if(getContext()->getParameterBase(p.second.getType()) == chimera::systemtypes::PID_INSTANCE && getContext()->getParameterTag(p.second.getType()) == randomEntryPoint && randomEntryPoint != 0)
                            {
                                chimera::simulation::AbstractRandom* rnd = (chimera::simulation::AbstractRandom*)p.second.getValue();
                                chimera::ParameterValue next = getContext()->createValue(rnd->getType(), rnd->getNext());
                                paramValue1->insert(
                                    std::make_pair(p.first, next)
                                );
                            }
                            else
                            {
                                paramValue1->insert(
                                    std::make_pair(p.first, p.second)
                                );
                            }
                        }
                    }
                    oscItem = (chimera::simulation::AbstractSystemDynamic*)sysMod->getInstance(entrypoint, initParams);


                    if (i == 0) {
                        odeItem1 = dynamic_cast<chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >*>(oscItem);
                        if (odeItem1) {
                            oscType = 1;
                            oscillators1.push_back(odeItem1);
                        } else {
                            odeItem2 = dynamic_cast<chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<std::complex<double> > >*>(oscItem);
                            if (odeItem2) {
                                oscType = 2;
                                oscillators2.push_back(odeItem2);
                            } else {
                                return nullptr;
                            }
                        }
                    } else {
                        switch (oscType)
                        {
                            case 1:
                                odeItem1 = dynamic_cast<chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >*>(oscItem);
                                if(odeItem1 != nullptr)
                                {
                                    oscillators1.push_back(odeItem1);
                                }
                                else
                                {
                                    delete odeItem1;
                                    for(auto osc : oscillators1)
                                    {
                                        delete osc;
                                    }
                                    return nullptr;
                                }
                                break;
                            case 2:
                                odeItem2 = dynamic_cast<chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<std::complex<double> > >*>(oscItem);
                                if(odeItem2 != nullptr)
                                {
                                    oscillators2.push_back(odeItem2);
                                }
                                else
                                {
                                    delete odeItem2;
                                    for(auto osc : oscillators2)
                                    {
                                        delete osc;
                                    }
                                    return nullptr;
                                }
                                break;
                        }
                    }
                }

                switch (oscType)
                {
                    case 1:
                        return new MultiNetwork_double_vecDouble(getContext(), sysMod, oscillators1, coupling1);
                    case 2:
                        return new MultiNetwork_double_vecComplex(getContext(), sysMod, oscillators2, coupling2);
                    default:
                        return nullptr;
                }
            }
        }
    } else {
        if(parameters.size() < 3)
        {
            return nullptr;
        }

        odeSys1 = getSystem1(parameters[0]);
        if(odeSys1 == nullptr)
        {
            return nullptr;
        }

        if(parameters[1].getType() != chimera::systemtypes::PID_NUMBER)
        {
            return nullptr;
        }
        number = parameters[1];

        coupling1 = getCoupling1(parameters[2]);
        if(coupling1 == nullptr)
        {
            return nullptr;
        }

        return new Network_double_vecDouble(getContext(), odeSys1, number, coupling1);
    }

    return nullptr;
}

void NetworkModule::destroyInstance(chimera::EntryPoint const * const entrypoint, void * const instance) const
{
    delete ((chimera::simulation::AbstractSystemDynamic*)instance);
}

void NetworkModule::init(chimera::EntryPoint const * const entryPoint)
{
    if(_ep == nullptr) {
        _ep = new CouplingEntryPoint();
    }
}

bool NetworkModule::load(chimera::EntryPoint const * const entryPoint, void const * const params)
{
    _callingEP = entryPoint;
    getContext()->addEntryPoint("coupling", _ep);
    return true;
}

void NetworkModule::unload(chimera::EntryPoint const * const entryPoint)
{
    getContext()->removeEntryPoint("coupling");
    //delete _ep;
    _ep = nullptr;
}
