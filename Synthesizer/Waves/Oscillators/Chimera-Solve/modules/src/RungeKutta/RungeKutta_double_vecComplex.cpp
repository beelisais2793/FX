#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>
#include <boost/numeric/odeint.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <complex>
//#include "lua.hpp"

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
#include "event/Observer.hpp"
#include "event/StateEventListener.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/DefaultEventProvider.hpp"
#include "event/NotificationManager.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/TemplateOdeSystem.hpp"
#include "EntryPointBase/AbstractInitializer.hpp"
#include "EntryPointBase/AbstractIntegrator.hpp"
#include "EntryPointBase/AbstractTemporalIntegrator.hpp"
#include "EntryPointBase/IntegratorModule.hpp"
#include "RungeKutta/RungeKuttaSystem.hpp"
#include "RungeKutta/RungeKutta_double_vecComplex.hpp"

namespace ublas = boost::numeric::ublas;

RungeKutta_double_vecComplex::RungeKutta_double_vecComplex(
        chimera::ChimeraContext* context,
        chimera::EntryPoint* init,
        chimera::simulation::TemplateOdeSystem<double, ublas::vector<std::complex<double> > > * system,
        double dt
    ):
    _context(context),
    _init((size_t)init)
{
    //ctor

    _rk = new odeint::runge_kutta4<ublas::vector<std::complex<double> > >();
    _dt = dt;
    _system = new RungeKuttaSystem<double, boost::numeric::ublas::vector<std::complex<double> > >(system);
    _context->addDependency(this, system);
    _context->addDependency(this, init);
    _time = 0;
    int length = system->getFeatures()[chimera::simulation::Naming::Feature_size];
    _state = new ublas::vector<std::complex<double> >(length);
}

RungeKutta_double_vecComplex::~RungeKutta_double_vecComplex()
{
    _context->removeDependencyItem(this);
    delete _rk;
    delete _state;
    delete _system;
}

size_t RungeKutta_double_vecComplex::getTimeType() const
{
    return chimera::systemtypes::PID_NUMBER;
}

size_t RungeKutta_double_vecComplex::getStateType() const
{
    static const std::string vectorComplexMetaName = (std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::simulation::Naming::Type_Complex));
    return _context->getParameterID(vectorComplexMetaName);
}

//double * RungeKutta::getTime()
void * RungeKutta_double_vecComplex::currentTime()
{
    return new double(_time);
}

//struct T_VectorDef* RungeKutta::getState()
void * RungeKutta_double_vecComplex::currentState()
{
    auto state = new struct chimera::simulation::T_VectorDef({_state->size(), true, false, _state});
    _context->addDependency(state, this);
    return state;
}

void RungeKutta_double_vecComplex::nextStep()
{
    _rk->do_step( *_system , *_state , _time , _dt );
    _time += _dt;
}

void RungeKutta_double_vecComplex::initialize(chimera::vec_t_LuaItem args)
{
    if(args.size() > 1) {
        if(args[0].getType() == chimera::systemtypes::PID_NUMBER)
        {
            _time = *((double*)args[0].getValue());
            if(args[1].getType() == getStateType())
            {
                struct chimera::simulation::T_VectorDef* vd = (struct chimera::simulation::T_VectorDef*)args[1].getValue();
                ublas::vector<std::complex<double> >* newState = (ublas::vector<std::complex<double> >*)vd->value;
                if(newState->size() >= _state->size())
                {
                    for(unsigned int i = 0; i < _state->size(); i++)
                    {
                        (*_state)[i] = (*newState)[i];
                    }
                }
            }
            else if(args[1].getType() == chimera::systemtypes::PID_INSTANCE && _context->getParameterTag(args[1].getType() == _init))
            {
                chimera::simulation::AbstractInitializer* init = (chimera::simulation::AbstractInitializer*)args[1].getValue();
                init->initialize((chimera::simulation::AbstractSystemDynamic*)_system->getSystem(), _state);
            }
        }
    } else {
        if(args.size() == 1)
        {
            if(args[0].getType() == getStateType()) {
                struct chimera::simulation::T_VectorDef* vd = (struct chimera::simulation::T_VectorDef*)args[0].getValue();
                ublas::vector<std::complex<double> >* newState = (ublas::vector<std::complex<double> >*)vd->value;
                if(newState->size() >= _state->size())
                {
                    for(unsigned int i = 0; i < _state->size(); i++)
                    {
                        (*_state)[i] = (*newState)[i];
                    }
                }
            } else if(args[0].getType() == chimera::systemtypes::PID_INSTANCE && _context->getParameterTag(args[0].getType() == _init)) {
                chimera::simulation::AbstractInitializer* init = (chimera::simulation::AbstractInitializer*)args[0].getValue();
                init->initialize((chimera::simulation::AbstractSystemDynamic*)_system->getSystem(), _state);
            }
        }
    }
}

void RungeKutta_double_vecComplex::start(chimera::vec_t_LuaItem args)
{
    if(args.size() > 0 && args[0].getType() == chimera::systemtypes::PID_NUMBER) {
        _endtime = _time + *((double*)args[0].getValue());
    } else {
        _endtime = _time + 1.0;
    }
}

bool RungeKutta_double_vecComplex::finished() const
{
    return _time >= _endtime;
}
