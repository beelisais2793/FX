#ifndef RUNGEKUTTASYSTEM_H
#define RUNGEKUTTASYSTEM_H

template<typename time_type, typename state_type>
class RungeKuttaSystem {
    public:
        RungeKuttaSystem(chimera::simulation::TemplateOdeSystem<time_type, state_type>* system)
        {
            _system = system;
        }
        virtual ~RungeKuttaSystem() {}
        void operator()(const state_type& x, state_type& dxdt, const time_type& t)
        {
            return (*_system)(x, dxdt, t);
        }
        chimera::simulation::TemplateOdeSystem<time_type, state_type>* getSystem() const
        {
            return _system;
        }
    private:
        chimera::simulation::TemplateOdeSystem<time_type, state_type>* _system;
};

#endif // RUNGEKUTTASYSTEM_H
