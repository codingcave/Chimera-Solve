#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>
#include <boost/numeric/odeint.hpp>
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
#include "interfaces/IEventListener.hpp"
#include "interfaces/IEventListenerProvider.hpp"
#include "event/Observer.hpp"
#include "event/StateEventListener.hpp"
#include "EntryPointBase/AbstractEventProvider.hpp"
#include "event/DefaultEventProvider.hpp"
#include "event/NotificationManager.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/TemplateOdeSystem.hpp"
#include "EntryPointBase/AbstractIntegrator.hpp"
#include "EntryPointBase/TemplateIntegrator.hpp"
#include "EntryPointBase/IntegratorModule.hpp"
#include "Euler/EulerSystem.hpp"
#include "Euler/Euler_double_vecDouble.hpp"
#include "Euler/Euler_double_vecvecDouble.hpp"
#include "Euler/Euler_recursive_vecDouble.hpp"
#include "Euler/Euler_recursive_vecvecDouble.hpp"
#include "Euler/EulerModule.hpp"

extern "C"
{
    chimera::Module* LoadModule()
    {
        return new EulerModule();
    }

    void UnloadModule(chimera::Module* reg)
    {
        delete (EulerModule*)reg;
    }
}

EulerModule::EulerModule()
{
    _init = nullptr;
}

EulerModule::~EulerModule()
{

}

//void* RungeKuttaModule::getInstance(vec_t_LuaItem& parameters) const
//{
//    return new RungeKutta(nullptr);
//}

const std::string EulerModule::getVersion() const
{
    return "1.0.0";
}

void EulerModule::destroyInstance(void * const instance) const
{
    delete (chimera::simulation::AbstractIntegrator*)instance;
}

void EulerModule::load(chimera::EntryPoint const * const entryPoint, void const * const params)
{
    _init = (chimera::EntryPoint*)params;
}

chimera::simulation::AbstractIntegrator* EulerModule::getSystem(const chimera::ParameterValue& param, const double& dt) const
{
    static const std::string vectorRealMetaName = (std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER));
    static const std::string vectorVectorRealMetaName = (std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER));
    static const size_t vectorRealType = getChimeraSystem()->getTypeSystem()->getParameterID(vectorRealMetaName);
    static const size_t vectorVectorRealType = getChimeraSystem()->getTypeSystem()->getParameterID(vectorVectorRealMetaName);

    static const size_t basetype = chimera::systemtypes::PID_INSTANCE;
    static const size_t tagtype = (size_t)(getChimeraSystem()->getEntryPointSystem()->getEntryPoint(chimera::simulation::Naming::EntryPoint_dynamics));

    if(getChimeraSystem()->getTypeSystem()->getParameterBase(param.getType()) == basetype &&
       getChimeraSystem()->getTypeSystem()->getParameterTag(param.getType()) == tagtype)
    {
        chimera::simulation::AbstractSystemDynamic* dyn = (chimera::simulation::AbstractSystemDynamic*)param.getValue();
        auto features = dyn->getFeatures();

        auto timeType = features.find(chimera::simulation::Naming::Feature_time_type);
        auto stateType = features.find(chimera::simulation::Naming::Feature_state_type);
        if(timeType != features.end() && stateType != features.end()) {
            if(timeType->second == chimera::systemtypes::PID_NUMBER) {
                if(stateType->second == vectorRealType) {
                    chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >* sys;
                    sys = dynamic_cast<chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >*>(dyn);
                    if(sys != nullptr) {
                        auto recursive = features.find("recursive");
                        if(recursive != features.end() && recursive->second != 0)
                        {
                            std::cout << "RECURSIVE SINGLE" << std::endl;
                            return (chimera::simulation::AbstractIntegrator*)new Euler_recursive_vecDouble(getChimeraSystem()->getTypeSystem(), _init, sys);
                        }
                        else
                        {
                            std::cout << "NOT! SINGLE" << std::endl;
                            return (chimera::simulation::AbstractIntegrator*)new Euler_double_vecDouble(getChimeraSystem()->getTypeSystem(), _init, sys, dt);
                        }
                    }
                }
                if(stateType->second == vectorVectorRealType) {
                    chimera::simulation::TemplateOdeSystem<double, vec_vec_real>* sys;
                    sys = dynamic_cast<chimera::simulation::TemplateOdeSystem<double, vec_vec_real>*>(dyn);
                    if(sys != nullptr) {
                        auto recursive = features.find("recursive");
                        if(recursive != features.end() && recursive->second != 0)
                        {
                            std::cout << "RECURSIVE MULTI" << std::endl;
                            return (chimera::simulation::AbstractIntegrator*)new Euler_recursive_vecvecDouble(getChimeraSystem()->getTypeSystem(), _init, sys);
                        }
                        else
                        {
                            std::cout << "NOT! MULTI" << std::endl;
                            return (chimera::simulation::AbstractIntegrator*)new Euler_double_vecvecDouble(getChimeraSystem()->getTypeSystem(), _init, sys, dt);
                        }
                        //std::cout << "RETURN new RungeKutta_double_vecvecDouble(sys, dt);" << std::endl;
                    }
                }
            }
        }
        return nullptr;
    }
}

chimera::simulation::AbstractIntegrator* EulerModule::getIntegratorInstance(chimera::vec_t_LuaItem& parameters) const
{
    double dt = EulerModule::DEFAULT_DT;

    if(parameters.size() == 1 && parameters[0].getType() == chimera::systemtypes::PID_TABLE)
    {
        chimera::ParameterValue pvSystem;
        bool inSystem = false;
        chimera::map_t_LuaItem* paramMap = (chimera::map_t_LuaItem*)parameters[0].getValue();
        for(auto p : *paramMap)
        {
            if(p.first == "system")
            {
                inSystem = true;
                pvSystem = p.second;
            }
            if(p.first == "h" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                dt = p.second;
                if(dt <= 0.0) dt = EulerModule::DEFAULT_DT;
            }
        }

        if(!inSystem)
        {
            return nullptr;
        }
        return getSystem(pvSystem, dt);
    }
    else
    {
        if(parameters.size() > 0) {
            if(parameters.size() > 1 && parameters[1].getType() == chimera::systemtypes::PID_NUMBER)
            {
                dt = *((double*)parameters[1].getValue());
                if(dt <= 0.0) dt = EulerModule::DEFAULT_DT;
            }

            return getSystem(parameters[0], dt);
        }
    }
    return nullptr;
}

bool EulerModule::checkFeatures(const chimera::map_t_size& features) const
{
    static const std::string vectorRealMetaName = (std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER));
    static const std::string vectorVectorRealMetaName = (std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER));
    static const size_t vectorRealType = getChimeraSystem()->getTypeSystem()->getParameterID(vectorRealMetaName);
    static const size_t vectorVectorRealType = getChimeraSystem()->getTypeSystem()->getParameterID(vectorVectorRealMetaName);

    auto timeType = features.find(chimera::simulation::Naming::Feature_time_type);
    auto stateType = features.find(chimera::simulation::Naming::Feature_state_type);
    if(timeType != features.end() && stateType != features.end()) {
        if(timeType->second == chimera::systemtypes::PID_NUMBER) {
            if(stateType->second == vectorRealType || stateType->second == vectorVectorRealType) {
                return true;
            }
        }
    }

    return false;
}

const std::string EulerModule::getGUID() const
{
    return "Euler";
}

const std::string EulerModule::getSystemName() const
{
    return "ode";
}
