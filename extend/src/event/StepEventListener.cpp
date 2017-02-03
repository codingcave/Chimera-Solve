#include <tuple>

#include "interfaces/IEventListener.hpp"
#include "event/StepEventListener.hpp"

StepEventListener::StepEventListener()
{
    //ctor
}

StepEventListener::~StepEventListener()
{
    //dtor
}

void StepEventListener::notify(void const * const sender, void* args)
{
    std::tuple<size_t, void*, size_t, void*>* t = (std::tuple<size_t, void*, size_t, void*>*)args;
    notifyStep(sender, std::get<0>(*t), std::get<1>(*t), std::get<2>(*t), std::get<3>(*t));
}
