#ifndef RUNGEKUTTA_DOUBLE_VECVECDOUBLE_H
#define RUNGEKUTTA_DOUBLE_VECVECDOUBLE_H

typedef boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> > vec_vec_real;
namespace odeint = boost::numeric::odeint;

//vec_vec_real& operator+=(vec_vec_real &v1, const vec_vec_real &v2);
//vec_vec_real& operator*=(vec_vec_real &v, const double a);

vec_vec_real operator*(const double a, const vec_vec_real &v);
vec_vec_real operator+(const vec_vec_real &a, const vec_vec_real &v);

class RungeKutta_double_vecvecDouble:
    public chimera::simulation::AbstractTemporalIntegrator
{
    public:
        RungeKutta_double_vecvecDouble(chimera::ChimeraContext* context, chimera::EntryPoint* init, chimera::simulation::TemplateOdeSystem<double, vec_vec_real>* system, double dt);
        //RungeKutta_double_vecvecDouble();
        virtual ~RungeKutta_double_vecvecDouble();
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
        odeint::runge_kutta4<vec_vec_real>* _rk;
        RungeKuttaSystem<double, vec_vec_real>* _system;
        double _time;
        double _endtime;
        double _dt;
        vec_vec_real* _state;
        const size_t _init;
};

#endif // RUNGEKUTTA_DOUBLE_VECVECDOUBLE_H
