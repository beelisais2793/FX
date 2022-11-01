#ifndef ABSTRACTEVENTMANAGER_H
#define ABSTRACTEVENTMANAGER_H

namespace chimera {
    namespace simulation {
        class AbstractEventManager:
            public IEventListenerProvider,
            public NotificationManager
        {
            public:
                AbstractEventManager();
                virtual ~AbstractEventManager();
                virtual NotificationManager* getObservationObject() = 0;
                virtual void assignObservation(NotificationManager* baseObject) = 0;
            protected:
            private:
        };
    }
}

#endif // ABSTRACTEVENTMANAGER_H
