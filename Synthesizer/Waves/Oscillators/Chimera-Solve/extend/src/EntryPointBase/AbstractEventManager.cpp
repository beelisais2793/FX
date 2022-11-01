#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>

#include "interfaces/IEventListener.hpp"
#include "interfaces/IEventListenerProvider.hpp"
#include "event/Observer.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/NotificationManager.hpp"
#include "EntryPointBase/AbstractEventManager.hpp"

chimera::simulation::AbstractEventManager::AbstractEventManager()
{
    //ctor
}

chimera::simulation::AbstractEventManager::~AbstractEventManager()
{
    //dtor
}
