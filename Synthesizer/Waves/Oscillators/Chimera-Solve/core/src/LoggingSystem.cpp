#include <iostream>
#include <map>
#include <vector>

#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"

chimera::LoggingSystem::LoggingSystem()
{
    _logList = new vec_t_logList();
    _loglevel = 0;
    _maxlevel = 0;
}

chimera::LoggingSystem::~LoggingSystem()
{
    delete _logList;
}

void chimera::LoggingSystem::log(const unsigned int& level, const std::string& message)
{
    for (auto it = _logList->rbegin(); it != _logList->rend(); ++it)
    {
        if(it->first < _loglevel)
        {
            break;
        }
        if(it->first <= level)
        {
            for(auto logger : it->second)
            {
                logger->LogMessage(message);
                if(!logger->bouble())
                {
                    break;
                }
            }
        }
    }
}

void chimera::LoggingSystem::debug(const std::string& message)
{
    log(0, message);
}

void chimera::LoggingSystem::addLogger(const unsigned int& level, chimera::ILogger *logger)
{
    auto levelExists = _logList->find(level);
    if(levelExists == _logList->end())
    {
        (*_logList)[level] = std::vector<ILogger*>();
        levelExists = _logList->find(level);
    }
    //for (vec_t_logList::iterator it = _logList->begin(); it != _logList->end(); ++it)
    //    if(*it == logger) return;
    levelExists->second.push_back(logger);
}

bool chimera::LoggingSystem::removeLogger(chimera::ILogger *logger)
{
    /*
    for (vec_t_logList::iterator it = _logList->begin(); it != _logList->end(); ++it)
    {
        if(*it == logger)
        {
            _logList->erase(it);
            return true;
        }
    }
    */
    return false;
}

void chimera::LoggingSystem::setLogLevel(const unsigned int& loglevel)
{
    _loglevel = loglevel;
}

unsigned int chimera::LoggingSystem::getLogLevel() const
{
    return _loglevel;
}

bool chimera::LoggingSystem::checkLogLevel(const unsigned int& loglevel) const
{
    return _loglevel <= loglevel;
}
