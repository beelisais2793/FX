#ifndef OBSERVER_H
#define OBSERVER_H

namespace chimera {
    namespace simulation {
        class IEventListener;
        class IEventListenerProvider;
        class NotificationManager;

        class Observer
        {
            public:
                Observer();
                Observer(size_t eventType);
                virtual ~Observer();
                void trigger(NotificationManager* sender, void const * const args);
                bool addListener(chimera::simulation::IEventListener* listener);
                bool removeListener(chimera::simulation::IEventListener* listener);
                bool empty() const;
                size_t getType() const;
            protected:
            private:
                std::unordered_set<IEventListener*>* _listeners;
                size_t _eventType;
        };
    }
}

#endif // OBSERVER_H
