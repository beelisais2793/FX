#ifndef ABSTRACTSIMULATION_H
#define ABSTRACTSIMULATION_H

namespace chimera {
    namespace simulation {
        class AbstractSimulation:
            public chimera::simulation::NotificationManager
        {
            public:
                AbstractSimulation() {}
                virtual ~AbstractSimulation() {}
                virtual bool isRunning() const = 0;
                virtual AbstractIntegrator* getIntegrator() = 0;
            protected:
            private:
        };
    }
}

#endif // ABSTRACTSIMULATION_H
