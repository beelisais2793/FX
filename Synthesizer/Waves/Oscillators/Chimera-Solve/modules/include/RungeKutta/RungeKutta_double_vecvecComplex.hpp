#ifndef RUNGEKUTTA_DOUBLE_VECVECCOMPLEX_H
#define RUNGEKUTTA_DOUBLE_VECVECCOMPLEX_H

typedef boost::numeric::ublas::vector<boost::numeric::ublas::vector<std::complex<double> > > vec_vec_complex;
namespace odeint = boost::numeric::odeint;

//vec_vec_real& operator+=(vec_vec_real &v1, const vec_vec_real &v2);
//vec_vec_real& operator*=(vec_vec_real &v, const double a);

vec_vec_complex operator*(const double a, const vec_vec_complex &v);
vec_vec_complex operator+(const vec_vec_complex &a, const vec_vec_complex &v);

class RungeKutta_double_vecvecComplex:
    public chimera::simulation::AbstractTemporalIntegrator
{
    public:
        RungeKutta_double_vecvecComplex(chimera::ChimeraContext* context, chimera::EntryPoint* init, chimera::simulation::TemplateOdeSystem<double, vec_vec_complex>* system, double dt);
        virtual ~RungeKutta_double_vecvecComplex();
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
        odeint::runge_kutta4<vec_vec_complex>* _rk;
        RungeKuttaSystem<double, vec_vec_complex>* _system;
        double _time;
        double _endtime;
        double _dt;
        vec_vec_complex* _state;
        const size_t _init;
};

#endif // RUNGEKUTTA_DOUBLE_VECVECCOMPLEX_H
