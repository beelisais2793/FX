#ifndef ABSTRACTRANDOM_H
#define ABSTRACTRANDOM_H

namespace chimera {
    namespace simulation {
        class AbstractRandom
        {
            public:
                AbstractRandom();
                virtual ~AbstractRandom();
                virtual const size_t getType() const = 0;
                virtual void* getNext() = 0;
            protected:
            private:
        };
    }
}

#endif // ABSTRACTRANDOM_H
