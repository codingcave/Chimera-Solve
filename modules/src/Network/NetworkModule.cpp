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

chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >* NetworkModule::getSystem(const chimera::ParameterValue& param) const
{
    static const size_t basetype = chimera::systemtypes::PID_INSTANCE;
    size_t tagtype;
    size_t type;

    type = param.getType();
    tagtype = (size_t)_callingEP;

    if(getChimeraSystem()->getTypeSystem()->getParameterBase(type) == basetype && getChimeraSystem()->getTypeSystem()->getParameterTag(type) == tagtype)
    {
        chimera::simulation::AbstractSystemDynamic* sd = (chimera::simulation::AbstractSystemDynamic*)param.getValue();
        return dynamic_cast<chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >*>(sd);
    }
    return nullptr;
}

TemplateCoupling<boost::numeric::ublas::vector<double> >* NetworkModule::getCoupling(const chimera::ParameterValue& param) const
{
    static const size_t basetype = chimera::systemtypes::PID_INSTANCE;
    size_t tagtype;
    size_t type;

    type = param.getType();
    tagtype = (size_t)_ep;

    if(getChimeraSystem()->getTypeSystem()->getParameterBase(type) == basetype && getChimeraSystem()->getTypeSystem()->getParameterTag(type) == tagtype)
    {
        AbstractCoupling* c = (AbstractCoupling*)param.getValue();
        return dynamic_cast<TemplateCoupling<boost::numeric::ublas::vector<double> >*>(c);
    }
    return nullptr;
}

void* NetworkModule::getInstance(chimera::vec_t_LuaItem& parameters) const
{
    chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >* odeSys;
    int number;
    TemplateCoupling<boost::numeric::ublas::vector<double> >* coupling;
    chimera::ParameterValue pvSystem;
    chimera::ParameterValue pvCoupling;

    if(parameters.size() == 1 && parameters[0].getType() == chimera::systemtypes::PID_TABLE)
    {
        bool inSystem = false, inNumber = false, inCoupling = false;

        chimera::map_t_LuaItem* paramMap = (chimera::map_t_LuaItem*)parameters[0].getValue();
        for(auto p : *paramMap)
        {
            if(p.first == "oscillator")
            {
                odeSys = getSystem(p.second);
                inSystem = (odeSys != nullptr);
                pvSystem = p.second;
            }
            if(p.first == "N" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                number = p.second;
                inNumber = true;
            }
            if(p.first == "coupling")
            {
                coupling = getCoupling(p.second);
                inCoupling = (coupling != nullptr);
                pvCoupling = p.second;
            }
        }

        if(!(inSystem && inNumber && inCoupling))
        {
            return nullptr;
        }
    } else {
        if(parameters.size() < 3)
        {
            return nullptr;
        }

        odeSys = getSystem(parameters[0]);
        if(odeSys == nullptr)
        {
            return nullptr;
        }
        pvSystem = parameters[0];

        if(parameters[1].getType() != chimera::systemtypes::PID_NUMBER)
        {
            return nullptr;
        }
        number = parameters[1];

        coupling = getCoupling(parameters[2]);
        if(coupling == nullptr)
        {
            return nullptr;
        }
        pvCoupling = parameters[2];
    }

    return new Network_double_vecDouble(getChimeraSystem()->getTypeSystem(), pvSystem, number, pvCoupling);
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
