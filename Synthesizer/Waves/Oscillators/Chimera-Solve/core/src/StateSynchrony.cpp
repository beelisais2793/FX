#include <unordered_set>
#include "StateSynchrony.hpp"

chimera::StateSynchrony::StateSynchrony()
{
    _context = nullptr;
    _listeners = new std::unordered_set<StateSynchrony*>();
    _linkedItems = new std::unordered_set<StateSynchrony*>();
    _loadStates = new std::unordered_set<StateSynchrony*>();
}

chimera::StateSynchrony::~StateSynchrony()
{
    if(_loadStates->size() == 0)
    {
        return;
    }

    std::unordered_set<StateSynchrony*> loadStates = std::unordered_set<StateSynchrony*>(*_loadStates);
    _loadStates->clear();

    {
        std::unordered_set<StateSynchrony*> visitedListeners;
        std::unordered_set<StateSynchrony*>::iterator it;
        bool openListeners;
        do
        {
            openListeners = false;
            for(auto it = _listeners->begin(); it != _listeners->end(); it++)
            {
                if(visitedListeners.count(*it)) {
                    continue;
                }
                openListeners = true;
                visitedListeners.insert(*it);

                for(auto lit = loadStates.begin(); lit != loadStates.end(); lit++)
                {
                    (*it)->notifyUnload(this, *lit, nullptr);
                }
                (*it)->_linkedItems->erase(this);
                (*it)->notifyDelete(this);
                break;
            }
        } while(openListeners);
    }

    {
        std::unordered_set<StateSynchrony*> visitedListeners;
        std::unordered_set<StateSynchrony*>::iterator it;
        bool openListeners;
        do
        {
            openListeners = false;
            for(auto it = _linkedItems->begin(); it != _linkedItems->end(); it++)
            {
                if(visitedListeners.count(*it)) {
                    continue;
                }
                openListeners = true;
                visitedListeners.insert(*it);
                (*it)->removeListener(this);
                break;
            }
        } while(openListeners);
    }

    delete _listeners;
    delete _linkedItems;
    delete _loadStates;
}

void chimera::StateSynchrony::addListener(StateSynchrony* listener)
{
    if(!_listeners->count(listener))
    {
        _listeners->insert(listener);
        listener->_linkedItems->insert(this);
    }
}

void chimera::StateSynchrony::removeListener(StateSynchrony* listener)
{
    if(_listeners->count(listener))
    {
        _listeners->erase(listener);
        listener->_linkedItems->erase(this);
    }
}

bool chimera::StateSynchrony::isLoaded(StateSynchrony const * const context) const
{
    for(auto it = _loadStates->begin(); it != _loadStates->end(); it++)
    {
        if(context == *it)
        {
            return true;
        }
    }
    return false;
}

chimera::ChimeraContext* chimera::StateSynchrony::getContext() const
{
    return _context;
}
// MOVED TO: ChimeraSystem to avoid all these nasty includes here

void chimera::StateSynchrony::itemAdded(void* item, void const * const data)
{
    std::unordered_set<StateSynchrony*> visitedListeners;
    std::unordered_set<StateSynchrony*>::iterator it;
    bool openListeners;
    do
    {
        openListeners = false;
        for(auto it = _listeners->begin(); it != _listeners->end(); it++)
        {
            if(visitedListeners.count(*it)) {
                continue;
            }
            openListeners = true;
            visitedListeners.insert(*it);
            (*it)->notifyItemAdded(this, item, data);
            break;
        }
    } while(openListeners);
}

void chimera::StateSynchrony::itemRemoved(void* item, void const * const data)
{
    std::unordered_set<StateSynchrony*> visitedListeners;
    std::unordered_set<StateSynchrony*>::iterator it;
    bool openListeners;
    do
    {
        openListeners = false;
        for(auto it = _listeners->begin(); it != _listeners->end(); it++)
        {
            if(visitedListeners.count(*it))
            {
                continue;
            }
            openListeners = true;
            visitedListeners.insert(*it);
            (*it)->notifyItemRemoved(this, item, data);
            break;
        }
    } while(openListeners);
}

void chimera::StateSynchrony::itemLoaded(void* item, void const * const data)
{
    std::unordered_set<StateSynchrony*> visitedListeners;
    std::unordered_set<StateSynchrony*>::iterator it;
    bool openListeners;
    do
    {
        openListeners = false;
        for(auto it = _listeners->begin(); it != _listeners->end(); it++)
        {
            if(visitedListeners.count(*it))
            {
                continue;
            }
            openListeners = true;
            visitedListeners.insert(*it);
            (*it)->notifyItemLoaded(this, item, data);
            break;
        }
    } while(openListeners);
}

void chimera::StateSynchrony::itemUnloaded(void* item, void const * const data)
{
    std::unordered_set<StateSynchrony*> visitedListeners;
    std::unordered_set<StateSynchrony*>::iterator it;
    bool openListeners;
    do
    {
        openListeners = false;
        for(auto it = _listeners->begin(); it != _listeners->end(); it++)
        {
            if(visitedListeners.count(*it))
            {
                continue;
            }
            openListeners = true;
            visitedListeners.insert(*it);
            (*it)->notifyItemUnloaded(this, item, data);
            break;
        }
    } while(openListeners);
}

void chimera::StateSynchrony::stateLoaded(StateSynchrony* context)
{
    stateLoaded(context, nullptr);
}

void chimera::StateSynchrony::stateLoaded(StateSynchrony* context, void const * const data)
{
    if(_loadStates->count(context) == 0)
    {
        _loadStates->insert(context);

        std::unordered_set<StateSynchrony*> visitedListeners;
        std::unordered_set<StateSynchrony*>::iterator it;
        bool openListeners;
        do
        {
            openListeners = false;
            for(auto it = _listeners->begin(); it != _listeners->end(); it++)
            {
                if(visitedListeners.count(*it))
                {
                    continue;
                }
                openListeners = true;
                visitedListeners.insert(*it);
                (*it)->notifyLoad(this, context, data);
                if(*it == context)
                {
                    (*it)->itemLoaded(this, data);
                }
                break;
            }
        } while(openListeners);
    }
}

void chimera::StateSynchrony::stateUnloaded(StateSynchrony* context)
{
    stateUnloaded(context, nullptr);
}

void chimera::StateSynchrony::stateUnloaded(StateSynchrony* context, void const * const data)
{
    if(_loadStates->count(context) == 1)
    {
        _loadStates->erase(context);

        std::unordered_set<StateSynchrony*> visitedListeners;
        std::unordered_set<StateSynchrony*>::iterator it;
        bool openListeners;
        do
        {
            openListeners = false;
            for(auto it = _listeners->begin(); it != _listeners->end(); it++)
            {
                if(visitedListeners.count(*it))
                {
                    continue;
                }
                openListeners = true;
                visitedListeners.insert(*it);
                (*it)->notifyUnload(this, context, data);
                if(*it == context)
                {
                    (*it)->itemUnloaded(this, data);
                }
                break;
            }
        } while(openListeners);
    }
}

void chimera::StateSynchrony::notifyLoad(StateSynchrony* sender, StateSynchrony* context, void const * const data)
{
    // virtual, do nothing
    return;
}

void chimera::StateSynchrony::notifyUnload(StateSynchrony* sender, StateSynchrony* context, void const * const data)
{
    // virtual, do nothing
    return;
}

void chimera::StateSynchrony::notifyItemAdded(StateSynchrony* sender, void* item, void const * const data)
{
    // virtual, do nothing
    return;
}

void chimera::StateSynchrony::notifyItemRemoved(StateSynchrony* sender, void* item, void const * const data)
{
    // virtual, do nothing
    return;
}

void chimera::StateSynchrony::notifyItemLoaded(StateSynchrony* sender, void* item, void const * const data)
{
    // virtual, do nothing
    return;
}

void chimera::StateSynchrony::notifyItemUnloaded(StateSynchrony* sender, void* item, void const * const data)
{
    // virtual, do nothing
    return;
}

void chimera::StateSynchrony::notifyDelete(StateSynchrony* sender)
{
    // virtual, do nothing
    return;
}
