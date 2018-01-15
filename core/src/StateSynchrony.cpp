#include <unordered_set>
#include "StateSynchrony.hpp"

chimera::StateSynchrony::StateSynchrony()
{
    _loaded = false;
    _listeners = new std::unordered_set<StateSynchrony*>();
    _linkedItems = new std::unordered_set<StateSynchrony*>();
}

chimera::StateSynchrony::~StateSynchrony()
{
    bool loaded = _loaded;
    _loaded = false;

    {
        std::unordered_set<StateSynchrony*> visitedListeners;
        std::unordered_set<StateSynchrony*>::iterator it;
        bool openListeners;
        do {
            openListeners = false;
            for(auto it = _listeners->begin(); it != _listeners->end(); it++)
            {
                if(visitedListeners.count(*it)) {
                    continue;
                }
                openListeners = true;
                visitedListeners.insert(*it);
                if(loaded)
                {
                    (*it)->notifyUnload(this, nullptr);
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
        do {
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
}

void chimera::StateSynchrony::addListener(StateSynchrony* listener)
{
    if(!_listeners->count(listener)) {
        _listeners->insert(listener);
        listener->_linkedItems->insert(this);
    }
}

void chimera::StateSynchrony::removeListener(StateSynchrony* listener)
{
    if(_listeners->count(listener)) {
        _listeners->erase(listener);
        listener->_linkedItems->erase(this);
    }
}

bool chimera::StateSynchrony::isLoaded() const
{
    return _loaded;
}

void chimera::StateSynchrony::itemAdded(void const * const item, void const * const data)
{
    std::unordered_set<StateSynchrony*> visitedListeners;
    std::unordered_set<StateSynchrony*>::iterator it;
    bool openListeners;
    do {
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

void chimera::StateSynchrony::itemRemoved(void const * const item, void const * const data)
{
    std::unordered_set<StateSynchrony*> visitedListeners;
    std::unordered_set<StateSynchrony*>::iterator it;
    bool openListeners;
    do {
        openListeners = false;
        for(auto it = _listeners->begin(); it != _listeners->end(); it++)
        {
            if(visitedListeners.count(*it)) {
                continue;
            }
            openListeners = true;
            visitedListeners.insert(*it);
            (*it)->notifyItemRemoved(this, item, data);
            break;
        }
    } while(openListeners);
}

void chimera::StateSynchrony::itemLoaded(void const * const item, void const * const data)
{
    std::unordered_set<StateSynchrony*> visitedListeners;
    std::unordered_set<StateSynchrony*>::iterator it;
    bool openListeners;
    do {
        openListeners = false;
        for(auto it = _listeners->begin(); it != _listeners->end(); it++)
        {
            if(visitedListeners.count(*it)) {
                continue;
            }
            openListeners = true;
            visitedListeners.insert(*it);
            (*it)->notifyItemLoaded(this, item, data);
            break;
        }
    } while(openListeners);
}

void chimera::StateSynchrony::itemUnloaded(void const * const item, void const * const data)
{
    std::unordered_set<StateSynchrony*> visitedListeners;
    std::unordered_set<StateSynchrony*>::iterator it;
    bool openListeners;
    do {
        openListeners = false;
        for(auto it = _listeners->begin(); it != _listeners->end(); it++)
        {
            if(visitedListeners.count(*it)) {
                continue;
            }
            openListeners = true;
            visitedListeners.insert(*it);
            (*it)->notifyItemUnloaded(this, item, data);
            break;
        }
    } while(openListeners);
}

void chimera::StateSynchrony::stateLoaded()
{
    stateLoaded(nullptr);
}

void chimera::StateSynchrony::stateLoaded(void const * const data)
{
    if(!_loaded)
    {
        _loaded = true;

        std::unordered_set<StateSynchrony*> visitedListeners;
        std::unordered_set<StateSynchrony*>::iterator it;
        bool openListeners;
        do {
            openListeners = false;
            for(auto it = _listeners->begin(); it != _listeners->end(); it++)
            {
                if(visitedListeners.count(*it)) {
                    continue;
                }
                openListeners = true;
                visitedListeners.insert(*it);
                (*it)->notifyLoad(this, data);
                (*it)->itemLoaded(this, data);
                break;
            }
        } while(openListeners);
    }
}

void chimera::StateSynchrony::stateUnloaded()
{
    stateUnloaded(nullptr);
}

void chimera::StateSynchrony::stateUnloaded(void const * const data)
{
    if(_loaded)
    {
        _loaded = false;

        std::unordered_set<StateSynchrony*> visitedListeners;
        std::unordered_set<StateSynchrony*>::iterator it;
        bool openListeners;
        do {
            openListeners = false;
            for(auto it = _listeners->begin(); it != _listeners->end(); it++)
            {
                if(visitedListeners.count(*it)) {
                    continue;
                }
                openListeners = true;
                visitedListeners.insert(*it);
                (*it)->notifyUnload(this, data);
                (*it)->itemUnloaded(this, data);
                break;
            }
        } while(openListeners);
    }
}

void chimera::StateSynchrony::notifyLoad(StateSynchrony* sender, void const * const data)
{
    // virtual, do nothing
    return;
}

void chimera::StateSynchrony::notifyUnload(StateSynchrony* sender, void const * const data)
{
    // virtual, do nothing
    return;
}

void chimera::StateSynchrony::notifyItemAdded(StateSynchrony* sender, void const * const item, void const * const data)
{
    // virtual, do nothing
    return;
}

void chimera::StateSynchrony::notifyItemRemoved(StateSynchrony* sender, void const * const item, void const * const data)
{
    // virtual, do nothing
    return;
}

void chimera::StateSynchrony::notifyItemLoaded(StateSynchrony* sender, void const * const item, void const * const data)
{
    // virtual, do nothing
    return;
}

void chimera::StateSynchrony::notifyItemUnloaded(StateSynchrony* sender, void const * const item, void const * const data)
{
    // virtual, do nothing
    return;
}

void chimera::StateSynchrony::notifyDelete(StateSynchrony* sender)
{
    // virtual, do nothing
    return;
}
