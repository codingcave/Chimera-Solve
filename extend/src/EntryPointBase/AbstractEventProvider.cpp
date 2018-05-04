#include <iostream>
#include <unordered_map>
#include <vector>

#include "event/Observer.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/DefaultEventProvider.hpp"
#include "event/NotificationManager.hpp"

chimera::simulation::AbstractEventProvider::AbstractEventProvider()
{

}

chimera::simulation::AbstractEventProvider::~AbstractEventProvider()
{
    //dtor
}
