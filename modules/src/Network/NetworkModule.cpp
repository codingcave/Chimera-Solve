#include <iostream>
#include <map>
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
#include "EntryPointBase/AbstractRandom.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/TemplateOdeSystem.hpp"
#include "EntryPointBase/SystemDynamicModule.hpp"
#include "Network/INetwork.hpp"
#include "Network/Network_double_vecDouble.hpp"
#include "Network/MultiNetwork_double_vecDouble.hpp"
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

TemplateCoupling<double, boost::numeric::ublas::vector<double> >* NetworkModule::getCoupling(const chimera::ParameterValue& param) const
{
    static const size_t basetype = chimera::systemtypes::PID_INSTANCE;
    size_t tagtype;
    size_t type;

    type = param.getType();
    tagtype = (size_t)_ep;

    if(getChimeraSystem()->getTypeSystem()->getParameterBase(type) == basetype && getChimeraSystem()->getTypeSystem()->getParameterTag(type) == tagtype)
    {
        AbstractCoupling* c = (AbstractCoupling*)param.getValue();
        return dynamic_cast<TemplateCoupling<double, boost::numeric::ublas::vector<double> >*>(c);
    }
    return nullptr;
}

void* NetworkModule::getInstance(chimera::vec_t_LuaItem& parameters) const
{
    chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >* odeSys;
    int number;
    int switchNumber = 0;
    TemplateCoupling<double, boost::numeric::ublas::vector<double> >* coupling;
    chimera::ParameterValue pvSystem;
    chimera::ParameterValue pvCoupling;

    if(parameters.size() == 1 && parameters[0].getType() == chimera::systemtypes::PID_TABLE)
    {
        bool inSystem = false, inNumber = false, inCoupling = false, inInit = false;
        chimera::map_t_LuaItem* initMap;
        chimera::simulation::SystemDynamicModule* sysMod;

        chimera::map_t_LuaItem* paramMap = (chimera::map_t_LuaItem*)parameters[0].getValue();
        for(auto p : *paramMap)
        {
            if(p.first == "oscillator")
            {
                odeSys = getSystem(p.second);
                inSystem = (odeSys != nullptr);
                if(inSystem)
                {
                    switchNumber = 0;
                    pvSystem = p.second;
                    continue;
                }
                else
                {
                    size_t randomEntryPoint = (size_t)getChimeraSystem()->getEntryPointSystem()->getEntryPoint(chimera::simulation::Naming::EntryPoint_random);
                    if(randomEntryPoint != 0)
                    {
                        if(p.second.getType() == chimera::systemtypes::PID_MODULE)
                        {
                            chimera::Module* module = (chimera::Module*)p.second.getValue();
                            sysMod = dynamic_cast<chimera::simulation::SystemDynamicModule*>(module);
                            if(sysMod != nullptr)
                            {
                                inSystem = true;
                                switchNumber = 1;
                                continue;
                            }
                        }
                    }
                    break;
                }
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
            if(p.first == "init")
            {
                if(p.second.getType() == chimera::systemtypes::PID_TABLE)
                {
                    initMap = (chimera::map_t_LuaItem*)p.second.getValue();
                    inInit = true;
                }
                else
                {
                    break;
                }
            }
        }

        if(!(inSystem && inNumber && inCoupling))
        {
            return nullptr;
        }

        switch(switchNumber)
        {
            case 0:
            {
                return new Network_double_vecDouble(getChimeraSystem()->getTypeSystem(), pvSystem, number, pvCoupling);
            }
            case 1:
            {
                std::vector<chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >*> oscillators;
                chimera::simulation::AbstractSystemDynamic* item;
                size_t randomEntryPoint = (size_t)getChimeraSystem()->getEntryPointSystem()->getEntryPoint(chimera::simulation::Naming::EntryPoint_random);
                size_t initEntryPoint = (size_t)getChimeraSystem()->getEntryPointSystem()->getEntryPoint(chimera::simulation::Naming::EntryPoint_initilizer);

                for(int i = 0; i < number; i++)
                {
                    chimera::vec_t_LuaItem initParams;
                    if(inInit)
                    {
                        chimera::map_t_LuaItem* paramValue1 = new chimera::map_t_LuaItem();
                        chimera::ParameterValue p1 = getChimeraSystem()->getTypeSystem()->createValue(chimera::systemtypes::PID_TABLE, paramValue1);
                        initParams.push_back(p1);
                        for(auto p : *initMap)
                        {
                            if(getChimeraSystem()->getTypeSystem()->getParameterBase(p.second.getType()) == chimera::systemtypes::PID_INSTANCE && getChimeraSystem()->getTypeSystem()->getParameterTag(p.second.getType()) == randomEntryPoint && randomEntryPoint != 0)
                            {
                                chimera::simulation::AbstractRandom* rnd = (chimera::simulation::AbstractRandom*)p.second.getValue();
                                chimera::ParameterValue next = getChimeraSystem()->getTypeSystem()->createValue(rnd->getType(), rnd->getNext());
                                paramValue1->insert(
                                    std::make_pair(p.first, next)
                                );
                            }
                            else
                            {
                                paramValue1->insert(
                                    std::make_pair(p.first, p.second)
                                );
                            }
                        }
                    }
                    chimera::simulation::AbstractSystemDynamic* oscItem = (chimera::simulation::AbstractSystemDynamic*)sysMod->getInstance(initParams);
                    chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >* odeItem = dynamic_cast<chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >*>(oscItem);
                    if(odeItem != nullptr)
                    {
                        oscillators.push_back(odeItem);
                    }
                    else
                    {
                        for(auto osc : oscillators)
                        {
                            delete osc;
                        }
                        return nullptr;
                    }
                }

                return new MultiNetwork_double_vecDouble(getChimeraSystem()->getTypeSystem(), sysMod, oscillators, pvCoupling);
            }
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

        return new Network_double_vecDouble(getChimeraSystem()->getTypeSystem(), pvSystem, number, pvCoupling);
    }

    return nullptr;
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
