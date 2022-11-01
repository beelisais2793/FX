#ifndef ABSTRACTRANDOMGENERATOR_H
#define ABSTRACTRANDOMGENERATOR_H


namespace chimera {
    namespace simulation {
        class AbstractRandomGenerator
        {
            public:
                typedef unsigned int result_type;
                AbstractRandomGenerator();
                virtual ~AbstractRandomGenerator();
                virtual unsigned int min() = 0;
                virtual unsigned int max() = 0;
                virtual unsigned int operator()() = 0;
            protected:
            private:
        };
    }
}

#endif // ABSTRACTRANDOMGENERATOR_H
