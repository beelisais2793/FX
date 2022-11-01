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
#include "EntryPointBase/SystemDynamicModule.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/TemplateOdeSystem.hpp"

#include "Network/INetwork.hpp"
#include "Network/AbstractCoupling.hpp"
#include "Network/TemplateCoupling.hpp"
#include "Network/CouplingEntryPoint.hpp"
#include "Network/Network_double_vecComplex.hpp"

Network_double_vecComplex::Network_double_vecComplex(chimera::ChimeraContext* context, chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<std::complex<double> > >* unit, const int& number, TemplateCoupling<double, boost::numeric::ublas::vector<std::complex<double> > >* coupling):
    _context(context)
{
    _unitSys = unit;
    _number = number;
    _coupling = coupling;
    _context->addDependency(this, unit);
    _context->addDependency(this, coupling);
    auto unitFeatures = _unitSys->getFeatures();
    auto unitSize = unitFeatures.find(chimera::simulation::Naming::Feature_size);
    _tmp = nullptr;
    if(unitSize != unitFeatures.end())
    {
        _tmp = new boost::numeric::ublas::vector<std::complex<double> >(unitSize->second);
    }

}

Network_double_vecComplex::~Network_double_vecComplex()
{
    _context->removeDependencyItem(this);
    if(_tmp) delete _tmp;
}

void Network_double_vecComplex::operator()(const boost::numeric::ublas::vector<boost::numeric::ublas::vector<std::complex<double> > >& x, boost::numeric::ublas::vector<boost::numeric::ublas::vector<std::complex<double> > >& dxdt, const double& t)
{
    //boost::numeric::ublas::vector<double> tmp(x[0].size());
    unsigned int vSize = dxdt.size();
    //std::cout << " >>step: " << t << std::endl;
    for(unsigned int i = 0; i < vSize; i++)
    {
        dxdt[i].resize(x[i].size());
        (*_unitSys)(x[i], dxdt[i], t);

        (*_coupling)(this, x, t, i, *_tmp);
        for(unsigned int inner = 0; inner < x[i].size(); inner++)
        {
            dxdt[i][inner] = dxdt[i][inner] + _tmp->operator[](inner);
        }
    }
}

std::unordered_map<std::string, size_t> Network_double_vecComplex::getFeatures() const
{
    std::unordered_map<std::string, size_t> features;
    static const std::string vectorComplexMetaName = (std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::simulation::Naming::Type_Complex));
    features[chimera::simulation::Naming::Feature_time_type] = chimera::systemtypes::PID_NUMBER;
    features[chimera::simulation::Naming::Feature_state_type] = _context->getParameterID(vectorComplexMetaName);
    features[chimera::simulation::Naming::Feature_size] = _number;

    auto unitFeatures = _unitSys->getFeatures();
    auto unitSize = unitFeatures.find(chimera::simulation::Naming::Feature_size);
    if(unitSize != unitFeatures.end())
    {
        features[chimera::simulation::Naming::Feature_unit_size] = unitSize->second;
    }
    auto unitRecursive = unitFeatures.find("recursive");
    if(unitRecursive != unitFeatures.end())
    {
        features["recursive"] = unitRecursive->second;
    }
    return features;
}

const std::string Network_double_vecComplex::getSystemName() const
{
    return "ode";
}

chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<std::complex<double> > >* Network_double_vecComplex::getNode(int i) const
{
    return _unitSys;
}
