#include <iostream>
#include <map>
#include <vector>

#include "Naming.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
#include "StandardLogger.hpp"

StandardLogger::StandardLogger()
{

}

StandardLogger::~StandardLogger()
{

}

void StandardLogger::LogMessage(const std::string& message)
{
    std::cerr << message << std::endl;
}

bool StandardLogger::bouble() const
{
    return true;
}
