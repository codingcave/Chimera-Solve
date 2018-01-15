#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>
#include <boost/numeric/ublas/vector.hpp>

#include "Naming.hpp"
#include "ExtensionNaming.hpp"
#include "StateSynchrony.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "extendTypes.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ChimeraSystem.hpp"

#include "Network/AbstractCoupling.hpp"
#include "Network/TemplateCoupling.hpp"
#include "Network/CouplingEntryPoint.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/TemplateOdeSystem.hpp"
#include "EntryPointBase/SystemDynamicModule.hpp"
#include "Network/Network_double_vecDouble.hpp"
#include "Network/NetworkModule.hpp"


extern "C"
{
    chimera::Module* LoadModule()
    {
        return new NetworkModule();
    }

    void UnloadModule(chimera::Module* reg)
    {
        delete (NetworkModule*)reg;
    }
}

NetworkModule::NetworkModule()
{
    _ep = nullptr;
}

NetworkModule::~NetworkModule()
{

}

const std::string NetworkModule::getGUID() const
{
    return "Network";
}

void* NetworkModule::getInstance(chimera::vec_t_LuaItem& parameters) const
{
    static const size_t basetype = chimera::systemtypes::PID_INSTANCE;
    size_t tagtype;
    size_t type;

    if(parameters.size() < 3)
    {
        return nullptr;
    }

    chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >* odeSys;
    int number;
    TemplateCoupling<boost::numeric::ublas::vector<double> >* coupling;

    type = parameters[0].getType();
    tagtype = (size_t)_callingEP;

    if(getChimeraSystem()->getTypeSystem()->getParameterBase(type) == basetype && getChimeraSystem()->getTypeSystem()->getParameterTag(type) == tagtype)
    {
        chimera::simulation::AbstractSystemDynamic* sd = (chimera::simulation::AbstractSystemDynamic*)parameters[0].getValue();
        odeSys = dynamic_cast<chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >*>(sd);
        if(odeSys == nullptr)
        {
            return nullptr;
        }
    }
    else
    {
        return nullptr;
    }


    if(parameters[1].getType() != chimera::systemtypes::PID_NUMBER)
    {
        return nullptr;
    }
    number = parameters[1];

    type = parameters[2].getType();
    tagtype = (size_t)_ep;

    //size_t xbase = getChimeraSystem()->getTypeSystem()->getParameterBase(type);
    //size_t xtag = getChimeraSystem()->getTypeSystem()->getParameterTag(type);

    if(getChimeraSystem()->getTypeSystem()->getParameterBase(type) == basetype && getChimeraSystem()->getTypeSystem()->getParameterTag(type) == tagtype)
    {
        AbstractCoupling* c = (AbstractCoupling*)parameters[2].getValue();
        coupling = dynamic_cast<TemplateCoupling<boost::numeric::ublas::vector<double> >*>(c);
        if(coupling == nullptr)
        {
            return nullptr;
        }
    }
    else
    {
        return nullptr;
    }

    return new Network_double_vecDouble(getChimeraSystem()->getTypeSystem(), parameters[0], number, parameters[2]);
}

void NetworkModule::destroyInstance(void * const instance) const
{
    delete ((chimera::simulation::AbstractSystemDynamic*)instance);
}

const std::string NetworkModule::getVersion() const
{
    return "1.0.0";
}

void NetworkModule::load(chimera::EntryPoint const * const entryPoint, void const * const params)
{
    _callingEP = entryPoint;
    if(_ep == nullptr) {
        _ep = new CouplingEntryPoint();
        getChimeraSystem()->getEntryPointSystem()->addEntryPoint("coupling", _ep);
    }
}

void NetworkModule::unload()
{
    //getChimeraSystem()->getEntryPointSystem()->removeEntryPoint("coupling");
    //delete _ep;
    _ep = nullptr;
}
