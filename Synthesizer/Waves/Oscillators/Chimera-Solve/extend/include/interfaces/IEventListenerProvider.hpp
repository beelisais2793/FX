#ifndef IEVENTLISTENERPROVIDER_H
#define IEVENTLISTENERPROVIDER_H

namespace chimera {
    namespace simulation {
        class IEventListenerProvider
        {
            public:
                IEventListenerProvider() {}
                virtual ~IEventListenerProvider() {}
                virtual IEventListener* provideListener(size_t id, void const * const args) = 0;
            protected:
            private:
        };
    }
}

#endif // IEVENTLISTENERPROVIDER_H
