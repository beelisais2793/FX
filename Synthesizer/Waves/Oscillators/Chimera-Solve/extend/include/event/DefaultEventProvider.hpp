#ifndef DEFAULTEVENTPROVIDER_H
#define DEFAULTEVENTPROVIDER_H

namespace chimera {
    namespace simulation {
        class DefaultEventProvider:
            public AbstractEventProvider
        {
            public:
                DefaultEventProvider();
                virtual ~DefaultEventProvider();
                virtual IEventListener* provideListener(IEventListenerProvider* provider) const = 0;
            protected:
            private:
        };
    }
}

#endif // DEFAULTEVENTPROVIDER_H
