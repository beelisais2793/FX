#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>
#include <boost/numeric/odeint.hpp>
#include <boost/numeric/ublas/vector.hpp>
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
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/StateEventListener.hpp"
#include "event/DefaultEventProvider.hpp"
#include "event/NotificationManager.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/TemplateOdeSystem.hpp"
#include "EntryPointBase/AbstractInitializer.hpp"
#include "EntryPointBase/AbstractIntegrator.hpp"
#include "EntryPointBase/AbstractTemporalIntegrator.hpp"
#include "EntryPointBase/IntegratorModule.hpp"
#include "RungeKutta/RungeKuttaSystem.hpp"
#include "RungeKutta/RungeKutta_double_vecvecDouble.hpp"

namespace ublas = boost::numeric::ublas;

RungeKutta_double_vecvecDouble::RungeKutta_double_vecvecDouble(
    chimera::ChimeraContext* context,
    chimera::EntryPoint* init,
    chimera::simulation::TemplateOdeSystem<double, vec_vec_real> * system,
    double dt
):
    _context(context),
    _init((size_t)init)
{
    //ctor
    _rk = new odeint::runge_kutta4<vec_vec_real>();
    _dt = dt;
    _context->addDependency(this, system);
    _context->addDependency(this, init);
    _time = 0;
    _system = new RungeKuttaSystem<double, vec_vec_real>(system);
    size_t length = system->getFeatures()[chimera::simulation::Naming::Feature_size];
    size_t unit_length = system->getFeatures()[chimera::simulation::Naming::Feature_unit_size];
    _state = new vec_vec_real(length);
    for(unsigned int i = 0; i < length; i++)
    {
        (*_state)[i].resize(unit_length);
    }
}

RungeKutta_double_vecvecDouble::~RungeKutta_double_vecvecDouble()
{
    _context->removeDependencyItem(this);
    delete _rk;
    delete _state;
    delete _system;
}

size_t RungeKutta_double_vecvecDouble::getTimeType() const
{
    return chimera::systemtypes::PID_NUMBER;
}

size_t RungeKutta_double_vecvecDouble::getStateType() const
{
    const std::string vectorVectorRealMetaName = std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER);
    return _context->getParameterID(vectorVectorRealMetaName);
}

//double * RungeKutta::getTime()
void * RungeKutta_double_vecvecDouble::currentTime()
{
    return new double(_time);
}

//struct T_VectorDef* RungeKutta::getState()
void * RungeKutta_double_vecvecDouble::currentState()
{
    auto state = new struct chimera::simulation::T_VectorDef({_state->size(), true, false, _state});
    _context->addDependency(state, this);
    return state;
}

void RungeKutta_double_vecvecDouble::nextStep()
{
    _rk->do_step( *_system , *_state , _time , _dt );
    _time += _dt;
}

void RungeKutta_double_vecvecDouble::initialize(chimera::vec_t_LuaItem args)
{
    if(args.size() > 1) {
        if(args[0].getType() == chimera::systemtypes::PID_NUMBER)
        {
            _time = *((double*)args[0].getValue());
            /*if(args[1].getType() == getStateType())
            {
                struct chimera::simulation::T_VectorDef* vd = (struct chimera::simulation::T_VectorDef*)args[1].getValue();
                ublas::vector<double>* newState = (ublas::vector<double>*)vd->value;
                if(newState->size() >= _state->size())
                {
                    for(unsigned int i = 0; i < _state->size(); i++)
                    {
                        (*_state)[i] = (*newState)[i];
                    }
                }
            }
            else */
            if(_context->getParameterBase(args[1].getType()) == chimera::systemtypes::PID_INSTANCE && _context->getParameterTag(args[1].getType()) == _init)
            {
                chimera::simulation::AbstractInitializer* init = (chimera::simulation::AbstractInitializer*)args[1].getValue();
                init->initialize((chimera::simulation::AbstractSystemDynamic*)_system->getSystem(), _state);
            }
        }
    } else {
        if(args.size() == 1)
        {
            /*if(args[0].getType() == getStateType()) {
                struct chimera::simulation::T_VectorDef* vd = (struct chimera::simulation::T_VectorDef*)args[0].getValue();
                ublas::vector<double>* newState = (ublas::vector<double>*)vd->value;
                if(newState->size() >= _state->size())
                {
                    for(unsigned int i = 0; i < _state->size(); i++)
                    {
                        (*_state)[i] = (*newState)[i];
                    }
                }
            } else */
            if(_context->getParameterBase(args[0].getType()) == chimera::systemtypes::PID_INSTANCE && _context->getParameterTag(args[0].getType()) == _init)
            {
                chimera::simulation::AbstractInitializer* init = (chimera::simulation::AbstractInitializer*)args[0].getValue();
                init->initialize((chimera::simulation::AbstractSystemDynamic*)_system->getSystem(), _state);
            }
        }
    }
}

void RungeKutta_double_vecvecDouble::start(chimera::vec_t_LuaItem args)
{
    if(args.size() > 0 && args[0].getType() == chimera::systemtypes::PID_NUMBER) {
        _endtime = _time + *((double*)args[0].getValue());
    } else {
        _endtime = _time + 1.0;
    }
}

bool RungeKutta_double_vecvecDouble::finished() const
{
    return _time >= _endtime;
}

/*
vec_vec_real& operator+=(vec_vec_real &v1, const vec_vec_real &v2)
{
    return v1;
}

vec_vec_real& operator*=(vec_vec_real &v, const double a)
{
    return v;
}
*/
vec_vec_real operator*(const double a, const vec_vec_real &v)
{
    vec_vec_real nv(v);
    for(unsigned int i = 0; i < v.size(); i++)
    {
        for(unsigned int j = 0; j < v[i].size(); j++)
        {
            nv[i][j] = a * v[i][j];
        }
    }
    return nv;
}

vec_vec_real operator+(const vec_vec_real &a, const vec_vec_real &b)
{
    vec_vec_real nv(a);
    for(unsigned int i = 0; i < a.size(); i++)
    {
        for(unsigned int j = 0; j < a[i].size(); j++)
        {
            nv[i][j] = a[i][j] + b[i][j];
        }
    }
    return nv;
}
