#ifndef ABSTRACTEVENTPROVIDER_H
#define ABSTRACTEVENTPROVIDER_H

namespace chimera {
    namespace simulation {
        // TODO Remove class, replace with simple args, used for provideListener

        class AbstractEventProvider
        {
            friend class NotificationManager;

            public:
                AbstractEventProvider();
                AbstractEventProvider(size_t eventType);
                virtual ~AbstractEventProvider();
                //void trigger(NotificationManager* sender);
                Observer* getObserver();
                size_t getType() const;
                virtual IEventListener* provideListener(IEventListenerProvider* provider) const = 0;
            protected:
                //virtual bool triggerCondition(NotificationManager const * const sender) = 0;
                //virtual void* getEventArgs(NotificationManager const * const sender) = 0;
            private:
                Observer* _observer;
        };

    }
}

#endif // ABSTRACTEVENTPROVIDER_H
