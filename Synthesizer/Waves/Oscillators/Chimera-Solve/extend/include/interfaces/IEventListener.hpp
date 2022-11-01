#ifndef IEVENTLISTENER_H
#define IEVENTLISTENER_H

namespace chimera {
    namespace simulation {
        class NotificationManager;

        class IEventListener
        {
            public:
                IEventListener() {}
                virtual ~IEventListener() {}
                virtual void notify(NotificationManager const * const sender, void const * const args) = 0;
            protected:
            private:
        };
    }
}

#endif // IEVENTLISTENER_H
