#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "event/Observer.hpp"
#include "interfaces/IEventListener.hpp"
#include "interfaces/IEventListenerProvider.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/DefaultEventProvider.hpp"
#include "event/NotificationManager.hpp"

chimera::simulation::DefaultEventProvider::DefaultEventProvider()
{
    //ctor
}

chimera::simulation::DefaultEventProvider::~DefaultEventProvider()
{
    //dtor
}

chimera::simulation::IEventListener* chimera::simulation::DefaultEventProvider::provideListener(IEventListenerProvider* provider) const
{
    return provider->provideListener(0, nullptr);
}
