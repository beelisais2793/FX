#ifndef ABSTRACTINTEGRATOR_H
#define ABSTRACTINTEGRATOR_H

namespace chimera {
    namespace simulation {
        class AbstractIntegrator
        {
            public:
                AbstractIntegrator();
                virtual ~AbstractIntegrator();

                virtual void initialize(vec_t_LuaItem args) = 0;
                virtual void start(vec_t_LuaItem args) = 0;
                virtual bool finished() const = 0;
            protected:
            private:
        };
    }
}

#endif // ABSTRACTINTEGRATOR_H
