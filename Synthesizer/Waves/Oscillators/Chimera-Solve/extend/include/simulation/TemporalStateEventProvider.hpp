#ifndef TEMPORALSTATEEVENTPROVIDER_H
#define TEMPORALSTATEEVENTPROVIDER_H

namespace chimera {
    namespace simulation {
        class IEventListener;
        class IEventListenerProvider;
        class AbstractTemporalIntegrator;

        struct T_TimeStateArgs
        {
            void* time;
            void* state;
        };

        struct T_StateProviderArgs {
            size_t time_type;
            size_t state_type;
        };

        class TemporalStateEventProvider:
            public AbstractEventProvider
        {
            public:
                TemporalStateEventProvider(size_t time_type, size_t state_type);
                virtual ~TemporalStateEventProvider();
                virtual IEventListener* provideListener(IEventListenerProvider* provider) const override;
            protected:
                //virtual bool triggerCondition(NotificationManager const * const sender) override;
                //virtual void* getEventArgs(NotificationManager const * const sender) override;
            private:
                T_StateProviderArgs _providerArgs;
                //struct T_TimeStateArgs* _args;
        };
    }
}

#endif // TEMPORALSTATEEVENTPROVIDER_H
