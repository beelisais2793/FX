#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>
#include <stdlib.h>
#include <boost/numeric/ublas/vector.hpp>

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
#include "interfaces/IEventListener.hpp"
#include "interfaces/IEventListenerProvider.hpp"
//#include "event/Observer.hpp"
//#include "event/StateEventListener.hpp"
//#include "NotificationManager.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/TemplateOdeSystem.hpp"
#include "Network/INetwork.hpp"
#include "Network/AbstractCoupling.hpp"
#include "Network/TemplateCoupling.hpp"
#include "Network/CouplingModule.hpp"
#include "NearestNeighbourCoupling.hpp"

extern "C"
{
    chimera::Module* LoadModule()
    {
        return new NearestNeighbourCouplingModule();
    }

    void UnloadModule(chimera::Module* reg)
    {
        delete (NearestNeighbourCouplingModule*)reg;
    }
}

NearestNeighbourCouplingModule::NearestNeighbourCouplingModule()
{
    //ctor
}

NearestNeighbourCouplingModule::~NearestNeighbourCouplingModule()
{
    //dtor
}

const std::string NearestNeighbourCouplingModule::getGUID() const
{
    return "Nearest-Neighbour coupling";
}

AbstractCoupling* NearestNeighbourCouplingModule::getCoupling(chimera::vec_t_LuaItem& parameters) const
{
    double sigma;
    int count;

    if(parameters.size() == 1 && parameters[0].getType() == chimera::systemtypes::PID_TABLE)
    {
        bool inSigma = false;
        bool inR = false;
        chimera::map_t_LuaItem* paramMap = (chimera::map_t_LuaItem*)parameters[0].getValue();
        for(auto p : *paramMap)
        {
            if(p.first == "sigma" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                inSigma = true;
                sigma = p.second;
            }
            if(p.first == "R" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                inR = true;
                count = p.second;
            }
        }
        if(!(inSigma && inR))
        {
            return nullptr;
        }
        return new NearestNeighbourCoupling(sigma, count);
    }
    else if(parameters.size() >= 2 && parameters[0].getType() == chimera::systemtypes::PID_NUMBER && parameters[1].getType() == chimera::systemtypes::PID_NUMBER)
    {
        sigma = parameters[0];
        count = parameters[1];
        return new NearestNeighbourCoupling(sigma, count);
    }

    return nullptr;
}

NearestNeighbourCoupling::NearestNeighbourCoupling(double sigma, int count):
    _sigma(sigma),
    _count(count)
{

}

NearestNeighbourCoupling::~NearestNeighbourCoupling()
{

}

void NearestNeighbourCoupling::operator()(const INetwork<double, boost::numeric::ublas::vector<double> >* network, const boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >& system, const double& t, const int& i, boost::numeric::ublas::vector<double>& out)
{
    int j, k;
    int N = system.size();
    int K = out.size();
    for(j = -_count; j <= _count; j++) {
        for(k = 0; k < K; k++)
        {
            out[k] += _sigma * (system[(i + j + N) % N][k] - system[i][k]);
        }
    }
    /*
    if(abs(i - j) <= _count && i != j)
    {
        for(unsigned int n = 0; n < out.size(); n++)
        {
            out[n] = _sigma * (system[j][n] - system[i][n]);
        }
    } else {
        for(unsigned int n = 0; n < out.size(); n++)
        {
            out[n] = 0;
        }
    }*/
}
