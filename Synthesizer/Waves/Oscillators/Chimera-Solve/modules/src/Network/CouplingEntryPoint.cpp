#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>
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
#include "EntryPointBase/SystemDynamicModule.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/TemplateOdeSystem.hpp"
#include "Network/AbstractCoupling.hpp"
#include "Network/CouplingModule.hpp"

#include "Network/CouplingEntryPoint.hpp"

CouplingEntryPoint::CouplingEntryPoint()
{

}

CouplingEntryPoint::~CouplingEntryPoint()
{
    //dtor
}

const std::string CouplingEntryPoint::getGUID() const
{
    return "coupling";
}

bool CouplingEntryPoint::checkModule(chimera::Module const * const module) const
{
    const CouplingModule* cm = dynamic_cast<const CouplingModule*>(module);
    return cm != nullptr;
}

void CouplingEntryPoint::init()
{

}
