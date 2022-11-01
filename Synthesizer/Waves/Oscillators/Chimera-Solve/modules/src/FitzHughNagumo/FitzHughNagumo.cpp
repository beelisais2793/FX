#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <boost/numeric/ublas/vector.hpp>
#include "lua.hpp"

#include "def.hpp"
#include "Naming.hpp"
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
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/TemplateOdeSystem.hpp"
#include "FitzHughNagumo/FitzHughNagumo.hpp"

namespace ublas = boost::numeric::ublas;

FitzHughNagumo::FitzHughNagumo(chimera::ChimeraContext* context, double epsilon, double a):
    _context(context),
    _epsilon(epsilon),
    _a(a)
{

}

void FitzHughNagumo::operator()(const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::vector<double>& dxdt, const double& t)
{
    static double _1_3 = 1.0/3.0;
    dxdt(0) = (x(0) - _1_3 * x(0) * x(0) * x(0) - x(1)) / _epsilon;
    dxdt(1) = (x(0) + _a);
}

std::unordered_map<std::string, size_t> FitzHughNagumo::getFeatures() const
{
    std::unordered_map<std::string, size_t> features;
    features[chimera::simulation::Naming::Feature_time_type] = chimera::systemtypes::PID_NUMBER;
    static const std::string vectorRealMetaName = (std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER));
    features[chimera::simulation::Naming::Feature_state_type] = _context->getParameterID(vectorRealMetaName);
    features[chimera::simulation::Naming::Feature_size] = 2;
    return features;
}

const std::string FitzHughNagumo::getSystemName() const
{
    return "ode";
}

FitzHughNagumo::~FitzHughNagumo()
{

}
