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
#include "ChimeraContext.hpp"
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
#include "RotationalCoupling2D.hpp"

extern "C"
{
    chimera::Module* LoadModule()
    {
        return new RotationalCoupling2DModule();
    }

    void UnloadModule(chimera::Module* reg)
    {
        delete (RotationalCoupling2DModule*)reg;
    }
}

RotationalCoupling2DModule::RotationalCoupling2DModule()
{
    //ctor
}

RotationalCoupling2DModule::~RotationalCoupling2DModule()
{
    //dtor
}

const std::string RotationalCoupling2DModule::getGUID() const
{
    return "Nearest-Neighbour coupling";
}

AbstractCoupling* RotationalCoupling2DModule::getCoupling(chimera::vec_t_LuaItem& parameters) const
{
    double epsilon;
    double sigma;
    double phi;
    struct chimera::simulation::T_VectorDef* vd;

    if(parameters.size() == 1 && parameters[0].getType() == chimera::systemtypes::PID_TABLE)
    {
        bool inSigma = false;
        bool inEpsilon = false;
        bool inPhi = false;
        bool inMatrix = false;

        const std::string vectorVectorRealMetaName = std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER);
        size_t type = getContext()->getParameterID(vectorVectorRealMetaName);

        chimera::map_t_LuaItem* paramMap = (chimera::map_t_LuaItem*)parameters[0].getValue();
        for(auto p : *paramMap)
        {
            if(p.first == "sigma" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                inSigma = true;
                sigma = p.second;
            }
            if(p.first == "epsilon" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                inEpsilon = true;
                epsilon = p.second;
            }
            if(p.first == "phi" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                inPhi = true;
                phi = p.second;
            }
            if(p.first == "G" && p.second.getType() == type)
            {
                inMatrix = true;
                vd = (struct chimera::simulation::T_VectorDef*)p.second.getValue();
            }
        }
        if(!(inSigma && inEpsilon && inPhi && inMatrix))
        {
            return nullptr;
        }
        auto result = new RotationalCoupling2D(epsilon, sigma, phi, (boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >*)vd->value);
        getContext()->addDependency(result, vd);
        return result;
    }
    /*
    else if(parameters.size() >= 2 && parameters[0].getType() == chimera::systemtypes::PID_NUMBER && parameters[1].getType() == chimera::systemtypes::PID_NUMBER)
    {
        sigma = parameters[0];
        count = parameters[1];
        return new RotationalCoupling2D(sigma, count);
    }*/

    return nullptr;
}

RotationalCoupling2D::RotationalCoupling2D(double epsilon, double sigma, double phi, boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >* coupling):
    _epsilon(epsilon),
    _sigma(sigma),
    _phi(phi)
{
    _sinPhi = sin(phi);
    _cosPhi = cos(phi);
    //struct chimera::simulation::T_VectorDef* vd = (struct chimera::simulation::T_VectorDef*)couplingValue.getValue();
    //_couplingMatrix = (boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >*)vd->value;
    _couplingMatrix = coupling;
}

RotationalCoupling2D::~RotationalCoupling2D()
{

}

void RotationalCoupling2D::operator()(const INetwork<double, boost::numeric::ublas::vector<double> >* network, const boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >& system, const double& t, const int& i, boost::numeric::ublas::vector<double>& out)
{
    static int N;
    N = system.size();

    //int k = out.size();
    out[0] = 0;
    out[1] = 0;
    for(int j = 0; j < N; j++)
    {
        out[0] += (*_couplingMatrix)[i][j] * (_cosPhi * (system[j][0] - system[i][0]) + _sinPhi * (system[j][1] - system[i][1]));
        out[1] += (*_couplingMatrix)[i][j] * (_cosPhi * (system[j][1] - system[i][1]) - _sinPhi * (system[j][0] - system[i][0]));
    }
    out[0] = _sigma / _epsilon * out[0];
    out[1] = _sigma * out[1];
}
