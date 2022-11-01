#ifndef RUNGEKUTTA_DOUBLE_VECDOUBLE_H
#define RUNGEKUTTA_DOUBLE_VECDOUBLE_H

namespace odeint = boost::numeric::odeint;

class RungeKutta_double_vecDouble:
    public chimera::simulation::AbstractTemporalIntegrator
{
    public:
        RungeKutta_double_vecDouble(chimera::ChimeraContext* context, chimera::EntryPoint* init, chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >* system, double dt);
        //RungeKutta_double_vecDouble();
        virtual ~RungeKutta_double_vecDouble();
        virtual void nextStep() override;
        virtual size_t getTimeType() const override;
        virtual size_t getStateType() const override;
        virtual void * currentTime() override;
        virtual void * currentState() override;
        virtual void initialize(chimera::vec_t_LuaItem args) override;
        virtual void start(chimera::vec_t_LuaItem args) override;
        virtual bool finished() const override;
    protected:
    private:
        chimera::ChimeraContext* _context;
        odeint::runge_kutta4<boost::numeric::ublas::vector<double> >* _rk;
        RungeKuttaSystem<double, boost::numeric::ublas::vector<double> >* _system;
        double _time;
        double _endtime;
        double _dt;
        boost::numeric::ublas::vector<double>* _state;
        size_t _init;
};

#endif // RUNGEKUTTA_DOUBLE_VECDOUBLE_H
