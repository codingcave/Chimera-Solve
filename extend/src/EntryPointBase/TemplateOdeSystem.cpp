#include <iostream>
#include <unordered_map>
#include <vector>

#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/TemplateOdeSystem.hpp"

template<typename state_type, typename time_type>
TemplateOdeSystem<state_type, time_type>::TemplateOdeSystem()
{
    //ctor
}

template<typename state_type, typename time_type>
TemplateOdeSystem<state_type, time_type>::~TemplateOdeSystem()
{
    //dtor
}
