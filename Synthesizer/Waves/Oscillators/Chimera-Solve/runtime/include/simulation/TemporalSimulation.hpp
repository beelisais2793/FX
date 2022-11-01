#ifndef TEMPORALSIMULATION_H
#define TEMPORALSIMULATION_H

namespace chimera {
    namespace runtime {

        class TemporalSimulation:
            public chimera::simulation::AbstractSimulation
        {
            public:
                TemporalSimulation(chimera::simulation::AbstractTemporalIntegrator* integrator);
                virtual ~TemporalSimulation();
                bool getYield() const;
                void setYield(bool value);
                bool nextStep();
                virtual bool isRunning() const override;
                virtual chimera::simulation::AbstractIntegrator* getIntegrator() override;
            protected:
            private:
                chimera::simulation::AbstractTemporalIntegrator* _integrator;
                chimera::simulation::TemporalStateEventProvider* _onStep;
                bool _yield;
                size_t _type;
                std::string _name;
                bool _running;
        };
    }
}
#endif // TEMPORALSIMULATION_H
