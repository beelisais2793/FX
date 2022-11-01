#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

namespace chimera {
    namespace simulation {
        class IEventListener;
        class IEventListenerProvider;

        class NotificationManager
        {
            public:
                NotificationManager();
                virtual ~NotificationManager();
                bool addListener(const std::string& name, IEventListener* listener);
                IEventListener const * const addListener(const std::string& name, IEventListenerProvider* listener);
                bool removeListener(const std::string& name, IEventListener* listener);
                bool hasEvent(const std::string& name) const;
                size_t getEventType(const std::string& name) const;
            protected:
                Observer* getObserver(const std::string& name);
                void registerEvent(const std::string& name, AbstractEventProvider* eventProvider);
                void notifyEvent(const std::string& name, void const * const args);
                //void notifyAll();
            private:
                std::unordered_map<std::string, AbstractEventProvider*>* _providerMap;
        };
    }
}

#endif // NOTIFICATIONMANAGER_H
